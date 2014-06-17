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
bool g_WalkAround = false;	// for keeping track of weather have walked around town today
bool g_TryCentre = false;
bool g_TryOuts = false;
bool g_TryEr = false;
bool g_TryCast = false;
int g_TalkCount = 10;
bool g_GenGirls = false;
bool g_Cheats = false;

extern int g_BrothelScreenImgX, g_BrothelScreenImgY, g_BrothelScreenImgW, g_BrothelScreenImgH;

sGirl* selected_girl;  // global pointer for the currently selected girl
vector<int> cycle_girls;  // globally available sorted list of girl IDs for Girl Details screen to cycle through
int cycle_pos;  //currently selected girl's position in the cycle_girls vector

sGirl* MarketSlaveGirls[8] = {0,0,0,0,0,0,0,0};
int MarketSlaveGirlsDel[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

CSurface* g_BrothelImages[7] = {0,0,0,0,0,0,0};
char buffer[1000];

extern bool g_LeftArrow;
extern bool g_RightArrow;
extern bool g_UpArrow;
extern bool g_DownArrow;
extern bool g_EnterKey;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;

extern	bool	g_R_Key;
extern	bool	g_F_Key;
extern	bool	g_T_Key;
extern	bool	g_G_Key;
extern	bool	g_Y_Key;
extern	bool	g_H_Key;
extern	bool	g_U_Key;
extern	bool	g_J_Key;
extern	bool	g_I_Key;
extern	bool	g_K_Key;

extern	int		g_CurrentScreen;

//used to store what files we have loaded
MasterFile loadedGirlsFiles;

void confirm_exit();

void LoadGameScreen()
{
	DirPath location = DirPath() << "Saves";
	const char *pattern = "*.gam";
	FileList fl(location, pattern);

	if(g_InitWin)
	{
		g_LoadGame.Focused();
/*
 *		clear the list box with the save games
 */
		g_LoadGame.ClearListBox(g_interfaceid.LIST_LOADGSAVES);
/*
 *		loop through the files, adding them to the box
 */
		for(int i = 0; i < fl.size(); i++) {
			g_LoadGame.AddToListBox(g_interfaceid.LIST_LOADGSAVES, i, fl[i].leaf());
		}
		g_InitWin = false;
	}

/*
 *	no events process means we can go home early
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

/*
 *	the next simplest case is the "back" button
 */
	if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGBACK))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
/*
 *	by this point, we're only interested if it's a click on the load game button or a double-click on a game in the list
 */
	if( !g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_LOADGLOAD)
		&& !g_LoadGame.ListDoubleClicked(g_interfaceid.LIST_LOADGSAVES) )
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
	if(selection == -1)
	{
		return;
	}
	string temp = fl[selection].leaf();
/* 
 *	enable cheat mode for a test brothel
 */
	g_Cheats = (temp == "Test.gam");

	if(LoadGame(location, fl[selection].leaf()))
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
	g_GenGirls = g_Cheats = g_WalkAround = false;	// for keeping track of weather have walked around town today
	g_GenGirls = g_Cheats = g_TryOuts = false;
	g_GenGirls = g_Cheats = g_TryCentre = false;
	g_GenGirls = g_Cheats = g_TryEr = false;
	g_GenGirls = g_Cheats = g_TryCast = false;
	g_TalkCount = 10;
	g_Brothels.Free();
	g_Clinic.Free();
	g_Studios.Free();
	g_Arena.Free();
	g_Centre.Free();
	g_House.Free();
	g_Gangs.Free();
	g_Customers.Free();
	g_Girls.Free();
	g_Traits.Free();
	g_GlobalTriggers.Free();
	g_Customers.Free();
	sm.Release();
	g_InvManager.Free();

	string d = g_ReturnText;
	if (g_ReturnText == "Test")
		g_Cheats = true;
	else
		g_Cheats = false;
	d += ".gam";

	// Load all the data
	LoadGameInfoFiles();
	loadedGirlsFiles.LoadXML(TiXmlHandle(0));
	LoadGirlsFiles();

	g_Girls.LoadDefaultImages();	// load the default girl images

	// load the global triggers
	g_GlobalTriggers.LoadList(
		DirPath() << "Resources" << "Scripts" << "GlobalTriggers.xml"
		);

	g_CurrBrothel = 0;

	g_Gold.reset();

	g_Year = 1209;
	g_Month = 1;
	g_Day = 1;

	selected_girl = 0;
	for (int i = 0; i<8; i++)
	{
		MarketSlaveGirls[i] = 0;
		MarketSlaveGirlsDel[i] = -1;
	}

	g_Brothels.NewBrothel(20);
	g_Brothels.SetName(0, g_ReturnText);
	for (int i = 0; i<NUM_STATS; i++)
		g_Brothels.GetPlayer()->m_Stats[i] = 60;
	for (u_int i = 0; i<NUM_SKILLS; i++)
		g_Brothels.GetPlayer()->m_Skills[i] = 10;
	g_Brothels.GetPlayer()->SetToZero();
	//g_Brothels.GetPlayer()->m_CustomerFear = 0;
	//g_Brothels.GetPlayer()->m_Disposition = 0;
	//g_Brothels.GetPlayer()->m_Suspicion = 0;
	//g_Brothels.GetPlayer()->m_WinGame = false;
	/*
	g_Studios.NewBrothel(20);
	g_Studios.SetName(0, "Studio");

	g_Arena.NewBrothel(20);
	g_Arena.SetName(0, "Arena");

	g_Centre.NewBrothel(20);
	g_Centre.SetName(0, "Centre");
	*/
	g_House.NewBrothel(20);
	g_House.SetName(0, "House");

	u_int start_random_gangs = cfg.gangs.start_random();
	u_int start_boosted_gangs = cfg.gangs.start_boosted();
	for (u_int i = 0; i<start_random_gangs; i++)
		g_Gangs.AddNewGang(false);
	for (u_int i = 0; i<start_boosted_gangs; i++)
		g_Gangs.AddNewGang(true);

	// update the shop inventory
	g_InvManager.UpdateShop();

	/*
	*	two strings speparated only by white space are concatenated
	*	by the compiler. Which means you can split up long text
	*	passages, making them easier to read:
	*/
