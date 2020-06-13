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
#pragma once
#include <map>
#include <iostream>
#include <istream>
#include <fstream>
#include <tinyxml2.h>
#include "xml/util.h"

class MasterFile
{
private:
    typedef map<string,int> FileFlags;
    FileFlags files;

public:

    bool    exists(string name)
    {
        return(files.find(name) != files.end());
    }

    size_t    size()
    {
        return files.size();
    }

    void    add(string str)
    {
        files[str] = 1;
    }

    bool LoadXML(tinyxml2::XMLElement* pLoadedFiles)
    {
        files.clear();
        if (pLoadedFiles == nullptr)
        {
            return false;
        }

        tinyxml2::XMLElement* pGirlsFiles = pLoadedFiles->FirstChildElement("Girls_Files");
        if (pGirlsFiles == nullptr)
        {
            return false;
        }

        for(tinyxml2::XMLElement* pFile = pGirlsFiles->FirstChildElement("File");
            pFile != nullptr;
            pFile = pFile->NextSiblingElement("File"))
        {
            if (pFile->Attribute("Filename"))
            {
                add(pFile->Attribute("Filename"));
            }
        }

        return true;
    }

    tinyxml2::XMLElement& SaveXML(tinyxml2::XMLElement& pRoot)
    {
        auto& elLoadedFiles = PushNewElement(pRoot, "Loaded_Files");
        auto& elGirlsFiles = PushNewElement(elLoadedFiles, "Girls_Files");
        int numfiles = 0;
        FileFlags::const_iterator it;
        for(it = files.begin(); it != files.end(); it++)
        {
            auto& elFile = PushNewElement(elGirlsFiles, "File");
            elFile.SetAttribute("Filename", it->first.c_str());
            numfiles++;
        }
        elLoadedFiles.SetAttribute("NumberofFiles", numfiles);
        return elLoadedFiles;
    }
};
