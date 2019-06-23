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

#include "cArena.h"
#include "cGangs.h"

extern cMessageQue      g_MessageQue;
extern cGirls           g_Girls;
extern cBrothelManager  g_Brothels;
extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;
extern cRng             g_Dice;
extern cGold            g_Gold;

// // ----- Strut sArena Create / destroy
sArena::sArena() : m_Finance(0)	// constructor
{
	m_var = 0;
	m_Name = "arena";
	m_Filthiness = 0;
	m_Next = 0;
	m_Girls = 0;
	m_LastGirl = 0;
	m_NumGirls = 0;
	m_SecurityLevel = 0;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;
}

sArena::~sArena()			// destructor
{
	m_var = 0;
	if (m_Next)		delete m_Next;
	m_Next = 0;
	if (m_Girls)	delete m_Girls;
	m_LastGirl = 0;
	m_Girls = 0;
}

void cArenaManager::AddGirl(int brothelID, sGirl* girl, bool keepjob)
{
	girl->where_is_she = 0;
	girl->m_InStudio = false;
	girl->m_InArena = true;
	girl->m_InCentre = false;
	girl->m_InClinic = false;
	girl->m_InFarm = false;
	girl->m_InHouse = false;
	cBrothelManager::AddGirl(brothelID, girl, keepjob);
}

void cArenaManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InArena = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}

// ----- Class cArenaManager Create / destroy
cArenaManager::cArenaManager()			// constructor
{
	m_JobManager.Setup();
}

cArenaManager::~cArenaManager()			// destructor
{
	cArenaManager::Free();
}

void cArenaManager::Free()
{
	if (m_Parent)	delete m_Parent;
	m_Parent = 0;
	m_Last = 0;
	m_NumBrothels = 0;
}

