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
#pragma region //	Includes and Externs			//
#include "InterfaceProcesses.h"
#include "InterfaceGlobals.h"
#include "GameFlags.h"
#include "main.h"
#include "cGetStringScreenManager.h"
#include "cTariff.h"
#include "cScriptManager.h"
#include "Revision.h"
#include "cScreenBrothelManagement.h"
#include "FileList.h"
#include "MasterFile.h"
#include "DirPath.h"
#include "cScreenGirlDetails.h"
#include "cBrothel.h"

#undef bool

#ifdef LINUX
#include "linux.h"
#else
#include <windows.h>
#endif
#undef GetMessage

extern cScreenGirlDetails g_GirlDetails;
extern cScreenBrothelManagement g_BrothelManagement;
extern sInterfaceIDs g_interfaceid;
extern bool eventrunning;
extern cRng g_Dice;

extern int g_BrothelScreenImgX, g_BrothelScreenImgY, g_BrothelScreenImgW, g_BrothelScreenImgH;

extern cPlayer* The_Player;

extern bool g_LeftArrow;	extern bool g_RightArrow;	extern bool g_UpArrow;		extern bool g_DownArrow;
extern bool g_EnterKey;		extern bool g_AltKeys;		extern bool g_SpaceKey;		extern bool g_EscapeKey;
extern bool g_HomeKey;		extern bool g_EndKey;		extern bool g_PageUpKey;	extern bool g_PageDownKey;

extern bool g_A_Key;		extern bool g_B_Key;		extern bool g_C_Key;		extern bool g_D_Key;
extern bool g_E_Key;		extern bool g_F_Key;		extern bool g_G_Key;		extern bool g_H_Key;
extern bool g_I_Key;		extern bool g_J_Key;		extern bool g_K_Key;		extern bool g_L_Key;
extern bool g_M_Key;		extern bool g_N_Key;		extern bool g_O_Key;		extern bool g_P_Key;
extern bool g_Q_Key;		extern bool g_R_Key;		extern bool g_S_Key;		extern bool g_T_Key;
extern bool g_U_Key;		extern bool g_V_Key;		extern bool g_W_Key;		extern bool g_X_Key;
extern bool g_Y_Key;		extern bool g_Z_Key;
extern	bool	g_CTRLDown;

extern int g_CurrentScreen;

#pragma endregion
#pragma region //	Local Variables			//

// globals used for the interface
string g_ReturnText = "";
int g_ReturnInt = -1;
bool g_InitWin = true;
bool g_AllTogle = false;	// used on screens when wishing to apply something to all items
long g_IntReturn;

// for keeping track of weather have walked around town today
bool g_WalkAround = false;
bool g_TryCentre = false;
bool g_TryOuts = false;
bool g_TryEr = false;
bool g_TryCast = false;

int g_TalkCount = 10;
// g_GenGirls - false means the girls need to be generated
bool g_GenGirls = false;
bool g_Cheats = false;


sGirl* selected_girl;  // global pointer for the currently selected girl
vector<int> cycle_girls;  // globally available sorted list of girl IDs for Girl Details screen to cycle through
int cycle_pos;  //currently selected girl's position in the cycle_girls vector
int summarysortorder = 0;	// the order girls get sorted in the summary lists

