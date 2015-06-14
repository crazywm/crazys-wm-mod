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
#include "cMovieStudio.h"
#include "cScreenStudioManagement.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "InterfaceProcesses.h"
#include "libintl.h"
#include "cScreenGirlDetails.h"

extern cScreenGirlDetails g_GirlDetails;

extern bool g_InitWin;
extern int g_CurrStudio;
extern cGold g_Gold;
extern cMovieStudioManager g_Studios;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
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
extern	int		g_CurrentScreen;

static cTariff tariff;
static stringstream ss;

static int lastNum = -1;
static int ImageNum = -1;
static bool FireGirl = false;
static bool FreeGirl = false;
static bool SellGirl = false;
static int selection = -1;
static bool Day0Night1 = SHIFT_NIGHT;	// 1 is night, 0 is day.
static bool SetJob = false;

extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;
extern cPlayer* The_Player;


bool cScreenStudioManagement::ids_set = false;

void cScreenStudioManagement::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	curstudio_id = get_id("Studio");
	girllist_id = get_id("GirlList");
	girlimage_id = get_id("GirlImage");
	girldesc_id = get_id("GirlDescription");
	viewdetails_id = get_id("ViewDetailsButton");
	transfer_id = get_id("TransferButton");
	createmovie_id = get_id("CreateMovieButton");
	freeslave_id = get_id("FreeSlaveButton");
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

void cScreenStudioManagement::init()
{

	if (FreeGirl)
	{
		if (g_ChoiceManager.GetChoice(0) == 0)
		{
			vector<int> girl_array;
			GetSelectedGirls(&girl_array);  // get and sort array of girls

			for (int i = girl_array.size(); i--> 0;)	// OK, we have the array, now step through it backwards
			{
				selected_girl = g_Studios.GetGirl(g_CurrStudio, girl_array[i]);
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

	g_CurrentScreen = SCREEN_STUDIO;
	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;


	////////////////////

	selection = GetSelectedItemFromList(girllist_id);
	string studio;
	studio += g_Studios.GetName(g_CurrStudio);
	EditTextItem(studio, curstudio_id);

	//selected_girl = 0;

	// clear the lists
	ClearListBox(girllist_id);
	ClearListBox(jobtypelist_id);

	// add the job filters
	//	for(int i=0; i<NUMJOBTYPES; i++)  // loop through all job types
	AddToListBox(jobtypelist_id, JOBFILTER_STUDIOCREW, g_Studios.m_JobManager.JobFilterName[JOBFILTER_STUDIOCREW]);
	AddToListBox(jobtypelist_id, JOBFILTER_MOVIESTUDIO, g_Studios.m_JobManager.JobFilterName[JOBFILTER_MOVIESTUDIO]);
	SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOCREW);

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[girllist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();
	string* Data = new string[numColumns];

	// Add girls to list
	for (int i = 0; i<g_Studios.GetNumGirls(g_CurrStudio); i++)
	{
		sGirl* gir = g_Studios.GetGirl(g_CurrStudio, i);
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
	{
		SetSelectedItemInList(girllist_id, 0);
		selection = 0;
	}

	HideButton(day_id, 1);
	HideButton(night_id, 1);
	RefreshSelectedJobType();
	RefreshJobList();
	update_image();
}

void cScreenStudioManagement::process()
{
	// we need to make sure the ID variables are set
	if (!ids_set)
		set_ids();

	// handle arrow keys
	if (check_keys())
		return;

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}


bool cScreenStudioManagement::check_keys()
{
	if (g_UpArrow) {
		selection = ArrowUpListBox(girllist_id);
		g_UpArrow = false;
		return true;
	}
	if (g_DownArrow) {
		selection = ArrowDownListBox(girllist_id);
		g_DownArrow = false;
		return true;
	}
	if (g_AltKeys)
	{
		if (g_A_Key) {
			selection = ArrowUpListBox(girllist_id);
			g_A_Key = false;
			return true;
		}
		if (g_D_Key) {
			selection = ArrowDownListBox(girllist_id);
			g_D_Key = false;
			return true;
		}
	}
	// Show Girl Details
	if (g_SpaceKey)
	{
		g_SpaceKey = false;
		g_GirlDetails.lastsexact = -1;
		ViewSelectedGirl();
		return true;
	}
	return false;
}

void cScreenStudioManagement::update_image()
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

		PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, Rand, lastNum);
		HideImage(girlimage_id, false);
	}
	else
	{
		selection = lastNum = -1;
		EditTextItem(gettext("No Girl Selected"), girldesc_id);
		HideImage(girlimage_id, true);
	}
}

void cScreenStudioManagement::check_events()
{
	// no events means we can go home
	if (g_InterfaceEvents.GetNumEvents() == 0)
		return;

	// if it's the back button, pop the window off the stack and we're done
	if (g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(viewdetails_id))
	{
		ViewSelectedGirl();
	}

	if (g_InterfaceEvents.CheckButton(createmovie_id))
	{
		g_InitWin = true;
		g_WinManager.push("Movie Maker");
		return;
	}
	if (g_InterfaceEvents.CheckButton(day_id))
	{
		DisableButton(day_id, true);
		DisableButton(night_id, false);
		Day0Night1 = SHIFT_NIGHT; // We only want night shifts in studio.. just a sanity check --PP
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

		if (selection == -1)
			EditTextItem(gettext("Nothing Selected"), jobtypedesc_id);
		else
		{
			// populate Jobs listbox with jobs in the selected category
			RefreshJobList();
			string jdmessage = g_Studios.m_JobManager.JobFilterDesc[selection];
			if (g_Studios.CrewNeeded())
			{
				jdmessage += gettext("\n** At least one Camera Mage and one Crystal Purifier is required to film a scene. ");
			}
			EditTextItem(jdmessage, jobtypedesc_id);
		}
	}
	if (g_InterfaceEvents.CheckListbox(joblist_id))
	{
		selection = GetSelectedItemFromList(joblist_id);
		if (selection != -1)
		{
			// first handle the descriptions
			EditTextItem(g_Studios.m_JobManager.JobDesc[selection], jobdesc_id);

			// Now assign the job to all the selected girls
			int pos = 0;
			int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
			while (GSelection != -1)
			{
				selected_girl = g_Studios.GetGirl(g_CurrStudio, GSelection);
				if (selected_girl)
				{
					int old_job = selected_girl->m_NightJob;

					// handle special job requirements and assign
					// if HandleSpecialJobs returns true, the job assignment was modified or cancelled

					if (g_Studios.m_JobManager.HandleSpecialJobs(g_CurrStudio, selected_girl, selection, old_job, false))
					{
						selection = selected_girl->m_NightJob;
						SetSelectedItemInList(joblist_id, selection, false);
					}


					if (old_job != selection)
					{
						// update the girl's listing to reflect the job change
						ss.str("");
						ss << g_Studios.m_JobManager.JobName[selected_girl->m_DayJob];
						SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->DayJobColumn());
						ss.str("");
						ss << g_Studios.m_JobManager.JobName[selected_girl->m_NightJob];
						SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());

						// refresh job worker counts for former job and current job
						SetSelectedItemText(joblist_id, old_job, g_Studios.m_JobManager.JobDescriptionCount(old_job, 0, SHIFT_NIGHT, false, true));
						SetSelectedItemText(joblist_id, selection, g_Studios.m_JobManager.JobDescriptionCount(selection, 0, SHIFT_NIGHT, false, true));
					}
				}
				if (g_Studios.is_Actress_Job(selected_girl->m_NightJob))	// `J` added
				{
					ss.str("");
					ss << g_Studios.m_JobManager.JobName[selected_girl->m_NightJob];
					if (g_Studios.CrewNeeded())	ss << " **";
					SetSelectedItemColumnText(girllist_id, GSelection, ss.str(), m_ListBoxes[girllist_id]->NightJobColumn());
				}

				GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
			}
		}
		else
			EditTextItem(gettext("Nothing Selected"), jobdesc_id);
	}

	if (g_InterfaceEvents.CheckListbox(girllist_id))
	{
		selection = GetSelectedItemFromList(girllist_id);
		if (selection != -1)
		{
			selected_girl = g_Studios.GetGirl(g_CurrStudio, selection);
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
		g_CurrentScreen = SCREEN_TRANSFERGIRLS;
		g_InitWin = true;
		g_WinManager.Push(TransferGirls, &g_TransferGirls);
		return;
	}
}


