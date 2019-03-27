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

#include <sstream>

#ifdef LINUX
#include "linux.h"
#endif

#include "cHouse.h"
#include "cGangs.h"
#include "DirPath.h"
#include "cMessageBox.h"
#include "strnatcmp.h"
#include "XmlMisc.h"
#include "libintl.h"

extern cMessageQue      g_MessageQue;
extern cGirls           g_Girls;
extern cBrothelManager  g_Brothels;
extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;
extern cRng             g_Dice;
extern cGold            g_Gold;
extern char             buffer[1000];

// // ----- Strut sHouse Create / destroy
sHouse::sHouse() : m_Finance(0)	// constructor
{
	m_var = 0;
	m_Name = "House";
	m_Filthiness = 0;
	m_Next = 0;
	m_Girls = 0;
	m_LastGirl = 0;
	m_NumGirls = 0;
	m_SecurityLevel = 0;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;
}

sHouse::~sHouse()			// destructor
{
	m_var = 0;
	if (m_Next)		delete m_Next;
	m_Next = 0;
	if (m_Girls)	delete m_Girls;
	m_LastGirl = 0;
	m_Girls = 0;
}

void cHouseManager::AddGirl(int brothelID, sGirl* girl, bool keepjob)
{
	girl->where_is_she = 0;
	girl->m_InStudio = false;
	girl->m_InArena = false;
	girl->m_InCentre = false;
	girl->m_InClinic = false;
	girl->m_InFarm = false;
	girl->m_InHouse = true;
	cBrothelManager::AddGirl(brothelID, girl, keepjob);
}

void cHouseManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InHouse = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}

// ----- Class cHouseManager Create / destroy
cHouseManager::cHouseManager()			// constructor
{
	m_JobManager.Setup();
}

cHouseManager::~cHouseManager()			// destructor
{
	cHouseManager::Free();
}

void cHouseManager::Free()
{
	if (m_Parent)	delete m_Parent;
	m_Parent = 0;
	m_Last = 0;
	m_NumBrothels = 0;
}

