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
	stringstream ss; stringstream sstemp;
	int imagetype = IMGTYPE_COMBAT, tex = g_Dice % 5;
	if(Preprocessing(ACTION_WORKSECURITY, girl, brothel, Day0Night1, summary, ss.str()))
		return true;

	sGirl* stripperonduty = NULL;
	string strippername = "the Stripper";
	vector<sGirl *> stripper = g_Brothels.GirlsOnJob(0, JOB_BARSTRIPPER, Day0Night1);
	if (stripper.size() > 0) stripperonduty = stripper[g_Dice%stripper.size()];
	if (stripperonduty) strippername = "Stripper " + stripperonduty->m_Realname + "";
	else strippername = "";

	sGirl* whoreonduty = NULL;
	string whorename = "the Whore";
	vector<sGirl *> whore = g_Brothels.GirlsOnJob(0, JOB_WHOREBROTHEL, Day0Night1);
	if (whore.size() > 0) whoreonduty = whore[g_Dice%whore.size()];
	if (whoreonduty) whorename = "Whore " + whoreonduty->m_Realname + "";
	else whorename = "";

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	// Complications
	if(g_Dice%100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, -1, true);
		ss << gettext("Had to deal with some very unruly patrons that gave her a hard time.");
		girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, +3, true);
		ss << gettext("Successfully handled unruly patrons.");

		// Just 'cause  CRAZY FIXME worked on it 1/12/15 its better but could be more
		if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  /*!g_Girls.HasTrait(girl, "Virgin") &&*/ g_Dice%100 <= 10)
			ss << gettext("\nGave some bonus service to the well behaved patrons.");
		if (tex == 1 && !brothel->m_RestrictHand)
		{
			ss << gettext(" She jerked them off."); imagetype = IMGTYPE_HAND;
		}
		else if (tex == 2 && !brothel->m_RestrictOral)
		{
			ss << gettext(" She sucked them off."); imagetype = IMGTYPE_ORAL;
		}
		else if (tex == 3 && !brothel->m_RestrictTitty)
		{
			ss << gettext(" She used her tits to get them off."); imagetype = IMGTYPE_TITTY;
		}
		else if (tex == 4 && !brothel->m_RestrictFoot)
		{
			ss << gettext(" She used her feet to get them off."); imagetype = IMGTYPE_FOOT;
		}
		else
		{
			ss << gettext(" She flashed them."); imagetype = IMGTYPE_STRIP;
		}
		girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	}

	// First lets decay the previous security level somewhat.  Lets not have too much banking
	// of security points.
	
	// Moving this to cBrothel::UpdateBrothels in so that it only happens once per day instead of everytime a girl works security. -PP
	// brothel->m_SecurityLevel -= 50;

	/* MYR: Modified security level calculation & added traits for it
	        I'm treating the service skill in a more general way here.
            A gang of 1-10 customers attack girls now in function
			work_related_violence.  It takes (# members x 5) security
			points to intercept them. see work_related_violence for
			details. */
			
	int SecLev = g_Dice%(g_Girls.GetSkill(girl, SKILL_COMBAT)/3) 
		+ g_Dice%(g_Girls.GetSkill(girl, SKILL_MAGIC)/3)
		//+ g_Dice%(g_Girls.GetSkill(girl, SKILL_SERVICE)/3); Changed to agility by CRAZY... service makes no sense change it back if you want to
		+ g_Dice%(g_Girls.GetStat(girl, STAT_AGILITY)/3);

	// Please excuse the comments. I wrote this soon after all the sex strings.

	// Good traits
	if (g_Girls.HasTrait(girl, "Psychic"))		SecLev += 10; // I sense danger
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))SecLev += 5; // Moves around quickly
	if (g_Girls.HasTrait(girl, "Charming"))		SecLev += 5; // Gets more cooperation
	if (g_Girls.HasTrait(girl, "Cool Person"))  SecLev += 5; // Gets more cooperation redux
	if (g_Girls.HasTrait(girl, "Adventurer"))	SecLev += 5; // Has experience
	if (g_Girls.HasTrait(girl, "Aggressive"))	SecLev += 5; // Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Yandere"))		SecLev += 5; // Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Tsundere"))		SecLev += 5; // Rawr! I kill you now!
	if (g_Girls.HasTrait(girl, "Sadistic"))		SecLev += 5; // I kill you slowly with this dull knife!
	if (g_Girls.HasTrait(girl, "Merciless"))	SecLev += 5; // Your cries for clemency amuse me
	if (g_Girls.HasTrait(girl, "Fearless"))		SecLev += 5; // Chhhhaaaarrrrrggggeeeeee!!
	if (g_Girls.HasTrait(girl, "Iron Will"))	SecLev += 5; // Hold the line!
	if (g_Girls.HasTrait(girl, "Construct"))	SecLev += 5; // Scary
	if (g_Girls.HasTrait(girl, "Not Human"))	SecLev += 5; // Scary
	if (g_Girls.HasTrait(girl, "Demon"))		SecLev += 10; // Even scarier
	if (g_Girls.HasTrait(girl, "Incorporeal"))	SecLev += 40; // I'm fucking Superman!
	if (g_Girls.HasTrait(girl, "Strange Eyes")) SecLev += 2;// I'm watching you	
	if (g_Girls.HasTrait(girl, "Assassin"))		SecLev += 50; // I was born for this job. I know how you think. Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it (That I'm aware of) so it trades off(?)                   
	if (g_Girls.HasTrait(girl, "Brawler"))		SecLev += 10;  //
	if (g_Girls.HasTrait(girl, "Agile"))		SecLev += 5;  //
	if (g_Girls.HasTrait(girl, "Strong"))		SecLev += 10;  //
	if (g_Girls.HasTrait(girl, "Tough"))		SecLev += 15;  //
	if (g_Girls.HasTrait(girl, "Dominatrix"))	SecLev += 5;  //likes to be in charge
	if (g_Girls.HasTrait(girl, "Giant"))		SecLev += 5; // Scary
	if (g_Girls.HasTrait(girl, "Goddess"))		SecLev += 5; // people might behave better around a goddess
	if (g_Girls.HasTrait(girl, "Heroine"))		SecLev += 5;  //likes to protect others
	if (g_Girls.HasTrait(girl, "Muscular"))		SecLev += 5;  //hit harder
	if (g_Girls.HasTrait(girl, "Powerful Magic"))SecLev += 10;  //
	if (g_Girls.HasTrait(girl, "Strong Magic"))	SecLev += 5;  //

	
	// Bad traits
	if (g_Girls.HasTrait(girl, "Lolita"))		SecLev -= 5; // Hi there kiddo.  Lost your mommy?
	if (g_Girls.HasTrait(girl, "Nerd"))			SecLev -= 5; // Gets no respect
	if (g_Girls.HasTrait(girl, "Bimbo"))		SecLev -= 5; // Gets no respect
	if (g_Girls.HasTrait(girl, "Nervous"))		SecLev -= 5; // Gets no respect
	if (g_Girls.HasTrait(girl, "Twisted"))		SecLev -= 5; // Wierd ideas about security rarely work
	if (g_Girls.HasTrait(girl, "Broken Will"))  SecLev -= 50; // I'm too tired to patrol	
	if (g_Girls.HasTrait(girl, "Nymphomaniac")) SecLev -= 20; // Wait! The security officer is a nymphomaniac in a brothel?	
	if (g_Girls.HasTrait(girl, "Meek"))			SecLev -= 5; // Wait... bad person... come back
	if (g_Girls.HasTrait(girl, "Clumsy"))		SecLev -= 5; // "Stop thief!" ..... "Ahhhhh! I fell again!"
	if (g_Girls.HasTrait(girl, "Dependant"))	SecLev -= 5; // I can't do this alone
	if (g_Girls.HasTrait(girl, "Mind Fucked"))  SecLev -= 50; // duurrrrrr..... secu.... sec... what? (Mind fucked can be cured btw.)  
	if (g_Girls.HasTrait(girl, "Retarded"))		SecLev -= 20; // duurrrrrr..... secu.... sec... what?      
	if (g_Girls.HasTrait(girl, "Alcoholic"))	SecLev -= 30; 
	if (g_Girls.HasTrait(girl, "Blind"))		SecLev -= 50; //cant see what people are doing
	if (g_Girls.HasTrait(girl, "Deaf"))			SecLev -= 25; //cant hear girls scream for help
	if (g_Girls.HasTrait(girl, "Cum Addict"))	SecLev -= 30; //Be looking for cum instead of doing job
	if (g_Girls.HasTrait(girl, "Delicate"))		SecLev -= 5; //
	if (g_Girls.HasTrait(girl, "No Arms"))		SecLev -= 40; 
	if (g_Girls.HasTrait(girl, "No Feet"))		SecLev -= 20; //
	if (g_Girls.HasTrait(girl, "No Legs"))		SecLev -= 50; //
	if (g_Girls.HasTrait(girl, "No Hands"))		SecLev -= 20; //
	if (g_Girls.HasTrait(girl, "Old"))			SecLev -= 5; //Gets no respect
	if (g_Girls.HasTrait(girl, "One Eye"))		SecLev -= 5; // 
	if (g_Girls.HasTrait(girl, "One Arm"))		SecLev -= 10; 
	if (g_Girls.HasTrait(girl, "One Foot"))		SecLev -= 10; //
	if (g_Girls.HasTrait(girl, "One Leg"))		SecLev -= 10; //
	if (g_Girls.HasTrait(girl, "One Hand"))		SecLev -= 5; //
	if (g_Girls.HasTrait(girl, "Plump"))		SecLev -= 5; //
	if (g_Girls.HasTrait(girl, "Smoker"))		SecLev -= 5; //
	if (g_Girls.HasTrait(girl, "Weak Magic"))	SecLev -= 10; //
	if (g_Girls.HasTrait(girl, "Shroud Addict"))SecLev -= 15; // high at work never good
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	SecLev -= 15; // high at work never good
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	SecLev -= 15; // high at work never good


	//pre job randomness
	if (g_Girls.GetStat(girl, STAT_LIBIDO) >= 70 && g_Dice%100 <= 20)
	{
		sstemp << gettext(" Her libido caused her to get distracted while watching ");
		/*might could do more with this FIXME CRAZY*/
		if (g_Girls.HasTrait(girl, "Lesbian"))
		{
			if (stripperonduty)
			{  sstemp << strippername << " dance.\n"; }
			else
			{ sstemp << gettext("one of the strippers dance.\n"); }
		}
		else if (g_Girls.HasTrait(girl, "Bisexual"))
		{
			if (g_Dice.percent(50))
			{
				if (stripperonduty)
				{  sstemp << strippername << " dance.\n"; }
				else
				{ sstemp << gettext("one of the strippers dance.\n"); }
			}
		else
			{
				if (whoreonduty)
				{ sstemp << whorename << " with a client.\n"; }
				else
				{ sstemp << gettext("one of the whores with a client.\n"); }
			}
		}
		else
		{
			if (whoreonduty)
				{ sstemp << whorename << " with a client.\n"; }
			else
				{ sstemp << gettext("one of the whores with a client.\n"); }
		}
		SecLev -= 20;
	}

	if (SecLev < 10)
		SecLev = 10;

	brothel->m_SecurityLevel += SecLev;

    sstemp << "\nPatrolling the building. Security level up by " << SecLev << ".";
	girl->m_Events.AddMessage(sstemp.str(), imagetype, Day0Night1);

	// Improve girl
	int xp = 15, libido = 1, skill = 2, magic = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; magic += 1; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	girl->m_Pay += 70;
	g_Gold.staff_wages(70);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);  //fixed by CRAZY skill and libido was backwards
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, magic);  //added by CRAZY
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	// Copy-pasta from WorkExploreCatacombs
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 15, ACTION_WORKSECURITY, gettext("She has become pretty Tough from all of the fights she's been in."), Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 45, ACTION_WORKSECURITY, gettext("She has been in enough tough spots to consider herself Adventurer."), Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_WORKSECURITY, gettext("She is getting rather Aggressive from her enjoyment of combat."), Day0Night1 == SHIFT_NIGHT);
	return false;
}
