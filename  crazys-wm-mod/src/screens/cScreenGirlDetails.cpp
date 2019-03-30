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
#include "buildings/cBrothel.h"
#include "cScreenGirlDetails.h"
#include "cWindowManager.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "cGangs.h"
#include "cScriptManager.h"
#include "Game.hpp"
#include "cTraits.h"
#include "InterfaceGlobals.h"


extern bool g_Cheats;
extern	int	g_TalkCount;
extern bool g_AllTogle;
extern	bool	eventrunning;
extern cTraits g_Traits;
extern cConfig cfg;

extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_CTRLDown;

static cTariff tariff;
static stringstream ss;

static int ImageNum = -1;
static int DetailLevel = 0;
static bool Day0Night1 = SHIFT_DAY;

cScreenGirlDetails::cScreenGirlDetails() : cInterfaceWindowXML("girl_details_screen.xml")
{
}

void cScreenGirlDetails::set_ids()
{
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

	SetButtonNavigation(back_id, "<back>");
	SetButtonNavigation(gallery_id, "Gallery");
	SetButtonHotKey(gallery_id, SDLK_SPACE);
	SetButtonCallback(day_id, [this]( ) { set_shift(SHIFT_DAY); });
	SetButtonCallback(night_id, [this]( ) { set_shift(SHIFT_NIGHT); });

	SetButtonCallback(next_id, [this]( ) { NextGirl(); });
	SetButtonCallback(prev_id, [this]( ) { PrevGirl(); });
	SetButtonHotKey(prev_id, SDLK_a);
	SetButtonHotKey(next_id, SDLK_d);

	SetButtonCallback(interact_id, [this]( ) { do_interaction(); });
	SetButtonCallback(senddungeon_id, [this]( ) { send_to_dungeon(); });
	SetButtonCallback(reldungeon_id, [this]( ) { release_from_dungeon(); });
	SetButtonCallback(takegold_id, [this]( ) { take_gold(m_SelectedGirl); });
	SetButtonCallback(accomup_id, [this]( ) { update_accomodation(1); });
	SetButtonCallback(accomdown_id, [this]( ) { update_accomodation(-1); });
	SetButtonCallback(more_id, [this]( ) {
        if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(m_SelectedGirl), girldesc_id); }
        else if (DetailLevel == 1)	{ DetailLevel = 2; EditTextItem(cGirls::GetThirdDetailsString(m_SelectedGirl), girldesc_id); }
        else						{ DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(m_SelectedGirl), girldesc_id); }
	});
	SetButtonCallback(inventory_id, [this]( ) {
        if (m_SelectedGirl)
        {
            if (m_SelectedGirl->is_dead()) return;
            g_AllTogle = true;
            push_window("Item Management");
            return;
        }
	});

	SetListBoxSelectionCallback(traitlist_id, [this](int selection){
        EditTextItem((selection != -1 ? m_SelectedGirl->m_Traits[selection]->desc() : ""), traitdesc_id);
	});
    SetListBoxSelectionCallback(jobtypelist_id, [this](int selection){
        RefreshJobList();
    });
    SetListBoxSelectionCallback(joblist_id, [this](int selection){
        if (selection != -1) { on_select_job(selection, g_CTRLDown);  }
    });

    SetCheckBoxCallback(antipreg_id, [this](int on) {
        m_SelectedGirl->m_UseAntiPreg = on;
    });

    SetSliderCallback(houseperc_id, [this](int value) { set_house_percentage(value); });
    SetSliderCallback(accom_id, [this](int value) { set_accomodation(value); });
}

