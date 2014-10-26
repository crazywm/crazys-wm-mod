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

// globals used for the interface
string g_ReturnText = "";
bool g_InitWin = true;
bool g_AllTogle = false;	// used on screens when wishing to apply something to all items
long g_IntReturn;
extern bool eventrunning;
extern cRng g_Dice;

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

extern int g_BrothelScreenImgX, g_BrothelScreenImgY, g_BrothelScreenImgW, g_BrothelScreenImgH;

sGirl* selected_girl;  // global pointer for the currently selected girl
vector<int> cycle_girls;  // globally available sorted list of girl IDs for Girl Details screen to cycle through
int cycle_pos;  //currently selected girl's position in the cycle_girls vector

sGirl* MarketSlaveGirls[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int MarketSlaveGirlsDel[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

CSurface* g_BrothelImages[7] = {0,0,0,0,0,0,0};
char buffer[1000];

extern bool g_LeftArrow;
extern bool g_RightArrow;
extern bool g_UpArrow;
extern bool g_DownArrow;
extern bool g_EnterKey;
extern bool g_AltKeys;
extern bool g_SpaceKey;
extern bool g_Q_Key;
extern bool g_W_Key;
extern bool g_E_Key;
extern bool g_A_Key;
extern bool g_S_Key;
extern bool g_D_Key;
extern bool g_Z_Key;
extern bool g_X_Key;
extern bool g_C_Key;

extern bool g_R_Key;
extern bool g_F_Key;
extern bool g_T_Key;
extern bool g_G_Key;
extern bool g_Y_Key;
extern bool g_H_Key;
extern bool g_U_Key;
extern bool g_J_Key;
extern bool g_I_Key;
extern bool g_K_Key;

extern int g_CurrentScreen;

//used to store what files we have loaded
MasterFile loadedGirlsFiles;

void confirm_exit();

void LoadGameScreen()
{
	DirPath location = DirPath() << "Saves";
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
		for (int i = 0; i < fl.size(); i++) {
			g_LoadGame.AddToListBox(g_interfaceid.LIST_LOADGSAVES, i, fl[i].leaf());
		}
		g_InitWin = false;
	}

	/*
	 *	no events process means we can go home early
	 */
	if (g_InterfaceEvents.GetNumEvents() == 0)
	{

		return;
	}

	/*
	 *	the next simplest case is the "back" button
	 */
	if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGBACK))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	/*
	 *	by this point, we're only interested if it's a click on the load game button or a double-click on a game in the list
	 */
	if (!g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGLOAD)
		&& !g_LoadGame.ListDoubleClicked(g_interfaceid.LIST_LOADGSAVES))
	{
		return;
	}
	/*
	 *	OK: So from this point onwards, we're loading the game
	 */
	int selection = g_LoadGame.GetLastSelectedItemFromList(g_interfaceid.LIST_LOADGSAVES);
	/*
	 *	nothing selected means nothing more to do
	 */
	if (selection == -1)
	{
		return;
	}
	string temp = fl[selection].leaf();
	/*
	 *	enable cheat mode for a cheat brothel
	 */
	g_Cheats = (temp == "Cheat.gam");

	if (LoadGame(location, fl[selection].leaf()))
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

void NewGame()
{
	cConfig cfg;
	cScriptManager sm;

	g_Cheats = false;
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

	g_Girls.LoadDefaultImages();


	g_GlobalTriggers.LoadList(DirPath() << "Resources" << "Scripts" << "GlobalTriggers.xml");

	g_CurrBrothel = 0;

	g_Gold.reset();

	g_Year = 1209; g_Month = 1; g_Day = 1;

	selected_girl = 0;
	for (int i = 0; i<8; i++)
	{
		MarketSlaveGirls[i] = 0;
		MarketSlaveGirlsDel[i] = -1;
	}

	g_Brothels.NewBrothel(20,250);
	g_Brothels.SetName(0, g_ReturnText);
	for (int i = 0; i<NUM_STATS; i++)		g_Brothels.GetPlayer()->m_Stats[i] = 60;
	for (u_int i = 0; i<NUM_SKILLS; i++)	g_Brothels.GetPlayer()->m_Skills[i] = 10;
	g_Brothels.GetPlayer()->SetToZero();

	g_House.NewBrothel(20,200);
	g_House.SetName(0, "House");

	u_int start_random_gangs = cfg.gangs.start_random();
	u_int start_boosted_gangs = cfg.gangs.start_boosted();
	for (u_int i = 0; i<start_random_gangs; i++)	g_Gangs.AddNewGang(false);
	for (u_int i = 0; i<start_boosted_gangs; i++)	g_Gangs.AddNewGang(true);

	// update the shop inventory
	g_InvManager.UpdateShop();
		
	// Add the begining rivals
	g_Brothels.GetRivalManager()->CreateRival(200, 5, 10000, 2, 0, 26, 2, 2);
	g_Brothels.GetRivalManager()->CreateRival(400, 10, 15000, 2, 1, 30, 2, 3);
	g_Brothels.GetRivalManager()->CreateRival(600, 15, 20000, 3, 1, 56, 3, 5);
	g_Brothels.GetRivalManager()->CreateRival(800, 20, 25000, 4, 2, 74, 4, 8);

	if (g_Cheats) { g_Gold.cheat(); g_InvManager.GivePlayerAllItems(); }
	
	g_WinManager.push("Brothel Management");

	DirPath text = DirPath() << "Saves" << (g_Brothels.GetBrothel(0)->m_Name + ".gam").c_str();
	sm.Load(ScriptPath("Intro.lua"), 0);
	SaveGameXML(text);
}

void GetString()
{
	cGetStringScreenManager gssm;
	gssm.process();
}

static string clobber_extension(string s)	// `J` debug logging
{
	cConfig cfg;
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: s = " << s << endl;
	size_t pos = s.rfind(".");
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: pos = " << pos << endl;
	string base = s.substr(0, pos);
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: s = " << s << endl;
	if (cfg.debug.log_debug())	g_LogFile.os() << "clobber_extension: base = " << base << endl;
	return base;
}

/*
 * interim loader to load XML files, and then non-xml ones
 * if there was no xml version.
 */
static void LoadXMLItems(FileList &fl)
{
	map<string, string> lookup;
	int loglevel = 0;
	cConfig cfg;
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

			if (loglevel > 0)	g_LogFile.os() << "\t\tloading xml" << endl;
			g_InvManager.LoadItemsXML(full_path);
		}
		else
		{

			if (loglevel > 0)	g_LogFile.os() << "\t\tloading orig" << endl;
			g_InvManager.LoadItems(full_path);
		}
	}
}

void LoadGameInfoFiles()
{
	stringstream ss;
	ifstream incol;
	cConfig cfg;

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
	DirPath location_i = DirPath() << "Resources" << "Items"; // `J` moved items from Data to Items folder
	FileList fl_i(location_i, "*.itemsx");
	if (cfg.debug.log_items())	g_LogFile.os() << "Found " << fl_i.size() <<" itemsx files" << endl;
	LoadXMLItems(fl_i);
}

