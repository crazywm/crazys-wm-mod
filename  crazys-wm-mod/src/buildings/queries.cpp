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

#include "queries.hpp"
#include "src/IBuilding.hpp"


bool DoctorNeeded(IBuilding& building) {
    return !(building.num_girls_on_job(JOB_DOCTOR, 0) > 0 ||
             building.num_girls_on_job(JOB_GETHEALING, 0) +
             building.num_girls_on_job(JOB_GETABORT, 0) +
             building.num_girls_on_job(JOB_COSMETICSURGERY, 0) +
             building.num_girls_on_job(JOB_LIPO, 0) +
             building.num_girls_on_job(JOB_BREASTREDUCTION, 0) +
             building.num_girls_on_job(JOB_BOOBJOB, 0) +
             building.num_girls_on_job(JOB_VAGINAREJUV, 0) +
             building.num_girls_on_job(JOB_TUBESTIED, 0) +
             building.num_girls_on_job(JOB_FERTILITY, 0) +
             building.num_girls_on_job(JOB_FACELIFT, 0) +
             building.num_girls_on_job(JOB_ASSJOB, 0) < 1);
}

int GetNumberPatients(IBuilding& building, bool Day0Night1)	// `J` added, if there is a doctor already on duty or there is no one needing surgery, return false
{
    return (building.num_girls_on_job(JOB_GETHEALING, Day0Night1) +
            building.num_girls_on_job(JOB_GETABORT, Day0Night1) +
            building.num_girls_on_job(JOB_COSMETICSURGERY, Day0Night1) +
            building.num_girls_on_job(JOB_LIPO, Day0Night1) +
            building.num_girls_on_job(JOB_BREASTREDUCTION, Day0Night1) +
            building.num_girls_on_job(JOB_BOOBJOB, Day0Night1) +
            building.num_girls_on_job(JOB_VAGINAREJUV, Day0Night1) +
            building.num_girls_on_job(JOB_FACELIFT, Day0Night1) +
            building.num_girls_on_job(JOB_ASSJOB, Day0Night1) +
            building.num_girls_on_job(JOB_TUBESTIED, Day0Night1) +
            building.num_girls_on_job(JOB_FERTILITY, Day0Night1));
}


int Num_Actress(const IBuilding& building)
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > Num_Actress
    int actresses = 0;
    for (int i = JOB_FILMACTION; i < JOB_FILMRANDOM + 1; i++)
    {
        actresses += building.num_girls_on_job(JOBS(i), 1);
    }

    return actresses;
}

bool is_Actress_Job(int testjob)
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >>cMovieStudio.cpp > is_Actress_Job
    return (testjob > JOB_STAGEHAND && testjob <= JOB_FILMRANDOM);
}

bool CrewNeeded(const IBuilding& building)	// `J` added, if CM and CP both on duty or there are no actresses, return false
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > CrewNeeded
    if ((building.num_girls_on_job(JOB_CAMERAMAGE, 1) > 0 &&
         building.num_girls_on_job(JOB_CRYSTALPURIFIER, 1) > 0) ||
        Num_Actress(building) < 1)
        return false;	// a CM or CP is not Needed
    return true;	// Otherwise a CM or CP is Needed
}

int Num_Patients(const IBuilding& building, bool at_night)
{
    return building.num_girls_on_job(JOB_REHAB, at_night) +
            building.num_girls_on_job(JOB_ANGER, at_night) +
            building.num_girls_on_job(JOB_EXTHERAPY, at_night) +
            building.num_girls_on_job(JOB_THERAPY, at_night);
}

