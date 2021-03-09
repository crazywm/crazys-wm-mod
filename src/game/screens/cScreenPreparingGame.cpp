/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http:  //pinkpetal.org
*
* This program is free software:   you can redistribute it and/or modify
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
* along with this program.  If not, see <http:  //www.gnu.org/licenses/>.
*/
#include <future>
#include "cScreenPreparingGame.h"
#include "buildings/cBuildingManager.h"
#include "buildings/house/cHouse.h"
#include "utils/FileList.h"
#include "utils/DirPath.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "cRival.h"
#include "buildings/brothel/cBrothel.h"
#include "IGame.h"
#include "character/cCustomers.h"
#include "cInventory.h"
#include "CLog.h"
#include "utils/string.hpp"

namespace settings {
    extern const char* INITIAL_RANDOM_GANGS;
    extern const char* INITIAL_BOOSTED_GANGS;
}

extern std::string g_ReturnText;
extern int g_ReturnInt;

extern bool g_WalkAround;
extern int g_TalkCount;

int load0new1 = 0;

cScreenPreparingGame::cScreenPreparingGame() : cGameWindow("preparing_game_screen.xml")
{
}
void cScreenPreparingGame::set_ids()
{
    text1_id  = get_id("Text1");
    text2_id  = get_id("Text2");
    text3_id  = get_id("Text3");

    cancel_id = get_id("BackButton","Back");

    SetButtonCallback(cancel_id, [this]() {
        if (!m_Loading)
        {
            m_Loading = true;
            pop_to_window("Main Menu");
        }
    });

}

void cScreenPreparingGame::init(bool back)
{
    LoadNames();
    EditTextItem("", text2_id);
    EditTextItem("", text3_id);

    if(!g_Game) {
        g_Game = IGame::CreateGame();
    }

    // make sure we clear any remaining active girl from the previous game
    set_active_girl(nullptr);

    if (g_ReturnInt >= 0)
    {
        resetScreen();

        m_Loading = true;
        load0new1 = g_ReturnInt;
        if(g_ReturnInt != 0) {
            std::stringstream ss1;
            ss1 << "Starting New Game:   " << g_ReturnText;
            EditTextItem(ss1.str(), text1_id);
            m_AsyncLoad = std::async(std::launch::async,
                       [this](){
               return NewGame(g_ReturnText);
            });
        } else {
            std::stringstream ss1;
            // TODO strip the save path here?
            ss1 << "Loading Game:   " << g_ReturnText;
            EditTextItem(ss1.str(), text1_id);
            m_AsyncLoad = std::async(std::launch::async,
                                     [this](){
                                         return LoadGame(g_ReturnText);
                                     });
        }
        g_ReturnInt = -1;
        return;
    }
    Focused();
}

void cScreenPreparingGame::resetScreen()
{
    m_MessagesText.str("");
    EditTextItem("", text1_id);
    EditTextItem("", text2_id);
}