void LoadGirlsFiles()
{
/*
 *	now get a list of all the file in the Characters folder
 *	start by building a path...
 */
	DirPath location = DirPath() << "Resources" << "Characters";
/*
 *	now scan for matching files. The XMLFileList
 *	will look for ".girls" and ".girlx" files
 *	with the XML versions shadowing the originals
 */
	XMLFileList girlfiles(location, "*.girls");
	XMLFileList rgirlfiles(location,"*.rgirls");
/*
 *	And we need to know which ".girls" files the saved game processed
 *	This information is stored in the master file - so we read that.
 */
	for(int i = 0; i < girlfiles.size(); i++)
	{
/*
 *		OK: if the current file is listed in the master file
 *		we don't need to load it. Unless the AllData flag is set
 *		and then we do. I think.
 */
		if(loadedGirlsFiles.exists(girlfiles[i].leaf()))
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
	for(int i = 0; i < rgirlfiles.size(); i++)
	{
		g_Girls.LoadRandomGirl(rgirlfiles[i].full());
	}
}

// MYR: Reordered the listing of messages: Critical (red) first, then important (dark blue)
//      then everything else.
// WD:	Copy sort code to Dungeon Girls

void Turnsummary()
{
	static int ImageType = -1, lastNum = -1, ImageNum = -1, LastType = -1, category = 0, category_last = 0, Item = 0;
	sGirl *girl;
	g_CurrentScreen = SCREEN_TURNSUMMARY;

	if (g_InitWin)
	{
		g_Turnsummary.Focused();
		string brothel = gettext("Current Brothel: ");
		brothel += g_Brothels.GetName(g_CurrBrothel);
		g_Turnsummary.EditTextItem(brothel, g_interfaceid.TEXT_CURRENTBROTHEL);

		if (category_last == category) Item = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
		else { Item = 0; category_last = category; }

		// Clear the lists
		g_Turnsummary.ClearListBox(g_interfaceid.LIST_TSCATEGORY);
		g_Turnsummary.ClearListBox(g_interfaceid.LIST_TSITEM);
		g_Turnsummary.ClearListBox(g_interfaceid.LIST_TSEVENTS);

/*
 *		CATEGORY Listbox
 */
		// list the categories of events
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 0, gettext("GIRLS"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 1, gettext("GANGS"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 2, gettext("BROTHELS"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 3, gettext("DUNGEON"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 4, gettext("CLINIC"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 5, gettext("STUDIO"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 6, gettext("ARENA"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 7, gettext("CENTRE"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 8, gettext("HOUSE"));
		g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 9, gettext("FARM"));
		//g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 4, "RIVALS");
		//g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 5, "GLOBAL");
		g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSCATEGORY, category, false);

/*
 *		ITEM Listbox
 */
		// 0. List girls
		if(category == 0)
		{
			int ID = 0, nNumGirls = g_Brothels.GetNumGirls(g_CurrBrothel);
			vector<sGirl*> tmpSexGirls, tmpServiceGirls, tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls;
			tmpSexGirls.clear(); tmpServiceGirls.clear(); tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear();
			sGirl* pTmpGirl;
			bool sexjob = false;

			// Major change in the way this is handled... want to be able to list sex related jobs together. 
			// Disabled and replaced the enire section dealing with populating the listboxes here. --PP
			// Find out which girls have sex type jobs.
			for (int i=0; i<nNumGirls; i++)
			{
				pTmpGirl = g_Brothels.GetGirl(g_CurrBrothel, i);
				sexjob = false;
				switch(pTmpGirl->m_DayJob)
				{
					case JOB_XXXENTERTAINMENT:
					case JOB_PEEP:
					case JOB_BARSTRIPPER:
					case JOB_BROTHELSTRIPPER:
					case JOB_MASSEUSE:
					case JOB_BARWHORE:
					case JOB_WHOREGAMBHALL:
					case JOB_WHOREBROTHEL:
					case JOB_WHORESTREETS:
					case JOB_ESCORT:
						sexjob = true;
						break;
					default:
						break;
				}
				switch(pTmpGirl->m_NightJob)
				{
					case JOB_XXXENTERTAINMENT:
					case JOB_PEEP:
					case JOB_BARSTRIPPER:
					case JOB_BROTHELSTRIPPER:
					case JOB_MASSEUSE:
					case JOB_BARWHORE:
					case JOB_WHOREGAMBHALL:
					case JOB_WHOREBROTHEL:
					case JOB_WHORESTREETS:
					case JOB_ESCORT:
						  sexjob = true;
						break;
					default:
						break;
				}
				// Sort the girls into 4 catagories... sex jobs, service jobs, warning, and danger
				// `J` added 5th catagory... goodnews
				// If we want to we could add a seperate catagory for each job and order the list even further, but it will make this clunkier.
				/* */if (!pTmpGirl->m_Events.HasUrgent() && sexjob)	tmpSexGirls.push_back(pTmpGirl);
				else if (!pTmpGirl->m_Events.HasUrgent())			tmpServiceGirls.push_back(pTmpGirl);
				else if (pTmpGirl->m_Events.HasGoodNews())			tmpGoodNewsGirls.push_back(pTmpGirl);
				else if (pTmpGirl->m_Events.HasDanger())			tmpDangerGirls.push_back(pTmpGirl);
				else /*                                 */			tmpWarningGirls.push_back(pTmpGirl);
			}
			// Put the catagories into the List Boxes... to change what order they are listed in, just swap these for-next loops. --PP
			//Girls with GoodNews events
			for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
			{
				string tname = tmpGoodNewsGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i]) Item = ID;
				ID++;
			}
			//Girls with Danger events
			for (u_int i = 0; i < tmpDangerGirls.size(); i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
				if (selected_girl == tmpDangerGirls[i]) Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for(u_int i = 0; i < tmpWarningGirls.size() ;i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_DARKBLUE);
				if (selected_girl == tmpWarningGirls[i]) Item = ID;
				ID++;
			}
			//ServiceJob Girls
			for(u_int i = 0; i < tmpServiceGirls.size() ;i++)
			{
				string tname = tmpServiceGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpServiceGirls[i]) Item = ID;
				ID++;
			}
			//SexJob girls
			for(u_int i = 0; i < tmpSexGirls.size() ;i++)
			{
				string tname = tmpSexGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpSexGirls[i]) Item = ID;
				ID++;
			}
		}

		// 1. Gangs
		else if(category == 1)
		{
			for(int i=0; i<g_Gangs.GetNumGangs(); i++)
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, i, g_Gangs.GetGang(i)->m_Name);
		}

		// 2. Brothels
		else if(category == 2)
		{
			for(int i=0; i<g_Brothels.GetNumBrothels(); i++)
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, i, g_Brothels.GetBrothel(i)->m_Name);
		}
		
		// 3. `J` New Dungeon code
		else if (category == 3)
		{

			// Fill the list box
			cDungeon* pDungeon = g_Brothels.GetDungeon();
			int ID = 0, nNumGirls = pDungeon->GetNumGirls();
			vector<sGirl*> tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls, tmpOtherGirls;
			tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear(); tmpOtherGirls.clear();
			sGirl* pTmpGirl;

			for (int i = 0; i < nNumGirls; i++)
			{
				pTmpGirl = pDungeon->GetGirl(i)->m_Girl;

				/* */if (!pTmpGirl->m_Events.HasUrgent())	tmpOtherGirls.push_back(pTmpGirl);
				else if (pTmpGirl->m_Events.HasGoodNews())	tmpGoodNewsGirls.push_back(pTmpGirl);
				else if (pTmpGirl->m_Events.HasDanger())	tmpDangerGirls.push_back(pTmpGirl);
				else /*                                */	tmpWarningGirls.push_back(pTmpGirl);
			}

			//Girls with GoodNews events
			for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
			{
				string tname = tmpGoodNewsGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i]) Item = ID;
				ID++;
			}
			//Girls with Danger events
			for (u_int i = 0; i < tmpDangerGirls.size(); i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
				if (selected_girl == tmpDangerGirls[i]) Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for (u_int i = 0; i < tmpWarningGirls.size(); i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_DARKBLUE);
				if (selected_girl == tmpWarningGirls[i]) Item = ID;
				ID++;
			}
			//ServiceJob Girls
			for (u_int i = 0; i < tmpOtherGirls.size(); i++)
			{
				string tname = tmpOtherGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpOtherGirls[i]) Item = ID;
				ID++;
			}
		}// End of New dungeon code

		// 4. Clinic
		else if(category == 4)
		{
			int nNumGirlsClinic = g_Clinic.GetNumGirls(0);
			int ID			= 0;

// `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirlsClinic; h++)
			{
				sGirl* pTmpGirl = g_Clinic.GetGirl(0, h);
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for (int i = 0; i<nNumGirlsClinic; i++)
			{
				sGirl* pTmpGirl = g_Clinic.GetGirl(0, i);
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
				
			// Girls with warning events next
			for(int j=0; j<nNumGirlsClinic; j++)
			{
				sGirl* pTmpGirl = g_Clinic.GetGirl(0, j);
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// clinic girl
			for(int k=0; k<nNumGirlsClinic; k++)
			{
				sGirl* pTmpGirl = g_Clinic.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of clinic

		// 5. Studio
		else if(category == 5)
		{
			int nNumGirlsStudio = g_Studios.GetNumGirls(g_CurrStudio);
			int ID			= 0;
			vector<sGirl*> tmpSexGirls;
			vector<sGirl*> tmpServiceGirls;
			vector<sGirl*> tmpGoodNewsGirls;
			vector<sGirl*> tmpDangerGirls;
			vector<sGirl*> tmpWarningGirls;
			tmpSexGirls.clear();
			tmpServiceGirls.clear();
			tmpGoodNewsGirls.clear();
			tmpDangerGirls.clear();
			tmpWarningGirls.clear();
			sGirl* pTmpGirl;
			bool sexjob = false;

			// Major change in the way this is handled... want to be able to list sex related jobs together. 
			// Disabled and replaced the enire section dealing with populating the listboxes here. --PP
			// Find out which girls have sex type jobs.
			for (int i=0; i<nNumGirlsStudio; i++)
			{
				pTmpGirl = g_Studios.GetGirl(0, i); // `J` changed g_CurrBrothel to 0
				sexjob = false;
				switch(pTmpGirl->m_NightJob)
				{
					case JOB_FILMBEAST:
					case JOB_FILMSEX:
					case JOB_FILMANAL:
					case JOB_FILMLESBIAN:
					case JOB_FILMBONDAGE:
					case JOB_FILMGROUP:
					case JOB_FILMORAL:
					case JOB_FILMMAST:
					case JOB_FILMTITTY:
					case JOB_FILMSTRIP:
					case JOB_FILMRANDOM:
					case JOB_FLUFFER:
						  sexjob = true;
						break;
					default:
						break;
				}
				// Sort the girls into 4 catagories... sex jobs, service jobs, warning, and danger
				// If we want to we could add a seperate catagory for each job and order the list even further, but it will make this clunkier.
				if (!pTmpGirl->m_Events.HasUrgent() && sexjob)
				{
					tmpSexGirls.push_back(pTmpGirl);
				}
				else if(!pTmpGirl->m_Events.HasUrgent())
				{
					tmpServiceGirls.push_back(pTmpGirl);
				}
				else if (pTmpGirl->m_Events.HasGoodNews())
				{
					tmpGoodNewsGirls.push_back(pTmpGirl);
				}
				else if (pTmpGirl->m_Events.HasDanger())
				{
					tmpDangerGirls.push_back(pTmpGirl);
				}
				else
					tmpWarningGirls.push_back(pTmpGirl);
			}
			// Put the catagories into the List Boxes... to change what order they are listed in, just swap these for-next loops. --PP
			//Girls with GoodNews events
			for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
			{
				string tname = tmpGoodNewsGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i])
					Item = ID;
				ID++;
			}
			//Girls with Danger events
			for(u_int i = 0; i < tmpDangerGirls.size() ;i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
				if (selected_girl == tmpDangerGirls[i])
					Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for(u_int i = 0; i < tmpWarningGirls.size() ;i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_DARKBLUE);
				if (selected_girl == tmpWarningGirls[i])
					Item = ID;
				ID++;
			}
			//ServiceJob Girls
			for(u_int i = 0; i < tmpServiceGirls.size() ;i++)
			{
				string tname = tmpServiceGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpServiceGirls[i])
					Item = ID;
				ID++;
			}
			//SexJob girls
			for(u_int i = 0; i < tmpSexGirls.size() ;i++)
			{
				string tname = tmpSexGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpSexGirls[i])
					Item = ID;
				ID++;
			}
		}

// MYR: Girls with danger events first
			/*for(int i=0; i<nNumGirlsStudio; i++)
			{
				sGirl* pTmpGirl = g_Studios.GetGirl(0, i);
				if(pTmpGirl->m_Events.HasDanger())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
				
			// Girls with warning events next
			for(int j=0; j<nNumGirlsStudio; j++)
			{
				sGirl* pTmpGirl = g_Studios.GetGirl(0, j);
				if( pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// studio girl
			for(int k=0; k<nNumGirlsStudio; k++)
			{
				sGirl* pTmpGirl = g_Studios.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of studio
*/
		// 6. arena
		else if(category == 6)
		{
			int nNumGirlsArena = g_Arena.GetNumGirls(g_CurrArena);
			int ID			= 0;

			// `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirlsArena; h++)
			{
				sGirl* pTmpGirl = g_Arena.GetGirl(0, h);
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for (int i = 0; i<nNumGirlsArena; i++)
			{
				sGirl* pTmpGirl = g_Arena.GetGirl(0, i);
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Girls with warning events next
			for(int j=0; j<nNumGirlsArena; j++)
			{
				sGirl* pTmpGirl = g_Arena.GetGirl(0, j);
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Arena girl
			for(int k=0; k<nNumGirlsArena; k++)
			{
				sGirl* pTmpGirl = g_Arena.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of arena

		// 7. centre
		else if(category == 7)
		{
			int nNumGirlsCentre = g_Centre.GetNumGirls(g_CurrCentre);
			int ID			= 0;

			// `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirlsCentre; h++)
			{
				sGirl* pTmpGirl = g_Centre.GetGirl(0, h);
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for (int i = 0; i<nNumGirlsCentre; i++)
			{
				sGirl* pTmpGirl = g_Centre.GetGirl(0, i);
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Girls with warning events next
			for(int j=0; j<nNumGirlsCentre; j++)
			{
				sGirl* pTmpGirl = g_Centre.GetGirl(0, j);
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Centre girl
			for(int k=0; k<nNumGirlsCentre; k++)
			{
				sGirl* pTmpGirl = g_Centre.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of Centre

		// 8. house
		else if(category == 8)
		{
			int nNumGirlsHouse = g_House.GetNumGirls(g_CurrHouse);
			int ID			= 0;

			// `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirlsHouse; h++)
			{
				sGirl* pTmpGirl = g_House.GetGirl(0, h);
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for (int i = 0; i<nNumGirlsHouse; i++)
			{
				sGirl* pTmpGirl = g_House.GetGirl(0, i);
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Girls with warning events next
			for(int j=0; j<nNumGirlsHouse; j++)
			{
				sGirl* pTmpGirl = g_House.GetGirl(0, j);
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// House girl
			for(int k=0; k<nNumGirlsHouse; k++)
			{
				sGirl* pTmpGirl = g_House.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of House

		// 9. farm
		else if(category == 9)
		{
			int nNumGirlsFarm = g_Farm.GetNumGirls(g_CurrFarm);
			int ID			= 0;

			// `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirlsFarm; h++)
			{
				sGirl* pTmpGirl = g_Farm.GetGirl(0, h);
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for (int i = 0; i<nNumGirlsFarm; i++)
			{
				sGirl* pTmpGirl = g_Farm.GetGirl(0, i);
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, COLOR_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Girls with warning events next
			for(int j=0; j<nNumGirlsFarm; j++)
			{
				sGirl* pTmpGirl = g_Farm.GetGirl(0, j);
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, COLOR_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Farm girl
			for(int k=0; k<nNumGirlsFarm; k++)
			{
				sGirl* pTmpGirl = g_Farm.GetGirl(0, k);
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of Farm


		// Sets default selected item
        if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSITEM) > 0)
		  g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSITEM, 0, true);

		if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSITEM) >= Item)
			g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSITEM, Item, true);
		else if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSITEM) > 0)
		{
			Item = 0;
			g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSITEM, Item, true);
		}

		// Set the brothels name
		g_Turnsummary.EditTextItem("", g_interfaceid.TEXT_TSEVENTDESC);

		g_InitWin = false;
		g_Turnsummary.DisableButton(g_interfaceid.BUTTON_TSGOTO, true);
	}  // End of if initwin

/*
 *		EVENTS Listbox
 */

	if(g_InterfaceEvents.GetNumEvents() != 0)
	{
		// Change category
		if(g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TSCATEGORY))
		{
			if((category = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSCATEGORY)) == -1)	// if a girl is selected then
			{
				category = 0;
			}
			else
				Item = 0;
			g_InitWin = true;
		}

		// Change item
		else if(g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TSITEM))
		{
			int selected = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
			if(selected == -1)
				g_Turnsummary.DisableButton(g_interfaceid.BUTTON_TSGOTO, true);
			else
				g_Turnsummary.DisableButton(g_interfaceid.BUTTON_TSGOTO, false);


			g_Turnsummary.ClearListBox(g_interfaceid.LIST_TSEVENTS);
			g_Turnsummary.EditTextItem("", g_interfaceid.TEXT_TSEVENTDESC);
			// ------------ Girl Event display
			if(category == 0)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------ Gangs item display
			else if(category == 1)
			{
				if(selected != -1)
				{
					// Get the gang
					sGang* gang = g_Gangs.GetGang(selected);

					if(gang == 0)
						return;

					if (!gang->m_Events.IsEmpty())
					{
						gang->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<gang->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= gang->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= gang->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------- Brothels item display
			else if(category == 2)
			{
				if(selected != -1)
				{
					sBrothel	*pSelectedBrothel = g_Brothels.GetBrothel(selected);
					if (!pSelectedBrothel->m_Events.IsEmpty())
					{
						pSelectedBrothel->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<g_Brothels.GetBrothel(selected)->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= pSelectedBrothel->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= pSelectedBrothel->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// -------- Dungeons item display
			else if (category == 3) {
				if(selected != -1)
				{
					// list the events
					// Get the girl
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl =	selected_girl	= g_Brothels.GetDungeon()->GetGirlByName(selectedName)->m_Girl;

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}
			// ------------ Girl clinic Event display
			if(category == 4)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Clinic.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}
			
			// ------------ Girl studio Event display
			if(category == 5)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Studios.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------ Girl arena Event display
			if(category == 6)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Arena.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------ Girl Centre Event display
			if(category == 7)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Centre.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------ Girl House Event display
			if(category == 8)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_House.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

			// ------------ Girl Farm Event display
			if(category == 9)
			{
				if(selected != -1)
				{
					string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Farm.GetGirlByName(0, selectedName);

					if(girl == 0)
						return;

					// change the picture to profile (no event selected)
					ImageType = IMGTYPE_PROFILE;

					if (!girl->m_Events.IsEmpty())
					{
						girl->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

						for(int l=0; l<girl->m_Events.GetNumEvents(); l++)
						{
							string			sTitle			= girl->m_Events.GetMessage(l).TitleText();
							unsigned int	uiListboxColour	= girl->m_Events.GetMessage(l).ListboxColour();
							g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSEVENTS, l, sTitle, uiListboxColour);
						}
					}
					if(g_Turnsummary.GetListBoxSize(g_interfaceid.LIST_TSEVENTS) > 0)
						g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSEVENTS, 0, true);
				}
			}

		}


		// Girl event selection
		else if(g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TSEVENTS))
		{
			if(category == 0)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Gang event selection
			else if(category == 1)
			{
				int SelEvent = -1;
				// if a event is selected then
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGang;
					// if a gang is selected then
					if((SelGang = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGang* gang = g_Gangs.GetGang(SelGang);

						// Set the event desc text
						g_Turnsummary.EditTextItem(gang->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = gang->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Brothel event selection
			else if(category == 2)
			{
				int SelEvent = -1;
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	// if a event is selected then
				{
					int SelBrothel;
					if((SelBrothel = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)		// if a brothel is selected then
					{
						// Set the event desc text
						g_Turnsummary.EditTextItem(g_Brothels.GetBrothel(SelBrothel)->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = g_Brothels.GetBrothel(SelBrothel)->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Dungeon event selection
			else if (category == 3)
			{
				int SelEvent = -1;
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	// if a event is selected then
				{
					int SelGirl;
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	// if a dungeon is selected then
					{
						// WD	Get girl by name
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						sGirl* girl			= g_Brothels.GetDungeon()->GetGirlByName(selectedName)->m_Girl;

						if(girl == 0)
							return;

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Clinic
			else if(category == 4)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Clinic.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Studio
			else if(category == 5)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Studios.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

		// arena
			else if(category == 6)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Arena.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Centre
			else if(category == 7)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Centre.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// House
			else if(category == 8)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_House.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}

			// Farm
			else if(category == 9)
			{
				int SelEvent = -1;
				// ****************************
				// When warnings and problems are displayed first, the ordering of events is messed up
				// A warning is at ID 0 in the list, and is ID X in the message queue
				// ****************************
				//
				//	WD: The message queue is now sorted in cEvents::DoSort()
				//		so ID's will match
				// ****************************
				if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
				{
					int SelGirl;
					// if a girl is selected then
					if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
					{
						sGirl* girl = 0;

						// MYR
						string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
						girl = selected_girl = g_Farm.GetGirlByName(0, selectedName);

						// Set the event desc text
						g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

						// Change the picture
						ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

						lastNum = -1;
					}
				}
			}
		}

		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TSCLOSE))
		{
			g_WinManager.Pop();
			g_InitWin = true;
			return;
		}

		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TSNEWWEEK))
		{
			g_InitWin = true;
			SaveGameXML( DirPath() << "Saves" << "autosave.gam" );
			NextWeek();
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TSNEXTBROTHEL))
		{
			g_CurrBrothel++;
			if(g_CurrBrothel >= g_Brothels.GetNumBrothels())
				g_CurrBrothel=0;
			g_InitWin = true;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TSPREVBROTHEL))
		{
			g_CurrBrothel--;
			if(g_CurrBrothel < 0)
				g_CurrBrothel =  g_Brothels.GetNumBrothels()-1;
			g_InitWin = true;
			return;
		}
		/*
		 *	GOTO Selected Girl or Gang
		 */
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TSGOTO))
		{
			int selected = 0;
			selected = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
			if(selected != -1)
			{
				g_GirlDetails.lastsexact = -1;
				if(category == 0)
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 1)
				{
					g_CurrentScreen = SCREEN_GANGMANAGEMENT;
					g_WinManager.push("Gangs");
				}
				else if (category == 3)		// Dungeon
				{
					// WD: Add Dungeon Girls Goto
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					selected_girl	= girl	= g_Brothels.GetDungeon()->GetGirlByName(selectedName)->m_Girl;
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");

				}
				else if(category == 4)  //clinc
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Clinic.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 5)  //studio
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Studios.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 6)  //arena
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Arena.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 7)  //centre
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Centre.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 8)  //House
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_House.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				else if(category == 9)  //Farm
				{
					// MYR
					string selectedName		= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl	= selected_girl	= g_Farm.GetGirlByName(0, selectedName);
					g_CurrentScreen = SCREEN_GIRLDETAILS;
					g_WinManager.push("Girl Details");
				}
				g_InitWin = true;
			}
			return;
		}
	}

	if(g_UpArrow)
	{
		Item = g_Turnsummary.ArrowUpListBox(g_interfaceid.LIST_TSITEM);
		g_UpArrow = false;
		selected_girl = 0;
		g_InitWin = true;
		return;
	}
	else if(g_DownArrow)
	{
		Item = g_Turnsummary.ArrowDownListBox(g_interfaceid.LIST_TSITEM);
		g_DownArrow = false;
		selected_girl = 0;
		g_InitWin = true;
		return;
	}
	// Left and right arrow keys now  scroll through events instead of Brothels. -PP
	if(g_LeftArrow)
	{
		g_LeftArrow = false;
		Item = g_Turnsummary.ArrowUpListBox(g_interfaceid.LIST_TSEVENTS);
		return;
	}
	else if(g_RightArrow)
	{
		g_RightArrow = false;
		Item = g_Turnsummary.ArrowDownListBox(g_interfaceid.LIST_TSEVENTS);
		return;
	}
	if(g_AltKeys)
	{		// A and D Scroll up and down the Girls list.
	if(g_A_Key)
	{
		Item = g_Turnsummary.ArrowUpListBox(g_interfaceid.LIST_TSITEM);
		g_A_Key = false;
		selected_girl = 0;
		g_InitWin = true;
		return;
	}
	else if(g_D_Key)
	{
		Item = g_Turnsummary.ArrowDownListBox(g_interfaceid.LIST_TSITEM);
		g_D_Key = false;
		selected_girl = 0;
		g_InitWin = true;
		return;
	}
	if(g_W_Key)		// W and S scroll through the events list
	{
		g_W_Key = false;
		Item = g_Turnsummary.ArrowUpListBox(g_interfaceid.LIST_TSEVENTS);
		return;
	}
	else if(g_S_Key)
	{
		g_S_Key = false;
		Item = g_Turnsummary.ArrowDownListBox(g_interfaceid.LIST_TSEVENTS);
		return;
	}
	if(g_E_Key)		//Q and E scrolls through the Catagories list
	{
		g_E_Key = false;
		category ++;
		if(category > 9)
			category = 0;
		g_InitWin = true;
	}
	else if(g_Q_Key)
	{
		g_Q_Key = false;
		category --;
		if(category < 0)
			category = 9;
		g_InitWin = true;
	}
	if(g_SpaceKey && category == 0)			//Space key changes the current show picture to another in the same catagory.
	{
		g_SpaceKey = false;
		int SelEvent = -1;
		if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
			{
				int SelGirl;
				// if a girl is selected then
				if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
				{
					sGirl* girl = 0;

					// MYR
					string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);

					// Set the event desc text
					g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

					// Change the picture
					ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

					lastNum = -1;
				}
			}
	}
	if(g_SpaceKey && category == 5)			//Space key changes the current show picture to another in the same catagory. For Studio
	{
		g_SpaceKey = false;
		int SelEvent = -1;
		if((SelEvent = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSEVENTS)) != -1)	
			{
				int SelGirl;
				// if a girl is selected then
				if((SelGirl = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM)) != -1)	
				{
					sGirl* girl = 0;

					// MYR
					string selectedName	= g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
					girl = selected_girl = g_Studios.GetGirlByName(0, selectedName);

					// Set the event desc text
					g_Turnsummary.EditTextItem(girl->m_Events.GetMessage(SelEvent).m_Message, g_interfaceid.TEXT_TSEVENTDESC);

					// Change the picture
					ImageType = girl->m_Events.GetMessage(SelEvent).m_MessageType;

					lastNum = -1;
				}
			}
	}
	}
/*
	if(g_LeftArrow)
	{
		g_CurrBrothel--;
		if(g_CurrBrothel < 0)
			g_CurrBrothel =  g_Brothels.GetNumBrothels()-1;
		g_InitWin = true;
		g_LeftArrow = false;
		category = Item = 0;
		return;
	}
	else if(g_RightArrow)
	{
		g_CurrBrothel++;
		if(g_CurrBrothel >= g_Brothels.GetNumBrothels())
			g_CurrBrothel=0;
		g_InitWin = true;
		g_RightArrow = false;
		category = Item = 0;
		return;
	}
*/

/*
 *	Draw a girls picture and description when selected
 *	Category 1 is easier, so let's get that out of the way first
 */
	if(category == 1) {
		g_Turnsummary.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_BrothelImages[g_CurrBrothel]);
		return;
	}
/*
 *	make sure we don't trip over any other categories that were being 
 *	silently ignored
 */
	if (category != 0 && category != 3 && category != 4 && category != 5 && 
		category != 6 && category != 7 && category != 8 && category != 9)
	{
		return;
	}
	int GirlNum = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
	if (GirlNum == -1) return;	// again, no selected item means "nothing to do" so let's do that first :)

/*
 *	both cases need the selected girl data - so let's get that
 */
	sGirl* selGirl = NULL;
	if (category == 0)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 3)
	{
		cDungeon* pDungeon = g_Brothels.GetDungeon();
		if (pDungeon->GetNumGirls() > 0)
		{
			sDungeonGirl *selDGirl = pDungeon->GetGirl(GirlNum);
			if(selDGirl != 0)
				selGirl = selDGirl->m_Girl;
			else
				selGirl = 0;
		}
	}
	else if (category == 4)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Clinic.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 5)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Studios.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 6)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Arena.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 7)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Centre.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 8)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_House.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
	else if (category == 9)
	{
		// MYR
		string selectedName = g_Turnsummary.GetSelectedTextFromList(g_interfaceid.LIST_TSITEM);
		selGirl = selected_girl = g_Farm.GetGirlByName(0, selectedName);
		//selGirl = g_Brothels.GetGirl(g_CurrBrothel, num);
	}
