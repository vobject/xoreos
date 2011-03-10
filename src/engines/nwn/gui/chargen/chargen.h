/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/chargen/chargen.h
 *  The NWN character generator.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARGEN_H
#define ENGINES_NWN_GUI_CHARGEN_CHARGEN_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN character generator. */
class CharGenMenu : public GUI {
public:
	CharGenMenu(Module &module);
	~CharGenMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARGEN_H