bool cScreenStudioManagement::GirlDead(sGirl *dgirl, bool sendmessage)
{
	if (g_Girls.GetStat(dgirl, STAT_HEALTH) <= 0)
	{
		if (sendmessage) g_MessageQue.AddToQue(gettext("This girl is dead. She isn't going to work anymore and her body will be removed by the end of the week."), 1);
		return true;
	}
	else
		return false;
}

void cScreenStudioManagement::RefreshSelectedJobType()
{
	selection = GetSelectedItemFromList(girllist_id);
	if (selection < 0) return;
	selected_girl = g_Studios.GetGirl(0, selection);
	u_int job = selected_girl->m_NightJob;
	// set the job filter
	if (job >= g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_MOVIESTUDIO] && job < g_Studios.m_JobManager.JobFilterIndex[JOBFILTER_MOVIESTUDIO + 1])
		SetSelectedItemInList(jobtypelist_id, JOBFILTER_MOVIESTUDIO);
	else SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOCREW);
	SetJob = true;
}

void cScreenStudioManagement::RefreshJobList()
{
	ClearListBox(joblist_id);
	int job_filter = GetSelectedItemFromList(jobtypelist_id);

	if (job_filter == -1)
		return;

	string text = "";

	// populate Jobs listbox with jobs in the selected category
	for (unsigned int i = g_Studios.m_JobManager.JobFilterIndex[job_filter]; i < g_Studios.m_JobManager.JobFilterIndex[job_filter + 1]; i++)
	{
		if (g_Studios.m_JobManager.JobName[i] == "")
			continue;
		text = g_Studios.m_JobManager.JobDescriptionCount(i, g_CurrStudio, SHIFT_NIGHT, false, true);
		AddToListBox(joblist_id, i, text);
	}

	if (selected_girl)
	{
		int sel_job = selected_girl->m_NightJob;
		SetSelectedItemInList(joblist_id, sel_job, false);
		EditTextItem(g_Studios.m_JobManager.JobDesc[sel_job], jobdesc_id);
	}
}

void cScreenStudioManagement::GetSelectedGirls(vector<int> *girl_array)
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

void cScreenStudioManagement::ViewSelectedGirl()
{
	if (selected_girl)
	{
		if (GirlDead(selected_girl)) return;

		//load up the cycle_girls vector with the ordered list of girl IDs
		FillSortedIDList(girllist_id, &cycle_girls, &cycle_pos);
		for (int i = cycle_girls.size(); i-- > 0;)
		{  // no viewing dead girls
			if (g_Studios.GetGirl(g_CurrStudio, cycle_girls[i])->health() <= 0)
				cycle_girls.erase(cycle_girls.begin() + i);
		}

		g_InitWin = true;
		g_WinManager.push("Girl Details");
		return;
	}
}
