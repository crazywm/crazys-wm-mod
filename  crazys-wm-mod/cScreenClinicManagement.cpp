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
#include "cClinic.h"
#include "cBrothel.h"
#include "cScreenClinicManagement.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "InterfaceProcesses.h"
#include "libintl.h"
#include "cScreenGirlDetails.h"

extern cScreenGirlDetails g_GirlDetails;

extern bool g_InitWin;
extern int g_CurrClinic;
extern cGold g_Gold;
extern cClinicManager g_Clinic;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
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
static bool FireGirl = false;
static bool FreeGirl = false;
static bool SellGirl = false;
static int selection = -1;
static bool Day0Night1 = SHIFT_DAY;	// 1 is night, 0 is day.
static bool SetJob = false;

extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;
extern cPlayer* The_Player;

bool cScreenClinicManagement::ids_set = false;

void cScreenClinicManagement::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	curclinic_id = get_id("Clinic");
	girllist_id = get_id("GirlList");
	girlimage_id = get_id("GirlImage");
	girldesc_id = get_id("GirlDescription");
	viewdetails_id = get_id("ViewDetailsButton");
	freeslave_id = get_id("FreeSlaveButton");
	transfer_id = get_id("TransferButton");
	jobtypehead_id = get_id("JobTypeHeader");
	jobtypelist_id = get_id("JobTypeList");
	jobtypedesc_id = get_id("JobTypeDescription");
	jobhead_id = get_id("JobHeader");
	joblist_id = get_id("JobList");
	jobdesc_id = get_id("JobDescription");
	day_id = get_id("DayButton");
	night_id = get_id("NightButton");

	//Set the default sort order for columns, so listbox knows the order in which data will be sent
	SortColumns(girllist_id, m_ListBoxes[girllist_id]->m_ColumnName, m_ListBoxes[girllist_id]->m_ColumnCount);
}

void cScreenClinicManagement::init()
{

	if (FreeGirl)
	{
		if (g_ChoiceManager.GetChoice(0) == 0)
		{
			vector<int> girl_array;
			GetSelectedGirls(&girl_array);  // get and sort array of girls

			for (int i = girl_array.size(); i--> 0;)	// OK, we have the array, now step through it backwards
			{
				selected_girl = g_Clinic.GetGirl(g_CurrClinic, girl_array[i]);
				if (GirlDead(selected_girl) || !selected_girl->is_slave()) continue;  // if dead or not a slave, can't free her
				if (selected_girl)
				{
					selected_girl->m_States &= ~(1 << STATUS_SLAVE);
					The_Player->disposition(5);
					g_Girls.UpdateStat(selected_girl, STAT_PCLOVE, 10);
					g_Girls.UpdateStat(selected_girl, STAT_PCFEAR, -20);
					g_Girls.UpdateStat(selected_girl, STAT_PCHATE, -25);
					g_Girls.UpdateStat(selected_girl, STAT_OBEDIENCE, 10);
					g_Girls.UpdateStat(selected_girl, STAT_HAPPINESS, 70);
					
					selected_girl->m_AccLevel = cfg.initial.girls_accom();
					selected_girl->m_Stats[STAT_HOUSE] = cfg.initial.girls_house_perc();
					g_InitWin = true;
				}
			}
		}
		g_ChoiceManager.Free();
		FreeGirl = false;
	}

	g_CurrentScreen = SCREEN_CLINIC;
	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;


	////////////////////

	selection = GetSelectedItemFromList(girllist_id);
	string clinic;
	clinic += g_Clinic.GetName(g_CurrClinic);
	EditTextItem(clinic, curclinic_id);

	//selected_girl = 0;

	// clear the lists
	ClearListBox(girllist_id);
	ClearListBox(jobtypelist_id);

	// add the job filters
	//	for(int i=0; i<NUMJOBTYPES; i++)  // loop through all job types
	AddToListBox(jobtypelist_id, JOBFILTER_CLINICSTAFF, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINICSTAFF]);
	AddToListBox(jobtypelist_id, JOBFILTER_CLINIC, g_Clinic.m_JobManager.JobFilterName[JOBFILTER_CLINIC]);
	RefreshJobList();
	SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINICSTAFF);

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[girllist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();
	string* Data = new string[numColumns];

	// Add girls to list
	for (int i = 0; i<g_Clinic.GetNumGirls(g_CurrClinic); i++)
	{
		sGirl* gir = g_Clinic.GetGirl(g_CurrClinic, i);
		if (selected_girl == gir)
			selection = i;

		unsigned int item_color = COLOR_BLUE;
		if (g_Girls.GetStat(gir, STAT_HEALTH) <= 30 || g_Girls.GetStat(gir, STAT_TIREDNESS) >= 80 || g_Girls.GetStat(gir, STAT_HAPPINESS) <= 30)
			item_color = COLOR_RED;

		gir->OutputGirlRow(Data, columnNames);
		AddToListBox(girllist_id, i, Data, numColumns, item_color);
	}
	delete[] Data;

	lastNum = -1;
	g_InitWin = false;

	if (selection >= 0)
	{
		while (selection > GetListBoxSize(girllist_id) && selection != -1)
			selection--;
	}
	if (selection >= 0)
		SetSelectedItemInList(girllist_id, selection);
	else
		SetSelectedItemInList(girllist_id, 0);

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
	if (g_UpArrow || (g_AltKeys && g_A_Key))
	{
		g_UpArrow = g_A_Key = false;
		selection = ArrowUpListBox(girllist_id);
		return true;
	}
	if (g_DownArrow || (g_AltKeys && g_D_Key))
	{
		g_DownArrow = g_D_Key = false;
		selection = ArrowDownListBox(girllist_id);
		return true;
	}
	if (g_SpaceKey)		// Show Girl Details
	{
		g_SpaceKey = false;
		g_GirlDetails.lastsexact = -1;
		ViewSelectedGirl();
		return true;
	}
	return false;
}

