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

#include <vector>
#include <sstream>
#include <algorithm>
#include "cMovieStudio.h"
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
extern bool				g_InitWin;
extern cMovieStudioManager  g_Studios;

// // ----- Strut sMovieStudio Create / destroy
sMovieStudio::sMovieStudio() : m_Finance(0)	// constructor
{
	m_var = 0;
	m_Name = "studio";
	m_Filthiness = 0;
	m_Next = 0;
	m_Girls = 0;
	m_LastGirl = 0;
	m_NumGirls = 0;
	m_SecurityLevel = 0;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;
	m_CurrFilm = 0;
	m_NumMovies = 0;
	m_LastMovies = 0;
	m_Movies = 0;
	m_MovieRunTime = 0;
}

sMovieStudio::~sMovieStudio()			// destructor
{
	m_var = 0;
	if (m_Next)		delete m_Next;
	m_Next = 0;
	if (m_Girls)	delete m_Girls;
	m_LastGirl = 0;
	m_Girls = 0;
	m_NumMovies = 0;
	if (m_Movies)	delete m_Movies;
	m_Movies = 0;
	m_LastMovies = 0;
	if (m_CurrFilm)	delete m_CurrFilm;
}

void cMovieStudioManager::AddGirl(int brothelID, sGirl* girl, bool keepjob)
{
	girl->where_is_she = 0;
	girl->m_InStudio = true;
	girl->m_InArena = false;
	girl->m_InCentre = false;
	girl->m_InClinic = false;
	girl->m_InFarm = false;
	girl->m_InHouse = false;
	cBrothelManager::AddGirl(brothelID, girl, keepjob);
}

void cMovieStudioManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InStudio = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}

// ----- Class cStudioManager Create / destroy
cMovieStudioManager::cMovieStudioManager()			// constructor
{
	m_JobManager.Setup();
}

cMovieStudioManager::~cMovieStudioManager()			// destructor
{
	cMovieStudioManager::Free();
}

void cMovieStudioManager::Free()
{
	if (m_Parent)	delete m_Parent;
	m_Parent = 0;
	m_Last = 0;
	m_NumBrothels = 0;
}