sGirl* MarketSlaveGirls[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int MarketSlaveGirlsDel[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

CSurface* g_BrothelImages[7] = { 0, 0, 0, 0, 0, 0, 0 };

#pragma endregion

//used to store what files we have loaded
MasterFile loadedGirlsFiles;

void NewGame()
{
	
	cScriptManager sm;

	g_GenGirls = g_Cheats = false;
	// for keeping track of weather have walked around town today
	g_WalkAround = g_TryOuts = g_TryCentre = g_TryEr = g_TryCast = false;

	g_TalkCount = 10;
	g_Brothels.Free();
	g_Clinic.Free();
	g_Studios.Free();
	g_Arena.Free();
	g_Centre.Free();
	g_House.Free();
	g_Farm.Free();
	g_Gangs.Free();
	g_Customers.Free();
	g_Girls.Free();
	g_Traits.Free();
	g_GlobalTriggers.Free();
	sm.Release();
	g_InvManager.Free();

	string d = g_ReturnText;
	g_Cheats = (g_ReturnText == "Cheat") ? true : false;

	d += ".gam";

	// Load all the data
	LoadGameInfoFiles();
	loadedGirlsFiles.LoadXML(TiXmlHandle(0));
	LoadGirlsFiles();


	g_GlobalTriggers.LoadList(DirPath() << "Resources" << "Scripts" << "GlobalTriggers.xml");

	g_CurrBrothel = 0;

	g_Gold.reset();

	g_Year = 1209; g_Month = 1; g_Day = 1;

	selected_girl = 0;
	for (int i = 0; i < 20; i++)
	{
		MarketSlaveGirls[i] = 0;
		MarketSlaveGirlsDel[i] = -1;
	}

	g_Brothels.NewBrothel(20, 250);
	g_Brothels.SetName(0, g_ReturnText);
	for (int i = 0; i < NUM_STATS; i++)		The_Player->m_Stats[i] = 60;
	for (u_int i = 0; i < NUM_SKILLS; i++)	The_Player->m_Skills[i] = 10;
	The_Player->SetToZero();

	g_House.NewBrothel(20, 200);
	g_House.SetName(0, "House");

	u_int start_random_gangs = cfg.gangs.start_random();
	u_int start_boosted_gangs = cfg.gangs.start_boosted();
	for (u_int i = 0; i < start_random_gangs; i++)	g_Gangs.AddNewGang(false);
	for (u_int i = 0; i < start_boosted_gangs; i++)	g_Gangs.AddNewGang(true);

	// update the shop inventory
	g_InvManager.UpdateShop();

	// Add the begining rivals
	for (int i = 0; i < 5; i++)
	{
		int str = g_Dice % 10 + 1;
		g_Brothels.GetRivalManager()->CreateRival(
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

	if (g_Cheats)
	{
		g_Gold.cheat();
		g_InvManager.GivePlayerAllItems();
		g_Gangs.NumBusinessExtorted(500);
	}

	g_WinManager.push("Brothel Management");

	DirPath text;
	sm.Load(ScriptPath("Intro.lua"), 0);
	SaveGame();
	g_InitWin = true;

}

void GetString()
{
	cGetStringScreenManager gssm;
	gssm.process();
}

static string clobber_extension(string s)	// `J` debug logging
{
	
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: s = " << s << endl;
	size_t pos = s.rfind(".");
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: pos = " << pos << endl;
	string base = s.substr(0, pos);
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: s = " << s << endl;
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: base = " << base << endl;
	return base;
}

void LoadGameScreen()
{

	DirPath location = DirPath(cfg.folders.saves().c_str());
	const char *pattern = "*.gam";
	FileList fl(location, pattern);

	int selected = 0;
	if (g_InitWin)
	{
		g_InitWin = false;
		g_LoadGame.Focused();
		g_LoadGame.ClearListBox(g_interfaceid.LIST_LOADGSAVES);	// clear the list box with the save games
		for (int i = 0; i < fl.size(); i++)						// loop through the files, adding them to the box
		{
			if (fl[i].leaf() != "autosave.gam")	g_LoadGame.AddToListBox(g_interfaceid.LIST_LOADGSAVES, i, fl[i].leaf());
			else if (i == selected) selected++;	// make sure autosave.gam is not the selected item
		}
		g_LoadGame.SetSelectedItemInList(g_interfaceid.LIST_LOADGSAVES, selected);
	}

	/*
	*	no events process means we can go home early
	*/
	if (g_InterfaceEvents.GetNumEvents() == 0 && !g_EscapeKey && !g_UpArrow && !g_DownArrow && !g_EnterKey)
	{
		return;
	}

	/*
	*	the next simplest case is the "back" button
	*/
	if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGBACK)
		|| g_EscapeKey)
	{
		g_EscapeKey = false;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

	int selection;
	if (g_UpArrow)
	{
		g_UpArrow = false;
		selection = g_LoadGame.ArrowUpListBox(g_interfaceid.LIST_LOADGSAVES);
	}
	if (g_DownArrow)
	{
		g_DownArrow = false;
		selection = g_LoadGame.ArrowDownListBox(g_interfaceid.LIST_LOADGSAVES);
	}

	/*
	*	by this point, we're only interested if it's a click on the load game button or a double-click on a game in the list
	*/
	if (!g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGLOAD)
		&& !g_LoadGame.ListDoubleClicked(g_interfaceid.LIST_LOADGSAVES) && !g_EnterKey)
	{
		return;
	}
	/*
	*	OK: So from this point onwards, we're loading the game
	*/
	selection = g_LoadGame.GetLastSelectedItemFromList(g_interfaceid.LIST_LOADGSAVES);
	/*
	*	nothing selected means nothing more to do
	*/
	if (selection == -1)
	{
		return;
	}
	string temp = fl[selection].leaf();
	g_ReturnText = temp;
	/*
	*	enable cheat mode for a cheat brothel
	*/
	g_Cheats = (temp == "Cheat.gam");

	g_ReturnInt = 0;
	g_InitWin = true;
	g_WinManager.Pop();
	g_WinManager.push("Preparing Game");
	return;
}
// interim loader to load XML files, and then non-xml ones if there was no xml version.
static void LoadXMLItems(FileList &fl)
{
	map<string, string> lookup;
	int loglevel = 0;
	
	if (cfg.debug.log_items())			loglevel++;
	if (cfg.debug.log_extradetails())	loglevel++;

	g_LogFile.os() << "itemsx files:" << endl;
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

	// Iterate over the map and print out all key/value pairs. kudos: wikipedia
	if (loglevel > 0)	g_LogFile.os() << "walking map..." << endl;
	for (map<string, string>::const_iterator it = lookup.begin(); it != lookup.end(); ++it)
	{
		string full_path = it->second;
		if (loglevel > 1)	g_LogFile.os() << "\tkey = " << it->first << endl;
		if (loglevel > 1)	g_LogFile.os() << "\tpath = " << full_path << endl;
		if (loglevel > 0)	g_LogFile.os() << "\t\tLoading xml Item: " << full_path<< endl;
		g_InvManager.LoadItemsXML(full_path);
	}
}
void LoadGameInfoFiles()
{
	stringstream ss;
	int loadtraits = 0;		// 0=default, 1=xml, 2=txt
	DirPath location = DirPath() << "Resources" << "Data";
	DirPath dp = DirPath() << "Resources" << "Data" << "CoreTraits.traitsx";
	TiXmlDocument docTraits(dp.c_str());
	if (docTraits.LoadFile())	loadtraits = 1;
	if (loadtraits == 1)
	{
		g_Traits.LoadXMLTraits(dp);
	}
	else if (loadtraits == 0)
	{
		FileList fl_t(location, "*.traitsx");				// get a file list
        for (int i = 0; i < fl_t.size(); i++)				// loop over the list, loading the files
        {
            g_Traits.LoadXMLTraits(fl_t[i].full());
        }
	}

	// `J` Load .itemsx files
	// DirPath location_i = DirPath() << "Resources" << "Items"; // `J` moved items from Data to Items folder
	DirPath location_i = DirPath(cfg.folders.items().c_str());
	FileList fl_i(location_i, "*.itemsx");
	if (cfg.debug.log_items())	g_LogFile.os() << "Found " << fl_i.size() << " itemsx files" << endl;
	LoadXMLItems(fl_i);

	// `J` load names lists
	DirPath location_N = DirPath() << "Resources" << "Data" << "RandomGirlNames.txt";
	g_GirlNameList.load(location_N);
	DirPath location_SN = DirPath() << "Resources" << "Data" << "RandomLastNames.txt";
	g_SurnameList.load(location_SN);
	// `J` Added g_BoysNameList for .06.03.00
	DirPath location_B = DirPath() << "Resources" << "Data" << "RandomBoysNames.txt";
	g_BoysNameList.load(location_B);



}
void LoadGirlsFiles()
{
	/*
	*	now get a list of all the file in the Characters folder
	*	start by building a path...
	*/
	DirPath location; 
		location = DirPath(cfg.folders.characters().c_str());
	/*
	*	now scan for matching files. The XMLFileList
	*	will look for ".girls" and ".girlx" files
	*	with the XML versions shadowing the originals
	*/
	XMLFileList girlfiles(location, "*.girls");
	XMLFileList rgirlfiles(location, "*.rgirls");
	/*
	*	And we need to know which ".girls" files the saved game processed
	*	This information is stored in the master file - so we read that.
	*/
	for (int i = 0; i < girlfiles.size(); i++)
	{
		/*
		*		OK: if the current file is listed in the master file
		*		we don't need to load it. Unless the AllData flag is set
		*		and then we do. I think.
		*/
		if (loadedGirlsFiles.exists(girlfiles[i].leaf()))
		{
			continue;
		}
		/*
		*		add the file to the master list
		*/
		loadedGirlsFiles.add(girlfiles[i].leaf());
		/*
		*		load the file
		*/
		g_Girls.LoadGirlsDecider(girlfiles[i].full());
	}
	/*
	*	Load random girls
	*
	*	"girlfiles" is still an XMLFileList, so this will get
	*	XML format files in preference to original format ones
	*/
	for (int i = 0; i < rgirlfiles.size(); i++)
	{
		g_Girls.LoadRandomGirl(rgirlfiles[i].full());
	}
}

void NextWeek()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Start"; g_LogFile.ssend(); }

	g_GenGirls = g_WalkAround = false;
	g_GenGirls = g_TryCentre = false;
	g_GenGirls = g_TryOuts = false;
	g_GenGirls = g_TryEr = false;
	g_GenGirls = g_TryCast = false;
	g_TalkCount = 10;
	/*
	// `J` I want to make the player start with 0 in all stats and skills
	// and have to gain them over time. When this gets implemented
	// g_TalkCount will be based on the player's charisma.
	g_TalkCount = 10 + (The_Player->m_Stats[STAT_CHARISMA] / 10);
	// */ //

	if (g_Cheats)	g_Gold.cheat();

	// Clear choice dialog
	g_ChoiceManager.Free();

	// update the shop inventory
	g_InvManager.UpdateShop();

	// Clear the interface events
	g_InterfaceEvents.ClearEvents();

	// clear the events of dungeon girls
	g_Brothels.m_Dungeon.ClearDungeonGirlEvents();

	// pass out potions and nets to gangs for the start of the shift
	g_Gangs.GangStartOfShift();

	// go through and update all the brothels (this updates the girls each brothel has and calculates sex and stuff)
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Clinic"; g_LogFile.ssend(); }
	if (g_Clinic.GetNumBrothels() > 0)		g_Clinic.UpdateClinic();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Studio"; g_LogFile.ssend(); }
	if (g_Studios.GetNumBrothels() > 0)		g_Studios.UpdateMovieStudio();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Arena"; g_LogFile.ssend(); }
	if (g_Arena.GetNumBrothels() > 0)		g_Arena.UpdateArena();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Centre"; g_LogFile.ssend(); }
	if (g_Centre.GetNumBrothels() > 0)		g_Centre.UpdateCentre();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Farm"; g_LogFile.ssend(); }
	if (g_Farm.GetNumBrothels() > 0)		g_Farm.UpdateFarm();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin House"; g_LogFile.ssend(); }
	g_House.UpdateHouse();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Brothels"; g_LogFile.ssend(); }
	g_Brothels.UpdateBrothels(); // Moved so new buildings show up in profit reports --PP

	// go ahead and handle pregnancies for girls not controlled by player
	g_Girls.UncontrolledPregnancies();

	// go through and update all the gang-related data (send them on missions, etc.)
	g_Gangs.UpdateGangs();

	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Begin Dungeon"; g_LogFile.ssend(); }
	g_Brothels.m_Dungeon.Update();	// update the people in the dungeon

	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || End Buildings"; g_LogFile.ssend(); }

	// update objectives or maybe create a new one
	if (g_Brothels.GetObjective()) g_Brothels.UpdateObjective();
	else if (g_Dice.percent(45)) g_Brothels.CreateNewObjective();

	// go through and update the population base
	g_Customers.ChangeCustomerBase();

	// Free customers
	g_Customers.Free();

	// Update the time
	g_Day += 7;
	if (g_Day > 30)
	{
		g_Day = g_Day - 30;
		g_Month++;
		if (g_Month > 12)
		{
			g_Month = 1;
			g_Year++;
		}
	}

	// update the players gold
	g_Gold.week_end();

	// Process Triggers
	g_GlobalTriggers.ProcessTriggers();

	GameEvents();
	g_CTRLDown = false;
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || End"; g_LogFile.ssend(); }

}

void GameEvents()
{
	cScriptManager sm;
	/*
	*	The "" on the end forces it to end in a / or \
	*/
	DirPath dp = DirPath() << "Resources" << "Scripts" << "";

	if (!sm.IsActive())
		eventrunning = false;

	// process global triggers here
	if (g_GlobalTriggers.GetNextQueItem() && !eventrunning)
	{
		g_GlobalTriggers.ProcessNextQueItem(dp);
		eventrunning = true;
	}
	else if (!eventrunning)	// check girl scripts
	{
		if (g_Brothels.CheckScripts())
			eventrunning = true;
	}

	if (eventrunning)
	{
		sm.RunScript();
	}

	if (CheckGameFlag(FLAG_DUNGEONGIRLDIE))	// a girl has died int the dungeon
	{
		g_MessageQue.AddToQue("A girl has died in the dungeon.\nHer body will be removed by the end of the week.", 1);

		if (g_Dice.percent(10))	// only 10% of being discovered
		{
			The_Player->suspicion(1);
		}
		The_Player->disposition(-1);
		g_Brothels.UpdateAllGirlsStat(0, STAT_PCFEAR, 2);

		ClearGameFlag(FLAG_DUNGEONGIRLDIE);
	}
	else if (CheckGameFlag(FLAG_DUNGEONCUSTDIE))	// a customer has died in the dungeon
	{
		g_MessageQue.AddToQue("A customer has died in the dungeon.\nTheir body will be removed by the end of the week.", 1);

		if (g_Dice.percent(10))	// only 10% chance of being found out
		{
			The_Player->suspicion(1);
		}
		The_Player->disposition(-1);
		The_Player->customerfear(1);

		ClearGameFlag(FLAG_DUNGEONCUSTDIE);
	}
}

void AutoSaveGame()
{
	SaveGameXML(DirPath(cfg.folders.saves().c_str()) << "autosave.gam");
}
void SaveGame(bool saveCSV)
{

	string filename = g_Brothels.GetBrothel(0)->m_Name;
	string filenamedotgam = filename + ".gam";
	string filenamedotcsv = filename + ".csv";

	SaveGameXML(DirPath(cfg.folders.saves().c_str()) << filenamedotgam);
	if (saveCSV) SaveGirlsCSV(DirPath(cfg.folders.saves().c_str()) << filenamedotcsv);
	if (cfg.folders.backupsaves())
	{
		SaveGameXML(DirPath() << "Saves" << filenamedotgam);
		if (saveCSV) SaveGirlsCSV(DirPath() << "Saves" << filenamedotcsv);
	}
}
void SimpleSaveGameXML(string filename)	// `J` zzzzzz - incomplete code
{
	TiXmlDocument docq(filename + ".q");
	TiXmlDeclaration* declq = new TiXmlDeclaration("1.0", "", "yes");
	docq.LinkEndChild(declq);
	TiXmlElement* qRoot = new TiXmlElement("Root");
	docq.LinkEndChild(qRoot);

	qRoot->SetAttribute("Year", g_Year);
	qRoot->SetAttribute("Month", g_Month);
	qRoot->SetAttribute("Day", g_Day);

	qRoot->SetAttribute("NumberOfBrothels", g_Brothels.GetNumBrothels());
	qRoot->SetAttribute("Gold", g_Gold.ival());

	qRoot->SetAttribute("LoadedGirlsFiles", loadedGirlsFiles.size());

	// Save Girls
	vector<string> GirlNames;

	TiXmlElement* pGirls = new TiXmlElement("GirlFoldersUsedInThisGame");
	qRoot->LinkEndChild(pGirls);
	for (int i = 0; i < 14; i++)
	{
		sGirl* girl = 0;
		if (i == 0)/*                                       */	girl = g_Brothels.GetBrothel(0)->m_Girls;
		if (i == 1 && g_Brothels.GetNumBrothels() >= 2)/*   */	girl = g_Brothels.GetBrothel(1)->m_Girls;
		if (i == 2 && g_Brothels.GetNumBrothels() >= 3)/*   */	girl = g_Brothels.GetBrothel(2)->m_Girls;
		if (i == 3 && g_Brothels.GetNumBrothels() >= 4)/*   */	girl = g_Brothels.GetBrothel(3)->m_Girls;
		if (i == 4 && g_Brothels.GetNumBrothels() >= 5)/*   */	girl = g_Brothels.GetBrothel(4)->m_Girls;
		if (i == 5 && g_Brothels.GetNumBrothels() >= 6)/*   */	girl = g_Brothels.GetBrothel(5)->m_Girls;
		if (i == 6 && g_Brothels.GetNumBrothels() >= 7)/*   */	girl = g_Brothels.GetBrothel(6)->m_Girls;
		if (i == 7 && g_Studios.GetNumBrothels() > 0)/*     */	girl = g_Studios.GetBrothel(0)->m_Girls;
		if (i == 8 && g_Arena.GetNumBrothels() > 0)/*       */	girl = g_Arena.GetBrothel(0)->m_Girls;
		if (i == 9 && g_Centre.GetNumBrothels() > 0)/*      */	girl = g_Centre.GetBrothel(0)->m_Girls;
		if (i == 10 && g_Clinic.GetNumBrothels() > 0)/*     */	girl = g_Clinic.GetBrothel(0)->m_Girls;
		if (i == 11 && g_Farm.GetNumBrothels() > 0)/*       */	girl = g_Farm.GetBrothel(0)->m_Girls;
		if (i == 12)/*                                      */	girl = g_House.GetBrothel(0)->m_Girls;
		if (i == 13 && g_Brothels.GetDungeon()->GetNumGirls())
		{
			sDungeonGirl* dgirl = g_Brothels.GetDungeon()->GetGirl(0);
			while (dgirl)
			{
				if (find(GirlNames.begin(), GirlNames.end(), dgirl->m_Girl->m_Name) == GirlNames.end())
					GirlNames.push_back(dgirl->m_Girl->m_Name);
				dgirl = dgirl->m_Next;
			}
		}
		else
		{
			while (girl)
			{
				if (find(GirlNames.begin(), GirlNames.end(), girl->m_Name) == GirlNames.end())
					GirlNames.push_back(girl->m_Name);
				girl = girl->m_Next;
			}
		}
	}

	sort(GirlNames.begin(), GirlNames.end());
	for (u_int i = 0; i < GirlNames.size(); i++)
	{
		TiXmlElement* girlname = new TiXmlElement("Folder");
		pGirls->LinkEndChild(girlname);
		girlname->SetAttribute("Name", GirlNames[i]);
	}


	docq.SaveFile();
}
void SaveGameXML(string filename)
{
	TiXmlDocument doc(filename);
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "yes");
	doc.LinkEndChild(decl);
	TiXmlElement* pRoot = new TiXmlElement("Root");
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

	// output year, month and day
	pRoot->SetAttribute("Year", g_Year);		
	pRoot->SetAttribute("Month", g_Month);		
	pRoot->SetAttribute("Day", g_Day);			

	//this replaces the "master file"
	loadedGirlsFiles.SaveXML(pRoot);			

	// output player gold
	g_Gold.saveGoldXML(pRoot);					

	// output girls
	g_Girls.SaveGirlsXML(pRoot);	// this is all the girls that have not been acquired
	
	// output gangs
	g_Gangs.SaveGangsXML(pRoot);

	// output brothels
	g_Brothels.SaveDataXML(pRoot);

	// output clinic
	g_Clinic.SaveDataXML(pRoot);

	// output studio
	g_Studios.SaveDataXML(pRoot);

	// output arena
	g_Arena.SaveDataXML(pRoot);

	// output centre
	g_Centre.SaveDataXML(pRoot);

	// output house
	g_House.SaveDataXML(pRoot);

	// output farm
	g_Farm.SaveDataXML(pRoot);

	// output global triggers
	g_GlobalTriggers.SaveTriggersXML(pRoot);
	doc.SaveFile();

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
	g_InvManager.UpdateShop();
#endif

	SimpleSaveGameXML(filename);

}




