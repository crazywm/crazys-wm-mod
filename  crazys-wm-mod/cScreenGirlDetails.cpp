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
#include <algorithm>
#include "cBrothel.h"
#include "cClinic.h"
#include "cCentre.h"
#include "cArena.h"
#include "cHouse.h"
#include "cFarm.h"
#include "cMovieStudio.h"
#include "cScreenGirlDetails.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "InterfaceProcesses.h"
#include "cGetStringScreenManager.h"
#include "cGangs.h"
#include "cScriptManager.h"

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cClinicManager g_Clinic;
extern cMovieStudioManager g_Studios;
extern cArenaManager g_Arena;
extern cCentreManager g_Centre;
extern cHouseManager g_House;
extern cFarmManager g_Farm;
extern cWindowManager g_WinManager;
extern long g_IntReturn;
extern bool g_Cheats;
extern	int	g_TalkCount;
extern bool g_AllTogle;
extern cGangManager g_Gangs;
extern	bool	eventrunning;
extern cTraits g_Traits;
extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;
extern cConfig cfg;
extern	int		g_CurrentScreen;

extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_EnterKey;
extern	bool	g_SpaceKey;
extern	bool	g_ShiftDown;
extern	bool	g_CTRLDown;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	bool	g_H_Key;
extern	bool	g_J_Key;

static cTariff tariff;
static stringstream ss;

static int ImageNum = -1;
static int DetailLevel = 0;
static bool Day0Night1 = SHIFT_DAY;
static bool SetJob = true;

bool cScreenGirlDetails::ids_set = false;
cScreenGirlDetails::cScreenGirlDetails()
{

	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "girl_details_screen.xml";
	m_filename = dp.c_str();
}
cScreenGirlDetails::~cScreenGirlDetails() {}

void cScreenGirlDetails::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenGirlDetails");

	back_id			/**/ = get_id("BackButton", "Back");
	girlname_id		/**/ = get_id("GirlName");
	girldesc_id		/**/ = get_id("GirlDescription");
	girlimage_id	/**/ = get_id("GirlImage");
	more_id			/**/ = get_id("MoreButton");
	antipreg_id		/**/ = get_id("UseAntiPregToggle");
	prev_id			/**/ = get_id("PrevButton","Prev");
	next_id			/**/ = get_id("NextButton","Next");
	inventory_id	/**/ = get_id("InventoryButton");
	senddungeon_id	/**/ = get_id("SendDungeonButton");
	reldungeon_id	/**/ = get_id("ReleaseDungeonButton");
	interact_id		/**/ = get_id("InteractButton");
	interactc_id	/**/ = get_id("InteractCount");
	takegold_id		/**/ = get_id("TakeGoldButton");
	accomup_id		/**/ = get_id("AccomUpButton");//
	accomdown_id	/**/ = get_id("AccomDownButton");//
	accom_id		/**/ = get_id("AccomSlider");
	accomval_id		/**/ = get_id("AccomValue");
	houseperc_id	/**/ = get_id("HousePercSlider");
	housepercval_id	/**/ = get_id("HousePercValue");
	gallery_id		/**/ = get_id("GalleryButton");
	jobtypehead_id	/**/ = get_id("JobTypeHeader");
	jobtypelist_id	/**/ = get_id("JobTypeList");
	jobhead_id		/**/ = get_id("JobHeader");
	joblist_id		/**/ = get_id("JobList");
	day_id			/**/ = get_id("DayButton");
	night_id		/**/ = get_id("NightButton");
	traithead_id	/**/ = get_id("TraitHeader");
	traitlist_id	/**/ = get_id("TraitList");
	traitdesc_id	/**/ = get_id("TraitDescription");
}

void cScreenGirlDetails::Free()
{
	cInterfaceWindow::Free();	//free up everything else
}