/*
 *	not really sure what's going on here. lastNum is static so 
 *	persists from one invocation to the next. Ditto lastType.
 *
 *	num is the selection in the list box
 *
 *	so we're saying "if this is a different girl, or if we didn't
 *	display a girl last time - do this:"
 *
 *	let's set a flag with that
 */
	bool image_changed = (lastNum != GirlNum || LastType != ImageType);
/*
 *	now: the setImage happens regardless of whether the image changed or not
 *	assuming selGirl is set, anyhow. The only difference is that the
 *	random parameter passed is true if the "image_changed" is true:
 *
 *	that makes sense - this is a new girl, so go find a random image of her
 */
	if(selGirl)
	{
		if((selGirl->m_newRandomFixed >= 0) && (ImageType == IMGTYPE_PROFILE))
			g_Turnsummary.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(selGirl, ImageType, false, selGirl->m_newRandomFixed));
		else 
		{
			g_Turnsummary.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(selGirl, ImageType, image_changed, ImageNum));
			if(g_Girls.IsAnimatedSurface(selGirl, ImageType, ImageNum))
				g_Turnsummary.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetAnimatedSurface(selGirl, ImageType, ImageNum));
		}
	}
/*
 *	now we need to update the "last" variables
 *
 *	this is cheap enough we can do it whether there's been a change
 *	or not
 */
	LastType = ImageType;
	lastNum = GirlNum;
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
	g_TalkCount = 10 + (g_Brothels.GetPlayer()->m_Stats[STAT_CHARISMA] / 10);
	// */ //

	if (g_Cheats)	g_Gold.cheat();

	// Clear choice dialog
	g_ChoiceManager.Free();

	// update the shop inventory
	g_InvManager.UpdateShop();

	// Clear the interface events
	g_InterfaceEvents.ClearEvents();

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
}

