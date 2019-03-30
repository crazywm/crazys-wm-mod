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
#include "cScriptManager.h"
#include "Revision.h"
#include "FileList.h"
#include "MasterFile.h"
#include "DirPath.h"
#include "src/buildings/cBrothel.h"
#include "cObjectiveManager.hpp"
#include "src/Game.hpp"
#include "cGangs.h"
#include "cInventory.h"

#undef bool

#ifdef LINUX
#include "linux.h"
#else
#include <windows.h>
#endif
#undef GetMessage

extern bool eventrunning;
extern cRng g_Dice;

extern bool g_UpArrow;		extern bool g_DownArrow;
extern bool g_EnterKey;		extern bool g_AltKeys;

extern bool g_O_Key;;
extern bool g_S_Key;
extern bool g_W_Key;
extern	bool	g_CTRLDown;

#pragma endregion
#pragma region //	Local Variables			//

// globals used for the interface
string g_ReturnText;
int g_ReturnInt = -1;
bool g_AllTogle = false;	// used on screens when wishing to apply something to all items

// for keeping track of weather have walked around town today
bool g_WalkAround = false;
bool g_TryCentre = false;
bool g_TryOuts = false;
bool g_TryEr = false;
bool g_TryCast = false;

int g_TalkCount = 10;
bool g_Cheats = false;


sGirl* g_selected_girl;  // global pointer for the currently selected girl
vector<int> cycle_girls;  // globally available sorted list of girl IDs for Girl Details screen to cycle through
int cycle_pos;  //currently selected girl's position in the cycle_girls vector
int summarysortorder = 0;	// the order girls get sorted in the summary lists

#pragma endregion

//used to store what files we have loaded
MasterFile loadedGirlsFiles;

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

// interim loader to load XML files, and then non-xml ones if there was no xml version.
void LoadXMLItems(FileList &fl)
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
	for (auto & it : lookup)
	{
		string full_path = it.second;
		if (loglevel > 1)	g_LogFile.os() << "\tkey = " << it.first << endl;
		if (loglevel > 1)	g_LogFile.os() << "\tpath = " << full_path << endl;
		if (loglevel > 0)	g_LogFile.os() << "\t\tLoading xml Item: " << full_path<< endl;
		g_Game.inventory_manager().LoadItemsXML(full_path);
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
void LoadGirlsFiles(MasterFile& master)
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
		if (master.exists(girlfiles[i].leaf()))
		{
			continue;
		}
		/*
		*		add the file to the master list
		*/
		master.add(girlfiles[i].leaf());
		/*
		*		load the file
		*/
		g_Game.girl_pool().LoadGirlsDecider(girlfiles[i].full());
	}
	/*
	*	Load random girls
	*
	*	"girlfiles" is still an XMLFileList, so this will get
	*	XML format files in preference to original format ones
	*/
	for (int i = 0; i < rgirlfiles.size(); i++)
	{
        g_Game.girl_pool().LoadRandomGirl(rgirlfiles[i].full());
	}
}

void NextWeek()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug NextWeek || Start"; g_LogFile.ssend(); }

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
	g_TalkCount = 10 + (g_Game.player().m_Stats[STAT_CHARISMA] / 10);
	// */ //

	if (g_Cheats)	g_Game.gold().cheat();

	// Clear choice dialog
	g_ChoiceManager.Free();

	// update the shop inventory
	g_Game.inventory_manager().UpdateShop();

	g_Game.next_week();

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
	if (g_Game.global_triggers().GetNextQueItem() && !eventrunning)
	{
        g_Game.global_triggers().ProcessNextQueItem(dp);
		eventrunning = true;
	}
	else if (!eventrunning)	// check girl scripts
	{
		if (g_Game.buildings().CheckScripts())
			eventrunning = true;
	}

	if (eventrunning)
	{
		sm.RunScript();
	}

	if (CheckGameFlag(FLAG_DUNGEONGIRLDIE))	// a girl has died int the dungeon
	{
		g_Game.push_message("A girl has died in the dungeon.\nHer body will be removed by the end of the week.", 1);

		if (g_Dice.percent(10))	// only 10% of being discovered
		{
            g_Game.player().suspicion(1);
		}
        g_Game.player().disposition(-1);
        for(auto& broth : g_Game.buildings().buildings()) {
            broth->update_all_girls_stat(STAT_PCFEAR, 2);
        }

		ClearGameFlag(FLAG_DUNGEONGIRLDIE);
	}
	else if (CheckGameFlag(FLAG_DUNGEONCUSTDIE))	// a customer has died in the dungeon
	{
		g_Game.push_message("A customer has died in the dungeon.\nTheir body will be removed by the end of the week.", 1);

		if (g_Dice.percent(10))	// only 10% chance of being found out
		{
            g_Game.player().suspicion(1);
		}
        g_Game.player().disposition(-1);
        g_Game.player().customerfear(1);

		ClearGameFlag(FLAG_DUNGEONCUSTDIE);
	}
}

void AutoSaveGame()
{
	SaveGameXML(DirPath(cfg.folders.saves().c_str()) << "autosave.gam");
}
void SaveGame(bool saveCSV)
{

	string filename = g_Game.buildings().get_building(0).name();
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

	//this replaces the "master file"
	// TODO Fix the master file stuff
	loadedGirlsFiles.SaveXML(pRoot);

	g_Game.save(*pRoot);
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
	g_Game.inventory_manager().UpdateShop();
#endif
}

static void SaveCSVHelper(IBuilding& building, ofstream& GirlsCSV) {
    for (auto& cgirl : building.girls())
    {
        GirlsCSV << "'" << building.name() << "'," << Girl2CSV(cgirl) << '\n';
    }
}

void SaveGirlsCSV(string filename)
{
	ofstream GirlsCSV;
	string eol = "\n";
	GirlsCSV.open(filename);
	GirlsCSV << "'Building','Girl Name','Based on','Slave?','Day Job','Night Job','Age','Level','Exp','Askprice','House','Fame','Tiredness','Health','Happiness','Constitution','Charisma','Beauty','Intelligence','Confidence','Agility','Obedience','Spirit','Morality','Refinement','Dignity','Mana','Libido','Lactation','PCFear','PCLove','PCHate','Magic','Combat','Service','Medicine','Performance','Crafting','Herbalism','Farming','Brewing','Animalhandling','Normalsex','Anal','Bdsm','Beastiality','Group','Lesbian','Strip','Oralsex','Tittysex','Handjob','Footjob'" << eol;
    for (auto& building : g_Game.buildings().buildings()) {
        SaveCSVHelper(*building, GirlsCSV);
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
