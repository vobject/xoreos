/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/resman.h
 *  The global resource manager for Aurora resources.
 */

#ifndef AURORA_RESMAN_H
#define AURORA_RESMAN_H

#include <string>
#include <stack>
#include <list>
#include <vector>
#include <map>

#include "common/types.h"
#include "common/filelist.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class KEYFile;
class BIFFile;

/** A resource manager holding information about and handling all request for all
 *  resources useable by the game.
 */
class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	/** Clear all resource information. */
	void clear();

	/** Return the detected game ID. */
	GameID getGameID() const;

	/** Save the current resource index onto the stack. */
	void stackPush();
	/** Pop the top-most resource index to the current state. */
	void stackPop();
	/** Apply the top-most resource index to the current state. */
	void stackApply();
	/** Drop the top-most resource index. */
	void stackDrop();

	/** Register a path to be the base data directory.
	 *
	 *  @param  path The path to a base data directory.
	 *  @return true if the path is a useable base data directory, false otherwise.
	 */
	bool registerDataBaseDir(const std::string &path);

	/** Load secondary resources.
	 *
	 *  Secondary resources are plain files found in the data directory structure.
	 */
	bool loadSecondaryResources();

	/** Return the list of KEY files found in the base data directory. */
	const Common::FileList &getKEYList() const;

	/** Return the list of ERF files found in the base data directory. */
	const Common::FileList &getERFList() const;

	/** Return the list of RIM files found in the base data directory. */
	const Common::FileList &getRIMList() const;

	/** Load a KEY index.
	 *
	 *  Add all resources found in the KEY and its BIF to the manager.
	 *
	 *  @param  key The KEY file to index.
	 *  @return true on success, false otherwise.
	 */
	bool loadKEY(Common::SeekableReadStream &key);

	/** Add resources found in the ERF file to the manager.
	 *
	 *  @param  erf The name of the ERF file within a valid ERF directory in the base dir.
	 *  @return true on success, false otherwise.
	 */
	bool addERF(const std::string &erf);

	/** Add resources found in the RIM file to the manager.
	 *
	 *  @param  rim The name of the RIM file within a valid RIM directory in the base dir.
	 *  @return true on success, false otherwise.
	 */
	bool addRIM(const std::string &rim);

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return true if the resource exists, fale otherwise.
	 */
	bool hasResource(const std::string &name, FileType type) const;

	/** Does a specific resource exists?
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's types.
	 *  @return true if the resource exists, fale otherwise.
	 */
	bool hasResource(const std::string &name, const std::vector<FileType> &types) const;

	/** Return a resource.
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  type The resource's type.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const std::string &name, FileType type) const;

	/** Return a resource.
	 *
	 *  This only returns one stream, even if more than one of the specified file types exist
	 *  for the given name.
	 *
	 *  @param  name The name (ResRef) of the resource.
	 *  @param  types A list of file types to look for.
	 *  @return The resource stream or 0 if the resource doesn't exist.
	 */
	Common::SeekableReadStream *getResource(const std::string &name, const std::vector<FileType> &types) const;

	/** Return a music resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @return The music resource stream or 0 if the music resource doesn't exist.
	 */
	Common::SeekableReadStream *getMusic(const std::string &name) const;

	/** Return a sound resource.
	 *
	 *  @param  name The name (ResRef or path) of the resource.
	 *  @return The sound resource stream or 0 if the sound resource doesn't exist.
	 */
	Common::SeekableReadStream *getSound(const std::string &name) const;

private:
	/** Where a resource can be found. */
	enum Source {
		kSourceBIF,  ///< Within a BIF file.
		kSourceERF,  ///< Within an ERF file.
		kSourceRIM,  ///< Within a RIM file.
		kSourceFile  ///< A direct file.
	};

	/** A resource. */
	struct Resource {
		FileType type; ///< The resource's type.

		Source source; ///< Where can the resource be found?

		// For kSourceBIF / kSourceERF
		uint32 idx;    ///< Index into the BIF/ERF vector.
		uint32 offset; ///< The offset within the BIF/ERF file.
		uint32 size;   ///< The size of the resource data.

		// For kSourceFile
		std::string path; ///< The file's path.
	};

	typedef std::vector<std::string> ResFileList;

	/** Map over resources with the same name but different type. */
	typedef std::map<FileType,    Resource>        ResourceTypeMap;
	/** Map over resources, indexed by name. */
	typedef std::map<std::string, ResourceTypeMap> ResourceMap;

	/** A state of the resource manager. */
	struct State {
		ResFileList bifs;      ///< BIFs used by the game resources.
		ResFileList erfs;      ///< ERFs currently searched for game resources.
		ResFileList rims;      ///< RIMs currently searched for game resources.
		ResourceMap resources; ///< All game-usable resources.

		void clear();
	};

	GameID _gameID;

	State _state; ///< The current state of the resource manager's index. */

	std::stack<State> _stateStack; ///< A stack with saved states. */

	std::string _baseDir; ///< The data base directory.
	std::string _modDir;  ///< The data directory for .mod files.
	std::string _hakDir;  ///< The data directory for .hak files.
	std::string _rimDir;  ///< The data directory for .rim files.

	Common::FileList _keyFiles; ///< List of all KEY files in the base directory.
	Common::FileList _bifFiles; ///< List of all BIF files in the base directory.
	Common::FileList _erfFiles; ///< List of all ERF files in the base directory.
	Common::FileList _rimFiles; ///< List of all RIM files in the base directory.

	void detectGameID(const Common::FileList &rootFiles);

	std::vector<FileType> _musicTypes; ///< All valid music file types.
	std::vector<FileType> _soundTypes; ///< All valid sound file types.

	// KEY/BIF loading helpers
	bool findBIFPaths(const KEYFile &keyFile, uint32 &bifStart);
	bool mergeKEYBIFResources(const KEYFile &keyFile, uint32 bifStart);

	void addResource(const Resource &resource, std::string name);
	void addResources(const Common::FileList &files);

	const Resource *getRes(std::string name, const std::vector<FileType> &types) const;

	Common::SeekableReadStream *getOffResFile(const ResFileList &list, const Resource &res) const;
};

} // End of namespace Aurora

#endif // AURORA_RESMAN_H
