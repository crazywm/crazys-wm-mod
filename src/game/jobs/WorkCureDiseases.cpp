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
#pragma region //    Includes and Externs            //
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include <sstream>
#include "cJobManager.h"
#include "character/predicates.h"
#include "cGirls.h"

#pragma endregion

// `J` Job Clinic - Full_Time_Job
bool WorkCureDiseases(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    std::stringstream ss;

    // if she was not in JOB_CUREDISEASES yesterday, reset working days to 0 before proceding
    if (girl.m_YesterDayJob != JOB_CUREDISEASES) girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
    if (girl.m_WorkingDay < 0) girl.m_WorkingDay = 0;
    girl.m_DayJob = girl.m_NightJob = JOB_CUREDISEASES;    // it is a full time job

    if (!has_disease(girl))
    {
        ss << "${name} has no diseases";
        JOBS newjob = JOB_RESTING;
        if (girl.health() < 80 || girl.tiredness() > 20)
        {
            ss << ". She was not feeling well so she ";
            if (girl.has_active_trait("Construct"))    { newjob = JOB_GETREPAIRS;    ss << "went to see the mechanic for repairs instead."; }
            else/*                         */    { newjob = JOB_GETHEALING;    ss << "checked herself in for treatment instead."; }
        }
        else/*                             */    { ss << " so she was sent to the waiting room."; }
        girl.FullJobReset(newjob);
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        if (Day0Night1 == SHIFT_DAY)    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return false;    // not refusing
    }
    int numdoctor = brothel->num_girls_on_job(JOB_DOCTOR, Day0Night1);
    int numnurse = brothel->num_girls_on_job(JOB_NURSE, Day0Night1);
    if (numdoctor + numnurse < 1)
    {
        ss << "${name} does nothing. You don't have any Doctors or Nurses working. (requires 2 Doctors and 4 Nurses for fastest results) ";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return false;    // not refusing
    }

    int cost = 0;
    std::vector<std::string> diseases;
    if (girl.has_active_trait("Herpes"))        { diseases.push_back("Herpes"); cost += 50; }
    if (girl.has_active_trait("Chlamydia"))    { diseases.push_back("Chlamydia"); cost += 100; }
    if (girl.has_active_trait("Syphilis"))    { diseases.push_back("Syphilis"); cost += 150; }
    if (girl.has_active_trait("AIDS"))        { diseases.push_back("AIDS"); cost += 200; }
    int numdiseases = diseases.size();

    if (numdiseases > 1)    ss << "${name} is in the Clinic to get her diseases treated";
    else/*            */    ss << "${name} is in the Clinic to get treatment for " << diseases[0];
    ss << ".\n \n";

    auto msgtype = Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    cGirls::UnequipCombat(girl);    // not for patient

    //    `J` We want to get 2 doctors and 4 nurses if possible
    int d1 = -1, d2 = -1, n1 = -1, n2 = -1, n3 = -1, n4 = -1;
    std::vector<sGirl*> doctors = brothel->girls_on_job(JOB_DOCTOR, Day0Night1);
    std::vector<sGirl*> nurses = brothel->girls_on_job(JOB_NURSE, Day0Night1);

    if (numdoctor > 2)    // choose 2 random doctors
    {
        d1 = rng%numdoctor;
        d2 = rng%numdoctor;
        while (d1 == d2)    d2 = rng%numdoctor;
    }
    else                    // otherwise take whoever is there
    {
        if (numdoctor > 0)    d1 = 0;
        if (numdoctor > 1)    d2 = 1;
    }
    if (numnurse > 4)    // choose 4 random nurses
    {
        n1 = rng%numnurse;    n2 = rng%numnurse;    n3 = rng%numnurse;    n4 = rng%numnurse;
        while (n2 == n1)                            n2 = rng%numnurse;
        while (n3 == n1 || n3 == n2)                n3 = rng%numnurse;
        while (n4 == n1 || n4 == n2 || n4 == n3)    n4 = rng%numnurse;
    }
    else                    // otherwise take whoever is there
    {
        if (numnurse > 0)    n1 = 0;
        if (numnurse > 1)    n2 = 1;
        if (numnurse > 2)    n2 = 2;
        if (numnurse > 3)    n2 = 3;
    }

    sGirl* Doctor1 = d1 != -1 ? doctors[d1] : nullptr;
    sGirl* Doctor2 = d2 != -1 ? doctors[d2] : nullptr;
    sGirl* Nurse1 = n1 != -1 ? nurses[n1] : nullptr;
    sGirl* Nurse2 = n2 != -1 ? nurses[n2] : nullptr;
    sGirl* Nurse3 = n3 != -1 ? nurses[n3] : nullptr;
    sGirl* Nurse4 = n4 != -1 ? nurses[n4] : nullptr;

    numdoctor = numnurse = 0;        // clear the count and recheck to make sure they were found properly
    if (Doctor1)    numdoctor++;
    if (Doctor2)    numdoctor++;
    if (Nurse1)        numnurse++;
    if (Nurse2)        numnurse++;
    if (Nurse3)        numnurse++;
    if (Nurse4)        numnurse++;

    // up to 13 per doctor and 6 per nurse = 50 total
    int doctormax = 13;        int doctordiv = 30;
    int nursemax = 6;        int nursediv = 60;

    if (numdoctor + numnurse == 6)
    {
        ss << "${name} has a full complement of Doctors and Nurses looking after her";
    }
    else if (numdoctor + numnurse > 0)
    {
        ss << "The Clinic ";
        if (numdoctor == 0)
        {
            ss << "has no Doctors working ";
            if (numnurse == 4)    ss << "so the Nurses have to take care of ${name} on their own";
            else/*          */    ss << "and not enough Nurses working either";
            nursemax = numnurse + 1;    nursediv = (13 - numnurse) * 10;
        }
        else if (numdoctor == 1)
        {
            ss << "is short handed on Doctors";
            if (numnurse == 0) ss << " and has no Nurses working";
            else if (numnurse < 4) ss << " and Nurses";
            ss << " so ${name}'s care is not as good as it should be";
            doctormax = 5 + numnurse;    doctordiv = 40;
            nursemax = numnurse + 1;    nursediv = (13 - numnurse) * 9;
        }
        else // 2 doctors
        {
            if (numnurse == 0)    ss << "has no nurses working and the Doctors can't spend all their time with ${name}";
            else ss << "is short staffed on Nurses so ${name}'s care is not as good as it should be";
            doctormax = 6 + numnurse;    doctordiv = 65 - (numnurse * 7);
            nursemax = numnurse + 2;    nursediv = 1 + ((12 - numnurse) * 9);
        }
    }
    else
    {
        ss << "There were no Doctors or Nurses working so ${name} just lay in bed getting sicker";
        girl.m_WorkingDay -= rng % 10;
        cost = 0;    // noone to give her the medicine
    }
    ss << ".\n \n";

    girl.m_WorkingDay += girl.constitution() / 10;
    if (girl.has_active_trait("Construct")) girl.m_WorkingDay += 3;            // Not flesh and blood so easier to cleanse
    if (girl.has_active_trait("Powerful Magic")) girl.m_WorkingDay += 3;            // She can use her magic to speed it up a bit
    if (girl.has_active_trait("Strong Magic")) girl.m_WorkingDay += 2;            // She can use her magic to speed it up a bit
    if (girl.has_active_trait("Doctor")) girl.m_WorkingDay += 2;            // She can use her msdicine to speed it up a bit
    if (girl.has_active_trait("Angel")) girl.m_WorkingDay += 2;            // A creature of purity
    if (girl.has_active_trait("Half-Construct")) girl.m_WorkingDay += 1;            // Not all flesh and blood so easier to cleanse
    if (girl.has_active_trait("Skeleton")) girl.m_WorkingDay += 1;            // No flesh or blood so easier to cleanse
    if (girl.has_active_trait("Dwarf")) girl.m_WorkingDay += 1;            // a hearty race
    if (girl.has_active_trait("Elf")) girl.m_WorkingDay += 1;            // a healty race
    if (girl.has_active_trait("Tough")) girl.m_WorkingDay += 1;            // her immune system is a little stronger
    if (girl.has_active_trait("Fallen Goddess")) girl.m_WorkingDay += rng % 2;    // she must have some power left
    if (girl.has_active_trait("Goddess")) girl.m_WorkingDay += rng % 2;    // she must have some power left
    if (girl.has_active_trait("Country Gal")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Farmer")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Farmers Daughter")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Homeless")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Maid")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Waitress")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Porn Star")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Whore")) girl.m_WorkingDay += rng % 2;    // she is used to dirty surroundings so her immune system is a little stronger
    if (girl.has_active_trait("Slitherer")) girl.m_WorkingDay += rng % 2;    // more of her body touches the ground so her immune system is a little stronger
    if (girl.has_active_trait("Succubus")) girl.m_WorkingDay += rng % 2;    // she feeds on dark energies so her immune system is a little stronger

    if (girl.has_active_trait("Zombie")) girl.m_WorkingDay -= 5;            // she is a rotting corpse, she has no immune system
    if (girl.has_active_trait("Demon")) girl.m_WorkingDay -= 3;            // a creature of taint
    if (girl.has_active_trait("Demon Possessed")) girl.m_WorkingDay -= 3;            // she is posessed by a creature of taint
    if (girl.has_active_trait("Old")) girl.m_WorkingDay -= 2;            // her immune system is diminished due to her age
    if (girl.has_active_trait("Fragile")) girl.m_WorkingDay -= 2;            // she gets sick easier
    if (girl.has_active_trait("Undead")) girl.m_WorkingDay -= 1;            // she is not living, her immune system is not the best
    if (girl.has_active_trait("Vampire")) girl.m_WorkingDay -= 1;            // she needs other peoples' blood to survive so her blood is not all the same
    if (girl.has_active_trait("Delicate")) girl.m_WorkingDay -= 1;            // she gets sick easier
    if (girl.has_active_trait("Fairy Dust Addict")) girl.m_WorkingDay -= 1;            // she is a little worse for wear because of drug abuse
    if (girl.has_active_trait("Former Addict")) girl.m_WorkingDay -= 1;            // she is a little worse for wear because of drug abuse
    if (girl.has_active_trait("Shroud Addict")) girl.m_WorkingDay -= 1;            // she is a little worse for wear because of drug abuse
    if (girl.has_active_trait("Viras Blood Addict")) girl.m_WorkingDay -= 1;            // she is a little worse for wear because of drug abuse
    if (girl.has_active_trait("Noble")) girl.m_WorkingDay -= rng % 2;    // she is not used to dirty surroundings so her immune system is a little weaker
    if (girl.has_active_trait("Princess")) girl.m_WorkingDay -= rng % 2;    // she is not used to dirty surroundings so her immune system is a little weaker
    if (girl.has_active_trait("Queen")) girl.m_WorkingDay -= rng % 3;    // she is not used to dirty surroundings so her immune system is a little weaker