// ----- Update & end of turn
void cArenaManager::UpdateArena()	// Start_Building_Process_A
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cArena.cpp
	u_int restjob = JOB_ARENAREST;
	u_int matronjob = JOB_DOCTORE;
	u_int firstjob = JOB_FIGHTBEASTS;
	u_int lastjob = JOB_CLEANARENA;
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
			cgirl->m_InArena = true;
			cgirl->m_InCentre = false;
			cgirl->m_InClinic = false;
			cgirl->m_InFarm = false;
			cgirl->m_InHouse = false;

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
void cArenaManager::UpdateGirls(sBrothel* brothel, bool Day0Night1)	// Start_Building_Process_B
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cArena.cpp
	u_int restjob = JOB_ARENAREST;
	u_int matronjob = JOB_DOCTORE;
	u_int firstjob = JOB_FIGHTBEASTS;
	u_int lastjob = JOB_CLEANARENA;
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
			g_Girls.UseItems(current);					// Girl uses items she has
			g_Girls.CalculateGirlType(current);			// update the fetish traits
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
				current->m_Events.AddMessage("The Doctore puts herself back to work.", IMGTYPE_PROFILE, EVENT_BACKTOWORK);
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
			ss << girlName << " refused to work as the Doctore.";
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
			else if (totalGold < 0)		{ sum = EVENT_DEBUG; ss << "ERROR: She has a loss of " << totalGold << " gold.\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << current->m_Realname << "\nJob: " << m_JobManager.JobName[(Day0Night1 ? current->m_NightJob : current->m_DayJob)] << "\nPay:     " << current->m_Pay << "\nTips:   " << current->m_Tips << "\nTotal: " << totalGold; }
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
				ss << "The Doctore puts " << girlName << " back to work.\n";
			}
			else if (current->m_DayJob == restjob && current->m_NightJob == restjob)
			{	// if they have no job at all, assign them a job
				ss << "The Doctore assigns " << girlName << " to ";

				// need at least 1 guard and 1 cleaner (because guards must be free, they get assigned first)
				if (current->is_free() && GetNumGirlsOnJob(0, JOB_CITYGUARD, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_CITYGUARD;
					ss << "work helping the city guard.";
				}
				else if (GetNumGirlsOnJob(0, JOB_CLEANARENA, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_CLEANARENA;
					ss << "work cleaning the arena.";
				}
				else if (GetNumGirlsOnJob(0, JOB_BLACKSMITH, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_BLACKSMITH;
					ss << "work making weapons and armor.";
				}
				else if (GetNumGirlsOnJob(0, JOB_COBBLER, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_COBBLER;
					ss << "work making shoes and leather items.";
				}
				else if (GetNumGirlsOnJob(0, JOB_JEWELER, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_JEWELER;
					ss << "work making jewelery.";
				}

				// next assign more guards and cleaners if there are a lot of girls to choose from
				else if (current->is_free() && GetNumGirlsOnJob(0, JOB_CITYGUARD, Day0Night1) < numgirls / 20)
				{
					current->m_DayJob = current->m_NightJob = JOB_CITYGUARD;
					ss << "work helping the city guard.";
				}
				else if (GetNumGirlsOnJob(0, JOB_CLEANARENA, Day0Night1) < numgirls / 20)
				{
					current->m_DayJob = current->m_NightJob = JOB_CLEANARENA;
					ss << "work cleaning the arena.";
				}
				else if (GetNumGirlsOnJob(0, JOB_BLACKSMITH, Day0Night1) < numgirls / 20)
				{
					current->m_DayJob = current->m_NightJob = JOB_BLACKSMITH;
					ss << "work making weapons and armor.";
				}
				else if (GetNumGirlsOnJob(0, JOB_COBBLER, Day0Night1) < numgirls / 20)
				{
					current->m_DayJob = current->m_NightJob = JOB_COBBLER;
					ss << "work making shoes and leather items.";
				}
				else if (GetNumGirlsOnJob(0, JOB_JEWELER, Day0Night1) < numgirls / 20)
				{
					current->m_DayJob = current->m_NightJob = JOB_JEWELER;
					ss << "work making jewelery.";
				}

				// Assign fighters - 50+ combat

				/*	Only fight beasts if there are 10 or more available
				*		and 1 girl per 10 beasts so they don't get depleted too fast.
				*	You can manually assign more if you want but I prefer to save beasts for the brothel
				*		until each building has their own beast supply.
				*	The farm will supply them when more work gets done to it
				*/
				else if (current->combat() > 60 && g_Brothels.GetNumBeasts() >= 10 &&
					GetNumGirlsOnJob(0, JOB_FIGHTBEASTS, Day0Night1) < g_Brothels.GetNumBeasts() / 10)
				{
					current->m_DayJob = current->m_NightJob = JOB_FIGHTBEASTS;
					ss << "work fighting beast in the arena.";
				}
				// if there are not enough beasts, have the girls fight other girls
				else if (current->combat() > 60 && GetNumGirlsOnJob(0, JOB_FIGHTARENAGIRLS, Day0Night1) < 1)
				{
					current->m_DayJob = current->m_NightJob = JOB_FIGHTARENAGIRLS;
					ss << "work fighting other girls in the arena.";
				}

				else	// assign anyone else to Traning
				{
					current->m_DayJob = current->m_NightJob = JOB_FIGHTTRAIN;
					ss << "train for the arena.";
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

	////////////////////////////////////////
	//  Run any races the girls can run.  //
	////////////////////////////////////////
//	current = brothel->m_Girls;
//	while (current)
//	{
//		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
//		if (current->is_dead() || sw != JOB_RACING)
//		{	// skip dead girls, and anyone not racing
//			if (current->m_Next) { current = current->m_Next; continue; }
//			else { current = 0; break; }
//		}
//		ss.str("");
//		// do their job
//		refused = m_JobManager.JobFunc[sw](current, brothel, Day0Night1, summary);
//
//		totalPay += current->m_Pay;
//		totalTips += current->m_Tips;
//		totalGold += current->m_Pay + current->m_Tips;
//		g_Brothels.CalculatePay(brothel, current, sw);
//
//		//		Summary Messages
//		if (refused)
//		{
//			brothel->m_Fame -= current->fame();
//			ss << girlName << " refused to work so made no money.";
//		}
//		else
//		{
//			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, Day0Night1);
//			if (totalGold < 0) sum = EVENT_DEBUG;
//
//			brothel->m_Fame += current->fame();
//		}
//		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
//
//		current = current->m_Next; // Next Girl
//	}



	/////////////////////////////////////////////////////////////////
	//  All other Jobs in the Arena can be done at the same time.  //
	/////////////////////////////////////////////////////////////////
	/* `J` zzzzzz - Need to split up the jobs
	Done - JOB_ARENAREST, JOB_DOCTORE

	JOB_CLEANARENA

	JOB_FIGHTBEASTS
	JOB_FIGHTARENAGIRLS
	JOB_FIGHTTRAIN
	JOB_CITYGUARD

	//*/
	current = brothel->m_Girls;
	while (current)
	{
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw == restjob || sw == matronjob)// || sw == JOB_RACING)
		{	// skip dead girls, resting girls and the matron and racers
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		// fight beasts so if there is no beasts dont want them doing nothing
		if (sw == JOB_FIGHTBEASTS && g_Brothels.GetNumBeasts() < 1)
		{
			stringstream ssc;
			ssc << "There are no beasts to fight so " << girlName << " was sent to ";

			if (current->health() < 50)
			{
				ssc << "rest and heal";
				sw = restjob;
			}
			else if (current->combat() > 90 && current->magic() > 90 && current->agility() > 90 && current->constitution() > 90 && current->health() > 90)
			{
				ssc << "fight other girls";
				sw = JOB_FIGHTARENAGIRLS;
			}
			else
			{
				ssc << "train for combat";
				sw = JOB_FIGHTTRAIN;
			}
			ssc << " instead.\n"<<"\n";
			current->m_Events.AddMessage(ssc.str(), IMGTYPE_PROFILE, Day0Night1);

		}

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
		current->health(2, false);
		current->tiredness(-2, false);

		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
		if (current->happiness()< 40)
		{
			if (sw != matronjob && matron && brothel->m_NumGirls > 1 && g_Dice.percent(70))
			{
				ss << "The Doctore helps cheer up " << girlName << " when she is feeling sad.\n";
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
			ss << "As Doctore, " << girlName << " has the keys to the store room.\nShe used them to 'borrow' ";
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
					ss << "The Doctore takes " << girlName << " off duty to rest due to her ";
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
						ss << "The Doctore helps ";
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

TiXmlElement* cArenaManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("Arena_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	string message;

	// save arena
	TiXmlElement* pBrothels = new TiXmlElement("Arenas");
	pBrothelManager->LinkEndChild(pBrothels);
	sArena* current = (sArena*)m_Parent;
	//         ...................................................
	message = "***************** Saving arenas *****************";
	g_LogFile.write(message);
	while (current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveArenaXML(pBrothels);
		current = (sArena*)current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sArena::SaveArenaXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("Arena");
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

bool cArenaManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too

	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0)
	{
		return false;
	}

	string message = "";
	//         ...................................................
	message = "***************** Loading arena ****************";
	g_LogFile.write(message);
	m_NumArenas = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Arenas");
	if (pBrothels)
	{
		for (TiXmlElement* pBrothel = pBrothels->FirstChildElement("Arena");
			pBrothel != 0;
			pBrothel = pBrothel->NextSiblingElement("Arena"))
		{
			sArena* current = new sArena();
			bool success = current->LoadArenaXML(TiXmlHandle(pBrothel));
			if (success == true)
			{
				AddBrothel(current);
			}
			else
			{
				delete current;
				continue;
			}

		} // load a arena
	}
	return true;
}

bool sArena::LoadArenaXML(TiXmlHandle hBrothel)
{
	// no need to init this, we just created it
	TiXmlElement* pBrothel = hBrothel.ToElement();
	if (pBrothel == 0)
	{
		return false;
	}

	if (pBrothel->Attribute("Name"))
	{
		m_Name = pBrothel->Attribute("Name");
	}

	int tempInt = 0;

	std::string message = "Loading arena: ";
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
		for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				girl->m_InArena = true;
				AddGirl(girl);
			}
			else
			{
				delete girl;
				continue;
			}
		}
	}

	//commented out before the conversion to XML
	//building.load(ifs);
	return true;
}