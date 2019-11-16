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

#pragma endregion

// `J` Job Clinic - Staff
bool cJobManager::WorkMechanic(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMECHANIC;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	if (girl->disobey_check(actiontype, JOB_MECHANIC))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a mechanic.\n \n";


	cGirls::UnequipCombat(girl);	// put that shit away, you'll scare off the customers!
	int wages = 25, tips = 0;
	int enjoy = 0, fame = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Mechanic(girl, false);


	if (jobperformance >= 245)
	{
		wages += 155;
		ss << "She must be the perfect mechanic patients go on and on about her and always come to see her when she works.\n \n";
	}
	else if (jobperformance >= 185)
	{
		wages += 95;
		ss << "She's unbelievable at this and is always getting praised by the patients for her work.\n \n";
	}
	else if (jobperformance >= 135)
	{
		wages += 55;
		ss << "She's good at this job and gets praised by the patients often.\n \n";
	}
	else if (jobperformance >= 85)
	{
		wages += 15;
		ss << "She made a few mistakes but overall she is okay at this.\n \n";
	}
	else if (jobperformance >= 65)
	{
		wages -= 5;
		ss << "She was nervous and made a few mistakes. She isn't that good at this.\n \n";
	}
	else
	{
		wages -= 15;
		ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
	}


	//try and add randomness here
	if (girl->beauty() > 85 && rng.percent(20))
	{
		wages += 25;
		ss << " Stunned by her beauty a patient left her a great tip.\n \n";
	}
	if (girl->has_trait( "Construct") && rng.percent(15))
	{
		wages += 15;
		ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
	}
	if (girl->has_trait( "Half-Construct") && rng.percent(15))
	{
		wages += 15;
		ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
	}
	if (girl->has_trait( "Clumsy") && rng.percent(15))
	{
		wages -= 15;
		ss << " Her clumsy nature caused her to drop parts everywhere.\n";
	}
	if (girl->has_trait( "Pessimist") && rng.percent(5))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << " Her pessimistic mood depressed the patients making them tip less.\n";
		}
		else
		{
			wages += 10;
			ss << girlName << " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
		}
	}
	if (girl->has_trait( "Optimist") && rng.percent(5))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << girlName << " was in a cheerful mood but the patients thought she needed to work more on her services.\n";
		}
		else
		{
			wages += 10;
			ss << " Her optimistic mood made patients cheer up increasing the amount they tip.\n";
		}
	}


#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	if (roll_a <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll_a <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		enjoy += 1;
	}


#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	int roll_max = (girl->intelligence() + girl->service());
	roll_max /= 4;
	wages += 10 + rng%roll_max;
	wages += 5 * brothel->num_girls_on_job(JOB_GETREPAIRS, Day0Night1);	// `J` pay her 5 for each patient you send to her
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve stats
	int xp = 10, libido = 1, skill = 2;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 50 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	girl->intelligence(rng%skill + 1);
	girl->medicine(rng%skill);
	girl->service(rng%skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(actiontype, enjoy);

	//gain traits
	cGirls::PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Strong", 60, actiontype, "Handling heavy parts and working with heavy tools has made " + girl->m_Realname + " much Stronger.", Day0Night1);

	//lose traits
	cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 20, actiontype, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	cGirls::PossiblyLoseExistingTrait(girl, "Elegant", 40, actiontype, " Working with dirty, greasy equipment has damaged " + girl->m_Realname + "'s hair, skin and nails making her less Elegant.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Mechanic(sGirl* girl, bool estimate)// not used
{
	double jobperformance
		= (girl->intelligence() +
		girl->medicine() / 2 +
		girl->service() / 2);
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.mechanic");
	
	return jobperformance;
}
