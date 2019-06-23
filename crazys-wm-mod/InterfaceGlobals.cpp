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
#include "cScreenAuction.h"
#include "cScreenArena.h"
#include "cScreenArenaTry.h"
#include "cScreenHouse.h"
#include "cScreenClinic.h"
#include "cScreenCentre.h"
#include "cScreenMovie.h"
#include "cScreenFarm.h"
#include "cScreenClinicTry.h"
#include "cScreenCastingTry.h"
#include "cScreenSlaveMarket.h"
#include "cScreenMayor.h"
#include "cScreenBank.h"
#include "cScreenHouseDetails.h"
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
#include "cScreenPreparingGame.h"
#include "cScreenNewGame.h"
#include "cScreenSettings.h"
#include "cScreenBrothelManagement.h"
#include "cScreenGetInput.h"
#include "cScreenGallery.h"
#include "sConfig.h"
#include "XmlUtil.h"
#include "cScreenTurnSummary.h"
#include "cScreenTransfer.h"

using namespace std;

extern sInterfaceIDs g_interfaceid;
extern cWindowManager g_WinManager;

cInterfaceEventManager g_InterfaceEvents;
cScreenMainMenu g_MainMenu;
cScreenPreparingGame g_Preparing;
cScreenNewGame g_NewGame;			// NewGame.xml
cScreenSettings g_Settings;			// `J` added

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
cScreenArenaTry g_ArenaTry;
cScreenClinic g_ClinicScreen;
cScreenCentre g_CentreScreen;
cScreenHouse g_HouseScreen;
cScreenFarm g_FarmScreen;
cMovieScreen g_MovieScreen;
cScreenClinicTry g_ClinicTry;
cScreenCastingTry g_CastingTry;
cScreenArena g_ArenaScreen;
cScreenAuction g_AuctionScreen;
cScreenBuildingSetup g_BuildingSetupScreen;
cScreenMayor g_MayorsOfficeScreen;
cScreenBank g_BankScreen;
cScreenHouseDetails g_PlayersHouse;
cScreenItemManagement g_ItemManagement;
cScreenPrison g_PrisonScreen;
cBuildingManagement g_BuildingManagementScreen;
cScreenPropertyManagement g_PropertyManagementScreen;	// `J` added
cScreenTurnSummary g_TurnSummary;
cScreenTransfer g_TransferScreen;	// `J` added for .06.03.02

cScreenGetInput g_GetInput;

cInterfaceWindow g_GetString;		// GetString.xml
cInterfaceWindow g_LoadGame;		// LoadMenu.xml
//cInterfaceWindow g_TransferGirls;	// Hard coded
//cInterfaceWindow g_Gallery;			// Hard coded
//cInterfaceWindow g_Gallery2;		// Hard coded
cInterfaceWindow g_ChangeJobs;		// Hard coded
//cInterfaceWindow g_Turnsummary;		// Hard coded

cMessageBox g_MessageBox;
cChoiceManager g_ChoiceManager;
cMessageQue g_MessageQue;

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

void FreeInterface()
{
	g_MainMenu.Free();
	g_Preparing.Free();
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
//	g_Turnsummary.Free();
	g_Dungeon.Free();
	g_SlaveMarket.Free();
	g_TownScreen.Free();
//	g_GalleryScreen.Free();
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
//	g_Gallery.Free();
//	g_Gallery2.Free();
	g_BuildingSetupScreen.Free();
	g_GetInput.Free();
	g_MayorsOfficeScreen.Free();
	g_BankScreen.Free();
	g_ChoiceManager.Free();
	g_MessageQue.Free();
	g_LoadGame.Free();
	g_Settings.Free();
	g_PlayersHouse.Free();
//	g_TransferGirls.Free();
	g_TransferScreen.Free();
	g_ItemManagement.Free();
	g_PrisonScreen.Free();
	g_MovieMaker.Free();
	g_BuildingManagementScreen.Free();
}