bool cScreenPreparingGame::NewGame(std::string name) {
    g_WalkAround = false;
    g_TalkCount = 10;

    auto callback = [this](std::string str) {
        std::lock_guard<std::mutex> lck(m_Mutex);
        if(starts_with(str, "ERROR:")) {
            m_LastError = str;
        }
        m_NewMessages.push_back(std::move(str));
    };
    g_Game->NewGame(callback);

    callback("Setting Up Buildings");
    g_LogFile.info("prepare", "Adding Brothel");
    auto& new_brot = g_Game->buildings().AddBuilding(std::unique_ptr<IBuilding>(new sBrothel()));
    new_brot.m_NumRooms = 20;
    new_brot.m_MaxNumRooms = 250;
    new_brot.set_name(name);
    new_brot.set_background_image("Brothel0.jpg");

    g_LogFile.info("prepare", "Adding House");
    auto& new_house = g_Game->buildings().AddBuilding(std::unique_ptr<IBuilding>(new sHouse()));
    new_house.m_NumRooms = 20;
    new_house.m_MaxNumRooms = 200;
    new_house.set_background_image("House.jpg");

    callback("Setting Up Gangs And Rivals");
    g_LogFile.info("prepare", "Setting Up Gangs");
    int start_random_gangs = g_Game->settings().get_integer(settings::INITIAL_RANDOM_GANGS);
    int start_boosted_gangs = g_Game->settings().get_integer(settings::INITIAL_BOOSTED_GANGS);
    for (int i = 0; i < start_random_gangs; i++)     g_Game->gang_manager().AddNewGang(false);
    for (int i = 0; i < start_boosted_gangs; i++)    g_Game->gang_manager().AddNewGang(true);

    g_LogFile.info("prepare", "Setting Up Rivals");
    // Add the beginning rivals
    for (int i = 0; i < 5; i++)
    {
        int str = g_Dice % 10 + 1;
        g_Game->rivals().CreateRival(
                str * 100,                                // BribeRate    = 100-1000
                (str * 3) + (g_Dice % 11),                 // Businesses    = 3-40
                str * 5000,                             // Gold            = 5000-50000
                (str / 2) + 1,                             // Bars            = 1-6
                (str / 4) + 1,                             // GambHalls    = 1-3
                (str * 5) + (g_Dice % (str * 5)),         // Girls        = 5-100
                (str / 2) + 1,                             // Brothels        = 1-6
                g_Dice % 6 + 1,                         // Gangs        = 1-6
                str                                         // Power        = 1-10    // `J` added - The rivals power level
        );
    }

    callback("Creating Savegame");
    g_LogFile.info("prepare", "Saving");
    if(g_ReturnText == "Cheat") {
        g_Game->enable_cheating();
    }
    SaveGame();
    return true;
}

bool cScreenPreparingGame::LoadGame(const std::string& file_path) {
    auto callback = [this](std::string str) {
        std::lock_guard<std::mutex> lck(m_Mutex);
        if(starts_with(str, "ERROR:")) {
            m_LastError = str;
        }
        m_NewMessages.push_back(std::move(str));
    };

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(file_path.c_str()) != tinyxml2::XML_SUCCESS) {
        m_Loading = false;
        callback(doc.ErrorStr());
        return false;
    }
    auto pRoot = doc.FirstChildElement("Root");
    if (pRoot == nullptr) {
        return false;
    }
    // load the version
    int minorA = -1;
    pRoot->QueryIntAttribute("MinorVersionA", &minorA);
    if (minorA != 7) {
        callback("You must start a new game with this version");
        return false;
    }
    std::string version("<blank>");
    if (pRoot->Attribute("ExeVersion")) { version = pRoot->Attribute("ExeVersion"); }
    if (version != "official") {
        callback("Warning, the exe was not detected as official, it was detected as " + version + ".  Attempting to load anyways.");
    }

    g_Game->LoadGame(*pRoot, callback);

    g_WalkAround = false;       pRoot->QueryAttribute("WalkAround", &g_WalkAround);
    g_TalkCount = 0;            pRoot->QueryIntAttribute("TalkCount", &g_TalkCount);
    if (g_Game->allow_cheats()) { g_WalkAround = false; g_TalkCount = 10; }
    return true;
}

void cScreenPreparingGame::loadFailed()
{
    EditTextItem("Something went wrong while loading the game. Click the back button to go to the main menu.", text3_id);
}

void cScreenPreparingGame::process()
{
    std::string last_error;
    {
        std::lock_guard<std::mutex> lck(m_Mutex);
        for(auto& msg : m_NewMessages) {
            m_MessagesText << msg << "\n";
        }
        if(!m_NewMessages.empty()) {
            EditTextItem(m_MessagesText.str(), text2_id);
        }
        m_NewMessages.clear();

        last_error = m_LastError;
        m_LastError.clear();
    }

    DisableWidget(cancel_id, m_Loading);
    if (!m_Loading)
    {
        loadFailed();
        return;
    }

    if(!last_error.empty())
        push_error(std::move(last_error));

    if(m_AsyncLoad.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
        return;
    } else {
        try {
            if (!m_AsyncLoad.get()) {
                m_Loading = false;
                return;
            }
        } catch (const std::exception& ex) {
            push_message(ex.what(), 1);
            m_Loading = false;
            return;
        }
        g_LogFile.info("prepare", "Finished");
        set_active_building(&g_Game->buildings().get_building(0));
        push_window("Building Management");
        if(load0new1 == 1) {
            g_Game->PushEvent("intro");
        }
    }
}

