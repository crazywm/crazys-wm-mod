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
#include "cBrothel.h"
#include "cScreenBuildingSetup.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cMovieStudio.h"
#include "cClinic.h"
#include "cArena.h"
#include "cCentre.h"
#include "cHouse.h"
#include "cFarm.h"

extern bool					g_InitWin;
extern int					g_CurrBrothel;
extern cGold				g_Gold;
extern cBrothelManager		g_Brothels;
extern cWindowManager		g_WinManager;
extern cMovieStudioManager	g_Studios;
extern cClinicManager		g_Clinic;
extern cArenaManager		g_Arena;
extern cCentreManager		g_Centre;
extern cHouseManager		g_House;
extern cFarmManager			g_Farm;
extern int					g_CurrentScreen;
extern int					g_Building;

static cTariff tariff;
static stringstream ss;

bool cScreenBuildingSetup::ids_set = false;
cScreenBuildingSetup::cScreenBuildingSetup()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "building_setup_screen.xml";
	m_filename = dp.c_str();
}
cScreenBuildingSetup::~cScreenBuildingSetup() {}

void cScreenBuildingSetup::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenBuildingSetup");

	back_id			/**/ = get_id("BackButton", "Back");
	curbrothel_id	/**/ = get_id("CurrentBrothel");
	gold_id			/**/ = get_id("Gold");
	potioncost_id	/**/ = get_id("PotionCost");
	potionavail_id	/**/ = get_id("AvailablePotions");
	potions10_id	/**/ = get_id("10PotionsButton");
	potions20_id	/**/ = get_id("20PotionsButton");
	autopotions_id	/**/ = get_id("AutoBuyPotionToggle");
	barstaff_id		/**/ = get_id("BarStaffCost", "*Unused*");//
	barhire_id		/**/ = get_id("BarHireButton", "*Unused*");//
	barfire_id		/**/ = get_id("BarFireButton", "*Unused*");//
	casinostaff_id	/**/ = get_id("CasinoStaffCost", "*Unused*");//
	casinohire_id	/**/ = get_id("CasinoHireButton", "*Unused*");//
	casinofire_id	/**/ = get_id("CasinoFireButton", "*Unused*");//
	roomcost_id		/**/ = get_id("RoomAddCost");
	buyrooms_id		/**/ = get_id("BuildRoomsButton");
	restrict_id		/**/ = get_id("SexRestrictions", "*Unused*");//

	noanal_id		/**/ = get_id("ProhibitAnalToggleRes");
	nobdsm_id		/**/ = get_id("ProhibitBDSMToggleRes");
	nobeast_id		/**/ = get_id("ProhibitBeastToggleRes");
	nofoot_id		/**/ = get_id("ProhibitFootJobToggleRes");
	nogroup_id		/**/ = get_id("ProhibitGroupToggleRes");
	nohand_id		/**/ = get_id("ProhibitHandJobToggleRes");
	nolesbian_id	/**/ = get_id("ProhibitLesbianToggleRes");
	nonormal_id		/**/ = get_id("ProhibitNormalToggleRes");
	nooral_id		/**/ = get_id("ProhibitOralToggleRes");
	nostrip_id		/**/ = get_id("ProhibitStripToggleRes");
	notitty_id		/**/ = get_id("ProhibitTittyToggleRes");

	advertsli_id	/**/ = get_id("AdvertisingSlider");
	advertamt_id	/**/ = get_id("AdvertisingValue");
}

