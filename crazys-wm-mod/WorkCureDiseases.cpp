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

#pragma endregion

// `J` Job Clinic - Full_Time_Job
bool cJobManager::WorkCureDiseases(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	int actiontype = ACTION_WORKTHERAPY;
	// if she was not in JOB_CUREDISEASES yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_CUREDISEASES) girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
	if (girl->m_WorkingDay < 0) girl->m_WorkingDay = 0;
	girl->m_DayJob = girl->m_NightJob = JOB_CUREDISEASES;	// it is a full time job

	if (!girl->has_disease())
	{
		ss << " has no diseases";
		int newjob = JOB_CLINICREST;
		if (girl->health() < 80 || girl->tiredness() > 20)
		{
			ss << ". She was not feeling well so she ";
			if (girl->has_trait("Construct"))	{ newjob = JOB_GETREPAIRS;	ss << "went to see the mechanic for repairs instead."; }
			else/*                         */	{ newjob = JOB_GETHEALING;	ss << "checked herself in for treatment instead."; }
		}
		else/*                             */	{ ss << " so she was sent to the waiting room."; }
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = newjob;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}
	int numdoctor = g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, Day0Night1);
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, Day0Night1);
	if (numdoctor + numnurse < 1)
	{
		ss << " does nothing. You don't have any Doctors or Nurses working. (requires 2 Doctors and 4 Nurses for fastest results) ";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}

	int enjoy = 0, cost = 0;
	vector<string> diseases;
	if (girl->has_trait("Herpes"))		{ diseases.push_back("Herpes");		cost += 50; }
	if (girl->has_trait("Chlamydia"))	{ diseases.push_back("Chlamydia");	cost += 100; }
	if (girl->has_trait("Syphilis"))	{ diseases.push_back("Syphilis");	cost += 150; }
	if (girl->has_trait("AIDS"))		{ diseases.push_back("AIDS");		cost += 200; }
	int numdiseases = diseases.size();

	if (numdiseases > 1)	ss << " is in the Clinic to get her diseases treated";
	else/*            */	ss << " is in the Clinic to get treatment for " << diseases[0];
	ss << ".\n \n";


	int startday = girl->m_WorkingDay;
	int libido = 1;
	int msgtype = Day0Night1, imagetype = IMGTYPE_SEX;
	g_Girls.UnequipCombat(girl);	// not for patient

	//	`J` We want to get 2 doctors and 4 nurses if possible
	int d1 = -1, d2 = -1, n1 = -1, n2 = -1, n3 = -1, n4 = -1;
	vector<sGirl*> doctors = girls_on_job(brothel, JOB_DOCTOR, Day0Night1);
	vector<sGirl*> nurses = girls_on_job(brothel, JOB_NURSE, Day0Night1);

	if (numdoctor > 2)	// choose 2 random doctors
	{
		d1 = g_Dice%numdoctor;
		d2 = g_Dice%numdoctor;
		while (d1 == d2)	d2 = g_Dice%numdoctor;
	}
	else					// otherwise take whoever is there
	{
		if (numdoctor > 0)	d1 = 0;
		if (numdoctor > 1)	d2 = 1;
	}
	if (numnurse > 4)	// choose 4 random nurses
	{
		n1 = g_Dice%numnurse;	n2 = g_Dice%numnurse;	n3 = g_Dice%numnurse;	n4 = g_Dice%numnurse;
		while (n2 == n1)							n2 = g_Dice%numnurse;
		while (n3 == n1 || n3 == n2)				n3 = g_Dice%numnurse;
		while (n4 == n1 || n4 == n2 || n4 == n3)	n4 = g_Dice%numnurse;
	}
	else					// otherwise take whoever is there
	{
		if (numnurse > 0)	n1 = 0;
		if (numnurse > 1)	n2 = 1;
		if (numnurse > 2)	n2 = 2;
		if (numnurse > 3)	n2 = 3;
	}

	sGirl* Doctor1 = d1 != -1 ? doctors[d1] : 0;
	sGirl* Doctor2 = d2 != -1 ? doctors[d2] : 0;
	sGirl* Nurse1 = n1 != -1 ? nurses[n1] : 0;
	sGirl* Nurse2 = n2 != -1 ? nurses[n2] : 0;
	sGirl* Nurse3 = n3 != -1 ? nurses[n3] : 0;
	sGirl* Nurse4 = n4 != -1 ? nurses[n4] : 0;

	numdoctor = numnurse = 0;		// clear the count and recheck to make sure they were found properly
	if (Doctor1)	numdoctor++;
	if (Doctor2)	numdoctor++;
	if (Nurse1)		numnurse++;
	if (Nurse2)		numnurse++;
	if (Nurse3)		numnurse++;
	if (Nurse4)		numnurse++;

	// up to 13 per doctor and 6 per nurse = 50 total
	int doctormax = 13;		int doctordiv = 30;
	int nursemax = 6;		int nursediv = 60;

	if (numdoctor + numnurse == 6)
	{
		ss << girlName << " has a full complement of Doctors and Nurses looking after her";
	}
	else if (numdoctor + numnurse > 0)
	{
		ss << "The Clinic ";
		if (numdoctor == 0)
		{
			ss << "has no Doctors working ";
			if (numnurse == 4)	ss << "so the Nurses have to take care of " << girlName << " on their own";
			else/*          */	ss << "and not enough Nurses working either";
			nursemax = numnurse + 1;	nursediv = (13 - numnurse) * 10;
		}
		else if (numdoctor == 1)
		{
			ss << "is short handed on Doctors";
			if (numnurse == 0) ss << " and has no Nurses working";
			else if (numnurse < 4) ss << " and Nurses";
			ss << " so " << girlName << "'s care is not as good as it should be";
			doctormax = 5 + numnurse;	doctordiv = 40;
			nursemax = numnurse + 1;	nursediv = (13 - numnurse) * 9;
		}
		else // 2 doctors
		{
			if (numnurse == 0)	ss << "has no nurses working and the Doctors can't spend all their time with " << girlName;
			else ss << "is short staffed on Nurses so " << girlName << "'s care is not as good as it should be";
			doctormax = 6 + numnurse;	doctordiv = 65 - (numnurse * 7);
			nursemax = numnurse + 2;	nursediv = 1 + ((12 - numnurse) * 9);
		}
	}
	else
	{
		ss << "There were no Doctors or Nurses working so " << girlName << " just lay in bed getting sicker";
		girl->m_WorkingDay -= g_Dice % 10;
		cost = 0;	// noone to give her the medicine
	}
	ss << ".\n \n";

	girl->m_WorkingDay += girl->constitution() / 10;
	if (girl->has_trait("Construct"))			girl->m_WorkingDay += 3;			// Not flesh and blood so easier to cleanse
	if (girl->has_trait("Powerful Magic"))		girl->m_WorkingDay += 3;			// She can use her magic to speed it up a bit
	if (girl->has_trait("Strong Magic"))		girl->m_WorkingDay += 2;			// She can use her magic to speed it up a bit
	if (girl->has_trait("Doctor"))				girl->m_WorkingDay += 2;			// She can use her msdicine to speed it up a bit
	if (girl->has_trait("Angel"))				girl->m_WorkingDay += 2;			// A creature of purity
	if (girl->has_trait("Half-Construct"))		girl->m_WorkingDay += 1;			// Not all flesh and blood so easier to cleanse
	if (girl->has_trait("Skeleton"))			girl->m_WorkingDay += 1;			// No flesh or blood so easier to cleanse
	if (girl->has_trait("Dwarf"))				girl->m_WorkingDay += 1;			// a hearty race
	if (girl->has_trait("Elf"))					girl->m_WorkingDay += 1;			// a healty race
	if (girl->has_trait("Tough"))				girl->m_WorkingDay += 1;			// her immune system is a little stronger
	if (girl->has_trait("Fallen Goddess"))		girl->m_WorkingDay += g_Dice % 2;	// she must have some power left
	if (girl->has_trait("Goddess"))				girl->m_WorkingDay += g_Dice % 2;	// she must have some power left
	if (girl->has_trait("Country Gal"))			girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Farmer"))				girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Farmers Daughter"))	girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Homeless"))			girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Maid"))				girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Waitress"))			girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Porn Star"))			girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Whore"))				girl->m_WorkingDay += g_Dice % 2;	// she is used to dirty surroundings so her immune system is a little stronger
	if (girl->has_trait("Slitherer"))			girl->m_WorkingDay += g_Dice % 2;	// more of her body touches the ground so her immune system is a little stronger
	if (girl->has_trait("Succubus"))			girl->m_WorkingDay += g_Dice % 2;	// she feeds on dark energies so her immune system is a little stronger

	if (girl->has_trait("Zombie"))				girl->m_WorkingDay -= 5;			// she is a rotting corpse, she has no immune system
	if (girl->has_trait("Demon"))				girl->m_WorkingDay -= 3;			// a creature of taint
	if (girl->has_trait("Demon Possessed"))		girl->m_WorkingDay -= 3;			// she is posessed by a creature of taint
	if (girl->has_trait("Old"))					girl->m_WorkingDay -= 2;			// her immune system is diminished due to her age
	if (girl->has_trait("Fragile"))				girl->m_WorkingDay -= 2;			// she gets sick easier
	if (girl->has_trait("Undead"))				girl->m_WorkingDay -= 1;			// she is not living, her immune system is not the best
	if (girl->has_trait("Vampire"))				girl->m_WorkingDay -= 1;			// she needs other peoples' blood to survive so her blood is not all the same
	if (girl->has_trait("Delicate"))			girl->m_WorkingDay -= 1;			// she gets sick easier
	if (girl->has_trait("Fairy Dust Addict"))	girl->m_WorkingDay -= 1;			// she is a little worse for wear because of drug abuse
	if (girl->has_trait("Former Addict"))		girl->m_WorkingDay -= 1;			// she is a little worse for wear because of drug abuse
	if (girl->has_trait("Shroud Addict"))		girl->m_WorkingDay -= 1;			// she is a little worse for wear because of drug abuse
	if (girl->has_trait("Viras Blood Addict"))	girl->m_WorkingDay -= 1;			// she is a little worse for wear because of drug abuse
	if (girl->has_trait("Noble"))				girl->m_WorkingDay -= g_Dice % 2;	// she is not used to dirty surroundings so her immune system is a little weaker
	if (girl->has_trait("Princess"))			girl->m_WorkingDay -= g_Dice % 2;	// she is not used to dirty surroundings so her immune system is a little weaker
	if (girl->has_trait("Queen"))				girl->m_WorkingDay -= g_Dice % 3;	// she is not used to dirty surroundings so her immune system is a little weaker


