/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/gui/main/movies.cpp
 *  The movies menu.
 */

#include "common/util.h"

#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/talkman.h"

#include "engines/aurora/widget.h"

#include "engines/kotor/gui/main/movies.h"

namespace Engines {

namespace KotOR {

bool MoviesMenu::MovieInfo::operator <(const MovieInfo &rhs) const {
	 return order < rhs.order;
}


MoviesMenu::MoviesMenu() {
	load("titlemovie");

	updateMovies();
}

MoviesMenu::~MoviesMenu() {
}

void MoviesMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

void MoviesMenu::updateMovies() {
	const Aurora::TwoDAFile &movies = TwoDAReg.get("movies");

	for (uint i = 0; i < movies.getRowCount(); i++) {
		const uint32 strRef = movies.getRow(i).getInt("strrefname");
		const Common::UString name = TalkMan.getString(strRef);
		const uint order = movies.getRow(i).getInt("order");

		if (name.empty() || !order)
			// Invalid entry. Either because it does not have a name or a valid
			// order ("****"). It will not have a video file.
			continue;

		MovieInfo movie;
		movie.name = name;
		movie.order = order;
		movie.showAlways = (movies.getRow(i).getInt("alwaysshow") == 0) ? false : true;

		_movies.push_back(movie);
	}
	std::sort(_movies.begin(), _movies.end());
}

} // End of namespace KotOR

} // End of namespace Engines

