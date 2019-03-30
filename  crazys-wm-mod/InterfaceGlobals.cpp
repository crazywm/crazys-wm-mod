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

#include "InterfaceGlobals.h"
#include "DirPath.h"
#include "cTariff.h"
#include "cWindowManager.h"
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

using namespace std;

extern cWindowManager g_WinManager;

cScreenBrothelManagement* g_BrothelManagement = nullptr;
cScreenGirlDetails*       g_GirlDetails       = nullptr;
cScreenMainMenu*          g_MainMenu          = nullptr;
cScreenNewGame*           g_NewGame           = nullptr;
cScreenPreparingGame*     g_Preparing         = nullptr;
cInterfaceWindow*         g_LoadGame          = nullptr; // LoadMenu.xml
cScreenGetInput*          g_GetInput          = nullptr;
cChoiceManager g_ChoiceManager;

// interface colors
unsigned char g_StaticImageR = 0, g_StaticImageG = 0, g_StaticImageB = 0;

unsigned char g_ChoiceMessageTextR = 0, g_ChoiceMessageTextG = 0, g_ChoiceMessageTextB = 0;
unsigned char g_ChoiceMessageBorderR = 0, g_ChoiceMessageBorderG = 0, g_ChoiceMessageBorderB = 0;
unsigned char g_ChoiceMessageHeaderR = 229, g_ChoiceMessageHeaderG = 227, g_ChoiceMessageHeaderB = 52;
unsigned char g_ChoiceMessageBackgroundR = 0, g_ChoiceMessageBackgroundG = 0, g_ChoiceMessageBackgroundB = 0;
unsigned char g_ChoiceMessageSelectedR = 0, g_ChoiceMessageSelectedG = 0, g_ChoiceMessageSelectedB = 0;

unsigned char g_EditBoxBorderR = 0, g_EditBoxBorderG = 0, g_EditBoxBorderB = 0;
unsigned char g_EditBoxBackgroundR = 0, g_EditBoxBackgroundG = 0, g_EditBoxBackgroundB = 0;
unsigned char g_EditBoxSelectedR = 0, g_EditBoxSelectedG = 0, g_EditBoxSelectedB = 0;
unsigned char g_EditBoxTextR = 0, g_EditBoxTextG = 0, g_EditBoxTextB = 0;

unsigned char g_CheckBoxBorderR = 0, g_CheckBoxBorderG = 0, g_CheckBoxBorderB = 0;
unsigned char g_CheckBoxBackgroundR = 0, g_CheckBoxBackgroundG = 0, g_CheckBoxBackgroundB = 0;

unsigned char g_WindowBorderR = 0, g_WindowBorderG = 0, g_WindowBorderB = 0;
unsigned char g_WindowBackgroundR = 0, g_WindowBackgroundG = 0, g_WindowBackgroundB = 0;

unsigned char g_ListBoxBorderR = 0, g_ListBoxBorderG = 0, g_ListBoxBorderB = 0;
unsigned char g_ListBoxBackgroundR = 0, g_ListBoxBackgroundG = 0, g_ListBoxBackgroundB = 0;
unsigned char g_ListBoxElementBackgroundR = 0, g_ListBoxElementBackgroundG = 0, g_ListBoxElementBackgroundB = 0;
unsigned char g_ListBoxS1ElementBackgroundR = 0, g_ListBoxS1ElementBackgroundG = 0, g_ListBoxS1ElementBackgroundB = 0;
unsigned char g_ListBoxS2ElementBackgroundR, g_ListBoxS2ElementBackgroundG, g_ListBoxS2ElementBackgroundB;
unsigned char g_ListBoxS3ElementBackgroundR, g_ListBoxS3ElementBackgroundG, g_ListBoxS3ElementBackgroundB;
unsigned char g_ListBoxS4ElementBackgroundR, g_ListBoxS4ElementBackgroundG, g_ListBoxS4ElementBackgroundB;
unsigned char g_ListBoxSelectedElementR = 0, g_ListBoxSelectedElementG = 0, g_ListBoxSelectedElementB = 0;
unsigned char g_ListBoxSelectedS1ElementR = 0, g_ListBoxSelectedS1ElementG = 0, g_ListBoxSelectedS1ElementB = 0;
unsigned char g_ListBoxSelectedS2ElementR = 0, g_ListBoxSelectedS2ElementG = 0, g_ListBoxSelectedS2ElementB = 0;
unsigned char g_ListBoxSelectedS3ElementR = 0, g_ListBoxSelectedS3ElementG = 0, g_ListBoxSelectedS3ElementB = 0;
unsigned char g_ListBoxSelectedS4ElementR = 0, g_ListBoxSelectedS4ElementG = 0, g_ListBoxSelectedS4ElementB = 0;
unsigned char g_ListBoxElementBorderR = 0, g_ListBoxElementBorderG = 0, g_ListBoxElementBorderB = 0;
unsigned char g_ListBoxElementBorderHR = 0, g_ListBoxElementBorderHG = 0, g_ListBoxElementBorderHB = 0;
unsigned char g_ListBoxTextR = 0, g_ListBoxTextG = 0, g_ListBoxTextB = 0;