#if 0
	stringstream ss;
	ss << gettext("Welcome to Crossgate, a city in the realm of Mundiga, ")
		<< gettext("where criminals rule and space and time overlap with ")
		<< gettext("other worlds and dimensions. Once a powerful crime lord ")
		<< gettext("in the city, your father was recently assassinated and ")
		<< gettext("his assets looted by rivals. All that remains is the ")
		<< gettext("fire-gutted shell of an old brothel that served as your ")
		<< gettext("father's headquarters.\n")
		<< gettext("However this building hides some interesting secrets. ")
		<< gettext("Still concealed in the cellars is the dungeon where ")
		<< gettext("your father conducted the less savory aspects of ")
		<< gettext("his business. Underneath, endless catacombs extend, ")
		<< gettext("constantly shifting in the dimensional flux, drawing ")
		<< gettext("in beings and plunder from a thousand different worlds.\n")
		<< gettext("Your job now is to return the brothel to its former ")
		<< gettext("glories of exotic women and carnal pleasures. That will ")
		<< gettext("give you the income you need to avenge your father, ")
		<< gettext("and resume your rightful place as his successor.\n")
		<< gettext("It is up to you if you will be as evil as your father ")
		<< gettext("or not, but in any case you start with very little gold ")
		<< gettext("and your first order of business should be to purchase ")
		<< gettext("some girls from the slave market, and to hire some goons ")
		<< gettext("to guard your headquarters.");

	g_MessageQue.AddToQue(
		ss.str(),
		0
		);
#endif
	// Add the begining rivals
	g_Brothels.GetRivalManager()->CreateRival(200, 5, 10000, 2, 0, 26, 2, 2);
	g_Brothels.GetRivalManager()->CreateRival(400, 10, 15000, 2, 1, 30, 2, 3);
	g_Brothels.GetRivalManager()->CreateRival(600, 15, 20000, 3, 1, 56, 3, 5);
	g_Brothels.GetRivalManager()->CreateRival(800, 20, 25000, 4, 2, 74, 4, 8);

	if (g_Cheats)
	{
		g_Gold.cheat();
		g_InvManager.GivePlayerAllItems();
		//g_Brothels.AddGirl(0, g_Girls.GetGirl(23));  // Adding girl to brothel (Ayanami Rei as it happens) for some reason?
	}

	g_WinManager.push("Brothel Management");

	DirPath text = DirPath()
		<< "Saves"
		<< (g_Brothels.GetBrothel(0)->m_Name + ".gam").c_str()
		;
	sm.Load(ScriptPath("Intro.lua"), 0);
	SaveGameXML(text);
}

void GetString()
{
	cGetStringScreenManager gssm;
	gssm.process();
}

