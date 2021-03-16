#include <algorithm>
#include "BuildingScreenManagement.h"
#include "interface/cWindowManager.h"
#include "buildings/IBuilding.h"
#include "widgets/IListBox.h"
#include "cTariff.h"
#include "InterfaceProcesses.h"
#include "IGame.h"
#include "buildings/queries.h"
#include "character/cPlayer.h"
#include <sstream>
#include <CLog.h>
#include "cJobManager.h"
#include "cGirls.h"

namespace settings {
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_ACCOMODATION_SLAVE;
}

extern    bool            g_AltKeys;    // New hotkeys --PP


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
            std::stringstream text;
            text << cGirls::GetGirlMood(*selected_girl) << "\n \n" << selected_girl->m_Desc;
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
        reset_cycle_list();

        if(IsMultiSelected(girllist_id)) {
            // if multiple girls are selected, put them into the selection list
            ForAllSelectedItems(girllist_id, [&](int sel) {
                add_to_cycle_list(active_building().get_girl(sel)->shared_from_this());
            });
        } else {
            // if only a single girl is selected, allow iterating over all
            active_building().girls().apply([this](sGirl& g) {
                if (!g.is_dead()) {
                    add_to_cycle_list(g.shared_from_this());
                }
            });
            cycle_to(selected_girl);
        }
        push_window("Girl Details");
    }
}

void IBuildingScreenManagement::RefreshJobList()
{
    ClearListBox(joblist_id);
    int job_filter = GetSelectedItemFromList(jobtypelist_id);
    if (job_filter == -1) return;
    // populate Jobs listbox with jobs in the selected category
    for (auto i : g_Game->job_manager().JobFilters.at(job_filter).Contents)
    {
        if (g_Game->job_manager().get_job_name(i).empty()) continue;
        AddToListBox(joblist_id, i, jobname_with_count((JOBS)i, Day0Night1));
    }
    if (selected_girl)
    {
        JOBS sel_job = selected_girl->get_job(Day0Night1);
        SetSelectedItemInList(joblist_id, sel_job, false);
        EditTextItem(g_Game->job_manager().get_job_description(sel_job) + update_job_description(*selected_girl), jobdesc_id);
        SetSelectedItemText(joblist_id, sel_job, jobname_with_count((JOBS)sel_job, Day0Night1));
    }
}

