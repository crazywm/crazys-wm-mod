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
#include "cScreenClinicManagement.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "InterfaceProcesses.h"
#include "cScreenGirlDetails.h"
#include "libintl.h"

extern cScreenGirlDetails	g_GirlDetails;
extern int					g_CurrClinic;
extern cClinicManager		g_Clinic;
extern cBrothelManager		g_Brothels;
extern bool					g_InitWin;
extern cWindowManager		g_WinManager;
extern cGold				g_Gold;
extern sGirl*				selected_girl;
extern vector<int>			cycle_girls;
extern int					cycle_pos;
extern cPlayer*				The_Player;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_EnterKey;
extern	bool	g_SpaceKey;
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
extern	int		g_CurrentScreen;

static cTariff tariff;
static stringstream ss;

static int lastNum = -1;
static int ImageNum = -1;
static int FFSD_Flag = -1;
static int selection = -1;
static bool Day0Night1 = SHIFT_DAY;	// 1 is night, 0 is day.
static bool SetJob = false;

bool cScreenClinicManagement::ids_set = false;
cScreenClinicManagement::cScreenClinicManagement()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "clinic_management_screen.xml";
	m_filename = dp.c_str();
}
cScreenClinicManagement::~cScreenClinicManagement() {}

void cScreenClinicManagement::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenClinicManagement");

	back_id			/**/ = get_id("BackButton", "Back");
	curclinic_id	/**/ = get_id("Clinic");
	gold_id			/**/ = get_id("Gold");
	girllist_id		/**/ = get_id("GirlList");
	girlimage_id	/**/ = get_id("GirlImage");
	girldesc_id		/**/ = get_id("GirlDescription");
	viewdetails_id	/**/ = get_id("ViewDetailsButton");
	transfer_id		/**/ = get_id("TransferButton");
	firegirl_id		/**/ = get_id("FireButton");
	freeslave_id	/**/ = get_id("FreeSlaveButton");
	sellslave_id	/**/ = get_id("SellSlaveButton");
	jobtypehead_id	/**/ = get_id("JobTypeHeader");
	jobtypelist_id	/**/ = get_id("JobTypeList");
	jobtypedesc_id	/**/ = get_id("JobTypeDescription");
	jobhead_id		/**/ = get_id("JobHeader");
	joblist_id		/**/ = get_id("JobList");
	jobdesc_id		/**/ = get_id("JobDescription");
	day_id			/**/ = get_id("DayButton");
	night_id		/**/ = get_id("NightButton");

	//Set the default sort order for columns, so listbox knows the order in which data will be sent
	SortColumns(girllist_id, m_ListBoxes[girllist_id]->m_ColumnName, m_ListBoxes[girllist_id]->m_ColumnCount);
}

void cScreenClinicManagement::init()
{
	if (FFSD_Flag >= 0)
	{
		vector<int> girl_array;
		GetSelectedGirls(&girl_array);
		g_Brothels.m_JobManager.ffsd_outcome(girl_array, "Cl", 0);
		girl_array.clear();
		g_ChoiceManager.Free();
		FFSD_Flag = -1;
	}

	g_CurrentScreen = SCREEN_CLINIC;
	if (!g_InitWin) return;
	g_InitWin = false;
	Focused();
	selection = GetSelectedItemFromList(girllist_id);

	EditTextItem(g_Clinic.GetName(g_CurrClinic), curclinic_id);

	if (gold_id >= 0)
	{
		ss.str(""); ss << "Gold: " << g_Gold.ival();
		EditTextItem(ss.str(), gold_id);
	}

	// clear the lists
	ClearListBox(girllist_id);
	ClearListBox(jobtypelist_id);

	// add the job filters
	AddToListBox(jobtypelist_id, JOBFILTER_CLINICSTAFF, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINICSTAFF]);
	AddToListBox(jobtypelist_id, JOBFILTER_CLINIC, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINIC]);
	SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINICSTAFF);

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[girllist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();
	string* Data = new string[numColumns];

	// Add girls to list
	for (int i = 0; i < g_Clinic.GetNumGirls(g_CurrClinic); i++)
	{
		sGirl* gir = g_Clinic.GetGirl(g_CurrClinic, i);
		if (selected_girl == gir) selection = i;
		unsigned int item_color = (gir->health() <= 30 || gir->tiredness() >= 80 || gir->happiness() <= 30) ? COLOR_RED : COLOR_BLUE;
		gir->OutputGirlRow(Data, columnNames);
		AddToListBox(girllist_id, i, Data, numColumns, item_color);
	}
	delete[] Data;

	DisableButton(firegirl_id, true);
	DisableButton(freeslave_id, true);
	DisableButton(sellslave_id, true);
	DisableButton(viewdetails_id, true);

	lastNum = -1;

	if (selection >= 0) while (selection > GetListBoxSize(girllist_id) && selection != -1) selection--;
	SetSelectedItemInList(girllist_id, selection >= 0 ? selection : 0);

	DisableButton(day_id, (Day0Night1 == SHIFT_DAY));
	DisableButton(night_id, (Day0Night1 == SHIFT_NIGHT));

	update_image();
}

