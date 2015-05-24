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

		int numfiles = 0;
		FileFlags::const_iterator it;
		for(it = files.begin(); it != files.end(); it++)
		{
			TiXmlElement* pFile = new TiXmlElement("File");
			pGirlsFiles->LinkEndChild(pFile);
			pFile->SetAttribute("Filename", it->first);
			numfiles++;
		}
		pLoadedFiles->SetAttribute("NumberofFiles", numfiles);
		return pLoadedFiles;
	}
};
