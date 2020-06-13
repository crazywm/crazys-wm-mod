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
#include "buildings/cBrothel.h"
#include "cRng.h"
#include "Game.hpp"
#include <sstream>
#include "cJobManager.h"

// `J` Job Movie Studio - Unused
bool WorkFilmRandom(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    int filmjob;
    int numfilmjobs = JOB_FILMRANDOM - JOB_STAGEHAND;
    bool cando = false;
    
    if (brothel->nothing_banned())
    {
        int roll = rng % numfilmjobs;
        filmjob = roll + JOB_STAGEHAND + 1;
    }
    else
    {
        do
        {
            int roll = rng % numfilmjobs;
            filmjob = roll + JOB_STAGEHAND + 1;
            // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> WorkFilmRandom.cpp
            switch (filmjob)
            {
            case    JOB_FILMMAST:
            case     JOB_FILMSTRIP:
            case     JOB_FILMTEASE:
                cando = brothel->is_sex_type_allowed(SKILL_STRIP);
                break;
            case     JOB_FILMANAL:
                cando = brothel->is_sex_type_allowed(SKILL_ANAL);
                break;
            case     JOB_FILMFOOTJOB:
                cando = brothel->is_sex_type_allowed(SKILL_FOOTJOB);
                break;
                //case     JOB_FILMFUTA            :
                //    cando = is_sex_type_allowed(SKILL_NORMALSEX, brothel);
                //    break;
            case     JOB_FILMHANDJOB:
                cando = brothel->is_sex_type_allowed(SKILL_HANDJOB);
                break;
            case     JOB_FILMLESBIAN:
                cando = brothel->is_sex_type_allowed(SKILL_LESBIAN);
                break;
            case     JOB_FILMORAL:
                cando = brothel->is_sex_type_allowed(SKILL_ORALSEX);
                break;
            case     JOB_FILMSEX:
                cando = brothel->is_sex_type_allowed(SKILL_NORMALSEX);
                break;
            case     JOB_FILMTITTY:
                cando = brothel->is_sex_type_allowed(SKILL_TITTYSEX);
                break;
            case     JOB_FILMBEAST:
                cando = brothel->is_sex_type_allowed(SKILL_BEASTIALITY);
                break;
            case     JOB_FILMBONDAGE:
            case     JOB_FILMPUBLICBDSM:
                cando = brothel->is_sex_type_allowed(SKILL_BDSM);
                break;
            case     JOB_FILMBUKKAKE:
                cando = brothel->is_sex_type_allowed(SKILL_ORALSEX) && brothel->is_sex_type_allowed(SKILL_NORMALSEX);
                break;
            case     JOB_FILMFACEFUCK:
                cando = brothel->is_sex_type_allowed(SKILL_ORALSEX);
                break;
            case     JOB_FILMGROUP:
                cando = brothel->is_sex_type_allowed(SKILL_GROUP);
                break;
            default:
                cando = true;
                break;
            }
        } while (!cando);
    }

    return g_Game->job_manager().do_job(girl, SHIFT_NIGHT);
}

double JP_FilmRandom(const sGirl& girl, bool estimate)// not used
{
    return 0;
}