void cScreenBuildingSetup::init()
{
	g_CurrentScreen = SCREEN_BROTHELMANAGEMENT;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	int rooms = 20, maxrooms = 200, antipregnum = 0, antipregused = 0, advert = 0;
	string brothel = "";
	switch (g_Building)
	{
	case BUILDING_STUDIO:
		brothel = "The Movie Studio";
		rooms = g_Studios.GetBrothel(0)->m_NumRooms;
		maxrooms = g_Studios.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_Studios.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_Studios.GetBrothel(0)->m_AntiPregUsed;
		advert = g_Studios.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Studios.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Studios.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Studios.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Studios.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Studios.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Studios.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Studios.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Studios.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Studios.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Studios.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Studios.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Studios.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_CLINIC:
		brothel = "The Clinic";
		rooms = g_Clinic.GetBrothel(0)->m_NumRooms;
		maxrooms = g_Clinic.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_Clinic.GetBrothel(0)->m_AntiPregUsed;
		advert = g_Clinic.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Clinic.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Clinic.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Clinic.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Clinic.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Clinic.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Clinic.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Clinic.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Clinic.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Clinic.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Clinic.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Clinic.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Clinic.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_ARENA:
		brothel = "The Arena";
		rooms = g_Arena.GetBrothel(0)->m_NumRooms;
		maxrooms = g_Arena.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_Arena.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_Arena.GetBrothel(0)->m_AntiPregUsed;
		advert = g_Arena.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Arena.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Arena.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Arena.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Arena.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Arena.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Arena.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Arena.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Arena.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Arena.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Arena.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Arena.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Arena.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_CENTRE:
		brothel = "The Community Centre";
		rooms = g_Centre.GetBrothel(0)->m_NumRooms;
		maxrooms = g_Centre.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_Centre.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_Centre.GetBrothel(0)->m_AntiPregUsed;
		advert = g_Centre.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Centre.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Centre.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Centre.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Centre.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Centre.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Centre.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Centre.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Centre.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Centre.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Centre.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Centre.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Centre.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_HOUSE:
		brothel = "Your House";
		rooms = g_House.GetBrothel(0)->m_NumRooms;
		maxrooms = g_House.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_House.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_House.GetBrothel(0)->m_AntiPregUsed;
		advert = g_House.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_House.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_House.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_House.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_House.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_House.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_House.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_House.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_House.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_House.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_House.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_House.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_House.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_FARM:
		brothel = "Your Farm";
		rooms = g_Farm.GetBrothel(0)->m_NumRooms;
		maxrooms = g_Farm.GetBrothel(0)->m_MaxNumRooms;
		antipregnum = g_Farm.GetBrothel(0)->m_AntiPregPotions;
		antipregused = g_Farm.GetBrothel(0)->m_AntiPregUsed;
		advert = g_Farm.GetBrothel(0)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Farm.GetBrothel(0)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Farm.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Farm.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Farm.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Farm.GetBrothel(0)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Farm.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Farm.GetBrothel(0)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Farm.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Farm.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Farm.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Farm.GetBrothel(0)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Farm.GetBrothel(0)->m_RestrictTitty);
		break;
	case BUILDING_BROTHEL:
	default:
		brothel = g_Brothels.GetName(g_CurrBrothel);
		rooms = g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms;
		maxrooms = g_Brothels.GetBrothel(g_CurrBrothel)->m_MaxNumRooms;
		antipregnum = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions;
		antipregused = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregUsed;
		advert = g_Brothels.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget / 50;

		// setup check boxes
		SetCheckBox(autopotions_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_KeepPotionsStocked);
		SetCheckBox(noanal_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictAnal);
		SetCheckBox(nobdsm_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBDSM);
		SetCheckBox(nobeast_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBeast);
		SetCheckBox(nofoot_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictFoot);
		SetCheckBox(nogroup_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictGroup);
		SetCheckBox(nohand_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictHand);
		SetCheckBox(nolesbian_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictLesbian);
		SetCheckBox(nonormal_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictNormal);
		SetCheckBox(nooral_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictOral);
		SetCheckBox(nostrip_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictStrip);
		SetCheckBox(notitty_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictTitty);
		break;
	}

	if (gold_id >= 0)
	{
		ss.str(""); ss << "Gold: " << g_Gold.ival();
		EditTextItem(ss.str(), gold_id);
	}

	EditTextItem(brothel, curbrothel_id);
	ss.str("");	ss << "Anti-Preg Potions: " << tariff.anti_preg_price(1) << " gold each.";
	EditTextItem(ss.str(), potioncost_id);

	// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
	advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
	ss.str("");	ss << "Advertising Budget: " << (advert * 50) << " gold / week";
	EditTextItem(ss.str(), advertamt_id);

	if (antipregused < 0) antipregused = 0;
	ss.str("");	ss << "         You have: " << antipregnum << "\nUsed Last Turn: " << antipregused;
	EditTextItem(ss.str(), potionavail_id);
	DisableCheckBox(autopotions_id, antipregnum < 1);

	ss.str("");	ss << "Add Rooms: " << tariff.add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << endl;
	EditTextItem(ss.str(), roomcost_id);
	DisableButton(buyrooms_id, rooms >= maxrooms);

	/*	ss.str("");
	ss << "Bar Staff: " << tariff.bar_staff_wages() << " gold / week";
	EditTextItem(ss.str(), barstaff_id);

	ss.str("");
	ss << "Casino Staff: " << tariff.casino_staff_wages() << " gold / week";
	EditTextItem(ss.str(), casinostaff_id);
	*/

	HideButton(barhire_id, true);
	HideButton(casinohire_id, true);
	HideButton(barfire_id, true);
	HideButton(casinofire_id, true);
}

