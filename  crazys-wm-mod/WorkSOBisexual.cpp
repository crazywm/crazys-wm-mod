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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

#pragma endregion

// `J` Job House - Training - Full_Time_Job
bool cJobManager::WorkSOBisexual(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname;
	int actiontype = ACTION_WORKTRAINING;
	// if she was not in JOB_SO_BISEXUAL yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_SO_BISEXUAL) girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
	if (girl->m_WorkingDay < 0) girl->m_WorkingDay = 0;
	girl->m_DayJob = girl->m_NightJob = JOB_SO_BISEXUAL;	// it is a full time job

	if (girl->has_trait( "Bisexual"))
	{
		ss << girlName << " is already Bisexual.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return false;	// not refusing
	}

	ss << "You procede to change " << girlName << "'s sexual orientation to Bisexual.\n \n";

	g_Girls.UnequipCombat(girl);	// not for patient

	int enjoy = 0, wages = 10, tips = 0;
	int startday = girl->m_WorkingDay;
	int libido = 0;
	int msgtype = Day0Night1, imagetype = IMGTYPE_SEX;

	// Base adjustment
	int tired = 5 + g_Dice % 11;
	girl->m_WorkingDay += 10 + g_Dice % 11;
	// Positive Stats/Skills

	if (girl->has_trait("Straight"))
	{
		girl->m_WorkingDay += girl->group() / 10;
		girl->m_WorkingDay += girl->normalsex() / 20;
		girl->m_WorkingDay += girl->lesbian() / 5;
		girl->m_WorkingDay += girl->oralsex() / 20;
		girl->m_WorkingDay += girl->tittysex() / 20;
		girl->m_WorkingDay += girl->anal() / 20;
	}
	else if (girl->has_trait("Lesbian"))
	{
		girl->m_WorkingDay += girl->group() / 10;
		girl->m_WorkingDay += girl->normalsex() / 5;
		girl->m_WorkingDay += girl->lesbian() / 20;
		girl->m_WorkingDay += girl->oralsex() / 15;
		girl->m_WorkingDay += girl->tittysex() / 15;
		girl->m_WorkingDay += girl->anal() / 15;
	}
	else
	{
		girl->m_WorkingDay += girl->group() / 5;
		girl->m_WorkingDay += girl->normalsex() / 10;
		girl->m_WorkingDay += girl->lesbian() / 10;
		girl->m_WorkingDay += girl->oralsex() / 20;
		girl->m_WorkingDay += girl->tittysex() / 20;
		girl->m_WorkingDay += girl->anal() / 20;
	}

	girl->m_WorkingDay += girl->obedience() / 20;
	if (girl->pcfear() > 50)				girl->m_WorkingDay += g_Dice % (girl->pcfear() / 20);		// She will do as she is told
	if (girl->pclove() > 50)				girl->m_WorkingDay += g_Dice % (girl->pclove() / 20);		// She will do what you ask
	// Negative Stats/Skills
	girl->m_WorkingDay -= girl->spirit() / 25;
	if (girl->pchate() > 30)				girl->m_WorkingDay -= g_Dice % (girl->pchate() / 10);		// She will not do what you want
	if (girl->happiness() < 50)				girl->m_WorkingDay -= 1 + g_Dice % 5;						// She is not feeling like it
	if (girl->health() < 50)				girl->m_WorkingDay -= 1 + g_Dice % 5;						// She is feeling sick
	if (girl->tiredness() > 50)				girl->m_WorkingDay -= 1 + g_Dice % 5;						// She is tired
	// Positive Traits
	if (girl->has_trait("Your Wife"))		girl->m_WorkingDay += g_Dice % 10;			// She wants to be with you, even if it is with another girl
	if (girl->has_trait("Porn Star"))		girl->m_WorkingDay += g_Dice % 10;			// She is used to having sex with anyone her director tells her to
	if (girl->has_trait("Whore"))			girl->m_WorkingDay += g_Dice % 8;			// She'll do anyone as long as they can pay
	if (girl->has_trait("Slut"))			girl->m_WorkingDay += g_Dice % 5;			// She'll do anyone
	if (girl->has_trait("Your Daughter"))	girl->m_WorkingDay += 2;					// She wants to partake in all that her father has to offer
	if (girl->has_trait("Actress"))			girl->m_WorkingDay += 2;					// She will do whatever her director tells her to
	if (girl->has_trait("Shape Shifter"))	girl->m_WorkingDay += 2;					// If she can become anyone she can have sex with anyone
	if (girl->has_trait("Succubus"))		girl->m_WorkingDay += 2;					// Males are easier to drain energy from but girls are ok
	if (girl->has_trait("Broodmother"))		girl->m_WorkingDay += 1;					// She prefers males who can get her pregnant
	if (girl->has_trait("Futanari"))		girl->m_WorkingDay += 1;					// If she has a dick she can put it anywhere
	// Negative Traits
	if (girl->has_trait("Broken Will"))	{ girl->m_WorkingDay -= g_Dice.bell(10, 20);	ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }
	if (girl->has_trait("Mind Fucked"))		girl->m_WorkingDay -= g_Dice.bell(10, 20);	// Does she even know who is fucking her?
	if (girl->has_trait("Retarded"))		girl->m_WorkingDay -= g_Dice.bell(5, 10);	// Does she even know who is fucking her?
	if (girl->has_trait("Slow Learner"))	girl->m_WorkingDay -= g_Dice % 10;			//
	if (girl->has_trait("Iron Will"))		girl->m_WorkingDay -= g_Dice % 5;			// She is set in her ways


	//	if (girl->check_virginity())				{}

	if (!is_sex_type_allowed(SKILL_ANAL, brothel))			girl->m_WorkingDay -= g_Dice % 3;
	if (!is_sex_type_allowed(SKILL_BDSM, brothel))			girl->m_WorkingDay -= g_Dice % 5;
	if (!is_sex_type_allowed(SKILL_FOOTJOB, brothel))		girl->m_WorkingDay -= g_Dice % 2;
	if (!is_sex_type_allowed(SKILL_GROUP, brothel))			girl->m_WorkingDay -= g_Dice % 15;
	if (!is_sex_type_allowed(SKILL_HANDJOB, brothel))		girl->m_WorkingDay -= g_Dice % 5;
	if (!is_sex_type_allowed(SKILL_LESBIAN, brothel))		girl->m_WorkingDay -= g_Dice % 10;
	if (!is_sex_type_allowed(SKILL_NORMALSEX, brothel))		girl->m_WorkingDay -= g_Dice % 10;
	if (!is_sex_type_allowed(SKILL_ORALSEX, brothel))		girl->m_WorkingDay -= g_Dice % 5;
	if (!is_sex_type_allowed(SKILL_TITTYSEX, brothel))		girl->m_WorkingDay -= g_Dice % 2;





	if (girl->disobey_check(actiontype, brothel))	girl->m_WorkingDay /= 2;	// if she disobeys, half her time is wasted

#pragma endregion
#pragma region //	Count the Days				//

	int total = girl->m_WorkingDay - startday;
	int xp = 1 + (max(0, girl->m_WorkingDay / 20));
	if (total <= 0)								// she lost time so more tired
	{
		tired += 5 + g_Dice % (-total);
		enjoy -= g_Dice % 3;
	}
	else if (total > 40)						// or if she trained a lot
	{
		tired += (total / 4) + g_Dice % (total / 2);
		enjoy += g_Dice % 3;
	}
	else										// otherwise just a bit tired
	{
		tired += g_Dice % (total / 3);
		enjoy -= g_Dice.bell(-2, 2);
	}

	if (girl->m_WorkingDay <= 0)
	{
		girl->m_WorkingDay = 0;
		msgtype = EVENT_WARNING;
		ss << "\nShe resisted all attempts to make her Bisexual.";
		tired += 5 + g_Dice % 11;
		wages = 0;
	}
	else if (girl->m_WorkingDay >= 100 && Day0Night1)
	{
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		msgtype = EVENT_GOODNEWS;
		ss << "\nHer Sexual Orientation conversion is complete. She is now Bisexual.";
		girl->remove_trait("Lesbian");	girl->add_trait("Bisexual");	girl->remove_trait("Straight");
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
		wages = 200;
	}
	else
	{
		ss << "Her Sexual Orientation conversion to Bisexual is ";
		if (girl->m_WorkingDay >= 100)
		{
			ss << "almost complete.";
			tired -= (girl->m_WorkingDay - 100) / 2;	// her last day so she rested a bit
		}
		else ss << "in progress (" << girl->m_WorkingDay << "%).";
		wages = min(100, girl->m_WorkingDay);
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	if (girl->is_slave()) wages /= 2;
	girl->m_Pay = wages;

	// Improve girl
	int I_lesbian = (g_Dice.bell(1, 10));
	int I_normalsex = (g_Dice.bell(1, 10));
	int I_group = (g_Dice.bell(2, 15));
	int I_anal = (g_Dice.bell(0, 5));
	int I_oralsex = (g_Dice.bell(0, 5));
	int I_handjob = (g_Dice.bell(0, 5));
	int I_tittysex = (g_Dice.bell(0, 3));

	girl->exp(xp);
	girl->tiredness(tired);
	girl->lesbian(I_lesbian);
	girl->normalsex(I_normalsex);
	girl->group(I_group);
	girl->anal(I_anal);
	girl->oralsex(I_oralsex);
	girl->handjob(I_handjob);
	girl->tittysex(I_tittysex);

	libido += girl->has_trait("Nymphomaniac") ? 3 : 1;
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);

	ss << "\n \nNumbers:"
		<< "\n Wages = " << (int)wages
		<< "\n Xp = " << xp
		<< "\n Libido = " << libido
		<< "\n Lesbian = " << I_lesbian
		<< "\n Normal Sex = " << I_normalsex
		<< "\n Group = " << I_group
		<< "\n Anal = " << I_anal
		<< "\n Oral Sex = " << I_oralsex
		<< "\n Handjob = " << I_handjob
		<< "\n Titty Sex = " << I_tittysex
		<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy;

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}

double cJobManager::JP_SOBisexual(sGirl* girl, bool estimate)// not used
{
	if (girl->has_trait( "Bisexual"))	return -1000;
	return 250;
}
