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

#include <iostream>
#include "InterfaceGlobals.h"
#include "DirPath.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cFarm.h"
#include "Constants.h"
#include "cTariff.h"
#include "cWindowManager.h"
#include "cScreenBuildingManagement.h"
#include "cScreenPropertyManagement.h"	// `J` added
#include "cScreenPrison.h"
#include "cScreenTown.h"
#include "cAuctionScreen.h"
#include "cArenaScreen.h"
#include "cArenaTry.h"
#include "cHouseScreen.h"
#include "cClinicScreen.h"
#include "cCentreScreen.h"
#include "cMovieScreen.h"
#include "cFarmScreen.h"
#include "cClinicTry.h"
#include "cCastingTry.h"
#include "cScreenSlaveMarket.h"
#include "cScreenMayor.h"
#include "cScreenBank.h"
#include "cScreenHouse.h"
#include "cScreenItemManagement.h"
#include "cScreenBuildingSetup.h"
#include "cScreenGangs.h"
#include "cScreenGirlManagement.h"
#include "cScreenClinicManagement.h"
#include "cScreenHouseManagement.h"
#include "cScreenFarmManagement.h"
#include "cScreenStudioManagement.h"
#include "cScreenMovieMaker.h"
#include "cScreenArenaManagement.h"
#include "cScreenCentreManagement.h"
#include "cScreenGirlDetails.h"
#include "cScreenDungeon.h"
#include "cScreenMainMenu.h"
#include "cScreenNewGame.h"
#include "cScreenBrothelManagement.h"
#include "cScreenGetInput.h"
#include "cScreenGallery.h"
#include "sConfig.h"
#include "XmlUtil.h"


using namespace std;

extern sInterfaceIDs g_interfaceid;
extern cWindowManager g_WinManager;

cInterfaceEventManager g_InterfaceEvents;
cScreenMainMenu g_MainMenu;
cScreenNewGame g_NewGame;			// NewGame.xml

cScreenBrothelManagement g_BrothelManagement;
cScreenGirlManagement g_GirlManagement;
cScreenClinicManagement g_ClinicManagement;
cScreenStudioManagement g_StudioManagement;
cScreenArenaManagement g_ArenaManagement;
cScreenCentreManagement g_CentreManagement;
cScreenHouseManagement g_HouseManagement;
cScreenFarmManagement g_FarmManagement;
cScreenMovieMaker g_MovieMaker;
cScreenGangs g_GangManagement;
cScreenGirlDetails g_GirlDetails;
cScreenDungeon g_Dungeon;
cScreenSlaveMarket g_SlaveMarket;
cScreenTown g_TownScreen;
cScreenGallery g_GalleryScreen;
cArenaTry g_ArenaTry;
cClinicScreen g_ClinicScreen;
cCentreScreen g_CentreScreen;
cHouseScreen g_HouseScreen;
cFarmScreen g_FarmScreen;
cMovieScreen g_MovieScreen;
cClinicTry g_ClinicTry;
cCastingTry g_CastingTry;
cArenaScreen g_ArenaScreen;
cAuctionScreen g_AuctionScreen;
cScreenBuildingSetup g_BuildingSetupScreen;
cScreenMayor g_MayorsOfficeScreen;
cScreenBank g_BankScreen;
cScreenHouse g_PlayersHouse;
cScreenItemManagement g_ItemManagement;
cScreenPrison g_PrisonScreen;
cBuildingManagement g_BuildingManagementScreen;
cScreenPropertyManagement g_PropertyManagementScreen;	// `J` added

cScreenGetInput g_GetInput;

cInterfaceWindow g_GetString;		// GetString.txt
cInterfaceWindow g_LoadGame;		// LoadMenu.txt
cInterfaceWindow g_TransferGirls;	// Hard coded
cInterfaceWindow g_Gallery;			// Hard coded
cInterfaceWindow g_Gallery2;		// Hard coded
cInterfaceWindow g_ChangeJobs;		// Hard coded
cInterfaceWindow g_Turnsummary;		// Hard coded

cMessageBox g_MessageBox;
cChoiceManager g_ChoiceManager;
cMessageQue g_MessageQue;

// interface colors
unsigned char g_StaticImageR = 0, g_StaticImageG = 0, g_StaticImageB = 0;

unsigned char g_ChoiceMessageTextR = 0, g_ChoiceMessageTextG = 0, g_ChoiceMessageTextB = 0;
unsigned char g_ChoiceMessageBorderR = 0, g_ChoiceMessageBorderG = 0, g_ChoiceMessageBorderB = 0;
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
unsigned char g_ListBoxSelectedElementR = 0, g_ListBoxSelectedElementG = 0, g_ListBoxSelectedElementB = 0;
unsigned char g_ListBoxSelectedS1ElementR = 0, g_ListBoxSelectedS1ElementG = 0, g_ListBoxSelectedS1ElementB = 0;
unsigned char g_ListBoxSelectedS2ElementR = 0, g_ListBoxSelectedS2ElementG = 0, g_ListBoxSelectedS2ElementB = 0;
unsigned char g_ListBoxSelectedS3ElementR = 0, g_ListBoxSelectedS3ElementG = 0, g_ListBoxSelectedS3ElementB = 0;
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
unsigned char g_MessageBoxTextR = 0, g_MessageBoxTextG = 0, g_MessageBoxTextB = 0;

void FreeInterface()
{
	g_MainMenu.Free();
	g_GetString.Free();
	g_BrothelManagement.Free();
	g_ClinicManagement.Free();
	g_StudioManagement.Free();
	g_ArenaManagement.Free();
	g_CentreManagement.Free();
	g_HouseManagement.Free();
	g_FarmManagement.Free();
	g_GirlManagement.Free();
	g_GangManagement.Free();
	g_GirlDetails.Free();
	g_ChangeJobs.Free();
	g_Turnsummary.Free();
	g_Dungeon.Free();
	g_SlaveMarket.Free();
	g_TownScreen.Free();
	g_GalleryScreen.Free();
	g_ArenaTry.Free();
	g_CentreScreen.Free();
	g_ClinicScreen.Free();
	g_ClinicTry.Free();
	g_CastingTry.Free();
	g_ArenaScreen.Free();
	g_AuctionScreen.Free();
	g_HouseScreen.Free();
	g_FarmScreen.Free();
	g_MovieScreen.Free();
	g_Gallery.Free();
	g_Gallery2.Free();
	g_BuildingSetupScreen.Free();
	g_GetInput.Free();
	g_MayorsOfficeScreen.Free();
	g_BankScreen.Free();
	g_ChoiceManager.Free();
	g_MessageQue.Free();
	g_LoadGame.Free();
	g_PlayersHouse.Free();
	g_TransferGirls.Free();
	g_ItemManagement.Free();
	g_PrisonScreen.Free();
	g_MovieMaker.Free();
	g_BuildingManagementScreen.Free();
}

