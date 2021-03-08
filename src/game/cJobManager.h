/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org
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
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Constants.h"
#include "cRng.h"

//I need a better place for this
struct sGirl;
struct sBrothel;
struct sGang;
class sCustomer;
class IBuilding;
class IGenericJob;

struct sFilm
{
    //int total_girls;
    //int total_cost;
    float quality_multiplyer;
    std::vector<int> scene_quality;
    char time;
    int final_quality;
    bool sex_acts_flags[5];
    int total_customers;
    sFilm()
    {
        quality_multiplyer=0;
    }
};

struct sJobFilter {
    std::string Name;
    std::string Description;
    std::vector<JOBS> Contents;
};

struct sWorkJobResult {
    bool Refused;           // Whether she actually worked
    int Tips = 0;           // how much she received in tips
    int Earnings = 0;       // how much money did she make you directly
    int Wages = 0;          // how much do you pay her for the job
};

struct sPaymentData {
    int Tips;
    int Earnings;
    int Wages;
    int GirlGets;
    int PlayerGets;
};

//mainly a list of functions 
class cJobManager
{
public:
    cJobManager();
    ~cJobManager();
    sWorkJobResult do_job(sGirl& girl, bool is_night);
    sWorkJobResult do_job(JOBS job, sGirl& girl, bool is_night);
    bool job_filter(int Filter, JOBS jobs) const;

    const IGenericJob* get_job(JOBS job) const;
    const std::string& get_job_name(JOBS job) const;
    const std::string& get_job_brief(JOBS job) const;
    const std::string& get_job_description(JOBS job) const;

    // does the whole package of job processing: Runs the job, in case of refusal creates an event, and processes
    // pay for the building.
    void handle_simple_job(sGirl& girl, bool is_night);

    std::array<sJobFilter, NUMJOBTYPES> JobFilters;

    // return a job description along with a count of how many girls are on it
    bool HandleSpecialJobs(sGirl& Girl, JOBS JobID, JOBS OldJobID, bool Day0Night1, bool fulltime = false );  // check for and handle special job assignments

    void Setup();

    // - stuff that does processing for jobs
    static bool AddictBuysDrugs(std::string Addiction, std::string Drug, sGirl& girl, IBuilding* brothel, bool Day0Night1);

    // MYR: New code for security.  All the old code is still here, commented out.
    static bool work_related_violence(sGirl&, bool, bool);
    static int guard_coverage(std::vector<sGang*> *v=nullptr);
    static bool security_stops_rape(sGirl& girl, sGang *enemy_gang, int day_night);
    static bool gang_stops_rape(sGirl& girl, std::vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night);
    static bool girl_fights_rape(sGirl& girl, sGang *enemy_gang, int day_night);
    static void customer_rape(sGirl& girl, int numberofattackers);
    static std::string GetGirlAttackedString(int attacktype = SKILL_COMBAT);    // `J` added attacktype to be used with sextype for more specific attacks defaulting to combat


    static sCustomer GetMiscCustomer(IBuilding& brothel);

    bool is_job_Paid_Player(JOBS Job);        //    WD:    Test for all jobs paid by player
    bool FullTimeJob(JOBS Job);            //    `J`    Test if job is takes both shifts
    sPaymentData CalculatePay(sGirl& girl, sWorkJobResult pay);

    static bool is_Surgery_Job(int testjob);

    static void CatchGirl(sGirl& girl, std::stringstream& fuckMessage, const sGang* guardgang);

    std::vector<std::unique_ptr<IGenericJob>> m_OOPJobs;

    void register_job(std::unique_ptr<IGenericJob> job);
};