void GameEvents()
{
	cScriptManager sm;
/*
 *	The "" on the end forces it to end in a / or \   
 */
	DirPath dp = DirPath() << "Resources" << "Scripts" << "";

	if(!sm.IsActive())
		eventrunning = false;

	// process global triggers here
	if(g_GlobalTriggers.GetNextQueItem() && !eventrunning)
	{
		g_GlobalTriggers.ProcessNextQueItem( dp );
		eventrunning = true;
	}
	else if(!eventrunning)	// check girl scripts
	{
		if(g_Brothels.CheckScripts())
			eventrunning = true;
	}

	if(eventrunning)
	{
		sm.RunScript();
	}

	if(CheckGameFlag(FLAG_DUNGEONGIRLDIE))	// a girl has died int the dungeon
	{
		g_MessageQue.AddToQue(gettext("A girl has died in the dungeon.\nHer body will be removed by the end of the week."), 1);

		if(g_Dice%100 < 10)	// only 10% of being discovered
		{
			g_Brothels.GetPlayer()->suspicion(1);
		}
		g_Brothels.GetPlayer()->disposition(-1);
		g_Brothels.UpdateAllGirlsStat(0, STAT_PCFEAR, 2);

		ClearGameFlag(FLAG_DUNGEONGIRLDIE);
	}
	else if(CheckGameFlag(FLAG_DUNGEONCUSTDIE))	// a customer has died in the dungeon
	{
		g_MessageQue.AddToQue(gettext("A customer has died in the dungeon.\nTheir body will be removed by the end of the week."), 1);
		
		if(g_Dice%100 < 10)	// only 10% chance of being found out
		{
			g_Brothels.GetPlayer()->suspicion(1);
		}
		g_Brothels.GetPlayer()->disposition(-1);
		g_Brothels.GetPlayer()->customerfear(1);

		ClearGameFlag(FLAG_DUNGEONCUSTDIE);
	}
}

