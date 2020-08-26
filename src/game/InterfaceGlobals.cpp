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

#include <chrono>
#include "interface/cColor.h"
#include "utils/DirPath.h"
#include "interface/cWindowManager.h"
#include "screens/cScreenPropertyManagement.h"    // `J` added
#include "screens/cScreenPrison.h"
#include "screens/cScreenTown.h"
#include "screens/cScreenSlaveMarket.h"
#include "screens/cScreenMayor.h"
#include "screens/cScreenBank.h"
#include "screens/cScreenHouseDetails.h"
#include "screens/cScreenItemManagement.h"
#include "screens/cScreenBuildingSetup.h"
#include "screens/cScreenGangs.h"
#include "screens/cScreenGirlDetails.h"
#include "screens/cScreenDungeon.h"
#include "screens/cScreenMainMenu.h"
#include "screens/cScreenPreparingGame.h"
#include "screens/cScreenNewGame.h"
#include "screens/cScreenLoadGame.hpp"
#include "screens/cScreenSettings.h"
#include "screens/cScreenBrothelManagement.h"
#include "widgets/cScreenGetInput.h"
#include "screens/cScreenGallery.h"
#include "screens/cScreenTurnSummary.h"
#include "screens/cScreenTransfer.h"
#include "screens/cScreenGameConfig.h"
#include <tinyxml2.h>
#include "CLog.h"
#include "xml/util.h"
#include "xml/getattr.h"

cScreenGetInput*          g_GetInput          = nullptr;

template<class T, class... Args>
T* load_window(const char* name, bool nonav=false, Args&&... args);

sColor& LookupThemeColor(const std::string& name);

void LoadInterface(const std::string& theme)
{
    std::stringstream ss;
    std::string image; std::string text; std::string file;
    std::ifstream incol;

    // load interface colors
    try {
        DirPath dp = DirPath() << "Resources" << "Interface" << theme << "InterfaceColors.xml";
        auto docInterfaceColors = LoadXMLDocument(dp.c_str());
        g_LogFile.log(ELogLevel::INFO,"Loading InterfaceColors.xml");
        const std::string& m_filename = dp.str();
        for (auto& el : IterateChildElements(*docInterfaceColors->RootElement())) {
            std::string tag = el.Value();
            if (tag == "Color") {
                try {
                    const char* name = GetStringAttribute(el, "Name");
                    auto& target = LookupThemeColor(name);
                    GetColorAttribute(el, target);
                } catch(std::runtime_error& error) {
                    g_LogFile.log(ELogLevel::ERROR, "Could not load color definition from ", m_filename,
                            "(", el.GetLineNum(), "): ", error.what());
                    continue;
                }
                // ItemRarity is loaded in sConfig.cpp
            }
        }
    } catch(std::runtime_error& error) {
        g_LogFile.log(ELogLevel::ERROR, "Could not load interface colors: ", error.what());
        g_LogFile.log(ELogLevel::INFO, "Keeping Default InterfaceColors");
    }

    g_LogFile.debug("interface", "Loading screens");
    auto start_time = std::chrono::steady_clock::now();

    // `J` Bookmark - Loading the screens
    load_window<cScreenPreparingGame>("Preparing Game", true);
    load_window<cScreenMainMenu>("Main Menu", true);
    load_window<cScreenNewGame>("New Game", true);
    load_window<cScreenLoadGame>("Load Game", true);
    load_window<cScreenSettings>("Settings", true);

    load_window<cScreenBrothelManagement>("Brothel Management");
    load_window<cScreenGirlDetails>("Girl Details");
    load_window<cScreenGangs>("Gangs");
    load_window<cScreenItemManagement>("Item Management");
    load_window<cMovieScreen>("Movie Screen");
    load_window<cScreenPropertyManagement>("Property Management");
    load_window<cScreenTransfer>("Transfer Screen");
    load_window<cScreenTurnSummary>("Turn Summary");
    load_window<cScreenGallery>("Gallery");
    g_GetInput = load_window<cScreenGetInput>("GetInput");

    load_window<CBuildingManagementScreenDispatch>("Girl Management");
    load_window<CBuildingScreenDispatch>("Building Management");
    load_window<cScreenGirlManagement>("Girl Management Brothel");
    load_window<cScreenClinicManagement>("Girl Management Clinic");
    load_window<cScreenStudioManagement>("Girl Management Studio");
    load_window<cScreenArenaManagement>("Girl Management Arena");
    load_window<cScreenCentreManagement>("Girl Management Centre");
    load_window<cScreenHouseManagement>("Girl Management House");
    load_window<cScreenFarmManagement>("Girl Management Farm");
    load_window<cScreenClinic>("Clinic Screen");
    load_window<cScreenCentre>("Centre Screen");
    load_window<cScreenArena>("Arena Screen");
    load_window<cScreenHouse>("Player House");
    load_window<cScreenFarm>("Farm Screen");
    load_window<cScreenDungeon>("Dungeon");
    load_window<cScreenTown>("Town");
    load_window<cScreenSlaveMarket>("Slave Market");
    load_window<cScreenBuildingSetup>("Building Setup");
    load_window<cScreenMayor>("Mayor");
    load_window<cScreenBank>("Bank");
    load_window<cScreenHouseDetails>("House");
    load_window<cScreenPrison>("Prison");
    load_window<cScreenGameConfig>("GameSetup", false, false);
    load_window<cScreenGameConfig>("UserSettings", false, true);

    auto duration = std::chrono::steady_clock::now() - start_time;
    g_LogFile.debug("interface", "Loaded screens in ", std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(), "ms");
}