void cScreenGirlDetails::init()
{
	if (selected_girl == 0)
	{
		g_InitWin = true;
		g_LogFile.write("ERROR - girl details screen, selected_girl is null");
		/*
		*		adding this because the game will crash if we
		*		go past this point with a null girl
		*
		*		Now as to why it was null in the first place ...
		*		-- doc
		*/
		g_WinManager.Pop();
		return;
	}

	g_CurrentScreen = SCREEN_GIRLDETAILS;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	if (selected_girl->is_dead())
	{
		// `J` instead of removing dead girls from the list which breaks the game, just skip past her in the list.
		NextGirl();								// `J` currently this prevents scrolling backwards past her - need to fix that.
		//		selected_girl = remove_selected_girl();
		if (selected_girl == 0)
		{
			g_WinManager.Pop();
			g_InitWin = true;
			return;
		}
	}

	int job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
	SetJob = true;

	EditTextItem(selected_girl->m_Realname, girlname_id);

	string detail;
	if (DetailLevel == 0)		detail = g_Girls.GetDetailsString(selected_girl);
	else if (DetailLevel == 1)	detail = g_Girls.GetMoreDetailsString(selected_girl);
	else						detail = g_Girls.GetThirdDetailsString(selected_girl);
	EditTextItem(detail, girldesc_id);

	if (selected_girl)
	{
		if (lastsexact != -1)
		{
			PrepareImage(girlimage_id, selected_girl, lastsexact, true, ImageNum);
			lastsexact = -1;
		}
		else
		{
			if (selected_girl->m_newRandomFixed >= 0)
			{
				PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, false, selected_girl->m_newRandomFixed);
			}
			else
			{
				PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, true, ImageNum);
			}
		}
	}

	SliderRange(houseperc_id, 0, 100, selected_girl->house(), 10);
	ss.str("");	ss << "House Percentage: " << SliderValue(houseperc_id) << "%";
	EditTextItem(ss.str(), housepercval_id);

	ClearListBox(jobtypelist_id);

	// `J` Replacing accom buttons with slider
	if (accomdown_id != -1)	DisableButton(accomdown_id, selected_girl->m_AccLevel < 1);
	if (accomup_id != -1)	DisableButton(accomup_id, selected_girl->m_AccLevel > 9);
	if (accom_id != -1)
	{
		SliderRange(accom_id, 0, 9, selected_girl->m_AccLevel, 1);
		SliderMarker(accom_id, g_Girls.PreferredAccom(selected_girl));
	}
	if (accomval_id != -1)
	{
		ss.str(""); ss << "Accommodation: " << g_Girls.Accommodation(SliderValue(accom_id));
		if (cfg.debug.log_extradetails())
		{
			int val = SliderValue(accom_id) - g_Girls.PreferredAccom(selected_girl);
			if (val != 0) ss << "  ( " << (val > 0 ? "+" : "") << val << " )";
		}
		EditTextItem(ss.str(), accomval_id);
	}
	DisableButton(interact_id, (g_TalkCount <= 0));
	if (interactc_id >= 0)
	{
		ss.str(""); ss << "Interactions Left: ";
		if (g_Cheats) ss << "Infinate Cheat";
		else if (g_TalkCount <= 0) ss << "0 (buy in House screen)";
		else ss << g_TalkCount;
		EditTextItem(ss.str(), interactc_id);
	}
	DisableButton(takegold_id, (selected_girl->m_Money <= 0));
	DisableButton(accomup_id, (selected_girl->m_AccLevel >= 9));
	DisableButton(accomdown_id, (selected_girl->m_AccLevel <= 0));
	SetCheckBox(antipreg_id, (selected_girl->m_UseAntiPreg));

	bool InMovieStudio = (selected_girl->m_InStudio);
	bool InArena = (selected_girl->m_InArena);
	bool InCentre = (selected_girl->m_InCentre);
	bool InClinic = (selected_girl->m_InClinic);
	bool InHouse = (selected_girl->m_InHouse);
	bool InFarm = (selected_girl->m_InFarm);
	bool InDungeon = (selected_girl->m_DayJob == JOB_INDUNGEON);
	DisableButton(reldungeon_id, !InDungeon);
	DisableButton(senddungeon_id, InDungeon);

	// Disable dungeon if selected girl is in a non-brothel building
	//	if (InMovieStudio || InArena || InCentre || InClinic || InFarm || InHouse) 
	//	{
	//		DisableButton(senddungeon_id, true);
	//	}

	ClearListBox(joblist_id);
	bool HideDNButtons = false;

	if (InArena)
	{
		AddToListBox(jobtypelist_id, JOBFILTER_ARENASTAFF, g_Arena.m_JobManager.JobFilterName[JOBFILTER_ARENASTAFF]);
		AddToListBox(jobtypelist_id, JOBFILTER_ARENA, g_Arena.m_JobManager.JobFilterName[JOBFILTER_ARENA]);
		RefreshJobList();
		if (job >= g_Arena.m_JobManager.JobFilterIndex[JOBFILTER_ARENA] && job < g_Arena.m_JobManager.JobFilterIndex[JOBFILTER_ARENA + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_ARENA);
		else // if (job >= g_Arena.m_JobManager.JobFilterIndex[JOBFILTER_ARENASTAFF] && job < g_Arena.m_JobManager.JobFilterIndex[JOBFILTER_ARENASTAFF + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_ARENASTAFF);
	}
	else if (InClinic)
	{
		AddToListBox(jobtypelist_id, JOBFILTER_CLINIC, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINIC]);
		AddToListBox(jobtypelist_id, JOBFILTER_CLINICSTAFF, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINICSTAFF]);
		RefreshJobList();
		if (job >= g_Clinic.m_JobManager.JobFilterIndex[JOBFILTER_CLINIC] && job < g_Clinic.m_JobManager.JobFilterIndex[JOBFILTER_CLINIC + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINIC);
		else SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINICSTAFF);
	}
	else if (InCentre)
	{
		AddToListBox(jobtypelist_id, JOBFILTER_COMMUNITYCENTRE, g_Centre.m_JobManager.JobFilterName[JOBFILTER_COMMUNITYCENTRE]);
		AddToListBox(jobtypelist_id, JOBFILTER_COUNSELINGCENTRE, g_Centre.m_JobManager.JobFilterName[JOBFILTER_COUNSELINGCENTRE]);
		RefreshJobList();
		if (job >= g_Centre.m_JobManager.JobFilterIndex[JOBFILTER_COUNSELINGCENTRE] && job < g_Centre.m_JobManager.JobFilterIndex[JOBFILTER_COUNSELINGCENTRE + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_COUNSELINGCENTRE);
		else SetSelectedItemInList(jobtypelist_id, JOBFILTER_COMMUNITYCENTRE);
	}
	else if (InHouse)
	{
		AddToListBox(jobtypelist_id, JOBFILTER_HOUSE, g_House.m_JobManager.JobFilterName[JOBFILTER_HOUSE]);
		AddToListBox(jobtypelist_id, JOBFILTER_HOUSETTRAINING, g_House.m_JobManager.JobFilterName[JOBFILTER_HOUSETTRAINING]);
		RefreshJobList();
		if (job >= g_House.m_JobManager.JobFilterIndex[JOBFILTER_HOUSETTRAINING] && job < g_House.m_JobManager.JobFilterIndex[JOBFILTER_HOUSETTRAINING + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_HOUSETTRAINING);
		else SetSelectedItemInList(jobtypelist_id, JOBFILTER_HOUSE);
	}
	else if (InMovieStudio)
	{
		Day0Night1 = SHIFT_NIGHT;
		HideDNButtons = true;
		// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cScreenGirlDetails.cpp
		AddToListBox(jobtypelist_id, JOBFILTER_STUDIOCREW, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIOCREW]);
		AddToListBox(jobtypelist_id, JOBFILTER_STUDIONONSEX, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIONONSEX]);
		AddToListBox(jobtypelist_id, JOBFILTER_STUDIOSOFTCORE, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIOSOFTCORE]);
		AddToListBox(jobtypelist_id, JOBFILTER_STUDIOPORN, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIOPORN]);
		AddToListBox(jobtypelist_id, JOBFILTER_STUDIOHARDCORE, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIOHARDCORE]);
		AddToListBox(jobtypelist_id, JOBFILTER_RANDSTUDIO, g_Studios.m_JobManager.JobFilterName[JOBFILTER_RANDSTUDIO]);
		RefreshJobList();
		/* */if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIONONSEX] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIONONSEX + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIONONSEX);
		else if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOSOFTCORE] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOSOFTCORE + 1])	SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOSOFTCORE);
		else if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOPORN] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOPORN + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOPORN);
		else if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOHARDCORE] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_STUDIOHARDCORE + 1])	SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOHARDCORE);
		else if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_RANDSTUDIO] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_RANDSTUDIO + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_RANDSTUDIO);
		else SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOCREW);
	}
	else if (InFarm)
	{
		AddToListBox(jobtypelist_id, JOBFILTER_FARMSTAFF, g_Farm.m_JobManager.JobFilterName[JOBFILTER_FARMSTAFF]);
		AddToListBox(jobtypelist_id, JOBFILTER_LABORERS, g_Farm.m_JobManager.JobFilterName[JOBFILTER_LABORERS]);
		AddToListBox(jobtypelist_id, JOBFILTER_PRODUCERS, g_Farm.m_JobManager.JobFilterName[JOBFILTER_PRODUCERS]);
		RefreshJobList();
		if (job >= g_Farm.m_JobManager.JobFilterIndex[JOBFILTER_LABORERS] && job < g_Farm.m_JobManager.JobFilterIndex[JOBFILTER_LABORERS + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_LABORERS);
		else if (job >= g_Farm.m_JobManager.JobFilterIndex[JOBFILTER_PRODUCERS] && job < g_Farm.m_JobManager.JobFilterIndex[JOBFILTER_PRODUCERS + 1])
			SetSelectedItemInList(jobtypelist_id, JOBFILTER_PRODUCERS);
		else SetSelectedItemInList(jobtypelist_id, JOBFILTER_FARMSTAFF);
	}
	else if (!InDungeon)
	{  // if not in dungeon, set up job lists
		// add the job filters
		//	for(int i=0; i<NUMJOBTYPES; i++)  // loop through all job types
		for (unsigned int i = 0; i <= JOBFILTER_BROTHEL; i++)  // temporary limit to job types shown
		{
			AddToListBox(jobtypelist_id, i, g_Brothels.m_JobManager.JobFilterName[i]);
		}

		// set the job filter
		int jobtype = 0;
		for (unsigned int i = 0; i < NUMJOBTYPES; i++)
		{
			if (job >= g_Brothels.m_JobManager.JobFilterIndex[i] && job < g_Brothels.m_JobManager.JobFilterIndex[i + 1])
			{
				jobtype = i;
				break;
			}
		}
		SetSelectedItemInList(jobtypelist_id, jobtype);
		RefreshJobList();
	}
	else
	{  // if in dungeon, effectively disable job lists
		HideDNButtons = true;
		ClearListBox(joblist_id);
		AddToListBox(jobtypelist_id, 0, "Languishing in dungeon");
		SetSelectedItemInList(jobtypelist_id, 0, false);
	}

	HideButton(day_id, HideDNButtons);
	HideButton(night_id, HideDNButtons);
	if (!HideDNButtons)
	{
		DisableButton(day_id, (Day0Night1 == SHIFT_DAY));
		DisableButton(night_id, (Day0Night1 == SHIFT_NIGHT));
	}

	ClearListBox(traitlist_id);
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (selected_girl->m_Traits[i])
		{
			ss.str(""); ss << selected_girl->m_Traits[i]->display_name();
			if (selected_girl->m_TempTrait[i] > 0) ss << "   (" << selected_girl->m_TempTrait[i] << ")";
			AddToListBox(traitlist_id, i, ss.str());
		}
	}
	EditTextItem("", traitdesc_id);
}

