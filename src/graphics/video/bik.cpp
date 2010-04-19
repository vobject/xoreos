/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/bik.cpp
 *  Decoding RAD Game Tools' Bink videos.
 */

#include <cmath>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/bik.h"

#include "events/events.h"

static const uint32 kBIKfID = MKID_BE('BIKf');
static const uint32 kBIKgID = MKID_BE('BIKg');
static const uint32 kBIKhID = MKID_BE('BIKh');
static const uint32 kBIKiID = MKID_BE('BIKi');

static const uint32 kVideoFlagAlpha = 0x00100000;

namespace Graphics {

BIK::BIK(Common::SeekableReadStream *bik) : _bik(bik), _curFrame(0) {
	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].data    = 0;
		_bundles[i].dataEnd = 0;
		_bundles[i].curDec  = 0;
		_bundles[i].curPtr  = 0;
	}

	load();
}

BIK::~BIK() {
	freeBundles();
}

bool BIK::gotTime() const {
	return true;
}

void BIK::processData() {
	uint32 curTime = EventMan.getTimestamp();

	if (!_started) {
		_startTime     = curTime;
		_lastFrameTime = curTime;
		_started       = true;
	}

	uint32 frameTime = ((uint64) (_curFrame * 1000 * ((uint64) _fpsDen))) / _fpsNum;
	if ((curTime - _startTime) < frameTime)
		return;

	if (_curFrame >= _frames.size()) {
		_finished = true;
		return;
	}

	if (!_bik->seek(_frames[_curFrame].offset))
		throw Common::Exception(Common::kSeekError);

	uint32 audioPacketLength = _bik->readUint32LE();
	if (audioPacketLength != 0) {
		audioPacket();
		_bik->skip(audioPacketLength);
	}

	videoPacket();

	_needCopy = true;

	warning("Frame %d / %d", _curFrame, (int) _frames.size());

	_curFrame++;
}

void BIK::audioPacket() {
}

void BIK::videoPacket() {
}

void BIK::load() {
	_id = _bik->readUint32BE();
	if ((_id != kBIKfID) && (_id != kBIKgID) && (_id != kBIKhID) && (_id != kBIKiID))
		throw Common::Exception("Unknown BIK FourCC %04X", _id);

	uint32 fileSize         = _bik->readUint32LE() + 8;
	uint32 frameCount       = _bik->readUint32LE();
	uint32 largestFrameSize = _bik->readUint32LE();

	if (largestFrameSize > fileSize)
		throw Common::Exception("Largest frame size greater than file size");

	_bik->skip(4);

	uint32 width  = _bik->readUint32LE();
	uint32 height = _bik->readUint32LE();

	createData(width, height);

	_fpsNum = _bik->readUint32LE();
	_fpsDen = _bik->readUint32LE();

	if ((_fpsNum == 0) || (_fpsDen == 0))
		throw Common::Exception("Invalid FPS (%d/%d)", _fpsNum, _fpsDen);

	_videoFlags = _bik->readUint32LE();

	uint32 audioTrackCount = _bik->readUint32LE();
	if (audioTrackCount > 0) {
		_audioTracks.resize(audioTrackCount);

		_bik->skip(4 * audioTrackCount);

		for (std::vector<AudioTrack>::iterator it = _audioTracks.begin(); it != _audioTracks.end(); ++it) {
			it->sampleRate = _bik->readUint16LE();
			it->flags      = _bik->readUint16LE();
		}

		_bik->skip(4 * audioTrackCount);
	}

	_frames.resize(frameCount);
	for (std::vector<VideoFrame>::iterator it = _frames.begin(); it != _frames.end(); ++it) {
		it->offset   = _bik->readUint32LE();
		it->keyFrame = it->offset & 1;

		it->offset &= ~1;
	}

	_hasAlpha   = _videoFlags & kVideoFlagAlpha;
	_swapPlanes = (_id == kBIKhID) || (_id == kBIKiID);

	initBundles();
}

void BIK::initBundles() {
	uint32 bw = (_width  + 7) >> 3;
	uint32 bh = (_height + 7) >> 3;

	uint32 blocks = bw * bh;

	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].data    = new byte[blocks * 64];
		_bundles[i].dataEnd = _bundles[i].data + blocks * 64;
	}
}

void BIK::freeBundles() {
	for (int i = 0; i < kSourceMAX; i++) {
		delete[] _bundles[i].data;

		_bundles[i].data    = 0;
		_bundles[i].dataEnd = 0;
		_bundles[i].curDec  = 0;
		_bundles[i].curPtr  = 0;
	}
}

// TODO: Don't use the standard slow log2()?
void BIK::initLengths(uint32 width, uint32 bw) {
	_bundles[kSourceBlockTypes].length    = log2((width >> 3)    + 511) + 1;

	_bundles[kSourceSubBlockTypes].length = log2((width >> 4)    + 511) + 1;

	_bundles[kSourceColors].length        = log2((width >> 3)*64 + 511) + 1;

	_bundles[kSourceIntraDC].length       = log2((width >> 3)    + 511) + 1;
	_bundles[kSourceInterDC].length       = log2((width >> 3)    + 511) + 1;
	_bundles[kSourceXOff].length          = log2((width >> 3)    + 511) + 1;
	_bundles[kSourceYOff].length          = log2((width >> 3)    + 511) + 1;

	_bundles[kSourcePattern].length       = log2((bw    << 3)    + 511) + 1;

	_bundles[kSourceRun].length           = log2((width >> 3)*48 + 511) + 1;
}

} // End of namespace Graphics