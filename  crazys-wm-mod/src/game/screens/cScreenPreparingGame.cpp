/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http:  //pinkpetal.org     // old site:   http:  //pinkpetal .co.cc
*
* This program is free software:   you can redistribute it and/or modify
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
* along with this program.  If not, see <http:  //www.gnu.org/licenses/>.
*/
#include "cScreenPreparingGame.h"
#include "buildings/cHouse.h"
#include "FileList.h"
#include "DirPath.h"
#include "InterfaceProcesses.h"
#include "MasterFile.h"
#include "cGangs.h"
#include "Game.hpp"
#include "character/cCustomers.h"
#include "cInventory.h"
#include "CLog.h"
#include "sConfig.h"

namespace settings {
    extern const char* INITIAL_RANDOM_GANGS;
    extern const char* INITIAL_BOOSTED_GANGS;
}

extern string g_ReturnText;
extern int g_ReturnInt;
extern MasterFile loadedGirlsFiles;

extern cNameList g_GirlNameList;
extern cNameList g_BoysNameList;
extern cNameList g_SurnameList;

extern bool g_WalkAround;
extern bool g_TryCentre;
extern bool g_TryOuts;
extern bool g_TryEr;
extern bool g_TryCast;
extern int g_TalkCount;
extern cConfig cfg;

bool loading = true;
int load0new1 = 0;
int prep_step = -1;
tinyxml2::XMLDocument	doc;
tinyxml2::XMLElement*	pRoot = nullptr;
stringstream ss1;
stringstream ss2;
stringstream ss3;
stringstream ss4;
stringstream ss5;

enum Load_Step {
l_freecache = 1,
l_traits,
l_items,
l_names,
l_gamexml,
l_girlfiles,
l_girls,
l_gangs,
l_finalstuff,
l_finished
};

enum New_Step {
	n_freecache = 1,
	n_LoadGameInfoFiles,
	n_Girls,
	n_Scripts,
	n_Player,
	n_Markets,
	n_GangsRivals,
	n_Saving,
	n_finished
};

cScreenPreparingGame::cScreenPreparingGame() : cGameWindow("preparing_game_screen.xml")
{
}
void cScreenPreparingGame::set_ids()
{
	text1_id			/**/ = get_id("Text1");
	text2_id			/**/ = get_id("Text2");
	text3_id			/**/ = get_id("Text3");
	text4_id			/**/ = get_id("Text4");
	text5_id			/**/ = get_id("Text5");

    cancel_id			/**/ = get_id("BackButton","Back");

    SetButtonCallback(cancel_id, [this]() {
        if (!loading)
        {
            prep_step = -1;
            loading = true;
            pop_to_window("Main Menu");
        }
    });

}

void cScreenPreparingGame::init(bool back)
{
    if(!g_Game) {
        g_Game = std::make_unique<Game>();
    }
	if (g_ReturnInt >= 0)
	{
		prep_step = 0;
		loading = true;
		load0new1 = g_ReturnInt;
		g_ReturnInt = -1;
		resetScreen();
		return;
	}
	Focused();
}
void cScreenPreparingGame::resetScreen()
{
	doc.Clear();
	pRoot = nullptr;
	ss1.str("");
	ss2.str("");
	ss3.str("");
	ss4.str("");
	ss5.str("");
	stringEmUp();
}
void cScreenPreparingGame::loadFailed()
{
	EditTextItem("Something went wrong while loading the game. Click the back button to go to the main menu.", text3_id);
}

void cScreenPreparingGame::stringEmUp()
{
	EditTextItem(ss1.str(), text1_id);
	EditTextItem(ss2.str(), text2_id);
	EditTextItem(ss3.str(), text3_id);
	EditTextItem(ss4.str(), text4_id);
	EditTextItem(ss5.str(), text5_id);
}

void LoadXMLItems(FileList &fl);