void cScreenClinicManagement::update_image()
{
	// Draw a girls profile picture and description when selected
	if (selected_girl)
	{
		bool Rand = false;
		if (lastNum != selection)
		{
			string text = g_Girls.GetGirlMood(selected_girl);
			text += "\n\n";
			text += selected_girl->m_Desc;
			// Added a little feedback here to show what character template a girl is based on --PP
				// `J` I usually don't care about this so I made it optional
			if (cfg.debug.log_extradetails())
			{
				text += "\n\nBased on: ";
				text += selected_girl->m_Name;
			}
			EditTextItem(text, girldesc_id);
			Rand = true;
			lastNum = selection;
		}

		SetImage(girlimage_id, g_Girls.GetImageSurface(selected_girl, IMGTYPE_PROFILE, Rand, ImageNum));
		// `J` added check in case the girl has no images and the game is using defaults (no .ani defaults)
		if (selected_girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
		{
			if (g_Girls.IsAnimatedSurface(selected_girl, IMGTYPE_PROFILE, ImageNum))
				SetImage(girlimage_id, g_Girls.GetAnimatedSurface(selected_girl, IMGTYPE_PROFILE, ImageNum));
		}
		HideImage(girlimage_id, false);
	}
	else
	{
		selection = lastNum = -1;
		EditTextItem(gettext("No Girl Selected"), girldesc_id);
		HideImage(girlimage_id, true);
	}
}

void cScreenClinicManagement::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	// if it's the back button, pop the window off the stack and we're done
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

	if (g_InterfaceEvents.CheckButton(viewdetails_id))
	{
		ViewSelectedGirl();
	}

	if (g_InterfaceEvents.CheckButton(day_id))
	{
		DisableButton(day_id, true);
		DisableButton(night_id, false);
		Day0Night1 = SHIFT_DAY;
		RefreshSelectedJobType();
	}
	if (g_InterfaceEvents.CheckButton(night_id))
	{
		DisableButton(day_id, false);
		DisableButton(night_id, true);
		Day0Night1 = SHIFT_NIGHT;
		RefreshSelectedJobType();
	}

	if (g_InterfaceEvents.CheckListbox(jobtypelist_id))
	{
		selection = GetSelectedItemFromList(jobtypelist_id);
		if (selection == -1) EditTextItem("Nothing Selected", jobtypedesc_id);
		else
		{
			// populate Jobs listbox with jobs in the selected category
			RefreshJobList();
			string jdmessage = g_Clinic.m_JobManager.JobFilterDesc[selection];
			if (g_Clinic.DoctorNeeded())
			{
				jdmessage += gettext("\n*** A Doctor is required to perform any surgeries. ");
			}
			if ((g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_MECHANIC, SHIFT_DAY) < 1 &&
				g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_GETREPAIRS, SHIFT_DAY) > 0) ||
				(g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_MECHANIC, SHIFT_NIGHT) < 1 &&
				g_Clinic.GetNumGirlsOnJob(g_CurrClinic, JOB_GETREPAIRS, SHIFT_NIGHT) > 0))
			{
				jdmessage += gettext("\n**** A Mechanic is required to perform any Repairs. ");
			}
			EditTextItem(jdmessage, jobtypedesc_id);
		}
	}
	if (g_InterfaceEvents.CheckListbox(joblist_id))
	{
		bool fulltime = g_CTRLDown;

		selection = GetSelectedItemFromList(joblist_id);
		if (selection != -1)
		{
			// first handle the descriptions
			EditTextItem(g_Clinic.m_JobManager.JobDesc[selection], jobdesc_id);

			// Now assign the job to all the selected girls
			int pos = 0;
			int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
			while (GSelection != -1)
			{
				u_int new_job = selection;
				selected_girl = g_Clinic.GetGirl(g_CurrClinic, GSelection);
				if (selected_girl)
				{
					int old_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);

					// handle special job requirements and assign
					// if HandleSpecialJobs returns true, the job assignment was modified or cancelled

					if (g_Clinic.m_JobManager.HandleSpecialJobs(g_CurrClinic, selected_girl, new_job, old_job, Day0Night1, fulltime))
					{
						new_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
						SetSelectedItemInList(joblist_id, new_job, false);
					}
					// update the girl's listing to reflect the job change
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob];
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob];
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());

					// refresh job worker counts for former job and current job
					SetSelectedItemText(joblist_id, old_job, g_Clinic.m_JobManager.JobDescriptionCount(old_job, 0, Day0Night1, true));
					SetSelectedItemText(joblist_id, new_job, g_Clinic.m_JobManager.JobDescriptionCount(new_job, 0, Day0Night1, true));
				}
				bool interrupted = false;	// `J` added
				if (selected_girl->m_YesterDayJob != selected_girl->m_DayJob &&
					g_Clinic.is_Surgery_Job(selected_girl->m_YesterDayJob) &&
					((selected_girl->m_WorkingDay > 0) || selected_girl->m_PrevWorkingDay > 0))
					interrupted = true;

				if (selected_girl->m_DayJob == JOB_GETABORT)	// `J` added
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " (" << 2 - selected_girl->m_WorkingDay << ")*";
					if (interrupted)	ss << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (g_Clinic.is_Surgery_Job(selected_girl->m_DayJob))	// `J` added
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " (" << 5 - selected_girl->m_WorkingDay << ")*";
					if (interrupted)	ss << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}
				else if (interrupted)
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_DayJob] << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
				}

				if (selected_girl->m_NightJob == JOB_GETABORT)	// `J` added
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " (" << 2 - selected_girl->m_WorkingDay << ")*";
					if (interrupted)	ss << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				else if (g_Clinic.is_Surgery_Job(selected_girl->m_NightJob))	// `J` added
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " (" << 5 - selected_girl->m_WorkingDay << ")*";
					if (interrupted)	ss << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				else if (interrupted)
				{
					ss.str("");
					ss << g_Clinic.m_JobManager.JobName[selected_girl->m_NightJob] << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}
				if (interrupted)
				{	// `J` added
					string jdmessage = g_Clinic.m_JobManager.JobDesc[new_job] + gettext("\n** This girl was getting ");
					if (selected_girl->m_YesterDayJob == JOB_BOOBJOB || selected_girl->m_YesterDayJob == JOB_FACELIFT)
						jdmessage += "a ";
					else if (selected_girl->m_YesterDayJob == JOB_GETABORT || selected_girl->m_YesterDayJob == JOB_ASSJOB)
						jdmessage += "an ";
					jdmessage += g_Clinic.m_JobManager.JobName[selected_girl->m_YesterDayJob]
						+ ", if you send her somewhere else, she will have to start her Surgery over.";
					EditTextItem(jdmessage, jobdesc_id);
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
			if (ListDoubleClicked(girllist_id)) ViewSelectedGirl();		// If double-clicked, try to bring up girl details
			DisableButton(freeslave_id, selected_girl->is_free());
			DisableButton(viewdetails_id, false);
			RefreshSelectedJobType();
		}
		else
		{
			DisableButton(freeslave_id, true);
			DisableButton(viewdetails_id, true);
			selected_girl = 0;
			selection = -1;
		}
		lastNum = -2;
		update_image();
		return;
	}
	if (g_InterfaceEvents.CheckButton(freeslave_id))
	{
		if (selected_girl)
		{
			if (IsMultiSelected(girllist_id))
			{  // multiple girls selected
				g_MessageQue.AddToQue("Are you sure you wish to give these girls their freedom?", 0);
				g_ChoiceManager.CreateChoiceBox(224, 112, 352, 384, 0, 2, 32, strlen("Keep as a slaves"));
				g_ChoiceManager.AddChoice(0, "Grant Freedom", 0);
				g_ChoiceManager.AddChoice(0, "Keep as a slaves", 1);
				g_ChoiceManager.SetActive(0);
				FreeGirl = true;
			}
			else  // only one girl selected
			{
				if (GirlDead(selected_girl)) return;
				g_MessageQue.AddToQue("Are you sure you wish to give " + selected_girl->m_Realname + " her freedom?", 0);
				g_ChoiceManager.CreateChoiceBox(224, 112, 352, 384, 0, 2, 32, strlen("Keep as a slave"));
				g_ChoiceManager.AddChoice(0, "Grant Freedom", 0);
				g_ChoiceManager.AddChoice(0, "Keep as a slave", 1);
				g_ChoiceManager.SetActive(0);
				FreeGirl = true;
			}
		}
		return;
	}
	if (g_InterfaceEvents.CheckButton(transfer_id))
	{
		g_InitWin = true;
		g_WinManager.Push(TransferGirls, &g_TransferGirls);
		return;
	}
}


