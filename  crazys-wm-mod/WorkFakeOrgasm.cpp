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
bool cJobManager::WorkFakeOrgasm(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname;
	int actiontype = ACTION_WORKTRAINING;
	// if she was not in JOB_FAKEORGASM yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_FAKEORGASM) girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
	if (girl->m_WorkingDay < 0) girl->m_WorkingDay = 0;
	girl->m_DayJob = girl->m_NightJob = JOB_FAKEORGASM;	// it is a full time job

	if (girl->has_trait("Fake Orgasm Expert"))
	{
		ss << " is already a \"Fake Orgasm Expert\".";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return false;	// not refusing
	}

	ss << "You teach " << girlName << " how to fake her orgasms.\n \n";

	g_Girls.UnequipCombat(girl);	// not for patient

	int enjoy = 0, wages = 10, tips = 0;
	int startday = girl->m_WorkingDay;
	int libido = 0;
	int msgtype = Day0Night1, imagetype = IMGTYPE_MAST;

	// Base adjustment
	int tired = 5 + g_Dice % 11;
	girl->m_WorkingDay += 10 + g_Dice % 11;
	// Positive Stats/Skills
	girl->m_WorkingDay += girl->performance() / 5;
	girl->m_WorkingDay += girl->group() / 20;
	girl->m_WorkingDay += girl->normalsex() / 20;
	girl->m_WorkingDay += girl->lesbian() / 20;
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
	if (girl->has_trait("Quick Learner"))	girl->m_WorkingDay += g_Dice % 20;			//
	if (girl->has_trait("Actress"))			girl->m_WorkingDay += g_Dice % 20;			// Acting is just faking anyway
	if (girl->has_trait("Porn Star"))		girl->m_WorkingDay += g_Dice % 20;			// The director just wants everything to cum together
	if (girl->has_trait("Your Daughter"))	girl->m_WorkingDay += g_Dice % 20;			// Living in a brothel you pick up a few things over the years
	if (girl->has_trait("Your Wife"))		girl->m_WorkingDay += g_Dice % 10;			// Most wives fake it eventually
	if (girl->has_trait("Shape Shifter"))	girl->m_WorkingDay += g_Dice % 10;			// She is used to faking who she is
	if (girl->has_trait("Exhibitionist"))	girl->m_WorkingDay += g_Dice % 5;			// She enjoys making a scene
	if (girl->has_trait("Whore"))			girl->m_WorkingDay += g_Dice % 5;			// Doing it so many times makes faking it a little easier
	if (girl->has_trait("Slut"))			girl->m_WorkingDay += g_Dice % 5;			// Doing it so many times makes faking it a little easier
	if (girl->has_trait("Succubus"))		girl->m_WorkingDay += g_Dice % 5;			// If faking it gets them to cum faster, do it
	if (girl->has_trait("Iron Will"))		girl->m_WorkingDay += g_Dice % 3;			// She is going to finish her taks, whatever it is
	if (girl->has_trait("Audacity"))		girl->m_WorkingDay += g_Dice % 3;			// She doesn't care what it looks like or who sees it
	// Negative Traits
	if (girl->has_trait("Broken Will"))	{ girl->m_WorkingDay -= g_Dice.bell(10, 20);	ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }
	if (girl->has_trait("Mind Fucked"))		girl->m_WorkingDay -= g_Dice.bell(10, 20);	// Does she even know who is fucking her?
	if (girl->has_trait("Retarded"))		girl->m_WorkingDay -= g_Dice.bell(5, 10);	// Does she even know who is fucking her?
	if (girl->has_trait("Slow Learner"))	girl->m_WorkingDay -= g_Dice % 10;			//
	if (girl->has_trait("Fast Orgasms"))	girl->m_WorkingDay -= g_Dice % 10;			// She cums before she can fake it
	if (girl->has_trait("Futanari"))		girl->m_WorkingDay -= g_Dice % 10;			// Kind of hard to fake an orgasm with a dick
	if (girl->has_trait("Broodmother"))		girl->m_WorkingDay -= g_Dice % 10;			// faking it kind of defeats the purpose
	if (girl->has_trait("Bimbo"))			girl->m_WorkingDay -= g_Dice % 5;			// She enjoys herself to much to remember to fake it
	if (girl->has_trait("Nymphomaniac"))	girl->m_WorkingDay -= g_Dice % 5;			// She enjoys herself to much to remember to fake it
	if (girl->has_trait("Blind"))			girl->m_WorkingDay -= g_Dice % 5;			// She can't see her partner to time it right
	if (girl->has_trait("Deaf"))			girl->m_WorkingDay -= g_Dice % 5;			// She can't hear what she sounds like
	if (girl->has_trait("Botox Treatment"))	girl->m_WorkingDay -= g_Dice % 5;			// Is she faking it? can't tell by her face

	/*		traits that may help or hurt training time.
	if (girl->has_trait("Agile"))				girl->m_WorkingDay += g_Dice.bell(0,1);		//
	if (girl->has_trait("Canine"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Cat Girl"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Charismatic"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Charming"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Clumsy"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Cool Person"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Country Gal"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Cow Girl"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Cyclops"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Demon Possessed"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Demon"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Dependant"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Director"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Dominatrix"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Elegant"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Equine"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Farmer"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Farmers Daughter"))	girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Fearless"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Flexible"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Flight"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Former Official"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Furry"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Goddess"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Idiot Savant"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Idol"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("MILF"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Malformed"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Manly"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Masochist"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Meek"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Merciless"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Muscular"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Mute"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Nerd"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Nervous"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Noble"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Open Minded"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Optimist"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Pessimist"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Porn Star"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Powerful Magic"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Prehensile Tail"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Priestess"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Princess"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Psychic"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Queen"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Reptilian"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Retarded"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Sadistic"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Shape Shifter"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Shy"))					girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Singer"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Skeleton"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Slitherer"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Smoker"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Social Drinker"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Spirit Possessed"))	girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Strong Magic"))		girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Strong"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Teacher"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Tomboy"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Tone Deaf"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Tough"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Tsundere"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Twisted"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Undead"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Vampire"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Virgin"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Waitress"))			girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Wings"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Yandere"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//
	if (girl->has_trait("Zombie"))				girl->m_WorkingDay -= g_Dice.bell(0,2);		//


	*/


	if (!is_sex_type_allowed(SKILL_ANAL, brothel))			girl->m_WorkingDay -= g_Dice % 3;
	if (!is_sex_type_allowed(SKILL_BDSM, brothel))			girl->m_WorkingDay -= g_Dice % 3;
	if (!is_sex_type_allowed(SKILL_BEASTIALITY, brothel))	girl->m_WorkingDay -= g_Dice % 2;
	if (!is_sex_type_allowed(SKILL_GROUP, brothel))			girl->m_WorkingDay -= g_Dice % 10;
	if (!is_sex_type_allowed(SKILL_LESBIAN, brothel))		girl->m_WorkingDay -= g_Dice % 10;
	if (!is_sex_type_allowed(SKILL_NORMALSEX, brothel))		girl->m_WorkingDay -= g_Dice % 10 + 5;


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

		if (girl->has_trait("Slow Learner") || girl->has_trait("Broken Will") || girl->has_trait("Mind Fucked") || girl->has_trait("Retarded"))
		{
			ss << "She was not mentally able to learn";
		}
		else if (girl->has_trait("Bimbo") || girl->has_trait("Fast Orgasms") || girl->has_trait("Nymphomaniac"))
		{
			ss << "She was too focused on the sex to learn";
			tired += 5 + g_Dice % 11;
		}
		else if (girl->has_trait("Blind") || girl->has_trait("Deaf"))
		{
			ss << "Her handicap kept her from learning";
		}
		else
		{
			ss << "She resisted all attempts to teach her";
			tired += 5 + g_Dice % 11;
		}
		ss << " to fake her orgasms.";
		wages = 0;
	}
	else if (girl->m_WorkingDay >= 100 && Day0Night1)
	{
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		msgtype = EVENT_GOODNEWS;
		ss << "With her training complete, she is now a \"Fake Orgasm Expert\".";
		girl->remove_trait("Slow Orgasms");	girl->remove_trait("Fast Orgasms");	girl->add_trait("Fake Orgasm Expert");
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
		wages = 200;
	}
	else
	{
		if (girl->m_WorkingDay >= 100)		tired -= (girl->m_WorkingDay - 100) / 2;	// her last day so she rested a bit
		else	ss << "Training in progress (" << girl->m_WorkingDay << "%).\n \n";
		wages = min(100, girl->m_WorkingDay);
		/* */if (girl->m_WorkingDay < 25)	ss << "She has no idea what she sounds like durring sex but it ain't orgasmic.";
		else if (girl->m_WorkingDay < 50)	ss << "When she realizes she should finish, you can see it click in her mind and easily notice her changing things up.";
		else if (girl->m_WorkingDay < 75)	ss << "She is still not getting into rhythm with " << (g_Dice % 3 ? "you" : "her partner") << " but it still seems enjoyable.";
		else/*                         */	ss << "She is almost there but you want her to practice a little more to get it perfect.";

		if (Day0Night1 == 0)
		{
			ss << "\nYou tell her to take a break for lunch and ";
			/* */if (girl->m_WorkingDay < 50)	ss << "clear her mind, she has a lot more work to do.";
			else if (girl->m_WorkingDay < 75)	ss << "relax, she has a bit more training to do.";
			else/*                         */	ss << "see if she can make anyone say \"I'll have what she's having\".";
		}
		else
		{
			ss << "\nThats all for tonight, ";
			/* */if (girl->m_WorkingDay < 50)	ss << "we have a lot more to do tomorrow (and probably the next few weeks).";
			else /*                        */	ss << "we'll pick things up in the morning.";
		}
	}