void confirm_exit()
{
	if (g_InitWin)
	{
		g_GetString.Focused();
		g_InitWin = false;
	}

	if (g_InterfaceEvents.GetNumEvents() == 0 && !g_EnterKey) { return; }

	if (g_InterfaceEvents.CheckButton(g_interfaceid.BUTTON_CANCEL)) {
		g_ReturnText = "";
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

	if (g_InterfaceEvents.CheckButton(g_interfaceid.BUTTON_OK) || g_EnterKey) {
		g_EnterKey = false;
		g_ReturnText = "";

		g_InitWin = true;
		g_WinManager.Pop();
		g_WinManager.Pop();

		ResetInterface();

		// Schedule Quit Event -- To quit
		//SDL_Event ev;
		//ev->type = SDL_QUIT;
		//if (!SDL_PushEvent(ev))
		//	g_LogFile.write("SDL Quit Re-Scheduled!");
	}
}

void SaveGirlsCSV(string filename)
{
	ofstream GirlsCSV;
	sGirl* cgirl;
	sBrothel* building;
	string eol = "\n";
	int buildingnum = 0;
	GirlsCSV.open(filename);
	GirlsCSV << "'Building','Girl Name','Based on','Slave?','Day Job','Night Job','Age','Level','Exp','Askprice','House','Fame','Tiredness','Health','Happiness','Constitution','Charisma','Beauty','Intelligence','Confidence','Agility','Obedience','Spirit','Morality','Refinement','Dignity','Mana','Libido','Lactation','PCFear','PCLove','PCHate','Magic','Combat','Service','Medicine','Performance','Crafting','Herbalism','Farming','Brewing','Animalhandling','Normalsex','Anal','Bdsm','Beastiality','Group','Lesbian','Strip','Oralsex','Tittysex','Handjob','Footjob'" << eol;
	building = g_Brothels.GetBrothel(0);
	while (building)
	{
		cgirl = building->m_Girls;
		while (cgirl)
		{
			GirlsCSV << "'" << building->m_Name << "'," << Girl2CSV(cgirl) << eol;
			cgirl = cgirl->m_Next;
		}
		building = building->m_Next;
		if (building == 0) buildingnum++;
		if (buildingnum == 1)	{ building = g_Studios.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 2)	{ building = g_Arena.GetBrothel(0);		if (building == 0) buildingnum++; }
		if (buildingnum == 3)	{ building = g_Centre.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 4)	{ building = g_Clinic.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 5)	{ building = g_Farm.GetBrothel(0);		if (building == 0) buildingnum++; }
		if (buildingnum == 6)	{ building = g_House.GetBrothel(0);		if (building == 0) buildingnum++; }
	}
	GirlsCSV.close();
}
string Girl2CSV(sGirl* girl)
{
	cJobManager m_JobManager;
	stringstream ss;
	ss << "'" << CSVifyString(girl->m_Realname) << "'"
		<< ",'" << CSVifyString(girl->m_Name) << "'"
		<< "," << (girl->is_slave() ? "'Slave'" : "'Free'")
		<< ",'" << m_JobManager.JobName[girl->m_DayJob] << "'"
		<< ",'" << m_JobManager.JobName[girl->m_NightJob] << "'"

		<< "," << girl->age()
		<< "," << girl->level()
		<< "," << girl->exp()
		<< "," << girl->askprice()
		<< "," << girl->house()
		<< "," << girl->fame()
		<< "," << girl->tiredness()
		<< "," << girl->health()
		<< "," << girl->happiness()
		<< "," << girl->constitution()
		<< "," << girl->charisma()
		<< "," << girl->beauty()
		<< "," << girl->intelligence()
		<< "," << girl->confidence()
		<< "," << girl->agility()
		<< "," << girl->obedience()
		<< "," << girl->spirit()
		<< "," << girl->morality()
		<< "," << girl->refinement()
		<< "," << girl->dignity()
		<< "," << girl->mana()
		<< "," << girl->libido()
		<< "," << girl->lactation()
		<< "," << girl->pcfear()
		<< "," << girl->pclove()
		<< "," << girl->pchate()

		<< "," << girl->magic()
		<< "," << girl->combat()
		<< "," << girl->service()
		<< "," << girl->medicine()
		<< "," << girl->performance()
		<< "," << girl->crafting()
		<< "," << girl->herbalism()
		<< "," << girl->farming()
		<< "," << girl->brewing()
		<< "," << girl->animalhandling()

		<< "," << girl->normalsex()
		<< "," << girl->anal()
		<< "," << girl->bdsm()
		<< "," << girl->beastiality()
		<< "," << girl->group()
		<< "," << girl->lesbian()
		<< "," << girl->strip()
		<< "," << girl->oralsex()
		<< "," << girl->tittysex()
		<< "," << girl->handjob()
		<< "," << girl->footjob()
		;

	return ss.str();
}
std::string CSVifyString(string name)
{
	std::string newName(name);
	//find characters that we can't put into XML names
	//and change them to '_' or whatever works in a name
	for (size_t position = newName.find_first_of("\'\",");
		position != newName.npos;
		position = newName.find_first_of("\'\",", position))
	{
		newName.replace(position, 1, 1, '_');
	}
	return newName;
}