#pragma endregion
#pragma region //    Count the Days                //

    if (Day0Night1)    // Count up the points at the end of the day
    {
        if (Doctor1)    girl.m_WorkingDay += std::min(doctormax, ((Doctor1->medicine() + Doctor1->intelligence() + Doctor1->magic() + Doctor1->herbalism()) / doctordiv));
        if (Doctor2)    girl.m_WorkingDay += std::min(doctormax, ((Doctor2->medicine() + Doctor2->intelligence() + Doctor2->magic() + Doctor2->herbalism()) / doctordiv));
        if (Nurse1)        girl.m_WorkingDay += std::min(nursemax, ((Nurse1->medicine() + Nurse1->intelligence() + Nurse1->magic() + Nurse1->herbalism()) / nursediv));
        if (Nurse2)        girl.m_WorkingDay += std::min(nursemax, ((Nurse2->medicine() + Nurse2->intelligence() + Nurse2->magic() + Nurse2->herbalism()) / nursediv));
        if (Nurse3)        girl.m_WorkingDay += std::min(nursemax, ((Nurse3->medicine() + Nurse3->intelligence() + Nurse3->magic() + Nurse1->herbalism()) / nursediv));
        if (Nurse4)        girl.m_WorkingDay += std::min(nursemax, ((Nurse4->medicine() + Nurse4->intelligence() + Nurse4->magic() + Nurse4->herbalism()) / nursediv));

    }
    if (numnurse == 4)
    {
        ss << "The Nurses kept her healthy and happy during her treatment";
        girl.health(rng.bell(0, 20));
        girl.happiness(rng.bell(0, 10));
        girl.spirit(rng.bell(0, 10));
        girl.mana(rng.bell(0, 20));
    }
    else if (numnurse > 0)
    {
        ss << "The Nurse" << (numnurse > 1 ? "s" : "") << " helped her during her treatment";
        girl.health(rng.bell(0, 10));
        girl.happiness(rng.bell(0, 5));
        girl.spirit(rng.bell(0, 5));
        girl.mana(rng.bell(0, 10));
    }
    else
    {
        ss << "She is sad and has lost some health during the treatment";
        girl.health(rng.bell(-20, 2));
        girl.happiness(rng.bell(-10, 1));
        girl.spirit(rng.bell(-5, 1));
        girl.mana(rng.bell(-20, 3));
    }


    ss << ".\n \n";