void cScreenGirlDetails::init(bool back)
{
    m_Refresh = false;
    m_SelectedGirl = &active_girl();
	Focused();

	if (m_SelectedGirl->is_dead())
	{
		// `J` instead of removing dead girls from the list which breaks the game, just skip past her in the list.
		NextGirl();								// `J` currently this prevents scrolling backwards past her - need to fix that.
		//		selected_girl = remove_selected_girl();
		if (m_SelectedGirl == nullptr)
		{
            pop_window();
			return;
		}
	}

	int job = (Day0Night1 ? m_SelectedGirl->m_NightJob : m_SelectedGirl->m_DayJob);

    EditTextItem(m_SelectedGirl->m_Realname, girlname_id);

	string detail;
	if (DetailLevel == 0)		detail = cGirls::GetDetailsString(m_SelectedGirl);
	else if (DetailLevel == 1)	detail = cGirls::GetMoreDetailsString(m_SelectedGirl);
	else						detail = cGirls::GetThirdDetailsString(m_SelectedGirl);
	EditTextItem(detail, girldesc_id);
	
    if (lastsexact != -1)
    {
        PrepareImage(girlimage_id, m_SelectedGirl, lastsexact, true, ImageNum);
        lastsexact = -1;
    }
    else
    {
        if (m_SelectedGirl->m_newRandomFixed >= 0)
        {
            PrepareImage(girlimage_id, m_SelectedGirl, IMGTYPE_PROFILE, false, m_SelectedGirl->m_newRandomFixed);
        }
        else
        {
            PrepareImage(girlimage_id, m_SelectedGirl, IMGTYPE_PROFILE, true, ImageNum);
        }
    }


	SliderRange(houseperc_id, 0, 100, m_SelectedGirl->house(), 10);
	ss.str("");	ss << "House Percentage: " << SliderValue(houseperc_id) << "%";
	EditTextItem(ss.str(), housepercval_id);

	ClearListBox(jobtypelist_id);

	// `J` Replacing accom buttons with slider
	if (accomdown_id != -1)	DisableButton(accomdown_id, m_SelectedGirl->m_AccLevel < 1);
	if (accomup_id != -1)	DisableButton(accomup_id, m_SelectedGirl->m_AccLevel > 9);
	if (accom_id != -1)
	{
		SliderRange(accom_id, 0, 9, m_SelectedGirl->m_AccLevel, 1);
		SliderMarker(accom_id, cGirls::PreferredAccom(m_SelectedGirl));
	}
	if (accomval_id != -1)
	{
		ss.str(""); ss << "Accommodation: " << cGirls::Accommodation(SliderValue(accom_id));
		if (cfg.debug.log_extradetails())
		{
			int val = SliderValue(accom_id) - cGirls::PreferredAccom(m_SelectedGirl);
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
	DisableButton(takegold_id, (m_SelectedGirl->m_Money <= 0));
	DisableButton(accomup_id, (m_SelectedGirl->m_AccLevel >= 9));
	DisableButton(accomdown_id, (m_SelectedGirl->m_AccLevel <= 0));
	SetCheckBox(antipreg_id, (m_SelectedGirl->m_UseAntiPreg));

    IBuilding* pBuilding = m_SelectedGirl->m_Building;
	DisableButton(reldungeon_id, m_SelectedGirl->m_DayJob != JOB_INDUNGEON);
	DisableButton(senddungeon_id, m_SelectedGirl->m_DayJob == JOB_INDUNGEON);

	ClearListBox(joblist_id);
	bool HideDNButtons = false;

	if(m_SelectedGirl->m_DayJob == JOB_INDUNGEON)
    {  // if in dungeon, effectively disable job lists
        HideDNButtons = true;
        ClearListBox(joblist_id);
        AddToListBox(jobtypelist_id, -1, "Languishing in dungeon");
        SetSelectedItemInList(jobtypelist_id, 0, false);
    } else if(pBuilding) {
        switch (pBuilding->type()) {
        case BuildingType::ARENA:
            AddToListBox(jobtypelist_id, JOBFILTER_ARENASTAFF,
                         g_Game.job_manager().JobFilterName[JOBFILTER_ARENASTAFF]);
            AddToListBox(jobtypelist_id, JOBFILTER_ARENA, g_Game.job_manager().JobFilterName[JOBFILTER_ARENA]);
            RefreshJobList();
            if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_ARENA] &&
                job < g_Game.job_manager().JobFilterIndex[JOBFILTER_ARENA + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_ARENA);
            else // if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_ARENASTAFF] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_ARENASTAFF + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_ARENASTAFF);
            break;
        case BuildingType::CLINIC:
            AddToListBox(jobtypelist_id, JOBFILTER_CLINIC, g_Game.job_manager().JobFilterName[JOBFILTER_CLINIC]);
            AddToListBox(jobtypelist_id, JOBFILTER_CLINICSTAFF, g_Game.job_manager().JobFilterName[JOBFILTER_CLINICSTAFF]);
            RefreshJobList();
            if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_CLINIC] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_CLINIC + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINIC);
            else SetSelectedItemInList(jobtypelist_id, JOBFILTER_CLINICSTAFF);
            break;
        case BuildingType::CENTRE:
            AddToListBox(jobtypelist_id, JOBFILTER_COMMUNITYCENTRE, g_Game.job_manager().JobFilterName[JOBFILTER_COMMUNITYCENTRE]);
            AddToListBox(jobtypelist_id, JOBFILTER_COUNSELINGCENTRE, g_Game.job_manager().JobFilterName[JOBFILTER_COUNSELINGCENTRE]);
            RefreshJobList();
            if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_COUNSELINGCENTRE] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_COUNSELINGCENTRE + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_COUNSELINGCENTRE);
            else SetSelectedItemInList(jobtypelist_id, JOBFILTER_COMMUNITYCENTRE);
            break;
        case BuildingType::HOUSE:
            AddToListBox(jobtypelist_id, JOBFILTER_HOUSE, g_Game.job_manager().JobFilterName[JOBFILTER_HOUSE]);
            AddToListBox(jobtypelist_id, JOBFILTER_HOUSETTRAINING, g_Game.job_manager().JobFilterName[JOBFILTER_HOUSETTRAINING]);
            RefreshJobList();
            if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_HOUSETTRAINING] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_HOUSETTRAINING + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_HOUSETTRAINING);
            else SetSelectedItemInList(jobtypelist_id, JOBFILTER_HOUSE);
            break;
        case BuildingType::STUDIO:
            Day0Night1 = SHIFT_NIGHT;
            HideDNButtons = true;
            // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cScreenGirlDetails.cpp
            AddToListBox(jobtypelist_id, JOBFILTER_STUDIOCREW, g_Game.job_manager().JobFilterName[JOBFILTER_STUDIOCREW]);
            AddToListBox(jobtypelist_id, JOBFILTER_STUDIONONSEX, g_Game.job_manager().JobFilterName[JOBFILTER_STUDIONONSEX]);
            AddToListBox(jobtypelist_id, JOBFILTER_STUDIOSOFTCORE, g_Game.job_manager().JobFilterName[JOBFILTER_STUDIOSOFTCORE]);
            AddToListBox(jobtypelist_id, JOBFILTER_STUDIOPORN, g_Game.job_manager().JobFilterName[JOBFILTER_STUDIOPORN]);
            AddToListBox(jobtypelist_id, JOBFILTER_STUDIOHARDCORE, g_Game.job_manager().JobFilterName[JOBFILTER_STUDIOHARDCORE]);
            AddToListBox(jobtypelist_id, JOBFILTER_RANDSTUDIO, g_Game.job_manager().JobFilterName[JOBFILTER_RANDSTUDIO]);
            RefreshJobList();
            /* */if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIONONSEX] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIONONSEX + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIONONSEX);
            else if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOSOFTCORE] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOSOFTCORE + 1])	SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOSOFTCORE);
            else if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOPORN] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOPORN + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOPORN);
            else if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOHARDCORE] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_STUDIOHARDCORE + 1])	SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOHARDCORE);
            else if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_RANDSTUDIO] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_RANDSTUDIO + 1])		SetSelectedItemInList(jobtypelist_id, JOBFILTER_RANDSTUDIO);
            else SetSelectedItemInList(jobtypelist_id, JOBFILTER_STUDIOCREW);
            break;
        case BuildingType::FARM:
            AddToListBox(jobtypelist_id, JOBFILTER_FARMSTAFF, g_Game.job_manager().JobFilterName[JOBFILTER_FARMSTAFF]);
            AddToListBox(jobtypelist_id, JOBFILTER_LABORERS, g_Game.job_manager().JobFilterName[JOBFILTER_LABORERS]);
            AddToListBox(jobtypelist_id, JOBFILTER_PRODUCERS, g_Game.job_manager().JobFilterName[JOBFILTER_PRODUCERS]);
            RefreshJobList();
            if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_LABORERS] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_LABORERS + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_LABORERS);
            else if (job >= g_Game.job_manager().JobFilterIndex[JOBFILTER_PRODUCERS] && job < g_Game.job_manager().JobFilterIndex[JOBFILTER_PRODUCERS + 1])
                SetSelectedItemInList(jobtypelist_id, JOBFILTER_PRODUCERS);
            else SetSelectedItemInList(jobtypelist_id, JOBFILTER_FARMSTAFF);
        case BuildingType::BROTHEL:
            // if not in dungeon, set up job lists
            // add the job filters
            //	for(int i=0; i<NUMJOBTYPES; i++)  // loop through all job types
            for (unsigned int i = 0; i <= JOBFILTER_BROTHEL; i++)  // temporary limit to job types shown
            {
                AddToListBox(jobtypelist_id, i, g_Game.job_manager().JobFilterName[i]);
            }

            // set the job filter
            int jobtype = 0;
            for (unsigned int i = 0; i < NUMJOBTYPES; i++)
            {
                if (job >= g_Game.job_manager().JobFilterIndex[i] && job < g_Game.job_manager().JobFilterIndex[i + 1])
                {
                    jobtype = i;
                    break;
                }
            }
            SetSelectedItemInList(jobtypelist_id, jobtype);
            RefreshJobList();
        }
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
		if (m_SelectedGirl->m_Traits[i])
		{
			ss.str(""); ss << m_SelectedGirl->m_Traits[i]->display_name();
			if (m_SelectedGirl->m_TempTrait[i] > 0) ss << "   (" << m_SelectedGirl->m_TempTrait[i] << ")";
			AddToListBox(traitlist_id, i, ss.str());
		}
	}
	EditTextItem("", traitdesc_id);
}

