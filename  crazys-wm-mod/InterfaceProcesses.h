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
#include "tinyxml.h"
using namespace std;

void MainMenu();
void GetString();
void GetInt();
void NewGame();
void BrothelScreen();
void Turnsummary();
void NextWeek();
void GameEvents();
void Gallery();
void Gallery2();
void SaveGameXML(string filename);
bool LoadGame(string directory, string filename);
bool LoadGameXML(TiXmlHandle hDoc);
//This legacy loader, and others like it, will load the game from its old format
//and the game will be saved in XML format thenafter.
//No SaveGameLegacy is necessary, and all savegame format changes
//should take place in SaveGameXML / LoadGameXML, which is extensible
//Therefore, there is a one time conversion, but saves are not invalidated
bool LoadGameLegacy(string directory, string filename);
void LoadGameScreen();
//the master file has been done away with! yay! It was only a stub anyways
//all the info is now in the XML savegame
//void SaveMasterFile(string filename);
void LoadGameInfoFiles();
void LoadGirlsFiles();
void TransferGirls();
void TransferGirlsRightToLeft(int rightBrothel, int leftBrothel);
void TransferGirlsLeftToRight(int rightBrothel, int leftBrothel);
#endif
