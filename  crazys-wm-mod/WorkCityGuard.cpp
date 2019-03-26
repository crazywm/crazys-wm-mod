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
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " helps guard the city.\n \n";

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100();
	int wages = 150, tips = 0, enjoy = 0, enjoyc = 0, sus = 0;
	int imagetype = IMGTYPE_PROFILE;

	int agl = (girl->agility() / 2 + g_Dice % (girl->combat() / 2));


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
			ss << "There were no criminals around for her to fight.\n \n";
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
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve girl
	int xp = 8, libido = 2, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->combat(g_Dice % skill);
	girl->magic(g_Dice % skill);
	girl->agility(g_Dice % skill);
	girl->constitution(g_Dice % skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);


	girl->upd_Enjoyment(actiontype, enjoy);
	girl->upd_Enjoyment(ACTION_COMBAT, enjoyc);
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
		SecLev = (girl->combat())
			+ (girl->magic() / 2)
			+ (girl->agility() / 2);
	}
	else			// for the actual check
	{
		SecLev = g_Dice % (girl->combat())
			+ g_Dice % (girl->magic() / 3)
			+ g_Dice % (girl->agility() / 3);

		int t = girl->tiredness() - 80;
		if (t > 0)
			SecLev -= t * 2;
	}

	// Good traits
	if (girl->has_trait( "Incorporeal"))			SecLev += 100;	// I'm fucking Superman!
	if (girl->has_trait( "Assassin"))				SecLev += 50;	// I was born for this job. I know how you think. Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it (That I'm aware of) so it trades off(?)
	if (girl->has_trait( "Zombie"))				SecLev += 20;	// You have what guarding the city?!?! I'm out of here.
	if (girl->has_trait( "Skeleton"))				SecLev += 10;	// You have what guarding the city?!?! I'm out of here.
	if (girl->has_trait( "Tough"))				SecLev += 15;	// You hit like a girl
	if (girl->has_trait( "Powerful Magic"))		SecLev += 10;	//
	if (girl->has_trait( "Psychic"))				SecLev += 10;	// I sense danger
	if (girl->has_trait( "Demon"))				SecLev += 10;	// Even scarier
	if (girl->has_trait( "Brawler"))				SecLev += 10;	//
	if (girl->has_trait( "Strong"))				SecLev += 10;	//
	if (girl->has_trait( "Fleet of Foot"))		SecLev += 5;	// Moves around quickly
	if (girl->has_trait( "Charming"))				SecLev += 5;	// Gets more cooperation
	if (girl->has_trait( "Cool Person"))			SecLev += 5;	// Gets more cooperation redux
	if (girl->has_trait( "Adventurer"))			SecLev += 5;	// Has experience
	if (girl->has_trait( "Aggressive"))			SecLev += 5;	// Rawr! I kill you now!
	if (girl->has_trait( "Yandere"))				SecLev += 5;	// Rawr! I kill you now!
	if (girl->has_trait( "Tsundere"))				SecLev += 5;	// Rawr! I kill you now!
	if (girl->has_trait( "Sadistic"))				SecLev += 5;	// I kill you slowly with this dull knife!
	if (girl->has_trait( "Merciless"))			SecLev += 5;	// Your cries for clemency amuse me
	if (girl->has_trait( "Fearless"))				SecLev += 5;	// Chhhhaaaarrrrrggggeeeeee!!
	if (girl->has_trait( "Iron Will"))			SecLev += 5;	// Hold the line!
	if (girl->has_trait( "Construct"))			SecLev += 5;	// Scary
	if (girl->has_trait( "Not Human"))			SecLev += 5;	// Scary
	if (girl->has_trait( "Agile"))				SecLev += 5;	//
	if (girl->has_trait( "Dominatrix"))			SecLev += 5;	// likes to be in charge
	if (girl->has_trait( "Giant"))				SecLev += 5;	// Scary
	if (girl->has_trait( "Goddess"))				SecLev += 5;	// people might behave better around a goddess
	if (girl->has_trait( "Heroine"))				SecLev += 5;	// likes to protect others
	if (girl->has_trait( "Muscular"))				SecLev += 5;	// hit harder
	if (girl->has_trait( "Strong Magic"))			SecLev += 5;	//
	if (girl->has_trait( "Strange Eyes"))			SecLev += 2;	// I'm watching you

	// Bad traits
	if (girl->has_trait( "Blind"))				SecLev -= 50;	// can't see what people are doing
	if (girl->has_trait( "No Arms"))				SecLev -= 40;	// catch me if you... use your feet?
	if (girl->has_trait( "No Legs"))				SecLev -= 50;	// run? whats that?
	if (girl->has_trait( "Deaf"))					SecLev -= 25;	// can't hear girls scream for help
	if (girl->has_trait( "No Hands"))				SecLev -= 20;	//
	if (girl->has_trait( "No Feet"))				SecLev -= 20;	//
	if (girl->has_trait( "One Arm"))				SecLev -= 10;	//
	if (girl->has_trait( "One Foot"))				SecLev -= 10;	//
	if (girl->has_trait( "One Leg"))				SecLev -= 10;	//
	if (girl->has_trait( "One Hand"))				SecLev -= 5;	//
	if (girl->has_trait( "One Eye"))				SecLev -= 5;	//

	if (girl->has_trait( "Broken Will"))			SecLev -= 50;	// I'm too tired to patrol
	if (girl->has_trait( "Mind Fucked"))			SecLev -= 50;	// duurrrrrr..... secu.... sec... what? (Mind fucked can be cured btw.)
	if (girl->has_trait( "Alcoholic"))			SecLev -= 30;	//
	if (girl->has_trait( "Cum Addict"))			SecLev -= 30;	// Be looking for cum instead of doing job
	if (girl->has_trait( "Retarded"))				SecLev -= 20;	// duurrrrrr..... secu.... sec... what?
	if (girl->has_trait( "Nymphomaniac"))			SecLev -= 20;	// Wait! The security officer is a nymphomaniac in a brothel?
	if (girl->has_trait( "Shroud Addict"))		SecLev -= 15;	// high at work never good
	if (girl->has_trait( "Fairy Dust Addict"))	SecLev -= 15;	// high at work never good
	if (girl->has_trait( "Viras Blood Addict"))	SecLev -= 15;	// high at work never good
	if (girl->has_trait( "Cum Addict"))			SecLev -= 5;
	if (girl->has_trait( "Smoker"))				SecLev -= 10;	// less lung power = less running
	if (girl->has_trait( "Dependant"))			SecLev -= 10;	// I can't do this alone
	if (girl->has_trait( "Weak Magic"))			SecLev -= 5;	//
	if (girl->has_trait( "Lolita"))				SecLev -= 5;	// Hi there kiddo.  Lost your mommy?
	if (girl->has_trait( "Nerd"))					SecLev -= 5;	// Gets no respect
	if (girl->has_trait( "Bimbo"))				SecLev -= 5;	// Gets no respect
	if (girl->has_trait( "Nervous"))				SecLev -= 5;	// Gets no respect
	if (girl->has_trait( "Twisted"))				SecLev -= 5;	// Wierd ideas about security rarely work
	if (girl->has_trait( "Meek"))					SecLev -= 5;	// Wait... bad person... come back
	if (girl->has_trait( "Clumsy"))				SecLev -= 5;	// "Stop thief!" ..... "Ahhhhh! I fell again!"
	if (girl->has_trait( "Delicate"))				SecLev -= 5;	// Awww, I broke a nail :(
	if (girl->has_trait( "Old"))					SecLev -= 5;	// Gets no respect
	if (girl->has_trait( "Plump"))				SecLev -= 5;	// Chubby... chaser?

	return SecLev;
}