static string clobber_extension(string s)	// `J` removed logging
{
	// g_LogFile.os() << "clobber_extension: s = " << s << endl;
	size_t pos = s.rfind(".");
	// g_LogFile.os() << "clobber_extension: pos = " << pos << endl;
	string base = s.substr(0, pos);
	// g_LogFile.os() << "clobber_extension: s = " << s << endl;
	// g_LogFile.os() << "clobber_extension: base = " << base << endl;
	return base;
}

/*
 * interim loader to load XML files, and then non-xml ones
 * if there was no xml version.
 */
static void LoadXMLItems(FileList &fl)
{
	map<string,string> lookup;

	g_LogFile.os() << "itemsx files:" << endl;
	fl.scan("*.itemsx");
	for(int i = 0; i < fl.size(); i++) {
		string str = fl[i].full();
		string key = clobber_extension(str);
		lookup[key] = str;
		g_LogFile.os() << "	adding " << str << endl;
		// g_LogFile.os() << "	under " << key << endl;
		// g_LogFile.os() << "	result " << lookup[key] << endl;
	}

	g_LogFile.os() << "items files:" << endl;
	fl.scan("*.items");
	for(int i = 0; i < fl.size(); i++) {
		string str = fl[i].full();
		string key = clobber_extension(str);
		// g_LogFile.os() << "	checking " << lookup[key] << endl;
		if(lookup[key] != "") {
			continue;
		}
		lookup[key] = str;
		g_LogFile.os() << "	adding " << str << endl;
		// g_LogFile.os() << "	under " << key << endl;
	}
/*
 *	Iterate over the map and print out all key/value pairs.
 *	kudos: wikipedia
 */
	// g_LogFile.os() << "walking map..." << endl;
	for(map<string,string>::const_iterator it = lookup.begin(); it != lookup.end(); ++it) {
		string full_path = it->second;
		// g_LogFile.os() <<	"\tkey = " << it->first << endl;
		// g_LogFile.os() <<	"\tpath = " << full_path << endl;
/*
 *		does it end in ".items" or ".itemsx"?
 */
		size_t len = full_path.length();
		char c = full_path.at(len-1);
		if(c == 'x') {
			// g_LogFile.os() << "\t\tloading xml" << endl;
			g_InvManager.LoadItemsXML(full_path);
		}
		else {
			// g_LogFile.os() << "\t\tloading orig" << endl;
			g_InvManager.LoadItems(full_path);
		}
	}
}