void cScreenGirlDetails::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

bool cScreenGirlDetails::check_keys()
{
	if (g_LeftArrow || g_RightArrow || (g_AltKeys && (g_A_Key || g_D_Key)))
	{
		if (g_LeftArrow || g_A_Key) PrevGirl(); else NextGirl();
		g_LeftArrow = g_RightArrow = g_A_Key = g_D_Key = false;
		return true;
	}
	if (g_AltKeys)
	{
		if (g_H_Key || g_J_Key)
		{
			int mod = (g_H_Key ? -1 : 1);
			selected_girl->house(mod);
			g_H_Key = g_J_Key = false;
			SliderValue(houseperc_id, selected_girl->house());
			ss.str(""); ss << "House Percentage: " << selected_girl->house() << "%";
			EditTextItem(ss.str(), housepercval_id);
			// Rebelliousness might have changed, so update details
			if (DetailLevel == 0)
			{
				string detail = g_Girls.GetDetailsString(selected_girl);
				EditTextItem(detail, girldesc_id);
			}
			return true;
		}
		if (g_S_Key)
		{
			g_S_Key = false;
			if (g_ShiftDown)
			{
				DetailLevel = 2;
				EditTextItem(g_Girls.GetThirdDetailsString(selected_girl), girldesc_id);
			}
			else
			{
				if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(g_Girls.GetMoreDetailsString(selected_girl), girldesc_id); }
				else						{ DetailLevel = 0; EditTextItem(g_Girls.GetDetailsString(selected_girl), girldesc_id); }
			}
			return true;
		}
		if (g_SpaceKey)
		{
			g_SpaceKey = false;
			g_WinManager.push("Gallery");
			g_InitWin = true;
			return true;
		}
	}
	return false;
}

