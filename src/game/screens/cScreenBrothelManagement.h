/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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

#include "BuildingScreenManagement.h"


class IBuildingScreen : public cGameWindow {
private:
    BuildingType m_Type;
protected:
    int buildinglabel_id = -1;
    int background_id = -1;
    int walk_id = -1;            // Walk Around Town button

    int weeks_id = -1;           // next week map button
    int details_id = -1;         // description text
    int save_id = -1;
    int quit_id = -1;

    int girlimage_id = -1;        // Girl image

    int nextbrothel_id = -1;        // next brothel button
    int prevbrothel_id = -1;        // prev brothel button

    void init(bool back) override;

    void set_ids() override;

    // walking
    void try_walk();

public:
    IBuildingScreen(const char * base_file, BuildingType building);
    void process() override;
};

struct CBuildingScreenDispatch: public cInterfaceWindow
{
    CBuildingScreenDispatch();
    void load() {};
    void process() override {}
    void init(bool back) override;
};

// ---------------------------------------------------------------------------------------------------------------------

class cScreenArena : public IBuildingScreen
{
public:
    cScreenArena();
};

class cScreenCentre : public IBuildingScreen
{
public:
    cScreenCentre();

};

class cScreenClinic : public IBuildingScreen
{
public:
    cScreenClinic();
};

class cScreenBrothelManagement : public IBuildingScreen
{
public:
    cScreenBrothelManagement();
};

class cScreenFarm : public IBuildingScreen
{
public:
    cScreenFarm();
};

class cScreenHouse : public IBuildingScreen
{
public:
    cScreenHouse();
};

class cMovieScreen : public IBuildingScreen
{
public:
    cMovieScreen();

};