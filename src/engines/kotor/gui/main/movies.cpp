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
#include "aurora/resman.h"

#include "engines/aurora/util.h"
#include "engines/aurora/widget.h"

#include "engines/kotor/gui/widgets/label.h"
#include "engines/kotor/gui/widgets/listbox.h"

#include "engines/kotor/gui/main/movies.h"

namespace Engines {

namespace KotOR {

MoviesMenu::MovieInfo::MovieInfo() : order(-1), showAlways(true) {
}

bool MoviesMenu::MovieInfo::operator <(const MovieInfo &rhs) const {
	 return order < rhs.order;
}


MoviesMenu::MoviesMenu() : _listbox(0) {
	load("titlemovie");

	_listbox = getListBox("LB_MOVIES", true);
}

MoviesMenu::~MoviesMenu() {
}

void MoviesMenu::show() {
	GUI::show();

	updateMovies();
	listVideos();

	// TODO: Fix positioning of listbox and its elements.
	_listbox->setPosition(0.0, 225.0, 0);
}

void MoviesMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "LB_MOVIES") {
		playSelection();
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

void MoviesMenu::updateMovies() {
	_movies.clear();

	std::list<Aurora::ResourceManager::ResourceID> movieFiles;
	ResMan.getAvailableResources(Aurora::kResourceVideo, movieFiles);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator it = movieFiles.begin();
		 it != movieFiles.end(); it++) {
		addMovie(it->name);
	}
	std::sort(_movies.begin(), _movies.end());
}

void MoviesMenu::addMovie(const Common::UString &filename) {
	const Aurora::TwoDAFile &movies = TwoDAReg.get("movies");

	for (uint i = 0; i < movies.getRowCount(); i++) {
		const uint32 strRef = movies.getRow(i).getInt("strrefname");

		MovieInfo movie;
		movie.name = TalkMan.getString(strRef);
		movie.filename = movies.getRow(i).getName();
		movie.order = movies.getRow(i).getInt("order");
		movie.showAlways = (movies.getRow(i).getInt("alwaysshow") == 0) ? false : true;

		if (movie.filename != filename)
			// Not the movie we are looking for.
			continue;

		if (movie.name.empty())
			// Use the filename if there is no pretty name.
			movie.name = movie.filename;

		_movies.push_back(movie);
		return;
	}

	// We could not find the name of the current movie file in the game's
	// resources. But it should be made available nevertheless.
	MovieInfo movie;
	movie.name = filename;
	movie.filename = filename;
	_movies.push_back(movie);
}

void MoviesMenu::listVideos() {
	_listbox->lock();
	_listbox->clear();

	for (std::vector<MovieInfo>::const_iterator v = _movies.begin();
		 v != _movies.end(); v++) {

		_listbox->add(new WidgetListItemTextLine(*this, "dialogfont16x16", v->name));
	}

	_listbox->unlock();
}

void MoviesMenu::playSelection() {
	playVideo(_movies[_listbox->getSelected()].filename);
}

} // End of namespace KotOR

} // End of namespace Engines