unsigned char g_ListBoxHeaderBackgroundR = 0, g_ListBoxHeaderBackgroundG = 0, g_ListBoxHeaderBackgroundB = 0;
unsigned char g_ListBoxHeaderBorderR = 0, g_ListBoxHeaderBorderG = 0, g_ListBoxHeaderBorderB = 0;
unsigned char g_ListBoxHeaderBorderHR = 0, g_ListBoxHeaderBorderHG = 0, g_ListBoxHeaderBorderHB = 0;
unsigned char g_ListBoxHeaderTextR = 0, g_ListBoxHeaderTextG = 0, g_ListBoxHeaderTextB = 0;

unsigned char g_MessageBoxBorderR = 0, g_MessageBoxBorderG = 0, g_MessageBoxBorderB = 0;
unsigned char g_MessageBoxBackground0R = 0, g_MessageBoxBackground0G = 0, g_MessageBoxBackground0B = 0;
unsigned char g_MessageBoxBackground1R = 0, g_MessageBoxBackground1G = 0, g_MessageBoxBackground1B = 0;
unsigned char g_MessageBoxBackground2R = 0, g_MessageBoxBackground2G = 0, g_MessageBoxBackground2B = 0;
unsigned char g_MessageBoxBackground3R = 0, g_MessageBoxBackground3G = 0, g_MessageBoxBackground3B = 0;
unsigned char g_MessageBoxBackground4R = 0, g_MessageBoxBackground4G = 0, g_MessageBoxBackground4B = 0;
unsigned char g_MessageBoxTextR = 0, g_MessageBoxTextG = 0, g_MessageBoxTextB = 0;

template<class T>
T* load_window(const char* name);

void FreeInterface()
{
    g_WinManager.FreeAllWindows();
}

void ResetInterface()
{
    g_WinManager.ResetAllWindows();
}

