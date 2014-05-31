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
extern cMovieStudioManager  g_Studios;
extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;
extern cRng             g_Dice;
extern cGold            g_Gold;
extern char             buffer[1000];
extern bool				g_InitWin;

// // ----- Strut sMovieStudio Create / destroy
sMovieStudio::sMovieStudio() :	m_Finance(0)	// constructor
{
	m_var				= 0;
	m_Name				= "studio";
	m_CurrFilm			= 0;
	m_NumMovies			= 0;
	m_LastMovies		= 0;
	m_Movies			= 0;
	m_MovieRunTime		= 0;
	m_Filthiness		= 0;
	m_Next				= 0;
	m_Girls				= 0;
	m_LastGirl			= 0;
	// end mod
	m_NumGirls			= 0;
	m_SecurityLevel		= 0;
	for(u_int i=0; i<NUMJOBTYPES; i++)		m_BuildingQuality[i] = 0;
}

sMovieStudio::~sMovieStudio()			// destructor
{
	m_NumMovies			= 0;
	if(m_Movies)	delete m_Movies;
	m_Movies			= 0;
	m_LastMovies		= 0;
	m_var				= 0;
	if(m_Next)		delete m_Next;
	m_Next				= 0;
	if(m_Girls)		delete m_Girls;
	m_LastGirl			= 0;
	m_Girls				= 0;
	if(m_CurrFilm)	delete m_CurrFilm;
}

void cMovieStudioManager::AddGirl(int brothelID, sGirl* girl)
{
	girl->m_InArena = false;
	girl->m_InCentre = false;
	girl->m_InClinic = false;
	girl->m_InHouse = false;
	girl->m_InMovieStudio = true;
	girl->where_is_she = 0;
	cBrothelManager::AddGirl(brothelID, girl);
}

void cMovieStudioManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InMovieStudio = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}

// ----- Add / remove
string cMovieStudioManager::AddScene(sGirl* girl, int Job, int Bonus)
{
	sMovieStudio* current = (sMovieStudio*) m_Parent;
	sMovieScene* newScene = new sMovieScene();

	if(newScene == 0)
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
	if(g_Girls.HasTrait(girl,"Abnormally Large Boobs"))	quality += 4;
	else if(g_Girls.HasTrait(girl,"Big Boobs"))			quality += 2;
	else if(g_Girls.HasTrait(girl,"Small Boobs"))		quality += 1;
	if(g_Girls.HasTrait(girl, "Great Figure"))			quality += 4;
	if(g_Girls.HasTrait(girl, "Great Arse"))			quality += 2;
	if(g_Girls.HasTrait(girl, "Charismatic"))			quality += 4;
	if(g_Girls.HasTrait(girl, "Charming"))				quality += 2;
	if(g_Girls.HasTrait(girl, "Long Legs"))				quality += 2;
	if(g_Girls.HasTrait(girl, "Perky Nipples"))			quality += 1;
	if(g_Girls.HasTrait(girl, "Puffy Nipples"))			quality += 1;
	if(g_Girls.HasTrait(girl, "Shape Shifter"))			quality += 5;
	if(g_Girls.HasTrait(girl, "Nymphomaniac"))			quality += 4;
	if(g_Girls.HasTrait(girl, "Good Kisser"))			quality += 2;

	if(g_Girls.HasTrait(girl, "Manly"))					quality -= 2;
	if(g_Girls.HasTrait(girl, "Fragile"))				quality -= 2;
	if(g_Girls.HasTrait(girl, "Mind Fucked"))			quality -= 4;
	if(g_Girls.HasTrait(girl, "Nervous"))				quality -= 2;
	if(g_Girls.HasTrait(girl, "Horrific Scars"))		quality -= 4;
	if(g_Girls.HasTrait(girl, "Clumsy"))				quality -= 2;
	if(g_Girls.HasTrait(girl, "Meek"))					quality -= 2;

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
	if(m_Parent)	delete m_Parent;
	m_Parent			= 0;
	m_Last				= 0;
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
	g_MessageQue.AddToQue(message, 0);
	g_InitWin = true;
	return result;
}

