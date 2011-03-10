/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/ingame.cpp
 *  The NWN ingame GUI elements.
 */

#include "engines/nwn/gui/ingame/ingame.h"
#include "engines/nwn/gui/ingame/main.h"
#include "engines/nwn/gui/ingame/quickbar.h"
#include "engines/nwn/gui/ingame/partyleader.h"

namespace Engines {

namespace NWN {

IngameGUI::IngameGUI() {
	_main = new IngameMainMenu;

	_quickbar = new Quickbar;

	_party.resize(1);
	_party[0] = new PartyLeader;
}

IngameGUI::~IngameGUI() {
	hide();

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		delete *p;

	delete _quickbar;

	delete _main;
}

int IngameGUI::showMain() {
	_main->show();
	int code = _main->run();
	_main->hide();

	return code;
}

void IngameGUI::show() {
	_quickbar->show();

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->show();
}

void IngameGUI::hide() {
	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->hide();

	_quickbar->hide();
}

void IngameGUI::setPortrait(uint partyMember, const Common::UString &portrait) {
	assert(partyMember < _party.size());

	_party[partyMember]->setPortrait(portrait);
}

void IngameGUI::setHealth(uint partyMember, float health) {
	assert(partyMember < _party.size());

	_party[partyMember]->setHealthLength(health);
}

void IngameGUI::setHealthy(uint partyMember) {
	assert(partyMember < _party.size());

	_party[partyMember]->setHealthColor(1.0, 0.0, 0.0, 1.0);
}

void IngameGUI::setSick(uint partyMember) {
	assert(partyMember < _party.size());

	_party[partyMember]->setHealthColor(189.0 / 255.0, 146.0 / 255.0,  74.0 / 255.0, 1.0);
}

void IngameGUI::setPoisoned(uint partyMember) {
	assert(partyMember < _party.size());

	_party[partyMember]->setHealthColor(132.0 / 255.0, 182.0 / 255.0,  74.0 / 255.0, 1.0);
}

} // End of namespace NWN

} // End of namespace Engines