void IBuildingScreenManagement::on_select_girl(int selection)
{
    if (selection != -1)
    {
        selected_girl = active_building().get_girl(selection);

        bool freefound = false;
        bool slavefound = false;
        ForAllSelectedItems(girllist_id, [&](int sel) {
            if (active_building().get_girl(sel)->is_slave()) slavefound = true;
            if (!active_building().get_girl(sel)->is_slave()) freefound = true;
        });
        DisableWidget(firegirl_id, !freefound);
        DisableWidget(freeslave_id, !slavefound);
        DisableWidget(sellslave_id, !slavefound);

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
    girlimage_id = get_id("GirlImage");
    girldesc_id  = get_id("GirlDescription");
    girllist_id  = get_id("GirlList");

    viewdetails_id = get_id("ViewDetailsButton");
    gold_id      = get_id("Gold");
    firegirl_id  = get_id("FireButton");
    freeslave_id = get_id("FreeSlaveButton");
    sellslave_id = get_id("SellSlaveButton");

    day_id       = get_id("DayButton");
    night_id     = get_id("NightButton");

    jobtypelist_id = get_id("JobTypeList");
    joblist_id     = get_id("JobList");
    jobdesc_id     = get_id("JobDescription");
    jobtypehead_id = get_id("JobTypeHeader");
    jobtypedesc_id = get_id("JobTypeDescription");
    curbrothel_id  = get_id("CurrentBrothel");

    // setting up button callbacks
    SetButtonCallback(viewdetails_id, [this](){
        ViewSelectedGirl();
    });
    SetButtonCallback(firegirl_id, [this](){  fire_girls(); });
    SetButtonCallback(freeslave_id, [this](){  free_girls(); });
    SetButtonCallback(sellslave_id, [this](){  fire_girls(); });

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
    SetListBoxHotKeys(joblist_id, SDLK_e, SDLK_q);
}

void IBuildingScreenManagement::on_select_job(int selection)
{
    bool fulltime = is_ctrl_held();
    if (selection != -1)
    {
        JOBS new_job = static_cast<JOBS>(selection);
        EditTextItem(job_manager().get_job_description((JOBS)selection), jobdesc_id);        // first handle the descriptions
        ForAllSelectedItems(girllist_id, [&](int sel) {
            auto girl = active_building().get_girl(sel);
            if (girl)
            {
                assign_job(*girl, new_job, sel, fulltime);
            }
        });
    }
    else EditTextItem("Nothing Selected", jobdesc_id);
}

void IBuildingScreenManagement::assign_job(sGirl& girl, JOBS new_job, int girl_selection, bool fulltime)
{
    // handle special job requirements and assign
    JOBS old_job = girl.get_job(Day0Night1);
    
    // if HandleSpecialJobs returns true, the job assignment was modified or cancelled
    if (job_manager().HandleSpecialJobs(girl, new_job, old_job, Day0Night1, fulltime))
    {
        // TODO WHAT HAPPENS HERE?
        //new_job = Day0Night1 ? night_job : day_job;
        SetSelectedItemInList(joblist_id, new_job, false);
    }

    JOBS day_job   = girl.get_job(false);
    JOBS night_job = girl.get_job(true);
    std::stringstream ss;
    // update the girl's listing to reflect the job change
    const auto day_job_name = job_manager().get_job_name(day_job);
    const auto night_job_name = job_manager().get_job_name(night_job);
    GetListBox(girllist_id)->UpdateRow(girl_selection, &girl, -1);

    // refresh job worker counts for former job and current job
    SetSelectedItemText(joblist_id, old_job, jobname_with_count((JOBS)old_job, Day0Night1));
    SetSelectedItemText(joblist_id, new_job, jobname_with_count((JOBS)new_job, Day0Night1));

    if (girl.m_YesterDayJob == JOB_REHAB && new_job != JOB_REHAB && (girl.m_WorkingDay > 0 || girl.m_PrevWorkingDay > 0))
    {    // `J` added
        ss.str("");    ss << job_manager().get_job_description(new_job) << "\n** This girl was in Rehab, if you send her somewhere else, she will have to start her Rehab over.";
        EditTextItem(ss.str(), jobdesc_id);
    }

    // handle surgeries
    bool interrupted = false;    // `J` added
    if (girl.m_YesterDayJob != day_job && cJobManager::is_Surgery_Job(girl.m_YesterDayJob) && ((girl.m_WorkingDay > 0) || girl.m_PrevWorkingDay > 0))
        interrupted = true;

    if (interrupted)
    {    // `J` added
        ss.str(""); ss << job_manager().get_job_description(new_job) << "\n** This girl was getting ";
        if (girl.m_YesterDayJob == JOB_CUREDISEASES)
        {
            ss << "her disease cured, if you send her somewhere else, she will have to start her treatment over.";
        }
        else
        {
            if (girl.m_YesterDayJob == JOB_BOOBJOB || girl.m_YesterDayJob == JOB_FACELIFT)        ss << "a ";
            else if (girl.m_YesterDayJob == JOB_GETABORT || girl.m_YesterDayJob == JOB_ASSJOB)    ss << "an ";
            else if (girl.m_YesterDayJob == JOB_TUBESTIED)/*                                          */    ss << "her ";
            ss << job_manager().get_job_name(girl.m_YesterDayJob) << ", if you send her somewhere else, she will have to start her Surgery over.";
        }
        EditTextItem(ss.str(), jobdesc_id);
    }

    // conversions
    if ((girl.m_WorkingDay > 0 || girl.m_PrevWorkingDay > 0) && (new_job != girl.m_YesterDayJob && (
            girl.m_YesterDayJob == JOB_SO_STRAIGHT ||
            girl.m_YesterDayJob == JOB_SO_BISEXUAL ||
            girl.m_YesterDayJob == JOB_SO_LESBIAN ||
            girl.m_YesterDayJob == JOB_FAKEORGASM)))
    {    // `J` added
        ss.str("");    ss << job_manager().get_job_description(new_job) <<
                          "\n** This girl was in training for " << job_manager().get_job_name(girl.m_YesterDayJob)
                          << ", if you send her somewhere else, she will have to start her training over.";
        EditTextItem(ss.str(), jobdesc_id);
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

    for (int i = 0; i < active_building().num_girls(); i++)    // Add girls to list
    {
        sGirl* gir = active_building().get_girl(i);
        if (selected_girl == gir) selection = i;
        unsigned int item_color = (gir->health() <= 30 || gir->tiredness() >= 80 || gir->happiness() <= 30) ? COLOR_RED : COLOR_BLUE;
        GetListBox(girllist_id)->AddRow(i, gir, item_color);
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

std::string IBuildingScreenManagement::jobname_with_count(JOBS job_id, bool is_night)
{
    std::stringstream text;
    text << g_Game->job_manager().get_job_name(job_id);
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
    int job = selected_girl->get_job(Day0Night1);
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
    cInterfaceWindowXML::OnKeyPress(keysym);

}

void IBuildingScreenManagement::SetShift(int shift)
{
    Day0Night1 = shift;
    DisableWidget(day_id, shift == SHIFT_DAY);
    DisableWidget(night_id, shift == SHIFT_NIGHT);
    RefreshSelectedJobType();
}

std::string IBuildingScreenManagement::get_job_description(int selection)
{
    return job_manager().JobFilters[selection].Description;
}

void IBuildingScreenManagement::free_girls() {
    std::vector<sGirl*> girls;
    ForAllSelectedItems(girllist_id, [&](int sel){
        sGirl* girl = active_building().get_girl(sel);
        if(!girl->is_dead() && girl->is_slave()) {
            girls.push_back(girl);
        }
    });

    if(girls.empty()) {
        g_LogFile.error("ui", "No living slave girl in selection");
        return;
    }

    std::vector<std::string> options;
    if(girls.size() == 1) {
        options.push_back("Free " + girls.front()->FullName());
        options.emplace_back("Throw a freedom party [100G]");
        options.emplace_back("Keep her as a slave");
    } else {
        options.emplace_back("Just free the slaves");
        options.emplace_back("Throw a freedom party [" + std::to_string(100 * girls.size()) + "G]");
        options.emplace_back("Keep them as slaves");
    }


    std::stringstream ask;
    ask << "You call in ";
    if(girls.size() == 1) {
        ask << girls.front()->FullName() << "because you want to grant her freedom.";
    } else {
        ask << girls.size() << " slaves because you want to grant them their freedom.";
    }

    ask << " Do you want to throw a Freedom Party or just get it over with quickly?";
    g_Game->push_message(ask.str(), COLOR_BLUE);

    input_choice("Is it party time?", std::move(options), [girls=std::move(girls)](int selected) {
        if(selected == 0) {
            cGirls::FreeGirls(girls, false);
        } else if (selected == 1) {
            cGirls::FreeGirls(girls, true);
        }
    });
}

void IBuildingScreenManagement::fire_girls() {
    std::vector<sGirl*> free_girls;
    std::vector<sGirl*> slave_girls;
    ForAllSelectedItems(girllist_id, [&](int sel){
        sGirl* girl = active_building().get_girl(sel);
        if(girl->is_dead()) return;
        if(girl->is_slave()) {
            slave_girls.push_back(girl);
        } else {
            free_girls.push_back(girl);
        }
    });

    if(free_girls.empty() && slave_girls.empty()) {
        g_LogFile.error("ui", "FireGirls called but no live girls selected.");
        return;
    }

    std::vector<std::string> options;
    if(!free_girls.empty()) {
        if(free_girls.size() == 1) {
            options.push_back("Fire " + free_girls.front()->FullName());
        } else if(slave_girls.empty()) {
            options.emplace_back("Fire them");
        } else {
            options.emplace_back("Fire the free girls");
        }
    }

    if(!slave_girls.empty()) {
        int price = 0;
        for(auto& girl : slave_girls) {
            price += g_Game->tariff().slave_sell_price(*girl);
        }
        if(slave_girls.size() == 1) {
            options.push_back("Sell " + slave_girls.front()->FullName());
        } else if(slave_girls.empty()) {
            options.emplace_back("Sell them");
        } else {
            options.emplace_back("Sell the slaves");
        }
        options.back() += " for " + std::to_string(price);
    }
    options.emplace_back("Do nothing");

    input_choice("", options, [free_girls=std::move(free_girls), slave_girls=std::move(slave_girls)](int selected) {
        if(selected == 0) {
            if(!free_girls.empty()) {
                cGirls::FireGirls(free_girls);
            } else {
                cGirls::SellSlaves(slave_girls);
            }
        } else if (selected == 1) {
            if(!free_girls.empty() && !slave_girls.empty()) {
                cGirls::SellSlaves(slave_girls);
            }
            // do nothing
        }
    });
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
    std::stringstream jdmessage; jdmessage << job_manager().JobFilters[selection].Description;
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
        int sel_job = girl.get_job(Day0Night1);
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
            ss << job_manager().get_job_name(girl.m_YesterDayJob) << ", if you send her somewhere else, she will have to start her Surgery over.";
        }
        return ss.str();
    }
    return "";
}

std::string cScreenClinicManagement::get_job_description(int selection)
{
    std::stringstream jdmessage; jdmessage << job_manager().JobFilters[selection].Description;
    if (DoctorNeeded(active_building()))
        jdmessage << "\n*** A Doctor is required to perform any surgeries. ";
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
    int prev_id = get_id("PrevButton","Prev","*Unused*");
    int next_id = get_id("NextButton","Next","*Unused*");

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
        ss << "\n** This girl was in training for " << job_manager().get_job_name(girl.m_YesterDayJob)
           << ", if you send her somewhere else, she will have to start her training over.";
        return ss.str();
    }
    return "";
}

cScreenStudioManagement::cScreenStudioManagement() :
        IBuildingScreenManagement(BuildingType::STUDIO, "studio_management_screen.xml")
{
    add_job_filter(JOBFILTER_STUDIOCREW);
    add_job_filter(JOBFILTER_STUDIOTEASE);
    add_job_filter(JOBFILTER_STUDIOSOFTCORE);
    add_job_filter(JOBFILTER_STUDIOPORN);
    add_job_filter(JOBFILTER_STUDIOHARDCORE);
    add_job_filter(JOBFILTER_RANDSTUDIO);
}


void cScreenStudioManagement::set_ids()
{
    IBuildingScreenManagement::set_ids();
    int createmovie_id = get_id("CreateMovieButton");
    SetButtonHotKey(createmovie_id, SDLK_c);
}

std::string cScreenStudioManagement::get_job_description(int selection)
{
    std::stringstream jdmessage;        jdmessage << job_manager().JobFilters[selection].Description;
    if (CrewNeeded(active_building()))    jdmessage << "\n** At least one Camera Mage and one Crystal Purifier are required to film a scene. ";
    return jdmessage.str();
}