void cScreenGirlDetails::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;		// no events means we can go home
	if (g_InterfaceEvents.CheckButton(back_id))				// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckSlider(houseperc_id))
	{
		g_Girls.SetStat(selected_girl, STAT_HOUSE, SliderValue(houseperc_id));
		ss.str(""); ss << "House Percentage: " << SliderValue(houseperc_id) << "%";
		EditTextItem(ss.str(), housepercval_id);
		if (DetailLevel == 0)								// Rebelliousness might have changed, so update details
		{
			string detail = g_Girls.GetDetailsString(selected_girl);
			EditTextItem(detail, girldesc_id);
		}
		return;
	}
	if (g_InterfaceEvents.CheckButton(more_id))
	{
		if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(g_Girls.GetMoreDetailsString(selected_girl), girldesc_id); }
		else if (DetailLevel == 1)	{ DetailLevel = 2; EditTextItem(g_Girls.GetThirdDetailsString(selected_girl), girldesc_id); }
		else						{ DetailLevel = 0; EditTextItem(g_Girls.GetDetailsString(selected_girl), girldesc_id); }
	}
	if (g_InterfaceEvents.CheckButton(day_id))
	{
		Day0Night1 = SHIFT_DAY;
		DisableButton(day_id, true);
		DisableButton(night_id, false);
		g_InitWin = true;
	}
	if (g_InterfaceEvents.CheckButton(night_id))
	{
		Day0Night1 = SHIFT_NIGHT;
		DisableButton(day_id, false);
		DisableButton(night_id, true);
		g_InitWin = true;
	}
	if (g_InterfaceEvents.CheckCheckbox(antipreg_id))
	{
		selected_girl->m_UseAntiPreg = (IsCheckboxOn(antipreg_id));
	}
	if (g_InterfaceEvents.CheckListbox(traitlist_id))
	{
		int selection = GetLastSelectedItemFromList(traitlist_id);
		EditTextItem((selection != -1 ? selected_girl->m_Traits[selection]->desc() : ""), traitdesc_id);
	}
	if (g_InterfaceEvents.CheckListbox(jobtypelist_id))
	{
		SetJob = true;
		RefreshJobList();
	}
	if (g_InterfaceEvents.CheckListbox(joblist_id))
	{
		bool fulltime = g_CTRLDown;
		int selection = GetSelectedItemFromList(joblist_id);
		if (selection != -1)
		{
			// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> 
			int old_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
			// handle special job requirements and assign - if HandleSpecialJobs returns true, the job assignment was modified or cancelled
			if (g_Brothels.m_JobManager.HandleSpecialJobs(g_CurrBrothel, selected_girl, selection, old_job, Day0Night1, fulltime))
			{
				selection = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
				SetSelectedItemInList(joblist_id, selection, false);
			}
			// refresh job worker counts for former job and current job
			if (old_job != selection)
			{
				SetSelectedItemText(joblist_id, old_job, g_Brothels.m_JobManager.JobDescriptionCount(old_job, g_CurrBrothel, Day0Night1));
				SetSelectedItemText(joblist_id, selection, g_Brothels.m_JobManager.JobDescriptionCount(selection, g_CurrBrothel, Day0Night1));
			}
			RefreshJobList();
		}
	}
	if (g_InterfaceEvents.CheckButton(inventory_id))
	{
		if (selected_girl)
		{
			if (selected_girl->is_dead()) return;
			g_InitWin = true;
			g_AllTogle = true;
			g_WinManager.push("Item Management");
			return;
		}
	}
	if (g_InterfaceEvents.CheckButton(gallery_id))
	{
		g_WinManager.push("Gallery");
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckSlider(accom_id))
	{
		selected_girl->m_AccLevel = SliderValue(accom_id);
		SliderRange(accom_id, 0, 9, selected_girl->m_AccLevel, 1);
		if (accomval_id != -1)
		{
			ss.str(""); ss << "Accommodation: " << g_Girls.Accommodation(SliderValue(accom_id));
			if (cfg.debug.log_extradetails())
			{
				int val = SliderValue(accom_id) - g_Girls.PreferredAccom(selected_girl);
				if (val != 0) ss << "  ( " << (val > 0 ? "+" : "") << val << " )";
			}
			EditTextItem(ss.str(), accomval_id);
		}
		g_InitWin = true;
		return;
	}
	int accadj = 0;
	if (g_InterfaceEvents.CheckButton(accomup_id)) accadj = 1;
	if (g_InterfaceEvents.CheckButton(accomdown_id)) accadj = -1;
	if (accadj != 0)
	{
		selected_girl->m_AccLevel += accadj;
		if (selected_girl->m_AccLevel > 9)	selected_girl->m_AccLevel = 9;
		else if (selected_girl->m_AccLevel < 0)	selected_girl->m_AccLevel = 0;
		if (accomval_id != -1) EditTextItem("Accommodation: " + g_Girls.Accommodation(selected_girl->m_AccLevel), accomval_id);
		g_InitWin = true;
		accadj = 0;
		return;
	}
	if (g_InterfaceEvents.CheckButton(takegold_id))
	{
		take_gold(selected_girl);
	}
	if (g_InterfaceEvents.CheckButton(reldungeon_id))
	{
		if (!selected_girl) return;
		g_Brothels.GetDungeon()->GetDungeonPos(selected_girl);
		if ((g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms - g_Brothels.GetBrothel(g_CurrBrothel)->m_NumGirls) == 0)
		{
			g_MessageQue.AddToQue("The current brothel has no more room.\nBuy a new one, get rid of some girls, or change the brothel you are currently managing.", 0);
		}
		else
		{
			sGirl* nextGirl = remove_selected_girl();
			sGirl* tempGirl = g_Brothels.GetDungeon()->RemoveGirl(g_Brothels.GetDungeon()->GetGirl(g_Brothels.GetDungeon()->GetGirlPos(selected_girl)));
			g_Brothels.AddGirl(g_CurrBrothel, tempGirl);

			if (g_Brothels.GetDungeon()->GetNumGirls() == 0)
			{
				selected_girl = 0;
				g_WinManager.Pop();
			}
			else selected_girl = nextGirl;
		}
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(senddungeon_id))
	{
		if (!selected_girl) return;
		ss.str("");
		g_Brothels.GetGirlPos(g_CurrBrothel, selected_girl);

		// does she decide to fight back
		if (g_Brothels.FightsBack(selected_girl))
		{
			bool win = true;
			sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
			int count = 8;
			while (gang && win && count >= 0)
			{
				win = (g_Gangs.GangCombat(selected_girl, gang));
				if (gang->m_Num == 0) gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
				count--;
				if (count<0) win = true;
			}
			// Calculate combat between goons and girl if she decides to fight back
			if (win)
			{
				ss << "She puts up a fight";
				if (gang && gang->m_Num == 0) ss << ", and the gang is completely wiped out";
				ss << ". ";

				if (g_Brothels.PlayerCombat(selected_girl))				// fight with the player
				{
					// If girl wins she escapes and leaves the brothel
					ss << "After defeating you as well, she escapes to the outside.\n";
					ss << "She will escape for good in 6 weeks if you don't send someone after her.";

					sGirl* nextGirl = remove_selected_girl();
					sGirl* temp = selected_girl;
					if (selected_girl->m_DayJob == JOB_INDUNGEON)	temp = g_Brothels.GetDungeon()->RemoveGirl(selected_girl);
					else if (selected_girl->m_InHouse)	g_House.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InFarm)	g_Farm.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InClinic)	g_Clinic.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InCentre)	g_Centre.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InArena)	g_Arena.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InStudio)	g_Studios.RemoveGirl(0, selected_girl, false);
					else g_Brothels.RemoveGirl(selected_girl->where_is_she, selected_girl, false);

					temp->m_RunAway = 6;
					temp->m_NightJob = temp->m_DayJob = JOB_RUNAWAY;
					g_Brothels.AddGirlToRunaways(temp);

					stringstream smess;
					smess << temp->m_Realname << " has run away";
					g_MessageQue.AddToQue(smess.str(), 1);

					selected_girl = nextGirl;
					if (selected_girl == 0) g_WinManager.Pop();
				}
				else	// otherwise put her in the dungeon
				{
					int reason = (selected_girl->m_Spotted ? DUNGEON_GIRLSTEAL : DUNGEON_GIRLWHIM);
					sGirl* nextGirl = remove_selected_girl();
					selected_girl->m_DayJob = selected_girl->m_NightJob = JOB_INDUNGEON;

					/* */if (selected_girl->m_InHouse)	g_House.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InFarm)	g_Farm.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InClinic)	g_Clinic.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InCentre)	g_Centre.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InArena)	g_Arena.RemoveGirl(0, selected_girl, false);
					else if (selected_girl->m_InStudio)	g_Studios.RemoveGirl(0, selected_girl, false);
					else g_Brothels.RemoveGirl(selected_girl->where_is_she, selected_girl, false);

					g_Brothels.GetDungeon()->AddGirl(selected_girl, reason);
					ss << "However, you manage to defeat her yourself and place her unconscious body in the dungeon.";

					bool pop = false;
					/* */if (selected_girl->m_InHouse)	if (g_House.GetNumGirls(0) == 0)	pop = true;
					else if (selected_girl->m_InFarm)	if (g_Farm.GetNumGirls(0) == 0)		pop = true;
					else if (selected_girl->m_InClinic)	if (g_Clinic.GetNumGirls(0) == 0)	pop = true;
					else if (selected_girl->m_InCentre)	if (g_Centre.GetNumGirls(0) == 0)	pop = true;
					else if (selected_girl->m_InArena)	if (g_Arena.GetNumGirls(0) == 0)	pop = true;
					else if (selected_girl->m_InStudio)	if (g_Studios.GetNumGirls(0) == 0)	pop = true;
					else if (g_Brothels.GetNumGirls(selected_girl->where_is_she) == 0)		pop = true;

					if (pop)	{ selected_girl = 0; g_WinManager.Pop(); }
					else		selected_girl = nextGirl;
				}
			}
			else	// otherwise put her in the dungeon
			{
				ss << "She puts up a fight ";
				if (gang && gang->m_Num == 0)	ss << "and the gang is wiped out, ";

				ss << "but your goons manage to drag her unconscious to the dungeon.";
				int reason = (selected_girl->m_Spotted ? DUNGEON_GIRLSTEAL : DUNGEON_GIRLWHIM);
				sGirl* nextGirl = remove_selected_girl();

				/* */if (selected_girl->m_InHouse)	g_House.RemoveGirl(0, selected_girl, false);
				else if (selected_girl->m_InFarm)	g_Farm.RemoveGirl(0, selected_girl, false);
				else if (selected_girl->m_InClinic)	g_Clinic.RemoveGirl(0, selected_girl, false);
				else if (selected_girl->m_InCentre)	g_Centre.RemoveGirl(0, selected_girl, false);
				else if (selected_girl->m_InArena)	g_Arena.RemoveGirl(0, selected_girl, false);
				else if (selected_girl->m_InStudio)	g_Studios.RemoveGirl(0, selected_girl, false);
				else g_Brothels.RemoveGirl(selected_girl->where_is_she, selected_girl, false);
				g_Brothels.GetDungeon()->AddGirl(selected_girl, reason);

				bool pop = false;
				/* */if (selected_girl->m_InHouse)	if (g_House.GetNumGirls(0) == 0)	pop = true;
				else if (selected_girl->m_InFarm)	if (g_Farm.GetNumGirls(0) == 0)		pop = true;
				else if (selected_girl->m_InClinic)	if (g_Clinic.GetNumGirls(0) == 0)	pop = true;
				else if (selected_girl->m_InCentre)	if (g_Centre.GetNumGirls(0) == 0)	pop = true;
				else if (selected_girl->m_InArena)	if (g_Arena.GetNumGirls(0) == 0)	pop = true;
				else if (selected_girl->m_InStudio)	if (g_Studios.GetNumGirls(0) == 0)	pop = true;
				else if (g_Brothels.GetNumGirls(selected_girl->where_is_she) == 0)		pop = true;

				if (pop)	{ selected_girl = 0; g_WinManager.Pop(); }
				else		selected_girl = nextGirl;
			}
		}
		else
		{
			int reason = (selected_girl->m_Spotted ? DUNGEON_GIRLSTEAL : DUNGEON_GIRLWHIM);
			sGirl* nextGirl = remove_selected_girl();

			/* */if (selected_girl->m_InHouse)	g_House.RemoveGirl(0, selected_girl, false);
			else if (selected_girl->m_InFarm)	g_Farm.RemoveGirl(0, selected_girl, false);
			else if (selected_girl->m_InClinic)	g_Clinic.RemoveGirl(0, selected_girl, false);
			else if (selected_girl->m_InCentre)	g_Centre.RemoveGirl(0, selected_girl, false);
			else if (selected_girl->m_InArena)	g_Arena.RemoveGirl(0, selected_girl, false);
			else if (selected_girl->m_InStudio)	g_Studios.RemoveGirl(0, selected_girl, false);
			else g_Brothels.RemoveGirl(selected_girl->where_is_she, selected_girl, false);

			g_Brothels.GetDungeon()->AddGirl(selected_girl, reason);
			ss << "She goes quietly with a sullen look on her face.";

			bool pop = false;
			/* */if (selected_girl->m_InHouse)	if (g_House.GetNumGirls(0) == 0)	pop = true;
			else if (selected_girl->m_InFarm)	if (g_Farm.GetNumGirls(0) == 0)		pop = true;
			else if (selected_girl->m_InClinic)	if (g_Clinic.GetNumGirls(0) == 0)	pop = true;
			else if (selected_girl->m_InCentre)	if (g_Centre.GetNumGirls(0) == 0)	pop = true;
			else if (selected_girl->m_InArena)	if (g_Arena.GetNumGirls(0) == 0)	pop = true;
			else if (selected_girl->m_InStudio)	if (g_Studios.GetNumGirls(0) == 0)	pop = true;
			else if (g_Brothels.GetNumGirls(g_CurrBrothel) == 0) /*              */	pop = true;

			if (pop)	{ selected_girl = 0; g_WinManager.Pop(); }
			else		selected_girl = nextGirl;
		}
		g_InitWin = true;
		g_MessageQue.AddToQue(ss.str(), 0);
		return;
	}
	if (g_InterfaceEvents.CheckButton(interact_id))
	{
		if (!selected_girl) return;
		if (g_TalkCount > 0)
		{
			DirPath dp;
			eventrunning = true;
			if (selected_girl->m_DayJob != JOB_INDUNGEON)
			{
				int v[2] = { 1, -1 };
				cTrigger* trig = 0;
				if (!(trig = selected_girl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))	// trigger any girl specific talk script
				{
					// no, so trigger the default one
					dp = dp << "Resources" << "Scripts" << "DefaultInteractDetails.script";
				}
				else
				{
					// yes, so use that instead
					dp = DirPath(cfg.folders.characters().c_str()) << selected_girl->m_Name << trig->m_Script;
				}
			}
			else
			{
				int v[2] = { 0, -1 };
				cTrigger* trig = 0;
				if (!(trig = selected_girl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))	// trigger any girl specific talk script
				{
					// no, so trigger the default one
					dp = dp << "Resources" << "Scripts" << "DefaultInteractDungeon.script";
				}
				else
				{
					dp = DirPath(cfg.folders.characters().c_str()) << selected_girl->m_Name << trig->m_Script;
				}
			}
			cScriptManager script_manager;
			script_manager.Load(dp, selected_girl);
			if (!g_Cheats)
				g_TalkCount--;
		}
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(next_id))
	{
		NextGirl();
	}
	if (g_InterfaceEvents.CheckButton(prev_id))
	{
		PrevGirl();
	}
}

