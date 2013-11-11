/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include <map>
#include <iostream>
#include <istream>
#include <fstream>
#include "tinyxml.h"

class MasterFile
{
private:
	typedef map<string,int> FileFlags;
	FileFlags files;

public:
	void LoadLegacy(string filename)
	{
		files.clear();
		ifstream ifs;
		char buffer[1024];	// power of 2 makes better use of memory
/*
 *		format the path for the master file. 
 */
		string mfile = filename + ".mast";
		DirPath mastfile = DirPath() << "Saves" << mfile;
		filename = mastfile.c_str();
/*
 *		open the file
 */
		ifs.open(mastfile.c_str());
/*
 *		problem opening the file?
 */
		if(!ifs.good())
		{
			ifs.close();
			return;
		}
/*
 *		loop through the file, one line at a time
 */
		while(ifs.good())
		{
/*
 *			Using "sizeof()" means that the size is right even if the
 *			buffer size is later changed.
 */
			ifs.getline(buffer, sizeof(buffer)-1, '\n');

			if (string(buffer).empty() == false)
			{
/*
 *				add the file to the map
 */
				files[string(buffer)] = 1;
			}
		}
		ifs.close();
	}

	bool	exists(string name)
	{
		return(files.find(name) != files.end());
	}

	size_t	size()
	{
		return files.size();
	}

	void	add(string str)
	{
		files[str] = 1;
	}

	bool LoadXML(TiXmlHandle hLoadedFiles)
	{
		files.clear();
		TiXmlElement* pLoadedFiles = hLoadedFiles.ToElement();
		if (pLoadedFiles == 0)
		{
			return false;
		}

		TiXmlElement* pGirlsFiles = pLoadedFiles->FirstChildElement("Girls_Files");
		if (pGirlsFiles == 0)
		{
			return false;
		}

		for(TiXmlElement* pFile = pGirlsFiles->FirstChildElement("File");
			pFile != 0;
			pFile = pFile->NextSiblingElement("File"))
		{
			if (pFile->Attribute("Filename"))
			{
				add(pFile->Attribute("Filename"));
			}
		}

		return true;
	}

	TiXmlElement* SaveXML(TiXmlElement* pRoot)
	{
		TiXmlElement* pLoadedFiles = new TiXmlElement("Loaded_Files");
		pRoot->LinkEndChild(pLoadedFiles);

		TiXmlElement* pGirlsFiles = new TiXmlElement("Girls_Files");
		pLoadedFiles->LinkEndChild(pGirlsFiles);

		FileFlags::const_iterator it;
		for(it = files.begin(); it != files.end(); it++)
		{
			TiXmlElement* pFile = new TiXmlElement("File");
			pGirlsFiles->LinkEndChild(pFile);
			pFile->SetAttribute("Filename", it->first);
		}
		return pLoadedFiles;
	}
};
