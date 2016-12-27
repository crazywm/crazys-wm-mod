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

// Job Movie Studio - Hardcore - 
bool cJobManager::WorkFilmPublicBDSM(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	//taken care of in building flow, leaving it in for robustness
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene. She was glad for the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50, tips = 0;
	int enjoy = 0;
	double jobperformance = JP_FilmPublicBDSM(girl, false);
	int bonus = 0;

	//JOB
	enum { BYMAN = 1, BYBEAST };
	int fucked = 0, impact = 0;
	bool throat = false, hard = false;
	

	g_Girls.UnequipCombat(girl);	// not for actress (yet)
	int roll = g_Dice.d100();
	if (g_Girls.GetStat(girl, STAT_HEALTH) < 50)
	{
		ss << gettext("The crew refused to film a Public Torture scene with ") << girlName << gettext(" because she is not healthy enough.\n\"We are NOT filming snuff.\"");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->is_pregnant())
	{
		ss << gettext("The crew refused to film a Public Torture scene with ") << girlName << gettext(" due to her pregnancy.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (g_Girls.HasTrait(girl, "Mind Fucked"))
	{
		ss << gettext("Mind fucked ") << girlName << gettext(" was pleased to be paraded naked in public and tortured and degraded. It is natural.");
		bonus += 10;
	}
	else if (g_Girls.HasTrait(girl, "Masochist"))
	{
		ss << gettext("Masochist ") << girlName << gettext(" was pleased to be tortured and degraded. It is her place.");
		bonus += 6;
	}
	else if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << girlName << gettext(" refused to have any part in this");
		if (girl->is_slave())
		{
			if (The_Player->disposition() > 30)  // nice
			{
				ss << gettext(" \"monstrous\" scene. She was clearly horrified at the thought so you allowed her the day off.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, 2);
				g_Girls.UpdateStat(girl, STAT_PCHATE, -1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (The_Player->disposition() > -30) //pragmatic
			{
				ss << gettext(" \"monstrous\" scene. She was clearly horrified so you had your men drug her before stripping her down for action.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
				g_Girls.UpdateStat(girl, STAT_PCHATE, 2);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, 2);
				The_Player->disposition(-1);
				enjoy -= 2;
			}
			else if (The_Player->disposition() > -30)
			{
				ss << gettext(" \"monstrous\" scene.\nShe was clearly horrified so you had your men strip her, drag her outside and ");
				/**/ if (g_Girls.HasTrait(girl, "Pierced Clit"))		ss << gettext("whip her clittoral piercing");
				else if (g_Girls.HasTrait(girl, "Pierced Nipples"))		ss << gettext("whip her nipple piercings");
				else ss << gettext("whip some humility into her");
				ss << gettext(" ahead of the scene.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -4);
				g_Girls.UpdateStat(girl, STAT_PCHATE, +5);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, +5);
				The_Player->disposition(-2);
				enjoy -= 6;
			}
		}
		else // not a slave
		{
			ss << " \"monstrous\" scene today and left.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << gettext(" was taken for filming in public bondage and torture scenes.");

	ss << gettext("\n\n");

	//SCENERY
	ss << gettext("A crowd gathered to watch as ") << girlName;
	
	//SCENARIOS
	int NUMSCENARIOS = 9;
	roll = g_Dice % NUMSCENARIOS;
		
	switch (roll)
	{
	case 0:
		ss << gettext(" was hogtied naked on the floor outside ") << g_Brothels.GetName(g_Dice%g_Brothels.GetNumBrothels()) << gettext(" brothel");
		break;
	case 1:
		ss << gettext(" was stripped and locked in the public stocks in the city square");
		break;
	case 2:
		ss << gettext(" was stripped and strapped over a barrel in the market");
		break;
	case 3:
		ss << gettext(", shackled and sent into the streets in only a maid hat, was quickly surrounded");
		break;
	case 4:
		ss << gettext(" was dragged naked from the studio and tied to a tree");
		break;
	case 5:
		ss << gettext(" was stripped naked and ");
		if (g_Girls.HasTrait(girl, "Pierced Clit") || g_Girls.HasTrait(girl, "Pierced Nipples") || g_Girls.HasTrait(girl, "Pierced Tongue")
			|| g_Girls.HasTrait(girl, "Pierced Navel") || g_Girls.HasTrait(girl, "Pierced Nose")) ss << gettext("tied tightly in place through the rings in her piercings");
		else
		{
			ss << gettext("had her arms taped-together behind her back, exposing her ");
			if (g_Girls.HasTrait(girl, "Perky Nipples")) ss << gettext("perky nipples");
			else if (g_Girls.HasTrait(girl, "Puffy Nipples")) ss << gettext("puffy nipples");
			else if (g_Girls.HasTrait(girl, "Abundant Lactation") || g_Girls.HasTrait(girl, "Cow Tits")) ss << gettext("large, milky breasts");
			else if (g_Girls.HasTrait(girl, "Flat Chest") || g_Girls.HasTrait(girl, "Petite Breasts") || g_Girls.HasTrait(girl, "Small Boobs")) ss << gettext("tiny breasts");
			else if (g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) ss << gettext("large breasts");
			else if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits")) ss << gettext("huge breasts");
			else ss << gettext("breasts");
		}
		break;
	case 6:
		ss << gettext(" was tied to a fence and had her clothes ripped off");
		break;
	case 7:
		ss << gettext(" was suspended in the air by chains");
		break;
	case 8:
		ss << gettext(" was rendered helpless and given over to the mob");
		break;
	default:
		ss << gettext(" was tied impossibly in the dirt");
		break;
	}
	ss << gettext(". The crowd jeered at her ");
	
	//Actions
	int NUMACTIONS = 10;
	roll = g_Dice % NUMACTIONS;
	
	switch (roll)
	{
	case 0:
		if (g_Girls.HasTrait(girl, "Strong Gag Reflex") || g_Girls.HasTrait(girl, "Gag Reflex"))
		{
			ss << gettext("gags, retches and gasps");
		}
		else ss << gettext("expressions");
		ss << gettext(" as guys competed to shove their dicks down her throat.");
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 1);
		throat = true;
		impact = 5;
		break;
	case 1:
		ss << gettext("yelps as a gang of teenagers slapped her tits and spanked her ass with sticks.");
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 1);
		impact = 1;
		break;
	case 2:
		ss << gettext("cries as she was double-fisted by three angry elves.");
		g_Girls.UpdateSkill(girl, SKILL_ANAL, 1);
		impact = 5;
		break;
	case 3:
		ss << gettext("gasps and squeals as a cackling old witch claiming to be her 'fairy godmother' mounted her with a thick double-dildo.");
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
		impact = 3;
		break;
	case 4:
		ss << gettext("cries as a demented priest shoved 'Candles of Cleansing' in her 'unclean places', chanting prayers as he lit them.");
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 1);
		impact = 8;
		break;
	case 5:
		ss << gettext("screams for help as she was fucked by a pack of wild beasts.");
		g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, 1);
		fucked = BYBEAST;
		hard = true;
		impact = 8;
		break;
	case 6:
		ss << gettext("struggles as a pair of ");
		if /*  */(g_Girls.GetStat(girl, STAT_BEAUTY) > 80)		ss << gettext("ugly whores, jealous of her looks");
		else if (g_Girls.GetStat(girl, STAT_AGE) < 25)			ss << gettext("old whores, jealous of her youth");
		else if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) > 75)	ss << gettext("dumb whores, jealous of her brains");
		else if (girl->is_free())									ss << gettext("slave whores, jealous of her freedom");
		else													ss << gettext("sadistic rival whores");
		ss << gettext(" spit-roasted her with some shockingly large strap-ons.");
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
		impact = 5;
		break;
	case 7:
		ss << gettext("cries as a large gang of ");
		if /* */(g_Girls.HasTrait(girl, "Tsundere") || g_Girls.HasTrait(girl, "Yandere"))	ss << gettext("losers she's rejected over the years");
		else if ((g_Girls.HasTrait(girl, "Lolita") || g_Girls.HasTrait(girl, "Flat Chest") || g_Girls.HasTrait(girl, "Petite Breasts") || g_Girls.HasTrait(girl, "Small Boobs"))
			&& (girl->age() < 21)) ss << gettext("probable pedos");
		else if (g_Girls.HasTrait(girl, "Queen") || g_Girls.HasTrait(girl, "Princess")) ss << gettext("her former subjects");
		else if (g_Girls.HasTrait(girl, "Idol") || (g_Girls.GetStat(girl, STAT_FAME) > 75))	ss << gettext("men from her fan-club");
		else if (g_Girls.HasTrait(girl, "Teacher")) ss << gettext("students from her class");
		else if (girl->m_NumCusts > 100) ss << gettext("her former customers");
		else if (girl->age() < 21) ss << gettext("guys she went to school with");
		else ss << gettext("tramps, gypsies and homeless");
		ss << gettext(" took this chance to spank her, grope her and fill her cunt with cum.");
		g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
		fucked = BYMAN;
		impact = 3;
		break;
	case 8:
		ss << gettext("screams as the police captain stubbed out a cigar on her asshole, once he and his men were 'done with that'.");
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
		fucked = BYMAN;
		hard = true;
		impact = 10;
		break;
	case 9:
		ss << gettext("agonised screams as a passing battle-mage tested out flame spells on her naked form.");
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
		hard = true;
		impact = 7;
		break;
	default:
		ss << gettext("puzzled expression as something impossible happened.");
		break;
	}
	ss << gettext("\nYour film crew rescued her once they had enough footage.\n\n");

	if (jobperformance >= 350)
	{
		ss << gettext("It was an outstanding scene, and definitely boosted her fame and resliience, even if it did wear her out a lot.");
		bonus = 12;
		g_Girls.UpdateStat(girl, STAT_FAME, 5);
		g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 3);
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 10 + impact);
	}
	else if (jobperformance >= 245)
	{
		ss << gettext("It was a great scene and should win her some fans. She looks wrecked now though.");
		bonus = 8;
		g_Girls.UpdateStat(girl, STAT_FAME, 2);
		g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 2);
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 15 + impact);
	}
	else if (jobperformance >= 185)
	{
		ss << gettext("It was a good scene, but not the best.");
		bonus = 4;
		g_Girls.UpdateStat(girl, STAT_FAME, 1);
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 15 + impact);
	}
	else if (jobperformance >= 145)
	{
		ss << gettext("It was okay, but something about the scene didn't work.");
		bonus = 2;
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 15 + impact);
	}
	else if (jobperformance >= 100)
	{
		ss << gettext("It wasn't a great public scene.");
		bonus = 1;
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 20 + impact);
	}
	else
	{
		ss << gettext("Sadly, the scene really didn't work. Even the onlookers weren't looking.");
		g_Girls.UpdateStat(girl, STAT_FAME, -1);
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, 20 + impact);
	}
	ss << gettext("\n");

	//Enjoyed? If she's deranged, she'd should have enjoyed it.
	if (g_Girls.HasTrait(girl, "Mind Fucked"))
	{
		enjoy += 16;
		ss << gettext("Being completely mind fucked, ") << girlName << gettext(" actually gets off on this.\n");
	}
	else if (g_Girls.HasTrait(girl, "Masochist"))
	{
		enjoy += 10;
		ss << girlName << gettext(" enjoys this. It's what she deserves.\n");
	}
	else if (g_Girls.HasTrait(girl, "Broken Will") || g_Girls.HasTrait(girl, "Dependant"))
	{
		enjoy += 5;
		ss << girlName << gettext(" accepts this. It is Master's will.\n");
	}
	else if (g_Girls.HasTrait(girl, "Iron Will") || g_Girls.HasTrait(girl, "Fearless"))
	{
		enjoy -= 5;
		ss << girlName << gettext(" endures in stoic silence, determined not to let you see her suffer.\n");
	}
	else if (g_Girls.HasTrait(girl, "Nymphomaniac"))
	{
		enjoy += 2;
		ss << girlName << gettext(" doesn't much like the pain, but enjoys getting this much sex and attention.\n");
	}
	//Feedback enjoyment
	if (enjoy > 10)
	{
		ss << gettext("She won't say it, but you suspect she secretly gets off on the attention, sin and degradation.\n\n");
	}
	else if (enjoy > 0)
	{
		ss << gettext("She's only a little traumatised.\n\n");
	}
	else
	{
		ss << gettext("From the way she's still crouched, rocking in a corner and silently weeping... you figure she didn't enjoy this.\n\n");
	}

	bonus = bonus + enjoy;
	
	if (fucked == BYMAN || fucked == BYBEAST)
	{
		if (g_Girls.CheckVirginity(girl))
		{
			ss << "Somewhere in all this, she lost her virginity.\n";
			g_Girls.LoseVirginity(girl);
			bonus += 5;
		}
		sCustomer* Cust = new sCustomer; g_Customers.GetCustomer(Cust, brothel); Cust->m_Amount = 1;
		if (fucked == BYMAN)
		{
			
			if (!girl->calc_pregnancy(Cust, false, 0.75))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant.", 0);
				ss << "And she's now pregnant.\nCongratulations!\n";
			}
		}
		else
		{
			if (!girl->calc_insemination(g_Girls.GetBeast(), false, 0.75))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has been inseminated.", 0);
				ss << "And she's been inseminated by a beast.\nCongratulations!\n";
			}
		}
		delete Cust;
	}
	
	//BSIN: Chaned .AddScene fn to use JOB_FILMxxxxx instead of SKILL as it makes much more sense.
	//I'm guessing this function was made before that Constant was added.
	int finalqual = g_Studios.AddScene(girl, JOB_FILMPUBLICBDSM, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);

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
	int xp = 10, skill = 3;
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }

	//Hate
	g_Girls.UpdateStat(girl, STAT_PCHATE, (impact - enjoy));
	g_Girls.UpdateStat(girl, STAT_PCFEAR, (impact - enjoy));
	//Health
	g_Girls.UpdateStat(girl, STAT_HEALTH, (4 * impact));


	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_BDSM, g_Dice%skill + 1);
	g_Girls.UpdateStat(girl, STAT_OBEDIENCE, impact / 2);

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	//gain
	g_Girls.PossiblyGainNewTrait(girl, "Masochist", 75, ACTION_SEX, girlName + " has turned into a Masochist from filming so many BDSM scenes.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (hard)
	{
		g_Girls.PossiblyGainNewTrait(girl, "Mind Fucked", 65, ACTION_SEX, girlName + " was pushed too far in her public torture film and is now completely Mind Fucked.", Day0Night1);
		g_Girls.GirlInjured(girl, 5);
	}
	//lose
	if (hard) g_Girls.PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, girlName + "'s public degradation has shattered her iron will.", Day0Night1);


	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = g_Dice % 8, MrNasty = g_Dice % 8;
	MrEvil = (MrEvil+ MrNasty) / 2;				//Should come out around 3 most of the time.

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

