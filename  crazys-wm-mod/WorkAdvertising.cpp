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
#include "cCustomers.h"
#include "cRng.h"
#include "sConfig.h"
#include "CLog.h"
#include "cTariff.h"
#include "cGold.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

#pragma endregion

// `J` Job Brothel - General
bool cJobManager::WorkAdvertising(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKADVERTISING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to advertise the brothel today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " is assigned to advertize the brothel.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away

	int wages = 20, tips = 0;
	int enjoy = 0, fame = 0;
	int imagetype = IMGTYPE_SIGN;
	int msgtype = EVENT_SUMMARY;

#pragma endregion
#pragma region //	Job Performance			//

	// How much will she help stretch your advertising budget? Let's find out
	double multiplier = JP_Advertising(girl, false);

	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
		multiplier *= 0.9;	// unpaid slaves don't seem to want to advertise as much.
	if (girl->is_free())
		multiplier *= 1.1;	// paid free girls seem to attract more business

	// add some more randomness
#if 0 // work in progress

	ss << "She gave a tour of building making sure not to show the rooms that are messy.\n";
	if ()
	{
		ss << "as she was walking the people around she would make sure to slip in a few recommendations for girls the people would enjoy getting services from.";
	}
	else
	{
		ss << "She spent more time trying to flirt with the customers then actually getting anywhere with showing them around,\n";
		ss << "She still got the job done but it was nowhere as good as it could have been";
	}
	if ()
	{
		ss << "Most of the time she spent reading or doing anything else to spend her time to pass the time.\n";
	}
	else
	{
		ss << "She decided not to get in trouble so she just waited there silently for someone to come so she could do her job properly.\n";
	}

#endif

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	// Complications
	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She was harassed and made fun of while advertising.\n";
		if (girl->happiness() < 50)
		{
			enjoy -= 1;
			ss << "Other then that she mostly just spent her time trying to not breakdown and cry.\n";
			fame -= g_Dice % 2;
		}
		multiplier *= 0.8;
		fame -= g_Dice % 2;
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She made sure many people were interested in the buildings facilities.\n";
		multiplier *= 1.1;
		fame += g_Dice % 3;
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "She had an uneventful day advertising.\n";
	}

	/* `J` If she is not happy at her job she may ask you to change her job.
	*	Submitted by MuteDay as a subcode of bad complications but I liked it and made it as a separate section
	*	I will probably make it its own function when it works better.
	*/
	if (girl->m_Enjoyment[actiontype] < -10) 						// if she does not like the job
	{
		int enjoyamount = girl->m_Enjoyment[actiontype];
		int saysomething = g_Dice%girl->confidence() - enjoyamount;	// the more she does not like the job the more likely she is to say something about it
		saysomething -= girl->pcfear() / (girl->is_free() ? 2 : 1);	// reduce by fear (half if free)

		if (saysomething > 50)
		{
			stringstream warning;
			warning << girlName << " comes up to you and asks you to change her job, She does not like advertizing.\n";
			girl->m_Events.AddMessage(warning.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		}
		else if (saysomething > 25)
		{
			ss << "She looks at you like she has something to say but then turns around and walks away.\n";
		}
	}

#pragma endregion
#pragma region	//	Money					//

	ss << "She managed to stretch the effectiveness of your advertising budget by about " << int(multiplier) << "%.";
	// if you pay slave girls out of pocket  or if she is a free girl  pay them
	if ((girl->is_slave() && cfg.initial.slave_pay_outofpocket()) || girl->is_free())
	{
		wages += 70;
		g_Gold.advertising_costs(70);
		ss << " You paid her 70 gold for her advertising efforts.";
	}
	else
	{
		ss << " You do not pay your slave for her advertising efforts.";
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// now to boost the brothel's advertising level accordingly
	brothel->m_AdvertisingLevel += (multiplier / 100);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	int I_xp = (g_Dice % xp) + 1;							girl->exp(I_xp);
	int I_libido = (g_Dice % libido) + 1;					girl->upd_temp_stat(STAT_LIBIDO, I_libido,false);//g_Girls.UpdateStatTemp(girl,STAT_LIBIDO,I_libido);

	// primary improvement (+2 for single or +1 for multiple)
	int I_performance	= (g_Dice % skill) + 1;				girl->performance(I_performance);
	int I_charisma		= (g_Dice % skill) + 1;				girl->charisma(I_charisma);

	// secondary improvement (-1 for one then -2 for others)
	int I_service		= max(0, (g_Dice % skill) - 1);		girl->service(I_service);
	int I_confidence	= max(0, (g_Dice % skill) - 2);		girl->confidence(I_confidence);
	int I_fame			= fame;								girl->fame(I_fame);

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	if (girl->strip() > 50)
		g_Girls.PossiblyGainNewTrait(girl, "Exhibitionist", 50, actiontype, girlName +" has become quite the Exhibitionist, she seems to prefer Advertising topless whenever she can.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 70, actiontype, "Advertising on a daily basis has made " + girl->m_Realname + " more Charismatic.", Day0Night1 == SHIFT_NIGHT);
	// Lose Traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 40, actiontype, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

	if (cfg.debug.log_show_numbers())
	{
		ss << "\n \nNumbers:"
			<< "\n Ad Multiplier = " << multiplier
			<< "\n Wages = " << (int)wages
			<< "\n Tips = " << (int)tips
			<< "\n Xp = " << I_xp
			<< "\n Libido = " << I_libido
			<< "\n Fame = " << I_fame
			<< "\n Performance = " << I_performance
			<< "\n Charisma = " << I_charisma
			<< "\n Service = " << I_service
			<< "\n Confidence = " << I_confidence
			<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy
			;
	}

#pragma endregion
	return false;
}

double cJobManager::JP_Advertising(sGirl* girl, bool estimate)
{
	double cval = 0.0;
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{
		jobperformance =
			((girl->fame() + girl->charisma() + girl->performance()) / 3) +
			((girl->beauty() + girl->confidence() + girl->service() + girl->intelligence()) / 4);
	}
	else			// for the actual check
	{
		cval = girl->performance();	// `J` added
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of performance skill to jobperformance
		}
		cval = girl->performance();
		if (cval > 0)	// `J` halved jobperformance to include performace without excessive change
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of service skill to jobperformance
		}
		cval = girl->charisma();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of charisma to jobperformance
		}
		cval = girl->beauty();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 10);  // add 10% of beauty to jobperformance
		}
		cval = girl->intelligence();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of intelligence to jobperformance
		}
		cval = girl->confidence();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 10);  // add 10% of confidence to jobperformance
		}
		cval = girl->fame();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 10);  // add 10% of fame to jobperformance
		}

		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
	// positiv traits
	if (girl->has_trait("Actress"))					jobperformance += 10;	//
	if (girl->has_trait("Charismatic"))				jobperformance += 10;	//
	if (girl->has_trait("Charming"))				jobperformance += 10;	//
	if (girl->has_trait("Cool Person"))				jobperformance += 10;	//
	if (girl->has_trait("Cute"))					jobperformance += 5;	//
	if (girl->has_trait("Director"))				jobperformance += 10;	//
	if (girl->has_trait("Dominatrix"))				jobperformance += 10;	//
	if (girl->has_trait("Elegant"))					jobperformance += 5;	//
	if (girl->has_trait("Exhibitionist"))			jobperformance += 15;	// Advertising topless
	if (girl->has_trait("Fake Orgasm Expert"))		jobperformance += 10;	//
	if (girl->has_trait("Fearless"))				jobperformance += 5;	//
	if (girl->has_trait("Flexible"))				jobperformance += 5;	//
	if (girl->has_trait("Former Official"))			jobperformance += 10;	//
	if (girl->has_trait("Idol"))					jobperformance += 20;	//
	if (girl->has_trait("Iron Will"))				jobperformance += 5;	//
	if (girl->has_trait("Natural Pheromones"))		jobperformance += 10;	//
	if (girl->has_trait("Open Minded"))				jobperformance += 5;	//
	if (girl->has_trait("Optimist"))				jobperformance += 5;	//
	if (girl->has_trait("Playful Tail"))			jobperformance += 5;	//
	if (girl->has_trait("Porn Star"))				jobperformance += 20;	//
	if (girl->has_trait("Powerful Magic"))			jobperformance += 20;	//
	if (girl->has_trait("Prehensile Tail"))			jobperformance += 5;	//
	if (girl->has_trait("Priestess"))				jobperformance += 10;	// used to preaching to the masses
	if (girl->has_trait("Princess"))				jobperformance += 5;	//
	if (girl->has_trait("Psychic"))					jobperformance += 10;	//
	if (girl->has_trait("Queen"))					jobperformance += 10;	//
	if (girl->has_trait("Sexy Air"))				jobperformance += 10;	//
	if (girl->has_trait("Shape Shifter"))			jobperformance += 20;	// she can show who is available
	if (girl->has_trait("Singer"))					jobperformance += 10;	//
	if (girl->has_trait("Slut"))					jobperformance += 10;	//
	if (girl->has_trait("Strong Magic"))			jobperformance += 10;	//
	if (girl->has_trait("Strong"))					jobperformance += 5;	//
	if (girl->has_trait("Whore"))					jobperformance += 10;	//
	if (girl->has_trait("Your Daughter"))			jobperformance += 20;	//
	if (girl->has_trait("Your Wife"))				jobperformance += 20;	//

	// negativ traits
	if (girl->has_trait("Aggressive"))				jobperformance -= 5;	//
	if (girl->has_trait("Blind"))					jobperformance -= 10;	//
	if (girl->has_trait("Broken Will"))				jobperformance -= 20;	//
	if (girl->has_trait("Clumsy"))					jobperformance -= 5;	//
	if (girl->has_trait("Deaf"))					jobperformance -= 10;	//
	if (girl->has_trait("Dependant"))				jobperformance -= 10;	//
	if (girl->has_trait("Emprisoned Customer"))		jobperformance -= 30;	// she may be warning the other customers
	if (girl->has_trait("Horrific Scars"))			jobperformance -= 10;	//
	if (girl->has_trait("Kidnapped"))				jobperformance -= 40;	// she may try to run away or get help
	if (girl->has_trait("Malformed"))				jobperformance -= 20;	//
	if (girl->has_trait("Meek"))					jobperformance -= 20;	//
	if (girl->has_trait("Mind Fucked"))				jobperformance -= 50;	//
	if (girl->has_trait("Mute"))					jobperformance -= 10;	//
	if (girl->has_trait("Nervous"))					jobperformance -= 5;	//
	if (girl->has_trait("No Arms"))					jobperformance -= 30;	//
	if (girl->has_trait("No Hands"))				jobperformance -= 20;	//
	if (girl->has_trait("No Legs"))					jobperformance -= 30;	//
	if (girl->has_trait("Nymphomaniac"))			jobperformance -= 5;	// free samples?
	if (girl->has_trait("One Arm"))					jobperformance -= 10;	//
	if (girl->has_trait("One Hand"))				jobperformance -= 5;	//
	if (girl->has_trait("One Leg"))					jobperformance -= 10;	//
	if (girl->has_trait("Pessimist"))				jobperformance -= 5;	//
	if (girl->has_trait("Retarded"))				jobperformance -= 20;	//
	if (girl->has_trait("Shy"))						jobperformance -= 10;	//
	if (girl->has_trait("Skeleton"))				jobperformance -= 50;	//
	if (girl->has_trait("Zombie"))					jobperformance -= 50;	//

	return jobperformance;
}
