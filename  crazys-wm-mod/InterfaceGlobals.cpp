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

#include "src/interface/cColor.h"
#include "DirPath.h"
#include "cTariff.h"
#include "interface/cWindowManager.h"
#include "src/screens/cScreenPropertyManagement.h"	// `J` added
#include "src/screens/cScreenPrison.h"
#include "src/screens/cScreenTown.h"
#include "src/screens/cScreenSlaveMarket.h"
#include "src/screens/cScreenMayor.h"
#include "src/screens/cScreenBank.h"
#include "src/screens/cScreenHouseDetails.h"
#include "src/screens/cScreenItemManagement.h"
#include "src/screens/cScreenBuildingSetup.h"
#include "src/screens/cScreenGangs.h"
#include "src/screens/cScreenGirlDetails.h"
#include "src/screens/cScreenDungeon.h"
#include "src/screens/cScreenMainMenu.h"
#include "src/screens/cScreenPreparingGame.h"
#include "src/screens/cScreenNewGame.h"
#include "src/screens/cScreenLoadGame.hpp"
#include "src/screens/cScreenSettings.h"
#include "src/screens/cScreenBrothelManagement.h"
#include "src/screens/cScreenGetInput.h"
#include "src/screens/cScreenGallery.h"
#include "XmlUtil.h"
#include "src/screens/cScreenTurnSummary.h"
#include "src/screens/cScreenTransfer.h"
#include "tinyxml.h"
#include "CLog.h"
#include "sConfig.h"

extern cConfig cfg;

cScreenGirlDetails*       g_GirlDetails       = nullptr;
cScreenGetInput*          g_GetInput          = nullptr;


// interface colors
sColor g_ChoiceMessageTextColor{0, 0, 0};
sColor g_ChoiceMessageBorderColor{0, 0, 0};
sColor g_ChoiceMessageHeaderColor{229, 227, 52};
sColor g_ChoiceMessageBackgroundColor{88, 163, 113};
sColor g_ChoiceMessageSelectedColor{229, 227, 52};

sColor g_EditBoxBorderColor{0, 0, 0};
sColor g_EditBoxBackgroundColor{90, 172, 161};
sColor g_EditBoxSelectedColor{114, 211, 198};
sColor g_EditBoxTextColor{0, 0, 0};

sColor g_CheckBoxBorderColor{0, 0, 0};
sColor g_CheckBoxBackgroundColor{180, 180, 180};

sColor g_WindowBorderColor{0, 0, 0};
sColor g_WindowBackgroundColor{140, 191, 228};

sColor g_ListBoxBorderColor{0, 0, 0};
sColor g_ListBoxBackgroundColor{217, 214, 139};
sColor g_ListBoxElementBackgroundColor[] = {{114, 139, 217}, {200, 30, 30}, {80, 80, 250},
                                            {30, 190, 30}, {190, 190, 00}};
sColor g_ListBoxSelectedElementColor[] = {{187,   90, 224}, {255, 167, 180}, {187, 190, 224},
                                          {0, 250, 0}, {250, 250, 250}};
sColor g_ListBoxElementBorderColor{79, 79, 111};
sColor g_ListBoxElementBorderHColor{159, 175, 255};
sColor g_ListBoxTextColor{0, 0, 0};

sColor g_ListBoxHeaderBackgroundColor{25, 100, 144};
sColor g_ListBoxHeaderBorderColor{120, 155, 176};
sColor g_ListBoxHeaderBorderHColor{15, 49, 69};
sColor g_ListBoxHeaderTextColor{255, 255, 255};

sColor g_MessageBoxBorderColor{255, 255, 255};
sColor g_MessageBoxBackgroundColor[] =  {{100, 100, 150}, {200, 100, 150}, {100, 200, 150},
                                         {100, 100, 200}, {190, 190, 0}};
sColor g_MessageBoxTextColor{0, 0, 0};

template<class T>
T* load_window(const char* name, bool nonav=false);