#pragma endregion
#pragma region	//	Finish the shift			//


	if (girl->is_slave()) wages /= 2;
	girl->m_Pay = wages;

	// Improve girl
	int I_performance = (g_Dice.bell(3, 15));
	int I_confidence = (g_Dice.bell(-1, 5));
	int I_constitution = (max(0, g_Dice.bell(-2, 1)));
	int I_spirit = (g_Dice.bell(-5, 5));
	int I_lesbian = (g_Dice.bell(0, 5));
	int I_normalsex = (g_Dice.bell(0, 5));
	int I_group = (g_Dice.bell(0, 5));
	int I_anal = (max(0, g_Dice.bell(-2, 2)));

	girl->exp(xp);
	girl->tiredness(tired);
	girl->lesbian(I_lesbian);
	girl->normalsex(I_normalsex);
	girl->group(I_group);
	girl->anal(I_anal);
	girl->performance(I_performance);
	girl->confidence(I_confidence);
	girl->constitution(I_constitution);
	girl->spirit(I_spirit);


	libido += girl->has_trait("Nymphomaniac") ? g_Dice.bell(3, 10) : g_Dice.bell(1, 5);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);

	ss << "\n \nNumbers:"
		<< "\n Wages = " << (int)wages
		<< "\n Xp = " << xp
		<< "\n Performance = " << I_performance
		<< "\n Confidence = " << I_confidence
		<< "\n Spirit = " << I_spirit
		<< "\n Constitution = " << I_constitution
		<< "\n Libido = " << libido
		<< "\n Lesbian = " << I_lesbian
		<< "\n Normal Sex = " << I_normalsex
		<< "\n Group = " << I_group
		<< "\n Anal = " << I_anal
		<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy;

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}

double cJobManager::JP_FakeOrgasm(sGirl* girl, bool estimate)// not used
{
	if (girl->has_trait( "Fake Orgasm Expert"))	return -1000;
	if (girl->has_trait( "Slow Orgasms"))			return 500;
	if (girl->has_trait( "Fast Orgasms"))			return 500;
	return 250;
}
