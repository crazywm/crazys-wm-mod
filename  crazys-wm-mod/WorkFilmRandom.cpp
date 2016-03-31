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
#include "cMovieStudio.h"
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
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern int g_Building;



// `J` Job Movie Studio - Unused
bool cJobManager::WorkFilmRandom(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	bool refused = false;
	int sextype = -1;
	int filmjob;
	int numfilmjobs = JOB_FILMRANDOM - JOB_STAGEHAND;
	bool cando = false;
	
	if (nothing_banned(brothel))
	{
		int roll = g_Dice % numfilmjobs;
		filmjob = roll + JOB_STAGEHAND + 1;
	}
	else
	{
		do
		{
			int roll = g_Dice % numfilmjobs;
			filmjob = roll + JOB_STAGEHAND + 1;
			// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> WorkFilmRandom.cpp
			switch (filmjob)
			{
			case	JOB_FILMMAST:
			case 	JOB_FILMSTRIP:
			case 	JOB_FILMTEASE:
				cando = is_sex_type_allowed(SKILL_STRIP, brothel);
				break;
			case 	JOB_FILMANAL:
				cando = is_sex_type_allowed(SKILL_ANAL, brothel);
				break;
			case 	JOB_FILMFOOTJOB:
				cando = is_sex_type_allowed(SKILL_FOOTJOB, brothel);
				break;
				//case 	JOB_FILMFUTA			:
				//	cando = is_sex_type_allowed(SKILL_NORMALSEX, brothel);
				//	break;
			case 	JOB_FILMHANDJOB:
				cando = is_sex_type_allowed(SKILL_HANDJOB, brothel);
				break;
			case 	JOB_FILMLESBIAN:
				cando = is_sex_type_allowed(SKILL_LESBIAN, brothel);
				break;
			case 	JOB_FILMORAL:
				cando = is_sex_type_allowed(SKILL_ORALSEX, brothel);
				break;
			case 	JOB_FILMSEX:
				cando = is_sex_type_allowed(SKILL_NORMALSEX, brothel);
				break;
			case 	JOB_FILMTITTY:
				cando = is_sex_type_allowed(SKILL_TITTYSEX, brothel);
				break;
			case 	JOB_FILMBEAST:
				cando = is_sex_type_allowed(SKILL_BEASTIALITY, brothel);
				break;
			case 	JOB_FILMBONDAGE:
				cando = is_sex_type_allowed(SKILL_BDSM, brothel);
				break;
			case 	JOB_FILMBUKKAKE:
				cando = is_sex_type_allowed(SKILL_ORALSEX, brothel) && is_sex_type_allowed(SKILL_NORMALSEX, brothel);
				break;
			case 	JOB_FILMFACEFUCK:
				cando = is_sex_type_allowed(SKILL_ORALSEX, brothel);
				break;
			case 	JOB_FILMGROUP:
				cando = is_sex_type_allowed(SKILL_GROUP, brothel);
				break;
			case 	JOB_FILMPUBLICBDSM:
				cando = is_sex_type_allowed(SKILL_BDSM, brothel);
				break;
			default:
				cando = true;
				break;
			}
		} while (!cando);
	}

	refused = g_Studios.m_JobManager.JobFunc[filmjob](girl, brothel, SHIFT_NIGHT, summary);
	return refused;
}

double cJobManager::JP_FilmRandom(sGirl* girl, bool estimate)// not used
{
	return 0;
}
