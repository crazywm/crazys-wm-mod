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
#include "main.h"
#include "FileList.h"
#include "DirPath.h"
#include "cWindowManager.h"
#include "InterfaceProcesses.h"
#include "cScreenMainMenu.h"
#include "MasterFile.h"

extern bool g_InitWin;
extern int g_CurrentScreen;
extern string g_ReturnText;
extern int g_ReturnInt;
extern cWindowManager g_WinManager;
extern cScreenMainMenu g_MainMenu;
extern MasterFile loadedGirlsFiles;

extern bool g_WalkAround;
extern bool g_TryCentre;
extern bool g_TryOuts;
extern bool g_TryEr;
extern bool g_TryCast;
extern int g_TalkCount;
extern bool g_Cheats;

extern sGirl* selected_girl;  // global pointer for the currently selected girl
extern sGirl* MarketSlaveGirls[20];
extern int MarketSlaveGirlsDel[20];


bool cScreenPreparingGame::ids_set = false;
bool loading = true;
int load0new1 = 0;
int prep_step = -1;
DirPath			location;
DirPath			thefile;
TiXmlDocument	doc;
TiXmlHandle		hRoot =0 ;
TiXmlElement*	pRoot=0;
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
l_brothels,
l_clinic,
l_studio,
l_arena,
l_centre,
l_house,
l_farm,
l_finalstuff,
l_finished
};


cScreenPreparingGame::cScreenPreparingGame()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "preparing_game_screen.xml";
	m_filename = dp.c_str();
}
void cScreenPreparingGame::set_ids()
{
	ids_set = true;
	text1_id = get_id("Text1");
	text2_id = get_id("Text2");
	text3_id = get_id("Text3");
	text4_id = get_id("Text4");
	text5_id = get_id("Text5");

//	counter1_id = get_id("Counter1");
//	counter2_id = get_id("Counter2");
//	counter3_id = get_id("Counter3");
//	counter4_id = get_id("Counter4");
//	counter5_id = get_id("Counter5");

	cancel_id = get_id("BackButton");
}

void cScreenPreparingGame::init()
{
	g_CurrentScreen = SCREEN_PREPARING;
	if (g_ReturnInt >= 0)
	{
		prep_step = 0;
		loading = true;
		load0new1 = g_ReturnInt;
		g_ReturnInt = -1;
		resetScreen();
		return;
	}
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;
}
void cScreenPreparingGame::resetScreen()
{
	doc.Clear();
	hRoot = 0;
	pRoot = 0;
	ss1.str("");
	ss2.str("");
	ss3.str("");
	ss4.str("");
	ss5.str("");
	stringEmUp();
}
void cScreenPreparingGame::loadFailed()
{
	ss3.str("");
	ss3 << "Something went wrong while loading the game. Click the back button to go to the main menu.";
	EditTextItem(ss3.str(), text3_id);
}
void cScreenPreparingGame::stringEmUp()
{
	EditTextItem(ss1.str(), text1_id);
	EditTextItem(ss2.str(), text2_id);
	EditTextItem(ss3.str(), text3_id);
	EditTextItem(ss4.str(), text4_id);
	EditTextItem(ss5.str(), text5_id);
}
void cScreenPreparingGame::clearall()
{
	g_Traits.Free();
	g_Girls.Free();
	g_InvManager.Free();
	g_Brothels.Free();
	g_Clinic.Free();
	g_Studios.Free();
	g_Arena.Free();
	g_Centre.Free();
	g_House.Free();
	g_Farm.Free();
}
static string clobber_extension(string s)	// `J` debug logging
{

	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension:   s = " << s << endl;
	size_t pos = s.rfind(".");
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension:   pos = " << pos << endl;
	string base = s.substr(0, pos);
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension:   s = " << s << endl;
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension:   base = " << base << endl;
	return base;
}
static void LoadXMLItems(FileList &fl)
{
	map<string, string> lookup;
	int loglevel = 0;

	if (cfg.debug.log_items())			loglevel++;
	if (cfg.debug.log_extradetails())	loglevel++;

	g_LogFile.os() << "itemsx files:  " << endl;
	fl.scan("*.itemsx");
	for (int i = 0; i < fl.size(); i++)
	{

		string str = fl[i].full();
		string key = clobber_extension(str);
		lookup[key] = str;
		if (loglevel>0)
			g_LogFile.os() << "	adding " << str << endl;
		if (loglevel > 1)
		{
			g_LogFile.os() << "	under " << key << endl;
			g_LogFile.os() << "	result " << lookup[key] << endl;
		}
	}

	// Iterate over the map and print out all key/value pairs. kudos:   wikipedia
	if (loglevel > 0)	g_LogFile.os() << "walking map..." << endl;
	for (map<string, string>::const_iterator it = lookup.begin(); it != lookup.end(); ++it)
	{
		string full_path = it->second;
		if (loglevel > 1)	g_LogFile.os() << "\tkey = " << it->first << endl;
		if (loglevel > 1)	g_LogFile.os() << "\tpath = " << full_path << endl;
		if (loglevel > 0)	g_LogFile.os() << "\t\tLoading xml Item:   " << full_path << endl;
		g_InvManager.LoadItemsXML(full_path);
	}
}

