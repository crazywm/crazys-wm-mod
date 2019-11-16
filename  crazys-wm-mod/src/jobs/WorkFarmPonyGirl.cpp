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
#pragma region //	Includes and Externs			//
#include "src/buildings/cBrothel.h"
#include "cRng.h"
#include <sstream>
#include "src/Game.hpp"

#pragma endregion

// `J` Job Farm - General
bool cJobManager::WorkFarmPonyGirl(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
	if (Day0Night1) return false;
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKHOUSEPET;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	int train = roll_a - girl->obedience() - girl->get_training(TRAINING_PUPPY);

	int wages = 100, tips = 0;
	int enjoy = 0, fame = 0, training = 0, ob = 0, fear = 0, love = 0;

	// `J` add in player's disposition so if the girl has heard of you
	int dispmod = 0;
	/* */if (g_Game->player().disposition() >= 100)	dispmod = 3;	// "Saint"
	else if (g_Game->player().disposition() >= 80)	dispmod = 2;	// "Benevolent"
	else if (g_Game->player().disposition() >= 50)	dispmod = 1;	// "Nice"
	else if (g_Game->player().disposition() >= 10)	dispmod = 0;	// "Pleasant"
	else if (g_Game->player().disposition() >= -10)	dispmod = 0;	// "Neutral"
	else if (g_Game->player().disposition() >= -50)	dispmod = -1;	// "Not nice"
	else if (g_Game->player().disposition() >= -80)	dispmod = -2;	// "Mean"
	else /*								  */	dispmod = -3;	// "Evil"

	int imagetype = IMGTYPE_PONYGIRL;
	int msgtype = Day0Night1;

	sGirl* headonduty = random_girl_on_job(*girl->m_Building, JOB_HEADGIRL, Day0Night1);
	string headname = (headonduty ? "Head Girl " + headonduty->m_Realname + "" : "the Head girl");

	if (train >= 50)			// they refuse to train
	{
		ss << " refused to train during the " << (Day0Night1 ? "night" : "day") << " shift.\n";
		ss << girlName << " is still in training, and is having difficulty accepting her new role. Today she was a bad girl!\n";
		if (girl->get_training(TRAINING_PONY) >= 50)
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
		ss << " trained to be a pony girl.\n \n";
		if (girl->get_training(TRAINING_PONY) >= 70)
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
						ss << girlName << "test message 3\n";
							training += 5;
					}
				else if (roll_b >= 20)
					{
						ss << girlName << "test message 4\n";
						training += 5;
					}
				else
					{
						ss << " test message 5\n";
						training += 5;
					}
			}
		else if (girl->get_training(TRAINING_PONY) >= 50)
			{
				ss << "has over 50";
				 if (roll_b >= 85)
				{
					ss << girlName << " test message 1\n";
				}
				else if (roll_b >= 70)
				{
					ss << girlName << "test message 2\n";
					training += 5;
				}
				else if (roll_b >= 55)
				{
					ss << girlName << " test message 3.\n";
					training += 5;
				}
				else if (roll_b >= 35)
				{
					ss << girlName << " test message 4\n";
					training += 5;
				}
				else if (roll_b >= 15)
				{
					ss << girlName << " test message 5\n";
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
					ss << girlName << " test line 1.";
					training += 5;
				}
				else if (roll_b >= 33)
				{
					ss << girlName << " test line 2.\n";
					training += 5;
				}
				else
				{
					if (girl->oralsex() < 35)
					{
						ss << "test line 3 low oral skill\n";
						training += 5;
						girl->oralsex(2);
						imagetype = IMGTYPE_ORAL;
					}
					else
					{
						ss << girlName << " test line 3 high oral skill.";
						training += 5;
						girl->oralsex(1);
						imagetype = IMGTYPE_ORAL;
					}
			}
		}
	}


#pragma endregion
#pragma region //	Job Performance			//


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


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);


	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);
	girl->upd_Training(TRAINING_PUPPY, training);
	girl->obedience(ob);

	return false;
}