void cScreenGirlDetails::set_accomodation(int value)
{
    m_SelectedGirl->m_AccLevel = value;
    SliderRange(accom_id, 0, 9, m_SelectedGirl->m_AccLevel, 1);
    if (accomval_id != -1)
    {
        ss.str(""); ss << "Accommodation: " << cGirls::Accommodation(value);
        if (cfg.debug.log_extradetails())
        {
            int val = SliderValue(accom_id) - cGirls::PreferredAccom(m_SelectedGirl);
            if (val != 0) ss << "  ( " << (val > 0 ? "+" : "") << val << " )";
        }
        EditTextItem(ss.str(), accomval_id);
    }
    init(false);
}

void cScreenGirlDetails::set_house_percentage(int value)
{
    m_SelectedGirl->set_stat(STAT_HOUSE, value);
    ss.str("");
    ss << "House Percentage: " << value << "%";
    EditTextItem(ss.str(), housepercval_id);
    if (DetailLevel == 0)								// Rebelliousness might have changed, so update details
    {
        string detail = cGirls::GetDetailsString(m_SelectedGirl);
        EditTextItem(detail, girldesc_id);
    }
}

void cScreenGirlDetails::on_select_job(int selection, bool fulltime)
{// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
    int old_job = (Day0Night1 ? m_SelectedGirl->m_NightJob : m_SelectedGirl->m_DayJob);
    // handle special job requirements and assign - if HandleSpecialJobs returns true, the job assignment was modified or cancelled
    if (g_Game.job_manager().HandleSpecialJobs(m_SelectedGirl, selection, old_job, Day0Night1, fulltime))
    {
        selection = (Day0Night1 ? m_SelectedGirl->m_NightJob : m_SelectedGirl->m_DayJob);
        SetSelectedItemInList(joblist_id, selection, false);
    }
    // refresh job worker counts for former job and current job
    if (old_job != selection)
    {
        stringstream text;
        text << g_Game.job_manager().JobName[old_job] << " (" << m_SelectedGirl->m_Building->num_girls_on_job((JOBS)old_job, Day0Night1) << ")";
        SetSelectedItemText(joblist_id, old_job, text.str());
        text.str("");
text << g_Game.job_manager().JobName[selection] << " (" << m_SelectedGirl->m_Building->num_girls_on_job((JOBS)selection, Day0Night1) << ")";
    }
    RefreshJobList();
}