bool cScreenClinicManagement::GirlDead(sGirl *dgirl, bool sendmessage)
{
	if (dgirl->health() <= 0)
	{
		if (sendmessage) g_MessageQue.AddToQue(gettext("This girl is dead. She isn't going to work anymore and her body will be removed by the end of the week."), 1);
		return true;
	}
	return false;
}

void cScreenClinicManagement::RefreshSelectedJobType()
{
	selection = GetSelectedItemFromList(girllist_id);
	if (selection < 0) return;
	selected_girl = g_Clinic.GetGirl(g_CurrClinic, selection);
	u_int job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
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

	string text = "";

	// populate Jobs listbox with jobs in the selected category
	for (unsigned int i = g_Clinic.m_JobManager.JobFilterIndex[job_filter]; i < g_Clinic.m_JobManager.JobFilterIndex[job_filter + 1]; i++)
	{
		if (g_Clinic.m_JobManager.JobName[i] == "") continue;
		text = g_Clinic.m_JobManager.JobDescriptionCount(i, g_CurrClinic, Day0Night1, true);
		AddToListBox(joblist_id, i, text);
	}

	if (selected_girl &&
		g_Clinic.is_Surgery_Job(selected_girl->m_YesterDayJob) &&		// `J` added
		selected_girl->m_YesterDayJob != selected_girl->m_DayJob &&
		(selected_girl->m_WorkingDay > 0 || selected_girl->m_PrevWorkingDay > 0))
	{
		int sel_job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
		SetSelectedItemInList(joblist_id, sel_job, false);

		stringstream jdmessage;
		jdmessage << g_Clinic.m_JobManager.JobDesc[sel_job] << gettext("\n** This girl was getting ");
		if (selected_girl->m_YesterDayJob == JOB_BOOBJOB || selected_girl->m_YesterDayJob == JOB_FACELIFT)
			jdmessage << "a ";
		else if (selected_girl->m_YesterDayJob == JOB_GETABORT || selected_girl->m_YesterDayJob == JOB_ASSJOB)
			jdmessage << "an ";
		else if (selected_girl->m_YesterDayJob == JOB_TUBESTIED)
			jdmessage << "her ";
		jdmessage << g_Clinic.m_JobManager.JobName[selected_girl->m_YesterDayJob]
			<< gettext(", if you send her somewhere else, she will have to start her Surgery over.");
		EditTextItem(jdmessage.str(), jobdesc_id);
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
		if (GirlDead(selected_girl)) return;

		//load up the cycle_girls vector with the ordered list of girl IDs
		FillSortedIDList(girllist_id, &cycle_girls, &cycle_pos);
		for (int i = cycle_girls.size(); i-- > 0;)
		{  // no viewing dead girls
			if (g_Clinic.GetGirl(g_CurrClinic, cycle_girls[i])->health() <= 0)
				cycle_girls.erase(cycle_girls.begin() + i);
		}

		g_InitWin = true;
		g_WinManager.push("Girl Details");
		return;
	}
}
