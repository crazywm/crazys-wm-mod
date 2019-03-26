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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"


extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;
extern cInventory g_InvManager;

#pragma endregion

// `J` Job Farm - Staff - Learning_Job
bool cJobManager::WorkFarmResearch(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKTRAINING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a reseacher on the farm.";

	g_Girls.UnequipCombat(girl);	// put that shit away

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	int train = 0;												// main skill trained
	int tanm = girl->m_Skills[SKILL_ANIMALHANDLING];			// Starting level - train = 1
	int tfar = girl->m_Skills[SKILL_FARMING];					// Starting level - train = 2
	int tmag = girl->m_Skills[SKILL_MAGIC];						// Starting level - train = 3
	int ther = girl->m_Skills[SKILL_HERBALISM];					// Starting level - train = 4
	int tint = girl->m_Stats[STAT_INTELLIGENCE];				// Starting level - train = 5
	bool gaintrait = false;										// posibility of gaining a trait
	int skill = 0;												// gian for main skill trained
	int dirtyloss = brothel->m_Filthiness / 100;				// training time wasted with bad equipment
	int sgAnm = 0, sgFar = 0, sgMag = 0, sgHer = 0, sgInt = 0;	// gains per skill


	/* */if (roll_a <= 5)	skill = 7;
	else if (roll_a <= 15)	skill = 6;
	else if (roll_a <= 30)	skill = 5;
	else if (roll_a <= 60)	skill = 4;
	else /*             */	skill = 3;
	/* */if (girl->has_trait( "Quick Learner"))	{ skill += 1; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; }
	skill -= dirtyloss;
	ss << "The Farm Lab is ";
	if (dirtyloss <= 0) ss << "clean and tidy";
	if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
	if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
	if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
	if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
	ss << ".\n \n";
	if (skill < 1) skill = 1;	// always at least 1

	do{		// `J` New method of selecting what job to do
		/* */if (roll_b < 20  && tanm < 100)	train = 1;	// animalhandling
		else if (roll_b < 40  && tfar < 100)	train = 2;	// farming
		else if (roll_b < 60  && tmag < 100)	train = 3;	// magic
		else if (roll_b < 80  && ther < 100)	train = 4;	// herbalism
		else if (roll_b < 100 && tint < 100)	train = 5;	// intelligence
		roll_b -= 10;
	} while (train == 0 && roll_b > 0);
	if (train == 0 || g_Dice.percent(5)) gaintrait = true;

	if (train == 1) { sgAnm = skill; ss << "She researches animals.\n"; }				else sgAnm = g_Dice % 2;
	if (train == 2) { sgFar = skill; ss << "She researches farming techniques.\n"; }	else sgFar = g_Dice % 2;
	if (train == 3) { sgMag = skill; ss << "She researches magical techniques.\n"; }	else sgMag = g_Dice % 2;
	if (train == 4) { sgHer = skill; ss << "She researches plants and their uses.\n"; }	else sgHer = g_Dice % 2;
	if (train == 5) { sgInt = skill; ss << "She researches general topics.\n"; }		else sgInt = g_Dice % 2;

	if (sgAnm + sgFar + sgMag + sgHer + sgInt > 0)
	{
		ss << "She managed to gain:\n";
		if (sgAnm > 0) { ss << sgAnm << " Animal Handling\n";	girl->animalhandling(sgAnm); }
		if (sgFar > 0) { ss << sgFar << " Farming\n";			girl->farming(sgFar); }
		if (sgMag > 0) { ss << sgMag << " Magic\n";				girl->magic(sgMag); }
		if (sgHer > 0) { ss << sgHer << " Herbalism\n";			girl->herbalism(sgHer); }
		if (sgInt > 0) { ss << sgInt << " Intelligence\n";		girl->intelligence(sgInt); }
	}

	int trycount = skill;
	while (gaintrait && trycount > 0)	// `J` Try to add a trait
	{
		trycount--;
		switch (g_Dice % 10)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			if (girl->has_trait( "Dependant"))
			{
				girl->remove_trait("Dependant");
				ss << "She seems to be getting over her Dependancy with her training.";
				gaintrait = false;
			}
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:

			break;
		case 6:

			break;

		default:	break;	// no trait gained
		}
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	//enjoyed the work or not
	/* */if (roll_c <= 10)	{ enjoy -= g_Dice % 3 + 1;	ss << "She did not enjoy her time training."; }
	else if (roll_c >= 90)	{ enjoy += g_Dice % 3 + 1;	ss << "She had a pleasant time training."; }
	else /*             */	{ enjoy += g_Dice % 2;		ss << "Otherwise, the shift passed uneventfully."; }

	ss << "\n \n";

#pragma endregion
#pragma region	//	Create Items				//



	// `J` Farm Bookmark - adding in items that can be created in the farm
	if (girl->intelligence() + girl->crafting() > 100 && g_Dice.percent(girl->intelligence() + girl->crafting() / 10))	// 10-20%
	{
		sInventoryItem* item = NULL;
		string itemname = "";
		int tries = skill;
		while (itemname == "" && tries > 0)
		{
			switch (g_Dice % 20)
			{
				/*	For each item available, the girl making it must have:
				Skills:
				*	If an item gives less than 40 skill points, the girl must have 40 more than the amount given in that skill
				*	If it gives more than 40 points, there will need to be some magic included.
				*	If there are multiple skills, add 40 to each bonus and if skills (a+b+c) > bonus total
				Stats:
				*	Stats will need more variation in how high they are needed because it is less knowledge than conditioning
				Traits:
				*	Most traits will require some magic
				Randomness:
				*	There also should be a g_Dice.percent() with the main Skills averaged and divided by 10
				*		If there are 3 skills, then g_Dice.percent((a+b+c)/30)
				*/
			case 0:
				if (girl->farming() > 50 && girl->animalhandling() > 50
					&& g_Dice.percent(girl->farming() + girl->animalhandling() / 20))
					itemname = "Farmer's Guide";		// +10 Farming, +10 AnimalHandling
				break;
			case 1:
				if (girl->constitution() > 60 && g_Dice.percent(girl->constitution() / 10))
					itemname = "Manual of Health";		// (+15 Beast/Group, +10 Cons/Str, +5 BDSM/Comb)
				break;
			case 2:
				if (girl->magic() > 60 && g_Dice.percent(girl->magic() / 10))
					itemname = "Manual of Magic";		// +20 magic
				break;
			case 3:
				if (girl->magic() > 80 && g_Dice.percent(girl->magic() / 10))
					itemname = "Codex of the Arcane";	// +40 magic
				break;
			case 4:
				if (girl->lesbian() > 40 && g_Dice.percent(girl->lesbian() / 10))
					itemname = "Manual of Two Roses";	// +20 Lesbian
				break;
			case 5:
			{
				if (girl->lesbian() > 80 && g_Dice.percent(girl->lesbian() / 10))
					itemname = "Codex of Sappho";		// +40 Lesbian
			}break;
			case 6:
				if (girl->bdsm() > 60 && g_Dice.percent(girl->bdsm() / 10))
					itemname = "Manual of Bondage";		// (+20 BDSM, +5 Cons)
				break;
			case 7:
				if (girl->combat() > 60 && g_Dice.percent(girl->combat() / 10))
					itemname = "Manual of Arms";		// (+20 Com)
				break;
			case 8:
				if (girl->performance() + girl->strip() > 100 && g_Dice.percent((girl->performance() + girl->strip()) / 20))
					itemname = "Manual of the Dancer";	// (+15 Serv/Strip/Perf, +5 Norm/Agi)
				break;
			case 9:
				if (girl->normalsex() + girl->oralsex() + girl->anal() > 150 && g_Dice.percent((girl->normalsex() + girl->oralsex() + girl->anal()) / 30))
					itemname = "Manual of Sex";			// (+15 Norm, +10 Oral, +5 Anal)
				break;
			case 10:
			{
				if (girl->magic() < 80 && girl->mana() < 20) break;
				int manacost = 60;
				/* */if (girl->has_trait("Sterile"))		manacost = 80;
				else if (girl->has_trait("Fertile"))		manacost = 40;
				else if (girl->has_trait("Broodmother"))	manacost = 20;
				if (girl->mana() >= manacost && g_Dice.percent(girl->magic() - manacost))
				{
					girl->mana(-manacost);
					itemname = "Fertility Tome";				// (-Sterile, +Fertile, +50 Normal Sex, +100 Libido)
				}
			}break;
			case 11:
			{
				// Noble, Princess and Queen needs 40, everyone else needs 60 to make this
				if (girl->has_trait("Noble") || girl->has_trait("Princess") || girl->has_trait("Queen"))
					if (girl->refinement() < 40 || girl->service() < 40 || girl->intelligence() < 40)		break;
				else if (girl->refinement() < 60 && girl->service() < 60 && girl->intelligence() < 60)		break;
				// she can make it, now does she?
				if (g_Dice.percent((girl->refinement() + girl->service() + girl->intelligence()) / 30))
					itemname = "Codex of the Courtesan";		// (+20 Serv/Strip/Refin, +10 Mor/Dig/Cha/Int/Lib/Conf/Oral)
			}break;
			case 12:
			{
				int manacost = 70;
				// Dominatrix, Masochist and Sadistic needs 50, everyone else needs 70 to make this
				if (girl->has_trait("Dominatrix") || girl->has_trait("Masochist") || girl->has_trait("Sadistic"))
				{
					if (girl->bdsm() < 50 || girl->magic() < 50 || girl->mana() < 50)		break;
					manacost = 50;
				}
				else if (girl->bdsm() < 70 && girl->magic() < 70 && girl->mana() < 70)		break;
				// she can make it, now does she?
				if (g_Dice.percent((girl->bdsm() + girl->magic()) / 20))
				{
					girl->mana(-manacost);
					itemname = "Codex of Submission";		// (+30 Obed, -30 Spi/Dig, +20 BDSM, +10 Anal/Group/Oral/Hand/Foot)
				}
			}break;
			case 13:
			{
				if (girl->combat() > 80 && g_Dice.percent(girl->combat() / 10))
					itemname = "Codex of Mars";			// (+40 Com, Adds Brawler)
			}break;
			case 14:
			{
				if (girl->normalsex() + girl->oralsex() + girl->anal() > 170 && g_Dice.percent((girl->normalsex() + girl->oralsex() + girl->anal()) / 30))
					itemname = "Codex of Eros";			// (+30 Norm, +10 Anal/Oral)
			}break;
			case 15:
			{
				if (girl->medicine() + girl->intelligence() > 110 && g_Dice.percent((girl->medicine() + girl->intelligence()) / 20))
					itemname = "Codex of Asclepius";	// (+20 Med, +10 Int)
			}break;
			case 16:
			{
				/*

				*/
			}break;
			case 17:
			{
				/*

				*/
			}break;
			case 18:
			{
				/*

				*/
			}break;

			default:
				break;
			}
		}

		item = g_InvManager.GetItem(itemname);
		if (item)
		{
			g_Brothels.AddItemToInventory(item);
			ss << girlName << " managed to create a " << itemname << " by compiling her notes together.\n";
		}
	}



#pragma endregion
#pragma region	//	Money					//

	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket())) { wages = 0; }
	else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more


