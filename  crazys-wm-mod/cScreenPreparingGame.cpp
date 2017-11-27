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

	prep_step = -1;
}

void cScreenPreparingGame::init()
{
	if (g_CurrentScreen != SCREEN_PREPARING)
	{
		prep_step = -1;
		loading = true;
		resetScreen();
	}

	g_CurrentScreen = SCREEN_PREPARING;
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
void cScreenPreparingGame::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	init();						// set up the window if needed
	if (prep_step > 100) loading = false;	// incase something breaks
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
			return;
		}
		return;
	}
	prep_step++;
	if (prep_step < 1) { resetScreen(); return; }
	if (g_ReturnInt == 0)		// load game
	{
		if (prep_step == 2)		{ ss1 << "Loading Game: " << g_ReturnText; ss2 << "Freeing Cache.\n"; }
		if (prep_step == 4)		{ g_Traits.Free(); g_Girls.Free(); g_InvManager.Free(); g_Brothels.Free(); g_Clinic.Free(); g_Studios.Free(); g_Arena.Free(); g_Centre.Free(); g_House.Free(); g_Farm.Free(); }
		if (prep_step == 6)		{ ss2 << "Load Game Info Files.\n"; }
		if (prep_step == 8)
		{ 
			LoadGameInfoFiles(); 
		}
		if (prep_step == 10)	{ ss2 << "Loading the Game XML.\n"; }
		if (prep_step == 12)
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
			
		}
		if (prep_step == 14)	{ ss2 << "Loading Girl Files.\n"; }
		if (prep_step == 16)
		{
			loadedGirlsFiles.LoadXML(hRoot.FirstChild("Loaded_Files"));
			LoadGirlsFiles();
		}
		if (prep_step == 18)	{ ss2 << "Loading Girls.\n"; }
		if (prep_step == 20)	{ g_Girls.LoadGirlsXML(hRoot.FirstChildElement("Girls")); }
		if (prep_step == 22)	{ ss2 << "Loading Gangs.\n"; }
		if (prep_step == 24)	{ g_Gangs.LoadGangsXML(hRoot.FirstChildElement("Gang_Manager")); }
		if (prep_step == 26)	{ ss2 << "Loading Brothels.\n"; }
		if (prep_step == 28)	{ g_Brothels.LoadDataXML(hRoot.FirstChildElement("Brothel_Manager")); }
		if (prep_step == 30)	{ ss2 << "Loading Clinic.\n"; }
		if (prep_step == 32)	{ g_Clinic.LoadDataXML(hRoot.FirstChildElement("Clinic_Manager")); }
		if (prep_step == 34)	{ ss2 << "Loading Studio.\n"; }
		if (prep_step == 36)	{ g_Studios.LoadDataXML(hRoot.FirstChildElement("MovieStudio_Manager")); }
		if (prep_step == 38)	{ ss2 << "Loading Arena.\n"; }
		if (prep_step == 40)	{ g_Arena.LoadDataXML(hRoot.FirstChildElement("Arena_Manager")); }
		if (prep_step == 42)	{ ss2 << "Loading Centre.\n"; }
		if (prep_step == 44)	{ g_Centre.LoadDataXML(hRoot.FirstChildElement("Centre_Manager")); }
		if (prep_step == 46)	{ ss2 << "Loading House.\n"; }
		if (prep_step == 48)	{ g_House.LoadDataXML(hRoot.FirstChildElement("House_Manager")); }
		if (prep_step == 50)	{ ss2 << "Loading Farm.\n"; } 
		if (prep_step == 52)	{ g_Farm.LoadDataXML(hRoot.FirstChildElement("Farm_Manager")); }
		if (prep_step == 54)	{ ss2 << "Loading Final Stuff.\n"; }
		if (prep_step == 56)
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
		}
		if (prep_step == 62)	{ ss2 << "Finished Loading.\n"; }
		if (prep_step == 99)
		{

			loading = false;
			g_WinManager.Pop();
			g_WinManager.push("Brothel Management");
			g_InitWin = true;
			prep_step = -1;
			return;
		}
		stringEmUp();
	}
	else						// new game
	{
		if (true)				// run the old new game
		{
			stringstream ss;
			ss << "Starting New Game: " << g_ReturnText;
			EditTextItem(ss.str(), text1_id);

			if (prep_step == 4)	NewGame();

		}
		else					// `J` added new new game for .06.03.00
		{

		}
	}
}