// ----- Update & end of turn
void cMovieStudioManager::UpdateMovieStudio()	// Start_Building_Process_A
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cMovieStudio.cpp
	u_int restjob = JOB_FILMFREETIME;
	u_int matronjob = JOB_DIRECTOR;
	u_int firstjob = JOB_FILMFREETIME;
	u_int lastjob = JOB_FILMRANDOM;
	cTariff tariff;
	stringstream ss;
	string girlName;

	sBrothel* current = (sBrothel*)m_Parent;
	current->m_Finance.zero();
	current->m_AntiPregUsed = 0;

	// `J` autocreatemovies added for .06.02.57
	if (GetNumScenes() < 10){}		// don't worry about it if there are less than 10 scenes
	else if (cfg.initial.autocreatemovies())	{ ReleaseCurrentMovie(true); }
	else if (GetNumScenes() > 0)
	{
		ss.str("");	ss << "You have " << GetNumScenes() << " unused scenes in the Movie Studio ready to be put into movies.";
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	}

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
			cgirl->m_InStudio = true;
			cgirl->m_InArena = false;
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

				if (cgirl->m_NightJob != restjob)	cgirl->m_PrevNightJob = cgirl->m_NightJob;
				cgirl->m_NightJob = restjob;
			}

			cgirl = cgirl->m_Next;
		}
	}

	m_FlufferQuality = m_CameraQuality = m_PurifierQaulity = m_DirectorQuality = 0;

	UpdateGirls(current);		// Run the Nighty Shift

	// Update movies currently being sold
	sMovie* movie = current->m_Movies;
	if (current->m_NumMovies > 0)
	{
		long income = 0;
		double bonusperc = g_Studios.m_PromoterBonus / 100;
		current->m_MovieRunTime++;	//	track how long the studio has been putting out movies

		while (movie)
		{
			if (movie->m_RunWeeks > 34)
			{
				movie->m_Promo_Quality = 0;
				movie->m_Quality = 0;
				movie->m_RunWeeks++;
				movie = movie->m_Next;
			}
			else
			{
				movie->m_Promo_Quality = (int)(movie->m_Quality * bonusperc);
				movie->m_Money_Made += movie->m_Promo_Quality;
				income += movie->m_Promo_Quality;
				int degrade = (int)((float)(movie->m_Quality)*0.15);
				movie->m_Quality -= degrade;
				movie->m_RunWeeks++;		// `J` each movie will have its own run tracked
				movie = movie->m_Next;
			}
		}
		current->m_Finance.movie_income(income);
		ss.str("");
		ss << "You earn " << income << " gold from movie income, at your " << current->m_Name;
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	}

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
void cMovieStudioManager::UpdateGirls(sBrothel* brothel)			// Start_Building_Process_B
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cMovieStudio.cpp
	u_int restjob = JOB_FILMFREETIME;
	u_int matronjob = JOB_DIRECTOR;
	u_int firstjob = JOB_FILMFREETIME;
	u_int lastjob = JOB_FILMRANDOM;
	stringstream ss;
	string summary, girlName;
	u_int sw = 0, psw = 0;

	int totalPay = 0, totalTips = 0, totalGold = 0;
	int sum = EVENT_SUMMARY;
	int numgirls = GetNumGirls(brothel->m_id);

	bool matron = false, matrondone = false;
	bool camera = false, crystal = false;
	bool readytofilm = false; // changes to true if both camera and crystal jobs are filled
	bool refused = false;

	m_Processing_Shift = SHIFT_NIGHT;	// there is only a night shift in the movie studio.

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
			(GetNumGirlsOnJob(0, matronjob, SHIFT_NIGHT) > 0 && (current->m_NightJob != matronjob)) ||
			(GetNumGirlsOnJob(0, matronjob, SHIFT_NIGHT) < 1 && (current->m_PrevNightJob != matronjob)))
		{	// Sanity check! Don't process dead girls and only process those with matron jobs
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// `J` so someone is or was a matron

		girlName = current->m_Realname;
		// if there is no matron on duty, we see who was on duty previously
		if (GetNumGirlsOnJob(0, matronjob, SHIFT_NIGHT) < 1)
		{
			// if a matron was found and she is healthy, not tired and not on maternity leave... send her back to work
			if (current->m_PrevNightJob == matronjob &&
				(current->health() >= 50 && current->tiredness() <= 50) &&
				current->m_PregCooldown < cfg.pregnancy.cool_down())
				// Matron job is more important so she will go back to work at 50% instead of regular 80% health and 20% tired
			{
				current->m_DayJob = matronjob;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				current->m_Events.AddMessage("The Director puts herself back to work.", IMGTYPE_PROFILE, EVENT_BACKTOWORK);
			}
			else if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// `J` Now we have a matron so lets see if she will work

		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");

		if (current->disobey_check(ACTION_WORKMATRON, brothel))
		{
			current->m_Refused_To_Work_Night = true;
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work as the Director.";
			sum = EVENT_NOWORK;
		}
		else
		{
			matron = true;
			totalPay = totalTips = totalGold = 0;
			m_JobManager.JobFunc[matronjob](current, brothel, SHIFT_NIGHT, summary);
			totalGold += current->m_Pay + current->m_Tips;




			current->m_Pay += max(0, totalGold);
			current->m_Pay = current->m_Tips = 0;

			brothel->m_Fame += current->fame();
			/* */if (totalGold > 0)		{ ss << girlName << " earned a total of " << totalGold << " gold directly from you. She gets to keep it all."; }
			else if (totalGold == 0)	{ ss << girlName << " made no money."; }
			else if (totalGold < 0)		{ sum = EVENT_DEBUG; ss << "ERROR: She has a loss of " << totalGold << " gold\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << current->m_Realname << "\nJob: " << m_JobManager.JobName[current->m_NightJob] << "\nPay:     " << current->m_Pay << "\nTips:   " << current->m_Tips << "\nTotal: " << totalGold; }
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

		if (current->is_dead() || current->m_NightJob != restjob)
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
			m_JobManager.JobFunc[restjob](current, brothel, SHIFT_NIGHT, summary);
		}
		else if (matron)	// send her back to work
		{
			psw = current->m_PrevNightJob;
			if (psw != restjob && psw != 255)
			{	// if she had a previous job, put her back to work.
				current->m_DayJob = restjob;
				current->m_NightJob = psw;
				ss << "The Director puts " << girlName << " back to work.\n";
			}
			else // otherwise give her a new job.
			{
				ss << "The Director assigns " << girlName << " to ";
				bool assign_actress = false;
				current->m_DayJob = restjob;

				// first, if there are not enough girls in the studio to film a scene, put them to work at something else.
				if (numgirls < 4)	// Director plus only 1 or 2 others
				{
					// if there are movies being sold and noone promoting them, assign a promoter.
					if (brothel->m_NumMovies > 0 && current->is_free() && g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_PROMOTER, 1) < 1)
					{
						current->m_NightJob = JOB_PROMOTER;
						ss << "promote the movies being sold.";
					}
					else // otherwise assign her to clean
					{
						current->m_NightJob = JOB_STAGEHAND;
						ss << "clean the building and take care of the equipment.";
					}
				}
				// second, make sure there is at least 1 camera, 1 crystal and 1 actress
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CAMERAMAGE, 1) < 1)
				{
					current->m_NightJob = JOB_CAMERAMAGE;
					ss << "film the scenes.";
				}
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CRYSTALPURIFIER, 1) < 1)
				{
					current->m_NightJob = JOB_CRYSTALPURIFIER;
					ss << "clean up the filmed scenes.";
				}
				else if (g_Studios.Num_Actress(brothel->m_id) < 1)
				{
					assign_actress = true;
				}

				// if there are a lot of girls in the studio, assign more to camera and crystal
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CAMERAMAGE, 1) < (numgirls / 20) + 1)
				{
					current->m_NightJob = JOB_CAMERAMAGE;
					ss << "film the scenes.";
				}
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CRYSTALPURIFIER, 1) < (numgirls / 20) + 1)
				{
					current->m_NightJob = JOB_CRYSTALPURIFIER;
					ss << "clean up the filmed scenes.";
				}
				// if there are more than 20 girls and no promoter, assign one
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_PROMOTER, 1) < 1 && current->is_free() && numgirls > 20)
				{
					current->m_NightJob = JOB_PROMOTER;
					ss << "advertise the movies.";
				}
				// assign at least 1 stagehand if there are 10 or more girls and 1 more for every 20 after that
				else if (numgirls >= 10 && g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_STAGEHAND, 1) < (numgirls / 20) + 1)
				{
					current->m_NightJob = JOB_STAGEHAND;
					ss << "setup equipment and keep the studio clean.";
				}
				// assign a fluffer if there are more than 20 and 1 more for every 20 after that
				else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_FLUFFER, 1) < (numgirls / 20))
				{
					current->m_NightJob = JOB_FLUFFER;
					ss << "keep the porn stars aroused.";
				}
				else assign_actress = true;

				if (assign_actress)		// everyone else gets assigned to film something they are good at
				{
					sw = JOB_FILMRANDOM;
					int test = 80;
					do // now roll a random number and if that skill is higher than the test number set that as her job
					{
						int testa = g_Dice % 12;
						if (testa == 0 && !m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel))	testa++;
						if (testa == 1 && !m_JobManager.is_sex_type_allowed(SKILL_BDSM, brothel))			testa++;
						if (testa == 2 && !m_JobManager.is_sex_type_allowed(SKILL_GROUP, brothel))			testa++;
						if (testa == 3 && !m_JobManager.is_sex_type_allowed(SKILL_ANAL, brothel))			testa++;
						if (testa == 4 && !m_JobManager.is_sex_type_allowed(SKILL_NORMALSEX, brothel))		testa++;
						if (testa == 5 && !m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))		testa++;
						if (testa == 6 && !m_JobManager.is_sex_type_allowed(SKILL_FOOTJOB, brothel))		testa++;
						if (testa == 7 && !m_JobManager.is_sex_type_allowed(SKILL_HANDJOB, brothel))		testa++;
						if (testa == 8 && !m_JobManager.is_sex_type_allowed(SKILL_ORALSEX, brothel))		testa++;
						if (testa == 9 && !m_JobManager.is_sex_type_allowed(SKILL_TITTYSEX, brothel))		testa++;
						if (testa == 10 && !m_JobManager.is_sex_type_allowed(SKILL_STRIP, brothel))			testa++;
						switch (testa)
						{
						case 0:		if (test <= current->get_skill(SKILL_BEASTIALITY))	{ sw = JOB_FILMBEAST;	ss << "perform in bestiality scenes."; }	break;
						case 1:		if (test <= current->get_skill(SKILL_BDSM))			{ sw = JOB_FILMBONDAGE;	ss << "perform in bondage scenes."; }		break;
						case 2:		if (test <= current->get_skill(SKILL_GROUP))		{ sw = JOB_FILMGROUP;	ss << "perform in group sex scenes."; }		break;
						case 3:		if (test <= current->get_skill(SKILL_ANAL))			{ sw = JOB_FILMANAL;	ss << "perform in anal scenes."; }			break;
						case 4:		if (test <= current->get_skill(SKILL_NORMALSEX))	{ sw = JOB_FILMSEX;		ss << "perform in normal sex scenes."; }	break;
						case 5:		if (test <= current->get_skill(SKILL_LESBIAN))		{ sw = JOB_FILMLESBIAN;	ss << "perform in lesbian scenes."; }		break;
						case 6:		if (test <= current->get_skill(SKILL_FOOTJOB))		{ sw = JOB_FILMFOOTJOB;	ss << "perform in foot job scenes."; }		break;
						case 7:		if (test <= current->get_skill(SKILL_HANDJOB))		{ sw = JOB_FILMHANDJOB;	ss << "perform in hand job scenes."; }		break;
						case 8:		if (test <= current->get_skill(SKILL_ORALSEX))		{ sw = JOB_FILMORAL;	ss << "perform in oral sex scenes."; }		break;
						case 9:		if (test <= current->get_skill(SKILL_TITTYSEX))		{ sw = JOB_FILMTITTY;	ss << "perform in titty fuck scenes."; }	break;
						case 10:	if (test <= current->get_skill(SKILL_STRIP))		{ sw = JOB_FILMSTRIP;	ss << "perform in strip tease scenes."; }	break;
						case 11:	if (test <= current->get_skill(SKILL_PERFORMANCE))	{ sw = JOB_FILMMAST;	ss << "perform in masturbation scenes."; }	break;
						default: break;
						}
						test -= 5;	// after each roll, lower the test number and roll again
					} while (sw == JOB_FILMRANDOM);	// until something is assigned or the test number gets too low
					current->m_NightJob = sw;	// when done set her job (random if the loop failed)
				}
			}
			current->m_PrevDayJob = current->m_PrevNightJob = 255;
			sum = EVENT_BACKTOWORK;
		}
		else if (current->health() < 100 || current->tiredness() > 0)	// if there is no matron to send her somewhere just do resting
		{
			m_JobManager.JobFunc[restjob](current, brothel, SHIFT_NIGHT, summary);
		}
		else	// no one to send her back to work
		{
			ss << "WARNING " << girlName << " is doing nothing!\n";
			sum = EVENT_WARNING;
		}

		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// Pre-Process Crew to make sure there is at least one camera mage and crystal purifier. //
	///////////////////////////////////////////////////////////////////////////////////////////
	current = brothel->m_Girls;
	while (current)
	{
		if (current->is_dead() || (current->m_NightJob != JOB_CAMERAMAGE && current->m_NightJob != JOB_CRYSTALPURIFIER))
		{	// skip dead girls and anyone not working the jobs we are processing
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		// will they do their job?
		if (current->disobey_check(ACTION_WORKMOVIE, brothel))
		{
			// if there is a Director and a camera or crystal refuses to work the director will not be happy.
			if (matron)
			{
				int test = 0; sw = JOB_FILMRANDOM;
				// setting day job as an easy way of tracking what the director makes the refuser do
				current->m_DayJob = current->m_NightJob;
				ss << "Annoyed by her refusal to " << (current->m_DayJob == JOB_CAMERAMAGE ? "film" : "edit");
				ss << " the scenes, the Director ordered " << girlName << " to ";
				// currently this is random but when the morality system gets added, this will be decided by the director's morality
				do // now roll a random number and make sure it is allowed
				{
					int testa = g_Dice % 12;
					if (testa == 0 && !m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel))	testa++;
					if (testa == 1 && !m_JobManager.is_sex_type_allowed(SKILL_BDSM, brothel))			testa++;
					if (testa == 2 && !m_JobManager.is_sex_type_allowed(SKILL_GROUP, brothel))			testa++;
					if (testa == 3 && !m_JobManager.is_sex_type_allowed(SKILL_ANAL, brothel))			testa++;
					if (testa == 4 && !m_JobManager.is_sex_type_allowed(SKILL_NORMALSEX, brothel))		testa++;
					if (testa == 5 && !m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))		testa++;
					if (testa == 6 && !m_JobManager.is_sex_type_allowed(SKILL_FOOTJOB, brothel))		testa++;
					if (testa == 7 && !m_JobManager.is_sex_type_allowed(SKILL_HANDJOB, brothel))		testa++;
					if (testa == 8 && !m_JobManager.is_sex_type_allowed(SKILL_ORALSEX, brothel))		testa++;
					if (testa == 9 && !m_JobManager.is_sex_type_allowed(SKILL_TITTYSEX, brothel))		testa++;
					if (testa == 10 && !m_JobManager.is_sex_type_allowed(SKILL_STRIP, brothel))			testa++;
					switch (testa)	// if that skill is lower than the test number have her do that as punishment
					{
					case 0:		if (test <= current->get_skill(SKILL_BEASTIALITY))	{ sw = JOB_FILMBEAST;	ss << "have sex with a beast"; }	break;
					case 1:		if (test <= current->get_skill(SKILL_BDSM))			{ sw = JOB_FILMBONDAGE;	ss << "get tied up and whipped"; }	break;
					case 2:		if (test <= current->get_skill(SKILL_GROUP))		{ sw = JOB_FILMGROUP;	ss << "have sex with all the extras"; }	break;
					case 3:		if (test <= current->get_skill(SKILL_ANAL))			{ sw = JOB_FILMANAL;	ss << "have anal sex with one of the extras"; }	break;
					case 4:		if (test <= current->get_skill(SKILL_NORMALSEX))	{ sw = JOB_FILMSEX;		ss << "have sex with one of the extras"; }	break;
					case 5:		if (test <= current->get_skill(SKILL_LESBIAN))		{ sw = JOB_FILMLESBIAN;	ss << "have sex with one of the other girls"; }	break;
					case 6:		if (test <= current->get_skill(SKILL_FOOTJOB))		{ sw = JOB_FILMFOOTJOB;	ss << "use her feet on one of the extras"; }	break;
					case 7:		if (test <= current->get_skill(SKILL_HANDJOB))		{ sw = JOB_FILMHANDJOB;	ss << "jack off one of the extras"; }	break;
					case 8:		if (test <= current->get_skill(SKILL_ORALSEX))		{ sw = JOB_FILMORAL;	ss << "blow one of the extras"; }	break;
					case 9:		if (test <= current->get_skill(SKILL_TITTYSEX))		{ sw = JOB_FILMTITTY;	ss << "let one of the extras fuck her tits"; }	break;
					case 10:	if (test <= current->get_skill(SKILL_STRIP))		{ sw = JOB_FILMSTRIP;	ss << "take her clothes off"; }	break;
					case 11:	if (test <= current->get_skill(SKILL_PERFORMANCE))	{ sw = JOB_FILMMAST;	ss << "get herself off"; }	break;
					default: break;
					}
					test += 5;	// after each roll, lower the test number and roll again
				} while (sw == JOB_FILMRANDOM);	// until something is assigned or the test number gets too low
				current->m_NightJob = sw;	// when done set her job (random if the loop failed)

				ss << " while someone else filmed it.";
			}
			else
			{
				current->m_Refused_To_Work_Night = true;
				brothel->m_Fame -= current->fame()/10;
				ss << girlName << " refused to work so made no money.";
			}
		}
		else	// if she did not refuse to work...
		{
			if (current->m_NightJob == JOB_CAMERAMAGE) camera = true;
			if (current->m_NightJob == JOB_CRYSTALPURIFIER) crystal = true;
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next; // Next Girl
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If after the camera and crystal are processed, one of those jobs is vacant, try to have the Director fill it.  //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	current = brothel->m_Girls;
	while (current && matron && (!camera || !crystal))
	{	// skip dead girls, the director and anyone resting (the director would have assigned them a new job already if they were able to work)
		if (current->is_dead() || current->m_NightJob == matronjob || current->m_NightJob == restjob ||
			// skip JOB_CAMERAMAGE and JOB_CRYSTALPURIFIER if there is only one of them
			(current->m_NightJob == JOB_CAMERAMAGE && GetNumGirlsOnJob(brothel->m_id, JOB_CAMERAMAGE, 1) < 2) ||
			(current->m_NightJob == JOB_CRYSTALPURIFIER && GetNumGirlsOnJob(brothel->m_id, JOB_CRYSTALPURIFIER, 1) < 2) ||
			// skip anyone if they have refused already
			(current->m_DayJob != restjob))
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		//		Summary Messages
		if (!current->disobey_check(ACTION_WORKMOVIE, brothel))	// if she did not refuse to work...
		{
			// setting day job as an easy way of tracking what the girl was doing before the director tries to reassign her
			current->m_DayJob = current->m_NightJob;
			if (!camera)
			{
				current->m_NightJob = JOB_CAMERAMAGE;
				camera = true;
			}
			else if (!crystal)
			{
				current->m_NightJob = JOB_CRYSTALPURIFIER;
				crystal = true;
			}
		}
		current = current->m_Next; // Next Girl
	}

	// last check, is there a crew to film?
	readytofilm = (camera && crystal);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If the filming can not procede even after trying to fill the jobs (or there is no Director to fill the jobs)  //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	current = brothel->m_Girls;
	while (current && !readytofilm)
	{
		if (current->is_dead() || current->m_NightJob == restjob || current->m_NightJob == matronjob)
		{	// skip dead girls, resting girls and the director (if there is one)
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;
		if (current->m_NightJob == JOB_STAGEHAND || current->m_NightJob == JOB_PROMOTER) // these two can still work
		{
			totalPay = totalTips = totalGold = 0;
			refused = m_JobManager.JobFunc[current->m_NightJob](current, brothel, SHIFT_NIGHT, summary);
			totalPay += current->m_Pay;
			totalTips += current->m_Tips;
			totalGold += current->m_Pay + current->m_Tips;
			g_Brothels.CalculatePay(brothel, current, current->m_NightJob);

			//		Summary Messages
			if (refused)
			{
				brothel->m_Fame -= current->fame();
				ss << girlName << " refused to work so made no money.";
			}
			else
			{
				ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, SHIFT_NIGHT);
				if (totalGold < 0) sum = EVENT_DEBUG;
				brothel->m_Fame += current->fame();
			}
		}
		else
		{
			ss.str(""); ss << "There was no crew to film the scene, so " << girlName << " took the day off";
			current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;
	}

	////////////////////////
	// Process the Crew.  //
	////////////////////////
	current = brothel->m_Girls;
	while (current && readytofilm)
	{
		if (current->is_dead() || (current->m_NightJob != JOB_PROMOTER && current->m_NightJob != JOB_STAGEHAND &&
			current->m_NightJob != JOB_FLUFFER && current->m_NightJob != JOB_CAMERAMAGE && current->m_NightJob != JOB_CRYSTALPURIFIER)
			// skip dead girls and anyone not working the jobs we are processing
			|| (current->m_NightJob == JOB_CAMERAMAGE && current->m_Refused_To_Work_Night)
			// and skip any camera or crystal who refused to work in the first check and were not reassigned
			|| (current->m_NightJob == JOB_CRYSTALPURIFIER && current->m_Refused_To_Work_Night))
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		if (current->m_NightJob == JOB_CAMERAMAGE || current->m_NightJob == JOB_CRYSTALPURIFIER) summary = "SkipDisobey";
		// do their job
		refused = m_JobManager.JobFunc[current->m_NightJob](current, brothel, SHIFT_NIGHT, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, current->m_NightJob);

		//		Summary Messages
		if (refused)
		{
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, SHIFT_NIGHT);
			if (totalGold < 0) sum = EVENT_DEBUG;

			brothel->m_Fame += current->fame();
		}
		current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next; // Next Girl
	}

	/////////////////////
	// Process Stars.  //
	/////////////////////
	current = brothel->m_Girls;
	while (current && readytofilm)
	{
		sw = current->m_NightJob;
		if (current->is_dead() || sw == restjob || sw == JOB_FLUFFER || sw == JOB_CAMERAMAGE || sw == JOB_CRYSTALPURIFIER || sw == JOB_DIRECTOR || sw == JOB_PROMOTER || sw == JOB_STAGEHAND)
		{	// skip dead girls and already processed jobs
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}

		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		refused = false;
		girlName = current->m_Realname;

		//		// `J` only JOB_FILMRANDOM will check if anything is prohibited. If you put them on a specific job, then it is you breaking the prohibition.
		//		if (sw == JOB_FILMRANDOM)
		//		{
		//			int test = 80;
		//			do // now roll a random number and if that skill is higher than the test number set that as her job
		//			{
		//				int testa = g_Dice % 12;
		//				if (testa == 0 && !m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel))	testa++;
		//				if (testa == 1 && !m_JobManager.is_sex_type_allowed(SKILL_BDSM, brothel))			testa++;
		//				if (testa == 2 && !m_JobManager.is_sex_type_allowed(SKILL_GROUP, brothel))			testa++;
		//				if (testa == 3 && !m_JobManager.is_sex_type_allowed(SKILL_ANAL, brothel))			testa++;
		//				if (testa == 4 && !m_JobManager.is_sex_type_allowed(SKILL_NORMALSEX, brothel))		testa++;
		//				if (testa == 5 && !m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))		testa++;
		//				if (testa == 6 && !m_JobManager.is_sex_type_allowed(SKILL_FOOTJOB, brothel))		testa++;
		//				if (testa == 7 && !m_JobManager.is_sex_type_allowed(SKILL_HANDJOB, brothel))		testa++;
		//				if (testa == 8 && !m_JobManager.is_sex_type_allowed(SKILL_ORALSEX, brothel))		testa++;
		//				if (testa == 9 && !m_JobManager.is_sex_type_allowed(SKILL_TITTYSEX, brothel))		testa++;
		//				if (testa == 10 && !m_JobManager.is_sex_type_allowed(SKILL_STRIP, brothel))			testa++;
		//				switch (testa)
		//				{
		//				case 0:		if (test <= current->get_skill(SKILL_BEASTIALITY))	{ sw = JOB_FILMBEAST;	}	break;
		//				case 1:		if (test <= current->get_skill(SKILL_BDSM))			{ sw = JOB_FILMBONDAGE;	}	break;
		//				case 2:		if (test <= current->get_skill(SKILL_GROUP))		{ sw = JOB_FILMGROUP;	}	break;
		//				case 3:		if (test <= current->get_skill(SKILL_ANAL))			{ sw = JOB_FILMANAL;	}	break;
		//				case 4:		if (test <= current->get_skill(SKILL_NORMALSEX))	{ sw = JOB_FILMSEX;		}	break;
		//				case 5:		if (test <= current->get_skill(SKILL_LESBIAN))		{ sw = JOB_FILMLESBIAN;	}	break;
		//				case 6:		if (test <= current->get_skill(SKILL_FOOTJOB))		{ sw = JOB_FILMFOOTJOB;	}	break;
		//				case 7:		if (test <= current->get_skill(SKILL_HANDJOB))		{ sw = JOB_FILMHANDJOB;	}	break;
		//				case 8:		if (test <= current->get_skill(SKILL_ORALSEX))		{ sw = JOB_FILMORAL;	}	break;
		//				case 9:		if (test <= current->get_skill(SKILL_TITTYSEX))		{ sw = JOB_FILMTITTY;	}	break;
		//				case 10:	if (test <= current->get_skill(SKILL_STRIP))		{ sw = JOB_FILMSTRIP;	}	break;
		//				case 11:	if (test <= current->get_skill(SKILL_PERFORMANCE))	{ sw = JOB_FILMMAST;	}	break;
		//				default: break;
		//				}
		//				test -= 5;	// after each roll, lower the test number and roll again
		//			} while (sw == JOB_FILMRANDOM);	// until something is assigned or the test number gets too low
		//		}
		refused = m_JobManager.JobFunc[sw](current, brothel, SHIFT_NIGHT, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, current->m_NightJob);

		//	Summary Messages
		if (refused)
		{
			brothel->m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, SHIFT_NIGHT);
			if (totalGold < 0) sum = EVENT_DEBUG;

			brothel->m_Fame += current->fame();
		}
		current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

		current = current->m_Next;	// Process next girl
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

		// reset temporary matron reassignment if any
		if (current->m_DayJob != restjob)
		{
			current->m_NightJob = current->m_DayJob;
			current->m_DayJob = restjob;
		}

		// update for girls items that are not used up
		do_daily_items(brothel, current);					// `J` added

		// Level the girl up if nessessary
		g_Girls.LevelUp(current);
		// Natural healing, 2% health and 2% tiredness per day
		current->health(2, false);
		current->tiredness(-2, false);

		sw = current->m_NightJob;
		if (current->happiness()< 40)
		{
			if (sw != matronjob && matron && brothel->m_NumGirls > 1 && g_Dice.percent(70))
			{
				ss << "The Director helps cheer up " << girlName << " when she is feeling sad.\n";
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
			ss << "As Director, " << girlName << " has the keys to the store room.\nShe used them to 'borrow' ";
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
					ss << "The Director takes " << girlName << " off duty to rest due to her ";
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
						ss << "The Director helps ";
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

	m_Processing_Shift = -1;			// WD: Finished Processing Shift set flag
}

TiXmlElement* cMovieStudioManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("MovieStudio_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	pBrothelManager->SetAttribute("TotalScenesFilmed", m_TotalScenesFilmed);
	string message;

	// save Studio
	TiXmlElement* pBrothels = new TiXmlElement("MovieStudios");
	pBrothelManager->LinkEndChild(pBrothels);
	sMovieStudio* current = (sMovieStudio*)m_Parent;
	//         ...................................................
	message = "***************** Saving studios *****************";
	g_LogFile.write(message);
	while (current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveMovieStudioXML(pBrothels);
		current = (sMovieStudio*)current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sMovieStudio::SaveMovieStudioXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("MovieStudio");
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
	pBrothel->SetAttribute("MovieRunTime", m_MovieRunTime);
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
	pBrothel->SetAttribute("AutoCreateMovies", cfg.initial.autocreatemovies());


	TiXmlElement* pMovies = new TiXmlElement("Movies");
	pBrothel->LinkEndChild(pMovies);
	sMovie* movie = m_Movies;
	while (movie)
	{
		TiXmlElement* pMovie = new TiXmlElement("Movie");
		pMovies->LinkEndChild(pMovie);
		pMovie->SetAttribute("Name", movie->m_Name);
		pMovie->SetAttribute("Director", movie->m_Director);
		pMovie->SetAttribute("Cast", movie->m_Cast);
		pMovie->SetAttribute("Crew", movie->m_Crew);
		pMovie->SetAttribute("Init_Qual", movie->m_Init_Quality);
		pMovie->SetAttribute("Qual", movie->m_Quality);
		pMovie->SetAttribute("Promo_Qual", movie->m_Promo_Quality);
		pMovie->SetAttribute("Money_Made", movie->m_Money_Made);
		pMovie->SetAttribute("RunWeeks", movie->m_RunWeeks);
		movie = movie->m_Next;
	}
	// `J` Save Scenes added for .06.02.55
	TiXmlElement* pScenes = new TiXmlElement("Scenes");
	pBrothel->LinkEndChild(pScenes);
	for (int i = 0; i < g_Studios.GetNumScenes(); i++)
	{
		TiXmlElement* pScene = new TiXmlElement("Scene");
		pScenes->LinkEndChild(pScene);
		pScene->SetAttribute("SceneNumber", g_Studios.GetScene(i)->m_SceneNum);
		pScene->SetAttribute("Name", g_Studios.GetScene(i)->m_Name);
		pScene->SetAttribute("Actress", g_Studios.GetScene(i)->m_Actress);
		pScene->SetAttribute("Director", g_Studios.GetScene(i)->m_Director);
		pScene->SetAttribute("CM", g_Studios.GetScene(i)->m_CM);
		pScene->SetAttribute("CP", g_Studios.GetScene(i)->m_CP);
		pScene->SetAttribute("Job", g_Brothels.m_JobManager.JobQkNm[g_Studios.GetScene(i)->m_Job]);
		pScene->SetAttribute("Init_Quality", g_Studios.GetScene(i)->m_Init_Quality);
		pScene->SetAttribute("Quality", g_Studios.GetScene(i)->m_Quality);
		pScene->SetAttribute("Promo_Quality", g_Studios.GetScene(i)->m_Promo_Quality);
		pScene->SetAttribute("Money_Made", g_Studios.GetScene(i)->m_Money_Made);
		pScene->SetAttribute("RunWeeks", g_Studios.GetScene(i)->m_RunWeeks);
	}
	for (int i = 0; i < g_Studios.GetNumMovieScenes(); i++)
	{
		TiXmlElement* pScene = new TiXmlElement("Scene");
		pScenes->LinkEndChild(pScene);
		pScene->SetAttribute("SceneNumber", g_Studios.GetMovieScene(i)->m_SceneNum);
		pScene->SetAttribute("Name", g_Studios.GetMovieScene(i)->m_Name);
		pScene->SetAttribute("Actress", g_Studios.GetMovieScene(i)->m_Actress);
		pScene->SetAttribute("Director", g_Studios.GetMovieScene(i)->m_Director);
		pScene->SetAttribute("CM", g_Studios.GetMovieScene(i)->m_CM);
		pScene->SetAttribute("CP", g_Studios.GetMovieScene(i)->m_CP);
		pScene->SetAttribute("Job", g_Brothels.m_JobManager.JobQkNm[g_Studios.GetMovieScene(i)->m_Job]);
		pScene->SetAttribute("Init_Quality", g_Studios.GetMovieScene(i)->m_Init_Quality);
		pScene->SetAttribute("Quality", g_Studios.GetMovieScene(i)->m_Quality);
		pScene->SetAttribute("Promo_Quality", g_Studios.GetMovieScene(i)->m_Promo_Quality);
		pScene->SetAttribute("Money_Made", g_Studios.GetMovieScene(i)->m_Money_Made);
		pScene->SetAttribute("RunWeeks", g_Studios.GetMovieScene(i)->m_RunWeeks);
	}
		

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

bool cMovieStudioManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too

	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0)
	{
		return false;
	}

	if (pBrothelManager->Attribute("TotalScenesFilmed"))
	{
		int tempInt = 0;
		pBrothelManager->QueryIntAttribute("TotalScenesFilmed", &tempInt); m_TotalScenesFilmed = tempInt; tempInt = 0;
	}
	else m_TotalScenesFilmed = 0;

	string message = "";
	//         ...................................................
	message = "***************** Loading studio ****************";
	g_LogFile.write(message);
	m_NumMovieStudios = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("MovieStudios");
	if (pBrothels)
	{
		for (TiXmlElement* pBrothel = pBrothels->FirstChildElement("MovieStudio");
			pBrothel != 0;
			pBrothel = pBrothel->NextSiblingElement("MovieStudio"))
		{
			sMovieStudio* current = new sMovieStudio();
			bool success = current->LoadMovieStudioXML(TiXmlHandle(pBrothel));
			if (success == true)
			{
				AddBrothel(current);
			}
			else
			{
				delete current;
				continue;
			}

		} // load a studio
	}
	return true;
}

bool sMovieStudio::LoadMovieStudioXML(TiXmlHandle hBrothel)
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

	std::string message = "Loading studio: ";
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
	pBrothel->QueryIntAttribute("MovieRunTime", &m_MovieRunTime);
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
	
	bool m_autocreatemovies = false;
	pBrothel->QueryValueAttribute<bool>("AutoCreateMovies", &m_autocreatemovies);
	cfg.initial.autocreatemovies() = m_autocreatemovies;

	m_NumMovies = 0;
	TiXmlElement* pMovies = pBrothel->FirstChildElement("Movies");
	if (pMovies)
	{
		for (TiXmlElement* pMovie = pMovies->FirstChildElement("Movie");
			pMovie != 0;
			pMovie = pMovie->NextSiblingElement("Movie"))
		{
			string Name		= (pMovie->Attribute("Name") ? pMovie->Attribute("Name") : "");
			string Director = (pMovie->Attribute("Director") ? pMovie->Attribute("Director") : "");
			string Cast		= (pMovie->Attribute("Cast") ? pMovie->Attribute("Cast") : "");
			string Crew		= (pMovie->Attribute("Crew") ? pMovie->Attribute("Crew") : "");
			long init_quality = 0;
			long quality = 0;
			long promo_quality = 0;
			long money_made = 0;
			long runweeks = -1;
			pMovie->QueryValueAttribute<long>("Init_Qual", &init_quality);
			pMovie->QueryValueAttribute<long>("Qual", &quality);
			pMovie->QueryValueAttribute<long>("Promo_Qual", &promo_quality);
			pMovie->QueryValueAttribute<long>("Money_Made", &money_made);
			pMovie->QueryValueAttribute<long>("RunWeeks", &runweeks);
			if (runweeks < 0)
			{
				runweeks = m_MovieRunTime - m_NumMovies;
			}
			//when you create a new movie, you set m_Quality to quality*0.5
			//but you directly save m_Quality, so this undoes the division
			// --PP Changed quality to be equal instead of half, to increase movie value.
			// quality *= 2;
			g_Studios.NewMovie(this, Name, Director, Cast, Crew, init_quality, quality, promo_quality, money_made, runweeks);

		}
	}
	// `J` load scenes added for .06.02.55
	TiXmlElement* pScenes = pBrothel->FirstChildElement("Scenes");
	if (pScenes)
	{
		for (TiXmlElement* pScene = pScenes->FirstChildElement("Scene");
			pScene != 0;
			pScene = pScene->NextSiblingElement("Scene"))
		{
			int scenenum; int mscenenum; int job;
			long init_quality = 0; long quality = 0; long promo_quality = 0; long money_made = 0; long runweeks = -1;

			pScene->QueryIntAttribute("SceneNumber", &scenenum);
			if (pScene->Attribute("MovieSceneNumber")) 
				{ pScene->QueryIntAttribute("MovieSceneNumber", &mscenenum); }
			else { mscenenum = -1; }
			string name = (pScene->Attribute("Name") ? pScene->Attribute("Name") : "");
			string actress = (pScene->Attribute("Actress") ? pScene->Attribute("Actress") : "");
			string director = (pScene->Attribute("Director") ? pScene->Attribute("Director") : "");
			string cm = (pScene->Attribute("CM") ? pScene->Attribute("CM") : "");
			string cp = (pScene->Attribute("CP") ? pScene->Attribute("CP") : "");
			job = (pScene->Attribute("Job") ? sGirl::lookup_jobs_code(pScene->Attribute("Job")) : JOB_FILMRANDOM);
			pScene->QueryValueAttribute<long>("Init_Quality", &init_quality);
			pScene->QueryValueAttribute<long>("Quality", &quality);
			pScene->QueryValueAttribute<long>("Promo_Quality", &promo_quality);
			pScene->QueryValueAttribute<long>("Money_Made", &money_made);
			pScene->QueryValueAttribute<long>("RunWeeks", &runweeks);
			g_Studios.LoadScene(scenenum, name, actress, director, job, init_quality, quality, promo_quality, money_made, runweeks, mscenenum, cm, cp);
		}
	}

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
				girl->m_InStudio = true;
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

// ----- Get / Set
sMovieScene* cMovieStudioManager::GetScene(int num)
{
	return g_Studios.m_availableScenes.at(num);
}

int cMovieStudioManager::GetNumScenes()
{
	return g_Studios.m_availableScenes.size();
}
sMovieScene* cMovieStudioManager::GetMovieScene(int num)
{
	return g_Studios.m_movieScenes.at(num);
}
int cMovieStudioManager::GetNumMovieScenes()
{
	return g_Studios.m_movieScenes.size();
}

vector<int> cMovieStudioManager::AddSceneToMovie(int num)
{
	if (num >= 0)
	{
		sMovieScene* scene = g_Studios.GetScene(num);
		scene->m_RowM = g_Studios.GetNumMovieScenes();
		scene->m_MovieSceneNum = g_Studios.GetNumMovieScenes() + 1;
		m_availableScenes.erase(m_availableScenes.begin() + num);
		m_movieScenes.push_back(scene);
		SortMovieScenes();
	}
	return{ (num >= g_Studios.GetNumScenes() ? g_Studios.GetNumScenes() - 1 : num), g_Studios.GetNumMovieScenes() - 1 };
}
vector<int> cMovieStudioManager::RemoveSceneFromMovie(int num)
{
	if (num >= 0)
	{
		sMovieScene* scene = g_Studios.GetMovieScene(num);
		scene->m_RowM = -1;
		scene->m_MovieSceneNum = -1;
		m_movieScenes.erase(m_movieScenes.begin() + num);
		m_availableScenes.push_back(scene);
		SortMovieScenes();
	}
	return { g_Studios.GetNumScenes() - 1, num - 1 };
}
int cMovieStudioManager::MovieSceneUp(int num)
{
	if (num > 0)
	{
		m_movieScenes[num]->m_RowM--;
		m_movieScenes[num]->m_MovieSceneNum--;
		m_movieScenes[num - 1]->m_RowM++;
		m_movieScenes[num - 1]->m_MovieSceneNum++;
		SortMovieScenes();
	}
	return num - 1;
}
int cMovieStudioManager::MovieSceneDown(int num)
{
	if (num >= 0 && num < g_Studios.GetNumMovieScenes() - 1)
	{
		m_movieScenes[num]->m_RowM++;
		m_movieScenes[num]->m_MovieSceneNum++;
		m_movieScenes[num + 1]->m_RowM--;
		m_movieScenes[num + 1]->m_MovieSceneNum--;
		SortMovieScenes();
	}
	return num + 1;
}
int cMovieStudioManager::DeleteScene(int num)
{
	if (num >= 0)
	{
		m_availableScenes.erase(m_availableScenes.begin() + num);
	}
	if (num >= g_Studios.GetNumScenes()) num = g_Studios.GetNumScenes();
	return num;
}
bool SortMovieScene(sMovieScene* a, sMovieScene* b)
{
	return (a->m_RowM < b->m_RowM);
}
void cMovieStudioManager::SortMovieScenes()
{
	if (g_Studios.GetNumMovieScenes() < 1) { return; }										// if there are none we don't need to sort it.
	std::sort(m_movieScenes.begin(), m_movieScenes.end(), SortMovieScene);

	for (int i = 0; i < g_Studios.GetNumMovieScenes(); i++)									// we need to go through the scenes and find out what order they are in
	{
		sMovieScene* scene = g_Studios.GetMovieScene(i);
		scene->m_RowM = i;
		scene->m_MovieSceneNum = i + 1;
	}

}


void sMovieScene::OutputSceneRow(string* Data, const vector<string>& columnNames)
{
	for (unsigned int x = 0; x < columnNames.size(); ++x)
	{
		//for each column, write out the statistic that goes in it
		OutputSceneDetailString(Data[x], columnNames[x]);
	}
}

void sMovieScene::OutputSceneDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");
	/* */if (detailName == "SceneNumber")		{ ss << m_SceneNum; }
	else if (detailName == "MovieSceneNumber")	{ ss << m_MovieSceneNum; }
	else if (detailName == "Name")				{ ss << m_Name; }
	else if (detailName == "Actress")			{ ss << m_Actress; }
	else if (detailName == "Director")			{ ss << m_Director; }
	else if (detailName == "Camera Mage")		{ ss << m_CM; }
	else if (detailName == "CameraMage")		{ ss << m_CM; }
	else if (detailName == "Camera_Mage")		{ ss << m_CM; }
	else if (detailName == "Camera")			{ ss << m_CM; }
	else if (detailName == "CM")				{ ss << m_CM; }
	else if (detailName == "Crystal Purifier")	{ ss << m_CP; }
	else if (detailName == "CrystalPurifier")	{ ss << m_CP; }
	else if (detailName == "Crystal_Purifier")	{ ss << m_CP; }
	else if (detailName == "Crystal")			{ ss << m_CP; }
	else if (detailName == "CP")				{ ss << m_CP; }
	else if (detailName == "Job")				{ ss << g_Brothels.m_JobManager.JobName[m_Job]; }
	else if (detailName == "Init_Quality")		{ ss << m_Init_Quality; }
	else if (detailName == "Quality")			{ ss << m_Quality; }
	else if (detailName == "Promo_Quality")		{ ss << m_Promo_Quality; }
	else if (detailName == "Money_Made")		{ ss << m_Money_Made; }
	else if (detailName == "RunWeeks")			{ ss << m_RunWeeks; }
	else /*                               */	{ ss << "Not found"; }
	Data = ss.str();
}

// ----- Movie
void cMovieStudioManager::StartMovie(int brothelID, int Time)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	current->m_ShowQuality = 0;
	current->m_ShowTime = Time;
}