void LoadInterface()
{
	cTariff tariff;
	stringstream ss;
	int r = 0, g = 0, b = 0, x = 0, y = 0, w = 0, h = 0, a = 0, c = 0, d = 0, e = 0, fontsize = 10, rowheight = 20,
		increment = 0, min = 0, max = 0, value = 0;
	string image; string text; string file;
	bool Transparency = false, Scale = true, multi = false, events = false, liveUpdate = false, leftorright = false;
	ifstream incol;
	

	g_LogFile.write("Begin Loading Interface");

	// load interface colors
	int loadcolors = 0;		// 0=default, 1=xml, 2=txt
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
				     if (name == "ImageBackground")						{ g_StaticImageR = r; g_StaticImageG = g; g_StaticImageB = b; }
				else if (name == "ChoiceBoxText")						{ g_ChoiceMessageTextR = r; g_ChoiceMessageTextG = g; g_ChoiceMessageTextB = b; }
				else if (name == "ChoiceBoxBorder")						{ g_ChoiceMessageBorderR = r; g_ChoiceMessageBorderG = g; g_ChoiceMessageBorderB = b; }
				else if (name == "ChoiceBoxHeader")						{ g_ChoiceMessageHeaderR = r; g_ChoiceMessageHeaderG = g; g_ChoiceMessageHeaderB = b; }
				else if (name == "ChoiceBoxBackground")					{ g_ChoiceMessageBackgroundR = r; g_ChoiceMessageBackgroundG = g; g_ChoiceMessageBackgroundB = b; }
				else if (name == "ChoiceBoxSelected")					{ g_ChoiceMessageSelectedR = r; g_ChoiceMessageSelectedG = g; g_ChoiceMessageSelectedB = b; }
				else if (name == "EditBoxBorder")						{ g_EditBoxBorderR = r; g_EditBoxBorderG = g; g_EditBoxBorderB = b; }
				else if (name == "EditBoxBackground")					{ g_EditBoxBackgroundR = r; g_EditBoxBackgroundG = g; g_EditBoxBackgroundB = b; }
				else if (name == "EditBoxSelected")						{ g_EditBoxSelectedR = r; g_EditBoxSelectedG = g; g_EditBoxSelectedB = b; }
				else if (name == "EditBoxText")							{ g_EditBoxTextR = r; g_EditBoxTextG = g; g_EditBoxTextB = b; }
				else if (name == "WindowBorder")						{ g_WindowBorderR = r; g_WindowBorderG = g; g_WindowBorderB = b; }
				else if (name == "WindowBackground")					{ g_WindowBackgroundR = r; g_WindowBackgroundG = g; g_WindowBackgroundB = b; }
				else if (name == "ListBoxBorder")						{ g_ListBoxBorderR = r; g_ListBoxBorderG = g; g_ListBoxBorderB = b; }
				else if (name == "ListBoxBackground")					{ g_ListBoxBackgroundR = r; g_ListBoxBackgroundG = g; g_ListBoxBackgroundB = b; }
				else if (name == "ListBoxElementBackground")			{ g_ListBoxElementBackgroundR = r; g_ListBoxElementBackgroundG = g; g_ListBoxElementBackgroundB = b; }
				else if (name == "ListBoxSpecialElement1")				{ g_ListBoxS1ElementBackgroundR = r; g_ListBoxS1ElementBackgroundG = g; g_ListBoxS1ElementBackgroundB = b; }
				else if (name == "ListBoxSpecialElement2")				{ g_ListBoxS2ElementBackgroundR = r; g_ListBoxS2ElementBackgroundG = g; g_ListBoxS2ElementBackgroundB = b; }
				else if (name == "ListBoxSpecialElement3")				{ g_ListBoxS3ElementBackgroundR = r; g_ListBoxS3ElementBackgroundG = g; g_ListBoxS3ElementBackgroundB = b; }
				else if (name == "ListBoxSpecialElement4")				{ g_ListBoxS4ElementBackgroundR = r; g_ListBoxS4ElementBackgroundG = g; g_ListBoxS4ElementBackgroundB = b; }
				else if (name == "ListBoxSelectedElement")				{ g_ListBoxSelectedElementR = r; g_ListBoxSelectedElementG = g; g_ListBoxSelectedElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement1")		{ g_ListBoxSelectedS1ElementR = r; g_ListBoxSelectedS1ElementG = g; g_ListBoxSelectedS1ElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement2")		{ g_ListBoxSelectedS2ElementR = r; g_ListBoxSelectedS2ElementG = g; g_ListBoxSelectedS2ElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement3")		{ g_ListBoxSelectedS3ElementR = r; g_ListBoxSelectedS3ElementG = g; g_ListBoxSelectedS3ElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement4")		{ g_ListBoxSelectedS4ElementR = r; g_ListBoxSelectedS4ElementG = g; g_ListBoxSelectedS4ElementB = b; }
				else if (name == "ListBoxElementBorderTopLeft")			{ g_ListBoxElementBorderR = r; g_ListBoxElementBorderG = g; g_ListBoxElementBorderB = b; }
				else if (name == "ListBoxElementBorderBottomRight")		{ g_ListBoxElementBorderHR = r; g_ListBoxElementBorderHG = g; g_ListBoxElementBorderHB = b; }
				else if (name == "ListBoxFont")							{ g_ListBoxTextR = r; g_ListBoxTextG = g; g_ListBoxTextB = b; }
				else if (name == "ListBoxColumnHeaderBackground")		{ g_ListBoxHeaderBackgroundR = r; g_ListBoxHeaderBackgroundG = g; g_ListBoxHeaderBackgroundB = b; }
				else if (name == "ListBoxColumnHeaderBorderTopLeft")	{ g_ListBoxHeaderBorderR = r; g_ListBoxHeaderBorderG = g; g_ListBoxHeaderBorderB = b; }
				else if (name == "ListBoxColumnHeaderBorderBottomRight"){ g_ListBoxHeaderBorderHR = r; g_ListBoxHeaderBorderHG = g; g_ListBoxHeaderBorderHB = b; }
				else if (name == "ListBoxColumnHeaderFont")				{ g_ListBoxHeaderTextR = r; g_ListBoxHeaderTextG = g; g_ListBoxHeaderTextB = b; }
				else if (name == "MessageBoxBorder")					{ g_MessageBoxBorderR = r; g_MessageBoxBorderG = g; g_MessageBoxBorderB = b; }
				else if (name == "MessageBoxBackground0")				{ g_MessageBoxBackground0R = r; g_MessageBoxBackground0G = g; g_MessageBoxBackground0B = b; }
				else if (name == "MessageBoxBackground1")				{ g_MessageBoxBackground1R = r; g_MessageBoxBackground1G = g; g_MessageBoxBackground1B = b; }
				else if (name == "MessageBoxBackground2")				{ g_MessageBoxBackground2R = r; g_MessageBoxBackground2G = g; g_MessageBoxBackground2B = b; }
				else if (name == "MessageBoxBackground3")				{ g_MessageBoxBackground3R = r; g_MessageBoxBackground3G = g; g_MessageBoxBackground3B = b; }
				else if (name == "MessageBoxBackground4")				{ g_MessageBoxBackground4R = r; g_MessageBoxBackground4G = g; g_MessageBoxBackground4B = b; }
				else if (name == "MessageBoxText")						{ g_MessageBoxTextR = r; g_MessageBoxTextG = g; g_MessageBoxTextB = b; }
				else if (name == "CheckboxBorder")						{ g_CheckBoxBorderR = r; g_CheckBoxBorderG = g; g_CheckBoxBorderB = b; }
				else if (name == "CheckboxBackground")					{ g_CheckBoxBackgroundR = r; g_CheckBoxBackgroundG = g; g_CheckBoxBackgroundB = b; }
				// ItemRarity is loaded in sConfig.cpp
			}
		}
	}
	else
	{
		g_LogFile.write("Loading Default InterfaceColors");
		g_StaticImageR = 0;						g_StaticImageG = 0;						g_StaticImageB = 0;
		g_ChoiceMessageTextR = 0;				g_ChoiceMessageTextG = 0;				g_ChoiceMessageTextB = 0;
		g_ChoiceMessageBorderR = 0;				g_ChoiceMessageBorderG = 0;				g_ChoiceMessageBorderB = 0;
		g_ChoiceMessageHeaderR = 229;			g_ChoiceMessageHeaderG = 227;			g_ChoiceMessageHeaderB = 52;
		g_ChoiceMessageBackgroundR = 88;		g_ChoiceMessageBackgroundG = 163;		g_ChoiceMessageBackgroundB = 113;
		g_ChoiceMessageSelectedR = 229;			g_ChoiceMessageSelectedG = 227;			g_ChoiceMessageSelectedB = 52;
		g_EditBoxBorderR = 0;					g_EditBoxBorderG = 0;					g_EditBoxBorderB = 0;
		g_EditBoxBackgroundR = 90;				g_EditBoxBackgroundG = 172;				g_EditBoxBackgroundB = 161;
		g_EditBoxSelectedR = 114;				g_EditBoxSelectedG = 211;				g_EditBoxSelectedB = 198;
		g_EditBoxTextR = 0;						g_EditBoxTextG = 0;						g_EditBoxTextB = 0;
		g_WindowBorderR = 0;					g_WindowBorderG = 0;					g_WindowBorderB = 0;
		g_WindowBackgroundR = 140;				g_WindowBackgroundG = 191;				g_WindowBackgroundB = 228;
		g_ListBoxBorderR = 0;					g_ListBoxBorderG = 0;					g_ListBoxBorderB = 0;
		g_ListBoxBackgroundR = 217;				g_ListBoxBackgroundG = 214;				g_ListBoxBackgroundB = 139;
		g_ListBoxElementBackgroundR = 114;		g_ListBoxElementBackgroundG = 139;		g_ListBoxElementBackgroundB = 217;
		g_ListBoxS1ElementBackgroundR = 200;	g_ListBoxS1ElementBackgroundG = 30;		g_ListBoxS1ElementBackgroundB = 30;
		g_ListBoxS2ElementBackgroundR = 80;		g_ListBoxS2ElementBackgroundG = 80;		g_ListBoxS2ElementBackgroundB = 250;
		g_ListBoxS3ElementBackgroundR = 30;		g_ListBoxS3ElementBackgroundG = 190;	g_ListBoxS3ElementBackgroundB = 30;
		g_ListBoxS4ElementBackgroundR = 190;	g_ListBoxS4ElementBackgroundG = 190;	g_ListBoxS4ElementBackgroundB = 0;
		g_ListBoxSelectedElementR = 187;		g_ListBoxSelectedElementG = 190;		g_ListBoxSelectedElementB = 224;
		g_ListBoxSelectedS1ElementR = 255;		g_ListBoxSelectedS1ElementG = 167;		g_ListBoxSelectedS1ElementB = 180;
		g_ListBoxSelectedS2ElementR = 187;		g_ListBoxSelectedS2ElementG = 190;		g_ListBoxSelectedS2ElementB = 224;
		g_ListBoxSelectedS3ElementR = 0;		g_ListBoxSelectedS3ElementG = 250;		g_ListBoxSelectedS3ElementB = 0;
		g_ListBoxSelectedS4ElementR = 250;		g_ListBoxSelectedS4ElementG = 250;		g_ListBoxSelectedS4ElementB = 250;
		g_ListBoxElementBorderR = 79;			g_ListBoxElementBorderG = 79;			g_ListBoxElementBorderB = 111;
		g_ListBoxElementBorderHR = 159;			g_ListBoxElementBorderHG = 175;			g_ListBoxElementBorderHB = 255;
		g_ListBoxTextR = 0;						g_ListBoxTextG = 0;						g_ListBoxTextB = 0;
		g_ListBoxHeaderBackgroundR = 25;		g_ListBoxHeaderBackgroundG = 100;		g_ListBoxHeaderBackgroundB = 144;
		g_ListBoxHeaderBorderR = 120;			g_ListBoxHeaderBorderG = 155;			g_ListBoxHeaderBorderB = 176;
		g_ListBoxHeaderBorderHR = 15;			g_ListBoxHeaderBorderHG = 49;			g_ListBoxHeaderBorderHB = 69;
		g_ListBoxHeaderTextR = 255;				g_ListBoxHeaderTextG = 255;				g_ListBoxHeaderTextB = 255;
		g_MessageBoxBorderR = 255;				g_MessageBoxBorderG = 255;				g_MessageBoxBorderB = 255;
		g_MessageBoxBackground0R = 100;			g_MessageBoxBackground0G = 100;			g_MessageBoxBackground0B = 150;
		g_MessageBoxBackground1R = 200;			g_MessageBoxBackground1G = 100;			g_MessageBoxBackground1B = 150;
		g_MessageBoxBackground2R = 100;			g_MessageBoxBackground2G = 200;			g_MessageBoxBackground2B = 150;
		g_MessageBoxBackground3R = 100;			g_MessageBoxBackground3G = 100;			g_MessageBoxBackground3B = 200;
		g_MessageBoxBackground4R = 190;			g_MessageBoxBackground4G = 190;			g_MessageBoxBackground4B = 0;
		g_MessageBoxTextR = 0;					g_MessageBoxTextG = 0;					g_MessageBoxTextB = 0;
		g_CheckBoxBorderR = 0;					g_CheckBoxBorderG = 0;					g_CheckBoxBorderB = 0;
		g_CheckBoxBackgroundR = 180;			g_CheckBoxBackgroundG = 180;			g_CheckBoxBackgroundB = 180;
	}

    // `J` Bookmark - Loading the screens
    g_WinManager.load();
    g_Preparing = load_window<cScreenPreparingGame>("Preparing Game");
    g_MainMenu = load_window<cScreenMainMenu>("Main Menu");
    g_NewGame = load_window<cScreenNewGame>("New Game");
    g_LoadGame = load_window<cScreenLoadGame>("Load Game");
    load_window<cScreenSettings>("Settings");

    g_BrothelManagement = load_window<cScreenBrothelManagement>("Brothel Management");
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
T* load_window(const char* name)
{
    g_LogFile.write(std::string("Loading Window '") + name + "'");
    auto window = std::make_unique<T>();
    window->load();
    auto result = window.get();
    g_WinManager.add_window(name, std::move(window));
    return result;
}
