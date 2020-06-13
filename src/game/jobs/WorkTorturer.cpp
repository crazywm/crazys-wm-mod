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
#include "buildings/cBrothel.h"
#include "cRng.h"
#include <sstream>
#include "Game.hpp"
#include "cJobManager.h"

// `J` Job Brothel - General
bool WorkTorturer(sGirl& girl, bool Day0Night1, cRng& rng)
{
    Action_Types actiontype = ACTION_WORKTORTURER;
    if (Day0Night1) return false;        // Do this only once a day
    stringstream ss;
    if (girl.disobey_check(actiontype, JOB_TORTURER))
    {
        ss << "${name} refused to torture anyone.";
        girl.morality(1);
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} is assigned to torture people in the dungeon.";
    girl.morality(-1);
    int wages = 0, tips = 0;

    cGirls::EquipCombat(&girl);    // ready armor and weapons!

    // Complications
    //SIN: bit more variety for the above
    int roll(rng % 5);
    bool forFree = false;
    if (rng.percent(10))
    {
        girl.upd_Enjoyment(actiontype, -3);
        if (girl.has_active_trait("Sadistic") || girl.has_active_trait("Merciless") || girl.morality() < 30)
            ss << ("${name} hurt herself while torturing someone.\n");
        else
        {
            switch (roll)
            {
            case 0:        ss << "${name} felt bad torturing people as she could easily see herself in the victim.\n"; break;
            case 1:        ss << "${name} doesn't like this as she feels it is wrong to torture people.\n"; break;
            case 2:        ss << "${name} feels like a bitch after one of her torture victims wept the entire time and kept begging her to stop.\n"; break;
            case 3:        ss << "${name} feels awful after accidentally whipping someone in an excruciating place.\n"; break;
            case 4:        ss << "${name} didn't enjoy this as she felt sorry for the victim.\n"; break;
            default:    ss << "${name} didn't enjoy this for some illogical reason. [error]\n"; break; //shouldn't happen
            }
            //And a little randomness
            if (rng.percent(40))
            {
                roll = rng % 3;
                switch (roll)
                {
                case 0:
                    ss << ("She hates you for making her do this today.\n");
                    girl.pclove(-(rng % 2));
                    girl.pchate(rng % 2);
                    break;
                case 1:
                    ss << ("${name} is terrified that you treat people like this.\n");
                    girl.pcfear(rng % 6);
                    girl.obedience(rng % 2);
                    break;
                case 2:
                    ss << ("She learned a bit about medicine while trying to stop the pain.\n");
                    girl.medicine(rng % 10);
                    break;
                default:
                    ss << ("${name} did something completely unexpected. [error]");
                    break;
                }
            }
        }
        girl.AddMessage(ss.str(), IMGTYPE_DOM, Day0Night1);
    }
    else
    {
        girl.upd_Enjoyment(actiontype, +3);
        switch (roll)
        {
        case 0:        ss << ("${name} enjoyed her job working in the dungeon.\n"); break;
        case 1:        ss << ("${name} is turned on by the power of torturing people.\n"); break;
        case 2:        ss << ("${name} enjoyed trying out different torture devices and watching the effects on the victim.\n"); break;
        case 3:        ss << ("${name} spent her time in the dungeon whipping her victim in time to music to make amusing sound effects.\n"); break;
        case 4:        ss << ("${name} uses the victim's cries and screams to to figure out the 'best' areas to torture.\n"); break;
        default:    ss << ("${name} enjoyed this for some illogical reason. [error]\n"); break;
        }

        //And a little randomness
        if ((girl.morality() < 20 || girl.has_active_trait("Sadistic")) && rng.percent(20))
        {
            ss << ("${name} loved this so much she wouldn't accept any money, as long as you promise she can do it again soon.\n");
            girl.upd_Enjoyment(actiontype, +3);
            forFree = true;
        }
        if (rng.percent(20))
        {
            roll = rng % 4;
            switch (roll)
            {
            case 0:
                ss << ("${name} put so much energy into this it seems to have improved her fitness.\n");
                girl.constitution(rng % 3);
                break;
            case 1:
                ss << ("${name} went way too far, creating a hell of a mess. Still it looks like she had fun - she hasn't stopped smiling.\n");
                girl.happiness(rng % 5);
                    girl.upd_Enjoyment(actiontype, +1);
                // TODO so the torturing happens in the brothel where the torturer is, not in the dungeon?
                girl.m_Building->m_Filthiness += 15;
                break;
            case 2:
                ss << ("${name} over-exerted herself.");
                girl.health(-(rng % 5));
                girl.tiredness(rng % 5);
                break;
            case 3:
                ss << ("${name} appreciates that you entrust her with this kind of work.");
                girl.pclove(rng % 2);
                girl.pchate(-(rng % 2));
                break;
            default:
                ss << ("${name} did something completely unexpected. [error]");
                break;
            }
        }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
    }

    // Improve girl
    int xp = 15, skill = 1;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    if (!forFree)
    {
        wages += 65;
        //g_Game->gold().staff_wages(65);  // wages come from you
    }
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    girl.exp(xp);
    girl.morality(-2);
    girl.bdsm(skill);
    girl.upd_temp_stat(STAT_LIBIDO, 4);

    // WD: Update flag
    g_Game->dungeon().SetTortureDone();

    // Check for new traits
    cGirls::PossiblyGainNewTrait(&girl, "Sadistic", 30, actiontype, "${name} has come to enjoy her job so much that she has become rather Sadistic.", Day0Night1);
    cGirls::PossiblyGainNewTrait(&girl, "Merciless", 50, actiontype, "${name} extensive experience with torture has made her absolutely Merciless.", Day0Night1);

    return false;
}

double JP_Torturer(const sGirl& girl, bool estimate)        // not used
{
    //SIN - this is a special case.
    //AFAIK the torturer ID/skills not used at all in the job processing (apart from names in strings)
    //Who does the currently has ZERO affect on outcome.
    //So this stat just shows how much THIS girl (i.e. the torturer) will 'enjoy' job.
    //standardized per J's instructs

    double jobperformance =
        //main stat - how evil?
        (100 - girl.morality()) +
        //secondary stats - obedience, effectiveness and understanding of anatomy
        ((girl.obedience() + girl.combat() + girl.strength() + girl.medicine()) / 4) +
        //add level
        girl.level();

    //either
    if (girl.has_active_trait("Psychic"))                                                //I feel your pain... such suffering...
    {
        if (girl.has_active_trait("Masochist")) jobperformance += 30;    //... [smiles] and I like it!
        else                                            jobperformance -= 30;
    }

    jobperformance += girl.get_trait_modifier("work.torturer");

    return jobperformance;
}