void cScreenGirlDetails::RefreshJobList()
{
	ClearListBox(joblist_id);
	int job_filter = GetSelectedItemFromList(jobtypelist_id);
	if (job_filter == -1) return;

	string text = "";
	// populate Jobs listbox with jobs in the selected category
	for (int i = g_Brothels.m_JobManager.JobFilterIndex[job_filter]; i<g_Brothels.m_JobManager.JobFilterIndex[job_filter + 1]; i++)
	{
		if (g_Brothels.m_JobManager.JobName[i] == "") continue;
		text = g_Brothels.m_JobManager.JobDescriptionCount(i, g_CurrBrothel, Day0Night1, selected_girl->m_InClinic, selected_girl->m_InStudio, selected_girl->m_InArena, selected_girl->m_InCentre, selected_girl->m_InHouse, selected_girl->m_InFarm);
		AddToListBox(joblist_id, i, text);
	}
	if (selected_girl)
	{
		int sel_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
		SetSelectedItemInList(joblist_id, sel_job, false);
	}
}

void cScreenGirlDetails::PrevGirl()
{
	selected_girl = get_prev_girl();
	g_InitWin = true;
}

void cScreenGirlDetails::NextGirl()
{
	selected_girl = get_next_girl();
	g_InitWin = true;
}

