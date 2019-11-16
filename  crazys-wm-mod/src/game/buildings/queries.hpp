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

#pragma once

#ifndef CRAZYS_WM_MOD_QUERIES_HPP
#define CRAZYS_WM_MOD_QUERIES_HPP

class IBuilding;

bool DoctorNeeded(IBuilding& building);
int GetNumberPatients(IBuilding& building, bool Day0Night1);
bool CrewNeeded(const IBuilding& building);
bool is_Actress_Job(int testjob);
int Num_Actress(const IBuilding& building);
int Num_Patients(const IBuilding& building, bool is_night);

#endif //CRAZYS_WM_MOD_QUERIES_HPP
