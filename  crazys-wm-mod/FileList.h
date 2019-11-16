/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string>
#include <vector>

#include "DirPath.h"

using namespace std;

/*
* Just a container class to hold the path, filename
* and full path of a file.
*
* This is reinventing Boost, just a little. But then
* we don't need to build boost, which is a good thing
* on the whole.
*/
class FileListEntry {
	string m_path;	// the base path: ".\Resources\Characters"
	string m_leaf;	// the file name: "Girls.girls"
	string m_full;	// the full path: ".\Resources\Characters\Girls.girls"
public:
	FileListEntry() = default;
	FileListEntry(const FileListEntry &fle) {
		m_path = fle.m_path;
		m_leaf = fle.m_leaf;
		m_full = fle.m_full;
	}
	FileListEntry(string a_path, string a_leaf) {
		m_path = a_path;
		m_leaf = a_leaf;
#ifdef LINUX
		m_full = m_path + "/" + m_leaf;
#else
		m_full = m_path + "\\" + m_leaf;
#endif
	}
	/*
	*	and some simple accessors
	*/
	string &leaf() { return m_leaf; }
	string &path() { return m_path; }
	/*
	*	we could do this on the fly, here,
	*	but the overhead of storing an extra string
	*	should be trivial
	*/
	string &full() { return m_full; }
};

class FileList {
	DirPath folder;
	/*
	*	std::vector - standard template library class.
	*	produces a typed dynamic array that grows as needed
	*/
	vector<FileListEntry>	files;
protected:
	DirPath &folder_dp()	{ return folder; }
public:
	/*
	*	initialise the list with the DirPath for a folder and
	*	a pattern string. So:
	*
	*		DirPath dp = DirPath() << "Resources" << "Characters";
	*		FileList(dp, "*.girls");
	*
	*	The folder is scanned, and any files that match the pattern
	*	are stored in the vector
	*/
	FileList(DirPath dp, const char *pattern = "*");
	FileList(DirPath dp, const char *pattern, bool no_load);
	/*
	*	[] operators so you can subscript the list like an array
	*/
	FileListEntry &operator[](int index) {
		return files[index];	// just pass it on to the vector
	}
	/*
	*	returns the number of elements in the list
	*	(everything int STL uses size so we use that.)
	*/
	int	size() const	{ return files.size(); }
	/*
	*	scan lets us run another scan on the same folder
	*	but using a different pattern
	*/
	virtual	void	scan(const char *);
	virtual	void	add(const char *);		// `J` added
	/*
	*	I need to concatentate these babies...
	*/
	void	operator+=(FileList &l) {
		for (int i = 0; i < l.size(); i++) {
			files.push_back(l[i]);
		}
	}
};


class XMLFileList {
	DirPath folder;
	vector<FileListEntry>	files;
public:
	XMLFileList(DirPath dp, char const *pattern = "*");
	FileListEntry &operator[](int index) {
		return files[index];	// just pass it on to the vector
	}
	int	size() { return files.size(); }
	void	scan(const char *);
};

