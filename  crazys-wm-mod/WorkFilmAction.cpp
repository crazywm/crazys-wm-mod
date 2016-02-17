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
extern cPlayer* The_Player;

void AddStory(stringstream *);

//Nice Job - Action Scene
//This film will use her combat and magic skills, and will be more popular if she's hot.
bool cJobManager::WorkFilmAction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	//Taken care of in building flow, leaving it in for robustness
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int bonus = 0;
	double jobperformance = JP_FilmAction(girl, false);

	//g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName;
	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << gettext(" refused to shoot an action scenes today.\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 75)
	{
		ss << gettext(" was too tired to take part in an action scene.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << gettext(" worked as an actress in an action scene.\n\n");


	//JOB
	ss << girlName;
	int OPTIONS = 3; //Right now this number of options applies for all levels of job
	if (jobperformance >= 350)
	{
		ss << gettext(" made an outstanding action scene,");
		AddStory(&ss);
		ss << gettext(" It will definitely win her some fans.");
		g_Girls.UpdateStat(girl, STAT_FAME, 3);
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << gettext(" made a great action scene,");
		AddStory(&ss);
		ss << gettext(" It should win her some fans.");
		g_Girls.UpdateStat(girl, STAT_FAME, 2);
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << gettext(" made a fairly good action scene,");
		AddStory(&ss);
		ss << gettext(" Her fans will enjoy it.");
		bonus = 4;
		g_Girls.UpdateStat(girl, STAT_FAME, 1);
	}
	else if (jobperformance >= 145)
	{
		ss << gettext(" made an uninspired action scene,");
		AddStory(&ss);
		ss << gettext(" Her diehard fans might enjoy it.");
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext(" made a weak action scene,");
		AddStory(&ss);
		bonus = 1;
		ss << gettext("\nThe CamerMage advised ") << girlName << gettext(" how to improve her performance");
		if (g_Dice.percent(40))
		{
			ss << gettext(" and her scene got a little better after this.");
			bonus++;
		}
		else
		{
			ss << gettext(", but she wouldn't listen.");
		}
	}
	else
	{
		ss << gettext(" made an awful action scene,");
		AddStory(&ss);
		ss << gettext(" Even her fans will hate it.");
		g_Girls.UpdateStat(girl, STAT_FAME, -1);
	}

	ss << gettext("\n");

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << gettext("She loved performing on film today.\n\n");
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << gettext("She enjoyed this performance.\n\n");
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << gettext("She found this awkward and did not enjoy it.\n\n");
	}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMACTION, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);

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
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	
	//gain traits
	if (jobperformance >= 140 && g_Dice.percent(50))
	{
		if (g_Girls.PossiblyGainNewTrait(girl, "Brawler", 60, ACTION_COMBAT, "She has performed in enough fight scenes that she has become quite a brawler.", Day0Night1));
		else if (g_Girls.PossiblyGainNewTrait(girl, "Tough", 70, ACTION_COMBAT, "She has performed in enough fight scenes that she has become quite tough.", Day0Night1));
		else if (g_Girls.PossiblyGainNewTrait(girl, "Agile", 80, ACTION_COMBAT, "She has performed martial arts scenes that she is becoming agile.", Day0Night1));
		else (g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 90, ACTION_COMBAT, "She has performed so many action scenes that she has become fast on her feet.", Day0Night1));
	}
		
	if (jobperformance >= 140 && g_Dice.percent(50))
	{
		if (g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKMOVIE, "She has performed in enough movie scenes that she has become charismatic.", Day0Night1));
		else if (g_Girls.PossiblyGainNewTrait(girl, "Actress", 70, ACTION_WORKMOVIE, "She has performed in enough movie scenes that she has become a good actress.", Day0Night1));
		else g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks sexiness.", Day0Night1);
	}

	//Nice job bonus-------------------------------------------------------
	//BONUS - Nice jobs show off her assets and make her happy, increasing fame and love.

	int MrNiceGuy = g_Dice % 6, MrFair = g_Dice % 6;
	MrNiceGuy = (MrNiceGuy + MrFair)/3;				//Should come out around 1-2 most of the time.

	g_Girls.UpdateStat(girl, STAT_HAPPINESS, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_FAME, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCHATE, -MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, -MrNiceGuy);
	The_Player->disposition(MrNiceGuy);

	//----------------------------------------------------------------------

	return false;
}

