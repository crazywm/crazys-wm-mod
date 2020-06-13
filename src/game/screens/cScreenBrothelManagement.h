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

#include "BuildingScreenManagement.h"


class IBuildingScreen : public cGameWindow {
private:
    BuildingType m_Type;
    bool* m_HasDoneWalkThisWeek = nullptr;
protected:
    bool m_first_walk = true;

    int buildinglabel_id = -1;
    int background_id = -1;
    int walk_id = -1;            // Walk Around Town button

    int weeks_id = -1;            // next week map button
    int    details_id = -1;    // description text
    int girls_id = -1;            // girl management map button
    int staff_id = -1;            // gang map button
    int setup_id = -1;            // setup map button
    int dungeon_id = -1;            //
    int turns_id = -1;            // turn summary map button
    int town_id = -1;
    int save_id = -1;
    int quit_id = -1;

    int girlimage_id = -1;        // Girl image

    int nextbrothel_id = -1;        // next brothel button
    int prevbrothel_id = -1;        // prev brothel button

    void init(bool back) override;

    void set_ids() override;

    // walking
    void try_walk();
    virtual void do_walk(sGirl* girl) {};
    virtual std::string walk_no_luck();

public:
    IBuildingScreen(const char * base_file, BuildingType building, bool * has_walked);
    void process() override;
};

struct CBuildingScreenDispatch: public cInterfaceWindow
{
    void load() {};
    void process() override {}
    void init(bool back) override;
};

// ---------------------------------------------------------------------------------------------------------------------

class cScreenArena : public IBuildingScreen
{
public:
    cScreenArena();
    void do_walk(sGirl * girl) override;
    std::string walk_no_luck() override;
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

    void do_walk(sGirl * girl) override;
    std::string walk_no_luck() override;
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
private:
    int house_id;
    void set_ids() override;

public:
    cScreenHouse();
};

class cMovieScreen : public IBuildingScreen
{
private:
    int createmovie_id;        // Create Movie

    void set_ids() override;
    void do_walk(sGirl* girl) override;

    std::string walk_no_luck() override;
public:
    cMovieScreen();

};