void cMovieStudioManager::ReleaseCurrentMovie(){
	sMovieStudio* current = (sMovieStudio*) m_Parent;
	long init_quality = calc_movie_quality();
	long quality = init_quality;	// calculate movie quality
	long promo_quality = 0;
	long money_made = 0;
	long runweeks = 0;
	m_movieScenes.clear();					// clear scene list
	NewMovie(current, init_quality, quality, promo_quality, money_made, runweeks);	//add new movie
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
		cgirl->m_InMovieStudio = true;
		cgirl->m_InArena = false;
		cgirl->m_InCentre = false;
		cgirl->m_InClinic = false;
		cgirl->m_InHouse = false;
		cgirl->where_is_she = 0;
		cgirl->m_Events.Clear();
		cgirl->m_Pay = 0;
		cgirl = cgirl->m_Next;
	}
	m_FlufferQuality = 0;
	m_CameraQuality = 0;
	m_PurifierQaulity = 0;

	if(current->m_Filthiness < 0)		current->m_Filthiness = 0;

	//	UpdateGirls(current, 0); Should only be one shift in movie studio --PP
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
			ss << gettext("You earn ") << income << gettext(" gold from movie income, at your ") << current->m_Name;
			g_MessageQue.AddToQue(ss.str(), 2);
		}
	}
}