int cMovieStudioManager::GetTimeToMovie(int brothelID)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	return current->m_ShowTime;
}

void cMovieStudioManager::NewMovie(sMovieStudio* brothel, string Name, string Director, string Cast, string Crew, int Init_Quality, int Quality, int Promo_Quality, int Money_Made, int RunWeeks)
{
	sMovie* newMovie = new sMovie();
	newMovie->m_Name = Name;
	newMovie->m_Director = Director;
	newMovie->m_Cast = Cast;
	newMovie->m_Crew = Crew;
	newMovie->m_Init_Quality = Init_Quality;
	newMovie->m_Promo_Quality = Promo_Quality;
	newMovie->m_Quality = Quality;
	newMovie->m_Money_Made = Money_Made;
	newMovie->m_RunWeeks = RunWeeks;

	if (brothel->m_NumMovies > 0)
	{
		brothel->m_LastMovies->m_Next = newMovie;
		brothel->m_LastMovies = newMovie;
	}
	else
		brothel->m_LastMovies = brothel->m_Movies = newMovie;
	brothel->m_NumMovies++;
}

void cMovieStudioManager::EndMovie(sBrothel* brothel)
{
	if (brothel->m_Movies)
	{
		sMovie* movie = brothel->m_Movies;
		brothel->m_Movies = movie->m_Next;
		movie->m_Next = 0;
		delete movie;
		movie = 0;
		brothel->m_NumMovies--;
	}
}

