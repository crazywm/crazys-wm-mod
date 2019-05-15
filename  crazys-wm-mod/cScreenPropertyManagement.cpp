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
#include "cScreenPropertyManagement.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "libintl.h"
#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cClinic.h"
#include "cArena.h"
#include "cCentre.h"
#include "cHouse.h"
#include "cFarm.h"

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cMovieStudioManager g_Studios;
extern cClinicManager g_Clinic;
extern cArenaManager g_Arena;
extern cCentreManager g_Centre;
extern cHouseManager g_House;
extern cFarmManager g_Farm;
extern int g_CurrentScreen;
extern int g_Building;
extern int g_CurrCentre;
extern int g_CurrArena;
extern int g_CurrClinic;
extern int g_CurrHouse;
extern int g_CurrStudio;
extern int g_CurrFarm;


static cTariff tariff;
static stringstream ss;

bool cScreenPropertyManagement::ids_set = false;

cScreenPropertyManagement::cScreenPropertyManagement()
{

	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "property_management.xml";
	m_filename = dp.c_str();
}
cScreenPropertyManagement::~cScreenPropertyManagement() {}

void cScreenPropertyManagement::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenPropertyManagement");

	back_id = get_id("BackButton", "Back");
	barstaff_id     = get_id("BarStaffCost");  // not used anymore?
	barhire_id      = get_id("BarHireButton");  // not used anymore?
	barfire_id      = get_id("BarFireButton");  // not used anymore?
	casinostaff_id  = get_id("CasinoStaffCost");  // not used anymore?
	casinohire_id   = get_id("CasinoHireButton");  // not used anymore?
	casinofire_id   = get_id("CasinoFireButton");  // not used anymore?
	roomcost_id		= get_id("RoomAddCost");
	restrict_id		= get_id("SexRestrictions");

    load_brothel_ui_ids("P_B0", p_b0);
    load_brothel_ui_ids("P_B1", p_b1);
    load_brothel_ui_ids("P_B2", p_b2);
    load_brothel_ui_ids("P_B3", p_b3);
    load_brothel_ui_ids("P_B4", p_b4);
    load_brothel_ui_ids("P_B5", p_b5);
    load_brothel_ui_ids("P_B6", p_b6);
    load_brothel_ui_ids("P_St", p_st);
    load_brothel_ui_ids("P_Ar", p_ar);
    load_brothel_ui_ids("P_Ce", p_ce);
    load_brothel_ui_ids("P_Cl", p_cl);
    load_brothel_ui_ids("P_Ho", p_ho);
    load_brothel_ui_ids("P_Fa", p_fa);

	curbrothel_id	= get_id("CurrentBrothel");
	buyrooms_id		= get_id("BuildRoomsButton");
	potioncost_id	= get_id("PotionCost");
	potionavail_id	= get_id("AvailablePotions");
	potions10_id	= get_id("10PotionsButton");
	potions20_id	= get_id("20PotionsButton");
	autopotions_id	= get_id("AutoBuyPotionToggle");
	noanal_id		= get_id("ProhibitAnalToggleRes");
	nobdsm_id		= get_id("ProhibitBDSMToggleRes");
	nonormal_id		= get_id("ProhibitNormalToggleRes");
	nobeast_id		= get_id("ProhibitBeastToggleRes");
	nogroup_id		= get_id("ProhibitGroupToggleRes");
	nolesbian_id	= get_id("ProhibitLesbianToggleRes");
	nooral_id		= get_id("ProhibitOralToggleRes");
	notitty_id		= get_id("ProhibitTittyToggleRes");
	nohand_id		= get_id("ProhibitHandToggleRes");
	advertsli_id	= get_id("AdvertisingSlider");
	advertamt_id	= get_id("AdvertisingValue");
}

void cScreenPropertyManagement::load_brothel_ui_ids(const std::string& prefix, BrothelUiIDs& target) {
    target.id       = get_id(prefix + "_Name");
    target.addroom  = get_id(prefix + "_AddRoom");
    target.pot_c    = get_id(prefix + "_Pot_C");
    target.pot_a    = get_id(prefix + "_Pot_A");
    target.pot10    = get_id(prefix + "_Pot10");
    target.pot20    = get_id(prefix + "_Pot20");
    target.pot_b    = get_id(prefix + "_Pot_B");
    target.noanal   = get_id(prefix + "_NoAnal");
    target.nobdsm   = get_id(prefix + "_NoBDSM");
    target.nonorm   = get_id(prefix + "_NoNorm");
    target.nobeast  = get_id(prefix + "_NoBeast");
    target.noles    = get_id(prefix + "_NoLes");
    target.nooral   = get_id(prefix + "_NoOral");
    target.notitty  = get_id(prefix + "_NoTitty");
    target.nohand   = get_id(prefix + "_NoHand");
    target.advslid  = get_id(prefix + "_AdvSlid");
    target.advamnt  = get_id(prefix + "_AdvAmnt");
}