#pragma endregion
#pragma region	//	Finish the shift			//



	girl->upd_Enjoyment(actiontype, enjoy);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill / 2);

	if (girl->has_trait( "Quick Learner"))		{ xp += 2; }
	else if (girl->has_trait( "Slow Learner"))	{ xp -= 2; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

#pragma endregion
	return false;
}

double cJobManager::JP_FarmResearch(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(100 - girl->m_Skills[SKILL_ANIMALHANDLING]) +
			(100 - girl->m_Skills[SKILL_FARMING]) +
			(100 - girl->m_Skills[SKILL_MAGIC]) +
			(100 - girl->m_Skills[SKILL_HERBALISM]) +
			(100 - girl->m_Stats[STAT_INTELLIGENCE]);
	}
	else// for the actual check
	{
		jobperformance = (girl->intelligence() / 2 +
			girl->herbalism() / 2 +
			girl->brewing());
		if (!estimate)
		{
			int t = girl->tiredness() - 80;
			if (t > 0)
				jobperformance -= (t + 2) * (t / 3);
		}

		//good traits
		if (girl->has_trait( "Quick Learner"))  jobperformance += 5;
		if (girl->has_trait( "Psychic"))		  jobperformance += 10;

		//bad traits
		if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
		if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
		if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
		if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
		if (girl->has_trait( "Meek"))			jobperformance -= 20;
	}
	return jobperformance;
}