sColor& LookupThemeColor(const std::string& name) {
    if (name == "ChoiceBoxText") { return g_ChoiceMessageTextColor; }
    else if (name == "ChoiceBoxBorder") { return g_ChoiceMessageBorderColor; }
    else if (name == "ChoiceBoxHeader") { return g_ChoiceMessageHeaderColor; }
    else if (name == "ChoiceBoxBackground") { return g_ChoiceMessageBackgroundColor; }
    else if (name == "ChoiceBoxSelected") { return g_ChoiceMessageSelectedColor; }
    else if (name == "EditBoxBorder") { return g_EditBoxBorderColor; }
    else if (name == "EditBoxBackground") { return g_EditBoxBackgroundColor; }
    else if (name == "EditBoxSelected") { return g_EditBoxSelectedColor; }
    else if (name == "EditBoxText") { return g_EditBoxTextColor; }
    else if (name == "WindowBorder") { return g_WindowBorderColor; }
    else if (name == "WindowBackground") { return g_WindowBackgroundColor; }
    else if (name == "ListBoxBorder") { return g_ListBoxBorderColor; }
    else if (name == "ListBoxBackground") { return g_ListBoxBackgroundColor; }
    else if (name == "ListBoxElementBackground") { return g_ListBoxElementBackgroundColor[0]; }
    else if (name == "ListBoxSpecialElement1") { return g_ListBoxElementBackgroundColor[1]; }
    else if (name == "ListBoxSpecialElement2") { return g_ListBoxElementBackgroundColor[2];; }
    else if (name == "ListBoxSpecialElement3") { return g_ListBoxElementBackgroundColor[3]; }
    else if (name == "ListBoxSpecialElement4") { return g_ListBoxElementBackgroundColor[4]; }
    else if (name == "ListBoxSelectedElement") { return g_ListBoxSelectedElementColor[0]; }
    else if (name == "ListBoxSelectedSpecialElement1") {
        return g_ListBoxSelectedElementColor[1];
    }
    else if (name == "ListBoxSelectedSpecialElement2") {
        return g_ListBoxSelectedElementColor[2];
    }
    else if (name == "ListBoxSelectedSpecialElement3") {
        return g_ListBoxSelectedElementColor[3];
    }
    else if (name == "ListBoxSelectedSpecialElement4") {
        return g_ListBoxSelectedElementColor[4];
    }
    else if (name == "ListBoxElementBorderTopLeft") { return g_ListBoxElementBorderColor; }
    else if (name == "ListBoxElementBorderBottomRight") { return g_ListBoxElementBorderHColor; }
    else if (name == "ListBoxFont") { return g_ListBoxTextColor; }
    else if (name == "ListBoxColumnHeaderBackground") { return g_ListBoxHeaderBackgroundColor; }
    else if (name == "ListBoxColumnHeaderBorderTopLeft") { return g_ListBoxHeaderBorderColor; }
    else if (name == "ListBoxColumnHeaderBorderBottomRight") {
        return g_ListBoxHeaderBorderHColor;
    }
    else if (name == "ListBoxColumnHeaderFont") { return g_ListBoxHeaderTextColor; }
    else if (name == "MessageBoxBorder") { return g_MessageBoxBorderColor; }
    else if (name == "MessageBoxBackground0") { return g_MessageBoxBackgroundColor[0]; }
    else if (name == "MessageBoxBackground1") { return g_MessageBoxBackgroundColor[1]; }
    else if (name == "MessageBoxBackground2") { return g_MessageBoxBackgroundColor[2]; }
    else if (name == "MessageBoxBackground3") { return g_MessageBoxBackgroundColor[3]; }
    else if (name == "MessageBoxBackground4") { return g_MessageBoxBackgroundColor[4]; }
    else if (name == "MessageBoxText") { return g_MessageBoxTextColor; }
    else if (name == "CheckboxBorder") { return g_CheckBoxBorderColor; }
    else if (name == "CheckboxBackground") { return g_CheckBoxBackgroundColor; }

    g_LogFile.log(ELogLevel::DEBUG, "Invalid color specification '", name, "' has been ignored");
    static sColor null{0, 0, 0};
    return null;
}

template<class T, class... Args>
T* load_window(const char* name, bool nonav, Args&&... args)
{
    g_LogFile.log(ELogLevel::DEBUG, "Loading Window '", name, '\'');
    auto window = std::make_shared<T>(std::forward<Args>(args)...);
    auto result = window.get();
    if (!nonav) {
        register_global_nav_keys(*window);
    }
    window_manager().add_window(name, std::move(window));
    return result;
}


void register_global_nav_keys(cInterfaceWindow& window) {
    window.AddKeyCallback(SDLK_F1, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Girl Management");
    });

    window.AddKeyCallback(SDLK_F2, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Gangs");
    });

    window.AddKeyCallback(SDLK_F3, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Dungeon");
    });

    window.AddKeyCallback(SDLK_F4, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Slave Market");
    });

    window.AddKeyCallback(SDLK_F5, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Item Management");
    });

    window.AddKeyCallback(SDLK_F6, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Transfer Screen");
    });

    window.AddKeyCallback(SDLK_F7, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Prison");
    });

    window.AddKeyCallback(SDLK_F8, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Mayor");
    });

    window.AddKeyCallback(SDLK_F9, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Bank");
    });

    window.AddKeyCallback(SDLK_F10, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
    });

    window.AddKeyCallback(SDLK_F11, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Turn Summary");
    });

    window.AddKeyCallback(SDLK_F12, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Property Management");
    });
    /*
    1-7: Select Brothel
    Tab: Cycle Brothel (Shitf: fwd / back)
    Esc: Back
    F5 Studio
    e/F6 Arena
    F7 Centre
    c/F8 Clinic
    F9 Farm
    p/i Inventory
    F12 House
    m Mayor
    b Bank
    u Building Setup
    */
}