sGirl *cScreenGirlDetails::get_prev_girl()		// return previous girl in the sorted list
{
	sGirl *prev_girl = 0;
	g_LogFile.write("Where is the girl??");

	if (g_Clinic.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Clinic");
		prev_girl = g_Clinic.GetGirl(0, g_Clinic.GetGirlPos(0, selected_girl) - 1);
	}
	else if (g_Studios.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Studio");
		prev_girl = g_Studios.GetGirl(0, g_Studios.GetGirlPos(0, selected_girl) - 1);
	}
	else if (g_Arena.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Arena");
		prev_girl = g_Arena.GetGirl(0, g_Arena.GetGirlPos(0, selected_girl) - 1);
	}
	else if (g_Centre.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Centre");
		prev_girl = g_Centre.GetGirl(0, g_Centre.GetGirlPos(0, selected_girl) - 1);
	}
	else if (g_House.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the House");
		prev_girl = g_House.GetGirl(0, g_House.GetGirlPos(0, selected_girl) - 1);
	}
	else if (g_Farm.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Farm");
		prev_girl = g_Farm.GetGirl(0, g_Farm.GetGirlPos(0, selected_girl) - 1);
	}
	else
	{
		if (selected_girl->m_DayJob == JOB_INDUNGEON)
		{
			g_LogFile.write("She is in the Dungeon");
			prev_girl = g_Brothels.GetDungeon()->GetGirl(g_Brothels.GetDungeon()->GetGirlPos(selected_girl) - 1)->m_Girl;
		}
		else
		{
			g_LogFile.write("She is in a Brothel");
			prev_girl = g_Brothels.GetGirl(g_CurrBrothel, g_Brothels.GetGirlPos(g_CurrBrothel, selected_girl) - 1);
		}
	}
	return prev_girl;
}