void cScreenPreparingGame::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	init();						// set up the window if needed
	if (prep_step >(l_finished * 4) + 70) loading = false;	// incase something breaks
	DisableButton(cancel_id, loading);
	if (!loading)
	{
		loadFailed();
		if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, cancel_id))
		{
			prep_step = -1;
			loading = true;
			g_InitWin = true;
			g_WinManager.PopToWindow(&g_MainMenu);
		}
		return;
	}
	if (load0new1 == 0)		// load game
	{
		switch (prep_step)
		{
		case (l_freecache * 4) - 2:  	{ ss1 << "Loading Game:   " << g_ReturnText; ss2 << "Freeing Cache.\n"; break; }
		case (l_freecache * 4):		{ clearall(); break; }
		case (l_traits * 4) - 2:  	{ ss2 << "Loading Traits.\n"; break; }
		case (l_traits * 4):  
		{
			DirPath core = DirPath() << "Resources" << "Data" << "CoreTraits.traitsx";
			TiXmlDocument docTraits(core.c_str());
			if (docTraits.LoadFile())	{ g_Traits.LoadXMLTraits(core); }
			else
			{
				DirPath traitdir = DirPath() << "Resources" << "Data";
				FileList fl_t(traitdir, "*.traitsx");				// get a file list
				if (fl_t.size() > 0)
				{
					for (int i = 0; i < fl_t.size(); i++)				// loop over the list, loading the files
					{
						g_Traits.LoadXMLTraits(fl_t[i].full());
					}
				}
			}
			break;
		}
		case (l_items * 4) - 2:  	{ ss2 << "Loading Items.\n"; break; }
		case (l_items * 4):  
		{
			DirPath location_i = DirPath(cfg.folders.items().c_str());
			FileList fl_i(location_i, "*.itemsx");
			if (cfg.debug.log_items())	g_LogFile.os() << "Found " << fl_i.size() << " itemsx files" << endl;
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
			location = DirPath(cfg.folders.saves().c_str());
			thefile = location.c_str();
			thefile << g_ReturnText;
			if (doc.LoadFile(thefile.c_str()) == false) { loading = false; }
			hRoot = doc.FirstChildElement("Root");
			pRoot = hRoot.ToElement();
			if (pRoot == 0) { loading = false; return; }
			// load the version
			int minorA = -1;
			pRoot->QueryIntAttribute("MinorVersionA", &minorA);
			if (minorA != 6) { g_MessageQue.AddToQue("You must start a new game with this version", 2); loading = false; return; }
			string version("<blank>");
			if (pRoot->Attribute("ExeVersion")) { version = pRoot->Attribute("ExeVersion"); }
			if (version != "official") { g_MessageQue.AddToQue("Warning, the exe was not detected as official, it was detected as " + version + ".  Attempting to load anyways.", 1); }
			break;
		}
		case (l_girlfiles * 4) - 2:  { ss2 << "Loading Girl Files.\n"; break; }
		case (l_girlfiles * 4):  
		{
			loadedGirlsFiles.LoadXML(hRoot.FirstChild("Loaded_Files"));
			LoadGirlsFiles();
			break;
		}
		case (l_girls * 4) - 2:			{ ss2 << "Loading Girls.\n"; break; }		case (l_girls * 4):		{ g_Girls.LoadGirlsXML(hRoot.FirstChildElement("Girls")); break; }
		case (l_gangs * 4) - 2:			{ ss2 << "Loading Gangs.\n"; break; }		case (l_gangs * 4):		{ g_Gangs.LoadGangsXML(hRoot.FirstChildElement("Gang_Manager")); break; }
		case (l_brothels * 4) - 2:		{ ss2 << "Loading Brothels.\n"; break; }	case (l_brothels * 4):	{ g_Brothels.LoadDataXML(hRoot.FirstChildElement("Brothel_Manager")); break; }
		case (l_clinic * 4) - 2:		{ ss2 << "Loading Clinic.\n"; break; }		case (l_clinic * 4):	{ g_Clinic.LoadDataXML(hRoot.FirstChildElement("Clinic_Manager")); break; }
		case (l_studio * 4) - 2:		{ ss2 << "Loading Studio.\n"; break; }		case (l_studio * 4):	{ g_Studios.LoadDataXML(hRoot.FirstChildElement("MovieStudio_Manager")); break; }
		case (l_arena * 4) - 2:			{ ss2 << "Loading Arena.\n"; break; }		case (l_arena * 4):		{ g_Arena.LoadDataXML(hRoot.FirstChildElement("Arena_Manager")); break; }
		case (l_centre * 4) - 2:		{ ss2 << "Loading Centre.\n"; break; }		case (l_centre * 4):	{ g_Centre.LoadDataXML(hRoot.FirstChildElement("Centre_Manager")); break; }
		case (l_house * 4) - 2:			{ ss2 << "Loading House.\n"; break; }		case (l_house * 4):		{ g_House.LoadDataXML(hRoot.FirstChildElement("House_Manager")); break; }
		case (l_farm * 4) - 2:			{ ss2 << "Loading Farm.\n"; break; }		case (l_farm * 4):  	{ g_Farm.LoadDataXML(hRoot.FirstChildElement("Farm_Manager")); break; }
		case (l_finalstuff * 4) - 2:	{ ss2 << "Loading Final Stuff.\n"; break; }
		case (l_finalstuff * 4):  
		{
			g_CurrBrothel = 0;
			g_WalkAround = false;	pRoot->QueryValueAttribute<bool>("WalkAround", &g_WalkAround);
			g_TryCentre = false;	pRoot->QueryValueAttribute<bool>("TryCentre", &g_TryCentre);	// `J` added
			g_TryOuts = false;		pRoot->QueryValueAttribute<bool>("TryOuts", &g_TryOuts);
			g_TryEr = false;		pRoot->QueryValueAttribute<bool>("TryEr", &g_TryEr);
			g_TryCast = false;		pRoot->QueryValueAttribute<bool>("TryCast", &g_TryCast);
			g_TalkCount = 0;		pRoot->QueryIntAttribute("TalkCount", &g_TalkCount);
			if (g_Cheats) { g_WalkAround = g_TryCentre = g_TryOuts = g_TryEr = g_TryCast = false; g_TalkCount = 10; }

			g_Gold.loadGoldXML(hRoot.FirstChildElement("Gold"));			// load player gold
			pRoot->QueryValueAttribute<unsigned long>("Year", &g_Year);
			pRoot->QueryValueAttribute<unsigned long>("Month", &g_Month);
			pRoot->QueryValueAttribute<unsigned long>("Day", &g_Day);
			g_GlobalTriggers.LoadTriggersXML(hRoot.FirstChildElement("Triggers"));
			selected_girl = 0;
			for (int i = 0; i < 20; i++)
			{
				MarketSlaveGirls[i] = 0;
				MarketSlaveGirlsDel[i] = -1;
			}
			g_InvManager.UpdateShop();
			break;
		}
		case (l_finished * 4) - 2:  	{ ss2 << "Finished Loading.\n"; break; }
		case (l_finished * 4) + 40:  
		{

			loading = false;
			g_WinManager.Pop();
			g_WinManager.push("Brothel Management");
			g_InitWin = true;
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
		if (true)				// run the old new game
		{
			loading = true;
			clearall();
			stringstream ss;
			ss << "Starting New Game:   " << g_ReturnText;
			EditTextItem(ss.str(), text1_id);

			if (prep_step == 4)	NewGame();

		}
		else					// `J` added new new game for .06.03.00
		{

		}
	}
	prep_step++;
}

