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
#include <sstream>
#include "src/buildings/cBrothel.h"

// `J` Job Brothel - General - job_is_cleaning
bool cJobManager::WorkCleaning(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;

	int actiontype = ACTION_WORKCLEANING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100();
	if (roll_a <= 50 && girl->disobey_check(actiontype, JOB_CLEANING))
	{
		ss << " refused to clean during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning the brothel.\n \n";

	cGirls::UnequipCombat(girl);	// put that shit away

	double jobperformance = JP_Cleaning(girl, false);		// `J` a replacement for job performance
	int enjoy = 0;
	int wages = 0;
	int tips = 0;
	int imagetype = IMGTYPE_MAID;
	int msgtype = Day0Night1;
	bool playtime = false;

	//SIN - a little more variety
	if (roll_a <= 10)
	{
		enjoy -= rng % 3 + 1;
		jobperformance *= 0.8;
		if (roll_b < 33)		ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 66)	ss << "An impatient group of customers got bored of waiting and roughly tried to 'use' her. She had to hide in a janitor closet for a while.";
		else					ss << "She did not like cleaning the brothel today.";
	}
	else if (roll_a <= 20)
	{
		--enjoy;
		jobperformance *= 0.9;
		if (roll_b < 33)		ss << "A waiting customer chatted with her, distracting her from her job.", enjoy +=3; // net result = enjoyment + 2
		else if (roll_b < 66)	ss << "While cleaning a bedroom, she put her hand right in a disgusting sticky patch. "
			<< (girl->has_trait( "Cum Addict") ? "It was her duty to stop and lick it clean." : "She wouldn't carry on until she had thoroughly washed her hands.");
		else					ss << "She walked into a room early, surprising the customer and landing her with some accidental 'crossfire'.";
	}
	else if (roll_a >= 90)
	{
		enjoy += rng % 3 + 1;
		jobperformance *= 1.1;
		if (roll_b < 33)		ss << "She cleaned the building while humming a pleasant tune.";
		else if (roll_b < 66)	ss << "A waiting customer chatted with her as she worked, and even helped her out a little.";
		else					ss << "She had a great time working today.";
	}
	else
	{
		enjoy += rng % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

	// `J` if she can clean more than is needed, she has a little free time after her shift
	if (brothel->m_Filthiness < jobperformance / 2) playtime = true;
	ss << "\n \nCleanliness rating improved by " << int(jobperformance);
	if (playtime)	//SIN: a bit more variation
	{
		ss << "\n \n" << girlName << " finished her cleaning early so ";
		roll_a = rng % 6;
		if (roll_a == 1 && !brothel->is_sex_type_allowed(SKILL_ORALSEX)) roll_a = 0;
		if (roll_a == 1 && girl->has_trait( "Lesbian")) roll_a = 0;
		if (roll_a != 2 && girl->tiredness() >= 80) roll_a = 2;
		if (roll_a != 1 && rng.percent(30) && girl->has_trait( "Cum Addict")) roll_a = 1;

		switch (roll_a)
		{
		case 1:
		{
			ss << "she hung out at the brothel, offering to \"clean off\" finished customers with her mouth.\n";//Made it actually use quote marks CRAZY
			tips = rng % 6 - 1; //how many 'tips' she clean? <6 for now, considered adjusting to amount playtime - didn't seem worth complexity
			if (tips > 0)
			{
				brothel->m_Happiness += (tips);
				girl->oralsex(tips / 2);
				tips *= 5; //customers tip 5 gold each
				ss << "She got " << tips << " in tips for this extra service.\n";
				imagetype = IMGTYPE_ORAL;
			}
			else
			{
				ss << "No one was interested.";
			}
		}break;

		case 2:
		{
			ss << "she had a rest.";
			girl->tiredness(-(rng % 10 + 1));
		}break;

		case 3:
		{
			ss << "she hung out around the brothel chatting with staff and patrons.\n";
			girl->charisma((rng % 3) + 1);
			girl->confidence((rng % 2) + 1);
		}break;

		case 4:
		{
			ss << "she spent some time training and getting herself fitter.\n";
			girl->constitution(rng % 2);
			girl->agility(rng % 2);
			girl->beauty(rng % 2);
			girl->spirit(rng % 2);
			girl->combat(rng % 2);
		}break;

		case 5:
		{
			if (girl->has_trait( "Your Wife") || rng.percent(30) && !girl->has_trait( "Your Daughter") && !girl->has_trait( "Lesbian")) //Flipped to fix the daughter issue

			{


				tips = 20; // you tip her for cleaning you
				ss << "she came to your room and cleaned you.\n \n" << girlName << " ran you a hot bath and bathed naked with you.";/* Need a check here so your daughters won't do this zzzzz FIXME*/
				imagetype = IMGTYPE_BATH;

				if (brothel->is_sex_type_allowed(SKILL_TITTYSEX))
				{
					ss << " Taking care to clean your whole body carefully, She rubbed cleansing oils over your back and chest with her ";
					if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Titanic Tits"))
						ss << "enormous, heaving ";
					else if (girl->has_trait( "Big Boobs") || girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs"))
						ss << "big, round ";
					else if (girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts") || girl->has_trait( "Small Boobs"))
						ss << "cute little ";
					else ss << "nice, firm ";
					ss << "breasts. ";
				}
				if (brothel->is_sex_type_allowed(SKILL_ORALSEX))
				{
					ss << "She finished by cleaning your cock with her " << (girl->has_trait( "Dick-Sucking Lips") ? "amazing dick-sucking lips" : "mouth") << ". ";
					if (girl->has_trait( "Cum Addict"))
					{
						ss << "She didn't stop 'cleaning' until you came in her mouth.\nAfterward, you notice her carefully "
							<< "crawling around and licking up every stray drop of cum. She must really love cleaning.\n";
						girl->oralsex(rng % 2);
						girl->spirit(-(rng % 2));
						tips += (rng % 20);  // tip her for hotness
					}
					imagetype = IMGTYPE_ORAL;
				}
				girl->service(rng % 5);
				girl->medicine(rng % 2);
				girl->obedience(rng % 4);
				girl->pclove(rng % 5);
			}
			else
			{
				ss << "she hung out around the brothel, watching the other girls and trying to learn tricks and techniques.\n";
				girl->normalsex(rng % 2);
				girl->anal(rng % 2);
				girl->oralsex(rng % 2);
				girl->bdsm(rng % 2);
				girl->lesbian(rng % 2);
			}
		}break;

		default:
			ss << "she hung out around the brothel a bit.";
			girl->upd_temp_stat(STAT_LIBIDO, rng % 3 + 1, true);
			girl->happiness((rng % 3) + 1);
			break;
		}
	}



	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance = jobperformance * (jobperformance >= 0 ? 0.9 : 1.1);
		wages = 0;
	}
	else
	{
		wages = int(jobperformance); // `J` Pay her based on how much she cleaned
	}

	brothel->m_Filthiness -= (int)jobperformance;

	girl->m_Tips = max(0, tips);	//job is classed as player-paid, so this is only way to give her tip
	girl->m_Pay = max(0, wages);

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);


	// Improve girl
	int xp = 5, libido = 1, skill = 3;
	if (enjoy > 1)										{ xp += 1; skill += 1; }
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((rng % xp) + 2);
	girl->service((rng % skill) + 2);
	girl->constitution(rng % skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	if (rng.percent(girl->service()))
		cGirls::PossiblyGainNewTrait(girl, "Maid", 70, actiontype, girlName + " has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
	// Lose Traits
	if (rng.percent(girl->service()))
		cGirls::PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}


double cJobManager::JP_Cleaning(sGirl* girl, bool estimate)
{
	//SIN - standardizing job performance calc per J's instructs
	double jobperformance =
		//main stat - first 100
		girl->service() +
		//secondary stats - second 100
		((girl->morality() + girl->obedience() + girl->agility()) / 3) +
		//add level
		girl->level();

	//tiredness penalty
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.cleaning");

	return jobperformance;
}
