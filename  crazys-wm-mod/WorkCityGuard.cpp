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
extern cMovieStudioManager g_Studios;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern cPlayer* The_Player;

// `J` Job Arena - Staff
bool cJobManager::WorkCityGuard(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKSECURITY;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " helps guard the city.\n\n";

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100();
	int wages = 150, enjoy = 0, enjoyc = 0, sus = 0;
	int imagetype = IMGTYPE_PROFILE;

	int agl = (g_Girls.GetStat(girl, STAT_AGILITY) / 2 + g_Dice % (g_Girls.GetSkill(girl, SKILL_COMBAT) / 2));


	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, false, false, true);

	if (roll_a >= 50)
	{
		ss << girlName << " didn't find any trouble today.";
		sus -= 5;
	}
	else if (roll_a >= 25)
	{
		ss << girlName << " spotted a theif and ";
		if (agl >= 90)
		{
			ss << "was on them before they could blink.  Putting a stop to the theft.";
			sus -= 20;
			enjoy += 3;
		}
		else if (agl >= 75)
		{
			ss << "was on them before they could get away.  She is quick.";
			sus -= 15;
			enjoy += 1;
		}
		else if (agl >= 50)
		{
			ss << "was able to keep up, ";
			if (roll_b >= 50)	{ sus += 5; ss << "but they ended up giving her the slip."; }
			else /*        */	{ sus -= 10; ss << "and was able to catch them."; }
		}
		else
		{
			ss << "was left eating dust. Damn is she slow.";
			sus += 10;
			enjoy -= 3;
		}
	}
	else
	{
		Uint8 fight_outcome = 0;
		if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
		{
			fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
		}
		else
		{
			g_LogFile.write("Error: You have no Random Girls for your girls to fight\n");
			g_LogFile.write("Error: You need a Random Girl to allow WorkCityGuard randomness");
			fight_outcome = 7;
		}
		if (fight_outcome == 7)
		{
			ss << "There were no criminals around for her to fight.\n\n";
			ss << "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
		}
		else if (fight_outcome == 1)	// she won
		{
			enjoy += 3; enjoyc += 3;
			ss << girlName << " ran into some trouble and ended up in a fight. She was able to win.";
			sus -= 20;
			imagetype = IMGTYPE_COMBAT;
		}
		else  // she lost or it was a draw
		{
			ss << girlName << " ran into some trouble and ended up in a fight. She was unable to win the fight.";
			enjoy -= 1; enjoyc -= 1;
			sus += 10;
			imagetype = IMGTYPE_COMBAT;
		}
	}

	// Cleanup
	if (tempgirl) delete tempgirl; tempgirl = 0;


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	The_Player->suspicion(sus);
	girl->m_Pay = wages;


	// Improve girl
	int xp = 8, libido = 2, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice % skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice % skill);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice % skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % skill);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);


	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, enjoyc);
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 20, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 30, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);

	return false;
}