void LoadInterface()
{
	stringstream ss;
	string image; string text; string file;
	ifstream incol;


	g_LogFile.write("Begin Loading Interface");

	// load interface colors
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "InterfaceColors.xml";
	TiXmlDocument docInterfaceColors(dp.c_str());
	if (docInterfaceColors.LoadFile())
	{
		g_LogFile.write("Loading InterfaceColors.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docInterfaceColors.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			string tag = el->ValueStr();
			if (tag == "Color")
			{
				XmlUtil xu(m_filename); string name; int r, g, b;
				xu.get_att(el, "Name", name);
				xu.get_att(el, "R", r); xu.get_att(el, "G", g); xu.get_att(el, "B", b);
				     if (name == "ChoiceBoxText")						{ g_ChoiceMessageTextColor = sColor(r, g, b); }
				else if (name == "ChoiceBoxBorder")						{ g_ChoiceMessageBorderColor = sColor(r, g, b); }
				else if (name == "ChoiceBoxHeader")						{ g_ChoiceMessageHeaderColor = sColor(r, g, b); }
				else if (name == "ChoiceBoxBackground")					{ g_ChoiceMessageBackgroundColor = sColor(r, g, b); }
				else if (name == "ChoiceBoxSelected")					{ g_ChoiceMessageSelectedColor = sColor(r, g, b); }
				else if (name == "EditBoxBorder")						{ g_EditBoxBorderColor = sColor(r, g, b); }
				else if (name == "EditBoxBackground")					{ g_EditBoxBackgroundColor = sColor(r, g, b); }
				else if (name == "EditBoxSelected")						{ g_EditBoxSelectedColor = sColor(r, g, b); }
				else if (name == "EditBoxText")							{ g_EditBoxTextColor = sColor(r, g, b); }
				else if (name == "WindowBorder")						{ g_WindowBorderColor = sColor(r, g, b); }
				else if (name == "WindowBackground")					{ g_WindowBackgroundColor = sColor(r, g, b); }
				else if (name == "ListBoxBorder")						{ g_ListBoxBorderColor = sColor(r, g, b); }
				else if (name == "ListBoxBackground")					{ g_ListBoxBackgroundColor = sColor(r, g, b); }
				else if (name == "ListBoxElementBackground")			{ g_ListBoxElementBackgroundColor[0] = sColor(r, g, b); }
				else if (name == "ListBoxSpecialElement1")				{ g_ListBoxElementBackgroundColor[1] = sColor(r, g, b); }
				else if (name == "ListBoxSpecialElement2")				{ g_ListBoxElementBackgroundColor[2] = sColor(r, g, b);; }
				else if (name == "ListBoxSpecialElement3")				{ g_ListBoxElementBackgroundColor[3] = sColor(r, g, b); }
				else if (name == "ListBoxSpecialElement4")				{ g_ListBoxElementBackgroundColor[4] = sColor(r, g, b); }
				else if (name == "ListBoxSelectedElement")				{ g_ListBoxSelectedElementColor[0] = sColor(r, g, b); }
				else if (name == "ListBoxSelectedSpecialElement1")		{ g_ListBoxSelectedElementColor[1] = sColor(r, g, b); }
				else if (name == "ListBoxSelectedSpecialElement2")		{ g_ListBoxSelectedElementColor[2] = sColor(r, g, b); }
				else if (name == "ListBoxSelectedSpecialElement3")		{ g_ListBoxSelectedElementColor[3] = sColor(r, g, b); }
				else if (name == "ListBoxSelectedSpecialElement4")		{ g_ListBoxSelectedElementColor[4] = sColor(r, g, b); }
				else if (name == "ListBoxElementBorderTopLeft")			{ g_ListBoxElementBorderColor = sColor(r, g, b); }
				else if (name == "ListBoxElementBorderBottomRight")		{ g_ListBoxElementBorderHColor = sColor(r, g, b); }
				else if (name == "ListBoxFont")							{ g_ListBoxTextColor = sColor(r, g, b); }
				else if (name == "ListBoxColumnHeaderBackground")		{ g_ListBoxHeaderBackgroundColor = sColor(r, g, b); }
				else if (name == "ListBoxColumnHeaderBorderTopLeft")	{ g_ListBoxHeaderBorderColor = sColor(r, g, b); }
				else if (name == "ListBoxColumnHeaderBorderBottomRight"){ g_ListBoxHeaderBorderHColor = sColor(r, g, b); }
				else if (name == "ListBoxColumnHeaderFont")				{ g_ListBoxHeaderTextColor = sColor(r, g, b); }
				else if (name == "MessageBoxBorder")					{ g_MessageBoxBorderColor = sColor(r, g, b); }
				else if (name == "MessageBoxBackground0")				{ g_MessageBoxBackgroundColor[0] = sColor(r, g, b); }
				else if (name == "MessageBoxBackground1")				{ g_MessageBoxBackgroundColor[1] = sColor(r, g, b); }
				else if (name == "MessageBoxBackground2")				{ g_MessageBoxBackgroundColor[2] = sColor(r, g, b); }
				else if (name == "MessageBoxBackground3")				{ g_MessageBoxBackgroundColor[3] = sColor(r, g, b); }
				else if (name == "MessageBoxBackground4")				{ g_MessageBoxBackgroundColor[4] = sColor(r, g, b); }
				else if (name == "MessageBoxText")						{ g_MessageBoxTextColor = sColor(r, g, b); }
				else if (name == "CheckboxBorder")						{ g_CheckBoxBorderColor = sColor(r, g, b); }
				else if (name == "CheckboxBackground")					{ g_CheckBoxBackgroundColor = sColor(r, g, b); }
				// ItemRarity is loaded in sConfig.cpp
			}
		}
	}
	else
	{
		g_LogFile.write("Keeping Default InterfaceColors");
	}

    window_manager().load();
    // `J` Bookmark - Loading the screens
    load_window<cScreenPreparingGame>("Preparing Game", true);
    load_window<cScreenMainMenu>("Main Menu", true);
    load_window<cScreenNewGame>("New Game", true);
    load_window<cScreenLoadGame>("Load Game", true);
    load_window<cScreenSettings>("Settings", true);

    load_window<cScreenBrothelManagement>("Brothel Management");
    g_GirlDetails = load_window<cScreenGirlDetails>("Girl Details");
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
}

template<class T>
T* load_window(const char* name, bool nonav)
{
    g_LogFile.write(std::string("Loading Window '") + name + "'");
    auto window = std::make_shared<T>();
    auto result = window.get();
    if(!nonav) {
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