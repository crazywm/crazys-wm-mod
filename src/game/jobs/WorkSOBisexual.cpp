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
#include "cGirls.h"

#pragma endregion

// `J` Job House - Training - Full_Time_Job
bool WorkSOBisexual(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    std::stringstream ss;
    Action_Types actiontype = ACTION_WORKTRAINING;
    // if she was not in JOB_SO_BISEXUAL yesterday, reset working days to 0 before proceding
    if (girl.m_YesterDayJob != JOB_SO_BISEXUAL) girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
    if (girl.m_WorkingDay < 0) girl.m_WorkingDay = 0;
    girl.m_DayJob = girl.m_NightJob = JOB_SO_BISEXUAL;    // it is a full time job

    if (girl.has_active_trait("Bisexual"))
    {
        ss << "${name} is already Bisexual.";
        if (Day0Night1 == SHIFT_DAY)    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_HOUSEREST);
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        return false;    // not refusing
    }

    ss << "You procede to change ${name}'s sexual orientation to Bisexual.\n \n";

    cGirls::UnequipCombat(girl);    // not for patient

    int enjoy = 0, wages = 10;
    int startday = girl.m_WorkingDay;
    auto msgtype = Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    int imagetype = IMGTYPE_SEX;

    // Base adjustment
    int tired = 5 + rng % 11;
    girl.m_WorkingDay += 10 + rng % 11;
    // Positive Stats/Skills

    if (girl.has_active_trait("Straight"))
    {
        girl.m_WorkingDay += girl.group() / 10;
        girl.m_WorkingDay += girl.normalsex() / 20;
        girl.m_WorkingDay += girl.lesbian() / 5;
        girl.m_WorkingDay += girl.oralsex() / 20;
        girl.m_WorkingDay += girl.tittysex() / 20;
        girl.m_WorkingDay += girl.anal() / 20;
    }
    else if (girl.has_active_trait("Lesbian"))
    {
        girl.m_WorkingDay += girl.group() / 10;
        girl.m_WorkingDay += girl.normalsex() / 5;
        girl.m_WorkingDay += girl.lesbian() / 20;
        girl.m_WorkingDay += girl.oralsex() / 15;
        girl.m_WorkingDay += girl.tittysex() / 15;
        girl.m_WorkingDay += girl.anal() / 15;
    }
    else
    {
        girl.m_WorkingDay += girl.group() / 5;
        girl.m_WorkingDay += girl.normalsex() / 10;
        girl.m_WorkingDay += girl.lesbian() / 10;
        girl.m_WorkingDay += girl.oralsex() / 20;
        girl.m_WorkingDay += girl.tittysex() / 20;
        girl.m_WorkingDay += girl.anal() / 20;
    }

    girl.m_WorkingDay += girl.obedience() / 20;
    if (girl.pcfear() > 50)                girl.m_WorkingDay += rng % (girl.pcfear() / 20);        // She will do as she is told
    if (girl.pclove() > 50)                girl.m_WorkingDay += rng % (girl.pclove() / 20);        // She will do what you ask
    // Negative Stats/Skills
    girl.m_WorkingDay -= girl.spirit() / 25;
    if (girl.pchate() > 30)                girl.m_WorkingDay -= rng % (girl.pchate() / 10);        // She will not do what you want
    if (girl.happiness() < 50)                girl.m_WorkingDay -= 1 + rng % 5;                        // She is not feeling like it
    if (girl.health() < 50)                girl.m_WorkingDay -= 1 + rng % 5;                        // She is feeling sick
    if (girl.tiredness() > 50)                girl.m_WorkingDay -= 1 + rng % 5;                        // She is tired
    // Positive Traits
    if (girl.has_active_trait("Your Wife")) girl.m_WorkingDay += rng % 10;            // She wants to be with you, even if it is with another girl
    if (girl.has_active_trait("Porn Star")) girl.m_WorkingDay += rng % 10;            // She is used to having sex with anyone her director tells her to
    if (girl.has_active_trait("Whore")) girl.m_WorkingDay += rng % 8;            // She'll do anyone as long as they can pay
    if (girl.has_active_trait("Slut")) girl.m_WorkingDay += rng % 5;            // She'll do anyone
    if (girl.has_active_trait("Your Daughter")) girl.m_WorkingDay += 2;                    // She wants to partake in all that her father has to offer
    if (girl.has_active_trait("Actress")) girl.m_WorkingDay += 2;                    // She will do whatever her director tells her to
    if (girl.has_active_trait("Shape Shifter")) girl.m_WorkingDay += 2;                    // If she can become anyone she can have sex with anyone
    if (girl.has_active_trait("Succubus")) girl.m_WorkingDay += 2;                    // Males are easier to drain energy from but girls are ok
    if (girl.has_active_trait("Broodmother")) girl.m_WorkingDay += 1;                    // She prefers males who can get her pregnant
    if (girl.has_active_trait("Futanari")) girl.m_WorkingDay += 1;                    // If she has a dick she can put it anywhere
    // Negative Traits
    if (girl.has_active_trait("Broken Will"))    { girl.m_WorkingDay -= rng.bell(10, 20);    ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }
    if (girl.has_active_trait("Mind Fucked")) girl.m_WorkingDay -= rng.bell(10, 20);    // Does she even know who is fucking her?
    if (girl.has_active_trait("Retarded")) girl.m_WorkingDay -= rng.bell(5, 10);    // Does she even know who is fucking her?
    if (girl.has_active_trait("Slow Learner")) girl.m_WorkingDay -= rng % 10;            //
    if (girl.has_active_trait("Iron Will")) girl.m_WorkingDay -= rng % 5;            // She is set in her ways


    //    if (girl.check_virginity())                {}

    if (!brothel->is_sex_type_allowed(SKILL_ANAL))            girl.m_WorkingDay -= rng % 3;
    if (!brothel->is_sex_type_allowed(SKILL_BDSM))            girl.m_WorkingDay -= rng % 5;
    if (!brothel->is_sex_type_allowed(SKILL_FOOTJOB))        girl.m_WorkingDay -= rng % 2;
    if (!brothel->is_sex_type_allowed(SKILL_GROUP))            girl.m_WorkingDay -= rng % 15;
    if (!brothel->is_sex_type_allowed(SKILL_HANDJOB))        girl.m_WorkingDay -= rng % 5;
    if (!brothel->is_sex_type_allowed(SKILL_LESBIAN))        girl.m_WorkingDay -= rng % 10;
    if (!brothel->is_sex_type_allowed(SKILL_NORMALSEX))        girl.m_WorkingDay -= rng % 10;
    if (!brothel->is_sex_type_allowed(SKILL_ORALSEX))        girl.m_WorkingDay -= rng % 5;
    if (!brothel->is_sex_type_allowed(SKILL_TITTYSEX))        girl.m_WorkingDay -= rng % 2;





    if (girl.disobey_check(actiontype, JOB_SO_BISEXUAL)) girl.m_WorkingDay /= 2;    // if she disobeys, half her time is wasted

