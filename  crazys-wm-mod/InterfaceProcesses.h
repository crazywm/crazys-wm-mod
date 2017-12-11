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
#ifndef __INTERFACEPROCESSES_H
#define __INTERFACEPROCESSES_H

#include <string>
#include <algorithm>
#include "tinyxml.h"
#include "cBrothel.h"
using namespace std;

void MainMenu();
void GetString();
void GetInt();
void NewGame();
void BrothelScreen();
void NextWeek();
void GameEvents();
void AutoSaveGame();
void SaveGame(bool saveCSV = false);
void SaveGameXML(string filename);
void SimpleSaveGameXML(string filename);
void LoadSettingsScreen();
void LoadGameScreen();
void LoadGameInfoFiles();
void LoadGirlsFiles();
void SaveGirlsCSV(string filename);
string Girl2CSV(sGirl* girl);
string CSVifyString(string name);
#endif
