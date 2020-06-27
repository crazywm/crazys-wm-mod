#include <algorithm>
#include "BuildingScreenManagement.h"
#include "interface/cWindowManager.h"
#include "buildings/cBrothel.h"
#include "widgets/cListBox.h"
#include "cTariff.h"
#include "InterfaceProcesses.h"
#include "Game.hpp"
#include "buildings/queries.hpp"
#include <sstream>
#include "cJobManager.h"
#include "sConfig.h"

namespace settings {
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_ACCOMODATION_SLAVE;
}

extern cConfig cfg;
extern vector<int> cycle_girls;
extern int cycle_pos;

extern    bool            g_AltKeys;    // New hotkeys --PP
extern    bool            g_CTRLDown;


IBuildingScreenManagement::IBuildingScreenManagement(BuildingType type, const char * base_file) :
        cGameWindow(base_file), m_Type(type)
{
}


void IBuildingScreenManagement::process()
{
}

void IBuildingScreenManagement::update_image()
{
    if (selected_girl)    // Draw a girls profile picture and description when selected
    {
        bool Rand = false;
        if (m_LastSelection != selected_girl)
        {
            stringstream text;
            text << cGirls::GetGirlMood(selected_girl) << "\n \n" << selected_girl->m_Desc;
            if (cfg.debug.log_extradetails()) text << "\n \nBased on: " << selected_girl->m_Name;
            EditTextItem(text.str(), girldesc_id);
            Rand = true;
            m_LastSelection = selected_girl;
        }
        PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, Rand);
        HideWidget(girlimage_id, false);
    }
    else
    {
        EditTextItem("No Girl Selected", girldesc_id);
        HideWidget(girlimage_id, true);
    }
}

void IBuildingScreenManagement::ViewSelectedGirl()
{
    if (selected_girl)
    {
        if (selected_girl->is_dead()) return;
        //load up the cycle_girls vector with the ordered list of girl IDs
        FillSortedIDList(girllist_id, cycle_girls, cycle_pos);
        for (int i = cycle_girls.size(); i-- > 0;)
        {  // no viewing dead girls
            if (active_building().get_girl(cycle_girls[i])->is_dead())
                cycle_girls.erase(cycle_girls.begin() + i);
        }

        set_active_girl(selected_girl);
        push_window("Girl Details");
    }
}

void IBuildingScreenManagement::GetSelectedGirls(vector<int> *girl_array)
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

void IBuildingScreenManagement::RefreshJobList()
{
    ClearListBox(joblist_id);
    int job_filter = GetSelectedItemFromList(jobtypelist_id);
    if (job_filter == -1) return;
    // populate Jobs listbox with jobs in the selected category
    for (auto i : g_Game->job_manager().JobFilters.at(job_filter).Contents)
    {
        if (g_Game->job_manager().JobData[i].name.empty()) continue;
        AddToListBox(joblist_id, i, jobname_with_count((JOBS)i, Day0Night1));
    }
    if (selected_girl)
    {
        int sel_job = Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob;
        SetSelectedItemInList(joblist_id, sel_job, false);
        EditTextItem(g_Game->job_manager().JobData[sel_job].description + update_job_description(*selected_girl), jobdesc_id);
        SetSelectedItemText(joblist_id, sel_job, jobname_with_count((JOBS)sel_job, Day0Night1));
    }
}

void IBuildingScreenManagement::handle_ffsd(int flag)
{
    vector<int> girl_array;
    GetSelectedGirls(&girl_array);
    if(!girl_array.empty()) {
        ffsd_choice(flag, girl_array);
        m_FFSD_Flag = flag;
    }
}

void IBuildingScreenManagement::on_select_girl(int selection)
{
    if (selection != -1)
    {
        selected_girl = active_building().get_girl(selection);
        if (IsMultiSelected(girllist_id))
        {
            bool freefound = false;
            bool slavefound = false;
            int pos = 0;
            int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
            while (GSelection != -1)
            {
                if (active_building().get_girl(pos)->is_slave()) slavefound = true;
                if (!active_building().get_girl(pos)->is_slave()) freefound = true;
                GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
            }
            DisableWidget(firegirl_id, !freefound);
            DisableWidget(freeslave_id, !slavefound);
            DisableWidget(sellslave_id, !slavefound);
        }
        else
        {
            DisableWidget(firegirl_id, selected_girl->is_slave());
            DisableWidget(freeslave_id, selected_girl->is_free());
            DisableWidget(sellslave_id, selected_girl->is_free());
        }
        DisableWidget(viewdetails_id, false);
        RefreshSelectedJobType();
    }
    else
    {
        DisableWidget(firegirl_id, true);
        DisableWidget(freeslave_id, true);
        DisableWidget(sellslave_id, true);
        DisableWidget(viewdetails_id, true);
        selected_girl = nullptr;
    }

    update_image();
}

