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

// Job Movie Studio - // nice Job - Action Scene
//This film will use her combat and magic skills, and will be more popular if she's hot.
bool cJobManager::WorkFilmAction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;	stringstream ss;	string girlName = girl->m_Realname;
	//Taken care of in building flow, leaving it in for robustness
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		ss << "There was no crew to film the scene, so " << girlName << " took the day off";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	int wages = 50, tips = 0, enjoy = 0, bonus = 0;
	double jobperformance = JP_FilmAction(girl, false);

	//g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName;
	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << " refused to shoot an action scenes today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->tiredness() > 75)
	{
		ss << " was too tired to take part in an action scene.\n \n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << " worked as an actress in an action scene.\n \n";


	//JOB
	ss << girlName;
	int OPTIONS = 3; //Right now this number of options applies for all levels of job
	if (jobperformance >= 350)
	{
		ss << " made an outstanding action scene,";
		AddStory(&ss);
		ss << " It will definitely win her some fans.";
		girl->fame(3);
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << " made a great action scene,";
		AddStory(&ss);
		ss << " It should win her some fans.";
		girl->fame(2);
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << " made a fairly good action scene,";
		AddStory(&ss);
		ss << " Her fans will enjoy it.";
		bonus = 4;
		girl->fame(1);
	}
	else if (jobperformance >= 145)
	{
		ss << " made an uninspired action scene,";
		AddStory(&ss);
		ss << " Her diehard fans might enjoy it.";
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << " made a weak action scene,";
		AddStory(&ss);
		bonus = 1;
		ss << "\nThe CamerMage advised " << girlName << " how to improve her performance";
		if (g_Dice.percent(40))
		{
			ss << " and her scene got a little better after this.";
			bonus++;
		}
		else
		{
			ss << ", but she wouldn't listen.";
		}
	}
	else
	{
		ss << " made an awful action scene,";
		AddStory(&ss);
		ss << " Even her fans will hate it.";
		girl->fame(-1);
	}

	ss << "\n";

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << "She loved performing on film today.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << "She enjoyed this performance.\n \n";
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << "She found this awkward and did not enjoy it.\n \n";
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
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, skill = 3, libido = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->combat(g_Dice%skill + 1);
	girl->magic(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(ACTION_COMBAT, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);

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

	// nice job bonus-------------------------------------------------------
	//BONUS - Nice jobs show off her assets and make her happy, increasing fame and love.

	int MrNiceGuy = g_Dice % 6, MrFair = g_Dice % 6;
	MrNiceGuy = (MrNiceGuy + MrFair)/3;				//Should come out around 1-2 most of the time.

	girl->happiness(MrNiceGuy);
	girl->fame(MrNiceGuy);
	girl->pclove(MrNiceGuy);
	girl->pchate(-MrNiceGuy);
	girl->pcfear(-MrNiceGuy);
	The_Player->disposition(MrNiceGuy);

	//----------------------------------------------------------------------

	return false;
}

void AddStory(stringstream * MyStr)
{
	int roll = g_Dice.d100();
	int OPTIONS = 15;
	*MyStr << " in which she ";
	switch (roll%OPTIONS)
	{
	case 0:
		*MyStr << "battles a gang of demons.";
		break;
	case 1:
		*MyStr << "defends a village against twisted, raping marauders.";
		break;
	case 2:
		*MyStr << "avenges her mentor and defends her family's honour.";
		break;
	case 3:
		*MyStr << "battles her evil step-brother for control of the Crimson Sceptre.";
		break;
	case 4:
		*MyStr << "saves a twisted nunnery from the evil within.";
		break;
	case 5:
		*MyStr << "opens hella whup-ass.";
		break;
	case 6:
		*MyStr << "protects the Elven Princess from the Orc Prince's evil magics.";
		break;
	case 7:
		*MyStr << "struggles to survive an island deathmatch sponsored by a corrupt state.";
		break;
	case 8:
		*MyStr << "dies unfairly, and is forced to beat the Challenge of the Eight Divines to earn back her place among the living.";
		break;
	case 9:
		*MyStr << "protects a handsome, kindly slave-master from his slaves' vicious mutiny.";
		break;
	case 10:
		*MyStr << "is a bounty hunter, hunting down desperate criminals and dangerous escaped slaves.";
		break;
	case 11:
		*MyStr << "battles her older sister who has been corrupted by the dark power of the Ninth Ward of Amocles.";
		break;
	case 12:
		*MyStr << "is the last of a race of female warriors, taking vengeance against a dark Prince.";
		break;
	case 13:
		*MyStr << "stars as a female monk defending a mountain temple from marauding Centaurs.";
		break;
	case 14:
		*MyStr << "hunts down the sadistic pirate gang who kidnapped her sister.";
		break;
	default:
		*MyStr << "does something very unexpected.";
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
	int	CombatSkill = girl->combat();
	int	MagicSkill = girl->magic();
	int TopSkill = ((CombatSkill > MagicSkill) ? CombatSkill : MagicSkill);
	double jobperformance =
		(((girl->charisma() + girl->beauty()) / 2)		//looks component
		+ (((CombatSkill + MagicSkill)/4) + (TopSkill/2)));										//Skills component

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//Good
	if (girl->has_trait( "Actress"))					jobperformance += 60;	//Can act
	if (girl->has_trait( "Charismatic"))				jobperformance += 30;	//Can act
	if (girl->has_trait( "Incorporeal"))				jobperformance += 60;	//Can't be hurt
	if (girl->has_trait( "Construct"))				jobperformance += 50;	//
	if (girl->has_trait( "Half-Construct"))			jobperformance += 35;	//
	if (girl->has_trait( "Muscular"))					jobperformance += 35;	//
	if (girl->has_trait( "Strong"))					jobperformance += 35;	//
	if (girl->has_trait( "Tough"))					jobperformance += 30;	//
	if (girl->has_trait( "Fleet of Foot"))			jobperformance += 30;	//
	if (girl->has_trait( "Agile"))					jobperformance += 30;	//
	if (girl->has_trait( "Princess"))					jobperformance += 30;	//High-Status
	if (girl->has_trait( "Queen"))					jobperformance += 30;	//
	if (girl->has_trait( "Goddess"))					jobperformance += 30;	//
	if (girl->has_trait( "Angel"))					jobperformance += 30;	//
	if (girl->has_trait( "Heroine"))					jobperformance += 20;	//Exciting
	if (girl->has_trait( "Brawler"))					jobperformance += 20;	//
	if (girl->has_trait( "Dominatrix"))				jobperformance += 20;	//
	if (girl->has_trait( "Tsundere"))					jobperformance += 15;	//
	if (girl->has_trait( "Yandere"))					jobperformance += 15;	//
	if (girl->has_trait( "Fearless"))					jobperformance += 15;	//
	if (girl->has_trait( "Iron Will"))				jobperformance += 10;	//

	//Bad
	if (girl->has_trait( "No Legs"))					jobperformance -= 80;	//Difficult to make action scene
	if (girl->has_trait( "One Leg"))					jobperformance -= 50;	//
	if (girl->has_trait( "No Feet"))					jobperformance -= 80;	//
	if (girl->has_trait( "One Foot"))					jobperformance -= 50;	//
	if (girl->has_trait( "No Arms"))					jobperformance -= 50;	//
	if (girl->has_trait( "One Arm"))					jobperformance -= 30;	//
	if (girl->has_trait( "One Hand"))					jobperformance -= 20;	//
	if (girl->has_trait( "No Hands"))					jobperformance -= 25;	//
	if (girl->has_trait( "Blind"))					jobperformance -= 50;	//
	if (girl->has_trait( "Fragile"))					jobperformance -= 30;	//Tends to get hurt on set
	if (girl->has_trait( "Delicate"))					jobperformance -= 30;	//
	if (girl->has_trait( "Clumsy"))					jobperformance -= 25;	//
	if (girl->has_trait( "Dojikko"))					jobperformance -= 15;	//
	if (girl->has_trait( "Muggle"))					jobperformance -= 10;	//
	if (girl->has_trait( "Meek"))						jobperformance -= 5;	//

	return jobperformance;
}
