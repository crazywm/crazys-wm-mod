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
#pragma region //    Includes and Externs            //
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include <sstream>
#include "Game.hpp"
#include "cJobManager.h"

#pragma endregion

// `J` Job Farm - General
bool WorkFarmPonyGirl(sGirl& girl, bool Day0Night1, cRng& rng)
{
    if (Day0Night1) return false;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKHOUSEPET;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    int train = roll_a - girl.obedience() - girl.get_training(TRAINING_PUPPY);

    int enjoy = 0, training = 0, ob = 0;

    int imagetype = IMGTYPE_PONYGIRL;

    const sGirl* headonduty = random_girl_on_job(*girl.m_Building, JOB_HEADGIRL, Day0Night1);
    string headname = (headonduty ? "Head Girl " + headonduty->FullName() + "" : "the Head girl");

    if (train >= 50)            // they refuse to train
    {
        ss << "${name} refused to train during the " << (Day0Night1 ? "night" : "day") << " shift.\n";
        ss << "${name} is still in training, and is having difficulty accepting her new role. Today she was a bad girl!\n";
        if (girl.get_training(TRAINING_PONY) >= 50)
        {
            ss << "high skill";
            if (roll_b >= 66)
            {
                ss << "test message 1.\n";
            }
            else if (roll_b >= 33)
            {
                ss << "test message 2";
                training += 5;
                ob += 2;
            }
            else
            {
                ss << "test message 3\n";
            }
        }
        else//low skill
        {
            ss << "\nlow skill.";
            if (roll_b >= 50)
            {
                ss << "test message 1 ";
            }
            else
            {
                ss << "test message 2.";
            }
        }
    }
    else /*She did the training*/
    {
        ss << "${name} trained to be a pony girl.\n \n";
        if (girl.get_training(TRAINING_PONY) >= 70)
            {
                ss << "has over 70 training";
                if (roll_b >= 80)
                    {
                        ss << "test message 1\n";
                        training += 5;
                    }
                else if (roll_b >= 60)
                    {
                        ss << "test message 2";
                            training += 5;
                    }
                else if (roll_b >= 40)
                    {
                        ss << "${name}test message 3\n";
                            training += 5;
                    }
                else if (roll_b >= 20)
                    {
                        ss << "${name}test message 4\n";
                        training += 5;
                    }
                else
                    {
                        ss << " test message 5\n";
                        training += 5;
                    }
            }
        else if (girl.get_training(TRAINING_PONY) >= 50)
            {
                ss << "has over 50";
                 if (roll_b >= 85)
                {
                    ss << "${name} test message 1\n";
                }
                else if (roll_b >= 70)
                {
                    ss << "${name}test message 2\n";
                    training += 5;
                }
                else if (roll_b >= 55)
                {
                    ss << "${name} test message 3.\n";
                    training += 5;
                }
                else if (roll_b >= 35)
                {
                    ss << "${name} test message 4\n";
                    training += 5;
                }
                else if (roll_b >= 15)
                {
                    ss << "${name} test message 5\n";
                    training += 5;
                }
                else
                {
                    ss << "test message 6\n";
                    training += 5;
                }
            }
        else/*LOW SKILL*/
            {
                ss << "girl has less then 20.\n";
                if (roll_b >= 66)
                {
                    ss << "${name} test line 1.";
                    training += 5;
                }
                else if (roll_b >= 33)
                {
                    ss << "${name} test line 2.\n";
                    training += 5;
                }
                else
                {
                    if (girl.oralsex() < 35)
                    {
                        ss << "test line 3 low oral skill\n";
                        training += 5;
                        girl.oralsex(2);
                        imagetype = IMGTYPE_ORAL;
                    }
                    else
                    {
                        ss << "${name} test line 3 high oral skill.";
                        training += 5;
                        girl.oralsex(1);
                        imagetype = IMGTYPE_ORAL;
                    }
            }
        }
    }


#pragma endregion
#pragma region //    Job Performance            //


    //enjoyed the work or not
    int roll = rng.d100();
    if (roll <= 5)
    {
        ss << "Some of the girls made fun of her for been a pony during the shift.";
        enjoy -= 1;
    }
    else if (roll <= 25)
    {
        ss << "She had a pleasant time training.";
        enjoy += 3;
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += 1;
    }


    girl.AddMessage(ss.str(), imagetype, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    int xp = 10, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.upd_Enjoyment(actiontype, enjoy);
    girl.upd_Training(TRAINING_PUPPY, training);
    girl.obedience(ob);

    return false;
}