int cMovieStudioManager::Num_Actress(int brothel)
{
	// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > Num_Actress
	int actresses = 0;
	for (int i = JOB_FILMACTION; i < JOB_FILMRANDOM + 1; i++)
	{
		actresses += GetNumGirlsOnJob(0, i, 1);
	}

	return actresses;
}

bool cMovieStudioManager::is_Actress_Job(int testjob)
{
	// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >>cMovieStudio.cpp > is_Actress_Job
	return (testjob > JOB_STAGEHAND && testjob <= JOB_FILMRANDOM);
}

bool cMovieStudioManager::CrewNeeded()	// `J` added, if CM and CP both on duty or there are no actresses, return false
{
	// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > CrewNeeded
	if ((GetNumGirlsOnJob(0, JOB_CAMERAMAGE, 1) > 0 &&
		GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, 1) > 0) ||
		g_Studios.Num_Actress(0) < 1)
		return false;	// a CM or CP is not Needed
	return true;	// Otherwise a CM or CP is Needed
}

// ----- Add / remove
int cMovieStudioManager::AddScene(sGirl* girl, int Job, int Bonus, sGirl* Director, sGirl* CM, sGirl* CP)
{
	sMovieStudio* current = (sMovieStudio*)m_Parent;
                                           
	stringstream ss;
	string girlName = girl->m_Realname;

	if (!Director) { Director = g_Studios.GetRandomGirlOnJob(0, JOB_DIRECTOR, SHIFT_NIGHT); }
	if (!CM) { CM = g_Studios.GetRandomGirlOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT); }
	if (!CP) { CP = g_Studios.GetRandomGirlOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT); }

	// NOTE i crazy added this to try and improve the movies before it only check for normalsex skill now it should check for each skill type i hope
	// Fixed so it will check for skill type being used --PP
	long quality = 0;
	quality += Bonus;
	quality += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP


	// Balance is...
	// Evil jobs should pay better and show higher 'quality' - but damage health, happiness, love and can possibly cause injury/disease
	// nice jobs should pay less, but be safe and boost fame, skills, happiness and health.
	// normal jobs should be normal.
	//
	enum { NICE, NORMAL, EVIL };
	int jobType = NORMAL;
	int Skill = SKILL_NORMALSEX;
	switch (Job)
	{
		// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > AddScene
	case JOB_FILMACTION:		jobType = NORMAL;	quality -= 8;	Skill = SKILL_COMBAT;		ss << "Action";			break;
	case JOB_FILMCHEF:			jobType = NICE;		quality -= 8;	Skill = SKILL_COOKING;		ss << "Cooking";		break;
		//case JOB_FILMCOMEDY:		jobType = NICE;		quality -= 2;	Skill = SKILL_PERFORMANCE;	ss << "Comedy";			break;
		//case JOB_FILMDRAMA:		jobType = NICE;		quality -= 2;	Skill = SKILL_PERFORMANCE;	ss << "Drama";			break;
		//case JOB_FILMHORROR:		jobType = NORMAL;	quality -= 2;	Skill = SKILL_PERFORMANCE;	ss << "Horror";			break;
		//case JOB_FILMIDOL:		jobType = NICE;		quality -= 5;	Skill = SKILL_PERFORMANCE;	ss << "Idol";			break;
	case JOB_FILMMUSIC:			jobType = NICE;		quality -= 3;	Skill = SKILL_PERFORMANCE;	ss << "Music";			break;
	case JOB_FILMMAST:			jobType = NICE;		quality -= 10;	Skill = SKILL_PERFORMANCE;	ss << "Masturbation";	break;
	case JOB_FILMSTRIP:			jobType = NICE;		quality += 0;	Skill = SKILL_STRIP;		ss << "Stripping";		break;
	case JOB_FILMTEASE:			jobType = NICE;		quality += 0;	Skill = SKILL_STRIP;		ss << "Teaser";			break;
	case JOB_FILMANAL:			jobType = NORMAL;	quality += 5;	Skill = SKILL_ANAL;			ss << "Anal";			break;
	case JOB_FILMFOOTJOB:		jobType = NORMAL;	quality -= 5;	Skill = SKILL_FOOTJOB;		ss << "Footjob";		break;
		//case JOB_FILMFUTA:		jobType = NORMAL;	quality += 5;	Skill = SKILL_NORMALSEX;	ss << "Futa";			break;
	case JOB_FILMHANDJOB:		jobType = NORMAL;	quality -= 5;	Skill = SKILL_HANDJOB;		ss << "Handjob";		break;
	case JOB_FILMLESBIAN:		jobType = NORMAL;	quality += 10;	Skill = SKILL_LESBIAN;		ss << "Lesbian";		break;
	case JOB_FILMORAL:			jobType = NORMAL;	quality -= 5;	Skill = SKILL_ORALSEX;		ss << "Oral";			break;
	case JOB_FILMSEX:			jobType = NORMAL;	quality += 0;	Skill = SKILL_NORMALSEX;	ss << "Sex";			break;
	case JOB_FILMTITTY:			jobType = NORMAL;	quality -= 5;	Skill = SKILL_TITTYSEX;		ss << "Titty";			break;
	case JOB_FILMBEAST:			jobType = EVIL;		quality += 5;	Skill = SKILL_BEASTIALITY;	ss << "Beast";			break;
	case JOB_FILMBONDAGE:		jobType = EVIL;		quality += 8;	Skill = SKILL_BDSM;			ss << "Bondage";		break;
	case JOB_FILMBUKKAKE:		jobType = EVIL;		quality += 5;	Skill = SKILL_GROUP;		ss << "Bukkake";		break;
	case JOB_FILMFACEFUCK:		jobType = EVIL;		quality += 5;	Skill = SKILL_ORALSEX;		ss << "Facefucking";	break;
	case JOB_FILMGROUP:			jobType = NORMAL;	quality += 5;	Skill = SKILL_GROUP;		ss << "Group";			break;
	case JOB_FILMPUBLICBDSM:	jobType = EVIL;		quality += 10;	Skill = SKILL_BDSM;			ss << "Public BDSM";	break;
		//case JOB_FILMDOM:			jobType = EVIL;		quality += 8;	Skill = SKILL_BDSM;			ss << "Dominatrix";		break;
	default:		ss << "Movie ";		break;
	}
	ss << " scene by " << girlName;

	// `J` do job based modifiers
	quality += girl->get_skill(Skill) / 5;


	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cMovieStudioManager::AddScene

	//CRAZY added this to have traits play a bigger part in the movies
	if (girl->has_trait("Porn Star"))					quality += 20;
	if (girl->has_trait("Actress"))						quality += 10;
	if (girl->has_trait("Shape Shifter"))				quality += 10;

	if (girl->has_trait("Fake Orgasm Expert"))			quality += 5;
	else if (girl->has_trait("Fast Orgasms"))			quality += 2;
	else if (girl->has_trait("Slow Orgasms"))			quality -= 2;

	if (girl->has_trait("Great Figure"))				quality += 4;
	if (girl->has_trait("Great Arse"))					quality += 2;
	if (girl->has_trait("Charismatic"))					quality += 4;
	if (girl->has_trait("Charming"))					quality += 2;
	if (girl->has_trait("Long Legs"))					quality += 2;
	if (girl->has_trait("Nymphomaniac"))				quality += 4;
	if (girl->has_trait("Good Kisser"))					quality += 2;
	if (girl->has_trait("Cute"))						quality += 2;
	if (girl->has_trait("Sexy Air"))					quality += 2;
	if (girl->has_trait("Psychic"))						quality += 4;
	if (girl->has_trait("Dick-Sucking Lips"))			quality += 1;

	if (girl->has_trait("Chaste"))						quality -= 2;
	if (girl->has_trait("Manly"))						quality -= 2;
	if (girl->has_trait("Fragile"))						quality -= 2;
	if (girl->has_trait("Mind Fucked"))					quality -= 4;
	if (girl->has_trait("Nervous"))						quality -= 2;
	if (girl->has_trait("Horrific Scars"))				quality -= 4;
	if (girl->has_trait("Clumsy"))						quality -= 2;
	if (girl->has_trait("Meek"))						quality -= 2;
	if (girl->has_trait("Aggressive"))					quality -= 2;
	if (girl->has_trait("Broken Will"))					quality -= 4;
	if (girl->has_trait("Dependant"))					quality -= 3;
	if (girl->has_trait("Shy"))							quality -= 3;


	// Idk if this is needed or not but can't hurt CRAZY // `J` breast size quality for non titjob is less importatnt
	if (girl->has_trait("Flat Chest"))					quality -= (Job == JOB_FILMTITTY ? 10 : 2);
	if (girl->has_trait("Petite Breasts"))				quality -= (Job == JOB_FILMTITTY ? 5 : 1);
	if (girl->has_trait("Small Boobs"))					quality -= (Job == JOB_FILMTITTY ? 1 : 0);
	if (girl->has_trait("Busty Boobs"))					quality += (Job == JOB_FILMTITTY ? 2 : 0);
	if (girl->has_trait("Big Boobs"))					quality += (Job == JOB_FILMTITTY ? 4 : 1);
	if (girl->has_trait("Giant Juggs"))					quality += (Job == JOB_FILMTITTY ? 6 : 2);
	if (girl->has_trait("Massive Melons"))				quality += (Job == JOB_FILMTITTY ? 8 : 3);
	if (girl->has_trait("Abnormally Large Boobs"))		quality += (Job == JOB_FILMTITTY ? 10 : 4);
	if (girl->has_trait("Titanic Tits"))				quality += (Job == JOB_FILMTITTY ? 12 : 5);
	if (girl->has_trait("Perky Nipples"))				quality += (Job == JOB_FILMTITTY ? 2 : 1);
	if (girl->has_trait("Puffy Nipples"))				quality += (Job == JOB_FILMTITTY ? 2 : 1);
	if (girl->has_trait("Pierced Nipples"))
	{
		if (Job == JOB_FILMBONDAGE || Job == JOB_FILMTITTY)
			quality += 5;
		if (Job == JOB_FILMSEX || Job == JOB_FILMBEAST || Job == JOB_FILMGROUP || Job == JOB_FILMLESBIAN || Job == SKILL_STRIP)
			quality += 1;
	}
	if (girl->has_trait("Pierced Navel"))
	{
		if (Job == JOB_FILMBONDAGE || Job == SKILL_STRIP)
			quality += 5;
		if (Job == JOB_FILMTITTY)
			quality += 2;
		if (Job == JOB_FILMSEX || Job == JOB_FILMBEAST || Job == JOB_FILMGROUP || Job == JOB_FILMLESBIAN)
			quality += 1;
	}
	if (girl->has_trait("Pierced Clit"))
	{
		if (Job == JOB_FILMBONDAGE)		quality += 5;
		if (Job == JOB_FILMLESBIAN)	quality += 3;
		if (Job == JOB_FILMANAL || Job == JOB_FILMSEX || Job == JOB_FILMGROUP || Job == SKILL_STRIP)
			quality += 1;
	}
	if (girl->has_trait("Hunter"))
	{
		if (Job == JOB_FILMBEAST)	quality += 3;
	}
	if (Job == JOB_FILMORAL || Job == JOB_FILMFACEFUCK)
	{
		if (girl->has_trait("Pierced Tongue"))		quality += 1;
		if (girl->has_trait("Dick-Sucking Lips"))	quality += 2;
		if (girl->has_trait("Deep Throat"))			quality += 5;
		if (girl->has_trait("No Gag Reflex"))		quality += 2;
		if (girl->has_trait("Gag Reflex"))			quality -= 5;
		if (girl->has_trait("Strong Gag Reflex"))	quality -= 10;
	}

	if (Job == JOB_FILMANAL) //May need work FIXME CRAZY
	{
		if (girl->has_trait("Great Arse"))			quality += 10;
		if (girl->has_trait("Tight Butt"))			quality += 8;
		if (girl->has_trait("Phat Booty"))			quality += 6;
		if (girl->has_trait("Wide Bottom"))			quality += 4;
		if (girl->has_trait("Plump Tush"))			quality += 2;
		if (girl->has_trait("Flat Ass"))			quality -= 10;
	}

	if (girl->has_trait("Lesbian"))
	{
		//a lesbian would be more into it and give a better show I would think CRAZY
		if (Job == JOB_FILMLESBIAN)					quality += 10;
		// `J` and she would be less into doing it with a guy
		if (Job == JOB_FILMANAL)					quality -= 10;
		if (Job == JOB_FILMGROUP)					quality -= 10;
		if (Job == JOB_FILMORAL)					quality -= 10;
		if (Job == JOB_FILMSEX)						quality -= 5;
		if (Job == JOB_FILMTITTY)					quality -= 4;
		if (Job == JOB_FILMFOOTJOB)					quality -= 2;
		if (Job == JOB_FILMHANDJOB)					quality -= 2;
		if (Job == JOB_FILMMAST)					quality -= 1;
	}
	if (girl->has_trait("Straight"))
	{
		// `J` similarly, a straight girl would be less into doing it with another girl (but not as much)
		if (Job == JOB_FILMLESBIAN)					quality -= 5;
		if (Job == JOB_FILMANAL)					quality += 2;
		if (Job == JOB_FILMSEX)						quality += 5;
		if (Job == JOB_FILMGROUP)					quality += 1;
		if (Job == JOB_FILMORAL)					quality += 1;
		if (Job == JOB_FILMTITTY)					quality += 2;
		if (Job == JOB_FILMFOOTJOB)					quality += 2;
		if (Job == JOB_FILMHANDJOB)					quality += 2;
		if (Job == JOB_FILMMAST)					quality += 1;	// foot job needs to be added so will use service until then
	}


	//CRAZY added this better looking girls should make better quality movies
	// Changed to work with new job revision --PP
	quality += girl->performance() / 10;
	quality += (girl->charisma() - 50) / 10;
	quality += (girl->beauty() - 50) / 10;
	quality += girl->fame();
	quality += girl->level();