void cScreenClinicManagement::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

bool cScreenClinicManagement::check_keys()
{
	if (g_UpArrow || (g_AltKeys && g_A_Key))	{ selection = ArrowUpListBox(girllist_id);		g_UpArrow = g_A_Key = false;		return true; }
	if (g_DownArrow || (g_AltKeys && g_D_Key))	{ selection = ArrowDownListBox(girllist_id);	g_DownArrow = g_D_Key = false;		return true; }
	if (g_SpaceKey || g_EnterKey)	{ g_GirlDetails.lastsexact = -1;	ViewSelectedGirl();		g_SpaceKey = g_EnterKey = false;	return true; }
	// Select Location
	if (g_W_Key)	{ selection = ArrowUpListBox(jobtypelist_id);	g_W_Key = false;	return true; }
	if (g_S_Key)	{ selection = ArrowDownListBox(jobtypelist_id);	g_S_Key = false;	return true; }
	// Toggle Day/Night shift
	if (g_Z_Key)	{ Day0Night1 = SHIFT_DAY;	DisableButton(day_id, true);	DisableButton(night_id, false);	RefreshSelectedJobType();	g_Z_Key = false;	return true; }
	if (g_C_Key)	{ Day0Night1 = SHIFT_NIGHT;	DisableButton(day_id, false);	DisableButton(night_id, true);	RefreshSelectedJobType();	g_C_Key = false;	return true; }
	if (g_Q_Key || g_E_Key)
	{
		if (g_Q_Key)	selection = ArrowUpListBox(joblist_id);
		if (g_E_Key)	selection = ArrowDownListBox(joblist_id);

		bool skip = false;
		if (selected_girl && selected_girl->m_States&(1 << STATUS_SLAVE) && (selection == JOB_CHAIRMAN || selection == JOB_DOCTOR))
			skip = true;
		if (selection == JOB_CHAIRMAN && (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 0) > 0 || g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 1) > 0))
			skip = true;
		if (selected_girl->has_trait("AIDS") && (selection == JOB_DOCTOR || selection == JOB_NURSE || selection == JOB_INTERN))
			skip = true;
		if (skip)
		{
			if (g_Q_Key)	selection = ArrowUpListBox(joblist_id);
			if (g_E_Key)	selection = ArrowDownListBox(joblist_id);
			// the purpose of this is to clear the extra event from the event queue, which prevents an error --PP
			bool tmp = g_InterfaceEvents.CheckListbox(joblist_id);
		}
		g_Q_Key = g_E_Key = false;
		return true;
	}
	return false;
}

void cScreenClinicManagement::update_image()
{
	if (selected_girl)	// Draw a girls profile picture and description when selected
	{
		bool Rand = false;
		if (lastNum != selection)
		{
			stringstream text;
			text << g_Girls.GetGirlMood(selected_girl) << "\n \n" << selected_girl->m_Desc;
			if (cfg.debug.log_extradetails()) text << "\n \nBased on: " << selected_girl->m_Name;
			EditTextItem(text.str(), girldesc_id);
			Rand = true;
			lastNum = selection;
		}
		PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, Rand, lastNum);
		HideImage(girlimage_id, false);
	}
	else
	{
		selection = lastNum = -1;
		EditTextItem("No Girl Selected", girldesc_id);
		HideImage(girlimage_id, true);
	}
}