#pragma endregion
#pragma region //    Count the Days                //

    int total = girl.m_WorkingDay - startday;
    int xp = 1 + std::max(0, girl.m_WorkingDay / 20);
    if (total <= 0)                                // she lost time so more tired
    {
        tired += 5 + rng % (-total);
        enjoy -= rng % 3;
    }
    else if (total > 40)                        // or if she trained a lot
    {
        tired += (total / 4) + rng % (total / 2);
        enjoy += rng % 3;
    }
    else                                        // otherwise just a bit tired
    {
        tired += rng % (total / 3);
        enjoy -= rng.bell(-2, 2);
    }

    if (girl.m_WorkingDay <= 0)
    {
        girl.m_WorkingDay = 0;
        msgtype = EVENT_WARNING;
        ss << "\nShe resisted all attempts to make her Bisexual.";
        tired += 5 + rng % 11;
        wages = 0;
    }
    else if (girl.m_WorkingDay >= 100 && Day0Night1)
    {
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        msgtype = EVENT_GOODNEWS;
        ss << "\nHer Sexual Orientation conversion is complete. She is now Bisexual.";
        girl.lose_trait("Lesbian");    girl.gain_trait("Bisexual");    girl.lose_trait("Straight");
        girl.FullJobReset(JOB_HOUSEREST);
        wages = 200;
    }
    else
    {
        ss << "Her Sexual Orientation conversion to Bisexual is ";
        if (girl.m_WorkingDay >= 100)
        {
            ss << "almost complete.";
            tired -= (girl.m_WorkingDay - 100) / 2;    // her last day so she rested a bit
        }
        else ss << "in progress (" << girl.m_WorkingDay << "%).";
        wages = std::min(100, girl.m_WorkingDay);
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    if (girl.is_slave()) wages /= 2;
    girl.m_Pay = wages;

    // Improve girl
    int I_lesbian = (rng.bell(1, 10));
    int I_normalsex = (rng.bell(1, 10));
    int I_group = (rng.bell(2, 15));
    int I_anal = (rng.bell(0, 5));
    int I_oralsex = (rng.bell(0, 5));
    int I_handjob = (rng.bell(0, 5));
    int I_tittysex = (rng.bell(0, 3));

    girl.exp(xp);
    girl.tiredness(tired);
    girl.lesbian(I_lesbian);
    girl.normalsex(I_normalsex);
    girl.group(I_group);
    girl.anal(I_anal);
    girl.oralsex(I_oralsex);
    girl.handjob(I_handjob);
    girl.tittysex(I_tittysex);

    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
    return false;
}

double JP_SOBisexual(const sGirl& girl, bool estimate)// not used
{
    if (girl.has_active_trait("Bisexual"))    return -1000;
    return 250;
}
