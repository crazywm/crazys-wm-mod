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
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Clinic Job - Staff
bool cJobManager::WorkDoctor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		stringstream ss;
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " <<
			girl->m_Realname << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return true;
	}

	string message = "";

	g_Girls.AddTiredness(girl);

	// put that shit away, you'll scare off the patients!
	g_Girls.UnequipCombat(girl);

	//	if(DayNight) // Doctor is a full time job now
	girl->m_DayJob = girl->m_NightJob = JOB_DOCTOR;

	girl->m_Pay += 100;
	message += gettext("She worked as a Doctor.");
	
	int roll = g_Dice%100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) +
		g_Girls.GetSkill(girl, SKILL_MEDICINE) + 
		g_Girls.GetStat(girl, STAT_LEVEL)/5);

	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Nerd"))				jobperformance += 30;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 20;	// Don't have to ask "Where does it hurt?"

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Retarded"))			jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;

	/*
JOB_CHAIRMAN      
JOB_DOCTOR        
JOB_NURSE         
JOB_MECHANIC      
JOB_JANITOR       
JOB_CLINICREST    

*/

	int patients = g_Clinic.GetNumGirlsOnJob(0, JOB_GETHEALING, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_GETABORT, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_PHYSICALSURGERY, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_LIPO, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BREASTREDUCTION, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BOOBJOB, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_VAGINAREJUV, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_FACELIFT, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_ASSJOB, DayNight);




	if(roll <= 25) {
		message += gettext(" She had a pleasant time working.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, +1, true);
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.");
	}

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;

	// Improve stats
	int xp = 10 + (patients * 2), libido = 1, skill = 1 + (patients / 2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	if (g_Girls.HasTrait(girl, "Lesbian"))
		libido += patients / 2;

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, skill);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	return false;
}
