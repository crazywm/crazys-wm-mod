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
#include "Game.hpp"
#include "cGirls.h"
#include <sstream>

#pragma endregion

// `J` Job Brothel - General
bool WorkAdvertising(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKADVERTISING;
    std::stringstream ss;
    int roll_a = rng.d100();
    if (girl.disobey_check(actiontype, JOB_ADVERTISING))
    {
        ss << "${name} refused to advertise the brothel today.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} is assigned to advertise the brothel.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away

    int wages = 20, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_SIGN;
    auto msgtype = EVENT_SUMMARY;

#pragma endregion
#pragma region //    Job Performance            //

    // How much will she help stretch your advertising budget? Let's find out
    double multiplier = girl.job_performance(JOB_ADVERTISING, false);

    if(girl.is_unpaid())
        multiplier *= 0.9;    // unpaid slaves don't seem to want to advertise as much.
    if (girl.is_free())
        multiplier *= 1.1;    // paid free girls seem to attract more business

    // add some more randomness
#if 0 // work in progress

    ss << "She gave a tour of building making sure not to show the rooms that are messy.\n";
    if ()
    {
        ss << "as she was walking the people around she would make sure to slip in a few recommendations for girls the people would enjoy getting services from.";
    }
    else
    {
        ss << "She spent more time trying to flirt with the customers then actually getting anywhere with showing them around,\n";
        ss << "She still got the job done but it was nowhere as good as it could have been";
    }
    if ()
    {
        ss << "Most of the time she spent reading or doing anything else to spend her time to pass the time.\n";
    }
    else
    {
        ss << "She decided not to get in trouble so she just waited there silently for someone to come so she could do her job properly.\n";
    }

#endif

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    // Complications
    if (roll_a <= 10)
    {
        enjoy -= rng % 3 + 1;
        ss << "She was harassed and made fun of while advertising.\n";
        if (girl.happiness() < 50)
        {
            enjoy -= 1;
            ss << "Other then that she mostly just spent her time trying to not breakdown and cry.\n";
            fame -= rng % 2;
        }
        multiplier *= 0.8;
        fame -= rng % 2;
    }
    else if (roll_a >= 90)
    {
        enjoy += rng % 3 + 1;
        ss << "She made sure many people were interested in the buildings facilities.\n";
        multiplier *= 1.1;
        fame += rng % 3;
    }
    else
    {
        enjoy += rng % 2;
        ss << "She had an uneventful day advertising.\n";
    }

    /* `J` If she is not happy at her job she may ask you to change her job.
    *    Submitted by MuteDay as a subcode of bad complications but I liked it and made it as a separate section
    *    I will probably make it its own function when it works better.
    */
    if (girl.m_Enjoyment[actiontype] < -10)                         // if she does not like the job
    {
        int enjoyamount = girl.m_Enjoyment[actiontype];
        int saysomething = rng%girl.confidence() - enjoyamount;    // the more she does not like the job the more likely she is to say something about it
        saysomething -= girl.pcfear() / (girl.is_free() ? 2 : 1);    // reduce by fear (half if free)

        if (saysomething > 50)
        {
            girl.AddMessage("${name} comes up to you and asks you to change her job, She does not like advertizing.\n",
                            IMGTYPE_PROFILE, EVENT_WARNING);
        }
        else if (saysomething > 25)
        {
            ss << "She looks at you like she has something to say but then turns around and walks away.\n";
        }
    }

#pragma endregion
#pragma region    //    Money                    //

    ss << "She managed to stretch the effectiveness of your advertising budget by about " << int(multiplier) << "%.";
    // if you pay slave girls out of pocket  or if she is a free girl  pay them
    if (!girl.is_unpaid())
    {
        wages += 70;
        g_Game->gold().advertising_costs(70);
        ss << " You paid her 70 gold for her advertising efforts.";
    }
    else
    {
        ss << " You do not pay your slave for her advertising efforts.";
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), imagetype, msgtype);

    // now to boost the brothel's advertising level accordingly
    brothel->m_AdvertisingLevel += (multiplier / 100);

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Base Improvement and trait modifiers
    int xp = 5, skill = 3;
    /* */if (girl.has_active_trait("Quick Learner"))    { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    // EXP and Libido
    int I_xp = (rng % xp) + 1;                            girl.exp(I_xp);

    // primary improvement (+2 for single or +1 for multiple)
    int I_performance    = (rng % skill) + 1;                girl.performance(I_performance);
    int I_charisma        = (rng % skill) + 1;                girl.charisma(I_charisma);

    // secondary improvement (-1 for one then -2 for others)
    int I_service        = std::max(0, (rng % skill) - 1);        girl.service(I_service);
    int I_confidence    = std::max(0, (rng % skill) - 2);        girl.confidence(I_confidence);
    int I_fame            = fame;                                girl.fame(I_fame);

    // Update Enjoyment
    girl.upd_Enjoyment(actiontype, enjoy);
    if (girl.strip() > 50)
        cGirls::PossiblyGainNewTrait(girl, "Exhibitionist", 50, actiontype, "${name} has become quite the Exhibitionist, she seems to prefer Advertising topless whenever she can.", Day0Night1 == SHIFT_NIGHT);
    cGirls::PossiblyGainNewTrait(girl, "Charismatic", 70, actiontype, "Advertising on a daily basis has made ${name} more Charismatic.", Day0Night1 == SHIFT_NIGHT);
    // Lose Traits
    cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 40, actiontype, "${name} seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

#pragma endregion
    return false;
}

double JP_Advertising(const sGirl& girl, bool estimate)
{
    double cval = 0.0;
    double jobperformance = 0.0;
    if (estimate)    // for third detail string
    {
        jobperformance =
            ((girl.fame() + girl.charisma() + girl.performance()) / 3) +
            ((girl.beauty() + girl.confidence() + girl.service() + girl.intelligence()) / 4);
    }
    else            // for the actual check
    {
        cval = girl.performance();    // `J` added
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of performance skill to jobperformance
        }
        cval = girl.performance();
        if (cval > 0)    // `J` halved jobperformance to include performace without excessive change
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of service skill to jobperformance
        }
        cval = girl.charisma();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of charisma to jobperformance
        }
        cval = girl.beauty();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 10);  // add 10% of beauty to jobperformance
        }
        cval = girl.intelligence();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of intelligence to jobperformance
        }
        cval = girl.confidence();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 10);  // add 10% of confidence to jobperformance
        }
        cval = girl.fame();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 10);  // add 10% of fame to jobperformance
        }

        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.advertising");

    return jobperformance;
}