void ResetInterface()
{
	g_MainMenu.Reset();
	g_GetString.Reset();
	g_BrothelManagement.Reset();
	g_ClinicManagement.Reset();
	g_StudioManagement.Reset();
	g_ArenaManagement.Reset();
	g_CentreManagement.Reset();
	g_HouseManagement.Reset();
	g_FarmManagement.Reset();
	g_GirlManagement.Reset();
	g_GangManagement.Reset();
	g_GirlDetails.Reset();
	g_ChangeJobs.Reset();
	g_Turnsummary.Reset();
	g_Dungeon.Reset();
	g_SlaveMarket.Reset();
	g_TownScreen.Reset();
	g_GalleryScreen.Reset();
	g_ArenaTry.Reset();
	g_CentreScreen.Reset();
	g_ClinicScreen.Reset();
	g_ClinicTry.Reset();
	g_CastingTry.Reset();
	g_ArenaScreen.Reset();
	g_AuctionScreen.Reset();
	g_HouseScreen.Reset();
	g_FarmScreen.Reset();
	g_MovieScreen.Reset();
	g_Gallery.Reset();
	g_Gallery2.Reset();
	g_BuildingSetupScreen.Reset();
	g_GetInput.Reset();
	g_MayorsOfficeScreen.Reset();
	g_BankScreen.Reset();
	g_ChoiceManager.Free();
	g_LoadGame.Reset();
	g_PlayersHouse.Reset();
	g_TransferGirls.Reset();
	g_ItemManagement.Reset();
	g_PrisonScreen.Reset();
	g_MovieMaker.Reset();
	g_BuildingManagementScreen.Reset();
}

