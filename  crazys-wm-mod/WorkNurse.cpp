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
extern int g_Building;

// `J` Clinic Job - Staff
bool cJobManager::WorkNurse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname;
	g_Building = BUILDING_CLINIC;

	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " << girlName << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return false;
	}
	if (Preprocessing(ACTION_WORKNURSE, girl, brothel, Day0Night1, summary, ss.str())) return true;

	cConfig cfg;
	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the patients!

	int hand = false, sex = false, les = false;
	int wages = 25, tips = 0, enjoy = 0;
	int imagetype = IMGTYPE_NURSE;

	// this will be added to the clinic's code eventually - for now it is just used for her pay
	int patients = 0;			// `J` how many patients the Doctor can see in a shift

	ss << "She worked as a nurse.";

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

	//Adding cust here for use in scripts...
	sCustomer cust;
	GetMiscCustomer(brothel, cust);


	if (jobperformance >= 245)
	{
		ss << " She must be the perfect nurse, patients go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			ss << "Just the sight of " + girlName + " brings happiness to the patients. She is a blessing to the entire ward.\n";
		}
		else if (roll <= 40)
		{
			ss << "Today the knowledge and swift reaction from " + girlName + " saved a patient's life!\n";
		}
		else if (roll <= 60)
		{
			ss << "She was doing her routine with a wide, beautiful smile and humming a pleasant tune.\n";
		}
		else if (roll <= 80)
		{
			ss << "Her work is her life. She feels fulfillment from every day working in the clinic.\n";
		}
		else
		{
			ss << "She spent her day supervising new nurses, showing them how to do the work properly and answering tons of questions.\n";
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the patients for her work.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			ss << girlName + " is good at her work. Besides helping patients, she gave some pointers to younger, less experience co-nurses.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName + " was on her feet all day long. She was really tired but also felt fulfilled.\n";
		}
		else if (roll <= 60)
		{
			ss << "She managed to calm a patient before a major surgery.\n";
		}
		else if (roll <= 80)
		{
			ss << "When assisting in a surgery, she knew what tool will be needed, before the surgeon called for them.\n";
		}
		else
		{
			ss << girlName + " is really committed to her job as a nurse. She's starting to feel enjoyment after every hard worked day.\n";
		}
	}
	else if (jobperformance >= 135)
	{
		ss << " She's good at this job and gets praised by the patients often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << "She made one of the patients heart beat dangerously high, while leaning over his face to rearrange the pillow.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName + " made her rounds and spend the rest of the day chatting with other staff members.\n";
		}
		else if (roll <= 60)
		{
			ss << "When giving medicine to the patient, " + girlName + " noticed that the dosage for one of them was wrong. She corrected her mistake immediately.\n";
		}
		else if (roll <= 80)
		{
			ss << "Today she was assigned to the surgery room. She learnt something new.\n";
		}
		else
		{
			ss << "It was a busy day for " + girlName + ", but she manage to get thru it without any slipups.\n";
		}
	}
	else if (jobperformance >= 85)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			ss << "She accidently mistaken the dosage of the drugs! Fortunately she understated them.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName + " made a mistake while changing bandages of one of her patients. The head nurse scolded her.\n";
		}
		else if (roll <= 60)
		{
			ss << "She spent her shift fallowing the doctor and learning more about the job.\n";
		}
		else if (roll <= 80)
		{
			ss << "She slapped an older patient for groping her ass! Good thing that the doctor was near to help him stay alive!\n";
		}
		else
		{
			ss << "When giving a sponge bath to one of male patients she accidently aroused his manhood. Ashamed from the sight, she run out the room!\n";
		}
	}
	else if (jobperformance >= 65)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			ss << "When prepping her patient before a mayor surgery she described all possible bad outcomes forgetting to mentioning the low risk percentage and good sides of this treatment.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName + " took the temperature of all patients in her block with the same thermometer without sterilizing it!\n";
		}
		else if (roll <= 60)
		{
			ss << "She got scolded today by the doctor for laughing at the patients weight that came here for liposuction.\n";
		}
		else if (roll <= 80)
		{
			ss << "Being responsible for utilizing today's medical waste, " + girlName + " just flashed them in the toilet.\n";
		}
		else
		{
			ss << "She spend most of her shift snoozing in the basement medical supply storage.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			ss << "Some could say that her technique of drawing blood is the same way people play pin the tail on the donkey.\n";
		}
		else if (roll <= 40)
		{
			ss << "Sponge bath and skinning someone alive. No difference for " + girlName + ".\n";
		}
		else if (roll <= 60)
		{
			ss << girlName + " covered her mouth while sneezing several times. After that she went to assist in the ongoing surgery without changing her gloves!\n";
		}
		else if (roll <= 80)
		{
			ss << "While assisting the doctor, she giggled when the patient was telling about its health problems.\n";
		}
		else
		{
			ss << girlName + " fainted on her first sight of blood. She wasn't helpful today.\n";
		}
	}

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		tips += 25;
		ss << "Stunned by her beauty a customer left her a great tip.\n\n";
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(20))
	{
		wages -= 15;
		ss << "Her clumsy nature caused her to spill some medicine everywhere.\n";
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << "Her pessimistic mood depressed the patients making them tip less.\n";
		}
		else
		{
			tips += 10;
			ss << girlName + " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << girlName + " was in a cheerful mood but the patients thought she needed to work more on her services.\n";
		}
		else
		{
			tips += 10;
			ss << "Her optimistic mood made patients cheer up increasing the amount they tip.\n";
		}
	}

	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 45 && g_Dice.percent(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
	{
		hand = true;
		ss << "An elderly fellow managed to convince " + girlName + " that her touch can heal! She ended up giving him a hand job!\n"; 
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") && !g_Girls.HasTrait(girl, "Virgin")
		&& !g_Girls.HasTrait(girl, "Lesbian") && g_Dice.percent(30))
	{
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 65 && (!brothel->m_RestrictNormal || !brothel->m_RestrictAnal))
		{
			tips += 50; 
			sex = true;
			enjoy += 1;
			ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. The man took advantage and fucked her brains out!\n";
		}
		else
		{ ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. But she wasn't in the mood so she left.\n"; }
	}

	if (g_Girls.HasTrait(girl, "Lesbian") && g_Girls.HasTrait(girl, "Aggressive") && 
		g_Girls.GetStat(girl, STAT_LIBIDO) > 65 && g_Dice.percent(10))
	{ 
		les = true;
		enjoy += 1;
		ss << "When giving a sponge bath to one of her female patients she couldn't help herself and took advantage of the situation.\n";
	}


	//enjoyed the work or not
	if (roll <= 5)
	{
		enjoy -= g_Dice % 3 + 1;
		jobperformance = int(jobperformance * 0.9);
		ss << "\nSome of the patrons abused her during the shift.";
	}
	else if (roll <= 25) 
	{
		enjoy += g_Dice % 3 + 1;
		jobperformance = int(jobperformance * 1.1);
		ss << "\nShe had a pleasant time working.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "\nOtherwise, the shift passed uneventfully.";
	}

	if (sex)
	{
		if (roll <= 50 && !brothel->m_RestrictNormal) 
		{ 
			imagetype = IMGTYPE_SEX; g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2); 
			if (!girl->calc_pregnancy(&cust,  false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
			}
		}
		else if (!brothel->m_RestrictAnal) /*      */	{ imagetype = IMGTYPE_ANAL; g_Girls.UpdateSkill(girl, SKILL_ANAL, 2); }
		brothel->m_Happiness += 100;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
	}
	else if (hand)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		g_Girls.UpdateSkill(girl, SKILL_HANDJOB, 2);
		imagetype = IMGTYPE_HAND;
	}
	else if (les)
	{
		brothel->m_Happiness += (g_Dice % 70) + 30;
		imagetype = IMGTYPE_LESBIAN;
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 2); 
	}

	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance = int(jobperformance * 0.9);
		patients += jobperformance / 5;		// `J` 1 patient per 5 point of performance
		wages = 0;
	}
	else
	{
		patients += jobperformance / 5;		// `J` 1 patient per 5 point of performance
		wages += patients*2;				// `J` pay her 2 for each patient you send to her
	}

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	if (wages < 0)	wages = 0;
	g_Clinic.m_Nurse_Patient_Time += patients;
	girl->m_Tips = tips;
	girl->m_Pay = wages;

	/* `J` this will be a place holder until a better payment system gets done
	*  this does not take into account any of your girls in surgery
	*/
	int earned = 0;
	for (int i = 0; i < patients; i++)
	{
		earned += g_Dice % 45 + 5; // 5-50 gold per customer
	}
	brothel->m_Finance.clinic_income(earned);
	ss.str("");
	ss << girlName << " earned " << earned << " gold from taking care of " << patients << " patients.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);


	// Improve stats
	int xp = 10 + patients/2, libido = 1, skill = 2 + patients / 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }
	if (g_Girls.HasTrait(girl, "Lesbian"))				{ libido += patients / 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else				g_Girls.UpdateStat(girl, STAT_CHARISMA, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, enjoy, true);
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKNURSE, "Dealing with patients and talking with them about their problems has made " + girlName + " more Charismatic.", Day0Night1 == SHIFT_NIGHT);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKNURSE, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

	return false;
}