#if 0
	int performance = quality;
#else
	int performance = 0;

	if (jobType <= NORMAL)
	{
		performance = quality;
	}
	else
	{
		performance = quality;			//Performance rate set
		quality += (quality / 2);		//Quality increased 50% - FIRST GUESS - MAY NEED BALANCING!!
	}

#endif

	// Add bonus for Fluffer, CameraMage and CrystalPurifier --PP
	if (Job != JOB_FILMMAST && Job != JOB_FILMLESBIAN && Job != JOB_FILMSTRIP)	// No fluffers needed --PP no need for strip either CRAZY
	quality += g_Studios.m_FlufferQuality;

	quality += g_Studios.m_CameraQuality;
	quality += g_Studios.m_PurifierQaulity;
	quality += g_Studios.m_DirectorQuality;

	quality += g_Studios.m_StagehandQuality;


	LoadScene(
		 /* int m_SceneNum			*/	++g_Studios.m_TotalScenesFilmed
		,/* string m_Name 			*/	ss.str()
		,/* string m_Actress 		*/	girl->m_Realname
		,/* string m_Director 		*/	Director->m_Realname
		,/* int m_Job 				*/	Job
		,/* long m_Init_Quality 	*/	quality
		,/* long m_Quality 			*/	quality
		,/* long m_Promo_Quality 	*/	0
		,/* long m_Money_Made 		*/	0
		,/* long m_RunWeeks 		*/	0
		,/* int m_MovieSceneNum 	*/	-1
		,/* string m_CM 			*/	CM->m_Realname
		,/* string m_CP				*/	CP->m_Realname
		);
	return performance;
}
void cMovieStudioManager::LoadScene(int m_SceneNum, string m_Name, string m_Actress, string m_Director, int m_Job, long m_Init_Quality, long m_Quality, long m_Promo_Quality, long m_Money_Made, long m_RunWeeks, int m_MovieSceneNum, string m_CM, string m_CP)
{
	sMovieScene* newScene = new sMovieScene();
	newScene->m_SceneNum = m_SceneNum;
	newScene->m_MovieSceneNum = m_MovieSceneNum;
	newScene->m_Name = m_Name;
                                   
	newScene->m_Actress = m_Actress;
	newScene->m_Director = m_Director;
	newScene->m_CM = m_CM;
	newScene->m_CP = m_CP;
	newScene->m_Job = m_Job;
	newScene->m_Init_Quality = m_Init_Quality;
	newScene->m_Quality = m_Quality;
	newScene->m_Promo_Quality = m_Promo_Quality;
	newScene->m_Money_Made = m_Money_Made;
	newScene->m_RunWeeks = m_RunWeeks;
	m_availableScenes.push_back(newScene);
}