void cScreenClinicManagement::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home
	if (g_InterfaceEvents.CheckButton(back_id))	{ g_InitWin = true;		g_WinManager.Pop();		return; }
	//if (g_InterfaceEvents.CheckButton(prev_id))	{ g_CurrBrothel--;	if (g_CurrBrothel < 0) g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;	g_InitWin = true; }
	//if (g_InterfaceEvents.CheckButton(next_id))	{ g_CurrBrothel++;	if (g_CurrBrothel >= g_Brothels.GetNumBrothels())	g_CurrBrothel = 0;	g_InitWin = true; }
	if (g_InterfaceEvents.CheckButton(viewdetails_id))	{ g_GirlDetails.lastsexact = -1;	ViewSelectedGirl(); }
	if (g_InterfaceEvents.CheckButton(day_id))	{ Day0Night1 = SHIFT_DAY;	DisableButton(day_id, true);	DisableButton(night_id, false);	RefreshSelectedJobType(); }
	if (g_InterfaceEvents.CheckButton(night_id)){ Day0Night1 = SHIFT_NIGHT;	DisableButton(day_id, false);	DisableButton(night_id, true);	RefreshSelectedJobType(); }
	if (g_InterfaceEvents.CheckListbox(jobtypelist_id))
	{
		selection = GetSelectedItemFromList(jobtypelist_id);
		if (selection == -1) EditTextItem("Nothing Selected", jobtypedesc_id);
		else
		{
			RefreshJobList();	// populate Jobs listbox with jobs in the selected category
			stringstream jdmessage; jdmessage << g_Clinic.m_JobManager.JobFilterDesc[selection];
			if (g_Clinic.DoctorNeeded())
				jdmessage << "\n*** A Doctor is required to perform any surgeries. ";
			if ((g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_MECHANIC, SHIFT_DAY) < 1 && g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_GETREPAIRS, SHIFT_DAY) > 0) || (g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_MECHANIC, SHIFT_NIGHT) < 1 && g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_GETREPAIRS, SHIFT_NIGHT) > 0))
				jdmessage << "\n**** A Mechanic is required to perform any Repairs. ";
			EditTextItem(jdmessage.str(), jobtypedesc_id);
		}
	}
	if (g_InterfaceEvents.CheckListbox(joblist_id))
	{
		bool fulltime = g_CTRLDown;
		selection = GetSelectedItemFromList(joblist_id);
		if (selection != -1)
		{
			EditTextItem(g_Clinic.m_JobManager.JobDesc[selection], jobdesc_id);		// first handle the descriptions
			int pos = 0;
			int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);		// Now assign the job to all the selected girls
			while (GSelection != -1)
			{
				// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
				int new_job = selection;
				selected_girl = g_Clinic.GetGirl(g_CurrClinic, GSelection);
				if (selected_girl)
				{
					// handle special job requirements and assign
					int old_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
					// if HandleSpecialJobs returns true, the job assignment was modified or cancelled
					if (g_Clinic.m_JobManager.HandleSpecialJobs(g_CurrClinic, selected_girl, new_job, old_job, Day0Night1, fulltime))
					{
						new_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
						SetSelectedItemInList(joblist_id, new_job, false);
					}
					// update the girl's listing to reflect the job change
					ss.str("");	ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob];
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
					ss.str("");	ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob];
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());

					// refresh job worker counts for former job and current job
					SetSelectedItemText(joblist_id, old_job, g_Clinic.m_JobManager.JobDescriptionCount(old_job, 0, Day0Night1, true));
					SetSelectedItemText(joblist_id, new_job, g_Clinic.m_JobManager.JobDescriptionCount(new_job, 0, Day0Night1, true));
				}
				bool interrupted = false;	// `J` added
				if (selected_girl->m_YesterDayJob != selected_girl->m_DayJob && g_Clinic.is_Surgery_Job(selected_girl->m_YesterDayJob) && ((selected_girl->m_WorkingDay > 0) || selected_girl->m_PrevWorkingDay > 0))
					interrupted = true;

				if (selected_girl->m_DayJob == JOB_CUREDISEASES)	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " (" << selected_girl->m_WorkingDay << "%)*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (selected_girl->m_DayJob == JOB_GETABORT)	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " (" << 2 - selected_girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (g_Clinic.is_Surgery_Job(selected_girl->m_DayJob))	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " (" << 5 - selected_girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (interrupted)
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}

				if (selected_girl->m_NightJob == JOB_CUREDISEASES)	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " (" << selected_girl->m_WorkingDay << "%)*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (selected_girl->m_NightJob == JOB_GETABORT)	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " (" << 2 - selected_girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				else if (g_Clinic.is_Surgery_Job(selected_girl->m_NightJob))	// `J` added
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " (" << 5 - selected_girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				else if (interrupted)
				{
					ss.str(""); ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				if (interrupted)
				{	// `J` added
					ss.str(""); ss << g_Clinic.m_JobManager.JobDesc[new_job] << "\n** This girl was getting ";
					if (selected_girl->m_YesterDayJob == JOB_CUREDISEASES)
					{
						ss << "her disease cured, if you send her somewhere else, she will have to start her treatment over.";
					}
					else
					{
						if (selected_girl->m_YesterDayJob == JOB_BOOBJOB || selected_girl->m_YesterDayJob == JOB_FACELIFT)		ss << "a ";
						else if (selected_girl->m_YesterDayJob == JOB_GETABORT || selected_girl->m_YesterDayJob == JOB_ASSJOB)	ss << "an ";
						else if (selected_girl->m_YesterDayJob == JOB_TUBESTIED)/*                                          */	ss << "her ";
						ss << g_Clinic.m_JobManager.JobName[selected_girl->m_YesterDayJob] << ", if you send her somewhere else, she will have to start her Surgery over.";
					}
					EditTextItem(ss.str(), jobdesc_id);
				}
				GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
			}
		}
		else EditTextItem("Nothing Selected", jobdesc_id);
	}
	if (g_InterfaceEvents.CheckListbox(girllist_id))
	{
		selection = GetSelectedItemFromList(girllist_id);
		if (selection != -1)
		{
			selected_girl = g_Clinic.GetGirl(g_CurrClinic, selection);
			if (ListDoubleClicked(girllist_id))		// If double-clicked, try to bring up girl details
			{
				g_GirlDetails.lastsexact = -1;
				ViewSelectedGirl();
			}
			if (IsMultiSelected(girllist_id))
			{
				bool freefound = false;
				bool slavefound = false;
				int pos = 0;
				int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
				while (GSelection != -1)
				{
					if (g_Clinic.GetGirl(0, pos)->is_slave()) slavefound = true;
					if (!g_Clinic.GetGirl(0, pos)->is_slave()) freefound = true;
					GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
				}
				DisableButton(firegirl_id, !freefound);
				DisableButton(freeslave_id, !slavefound);
				DisableButton(sellslave_id, !slavefound);
			}
			else
			{
				DisableButton(firegirl_id, selected_girl->is_slave());
				DisableButton(freeslave_id, selected_girl->is_free());
				DisableButton(sellslave_id, selected_girl->is_free());
			}
			DisableButton(viewdetails_id, false);
			RefreshSelectedJobType();
		}
		else
		{
			DisableButton(firegirl_id, true);
			DisableButton(freeslave_id, true);
			DisableButton(sellslave_id, true);
			DisableButton(viewdetails_id, true);
			selected_girl = 0;
			selection = -1;
		}
		lastNum = -2;
		update_image();
		return;
	}
	if (g_InterfaceEvents.CheckButton(transfer_id))
	{
		g_InitWin = true;
		g_WinManager.push("Transfer Screen");
		return;
	}
	/* */if (g_InterfaceEvents.CheckButton(firegirl_id))	FFSD_Flag = FFSD_fire;
	else if (g_InterfaceEvents.CheckButton(freeslave_id))	FFSD_Flag = FFSD_free;
	else if (g_InterfaceEvents.CheckButton(sellslave_id))	FFSD_Flag = FFSD_sell;

	if (FFSD_Flag > 0)
	{
		int num = 0;

		if (selected_girl)
		{
			vector<int> girl_array;
			GetSelectedGirls(&girl_array);
			g_Clinic.m_JobManager.ffsd_choice(FFSD_Flag, girl_array, "Cl", 0);
			girl_array.clear();
		}
		return;
	}
}