#pragma endregion
#pragma region //	Count the Days				//

	if (Day0Night1)	// Count up the points at the end of the day
	{
		if (Doctor1)	girl->m_WorkingDay += min(doctormax, ((Doctor1->medicine() + Doctor1->intelligence() + Doctor1->magic() + Doctor1->herbalism()) / doctordiv));
		if (Doctor2)	girl->m_WorkingDay += min(doctormax, ((Doctor2->medicine() + Doctor2->intelligence() + Doctor2->magic() + Doctor2->herbalism()) / doctordiv));
		if (Nurse1)		girl->m_WorkingDay += min(nursemax, ((Nurse1->medicine() + Nurse1->intelligence() + Nurse1->magic() + Nurse1->herbalism()) / nursediv));
		if (Nurse2)		girl->m_WorkingDay += min(nursemax, ((Nurse2->medicine() + Nurse2->intelligence() + Nurse2->magic() + Nurse2->herbalism()) / nursediv));
		if (Nurse3)		girl->m_WorkingDay += min(nursemax, ((Nurse3->medicine() + Nurse3->intelligence() + Nurse3->magic() + Nurse1->herbalism()) / nursediv));
		if (Nurse4)		girl->m_WorkingDay += min(nursemax, ((Nurse4->medicine() + Nurse4->intelligence() + Nurse4->magic() + Nurse4->herbalism()) / nursediv));

	}
	if (numnurse == 4)
	{
		ss << "The Nurses kept her healthy and happy during her treatment";
		girl->health(g_Dice.bell(0, 20));
		girl->happiness(g_Dice.bell(0, 10));
		girl->spirit(g_Dice.bell(0, 10));
		girl->mana(g_Dice.bell(0, 20));
	}
	else if (numnurse > 0)
	{
		ss << "The Nurse" << (numnurse > 1 ? "s" : "") << " helped her during her treatment";
		girl->health(g_Dice.bell(0, 10));
		girl->happiness(g_Dice.bell(0, 5));
		girl->spirit(g_Dice.bell(0, 5));
		girl->mana(g_Dice.bell(0, 10));
	}
	else
	{
		ss << "She is sad and has lost some health during the treatment";
		girl->health(g_Dice.bell(-20, 2));
		girl->happiness(g_Dice.bell(-10, 1));
		girl->spirit(g_Dice.bell(-5, 1));
		girl->mana(g_Dice.bell(-20, 3));
	}


	ss << ".\n \n";

