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
#include "libintl.h"
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
extern void PreparingLoad();
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
	g_Customers.Free();
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

	if (g_InitWin)
	{
		g_LoadGame.Focused();
		/*
		*		clear the list box with the save games
		*/
		g_LoadGame.ClearListBox(g_interfaceid.LIST_LOADGSAVES);
		/*
		*		loop through the files, adding them to the box
		*/
		for (int i = 0; i < fl.size(); i++)
		{
			g_LoadGame.AddToListBox(g_interfaceid.LIST_LOADGSAVES, i, fl[i].leaf());
		}
		g_LoadGame.SetSelectedItemInList(g_interfaceid.LIST_LOADGSAVES, 0);
		g_InitWin = false;
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


	g_InitWin = true;
	g_WinManager.Pop();
	g_WinManager.Push(PreparingLoad, &g_Preparing);
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

	g_LogFile.os() << "items files:" << endl;
	fl.scan("*.items");
	for (int i = 0; i < fl.size(); i++)
	{

		string str = fl[i].full();
		string key = clobber_extension(str);
		if (loglevel>0) g_LogFile.os() << "	checking " << lookup[key] << endl;
		if (lookup[key] != "")	continue;


		lookup[key] = str;
		if (loglevel > 0) g_LogFile.os() << "	adding " << str << endl;
		if (loglevel > 1)	g_LogFile.os() << "	under " << key << endl;
	}
	/*
	*	Iterate over the map and print out all key/value pairs.
	*	kudos: wikipedia
	*/

	if (loglevel > 0)	g_LogFile.os() << "walking map..." << endl;
	for (map<string, string>::const_iterator it = lookup.begin(); it != lookup.end(); ++it)
	{

		string full_path = it->second;
		if (loglevel > 1)	g_LogFile.os() << "\tkey = " << it->first << endl;
		if (loglevel > 1)	g_LogFile.os() << "\tpath = " << full_path << endl;
		/*
		*		does it end in ".items" or ".itemsx"?
		*/
		size_t len = full_path.length();
		char c = full_path.at(len - 1);
		if (c == 'x')
		{

			if (loglevel > 0)	g_LogFile.os() << "\t\tLoading xml Item: " << full_path<< endl;
			g_InvManager.LoadItemsXML(full_path);
		}
		else
		{

			if (loglevel > 0)	g_LogFile.os() << "\t\tLoading orig Item: " << full_path << endl;
			g_InvManager.LoadItems(full_path);
		}
	}
}
void LoadGameInfoFiles()
{
	stringstream ss;
	ifstream incol;
	

	int loadtraits = 0;		// 0=default, 1=xml, 2=txt
	DirPath location = DirPath() << "Resources" << "Data";
	DirPath dp = DirPath() << "Resources" << "Data" << "CoreTraits.traitsx";
	TiXmlDocument docTraits(dp.c_str());
	if (docTraits.LoadFile())	loadtraits = 1;
	else // try txt
	{
		if (cfg.debug.log_debug())
		{
			g_LogFile.ss() << "Error: line " << docTraits.ErrorRow() << ", col " << docTraits.ErrorCol() << ": " << docTraits.ErrorDesc() << endl;
			g_LogFile.ssend();
		}
		dp = DirPath() << "Resources" << "Data" << "CoreTraits.traits";
		incol.open(dp.c_str());
		loadtraits = (incol.good()) ? 2 : 0;
		incol.close();
	}
	if (loadtraits == 1)
	{
		g_Traits.LoadXMLTraits(dp);
	}
	else if (loadtraits == 2)
	{
		g_Traits.LoadTraits(dp);
	}
	else if (loadtraits == 0)
	{
		FileList fl_t(location, "*.traitsx");				// get a file list
		FileList fl(location, "*.traits");				// get a file list
		if (fl_t.size() > 0)
		{
			for (int i = 0; i < fl_t.size(); i++)				// loop over the list, loading the files
			{
				g_Traits.LoadXMLTraits(fl_t[i].full());
			}
		}
		else if (fl.size() > 0)
		{
			for (int i = 0; i < fl.size(); i++)				// loop over the list, loading the files
			{
				g_Traits.LoadTraits(fl[i].full());
			}
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
	g_NameList.load(location_N);
	DirPath location_SN = DirPath() << "Resources" << "Data" << "RandomLastNames.txt";
	g_SurnameList.load(location_SN);


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
	if (g_Clinic.GetNumBrothels() > 0)		g_Clinic.UpdateClinic();
	if (g_Studios.GetNumBrothels() > 0)		g_Studios.UpdateMovieStudio();
	if (g_Arena.GetNumBrothels() > 0)		g_Arena.UpdateArena();
	if (g_Centre.GetNumBrothels() > 0)		g_Centre.UpdateCentre();
	if (g_Farm.GetNumBrothels() > 0)		g_Farm.UpdateFarm();
	g_House.UpdateHouse();

	g_Brothels.UpdateBrothels(); // Moved so new buildings show up in profit reports --PP

	// go ahead and handle pregnancies for girls not controlled by player
	g_Girls.UncontrolledPregnancies();

	// go through and update all the gang-related data (send them on missions, etc.)
	g_Gangs.UpdateGangs();

	g_Brothels.m_Dungeon.Update();	// update the people in the dungeon

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
		g_MessageQue.AddToQue(gettext("A girl has died in the dungeon.\nHer body will be removed by the end of the week."), 1);

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
		g_MessageQue.AddToQue(gettext("A customer has died in the dungeon.\nTheir body will be removed by the end of the week."), 1);

		if (g_Dice.percent(10))	// only 10% chance of being found out
		{
			The_Player->suspicion(1);
		}
		The_Player->disposition(-1);
		The_Player->customerfear(1);

		ClearGameFlag(FLAG_DUNGEONCUSTDIE);
	}
}

void PreparingNew()
{
	g_CurrentScreen = SCREEN_PREPARING;
	if (g_InitWin)
	{
		g_InitWin = false;
		g_Preparing.Focused();
	}
	else
	{
		g_WinManager.Pop();
		NewGame();
	}
}
void PreparingLoad()
{
	g_CurrentScreen = SCREEN_PREPARING;
	if (g_InitWin)
	{
		g_InitWin = false;
		g_Preparing.Focused();
	}
	else
	{
		DirPath location = DirPath(cfg.folders.saves().c_str());
		const char *pattern = "*.gam";
		FileList fl(location, pattern);
		if (LoadGame(location, g_ReturnText))
		{
			g_WinManager.Pop();
			g_WinManager.push("Brothel Management");
			g_InitWin = true;
		}
		else
		{
			g_InitWin = true;
			g_WinManager.Pop();
		}
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

bool LoadGame(string directory, string filename)	// `J` Bookmark - Loading a game
{
	// FREE ANYTHING
	//other stuff will be freed as it is loaded
	g_Traits.Free();
	g_Girls.Free();
	g_InvManager.Free();

	// clear all buildings
	g_Brothels.Free();
	g_Clinic.Free();
	g_Studios.Free();
	g_Arena.Free();
	g_Centre.Free();
	g_House.Free();
	g_Farm.Free();


	//load items database, traits info, etc
	LoadGameInfoFiles();

	DirPath thefile = directory.c_str();
	thefile << filename;
	TiXmlDocument doc(thefile.c_str());
	if (doc.LoadFile() == false)
	{
		g_LogFile.ss() << "Tried and failed to parse savegame as XML file, error as follows" << endl;
		g_LogFile.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << endl;
		g_LogFile.ss() << doc.ErrorDesc() << endl;
		g_LogFile.ss() << "(If it says 'Error document empty.' it's probably old format" << ".06 no longer supports Legacy Savegames. Start a new game.)" << endl;
		g_LogFile.ssend();
		return false;
	}
	else
	{
		return LoadGameXML(TiXmlHandle(&doc));
	}
}
bool LoadGameXML(TiXmlHandle hDoc)
{
	TiXmlHandle hRoot = hDoc.FirstChildElement("Root");
	TiXmlElement* pRoot = hRoot.ToElement();
	if (pRoot == 0)
	{
		return false;
	}

	// load the version
	int minorA = -1;
	pRoot->QueryIntAttribute("MinorVersionA", &minorA);

	if (minorA != 6)
	{
		g_MessageQue.AddToQue(gettext("You must start a new game with this version"), 2);
		return false;
	}

	string version("<blank>");
	if (pRoot->Attribute("ExeVersion"))
	{
		version = pRoot->Attribute("ExeVersion");
	}
	if (version != "official")
	{
		g_MessageQue.AddToQue("Warning, the exe was not detected as official, it was detected as "
			+ version + ".  Attempting to load anyways.", 1);
	}
	/*
	*	We need to load the .girls/.girlsx files
	*
	*	To do that, we need to know which we might have already loaded
	*	that information is kept in the master file
	*	so we load that first
	*/
	loadedGirlsFiles.LoadXML(hRoot.FirstChild("Loaded_Files"));
	LoadGirlsFiles();

	g_CurrBrothel = 0;

	// load interface variables
	g_WalkAround = false;
	pRoot->QueryValueAttribute<bool>("WalkAround", &g_WalkAround);
	g_TryCentre = false;
	pRoot->QueryValueAttribute<bool>("TryCentre", &g_TryCentre);	// `J` added
	g_TryOuts = false;
	pRoot->QueryValueAttribute<bool>("TryOuts", &g_TryOuts);
	g_TryEr = false;
	pRoot->QueryValueAttribute<bool>("TryEr", &g_TryEr);
	g_TryCast = false;
	pRoot->QueryValueAttribute<bool>("TryCast", &g_TryCast);
	g_TalkCount = 0;
	pRoot->QueryIntAttribute("TalkCount", &g_TalkCount);


	// load player gold
	g_Gold.loadGoldXML(hRoot.FirstChildElement("Gold"));

	// load year, month and day
	//if these aren't loaded, they default to whatever numbers they were before
	pRoot->QueryValueAttribute<unsigned long>("Year", &g_Year);
	pRoot->QueryValueAttribute<unsigned long>("Month", &g_Month);
	pRoot->QueryValueAttribute<unsigned long>("Day", &g_Day);

	// load main girls
	g_LogFile.write("Loading girls");
	g_Girls.LoadGirlsXML(hRoot.FirstChildElement("Girls"));

	// load gangs
	g_LogFile.write("Loading gang Data");
	g_Gangs.LoadGangsXML(hRoot.FirstChildElement("Gang_Manager"));

	// load brothels
	g_LogFile.write("Loading brothel Data");
	g_Brothels.LoadDataXML(hRoot.FirstChildElement("Brothel_Manager"));

	// load clinic
	g_LogFile.write("Loading clinic Data");
	g_Clinic.LoadDataXML(hRoot.FirstChildElement("Clinic_Manager"));

	// load studio
	g_LogFile.write("Loading studio Data");
	g_Studios.LoadDataXML(hRoot.FirstChildElement("MovieStudio_Manager"));

	// load arena
	g_LogFile.write("Loading arena Data");
	g_Arena.LoadDataXML(hRoot.FirstChildElement("Arena_Manager"));

	// load centre
	g_LogFile.write("Loading centre Data");
	g_Centre.LoadDataXML(hRoot.FirstChildElement("Centre_Manager"));

	// load house
	g_LogFile.write("Loading house Data");
	g_House.LoadDataXML(hRoot.FirstChildElement("House_Manager"));

	// load farm
	g_LogFile.write("Loading farm Data");
	g_Farm.LoadDataXML(hRoot.FirstChildElement("Farm_Manager"));

	// load global triggers
	g_LogFile.write("Loading global triggers");
	g_GlobalTriggers.LoadTriggersXML(hRoot.FirstChildElement("Triggers"));

	g_LogFile.write("Loading default images");

	if (g_Cheats)
	{
		g_WalkAround = false;
		g_TryCentre = false;
		g_TryOuts = false;
		g_TryEr = false;
		g_TryCast = false;
		g_TalkCount = 10;
	}

	selected_girl = 0;
	for (int i = 0; i < 20; i++)
	{
		MarketSlaveGirls[i] = 0;
		MarketSlaveGirlsDel[i] = -1;
	}

	// update the shop inventory
	g_InvManager.UpdateShop();
	return true;
}


void TransferGirls()
{
	static int leftBrothel = 0;
	static int rightBrothel = 0;
	g_CurrentScreen = SCREEN_TRANSFERGIRLS;
	if (g_InitWin)
	{
		leftBrothel = rightBrothel = 0;
		// clear list boxes
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS);

		// list all the brothels
		sBrothel* current = g_Brothels.GetBrothel(0);
		int i = 6;
		while (current)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, i, current->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, i, current->m_Name);
			i++;
			current = current->m_Next;
		}

		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 6);
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 6);

		// add the movie studio studio
		sMovieStudio* currentStudio = (sMovieStudio*)g_Studios.GetBrothel(0);
		while (currentStudio)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 0, currentStudio->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 0, currentStudio->m_Name);
			currentStudio = (sMovieStudio*)currentStudio->m_Next;
		}
		// add the arena
		sArena* currentArena = (sArena*)g_Arena.GetBrothel(0);
		while (currentArena)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 1, currentArena->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 1, currentArena->m_Name);
			currentArena = (sArena*)currentArena->m_Next;
		}
		// add the centre
		sCentre* currentCentre = (sCentre*)g_Centre.GetBrothel(0);
		while (currentCentre)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 2, currentCentre->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 2, currentCentre->m_Name);
			currentCentre = (sCentre*)currentCentre->m_Next;
		}
		// add the clinic
		sClinic* currentClinic = (sClinic*)g_Clinic.GetBrothel(0);
		while (currentClinic)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 3, currentClinic->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 3, currentClinic->m_Name);
			currentClinic = (sClinic*)currentClinic->m_Next;
		}
		// add the farm
		sFarm* currentFarm = (sFarm*)g_Farm.GetBrothel(0);
		while (currentFarm)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 4, currentFarm->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 4, currentFarm->m_Name);
			currentFarm = (sFarm*)currentFarm->m_Next;
		}
		// add the house
		sHouse* currentHouse = (sHouse*)g_House.GetBrothel(0);
		while (currentHouse)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 5, currentHouse->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 5, currentHouse->m_Name);
			currentHouse = (sHouse*)currentHouse->m_Next;
		}

		g_InitWin = false;
	}

	if (g_InterfaceEvents.GetNumEvents() != 0)
	{
		if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGBACK))
		{
			g_InitWin = true;
			g_WinManager.Pop();
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGSHIFTR))
		{
			if ((rightBrothel != -1 && leftBrothel != -1))
			{
				TransferGirlsRightToLeft(rightBrothel, leftBrothel);
			}
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGSHIFTL))
		{
			if ((rightBrothel != -1 && leftBrothel != -1))
			{
				TransferGirlsLeftToRight(rightBrothel, leftBrothel);
			}
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TRANSGLEFTBROTHEL))
		{
			int color = COLOR_BLUE;
			g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS);
			leftBrothel = g_TransferGirls.GetSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTBROTHEL);
			if (leftBrothel != -1)
			{
				if (leftBrothel > 5){
					// add the girls to the list
					sGirl* temp = g_Brothels.GetGirl(leftBrothel - 6, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_MATRON || temp->m_NightJob == JOB_MATRON)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_RESTING && temp->m_NightJob == JOB_RESTING)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 4)
				{
					// add the girls to the list
					sGirl* temp = g_Farm.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_FARMMANGER || temp->m_NightJob == JOB_FARMMANGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_FARMREST && temp->m_NightJob == JOB_FARMREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 5)
				{
					// add the girls to the list
					sGirl* temp = g_House.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_HEADGIRL || temp->m_NightJob == JOB_HEADGIRL)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_HOUSEREST && temp->m_NightJob == JOB_HOUSEREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 2)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_COUNSELOR || temp->m_NightJob == JOB_COUNSELOR)
						{
							if (g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, SHIFT_NIGHT) < 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_CENTREREST && temp->m_NightJob == JOB_CENTREREST)
							color = COLOR_GREEN;
						else if (temp->m_DayJob == JOB_REHAB || temp->m_NightJob == JOB_REHAB)
						{
							if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 1)
				{
					// add the girls to the list
					sGirl* temp = g_Arena.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_DOCTORE || temp->m_NightJob == JOB_DOCTORE)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_ARENAREST && temp->m_NightJob == JOB_ARENAREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Clinic.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CHAIRMAN || temp->m_NightJob == JOB_CHAIRMAN)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_CLINICREST && temp->m_NightJob == JOB_CLINICREST)
							color = COLOR_GREEN;
						else if (temp->m_DayJob == JOB_DOCTOR || temp->m_NightJob == JOB_DOCTOR)
						{
							if (g_Clinic.GetNumberPatients(0) < 1 && g_Clinic.GetNumberPatients(1) < 1)
								color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_MECHANIC || temp->m_NightJob == JOB_MECHANIC)
						{
							if (g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 0) < 1 && g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 1) < 1)
								color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_GETHEALING || temp->m_NightJob == JOB_GETHEALING
							|| temp->m_DayJob == JOB_GETREPAIRS || temp->m_NightJob == JOB_GETREPAIRS)
						{
							if (temp->health() > 70 && temp->tiredness() < 30) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_GETABORT || temp->m_NightJob == JOB_GETABORT
							|| temp->m_DayJob == JOB_PHYSICALSURGERY || temp->m_NightJob == JOB_PHYSICALSURGERY
							|| temp->m_DayJob == JOB_LIPO || temp->m_NightJob == JOB_LIPO
							|| temp->m_DayJob == JOB_BREASTREDUCTION || temp->m_NightJob == JOB_BREASTREDUCTION
							|| temp->m_DayJob == JOB_BOOBJOB || temp->m_NightJob == JOB_BOOBJOB
							|| temp->m_DayJob == JOB_VAGINAREJUV || temp->m_NightJob == JOB_VAGINAREJUV
							|| temp->m_DayJob == JOB_FACELIFT || temp->m_NightJob == JOB_FACELIFT
							|| temp->m_DayJob == JOB_ASSJOB || temp->m_NightJob == JOB_ASSJOB
							|| temp->m_DayJob == JOB_TUBESTIED || temp->m_NightJob == JOB_TUBESTIED
							|| temp->m_DayJob == JOB_FERTILITY || temp->m_NightJob == JOB_FERTILITY)
						{
							if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 0)
				{
					// add the girls to the list
					sGirl* temp = g_Studios.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_NightJob == JOB_DIRECTOR)					color = COLOR_RED;
						else if (temp->m_NightJob == JOB_FILMFREETIME)			color = COLOR_GREEN;
						else if (temp->m_NightJob == JOB_CAMERAMAGE)
						{
							if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_NightJob == JOB_CRYSTALPURIFIER)
						{
							if (g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
			}
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TRANSGRIGHTBROTHEL))
		{
			int color = COLOR_BLUE;
			g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS);
			rightBrothel = g_TransferGirls.GetSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL);
			if (rightBrothel != -1)
			{
				if (rightBrothel > 5){
					// add the girls to the list
					sGirl* temp = g_Brothels.GetGirl(rightBrothel - 6, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_MATRON || temp->m_NightJob == JOB_MATRON)			color = COLOR_RED;
						else if (temp->m_DayJob == JOB_RESTING && temp->m_NightJob == JOB_RESTING)	color = COLOR_GREEN;
						else color = COLOR_BLUE;
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 4)
				{
					// add the girls to the list
					sGirl* temp = g_Farm.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_FARMMANGER || temp->m_NightJob == JOB_FARMMANGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_FARMREST && temp->m_NightJob == JOB_FARMREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 5)
				{
					// add the girls to the list
					sGirl* temp = g_House.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_HEADGIRL || temp->m_NightJob == JOB_HEADGIRL)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_HOUSEREST && temp->m_NightJob == JOB_HOUSEREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 2)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_COUNSELOR || temp->m_NightJob == JOB_COUNSELOR)
						{
							if (g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, SHIFT_NIGHT) < 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_CENTREREST && temp->m_NightJob == JOB_CENTREREST)
							color = COLOR_GREEN;
						else if (temp->m_DayJob == JOB_REHAB || temp->m_NightJob == JOB_REHAB)
						{
							if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 1)
				{
					// add the girls to the list
					sGirl* temp = g_Arena.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_DOCTORE || temp->m_NightJob == JOB_DOCTORE)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_ARENAREST && temp->m_NightJob == JOB_ARENAREST)
							color = COLOR_GREEN;
						else color = COLOR_BLUE;
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Clinic.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CHAIRMAN || temp->m_NightJob == JOB_CHAIRMAN)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_CLINICREST && temp->m_NightJob == JOB_CLINICREST)
							color = COLOR_GREEN;
						else if (temp->m_DayJob == JOB_DOCTOR || temp->m_NightJob == JOB_DOCTOR)
						{
							if (g_Clinic.GetNumberPatients(0) < 1 && g_Clinic.GetNumberPatients(1) < 1)
								color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_MECHANIC || temp->m_NightJob == JOB_MECHANIC)
						{
							if (g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 0) < 1 && g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 1) < 1)
								color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_GETHEALING || temp->m_NightJob == JOB_GETHEALING
							|| temp->m_DayJob == JOB_GETREPAIRS || temp->m_NightJob == JOB_GETREPAIRS)
						{
							if (temp->health() > 70 && temp->tiredness() < 30) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_DayJob == JOB_GETABORT || temp->m_NightJob == JOB_GETABORT
							|| temp->m_DayJob == JOB_PHYSICALSURGERY || temp->m_NightJob == JOB_PHYSICALSURGERY
							|| temp->m_DayJob == JOB_LIPO || temp->m_NightJob == JOB_LIPO
							|| temp->m_DayJob == JOB_BREASTREDUCTION || temp->m_NightJob == JOB_BREASTREDUCTION
							|| temp->m_DayJob == JOB_BOOBJOB || temp->m_NightJob == JOB_BOOBJOB
							|| temp->m_DayJob == JOB_VAGINAREJUV || temp->m_NightJob == JOB_VAGINAREJUV
							|| temp->m_DayJob == JOB_FACELIFT || temp->m_NightJob == JOB_FACELIFT
							|| temp->m_DayJob == JOB_ASSJOB || temp->m_NightJob == JOB_ASSJOB
							|| temp->m_DayJob == JOB_TUBESTIED || temp->m_NightJob == JOB_TUBESTIED
							|| temp->m_DayJob == JOB_FERTILITY || temp->m_NightJob == JOB_FERTILITY)
						{
							if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 0)
				{
					// add the girls to the list
					sGirl* temp = g_Studios.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{

						if (temp->m_NightJob == JOB_DIRECTOR)					color = COLOR_RED;
						else if (temp->m_NightJob == JOB_FILMFREETIME)			color = COLOR_GREEN;
						else if (temp->m_NightJob == JOB_CAMERAMAGE)
						{
							if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else if (temp->m_NightJob == JOB_CRYSTALPURIFIER)
						{
							if (g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
							else color = COLOR_RED;
						}
						else color = COLOR_BLUE;

						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname, color);
						i++;
						temp = temp->m_Next;
					}
				}
			}
		}
	}
}
void TransferGirlsLeftToRight(int rightBrothel, int leftBrothel)
{
    sBrothel* brothel = nullptr;
	/* */if (leftBrothel > 5)	{ brothel = g_Brothels.GetBrothel(leftBrothel - 6); }
	else if (leftBrothel == 5)	{ brothel = g_House.GetBrothel(0); }
	else if (leftBrothel == 4)	{ brothel = g_Farm.GetBrothel(0); }
	else if (leftBrothel == 3)	{ brothel = g_Clinic.GetBrothel(0); }
	else if (leftBrothel == 2)	{ brothel = g_Centre.GetBrothel(0); }
	else if (leftBrothel == 1)	{ brothel = g_Arena.GetBrothel(0); }
	else if (leftBrothel == 0)	{ brothel = g_Studios.GetBrothel(0); }

	if (brothel->m_NumGirls == brothel->m_NumRooms)
	{
		g_MessageQue.AddToQue(gettext("Left side building is full"), 1);
	}
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTGIRLS, 0, pos);
		while (girlSelection != -1)
		{
			sGirl* temp = nullptr;
			// get the girl
			/* */if (rightBrothel > 5)	{ temp = g_Brothels.GetGirl(rightBrothel - 6, girlSelection - NumRemoved); }
			else if (rightBrothel == 5)	{ temp = g_House.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 4)	{ temp = g_Farm.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 3)	{ temp = g_Clinic.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 2)	{ temp = g_Centre.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 1)	{ temp = g_Arena.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 0)	{ temp = g_Studios.GetGirl(0, girlSelection - NumRemoved); }

			// check there is still room
			if (brothel->m_NumGirls + 1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue(gettext("Left side building is full"), 1);
				break;
			}

			// remove girl from right side
			NumRemoved++;
			/* */if (rightBrothel > 5)	{ g_Brothels.RemoveGirl(rightBrothel - 6, temp, false); }
			else if (rightBrothel == 5)	{ g_House.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 4)	{ g_Farm.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 3)	{ g_Clinic.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 2)	{ g_Centre.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 1)	{ g_Arena.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 0)	{ g_Studios.RemoveGirl(0, temp, false); }

			// add to left side
			/* */if (leftBrothel > 5)	{ g_Brothels.AddGirl(leftBrothel - 6, temp); }
			else if (leftBrothel == 5)	{ g_House.AddGirl(0, temp); }
			else if (leftBrothel == 4)	{ g_Farm.AddGirl(0, temp); }
			else if (leftBrothel == 3)	{ g_Clinic.AddGirl(0, temp); }
			else if (leftBrothel == 2)	{ g_Centre.AddGirl(0, temp); }
			else if (leftBrothel == 1)	{ g_Arena.AddGirl(0, temp); }
			else if (leftBrothel == 0)	{ g_Studios.AddGirl(0, temp); }

			// get next girl
			girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTGIRLS, pos + 1, pos);
		}

		// update the girl lists
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGLEFTBROTHEL, leftBrothel);
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, rightBrothel);
	}
}
void TransferGirlsRightToLeft(int rightBrothel, int leftBrothel)
{
    sBrothel* brothel = nullptr;
	/* */if (rightBrothel > 5)	{ brothel = g_Brothels.GetBrothel(rightBrothel - 6); }
	else if (rightBrothel == 5)	{ brothel = g_House.GetBrothel(0); }
	else if (rightBrothel == 4)	{ brothel = g_Farm.GetBrothel(0); }
	else if (rightBrothel == 3)	{ brothel = g_Clinic.GetBrothel(0); }
	else if (rightBrothel == 2)	{ brothel = g_Centre.GetBrothel(0); }
	else if (rightBrothel == 1)	{ brothel = g_Arena.GetBrothel(0); }
	else if (rightBrothel == 0)	{ brothel = g_Studios.GetBrothel(0); }

	if (brothel->m_NumGirls == brothel->m_NumRooms)
	{
		g_MessageQue.AddToQue(gettext("Right side building is full"), 1);
	}
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTGIRLS, 0, pos);
		while (girlSelection != -1)
		{
            sGirl* temp = nullptr;
			/* */if (leftBrothel > 5)	{ temp = g_Brothels.GetGirl(leftBrothel - 6, girlSelection - NumRemoved); }
			else if (leftBrothel == 5)	{ temp = g_House.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 4)	{ temp = g_Farm.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 3)	{ temp = g_Clinic.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 2)	{ temp = g_Centre.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 1)	{ temp = g_Arena.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 0)	{ temp = g_Studios.GetGirl(0, girlSelection - NumRemoved); }

			// check there is still room
			if (brothel->m_NumGirls + 1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue(gettext("Right side building is full"), 1);
				break;
			}

			// remove girl from left side
			NumRemoved++;
			/* */if (leftBrothel > 5)	{ g_Brothels.RemoveGirl(leftBrothel - 6, temp, false); }
			else if (leftBrothel == 5)	{ g_House.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 4)	{ g_Farm.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 3)	{ g_Clinic.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 2)	{ g_Centre.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 1)	{ g_Arena.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 0)	{ g_Studios.RemoveGirl(0, temp, false); }

			// add to right side
			/* */if (rightBrothel > 5)	{ g_Brothels.AddGirl(rightBrothel - 6, temp); }
			else if (rightBrothel == 5)	{ g_House.AddGirl(0, temp); }
			else if (rightBrothel == 4)	{ g_Farm.AddGirl(0, temp); }
			else if (rightBrothel == 3)	{ g_Clinic.AddGirl(0, temp); }
			else if (rightBrothel == 2)	{ g_Centre.AddGirl(0, temp); }
			else if (rightBrothel == 1)	{ g_Arena.AddGirl(0, temp); }
			else if (rightBrothel == 0)	{ g_Studios.AddGirl(0, temp); }

			// get next girl
			girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTGIRLS, pos + 1, pos);
		}

		// update the girl lists
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGLEFTBROTHEL, leftBrothel);
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, rightBrothel);
	}
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

		<< "," << g_Girls.GetStat(girl, STAT_AGE)
		<< "," << g_Girls.GetStat(girl, STAT_LEVEL)
		<< "," << g_Girls.GetStat(girl, STAT_EXP)
		<< "," << g_Girls.GetStat(girl, STAT_ASKPRICE)
		<< "," << g_Girls.GetStat(girl, STAT_HOUSE)
		<< "," << g_Girls.GetStat(girl, STAT_FAME)
		<< "," << g_Girls.GetStat(girl, STAT_TIREDNESS)
		<< "," << g_Girls.GetStat(girl, STAT_HEALTH)
		<< "," << g_Girls.GetStat(girl, STAT_HAPPINESS)
		<< "," << g_Girls.GetStat(girl, STAT_CONSTITUTION)
		<< "," << g_Girls.GetStat(girl, STAT_CHARISMA)
		<< "," << g_Girls.GetStat(girl, STAT_BEAUTY)
		<< "," << g_Girls.GetStat(girl, STAT_INTELLIGENCE)
		<< "," << g_Girls.GetStat(girl, STAT_CONFIDENCE)
		<< "," << g_Girls.GetStat(girl, STAT_AGILITY)
		<< "," << g_Girls.GetStat(girl, STAT_OBEDIENCE)
		<< "," << g_Girls.GetStat(girl, STAT_SPIRIT)
		<< "," << g_Girls.GetStat(girl, STAT_MORALITY)
		<< "," << g_Girls.GetStat(girl, STAT_REFINEMENT)
		<< "," << g_Girls.GetStat(girl, STAT_DIGNITY)
		<< "," << g_Girls.GetStat(girl, STAT_MANA)
		<< "," << g_Girls.GetStat(girl, STAT_LIBIDO)
		<< "," << g_Girls.GetStat(girl, STAT_LACTATION)
		<< "," << g_Girls.GetStat(girl, STAT_PCFEAR)
		<< "," << g_Girls.GetStat(girl, STAT_PCLOVE)
		<< "," << g_Girls.GetStat(girl, STAT_PCHATE)

		<< "," << g_Girls.GetSkill(girl, SKILL_MAGIC)
		<< "," << g_Girls.GetSkill(girl, SKILL_COMBAT)
		<< "," << g_Girls.GetSkill(girl, SKILL_SERVICE)
		<< "," << g_Girls.GetSkill(girl, SKILL_MEDICINE)
		<< "," << g_Girls.GetSkill(girl, SKILL_PERFORMANCE)
		<< "," << g_Girls.GetSkill(girl, SKILL_CRAFTING)
		<< "," << g_Girls.GetSkill(girl, SKILL_HERBALISM)
		<< "," << g_Girls.GetSkill(girl, SKILL_FARMING)
		<< "," << g_Girls.GetSkill(girl, SKILL_BREWING)
		<< "," << g_Girls.GetSkill(girl, SKILL_ANIMALHANDLING)

		<< "," << g_Girls.GetSkill(girl, SKILL_NORMALSEX)
		<< "," << g_Girls.GetSkill(girl, SKILL_ANAL)
		<< "," << g_Girls.GetSkill(girl, SKILL_BDSM)
		<< "," << g_Girls.GetSkill(girl, SKILL_BEASTIALITY)
		<< "," << g_Girls.GetSkill(girl, SKILL_GROUP)
		<< "," << g_Girls.GetSkill(girl, SKILL_LESBIAN)
		<< "," << g_Girls.GetSkill(girl, SKILL_STRIP)
		<< "," << g_Girls.GetSkill(girl, SKILL_ORALSEX)
		<< "," << g_Girls.GetSkill(girl, SKILL_TITTYSEX)
		<< "," << g_Girls.GetSkill(girl, SKILL_HANDJOB)
		<< "," << g_Girls.GetSkill(girl, SKILL_FOOTJOB)
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