void ResetInterface()
{
	g_MainMenu.Reset();
	g_Preparing.Reset();
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
//	g_Turnsummary.Reset();
	g_Dungeon.Reset();
	g_SlaveMarket.Reset();
	g_TownScreen.Reset();
//	g_GalleryScreen.Reset();
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
//	g_Gallery.Reset();
//	g_Gallery2.Reset();
	g_BuildingSetupScreen.Reset();
	g_GetInput.Reset();
	g_MayorsOfficeScreen.Reset();
	g_BankScreen.Reset();
	g_ChoiceManager.Free();
	g_LoadGame.Reset();
	g_Settings.Reset();
	g_PlayersHouse.Reset();
//	g_TransferGirls.Reset();
	g_TransferScreen.Reset();
	g_ItemManagement.Reset();
	g_PrisonScreen.Reset();
	g_MovieMaker.Reset();
	g_BuildingManagementScreen.Reset();
}

void LoadInterface()
{
	cTariff tariff;
	stringstream ss;
	int r = 0, g = 0, b = 0, x = 0, y = 0, w = 0, h = 0, a = 0, c = 0, d = 0, e = 0, fontsize = 10, rowheight = 20,
		increment = 0, min = 0, max = 0, value = 0;
	string image = ""; string text = ""; string file = "";
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


	// Load game screen
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.xml";
	TiXmlDocument docLoadMenu(dp.c_str());
	if (docLoadMenu.LoadFile())
	{
		g_LogFile.write("Loading LoadMenu.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docLoadMenu.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename); string name;
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", x); xu.get_att(el, "YPos", y); xu.get_att(el, "Width", w); xu.get_att(el, "Height", h); xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true); xu.get_att(el, "Transparency", Transparency, true); xu.get_att(el, "Scale", Scale, true);
			xu.get_att(el, "File", file, true); xu.get_att(el, "Text", text, true);
			xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 10;
			xu.get_att(el, "RowHeight", rowheight); if (rowheight == 0) rowheight = 20;
			xu.get_att(el, "Red", r, true); xu.get_att(el, "Green", g, true); xu.get_att(el, "Blue", b, true);

			if (name == "LoadMenu")		g_LoadGame.CreateWindow(x, y, w, h, e);
			if (name == "WhoreMaster")	g_LoadGame.AddTextItem(g_interfaceid.STATIC_STATIC, x, y, w, h, text, fontsize, false, false, false, r, g, b);
			if (name == "FileName")		g_LoadGame.AddListBox(g_interfaceid.LIST_LOADGSAVES, x, y, w, h, e, true, false, false, true, true, fontsize, rowheight);
			if (name == "LoadGame")		g_LoadGame.AddButton(image, g_interfaceid.BUTTON_LOADGLOAD, x, y, w, h, Transparency, Scale);
			if (name == "BackButton")	g_LoadGame.AddButton(image, g_interfaceid.BUTTON_LOADGBACK, x, y, w, h, Transparency, Scale);
			if (name == "Background")
			{
				DirPath dp = ImagePath(file);
				g_LoadGame.AddImage(g_interfaceid.IMAGE_BGIMAGE, dp, x, y, w, h);
			}
		}
	}
	else
	{
		g_LogFile.write("Loading Default LoadMenu");
		g_LoadGame.CreateWindow(224, 128, 344, 344, 1);
		g_LoadGame.AddTextItem(g_interfaceid.STATIC_STATIC, 0, 334 - 10, 344, 12, "Please read the readme.html", 10);
		g_LoadGame.AddListBox(g_interfaceid.LIST_LOADGSAVES, 8, 8, 328, 288, 1, true, false, false, true, true, fontsize, rowheight);
		g_LoadGame.AddButton("Load", g_interfaceid.BUTTON_LOADGLOAD, 8, 304, 160, 32, true);
		g_LoadGame.AddButton("Back", g_interfaceid.BUTTON_LOADGBACK, 176, 304, 160, 32, true);
	}

	


	// Load GetString screen
	dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "GetString.xml";
	TiXmlDocument docGetString(dp.c_str());
	if (docGetString.LoadFile())
	{
		g_LogFile.write("Loading GetString.xml");
		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docGetString.RootElement();

		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename);	string name;
			xu.get_att(el, "Name", name);
			xu.get_att(el, "XPos", x); xu.get_att(el, "YPos", y); xu.get_att(el, "Width", w); xu.get_att(el, "Height", h); 
			xu.get_att(el, "Red", r); xu.get_att(el, "Green", g); xu.get_att(el, "Blue", b); xu.get_att(el, "LeftOrRight", leftorright);
			xu.get_att(el, "Border", e, true);
			xu.get_att(el, "Image", image, true); xu.get_att(el, "Transparency", Transparency, true); 
			xu.get_att(el, "Scale", Scale, true); xu.get_att(el, "Text", text, true);
			xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 16;

			if (name == "GetString")	g_GetString.CreateWindow(x, y, w, h, e);
			if (name == "Ok")			g_GetString.AddButton(image, g_interfaceid.BUTTON_OK, x, y, w, h, Transparency, Scale);
			if (name == "Cancel")		g_GetString.AddButton(image, g_interfaceid.BUTTON_CANCEL, x, y, w, h, Transparency, Scale);
			if (name == "Label")		g_GetString.AddTextItem(g_interfaceid.TEXT_TEXT1, x, y, w, h, text, fontsize, true, false, leftorright, r, g, b);
			if (name == "TextField")	g_GetString.AddEditBox(g_interfaceid.EDITBOX_NAME, x, y, w, h, e, fontsize);
		}
	}
	else
	{
		g_LogFile.write("Loading Default GetString");
		g_GetString.CreateWindow(224, 127, 352, 160, 1);
		g_GetString.AddButton("Ok", g_interfaceid.BUTTON_OK, 32, 104, 128, 32, true);
		g_GetString.AddButton("Cancel", g_interfaceid.BUTTON_CANCEL, 192, 104, 128, 32, true);
		g_GetString.AddTextItem(g_interfaceid.TEXT_TEXT1, 32, 32, 128, 32, "Enter Text:", 16);
		g_GetString.AddEditBox(g_interfaceid.EDITBOX_NAME, 160, 32, 160, 32, 1, fontsize);
	}


	// `J` This is never used so not updating it now
	// Change Jobs Screen
	g_LogFile.write("Loading Change Jobs Screen");
	g_ChangeJobs.CreateWindow(256, 120, 304, 376, 1);
	g_ChangeJobs.AddButton("Ok", g_interfaceid.BUTTON_CJOK, 8, 336, 128, 32, true);
	g_ChangeJobs.AddButton("Cancel", g_interfaceid.BUTTON_CJCANCEL, 152, 336, 128, 32, true);
	g_ChangeJobs.AddTextItem(g_interfaceid.TEXT_CJDESC, 8, 240, 272, 88, "", 10);
	g_ChangeJobs.AddListBox(g_interfaceid.LIST_CJDAYTIME, 8, 48, 144, 192, 1, true, false, false, true, true, fontsize, rowheight);
	g_ChangeJobs.AddListBox(g_interfaceid.LIST_CJNIGHTTIME, 152, 48, 144, 192, 1, true, false, false, true, true, fontsize, rowheight);
	g_ChangeJobs.AddTextItem(g_interfaceid.STATIC_STATIC, 8, 8, 128, 32, "Day Shift");
	g_ChangeJobs.AddTextItem(g_interfaceid.STATIC_STATIC, 152, 8, 128, 32, "Night Shift");


	g_LogFile.write("Loading Preparing Game Screen");
	g_Preparing.load();
	g_WinManager.add_window("Preparing Game", &g_Preparing);
	// `J` Bookmark - Loading the screens

	// Main Menu
	g_LogFile.write("Loading MainMenu");
	g_MainMenu.load();
	g_WinManager.add_window("Main Menu", &g_MainMenu);

	// New Game
	g_LogFile.write("Loading NewGame");
	g_NewGame.load();
	g_WinManager.add_window("New Game", &g_NewGame);

	// Settings
	g_LogFile.write("Loading Settings");
	g_Settings.load();
	g_WinManager.add_window("Settings", &g_Settings);

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

	// Transfer Screen
	g_LogFile.write("Loading Transfer Screen");
	g_TransferScreen.load();
	g_WinManager.add_window("Transfer Screen", &g_TransferScreen);

	// Turn Summary Management
	g_LogFile.write("Loading Turn Summary Screen");
	g_TurnSummary.load();
	g_WinManager.add_window("Turn Summary", &g_TurnSummary);

	// Dungeon Screen
	g_LogFile.write("Loading Dungeon Screen");
	g_Dungeon.load();
	g_WinManager.add_window("Dungeon", &g_Dungeon);

	// Town screen
	g_LogFile.write("Loading Town Screen");
	g_TownScreen.load();
	g_WinManager.add_window("Town", &g_TownScreen);

	// Gallery screen
	g_LogFile.write("Loading Gallery Screen");
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