// End of turn stuff is here
// Same method than Brothel but different job
void cMovieStudioManager::UpdateGirls(sBrothel* brothel)
{
	sGirl* current = brothel->m_Girls;
	string summary, msg, girlName;
	int totalGold = 0;
	bool refused;
	sGirl* DeadGirl = 0;
	m_Processing_Shift= SHIFT_NIGHT;	

	// Process crew first --PP
	while(current)
	{
		// Sanity check! Don't process dead girls
		// Sanity check part 2: Check that m_Next points to something
		if(current->health() <= 0)
		{
			if (current->m_Next) // If there are more girls to process
			{
			    current = current->m_Next;
				continue;
			}
			else
			{
				current = 0;
				break;
			}
		}

		totalGold = 0;
		bool sum = true;
		refused = false;
		current->m_Pay = 0;
		girlName = current->m_Realname;
		u_int sw = 0;						//	Job type
		if(current->m_JustGaveBirth)		// if she gave birth, let her rest this week
			sw = JOB_FILMFREETIME;
		else
			sw = current->m_NightJob;

		// do their job
		switch(sw)
		{
		case JOB_DIRECTOR:
		case JOB_PROMOTER:
		case JOB_STAGEHAND:
		case JOB_FLUFFER:
		case JOB_CAMERAMAGE:
		case JOB_CRYSTALPURIFIER:
			refused = m_JobManager.JobFunctions[sw](current,brothel,SHIFT_NIGHT,summary);
	
			if(refused)						// if she refused she still gets tired
				g_Girls.AddTiredness(current);
			totalGold += current->m_Pay;
			g_Brothels.CalculatePay(brothel, current, sw);
			brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);
			
			//		Summary Messages
			if(refused)											
				summary += girlName + gettext(" refused to work so made no money.");
			// WD:	Only do summary messages if there is income to report
			else if(totalGold > 0)										
			{
				summary += girlName + gettext(" earned a total of ");
				_itoa(totalGold, buffer, 10);
				summary += buffer;
				summary += gettext(" gold");

				// WD: Job Paid by player
				if(m_JobManager.is_job_Paid_Player(sw))					
					summary += gettext(" directly from you. She gets to keep it all.");
				else if(current->house() <= 0)
					summary += gettext(" and she gets to keep it all.");
				else
				{
					summary += gettext(", you keep ");
					_itoa((int)current->m_Stats[STAT_HOUSE], buffer, 10);
					summary += buffer;
					summary += gettext("%. ");
				}
			}

			// WD:	No Income today
			else if(totalGold == 0)										
				summary += girlName + gettext(" made no money.");
			else if(totalGold < 0)										
			{
				summary += "ERROR: She has a loss of ";
				_itoa(totalGold, buffer, 10);
				summary += buffer;
				summary += " gold";
				summary += "\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
				current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_DEBUG);
				sum = false;
			}

			if(sum)
				current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_SUMMARY);

			summary = "";

			// Level the girl up if nessessary
			if(g_Girls.GetStat(current, STAT_EXP) == 255)
				g_Girls.LevelUp(current);

			// Natural healing, 2% health and 2% tiredness per day
			current->m_Stats[STAT_HEALTH] += 2;
			if (current->m_Stats[STAT_HEALTH] > 100)
				current->m_Stats[STAT_HEALTH] = 100;

			if (current->m_Stats[STAT_TIREDNESS] < 3)
				current->m_Stats[STAT_TIREDNESS] = 0;
			else
				current->m_Stats[STAT_TIREDNESS] = current->m_Stats[STAT_TIREDNESS] - 2;
			break;
		default:		// All non-crew jobs just skips to next girl
			break;
		}
		current = current->m_Next; // Next Girl
	}

	// Process Stars
	current = brothel->m_Girls;
	while(current)
	{
		brothel->m_Filthiness++;
		totalGold = 0;
		refused = false;
		current->m_Pay = 0;
		girlName = current->m_Realname;

		// Remove any dead bodies from last week
		if(current->health() <= 0)	
		{
			DeadGirl = current;

			if (current->m_Next) // If there are more girls to process
				current = current->m_Next;
			else
				current = 0;

			UpdateAllGirlsStat(brothel, STAT_PCFEAR, 2);	// increase all the girls fear of the player for letting her die (weather his fault or not)
			UpdateAllGirlsStat(brothel, STAT_PCHATE, 1);	// increase all the girls hate of the player for letting her die (weather his fault or not)

			// Two messages go into the girl queue...
			msg += girlName + gettext(" has died from her injuries, the other girls all fear and hate you a little more.");
			DeadGirl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			summary += girlName + gettext(" has died from her injuries.  Her body will be removed by the end of the week.");
			DeadGirl->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);

			// There is also one global message
			g_MessageQue.AddToQue(msg, 1);

			// I forgot to set msg and summary to empty. D'OH!
			msg = "";
			summary = "";

			RemoveGirl(0, DeadGirl, true);
			DeadGirl = 0;

			if (current)  // If there are more girls to process
				continue;
			else
				break;
		}

		// Brothel only update for girls accomadation level
		do_food_and_digs(brothel, current);

		// update the fetish traits
		g_Girls.CalculateGirlType(current);

		// update birthday counter and age the girl
		g_Girls.updateGirlAge(current, true);

		// update temp stats
		g_Girls.updateTempStats(current);

		// update temp skills
		g_Girls.updateTempSkills(current);

		// update temp traits
		g_Girls.updateTempTraits(current);

		// handle pregnancy and children growing up
		g_Girls.HandleChildren(current, summary);
			
		// health loss to STD's		NOTE: Girl can die
		g_Girls.updateSTD(current);

		// Update happiness due to Traits		NOTE: Girl can die
		g_Girls.updateHappyTraits(current);

		//	Update daily stats	Now only runs once per day
		updateGirlTurnBrothelStats(current);

		//	Stat Code common to Dugeon and Brothel
		g_Girls.updateGirlTurnStats(current);
		


/*
 *		EVERY SHIFT processing
 */

		// Sanity check! Don't process dead girls
		// Sanity check part 2: Check that m_Next points to something
		if(current->health() <= 0)
		{
			if (current->m_Next) // If there are more girls to process
			{
			    current = current->m_Next;
				continue;
			}
			else
			{
				current = 0;
				break;
			}
		}

		// Girl uses items she has
		g_Girls.UseItems(current);

		// Calculate the girls asking price
		g_Girls.CalculateAskPrice(current, true);