sGirl *cScreenGirlDetails::get_next_girl()		// return next girl in the sorted list
{
	sGirl *next_girl = 0;
	g_LogFile.write("Where is the girl??");

	if (g_Clinic.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Clinic");
		next_girl = g_Clinic.GetGirl(0, g_Clinic.GetGirlPos(0, selected_girl) + 1);
	}
	else if (g_Studios.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Studio");
		next_girl = g_Studios.GetGirl(0, g_Studios.GetGirlPos(0, selected_girl) + 1);
	}
	else if (g_Arena.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Arena");
		next_girl = g_Arena.GetGirl(0, g_Arena.GetGirlPos(0, selected_girl) + 1);
	}
	else if (g_Centre.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Centre");
		next_girl = g_Centre.GetGirl(0, g_Centre.GetGirlPos(0, selected_girl) + 1);
	}
	else if (g_House.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the House");
		next_girl = g_House.GetGirl(0, g_House.GetGirlPos(0, selected_girl) + 1);
	}
	else if (g_Farm.GetGirlsCurrentBrothel(selected_girl) != -1)
	{
		g_LogFile.write("She is in the Farm");
		next_girl = g_Farm.GetGirl(0, g_Farm.GetGirlPos(0, selected_girl) + 1);
	}
	else
	{
		if (selected_girl->m_DayJob == JOB_INDUNGEON)
		{
			g_LogFile.write("She is in the Dungeon");
			next_girl = g_Brothels.GetDungeon()->GetGirl(g_Brothels.GetDungeon()->GetGirlPos(selected_girl) + 1)->m_Girl;
		}
		else
		{
			g_LogFile.write("She is in a Brothel");
			next_girl = g_Brothels.GetGirl(g_CurrBrothel, g_Brothels.GetGirlPos(g_CurrBrothel, selected_girl) + 1);
		}
	}

	return next_girl;
}

sGirl *cScreenGirlDetails::remove_selected_girl()		// the selected girl is to be removed from the current list; returns next selected girl
{
	sGirl *next_girl = 0;
	if (cycle_girls.size() == 0) return 0;
	int cur_id			= cycle_girls[cycle_pos];
	cycle_girls.erase(cycle_girls.begin() + cycle_pos);  // remove her
	if (cycle_pos >= (int)cycle_girls.size())
	{												
		if (cycle_pos > 0) cycle_pos--;					// if this girl was the last in the list, move list position -1 if possible
		else return next_girl;							// or, maybe we have no more girls in the list
	}

	if (selected_girl->m_DayJob == JOB_INDUNGEON) next_girl = g_Brothels.GetDungeon()->GetGirl(cycle_girls[cycle_pos])->m_Girl;
	else if (selected_girl->m_InHouse)	next_girl = g_House.GetGirl(0, cycle_girls[cycle_pos]);
	else if (selected_girl->m_InFarm)	next_girl = g_Farm.GetGirl(0, cycle_girls[cycle_pos]);
	else if (selected_girl->m_InClinic)	next_girl = g_Clinic.GetGirl(0, cycle_girls[cycle_pos]);
	else if (selected_girl->m_InCentre)	next_girl = g_Centre.GetGirl(0, cycle_girls[cycle_pos]);
	else if (selected_girl->m_InArena)	next_girl = g_Arena.GetGirl(0, cycle_girls[cycle_pos]);
	else if (selected_girl->m_InStudio)	next_girl = g_Studios.GetGirl(0, cycle_girls[cycle_pos]);
	else next_girl = g_Brothels.GetGirl(g_CurrBrothel, cycle_girls[cycle_pos]);

	for (int i = cycle_girls.size(); i--> 0;)
	{
		if (cycle_girls[i] > cur_id) cycle_girls[i]--;	// all girls with ID higher than removed girl need their ID reduced
	}
	return next_girl;
}