void Gallery()
{
	static int Mode = IMGTYPE_ANAL;
	static int Img = 0;	// what image currently drawing
	sGirl *girl = selected_girl;

	g_CurrentScreen = SCREEN_GALLERY;
	if (g_InitWin)
	{
		if (girl == 0)
		{
			g_InitWin = true;
			g_MessageQue.AddToQue("ERROR: No girl selected", 1);
			g_WinManager.Pop();
			return;
		}
		g_Gallery.Reset();
		Mode = IMGTYPE_ANAL;
		Img = 0;
		g_Gallery.Focused();

		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, (girl->m_GirlImages->m_Images[IMGTYPE_ANAL].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBDSM, (girl->m_GirlImages->m_Images[IMGTYPE_BDSM].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYSEX, (girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBEAST, (girl->m_GirlImages->m_Images[IMGTYPE_BEAST].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYGROUP, (girl->m_GirlImages->m_Images[IMGTYPE_GROUP].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYLESBIAN, (girl->m_GirlImages->m_Images[IMGTYPE_LESBIAN].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPREGNANT, (girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYDEATH, (girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPROFILE, (girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYCOMBAT, (girl->m_GirlImages->m_Images[IMGTYPE_COMBAT].m_NumImages == 0));
		g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYORAL, (girl->m_GirlImages->m_Images[IMGTYPE_ORAL].m_NumImages == 0));

		while (girl->m_GirlImages->m_Images[Mode].m_NumImages == 0 && Mode < NUM_IMGTYPES)
		{
			Mode++;
		}

		if (Img >= girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		else if (Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages - 1;

		g_InitWin = false;
	}

	if (g_InterfaceEvents.GetNumEvents() != 0)
	{
		if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBACK))
		{
			g_WinManager.Pop();
			g_InitWin = true;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYANAL))
		{
			Mode = IMGTYPE_ANAL;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBDSM))
		{
			Mode = IMGTYPE_BDSM;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSEX))
		{
			Mode = IMGTYPE_SEX;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBEAST))
		{
			Mode = IMGTYPE_BEAST;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYGROUP))
		{
			Mode = IMGTYPE_GROUP;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYLESBIAN))
		{
			Mode = IMGTYPE_LESBIAN;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREGNANT))
		{
			Mode = IMGTYPE_PREGNANT;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYDEATH))
		{
			Mode = IMGTYPE_DEATH;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPROFILE))
		{
			Mode = IMGTYPE_PROFILE;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYCOMBAT))
		{
			Mode = IMGTYPE_COMBAT;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYORAL))
		{
			Mode = IMGTYPE_ORAL;
			Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREV))
		{
			Img--;
			if (Img < 0)
				Img = girl->m_GirlImages->m_Images[Mode].m_NumImages - 1;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYNEXT))
		{
			Img++;
			if (Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
				Img = 0;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_NEXTGALLERY))
		{
			g_InitWin = true;
			g_WinManager.Push(Gallery2, &g_Gallery2);
			return;
		}
	}

	if (g_LeftArrow || g_A_Key)
	{
		g_LeftArrow = g_A_Key = false;
		Img--;
		if (Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages - 1;
		return;
	}
	else if (g_RightArrow || g_D_Key)
	{
		g_RightArrow = g_D_Key = false;
		Img++;
		if (Img >= girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		return;
	}
	if (g_UpArrow || g_W_Key)
	{
		int i = 0;
		while (i <= NUM_IMGTYPES)
		{
			g_UpArrow = g_W_Key = false;
			Mode--; i++;
			if (Mode < 0)
				Mode = NUM_IMGTYPES - 1;
			Img = 0;
			if (girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if (g_DownArrow || g_S_Key)
	{
		int i = 0;
		while (i <= NUM_IMGTYPES)
		{
			g_DownArrow = g_S_Key = false;
			Mode++; i++;
			if (Mode >= NUM_IMGTYPES)
				Mode = 0;
			Img = 0;
			if (girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	if (Mode >= NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}

	// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> InterfaceProcesses.cpp > Gallery
	string galtxt[] = { "Anal", "BDSM", "Sex", "Beast", "Group", "Lesbian", "Torture", "Death", "Profile", "Combat",
		"Oral", "Ecchi", "Strip", "Maid", "Sing", "Wait", "Card", "Bunny", "Nude", "Mast", "Titty", "Milk", "Hand",
		"Foot", "Bed", "Farm", "Herd", "Cook", "Craft", "Swim", "Bath", "Nurse", "Formal", "Shop", "Magic",
		// pregnant varients
		"Pregnant", "Pregnant\nAnal", "Pregnant\nBDSM", "Pregnant\nSex", "Pregnant\nBeast", "Pregnant\nGroup",
		"Pregnant\nLesbian", "Pregnant\nTorture", "Pregnant\nDeath", "Pregnant\nProfile", "Pregnant\nCombat",
		"Pregnant\nOral", "Pregnant\nEcchi", "Pregnant\nStrip", "Pregnant\nMaid", "Pregnant\nSing", "Pregnant\nWait",
		"Pregnant\nCard", "Pregnant\nBunny", "Pregnant\nNude", "Pregnant\nMast", "Pregnant\nTitty", "Pregnant\nMilk",
		"Pregnant\nHand", "Pregnant\nFoot", "Pregnant\nBed", "Pregnant\nFarm", "Pregnant\nHerd", "Pregnant\nCook",
		"Pregnant\nCraft", "Pregnant\nSwim", "Pregnant\nBath", "Pregnant\nNurse", "Pregnant\nFormal", "Pregnant\nShop",
		"Pregnant\nMagic" };
	g_Gallery.EditTextItem(galtxt[Mode], g_interfaceid.TEXT_GALLERYTYPE);	// Set the text for gallery type

	// Draw the image
	if (girl)
	{
		g_Gallery.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(girl, Mode, false, Img, true));
		if (g_Girls.IsAnimatedSurface(girl, Mode, Img))
			g_Gallery.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetAnimatedSurface(girl, Mode, Img));
	}
}
void Gallery2()
{
	static int Mode = IMGTYPE_ECCHI;
	static int Img = 0;	// what image currently drawing
	sGirl *girl = selected_girl;
	g_CurrentScreen = SCREEN_GALLERY2;
	if(g_InitWin)
	{
		if(girl == 0)
		{
			g_InitWin = true;
			g_MessageQue.AddToQue("ERROR: No girl selected", 1);
			g_WinManager.Pop();
			return;
		}
		g_Gallery2.Focused();

		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYECCHI, (girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSTRIP, (girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAID, (girl->m_GirlImages->m_Images[IMGTYPE_MAID].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSING, (girl->m_GirlImages->m_Images[IMGTYPE_SING].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYWAIT, (girl->m_GirlImages->m_Images[IMGTYPE_WAIT].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYCARD, (girl->m_GirlImages->m_Images[IMGTYPE_CARD].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYBUNNY, (girl->m_GirlImages->m_Images[IMGTYPE_BUNNY].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYNUDE, (girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAST, (girl->m_GirlImages->m_Images[IMGTYPE_MAST].m_NumImages == 0));
		g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYTITTY, (girl->m_GirlImages->m_Images[IMGTYPE_TITTY].m_NumImages == 0));

		while(girl->m_GirlImages->m_Images[Mode].m_NumImages == 0 && Mode < NUM_IMGTYPES)
		{
			Mode++;
		}

		if(Img >= girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		else if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;

		g_InitWin = false;
	}

	if(g_InterfaceEvents.GetNumEvents() != 0)
	{
		if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBACK))
		{
			g_WinManager.Pop();
			g_InitWin = true;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYECCHI))
		{
			Mode = IMGTYPE_ECCHI;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSTRIP))
		{
			Mode = IMGTYPE_STRIP;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYMAID))
		{
			Mode = IMGTYPE_MAID;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSING))
		{
			Mode = IMGTYPE_SING;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYWAIT))
		{
			Mode = IMGTYPE_WAIT;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYCARD))
		{
			Mode = IMGTYPE_CARD;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBUNNY))
		{
			Mode = IMGTYPE_BUNNY;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYNUDE))
		{
			Mode = IMGTYPE_NUDE;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYMAST))
		{
			Mode = IMGTYPE_MAST;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYTITTY))
		{
			Mode = IMGTYPE_TITTY;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREV))
		{
			Img--;
			if(Img < 0)
				Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYNEXT))
		{
			Img++;
			if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
				Img = 0;
			return;
		}
	}

	if (g_LeftArrow || g_A_Key)
	{
		g_LeftArrow = g_A_Key = false;
		Img--;
		if (Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages - 1;
		return;
	}
	else if (g_RightArrow || g_D_Key)
	{
		g_RightArrow = g_D_Key = false;
		Img++;
		if (Img >= girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		return;
	}
	if (g_UpArrow || g_W_Key)
	{
		int i = 0;
		while (i <= NUM_IMGTYPES)
		{
			g_UpArrow = g_W_Key = false;
			Mode--; i++;
			if (Mode < 0)
				Mode = NUM_IMGTYPES - 1;
			Img = 0;
			if (girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if (g_DownArrow || g_S_Key)
	{
		int i = 0;
		while (i <= NUM_IMGTYPES)
		{
			g_DownArrow = g_S_Key = false;
			Mode++; i++;
			if (Mode >= NUM_IMGTYPES)
				Mode = 0;
			Img = 0;
			if (girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	if (Mode >= NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}
	// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> InterfaceProcesses.cpp > Gallery2
	string galtxt[] = { "Anal", "BDSM", "Sex", "Beast", "Group", "Lesbian", "Torture", "Death", "Profile", "Combat",
		"Oral", "Ecchi", "Strip", "Maid", "Sing", "Wait", "Card", "Bunny", "Nude", "Mast", "Titty", "Milk", "Hand",
		"Foot", "Bed", "Farm", "Herd", "Cook", "Craft", "Swim", "Bath", "Nurse", "Formal", "Shop", "Magic",
		// pregnant varients
		"Pregnant", "Pregnant\nAnal", "Pregnant\nBDSM", "Pregnant\nSex", "Pregnant\nBeast", "Pregnant\nGroup",
		"Pregnant\nLesbian", "Pregnant\nTorture", "Pregnant\nDeath", "Pregnant\nProfile", "Pregnant\nCombat",
		"Pregnant\nOral", "Pregnant\nEcchi", "Pregnant\nStrip", "Pregnant\nMaid", "Pregnant\nSing", "Pregnant\nWait",
		"Pregnant\nCard", "Pregnant\nBunny", "Pregnant\nNude", "Pregnant\nMast", "Pregnant\nTitty", "Pregnant\nMilk",
		"Pregnant\nHand", "Pregnant\nFoot", "Pregnant\nBed", "Pregnant\nFarm", "Pregnant\nHerd", "Pregnant\nCook",
		"Pregnant\nCraft", "Pregnant\nSwim", "Pregnant\nBath", "Pregnant\nNurse", "Pregnant\nFormal", "Pregnant\nShop", 
		"Pregnant\nMagic" };
	g_Gallery.EditTextItem(galtxt[Mode], g_interfaceid.TEXT_GALLERYTYPE);	// Set the text for gallery type


	// Draw the image
	if(girl)
	{
		g_Gallery2.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(girl, Mode, false, Img, true));
		if(g_Girls.IsAnimatedSurface(girl, Mode, Img))
			g_Gallery2.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetAnimatedSurface(girl, Mode, Img));
	}
}

void SaveMasterFile(string filename)
{
/*
 *	I think this is outdated now. Which is to say
 *	we re-write the masterfile after we finish loading
 *	and I don't think we can pick up any new .girls files
 *	in mid play. So this should already be up to date.
 *
 *	I'm just commenting it out in case I'm wrong
 *	(Do we need this the first time we save?)
 *
	string savedFiles[400];
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	int numSaved = 0;

	// first load the masterlist to see what has been loaded before
	ifstream load;
	string mastfile = ".\\Saves\\";
	mastfile += filename;
	mastfile += ".mast";
	load.open(mastfile.c_str());

	numSaved = 0; 
	if(load)
	{
		while(load.good())
		{
			load.getline(buffer, 1000, '\n');
			savedFiles[numSaved] = buffer;
			numSaved++;
		}
	}
	load.close();

	// Load Girls
	DirPath location = DirPath() << "Resources" << "Characters\\*.girls";
	hFind = FindFirstFileA(location.c_str(), &FindFileData);
	do
	{
		if(hFind != INVALID_HANDLE_VALUE)
		{
			bool match = false;
			for(int i=0; i<numSaved; i++)
			{
				if(savedFiles[i] == FindFileData.cFileName)
					match = true;
			}

			if(!match)
			{
				savedFiles[numSaved] = FindFileData.cFileName;
				numSaved++;
			}
		}
		else
			break;
	}
	while(FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);

	ofstream save;
	mastfile = filename;
	mastfile += ".mast";
	save.open(mastfile.c_str());
	for(int j=0; j<numSaved; j++)
	{
		if(j==(numSaved-1))
			save<<savedFiles[j].c_str();
		else
			save<<savedFiles[j].c_str()<<endl;
	}
	save.close();
*/
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
	for(int i=0; i<8; i++)
	{
		MarketSlaveGirls[i] = 0;
		MarketSlaveGirlsDel[i] = -1;
	}

	// update the shop inventory
	g_InvManager.UpdateShop();
#endif

}

bool LoadGame(string directory, string filename)
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

	DirPath thefile;
	thefile<<directory<<filename;
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

	if(minorA != 6)
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
	g_Girls.LoadDefaultImages();

	if(g_Cheats)
	{
		g_WalkAround = false;
		g_TryCentre = false;
		g_TryOuts = false;
		g_TryEr = false;
		g_TryCast = false;
		g_TalkCount = 10;
	}

	selected_girl = 0;
	for(int i=0; i<8; i++)
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
		// add the clinic
		sClinic* currentClinic = (sClinic*)g_Clinic.GetBrothel(0);
		while (currentClinic)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 1, currentClinic->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 1, currentClinic->m_Name);
			currentClinic = (sClinic*)currentClinic->m_Next;
		}

		// add the arena
		sArena* currentArena = (sArena*)g_Arena.GetBrothel(0);
		while (currentArena)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 2, currentArena->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 2, currentArena->m_Name);
			currentArena = (sArena*)currentArena->m_Next;
		}

		// add the centre
		sCentre* currentCentre = (sCentre*)g_Centre.GetBrothel(0);
		while (currentCentre)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 3, currentCentre->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 3, currentCentre->m_Name);
			currentCentre = (sCentre*)currentCentre->m_Next;
		}

		// add the house
		sHouse* currentHouse = (sHouse*)g_House.GetBrothel(0);
		while (currentHouse)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 4, currentHouse->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 4, currentHouse->m_Name);
			currentHouse = (sHouse*)currentHouse->m_Next;
		}

		// add the farm
		sFarm* currentFarm = (sFarm*)g_Farm.GetBrothel(0);
		while (currentFarm)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 5, currentFarm->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 5, currentFarm->m_Name);
			currentFarm = (sFarm*)currentFarm->m_Next;
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
				else if (leftBrothel == 5)
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
				else if (leftBrothel == 4)
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
				else if (leftBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_DRUGCOUNSELOR || temp->m_NightJob == JOB_DRUGCOUNSELOR)
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
				else if (leftBrothel == 2)
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
				else if (leftBrothel == 1)
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
				else if (rightBrothel == 5)
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
				else if (rightBrothel == 4)
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
				else if (rightBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i = 0;
					while (temp)
					{
						if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
							color = COLOR_RED;
						else if (temp->m_DayJob == JOB_DRUGCOUNSELOR || temp->m_NightJob == JOB_DRUGCOUNSELOR)
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
				else if (rightBrothel == 2)
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
				else if (rightBrothel == 1)
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

static void TransferGirlsLeftToRight(int rightBrothel, int leftBrothel)
{
	sBrothel* brothel;
	if (leftBrothel > 5)
	{
		brothel = g_Brothels.GetBrothel(leftBrothel - 6);
	}
	else if (leftBrothel == 5)
	{
		brothel = g_Farm.GetBrothel(0);
	}
	else if (leftBrothel == 4)
	{
		brothel = g_House.GetBrothel(0);
	}
	else if (leftBrothel == 3)
	{
		brothel = g_Centre.GetBrothel(0);
	}
	else if (leftBrothel == 2)
	{
		brothel = g_Arena.GetBrothel(0);
	}
	else if (leftBrothel == 1)
	{
		brothel = g_Clinic.GetBrothel(0);
	}
	else if (leftBrothel == 0)
	{
		brothel = g_Studios.GetBrothel(0);
	}

	if(brothel->m_NumGirls == brothel->m_NumRooms)
		g_MessageQue.AddToQue(gettext("Left side building is full"), 1);
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTGIRLS, 0, pos);
		while(girlSelection != -1)
		{
			sGirl* temp;
			if (rightBrothel > 5)
			{
				// get the girl
				temp = g_Brothels.GetGirl(rightBrothel-6, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 5)
			{
				// get the girl
				temp = g_Farm.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 4)
			{
				// get the girl
				temp = g_House.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 3)
			{
				// get the girl
				temp = g_Centre.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 2)
			{
				// get the girl
				temp = g_Arena.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 1)
			{
				// get the girl
				temp = g_Clinic.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (rightBrothel == 0)
			{
				// get the girl
				temp = g_Studios.GetGirl(0, girlSelection-NumRemoved);
			}

			// check there is still room
			if(brothel->m_NumGirls+1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue(gettext("Left side building is full"), 1);
				break;
			}

			// remove girl from right side
			NumRemoved++;
			if (rightBrothel > 5)
			{
				g_Brothels.RemoveGirl(rightBrothel - 6, temp, false);
			} 
			else if (rightBrothel == 5)
			{
				g_Farm.RemoveGirl(0, temp, false);
			}
			else if (rightBrothel == 4)
			{
				g_House.RemoveGirl(0, temp, false);
			}
			else if (rightBrothel == 3)
			{
				g_Centre.RemoveGirl(0, temp, false);
			}
			else if (rightBrothel == 2)
			{
				g_Arena.RemoveGirl(0, temp, false);
			}
			else if (rightBrothel == 1)
			{
				g_Clinic.RemoveGirl(0, temp, false);
			}
			else if (rightBrothel == 0)
			{
				g_Studios.RemoveGirl(0, temp, false);
			}

			// add to left side
			if (leftBrothel > 5)
			{
				
				g_Brothels.AddGirl(leftBrothel - 6, temp);
			}
			else if (leftBrothel == 5)
			{
				g_Farm.AddGirl(0, temp);
			}
			else if (leftBrothel == 4)
			{
				g_House.AddGirl(0, temp);
			}
			else if (leftBrothel == 3)
			{
				g_Centre.AddGirl(0, temp);
			}
			else if (leftBrothel == 2)
			{
				g_Arena.AddGirl(0, temp);
			}
			else if (leftBrothel == 1)
			{
				g_Clinic.AddGirl(0, temp);
			}
			else if (leftBrothel == 0)
			{
				g_Studios.AddGirl(0, temp);
			}

			// get next girl
			girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTGIRLS, pos+1, pos);
		}

		// update the girl lists
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGLEFTBROTHEL, leftBrothel);
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, rightBrothel);
	}
}

