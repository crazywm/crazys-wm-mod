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
extern cGold g_Gold;

bool cJobManager::WorkNurse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;


	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		stringstream ss;
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " <<
			girl->m_Realname << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return true;
	}

	if (Preprocessing(ACTION_WORKNURSE, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the patients!
	g_Girls.UnequipCombat(girl);



	int hand = false;
	int sex = false;
	int wages = 25;
	message += "She worked as a nurse.";

	int roll = g_Dice % 100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetStat(girl, STAT_CHARISMA) / 2 +
		g_Girls.GetSkill(girl, SKILL_MEDICINE) +
		g_Girls.GetStat(girl, STAT_LEVEL) / 5);

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Nerd"))				jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Retarded"))			jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;


	if (jobperformance >= 245)
	{
		message += " She must be the perfect nurse patients go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			message += "Just the sight of " + girlName + " brings happiness to the patients. She is a blessing to the entire ward.\n";
		}
		else if (roll <= 40)
		{
			message += "Today the knowledge and swift reaction from " + girlName + " saved a patient�s life!\n";
		}
		else if (roll <= 60)
		{
			message += "She was doing her routine with a wide, beautiful smile and humming a pleasant tune.\n";
		}
		else if (roll <= 80)
		{
			message += "Her work is her life. She feels fulfillment from every day working in the clinic.\n";
		}
		else
		{
			message += "She spent her day supervising new nurses, showing them how to do the work properly and answering tons of questions.\n";
		}
	}
	else if (jobperformance >= 185)
	{
		message += " She's unbelievable at this and is always getting praised by the patients for her work.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			message += girlName + " is good at her work. Besides helping patients, she gave some pointers to younger, less experience co-nurses.\n";
		}
		else if (roll <= 40)
		{
			message += girlName + " was on her feet all day long. She was really tired but also felt fulfilled.\n";
		}
		else if (roll <= 60)
		{
			message += "She managed to calm a patient before a major surgery.\n";
		}
		else if (roll <= 80)
		{
			message += "When assisting in a surgery, she knew what tool will be needed, before the surgeon called for them.\n";
		}
		else
		{
			message += girlName + " is really committed to her job as a nurse. She�s starting to feel enjoyment after every hard worked day.\n";
		}
	}
	else if (jobperformance >= 135)
	{
		message += " She's good at this job and gets praised by the patients often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			message += "She made one of the patients heart beat dangerously high, while leaning over his face to rearrange the pillow.\n";
		}
		else if (roll <= 40)
		{
			message += girlName + " made her rounds and spend the rest of the day chatting with other staff members.\n";
		}
		else if (roll <= 60)
		{
			message += "When giving medicine to the patient, " + girlName + " noticed that the dosage for one of them was wrong. She corrected her mistake immediately.\n";
		}
		else if (roll <= 80)
		{
			message += "Today she was assigned to the surgery room. She learnt something new.\n";
		}
		else
		{
			message += "It was a busy day for " + girlName + ", but she manage to get thru it without any slipups.\n";
		}
	}
	else if (jobperformance >= 85)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			message += "She accidently mistaken the dosage of the drugs! Fortunately she understated them.\n";
		}
		else if (roll <= 40)
		{
			message += girlName + " made a mistake while changing bandages of one of her patients. The head nurse scolded her.\n";
		}
		else if (roll <= 60)
		{
			message += "She spent her shift fallowing the doctor and learning more about the job.\n";
		}
		else if (roll <= 80)
		{
			message += "She slapped an older patient for groping her ass! Good thing that the doctor was near to help him stay alive!\n";
		}
		else
		{
			message += "When giving a sponge bath to one of male patients she accidently aroused his manhood. Ashamed from the sight, she run out the room!\n";
		}
	}
	else if (jobperformance >= 65)
	{
		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			message += "When prepping her patient before a mayor surgery she described all possible bad outcomes forgetting to mentioning the low risk percentage and good sides of this treatment.\n";
		}
		else if (roll <= 40)
		{
			message += girlName + " took the temperature of all patients in her block with the same thermometer without sterilizing it!\n";
		}
		else if (roll <= 60)
		{
			message += "She got scolded today by the doctor for laughing at the patients weight that came here for liposuction.\n";
		}
		else if (roll <= 80)
		{
			message += "Being responsible for utilizing today�s medical waste, " + girlName + " just flashed them in the toilet.\n";
		}
		else
		{
			message += "She spend most of her shift snoozing in the basement medical supply storage.\n";
		}
	}
	else
	{
		message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			message += "Some could say that her technique of drawing blood is the same way people play pin the tail on the donkey.\n";
		}
		else if (roll <= 40)
		{
			message += "Sponge bath and skinning someone alive. No difference for " + girlName + ".\n";
		}
		else if (roll <= 60)
		{
			message += girlName + " covered her mouth while sneezing several times. After that she went to assist in the ongoing surgery without changing her gloves!\n";
		}
		else if (roll <= 80)
		{
			message += "While assisting the doctor, she giggled when the patient was telling about its health problems.\n";
		}
		else
		{
			message += girlName + " fainted on her first sight of blood. She wasn�t helpful today.\n";
		}
	}

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85)
	{
		if (g_Dice % 100 <= 20)
		{
			message += "Stunned by her beauty a patient left her a great tip.\n\n";
			wages += 25;
		}
	}
	if (g_Girls.HasTrait(girl, "Clumsy"))
	{
		if (g_Dice % 100 <= 15)
		{
			message += "Her clumsy nature caused her to spill some medicine everywhere.\n";
			wages -= 15;
		}
	}
	if (g_Girls.HasTrait(girl, "Pessimist"))
	{
		if (g_Dice % 100 <= 5)
		{
			if (jobperformance < 125)
			{
				message += "Her pessimistic mood depressed the patients making them tip less.\n";
				wages -= 10;
			}
			else
			{
				message += girl->m_Realname + " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
				wages += 10;
			}
		}
	}
	if (g_Girls.HasTrait(girl, "Optimist"))
	{
		if (g_Dice % 100 <= 5)
		{
			if (jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patients thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
				message += "Her optimistic mood made patients cheer up increasing the amount they tip.\n";
				wages += 10;
			}
		}
	}
	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 55)
	{
		if (g_Dice % 100 <= 30)
		{
			message += "An elderly fellow managed to convince " + girlName + " that her touch can heal! She ended up giving him a hand job!\n\n";
			hand = true;
		}
	}
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
	{
		if (g_Dice % 100 <= 30)
		{
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 65)
			{
				message += "When giving a sponge bath to one of her male patients she couldn�t look away from his enormous manhood. The man took advantage and fuck her brains out!\n";
				wages += 50;
				sex = true;
			}
			else
			{
				message += "When giving a sponge bath to one of her male patients she couldn�t look away from his enormous manhood. But she wasn't in the mood so she left.\n";
			}
		}
	}

	//enjoyed the work or not
	if (roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, -1, true);
	}
	else if (roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +1, true);
	}


	if (sex)
	{
		if (roll <= 50)
		{
			girl->m_Events.AddMessage(message, IMGTYPE_SEX, DayNight);
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
		}
		else
		{
			girl->m_Events.AddMessage(message, IMGTYPE_ANAL, DayNight);
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
		}
		brothel->m_Happiness += 100;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE);
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
	}
	else if (hand)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 25 + g_Dice%roll_max;
		g_Girls.UpdateSkill(girl, SKILL_HANDJOB, 2);
		girl->m_Events.AddMessage(message, IMGTYPE_HAND, DayNight);
	}
	else
	{
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}

	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetSkill(girl, SKILL_MEDICINE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;

	int patients = g_Clinic.GetNumGirlsOnJob(0, JOB_GETHEALING, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_GETABORT, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_PHYSICALSURGERY, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_LIPO, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BREASTREDUCTION, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BOOBJOB, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_VAGINAREJUV, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_FACELIFT, DayNight) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_ASSJOB, DayNight);


	wages += 5 * patients;		// `J` pay her 5 for each patient you send to her		

	if (wages < 0)	wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 5 + patients * 2, libido = 1, skill = 2 + patients / 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	if (g_Girls.HasTrait(girl, "Lesbian"))
		libido += patients / 2;

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else				g_Girls.UpdateStat(girl, STAT_CHARISMA, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKNURSE, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKNURSE, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);

	return false;
}