void LoadInterface()
{
	cTariff tariff;
	stringstream ss;
	int r = 0, g = 0, b = 0, a = 0, c = 0, d = 0, e = 0, fontsize = 16,
		increment = 0, min = 0, max = 0, value = 0;
	string image = ""; string text = ""; string file = "";
	bool Transparency = false, Scale = true, multi = false, events = false, liveUpdate = false;
	ifstream incol;
	cConfig cfg;

	g_LogFile.write("Begin Loading Interface");

	// load interface colors
	int loadcolors = 0;		// 0=default, 1=xml, 2=txt
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "InterfaceColors.xml";
	TiXmlDocument docInterfaceColors(dp.c_str());
	if (docInterfaceColors.LoadFile())	loadcolors = 1;
	else // try txt
	{
		if (cfg.debug.log_debug())
		{
			g_LogFile.ss() << "Error: line " << docInterfaceColors.ErrorRow() << ", col " << docInterfaceColors.ErrorCol() << ": " << docInterfaceColors.ErrorDesc() << endl;
			g_LogFile.ssend();
		}
		DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "InterfaceColors.txt";
		incol.open(dp.c_str());
		loadcolors = (incol.good()) ? 2 : 0;
		incol.close();
	}
	if (loadcolors == 1)	// load "InterfaceColors.xml"
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
				else if (name == "ListBoxSelectedElement")				{ g_ListBoxSelectedElementR = r; g_ListBoxSelectedElementG = g; g_ListBoxSelectedElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement1")		{ g_ListBoxSelectedS1ElementR = r; g_ListBoxSelectedS1ElementG = g; g_ListBoxSelectedS1ElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement2")		{ g_ListBoxSelectedS2ElementR = r; g_ListBoxSelectedS2ElementG = g; g_ListBoxSelectedS2ElementB = b; }
				else if (name == "ListBoxSelectedSpecialElement3")		{ g_ListBoxSelectedS3ElementR = r; g_ListBoxSelectedS3ElementG = g; g_ListBoxSelectedS3ElementB = b; }
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
				else if (name == "MessageBoxText")						{ g_MessageBoxTextR = r; g_MessageBoxTextG = g; g_MessageBoxTextB = b; }
				else if (name == "CheckboxBorder")						{ g_CheckBoxBorderR = r; g_CheckBoxBorderG = g; g_CheckBoxBorderB = b; }
				else if (name == "CheckboxBackground")					{ g_CheckBoxBackgroundR = r; g_CheckBoxBackgroundG = g; g_CheckBoxBackgroundB = b; }
				// ItemRarity is loaded in sConfig.cpp
			}
		}
	}
	else if (loadcolors==2)
	{
		g_LogFile.write("Loading InterfaceColors.txt");
		incol.open(dp.c_str());
		incol.seekg(0);
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_StaticImageR = r;                g_StaticImageG = g;                g_StaticImageB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ChoiceMessageTextR = r;          g_ChoiceMessageTextG = g;          g_ChoiceMessageTextB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ChoiceMessageBorderR = r;        g_ChoiceMessageBorderG = g;        g_ChoiceMessageBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ChoiceMessageBackgroundR = r;    g_ChoiceMessageBackgroundG = g;    g_ChoiceMessageBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ChoiceMessageSelectedR = r;      g_ChoiceMessageSelectedG = g;      g_ChoiceMessageSelectedB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_EditBoxBorderR = r;              g_EditBoxBorderG = g;              g_EditBoxBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_EditBoxBackgroundR = r;          g_EditBoxBackgroundG = g;          g_EditBoxBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_EditBoxSelectedR = r;            g_EditBoxSelectedG = g;            g_EditBoxSelectedB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_EditBoxTextR = r;                g_EditBoxTextG = g;                g_EditBoxTextB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_WindowBorderR = r;               g_WindowBorderG = g;               g_WindowBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_WindowBackgroundR = r;           g_WindowBackgroundG = g;           g_WindowBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxBorderR = r;              g_ListBoxBorderG = g;              g_ListBoxBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxBackgroundR = r;          g_ListBoxBackgroundG = g;          g_ListBoxBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxElementBackgroundR = r;   g_ListBoxElementBackgroundG = g;   g_ListBoxElementBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxS1ElementBackgroundR = r; g_ListBoxS1ElementBackgroundG = g; g_ListBoxS1ElementBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxS2ElementBackgroundR = r; g_ListBoxS2ElementBackgroundG = g; g_ListBoxS2ElementBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxS3ElementBackgroundR = r; g_ListBoxS3ElementBackgroundG = g; g_ListBoxS3ElementBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxSelectedElementR = r;     g_ListBoxSelectedElementG = g;     g_ListBoxSelectedElementB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxSelectedS1ElementR = r;   g_ListBoxSelectedS1ElementG = g;   g_ListBoxSelectedS1ElementB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxSelectedS2ElementR = r;   g_ListBoxSelectedS2ElementG = g;   g_ListBoxSelectedS2ElementB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxSelectedS3ElementR = r;   g_ListBoxSelectedS3ElementG = g;   g_ListBoxSelectedS3ElementB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxElementBorderR = r;       g_ListBoxElementBorderG = g;       g_ListBoxElementBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxElementBorderHR = r;      g_ListBoxElementBorderHG = g;      g_ListBoxElementBorderHB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxTextR = r;                g_ListBoxTextG = g;                g_ListBoxTextB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxHeaderBackgroundR = r;    g_ListBoxHeaderBackgroundG = g;    g_ListBoxHeaderBackgroundB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxHeaderBorderR = r;        g_ListBoxHeaderBorderG = g;        g_ListBoxHeaderBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxHeaderBorderHR = r;       g_ListBoxHeaderBorderHG = g;       g_ListBoxHeaderBorderHB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_ListBoxHeaderTextR = r;          g_ListBoxHeaderTextG = g;          g_ListBoxHeaderTextB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxBorderR = r;           g_MessageBoxBorderG = g;           g_MessageBoxBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxBackground0R = r;      g_MessageBoxBackground0G = g;      g_MessageBoxBackground0B = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxBackground1R = r;      g_MessageBoxBackground1G = g;      g_MessageBoxBackground1B = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxBackground2R = r;      g_MessageBoxBackground2G = g;      g_MessageBoxBackground2B = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxBackground3R = r;      g_MessageBoxBackground3G = g;      g_MessageBoxBackground3B = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_MessageBoxTextR = r;             g_MessageBoxTextG = g;             g_MessageBoxTextB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_CheckBoxBorderR = r;             g_CheckBoxBorderG = g;             g_CheckBoxBorderB = b;
		incol >> r >> g >> b; incol.ignore(1000, '\n'); g_CheckBoxBackgroundR = r;         g_CheckBoxBackgroundG = g;         g_CheckBoxBackgroundB = b;
		incol.close();
	}
	else
	{
		g_LogFile.write("Loading Default InterfaceColors");
		g_StaticImageR = 0;                   g_StaticImageG = 0;                   g_StaticImageB = 0;
		g_ChoiceMessageTextR = 0;             g_ChoiceMessageTextG = 0;             g_ChoiceMessageTextB = 0;
		g_ChoiceMessageBorderR = 0;           g_ChoiceMessageBorderG = 0;           g_ChoiceMessageBorderB = 0;
		g_ChoiceMessageBackgroundR = 88;      g_ChoiceMessageBackgroundG = 163;     g_ChoiceMessageBackgroundB = 113;
		g_ChoiceMessageSelectedR = 229;       g_ChoiceMessageSelectedG = 227;       g_ChoiceMessageSelectedB = 52;
		g_EditBoxBorderR = 0;                 g_EditBoxBorderG = 0;                 g_EditBoxBorderB = 0;
		g_EditBoxBackgroundR = 90;            g_EditBoxBackgroundG = 172;           g_EditBoxBackgroundB = 161;
		g_EditBoxSelectedR = 114;             g_EditBoxSelectedG = 211;             g_EditBoxSelectedB = 198;
		g_EditBoxTextR = 0;                   g_EditBoxTextG = 0;                   g_EditBoxTextB = 0;
		g_WindowBorderR = 0;                  g_WindowBorderG = 0;                  g_WindowBorderB = 0;
		g_WindowBackgroundR = 140;            g_WindowBackgroundG = 191;            g_WindowBackgroundB = 228;
		g_ListBoxBorderR = 0;                 g_ListBoxBorderG = 0;                 g_ListBoxBorderB = 0;
		g_ListBoxBackgroundR = 217;           g_ListBoxBackgroundG = 214;           g_ListBoxBackgroundB = 139;
		g_ListBoxElementBackgroundR = 114;    g_ListBoxElementBackgroundG = 139;    g_ListBoxElementBackgroundB = 217;
		g_ListBoxS1ElementBackgroundR = 200;  g_ListBoxS1ElementBackgroundG = 30;   g_ListBoxS1ElementBackgroundB = 30;
		g_ListBoxS2ElementBackgroundR = 80;   g_ListBoxS2ElementBackgroundG = 80;   g_ListBoxS2ElementBackgroundB = 250;
		g_ListBoxS3ElementBackgroundR = 30;   g_ListBoxS3ElementBackgroundG = 190;  g_ListBoxS3ElementBackgroundB = 30;
		g_ListBoxSelectedElementR = 187;      g_ListBoxSelectedElementG = 190;      g_ListBoxSelectedElementB = 224;
		g_ListBoxSelectedS1ElementR = 255;    g_ListBoxSelectedS1ElementG = 167;    g_ListBoxSelectedS1ElementB = 180;
		g_ListBoxSelectedS2ElementR = 187;    g_ListBoxSelectedS2ElementG = 190;    g_ListBoxSelectedS2ElementB = 224;
		g_ListBoxSelectedS3ElementR = 0;      g_ListBoxSelectedS3ElementG = 250;    g_ListBoxSelectedS3ElementB = 0;
		g_ListBoxElementBorderR = 79;         g_ListBoxElementBorderG = 79;         g_ListBoxElementBorderB = 111;
		g_ListBoxElementBorderHR = 159;       g_ListBoxElementBorderHG = 175;       g_ListBoxElementBorderHB = 255;
		g_ListBoxTextR = 0;                   g_ListBoxTextG = 0;                   g_ListBoxTextB = 0;
		g_ListBoxHeaderBackgroundR = 25;      g_ListBoxHeaderBackgroundG = 100;     g_ListBoxHeaderBackgroundB = 144;
		g_ListBoxHeaderBorderR = 120;         g_ListBoxHeaderBorderG = 155;         g_ListBoxHeaderBorderB = 176;
		g_ListBoxHeaderBorderHR = 15;         g_ListBoxHeaderBorderHG = 49;         g_ListBoxHeaderBorderHB = 69;
		g_ListBoxHeaderTextR = 255;           g_ListBoxHeaderTextG = 255;           g_ListBoxHeaderTextB = 255;
		g_MessageBoxBorderR = 255;            g_MessageBoxBorderG = 255;            g_MessageBoxBorderB = 255;
		g_MessageBoxBackground0R = 100;       g_MessageBoxBackground0G = 100;       g_MessageBoxBackground0B = 150;
		g_MessageBoxBackground1R = 200;       g_MessageBoxBackground1G = 100;       g_MessageBoxBackground1B = 150;
		g_MessageBoxBackground2R = 100;       g_MessageBoxBackground2G = 200;       g_MessageBoxBackground2B = 150;
		g_MessageBoxBackground3R = 100;       g_MessageBoxBackground3G = 100;       g_MessageBoxBackground3B = 200;
		g_MessageBoxTextR = 0;                g_MessageBoxTextG = 0;                g_MessageBoxTextB = 0;
		g_CheckBoxBorderR = 0;                g_CheckBoxBorderG = 0;                g_CheckBoxBorderB = 0;
		g_CheckBoxBackgroundR = 180;          g_CheckBoxBackgroundG = 180;          g_CheckBoxBackgroundB = 180;
	}


	// Load game screen
	int loadmenu = 0;		// 0=default, 1=xml, 2=txt
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.xml";
	TiXmlDocument docLoadMenu(dp.c_str());
	if (docLoadMenu.LoadFile())	loadmenu = 1;
	else // try txt
	{
		if (cfg.debug.log_debug())
		{
			g_LogFile.ss() << "Error: line " << docLoadMenu.ErrorRow() << ", col " << docLoadMenu.ErrorCol() << ": " << docLoadMenu.ErrorDesc() << endl;
			g_LogFile.ssend();
		}
		dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.txt";
		incol.open(dp.c_str());
		loadmenu = (incol.good()) ? 2 : loadmenu = 0;
		incol.close();
	}
	if (loadmenu == 1)
	{
		g_LogFile.write("Loading LoadMenu.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docLoadMenu.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename); string name;
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", a); xu.get_att(el, "YPos", b); xu.get_att(el, "Width", c); xu.get_att(el, "Height", d); xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true); xu.get_att(el, "Transparency", Transparency, true); xu.get_att(el, "Scale", Scale, true);

			if (name == "LoadMenu")	g_LoadGame.CreateWindow(a, b, c, d, e);
			if (name == "FileName")	g_LoadGame.AddListBox(g_interfaceid.LIST_LOADGSAVES, a, b, c, d, e, true);
			if (name == "LoadGame")	g_LoadGame.AddButton(image, g_interfaceid.BUTTON_LOADGLOAD, a, b, c, d, Transparency, Scale);
			if (name == "Back")		g_LoadGame.AddButton(image, g_interfaceid.BUTTON_LOADGBACK, a, b, c, d, Transparency, Scale);
		}
	}
	else if (loadmenu == 2)
	{
		g_LogFile.write("Loading LoadMenu.txt");
		dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.txt";
		incol.open(dp.c_str());
		incol.seekg(0);
		incol >> a >> b >> c >> d >> e; incol.ignore(1000, '\n');
		g_LoadGame.CreateWindow(a, b, c, d, e);
		g_LoadGame.AddTextItem(g_interfaceid.STATIC_STATIC, 0, d - 10, c, 12, "Please read the readme.html", 10);
		incol >> a >> b >> c >> d >> e; incol.ignore(1000, '\n');
		g_LoadGame.AddListBox(g_interfaceid.LIST_LOADGSAVES, a, b, c, d, e, true);
		incol >> a >> b >> c >> d; incol.ignore(1000, '\n');
		g_LoadGame.AddButton("Load", g_interfaceid.BUTTON_LOADGLOAD, a, b, c, d, true);
		incol >> a >> b >> c >> d; incol.ignore(1000, '\n');
		g_LoadGame.AddButton("Back", g_interfaceid.BUTTON_LOADGBACK, a, b, c, d, true);
		incol.close();
	}
	else
	{
		g_LogFile.write("Loading Default LoadMenu");
		g_LoadGame.CreateWindow(224, 128, 344, 344, 1);
		g_LoadGame.AddTextItem(g_interfaceid.STATIC_STATIC, 0, 334 - 10, 344, 12, "Please read the readme.html", 10);
		g_LoadGame.AddListBox(g_interfaceid.LIST_LOADGSAVES, 8, 8, 328, 288, 1, true);
		g_LoadGame.AddButton("Load", g_interfaceid.BUTTON_LOADGLOAD, 8, 304, 160, 32, true);
		g_LoadGame.AddButton("Back", g_interfaceid.BUTTON_LOADGBACK, 176, 304, 160, 32, true);
	}


	// Load GetString screen
	int GetString = 0;		// 0=default, 1=xml, 2=txt
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "GetString.xml";
	TiXmlDocument docGetString(dp.c_str());
	if (docGetString.LoadFile())	GetString = 1;
	else // try txt
	{
		if (cfg.debug.log_debug())
		{
			g_LogFile.ss() << "Error: line " << docGetString.ErrorRow() << ", col " << docGetString.ErrorCol() << ": " << docGetString.ErrorDesc() << endl;
			g_LogFile.ssend();
		}
		dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "GetString.txt";
		incol.open(dp.c_str());
		GetString = (incol.good()) ? 2 : GetString = 0;
		incol.close();
	}
	if (GetString == 1)
	{
		g_LogFile.write("Loading GetString.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docGetString.RootElement();

		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename);	string name;
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", a); xu.get_att(el, "YPos", b); xu.get_att(el, "Width", c); xu.get_att(el, "Height", d); xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true); xu.get_att(el, "Transparency", Transparency, true); 
			xu.get_att(el, "Scale", Scale, true); xu.get_att(el, "Text", text,true);

			if (name == "GetString")	g_GetString.CreateWindow(a, b, c, d, e);
			if (name == "Ok")			g_GetString.AddButton(image, g_interfaceid.BUTTON_OK, a, b, c, d, Transparency, Scale);
			if (name == "Cancel")		g_GetString.AddButton(image, g_interfaceid.BUTTON_CANCEL, a, b, c, d, Transparency, Scale);
			if (name == "Label")		g_GetString.AddTextItem(g_interfaceid.TEXT_TEXT1, a, b, c, d, text, fontsize);
			if (name == "TextField")	g_GetString.AddEditBox(g_interfaceid.EDITBOX_NAME, a, b, c, d, e);
		}
	}
	else if (GetString == 2)
	{
		g_LogFile.write("Loading GetString.txt");
		dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "GetString.txt";
		incol.open(dp.c_str());
		incol.seekg(0);
		incol >> a >> b >> c >> d >> e; incol.ignore(1000, '\n');
		g_GetString.CreateWindow(a, b, c, d, e);
		incol >> a >> b >> c >> d; incol.ignore(1000, '\n');
		g_GetString.AddButton("Ok", g_interfaceid.BUTTON_OK, a, b, c, d, true);
		incol >> a >> b >> c >> d; incol.ignore(1000, '\n');
		g_GetString.AddButton("Cancel", g_interfaceid.BUTTON_CANCEL, a, b, c, d, true);
		incol >> a >> b >> c >> d >> e; incol.ignore(1000, '\n');
		g_GetString.AddTextItem(g_interfaceid.TEXT_TEXT1, a, b, c, d, "Enter Text:", e);
		incol >> a >> b >> c >> d >> e; incol.ignore(1000, '\n');
		g_GetString.AddEditBox(g_interfaceid.EDITBOX_NAME, a, b, c, d, e);
		incol.close();
	}
	else
	{
		g_LogFile.write("Loading Default GetString");
		g_GetString.CreateWindow(224, 127, 352, 160, 1);
		g_GetString.AddButton("Ok", g_interfaceid.BUTTON_OK, 32, 104, 128, 32, true);
		g_GetString.AddButton("Cancel", g_interfaceid.BUTTON_CANCEL, 192, 104, 128, 32, true);
		g_GetString.AddTextItem(g_interfaceid.TEXT_TEXT1, 32, 32, 128, 32, "Enter Text:", 16);
		g_GetString.AddEditBox(g_interfaceid.EDITBOX_NAME, 160, 32, 160, 32, 1);
	}


	// `J` This is never used so not updating it now
	// Change Jobs Screen
	g_LogFile.write("Loading Change Jobs Screen");
	g_ChangeJobs.CreateWindow(256, 120, 304, 376, 1);
	g_ChangeJobs.AddButton("Ok", g_interfaceid.BUTTON_CJOK, 8, 336, 128, 32, true);
	g_ChangeJobs.AddButton("Cancel", g_interfaceid.BUTTON_CJCANCEL, 152, 336, 128, 32, true);
	g_ChangeJobs.AddTextItem(g_interfaceid.TEXT_CJDESC, 8, 240, 272, 88, "", 10);
	g_ChangeJobs.AddListBox(g_interfaceid.LIST_CJDAYTIME, 8, 48, 144, 192, 1, true);
	g_ChangeJobs.AddListBox(g_interfaceid.LIST_CJNIGHTTIME, 152, 48, 144, 192, 1, true);
	g_ChangeJobs.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 8, 128, 32, "Day Shift");
	g_ChangeJobs.AddTextItem(g_interfaceid.STATIC_STATIC, 152, 8, 128, 32, "Night Shift");


	// Load TurnSummary screen
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "TurnSummary.xml";
	TiXmlDocument docTurnSummary(dp.c_str());
	if (docTurnSummary.LoadFile())
	{
		g_LogFile.write("Loading TurnSummary.xml");
		string m_filename = dp.c_str();
		string file = "blank.png";
		TiXmlElement *el, *root_el = docTurnSummary.RootElement();
		
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{

			XmlUtil xu(m_filename);	string name = "";
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", a); xu.get_att(el, "YPos", b); xu.get_att(el, "Width", c); xu.get_att(el, "Height", d); xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true);
			
			xu.get_att(el, "File", file, true);
			xu.get_att(el, "Transparency", Transparency, true); xu.get_att(el, "Scale", Scale, true);
			xu.get_att(el, "Text", text, true); xu.get_att(el, "FontSize", fontsize, true);
			DirPath dp = ImagePath(file);

			if (name == "Turn Summary")		g_Turnsummary.CreateWindow(a, b, c, d, e);
			if (name == "CurrentBrothel")	g_Turnsummary.AddTextItem(g_interfaceid.TEXT_CURRENTBROTHEL, a, b, c, d, text, fontsize);
			if (name == "LabelCategory")	g_Turnsummary.AddTextItem(g_interfaceid.STATIC_STATIC, a, b, c, d, text, fontsize);
			if (name == "LabelItem")		g_Turnsummary.AddTextItem(g_interfaceid.TEXT_LABELITEM, a, b, c, d, text, fontsize);
			if (name == "LabelEvent")		g_Turnsummary.AddTextItem(g_interfaceid.STATIC_STATIC, a, b, c, d, text, fontsize);
			if (name == "LabelDesc")		g_Turnsummary.AddTextItem(g_interfaceid.TEXT_TSEVENTDESC, a, b, c, d, text, fontsize);
			if (name == "GoTo")				g_Turnsummary.AddButton(image, g_interfaceid.BUTTON_TSGOTO, a, b, c, d, Transparency, Scale);
			if (name == "NextWeek")			g_Turnsummary.AddButton(image, g_interfaceid.BUTTON_TSNEWWEEK, a, b, c, d, Transparency, Scale);
			if (name == "Back")				g_Turnsummary.AddButton(image, g_interfaceid.BUTTON_TSCLOSE, a, b, c, d, Transparency, Scale);
			if (name == "Prev")				g_Turnsummary.AddButton(image, g_interfaceid.BUTTON_TSPREVBROTHEL, a, b, c, d, Transparency, Scale);
			if (name == "Next")				g_Turnsummary.AddButton(image, g_interfaceid.BUTTON_TSNEXTBROTHEL, a, b, c, d, Transparency, Scale);
			if (name == "Category")			g_Turnsummary.AddListBox(g_interfaceid.LIST_TSCATEGORY, a, b, c, d, e, true);
			if (name == "Item")				g_Turnsummary.AddListBox(g_interfaceid.LIST_TSITEM, a, b, c, d, e, true);
			if (name == "Event")			g_Turnsummary.AddListBox(g_interfaceid.LIST_TSEVENTS, a, b, c, d, e, true);
			if (name == "Background")		g_Turnsummary.AddImage(g_interfaceid.IMAGE_TSIMAGE, dp, a, b, c, d);
//														  AddImage(g_interfaceid.IMAGE_TSIMAGE, dp, full_x, full_y, xw.w, xw.h, xw.stat, xw.r, xw.g, xw.b);


		}
	}
	else // because there never was a TurnSummary.txt, just do defaults if there is no xml
	{
		g_LogFile.write("Loading Default TurnSummary");
		g_Turnsummary.CreateWindow(8, 8, 786, 584, 1);
		g_Turnsummary.AddTextItem(g_interfaceid.TEXT_CURRENTBROTHEL, 0, 0, 900, 32, "", 10);
		g_Turnsummary.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 8, 120, 32, "Category", 15);
		g_Turnsummary.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 106, 120, 32, "Item", 15);
		g_Turnsummary.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 266, 120, 32, "Event", 15);
		g_Turnsummary.AddTextItem(g_interfaceid.TEXT_TSEVENTDESC, 575, 8, 202, 406, "", 12);
		g_Turnsummary.AddButton("GoTo", g_interfaceid.BUTTON_TSGOTO, 598, 462, 160, 32, true);
		g_Turnsummary.AddButton("NextWeek", g_interfaceid.BUTTON_TSNEWWEEK, 598, 504, 160, 32, true);
		g_Turnsummary.AddButton("Back", g_interfaceid.BUTTON_TSCLOSE, 598, 546, 160, 32, true);
		g_Turnsummary.AddButton("Prev", g_interfaceid.BUTTON_TSPREVBROTHEL, 598, 422, 72, 32, true);
		g_Turnsummary.AddButton("Next", g_interfaceid.BUTTON_TSNEXTBROTHEL, 686, 422, 72, 32, true);
		g_Turnsummary.AddListBox(g_interfaceid.LIST_TSCATEGORY, 8, 40, 120, 66, 1, true);
		g_Turnsummary.AddListBox(g_interfaceid.LIST_TSITEM, 8, 136, 120, 128, 1, true);
		g_Turnsummary.AddListBox(g_interfaceid.LIST_TSEVENTS, 8, 296, 120, 282, 1, true);
		g_Turnsummary.AddImage(g_interfaceid.IMAGE_TSIMAGE, "", 136, 8, 434, 570);
	}

	// Load TransferGirls screen
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "TransferGirls.xml";
	TiXmlDocument docTransferGirls(dp.c_str());
	if (docTransferGirls.LoadFile())
	{
		g_LogFile.write("Loading TransferGirls.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docTransferGirls.RootElement();

		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename);	string name = "";
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", a); xu.get_att(el, "YPos", b); xu.get_att(el, "Width", c); xu.get_att(el, "Height", d); xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true); xu.get_att(el, "File", file, true);
			xu.get_att(el, "Transparency", Transparency, true); xu.get_att(el, "Scale", Scale, true);
			xu.get_att(el, "Text", text, true); xu.get_att(el, "FontSize", fontsize, true);

			if (name == "Transfer Girls")	g_TransferGirls.CreateWindow(a, b, c, d, e);
			if (name == "Back")				g_TransferGirls.AddButton(image, g_interfaceid.BUTTON_TRANSGBACK, a, b, c, d, Transparency, Scale);
			if (name == "ShiftLeft")		g_TransferGirls.AddButton(image, g_interfaceid.BUTTON_TRANSGSHIFTL, a, b, c, d, Transparency, Scale);
			if (name == "ShiftRight")		g_TransferGirls.AddButton(image, g_interfaceid.BUTTON_TRANSGSHIFTR, a, b, c, d, Transparency, Scale);
			if (name == "ListLeft")			g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, a, b, c, d, e, true, true);
			if (name == "ListRight")		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, a, b, c, d, e, true, true);
			if (name == "BrothelLeft")		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, a, b, c, d, e, true);
			if (name == "BrothelRight")		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, a, b, c, d, e, true);
			if (name == "LabelLeft")		g_TransferGirls.AddTextItem(g_interfaceid.STATIC_STATIC, a, b, c, d, text, fontsize);
			if (name == "LabelRight")		g_TransferGirls.AddTextItem(g_interfaceid.STATIC_STATIC, a, b, c, d, text, fontsize);
		}
	}
	else // because there never was a TransferGirls.txt, just do defaults if there is no xml
	{
		g_LogFile.write("Loading Default TransferGirls");

		g_TransferGirls.CreateWindow(16, 16, 768, 576, 1);
		g_TransferGirls.AddButton("Back", g_interfaceid.BUTTON_TRANSGBACK, 308, 536, 160, 32, true);
		g_TransferGirls.AddButton("ShiftLeft", g_interfaceid.BUTTON_TRANSGSHIFTL, 366, 214, 48, 48, true);
		g_TransferGirls.AddButton("ShiftRight", g_interfaceid.BUTTON_TRANSGSHIFTR, 366, 304, 48, 48, true);
		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGLEFTGIRLS, 168, 48, 190, 482, 1, true, true);
		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGRIGHTGIRLS, 418, 48, 190, 482, 1, true, true);
		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGLEFTBROTHEL, 8, 48, 126, 234, 1, true, false);
		g_TransferGirls.AddListBox(g_interfaceid.LIST_TRANSGRIGHTBROTHEL, 632, 48, 126, 234, 1, true, false);
		g_TransferGirls.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 8, 160, 32, "BROTHELS");
		g_TransferGirls.AddTextItem(g_interfaceid.STATIC_STATIC, 632, 8, 160, 32, "BROTHELS");
	}


	g_LogFile.write("Loading Preparing Game Screen");
	g_Preparing.CreateWindow(300, 200, 200, 200, 1);
	g_Preparing.AddTextItem(g_interfaceid.STATIC_STATIC, 50, 20, 180, 30, "Preparing", 25, true, false);
	g_Preparing.AddTextItem(g_interfaceid.STATIC_STATIC, 65, 65, 180, 30, "Game", 25, true, false);
	g_Preparing.AddTextItem(g_interfaceid.STATIC_STATIC, 65, 110, 180, 30, "Please", 25, true, false);
	g_Preparing.AddTextItem(g_interfaceid.STATIC_STATIC, 70, 155, 180, 30, "Wait", 25, true, false);


	// gallery screen
	g_LogFile.write("Loading Gallery Screen");
	g_Gallery.CreateWindow(16, 16, 768, 576, 1);
	g_Gallery.AddButton("Anal", g_interfaceid.BUTTON_GALLERYANAL, 600, 8, 160, 32, true);
	g_Gallery.AddButton("BDSM", g_interfaceid.BUTTON_GALLERYBDSM, 600, 48, 160, 32, true);
	g_Gallery.AddButton("Sex", g_interfaceid.BUTTON_GALLERYSEX, 600, 88, 160, 32, true);
	g_Gallery.AddButton("Beast", g_interfaceid.BUTTON_GALLERYBEAST, 600, 128, 160, 32, true);
	g_Gallery.AddButton("Group", g_interfaceid.BUTTON_GALLERYGROUP, 600, 168, 160, 32, true);
	g_Gallery.AddButton("Lesbian", g_interfaceid.BUTTON_GALLERYLESBIAN, 600, 208, 160, 32, true);
	g_Gallery.AddButton("Pregnant", g_interfaceid.BUTTON_GALLERYPREGNANT, 600, 248, 160, 32, true);
	g_Gallery.AddButton("Death", g_interfaceid.BUTTON_GALLERYDEATH, 600, 288, 160, 32, true);
	g_Gallery.AddButton("Profile", g_interfaceid.BUTTON_GALLERYPROFILE, 600, 328, 160, 32, true);
	g_Gallery.AddButton("Combat", g_interfaceid.BUTTON_GALLERYCOMBAT, 600, 368, 160, 32, true);
	g_Gallery.AddButton("Oral", g_interfaceid.BUTTON_GALLERYORAL, 600, 408, 160, 32, true);
	g_Gallery.AddButton("Back", g_interfaceid.BUTTON_GALLERYBACK, 600, 448, 160, 32, true);
	g_Gallery.AddButton("Prev", g_interfaceid.BUTTON_GALLERYPREV, 600, 488, 72, 32, true);
	g_Gallery.AddButton("Next", g_interfaceid.BUTTON_GALLERYNEXT, 688, 488, 72, 32, true);
	g_Gallery.AddImage(g_interfaceid.IMAGE_GALLERYIMAGE, "", 8, 8, 584, 560);
	g_Gallery.AddTextItem(g_interfaceid.TEXT_GALLERYTYPE, 600, 528, 160, 32, "", 16, true, false);
	g_Gallery.AddButton("Next", g_interfaceid.BUTTON_NEXTGALLERY, 688, 528, 72, 32, true);

	// gallery 2 screen
	g_LogFile.write("Loading Gallery 2 Screen");
	g_Gallery2.CreateWindow(16, 16, 768, 576, 1);
	g_Gallery2.AddButton("Ecchi", g_interfaceid.BUTTON_GALLERYECCHI, 600, 8, 160, 32, true);
	g_Gallery2.AddButton("Striptease", g_interfaceid.BUTTON_GALLERYSTRIP, 600, 48, 160, 32, true);
	g_Gallery2.AddButton("Maid", g_interfaceid.BUTTON_GALLERYMAID, 600, 88, 160, 32, true);
	g_Gallery2.AddButton("Singer", g_interfaceid.BUTTON_GALLERYSING, 600, 128, 160, 32, true);
	g_Gallery2.AddButton("Waitress", g_interfaceid.BUTTON_GALLERYWAIT, 600, 168, 160, 32, true);
	g_Gallery2.AddButton("Dealer", g_interfaceid.BUTTON_GALLERYCARD, 600, 208, 160, 32, true);
	g_Gallery2.AddButton("Bunny", g_interfaceid.BUTTON_GALLERYBUNNY, 600, 248, 160, 32, true);
	g_Gallery2.AddButton("Nude", g_interfaceid.BUTTON_GALLERYNUDE, 600, 288, 160, 32, true);
	g_Gallery2.AddButton("Mast", g_interfaceid.BUTTON_GALLERYMAST, 600, 328, 160, 32, true);
	g_Gallery2.AddButton("Titfuck", g_interfaceid.BUTTON_GALLERYTITTY, 600, 368, 160, 32, true);
	g_Gallery2.AddButton("", g_interfaceid.BUTTON_WHAT, 600, 368, 160, 32, true);
	g_Gallery2.AddButton("Back", g_interfaceid.BUTTON_GALLERYBACK, 600, 448, 160, 32, true);
	g_Gallery2.AddButton("Prev", g_interfaceid.BUTTON_GALLERYPREV, 600, 488, 72, 32, true);
	g_Gallery2.AddButton("Next", g_interfaceid.BUTTON_GALLERYNEXT, 688, 488, 72, 32, true);
	g_Gallery2.AddImage(g_interfaceid.IMAGE_GALLERYIMAGE, "", 8, 8, 584, 560);
	g_Gallery2.AddTextItem(g_interfaceid.TEXT_GALLERYTYPE, 600, 528, 160, 32, "", 16, true, false);

	// `J` Bookmark - Loading the screens

	// Main Menu
	g_LogFile.write("Loading MainMenu");
	g_MainMenu.load();
	g_WinManager.add_window("Main Menu", &g_MainMenu);

	// New Game
	g_LogFile.write("Loading NewGame");
	g_NewGame.load();
	g_WinManager.add_window("New Game", &g_NewGame);

	// Get Input Screen
	g_LogFile.write("Loading Input Screen");
	g_GetInput.load();
	g_WinManager.add_window("GetInput", &g_GetInput);

	// Brothel Management Screen
	g_LogFile.write("Loading Brothel Management Screen");
	g_BrothelManagement.load();
	g_WinManager.add_window("Brothel Management", &g_BrothelManagement);

	// GIRL MANAGEMENT SCREEN
	g_LogFile.write("Loading Girl Management Screen");
	g_GirlManagement.load();
	g_WinManager.add_window("Girl Management", &g_GirlManagement);
	
	// CLINIC MANAGEMENT SCREEN
	g_LogFile.write("Loading Clinic Management Screen");
	g_ClinicManagement.load();
	g_WinManager.add_window("Clinic", &g_ClinicManagement);

	// MOVIE STUDIO SCREEN
	g_LogFile.write("Loading Studio Management Screen");
	g_StudioManagement.load();
	g_WinManager.add_window("Studio", &g_StudioManagement);

	// MOVIE MAKER SCREEN
	g_LogFile.write("Loading Movie Maker Screen");
	g_MovieMaker.load();
	g_WinManager.add_window("Movie Maker", &g_MovieMaker);

	// arena SCREEN
	g_LogFile.write("Loading Arena Management Screen");
	g_ArenaManagement.load();
	g_WinManager.add_window("Arena", &g_ArenaManagement);

	// centre SCREEN
	g_LogFile.write("Loading Centre Management Screen");
	g_CentreManagement.load();
	g_WinManager.add_window("Centre", &g_CentreManagement);

	// house management SCREEN
	g_LogFile.write("Loading House Management Screen");
	g_HouseManagement.load();
	g_WinManager.add_window("House Management", &g_HouseManagement);

	// farm management SCREEN
	g_LogFile.write("Loading Farm Management Screen");
	g_FarmManagement.load();
	g_WinManager.add_window("Farm", &g_FarmManagement);

	// GIRL DETAILS
	g_LogFile.write("Loading Girl Details Screen");
	g_GirlDetails.load();
	g_WinManager.add_window("Girl Details", &g_GirlDetails);

	// Gang Management
	g_LogFile.write("Loading Gang Management Screen");
	g_GangManagement.load();
	g_WinManager.add_window("Gangs", &g_GangManagement);

	// Dungeon Screen
	g_LogFile.write("Loading Dungeon Screen");
	g_Dungeon.load();
	g_WinManager.add_window("Dungeon", &g_Dungeon);

	// Town screen
	g_LogFile.write("Loading Town Screen");
	g_TownScreen.load();
	g_WinManager.add_window("Town", &g_TownScreen);

	// Gallery screen
	g_LogFile.write("Loading Test Gallery Screen");
	g_GalleryScreen.load();
	g_WinManager.add_window("Gallery", &g_GalleryScreen);

	// clinic screen
	g_LogFile.write("Loading Clinic Screen");
	g_ClinicScreen.load();
	g_WinManager.add_window("Clinic Screen", &g_ClinicScreen);

	// centre screen
	g_LogFile.write("Loading Centre Screen");
	g_CentreScreen.load();
	g_WinManager.add_window("Centre Screen", &g_CentreScreen);

	// movie screen
	g_LogFile.write("Loading Movie Screen");
	g_MovieScreen.load();
	g_WinManager.add_window("Movie Screen", &g_MovieScreen);

	// auction SCREEN
	g_LogFile.write("Loading Auction Screen");
	g_AuctionScreen.load();
	g_WinManager.add_window("Auction", &g_AuctionScreen);

	// arena screen
	g_LogFile.write("Loading Arena Screen");
	g_ArenaScreen.load();
	g_WinManager.add_window("Arena Screen", &g_ArenaScreen);

	// Arena tryout screen
	g_LogFile.write("Loading Arena Try Screen");
	g_ArenaTry.load();
	g_WinManager.add_window("Arena Try", &g_ArenaTry);

	// clinic tryout screen
	g_LogFile.write("Loading Clinic Try Screen");
	g_ClinicTry.load();
	g_WinManager.add_window("Clinic Try", &g_ClinicTry);

	// casting tryout screen
	g_LogFile.write("Loading Casting Try Screen");
	g_CastingTry.load();
	g_WinManager.add_window("Casting Try", &g_CastingTry);

	// Slave market screen
	g_LogFile.write("Loading Slave Market Screen");
	g_SlaveMarket.load();
	g_WinManager.add_window("Slave Market", &g_SlaveMarket);

	// Building Setup Screen
	g_LogFile.write("Loading Building Setup Screen");
	g_BuildingSetupScreen.load();
	g_WinManager.add_window("Building Setup", &g_BuildingSetupScreen);

	// Mayors office
	g_LogFile.write("Loading Mayor Screen");
	g_MayorsOfficeScreen.load();
	g_WinManager.add_window("Mayor", &g_MayorsOfficeScreen);

	// Bank Screen
	g_LogFile.write("Loading Bank Screen");
	g_BankScreen.load();
	g_WinManager.add_window("Bank", &g_BankScreen);

	// Players house
	g_LogFile.write("Loading Player House Screen");
	g_HouseScreen.load();
	g_WinManager.add_window("Player House", &g_HouseScreen);

	// farm screen
	g_LogFile.write("Loading Farm Screen");
	g_FarmScreen.load();
	g_WinManager.add_window("Farm Screen", &g_FarmScreen);

	// Players house (statistic screen)
	g_LogFile.write("Loading House Screen");
	g_PlayersHouse.load();
	g_WinManager.add_window("House", &g_PlayersHouse);

	// Item Management Screen
	g_LogFile.write("Loading Item Management Screen");
	g_ItemManagement.load();
	g_WinManager.add_window("Item Management", &g_ItemManagement);

	// Prison Screen
	g_LogFile.write("Loading Prison Screen");
	g_PrisonScreen.load();
	g_WinManager.add_window("Prison", &g_PrisonScreen);
	
	// Building Management Screen
	g_LogFile.write("Loading Building Management Screen");
	g_BuildingManagementScreen.load();
	g_WinManager.add_window("Building Management", &g_BuildingManagementScreen);

	// Property Management Screen	// `J` added
	g_LogFile.write("Loading Property Management Screen");
	g_PropertyManagementScreen.load();
	g_WinManager.add_window("Property Management", &g_PropertyManagementScreen);

	// Setup the messagebox
	g_LogFile.write("Setting up MessageBox");
	g_interfaceid.STATIC_STATIC=1;//evil magic number
	g_MessageBox.CreateWindow();
}
