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
#include "cJobManager.h"
#include "cBrothel.h"
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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

static cDungeon* m_Dungeon = g_Brothels.GetDungeon();

// `J` House Job - General
bool cJobManager::WorkRecruiter(sGirl* girl, sBrothel* brothel, int Day0Night1, string& summary)
{
	if (Day0Night1 == SHIFT_NIGHT) return false;
	cTariff tariff;
	string message = "";

	if(Preprocessing(ACTION_WORKRECRUIT, girl, brothel, Day0Night1, summary, message))		return true;


	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int jobperformance = (HateLove + g_Girls.GetStat(girl, STAT_CHARISMA));
	int wages = 100, work = 0;
	int roll = g_Dice % 100;

	message += "She worked trying to recruit girls for you.";

	/* */if (HateLove < -80)	message += " She hates you more then anything so she doesn't try that hard.\n\n";
	else if (HateLove < -60)	message += " She hates you.\n\n";
	else if (HateLove < -40)	message += " She doesn't like you.\n\n";
	else if (HateLove < -20)	message += " She finds you to be annoying.\n\n";
	else if (HateLove <   0)	message += " She finds you to be annoying.\n\n";
	else if (HateLove <  20)	message += " She finds you to be decent.\n\n";
	else if (HateLove <  40)	message += " She finds you to be a good person.\n\n";
	else if (HateLove <  60)	message += " She finds you to be a good person.\n\n";
	else if (HateLove <  80)	message += " She has really strong feelings for you so she trys really hard for you.\n\n";
	else						message += " She loves you more then anything so she gives it her all.\n\n";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Cool Person"))  jobperformance += 10; //people love to be around her
	if (g_Girls.HasTrait(girl, "Charming"))		jobperformance += 10; //people like charming people	
	if (g_Girls.HasTrait(girl, "Psychic"))		jobperformance += 20; //knows what people want to hear
		

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))	jobperformance -= 50;


	int findchance = 0;

	if (jobperformance >= 245)
	{
		message += "She must be the perfect recruiter.\n\n";
		findchance = 20;
	}
	else if (jobperformance  >= 185)
	{
		message += "She's unbelievable at this.\n\n";
		findchance = 15;
	}
	else if (jobperformance >= 135)
	{
		message += "She's good at this job.\n\n";
		findchance = 12;
	}
	else if (jobperformance >= 85)
	{
		message += "She made a few mistakes but overall she is okay at this.\n\n";
		findchance = 10;
	}
	else if (jobperformance >= 65)
	{
		message += "She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		findchance = 8;
	}
	else
	{
		message += "She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		findchance = 4;
	}
	// `J` add in player's disposition so if the girl has heard of you
	cPlayer m_Player;
	int findroll = (g_Dice % 101);
	int dispmod = 0;
	     if (m_Player.disposition() >= 100)	dispmod = 3;	// "Saint"
	else if (m_Player.disposition() >= 80)	dispmod = 2;	// "Benevolent"
	else if (m_Player.disposition() >= 50)	dispmod = 1;	// "Nice"
	else if (m_Player.disposition() >= 10)	dispmod = 0;	// "Pleasant"
	else if (m_Player.disposition() >= -10)	dispmod = 0;	// "Neutral"
	else if (m_Player.disposition() >= -50)	dispmod = -1;	// "Not nice"
	else if (m_Player.disposition() >= -80)	dispmod = -2;	// "Mean"
	else /*								*/	dispmod = -3;	// "Evil"

	if (findroll < findchance + 10)	// `J` While out recruiting she does find someone...
	{
		int finddif = findroll - findchance;
		sGirl* girl = g_Girls.GetRandomGirl(false, (dispmod == -3 && g_Dice%4!=0));
		if (girl)
		{
			bool add = false;
			message += "She finds a girl, ";
			message += girl->m_Name;
			if (findroll < findchance - 5)
			{		// `J` ... and your disposition did not come up.
				add = true;
				message += " and convinces her that she should work for you.";
			}
			else if (findroll < findchance + 5)	// `J` ... and your disposition did come up...
			{
				if (findroll < findchance + dispmod)	// `J` ... and she was recruited
				{
					add = true;
					if (dispmod > 0)
					{
						message += "\nYour nice reputation has helped you today as she agrees to come work for you.";
					}
					else if (dispmod < 0)
					{
						message += "\nScared of what you might to do to her if you don't come work for her she agrees to taking the job.";
					}
					else
					{
						message += "\nThe fact that your neither good nor evil seems to have helped you today.  As the girl doesn't think your evil nor a \"pussy\" and comes to work for you.";
					}
					if (dispmod == 3)
					{
						int rollt(g_Dice % 4);
						if (rollt == 0)	girl->add_trait("Optimist");
					}
					if (dispmod == -3)
					{
						int rollt(g_Dice % 4);
						if (rollt == 0)	girl->add_trait("Demon");
						if (rollt == 1)	girl->add_trait("Fearless");
					}
				}
				else	// `J` ... and she was not recruited
				{

					if (dispmod > 0)
					{
						message += "\nBeing so nice she is worried you will get yourself killed and someone else will take over that she won't like.  So she won't come work for you.";
					}
					else if (dispmod < 0)
					{
						message += "\nShe fears you to much to come and work for you.";
					}
					else
					{
						message += "\nAs you are on the fence with your reputation she doesn't know what to think about you and won't work for you.";
					}
				}
			}
			else	// `J` ... She was not recruited.
			{
				message += gettext(" but was unable to convince her that she should work for you.");
			}
			if (add)
			{
				girl->m_Stats[STAT_HOUSE] = 60;

				m_Dungeon->AddGirl(girl, DUNGEON_NEWGIRL);
			}
		}
		else
		{
			message += "But was unable to find anyone to join.";
		}
	}
	else
	{
		message += "But was unable to find anyone to join.";
	}



	//enjoyed the work or not
	if (roll <= 5)
	{ message += "\nSome of the people abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ message += "\nShe had a pleasant time working."; work += 3; }
	else
	{ message += "\nOtherwise, the shift passed uneventfully."; work += 1; }

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKRECRUIT, work, true);
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, Day0Night1);
	int roll_max = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;
	

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2)	g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
	else			g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	if (g_Dice % 2)	g_Girls.UpdateStat(girl, STAT_CHARISMA, skill);
	else			g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKRECRUIT, "Dealing with people all day has made " + girl->m_Realname + " more Charismatic.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 80, ACTION_WORKRECRUIT, girl->m_Realname + " has been doing this for so long it's as if she can read minds now.", Day0Night1 == SHIFT_NIGHT);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKRECRUIT, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);
	return false;
	}