void cScreenGirlDetails::update_accomodation(int accadj)
{
    m_SelectedGirl->m_AccLevel += accadj;
    if (m_SelectedGirl->m_AccLevel > 9) m_SelectedGirl->m_AccLevel = 9;
    else if (m_SelectedGirl->m_AccLevel < 0) m_SelectedGirl->m_AccLevel = 0;
    if (accomval_id != -1) EditTextItem("Accommodation: " + cGirls::Accommodation(m_SelectedGirl->m_AccLevel),
                                        accomval_id);
    init(false);
}

void cScreenGirlDetails::release_from_dungeon()
{
    if (!m_SelectedGirl) return;
    g_Game.dungeon().GetDungeonPos(m_SelectedGirl);
    IBuilding& current_brothel = active_building();
    if (current_brothel.free_rooms() <= 0)
    {
        g_Game.push_message("The current brothel has no more room.\nBuy a new one, get rid of some girls, or change the brothel you are currently managing.", 0);
    }
    else
    {
        sGirl* nextGirl = remove_selected_girl();
        sGirl* tempGirl = g_Game.dungeon().RemoveGirl(g_Game.dungeon().GetGirl(g_Game.dungeon().GetGirlPos(
                m_SelectedGirl)));
        current_brothel.add_girl(tempGirl);

        if (g_Game.dungeon().GetNumGirls() == 0)
        {
            m_SelectedGirl = nullptr;
            pop_window();
        }
        else m_SelectedGirl = nextGirl;
    }
    init(false);
}