void cScreenPropertyManagement::init_building_ui(std::string title, const sBrothel& building, const BrothelUiIDs& ui)
{
    EditTextItem(std::move(title), ui.id);

    SetCheckBox(ui.noanal, building.m_RestrictAnal);
    SetCheckBox(ui.nobdsm, building.m_RestrictBDSM);
    SetCheckBox(ui.nonorm, building.m_RestrictNormal);
    SetCheckBox(ui.nobeast, building.m_RestrictBeast);
    SetCheckBox(ui.nogroup, building.m_RestrictGroup);
    SetCheckBox(ui.noles, building.m_RestrictLesbian);
    SetCheckBox(ui.nooral, building.m_RestrictOral);
    SetCheckBox(ui.notitty, building.m_RestrictTitty);
    SetCheckBox(ui.nohand, building.m_RestrictHand);

    int advert = building.m_AdvertisingBudget / 50;
    advert = SliderRange(ui.advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
    ss.str(""); ss << "Promotion Budget: " << (advert * 50) << (" gold / week");
    EditTextItem(ss.str(), ui.advamnt);

    SetCheckBox(ui.pot_b, building.m_KeepPotionsStocked);
    ss.str("");
    int number = building.m_AntiPregPotions;
    ss << ("You have: ") << number;
    EditTextItem(ss.str(), ui.pot_a);
    DisableCheckBox(ui.pot_b, number < 1);
}

void cScreenPropertyManagement::check_building_ui_events(sBrothel& building, const BrothelUiIDs& ui) {
    if (g_InterfaceEvents.CheckCheckbox(ui.pot_b))	building.m_KeepPotionsStocked = IsCheckboxOn(ui.pot_b);

    if (g_InterfaceEvents.CheckCheckbox(ui.noanal))	building.m_RestrictAnal = IsCheckboxOn(ui.noanal);
    if (g_InterfaceEvents.CheckCheckbox(ui.nobdsm))	building.m_RestrictBDSM = IsCheckboxOn(ui.nobdsm);
    if (g_InterfaceEvents.CheckCheckbox(ui.nonorm))	building.m_RestrictNormal = IsCheckboxOn(ui.nonorm);
    if (g_InterfaceEvents.CheckCheckbox(ui.nobeast))	building.m_RestrictBeast = IsCheckboxOn(ui.nobeast);
    if (g_InterfaceEvents.CheckCheckbox(ui.nogroup))	building.m_RestrictGroup = IsCheckboxOn(ui.nogroup);
    if (g_InterfaceEvents.CheckCheckbox(ui.noles))	building.m_RestrictLesbian = IsCheckboxOn(ui.noles);
    if (g_InterfaceEvents.CheckCheckbox(ui.nooral))	building.m_RestrictOral = IsCheckboxOn(ui.nooral);
    if (g_InterfaceEvents.CheckCheckbox(ui.notitty))	building.m_RestrictTitty = IsCheckboxOn(ui.notitty);
    if (g_InterfaceEvents.CheckCheckbox(ui.nohand))	building.m_RestrictHand = IsCheckboxOn(ui.nohand);

    if (g_InterfaceEvents.CheckSlider(ui.advslid))
    {
        building.m_AdvertisingBudget = SliderValue(ui.advslid) * 50;
        ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(ui.advslid) * 50) << (" gold / week");
        EditTextItem(ss.str(), ui.advamnt);
    }
}