void LoadGameInfoFiles()
{
// Load the traits: first build the path
	DirPath location = DirPath() << "Resources" << "Data";
 	FileList fl(location, "*.traits");	// get a file list
	for (int i = 0; i < fl.size(); i++)	// loop over the list, loading the files
	{
		g_Traits.LoadTraits(fl[i].full());
	}
/* `J` Load .traitsx files (work in progress)
	FileList fl_t(location, "*.traitsx");	// get a file list
	cTraits::LoadXMLTraits(fl_t);
*/

// `J` Load .itemsx files
	DirPath location_i = DirPath() << "Resources" << "Items"; // `J` moved items from Data to Items folder
	FileList fl_i(location_i, "*.itemsx");
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
	static int ImageType = -1;
	static int lastNum = -1;
	static int ImageNum = -1;
	static int LastType = -1;
	static int category = 0;
	static int category_last = 0;
	static int Item = 0;

	sGirl *girl;
	g_CurrentScreen = SCREEN_TURNSUMMARY;
	if(g_InitWin)
	{
		g_Turnsummary.Focused();
		string brothel = gettext("Current Brothel: ");
		brothel += g_Brothels.GetName(g_CurrBrothel);
		g_Turnsummary.EditTextItem(brothel, g_interfaceid.TEXT_CURRENTBROTHEL);

		if(category_last == category)
			Item = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
		else
		{
			Item = 0;
			category_last = category;
		}

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
		//g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 4, "RIVALS");
		//g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSCATEGORY, 5, "GLOBAL"); */
		g_Turnsummary.SetSelectedItemInList(g_interfaceid.LIST_TSCATEGORY, category, false);

/*
 *		ITEM Listbox
 */
		// 0. List girls
		if(category == 0)
		{
			int nNumGirls	= g_Brothels.GetNumGirls(g_CurrBrothel);
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
						  sexjob = true;
						break;
					default:
						break;
				}
				// Sort the girls into 4 catagories... sex jobs, service jobs, warning, and danger
				// `J` added 5th catagory... goodnews
				// If we want to we could add a seperate catagory for each job and order the list even further, but it will make this clunkier.
				if (!pTmpGirl->m_Events.HasUrgent() && sexjob)
				{
					tmpSexGirls.push_back(pTmpGirl);
				}
				else if (!pTmpGirl->m_Events.HasUrgent())
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
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i])
					Item = ID;
				ID++;
			}
			//Girls with Danger events
			for (u_int i = 0; i < tmpDangerGirls.size(); i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
				if (selected_girl == tmpDangerGirls[i])
					Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for(u_int i = 0; i < tmpWarningGirls.size() ;i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_DARKBLUE);
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

/* `J` Old Dungeon code
		// 3. Dungeon
		else if(category == 3)
		{
			
			// Fill the list box
			cDungeon* pDungeon	= g_Brothels.GetDungeon();
			int nNumGirls		= pDungeon->GetNumGirls();
			int ID	= 0;

			// WD:	Copied sort to dungeon girls.  Added warnings to cDungeon::Update() to sort
			//  `J` Girls with GoodNews events first
			for (int h = 0; h<nNumGirls; h++)
			{
				sGirl* pTmpGirl = pDungeon->GetGirl(h)->m_Girl;
				if (pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
			// MYR: Girls with danger events first
			for(int i=0; i<nNumGirls; i++)
			{
				sGirl* pTmpGirl = pDungeon->GetGirl(i)->m_Girl;
				if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string tname = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
				
			// Girls with warning events next
			for(int j=0; j<nNumGirls; j++)
			{
				sGirl* pTmpGirl = pDungeon->GetGirl(j)->m_Girl;
				if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}

			// Then every other girl
			for(int k=0; k<nNumGirls; k++)
			{
				sGirl* pTmpGirl = pDungeon->GetGirl(k)->m_Girl;
				if(!pTmpGirl->m_Events.HasUrgent())
				{
					string temp = pTmpGirl->m_Realname;
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp);
					if (selected_girl == pTmpGirl)
						Item = ID;
					ID++;
				}
			}
		} // End of dungeon
//	*/ // End of old dungeon


		// 3. `J` New Dungeon code
		else if (category == 3)
		{

			// Fill the list box
			cDungeon* pDungeon = g_Brothels.GetDungeon();
			int nNumGirls = pDungeon->GetNumGirls();
			int ID = 0;
			vector<sGirl*> tmpGoodNewsGirls;
			vector<sGirl*> tmpDangerGirls;
			vector<sGirl*> tmpWarningGirls;
			vector<sGirl*> tmpOtherGirls;
			tmpGoodNewsGirls.clear();
			tmpDangerGirls.clear();
			tmpWarningGirls.clear();
			tmpOtherGirls.clear();
			sGirl* pTmpGirl;

			for (int i = 0; i < nNumGirls; i++)
			{
				pTmpGirl = pDungeon->GetGirl(i)->m_Girl;

				if (!pTmpGirl->m_Events.HasUrgent())
				{
					tmpOtherGirls.push_back(pTmpGirl);
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

			//Girls with GoodNews events
			for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
			{
				string tname = tmpGoodNewsGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i])
					Item = ID;
				ID++;
			}
			//Girls with Danger events
			for (u_int i = 0; i < tmpDangerGirls.size(); i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
				if (selected_girl == tmpDangerGirls[i])
					Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for (u_int i = 0; i < tmpWarningGirls.size(); i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_DARKBLUE);
				if (selected_girl == tmpWarningGirls[i])
					Item = ID;
				ID++;
			}
			//ServiceJob Girls
			for (u_int i = 0; i < tmpOtherGirls.size(); i++)
			{
				string tname = tmpOtherGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname);
				if (selected_girl == tmpOtherGirls[i])
					Item = ID;
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
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
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
				if (selected_girl == tmpGoodNewsGirls[i])
					Item = ID;
				ID++;
			}
			//Girls with Danger events
			for(u_int i = 0; i < tmpDangerGirls.size() ;i++)
			{
				string tname = tmpDangerGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
				if (selected_girl == tmpDangerGirls[i])
					Item = ID;
				ID++;
			}
			//Girls wih Warnings
			for(u_int i = 0; i < tmpWarningGirls.size() ;i++)
			{
				string tname = tmpWarningGirls[i]->m_Realname;
				g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_DARKBLUE);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_GREEN);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, tname, LISTBOX_RED);
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
					g_Turnsummary.AddToListBox(g_interfaceid.LIST_TSITEM, ID, temp, LISTBOX_DARKBLUE);
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
		if(category > 8)
			category = 0;
		g_InitWin = true;
	}
	else if(g_Q_Key)
	{
		g_Q_Key = false;
		category --;
		if(category < 0)
			category = 8;
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
	if(category != 0 && category != 3 && category != 4 && category != 5 && category != 6 && category != 7 && category != 8) {
		return;
	}
/*
 *	anythign hereafter is category zero - so we can lose the "if"
 */
	int GirlNum = g_Turnsummary.GetSelectedItemFromList(g_interfaceid.LIST_TSITEM);
/*
 *	again, no selected item means "nothing to do"
 *	so let's do that first :)
 */
	if(GirlNum == -1) {
		return;
	}

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

	if(g_Cheats)
	{
		g_Gold.cheat();
	}

	// Clear choice dialog
	g_ChoiceManager.Free();

	// update the shop inventory
	g_InvManager.UpdateShop();

	// Clear the interface events
	g_InterfaceEvents.ClearEvents();

	// go through and update all the brothels (this updates the girls each brothel has and calculates sex and stuff)
		if (g_Clinic.GetNumBrothels() > 0)
	{
		g_Clinic.UpdateClinic();
	}
		if (g_Studios.GetNumBrothels() > 0)
	{
		g_Studios.UpdateMovieStudio();
	}
		if (g_Arena.GetNumBrothels() > 0)
	{
		g_Arena.UpdateArena();
	}
		if (g_Centre.GetNumBrothels() > 0)
	{
		g_Centre.UpdateCentre();
	}

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
	g_Day+=7;
	if(g_Day > 30)
	{
		g_Day = g_Day-30;
		g_Month++;
		if(g_Month > 12)
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
			//g_Brothels.GetPlayer()->m_Suspicion++;
			//if(g_Brothels.GetPlayer()->m_Suspicion > 100)
			//	g_Brothels.GetPlayer()->m_Suspicion = 100;
		}

		g_Brothels.GetPlayer()->disposition(-1);
		//g_Brothels.GetPlayer()->m_Disposition--;
		//if(g_Brothels.GetPlayer()->m_Disposition < -100)
		//	g_Brothels.GetPlayer()->m_Disposition = -100;

		g_Brothels.UpdateAllGirlsStat(0, STAT_PCFEAR, 2);

		ClearGameFlag(FLAG_DUNGEONGIRLDIE);
	}
	else if(CheckGameFlag(FLAG_DUNGEONCUSTDIE))	// a customer has died in the dungeon
	{
		g_MessageQue.AddToQue(gettext("A customer has died in the dungeon.\nTheir body will be removed by the end of the week."), 1);
		
		if(g_Dice%100 < 10)	// only 10% chance of being found out
		{
			// WD: Use acessor methods 
			//g_Brothels.GetPlayer()->m_Suspicion++;
			//if(g_Brothels.GetPlayer()->m_Suspicion > 100)
			//	g_Brothels.GetPlayer()->m_Suspicion = 100;
			g_Brothels.GetPlayer()->suspicion(1);
			
		}

		// WD: Use acessor methods 
		//g_Brothels.GetPlayer()->m_Disposition--;
		//if(g_Brothels.GetPlayer()->m_Disposition < -100)
		//	g_Brothels.GetPlayer()->m_Disposition = -100;
		g_Brothels.GetPlayer()->disposition(-1);

		// WD: Use acessor methods 
		//g_Brothels.GetPlayer()->m_CustomerFear++;
		//if(g_Brothels.GetPlayer()->m_CustomerFear > 100)
		//	g_Brothels.GetPlayer()->m_CustomerFear = 100;
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
	if(g_InitWin)
	{
		if(girl == 0)
		{
			g_InitWin = true;
			g_MessageQue.AddToQue("ERROR: No girl selected", 1);
			g_WinManager.Pop();
			return;
		}
		g_Gallery.Focused();

		if(girl->m_GirlImages->m_Images[IMGTYPE_ANAL].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_BDSM].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBDSM, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBDSM, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYSEX, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYSEX, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_BEAST].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBEAST, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBEAST, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_GROUP].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYGROUP, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYGROUP, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_LESBIAN].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYLESBIAN, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYLESBIAN, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPREGNANT, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPREGNANT, false);
		if (girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYDEATH, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYDEATH, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPROFILE, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPROFILE, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_COMBAT].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYCOMBAT, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYCOMBAT, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_ORAL].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYORAL, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYORAL, false);

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
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYANAL))
		{
			Mode=0;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBDSM))
		{
			Mode=1;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSEX))
		{
			Mode=2;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBEAST))
		{
			Mode=3;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYGROUP))
		{
			Mode=4;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYLESBIAN))
		{
			Mode=5;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREGNANT))
		{
			Mode=6;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYDEATH))
		{
			Mode=7;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPROFILE))
		{
			Mode=8;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYCOMBAT))
		{
			Mode=9;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYORAL))
		{
			Mode=10;
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
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_NEXTGALLERY))
		{
			g_InitWin = true;
		g_WinManager.Push(Gallery2, &g_Gallery2);
			return;
		}
	}

	if(g_LeftArrow)
	{
		g_LeftArrow = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_RightArrow)
	{
		g_RightArrow = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		return;
	}
	if(g_A_Key)
	{
		g_A_Key = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_D_Key)
	{
		g_D_Key = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)	
			Img = 0;
		return;
	}
	if(g_W_Key)
	{
		while(1)
		{
			g_W_Key = false;
			Mode --;
			if(Mode < 0)
				Mode = 10;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_S_Key)
	{
		while(1)
		{
			g_S_Key = false;
			Mode ++;
			if(Mode > 10)
				Mode = 0;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
		if(g_UpArrow)
	{
		while(1)
		{
			g_UpArrow = false;
			Mode --;
			if(Mode < 0)
				Mode = 10;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_DownArrow)
	{
		while(1)
		{
			g_DownArrow = false;
			Mode ++;
			if(Mode > 10)
				Mode = 0;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	if (Mode == NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}
	// Set the text for gallery type
	string galtxt = "";
	switch(Mode)
	{
	case 0:
		galtxt = "Anal";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 1:
		galtxt = "BDSM";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 2:
		galtxt = "Sex";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 3:
		galtxt = "Beastiality";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 4:
		galtxt = "Group";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 5:
		galtxt = "Lesbian";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 6:
		galtxt = "Pregnant";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 7:
		galtxt = "Death";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 8:
		galtxt = "Profile";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 9:
		galtxt = "Combat";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 10:
		galtxt = "Oral";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	default:
		galtxt = "";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	}
	// Draw the image
	if(girl)
	{
		g_Gallery.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(girl, Mode, false, Img, true));
		if(g_Girls.IsAnimatedSurface(girl, Mode, Img))
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

		if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYECCHI, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYECCHI, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSTRIP, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSTRIP, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_MAID].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAID, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAID, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_SING].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSING, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYSING, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_WAIT].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYWAIT, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYWAIT, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_CARD].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYCARD, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYCARD, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_BUNNY].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYBUNNY, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYBUNNY, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYNUDE, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYNUDE, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_MAST].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAST, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYMAST, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_TITTY].m_NumImages == 0)
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYTITTY, true);
		else
			g_Gallery2.DisableButton(g_interfaceid.BUTTON_GALLERYTITTY, false);

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
			Mode=11;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSTRIP))
		{
			Mode=12;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYMAID))
		{
			Mode=13;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSING))
		{
			Mode=14;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYWAIT))
		{
			Mode=15;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYCARD))
		{
			Mode=16;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBUNNY))
		{
			Mode=17;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYNUDE))
		{
			Mode=18;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYMAST))
		{
			Mode=19;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYTITTY))
		{
			Mode=20;
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

	if(g_LeftArrow)
	{
		g_LeftArrow = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_RightArrow)
	{
		g_RightArrow = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		return;
	}
	if(g_A_Key)
	{
		g_A_Key = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_D_Key)
	{
		g_D_Key = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)	
			Img = 0;
		return;
	}
	if(g_W_Key)
	{
		while(1)
		{
			g_W_Key = false;
			Mode --;
			if(Mode < 11)
				Mode = 20;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_S_Key)
	{
		while(1)
		{
			g_S_Key = false;
			Mode ++;
			if(Mode > 20)
				Mode = 11;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
		if(g_UpArrow)
	{
		while(1)
		{
			g_UpArrow = false;
			Mode --;
			if(Mode < 11)
				Mode = 20;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_DownArrow)
	{
		while(1)
		{
			g_DownArrow = false;
			Mode ++;
			if(Mode > 20)
				Mode = 11;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	if (Mode == NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}
	// Set the text for gallery type
	string galtxt = "";
	switch(Mode)
	{
	case 11:
		galtxt = "Ecchi";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 12:
		galtxt = "Strip";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 13:
		galtxt = "Maid";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 14:
		galtxt = "Sing";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 15:
		galtxt = "Waitress";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 16:
		galtxt = "Card";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 17:
		galtxt = "Bunny";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 18:
		galtxt = "Nude";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 19:
		galtxt = "Masturbation";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 20:
		galtxt = "Titty Fuck";
		g_Gallery2.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	default:
		galtxt = "";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	}
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

	//this replaces the "master file"
	loadedGirlsFiles.SaveXML(pRoot);

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

	// output player gold
	g_Gold.saveGoldXML(pRoot);

	// output year, month and day
	pRoot->SetAttribute("Year", g_Year);
	pRoot->SetAttribute("Month", g_Month);
	pRoot->SetAttribute("Day", g_Day);

	// output girls
	g_Girls.SaveGirlsXML(pRoot);

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


	//load items database, traits info, etc
	LoadGameInfoFiles();

	DirPath thefile;
	thefile<<directory<<filename;
	TiXmlDocument doc(thefile.c_str());
	if (doc.LoadFile() == false)
	{
		g_LogFile.write("Tried and failed to parse savegame as XML file, error as follows");
		g_LogFile.write("(If it says 'Error document empty.' it's probably old format)");
		g_LogFile.write(doc.ErrorDesc());
		g_LogFile.write("Attempting to load savegame as old format");
		bool success = LoadGameLegacy(directory, filename);
		if (success == true)
		{
			g_LogFile.write("Successfully loaded from old savegame format");
		}
		else
		{
			g_LogFile.write("Unknown error, did not load any savegame format");
		}
		return success;
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

	if(minorA != 3)
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

bool LoadGameLegacy(string directory, string filename)
{
	// load templates
	g_LogFile.write("Loading what used to be the master file");
/*
 *	We need to load the .girls/.girlsx files
 *
 *	To do that, we need to know which we might have already loaded
 *	that information is kept in the master file
 *	so we load that first
 */
	loadedGirlsFiles.LoadLegacy(filename);
	LoadGirlsFiles();

	int major = 0;
	int minorA = 0;
	int minorB = 0;
	int temp = 0;

	ifstream ifs;
	DirPath thefile;
	thefile<<directory<<filename;
	ifs.open(thefile.c_str());

	// load the version
	ifs>>major>>minorA>>minorB;

	if(minorA != 3)
	{
		g_MessageQue.AddToQue(gettext("You must start a new game with this version"), 2);
		return false;
	}

	g_CurrBrothel = 0;

	// load interface variables
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	if(temp == 1)
		g_WalkAround = true;
	else
		g_WalkAround = false;
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>g_TalkCount;


	// load player gold
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	g_Gold.loadGoldLegacy(ifs);

	// load year, month and day
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>g_Year>>g_Month>>g_Day;

	// load main girls
	g_LogFile.write("Loading girls");
	g_Girls.LoadGirlsLegacy(ifs);

	// load gangs
	g_LogFile.write("Loading gang Data");
	g_Gangs.LoadGangsLegacy(ifs);

	// load brothels
	g_LogFile.write("Loading brothel Data");
	g_Brothels.LoadDataLegacy(ifs);

	// load clinic
	g_LogFile.write("Loading clinic Data");
	g_Clinic.LoadDataLegacy(ifs);

	// load studio
	g_LogFile.write("Loading clinic Data");
	g_Studios.LoadDataLegacy(ifs);

	// load arena
	g_LogFile.write("Loading arena Data");
	g_Arena.LoadDataLegacy(ifs);

	// load centre
	g_LogFile.write("Loading centre Data");
	g_Centre.LoadDataLegacy(ifs);

	// load house
	g_LogFile.write("Loading house Data");
	g_House.LoadDataLegacy(ifs);

	// load global triggers
	g_LogFile.write("Loading global triggers");
	g_GlobalTriggers.LoadTriggersLegacy(ifs);

	g_LogFile.write("Loading default images");
	g_Girls.LoadDefaultImages();

	ifs.close();
	
	if(g_Cheats)
	{
		g_WalkAround = false;
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
	if(g_InitWin)
	{
		leftBrothel = rightBrothel = 0;
		// clear list boxes
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS);
		g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS);

		// list all the brothels
		sBrothel* current = g_Brothels.GetBrothel(0);
		int i=5;
		while(current)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, i, current->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, i, current->m_Name);
			i++;
			current = current->m_Next;
		}

		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 5);
		g_TransferGirls.SetSelectedItemInList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 5);

		// add the movie studio studio
		sMovieStudio* currentStudio = (sMovieStudio*) g_Studios.GetBrothel(0);
		while(currentStudio)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 0, currentStudio->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 0, currentStudio->m_Name);
			currentStudio = (sMovieStudio*) currentStudio->m_Next;
		} 
		// add the clinic
		sClinic* currentClinic = (sClinic*) g_Clinic.GetBrothel(0);
		while(currentClinic)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 1, currentClinic->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 1, currentClinic->m_Name);
			currentClinic = (sClinic*) currentClinic->m_Next;
		}

		// add the arena
		sArena* currentArena = (sArena*) g_Arena.GetBrothel(0);
		while(currentArena)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 2, currentArena->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 2, currentArena->m_Name);
			currentArena = (sArena*) currentArena->m_Next;
		}

		// add the centre
		sCentre* currentCentre = (sCentre*) g_Centre.GetBrothel(0);
		while(currentCentre)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 3, currentCentre->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 3, currentCentre->m_Name);
			currentCentre = (sCentre*) currentCentre->m_Next;
		}

		// add the house
		sHouse* currentHouse = (sHouse*) g_House.GetBrothel(0);
		while(currentHouse)
		{
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 4, currentHouse->m_Name);
			g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 4, currentHouse->m_Name);
			currentHouse = (sHouse*) currentHouse->m_Next;
		}

		g_InitWin = false;
	}

	if(g_InterfaceEvents.GetNumEvents() != 0)
	{
		if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGBACK))
		{
			g_InitWin = true;
			g_WinManager.Pop();
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGSHIFTR))
		{
			if((rightBrothel != -1 && leftBrothel != -1))
			{
				TransferGirlsRightToLeft(rightBrothel, leftBrothel);
			}
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_TRANSGSHIFTL))
		{
			if((rightBrothel != -1 && leftBrothel != -1))
			{
				TransferGirlsLeftToRight(rightBrothel, leftBrothel);
			}
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TRANSGLEFTBROTHEL))
		{
			g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS);
			leftBrothel = g_TransferGirls.GetSelectedItemFromList(g_interfaceid.LIST_TRANSGLEFTBROTHEL);
			if(leftBrothel != -1)
			{
				if (leftBrothel > 4){
					// add the girls to the list
					sGirl* temp = g_Brothels.GetGirl(leftBrothel - 5, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 4)
				{
					// add the girls to the list
					sGirl* temp = g_House.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 2)
				{
					// add the girls to the list
					sGirl* temp = g_Arena.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 1)
				{
					// add the girls to the list
					sGirl* temp = g_Clinic.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (leftBrothel == 0)
				{
					// add the girls to the list
					sGirl* temp = g_Studios.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}

				}
			}
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, g_interfaceid.LIST_TRANSGRIGHTBROTHEL))
		{
			g_TransferGirls.ClearListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS);
			rightBrothel = g_TransferGirls.GetSelectedItemFromList(g_interfaceid.LIST_TRANSGRIGHTBROTHEL);
			if(rightBrothel != -1)
			{
				if (rightBrothel > 4){
					// add the girls to the list
					sGirl* temp = g_Brothels.GetGirl(rightBrothel - 5, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 4)
				{
					// add the girls to the list
					sGirl* temp = g_House.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 3)
				{
					// add the girls to the list
					sGirl* temp = g_Centre.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 2)
				{
					// add the girls to the list
					sGirl* temp = g_Arena.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 1)
				{
					// add the girls to the list
					sGirl* temp = g_Clinic.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
						i++;
						temp = temp->m_Next;
					}
				}
				else if (rightBrothel == 0)
				{
					// add the girls to the list
					sGirl* temp = g_Studios.GetGirl(0, 0);
					int i=0;
					while(temp)
					{
						g_TransferGirls.AddToListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, i, temp->m_Realname);
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
	if (leftBrothel > 4)
	{
		brothel = g_Brothels.GetBrothel(leftBrothel - 5);
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
			if (rightBrothel > 4)
			{
				// get the girl
				temp = g_Brothels.GetGirl(rightBrothel-5, girlSelection-NumRemoved);
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
			if (rightBrothel > 4)
			{
				g_Brothels.RemoveGirl(rightBrothel - 5, temp, false);
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
			if (leftBrothel > 4)
			{
				
				g_Brothels.AddGirl(leftBrothel - 5, temp);
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
	if (rightBrothel > 4)
	{
		brothel = g_Brothels.GetBrothel(rightBrothel - 5);
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
			if (leftBrothel > 4)
			{
				// get the girl
				temp = g_Brothels.GetGirl(leftBrothel-5, girlSelection-NumRemoved);
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
			if (leftBrothel > 4)
			{
				g_Brothels.RemoveGirl(leftBrothel - 5, temp, false);
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
			if (rightBrothel > 4)
			{
				
				g_Brothels.AddGirl(rightBrothel - 5, temp);
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

