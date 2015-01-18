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
extern cGold g_Gold;

// `J` Brothel Job - General
bool cJobManager::WorkSecurity(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	stringstream ss; string girlName = girl->m_Realname;
	cConfig cfg;

	if (Preprocessing(ACTION_WORKSECURITY, girl, brothel, Day0Night1, summary, ss.str()))
		return true;


	int enjoy = 0;
	int wages = 0;
	int tips = 0;
	int jobperformance = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	int imagetype = IMGTYPE_COMBAT;


	vector<sGirl *> strippers = g_Brothels.GirlsOnJob(brothel->m_id, JOB_BARSTRIPPER, Day0Night1);
	sGirl* stripperonduty = (strippers.size() > 0 ? strippers[g_Dice%strippers.size()] : 0);
	string strippername = (stripperonduty ? "Stripper " + stripperonduty->m_Realname + "" : "the Stripper");
	
	vector<sGirl *> whores = g_Brothels.GirlsOnJob(brothel->m_id, JOB_WHOREBROTHEL, Day0Night1);
	sGirl* whoreonduty = (whores.size() > 0 ? whores[g_Dice%whores.size()] : 0);
	string whorename = (whoreonduty ? "Whore " + whoreonduty->m_Realname + "" : "the Whore");

	ss << girlName << " worked Security in the brothel.\n\n";

	/* MYR: Modified security level calculation & added traits for it
	        I'm treating the service skill in a more general way here.
            A gang of 1-10 customers attack girls now in function
			work_related_violence.  It takes (# members x 5) security
			points to intercept them. see work_related_violence for
			details. */
			
	int SecLev = g_Dice%(g_Girls.GetSkill(girl, SKILL_COMBAT)/3) 
		+ g_Dice%(g_Girls.GetSkill(girl, SKILL_MAGIC)/3)
		+ g_Dice%(g_Girls.GetStat(girl, STAT_AGILITY)/3);

	// Please excuse the comments. I wrote this soon after all the sex strings.

	// Good traits
	if (g_Girls.HasTrait(girl, "Incorporeal"))			SecLev += 100;	// I'm fucking Superman!
	if (g_Girls.HasTrait(girl, "Assassin"))				SecLev += 50;	// I was born for this job. I know how you think. Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it (That I'm aware of) so it trades off(?)                   
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
	if (g_Girls.HasTrait(girl, "Delicate"))				SecLev -= 5;	// Awww, I broke a nial :(
	if (g_Girls.HasTrait(girl, "Old"))					SecLev -= 5;	// Gets no respect
	if (g_Girls.HasTrait(girl, "Plump"))				SecLev -= 5;	// Chubby... chaser?


	// Complications
	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		SecLev -= SecLev / 10;
		ss << "She had to deal with some very unruly patrons that gave her a hard time.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		SecLev += SecLev / 10;
		ss << gettext("She successfully handled unruly patrons.");
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "She had an uneventful day watching over the brothel.";
	}
	ss << "\n\n";

	if (g_Girls.GetStat(girl, STAT_LIBIDO) >= 70 && g_Dice.percent(20))
	{
		int choice = g_Dice % 2;
		ss << "Her libido caused her to get distracted while watching ";
		/*might could do more with this FIXME CRAZY*/
		if (g_Girls.HasTrait(girl, "Lesbian"))	choice = 0;
		if (g_Girls.HasTrait(girl, "Straight"))	choice = 1;
		switch (choice)
		{
		case 0:
			ss << (stripperonduty ? strippername : "one of the strippers") << " dance.\n";
			break;
		case 1:
		default:
			ss << (whoreonduty ? whorename : "one of the whores") << " with a client.\n";
			break;
		}
		SecLev -= 20;
		ss << "\n\n";

	}

	if (g_Dice.percent(5) || (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Dice.percent(20)))
	{
		int  tex = g_Dice % 5;
		ss << gettext("\nGave some bonus service to the well behaved patrons, ");
		if (tex <= 1 && !brothel->m_RestrictHand)
		{
			ss << gettext(" She jerked them off.");
			imagetype = IMGTYPE_HAND;
		}
		else if (tex <= 2 && !brothel->m_RestrictOral)
		{
			ss << gettext(" She sucked them off.");
			imagetype = IMGTYPE_ORAL;
		}
		else if (tex <= 3 && !brothel->m_RestrictTitty)
		{
			ss << gettext(" She used her tits to get them off.");
			imagetype = IMGTYPE_TITTY;
		}
		else if (tex <= 4 && !brothel->m_RestrictFoot)
		{
			ss << gettext(" She used her feet to get them off.");
			imagetype = IMGTYPE_FOOT;
		}
		else
		{
			ss << gettext(" She flashed them.");
			imagetype = IMGTYPE_STRIP;
		}
		ss << "\n\n";
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -10);
	}

	if (SecLev < 10) SecLev = 10;
	brothel->m_SecurityLevel += SecLev;

	ss << "\nPatrolling the building, " << girlName <<" increased the security level by " << SecLev << ".";
	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);

	// Improve girl
	int xp = 15, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	girl->m_Pay += 70;
	g_Gold.staff_wages(70);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT,	(g_Dice % skill) + 1);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC,	(g_Dice % skill) + 1);
	g_Girls.UpdateSkill(girl, STAT_AGILITY,	(g_Dice % skill) + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, enjoy, true);

	// Copy-pasta from WorkExploreCatacombs
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 15, ACTION_WORKSECURITY, gettext("She has become pretty Tough from all of the fights she's been in."), Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 45, ACTION_WORKSECURITY, gettext("She has been in enough tough spots to consider herself Adventurer."), Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_WORKSECURITY, gettext("She is getting rather Aggressive from her enjoyment of combat."), Day0Night1 == SHIFT_NIGHT);
	return false;
}