void cScreenClinicManagement::RefreshSelectedJobType()
{
	selection = GetSelectedItemFromList(girllist_id);
	if (selection < 0) return;
	selected_girl = g_Clinic.GetGirl(g_CurrClinic, selection);
	int job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
	// set the job filter
	if (job >= g_Clinic.m_JobManager.JobFilterIndex[JOBFILTER_CLINIC] && job < g_Clinic.m_JobManager.JobFilterIndex[JOBFILTER_CLINIC + 1])
		SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINIC);
	else SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINICSTAFF);
	SetJob = true;
}

void cScreenClinicManagement::RefreshJobList()
{
	ClearListBox(joblist_id);
	int job_filter = GetSelectedItemFromList(jobtypelist_id);
	if (job_filter == -1) return;
	// populate Jobs listbox with jobs in the selected category
	for (int i = g_Clinic.m_JobManager.JobFilterIndex[job_filter]; i < g_Clinic.m_JobManager.JobFilterIndex[job_filter + 1]; i++)
	{
		if (g_Clinic.m_JobManager.JobName[i] == "") continue;
		AddToListBox(joblist_id, i, g_Clinic.m_JobManager.JobDescriptionCount(i, g_CurrClinic, Day0Night1, true));
	}
	if (selected_girl && g_Clinic.is_Surgery_Job(selected_girl->m_YesterDayJob) && selected_girl->m_YesterDayJob != selected_girl->m_DayJob && (selected_girl->m_WorkingDay > 0 || selected_girl->m_PrevWorkingDay > 0))
	{
		int sel_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
		SetSelectedItemInList(joblist_id, sel_job, false);

		ss.str(""); ss << g_Clinic.m_JobManager.JobDesc[sel_job] << "\n** This girl was getting ";
		if (selected_girl->m_YesterDayJob == JOB_CUREDISEASES)
		{
			ss << "her disease cured, if you send her somewhere else, she will have to start her treatment over.";
		}
		else
		{
			if (selected_girl->m_YesterDayJob == JOB_BOOBJOB || selected_girl->m_YesterDayJob == JOB_FACELIFT)		ss << "a ";
			else if (selected_girl->m_YesterDayJob == JOB_GETABORT || selected_girl->m_YesterDayJob == JOB_ASSJOB)	ss << "an ";
			else if (selected_girl->m_YesterDayJob == JOB_TUBESTIED)/*                                          */	ss << "her ";
			ss << g_Clinic.m_JobManager.JobName[selected_girl->m_YesterDayJob] << ", if you send her somewhere else, she will have to start her Surgery over.";
		}
		EditTextItem(ss.str(), jobdesc_id);
	}
	else if (selected_girl)
	{
		int sel_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
		SetSelectedItemInList(joblist_id, sel_job, false);
		EditTextItem(g_Clinic.m_JobManager.JobDesc[sel_job], jobdesc_id);
	}
}

void cScreenClinicManagement::GetSelectedGirls(vector<int> *girl_array)
{  // take passed vector and fill it with sorted list of selected girl IDs
	int pos = 0;
	int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
	while (GSelection != -1)
	{
		girl_array->push_back(GSelection);
		GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
	}
	sort(girl_array->begin(), girl_array->end());
}

void cScreenClinicManagement::ViewSelectedGirl()
{
	if (selected_girl)
	{
		if (selected_girl->is_dead()) return;
		//load up the cycle_girls vector with the ordered list of girl IDs
		FillSortedIDList(girllist_id, &cycle_girls, &cycle_pos);
		for (int i = cycle_girls.size(); i-- > 0;)
		{  // no viewing dead girls
			if (g_Clinic.GetGirl(g_CurrClinic, cycle_girls[i])->is_dead())
				cycle_girls.erase(cycle_girls.begin() + i);
		}
		g_CurrentScreen = SCREEN_GIRLDETAILS;
		g_InitWin = true;
		g_WinManager.push("Girl Details");
		return;
	}
}
