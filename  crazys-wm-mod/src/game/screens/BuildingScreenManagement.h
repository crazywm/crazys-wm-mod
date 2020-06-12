/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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
#pragma once

#ifndef CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H
#define CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H

#include "cGameWindow.h"
#include "Constants.h"

class cBrothelManager;
class cJobManager;

class IBuildingScreenManagement : public cGameWindow
{
public:
    IBuildingScreenManagement(BuildingType type, const char * base_file);

    void process() override;
protected:
    void update_image();
    void ViewSelectedGirl();
    void GetSelectedGirls(std::vector<int>* girl_array);
    void RefreshJobList();

    void add_job_filter(JOBFILTER filter);
    cJobManager& job_manager();


    int curbrothel_id = -1;		// Current Brothel text
    int girllist_id   = -1;		// Girls listbox

    int joblist_id;			// Job listbox
    int jobtypelist_id;		// Job Types listbox
    int jobdesc_id;			// Job Description text
    int jobhead_id;			// Job header text
    int jobtypedesc_id;		// Job Types Description text
    int jobtypehead_id;		// Job Types header text

    int day_id;				// Day button
    int night_id;			// Night button

    std::string jobname_with_count(JOBS job_id, bool is_night);

    sGirl* selected_girl = nullptr;

    void set_ids() override;
private:
    void OnKeyPress(SDL_Keysym keysym) override;

    int back_id;			// Back button
    int gold_id;			// Player Gold
    int girldesc_id;		// Girl Description text
    int girlimage_id;		// Girl image
    int firegirl_id;		// Fire Girl button
    int freeslave_id;		// Free Slave Button
    int sellslave_id;		// Sell Slave button
    int transfer_id;		// Transfer Girl button
    int viewdetails_id;		// View Details button

    virtual std::string update_job_description(const sGirl& girl) { return ""; };
    virtual std::string get_job_description(int selection);

    void SetShift(int shift);

    void handle_ffsd(int flag);
    void init(bool back) override;

    void RefreshSelectedJobType();

    void ffsd_outcome(int option);
    void ffsd_choice(int ffsd, std::vector<int> girl_array);
    bool is_job_allowed(JOBS job);

    BuildingType m_Type;

    sGirl* m_LastSelection;

    int m_FFSD_Flag = -1;

    std::vector<JOBFILTER> m_JobFilters;
protected:
    bool Day0Night1 = SHIFT_DAY;

    void assign_job(sGirl * girl, int new_job, int girl_selection, bool fulltime);

    void on_select_job(int selection);

    void on_select_girl(int selection);
};

struct CBuildingManagementScreenDispatch: public cInterfaceWindow
{
    void load() {};
    void process() override {}
    void init(bool back) override;
};

// the specialized screens

class cScreenArenaManagement : public IBuildingScreenManagement
{
public:
    cScreenArenaManagement();
};

class cScreenFarmManagement : public IBuildingScreenManagement
{
public:
    cScreenFarmManagement();
};

class cScreenCentreManagement : public IBuildingScreenManagement
{
private:
    std::string update_job_description(const sGirl& girl) override;
    std::string get_job_description(int selection) override;
public:
    cScreenCentreManagement();
};

class cScreenClinicManagement : public IBuildingScreenManagement
{
private:
    std::string update_job_description(const sGirl& girl) override;
    std::string get_job_description(int selection) override;
public:
    cScreenClinicManagement();
};

class cScreenGirlManagement : public IBuildingScreenManagement
{
private:
    int next_id;
    int prev_id;

    void set_ids() override;

public:
    cScreenGirlManagement();
};

class cScreenHouseManagement : public IBuildingScreenManagement
{
private:
    std::string update_job_description(const sGirl& girl) override;
public:
    cScreenHouseManagement();
};

class cScreenStudioManagement : public IBuildingScreenManagement
{
private:
    int createmovie_id;		// Create Movie

    void set_ids() override;
    std::string get_job_description(int selection) override;
public:
    cScreenStudioManagement();
};


#endif //CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H
