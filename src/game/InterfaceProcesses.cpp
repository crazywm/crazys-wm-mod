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
#pragma region //    Includes and Externs            //
#include "InterfaceProcesses.h"
#include "main.h"
#include "Revision.h"
#include "FileList.h"
#include "MasterFile.h"
#include "DirPath.h"
#include "buildings/cBrothel.h"
#include "cObjectiveManager.hpp"
#include "Game.hpp"
#include "cGangs.h"
#include "cInventory.h"
#include "sConfig.h"

#undef bool

#ifdef LINUX
#include "linux.h"
#else
#include <windows.h>
#endif
#undef GetMessage

extern cRng g_Dice;
extern cConfig cfg;

extern    bool    g_CTRLDown;

#pragma endregion
#pragma region //    Local Variables            //

// globals used for the interface
string g_ReturnText;
int g_ReturnInt = -1;
bool g_AllTogle = false;    // used on screens when wishing to apply something to all items

// for keeping track of weather have walked around town today
bool g_WalkAround = false;
bool g_TryCentre = false;
bool g_TryOuts = false;
bool g_TryEr = false;
bool g_TryCast = false;

int g_TalkCount = 10;


vector<int> cycle_girls;  // globally available sorted list of girl IDs for Girl Details screen to cycle through
int cycle_pos;  //currently selected girl's position in the cycle_girls vector
int summarysortorder = 0;    // the order girls get sorted in the summary lists

#pragma endregion

//used to store what files we have loaded
MasterFile loadedGirlsFiles;

static string clobber_extension(string s)    // `J` debug logging
{
    size_t pos = s.rfind(".");
    string base = s.substr(0, pos);
    return base;
}

// interim loader to load XML files, and then non-xml ones if there was no xml version.
void LoadXMLItems(FileList &fl)
{
    map<string, string> lookup;
    g_LogFile.log(ELogLevel::DEBUG, "itemsx files:");
    fl.scan("*.itemsx");
    for (int i = 0; i < fl.size(); i++)
    {

        string str = fl[i].full();
        string key = clobber_extension(str);
        lookup[key] = str;
        g_LogFile.debug("items",   "    adding ", key, " = ", str);
    }

    // Iterate over the map and print out all key/value pairs. kudos: wikipedia
    g_LogFile.debug("items", "walking map...");
    for (auto & it : lookup)
    {
        try {
            string full_path = it.second;
            g_LogFile.debug("items", "\t\tLoading xml Item from", full_path);
            g_Game->inventory_manager().LoadItemsXML(full_path);
        } catch (std::runtime_error& error) {
            g_LogFile.error("items", "Could not load items from '", it.second, "': ", error.what());
        }
    }
}

void LoadGameInfoFiles()
{
    stringstream ss;
    // `J` Load .itemsx files
    // DirPath location_i = DirPath() << "Resources" << "Items"; // `J` moved items from Data to Items folder
    DirPath location_i = DirPath(cfg.folders.items().c_str());
    FileList fl_i(location_i, "*.itemsx");
    g_LogFile.log(ELogLevel::INFO, "Found ", fl_i.size(), " itemsx files");
    LoadXMLItems(fl_i);
    LoadNames();

}

void LoadNames()
{// `J` load names lists
    DirPath location_N = DirPath() << "Resources" << "Data" << "RandomGirlNames.txt";
    g_GirlNameList.load(location_N);
    DirPath location_SN = DirPath() << "Resources" << "Data" << "RandomLastNames.txt";
    g_SurnameList.load(location_SN);
    // `J` Added g_BoysNameList for .06.03.00
    DirPath location_B = DirPath() << "Resources" << "Data" << "RandomBoysNames.txt";
    g_BoysNameList.load(location_B);
}

