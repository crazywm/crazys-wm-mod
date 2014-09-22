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
sMovieStudio::sMovieStudio() :	m_Finance(0)	// constructor
{
	m_var				= 0;
	m_Name				= "studio";
	m_Filthiness		= 0;
	m_Next				= 0;
	m_Girls				= 0;
	m_LastGirl			= 0;
	m_NumGirls			= 0;
	m_SecurityLevel		= 0;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;
	m_CurrFilm			= 0;
	m_NumMovies			= 0;
	m_LastMovies		= 0;
	m_Movies			= 0;
	m_MovieRunTime		= 0;
}

sMovieStudio::~sMovieStudio()			// destructor
{
	m_var			= 0;
	if (m_Next)		delete m_Next;
	m_Next			= 0;
	if (m_Girls)	delete m_Girls;
	m_LastGirl		= 0;
	m_Girls			= 0;
	m_NumMovies		= 0;
	if (m_Movies)	delete m_Movies;
	m_Movies		= 0;
	m_LastMovies	= 0;
	if (m_CurrFilm)	delete m_CurrFilm;
}

void cMovieStudioManager::AddGirl(int brothelID, sGirl* girl)
{
	girl->where_is_she		= 0;
	girl->m_InMovieStudio	= true;
	girl->m_InArena			= false;
	girl->m_InCentre		= false;
	girl->m_InClinic		= false;
	girl->m_InFarm			= false;
	girl->m_InHouse			= false;
	cBrothelManager::AddGirl(brothelID, girl);
}

void cMovieStudioManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InMovieStudio = false;
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
void cMovieStudioManager::UpdateMovieStudio()
{
	sBrothel* current = (sBrothel*) m_Parent;
	current->m_Finance.zero();
	// Clear the girls' events from the last turn
	sGirl* cgirl = current->m_Girls;
	while(cgirl)
	{
		cgirl->where_is_she		= 0;
		cgirl->m_InMovieStudio	= true;
		cgirl->m_InArena		= false;
		cgirl->m_InCentre		= false;
		cgirl->m_InClinic		= false;
		cgirl->m_InFarm			= false;
		cgirl->m_InHouse		= false;
		cgirl->m_Pay			= 0;
		cgirl->m_Events.Clear();
		cgirl = cgirl->m_Next;
	}
	m_FlufferQuality	= 0;
	m_CameraQuality		= 0;
	m_PurifierQaulity	= 0;
	m_DirectorQuality	= 0;
	
	if (current->m_Filthiness < 0) current->m_Filthiness = 0;

	// update the girls and satisfy the customers for this brothel during the night
	UpdateGirls(current);

	// Update movies currently being sold
	sMovie* movie = current->m_Movies;
	if(current->m_NumMovies > 0)
	{
		current->m_MovieRunTime++;
		/* `J` Replaceing the global MovieRunTime with individual movie->m_RunWeeks below
		if(current->m_MovieRunTime == 35)
		{
			EndMovie(current);
			current->m_MovieRunTime = 0;
		}
		*/
		long income = 0;
		if(current->m_NumMovies > 0)
		{
			double bonusperc = g_Studios.m_PromoterBonus / 100;
			while (movie)
			{
				if (movie->m_RunWeeks > 34)
				{
					movie->m_Promo_Quality = 0;
					movie->m_Quality = 0;
					movie->m_RunWeeks += 1;
					movie = movie->m_Next;
				}
				else
				{
					movie->m_Promo_Quality = (int)(movie->m_Quality * bonusperc);
					movie->m_Money_Made += movie->m_Promo_Quality;
					income += movie->m_Promo_Quality;
					int degrade = (int)((float)(movie->m_Quality)*0.15);
					movie->m_Quality -= degrade;
					movie->m_RunWeeks += 1;		// `J` each movie will have its own run tracked
					movie = movie->m_Next;
				}
			}
			current->m_Finance.movie_income(income);
			g_Gold.brothel_accounts(current->m_Finance, current->m_id);
			stringstream ss;
			ss.str("");
			ss << "You earn " << income << " gold from movie income, at your " << current->m_Name;
			g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
		}
	}
}