// ----- Update & end of turn
void cHouseManager::UpdateHouse()	// Start_Building_Process_A
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cHouse.cpp
	u_int restjob = JOB_HOUSEREST;
	u_int matronjob = JOB_HEADGIRL;
	u_int firstjob = JOB_HOUSEREST;
	u_int lastjob = JOB_HOUSEPET;
	cTariff tariff;
	stringstream ss;
	string girlName;

	sBrothel* current = (sBrothel*)m_Parent;

	current->m_Finance.zero();
	current->m_AntiPregUsed = 0;


	sGirl* cgirl = current->m_Girls;
	while (cgirl)
	{
		current->m_Filthiness++;
		if (cgirl->is_dead())			// Remove any dead bodies from last week
		{
			current->m_Filthiness++; // `J` Death is messy
			sGirl* DeadGirl = 0;
			girlName = cgirl->m_Realname;
			DeadGirl = cgirl;
			// If there are more girls to process
			cgirl = (cgirl->m_Next) ? cgirl->m_Next : 0;
			// increase all the girls fear and hate of the player for letting her die (weather his fault or not)
			UpdateAllGirlsStat(current, STAT_PCFEAR, 2);
			UpdateAllGirlsStat(current, STAT_PCHATE, 1);

			ss.str(""); ss << girlName << " has died from her injuries, the other girls all fear and hate you a little more.";
			DeadGirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);
			ss.str(""); ss << girlName << " has died from her injuries.  Her body will be removed by the end of the week.";
			DeadGirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_SUMMARY);

			RemoveGirl(0, DeadGirl); DeadGirl = 0;	// cleanup
		}
		else
		{
			cgirl->m_Events.Clear();			// Clear the girls' events from the last turn
			cgirl->where_is_she = 0;
			cgirl->m_InStudio = false;
			cgirl->m_InArena = false;
			cgirl->m_InCentre = false;
			cgirl->m_InClinic = false;
			cgirl->m_InFarm = false;
			cgirl->m_InHouse = true;

			cgirl->m_Pay = cgirl->m_Tips = 0;

			// `J` Check for out of building jobs and set yesterday jobs for everyone first
			if (cgirl->m_DayJob	  < firstjob || cgirl->m_DayJob   > lastjob)	cgirl->m_DayJob = restjob;
			if (cgirl->m_NightJob < firstjob || cgirl->m_NightJob > lastjob)	cgirl->m_NightJob = restjob;
			if (cgirl->m_PrevDayJob != 255 && (cgirl->m_PrevDayJob	 < firstjob || cgirl->m_PrevDayJob   > lastjob))	cgirl->m_PrevDayJob = 255;
			if (cgirl->m_PrevNightJob != 255 && (cgirl->m_PrevNightJob < firstjob || cgirl->m_PrevNightJob > lastjob))	cgirl->m_PrevNightJob = 255;
			cgirl->m_YesterDayJob = cgirl->m_DayJob;		// `J` set what she did yesterday
			cgirl->m_YesterNightJob = cgirl->m_NightJob;	// `J` set what she did yesternight
			cgirl->m_Refused_To_Work_Day = false;
			cgirl->m_Refused_To_Work_Night = false;
			string summary = "";

			g_Girls.AddTiredness(cgirl);			// `J` moved all girls add tiredness to one place
			do_food_and_digs(current, cgirl);		// Brothel only update for girls accommodation level
			g_Girls.updateGirlAge(cgirl, true);		// update birthday counter and age the girl
			g_Girls.HandleChildren(cgirl, summary);	// handle pregnancy and children growing up
			g_Girls.updateSTD(cgirl);				// health loss to STD's				NOTE: Girl can die
			g_Girls.updateHappyTraits(cgirl);		// Update happiness due to Traits	NOTE: Girl can die
			updateGirlTurnBrothelStats(cgirl);		// Update daily stats				Now only runs once per day
			g_Girls.updateGirlTurnStats(cgirl);		// Stat Code common to Dugeon and Brothel

			if (cgirl->m_JustGaveBirth)				// if she gave birth, let her rest this week
			{
				if (cgirl->m_DayJob != restjob)		cgirl->m_PrevDayJob = cgirl->m_DayJob;
				if (cgirl->m_NightJob != restjob)	cgirl->m_PrevNightJob = cgirl->m_NightJob;
				cgirl->m_DayJob = cgirl->m_NightJob = restjob;
			}

			cgirl = cgirl->m_Next;
		}
	}

	UpdateGirls(current, 0);	// Run the Day Shift

	UpdateGirls(current, 1);	// Run the Nighty Shift

	g_Gold.brothel_accounts(current->m_Finance, current->m_id);

	cgirl = current->m_Girls;
	while (cgirl)
	{
		g_Girls.updateTemp(cgirl);			// update temp stuff
		g_Girls.EndDayGirls(current, cgirl);
		cgirl = cgirl->m_Next;
	}
	if (current->m_Filthiness < 0)		current->m_Filthiness = 0;
	if (current->m_SecurityLevel < 0)	current->m_SecurityLevel = 0;
}