void cScreenBuildingSetup::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

void cScreenBuildingSetup::check_events()
{
	int buypotions = 0;
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	if (g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_Building = BUILDING_BROTHEL;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(buyrooms_id))
	{
		if (!g_Gold.brothel_cost(tariff.add_room_cost(5)))
		{
			ss.str("");
			ss << "You Need " << tariff.add_room_cost(5) << " gold to add 5 rooms.";
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		}
		else
		{
			int rooms = 20, maxrooms = 200;
			switch (g_Building)
			{
			case BUILDING_STUDIO:
				g_Studios.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_Studios.GetBrothel(0)->m_NumRooms;
				maxrooms = g_Studios.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_CLINIC:
				g_Clinic.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_Clinic.GetBrothel(0)->m_NumRooms;
				maxrooms = g_Clinic.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_ARENA:
				g_Arena.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_Arena.GetBrothel(0)->m_NumRooms;
				maxrooms = g_Arena.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_CENTRE:
				g_Centre.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_Centre.GetBrothel(0)->m_NumRooms;
				maxrooms = g_Centre.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_HOUSE:
				g_House.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_House.GetBrothel(0)->m_NumRooms;
				maxrooms = g_House.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_FARM:
				g_Farm.GetBrothel(0)->m_NumRooms += 5;
				rooms = g_Farm.GetBrothel(0)->m_NumRooms;
				maxrooms = g_Farm.GetBrothel(0)->m_MaxNumRooms;
				break;
			case BUILDING_BROTHEL:
			default:
				g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms += 5;
				rooms = g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms;
				maxrooms = g_Brothels.GetBrothel(g_CurrBrothel)->m_MaxNumRooms;
				break;
			}
			ss.str(""); ss << "Add Rooms: " << tariff.add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << endl;
			EditTextItem(ss.str(), roomcost_id);
			DisableButton(buyrooms_id, rooms >= maxrooms);
			g_InitWin = true;

		}
	}
	if (g_InterfaceEvents.CheckButton(potions10_id)) buypotions = 10;
	if (g_InterfaceEvents.CheckButton(potions20_id)) buypotions = 20;
	if (buypotions > 0)
	{
		int buynum = buypotions;
		int buysum = buynum;
		buypotions = 0;
		int MaxSupplies = 0, antipregnum = 0;
		if (!g_Gold.afford(tariff.anti_preg_price(buynum)))	g_MessageQue.AddToQue("You don't have enough gold", COLOR_RED);
		else
		{
			switch (g_Building)
			{
			case BUILDING_STUDIO:
				MaxSupplies = g_Studios.GetSupplyShedLevel() * 700;
				antipregnum = g_Studios.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Studios.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_CLINIC:
				MaxSupplies = g_Clinic.GetSupplyShedLevel() * 700;
				antipregnum = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Clinic.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_ARENA:
				MaxSupplies = g_Arena.GetSupplyShedLevel() * 700;
				antipregnum = g_Arena.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Arena.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_CENTRE:
				MaxSupplies = g_Centre.GetSupplyShedLevel() * 700;
				antipregnum = g_Centre.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Centre.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_HOUSE:
				MaxSupplies = g_House.GetSupplyShedLevel() * 700;
				antipregnum = g_House.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_House.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_FARM:
				MaxSupplies = g_Farm.GetSupplyShedLevel() * 700;
				antipregnum = g_Farm.GetBrothel(0)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Farm.GetBrothel(0)->m_AntiPregPotions += buysum;
				break;
			case BUILDING_BROTHEL:
			default:
				MaxSupplies = g_Brothels.GetSupplyShedLevel() * 700;
				antipregnum = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions;
				if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
				g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions += buysum;
				break;
			}
			if (buysum < buynum)
			{
				ss.str(""); ss << "You can only store up to " << MaxSupplies << " potions.";
				if (buysum > 0)
				{
					ss << "\nYou buy " << buysum << " to fill the stock.";
					g_Gold.item_cost(tariff.anti_preg_price(buysum));
				}
				g_MessageQue.AddToQue(ss.str(), 0);
			}
			else g_Gold.item_cost(tariff.anti_preg_price(buynum));
		}
		g_InitWin = true;
		return;
	}

	/*	if(g_InterfaceEvents.CheckButton(barhire_id))
	{
	g_Brothels.GetBrothel(g_CurrBrothel)->m_HasBarStaff = 1;
	g_InitWin = true;
	}
	if(g_InterfaceEvents.CheckButton(barfire_id))
	{
	g_Brothels.GetBrothel(g_CurrBrothel)->m_HasBarStaff = 0;
	g_InitWin = true;
	}
	if(g_InterfaceEvents.CheckButton(casinohire_id))
	{
	g_Brothels.GetBrothel(g_CurrBrothel)->m_HasGambStaff = 1;
	g_InitWin = true;
	}
	if(g_InterfaceEvents.CheckButton(casinofire_id))
	{
	g_Brothels.GetBrothel(g_CurrBrothel)->m_HasGambStaff = 0;
	g_InitWin = true;
	}	*/

	if (g_InterfaceEvents.CheckSlider(advertsli_id))
	{
		switch (g_Building)		// set advertising budget based on slider
		{
		case BUILDING_STUDIO:	g_Studios.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;	break;
		case BUILDING_CLINIC:	g_Clinic.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;	break;
		case BUILDING_ARENA:	g_Arena.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;	break;
		case BUILDING_CENTRE:	g_Centre.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;	break;
		case BUILDING_HOUSE:	g_House.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;	break;
		case BUILDING_FARM:		g_Farm.GetBrothel(0)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;		break;
		case BUILDING_BROTHEL:
		default:				g_Brothels.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = tariff.advertising_costs(SliderValue(advertsli_id) * 50);
			break;
		}
		ss.str(""); ss << "Advertising Budget: " << tariff.advertising_costs(SliderValue(advertsli_id) * 50) << " gold / week";
		EditTextItem(ss.str(), advertamt_id);
	}
	switch (g_Building)
	{
	case BUILDING_STUDIO:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Studios.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Studios.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Studios.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Studios.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Studios.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Studios.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Studios.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Studios.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Studios.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Studios.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Studios.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Studios.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_CLINIC:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Clinic.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Clinic.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Clinic.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Clinic.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Clinic.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Clinic.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Clinic.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Clinic.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Clinic.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Clinic.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Clinic.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Clinic.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_ARENA:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Arena.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Arena.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Arena.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Arena.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Arena.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Arena.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Arena.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Arena.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Arena.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Arena.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Arena.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Arena.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_CENTRE:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Centre.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Centre.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Centre.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Centre.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Centre.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Centre.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Centre.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Centre.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Centre.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Centre.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Centre.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Centre.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_HOUSE:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_House.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_House.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_House.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_House.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_House.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_House.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_House.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_House.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_House.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_House.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_House.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_House.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_FARM:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Farm.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Farm.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Farm.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Farm.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Farm.GetBrothel(0)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Farm.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Farm.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Farm.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Farm.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Farm.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Farm.GetBrothel(0)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Farm.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_BROTHEL:
	default:
		if (g_InterfaceEvents.CheckCheckbox(autopotions_id))	g_Brothels.GetBrothel(g_CurrBrothel)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if (g_InterfaceEvents.CheckCheckbox(noanal_id))			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if (g_InterfaceEvents.CheckCheckbox(nobdsm_id))			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if (g_InterfaceEvents.CheckCheckbox(nobeast_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if (g_InterfaceEvents.CheckCheckbox(nofoot_id))			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictFoot = IsCheckboxOn(nofoot_id);
		if (g_InterfaceEvents.CheckCheckbox(nogroup_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if (g_InterfaceEvents.CheckCheckbox(nohand_id))			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictHand = IsCheckboxOn(nohand_id);
		if (g_InterfaceEvents.CheckCheckbox(nolesbian_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if (g_InterfaceEvents.CheckCheckbox(nonormal_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if (g_InterfaceEvents.CheckCheckbox(nooral_id))			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if (g_InterfaceEvents.CheckCheckbox(nostrip_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictStrip = IsCheckboxOn(nostrip_id);
		if (g_InterfaceEvents.CheckCheckbox(notitty_id))		g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	}
}