void cScreenGirlDetails::send_to_dungeon()
{
    if (!m_SelectedGirl) return;
    if(g_Game.dungeon().SendGirlToDungeon(*m_SelectedGirl)) {
        // if this was successful, change nothing
    } else {
        sGirl* nextGirl = remove_selected_girl();
        m_SelectedGirl = nextGirl;
        if (m_SelectedGirl == nullptr) pop_window();
    }

    // cannot call init() directly, because we would enable the release button directly under the pressed mouse
    m_Refresh = true;
}

void cScreenGirlDetails::do_interaction()
{
    if (!m_SelectedGirl) return;
    if (g_TalkCount > 0)
    {
        DirPath dp;
        eventrunning = true;
        if (m_SelectedGirl->m_DayJob != JOB_INDUNGEON)
        {
            int v[2] = { 1, -1 };
            cTrigger* trig = nullptr;
            if (!(trig = m_SelectedGirl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))	// trigger any girl specific talk script
            {
                // no, so trigger the default one
                dp = dp << "Resources" << "Scripts" << "DefaultInteractDetails.script";
            }
            else
            {
                // yes, so use that instead
                dp = DirPath(cfg.folders.characters().c_str()) << m_SelectedGirl->m_Name << trig->m_Script;
            }
        }
        else
        {
            int v[2] = { 0, -1 };
            cTrigger* trig = nullptr;
            if (!(trig = m_SelectedGirl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))	// trigger any girl specific talk script
            {
                // no, so trigger the default one
                dp = dp << "Resources" << "Scripts" << "DefaultInteractDungeon.script";
            }
            else
            {
                dp = DirPath(cfg.folders.characters().c_str()) << m_SelectedGirl->m_Name << trig->m_Script;
            }
        }
        cScriptManager script_manager;
        script_manager.Load(dp, m_SelectedGirl);
        if (!g_Cheats)
            g_TalkCount--;
    }
    init(false);
    return;
}

void cScreenGirlDetails::set_shift(int shift)
{
    Day0Night1 = shift;
    DisableButton(day_id, shift == SHIFT_DAY);
    DisableButton(night_id, shift == SHIFT_NIGHT);
    init(false);
}

void cScreenGirlDetails::RefreshJobList()
{
	ClearListBox(joblist_id);
	int job_filter = GetSelectedItemFromList(jobtypelist_id);
	if (job_filter == -1) return;

	string text;
	// populate Jobs listbox with jobs in the selected category
	for (int i = g_Game.job_manager().JobFilterIndex[job_filter]; i<g_Game.job_manager().JobFilterIndex[job_filter + 1]; i++)
	{
		if (g_Game.job_manager().JobName[i].empty()) continue;
		std::stringstream btext;
        btext << g_Game.job_manager().JobName[i];
        btext << " (";
        if(m_SelectedGirl->m_Building) {
            btext << m_SelectedGirl->m_Building->num_girls_on_job((JOBS)i, Day0Night1);
        } else {
            btext << '?';
        }
        btext << ")";
		AddToListBox(joblist_id, i, btext.str());
	}
	if (m_SelectedGirl)
	{
		int sel_job = (Day0Night1 ? m_SelectedGirl->m_NightJob : m_SelectedGirl->m_DayJob);
		SetSelectedItemInList(joblist_id, sel_job, false);
	}
}

void cScreenGirlDetails::PrevGirl()
{
    set_active_girl(get_prev_girl());
    init(false);
}

void cScreenGirlDetails::NextGirl()
{
    set_active_girl(get_next_girl());
    init(false);
}

sGirl *cScreenGirlDetails::get_prev_girl()		// return previous girl in the sorted list
{
	g_LogFile.write("Where is the girl??");

    if (m_SelectedGirl->m_DayJob == JOB_INDUNGEON)
    {
        g_LogFile.write("She is in the Dungeon");
        return g_Game.dungeon().GetGirl(g_Game.dungeon().GetGirlPos(m_SelectedGirl) - 1)->m_Girl.get();
    }
    else
    {
        auto source_building = m_SelectedGirl->m_Building;

        g_LogFile.write("She is in '" + source_building->name() + "'");
        auto index = source_building->get_girl_index(*m_SelectedGirl);
        if(index == -1) {
            g_LogFile.write("Error: Girl is not in building she claims she is!");
            return m_SelectedGirl;
        } else if (index == 0) {
            return source_building->get_girl(source_building->num_girls() - 1);
        }
        return source_building->get_girl(index - 1);
    }
}