// Run the shifts
void cHouseManager::UpdateGirls(sBrothel* brothel, bool Day0Night1)	// Start_Building_Process_B
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cHouse.cpp
	u_int restjob = JOB_HOUSEREST;
	u_int matronjob = JOB_HEADGIRL;
	u_int firstjob = JOB_HOUSEREST;
	u_int lastjob = JOB_HOUSEPET;
	stringstream ss;
	string summary, girlName;
	u_int sw = 0, psw = 0;

	int totalPay = 0, totalTips = 0, totalGold = 0;
	int sum = EVENT_SUMMARY;
	int numgirls = GetNumGirls(brothel->m_id);

	bool matron = false, matrondone = false;


	bool refused = false;

	m_Processing_Shift = Day0Night1;		// WD:	Set processing flag to shift type

	//////////////////////////////////////////////////////
	//  Handle the start of shift stuff for all girls.  //
	//////////////////////////////////////////////////////
	sGirl* current = brothel->m_Girls;
	while (current)
	{
		if (current->is_dead())		// skip dead girls
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		else
		{
			g_Girls.UseItems(current);				// Girl uses items she has
			g_Girls.CalculateGirlType(current);		// update the fetish traits
			g_Girls.CalculateAskPrice(current, true);	// Calculate the girls asking price
			current = current->m_Next; // Next Girl
		}
	}

	////////////////////////////////////////////////////////
	//  Process Matron first incase she refuses to work.  //
	////////////////////////////////////////////////////////
	current = brothel->m_Girls;
	while (current && !matrondone)
	{
		if (current->is_dead() ||
			(GetNumGirlsOnJob(0, matronjob, Day0Night1) > 0 && (current->m_DayJob != matronjob || current->m_NightJob != matronjob)) ||
			(GetNumGirlsOnJob(0, matronjob, Day0Night1) < 1 && (current->m_PrevDayJob != matronjob || current->m_PrevNightJob != matronjob)))
		{	// Sanity check! Don't process dead girls and only process those with matron jobs
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// `J` so someone is or was a matron

		girlName = current->m_Realname;
		// if there is no matron on duty, we see who was on duty previously
		if (GetNumGirlsOnJob(0, matronjob, Day0Night1) < 1)
		{
			// if a matron was found and she is healthy, not tired and not on maternity leave... send her back to work
			if ((current->m_PrevDayJob == matronjob || current->m_PrevNightJob == matronjob) &&
				(current->health() >= 50 && current->tiredness() <= 50) &&
				current->m_PregCooldown < cfg.pregnancy.cool_down())
				// Matron job is more important so she will go back to work at 50% instead of regular 80% health and 20% tired
			{
				current->m_DayJob = current->m_NightJob = matronjob;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				current->m_Events.AddMessage("The Head Girl puts herself back to work.", IMGTYPE_PROFILE, EVENT_BACKTOWORK);
			}
			else if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// `J` Now we have a matron so lets see if she will work

		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");

		// `J` she can refuse the first shift then decide to work the second shift 
		if (!current->m_Refused_To_Work_Day && Day0Night1 == SHIFT_NIGHT)	// but if she worked the first shift she continues the rest of the night
		{
			matron = true;
			ss << girlName << " continued to help the other girls throughout the night.";
		}
		else if (current->disobey_check(ACTION_WORKMATRON, brothel))
		{
			(Day0Night1 ? current->m_Refused_To_Work_Night = true : current->m_Refused_To_Work_Day = true);
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work as the Head Girl.";
			sum = EVENT_NOWORK;
		}
		else	// so there is less chance of a matron refusing the entire turn
		{
			matron = true;
			totalPay = totalTips = totalGold = 0;
			m_JobManager.JobFunc[matronjob](current, brothel, Day0Night1, summary);
			totalGold += current->m_Pay + current->m_Tips;

			// She does not get paid for the first shift and gets docked some pay from the second shift if she refused once
			if (Day0Night1 == SHIFT_NIGHT) totalGold /= 3;

			current->m_Pay += max(0, totalGold);
			current->m_Pay = current->m_Tips = 0;

			brothel->m_Fame += current->fame();
			/* */if (totalGold > 0)		{ ss << girlName << " earned a total of " << totalGold << " gold directly from you. She gets to keep it all."; }
			else if (totalGold == 0)	{ ss << girlName << " made no money."; }
			else if (totalGold < 0)		{ sum = EVENT_DEBUG; ss << "ERROR: She has a loss of " << totalGold << " gold\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << current->m_Realname << "\nJob: " << m_JobManager.JobName[(Day0Night1 ? current->m_NightJob : current->m_DayJob)] << "\nPay:     " << current->m_Pay << "\nTips:   " << current->m_Tips << "\nTotal: " << totalGold; }
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;	// Next Girl
		matrondone = true;			// there can be only one matron so this ends the while loop
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Now If there is a matron and she is not refusing to work, then she can delegate the girls in this building.  //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	current = brothel->m_Girls;
	while (current)
	{
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw != restjob)
		{	// skip dead girls and anyone not resting
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		if (current->m_PregCooldown == cfg.pregnancy.cool_down())
		{
			ss << girlName << " is on maternity leave.";
		}
		else if (current->health() < 80 || current->tiredness() > 20)
		{
			m_JobManager.JobFunc[restjob](current, brothel, Day0Night1, summary);
		}
		else if (matron)	// send her back to work
		{
			psw = (Day0Night1 ? current->m_PrevNightJob : current->m_PrevDayJob);
			if (psw != restjob && psw != 255)
			{	// if she had a previous job, put her back to work.
				if (Day0Night1 == SHIFT_DAY)
				{
					current->m_DayJob = current->m_PrevDayJob;
					if (current->m_NightJob == restjob && current->m_PrevNightJob != restjob && current->m_PrevNightJob != 255)
						current->m_NightJob = current->m_PrevNightJob;
				}
				else
				{
					if (current->m_DayJob == restjob && current->m_PrevDayJob != restjob && current->m_PrevDayJob != 255)
						current->m_DayJob = current->m_PrevDayJob;
					current->m_NightJob = current->m_PrevNightJob;
				}
				ss << "The Head Girl puts " << girlName << " back to work.\n";
			}
			else if (current->m_DayJob == restjob && current->m_NightJob == restjob)
			{	// if they have no job at all, assign them a job
				ss << "The Head Girl assigns " << girlName << " to ";

				// Set any free girls who would do well at recruiting to recruit for you
				if (current->is_free() && m_JobManager.JP_Recruiter(current, true) >= 185)
				{
					current->m_DayJob = current->m_NightJob = JOB_RECRUITER;
					ss << "work recruiting girls for you.";
				}
				// assign a slave to clean 
				else if (current->is_slave() && GetNumGirlsOnJob(0, JOB_CLEANHOUSE, Day0Night1) < max(1, numgirls / 20))
				{
					current->m_DayJob = current->m_NightJob = JOB_CLEANHOUSE;
					ss << "work cleaning the house.";
				}
				// and a free girl to recruit for you
				else if (current->is_free() && GetNumGirlsOnJob(0, JOB_RECRUITER, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_RECRUITER;
					ss << "work recruiting girls for you.";
				}
				// set at least 1 bed warmer
				else if (GetNumGirlsOnJob(0, JOB_PERSONALBEDWARMER, Day0Night1) < 1
					&& !current->check_virginity())	// Crazy added this so that it wont set virgins to this
				{
					current->m_DayJob = current->m_NightJob = JOB_PERSONALBEDWARMER;
					ss << "work warming your bed.";
				}
				// assign 1 cleaner per 20 girls
				else if (GetNumGirlsOnJob(0, JOB_CLEANHOUSE, Day0Night1) < max(1, numgirls / 20))
				{
					current->m_DayJob = current->m_NightJob = JOB_CLEANHOUSE;
					ss << "work cleaning the house.";
				}
				// Put every one else who is not a virgin and needs some training
				else if (!current->check_virginity() && g_Girls.GetAverageOfSexSkills(current) < 99)
				{
					current->m_DayJob = current->m_NightJob = JOB_PERSONALTRAINING;
					ss << "get personal training from you.";
				}
				// Set any other free girls to recruit for you
				else if (current->is_free())
				{
					current->m_DayJob = current->m_NightJob = JOB_RECRUITER;
					ss << "work recruiting girls for you.";
				}
				else
				{
					ss << "do nothing because this part of the code has not been finished yet.";
				}
			}
			current->m_PrevDayJob = current->m_PrevNightJob = 255;
			sum = EVENT_BACKTOWORK;
		}
		else if (current->health() < 100 || current->tiredness() > 0)	// if there is no matron to send her somewhere just do resting
		{
			m_JobManager.JobFunc[restjob](current, brothel, Day0Night1, summary);
		}
		else	// no one to send her back to work
		{
			ss << "WARNING " << girlName << " is doing nothing!\n";
			sum = EVENT_WARNING;
		}

		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;
	}
	
	/////////////////////////////////////////////
	//  Do all Personal Bed Warmers together.  //
	/////////////////////////////////////////////


	current = brothel->m_Girls;
	while (current)
	{
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw != JOB_PERSONALBEDWARMER)
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		// do their job
		refused = m_JobManager.JobFunc[sw](current, brothel, Day0Night1, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, sw);

		//		Summary Messages
		if (refused)
		{
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, Day0Night1);
			if (totalGold < 0) sum = EVENT_DEBUG;

			brothel->m_Fame += current->fame();
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next; // Next Girl
	}




	/////////////////////////////////////////////////////////////////////
	//  All the orher Jobs in the House can be done at the same time.  //
	/////////////////////////////////////////////////////////////////////
	/* `J` zzzzzz - Need to split up the jobs
	Done - JOB_HOUSEREST, JOB_HEADGIRL, JOB_PERSONALBEDWARMER

	JOB_CLEANHOUSE
	JOB_RECRUITER
	JOB_PERSONALTRAINING

	//*/
	current = brothel->m_Girls;
	while (current)
	{
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw == restjob || sw == matronjob ||	// skip dead girls, resting girls and the matron
			sw == JOB_PERSONALBEDWARMER ||
			(Day0Night1 && sw == JOB_RECRUITER))							// skip recruiters on the night shift
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		// do their job
		refused = m_JobManager.JobFunc[sw](current, brothel, Day0Night1, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, sw);

		//		Summary Messages
		if (refused)
		{
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, Day0Night1);
			if (totalGold < 0) sum = EVENT_DEBUG;

			brothel->m_Fame += current->fame();
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next; // Next Girl
	}

	///////////////////////////////////
	//  Finaly do end of day stuff.  //
	///////////////////////////////////
	current = brothel->m_Girls;
	while (current)
	{
		if (current->is_dead())
		{	// skip dead girls
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");

		// update for girls items that are not used up
		do_daily_items(brothel, current);					// `J` added

		// Level the girl up if nessessary
		g_Girls.LevelUp(current);
		// Natural healing, 2% health and 2% tiredness per day
		current->health(2);
		current->health(2, false);
		current->tiredness(-2, false);

		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->happiness()< 40)
		{
			if (sw != matronjob && matron && brothel->m_NumGirls > 1 && g_Dice.percent(70))
			{
				ss << "The Head Girl helps cheer up " << girlName << " when she is feeling sad.\n";
				current->happiness(g_Dice % 10 + 5);
			}
			else if (brothel->m_NumGirls > 10 && g_Dice.percent(50))
			{
				ss << "Some of the other girls help cheer up " << girlName << " when she is feeling sad.\n";
				current->happiness(g_Dice % 8 + 3);
			}
			else if (brothel->m_NumGirls > 1 && g_Dice.percent(max(brothel->m_NumGirls, 50)))
			{
				ss << "One of the other girls helps cheer up " << girlName << " when she is feeling sad.\n";
				current->happiness(g_Dice % 6 + 2);
			}
			else if (brothel->m_NumGirls == 1 && g_Dice.percent(70))
			{
				ss << girlName << " plays around in the empty building until she feels better.\n";
				current->happiness(g_Dice % 10 + 10);
			}
			else if (current->health()< 20) // no one helps her and she is really unhappy
			{
				ss << girlName << " is looking very depressed. You may want to do something about that before she does something drastic.\n";
				sum = EVENT_WARNING;
			}
		}

		int t = current->tiredness();
		int h = current->health();
		if (sw == matronjob && (t > 60 || h < 40))
		{
			ss << "As Head Girl, " << girlName << " has the keys to the store room.\nShe used them to 'borrow' ";
			if (t > 50 && h < 50)
			{
				ss << "some potions";
				current->health(20 + g_Dice % 20, false);
				current->tiredness(-(20 + g_Dice % 20), false);
				g_Gold.consumable_cost(20, true);
			}
			else if (t > 50)
			{
				ss << "a resting potion";
				current->tiredness(-(20 + g_Dice % 20), false);
				g_Gold.consumable_cost(10, true);
			}
			else if (h < 50)
			{
				ss << "a healing potion";
				current->health(20 + g_Dice % 20, false);
				g_Gold.consumable_cost(10, true);
			}
			else
			{
				ss << "a potion";
				current->health(10 + g_Dice % 10, false);
				current->tiredness(-(10 + g_Dice % 10), false);
				g_Gold.consumable_cost(5, true);
			}
			ss << " for herself.\n";
		}
		else if (t > 80 || h < 40)
		{
			if (!matron)	// do no matron first as it is the easiest
			{
				ss << "WARNING! " << girlName;
				/* */if (t > 80 && h < 20)	ss << " is in real bad shape, she is tired and injured.\nShe should go to the Clinic.\n";
				else if (t > 80 && h < 40)	ss << " is in bad shape, she is tired and injured.\nShe should rest or she may die!\n";
				else if (t > 80)/*      */	ss << " is desparatly in need of rest.\nGive her some free time\n";
				else if (h < 20)/*      */	ss << " is badly injured.\nShe should rest or go to the Clinic.\n";
				else if (h < 40)/*      */	ss << " is hurt.\nShe should rest and recuperate.\n";
				sum = EVENT_WARNING;
			}
			else	// do all other girls with a matron working
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255) // the girl has been working
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = restjob;
					ss << "The Head Girl takes " << girlName << " off duty to rest due to her ";
					if (t > 80 && h < 40)	ss << "exhaustion.\n";
					else if (t > 80)		ss << "tiredness.\n";
					else if (h < 40)		ss << "low health.\n";
					else /*       */		ss << "current state.\n";
					sum = EVENT_WARNING;
				}
				else	// the girl has already been taken off duty by the matron
				{
					if (g_Dice.percent(70))
					{
						ss << "The Head Girl helps ";
						if (t > 80 && h < 40)
						{
							ss << girlName << " recuperate.\n";
							current->health(2 + g_Dice % 4, false);
							current->tiredness(-(2 + g_Dice % 4), false);
						}
						else if (t > 80)
						{
							ss << girlName << " to relax.\n";
							current->tiredness(-(5 + g_Dice % 5), false);
						}
						else if (h < 40)
						{
							ss << " heal " << girlName << ".\n";
							current->health(5 + g_Dice % 5, false);
						}
					}
				}
			}
		}

		if (ss.str().length() > 0)	current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;		// Process next girl
	}
	m_Processing_Shift = -1;	// WD: Finished Processing Shift set flag
}