bool cScreenGirlDetails::do_take_gold(sGirl *girl, string &message)	// returns TRUE if the girl won
{
	const int GIRL_LOSES = false;
	const int GIRL_WINS = true;
	bool girl_win_flag = GIRL_WINS;
	/*
	*	we're taking the girl's gold. Life gets complicated if she
	*	makes a fight of it - so lets do the case where she meekly complies
	*	first
	*/
	if (!g_Brothels.FightsBack(girl))
	{
		message += "She quietly allows you to take her gold.";
		return GIRL_LOSES;	// no fight -> girl lose
	}
	/*
	*	OK - she's going to fight -
	*/
	sGang* gang;
	/*
	*	ok: to win she needs to defeat all the gangs on
	*	guard duty. I've made a change here so that she doesn't
	*	need to wipe one gang out before moving on to the next one
	*	which means that she can cause some damage on the way out
	*	without necessarily slaying all who stand before her
	*
	*	it also means that if you have 5 gangs guarding, she needs
	*	to get past 5 gangs, but you don't have to have them all die
	*	in the process
	*/
	while ((gang = g_Gangs.GetGangOnMission(MISS_GUARDING)))
	{
		/*
		*		this returns true if the girl wins, false if she loses
		*
		*		Suggestion on the forums that we allow clever girls to
		*		outwit stupid gang memebers here, which sounds cool.
		*		Also nice would be if a strongly magical girl could
		*		use sorcery to evade a none-too-crafty goon squad.
		*		(possibly make her fight the first one).
		*
		*		But none of this makes much difference if the user
		*		never sees it happen. We can make combat as textured as
		*		we like, but unless the details are reported to the player
		*		we might as well roll a single die and be done with it.
		*/
		girl_win_flag = g_Gangs.GangCombat(girl, gang);
		/*
		*		if she didn't win, exit the loop
		*/
		if (girl_win_flag == GIRL_LOSES) break;
	}
	/*
	*	the "girl lost" case is easier
	*/
	if (girl_win_flag == GIRL_LOSES)
	{		// put her in the dungeon
		message += "She puts up a fight ";
		if (gang && gang->m_Num == 0)
		{
			message += "and the gang is wiped out, ";
		}
		message += " but you take her gold anyway.";
		return girl_win_flag;
	}
	/*
	*	from here on down, the girl won against the goons
	*/
	message += "She puts up a fight ";
	if (gang && gang->m_Num == 0)	message += " and the gang is wiped out ";
	/*
	*	can the player tame this particular shrew?
	*/
	if (!g_Brothels.PlayerCombat(girl))	// fight with the player
	{
		message += "but you defeat her yourself and take her gold.";
		return false;	// girl did not win, after all
	}
	/*
	*	Looks like she won: put her out of the brothel
	*	and post her as a runaway
	*/
	message += "after defeating you as well she escapes to the outside.\n";

	sGirl* nextGirl = remove_selected_girl();
	sGirl* temp = girl;
	/*
	*	what we have to do depends on whether she was in brothel
	*	or dungeon
	*/
	if (girl->m_DayJob != JOB_INDUNGEON)
		g_Brothels.RemoveGirl(g_CurrBrothel, girl, false);
	else
		temp = g_Brothels.GetDungeon()->RemoveGirl(girl);
	/*
	*	set her job
	*/
	temp->m_RunAway = 6;	// player has 6 weeks to retreive
	temp->m_NightJob = girl->m_DayJob = JOB_RUNAWAY;
	/*
	*	add her to the runaway list
	*/
	g_Brothels.AddGirlToRunaways(temp);

	stringstream smess;
	smess << temp->m_Realname << " has run away";
	g_MessageQue.AddToQue(smess.str(), 1);

	selected_girl = nextGirl;
	g_InitWin = true;

	if (selected_girl == 0) g_WinManager.Pop();

	return true;	// the girl still won
}

void cScreenGirlDetails::take_gold(sGirl *girl)
{
	string message;
	bool girl_win = do_take_gold(girl, message);
	/*
	*	if the girl won, then we're pretty much sorted
	*	display the message and return
	*/
	if (girl_win)
	{
		g_MessageQue.AddToQue(message, 0);
		g_InitWin = true;
		return;
	}
	/*
	*	so the girl lost: take away her money now
	*/
	g_Gold.misc_credit(girl->m_Money);
	girl->m_Money = 0;
	/*
	*	she isn't going to like this much
	*	but it's not so bad for slaves
	*/
	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		girl->confidence(-1);
		girl->obedience(5);
		girl->spirit(-2);
		girl->pchate(5);
		girl->pclove(-5);
		girl->pcfear(5);
		girl->happiness(-20);
	}
	else
	{
		girl->confidence(-5);
		girl->obedience(5);
		girl->spirit(-10);
		girl->pchate(30);
		girl->pclove(-30);
		girl->pcfear(10);
		girl->happiness(-50);
	}
	/*
	*	and queue the message again
	*/
	g_MessageQue.AddToQue(message, 0);
	g_InitWin = true;
	return;
}
