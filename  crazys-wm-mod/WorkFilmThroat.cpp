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
extern sGang g_Gang;
extern cPlayer* The_Player;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmThroat(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	int wages = 50, tips = 0;
	int enjoy = 0, hate = 0, tired = 0, bonus = 0;
	double jobperformance = JP_FilmThroat(girl, false);

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	int roll = g_Dice.d100();

	if (g_Girls.GetStat(girl, STAT_HEALTH) < 20)
	{
		ss << gettext("The crew refused to film a throatjob scene because ") << girlName << gettext(" is not healthy enough.\n\"We are NOT filming snuff\".");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (g_Girls.HasTrait(girl, "Mind Fucked"))
	{
		ss << gettext("Mind fucked ") << girlName << gettext(" was pleased to be paraded naked in public and tortured and degraded. It is natural.\n");
		bonus += 10;
	}
	else if (g_Girls.HasTrait(girl, "Masochist"))
	{
		ss << gettext("Masochist ") << girlName << gettext(" was pleased to be tortured, used and degraded. It is her place.\n");
		bonus += 6;
	}
	else if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << girlName << gettext(" angrily refused to be throat-fucked on film today.");
		if (girl->is_slave())
		{
			if (The_Player->disposition() > 30)  // nice
			{
				ss << gettext("\nThough she is a slave, she was upset so you allowed her the day off.\n");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, 2);
				g_Girls.UpdateStat(girl, STAT_SPIRIT, 1);
				g_Girls.UpdateStat(girl, STAT_PCHATE, -1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (The_Player->disposition() > -30) //pragmatic
			{
				ss << gettext(" Amused, you over-ruled her, and gave owner's consent for her. She glared at you as they dragged her away.\n");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
				g_Girls.UpdateStat(girl, STAT_PCHATE, 2);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, 2);
				The_Player->disposition(-1);
				enjoy -= 2;
			}
			else if (The_Player->disposition() > -30)
			{
				ss << gettext(" Amused, you over-ruled her, and gave owner's consent.\nShe made a hell of a fuss, but you knew just the thing to shut her up.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -4);
				g_Girls.UpdateStat(girl, STAT_PCHATE, +5);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, +5);
				The_Player->disposition(-2);
				enjoy -= 6;
			}
		}
		else // not a slave
		{
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << " was filmed in facefucking scenes.\n\n";

	
	int OPTION = g_Dice % 3;
	int OUTCOME = g_Dice % 3;
	if (jobperformance >= 350)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << gettext(" knelt in front of him, opened wide, and swallowed down his whole length, bouncing back and forward until he came ");
			break;
		case 1:
			ss << girlName << gettext(" lay on her back on the bed with her head hanging back over the edge. Gripping her breasts, he rammed his cock down her throat until cum exploded ");
			break;
		case 2:
			ss << gettext("The actor sat on a chair, with ") << girlName << gettext(" knelt before him. Gripping her head, he pulled her onto his cock, rocking her back and forward on his cock until he came ");
			break;
		default:
			ss << girlName << gettext(" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << gettext("deep down her throat");
			break;
		case 1:
			ss << gettext("in her mouth");
			break;
		case 2:
			ss << gettext("deep in her stomach");
			break;
		default:
			ss << gettext("somewhere");
			break;
		}
		ss << gettext(".\n");
		bonus = 12;
		hate = 1;
		tired = 1;
	}
	else if (jobperformance >= 245)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << gettext(" knelt in front of him and sucked his cock down, bouncing back and forward until he came ");
			break;
		case 1:
			ss << girlName << gettext(" lay back on the bed with her head over the edge. He fucked her throat until he came ");
			break;
		case 2:
			ss << gettext("The actor sat on a chair, with ") << girlName << gettext(" before him. Gripping her head, he pulled her onto his cock, back and forth until he came ");
			break;
		default:
			ss << girlName << gettext(" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << gettext("all over her");
			break;
		case 1:
			ss << gettext("in her mouth");
			break;
		case 2:
			ss << gettext("deep down her throat");
			break;
		default:
			ss << gettext("somewhere");
			break;
		}
		hate = 2;
		tired = 4;
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << gettext(" knelt down and let him facefuck her until he came ");
			break;
		case 1:
			ss << girlName << gettext(" lay on the bed and let him fuck her throat. He carefully facefucked her until he came ");
			break;
		case 2:
			ss << gettext("The actor pulled ") << girlName << gettext("'s head onto his cock, rubbing his cock down her throat until he came ");
			break;
		default:
			ss << girlName << gettext(" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << gettext("all over her");
			break;
		case 1:
			ss << gettext("in her mouth");
			break;
		case 2:
			ss << gettext("down her throat");
			break;
		default:
			ss << gettext("somewhere");
			break;
		}
		bonus = 4;
		hate = 3;
		tired = 8;
	}
	else if (jobperformance >= 145)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << gettext(" knelt down and get his cock down her throat. Eventually he came ");
			break;
		case 1:
			ss << girlName << gettext(" lay on the bed and tried to take his meat in her throat. He facefucked her until he finally came ");
			break;
		case 2:
			ss << gettext("The actor pulled ") << girlName << gettext("'s head onto his cock, making her gag, until he came ");
			break;
		default:
			ss << girlName << gettext(" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << gettext("in her face");
			break;
		case 1:
			ss << gettext("in her mouth");
			break;
		case 2:
			ss << gettext("over her face");
			break;
		default:
			ss << gettext("somewhere");
			break;
		}
		bonus = 2;
		hate = 5;
		tired = 10;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext("It was a pretty awkward scene, with the actor not getting much pleasure trying to fuck her throat. ");
		ss << gettext("In the end he gave up and plunged his cock down her throat, making her throw up through her nose, as he came in her head.");
		bonus = 1;
		hate = 8;
		tired = 12;
	}
	else
	{
		ss << gettext("With her continual gagging, retching and vomittig the actor couldn't get any pleasure, and ended up wanking over her face.");
		hate = 8;
		tired = 12;
	}
	ss << gettext("\n");

	//Enjoyed?! No, but some will hate it less.
	if (jobperformance >= 200)
	{
		enjoy++;
		ss << gettext("She won't say it, but you suspect she secretly gets off on the degradation.\n\n");
	}
	else if (jobperformance >= 100)
	{
		enjoy -= (1 + g_Dice % 2);
		ss << gettext("She's pretty raw and her makeup is everywhere.\n\n");
	}
	else
	{
		enjoy -= (2 + g_Dice % 3);
		ss << gettext("From the way she's coughing and gagging and has bright red eyes, you suspect this wasn't much fun for her.\n\n");
	}
	bonus = bonus + enjoy;

	int impact = g_Dice%10;
	if (g_Girls.HasTrait(girl, "Strong Gag Reflex"))
	{
		ss << gettext("She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterward.\n\n");
		g_Girls.UpdateStat(girl, STAT_HEALTH, (10 + impact));
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, (10 + impact + tired));
		g_Girls.UpdateStat(girl, STAT_PCHATE, (2 + hate));
		g_Girls.UpdateStat(girl, STAT_PCFEAR, (2 + hate));
	}
	else if (g_Girls.HasTrait(girl, "Gag Reflex"))
	{
		ss << gettext("She gagged and retched a lot. It was exhausting and left her feeling sick.\n\n");
		g_Girls.UpdateStat(girl, STAT_HEALTH, (2 + impact));
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, (5 + impact + tired));
		g_Girls.UpdateStat(girl, STAT_PCHATE, (hate));
		g_Girls.UpdateStat(girl, STAT_PCFEAR, (hate));
	}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMORAL, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }


	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_ORALSEX, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Masochist", 75, ACTION_SEX, girlName + " has turned into a Masochist from filming so many BDSM scenes.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Mind Fucked", 90, ACTION_WORKMOVIE, "She has been abused so much she is now completely Mind Fucked.", Day0Night1);
	if (jobperformance > 200) g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (g_Dice.percent(5) && (g_Girls.GetStat(girl, STAT_HAPPINESS) > 80) && (g_Girls.GetEnjoyment(girl, ACTION_WORKMOVIE) > 75))
		g_Girls.AdjustTraitGroupGagReflex(girl, 1, true, Day0Night1);

	//lose
	if (g_Dice.percent(5)) g_Girls.PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, "Somewhere between having a dick in her throat, balls slapping her eyes and a camera watching her retch, " + girlName + " has lost her iron will.", Day0Night1);
	
	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = g_Dice % 8, MrNasty = g_Dice % 8;
	MrEvil = (MrEvil + MrNasty) / 2;				//Should come out around 3 most of the time.

	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_SPIRIT, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_DIGNITY, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCHATE, MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, MrEvil);
	The_Player->disposition(-MrEvil);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmThroat(sGirl* girl, bool estimate)
{
	double jobperformance =
		(((g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY)) / 2)
		+ g_Girls.GetSkill(girl, SKILL_ORALSEX));
	
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
	
	//Good
	if (g_Girls.HasTrait(girl, "Deep Throat"))					jobperformance += 80;	//better ability
	if (g_Girls.HasTrait(girl, "Mind Fucked"))					jobperformance += 60;	//eager to please
	if (g_Girls.HasTrait(girl, "No Gag Reflex"))				jobperformance += 50;	//
	if (g_Girls.HasTrait(girl, "Masochist"))					jobperformance += 50;	//
	if (g_Girls.HasTrait(girl, "Cum Addict"))					jobperformance += 40;	//
	if (g_Girls.HasTrait(girl, "No Teeth"))						jobperformance += 25;	//Lower resistance
	if (g_Girls.HasTrait(girl, "Missing Teeth"))				jobperformance += 25;	//
	if (g_Girls.HasTrait(girl, "Good Kisser"))					jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Dick-Sucking Lips"))			jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Demon Possessed"))				jobperformance += 20;	//Wild		
	if (g_Girls.HasTrait(girl, "Nimble Tongue"))				jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Pierced Tongue"))				jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Exotic"))						jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Whore"))						jobperformance += 20;	//Knows how to work it
	if (g_Girls.HasTrait(girl, "Porn Star"))					jobperformance += 30;	//Knows how to work it on film
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))					jobperformance += 25;	//had lots of practice
	if (g_Girls.HasTrait(girl, "Slut"))							jobperformance += 15;	//had practice
	if (g_Girls.HasTrait(girl, "Sexy Air"))						jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Open Minded"))					jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Natural Pheromones"))			jobperformance += 5;


	//Bad
	if (g_Girls.HasTrait(girl, "Strong Gag Reflex"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Gag Reflex"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Clumsy"))						jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Nervous"))						jobperformance -= 15;

	return jobperformance;
}