void cScreenPreparingGame::process()
{
	if (prep_step >( (load0new1 == 0? l_finished : n_finished) * 4) + 70) loading = false;	// incase something breaks
    DisableWidget(cancel_id, loading);
	if (!loading)
	{
		loadFailed();
		return;
	}
	if (load0new1 == 0)		// load game
	{
		switch (prep_step)
		{
		case (l_freecache * 4) - 2:  	{ ss1 << "Loading Game:   " << g_ReturnText; ss2 << "Freeing Cache.\n"; break; }
		case (l_traits * 4) - 2:  	{ ss2 << "Loading Traits.\n"; break; }
		case (l_traits * 4):  
		{
			g_Game->LoadData();
			break;
		}
		case (l_items * 4) - 2:  	{ ss2 << "Loading Items.\n"; break; }
		case (l_items * 4):  
		{
			DirPath location_i = DirPath(cfg.folders.items().c_str());
			FileList fl_i(location_i, "*.itemsx");
			g_LogFile.log(ELogLevel::INFO, "Found ", fl_i.size(), " itemsx files");
			LoadXMLItems(fl_i);
			break;
		}
		case (l_names * 4) - 2:  	{ ss2 << "Loading Names.\n"; break; }
		case (l_names * 4):  
		{
			DirPath location_N = DirPath() << "Resources" << "Data" << "RandomGirlNames.txt";		g_GirlNameList.load(location_N);
			DirPath location_SN = DirPath() << "Resources" << "Data" << "RandomLastNames.txt";		g_SurnameList.load(location_SN);
			DirPath location_B = DirPath() << "Resources" << "Data" << "RandomBoysNames.txt";		g_BoysNameList.load(location_B);	// `J` Added g_BoysNameList for .06.03.00
			break;
		}
		case (l_gamexml * 4) - 2:  	{ ss2 << "Loading the Game XML.\n"; break; }
		case (l_gamexml * 4):  
		{
			DirPath location = cfg.folders.saves().c_str();
			DirPath thefile = location.c_str();
			thefile << g_ReturnText;
			if (doc.LoadFile(thefile.c_str()) != tinyxml2::XML_SUCCESS) {
				loading = false;
				push_message(doc.ErrorStr(), 1);
			}
            pRoot = doc.FirstChildElement("Root");
			if (pRoot == nullptr) { loading = false; return; }
			// load the version
			int minorA = -1;
			pRoot->QueryIntAttribute("MinorVersionA", &minorA);
			if (minorA != 6) { push_message("You must start a new game with this version", 2); loading = false; return; }
			string version("<blank>");
			if (pRoot->Attribute("ExeVersion")) { version = pRoot->Attribute("ExeVersion"); }
			if (version != "official") { push_message("Warning, the exe was not detected as official, it was detected as " + version + ".  Attempting to load anyways.", 1); }
			g_Game->load(*pRoot);
			break;
		}
		case (l_girlfiles * 4) - 2:  { ss2 << "Loading Girl Files.\n"; break; }
		case (l_girlfiles * 4):  
		{
			loadedGirlsFiles.LoadXML(pRoot->FirstChildElement("Loaded_Files"));
			LoadGirlsFiles(loadedGirlsFiles);
			break;
		}
		case (l_girls * 4) - 2:			{ ss2 << "Loading Girls.\n"; break; }		case (l_girls * 4):		{ g_Game->girl_pool().LoadGirlsXML(pRoot->FirstChildElement("Girls")); break; }
		case (l_gangs * 4) - 2:			{ ss2 << "Loading Gangs.\n"; break; }		case (l_gangs * 4):		{ g_Game->gang_manager().LoadGangsXML(pRoot->FirstChildElement("Gang_Manager")); break; }
		case (l_finalstuff * 4) - 2:	{ ss2 << "Loading Final Stuff.\n"; break; }
		case (l_finalstuff * 4):  
		{
			g_WalkAround = false;	pRoot->QueryAttribute("WalkAround", &g_WalkAround);
			g_TryCentre = false;	pRoot->QueryAttribute("TryCentre", &g_TryCentre);	// `J` added
			g_TryOuts = false;		pRoot->QueryAttribute("TryOuts", &g_TryOuts);
			g_TryEr = false;		pRoot->QueryAttribute("TryEr", &g_TryEr);
			g_TryCast = false;		pRoot->QueryAttribute("TryCast", &g_TryCast);
			g_TalkCount = 0;		pRoot->QueryIntAttribute("TalkCount", &g_TalkCount);
			if (g_Game->allow_cheats()) { g_WalkAround = g_TryCentre = g_TryOuts = g_TryEr = g_TryCast = false; g_TalkCount = 10; }

            g_Game->UpdateMarketSlaves();
			break;
		}
		case (l_finished * 4) - 2:  	{ ss2 << "Finished Loading.\n"; break; }
		case (l_finished * 4) + 40:  
		{

			loading = false;
			set_active_building(&g_Game->buildings().get_building(0));
			replace_window("Building Management");
			prep_step = -1;
			return;
			break;
		}
		default:  
			break;
		}
		stringEmUp();
	}
	else						// new game
	{
		// `J` added new new game for .06.04.01
			loading = true;
			switch (prep_step)
			{
			case (n_freecache * 4) - 2:  	{ ss1 << "Starting New Game:   " << g_ReturnText; ss2 << "Freeing Cache.\n"; break; }
			case (n_LoadGameInfoFiles * 4) - 2:		{ ss2 << "Loading Game Info Files.\n"; break; }
			case (n_LoadGameInfoFiles * 4):			{
			    g_Game->LoadData();
				g_WalkAround = g_TryOuts = g_TryCentre = g_TryEr = g_TryCast = false;
				g_TalkCount = 10;
				g_Game->gold().reset();
				LoadGameInfoFiles();
				break;
			}
			case (n_Girls * 4) - 2:		{ ss2 << "Loading Girl Files.\n"; break; }
			case (n_Girls * 4) : {
				loadedGirlsFiles.LoadXML(nullptr);
				LoadGirlsFiles(loadedGirlsFiles);
				break;
			}
			case (n_Player * 4) - 2:		{ ss2 << "Loading Buildings and Player.\n"; break; }
			case (n_Player * 4) : {
                auto& new_brot = g_Game->buildings().AddBuilding(std::unique_ptr<IBuilding>(new sBrothel()));
                new_brot.m_NumRooms = 20;
                new_brot.m_MaxNumRooms = 250;
                new_brot.set_name(g_ReturnText);
                new_brot.set_background_image("Brothel0.jpg");
                set_active_building(&new_brot);
                auto& new_house = g_Game->buildings().AddBuilding(std::unique_ptr<IBuilding>(new sHouse()));
                new_house.m_NumRooms = 20;
                new_house.m_MaxNumRooms = 200;
                new_house.set_background_image("House.jpg");
                for(int i = 0; i < NUM_STATS; ++i) g_Game->player().set_stat(i, 60);
                for(int i = 0; i < NUM_SKILLS; ++i) g_Game->player().set_skill(i, 10);
				g_Game->player().SetToZero();
				break;
			}
			case (n_Markets * 4) - 2:		{ ss2 << "Preparing Slave Market and Shop.\n"; break; }
			case (n_Markets * 4) : {
                g_Game->UpdateMarketSlaves();
				break;
			}
			case (n_GangsRivals * 4) - 2:		{ ss2 << "Generating Gangs and Rivals.\n"; break; }
			case (n_GangsRivals * 4) : {
				int start_random_gangs = g_Game->settings().get_integer(settings::INITIAL_RANDOM_GANGS);
				int start_boosted_gangs = g_Game->settings().get_integer(settings::INITIAL_BOOSTED_GANGS);
				for (int i = 0; i < start_random_gangs; i++)	g_Game->gang_manager().AddNewGang(false);
				for (int i = 0; i < start_boosted_gangs; i++)	g_Game->gang_manager().AddNewGang(true);
				// Add the begining rivals
				for (int i = 0; i < 5; i++)
				{
					int str = g_Dice % 10 + 1;
					g_Game->rivals().CreateRival(
						str * 100,								// BribeRate	= 100-1000
						(str * 3) + (g_Dice % 11),	 			// Businesses	= 3-40
						str * 5000, 							// Gold			= 5000-50000
						(str / 2) + 1, 							// Bars			= 1-6
						(str / 4) + 1, 							// GambHalls	= 1-3
						(str * 5) + (g_Dice % (str * 5)), 		// Girls		= 5-100
						(str / 2) + 1, 							// Brothels		= 1-6
						g_Dice % 6 + 1, 						// Gangs		= 1-6
						str	 									// Power		= 1-10	// `J` added - The rivals power level
						);
				}
				break;
			}
			case (n_Saving * 4) - 2:		{ ss2 << "Saving Game.\n"; break; }
			case (n_Saving * 4) : {
			    if(g_ReturnText == "Cheat") {
			        g_Game->enable_cheating();
			    }
				SaveGame();
				break;
			}
			case (n_finished * 4) - 2:		{ ss2 << "Finished.\n"; break; }
			case (n_finished * 4) : {
				push_window("Building Management");
				g_Game->RunEvent("intro");
				break;
			}
			default:
				break;
			}
			stringEmUp();
	}
	prep_step++;
}