double cJobManager::JP_CityGuard(sGirl* girl, bool estimate)// not used
{
	// copied from Security
	int SecLev = 0;
	if (estimate)	// for third detail string
	{
		SecLev = (g_Girls.GetSkill(girl, SKILL_COMBAT))
			+ (g_Girls.GetSkill(girl, SKILL_MAGIC) / 2)
			+ (g_Girls.GetStat(girl, STAT_AGILITY) / 2);
	}
	else			// for the actual check
	{
		SecLev = g_Dice % (g_Girls.GetSkill(girl, SKILL_COMBAT))
			+ g_Dice % (g_Girls.GetSkill(girl, SKILL_MAGIC) / 3)
			+ g_Dice % (g_Girls.GetStat(girl, STAT_AGILITY) / 3);

		int t = girl->tiredness() - 80;
		if (t > 0)
			SecLev -= t * 2;
	}

	// Good traits
	if (g_Girls.HasTrait(girl, "Incorporeal"))			SecLev += 100;	// I'm fucking Superman!
	if (g_Girls.HasTrait(girl, "Assassin"))				SecLev += 50;	// I was born for this job. I know how you think. Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it (That I'm aware of) so it trades off(?)                   
	if (g_Girls.HasTrait(girl, "Zombie"))				SecLev += 20;	// You have what guarding the city?!?! I'm out of here.
	if (g_Girls.HasTrait(girl, "Skeleton"))				SecLev += 10;	// You have what guarding the city?!?! I'm out of here.
	if (g_Girls.HasTrait(girl, "Tough"))				SecLev += 15;	// You hit like a girl
	if (g_Girls.HasTrait(girl, "Powerful Magic"))		SecLev += 10;	//
	if (g_Girls.HasTrait(girl, "Psychic"))				SecLev += 10;	// I sense danger
	if (g_Girls.HasTrait(girl, "Demon"))				SecLev += 10;	// Even scarier
	if (g_Girls.HasTrait(girl, "Brawler"))				SecLev += 10;	//
	if (g_Girls.HasTrait(girl, "Strong"))				SecLev += 10;	//
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))		SecLev += 5;	// Moves around quickly
	if (g_Girls.HasTrait(girl, "Charming"))				SecLev += 5;	// Gets more cooperation
	if (g_Girls.HasTrait(girl, "Cool Person"))			SecLev += 5;	// Gets more cooperation redux
	if (g_Girls.HasTrait(girl, "Adventurer"))			SecLev += 5;	// Has experience
	if (g_Girls.HasTrait(girl, "Aggressive"))			SecLev += 5;	// Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Yandere"))				SecLev += 5;	// Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Tsundere"))				SecLev += 5;	// Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Sadistic"))				SecLev += 5;	// I kill you slowly with this dull knife!
	if (g_Girls.HasTrait(girl, "Merciless"))			SecLev += 5;	// Your cries for clemency amuse me
	if (g_Girls.HasTrait(girl, "Fearless"))				SecLev += 5;	// Chhhhaaaarrrrrggggeeeeee!!
	if (g_Girls.HasTrait(girl, "Iron Will"))			SecLev += 5;	// Hold the line!
	if (g_Girls.HasTrait(girl, "Construct"))			SecLev += 5;	// Scary
	if (g_Girls.HasTrait(girl, "Not Human"))			SecLev += 5;	// Scary
	if (g_Girls.HasTrait(girl, "Agile"))				SecLev += 5;	//
	if (g_Girls.HasTrait(girl, "Dominatrix"))			SecLev += 5;	// likes to be in charge
	if (g_Girls.HasTrait(girl, "Giant"))				SecLev += 5;	// Scary
	if (g_Girls.HasTrait(girl, "Goddess"))				SecLev += 5;	// people might behave better around a goddess
	if (g_Girls.HasTrait(girl, "Heroine"))				SecLev += 5;	// likes to protect others
	if (g_Girls.HasTrait(girl, "Muscular"))				SecLev += 5;	// hit harder
	if (g_Girls.HasTrait(girl, "Strong Magic"))			SecLev += 5;	//
	if (g_Girls.HasTrait(girl, "Strange Eyes"))			SecLev += 2;	// I'm watching you	

	// Bad traits
	if (g_Girls.HasTrait(girl, "Blind"))				SecLev -= 50;	// can't see what people are doing
	if (g_Girls.HasTrait(girl, "No Arms"))				SecLev -= 40;	// catch me if you... use your feet?
	if (g_Girls.HasTrait(girl, "No Legs"))				SecLev -= 50;	// run? whats that?
	if (g_Girls.HasTrait(girl, "Deaf"))					SecLev -= 25;	// can't hear girls scream for help
	if (g_Girls.HasTrait(girl, "No Hands"))				SecLev -= 20;	// 
	if (g_Girls.HasTrait(girl, "No Feet"))				SecLev -= 20;	// 
	if (g_Girls.HasTrait(girl, "One Arm"))				SecLev -= 10;	// 
	if (g_Girls.HasTrait(girl, "One Foot"))				SecLev -= 10;	// 
	if (g_Girls.HasTrait(girl, "One Leg"))				SecLev -= 10;	// 
	if (g_Girls.HasTrait(girl, "One Hand"))				SecLev -= 5;	// 
	if (g_Girls.HasTrait(girl, "One Eye"))				SecLev -= 5;	// 

	if (g_Girls.HasTrait(girl, "Broken Will"))			SecLev -= 50;	// I'm too tired to patrol	
	if (g_Girls.HasTrait(girl, "Mind Fucked"))			SecLev -= 50;	// duurrrrrr..... secu.... sec... what? (Mind fucked can be cured btw.)  
	if (g_Girls.HasTrait(girl, "Alcoholic"))			SecLev -= 30;	// 
	if (g_Girls.HasTrait(girl, "Cum Addict"))			SecLev -= 30;	// Be looking for cum instead of doing job
	if (g_Girls.HasTrait(girl, "Retarded"))				SecLev -= 20;	// duurrrrrr..... secu.... sec... what?      
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			SecLev -= 20;	// Wait! The security officer is a nymphomaniac in a brothel?	
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		SecLev -= 15;	// high at work never good
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	SecLev -= 15;	// high at work never good
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	SecLev -= 15;	// high at work never good
	if (g_Girls.HasTrait(girl, "Cum Addict"))			SecLev -= 5;
	if (g_Girls.HasTrait(girl, "Smoker"))				SecLev -= 10;	// less lung power = less running
	if (g_Girls.HasTrait(girl, "Dependant"))			SecLev -= 10;	// I can't do this alone
	if (g_Girls.HasTrait(girl, "Weak Magic"))			SecLev -= 5;	// 
	if (g_Girls.HasTrait(girl, "Lolita"))				SecLev -= 5;	// Hi there kiddo.  Lost your mommy?
	if (g_Girls.HasTrait(girl, "Nerd"))					SecLev -= 5;	// Gets no respect
	if (g_Girls.HasTrait(girl, "Bimbo"))				SecLev -= 5;	// Gets no respect
	if (g_Girls.HasTrait(girl, "Nervous"))				SecLev -= 5;	// Gets no respect
	if (g_Girls.HasTrait(girl, "Twisted"))				SecLev -= 5;	// Wierd ideas about security rarely work
	if (g_Girls.HasTrait(girl, "Meek"))					SecLev -= 5;	// Wait... bad person... come back
	if (g_Girls.HasTrait(girl, "Clumsy"))				SecLev -= 5;	// "Stop thief!" ..... "Ahhhhh! I fell again!"
	if (g_Girls.HasTrait(girl, "Delicate"))				SecLev -= 5;	// Awww, I broke a nail :(
	if (g_Girls.HasTrait(girl, "Old"))					SecLev -= 5;	// Gets no respect
	if (g_Girls.HasTrait(girl, "Plump"))				SecLev -= 5;	// Chubby... chaser?

	return SecLev;
}