#pragma endregion
#pragma region //	Surgery Finished			//

	if (Day0Night1 && girl->m_WorkingDay >= 100)
	{
		msgtype = EVENT_GOODNEWS;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;

		string diseasecured = diseases[g_Dice%numdiseases];
		girl->remove_trait(diseasecured);
		numdiseases--;
		ss << "You pay " << cost << " gold for last dose of the medicine used in her treatment.\n \nThe treatment is a success, " << girlName << " no longer has " << diseasecured << "!\n \n";

		if (!girl->has_disease())
		{
			ss << "She has been released from the Clinic";
			girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		}
		else
		{
			ss << "Her ";
			if (numdoctor > 0) ss << "Doctor" << (numdoctor > 1 ? "s" : "");
			else ss << "Nurse" << (numnurse > 1 ? "s" : "");
			ss << " keep her in bed so they can treat her other disease" << (numdiseases > 1 ? "s" : "");
		}
	}
	else if (girl->m_WorkingDay < 0)
	{
		ss << "You pay " << cost << " gold for the medicine and equipment used in her treatment, however she doesn't seem to be getting any better";
		if (girl->m_WorkingDay < 3) ss << ", in fact, she seems to be getting worse";
		girl->health(-girl->m_WorkingDay);
		girl->happiness(-girl->m_WorkingDay/2);
		girl->tiredness(girl->m_WorkingDay);
		girl->constitution(-girl->m_WorkingDay / 10);

		girl->m_WorkingDay = 0;
	}
	else
	{
		ss << "You pay " << cost << " gold for the medicine and equipment used in her treatment";
	}
	ss << ".\n \n";

	brothel->m_Finance.clinic_costs(cost);	// pay for it

#pragma endregion
#pragma region	//	Finish the shift			//

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

	// Improve girl
	if (girl->has_trait( "Lesbian"))		libido += numnurse + numdoctor;
	if (girl->has_trait( "Masochist"))	libido += 1;
	if (girl->has_trait( "Nymphomaniac"))	libido += 2;
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	if (g_Dice % 10 == 0)
		girl->medicine(1);	// `J` she watched what the doctors and nurses were doing

#pragma endregion
	return false;
}

double cJobManager::JP_CureDiseases(sGirl* girl, bool estimate)// not used
{
	int numdiseases = 0;
	if (girl->has_trait("AIDS"))			numdiseases++;
	if (girl->has_trait("Herpes"))			numdiseases++;
	if (girl->has_trait("Chlamydia"))		numdiseases++;
	if (girl->has_trait("Syphilis"))		numdiseases++;

	if (numdiseases == 0)	return -1000;
	return numdiseases * 100;
}
