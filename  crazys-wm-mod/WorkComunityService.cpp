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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cCentre.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;
extern cPlayer* The_Player;

#pragma endregion

// `J` Job Centre - General
bool cJobManager::WorkComunityService(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCENTRE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked doing community service.\n \n";

	g_Building = BUILDING_CENTRE;

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	bool blow = false, sex = false;
	int dispo = 0;
	int wages = 20, tips = 0;
	int enjoy = 0, help = 0, fame = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_ComunityService(girl, false);

	//Adding cust here for use in scripts...
	sCustomer Cust = GetMiscCustomer(*brothel);

	// `J` merged slave/free messages and moved actual dispo change to after
	/* */if (jobperformance >= 245)
	{
		dispo = 12;
		ss << " She must be perfect at this.\n \n";
	}
	else if (jobperformance >= 185)
	{
		dispo = 10;
		ss << " She's unbelievable at this and is always getting praised by people for her work.\n \n";
	}
	else if (jobperformance >= 145)
	{
		dispo = 8;
		ss << " She's good at this job and gets praised by people often.\n \n";
	}
	else if (jobperformance >= 100)
	{
		dispo = 6;
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
	}
	else if (jobperformance >= 70)
	{
		dispo = 4;
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
	}
	else
	{
		dispo = 2;
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
	}



	//try and add randomness here
	if (girl->has_trait( "Nymphomaniac") && g_Dice.percent(30) && !girl->check_virginity()
		&& !girl->has_trait( "Lesbian") && girl->libido() > 75
		&& (!brothel->m_RestrictNormal || !brothel->m_RestrictAnal))
	{
		sex = true;
		ss << "Her Nymphomania got the better of her today and she decided the best way to serve her community was on her back!\n \n";
	}

	if (g_Dice.percent(30) && girl->intelligence() < 55)//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
	{
		blow = true;
		ss << "An elderly fellow managed to convince " << girlName << " that the best way to serve her community was on her knees. She ended up giving him a blow job!\n \n";
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

	girl->upd_Enjoyment(actiontype, enjoy);


	if (sex)
	{
		if (!brothel->m_RestrictNormal && (roll_b <= 50 || brothel->m_RestrictAnal)) //Tweak to avoid an issue when roll > 50 && anal is restricted
		{
			girl->normalsex(2);
			imagetype = IMGTYPE_SEX;
			if (girl->check_virginity())
			{
				girl->lose_virginity();	// `J` updated for trait/status
				ss << "\nShe is no longer a virgin.\n";
			}
			if (!girl->calc_pregnancy(Cust, false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
			}
		}
		else if (!brothel->m_RestrictAnal)
		{
			girl->anal(2);
			imagetype = IMGTYPE_ANAL;
		}
		brothel->m_Happiness += 100;
		girl->upd_temp_stat(STAT_LIBIDO, -20, true);
		girl->upd_Enjoyment(ACTION_SEX, +3);
		fame += 1;
		dispo += 6;
	}
	else if (blow)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		dispo += 4;
		girl->oralsex(2);
		fame += 1;
		imagetype = IMGTYPE_ORAL;
	}

	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
		wages = 0;
	}
	else
	{
		ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
		g_Gold.staff_wages(100);  // wages come from you
		dispo = int(dispo*1.5);
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	The_Player->disposition(dispo);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	help += (int)(jobperformance / 10);		//  1 helped per 10 point of performance

	ss.str("");
	ss << girlName << " helped " << help << " people today.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 50 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	if (g_Dice % 2 == 1)	girl->intelligence(g_Dice%skill);
	else				girl->charisma(g_Dice%skill);
	girl->service(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);


#pragma endregion
	return false;
}

double cJobManager::JP_ComunityService(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(girl->intelligence() / 2 +
		girl->charisma() / 2 +
		girl->service());
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}



	//good traits
	if (girl->has_trait( "Charismatic"))		jobperformance += 20;
	if (girl->has_trait( "Sexy Air"))			jobperformance += 10;
	if (girl->has_trait( "Cool Person"))		jobperformance += 10;	//people love to be around her
	if (girl->has_trait( "Cute"))				jobperformance += 5;
	if (girl->has_trait( "Charming")) 		jobperformance += 15;	//people like charming people
	if (girl->has_trait( "Optimist"))			jobperformance += 10;

	//bad traits
	if (girl->has_trait( "Dependant"))		jobperformance -= 50;	// needs others to do the job
	if (girl->has_trait( "Clumsy"))			jobperformance -= 20;	//spills food and breaks things often
	if (girl->has_trait( "Aggressive"))		jobperformance -= 20;	//gets mad easy and may attack people
	if (girl->has_trait( "Nervous"))			jobperformance -= 30;	//don't like to be around people
	if (girl->has_trait( "Meek"))				jobperformance -= 20;

	return jobperformance;
}
