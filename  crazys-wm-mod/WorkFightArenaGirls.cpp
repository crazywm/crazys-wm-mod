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
#include "cArena.h"
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
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

#pragma endregion

// `J` Job Arena - Fighting
bool cJobManager::WorkFightArenaGirls(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " was assigned to fight other girls in the arena.\n \n";


	int wages = 0, tips = 0;
	int fight_outcome = 0, enjoy = 0, fame = 0;

	int imagetype = IMGTYPE_COMBAT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_FightArenaGirls(girl, false);

	g_Girls.EquipCombat(girl);		// ready armor and weapons!

	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, false, false, true);
	if (tempgirl) fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
	else fight_outcome = 7;			// `J` reworked incase there are no Non-Human Random Girls
	if (fight_outcome == 7)
	{
		g_LogFile.write("Error: You have no Arena Girls for your girls to fight\n");
		g_LogFile.write("Error: You need an Arena Girl to allow WorkFightArenaGirls randomness");
		ss << "There were no Arena Girls for her to fight.\n \n(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
		imagetype = IMGTYPE_PROFILE;
	}
	else if (fight_outcome == 1)	// she won
	{
		enjoy = g_Dice % 3 + 1;
		fame = g_Dice % 3 + 1;
		sGirl* ugirl = 0;
		if (g_Dice.percent(10))		// chance of getting unique girl
		{
			ugirl = g_Girls.GetRandomGirl(false, false, true);
		}
		if (ugirl)
		{
			stringstream msg;	// goes to the girl and the g_MessageQue
			stringstream Umsg;	// goes to the new girl
			stringstream Tmsg;	// temp msg
			ugirl->m_Stats[STAT_HEALTH] = g_Dice % 50 + 1;
			ugirl->m_Stats[STAT_HAPPINESS] = g_Dice % 80 + 1;
			ugirl->m_Stats[STAT_TIREDNESS] = g_Dice % 50 + 50;
			ugirl->m_States |= (1 << STATUS_ARENA);
			msg << girlName << " won her fight against " << ugirl->m_Realname << ".\n \n";
			Umsg << ugirl->m_Realname << " lost her fight against your girl " << girlName << ".\n \n";
			Tmsg << ugirl->m_Realname;
			if (g_Dice.percent(50))
			{
				ugirl->m_States |= (1 << STATUS_SLAVE);
				Tmsg << "'s owner could not afford to pay you your winnings so he gave her to you instead.\n \n";
			}
			else
			{
				Tmsg << " put up a good fight so you let her live as long as she came work for you.\n \n";
				wages = 100 + g_Dice % (girl->fame() + girl->charisma());
			}
			msg << Tmsg.str();
			Umsg << Tmsg.str();
			ss << msg.str();
			g_MessageQue.AddToQue(msg.str(), 0);
			ugirl->m_Events.AddMessage(Umsg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);

			g_Brothels.GetDungeon()->AddGirl(ugirl, DUNGEON_NEWARENA);
		}
		else
		{
			if (girl->has_trait( "Exhibitionist"))
			{
				ss << "As she enjoys showing off her body, " << girlName << " lets her opponent cut away her already-skimpy clothing, ending her match all but naked - but victorious.  Many of the patrons seem to enjoy the show.\n";
			}
			else if (girl->has_trait( "Sadist"))
			{
				ss << "As the match draws to a close, " << girlName << " stops trying to win and simply toys with her opponent, practically torturing her in front of the audience.  Many patrons disapprove, although a few respond favorably.\n";
			}
			else if (girl->has_trait( "Dominatrix"))
			{
				ss << girlName << "'s verbal abuse, along with her combat prowess, lead to a dramatic defeat of her opponent, although her behavior towards the loser disappoints many patrons.\n";
			}
			else if (girl->has_trait( "Brawler"))
			{
				ss << girlName << "'s preference for grappling and clinch-fighting throws her opponent off-balance and gives her an easy, if somewhat boring, victory.\n";
			}
			else if (girl->has_trait( "Flight"))
			{
				ss << "Being able to fly is an almost-unfair advantage that most opponents can't counter.  Thanks to that, " << girlName << " wins handily.\n";
			}
			else if (girl->has_trait( "Assassin"))
			{
				ss << "Utilizing a no-rules, victory-at-any-cost combat style, " << girlName << " won a decisive victory over her orthodox opponent.  This was not unnoticed by some people in the audience.\n";
			}
			else if (girl->has_trait( "Masochist"))
			{
				ss << girlName << "'s cries of joy and outright pleasure upon being injured distract her opponent, and the pain-loving girl comes back from apparent defeat to achieve a dramatic victory.  Despite her many injuries, she smiles happily as she thanks her opponent for the match.\n";
			}
			else if (girl->magic() >= 50 && girl->combat() < 50)
			{
				ss << girlName << "'s powerful magic demonstrates precisely why mages are feared by many.  She even manages to look good while doing it!\n";
			}
			else if (girl->magic() >= 75 && girl->combat() >= 75)
			{
				ss << "Having mastered both weapons and sorcery, " << girlName << " is a nearly unstoppable force of nature in the Arena, easily dispatching opponents who focus on one branch of combat over the other.\n";
			}
			else
			{
				ss << girlName << " won her fight.";
			}
			wages = 100 + g_Dice % (girl->fame() + girl->charisma());
		}
	}
	else if (fight_outcome == 2) // she lost
	{
		enjoy = -(g_Dice % 3 + 1);
		fame = -(g_Dice % 3 + 1);
		if (girl->has_trait( "Exhibitionist"))
		{
			ss << "As she enjoys showing off her body, " << girlName << " lets her opponent cut away her already-skimpy clothing, but either because of her lack of skill or just bad luck, she takes a real hit and is defeated.\n";
		}
		else if (girl->has_trait( "Sadist"))
		{
			ss << girlName << " tries to torment her opponent, but her overconfidence leads to a stunning defeat when the other fighter feigns injury.\n";
		}
		else if (girl->has_trait( "Flight"))
		{
			ss << girlName << " was unable to leverage her greater maneuverability and yielded the match after being forced to the ground.\n";
		}
		else if (girl->has_trait( "Masochist"))
		{
			ss << "Overwhelmed by pleasure, " << girlName << " is unable to defend herself from her opponent and is easily defeated.  After the match ends, she begs the other fighter for another match - 'just like this one.'\n";
		}
		else if (girl->magic() >= 50 && girl->combat() < 50)
		{
			ss << "Lacking the physical prowess to hold her opponent off while she readies her spells, " << girlName << " is quickly defeated by her opponent.\n";
		}
		else if (girl->magic() >= 75 && girl->combat() >= 75)
		{
			ss << "You can't belive, " << girlName << " lost. With her skill in combat and magic you thought her unbeatable.\n";
		}
		else
		{
			ss << "She lost the fight.";
		}
		int cost = 150;
		brothel->m_Finance.arena_costs(cost);
		ss << " You had to pay " << cost << " gold cause your girl lost.";
		/*that should work but now need to make if you lose the girl if you dont have the gold zzzzz FIXME*/
	}
	else if (fight_outcome == 0)  // it was a draw
	{
		enjoy = g_Dice % 3 - 2;
		fame = g_Dice % 3 - 2;
		ss << "The fight ended in a draw.";
	}

	if (girl->is_pregnant())
	{
		if (girl->strength() >= 60)
		{
			ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as " << girlName << " .\n";
		}
		else
		{
			ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like " << girlName << " .\n";
		}
		girl->tiredness(10 - girl->strength() / 20 );
	}

	if (girl->has_trait( "Exhibitionist") && g_Dice.percent(15))
	{
		ss  << "A flamboyant fighter, " << girlName << " fights with as little armor and clothing as possible, and sometimes takes something off in the middle of a match, to the enjoyment of many fans.\n";
	}

	if (girl->has_trait( "Idol") && g_Dice.percent(15))
	{
		ss  << girlName << " has quite the following, and the Arena is almost always packed when she fights.  People just love to watch her in action.\n";
	}

	if (tempgirl) delete tempgirl; tempgirl = 0;	// Cleanup


	// Improve girl
	int fightxp = (fight_outcome == 1) ? 3 : 1;
	int xp = 5 * fightxp, libido = 5, skill = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		wages = 0;
	}

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	girl->fame(fame);
	girl->exp(xp);
	girl->combat(g_Dice%fightxp + skill);
	girl->magic(g_Dice%fightxp + skill);
	girl->agility(g_Dice%fightxp + skill);
	girl->constitution(g_Dice%fightxp + skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);

	/* `J` this will be a place holder until a better payment system gets done
	*
	*/
	int earned = 0;
	for (int i = 0; i < jobperformance; i++)
	{
		earned += g_Dice % 10 + 5; // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
	}
	brothel->m_Finance.arena_income(earned);
	ss.str("");
	ss << girlName << " drew in " << jobperformance << " people to watch her and you earned " << earned << " from it.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);


	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 65, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 55, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 70, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
	if (g_Dice.percent(25) && girl->strength() >= 65 && girl->combat() > girl->magic())
	{
		g_Girls.PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, girlName + " has become pretty Strong from all of the fights she's been in.", Day0Night1);
	}
	if (g_Dice.percent(25) && girl->combat() >= 60 && girl->combat() > girl->magic())
	{
		g_Girls.PossiblyGainNewTrait(girl, "Brawler", 60, ACTION_COMBAT, girlName + " has become pretty good at fighting.", Day0Night1);
	}
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 35, actiontype, girlName + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_FightArenaGirls(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;

	if (estimate)// for third detail string
	{
		jobperformance +=
			(girl->fame() / 2) +
			(girl->charisma() / 2) +
			(girl->combat() / 2) +
			(girl->magic() / 2) +
			(girl->level());
	}
	else// for the actual check
	{
		jobperformance += (girl->fame() + girl->charisma()) / 2;
		if (!estimate)
		{
			int t = girl->tiredness() - 80;
			if (t > 0)
				jobperformance -= (t + 2) * (t / 2);
		}

	}
	return jobperformance;
}