void AddStory(stringstream * MyStr)
{
	int roll = g_Dice.d100();
	int OPTIONS = 15;
	*MyStr << gettext(" in which she ");
	switch (roll%OPTIONS)
	{
	case 0:
		*MyStr << gettext("battles a gang of demons.");
		break;
	case 1:
		*MyStr << gettext("defends a village against twisted, raping marauders.");
		break;
	case 2:
		*MyStr << gettext("avenges her mentor and defends her family's honour.");
		break;
	case 3:
		*MyStr << gettext("battles her evil step-brother for control of the Crimson Sceptre.");
		break;
	case 4:
		*MyStr << gettext("saves a twisted nunnery from the evil within.");
		break;
	case 5:
		*MyStr << gettext("opens hella whup-ass.");
		break;
	case 6:
		*MyStr << gettext("protects the Elven Princess from the Orc Prince's evil magics.");
		break;
	case 7:
		*MyStr << gettext("struggles to survive an island deathmatch sponsored by a corrupt state.");
		break;
	case 8:
		*MyStr << gettext("dies unfairly, and is forced to beat the Challenge of the Eight Divines to earn back her place among the living.");
		break;
	case 9:
		*MyStr << gettext("protects a handsome, kindly slave-master from his slaves' vicious mutiny.");
		break;
	case 10:
		*MyStr << gettext("is a bounty hunter, hunting down desperate criminals and dangerous escaped slaves.");
		break;
	case 11:
		*MyStr << gettext("battles her older sister who has been corrupted by the dark power of the Ninth Ward of Amocles.");
		break;
	case 12:
		*MyStr << gettext("is the last of a race of female warriors, taking vengeance against a dark Prince.");
		break;
	case 13:
		*MyStr << gettext("stars as a female monk defending a mountain temple from marauding Centaurs.");
		break;
	case 14:
		*MyStr << gettext("hunts down the sadistic pirate gang who kidnapped her sister.");
		break;
	default:
		*MyStr << gettext("does something very unexpected.");
		break;
	}
	return;
}


double cJobManager::JP_FilmAction(sGirl* girl, bool estimate)	// not used
{
	/*BSIN: She can work to her skills: if she's good at combat, make it combat focussed, if magic make it mage craft
	//So calc is as follows:
			total/4						(magic + combat)/4				- gives a max of 50 for combined score
		+	best_skill/2			+	(whichever is higher)/2			- max of 50 again for highest skill

		So if comb = magic = 100, then total = 100
		if comb = 100, magic = 0, then total = (100 + 0)/4 + (100/2) = 25 + 50 = 75
		if comb = 50, magic = 100, then total = (50 + 100)/4 + (100/2) = 37 + 50 = 87
		if comb = 50, magic = 50, then total = (50 + 50)/4 + (50/2) = 25 + 25 = 50

		Second factor is looks: (beau + char)/ 2
	*/
	int	CombatSkill = g_Girls.GetSkill(girl, SKILL_COMBAT);
	int	MagicSkill = g_Girls.GetSkill(girl, SKILL_MAGIC);
	int TopSkill = ((CombatSkill > MagicSkill) ? CombatSkill : MagicSkill);
	double jobperformance =
		(((g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY)) / 2)		//looks component
		+ (((CombatSkill + MagicSkill)/4) + (TopSkill/2)));										//Skills component

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//Good
	if (g_Girls.HasTrait(girl, "Actress"))					jobperformance += 60;	//Can act
	if (g_Girls.HasTrait(girl, "Charismatic"))				jobperformance += 30;	//Can act
	if (g_Girls.HasTrait(girl, "Incorporeal"))				jobperformance += 60;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Construct"))				jobperformance += 50;	//
	if (g_Girls.HasTrait(girl, "Half-Construct"))			jobperformance += 35;	//
	if (g_Girls.HasTrait(girl, "Muscular"))					jobperformance += 35;	//
	if (g_Girls.HasTrait(girl, "Strong"))					jobperformance += 35;	//
	if (g_Girls.HasTrait(girl, "Tough"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))			jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Agile"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Princess"))					jobperformance += 30;	//High-Status
	if (g_Girls.HasTrait(girl, "Queen"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Goddess"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Angel"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Heroine"))					jobperformance += 20;	//Exciting
	if (g_Girls.HasTrait(girl, "Brawler"))					jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Dominatrix"))				jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Tsundere"))					jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Yandere"))					jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Fearless"))					jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Iron Will"))				jobperformance += 10;	//
	
	//Bad
	if (g_Girls.HasTrait(girl, "No Legs"))					jobperformance -= 80;	//Difficult to make action scene
	if (g_Girls.HasTrait(girl, "One Leg"))					jobperformance -= 50;	//
	if (g_Girls.HasTrait(girl, "No Feet"))					jobperformance -= 80;	//
	if (g_Girls.HasTrait(girl, "One Foot"))					jobperformance -= 50;	//
	if (g_Girls.HasTrait(girl, "No Arms"))					jobperformance -= 50;	//
	if (g_Girls.HasTrait(girl, "One Arm"))					jobperformance -= 30;	//
	if (g_Girls.HasTrait(girl, "One Hand"))					jobperformance -= 20;	//
	if (g_Girls.HasTrait(girl, "No Hands"))					jobperformance -= 25;	//
	if (g_Girls.HasTrait(girl, "Blind"))					jobperformance -= 50;	//
	if (g_Girls.HasTrait(girl, "Fragile"))					jobperformance -= 30;	//Tends to get hurt on set
	if (g_Girls.HasTrait(girl, "Delicate"))					jobperformance -= 30;	//
	if (g_Girls.HasTrait(girl, "Clumsy"))					jobperformance -= 25;	//
	if (g_Girls.HasTrait(girl, "Dojikko"))					jobperformance -= 15;	//
	if (g_Girls.HasTrait(girl, "Muggle"))					jobperformance -= 10;	//
	if (g_Girls.HasTrait(girl, "Meek"))						jobperformance -= 5;	//
	
	return jobperformance;
}