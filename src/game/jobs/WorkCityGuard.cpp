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
#include "buildings/cBuildingManager.h"
#include "character/cPlayer.h"
#include <sstream>
#include "Game.hpp"
#include "CLog.h"
#include "cGirlGangFight.h"
#include "cJobManager.h"

// `J` Job Arena - Staff
bool WorkCityGuard(sGirl& girl, bool Day0Night1, cRng& rng)
{
    Action_Types actiontype = ACTION_WORKSECURITY;
    stringstream ss;
    if (girl.disobey_check(actiontype, JOB_CITYGUARD))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} helps guard the city.\n \n";

    cGirls::EquipCombat(girl);    // ready armor and weapons!

    int roll_a = rng.d100(), roll_b = rng.d100();
    int wages = 150, tips = 0, enjoy = 0, enjoyc = 0, sus = 0;
    int imagetype = IMGTYPE_PROFILE;

    int agl = (girl.agility() / 2 + rng % (girl.combat() / 2));

    if (roll_a >= 50)
    {
        ss << "${name} didn't find any trouble today.";
        sus -= 5;
    }
    else if (roll_a >= 25)
    {
        ss << "${name} spotted a theif and ";
        if (agl >= 90)
        {
            ss << "was on them before they could blink.  Putting a stop to the theft.";
            sus -= 20;
            enjoy += 3;
        }
        else if (agl >= 75)
        {
            ss << "was on them before they could get away.  She is quick.";
            sus -= 15;
            enjoy += 1;
        }
        else if (agl >= 50)
        {
            ss << "was able to keep up, ";
            if (roll_b >= 50)    { sus += 5; ss << "but they ended up giving her the slip."; }
            else /*        */    { sus -= 10; ss << "and was able to catch them."; }
        }
        else
        {
            ss << "was left eating dust. Damn is she slow.";
            sus += 10;
            enjoy -= 3;
        }
    }
    else
    {
        auto tempgirl = g_Game->CreateRandomGirl(18, false, false, false, false, true);
        if (tempgirl)        // `J` reworked incase there are no Non-Human Random Girls
        {
            auto outcome = GirlFightsGirl(girl, *tempgirl);
            if (outcome == EFightResult::VICTORY)    // she won
            {
                enjoy += 3; enjoyc += 3;
                ss << "${name} ran into some trouble and ended up in a fight. She was able to win.";
                sus -= 20;
                imagetype = IMGTYPE_COMBAT;
            }
            else  // she lost or it was a draw
            {
                ss << "${name} ran into some trouble and ended up in a fight. She was unable to win the fight.";
                enjoy -= 1; enjoyc -= 1;
                sus += 10;
                imagetype = IMGTYPE_COMBAT;
            }
        }
        else
        {
            g_LogFile.log(ELogLevel::ERROR, "You have no Random Girls for your girls to fight");
            ss << "There were no criminals around for her to fight.\n \n";
            ss << "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
        }
    }

    girl.AddMessage(ss.str(), imagetype, Day0Night1);
    g_Game->player().suspicion(sus);
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);


    // Improve girl
    int xp = 8, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.combat(rng % skill);
    girl.magic(rng % skill);
    girl.agility(rng % skill);
    girl.constitution(rng % skill);


    girl.upd_Enjoyment(actiontype, enjoy);
    girl.upd_Enjoyment(ACTION_COMBAT, enjoyc);
    cGirls::PossiblyGainNewTrait(girl, "Tough", 20, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 30, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);

    return false;
}

double JP_CityGuard(const sGirl& girl, bool estimate)// not used
{
    // copied from Security
    int jobperformance = 0;
    if (estimate)    // for third detail string
    {
        jobperformance = (girl.combat())
            + (girl.magic() / 2)
            + (girl.agility() / 2);
    }
    else            // for the actual check
    {
        jobperformance = g_Dice % (girl.combat())
            + g_Dice % (girl.magic() / 3)
            + g_Dice % (girl.agility() / 3);

        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= t * 2;
    }

    jobperformance += girl.get_trait_modifier("work.cityguard");

    return jobperformance;
}