void IBuildingScreenManagement::set_ids() {
    back_id      = get_id("BackButton", "Back");
    girlimage_id = get_id("GirlImage");
    girldesc_id  = get_id("GirlDescription");
    girllist_id  = get_id("GirlList");

    viewdetails_id = get_id("ViewDetailsButton");
    gold_id      = get_id("Gold");
    transfer_id  = get_id("TransferButton");
    firegirl_id  = get_id("FireButton");
    freeslave_id = get_id("FreeSlaveButton");
    sellslave_id = get_id("SellSlaveButton");

    jobhead_id   = get_id("JobHeader");
    day_id       = get_id("DayButton");
    night_id     = get_id("NightButton");

    jobtypelist_id = get_id("JobTypeList");
    joblist_id     = get_id("JobList");
    jobdesc_id     = get_id("JobDescription");
    jobtypehead_id = get_id("JobTypeHeader");
    jobtypedesc_id = get_id("JobTypeDescription");
    curbrothel_id  = get_id("CurrentBrothel");


    //Set the default sort order for columns, so listbox knows the order in which data will be sent
    SortColumns(girllist_id, GetListBox(girllist_id)->GetColumnNames());

    // setting up button callbacks
    SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(viewdetails_id, [this](){
        ViewSelectedGirl();
    });
    SetButtonNavigation(transfer_id, "Transfer Screen");
    SetButtonCallback(firegirl_id, [this](){  handle_ffsd(FFSD_fire); });
    SetButtonCallback(freeslave_id, [this](){  handle_ffsd(FFSD_free); });
    SetButtonCallback(sellslave_id, [this](){  handle_ffsd(FFSD_sell); });

    SetButtonCallback(day_id, [this](){ SetShift(SHIFT_DAY); });
    SetButtonCallback(night_id, [this](){ SetShift(SHIFT_NIGHT); });
    SetButtonHotKey(day_id, SDLK_z);
    SetButtonHotKey(night_id, SDLK_c);

    SetListBoxSelectionCallback(jobtypelist_id, [this](int selection) {
        if (selection == -1) EditTextItem("Nothing Selected", jobtypedesc_id);
        else
        {
            RefreshJobList();    // populate Jobs listbox with jobs in the selected category
            EditTextItem(get_job_description(selection), jobtypedesc_id);
        }
    });
    SetListBoxHotKeys(jobtypelist_id, SDLK_w, SDLK_s);

    SetListBoxSelectionCallback(joblist_id, [this](int selection) { on_select_job(selection); });
    SetListBoxSelectionCallback(girllist_id, [this](int selection) { on_select_girl(selection); });
    SetListBoxDoubleClickCallback(girllist_id, [this](int sel){
        ViewSelectedGirl();
    });
    SetListBoxHotKeys(girllist_id, g_AltKeys ? SDLK_a : SDLK_UP, g_AltKeys ? SDLK_d : SDLK_DOWN);
}

void IBuildingScreenManagement::on_select_job(int selection)
{
    bool fulltime = g_CTRLDown;
    if (selection != -1)
    {
        EditTextItem(job_manager().JobData[selection].description, jobdesc_id);        // first handle the descriptions
        int pos = 0;
        int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);        // Now assign the job to all the selected girls
        while (GSelection != -1)
        {
            // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
            int new_job = selection;
            auto girl = active_building().get_girl(GSelection);
            if (girl)
            {
                assign_job(girl, new_job, GSelection, fulltime);
            }
            GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
        }
    }
    else EditTextItem("Nothing Selected", jobdesc_id);
}

