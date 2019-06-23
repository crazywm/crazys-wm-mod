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

#if 0
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern int g_Building;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmDominatrix(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;
	int bonus = 0;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming Dominatrix scences.\n\n";

	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to be a Dominatrix on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) { enjoy -= g_Dice % 3 + 1;	ss << "She didn't want be a Dominatrix today, but she did it anyway.\n\n"; }
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1;	ss << "She loved begin a Dominatrix today.\n\n"; }
	else /*            */{ enjoy += g_Dice % 2;		ss << "She had a pleasant day dominating her co-star.\n\n"; }
	jobperformance = enjoy * 2;

	//if (girl->check_virginity())
	//{
	//	girl->lose_virginity();	// `J` updated for trait/status
	//	jobperformance += 50;
	//	ss << "She is no longer a virgin.\n";
	//}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMDOM, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	g_Building = BUILDING_STUDIO;
	/*sCustomer* Cust = new sCustomer; g_Customers.GetCustomer(Cust, brothel);	Cust->m_Amount = 1; Cust->m_IsWoman = false;
	if (!girl->calc_pregnancy(Cust, false, 1.0))
	{
		g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
	}*/

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_DOM, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, skill = 3;

	if (girl->has_trait("Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->bdsm(g_Dice%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	//gain
	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1, EVENT_WARNING);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	//lose

	//delete Cust;
	return false;
}

double cJobManager::JP_FilmDom(sGirl* girl, bool estimate)// not used
{
	return 0;
}
#endif