#pragma endregion
#pragma region //    Surgery Finished            //

    if (Day0Night1 && girl.m_WorkingDay >= 100)
    {
        msgtype = EVENT_GOODNEWS;
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;

        std::string diseasecured = diseases[rng%numdiseases];
        girl.lose_trait(diseasecured.c_str());
        numdiseases--;
        ss << "You pay " << cost << " gold for last dose of the medicine used in her treatment.\n \nThe treatment is a success, ${name} no longer has " << diseasecured << "!\n \n";

        if (!has_disease(girl))
        {
            ss << "She has been released from the Clinic";
            girl.FullJobReset(JOB_RESTING);
        }
        else
        {
            ss << "Her ";
            if (numdoctor > 0) ss << "Doctor" << (numdoctor > 1 ? "s" : "");
            else ss << "Nurse" << (numnurse > 1 ? "s" : "");
            ss << " keep her in bed so they can treat her other disease" << (numdiseases > 1 ? "s" : "");
        }
    }
    else if (girl.m_WorkingDay < 0)
    {
        ss << "You pay " << cost << " gold for the medicine and equipment used in her treatment, however she doesn't seem to be getting any better";
        if (girl.m_WorkingDay < 3) ss << ", in fact, she seems to be getting worse";
        girl.health(-girl.m_WorkingDay);
        girl.happiness(-girl.m_WorkingDay/2);
        girl.tiredness(girl.m_WorkingDay);
        girl.constitution(-girl.m_WorkingDay / 10);

        girl.m_WorkingDay = 0;
    }
    else
    {
        ss << "You pay " << cost << " gold for the medicine and equipment used in her treatment";
    }
    ss << ".\n \n";

    brothel->m_Finance.clinic_costs(cost);    // pay for it

#pragma endregion
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

    // Improve girl
    if (girl.has_active_trait("Lesbian"))    girl.upd_temp_stat(STAT_LIBIDO, numnurse + numdoctor);
    if (girl.has_active_trait("Masochist"))    girl.upd_temp_stat(STAT_LIBIDO, 1);
    if (rng % 10 == 0)
        girl.medicine(1);    // `J` she watched what the doctors and nurses were doing

#pragma endregion
    return false;
}

double JP_CureDiseases(const sGirl& girl, bool estimate)// not used
{
    int numdiseases = 0;
    if (girl.has_active_trait("AIDS"))            numdiseases++;
    if (girl.has_active_trait("Herpes"))            numdiseases++;
    if (girl.has_active_trait("Chlamydia"))        numdiseases++;
    if (girl.has_active_trait("Syphilis"))        numdiseases++;

    if (numdiseases == 0)    return -1000;
    return numdiseases * 100;
}