sGirl *cScreenGirlDetails::get_next_girl()		// return next girl in the sorted list
{
	g_LogFile.write("Where is the girl??");

    if (m_SelectedGirl->m_DayJob == JOB_INDUNGEON)
    {
        g_LogFile.write("She is in the Dungeon");
        return g_Game.dungeon().GetGirl(g_Game.dungeon().GetGirlPos(m_SelectedGirl) + 1)->m_Girl.get();
    }
    else
    {
        auto source_building = m_SelectedGirl->m_Building;

        g_LogFile.write("She is in '" + source_building->name() + "'");
        auto index = source_building->get_girl_index(*m_SelectedGirl);
        if(index == -1) {
            g_LogFile.write("Error: Girl is not in building she claims she is!");
            return m_SelectedGirl;
        } else if (index == source_building->num_girls() - 1) {
            return source_building->get_girl(0);
        }
        return source_building->get_girl(index + 1);
    }
}

sGirl *cScreenGirlDetails::remove_selected_girl()		// the selected girl is to be removed from the current list; returns next selected girl
{
	/*sGirl *next_girl = nullptr;
	if (cycle_girls.empty()) return nullptr;
	int cur_id			= cycle_girls[cycle_pos];
	cycle_girls.erase(cycle_girls.begin() + cycle_pos);  // remove her
	if (cycle_pos >= (int)cycle_girls.size())
	{												
		if (cycle_pos > 0) cycle_pos--;					// if this girl was the last in the list, move list position -1 if possible
		else return next_girl;							// or, maybe we have no more girls in the list
	}

	if (m_SelectedGirl->m_DayJob == JOB_INDUNGEON) next_girl = g_Game.dungeon().GetGirl(cycle_girls[cycle_pos])->m_Girl.get();
	else if(m_SelectedGirl->m_Building) {
	    next_girl = *std::next(m_SelectedGirl->m_Building->girls().begin(), cycle_girls[cycle_pos]);
	}

	return next_girl;
	 */
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
	if (!girl->fights_back())
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
	while ((gang = g_Game.gang_manager().GetGangOnMission(MISS_GUARDING)))
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
		girl_win_flag = g_Game.gang_manager().GangCombat(girl, gang);
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
	if (!g_Game.player().Combat(girl))	// fight with the player
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
	girl->run_away();

	stringstream smess;
	smess << girl->m_Realname << " has run away";
	g_Game.push_message(smess.str(), 1);

    m_SelectedGirl = nextGirl;
    init(false);

	if (m_SelectedGirl == nullptr) pop_window();

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
		g_Game.push_message(message, 0);
        init(false);
		return;
	}
	/*
	*	so the girl lost: take away her money now
	*/
	g_Game.gold().misc_credit(girl->m_Money);
	girl->m_Money = 0;
	/*
	*	she isn't going to like this much
	*	but it's not so bad for slaves
	*/
	if (girl->is_slave())
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
	g_Game.push_message(message, 0);
    init(false);
	return;
}

void cScreenGirlDetails::OnKeyPress(SDL_keysym keysym)
{
    auto key = keysym.sym;

    if (g_AltKeys)
    {
        if (key == SDLK_h || key == SDLK_j)
        {
            int mod = (key == SDLK_h ? -1 : 1);
            m_SelectedGirl->house(mod);
            SliderValue(houseperc_id, m_SelectedGirl->house());
            ss.str(""); ss << "House Percentage: " << m_SelectedGirl->house() << "%";
            EditTextItem(ss.str(), housepercval_id);
            // Rebelliousness might have changed, so update details
            if (DetailLevel == 0)
            {
                string detail = cGirls::GetDetailsString(m_SelectedGirl);
                EditTextItem(detail, girldesc_id);
            }
        }
        if (key == SDLK_s)
        {
            if (keysym.mod & KMOD_SHIFT)
            {
                DetailLevel = 2;
                EditTextItem(cGirls::GetThirdDetailsString(m_SelectedGirl), girldesc_id);
            }
            else
            {
                if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(m_SelectedGirl), girldesc_id); }
                else						{ DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(m_SelectedGirl), girldesc_id); }
            }
        }
    }
}

void cScreenGirlDetails::process()
{
    if(m_Refresh) {
        init(false);
    }
}
