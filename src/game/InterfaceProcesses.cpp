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
#include "utils/FileList.h"
#include "utils/DirPath.h"
#include "buildings/cBrothel.h"
#include "cObjectiveManager.hpp"
#include "Game.hpp"
#include "cGangs.h"
#include "cInventory.h"
#include "sConfig.h"

#undef bool

#ifdef LINUX
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

void NextWeek()
{
    g_LogFile.debug("turn", " *** NextWeek || Start ***");

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
    g_LogFile.info("save", "Saving game to '", filename, '\'');
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

    g_Game->save(*pRoot);
    doc.SaveFile(filename.c_str());
}
