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

#ifndef CRAZYS_WM_MOD_GANG_MISSIONS_HPP
#define CRAZYS_WM_MOD_GANG_MISSIONS_HPP

#include <string>
#include <memory>

class sGang;
class sGirl;
class cGangManager;

class IGangMission
{
public:
    IGangMission(std::string name, bool potions, bool nets);
    virtual ~IGangMission() = default;

    const std::string& name() const;
    bool requires_potions() const;
    bool requires_nets() const;

    bool run(sGang& gang);

protected:
    virtual bool execute_mission(sGang& gang, std::stringstream& event_text) = 0;
    cGangManager& gang_manager();
private:
    std::string m_Name;
    bool m_RequiresPotions = false;
    bool m_RequiresNets = false;
};

class cMissionGuarding : public IGangMission {
public:
    cMissionGuarding();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override { return true; };
};

class cMissionSpyGirls : public IGangMission {
public:
    cMissionSpyGirls();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override { return true; };
};


class cMissionGrandTheft : public IGangMission {
public:
    cMissionGrandTheft();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionKidnap : public IGangMission {
public:
    cMissionKidnap();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
private:
    bool kidnap(sGang& gang, std::stringstream& ss, std::shared_ptr<sGirl> girl);
};

class cMissionSabotage : public IGangMission {
public:
    cMissionSabotage();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionRecapture : public IGangMission {
public:
    cMissionRecapture();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionExtortion : public IGangMission {
public:
    cMissionExtortion();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionPettyTheft : public IGangMission {
public:
    cMissionPettyTheft();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionCatacombs : public IGangMission {
public:
    cMissionCatacombs();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionService : public IGangMission {
public:
    cMissionService();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionTraining : public IGangMission {
public:
    cMissionTraining();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

class cMissionRecruiting : public IGangMission {
public:
    cMissionRecruiting();
    bool execute_mission(sGang& gang, std::stringstream& event_text) override;
};

#endif //CRAZYS_WM_MOD_GANG_MISSIONS_HPP