long cMovieStudioManager::calc_movie_quality(bool autoreleased)
{
	stringstream ss;
	long quality = 0;


	if (autoreleased)
	{
		int numscenes = m_availableScenes.size();
		int makerqual = g_Dice % 10 + 1;
		int thrownout = 0;
		int edited = 0;
		sGirl* maker = GetFirstGirlOnJob(0, JOB_DIRECTOR, SHIFT_NIGHT);
		if (!maker)	{ makerqual = g_Dice % 5 + 1;	maker = GetFirstGirlOnJob(0, JOB_PROMOTER, SHIFT_NIGHT); }
		if (!maker)	{ makerqual = g_Dice % 3;		maker = GetRandomGirlOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT); }
		if (!maker)	{ makerqual = g_Dice % 3;		maker = GetRandomGirlOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT); }

		if (!maker)
		{
			makerqual = 0;
			ss << "A movie was automatically made from the scenes that are ready for use";
		}
		else
		{
			quality += makerqual;
			if (maker->m_NightJob == JOB_DIRECTOR)			ss << "Director ";
			if (maker->m_NightJob == JOB_PROMOTER)			ss << "Promoter ";
			ss << maker->m_Realname << " produced a movie from the scenes shot last week";
			for (int i = 0; i < (int)m_availableScenes.size(); i++)
			{
				if (m_availableScenes[i]->m_Quality < 0)
				{
					m_availableScenes[i]->m_Quality = 0;
					thrownout++;
				}
			}
			if (thrownout >= numscenes * 8 / 10)
			{
				ss << ".\nThe movie was total crap but " << maker->m_Realname << " chopped it up and put it out as a blooper reel";
				numscenes = thrownout + g_Dice%thrownout;
			}
			else if (thrownout>numscenes / 2)
			{
				ss << ".\nMore than half of the scenes were crap but " << maker->m_Realname << " chopped them up and added them as a blooper reel at the end of the movie";
				numscenes -= thrownout;
				numscenes += g_Dice%thrownout;
			}
			else if (thrownout > 0)
			{
				ss << ".\nShe threw out ";
				/* */if (thrownout == 1)	ss << "one";
				else if (thrownout == 1)	ss << "two";
				else/*                */	ss << "several";
				ss << " scenes that didn't meet her standards";
				numscenes -= thrownout;
			}
		}
		ss << ".\n \n";
  
		for (int i = 0; i < (int)m_availableScenes.size(); i++)
		{
			quality += m_availableScenes[i]->m_Quality;
		}
		quality += numscenes * 10;
		ss << "This movie will sell at " << quality << " gold, for 35 weeks, but it's value will drop over time.\n \n";
		ss << (g_Studios.GetNumGirlsOnJob(0, JOB_PROMOTER, 0) > 0 ? "Your" : "A");
		ss << " promoter with an advertising budget will help it sell for more.";
	}
	else
	{
		int numscenes = m_movieScenes.size();
		for (int i = 0; i < numscenes; i++)
		{
			quality += m_movieScenes[i]->m_Quality;
		}
		quality += numscenes * 10;
		ss << "This movie will sell at " << quality << " gold, for 35 weeks, but it's value will drop over time.\n \n";
		ss << (g_Studios.GetNumGirlsOnJob(0, JOB_PROMOTER, 0) > 0 ? "Your" : "A");
		ss << " promoter with an advertising budget will help it sell for more.";

	}
	g_MessageQue.AddToQue(ss.str(), COLOR_BLUE);
	g_InitWin = true;
	return quality;
}