static void TransferGirlsRightToLeft(int rightBrothel, int leftBrothel)
{
	sBrothel* brothel;
	if (rightBrothel > 5)
	{
		brothel = g_Brothels.GetBrothel(rightBrothel - 6);
	}
	else if (rightBrothel == 5)
	{
		brothel = g_Farm.GetBrothel(0);
	}
	else if (rightBrothel == 4)
	{
		brothel = g_House.GetBrothel(0);
	}
	else if (rightBrothel == 3)
	{
		brothel = g_Centre.GetBrothel(0);
	}
	else if (rightBrothel == 2)
	{
		brothel = g_Arena.GetBrothel(0);
	}
	else if (rightBrothel == 1)
	{
		brothel = g_Clinic.GetBrothel(0);
	}
	else if (rightBrothel == 0)
	{
		brothel = g_Studios.GetBrothel(0);
	}
	if(brothel->m_NumGirls == brothel->m_NumRooms)
		g_MessageQue.AddToQue(gettext("Right side building is full"), 1);
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTGIRLS, 0, pos);
		while(girlSelection != -1)
		{
			sGirl* temp;
			if (leftBrothel > 5)
			{
				// get the girl
				temp = g_Brothels.GetGirl(leftBrothel-6, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 5)
			{
				temp = g_Farm.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 4)
			{
				temp = g_House.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 3)
			{
				temp = g_Centre.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 2)
			{
				temp = g_Arena.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 1)
			{
				temp = g_Clinic.GetGirl(0, girlSelection-NumRemoved);
			}
			else if (leftBrothel == 0)
			{
				temp = g_Studios.GetGirl(0, girlSelection-NumRemoved);
			}

			// check there is still room
			if(brothel->m_NumGirls+1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue(gettext("Right side building is full"), 1);
				break;
			}

			// remove girl from left side
			NumRemoved++;
			if (leftBrothel > 5)
			{
				g_Brothels.RemoveGirl(leftBrothel - 6, temp, false);
			}
			else if (leftBrothel == 5)
			{
				g_Farm.RemoveGirl(0, temp, false);
			}
			else if (leftBrothel == 4)
			{
				g_House.RemoveGirl(0, temp, false);
			}
			else if (leftBrothel == 3)
			{
				g_Centre.RemoveGirl(0, temp, false);
			}
			else if (leftBrothel == 2)
			{
				g_Arena.RemoveGirl(0, temp, false);
			}
			else if (leftBrothel == 1)
			{
				g_Clinic.RemoveGirl(0, temp, false);
			}
			else if (leftBrothel == 0)
			{
				g_Studios.RemoveGirl(0, temp, false);
			}

			// add to right side
			if (rightBrothel > 5)
			{
				
				g_Brothels.AddGirl(rightBrothel - 6, temp);
			}
			else if (rightBrothel == 5)
			{
				g_Farm.AddGirl(0, temp);
			}
			else if (rightBrothel == 4)
			{
				g_House.AddGirl(0, temp);
			}
			else if (rightBrothel == 3)
			{
				g_Centre.AddGirl(0, temp);
			}
			else if (rightBrothel == 2)
			{
				g_Arena.AddGirl(0, temp);
			}
			else if (rightBrothel == 1)
			{
				g_Clinic.AddGirl(0, temp);
			}
			else if (rightBrothel == 0)
			{
				g_Studios.AddGirl(0, temp);
			}

			// get next girl
			girlSelection = g_TransferGirls.GetNextSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTGIRLS, pos+1, pos);
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

	if (g_InterfaceEvents.GetNumEvents() == 0 && !g_EnterKey) {
		return;
	}

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
	GirlsCSV << "'Building','Girl Name','Based on','Slave?','Day Job','Night Job','Age','Level','Exp','Askprice','House','Fame','Tiredness','Health','Happiness','Constitution','Charisma','Beauty','Intelligence','Confidence','Agility','Obedience','Spirit','Morality','Refinement','Dignity','Mana','Libido','Lactation','PCFear','PCLove','PCHate','Magic','Combat','Service','Medicine','Performance','Crafting','Herbalism','Farming','Brewing','Animalhandling','Normalsex','Anal','Bdsm','Beastiality','Group','Lesbian','Strip','Oralsex','Tittysex','Handjob','Footjob'"<<eol;
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
		if (buildingnum == 1)	{ building = g_House.GetBrothel(0);		if (building == 0) buildingnum++; }
		if (buildingnum == 2)	{ building = g_Arena.GetBrothel(0);		if (building == 0) buildingnum++; }
		if (buildingnum == 3)	{ building = g_Centre.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 4)	{ building = g_Clinic.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 5)	{ building = g_Studios.GetBrothel(0);	if (building == 0) buildingnum++; }
		if (buildingnum == 6)	{ building = g_Farm.GetBrothel(0);		if (building == 0) buildingnum++; }
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