void LoadGirlsFiles(MasterFile& master)
{
    /*
    *    now get a list of all the file in the Characters folder
    *    start by building a path...
    */
    DirPath location = DirPath(cfg.folders.characters().c_str());
    /*
    *    now scan for matching files. The XMLFileList
    *    will look for ".girls" and ".girlx" files
    *    with the XML versions shadowing the originals
    */
    XMLFileList girlfiles(location, "*.girls");
    XMLFileList rgirlfiles(location, "*.rgirls");
    /*
    *    And we need to know which ".girls" files the saved game processed
    *    This information is stored in the master file - so we read that.
    */
    for (int i = 0; i < girlfiles.size(); i++)
    {
        /*
        *        OK: if the current file is listed in the master file
        *        we don't need to load it. Unless the AllData flag is set
        *        and then we do. I think.
        */
        if (master.exists(girlfiles[i].leaf()))
        {
            continue;
        }
        /*
        *        add the file to the master list
        */
        master.add(girlfiles[i].leaf());
        /*
        *        load the file
        */
        g_Game->girl_pool().LoadGirlsXML(girlfiles[i].full());
    }
    /*
    *    Load random girls
    *
    *    "girlfiles" is still an XMLFileList, so this will get
    *    XML format files in preference to original format ones
    */
    for (int i = 0; i < rgirlfiles.size(); i++)
    {
        g_Game->girl_pool().LoadRandomGirl(rgirlfiles[i].full());
    }
}

void NextWeek()
{
    g_LogFile.log(ELogLevel::DEBUG, " *** NextWeek || Start ***");

    g_WalkAround = false;
    g_TryCentre = false;
    g_TryOuts = false;
    g_TryEr = false;
    g_TryCast = false;
    g_TalkCount = 10;
    /*
    // `J` I want to make the player start with 0 in all stats and skills
    // and have to gain them over time. When this gets implemented
    // g_TalkCount will be based on the player's charisma.
    g_TalkCount = 10 + (g_Game->player().m_Stats[STAT_CHARISMA] / 10);
    // */ //

    g_Game->next_week();

    g_CTRLDown = false;
    g_LogFile.log(ELogLevel::DEBUG, " *** NextWeek || End ***");
}

void AutoSaveGame()
{
    SaveGameXML(DirPath(cfg.folders.saves().c_str()) << "autosave.gam");
}
void SaveGame()
{
    string filename = g_Game->buildings().get_building(0).name();
    string filenamedotgam = filename + ".gam";

    SaveGameXML(DirPath(cfg.folders.saves().c_str()) << filenamedotgam);
    if (cfg.folders.backupsaves())
    {
        SaveGameXML(DirPath() << "Saves" << filenamedotgam);
    }
}

void SaveGameXML(string filename)
{
    tinyxml2::XMLDocument doc;
    doc.LinkEndChild(doc.NewDeclaration());
    auto* pRoot = doc.NewElement("Root");
    doc.LinkEndChild(pRoot);

    // output game version
    pRoot->SetAttribute("MajorVersion", g_MajorVersion);
    pRoot->SetAttribute("MinorVersionA", g_MinorVersionA);
    pRoot->SetAttribute("MinorVersionB", g_MinorVersionB);
    pRoot->SetAttribute("StableVersion", g_StableVersion);
    pRoot->SetAttribute("ApproxRevision", svn_revision);

    //if a user mods the exe, he can tell us that here
    //by changing it to anything besides official
    pRoot->SetAttribute("ExeVersion", "official");

    // output interface variables
    pRoot->SetAttribute("WalkAround", g_WalkAround);
    pRoot->SetAttribute("TalkCount", g_TalkCount);
    pRoot->SetAttribute("TryCentre", g_TryCentre);
    pRoot->SetAttribute("TryOuts", g_TryOuts);
    pRoot->SetAttribute("TryEr", g_TryEr);
    pRoot->SetAttribute("TryCast", g_TryCast);

    //this replaces the "master file"
    // TODO Fix the master file stuff
    loadedGirlsFiles.SaveXML(*pRoot);

    g_Game->save(*pRoot);
    doc.SaveFile(filename.c_str());

    //ADB TODO
#if 0
    //this stuff is not saved, save it and load it
    selected_girl = 0;
    for (int i = 0; i<8; i++)
    {
        MarketSlaveGirls[i] = 0;
        MarketSlaveGirlsDel[i] = -1;
    }

    // update the shop inventory
    g_Game->inventory_manager().UpdateShop();
#endif
}
