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
#include <sstream>
#include "cRng.h"
#include "buildings/cBrothel.h"
#include "Game.hpp"
#include "sStorage.hpp"

#pragma endregion

// `J` Job Brothel - General
bool WorkBeastCare(sGirl& girl, bool Day0Night1, cRng& rng)
{
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKCARING;
    stringstream ss;
    int roll_a = rng.d100();
    if (roll_a < 50 && girl.disobey_check(actiontype, JOB_BEASTCARER))
    {
        ss << "${name} refused to take care of beasts during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked taking care of beasts.\n \n";

    if (g_Game->storage().beasts() < 1)
    {
        ss << "There were no beasts in the brothel to take care of.\n \n";
    }

    cGirls::UnequipCombat(&girl);    // put that shit away

    int wages = 20, tips = 0;
    int enjoy = 0;
    int imagetype = IMGTYPE_FARM;
    int msgtype = Day0Night1;




#pragma endregion
#pragma region //    Job Performance            //

    int numhandle = girl.animalhandling() * 2;    // `J` first we assume a girl can take care of 2 beasts per point of animalhandling
    int addbeasts = -1;

    // `J` if she has time to spare after taking care of the current beasts, she may try to get some new ones.
    if (numhandle / 2 > g_Game->storage().beasts() && rng.percent(50))    // `J` these need more options
    {
        if (girl.magic() > 70 && girl.mana() >= 30)
        {
            addbeasts = (rng % ((girl.mana() / 30) + 1));
            ss << "${name}";
            ss << (addbeasts > 0 ? " used" : " tried to use") << " her magic to summon ";
            if (addbeasts < 2) ss << "a beast";
            else ss << addbeasts << " beasts";
            ss << " for the brothel" << (addbeasts > 0 ? "." : " but failed.");
            girl.magic(addbeasts);
            girl.mana(-30 * max(1, addbeasts));
        }
        else if (girl.animalhandling() > 50 && girl.charisma() > 50)
        {
            addbeasts =
                rng.percent(girl.combat()) +
                rng.percent(girl.charisma()) +
                rng.percent(girl.animalhandling());
            if (addbeasts <= 0)
            {
                addbeasts = 0;
                ss << "${name} tried to lure in some beasts for the brothel but failed.";
            }
            else
            {
                ss << "${name} lured in ";
                if (addbeasts == 1) ss << "a stray beast";
                else ss << addbeasts << " stray beasts";
                ss << " for the brothel.";
                girl.confidence(addbeasts);
            }
        }
        else if (girl.combat() > 50 && (girl.has_active_trait("Adventurer") || girl.confidence() > 70))
        {
            addbeasts = (rng % 2);
            ss << "${name} stood near the entrance to the catacombs, trying to lure out a beast by making noises of an injured animal.\n";
            if (addbeasts > 0) ss << "After some time, a beast came out of the catacombs. ${name} threw a net over it and wrestled it into submission.\n";
            else ss << "After a few hours, she gave up.";
            girl.combat(addbeasts);
        }
    }
    if (addbeasts >= 0) ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //
    if (roll_a <= 10)
    {
        enjoy -= rng % 3 + 1;
        addbeasts--;
        ss << "The animals were restless and disobedient.";
    }
    else if (roll_a >= 90)
    {
        enjoy += rng % 3 + 1;
        addbeasts++;
        ss << "She enjoyed her time working with the animals today.";
    }
    else
    {
        enjoy += rng % 2;
        ss << (addbeasts>=0 ? "Otherwise, the" : "The") << " shift passed uneventfully.\n \n";
    }

#pragma endregion
#pragma region    //    Create Items                //

#pragma endregion
#pragma region    //    Money                    //

    if (addbeasts < 0)    addbeasts = 0;
    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        wages = 0;
    }
    else
    {
        wages += g_Game->storage().beasts()/5;
        tips += addbeasts * 5;                // a little bonus for getting new beasts
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    g_Game->storage().add_to_beasts(addbeasts);
    girl.AddMessage(ss.str(), imagetype, msgtype);

    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve girl
    int xp = 5 + (g_Game->storage().beasts() / 10), skill = 2 + (g_Game->storage().beasts() / 20);

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.service(max(1, (rng % skill) - 1));
    girl.animalhandling(max(1, (rng % skill) + 1));

    girl.upd_Enjoyment(actiontype, enjoy);
    cGirls::PossiblyLoseExistingTrait(&girl, "Elegant", 40, actiontype, " Working with dirty, smelly beasts has damaged ${name}'s hair, skin and nails making her less Elegant.", Day0Night1);

#pragma endregion
    return false;
}

double JP_BeastCare(const sGirl& girl, bool estimate)
{
    //SIN - standardizing job performance calc per J's instructs
    double jobperformance =
        //main stat - first 100
        girl.animalhandling() +
        //secondary stats - second 100
        ((girl.intelligence() + girl.service() + girl.magic()) / 3) +
        //add level
        girl.level();

    //tiredness penalty
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.beastcare");


    return jobperformance;
}