double cJobManager::JP_FilmPublicBDSM(sGirl* girl, bool estimate)
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_DIGNITY));

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	if (g_Girls.HasTrait(girl, "Mind Fucked"))				jobperformance += 50;	//this is her home
	if (g_Girls.HasTrait(girl, "Masochist"))				jobperformance += 30;	//she likes it
	if (g_Girls.HasTrait(girl, "Twisted"))					jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Open Minded"))				jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Broken Will"))				jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Pierced Clit"))				jobperformance += 40;	//Piercings can be useful for this...
	if (g_Girls.HasTrait(girl, "Pierced Nipples"))			jobperformance += 40;	//
	if (g_Girls.HasTrait(girl, "Pierced Tongue"))			jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Pierced Nose"))				jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Pierced Navel"))			jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "Princess"))					jobperformance += 30;	//High-Status degraded
	if (g_Girls.HasTrait(girl, "Queen"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Goddess"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Angel"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Noble"))					jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Elegant"))					jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Branded on the Forehead"))	jobperformance += 5;	//Degraded
	if (g_Girls.HasTrait(girl, "Branded on the Ass"))		jobperformance += 5;	//
	if (g_Girls.CheckVirginity(girl))						jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Strong Gag Reflex"))		jobperformance += 10;	//Degrading
	if (g_Girls.HasTrait(girl, "Gag Reflex"))				jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "No Gag Reflex"))			jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "Deep Throat"))				jobperformance += 10;	//Capable
	if (g_Girls.HasTrait(girl, "Tsundere"))					jobperformance += 15;	//Mistreated customers can see haughty girl knocked down
	if (g_Girls.HasTrait(girl, "Yandere"))					jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Actress"))					jobperformance += 25;	//Can play it up for crowd

	if (g_Girls.HasTrait(girl, "Iron Will"))				jobperformance -= 30;	//Refuses to react
	if (g_Girls.HasTrait(girl, "Incorporeal"))				jobperformance -= 60;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Construct"))				jobperformance -= 60;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Half-Construct"))			jobperformance -= 50;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Skeleton"))					jobperformance -= 80;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Undead"))					jobperformance -= 80;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Zombie"))					jobperformance -= 80;	//Can't be hurt
	if (g_Girls.HasTrait(girl, "Fragile"))					jobperformance -= 80;	//Too quickly damaged
	if (g_Girls.HasTrait(girl, "Delicate"))					jobperformance -= 80;	//Too quickly damaged

return jobperformance;
}