TiXmlElement* cHouseManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("House_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	TiXmlElement* pBrothels = new TiXmlElement("Houses");
	pBrothelManager->LinkEndChild(pBrothels);
	sHouse* current = (sHouse*)m_Parent;
	g_LogFile.write("***************** Saving houses *****************");
	while (current)
	{
		g_LogFile.write("Saving brothel: " + current->m_Name);
		current->SaveHouseXML(pBrothels);
		current = (sHouse*)current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sHouse::SaveHouseXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("House");
	pRoot->LinkEndChild(pBrothel);
	pBrothel->SetAttribute("Name", m_Name);

	pBrothel->SetAttribute("id", m_id);
	pBrothel->SetAttribute("NumRooms", m_NumRooms);
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
	pBrothel->SetAttribute("MaxNumRooms", m_MaxNumRooms);
	pBrothel->SetAttribute("Fame", m_Fame);
	pBrothel->SetAttribute("Happiness", m_Happiness);
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);
	// save variables for sex restrictions
	pBrothel->SetAttribute("RestrictAnal", m_RestrictAnal);
	pBrothel->SetAttribute("RestrictBDSM", m_RestrictBDSM);
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictFoot", m_RestrictFoot);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictHand", m_RestrictHand);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictStrip", m_RestrictStrip);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);

	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("AntiPregUsed", m_AntiPregUsed);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);

	// Save Girls
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pBrothel->LinkEndChild(pGirls);
	sGirl* girl = m_Girls;
	while (girl)
	{
		girl->SaveGirlXML(pGirls);
		girl = girl->m_Next;
	}
	return pBrothel;
}