// End of turn stuff is here
// Same method than Brothel but different job
void cMovieStudioManager::UpdateGirls(sBrothel* brothel)
{
	// `J` added to allow for easier copy/paste to other buildings
	u_int restjob = JOB_FILMFREETIME;
	u_int matronjob = JOB_DIRECTOR;
	u_int firstjob = JOB_FILMBEAST;
	u_int lastjob = JOB_STAGEHAND;
	bool matron = false;
	string MatronMsg = "", MatronWarningMsg = "";
	bool camera = false;
	bool crystal = false;
	bool readytofilm = true; // changes to false if both camera and crystal jobs are not filled

	cConfig cfg;
	sGirl* current = brothel->m_Girls;
	sGirl* DeadGirl = 0;
	string summary, msg, girlName;
	int totalPay = 0, totalTips = 0, totalGold = 0;

	int sum = EVENT_SUMMARY;
	int numgirls = GetNumGirls(brothel->m_id);
	u_int sw = 0;						//	Job type
	bool refused = false;
	m_Processing_Shift = SHIFT_NIGHT;	// there is only a night shift in the movie studio.

	// `J` Handle the base checks for all girls first
	while (current)
	{
		current->m_Pay = current->m_Tips = 0;
		brothel->m_Filthiness++;
		// Remove any dead bodies from last week
		if (current->health() <= 0)
		{
			brothel->m_Filthiness++; // `J` Death is messy
			DeadGirl = current;
			if (current->m_Next) current = current->m_Next; else current = 0;
			// increase all the girls fear and hate of the player for letting her die (weather his fault or not)
			UpdateAllGirlsStat(brothel, STAT_PCFEAR, 2); UpdateAllGirlsStat(brothel, STAT_PCHATE, 1);

			// Do the messages
			msg += girlName + gettext(" has died from her injuries, the other girls all fear and hate you a little more.");
			DeadGirl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			g_MessageQue.AddToQue(msg, COLOR_RED);
			summary += girlName + gettext(" has died from her injuries.  Her body will be removed by the end of the week.");
			DeadGirl->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);
			msg = ""; summary = "";

			RemoveGirl(0, DeadGirl, true); DeadGirl = 0;
			if (current) continue; else break;
		}
		if (current->m_DayJob < firstjob && current->m_DayJob > lastjob)		current->m_DayJob = restjob;
		if (current->m_NightJob < firstjob && current->m_NightJob > lastjob)	current->m_NightJob = restjob;
		current->m_YesterDayJob = current->m_DayJob;		// `J` set what she did yesterday
		current->m_YesterNightJob = current->m_NightJob;	// `J` set what she did yesternight
		if (current->m_JustGaveBirth)		// if she gave birth, let her rest this week
		{
			if (current->m_NightJob != restjob)	current->m_PrevNightJob = current->m_NightJob;
			current->m_NightJob = restjob;
		}

		// `J` update stuff. only 1 shift in studio so bunch it all together.
		g_Girls.UseItems(current);					// Girl uses items she has
		do_food_and_digs(brothel, current);			// Brothel only update for girls accommodation level
		g_Girls.updateGirlAge(current, true);		// update birthday counter and age the girl
		g_Girls.updateTempStats(current);			// update temp stats
		g_Girls.updateTempSkills(current);			// update temp skills
		g_Girls.updateTempTraits(current);			// update temp traits
		g_Girls.HandleChildren(current, summary);	// handle pregnancy and children growing up
		g_Girls.updateSTD(current);					// health loss to STD's		NOTE: Girl can die
		g_Girls.updateHappyTraits(current);			// Update happiness due to Traits		NOTE: Girl can die
		updateGirlTurnBrothelStats(current);		// Update daily stats	Now only runs once per day
		g_Girls.updateGirlTurnStats(current);		// Stat Code common to Dugeon and Brothel
		g_Girls.CalculateGirlType(current);			// update the fetish traits
		g_Girls.CalculateAskPrice(current, true);	// Calculate the girls asking price

		current = current->m_Next; // Next Girl
	}


	// Process Matron first incase she refuses to work.
	current = brothel->m_Girls;
	bool matrondone = false;
	while (current && !matrondone)
	{
		if (current->health() <= 0 || (brothel->matron_on_shift(SHIFT_NIGHT, false, true) && current->m_NightJob != matronjob) || (!brothel->matron_on_shift(SHIFT_NIGHT, false, true) && current->m_PrevNightJob != matronjob))
		{	// Sanity check! Don't process dead girls and only process those with matron jobs
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;

		// `J` so someone is or was a matron
		// if there is no matron on duty, we see who was on duty previously
		if (!brothel->matron_on_shift(SHIFT_NIGHT, false, true))
		{
			// if a matron was found and she is healthy, not tired and not on maternity leave... send her back to work
			// Matron job is more important so she will go back to work at 50% instead of regular 80% health and 20% tired
			if (current->m_PrevNightJob == matronjob &&
				(g_Girls.GetStat(current, STAT_HEALTH) >= 50 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 50) &&
				current->m_PregCooldown < cfg.pregnancy.cool_down())
			{
				g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevNightJob, current->m_NightJob, true);
				current->m_DayJob = restjob;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				MatronMsg += gettext("The Director puts herself back to work.\n");
				current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, EVENT_BACKTOWORK);
				MatronMsg = "";
			}
			else if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// `J` Now we have a matron so lets see if she will work

		current->m_Pay = current->m_Tips = totalPay = totalTips = totalGold = 0;
		refused = false;
		sum = EVENT_SUMMARY;

		refused = m_JobManager.JobFunc[matronjob](current, brothel, SHIFT_NIGHT, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, matronjob);

		if (refused)
		{
			matron = false;
			brothel->m_Fame -= g_Girls.GetStat(current, STAT_FAME);
			g_Girls.AddTiredness(current);		// if she refused she still gets tired
			summary += girlName + gettext(" refused to work so made no money.");
		}
		else
		{
			matron = true;
			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			if (totalGold > 0) 
			{ 
				stringstream ss; ss << girlName << " earned a total of " << totalGold << " gold directly from you. She gets to keep it all."; summary += ss.str();
			}
			else if (totalGold == 0) summary += girlName + gettext(" made no money.");
			else if (totalGold < 0) 
			{ 
				sum = EVENT_DEBUG; 
				stringstream ss; ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org"; summary += ss.str(); 
			}
		}
		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);
		summary = "";

		current = current->m_Next;	// Next Girl
		matrondone = true;			// there can be only one matron so this ends the while loop
	}

	// `J` Now If there is a matron and she is not refusing to work, then she can delegate the girls in this building.

	// Back to work
	current = brothel->m_Girls;
	while (current)
	{
		if (current->health() <= 0 || current->m_NightJob != restjob)		
		{	// skip dead girls and anyone not resting
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;

		if (current->m_PregCooldown < cfg.pregnancy.cool_down() && g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20)
		{	// if she is healthy enough to go back to work... 
			if (matron)	// and there is a marton working...
			{
				if (current->m_PrevNightJob != restjob && current->m_PrevNightJob != 255)	// 255 = nothing, in other words no previous job stored
				{	// if she had a previous job, put her back to work.
					current->m_DayJob = restjob;
					current->m_NightJob = current->m_PrevNightJob;
					MatronMsg += "The Director puts " + girlName + " back to work.\n";
				}
				else // otherwise give her a new job.
				{
					MatronMsg += "The Director assigns " + girlName + " to ";

					string swt = "";
					current->m_DayJob = restjob;
					// if there are a lot of girls in the studio, assign more to camera and crystal
					if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CAMERAMAGE, 1) < (numgirls / 20) + 1)
					{
						current->m_NightJob = JOB_CAMERAMAGE;
						swt = "film the scenes.\n";
					}
					else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_CRYSTALPURIFIER, 1) < (numgirls / 20) + 1)
					{
						current->m_NightJob = JOB_CRYSTALPURIFIER;
						swt = "clean up the filmed scenes.\n";
					}
					// if there are more than 20 girls and no promoter, assign one
					else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_PROMOTER, 1) < 1 && numgirls > 20)
					{
						current->m_NightJob = JOB_PROMOTER;
						swt = "advertise the movies.\n";
					}
					// assign a fluffer and stagehand if there are more than 20 and 1 more for every 20 after that
					else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_FLUFFER, 1) < (numgirls / 20))
					{
						current->m_NightJob = JOB_FLUFFER;
						swt = "keep the porn stars aroused.\n";
					}
					else if (g_Studios.GetNumGirlsOnJob(brothel->m_id, JOB_STAGEHAND, 1) < (numgirls / 20))
					{
						current->m_NightJob = JOB_STAGEHAND;
						swt = "setup equipment, and keep the studio clean.\n";
					}
					else	// everyone else gets assigned to film something they are good at
					{
						sw = JOB_FILMRANDOM;
						swt = "perform in random sex scenes";
						// load the girls stats but if banned set to -10
						int s__b = (m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel)) ? current->get_skill(SKILL_BEASTIALITY) : -10;
						int s__n = (m_JobManager.is_sex_type_allowed(SKILL_NORMALSEX, brothel)) ? current->get_skill(SKILL_NORMALSEX) : -10;
						int s__a = (m_JobManager.is_sex_type_allowed(SKILL_ANAL, brothel)) ? current->get_skill(SKILL_ANAL) : -10;
						int s__l = (m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel)) ? current->get_skill(SKILL_LESBIAN) : -10;
						int s__d = (m_JobManager.is_sex_type_allowed(SKILL_BDSM, brothel)) ? current->get_skill(SKILL_BDSM) : -10;
						int s__g = (m_JobManager.is_sex_type_allowed(SKILL_GROUP, brothel)) ? current->get_skill(SKILL_GROUP) : -10;
						int s__o = (m_JobManager.is_sex_type_allowed(SKILL_ORALSEX, brothel)) ? current->get_skill(SKILL_ORALSEX) : -10;
						int s__t = (m_JobManager.is_sex_type_allowed(SKILL_TITTYSEX, brothel)) ? current->get_skill(SKILL_TITTYSEX) : -10;
						int s__h = (m_JobManager.is_sex_type_allowed(SKILL_HANDJOB, brothel)) ? current->get_skill(SKILL_HANDJOB) : -10;
						int s__f = current->get_skill(SKILL_SERVICE);
						int s__s = current->get_skill(SKILL_STRIP);

						int test = 80;
						do // now roll a random number and if that skill is higher than the test number set that as her job
						{
							switch (g_Dice % 12)
							{
							case 0:		if (test <= s__b) { sw = JOB_FILMBEAST;		swt = "perform in bestiality scenes"; }		break;
							case 1:		if (test <= s__n) { sw = JOB_FILMSEX;		swt = "perform in normal sex scenes"; }		break;
							case 2:		if (test <= s__a) { sw = JOB_FILMANAL;		swt = "perform in anal scenes"; }			break;
							case 3:		if (test <= s__l) { sw = JOB_FILMLESBIAN;	swt = "perform in lesbian scenes"; }		break;
							case 4:		if (test <= s__d) { sw = JOB_FILMBONDAGE;	swt = "perform in bondage scenes"; }		break;
							case 5:		if (test <= s__g) { sw = JOB_FILMGROUP;		swt = "perform in group sex scenes"; }		break;
							case 6:		if (test <= s__o) { sw = JOB_FILMORAL;		swt = "perform in oral sex scenes"; }		break;
							case 7:		if (test <= s__t) { sw = JOB_FILMTITTY;		swt = "perform in titty fuck scenes"; }		break;
							case 8:		if (test <= s__h) { sw = JOB_FILMHANDJOB;	swt = "perform in hand job scenes"; }		break;
							case 9:		if (test <= s__s) { sw = JOB_FILMMAST;		swt = "perform in masturbation scenes"; }	break;
							case 10:	if (test <= s__f) { sw = JOB_FILMFOOTJOB;	swt = "perform in foot job scenes"; }		break;
							case 11:	if (test <= s__s) { sw = JOB_FILMSTRIP;		swt = "perform in strip tease scenes"; }	break;
							default: break;
							}
							test -= 2;	// after each roll, lower the test number roll again
						} while (sw == JOB_FILMRANDOM && test > 0);	// until something is assigned or the test number gets too low
						current->m_NightJob = sw;	// when done set her job (random if the loop failed)
					}
					MatronMsg += swt + ".\n";
				}
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, EVENT_BACKTOWORK);
				MatronMsg = "";
			}
			else
			{
				current->m_DayJob = current->m_NightJob = restjob;
				MatronWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
				current->m_Events.AddMessage(MatronWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
				MatronWarningMsg = "";
			}
		}
		current = current->m_Next;
	}

	// Pre-Process Crew to make sure there is at least one camera mage and crystal purifier
	current = brothel->m_Girls;
	while (current)
	{
		if (current->health() <= 0 || (current->m_NightJob != JOB_CAMERAMAGE && current->m_NightJob != JOB_CRYSTALPURIFIER))
		{	// skip dead girls and anyone not working the jobs we are processing
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		current->m_Pay = current->m_Tips = totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY;

		// do their job
		sw = current->m_NightJob;
		refused = m_JobManager.JobFunc[current->m_NightJob](current, brothel, SHIFT_NIGHT, summary);

		//		Summary Messages
		if (refused)
		{
			// if there is a Director and a camera or crystal refuses to work the director will not be happy.
			if (matron)
			{
				// setting day job as an easy way of tracking what the director makes the refuser do
				current->m_DayJob = current->m_NightJob;
				string swt = "";
				summary += "Annoyed by her refusal to ";
				summary += (current->m_DayJob == JOB_CAMERAMAGE) ? "film" : "edit";
				summary += " the scenes, the Director ordered " + current->m_Realname + " to ";

				// currently this is random but when the morality system gets added, this will be decided by the director's morality
				int skill = SKILL_STRIP;
				do	{
					int maxroll = JOB_FILMRANDOM - 1 - JOB_FILMBEAST;
					int roll = g_Dice%maxroll;
					sw = roll + JOB_FILMBEAST;
					switch (sw)
					{
					case JOB_FILMBEAST:				skill = SKILL_BEASTIALITY;	swt = "have sex with a beast";	break;
					case JOB_FILMSEX:				skill = SKILL_NORMALSEX;	swt = "have sex with one of the extras";	break;
					case JOB_FILMANAL:				skill = SKILL_ANAL;			swt = "have anal sex with one of the extras";	break;
					case JOB_FILMLESBIAN:			skill = SKILL_LESBIAN;		swt = "have sex with one of the other girls";	break;
					case JOB_FILMBONDAGE:			skill = SKILL_BDSM;			swt = "get tied up and whipped";	break;
					case JOB_FILMGROUP:				skill = SKILL_GROUP;		swt = "have sex with all the extras";	break;
					case JOB_FILMORAL:				skill = SKILL_ORALSEX;		swt = "blow one of the extras";	break;
					case JOB_FILMTITTY:				skill = SKILL_TITTYSEX;		swt = "let one of the extras fuck her tits";	break;
					case JOB_FILMHANDJOB:			skill = SKILL_HANDJOB;		swt = "jack off one of the extras";	break;
					case JOB_FILMFOOTJOB:			skill = SKILL_SERVICE;		swt = "use her feet on one of the extras";	break;
					case JOB_FILMMAST:				skill = SKILL_STRIP;		swt = "get herself off";	break;
					case JOB_FILMSTRIP:	default:	skill = SKILL_STRIP;		swt = "take her clothes off";	break;
					}
				} while (!(m_JobManager.is_sex_type_allowed(skill, brothel)));
				current->m_NightJob = sw;
				summary += swt + " while someone else filmed it.";
			}
			else
			{
				brothel->m_Fame -= g_Girls.GetStat(current, STAT_FAME);
				g_Girls.AddTiredness(current);
				summary += girlName + gettext(" refused to work so made no money.");
			}
		}
		else	// if she did not refuse to work...
		{
			if (current->m_NightJob == JOB_CAMERAMAGE) camera = true;
			if (current->m_NightJob == JOB_CRYSTALPURIFIER) crystal = true;

			totalPay += current->m_Pay;
			totalTips += current->m_Tips;
			totalGold += current->m_Pay + current->m_Tips;
			g_Brothels.CalculatePay(brothel, current, current->m_NightJob);


			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			if (totalGold > 0)
			{
				stringstream ss;
				ss << girlName << " earned a total of " << totalGold << " gold";
				// if it is a player paid job and she is not a slave
				if ((m_JobManager.is_job_Paid_Player(current->m_NightJob) && !current->is_slave()) ||
					// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
					(m_JobManager.is_job_Paid_Player(current->m_NightJob) && current->is_slave() && cfg.initial.slave_pay_outofpocket()))
					ss << " directly from you. She gets to keep it all.";
				else if (current->house() <= 0)				ss << " and she gets to keep it all.";
				else if (totalTips>0 && ((cfg.initial.girls_keep_tips() && !current->is_slave()) || (cfg.initial.slave_keep_tips() && current->is_slave())))
				{
					int hpay = int(double(totalPay * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalPay - hpay;
					ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut (" << 100 - current->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay << " gold.\n\nYou got " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				else
				{
					int hpay = int(double(totalGold * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalGold - hpay;
					ss << ".\nShe keeps " << gpay << " gold. (" << 100 - current->m_Stats[STAT_HOUSE] << "%)\nYou keep " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				summary += ss.str();
			}
			else if (totalGold == 0) summary += girlName + gettext(" made no money.");
			else if (totalGold < 0)
			{
				stringstream ss;
				ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
				summary += ss.str();
				sum = EVENT_DEBUG;
			}
		}
		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);
		summary = "";

		current = current->m_Next; // Next Girl
	}

	// If after the camera and crystal are processed, one of those jobs is vacant, try to fill it.
	while (current && matron && (!camera || !crystal))
	{	// skip dead girls, the director and anyone resting (the director would have assigned them a new job already if they were able to work)
		if (current->health() <= 0 || current->m_NightJob == matronjob || current->m_NightJob == restjob ||
			// skip JOB_CAMERAMAGE and JOB_CRYSTALPURIFIER if there is only one of them
			(current->m_NightJob == JOB_CAMERAMAGE && GetNumGirlsOnJob(brothel->m_id, JOB_CAMERAMAGE, 1) < 2) ||
			(current->m_NightJob == JOB_CRYSTALPURIFIER && GetNumGirlsOnJob(brothel->m_id, JOB_CRYSTALPURIFIER, 1) < 2))
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		// setting day job as an easy way of tracking what the girl was doing before the director tries to reassign her
		current->m_DayJob = current->m_NightJob;
		current->m_Pay = current->m_Tips = totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY;

		// now to assign the new job and see if she will do it
		if (!camera)		current->m_NightJob = JOB_CAMERAMAGE;
		else if (!crystal)	current->m_NightJob = JOB_CRYSTALPURIFIER;
		refused = m_JobManager.JobFunc[current->m_NightJob](current, brothel, SHIFT_NIGHT, summary);

		//		Summary Messages
		if (refused) // put her back on her other job
		{
			current->m_NightJob = current->m_DayJob;
			current->m_DayJob = restjob;
		}
		else	// if she did not refuse to work...
		{
			totalPay += current->m_Pay;
			totalTips += current->m_Tips;
			totalGold += current->m_Pay + current->m_Tips;
			g_Brothels.CalculatePay(brothel, current, current->m_NightJob);

			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			if (totalGold > 0)
			{
				stringstream ss;
				ss << girlName << " earned a total of " << totalGold << " gold";
				// if it is a player paid job and she is not a slave
				if ((m_JobManager.is_job_Paid_Player(current->m_NightJob) && !current->is_slave()) ||
					// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
					(m_JobManager.is_job_Paid_Player(current->m_NightJob) && current->is_slave() && cfg.initial.slave_pay_outofpocket()))
					ss << " directly from you. She gets to keep it all.";
				else if (current->house() <= 0)				ss << " and she gets to keep it all.";
				else if (totalTips>0 && ((cfg.initial.girls_keep_tips() && !current->is_slave()) || (cfg.initial.slave_keep_tips() && current->is_slave())))
				{
					int hpay = int(double(totalPay * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalPay - hpay;
					ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut (" << 100 - current->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay << " gold.\n\nYou got " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				else
				{
					int hpay = int(double(totalGold * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalGold - hpay;
					ss << ".\nShe keeps " << gpay << " gold. (" << 100 - current->m_Stats[STAT_HOUSE] << "%)\nYou keep " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				summary += ss.str();
			}
			else if (totalGold == 0) summary += girlName + gettext(" made no money.");
			else if (totalGold < 0)
			{
				stringstream ss;
				ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
				summary += ss.str();
				sum = EVENT_DEBUG;
			}
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);
			summary = "";
			if (current->m_NightJob == JOB_CAMERAMAGE) camera = true;
			if (current->m_NightJob == JOB_CRYSTALPURIFIER) crystal = true;
		}
		current = current->m_Next; // Next Girl
	}

	// last check, is there a crew to film?
	readytofilm = (camera && crystal) ? true : false;

	// if the filming can not procede even after trying to fill the jobs (or there is no Director to fill the jobs)
	while (current && !readytofilm)
	{
		if (current->health() <= 0 || current->m_NightJob != matronjob)
		{	// skip dead girls and the director (if there is one)
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;
		summary = "There was no crew to film the scene, so " + girlName + " took the day off";
		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_NOWORK);
		g_Girls.AddTiredness(current);

		current = current->m_Next;
	}

	// Process the rest of the Crew
	current = brothel->m_Girls;
	while (current && readytofilm)
	{
		if (current->health() <= 0 || (current->m_NightJob != JOB_PROMOTER && current->m_NightJob != JOB_STAGEHAND && current->m_NightJob != JOB_FLUFFER))
		{	// skip dead girls and anyone not working the jobs we are processing
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		current->m_Pay = current->m_Tips = totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY;

		// do their job
		refused = m_JobManager.JobFunc[current->m_NightJob](current, brothel, SHIFT_NIGHT, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, current->m_NightJob);
		//		Summary Messages
		if (refused)
		{
			brothel->m_Fame -= g_Girls.GetStat(current, STAT_FAME);
			g_Girls.AddTiredness(current);
			summary += girlName + gettext(" refused to work so made no money.");
		}
		else
		{
			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			if (totalGold > 0)
			{
				stringstream ss;
				ss << girlName << " earned a total of " << totalGold << " gold";
				// if it is a player paid job and she is not a slave
				if ((m_JobManager.is_job_Paid_Player(current->m_NightJob) && !current->is_slave()) ||
					// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
					(m_JobManager.is_job_Paid_Player(current->m_NightJob) && current->is_slave() && cfg.initial.slave_pay_outofpocket()))
					ss << " directly from you. She gets to keep it all.";
				else if (current->house() <= 0)				ss << " and she gets to keep it all.";
				else if (totalTips>0 && ((cfg.initial.girls_keep_tips() && !current->is_slave()) || (cfg.initial.slave_keep_tips() && current->is_slave())))
				{
					int hpay = int(double(totalPay * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalPay - hpay;
					ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut (" << 100 - current->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay << " gold.\n\nYou got " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				else
				{
					int hpay = int(double(totalGold * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalGold - hpay;
					ss << ".\nShe keeps " << gpay << " gold. (" << 100 - current->m_Stats[STAT_HOUSE] << "%)\nYou keep " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				summary += ss.str();
			}
			else if (totalGold == 0) summary += girlName + gettext(" made no money.");
			else if (totalGold < 0)
			{
				stringstream ss;
				ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
				summary += ss.str();
				sum = EVENT_DEBUG;
			}
		}
		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);
		summary = "";

		current = current->m_Next; // Next Girl
	}

	// Process Stars
	current = brothel->m_Girls;
	while (current && readytofilm)
	{
		sw = current->m_NightJob;
		if (current->health() <= 0 || sw == restjob || sw == JOB_FLUFFER || sw == JOB_CAMERAMAGE || sw == JOB_CRYSTALPURIFIER || sw == JOB_DIRECTOR || sw == JOB_PROMOTER || sw == JOB_STAGEHAND)
		{	// skip dead girls and already processed jobs
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}

		current->m_Pay = current->m_Tips = totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;

		// `J` only JOB_FILMRANDOM will check if anything is prohibited. If you put them on a specific job, then it is you breaking the prohibition.
		if (sw == JOB_FILMRANDOM)
		{
			int skill = SKILL_STRIP;
			do	{
				int maxroll = JOB_FILMRANDOM - 1 - JOB_FILMBEAST;
				int roll = g_Dice%maxroll;
				sw = roll + JOB_FILMBEAST;
				switch (sw)
				{
				case JOB_FILMBEAST:		skill = SKILL_BEASTIALITY;	break;
				case JOB_FILMSEX:		skill = SKILL_NORMALSEX;	break;
				case JOB_FILMANAL:		skill = SKILL_ANAL;			break;
				case JOB_FILMLESBIAN:	skill = SKILL_LESBIAN;		break;
				case JOB_FILMBONDAGE:	skill = SKILL_BDSM;			break;
				case JOB_FILMGROUP:		skill = SKILL_GROUP;		break;
				case JOB_FILMORAL:		skill = SKILL_ORALSEX;		break;
				case JOB_FILMTITTY:		skill = SKILL_TITTYSEX;		break;
				case JOB_FILMHANDJOB:	skill = SKILL_HANDJOB;		break;
					// strip and mast are the only ones that can not be prohibited.
				case JOB_FILMMAST:
				case JOB_FILMFOOTJOB:
				case JOB_FILMSTRIP:		skill = SKILL_STRIP;		break;
				default:					break;
				}
			} while (!(m_JobManager.is_sex_type_allowed(skill, brothel)));
		}
		refused = m_JobManager.JobFunc[sw](current, brothel, SHIFT_NIGHT, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		g_Brothels.CalculatePay(brothel, current, sw);
		//	Summary Messages
		if (refused)
		{
			brothel->m_Fame -= g_Girls.GetStat(current, STAT_FAME);
			g_Girls.AddTiredness(current);
			summary += girlName + gettext(" refused to work so made no money.");
		}
		else
		{
			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			if (totalGold > 0)
			{
				stringstream ss;
				ss << girlName << " earned a total of " << totalGold << " gold";
				// if it is a player paid job and she is not a slave
				if ((m_JobManager.is_job_Paid_Player(current->m_NightJob) && !current->is_slave()) ||
					// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
					(m_JobManager.is_job_Paid_Player(current->m_NightJob) && current->is_slave() && cfg.initial.slave_pay_outofpocket()))
					ss << " directly from you. She gets to keep it all.";
				else if (current->house() <= 0)				ss << " and she gets to keep it all.";
				else if (totalTips>0 && ((cfg.initial.girls_keep_tips() && !current->is_slave()) || (cfg.initial.slave_keep_tips() && current->is_slave())))
				{
					int hpay = int(double(totalPay * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalPay - hpay;
					ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut (" << 100 - current->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay << " gold.\n\nYou got " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				else
				{
					int hpay = int(double(totalGold * double(current->m_Stats[STAT_HOUSE] * 0.01)));
					int gpay = totalGold - hpay;
					ss << ".\nShe keeps " << gpay << " gold. (" << 100 - current->m_Stats[STAT_HOUSE] << "%)\nYou keep " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
				}
				summary += ss.str();
			}
			else if (totalGold == 0) summary += girlName + gettext(" made no money.");
			else if (totalGold < 0)
			{
				stringstream ss;
				ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
				summary += ss.str();
				sum = EVENT_DEBUG;
			}
		}
		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);
		summary = "";
		current = current->m_Next;	// Process next girl
	}



	// Finaly do end of day stuff
	current = brothel->m_Girls;
	while (current)
	{
		if (current->health() <= 0)
		{	// skip dead girls
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		girlName = current->m_Realname;

		// reset temporary matron reassignment if any
		if (current->m_DayJob != restjob)
		{
			current->m_NightJob = current->m_DayJob;
			current->m_DayJob = restjob;
		}

		// update for girls items that are not used up
		do_daily_items(brothel, current);					// `J` added

		// Level the girl up if nessessary
		if ((g_Girls.GetStat(current, STAT_EXP) >= (g_Girls.GetStat(current, STAT_LEVEL) + 1) * 125) || (g_Girls.GetStat(current, STAT_EXP) >= 32000)) g_Girls.LevelUp(current);
		// Natural healing, 2% health and 2% tiredness per day
		current->m_Stats[STAT_HEALTH] = min(current->m_Stats[STAT_HEALTH] + 2, 100);
		current->m_Stats[STAT_TIREDNESS] = max(current->m_Stats[STAT_TIREDNESS] - 2, 0);

		MatronMsg = "", MatronWarningMsg = "";
		if (g_Girls.GetStat(current, STAT_HAPPINESS) < 40)
		{
			if (current->m_NightJob != matronjob && matron && brothel->m_NumGirls > 1 && g_Dice.percent(70))
			{
				MatronMsg = gettext("The Director helps cheer up ") + girlName + gettext(" when she is feeling sad.\n");
				g_Girls.UpdateStat(current, STAT_HAPPINESS, g_Dice % 10 + 5);
			}
			else if (brothel->m_NumGirls > 10 && g_Dice.percent(50))
			{
				MatronMsg = gettext("Some of the other girls help cheer up ") + girlName + gettext(" when she is feeling sad.\n");
				g_Girls.UpdateStat(current, STAT_HAPPINESS, g_Dice % 8 + 3);
			}
			else if (brothel->m_NumGirls > 1 && g_Dice.percent(max(brothel->m_NumGirls, 50)))
			{
				MatronMsg = gettext("One of the other girls helps cheer up ") + girlName + gettext(" when she is feeling sad.\n");
				g_Girls.UpdateStat(current, STAT_HAPPINESS, g_Dice % 6 + 2);
			}
			else if (brothel->m_NumGirls == 1 && g_Dice.percent(70))
			{
				MatronMsg = girlName + gettext(" plays around in the empty Studio until she feels better.\n");
				g_Girls.UpdateStat(current, STAT_HAPPINESS, g_Dice % 10 + 10);
			}
			else if (g_Girls.GetStat(current, STAT_HAPPINESS) < 20) // no one helps her and she is really unhappy
			{
				MatronWarningMsg = girlName + gettext(" is looking very depressed. You may want to do something about that before she does something drastic.\n");
			}
		}

		if (g_Girls.GetStat(current, STAT_TIREDNESS) > 80 || g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			int t = g_Girls.GetStat(current, STAT_TIREDNESS);
			int h = g_Girls.GetStat(current, STAT_HEALTH);

			if (!matron)	// do no matron first as it is the easiest
			{
				MatronWarningMsg += gettext("WARNING! ") + girlName;
				if (t > 80 && h < 20)		MatronWarningMsg += gettext(" is in real bad shape, she is tired and injured.\nShe should go to the Clinic.\n");
				else if (t > 80 && h < 40)	MatronWarningMsg += gettext(" is in bad shape, she is tired and injured.\nShe should rest or she may die!\n");
				else if (t > 80)			MatronWarningMsg += gettext(" is desparatly in need of rest.\nGive her some free time\n");
				else if (h < 20)			MatronWarningMsg += gettext(" is badly injured.\nShe should rest or go to the Clinic.\n");
				else if (h < 40)			MatronWarningMsg += gettext(" is hurt.\nShe should rest and recuperate.\n");
			}
			else if (current->m_NightJob == matronjob && matron)	// do matron	
			{
				if (t > 90 && h < 10)	// The matron may take herself off work if she is really bad off
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = restjob;
					MatronWarningMsg += gettext("The Director takes herself off duty because she is just too damn sore.\n");
					g_Girls.UpdateEnjoyment(current, ACTION_WORKMOVIE, -10, true);
				}
				else
				{
					MatronWarningMsg += gettext("As Director, ") + girlName + gettext(" has the keys to the store room.\nShe used them to 'borrow' ");
					if (t > 80 && h < 40)
					{
						MatronWarningMsg += gettext("some potions");
						g_Gold.consumable_cost(20, true);
						current->m_Stats[STAT_HEALTH] = min(current->m_Stats[STAT_HEALTH] + 20, 100);
						current->m_Stats[STAT_TIREDNESS] = max(current->m_Stats[STAT_TIREDNESS] - 20, 0);
					}
					else if (t > 80)
					{
						MatronWarningMsg += gettext("a resting potion");
						g_Gold.consumable_cost(10, true);
						current->m_Stats[STAT_TIREDNESS] = max(current->m_Stats[STAT_TIREDNESS] - 20, 0);
					}
					else if (h < 40)
					{
						MatronWarningMsg += gettext("a healing potion");
						g_Gold.consumable_cost(10, true);
						current->m_Stats[STAT_HEALTH] = min(current->m_Stats[STAT_HEALTH] + 20, 100);
					}
					MatronWarningMsg += gettext(" for herself.\n");
				}
			}
			else	// do all other girls with a matron working
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255) // the girl has been working
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = restjob;
					MatronWarningMsg += gettext("The Director takes ") + girlName + gettext(" off duty to rest due to her ");
					if (t > 80 && h < 40)	MatronWarningMsg += gettext("exhaustion.\n");
					else if (t > 80)		MatronWarningMsg += gettext("tiredness.\n");
					else if (h < 40)		MatronWarningMsg += gettext("low health.\n");
					else MatronWarningMsg += gettext("current state.\n");
				}
				else	// the girl has already been taken off duty by the matron
				{
					if (g_Dice.percent(70))
					{
						MatronMsg += gettext("The Director helps ");
						if (t > 80 && h < 40)
						{
							MatronMsg += girlName + gettext(" recuperate.\n");
							g_Girls.UpdateStat(current, STAT_TIREDNESS, -(g_Dice % 4 + 2));
							g_Girls.UpdateStat(current, STAT_HEALTH, (g_Dice % 4 + 2));
						}
						else if (t > 80)
						{
							MatronMsg += girlName + gettext(" to relax.\n");
							g_Girls.UpdateStat(current, STAT_TIREDNESS, -(g_Dice % 5 + 5));
						}
						else if (h < 40)
						{
							MatronMsg += gettext(" heal ") + girlName + gettext(".\n");
							g_Girls.UpdateStat(current, STAT_HEALTH, (g_Dice % 5 + 5));
						}
					}
				}
			}
		}

		// Now print out the consolodated message
		if (strcmp(MatronMsg.c_str(), "") != 0)			current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, SHIFT_NIGHT);
		if (strcmp(MatronWarningMsg.c_str(), "") != 0)	current->m_Events.AddMessage(MatronWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);

		current = current->m_Next;		// Process next girl
	}
	m_Processing_Shift = -1;			// WD: Finished Processing Shift set flag
}

TiXmlElement* cMovieStudioManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("MovieStudio_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	string message;

	// save Studio
	TiXmlElement* pBrothels = new TiXmlElement("MovieStudios");
	pBrothelManager->LinkEndChild(pBrothels);
	sMovieStudio* current = (sMovieStudio*) m_Parent;
	//         ...................................................
	message = "***************** Saving studios *****************";
	g_LogFile.write(message);
	while(current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveMovieStudioXML(pBrothels);
		current = (sMovieStudio*) current->m_Next;
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
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("MovieRunTime", m_MovieRunTime);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);
	// save variables for sex restrictions
	pBrothel->SetAttribute("RestrictAnal", m_RestrictAnal);
	pBrothel->SetAttribute("RestrictBDSM", m_RestrictBDSM);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);
	pBrothel->SetAttribute("RestrictHand", m_RestrictHand);
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);
	
	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);
	
	TiXmlElement* pMovies = new TiXmlElement("Movies");
	pBrothel->LinkEndChild(pMovies);
	sMovie* movie = m_Movies;
	while(movie)
	{
		TiXmlElement* pMovie = new TiXmlElement("Movie");
		pMovies->LinkEndChild(pMovie);
		pMovie->SetAttribute("Init_Qual", movie->m_Init_Quality);
		pMovie->SetAttribute("Qual", movie->m_Quality);
		pMovie->SetAttribute("Promo_Qual", movie->m_Promo_Quality);
		pMovie->SetAttribute("Money_Made", movie->m_Money_Made);
		pMovie->SetAttribute("RunWeeks", movie->m_RunWeeks);
		movie = movie->m_Next;
	}

	// Save Girls
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pBrothel->LinkEndChild(pGirls);
	sGirl* girl = m_Girls;
	while(girl)
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

	string message = "";
	//         ...................................................
	message = "***************** Loading studio ****************";
	g_LogFile.write(message);
	m_NumMovieStudios = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("MovieStudios");
	if (pBrothels)
	{
		for(TiXmlElement* pBrothel = pBrothels->FirstChildElement("MovieStudio");
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
	//no need to init this, we just created it
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
	pBrothel->QueryIntAttribute("MovieRunTime", &m_MovieRunTime);
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);
	pBrothel->QueryValueAttribute<bool>("RestrictHand", &m_RestrictHand);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);

	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);

	m_NumMovies = 0;
	TiXmlElement* pMovies = pBrothel->FirstChildElement("Movies");
	if (pMovies)
	{
		for(TiXmlElement* pMovie = pMovies->FirstChildElement("Movie");
			pMovie != 0;
			pMovie = pMovie->NextSiblingElement("Movie"))
		{
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
			g_Studios.NewMovie(this, init_quality, quality, promo_quality, money_made, runweeks);

		}
	}

	// Load girls
	m_NumGirls = 0;
	TiXmlElement* pGirls = pBrothel->FirstChildElement("Girls");
	if (pGirls)
	{
		for(TiXmlElement* pGirl = pGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				girl->m_InMovieStudio = true;
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
	return g_Studios.m_movieScenes.at(num);
}

int cMovieStudioManager::GetNumScenes()
{
	return g_Studios.m_movieScenes.size();
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

	if (detailName == "Name")
	{
		ss << m_Name;
	}
	else if (detailName == "Quality")
	{
		ss << m_Quality;
	}
	else
	{
		ss << gettext("Not found");
	}
	Data = ss.str();
}


// ----- Movie
void cMovieStudioManager::StartMovie(int brothelID, int Time)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	current->m_ShowQuality = 0;
	current->m_ShowTime = Time;
}

int cMovieStudioManager::GetTimeToMovie(int brothelID)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	return current->m_ShowTime;
}

void cMovieStudioManager::NewMovie(sMovieStudio* brothel, int Init_Quality, int Quality, int Promo_Quality, int Money_Made, int RunWeeks)
{
	sMovie* newMovie = new sMovie();
	newMovie->m_Init_Quality = Init_Quality;
	newMovie->m_Promo_Quality = Promo_Quality;
	newMovie->m_Quality = Quality;
	newMovie->m_Money_Made = Money_Made;
	newMovie->m_RunWeeks = RunWeeks;

	if(brothel->m_NumMovies > 0)
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
	if(brothel->m_Movies)
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
	return 
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMBEAST, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMSEX, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMANAL, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMLESBIAN, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMBONDAGE, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMGROUP, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMORAL, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMMAST, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMTITTY, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMSTRIP, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMHANDJOB, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMFOOTJOB, 1) +
		g_Studios.GetNumGirlsOnJob(brothel, JOB_FILMRANDOM, 1);
}

bool cMovieStudioManager::is_Actress_Job(int testjob){
	if (testjob == JOB_FILMBEAST ||
		testjob == JOB_FILMSEX ||
		testjob == JOB_FILMANAL ||
		testjob == JOB_FILMLESBIAN ||
		testjob == JOB_FILMBONDAGE ||
		testjob == JOB_FILMGROUP ||
		testjob == JOB_FILMORAL ||
		testjob == JOB_FILMMAST ||
		testjob == JOB_FILMTITTY ||
		testjob == JOB_FILMSTRIP ||
		testjob == JOB_FILMHANDJOB ||
		testjob == JOB_FILMFOOTJOB ||
		testjob == JOB_FILMRANDOM)
		return true;
	return false;
}

bool cMovieStudioManager::CrewNeeded()	// `J` added, if CM and CP both on duty or there are no actresses, return false
{
	if ((GetNumGirlsOnJob(0, JOB_CAMERAMAGE, 1) > 0 &&
		GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, 1) > 0) ||
		GetNumGirlsOnJob(0, JOB_FILMBEAST, 1) +
		GetNumGirlsOnJob(0, JOB_FILMSEX, 1) +
		GetNumGirlsOnJob(0, JOB_FILMANAL, 1) +
		GetNumGirlsOnJob(0, JOB_FILMLESBIAN, 1) +
		GetNumGirlsOnJob(0, JOB_FILMBONDAGE, 1) +
		GetNumGirlsOnJob(0, JOB_FILMGROUP, 1) +
		GetNumGirlsOnJob(0, JOB_FILMORAL, 1) +
		GetNumGirlsOnJob(0, JOB_FILMMAST, 1) +
		GetNumGirlsOnJob(0, JOB_FILMTITTY, 1) +
		GetNumGirlsOnJob(0, JOB_FILMSTRIP, 1) +
		GetNumGirlsOnJob(0, JOB_FILMHANDJOB, 1) +
		GetNumGirlsOnJob(0, JOB_FILMFOOTJOB, 1) +
		GetNumGirlsOnJob(0, JOB_FILMRANDOM, 1) < 1)
		return false;	// a CM or CP is not Needed
	return true;	// Otherwise a CM or CP is Needed
}

// ----- Add / remove
string cMovieStudioManager::AddScene(sGirl* girl, int Job, int Bonus)
{
	sMovieStudio* current = (sMovieStudio*)m_Parent;
	sMovieScene* newScene = new sMovieScene();

	if (newScene == 0)
		return "";
	//NOTE i crazy added this to try and improve the movies before it only check for normalsex skill now it should check for each skill type i hope
	// Fixed so it will check for skill type being used --PP
	long quality = 0;
	quality = g_Girls.GetSkill(girl, Job) / 5;
	quality += Bonus;

	//CRAZY added this to have traits play a bigger part in the movies
	if (g_Girls.HasTrait(girl, "Fast orgasms") || g_Girls.HasTrait(girl, "Fast Orgasms"))			quality += 4;
	else if (g_Girls.HasTrait(girl, "Slow orgasms") || g_Girls.HasTrait(girl, "Slow Orgasms"))		quality -= 2;
	if (g_Girls.HasTrait(girl, "Fake orgasm expert") || g_Girls.HasTrait(girl, "Fake Orgasm Expert"))		quality += 3;
	if (g_Girls.HasTrait(girl, "Puffy Nipples") || g_Girls.HasTrait(girl, "Perky Nipples"))		quality += 2;
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))	quality += 4;
	else if (g_Girls.HasTrait(girl, "Big Boobs"))			quality += 2;
	else if (g_Girls.HasTrait(girl, "Small Boobs"))		quality += 1;
	if (g_Girls.HasTrait(girl, "Great Figure"))			quality += 4;
	if (g_Girls.HasTrait(girl, "Great Arse"))			quality += 2;
	if (g_Girls.HasTrait(girl, "Charismatic"))			quality += 4;
	if (g_Girls.HasTrait(girl, "Charming"))				quality += 2;
	if (g_Girls.HasTrait(girl, "Long Legs"))			quality += 2;
	if (g_Girls.HasTrait(girl, "Perky Nipples"))		quality += 1;
	if (g_Girls.HasTrait(girl, "Puffy Nipples"))		quality += 1;
	if (g_Girls.HasTrait(girl, "Shape Shifter"))		quality += 5;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			quality += 4;
	if (g_Girls.HasTrait(girl, "Good Kisser"))			quality += 2;
	if (g_Girls.HasTrait(girl, "Cute"))					quality += 2;
	if (g_Girls.HasTrait(girl, "Sexy Air"))				quality += 2;
	if (g_Girls.HasTrait(girl, "Psychic"))				quality += 4;
	if (g_Girls.HasTrait(girl, "Actress"))				quality += 15;

	if (g_Girls.HasTrait(girl, "Manly"))				quality -= 2;
	if (g_Girls.HasTrait(girl, "Fragile"))				quality -= 2;
	if (g_Girls.HasTrait(girl, "Mind Fucked"))			quality -= 4;
	if (g_Girls.HasTrait(girl, "Nervous"))				quality -= 2;
	if (g_Girls.HasTrait(girl, "Horrific Scars"))		quality -= 4;
	if (g_Girls.HasTrait(girl, "Clumsy"))				quality -= 2;
	if (g_Girls.HasTrait(girl, "Meek"))					quality -= 2;
	if (g_Girls.HasTrait(girl, "Aggressive"))			quality -= 2;
	if (g_Girls.HasTrait(girl, "Broken Will"))			quality -= 4;
	if (g_Girls.HasTrait(girl, "Dependant"))			quality -= 3;

	if (Job == SKILL_ORALSEX)
	{
		if (g_Girls.HasTrait(girl, "Pierced Tongue"))		quality += 1;
		if (g_Girls.HasTrait(girl, "No Gag Reflex"))		quality += 2;
		if (g_Girls.HasTrait(girl, "Deep Throat"))			quality += 5;
		if (g_Girls.HasTrait(girl, "Gag Reflex"))			quality -= 3;
	}

	//CRAZY added this better looking girls should make better quality movies 
	// Changed to work with new job revision --PP
	quality += (g_Girls.GetStat(girl, STAT_CHARISMA) - 50) / 10;
	quality += (g_Girls.GetStat(girl, STAT_BEAUTY) - 50) / 10;
	quality += g_Girls.GetStat(girl, STAT_FAME);
	quality += g_Girls.GetStat(girl, STAT_LEVEL);
	char performance[1000];
	_itoa(quality, performance, 10);

	// Add bonus for Fluffer, CameraMage and CrystalPurifier --PP
	if (Job != JOB_FILMMAST && Job != JOB_FILMLESBIAN)	// No fluffers needed --PP
		quality += g_Studios.m_FlufferQuality;

	quality += g_Studios.m_CameraQuality + g_Studios.m_PurifierQaulity + g_Studios.m_DirectorQuality + g_Studios.m_StagehandQuality;

	newScene->m_Director = g_Studios.m_DirectorName;
	newScene->m_Actress = girl->m_Realname;
	newScene->m_Job = Job;
	newScene->m_Init_Quality = quality;
	newScene->m_Quality = quality;
	newScene->m_Promo_Quality = 0;
	newScene->m_Money_Made = 0;
	newScene->m_RunWeeks = 0;
	stringstream ss;
	ss << "scene " << m_movieScenes.size() + 1;
	newScene->m_Name = ss.str();
	m_movieScenes.push_back(newScene);
	return performance;
}



long cMovieStudioManager::calc_movie_quality(){
	long quality = 0;
	long result = 0;
	for each (sMovieScene* scene in m_movieScenes)
	{
		result += scene->m_Quality;
	}
	result += m_movieScenes.size() * 10;
	char buffer[1000];
	int iresult = result;
	string message = "This movie will sell at ";
	_itoa(result, buffer, 10);
	message += buffer;
	message += " gold, for 35 weeks, but it's value will drop over time. \n A promoter with an advertising budget will help it sell for more.";
	g_MessageQue.AddToQue(message, COLOR_BLUE);
	g_InitWin = true;
	return result;
}

void cMovieStudioManager::ReleaseCurrentMovie(){
	sMovieStudio* current = (sMovieStudio*)m_Parent;
	long init_quality = calc_movie_quality();
	long quality = init_quality;	// calculate movie quality
	long promo_quality = 0;
	long money_made = 0;
	long runweeks = 0;
	m_movieScenes.clear();					// clear scene list
	NewMovie(current, init_quality, quality, promo_quality, money_made, runweeks);	//add new movie
}