void IBuildingScreenManagement::assign_job(sGirl * girl, int new_job, int girl_selection, bool fulltime)
{
    // handle special job requirements and assign
    unsigned int old_job   = Day0Night1 ? girl->m_NightJob : girl->m_DayJob;
    
    // if HandleSpecialJobs returns true, the job assignment was modified or cancelled
    if (job_manager().HandleSpecialJobs(girl, new_job, old_job, Day0Night1, fulltime))
    {
        // TODO WHAT HAPPENS HERE?
        //new_job = Day0Night1 ? night_job : day_job;
        SetSelectedItemInList(joblist_id, new_job, false);
    }

    unsigned int day_job   = girl->m_DayJob;
    unsigned int night_job = girl->m_NightJob;
    stringstream ss;
    // update the girl's listing to reflect the job change
    ss << job_manager().JobData[day_job].name;
    SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    ss.str("");
    ss << job_manager().JobData[night_job].name;
    SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");

    // refresh job worker counts for former job and current job
    SetSelectedItemText(joblist_id, old_job, jobname_with_count((JOBS)old_job, Day0Night1));
    SetSelectedItemText(joblist_id, new_job, jobname_with_count((JOBS)new_job, Day0Night1));

    // handle rehab
    if (day_job == JOB_REHAB)    // `J` added
    {
        ss.str("");    ss << job_manager().JobData[day_job].name << " (" << 3 - girl->m_WorkingDay << ")";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if (girl->m_YesterDayJob == JOB_REHAB && ((girl->m_WorkingDay > 0) || girl->m_PrevWorkingDay > 0))
    {
        ss.str("");    ss << job_manager().JobData[day_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }

    if (night_job == JOB_REHAB)    // `J` added
    {
        ss.str("");    ss << job_manager().JobData[night_job].name << " (" << 3 - girl->m_WorkingDay << ")";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
    else if (girl->m_YesterNightJob == JOB_REHAB && ((girl->m_WorkingDay > 0) || girl->m_PrevWorkingDay > 0))
    {
        ss.str("");    ss << job_manager().JobData[night_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }

    if (girl->m_YesterDayJob == JOB_REHAB && new_job != JOB_REHAB && (girl->m_WorkingDay > 0 || girl->m_PrevWorkingDay > 0))
    {    // `J` added
        ss.str("");    ss << job_manager().JobData[new_job].description << "\n** This girl was in Rehab, if you send her somewhere else, she will have to start her Rehab over.";
        EditTextItem(ss.str(), jobdesc_id);
    }

    // handle surgeries
    bool interrupted = false;    // `J` added
    if (girl->m_YesterDayJob != day_job && cJobManager::is_Surgery_Job(girl->m_YesterDayJob) && ((girl->m_WorkingDay > 0) || girl->m_PrevWorkingDay > 0))
        interrupted = true;

    if (day_job == JOB_CUREDISEASES)    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[day_job].name << " (" << girl->m_WorkingDay << "%)*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if (day_job == JOB_GETABORT)    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[day_job].name << " (" << 2 - girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if (cJobManager::is_Surgery_Job(day_job))    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[day_job].name << " (" << 5 - girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if (interrupted)
    {
        ss.str(""); ss << job_manager().JobData[day_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }

    if (night_job == JOB_CUREDISEASES)    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[night_job].name << " (" << girl->m_WorkingDay << "%)*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if (night_job == JOB_GETABORT)    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[night_job].name << " (" << 2 - girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
    else if (cJobManager::is_Surgery_Job(night_job))    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[night_job].name << " (" << 5 - girl->m_WorkingDay << ")*" << (interrupted ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
    else if (interrupted)
    {
        ss.str(""); ss << job_manager().JobData[night_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
    if (interrupted)
    {    // `J` added
        ss.str(""); ss << job_manager().JobData[new_job].description << "\n** This girl was getting ";
        if (girl->m_YesterDayJob == JOB_CUREDISEASES)
        {
            ss << "her disease cured, if you send her somewhere else, she will have to start her treatment over.";
        }
        else
        {
            if (girl->m_YesterDayJob == JOB_BOOBJOB || girl->m_YesterDayJob == JOB_FACELIFT)        ss << "a ";
            else if (girl->m_YesterDayJob == JOB_GETABORT || girl->m_YesterDayJob == JOB_ASSJOB)    ss << "an ";
            else if (girl->m_YesterDayJob == JOB_TUBESTIED)/*                                          */    ss << "her ";
            ss << job_manager().JobData[girl->m_YesterDayJob].name << ", if you send her somewhere else, she will have to start her Surgery over.";
        }
        EditTextItem(ss.str(), jobdesc_id);
    }

    // conversions
    if (day_job == JOB_SO_STRAIGHT || day_job == JOB_SO_BISEXUAL || day_job == JOB_SO_LESBIAN || day_job == JOB_FAKEORGASM)    // `J` added
    {
        ss.str("");    ss << job_manager().JobData[day_job].name << " (" << girl->m_WorkingDay << "%)";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }
    else if ((girl->m_WorkingDay > 0 || girl->m_PrevWorkingDay > 0) && (
            girl->m_YesterDayJob == JOB_SO_STRAIGHT ||
            girl->m_YesterDayJob == JOB_SO_BISEXUAL ||
            girl->m_YesterDayJob == JOB_SO_LESBIAN ||
            girl->m_YesterDayJob == JOB_FAKEORGASM))
    {
        ss.str("");    ss << job_manager().JobData[day_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "DayJob");
    }

    if (night_job == JOB_SO_STRAIGHT || night_job == JOB_SO_BISEXUAL || night_job == JOB_SO_LESBIAN || night_job == JOB_FAKEORGASM)    // `J` added
    {
        ss.str("");    ss << job_manager().JobData[night_job].name << " (" << girl->m_WorkingDay << "%)";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
    else if ((girl->m_WorkingDay > 0 || girl->m_PrevWorkingDay > 0) && (
            girl->m_YesterNightJob == JOB_SO_STRAIGHT ||
            girl->m_YesterNightJob == JOB_SO_BISEXUAL ||
            girl->m_YesterNightJob == JOB_SO_LESBIAN ||
            girl->m_YesterNightJob == JOB_FAKEORGASM))
    {
        ss.str("");    ss << job_manager().JobData[night_job].name << " **";
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }

    if ((girl->m_WorkingDay > 0 || girl->m_PrevWorkingDay > 0) && (new_job != girl->m_YesterDayJob && (
            girl->m_YesterDayJob == JOB_SO_STRAIGHT ||
            girl->m_YesterDayJob == JOB_SO_BISEXUAL ||
            girl->m_YesterDayJob == JOB_SO_LESBIAN ||
            girl->m_YesterDayJob == JOB_FAKEORGASM)))
    {    // `J` added
        ss.str("");    ss << job_manager().JobData[new_job].description <<
                          "\n** This girl was in training for " << job_manager().JobData[girl->m_YesterDayJob].name
                          << ", if you send her somewhere else, she will have to start her training over.";
        EditTextItem(ss.str(), jobdesc_id);
    }

    // actress
    if (is_Actress_Job(night_job))    // `J` added
    {
        ss.str(""); ss << job_manager().JobData[night_job].name << (CrewNeeded(*girl->m_Building) ? " **" : "");
        SetSelectedItemColumnText(girllist_id, girl_selection, ss.str(), "NightJob");
    }
}

void IBuildingScreenManagement::init(bool back)
{
    Focused();
    selected_girl = nullptr;

    EditTextItem(active_building().name(), curbrothel_id);
    int selection = GetSelectedItemFromList(girllist_id);

    if (gold_id >= 0)
    {
        std::stringstream ss;
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }

    // clear the lists
    ClearListBox(girllist_id);
    ClearListBox(jobtypelist_id);

    // get a list of all the column names, so we can find which data goes in that column
    vector<string> columnNames = GetListBox(girllist_id)->GetColumnNames();
    int numColumns = columnNames.size();
    std::vector<std::string> data(numColumns);

    for (int i = 0; i < active_building().num_girls(); i++)    // Add girls to list
    {
        sGirl* gir = active_building().get_girl(i);
        if (selected_girl == gir) selection = i;
        unsigned int item_color = (gir->health() <= 30 || gir->tiredness() >= 80 || gir->happiness() <= 30) ? COLOR_RED : COLOR_BLUE;
        gir->OutputGirlRow(data, columnNames);
        AddToListBox(girllist_id, i, data, item_color);
    }

    DisableWidget(firegirl_id, true);
    DisableWidget(freeslave_id, true);
    DisableWidget(sellslave_id, true);
    DisableWidget(viewdetails_id, true);

    // add the job filters
    for(auto filter : m_JobFilters) {
        AddToListBox(jobtypelist_id, filter, g_Game->job_manager().JobFilters[filter].Name);
    }
    SetSelectedItemInList(jobtypelist_id, m_JobFilters.front());

    if (selection >= 0) while (selection > GetListBoxSize(girllist_id) && selection != -1) selection--;
    SetSelectedItemInList(girllist_id, selection >= 0 ? selection : 0);

    DisableWidget(day_id, Day0Night1 == SHIFT_DAY);
    DisableWidget(night_id, Day0Night1 == SHIFT_NIGHT);

    update_image();
}

static vector<int> ffsd_choicelist;

void IBuildingScreenManagement::ffsd_outcome(int selected)
{
    vector<int> girl_array;
    GetSelectedGirls(&girl_array);

    bool free = false, fire = false, sell = false, dump = false;
    int option = ffsd_choicelist[selected];
    switch (option)
    {
    case FFSD_fire:        fire = true;    break;

    case FFSD_free1:    // Throw a freedom party
    case FFSD_free:        free = true;    break;

    case FFSD_sell:        sell = true;    break;

    case FFSD_fidu:        fire = true;    dump = true;    break;

    case FFSD_fise:        fire = true;    sell = true;    break;

    case FFSD_fisd:        free = true;    sell = true;    dump = true;    break;

    case FFSD_frdu1:    // dump the bodies and get your freedom
    case FFSD_frdu:        free = true;    dump = true;    break;

    case FFSD_sedu1:    // Sell all the girls, living and dead
    case FFSD_sedu:        sell = true;    dump = true;    break;

    case FFSD_dump1:    // Proper funeral
    case FFSD_dump2:    // Unmarked grave
    case FFSD_dump3:    // Side of the road
    case FFSD_dump4:    // Sell the bodies
    case FFSD_frdu2:    // Dump then get back to work
    case FFSD_dump:        dump = true;    break;


    default:    break;    // keep
    }

    if (free || fire || sell || dump)
    {
        stringstream ss;
        vector<int>    sellgirl_price;
        vector<string> freegirl_names; vector<string> firegirl_names; vector<string> sellgirl_names; vector<string> dumpgirl_names;

        for (int i = girl_array.size(); i-- > 0;)    // OK, we have the array, now step through it backwards
        {
            selected_girl = active_building().get_girl(girl_array[i]);

            if (selected_girl->is_dead())
            {
                if (dump)
                {
                    dumpgirl_names.push_back(selected_girl->FullName());
                    active_building().remove_girl(selected_girl);
                    // dead girls get removed from the game
                    delete selected_girl;
                    selected_girl = nullptr;
                }
                continue;
            }
            else if (selected_girl->is_slave())
            {
                if (option == FFSD_free1)    // Throw a freedom party
                {
                    freegirl_names.push_back(selected_girl->FullName());
                    selected_girl->remove_status(STATUS_SLAVE);
                    g_Game->player().disposition(7);
                    selected_girl->pclove(20);
                    selected_girl->pcfear(-40);
                    selected_girl->pchate(-50);
                    selected_girl->obedience(10);
                    selected_girl->happiness(100);
                    selected_girl->health(10);
                    selected_girl->tiredness(10);
                    selected_girl->charisma(5);
                    selected_girl->fame(2);
                    selected_girl->confidence(5);
                    selected_girl->obedience(10);
                    selected_girl->dignity(5);
                    selected_girl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
                    selected_girl->set_default_house_percent();
                }
                if (option == FFSD_frdu1)    // dump the bodies and get your freedom
                {
                    firegirl_names.push_back(selected_girl->FullName());
                    selected_girl->remove_status(STATUS_SLAVE);
                    g_Game->player().disposition(3);
                    selected_girl->pclove(5);
                    selected_girl->pcfear(5);
                    selected_girl->pchate(-5);
                    selected_girl->obedience(20);
                    selected_girl->happiness(40);
                    selected_girl->tiredness(5);
                    selected_girl->dignity(-2);
                    selected_girl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE);
                    selected_girl->set_default_house_percent();
                }
                else if (option == FFSD_frdu2)    // dump the bodies then get back to work
                {
                    freegirl_names.push_back(selected_girl->FullName());
                    g_Game->player().disposition(-1);
                    selected_girl->pclove(-2);
                    selected_girl->pcfear(10);
                    selected_girl->pchate(5);
                    selected_girl->obedience(5);
                    selected_girl->happiness(-10);
                    selected_girl->tiredness(5);
                    selected_girl->dignity(-5);
                }
                else if (free)
                {
                    freegirl_names.push_back(selected_girl->FullName());
                    selected_girl->remove_status(STATUS_SLAVE);
                    g_Game->player().disposition(5);
                    selected_girl->pclove(10);
                    selected_girl->pcfear(-20);
                    selected_girl->pchate(-25);
                    selected_girl->obedience(10);
                    selected_girl->happiness(70);

                    selected_girl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
                    selected_girl->set_default_house_percent();
                }
                else if (sell)
                {
                    sellgirl_names.push_back(selected_girl->FullName());
                    sellgirl_price.push_back(g_Game->tariff().slave_sell_price(selected_girl));
                    g_Game->gold().slave_sales(g_Game->tariff().slave_sell_price(selected_girl));

                    active_building().remove_girl(selected_girl);

                    if (selected_girl->FullName() == selected_girl->m_Name)
                        g_Game->girl_pool().AddGirl(selected_girl);  // add unique girls back to main pool
                    else
                    {  // random girls simply get removed from the game
                        delete selected_girl;
                        selected_girl = nullptr;
                    }
                }
                continue;
            }
            else if (selected_girl)        // the girl is free
            {
                if (option == FFSD_free1)    // Throw a freedom party - Guest
                {
                    selected_girl->pclove(5);
                    selected_girl->pcfear(-5);
                    selected_girl->pchate(-5);
                    selected_girl->obedience(2);
                    selected_girl->happiness(100);
                    selected_girl->health(10);
                    selected_girl->tiredness(10);
                    selected_girl->confidence(2);
                    selected_girl->spirit(5);
                    selected_girl->dignity(1);
                }
                else if (fire)
                {
                    firegirl_names.push_back(selected_girl->FullName());
                    active_building().remove_girl(selected_girl);

                    if (selected_girl->FullName() == selected_girl->m_Name)
                       g_Game->girl_pool().AddGirl(selected_girl);  // add unique girls back to main pool
                    else
                    {  // random girls simply get removed from the game
                        delete selected_girl;
                        selected_girl = nullptr;
                    }
                }
            }
        }

        if (!freegirl_names.empty())
        {
            ss << "You grant " << freegirl_names[0];
            if (freegirl_names.size() == 1)            ss << " her";
            else if (freegirl_names.size() == 2)    ss << " and " << freegirl_names[1] << " thier";
            else
            {
                for (int i = 1; i < (int)freegirl_names.size() - 1; i++) ss << ", " << freegirl_names[i];
                ss << " and " << freegirl_names[freegirl_names.size() - 1] << " their";
            }
            ss << " freedom.\n \n";
        }
        if (!firegirl_names.empty())
        {
            ss << "You fire " << firegirl_names[0];
            if (firegirl_names.size() == 2)    ss << " and " << firegirl_names[1];
            else if (firegirl_names.size() > 2)
            {
                for (int i = 1; i < (int)firegirl_names.size() - 1; i++) ss << ", " << firegirl_names[i];
                ss << " and " << firegirl_names[firegirl_names.size() - 1];
            }
            ss << ".\n \n";
        }
        if (!sellgirl_names.empty())
        {
            int sell = 0;
            int sellsize = sellgirl_names.size();
            ss << "You sell ";
            if (sellsize == 1)        ss << sellgirl_names[0] << " for " << sellgirl_price[0] << " gold";
            else if (sellsize == 2)    ss << "two slaves:\n" << sellgirl_names[0] << " for " << sellgirl_price[0] << " gold and\n" << sellgirl_names[1] << " for " << sellgirl_price[0] << " gold";
            else
            {
                ss << sellsize << " slaves:\n" << sellgirl_names[0] << " for " << sellgirl_price[0] << " gold";
                for (int i = 1; i < (int)sellsize - 1; i++)
                {
                    ss << ",\n" << sellgirl_names[i] << " for " << sellgirl_price[i] << " gold";
                }
                ss << " and\n" << sellgirl_names[sellsize - 1] << " for " << sellgirl_price[sellsize - 1] << " gold";
            }
            if (sellsize > 1)
            {
                for (int i = 0; i < (int)sellsize; i++) sell += sellgirl_price[i];
                ss << ".\nYour total take was " << sell << " gold";
            }
            ss << ".\n \n";
        }
        if (!dumpgirl_names.empty())
        {
            int sell = 0;
            int dumpsize = dumpgirl_names.size();
            switch (option)
            {
            case FFSD_dump1:    // proper funeral
                if (dumpsize == 1) ss << "You spend 100 gold for a proper funeral for ";
                else ss << "You spend 100 gold each for proper funerals for ";
                active_building().m_Finance.building_upkeep(100 * dumpsize);

                g_Game->player().customerfear(-dumpsize);
                g_Game->player().suspicion(-dumpsize);
                g_Game->player().disposition(dumpsize);
                break;
            case FFSD_dump2:    // unmarked grave
                ss << "You have your goons dig " << (dumpsize > 1 ? "graves " : "a grave") << " for ";
                g_Game->player().disposition(-dumpsize);
                break;
            case FFSD_dump3:    // side of the road
                ss << "You have your goons dump the bod" << (dumpsize > 1 ? "ies" : "y") << " of ";
                g_Game->player().customerfear(dumpsize);
                g_Game->player().suspicion(dumpsize);
                g_Game->player().disposition(-dumpsize);
                break;
            case FFSD_dump4:    // Sell the bodies
            case FFSD_sedu1:    // Sell all the girls, living and dead
                for (int i = 0; i < (int)dumpsize; i++) sell += g_Dice % 250 + g_Dice % 50 + 1;
                ss << "You make " << sell << " gold for selling the dead bod" << (dumpsize > 1 ? "ies" : "y") << " of ";
                active_building().m_Finance.slave_sales(sell);

                g_Game->player().customerfear(dumpsize * 2);
                g_Game->player().suspicion(dumpsize * 2);
                g_Game->player().disposition(-dumpsize * 2);
                break;
            default:
                ss << "You have your goons remove the bod" << (dumpsize > 1 ? "ies" : "y") << " of ";
                break;
            }
            ss << dumpgirl_names[0];
            if (dumpsize == 2)    ss << " and " << dumpgirl_names[1];
            else
            {
                for (int i = 1; i < (int)dumpsize - 1; i++) ss << ", " << dumpgirl_names[i];
                ss << " and " << dumpgirl_names[dumpsize - 1];
            }
            if (option == FFSD_dump2) ss << " and dump their bod" << (dumpsize > 1 ? "ies" : "y") << " in.";
            if (option == FFSD_dump3) ss << " on the side of the road.";
            ss << ".\n \n";
        }
        if (ss.str().length() > 0)    g_Game->push_message(ss.str(), 0);

        freegirl_names.clear(); firegirl_names.clear(); sellgirl_names.clear(); dumpgirl_names.clear();
        sellgirl_price.clear();
    }
}

void IBuildingScreenManagement::ffsd_choice(int ffsd, vector<int> girl_array) // `J` added for .06.02.37
{
    int slavegirls = 0, freegirls = 0, deadgirls = 0, selltotal = 0;
    stringstream firstgirlname;

    for (int i = girl_array.size(); i-- > 0;)    // OK, we have the array, now step through it backwards
    {
        selected_girl = active_building().get_girl(girl_array[i]);

        if (firstgirlname.str().length() == 0)    firstgirlname << selected_girl->FullName();

        if (selected_girl->is_dead())    deadgirls++;
        else if (selected_girl->is_slave())
        {
            slavegirls++;
            selltotal += g_Game->tariff().slave_sell_price(selected_girl);
        }
        else if (selected_girl)                freegirls++;
    }

    int totalgirls = freegirls + slavegirls + deadgirls;
    if (totalgirls == 0) return;                            // No girls so quit

    stringstream ask, question, keep, fire, free, dump, sell, dump1, dump2, dump3, dump4,
                 frdu1, frdu2, fidu, fise, fisd, frdu, sedu, sedu1, free1;
    keep << "\"Nevermind, Back to work.\"";

    /* Free girls only */
    if (freegirls > 0 && slavegirls == 0 && deadgirls == 0)
    {
        question << "Do you want to fire ";
        if (freegirls == 1)    { question << firstgirlname.str();                    fire << "Fire her."; }
        else if (freegirls == 2)    { question << "these two girls";                    fire << "Fire them both."; }
        else    { question << "these " << totalgirls << " girls";    fire << "Fire them all."; }
        question << "?";
    }
        /* Slave girls only */
    else if (freegirls == 0 && slavegirls > 0 && deadgirls == 0)
    {
        if (ffsd == FFSD_free && g_Game->player().disposition() > -10)
        {
            question << "Do you want to free ";
            /* */if (slavegirls == 1)    { question << firstgirlname.str();    free << "Free her."; }
            else if (slavegirls == 2)    { question << "these two girls";    free << "Free them both."; }
            else        { question << "these " << totalgirls << " girls";    free << "Free them all."; }
            question << "?";
        }
        else if (ffsd == FFSD_sell && g_Game->player().disposition() < 10)
        {
            question << "Do you want to sell ";
            /* */if (slavegirls == 1)    { question << firstgirlname.str();        sell << "Sell her."; }
            else if (slavegirls == 2)    { question << "these two girls";        sell << "Sell them both."; }
            else        { question << "these " << totalgirls << " girls";        sell << "Sell them all."; }
            question << "?\nYou could get " << selltotal << " gold for selling them.";
        }
        else
        {
            question << "Do you want to free or sell ";
            /* */if (slavegirls == 1)    { question << firstgirlname.str();        free << "Free her.";        sell << "Sell her."; }
            else if (slavegirls == 2)    { question << "these two girls";        free << "Free them both.";    sell << "Sell them both."; }
            else        { question << "these " << totalgirls << " girls";        free << "Free them all.";    sell << "Sell them all."; }
            question << "?\nYou could get " << selltotal << " gold for selling them.";
        }
    }
        /* Dead girls only */
    else if (freegirls == 0 && slavegirls == 0 && deadgirls > 0)
    {
        question << "What do you want to do with ";
        /* */if (deadgirls == 1)    question << firstgirlname.str() << "'s dead body";
        else if (deadgirls == 2)    question << "the two dead bodies";
        else/*                */    question << "the " << totalgirls << " dead bodies";
        question << "?";
        keep << "Nevermind, I'll deal with them later.";
        dump1 << "Give " << (deadgirls == 1 ? "her" : "them") << " a proper funeral.";
        dump2 << "Bury the bod" << (deadgirls == 1 ? "y" : "ies") << " in a shollow unmarked grave.";
        dump3 << "Dump the bod" << (deadgirls == 1 ? "y" : "ies") << " on the side of the road.";
        dump4 << "Sell the bod" << (deadgirls == 1 ? "y" : "ies") << " to the highest bidder" << (deadgirls == 1 ? "" : "s") << ".";
    }
        /* Slave and Dead girls */
    else if (freegirls == 0 && slavegirls > 0 && deadgirls > 0)
    {
        if (ffsd == FFSD_free)
        {
            question << "Free slaves or dispose bodies?";
            ask << "You have chosen to free ";
            if (slavegirls == 1) ask << "a"; else if (slavegirls == 2) ask << "two"; else ask << slavegirls;
            ask << " slave girl" << (slavegirls > 1 ? "s" : "") << ". Do you want them to:\nDispose of the bodies of the dead girl"
                << (deadgirls > 1 ? "s" : "") << " as their last act as your slave" << (slavegirls > 1 ? "s?" : "?")
                << "\nDispose of the bodies and get back to work?";

            if (g_Game->player().disposition() < -10)
            {
                ask << "\nOr you can sell all the girls, living and dead.";
                sedu1 << "\"Show me the MONEY\"";
            }
            free << "Free them and deal with the bod" << (deadgirls > 1 ? "ies" : "y") << " later.";
            frdu1 << "\"Dispose of them and you get your freedom.\"";
            frdu2 << "\"Dispose of them then get back to work.\"";

        }
        else    // fire or sell buttons
        {
            question << "Sell who?";
            ask << "You have chosen to sell ";
            if (slavegirls == 1) ask << "a"; else if (slavegirls == 2) ask << "two"; else ask << slavegirls;
            ask << " living slave girl" << (slavegirls > 1 ? "s" : "") << " and ";
            if (deadgirls == 1) ask << "a"; else if (deadgirls == 2) ask << "two"; else ask << deadgirls;
            ask << " dead bod" << (deadgirls > 1 ? "ies" : "y") << ".";
            ask << "\nYou could get " << selltotal << " gold for selling the living girl" << (slavegirls > 1 ? "s" : "") << ".";

            sell << "Sell just the living.";
            dump4 << "Sell just the dead.";
            sedu1 << "Sell them all.";
        }
    }
        /* Free and Slave girls */
    else if (freegirls > 0 && slavegirls > 0 && deadgirls == 0)
    {
        ask << "You call in " << totalgirls << " girls, " << slavegirls << " slave girl" << (slavegirls > 1 ? "s" : "") << " and " << freegirls << " free girl" << (freegirls > 1 ? "s" : "") << ".\n";
        if (ffsd == FFSD_free)
        {
            ask << "Do you want to throw them a Freedom Party or just get it over with quickly?";
            question << "Is it Party Time?";
            free << "Just free " << (slavegirls > 1 ? "them" : "her") << ".";
            free1 << "Throw a Freedom Party!";
        }
        else    // fire or sell
        {
            ask << "Do you want to make room for new girls by firing or selling some old girls?";
            question << "Get rid of who?";
            fire << "Fire the free girl" << (freegirls > 1 ? "s" : "") << ".";
            sell << "Sell the slave" << (slavegirls > 1 ? "s" : "") << ".";
            fise << "Get rid of them all.";
        }

    }
        /* Any girls */
    else
    {
        ask << "You have chosen " << totalgirls << " girls, what do you want to do with them?";
        question << "What do you want to do?";
        fire << "Fire the free girls.";
        free << "Free the slaves.";
        sell << "Sell the slaves.";
        dump << "Dump the bodies.";
        fisd << "Get rid of them all.";
    }

    std::array<std::stringstream*, FFSD_COUNT> options = {
            &keep, &fire, &free, &sell, &dump, &fidu, &fise, &fisd, &frdu, &sedu, &dump1, &dump2, &dump3, &dump4,
            &frdu1, &frdu2, &sedu1, &free1
    };

    ffsd_choicelist.clear();
    std::vector<std::string> choices;
    choices.reserve(FFSD_COUNT);
    for(unsigned i = 0; i < options.size(); ++i) {
        // has the option been used?
        if(options[i]->tellp() != std::streampos(0)) {
            ffsd_choicelist.push_back(i);
            choices.push_back(options[i]->str());
        }
    }
    if (ask.str().length() > 0)    g_Game->push_message(ask.str(), 0);
    input_choice(question.str(), std::move(choices), [this](int selected){ ffsd_outcome(selected); });
}

std::string IBuildingScreenManagement::jobname_with_count(JOBS job_id, bool is_night)
{
    stringstream text;
    text << g_Game->job_manager().JobData[job_id].name;
    text << " (" << active_building().num_girls_on_job(job_id, is_night) << ")";
    return text.str();
}

void IBuildingScreenManagement::add_job_filter(JOBFILTER filter)
{
    m_JobFilters.push_back(filter);
}

void IBuildingScreenManagement::RefreshSelectedJobType()
{
    int selection = GetSelectedItemFromList(girllist_id);
    if (selection < 0) return;
    selected_girl = active_building().get_girl(selection);
    int job = (Day0Night1 ? selected_girl->m_NightJob : selected_girl->m_DayJob);
    for(auto& filter : m_JobFilters) {
        if(job_manager().job_filter(filter, (JOBS)job)) {
            SetSelectedItemInList(jobtypelist_id, filter);
            return;
        }
    }
    SetSelectedItemInList(jobtypelist_id, m_JobFilters.back());
}

cJobManager& IBuildingScreenManagement::job_manager()
{
    return g_Game->job_manager();
}

void IBuildingScreenManagement::OnKeyPress(SDL_Keysym keysym)
{
    auto key = keysym.sym;

    // girl list
    if (key == SDLK_SPACE || key == SDLK_KP_ENTER)    {
        ViewSelectedGirl();
    }

    else if (key == SDLK_q || key == SDLK_e) {
        int selection = -1;
        if (key == SDLK_q) selection = ArrowUpListBox(joblist_id);
        if (key == SDLK_e) selection = ArrowDownListBox(joblist_id);

        if (!is_job_allowed((JOBS) selection)) {
            if (key == SDLK_q) ArrowUpListBox(joblist_id);
            if (key == SDLK_e) ArrowDownListBox(joblist_id);
            // the purpose of this is to clear the extra event from the event queue, which prevents an error --PP
            /// TODO figure this out!
            // g_InterfaceEvents.CheckListbox(joblist_id);
        }
    }

    cInterfaceWindowXML::OnKeyPress(keysym);

}

void IBuildingScreenManagement::SetShift(int shift)
{
    Day0Night1 = shift;
    DisableWidget(day_id, shift == SHIFT_DAY);
    DisableWidget(night_id, shift == SHIFT_NIGHT);
    RefreshSelectedJobType();
}

bool IBuildingScreenManagement::is_job_allowed(JOBS job)
{
    JOBS free_jobs[] = {
            JOB_DOCTORE, JOB_CITYGUARD, JOB_CENTREMANAGER, JOB_COUNSELOR, JOB_CHAIRMAN,
            JOB_DOCTOR, JOB_FARMMANGER, JOB_MARKETER, JOB_MATRON, JOB_TORTURER,
            JOB_HEADGIRL, JOB_RECRUITER, JOB_DIRECTOR, JOB_PROMOTER
    };

    // prohibit free jobs for slave girls
    if (selected_girl && selected_girl->is_slave()) {
        for(auto& fj : free_jobs) {
            if(job == fj) return false;
        }
    }

    JOBS slave_jobs[] = {
            JOB_HOUSEPET
    };

    // prohibit free jobs for slave girls
    if (selected_girl && selected_girl->is_free()) {
        for(auto& fj : slave_jobs) {
            if(job == fj) return false;
        }
    }

    JOBS singleton_jobs[] = {
            JOB_CENTREMANAGER, JOB_DOCTORE, JOB_CHAIRMAN, JOB_FARMMANGER, JOB_MARKETER,
            JOB_MATRON, JOB_TORTURER, JOB_HEADGIRL, JOB_DIRECTOR, JOB_PROMOTER
    };

    for(auto& sj : singleton_jobs) {
        if(job == sj){
            if(active_building().num_girls_on_job(sj, 0) > 0 ||
               active_building().num_girls_on_job(sj, 1) > 0) {
                return false;
            }
        }
    }

    // special cases
    if (selected_girl && selected_girl->has_active_trait("AIDS") && (job == JOB_DOCTOR || job == JOB_NURSE || job == JOB_INTERN))
        return false;

    return true;
}

std::string IBuildingScreenManagement::get_job_description(int selection)
{
    return job_manager().JobFilters[selection].Description;
}

void CBuildingManagementScreenDispatch::init(bool back)
{
    auto bt = window_manager().GetActiveBuilding()->type();
    switch(bt) {
    case BuildingType ::BROTHEL:
        replace_window("Girl Management Brothel");
        break;
    case BuildingType::STUDIO:
        replace_window("Girl Management Studio");
        break;
    case BuildingType::CLINIC:
        replace_window("Girl Management Clinic");
        break;
    case BuildingType::ARENA:
        replace_window("Girl Management Arena");
        break;
    case BuildingType::CENTRE:
        replace_window("Girl Management Centre");
        break;
    case BuildingType::HOUSE:
        replace_window("Girl Management House");
        break;
    case BuildingType::FARM:
        replace_window("Girl Management Farm");
        break;
    }
}

CBuildingManagementScreenDispatch::CBuildingManagementScreenDispatch() :
    cInterfaceWindow("BuildingManagementDispatch") {
}


cScreenArenaManagement::cScreenArenaManagement() :
        IBuildingScreenManagement(BuildingType::ARENA, "arena_management_screen.xml")
{
    add_job_filter(JOBFILTER_ARENASTAFF);
    add_job_filter(JOBFILTER_ARENA);
}

// --------------------------------------------------------------------
//                  Farm
// --------------------------------------------------------------------

cScreenFarmManagement::cScreenFarmManagement() :
        IBuildingScreenManagement(BuildingType::FARM, "farm_management_screen.xml")
{
    add_job_filter(JOBFILTER_FARMSTAFF);
    add_job_filter(JOBFILTER_LABORERS);
    add_job_filter(JOBFILTER_PRODUCERS);
}



// --------------------------------------------------------------------
//                  Centre
// --------------------------------------------------------------------

cScreenCentreManagement::cScreenCentreManagement() :
        IBuildingScreenManagement(BuildingType::CENTRE, "centre_management_screen.xml")
{
    add_job_filter(JOBFILTER_COMMUNITYCENTRE);
    add_job_filter(JOBFILTER_COUNSELINGCENTRE);
}

std::string cScreenCentreManagement::update_job_description(const sGirl& girl)
{
    if (girl.m_YesterDayJob == JOB_REHAB && girl.m_DayJob != JOB_REHAB && (girl.m_WorkingDay > 0 || girl.m_PrevWorkingDay > 0))
    {
        return "\n** This girl was in Rehab, if you send her somewhere else, she will have to start her Rehab over.";
    }
    return "";
}

std::string cScreenCentreManagement::get_job_description(int selection)
{
    stringstream jdmessage; jdmessage << job_manager().JobFilters[selection].Description;
    auto& centre = active_building();
    if ((centre.num_girls_on_job(JOB_COUNSELOR, 0) < 1 && Num_Patients(centre, 0) > 0) ||
         (centre.num_girls_on_job(JOB_COUNSELOR, 1) < 1 && Num_Patients(centre, 1) > 0))
        jdmessage << "\n*** A Counselor is required to guide Rehab and Therapy patients. ";
    return jdmessage.str();
}

// --------------------------------------------------------------------
//                  Clinic
// --------------------------------------------------------------------

cScreenClinicManagement::cScreenClinicManagement() :
        IBuildingScreenManagement(BuildingType::CLINIC, "clinic_management_screen.xml")
{
    add_job_filter(JOBFILTER_CLINICSTAFF);
    add_job_filter(JOBFILTER_CLINIC);
}

std::string cScreenClinicManagement::update_job_description(const sGirl& girl)
{
    if(cJobManager::is_Surgery_Job(girl.m_YesterDayJob) && girl.m_YesterDayJob != girl.m_DayJob && (girl.m_WorkingDay > 0 || girl.m_PrevWorkingDay > 0))
    {
        int sel_job = (Day0Night1 ? girl.m_NightJob : girl.m_DayJob);
        SetSelectedItemInList(joblist_id, sel_job, false);

        std::stringstream ss;
        ss  << "\n** This girl was getting ";
        if (girl.m_YesterDayJob == JOB_CUREDISEASES)
        {
            ss << "her disease cured, if you send her somewhere else, she will have to start her treatment over.";
        }
        else
        {
            if (girl.m_YesterDayJob == JOB_BOOBJOB || girl.m_YesterDayJob == JOB_FACELIFT)        ss << "a ";
            else if (girl.m_YesterDayJob == JOB_GETABORT || girl.m_YesterDayJob == JOB_ASSJOB)    ss << "an ";
            else if (girl.m_YesterDayJob == JOB_TUBESTIED)/*                                          */    ss << "her ";
            ss << job_manager().JobData[girl.m_YesterDayJob].name << ", if you send her somewhere else, she will have to start her Surgery over.";
        }
        return ss.str();
    }
    return "";
}

std::string cScreenClinicManagement::get_job_description(int selection)
{
    stringstream jdmessage; jdmessage << job_manager().JobFilters[selection].Description;
    if (DoctorNeeded(active_building()))
        jdmessage << "\n*** A Doctor is required to perform any surgeries. ";
    if ((active_building().num_girls_on_job(JOB_MECHANIC, SHIFT_DAY) < 1 &&
         active_building().num_girls_on_job(JOB_GETREPAIRS, SHIFT_DAY) > 0) || (active_building().num_girls_on_job(JOB_MECHANIC, SHIFT_NIGHT) < 1 &&
                                                                                active_building().num_girls_on_job(JOB_GETREPAIRS, SHIFT_NIGHT) > 0))
        jdmessage << "\n**** A Mechanic is required to perform any Repairs. ";
    return jdmessage.str();
}

cScreenGirlManagement::cScreenGirlManagement() :
        IBuildingScreenManagement(BuildingType::BROTHEL, "girl_management_screen.xml")
{
    // add the job filters
    for (unsigned int i = 0; i <= JOBFILTER_BROTHEL; i++)  // temporary limit to job types shown
        add_job_filter((JOBFILTER)i);
}


void cScreenGirlManagement::set_ids()
{
    IBuildingScreenManagement::set_ids();
    prev_id            /**/ = get_id("PrevButton","Prev","*Unused*");
    next_id            /**/ = get_id("NextButton","Next","*Unused*");

    SetButtonCallback(prev_id, [this](){ cycle_building(-1); });
    SetButtonCallback(next_id, [this](){ cycle_building(1); });
}

cScreenHouseManagement::cScreenHouseManagement() :
        IBuildingScreenManagement(BuildingType::HOUSE, "house_management_screen.xml")
{
    add_job_filter(JOBFILTER_HOUSE);
    add_job_filter(JOBFILTER_HOUSETTRAINING);
}

std::string cScreenHouseManagement::update_job_description(const sGirl& girl)
{
    if ((girl.m_WorkingDay > 0 || girl.m_PrevWorkingDay > 0) && (
            (girl.m_YesterDayJob == JOB_SO_STRAIGHT    && girl.m_DayJob != JOB_SO_STRAIGHT) ||
            (girl.m_YesterDayJob == JOB_SO_BISEXUAL    && girl.m_DayJob != JOB_SO_BISEXUAL) ||
            (girl.m_YesterDayJob == JOB_SO_LESBIAN    && girl.m_DayJob != JOB_SO_LESBIAN) ||
            (girl.m_YesterDayJob == JOB_FAKEORGASM    && girl.m_DayJob != JOB_FAKEORGASM)))
    {
        std::stringstream ss;
        ss << "\n** This girl was in training for " << job_manager().JobData[girl.m_YesterDayJob].name
           << ", if you send her somewhere else, she will have to start her training over.";
        return ss.str();
    }
    return "";
}

cScreenStudioManagement::cScreenStudioManagement() :
        IBuildingScreenManagement(BuildingType::STUDIO, "studio_management_screen.xml")
{
    add_job_filter(JOBFILTER_STUDIOCREW);
    add_job_filter(JOBFILTER_STUDIONONSEX);
    add_job_filter(JOBFILTER_STUDIOSOFTCORE);
    add_job_filter(JOBFILTER_STUDIOPORN);
    add_job_filter(JOBFILTER_STUDIOHARDCORE);
    add_job_filter(JOBFILTER_RANDSTUDIO);
}


void cScreenStudioManagement::set_ids()
{
    IBuildingScreenManagement::set_ids();
    createmovie_id    /**/ = get_id("CreateMovieButton");
    SetButtonNavigation(createmovie_id, "Movie Maker", false);
    SetButtonHotKey(createmovie_id, SDLK_c);
}

std::string cScreenStudioManagement::get_job_description(int selection)
{
    stringstream jdmessage;        jdmessage << job_manager().JobFilters[selection].Description;
    if (CrewNeeded(active_building()))    jdmessage << "\n** At least one Camera Mage and one Crystal Purifier are required to film a scene. ";
    return jdmessage.str();
}