void cScreenPropertyManagement::init()
{
	g_CurrentScreen = SCREEN_PROPERTYMANAGEMENT;
	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;

	ss.str(""); ss << "Add Rooms: " << tariff.add_room_cost(5) << " gold";
	EditTextItem(ss.str(), roomcost_id);

	ss.str(""); ss << "Anti-Preg Potions: " << tariff.anti_preg_price(1) << " gold each";
	EditTextItem(ss.str(), potioncost_id);

	string brothel = "";
	string message = "";
	int number = 0;
	int advert = 0;

	if (true) // Always include the house
	{
	    init_building_ui("Your House", *g_House.GetBrothel(0), p_ho);
	}


	if (g_Brothels.GetNumBrothels() > 0)
	{
        init_building_ui(g_Brothels.GetName(0), *g_Brothels.GetBrothel(0), p_b0);
	}
	else
	{
		EditTextItem("(You do not own this brothel yet.)", p_b0.id);
	}

	if (g_Brothels.GetNumBrothels() > 1)
	{
        init_building_ui(g_Brothels.GetName(1), *g_Brothels.GetBrothel(1), p_b1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b1.id);
	}

	if (g_Brothels.GetNumBrothels() > 2)
	{
        init_building_ui(g_Brothels.GetName(2), *g_Brothels.GetBrothel(2), p_b2);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b2.id);
	}

	if (g_Brothels.GetNumBrothels() > 3)
	{
        init_building_ui(g_Brothels.GetName(3), *g_Brothels.GetBrothel(3), p_b3);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b3.id);
	}

	if (g_Brothels.GetNumBrothels() > 4)
	{
        init_building_ui(g_Brothels.GetName(4), *g_Brothels.GetBrothel(4), p_b4);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b4.id);
	}

	if (g_Brothels.GetNumBrothels() > 5)
	{
        init_building_ui(g_Brothels.GetName(5), *g_Brothels.GetBrothel(5), p_b5);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b5.id);
	}

	if (g_Brothels.GetNumBrothels() > 6)
	{
        init_building_ui(g_Brothels.GetName(6), *g_Brothels.GetBrothel(6), p_b6);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b6.id);
	}

	if (g_Studios.GetNumBrothels() > 0)
	{
        init_building_ui("The Movie Studio", *g_Studios.GetBrothel(0), p_st);
	}
	else
	{
		EditTextItem(("(You do not own the Studio yet.)"), p_st.id);
	}

	if (g_Arena.GetNumBrothels() > 0)
	{
        init_building_ui("The Arena", *g_Arena.GetBrothel(0), p_ar);
	}
	else
	{
		EditTextItem(("(You do not own the Arena yet.)"), p_ar.id);
	}

	if (g_Centre.GetNumBrothels() > 0)
	{
        init_building_ui("The Centre", *g_Centre.GetBrothel(0), p_ce);
	}
	else
	{
		EditTextItem(("(You do not own the Centre yet.)"), p_ce.id);
	}

	if (g_Clinic.GetNumBrothels() > 0)
	{
        init_building_ui("The Clinic", *g_Clinic.GetBrothel(0), p_cl);
	}
	else
	{
		EditTextItem(("(You do not own the Clinic yet.)"), p_cl.id);
	}

	if (g_Farm.GetNumBrothels() > 0)
	{
        init_building_ui("The Farm", *g_Farm.GetBrothel(0), p_fa);
	}
	else
	{
		EditTextItem(("(You do not own the Farm yet.)"), p_fa.id);
	}

}

void cScreenPropertyManagement::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set

	init();	// set up the window if needed

	check_events();	// check to see if there's a button event needing handling
}


void cScreenPropertyManagement::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	// if it's the back button, pop the window off the stack and we're done
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_Building = BUILDING_BROTHEL;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

	if (true) // Always include the house
	{
        check_building_ui_events(*g_House.GetBrothel(0), p_ho);
	}

	if (g_Brothels.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(0), p_b0);
	}

	if (g_Brothels.GetNumBrothels() > 1)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(1), p_b1);
	}
	if (g_Brothels.GetNumBrothels() > 2)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(2), p_b2);
	}
	if (g_Brothels.GetNumBrothels() > 3)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(3), p_b3);
	}
	if (g_Brothels.GetNumBrothels() > 4)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(4), p_b4);
	}
	if (g_Brothels.GetNumBrothels() > 5)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(5), p_b5);
	}
	if (g_Brothels.GetNumBrothels() > 6)
	{
        check_building_ui_events(*g_Brothels.GetBrothel(6), p_b6);
	}
	if (g_Studios.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Studios.GetBrothel(0), p_st);
	}
	if (g_Arena.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Arena.GetBrothel(0), p_ar);
	}
	if (g_Centre.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Centre.GetBrothel(0), p_ce);
	}
	if (g_Clinic.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Clinic.GetBrothel(0), p_cl);
	}
	if (g_Farm.GetNumBrothels() > 0)
	{
        check_building_ui_events(*g_Farm.GetBrothel(0), p_fa);
	}
}