/*
 *		JOB PROCESSING
 */
		u_int sw = 0;						//	Job type
		if(current->m_JustGaveBirth)		// if she gave birth, let her rest this week
			sw = JOB_FILMFREETIME;
		else
			sw = current->m_NightJob;

		// Already processed crew jobs, so check for those jobs and move to next girl if true.
		if (sw == JOB_FLUFFER || sw == JOB_CAMERAMAGE || sw == JOB_CRYSTALPURIFIER || 
			sw == JOB_DIRECTOR || sw == JOB_PROMOTER || sw == JOB_STAGEHAND)
		{
			if (current->m_Next)
			{
			    current = current->m_Next;
				continue;
			}
			else
			{
				current = 0;
				break;
			}
		}
		else if (sw == JOB_FILMRANDOM)
		{
			int skill = 0;
			do	{
				int maxroll = JOB_FILMRANDOM - JOB_FILMBEAST;
				int roll = g_Dice%maxroll;

				sw = roll + JOB_FILMBEAST;
				if (sw == JOB_FILMRANDOM)
					sw -= 1;
				switch (sw)
				{
				case JOB_FILMBEAST:
					skill = SKILL_BEASTIALITY;
					break;
				case JOB_FILMSEX:
					skill = SKILL_NORMALSEX;
					break;
				case JOB_FILMANAL:
					skill = SKILL_ANAL;
					break;
				case JOB_FILMLESBIAN:
					skill = SKILL_LESBIAN;
					break;
				case JOB_FILMBONDAGE:
					skill = SKILL_BDSM;
					break;
				case JOB_FILMGROUP:
					skill = SKILL_GROUP;
					break;
				case JOB_FILMORAL:
					skill = SKILL_ORALSEX;
					break;
				case JOB_FILMTITTY:
					skill = SKILL_TITTYSEX;
					break;
				case JOB_FILMSTRIP:
					skill = SKILL_STRIP;
					break;
				default:
					break;
				}
			} while (!(m_JobManager.is_sex_type_allowed(skill, brothel)));
			refused = m_JobManager.JobFunctions[sw](current, brothel, SHIFT_NIGHT, summary);
		}
		else if (sw != JOB_FILMFREETIME && sw >= JOB_FILMBEAST && sw <= JOB_FILMFREETIME)
		{
			refused = m_JobManager.JobFunctions[sw](current, brothel, SHIFT_NIGHT, summary);
		}
		else
		{
			sw = current->m_DayJob = current->m_NightJob = JOB_FILMFREETIME;
			refused = m_JobManager.JobFunctions[JOB_RESTING](current, brothel, SHIFT_NIGHT, summary);
		}

		if(refused)						// if she refused she still gets tired
			g_Girls.AddTiredness(current);
		totalGold += current->m_Pay;

		// work out the pay between the house and the girl 
		// may be change this for clinic
		g_Brothels.CalculatePay(brothel, current, sw);

		brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);

