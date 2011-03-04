/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/types.h
 *  Basic NWN type definitions.
 */

#ifndef ENGINES_NWN_TYPES_H
#define ENGINES_NWN_TYPES_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"

namespace Engines {

namespace NWN {

class Creature;

struct CharacterKey {
	Common::UString name;
	uint number;

	CharacterKey(const Common::UString &na = "", uint nu = 0);

	bool operator<(const CharacterKey &key) const;
};

class CharacterID {
public:
	CharacterID();
	~CharacterID();

	bool empty() const;

	uint getNumber() const;

	const Creature &operator*() const;
	const Creature *operator->() const;

	void clear();

private:
	typedef std::map<CharacterKey, Creature *> CharMap;

	bool _empty;
	CharMap::iterator _char;

	CharacterID(CharMap::iterator c);

	void set(CharMap::iterator c);

	friend class CharacterStore;
};

struct ModuleContext {
	Common::UString module;
	CharacterID pc;

	void clear();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_TYPES_H