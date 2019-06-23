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
#include "cClinic.h"
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
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

#pragma endregion

// `J` Job Clinic - Surgery
bool cJobManager::WorkGetAbort(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_GENERAL;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	// if she was not in surgery last turn, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_GETABORT) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }
	girl->m_DayJob = girl->m_NightJob = JOB_GETABORT;	// it is a full time job

	if (!girl->is_pregnant())
	{
		ss << " is not pregant so she was sent to the waiting room.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		return false;	// not refusing
	}
	bool hasDoctor = g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, Day0Night1) > 0;
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, Day0Night1);
	if (!hasDoctor)
	{
		ss << " does nothing. You don't have any Doctors working. (require 1) ";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}
	ss << " is in the Clinic to get an abortion.\n \n";

	int msgtype = Day0Night1;
	g_Girls.UnequipCombat(girl);	// not for patient

#pragma endregion
#pragma region //	Count the Days				//

	if (Day0Night1 == SHIFT_DAY)	// the Doctor works on her durring the day
	{
		girl->m_WorkingDay++;
	}
	else	// and if there are nurses on duty, they take care of her at night
	{
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 0)
		{
			girl->m_WorkingDay++;
			girl->happiness(5);
			girl->mana(5);
		}
	}

#pragma endregion
#pragma region //	Night Check				//

	if (girl->m_WorkingDay >= 2 && Day0Night1 == SHIFT_NIGHT)
	{
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		ss << "The girl had an abortion.\n";
		msgtype = EVENT_GOODNEWS;

		// `J` first set the base stat modifiers
		int happy = -10, health = -20, mana = -20, spirit = -5, love = -5, hate = 5;

		if (numnurse > 0)
		{
			ss << "The Nurse tried to keep her healthy and happy during her recovery.\n";
			// `J` then adjust if a nurse helps her through it
			happy += 10;	health += 10;	mana += 10;	spirit += 5;	love += 1;	hate -= 1;
		}
		else
		{
			ss << "She is sad and has lost some health during the operation.\n";
		}

		// `J` next, check traits
		if (girl->has_trait( "Pessimist"))	// natural adj
		{
			happy -= 5;		health += 0;	mana += 0;	spirit -= 1;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Optimist"))		// natural adj
		{
			happy += 5;		health += 0;	mana += 0;	spirit += 1;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Lesbian"))		// reaffirms her dislike of men.
		{
			happy += 5;		health += 0;	mana += 0;	spirit += 0;	love -= 1;	hate += 1;
		}
		if (girl->has_trait( "Sadistic"))		// "If only someone had done this to your father..."
		{
			happy += 0;		health += 0;	mana += 0;	spirit += 1;	love -= 1;	hate += 0;
		}
		if (girl->has_trait( "Masochist"))	// "If only someone had done this to me..."
		{
			happy += 0;		health += 0;	mana += 0;	spirit -= 1;	love -= 1;	hate -= 1;
		}
		if (girl->has_trait( "Your Daughter"))// "Why Daddy?"
		{
			happy += 0;		health += 0;	mana += 0;	spirit -= 1;	love -= 3;	hate += 0;
		}
		if (girl->has_trait( "MILF"))			// "But I want more children"
		{
			happy -= 5;		health += 0;	mana += 0;	spirit += 0;	love -= 1;	hate += 1;
		}
		if (girl->has_trait( "Twisted"))		// Twisted
		{
			happy += 2;		health += 0;	mana += 0;	spirit += 1;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Demon"))		// "I'm going to hell anyway..."
		{
			happy += 5;		health += 0;	mana += 10;	spirit += 5;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Angel"))		// "I'm going to hell."
		{
			happy -= 10;	health += 0;	mana -= 10;	spirit -= 5;	love -= 5;	hate += 5;
		}
		if (girl->has_trait( "Adventurer"))	// "At least a kid will not get in my way."
		{
			happy += 5;		health += 0;	mana += 0;	spirit += 1;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Fragile"))		// natural adj
		{
			happy += 0;		health -= 5;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Tough"))		// natural adj
		{
			happy += 0;		health += 5;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
		}
		if (girl->has_trait( "Shape Shifter"))// "The baby may have caused complications when I change shape."
		{
			happy += 2;		health += 0;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
		}


		// `J` finally see what type of pregnancy it is and get her reaction to the abortion.
		if (girl->m_States & (1 << STATUS_PREGNANT))
		{
			if (happy < -50)
			{
				ss << "She is very distraught about the loss of her baby.\n";
				hate += 10;
				girl->add_trait("Pessimist", false);
			}
			else if (happy < -25)
			{
				ss << "She is distraught about the loss of her baby.\n";
				hate += 5;
			}
			else if (happy < -5)
			{
				ss << "She is sad about the loss of her baby.\n";
			}
			else if (happy < 10)
			{
				ss << "She accepts that she is not ready to have her baby.\n";
			}
			else if (happy < 25)
			{
				ss << "She is glad she is not going to have her baby.\n";
			}
			else if (happy < 50)
			{
				ss << "She is happy she is not going to have her baby.\n";
			}
			else
			{
				ss << "She is overjoyed not to be forced to carry her baby.\n";
				hate -= 5;
				girl->add_trait("Optimist", false);
			}
		}
		else if (girl->m_States & (1 << STATUS_PREGNANT_BY_PLAYER))
		{
			// `J` adjust her happiness by her hate-love for you
			happy += int(((girl->pchate() + hate) - (girl->pclove() + love)) / 2);
			if (girl->has_trait( "Your Wife"))// "Why?"
			{
				happy -= 20;		health += 0;	mana += 0;	spirit -= 1;	love -= 3;	hate += 0;
			}

			if (happy < -50)
			{
				ss << "She is very distraught about the loss of your baby.\n";
				hate += 10;
				girl->add_trait("Pessimist", false);
			}
			else if (happy < -25)
			{
				ss << "She is distraught about the loss of your baby.\n";
				hate += 5;
			}
			else if (happy < -5)
			{
				ss << "She is sad about the loss of your baby.\n";
			}
			else if (happy < 10)
			{
				ss << "She accepts that she is not ready to have your baby.\n";
			}
			else if (happy < 25)
			{
				ss << "She is glad she is not going to have your baby.\n";
			}
			else if (happy < 50)
			{
				ss << "She is happy she is not going to have your baby.\n";
			}
			else
			{
				ss << "She is overjoyed not to be forced to carry your hellspawn.\n";
				hate -= 5;
				girl->add_trait("Optimist", false);
			}
		}
		else if (girl->m_States & (1 << STATUS_INSEMINATED))
		{
			// `J` Some traits would react diferently to non-human pregnancys.
			if (girl->has_trait( "Adventurer"))	// "It could have been interesting to see what became of that."
			{
				happy -= 2;		health += 0;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
			}
			if (girl->has_trait( "Angel"))		// "DEAR GOD, WHAT WAS THAT THING?"
			{
				happy += 5;		health += 0;	mana -= 5;	spirit -= 5;	love -= 5;	hate += 5;
			}
			if (girl->has_trait( "Cat Girl"))		// "No kittens for me. :("
			{
				happy -= 2;		health += 0;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
			}
			if (girl->has_trait( "Demon"))		// "I guess my evil brood will have to wait."
			{
				happy -= 10;	health += 0;	mana += 0;	spirit += 0;	love -= 2;	hate += 2;
			}
			if (girl->has_trait( "Shape Shifter"))// "What would that have looked like?"
			{
				happy -= 2;		health += 0;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
			}
			if (girl->has_trait( "Twisted"))		// "What would that have felt like?"
			{
				happy -= 2;		health += 0;	mana += 0;	spirit += 0;	love += 0;	hate += 0;
			}
			if (girl->has_trait( "Queen"))		// "Thank God I didn't have to carry that."
			{
				happy += 40;		health += 0;	mana += 0;	spirit += 0;	love += 8;	hate -= 4;
			}
			if (girl->has_trait( "Princess"))		// "Thank God I didn't have to carry that."
			{
				happy += 20;		health += 0;	mana += 0;	spirit += 0;	love += 4;	hate -= 2;
			}
			if (girl->has_trait( "Noble"))		// "Thank God I didn't have to carry that."
			{
				happy += 15;		health += 0;	mana += 0;	spirit += 0;	love += 4;	hate -= 2;
			}

			if (happy < -50)
			{
				ss << "She is very distraught about the loss of the creature growing inside her.\n";
				hate += 10;
				girl->add_trait("Pessimist", false);
			}
			else if (happy < -25)
			{
				ss << "She is distraught about the loss of the creature growing inside her.\n";
				hate += 5;
			}
			else if (happy < -10)
			{
				ss << "She is sad about the loss of the creature growing inside her.\n";
			}
			else if (happy < 10)
			{
				ss << "She accepts that she is not ready to bring a strange creature into this world.\n";
			}
			else if (happy < 25)
			{
				ss << "She is glad she is not going to have to carry that strange creature inside her.\n";
			}
			else if (happy < 50)
			{
				ss << "She is happy she is not going to have to carry that strange creature inside her.\n";
			}
			else
			{
				ss << "She is overjoyed not to be forced to carry that hellspawn anymore.\n";
				hate -= 5;
				girl->add_trait("Optimist", false);
			}
		}
		// `J` now apply all the stat changes and finalize the transaction
		girl->happiness(happy);
		girl->health(health);
		girl->mana(mana);
		girl->spirit(spirit);
		girl->pclove(love);
		girl->pchate(hate);

		girl->m_ChildrenCount[CHILD09_ABORTIONS]++;
		girl->clear_pregnancy();
		girl->m_PregCooldown = cfg.pregnancy.cool_down();
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
	}
	else
	{
		ss << "The abortion is in progress (1 day remaining).";
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

	// Improve girl
	int libido = -10;
	if (girl->has_trait( "Lesbian"))		libido += numnurse;
	if (girl->has_trait( "Masochist"))	libido += 1;
	if (girl->has_trait( "Nymphomaniac"))	libido += 2;
	girl->upd_temp_stat(STAT_LIBIDO, libido);

#pragma endregion
	return false;
}

double cJobManager::JP_GetAbort(sGirl* girl, bool estimate)
{
	if (!girl->is_pregnant())			return -1000;	// X - not needed
	if (girl->carrying_players_child())	return 1;		// E - its your's
	if (girl->carrying_monster())		return 150;		// B - Beast
	return 100;											// C - customer's child
}