void cMovieStudioManager::ReleaseCurrentMovie(bool autoreleased, bool save)
{
	sMovieStudio* current = (sMovieStudio*)m_Parent;
	string Name = "";
	string Director = BuildDirectorList(autoreleased,true);
	string Cast = BuildCastList(autoreleased, true);
	string Crew = BuildCrewList(autoreleased, true);
	long init_quality = calc_movie_quality(autoreleased);
	long quality = init_quality;	// calculate movie quality
	long promo_quality = 0;
	long money_made = 0;
	long runweeks = 0;

	if (autoreleased) {	// old code
		m_availableScenes.clear();					// clear scene list
	}
	else
	{
		m_movieScenes.clear();						// clear scene list
                                                                                               
	}
	NewMovie(current, Name, Director, Cast, Crew, init_quality, quality, promo_quality, money_made, runweeks);	//add new movie

}
string cMovieStudioManager::BuildDirectorList(bool autoreleased, bool save)
{
	vector<string> names;
	vector<string> namesB;
	stringstream ss;
	if (autoreleased)
	{
		for (int i = 0; i < (int)m_availableScenes.size(); i++)
		{
			
			if (m_availableScenes[i]->m_Director != "") names.push_back(m_availableScenes[i]->m_Director);
		}
	}
	else
	{
		for (int i = 0; i < (int)m_movieScenes.size(); i++)
		{
			if (m_movieScenes[i]->m_Director != "") names.push_back(m_movieScenes[i]->m_Director);
		}
	}
	if (names.size() <= 0) { return ""; }
	namesB = names;
	std::sort(names.begin(), names.end());
	names.resize(std::distance(names.begin(),std::unique(names.begin(), names.end())));

	if (!save)
	{
		ss << "Director" << (names.size() == 1 ? ":  " : "s:  ");
	}
	for (int i = 0; i < (int)names.size(); i++)
	{
		ss << names[i];
		if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
	}

	return ss.str();
}
string cMovieStudioManager::BuildCastList(bool autoreleased, bool save)
{
	vector<string> names;
	vector<string> namesB;
	stringstream ss;
	if (autoreleased)
	{
		for (int i = 0; i < (int)m_availableScenes.size(); i++)
		{
			if (m_availableScenes[i]->m_Actress != "") names.push_back(m_availableScenes[i]->m_Actress);
		}
	}
	else
	{
		for (int i = 0; i < (int)m_movieScenes.size(); i++)
		{
			if (m_movieScenes[i]->m_Actress != "") names.push_back(m_movieScenes[i]->m_Actress);
		}
	}
	if (names.size() <= 0) { return ""; }

	namesB = names;
	std::sort(names.begin(), names.end());
	names.resize(std::distance(names.begin(), std::unique(names.begin(), names.end())));

	if (!save)
	{
		ss << "Actress" << (names.size() == 1 ? ":  " : "es:  ");
	}
	for (int i = 0; i < (int)names.size(); i++)
	{
		ss << names[i];
		if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
	}

	return ss.str();
}
string cMovieStudioManager::BuildCrewList(bool autoreleased, bool save)
{
	vector<string> names;
	vector<string> namesB;
	stringstream ss;
	if (autoreleased)
	{
		for (int i = 0; i < (int)m_availableScenes.size(); i++)
		{
			if (m_availableScenes[i]->m_CM != "") names.push_back(m_availableScenes[i]->m_CM);
			if (m_availableScenes[i]->m_CP != "") names.push_back(m_availableScenes[i]->m_CP);
		}
	}
	else
	{
		for (int i = 0; i < (int)m_movieScenes.size(); i++)
		{
			if (m_movieScenes[i]->m_CM != "") names.push_back(m_movieScenes[i]->m_CM);
			if (m_movieScenes[i]->m_CP != "") names.push_back(m_movieScenes[i]->m_CP);
		}
	}
	if (names.size() <= 0) { return ""; }

	namesB = names;
	std::sort(names.begin(), names.end());
	names.resize(std::distance(names.begin(), std::unique(names.begin(), names.end())));

	if (!save)
	{
		ss << "Crew:  ";
	}
	for (int i = 0; i < (int)names.size(); i++)
	{
		ss << names[i];
		if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
	}

	return ss.str();
}