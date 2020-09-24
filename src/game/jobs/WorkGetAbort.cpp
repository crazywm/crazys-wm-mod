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
#include "Game.hpp"
#include "cGirls.h"

namespace settings {
    extern const char* PREG_COOL_DOWN;
}

#pragma endregion

static void process_happiness(std::stringstream& ss, sGirl& girl, int happy, int& hate, std::array<const char*, 7> messages) {
    if (happy < -50)
    {
        ss << messages[0];
        hate += 10;
        girl.add_temporary_trait("Pessimist", 20);
    }
    else if (happy < -25)
    {
        ss << messages[1];
        hate += 5;
    }
    else if (happy < -5)
    {
        ss << messages[2];
    }
    else if (happy < 10)
    {
        ss << messages[3];
    }
    else if (happy < 25)
    {
        ss << messages[4];
    }
    else if (happy < 50)
    {
        ss << messages[5];
    }
    else
    {
        ss << messages[6];
        hate -= 5;
        girl.add_temporary_trait("Optimist", 20);
    }

    ss << "\n";
}

// `J` Job Clinic - Surgery
bool WorkGetAbort(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    std::stringstream ss;
    // if she was not in surgery last turn, reset working days to 0 before proceding
    if (girl.m_YesterDayJob != JOB_GETABORT) { girl.m_WorkingDay = girl.m_PrevWorkingDay = 0; }
    girl.m_DayJob = girl.m_NightJob = JOB_GETABORT;    // it is a full time job

    if (!girl.is_pregnant())
    {
        ss << "${name} is not pregant so she was sent to the waiting room.";
        if (Day0Night1 == SHIFT_DAY)    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        return false;    // not refusing
    }
    bool hasDoctor = brothel->num_girls_on_job(JOB_DOCTOR, Day0Night1) > 0;
    int numnurse = brothel->num_girls_on_job(JOB_NURSE, Day0Night1);
    if (!hasDoctor)
    {
        ss << "${name} does nothing. You don't have any Doctors working. (require 1) ";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return false;    // not refusing
    }
    ss << "${name} is in the Clinic to get an abortion.\n \n";

    auto msgtype = Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    cGirls::UnequipCombat(girl);    // not for patient

#pragma endregion
#pragma region //    Count the Days                //

    if (Day0Night1 == SHIFT_DAY)    // the Doctor works on her durring the day
    {
        girl.m_WorkingDay++;
    }
    else    // and if there are nurses on duty, they take care of her at night
    {
        if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0)
        {
            girl.m_WorkingDay++;
            girl.happiness(5);
            girl.mana(5);
        }
    }

#pragma endregion
#pragma region //    Night Check                //

    if (girl.m_WorkingDay >= 2 && Day0Night1 == SHIFT_NIGHT)
    {
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        ss << "The girl had an abortion.\n";
        msgtype = EVENT_GOODNEWS;

        // `J` first set the base stat modifiers
        int happy = -10, health = -20, mana = -20, spirit = -5, love = -5, hate = 5;

        if (numnurse > 0)
        {
            ss << "The Nurse tried to keep her healthy and happy during her recovery.\n";
            // `J` then adjust if a nurse helps her through it
            happy += 10;    health += 10;    mana += 10;    spirit += 5;    love += 1;    hate -= 1;
        }
        else
        {
            ss << "She is sad and has lost some health during the operation.\n";
        }

        happy += girl.get_trait_modifier("preg.abort.happy");
        love += girl.get_trait_modifier("preg.abort.love");
        hate += girl.get_trait_modifier("preg.abort.hate");

        // `J` next, check traits
        if (girl.has_active_trait("Demon"))        // "I'm going to hell anyway..."
        {
            mana += 10;
        }
        if (girl.has_active_trait("Angel"))        // "I'm going to hell."
        {
            mana -= 10;
        }

        if (girl.has_active_trait("Fragile"))        // natural adj
        {
            health -= 5;
        }
        if (girl.has_active_trait("Tough"))        // natural adj
        {
            health += 5;
        }

        // `J` finally see what type of pregnancy it is and get her reaction to the abortion.
        if (girl.has_status(STATUS_PREGNANT))
        {
            process_happiness(ss, girl, happy, hate,
                    {"She is very distraught about the loss of her baby.",
                     "She is distraught about the loss of her baby.",
                     "She is sad about the loss of her baby.",
                     "She accepts that she is not ready to have her baby.",
                     "She is glad she is not going to have her baby.",
                     "She is happy she is not going to have her baby.",
                     "She is overjoyed not to be forced to carry her baby."
                     });
        }
        else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
        {
            // `J` adjust her happiness by her hate-love for you
            happy += int(((girl.pchate() + hate) - (girl.pclove() + love)) / 2);
            if (girl.has_active_trait("Your Wife"))// "Why?"
            {
                happy -= 20;    spirit -= 1;    love -= 3;    hate += 0;
            }

            process_happiness(ss, girl, happy, hate,
                              {"She is very distraught about the loss of your baby.",
                               "She is distraught about the loss of your baby.",
                               "She is sad about the loss of your baby.",
                               "She accepts that she is not ready to have your baby.",
                               "She is glad she is not going to have your baby.",
                               "She is happy she is not going to have your baby.",
                               "She is overjoyed not to be forced to carry your hellspawn."
                              });
        }
        else if (girl.has_status(STATUS_INSEMINATED))
        {
            // `J` Some traits would react differently to non-human pregnancies.
            happy += girl.get_trait_modifier("inseminated.abort.happy");
            love += girl.get_trait_modifier("inseminated.abort.love");
            hate += girl.get_trait_modifier("inseminated.abort.hate");

            if (girl.has_active_trait("Angel"))        // "DEAR GOD, WHAT WAS THAT THING?"
            {
                spirit -= 5;    mana -= 5;
            }

            process_happiness(ss, girl, happy, hate,
                              {"She is very distraught about the loss of the creature growing inside her.",
                               "She is distraught about the loss of the creature growing inside her.",
                               "She is sad about the loss of the creature growing inside her.",
                               "She accepts that she is not ready to bring a strange creature into this world.",
                               "She is glad she is not going to have to carry that strange creature inside her.",
                               "She is happy she is not going to have to carry that strange creature inside her.",
                               "She is overjoyed not to be forced to carry that hellspawn anymore."
                              });
        }
        // `J` now apply all the stat changes and finalize the transaction
        girl.happiness(happy);
        girl.health(health);
        girl.mana(mana);
        girl.spirit(spirit);
        girl.pclove(love);
        girl.pchate(hate);

        girl.m_ChildrenCount[CHILD09_ABORTIONS]++;
        girl.clear_pregnancy();
        girl.m_PregCooldown = g_Game->settings().get_integer(settings::PREG_COOL_DOWN);
        girl.FullJobReset(JOB_RESTING);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
    }
    else
    {
        ss << "The abortion is in progress (1 day remaining).";
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

    // Improve girl
    int libido = -8;
    if (girl.has_active_trait("Lesbian")) libido += numnurse;
    if (girl.has_active_trait("Masochist")) libido += 1;
    girl.upd_temp_stat(STAT_LIBIDO, libido);

#pragma endregion
    return false;
}

double JP_GetAbort(const sGirl& girl, bool estimate)
{
    if (!girl.is_pregnant())            return -1000;    // X - not needed
    if (girl.carrying_players_child())    return 1;        // E - its your's
    if (girl.carrying_monster())        return 150;        // B - Beast
    return 100;                                            // C - customer's child
}
