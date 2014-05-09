/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

bool cJobManager::WorkSecurity(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_WORKSECURITY, girl, brothel, DayNight, summary, message))
		return true;

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	// Complications
	if(g_Dice%100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, -1, true);
		message = gettext("Had to deal with some very unruly patrons that gave her a hard time.");
		girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, DayNight);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, +3, true);
		message = gettext("Successfully handled unruly patrons.");
		girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, DayNight);

		// Just 'cause  CRAZY FIXME this seem to do nothing
		if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Dice%100 <= 50)
			message += gettext("\nGave some bonus service to the well behaved patrons.");
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
		+ g_Dice%(g_Girls.GetSkill(girl, SKILL_SERVICE)/3);

	// Please excuse the comments. I wrote this soon after all the sex strings.

	// Good traits
	if (g_Girls.HasTrait(girl, "Psychic"))  // I sense danger
		SecLev += 10;

	if (g_Girls.HasTrait(girl, "Fleet of Foot"))  // Moves around quickly
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Charming"))  // Gets more cooperation
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Cool Person"))  // Gets more cooperation redux
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Adventurer"))  // Has experience
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Aggressive"))  // Rawr! I kill you now!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Yandere"))  // Rawr! I kill you now!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Tsundere"))  // Rawr! I kill you now!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Sadistic"))  // I kill you slowly with this dull knife!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Merciless"))  // Your cries for clemency amuse me
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Fearless"))  // Chhhhaaaarrrrrggggeeeeee!!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Iron Will"))  // Hold the line!
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Construct") || g_Girls.HasTrait(girl, "Not Human"))  // Scary
		SecLev += 5;

	if (g_Girls.HasTrait(girl, "Demon"))  // Even scarier
		SecLev += 10;

	if (g_Girls.HasTrait(girl, "Incorporeal"))  // I'm fucking Superman!
		SecLev += 40;

	if (g_Girls.HasTrait(girl, "Strange Eyes"))  // I'm watching you
		SecLev += 2;

	if (g_Girls.HasTrait(girl, "Assassin"))  // I was born for this job. I know how you think.
		SecLev += 50;    // Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it
	                     // (That I'm aware of) so it trades off(?)

	
	// Bad traits
	if (g_Girls.HasTrait(girl, "Lolita"))  // Hi there kiddo.  Lost your mommy?  OHMYGOD! She has two Swords of Forever!  CRAZY don't think this is right changing it to a bad trait
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Nerd"))  // Gets no respect
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Tsundere"))  // Puts people off
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Twisted"))  // Wierd ideas about security rarely work
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Broken Will"))  // I'm too tired to patrol
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))  // Wait! The security officer is a nymphomaniac in a brothel?
		SecLev -= 20;      // This hurts people like me who use the automation functions.

	if (g_Girls.HasTrait(girl, "Meek"))  // Wait... bad person... come back
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Clumsy"))  // "Stop thief!" ..... "Ahhhhh! I fell again!"
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Dependant"))  // I can't do this alone
		SecLev -= 5;

	if (g_Girls.HasTrait(girl, "Mind Fucked"))  // duurrrrrr..... secu.... sec... what?
		SecLev -= 50;                           // (Mind fucked can be cured btw.)

	if (SecLev < 10)
		SecLev = 10;

	brothel->m_SecurityLevel += SecLev;

	stringstream sstemp;
    sstemp << "Patrolling the building. Security level up by " << SecLev << ".";
	girl->m_Events.AddMessage(sstemp.str(), IMGTYPE_COMBAT, DayNight);

	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 5;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 5;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	girl->m_Pay += 70;
	g_Gold.staff_wages(70);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);  //fixed by CRAZY skill and libido was backwards
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	// Copy-pasta from WorkExploreCatacombs
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 15, ACTION_WORKSECURITY,  gettext("She has become pretty Tough from all of the fights she's been in."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 45, ACTION_WORKSECURITY,  gettext("She has been in enough tough spots to consider herself Adventurer."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_WORKSECURITY,  gettext("She is getting rather Aggressive from her enjoyment of combat."), DayNight != 0);
	return false;
}