/*
 *		DIRECTORS CODE START
 */

		// Lets try to compact multiple messages into one.
		string DirectorMsg = "";
		string DirectorWarningMsg = "";

		bool director = false;
		if(GetNumGirlsOnJob(brothel->m_id, JOB_DIRECTOR, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_DIRECTOR, false) >= 1)
			director = true;

		if(g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (director)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_NightJob = JOB_FILMFREETIME;
					DirectorWarningMsg += gettext("The Director takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
				}
				else
				{
					if((g_Dice%100)+1 < 70)
					{
						DirectorMsg += gettext("The Director helps ") + girlName + gettext(" to relax.\n");
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				DirectorWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
		}

		if(g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && director && (g_Dice%100) +1 < 70)
		{
			DirectorMsg = gettext("The Director helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if(g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if(director)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_NightJob = JOB_FILMFREETIME;
					DirectorWarningMsg += girlName + gettext(" is taken off duty by the Director to rest due to her low health.\n");
				}
				else
				{
					DirectorMsg = gettext("The Director helps heal ") + girlName + gettext(".\n");
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				DirectorWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
			}
		}

		// Back to work
		if((current->m_NightJob == JOB_FILMFREETIME) && (g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20))
		{
			if(
				(director || current->m_PrevDayJob == JOB_DIRECTOR)  // do we have a director, or was she the director and made herself rest?
				&& current->m_PrevDayJob != 255  // 255 = nothing, in other words no previous job stored
				&& current->m_PrevNightJob != 255
				)
			{
				g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, true);
				if(current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
					current->m_NightJob = current->m_PrevNightJob;
				else
					current->m_DayJob = JOB_FILMFREETIME;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				DirectorMsg += gettext("The Director puts ") + girlName + gettext(" back to work.\n");
			}
			else
			{
				current->m_DayJob = JOB_FILMFREETIME;
				DirectorWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
			}
		}

		// Now print out the consolodated message
		if (strcmp(DirectorMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(DirectorMsg, IMGTYPE_PROFILE, SHIFT_NIGHT);
			DirectorMsg = "";
		}

        if (strcmp(DirectorWarningMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(DirectorWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
			DirectorWarningMsg = "";
		}
/*
 *		DIRECTOR CODE END
 */

/*
 *		Summary Messages
 */
		bool sum = true;

		if(refused)											
			summary += girlName + gettext(" refused to work so made no money.");

		// WD:	Only do summary messages if there is income to report
		else if(totalGold > 0)										
		{
			summary += girlName + gettext(" earned a total of ");
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += gettext(" gold");

			// WD: Job Paid by player
			if(m_JobManager.is_job_Paid_Player(sw))					
				summary += gettext(" directly from you. She gets to keep it all.");
			else if(current->house() <= 0)
				summary += gettext(" and she gets to keep it all.");
			else
			{
				summary += gettext(", you keep ");
				_itoa((int)current->m_Stats[STAT_HOUSE], buffer, 10);
				summary += buffer;
				summary += gettext("%. ");
			}
		}

		// WD:	No Income today
		else if(totalGold == 0)										
			summary += girlName + gettext(" made no money.");

#if 1																// WD: Income Loss Sanity Checking
		else if(totalGold < 0)										
		{
			summary += "ERROR: She has a loss of ";
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += " gold";
			summary += "\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_DEBUG);
			sum = false;
		}
#endif

		if(sum)
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_SUMMARY);

		summary = "";

		// Level the girl up if nessessary
		if(g_Girls.GetStat(current, STAT_EXP) == 255)
			g_Girls.LevelUp(current);

		// Natural healing, 2% health and 2% tiredness per day
		current->m_Stats[STAT_HEALTH] += 2;
		if (current->m_Stats[STAT_HEALTH] > 100)
			current->m_Stats[STAT_HEALTH] = 100;
		
		if (current->m_Stats[STAT_TIREDNESS] < 3)
				current->m_Stats[STAT_TIREDNESS] = 0;
			else
				current->m_Stats[STAT_TIREDNESS] = current->m_Stats[STAT_TIREDNESS] - 2;
		
		current = current->m_Next;	// Process next girl
	}

	// WD: Finished Processing Shift set flag
	m_Processing_Shift= -1;				
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
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("MovieRunTime", m_MovieRunTime);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);
	// save variables for sex restrictions
	pBrothel->SetAttribute("RestrictAnal", m_RestrictAnal);
	pBrothel->SetAttribute("RestrictBDSM", m_RestrictBDSM);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);
	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);
	if (m_AntiPregPotions < 0){ m_AntiPregPotions = 0; }
	if (m_KeepPotionsStocked != 0 && m_KeepPotionsStocked != 1){ m_KeepPotionsStocked = 0; }

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
	pBrothel->QueryIntAttribute("MovieRunTime", &m_MovieRunTime);
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);
	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
	// `J` Added to save potion stuff in individual buildings
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	if (m_AntiPregPotions < 0){ m_AntiPregPotions = 0; }
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);
	if (m_KeepPotionsStocked != 0 && m_KeepPotionsStocked != 1){ m_KeepPotionsStocked = 0; }

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