bool cHouseManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too
	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0) return false;
	g_LogFile.write("***************** Loading house ****************");
	m_NumHouses = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Houses");
	if (pBrothels)
	{
		for (TiXmlElement* pBrothel = pBrothels->FirstChildElement("House"); pBrothel != 0; pBrothel = pBrothel->NextSiblingElement("House"))
		{
			sHouse* current = new sHouse();
			bool success = current->LoadHouseXML(TiXmlHandle(pBrothel));
			if (success == true) { AddBrothel(current); }
			else { delete current; continue; }
		} // load a house
	}
	return true;
}

bool sHouse::LoadHouseXML(TiXmlHandle hBrothel)
{
	// no need to init this, we just created it
	TiXmlElement* pBrothel = hBrothel.ToElement();
	if (pBrothel == 0) return false;
	if (pBrothel->Attribute("Name")) m_Name = pBrothel->Attribute("Name");
	int tempInt = 0;

	std::string message = "Loading house: ";
	message += m_Name;
	g_LogFile.write(message);

	pBrothel->QueryIntAttribute("id", &m_id);
	pBrothel->QueryIntAttribute("NumRooms", &tempInt); m_NumRooms = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("MaxNumRooms", &tempInt); m_MaxNumRooms = tempInt; tempInt = 0;
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
	if (pBrothel->Attribute("Fame"))
	{
		pBrothel->QueryIntAttribute("Fame", &tempInt); m_Fame = tempInt; tempInt = 0;
	}
	else m_Fame = 0;
	if (pBrothel->Attribute("Happiness"))
	{
		pBrothel->QueryValueAttribute<unsigned short>("Happiness", &m_Happiness);
	}
	else m_Happiness = 0;
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictFoot", &m_RestrictFoot);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictHand", &m_RestrictHand);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictStrip", &m_RestrictStrip);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);

	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	pBrothel->QueryIntAttribute("AntiPregUsed", &m_AntiPregUsed);
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);

	// Load girls
	m_NumGirls = 0;
	TiXmlElement* pGirls = pBrothel->FirstChildElement("Girls");
	if (pGirls)
	{
		for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
		{
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				girl->m_InHouse = true;
				AddGirl(girl);
			}
			else
			{
				delete girl;
				continue;
			}
		}
	}
	return true;
}