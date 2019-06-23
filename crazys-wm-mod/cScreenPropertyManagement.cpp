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

	curbrothel_id	= get_id("CurrentBrothel");
	p_b0_id			= get_id("P_B0_Name");
	p_b1_id			= get_id("P_B1_Name");
	p_b2_id			= get_id("P_B2_Name");
	p_b3_id			= get_id("P_B3_Name");
	p_b4_id			= get_id("P_B4_Name");
	p_b5_id			= get_id("P_B5_Name");
	p_b6_id			= get_id("P_B6_Name");
	p_st_id			= get_id("P_St_Name");
	p_ar_id			= get_id("P_Ar_Name");
	p_ce_id			= get_id("P_Ce_Name");
	p_cl_id			= get_id("P_Cl_Name");
	p_ho_id			= get_id("P_Ho_Name");
	p_fa_id			= get_id("P_Fa_Name");

	buyrooms_id		= get_id("BuildRoomsButton");
	p_b0_addroom	= get_id("P_B0_AddRoom");
	p_b1_addroom	= get_id("P_B1_AddRoom");
	p_b2_addroom	= get_id("P_B2_AddRoom");
	p_b3_addroom	= get_id("P_B3_AddRoom");
	p_b4_addroom	= get_id("P_B4_AddRoom");
	p_b5_addroom	= get_id("P_B5_AddRoom");
	p_b6_addroom	= get_id("P_B6_AddRoom");
	p_st_addroom	= get_id("P_St_AddRoom");
	p_ar_addroom	= get_id("P_Ar_AddRoom");
	p_ce_addroom	= get_id("P_Ce_AddRoom");
	p_cl_addroom	= get_id("P_Cl_AddRoom");
	p_ho_addroom	= get_id("P_Ho_AddRoom");
	p_fa_addroom	= get_id("P_Fa_AddRoom");

	potioncost_id	= get_id("PotionCost");
	p_b0_pot_c		= get_id("P_B0_Pot_C");
	p_b1_pot_c		= get_id("P_B1_Pot_C");
	p_b2_pot_c		= get_id("P_B2_Pot_C");
	p_b3_pot_c		= get_id("P_B3_Pot_C");
	p_b4_pot_c		= get_id("P_B4_Pot_C");
	p_b5_pot_c		= get_id("P_B5_Pot_C");
	p_b6_pot_c		= get_id("P_B6_Pot_C");
	p_st_pot_c		= get_id("P_St_Pot_C");
	p_ar_pot_c		= get_id("P_Ar_Pot_C");
	p_ce_pot_c		= get_id("P_Ce_Pot_C");
	p_cl_pot_c		= get_id("P_Cl_Pot_C");
	p_ho_pot_c		= get_id("P_Ho_Pot_C");
	p_fa_pot_c		= get_id("P_Fa_Pot_C");

	potionavail_id	= get_id("AvailablePotions");
	p_b0_pot_a		= get_id("P_B0_Pot_A");
	p_b1_pot_a		= get_id("P_B1_Pot_A");
	p_b2_pot_a		= get_id("P_B2_Pot_A");
	p_b3_pot_a		= get_id("P_B3_Pot_A");
	p_b4_pot_a		= get_id("P_B4_Pot_A");
	p_b5_pot_a		= get_id("P_B5_Pot_A");
	p_b6_pot_a		= get_id("P_B6_Pot_A");
	p_st_pot_a		= get_id("P_St_Pot_A");
	p_ar_pot_a		= get_id("P_Ar_Pot_A");
	p_ce_pot_a		= get_id("P_Ce_Pot_A");
	p_cl_pot_a		= get_id("P_Cl_Pot_A");
	p_ho_pot_a		= get_id("P_Ho_Pot_A");
	p_fa_pot_a		= get_id("P_Fa_Pot_A");

	potions10_id	= get_id("10PotionsButton");
	p_b0_pot10		= get_id("P_B0_Pot10");
	p_b1_pot10		= get_id("P_B1_Pot10");
	p_b2_pot10		= get_id("P_B2_Pot10");
	p_b3_pot10		= get_id("P_B3_Pot10");
	p_b4_pot10		= get_id("P_B4_Pot10");
	p_b5_pot10		= get_id("P_B5_Pot10");
	p_b6_pot10		= get_id("P_B6_Pot10");
	p_st_pot10		= get_id("P_St_Pot10");
	p_ar_pot10		= get_id("P_Ar_Pot10");
	p_ce_pot10		= get_id("P_Ce_Pot10");
	p_cl_pot10		= get_id("P_Cl_Pot10");
	p_ho_pot10		= get_id("P_Ho_Pot10");
	p_fa_pot10		= get_id("P_Fa_Pot10");

	potions20_id	= get_id("20PotionsButton");
	p_b0_pot20		= get_id("P_B0_Pot20");
	p_b1_pot20		= get_id("P_B1_Pot20");
	p_b2_pot20		= get_id("P_B2_Pot20");
	p_b3_pot20		= get_id("P_B3_Pot20");
	p_b4_pot20		= get_id("P_B4_Pot20");
	p_b5_pot20		= get_id("P_B5_Pot20");
	p_b6_pot20		= get_id("P_B6_Pot20");
	p_st_pot20		= get_id("P_St_Pot20");
	p_ar_pot20		= get_id("P_Ar_Pot20");
	p_ce_pot20		= get_id("P_Ce_Pot20");
	p_cl_pot20		= get_id("P_Cl_Pot20");
	p_ho_pot20		= get_id("P_Ho_Pot20");
	p_fa_pot20		= get_id("P_Fa_Pot20");

	autopotions_id	= get_id("AutoBuyPotionToggle");
	p_b0_pot_b		= get_id("P_B0_Pot_B");
	p_b1_pot_b		= get_id("P_B1_Pot_B");
	p_b2_pot_b		= get_id("P_B2_Pot_B");
	p_b3_pot_b		= get_id("P_B3_Pot_B");
	p_b4_pot_b		= get_id("P_B4_Pot_B");
	p_b5_pot_b		= get_id("P_B5_Pot_B");
	p_b6_pot_b		= get_id("P_B6_Pot_B");
	p_st_pot_b		= get_id("P_St_Pot_B");
	p_ar_pot_b		= get_id("P_Ar_Pot_B");
	p_ce_pot_b		= get_id("P_Ce_Pot_B");
	p_cl_pot_b		= get_id("P_Cl_Pot_B");
	p_ho_pot_b		= get_id("P_Ho_Pot_B");
	p_fa_pot_b		= get_id("P_Fa_Pot_B");

	noanal_id		= get_id("ProhibitAnalToggleRes");
	p_b0_noanal		= get_id("P_B0_NoAnal");
	p_b1_noanal		= get_id("P_B1_NoAnal");
	p_b2_noanal		= get_id("P_B2_NoAnal");
	p_b3_noanal		= get_id("P_B3_NoAnal");
	p_b4_noanal		= get_id("P_B4_NoAnal");
	p_b5_noanal		= get_id("P_B5_NoAnal");
	p_b6_noanal		= get_id("P_B6_NoAnal");
	p_st_noanal		= get_id("P_St_NoAnal");
	p_ar_noanal		= get_id("P_Ar_NoAnal");
	p_ce_noanal		= get_id("P_Ce_NoAnal");
	p_cl_noanal		= get_id("P_Cl_NoAnal");
	p_ho_noanal		= get_id("P_Ho_NoAnal");
	p_fa_noanal		= get_id("P_Fa_NoAnal");

	nobdsm_id		= get_id("ProhibitBDSMToggleRes");
	p_b0_nobdsm		= get_id("P_B0_NoBDSM");
	p_b1_nobdsm		= get_id("P_B1_NoBDSM");
	p_b2_nobdsm		= get_id("P_B2_NoBDSM");
	p_b3_nobdsm		= get_id("P_B3_NoBDSM");
	p_b4_nobdsm		= get_id("P_B4_NoBDSM");
	p_b5_nobdsm		= get_id("P_B5_NoBDSM");
	p_b6_nobdsm		= get_id("P_B6_NoBDSM");
	p_st_nobdsm		= get_id("P_St_NoBDSM");
	p_ar_nobdsm		= get_id("P_Ar_NoBDSM");
	p_ce_nobdsm		= get_id("P_Ce_NoBDSM");
	p_cl_nobdsm		= get_id("P_Cl_NoBDSM");
	p_ho_nobdsm		= get_id("P_Ho_NoBDSM");
	p_fa_nobdsm		= get_id("P_Fa_NoBDSM");

	nonormal_id		= get_id("ProhibitNormalToggleRes");
	p_b0_nonorm		= get_id("P_B0_NoNorm");
	p_b1_nonorm		= get_id("P_B1_NoNorm");
	p_b2_nonorm		= get_id("P_B2_NoNorm");
	p_b3_nonorm		= get_id("P_B3_NoNorm");
	p_b4_nonorm		= get_id("P_B4_NoNorm");
	p_b5_nonorm		= get_id("P_B5_NoNorm");
	p_b6_nonorm		= get_id("P_B6_NoNorm");
	p_st_nonorm		= get_id("P_St_NoNorm");
	p_ar_nonorm		= get_id("P_Ar_NoNorm");
	p_ce_nonorm		= get_id("P_Ce_NoNorm");
	p_cl_nonorm		= get_id("P_Cl_NoNorm");
	p_ho_nonorm		= get_id("P_Ho_NoNorm");
	p_fa_nonorm		= get_id("P_Fa_NoNorm");

	nobeast_id		= get_id("ProhibitBeastToggleRes");
	p_b0_nobeast	= get_id("P_B0_NoBeast");
	p_b1_nobeast	= get_id("P_B1_NoBeast");
	p_b2_nobeast	= get_id("P_B2_NoBeast");
	p_b3_nobeast	= get_id("P_B3_NoBeast");
	p_b4_nobeast	= get_id("P_B4_NoBeast");
	p_b5_nobeast	= get_id("P_B5_NoBeast");
	p_b6_nobeast	= get_id("P_B6_NoBeast");
	p_st_nobeast	= get_id("P_St_NoBeast");
	p_ar_nobeast	= get_id("P_Ar_NoBeast");
	p_ce_nobeast	= get_id("P_Ce_NoBeast");
	p_cl_nobeast	= get_id("P_Cl_NoBeast");
	p_ho_nobeast	= get_id("P_Ho_NoBeast");
	p_fa_nobeast	= get_id("P_Fa_NoBeast");

	nogroup_id		= get_id("ProhibitGroupToggleRes");
	p_b0_nogroup	= get_id("P_B0_NoGroup");
	p_b1_nogroup	= get_id("P_B1_NoGroup");
	p_b2_nogroup	= get_id("P_B2_NoGroup");
	p_b3_nogroup	= get_id("P_B3_NoGroup");
	p_b4_nogroup	= get_id("P_B4_NoGroup");
	p_b5_nogroup	= get_id("P_B5_NoGroup");
	p_b6_nogroup	= get_id("P_B6_NoGroup");
	p_st_nogroup	= get_id("P_St_NoGroup");
	p_ar_nogroup	= get_id("P_Ar_NoGroup");
	p_ce_nogroup	= get_id("P_Ce_NoGroup");
	p_cl_nogroup	= get_id("P_Cl_NoGroup");
	p_ho_nogroup	= get_id("P_Ho_NoGroup");
	p_fa_nogroup	= get_id("P_Fa_NoGroup");

	nolesbian_id	= get_id("ProhibitLesbianToggleRes");
	p_b0_noles		= get_id("P_B0_NoLes");
	p_b1_noles		= get_id("P_B1_NoLes");
	p_b2_noles		= get_id("P_B2_NoLes");
	p_b3_noles		= get_id("P_B3_NoLes");
	p_b4_noles		= get_id("P_B4_NoLes");
	p_b5_noles		= get_id("P_B5_NoLes");
	p_b6_noles		= get_id("P_B6_NoLes");
	p_st_noles		= get_id("P_St_NoLes");
	p_ar_noles		= get_id("P_Ar_NoLes");
	p_ce_noles		= get_id("P_Ce_NoLes");
	p_cl_noles		= get_id("P_Cl_NoLes");
	p_ho_noles		= get_id("P_Ho_NoLes");
	p_fa_noles		= get_id("P_Fa_NoLes");

	nooral_id		= get_id("ProhibitOralToggleRes");
	p_b0_nooral		= get_id("P_B0_NoOral");
	p_b1_nooral		= get_id("P_B1_NoOral");
	p_b2_nooral		= get_id("P_B2_NoOral");
	p_b3_nooral		= get_id("P_B3_NoOral");
	p_b4_nooral		= get_id("P_B4_NoOral");
	p_b5_nooral		= get_id("P_B5_NoOral");
	p_b6_nooral		= get_id("P_B6_NoOral");
	p_st_nooral		= get_id("P_St_NoOral");
	p_ar_nooral		= get_id("P_Ar_NoOral");
	p_ce_nooral		= get_id("P_Ce_NoOral");
	p_cl_nooral		= get_id("P_Cl_NoOral");
	p_ho_nooral		= get_id("P_Ho_NoOral");
	p_fa_nooral		= get_id("P_Fa_NoOral");

	notitty_id		= get_id("ProhibitTittyToggleRes");
	p_b0_notitty	= get_id("P_B0_NoTitty");
	p_b1_notitty	= get_id("P_B1_NoTitty");
	p_b2_notitty	= get_id("P_B2_NoTitty");
	p_b3_notitty	= get_id("P_B3_NoTitty");
	p_b4_notitty	= get_id("P_B4_NoTitty");
	p_b5_notitty	= get_id("P_B5_NoTitty");
	p_b6_notitty	= get_id("P_B6_NoTitty");
	p_st_notitty	= get_id("P_St_NoTitty");
	p_ar_notitty	= get_id("P_Ar_NoTitty");
	p_ce_notitty	= get_id("P_Ce_NoTitty");
	p_cl_notitty	= get_id("P_Cl_NoTitty");
	p_ho_notitty	= get_id("P_Ho_NoTitty");
	p_fa_notitty	= get_id("P_Fa_NoTitty");

	nohand_id		= get_id("ProhibitHandToggleRes");
	p_b0_nohand		= get_id("P_B0_NoHand");
	p_b1_nohand		= get_id("P_B1_NoHand");
	p_b2_nohand		= get_id("P_B2_NoHand");
	p_b3_nohand		= get_id("P_B3_NoHand");
	p_b4_nohand		= get_id("P_B4_NoHand");
	p_b5_nohand		= get_id("P_B5_NoHand");
	p_b6_nohand		= get_id("P_B6_NoHand");
	p_st_nohand		= get_id("P_St_NoHand");
	p_ar_nohand		= get_id("P_Ar_NoHand");
	p_ce_nohand		= get_id("P_Ce_NoHand");
	p_cl_nohand		= get_id("P_Cl_NoHand");
	p_ho_nohand		= get_id("P_Ho_NoHand");
	p_fa_nohand		= get_id("P_Fa_NoHand");

	advertsli_id	= get_id("AdvertisingSlider");
	p_b0_advslid	= get_id("P_B0_AdvSlid");
	p_b1_advslid	= get_id("P_B1_AdvSlid");
	p_b2_advslid	= get_id("P_B2_AdvSlid");
	p_b3_advslid	= get_id("P_B3_AdvSlid");
	p_b4_advslid	= get_id("P_B4_AdvSlid");
	p_b5_advslid	= get_id("P_B5_AdvSlid");
	p_b6_advslid	= get_id("P_B6_AdvSlid");
	p_st_advslid	= get_id("P_St_AdvSlid");
	p_ar_advslid	= get_id("P_Ar_AdvSlid");
	p_ce_advslid	= get_id("P_Ce_AdvSlid");
	p_cl_advslid	= get_id("P_Cl_AdvSlid");
	p_ho_advslid	= get_id("P_Ho_AdvSlid");
	p_fa_advslid	= get_id("P_Fa_AdvSlid");

	advertamt_id	= get_id("AdvertisingValue");
	p_b0_advamnt	= get_id("P_B0_AdvAmnt");
	p_b1_advamnt	= get_id("P_B1_AdvAmnt");
	p_b2_advamnt	= get_id("P_B2_AdvAmnt");
	p_b3_advamnt	= get_id("P_B3_AdvAmnt");
	p_b4_advamnt	= get_id("P_B4_AdvAmnt");
	p_b5_advamnt	= get_id("P_B5_AdvAmnt");
	p_b6_advamnt	= get_id("P_B6_AdvAmnt");
	p_st_advamnt	= get_id("P_St_AdvAmnt");
	p_ar_advamnt	= get_id("P_Ar_AdvAmnt");
	p_ce_advamnt	= get_id("P_Ce_AdvAmnt");
	p_cl_advamnt	= get_id("P_Cl_AdvAmnt");
	p_ho_advamnt	= get_id("P_Ho_AdvAmnt");
	p_fa_advamnt	= get_id("P_Fa_AdvAmnt");

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
		EditTextItem("Your House", p_ho_id);

		SetCheckBox(p_ho_noanal, g_House.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_ho_nobdsm, g_House.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_ho_nonorm, g_House.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_ho_nobeast, g_House.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_ho_nogroup, g_House.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_ho_noles, g_House.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_ho_nooral, g_House.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_ho_notitty, g_House.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_ho_nohand, g_House.GetBrothel(0)->m_RestrictHand);

		advert = g_House.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_ho_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << "Promotion Budget: " << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_ho_advamnt);

		SetCheckBox(p_ho_pot_b, g_House.GetBrothel(0)->m_KeepPotionsStocked);
		ss.str(""); number = g_House.GetBrothel(0)->m_AntiPregPotions;
		ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_ho_pot_a);
		DisableCheckBox(p_ho_pot_b, number < 1);
	}


	if (g_Brothels.GetNumBrothels() > 0)
	{
		EditTextItem(g_Brothels.GetName(0), p_b0_id);

		SetCheckBox(p_b0_noanal, g_Brothels.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_b0_nobdsm, g_Brothels.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_b0_nonorm, g_Brothels.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_b0_nobeast, g_Brothels.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_b0_nogroup, g_Brothels.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_b0_noles, g_Brothels.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_b0_nooral, g_Brothels.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_b0_notitty, g_Brothels.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_b0_nohand, g_Brothels.GetBrothel(0)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b0_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b0_advamnt);

		SetCheckBox(p_b0_pot_b, g_Brothels.GetBrothel(0)->m_KeepPotionsStocked);
		ss.str(""); number = g_Brothels.GetBrothel(0)->m_AntiPregPotions;
		ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b0_pot_a);
		DisableCheckBox(p_b0_pot_b, number < 1);
	}
	else
	{
		EditTextItem("(You do not own this brothel yet.)", p_b0_id);
	}

	if (g_Brothels.GetNumBrothels() > 1)
	{
		EditTextItem(g_Brothels.GetName(1), p_b1_id);

		SetCheckBox(p_b1_noanal, g_Brothels.GetBrothel(1)->m_RestrictAnal);
		SetCheckBox(p_b1_nobdsm, g_Brothels.GetBrothel(1)->m_RestrictBDSM);
		SetCheckBox(p_b1_nonorm, g_Brothels.GetBrothel(1)->m_RestrictNormal);
		SetCheckBox(p_b1_nobeast, g_Brothels.GetBrothel(1)->m_RestrictBeast);
		SetCheckBox(p_b1_nogroup, g_Brothels.GetBrothel(1)->m_RestrictGroup);
		SetCheckBox(p_b1_noles, g_Brothels.GetBrothel(1)->m_RestrictLesbian);
		SetCheckBox(p_b1_nooral, g_Brothels.GetBrothel(1)->m_RestrictOral);
		SetCheckBox(p_b1_notitty, g_Brothels.GetBrothel(1)->m_RestrictTitty);
		SetCheckBox(p_b1_nohand, g_Brothels.GetBrothel(1)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(1)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b1_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b1_advamnt);

		SetCheckBox(p_b1_pot_b, g_Brothels.GetBrothel(1)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(1)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b1_pot_a);
		DisableCheckBox(p_b1_pot_b, number < 1);

	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b1_id);
	}

	if (g_Brothels.GetNumBrothels() > 2)
	{
		EditTextItem(g_Brothels.GetName(2), p_b2_id);

		SetCheckBox(p_b2_noanal, g_Brothels.GetBrothel(2)->m_RestrictAnal);
		SetCheckBox(p_b2_nobdsm, g_Brothels.GetBrothel(2)->m_RestrictBDSM);
		SetCheckBox(p_b2_nonorm, g_Brothels.GetBrothel(2)->m_RestrictNormal);
		SetCheckBox(p_b2_nobeast, g_Brothels.GetBrothel(2)->m_RestrictBeast);
		SetCheckBox(p_b2_nogroup, g_Brothels.GetBrothel(2)->m_RestrictGroup);
		SetCheckBox(p_b2_noles, g_Brothels.GetBrothel(2)->m_RestrictLesbian);
		SetCheckBox(p_b2_nooral, g_Brothels.GetBrothel(2)->m_RestrictOral);
		SetCheckBox(p_b2_notitty, g_Brothels.GetBrothel(2)->m_RestrictTitty);
		SetCheckBox(p_b2_nohand, g_Brothels.GetBrothel(2)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(2)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b2_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b2_advamnt);

		SetCheckBox(p_b2_pot_b, g_Brothels.GetBrothel(2)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(2)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b2_pot_a);
		DisableCheckBox(p_b2_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b2_id);
	}

	if (g_Brothels.GetNumBrothels() > 3)
	{
		EditTextItem(g_Brothels.GetName(3), p_b3_id);

		SetCheckBox(p_b3_noanal, g_Brothels.GetBrothel(3)->m_RestrictAnal);
		SetCheckBox(p_b3_nobdsm, g_Brothels.GetBrothel(3)->m_RestrictBDSM);
		SetCheckBox(p_b3_nonorm, g_Brothels.GetBrothel(3)->m_RestrictNormal);
		SetCheckBox(p_b3_nobeast, g_Brothels.GetBrothel(3)->m_RestrictBeast);
		SetCheckBox(p_b3_nogroup, g_Brothels.GetBrothel(3)->m_RestrictGroup);
		SetCheckBox(p_b3_noles, g_Brothels.GetBrothel(3)->m_RestrictLesbian);
		SetCheckBox(p_b3_nooral, g_Brothels.GetBrothel(3)->m_RestrictOral);
		SetCheckBox(p_b3_notitty, g_Brothels.GetBrothel(3)->m_RestrictTitty);
		SetCheckBox(p_b3_nohand, g_Brothels.GetBrothel(3)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(3)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b3_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b3_advamnt);

		SetCheckBox(p_b3_pot_b, g_Brothels.GetBrothel(3)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(3)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b3_pot_a);
		DisableCheckBox(p_b3_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b3_id);
	}

	if (g_Brothels.GetNumBrothels() > 4)
	{
		EditTextItem(g_Brothels.GetName(4), p_b4_id);

		SetCheckBox(p_b4_noanal, g_Brothels.GetBrothel(4)->m_RestrictAnal);
		SetCheckBox(p_b4_nobdsm, g_Brothels.GetBrothel(4)->m_RestrictBDSM);
		SetCheckBox(p_b4_nonorm, g_Brothels.GetBrothel(4)->m_RestrictNormal);
		SetCheckBox(p_b4_nobeast, g_Brothels.GetBrothel(4)->m_RestrictBeast);
		SetCheckBox(p_b4_nogroup, g_Brothels.GetBrothel(4)->m_RestrictGroup);
		SetCheckBox(p_b4_noles, g_Brothels.GetBrothel(4)->m_RestrictLesbian);
		SetCheckBox(p_b4_nooral, g_Brothels.GetBrothel(4)->m_RestrictOral);
		SetCheckBox(p_b4_notitty, g_Brothels.GetBrothel(4)->m_RestrictTitty);
		SetCheckBox(p_b4_nohand, g_Brothels.GetBrothel(4)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(4)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b4_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b4_advamnt);

		SetCheckBox(p_b4_pot_b, g_Brothels.GetBrothel(4)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(4)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b4_pot_a);
		DisableCheckBox(p_b4_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b4_id);
	}

	if (g_Brothels.GetNumBrothels() > 5)
	{
		EditTextItem(g_Brothels.GetName(5), p_b5_id);

		SetCheckBox(p_b5_noanal, g_Brothels.GetBrothel(5)->m_RestrictAnal);
		SetCheckBox(p_b5_nobdsm, g_Brothels.GetBrothel(5)->m_RestrictBDSM);
		SetCheckBox(p_b5_nonorm, g_Brothels.GetBrothel(5)->m_RestrictNormal);
		SetCheckBox(p_b5_nobeast, g_Brothels.GetBrothel(5)->m_RestrictBeast);
		SetCheckBox(p_b5_nogroup, g_Brothels.GetBrothel(5)->m_RestrictGroup);
		SetCheckBox(p_b5_noles, g_Brothels.GetBrothel(5)->m_RestrictLesbian);
		SetCheckBox(p_b5_nooral, g_Brothels.GetBrothel(5)->m_RestrictOral);
		SetCheckBox(p_b5_notitty, g_Brothels.GetBrothel(5)->m_RestrictTitty);
		SetCheckBox(p_b5_nohand, g_Brothels.GetBrothel(5)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(5)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b5_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b5_advamnt);
		SetCheckBox(p_b5_pot_b, g_Brothels.GetBrothel(5)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(5)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b5_pot_a);
		DisableCheckBox(p_b5_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b5_id);
	}

	if (g_Brothels.GetNumBrothels() > 6)
	{
		EditTextItem(g_Brothels.GetName(6), p_b6_id);

		SetCheckBox(p_b6_noanal, g_Brothels.GetBrothel(6)->m_RestrictAnal);
		SetCheckBox(p_b6_nobdsm, g_Brothels.GetBrothel(6)->m_RestrictBDSM);
		SetCheckBox(p_b6_nonorm, g_Brothels.GetBrothel(6)->m_RestrictNormal);
		SetCheckBox(p_b6_nobeast, g_Brothels.GetBrothel(6)->m_RestrictBeast);
		SetCheckBox(p_b6_nogroup, g_Brothels.GetBrothel(6)->m_RestrictGroup);
		SetCheckBox(p_b6_noles, g_Brothels.GetBrothel(6)->m_RestrictLesbian);
		SetCheckBox(p_b6_nooral, g_Brothels.GetBrothel(6)->m_RestrictOral);
		SetCheckBox(p_b6_notitty, g_Brothels.GetBrothel(6)->m_RestrictTitty);
		SetCheckBox(p_b6_nohand, g_Brothels.GetBrothel(6)->m_RestrictHand);

		advert = g_Brothels.GetBrothel(6)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_b6_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Advertising Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_b6_advamnt);

		SetCheckBox(p_b6_pot_b, g_Brothels.GetBrothel(6)->m_KeepPotionsStocked);
		number = g_Brothels.GetBrothel(6)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_b6_pot_a);
		DisableCheckBox(p_b6_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own this brothel yet.)"), p_b6_id);
	}

	if (g_Studios.GetNumBrothels() > 0)
	{
		EditTextItem(("The Movie Studio"), p_st_id);

		SetCheckBox(p_st_noanal, g_Studios.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_st_nobdsm, g_Studios.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_st_nonorm, g_Studios.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_st_nobeast, g_Studios.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_st_nogroup, g_Studios.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_st_noles, g_Studios.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_st_nooral, g_Studios.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_st_notitty, g_Studios.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_st_nohand, g_Studios.GetBrothel(0)->m_RestrictHand);

		advert = g_Studios.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_st_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Promotion Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_st_advamnt);

		SetCheckBox(p_st_pot_b, g_Studios.GetBrothel(0)->m_KeepPotionsStocked);
		number = g_Studios.GetBrothel(0)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_st_pot_a);
		DisableCheckBox(p_st_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own the Studio yet.)"), p_st_id);
	}

	if (g_Arena.GetNumBrothels() > 0)
	{
		EditTextItem(("The Arena"), p_ar_id);

		SetCheckBox(p_ar_noanal, g_Arena.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_ar_nobdsm, g_Arena.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_ar_nonorm, g_Arena.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_ar_nobeast, g_Arena.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_ar_nogroup, g_Arena.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_ar_noles, g_Arena.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_ar_nooral, g_Arena.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_ar_notitty, g_Arena.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_ar_nohand, g_Arena.GetBrothel(0)->m_RestrictHand);

		advert = g_Arena.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_ar_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Promotion Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_ar_advamnt);

		SetCheckBox(p_ar_pot_b, g_Arena.GetBrothel(0)->m_KeepPotionsStocked);
		number = g_Arena.GetBrothel(0)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_ar_pot_a);
		DisableCheckBox(p_ar_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own the Arena yet.)"), p_ar_id);
	}

	if (g_Centre.GetNumBrothels() > 0)
	{
		EditTextItem(("The Centre"), p_ce_id);

		SetCheckBox(p_ce_noanal, g_Centre.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_ce_nobdsm, g_Centre.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_ce_nonorm, g_Centre.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_ce_nobeast, g_Centre.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_ce_nogroup, g_Centre.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_ce_noles, g_Centre.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_ce_nooral, g_Centre.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_ce_notitty, g_Centre.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_ce_nohand, g_Centre.GetBrothel(0)->m_RestrictHand);

		advert = g_Centre.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_ce_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Promotion Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_ce_advamnt);

		SetCheckBox(p_ce_pot_b, g_Centre.GetBrothel(0)->m_KeepPotionsStocked);
		number = g_Centre.GetBrothel(0)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_ce_pot_a);
		DisableCheckBox(p_ce_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own the Centre yet.)"), p_ce_id);
	}

	if (g_Clinic.GetNumBrothels() > 0)
	{
		EditTextItem(("The Clinic"), p_cl_id);

		SetCheckBox(p_cl_noanal, g_Clinic.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_cl_nobdsm, g_Clinic.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_cl_nonorm, g_Clinic.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_cl_nobeast, g_Clinic.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_cl_nogroup, g_Clinic.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_cl_noles, g_Clinic.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_cl_nooral, g_Clinic.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_cl_notitty, g_Clinic.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_cl_nohand, g_Clinic.GetBrothel(0)->m_RestrictHand);

		advert = g_Clinic.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_cl_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Promotion Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_cl_advamnt);

		SetCheckBox(p_cl_pot_b, g_Clinic.GetBrothel(0)->m_KeepPotionsStocked);
		number = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_cl_pot_a);
		DisableCheckBox(p_cl_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own the Clinic yet.)"), p_cl_id);
	}

	if (g_Farm.GetNumBrothels() > 0)
	{
		EditTextItem(("The Clinic"), p_cl_id);

		SetCheckBox(p_fa_noanal, g_Farm.GetBrothel(0)->m_RestrictAnal);
		SetCheckBox(p_fa_nobdsm, g_Farm.GetBrothel(0)->m_RestrictBDSM);
		SetCheckBox(p_fa_nonorm, g_Farm.GetBrothel(0)->m_RestrictNormal);
		SetCheckBox(p_fa_nobeast, g_Farm.GetBrothel(0)->m_RestrictBeast);
		SetCheckBox(p_fa_nogroup, g_Farm.GetBrothel(0)->m_RestrictGroup);
		SetCheckBox(p_fa_noles, g_Farm.GetBrothel(0)->m_RestrictLesbian);
		SetCheckBox(p_fa_nooral, g_Farm.GetBrothel(0)->m_RestrictOral);
		SetCheckBox(p_fa_notitty, g_Farm.GetBrothel(0)->m_RestrictTitty);
		SetCheckBox(p_fa_nohand, g_Farm.GetBrothel(0)->m_RestrictHand);

		advert = g_Farm.GetBrothel(0)->m_AdvertisingBudget / 50;
		advert = SliderRange(p_fa_advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
		ss.str(""); ss << ("Promotion Budget: ") << (advert * 50) << (" gold / week");
		EditTextItem(ss.str(), p_fa_advamnt);

		SetCheckBox(p_fa_pot_b, g_Farm.GetBrothel(0)->m_KeepPotionsStocked);
		number = g_Farm.GetBrothel(0)->m_AntiPregPotions;
		ss.str(""); ss << ("You have: ") << number;
		EditTextItem(ss.str(), p_fa_pot_a);
		DisableCheckBox(p_fa_pot_b, number < 1);
	}
	else
	{
		EditTextItem(("(You do not own the Farm yet.)"), p_fa_id);
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
		if (g_InterfaceEvents.CheckCheckbox(p_ho_pot_b))	g_House.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_ho_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_ho_noanal))	g_House.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_ho_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nobdsm))	g_House.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_ho_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nonorm))	g_House.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_ho_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nobeast))	g_House.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_ho_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nogroup))	g_House.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_ho_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_noles))	g_House.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_ho_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nooral))	g_House.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_ho_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_notitty))	g_House.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_ho_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_ho_nohand))	g_House.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_ho_nohand);

		if (g_InterfaceEvents.CheckSlider(p_ho_advslid))
		{
			g_House.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_ho_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_ho_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_ho_advamnt);
		}
	}

	if (g_Brothels.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b0_pot_b))	g_Brothels.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_b0_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b0_noanal))	g_Brothels.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_b0_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nobdsm))	g_Brothels.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_b0_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nonorm))	g_Brothels.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_b0_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nobeast))	g_Brothels.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_b0_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nogroup))	g_Brothels.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_b0_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_noles))	g_Brothels.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_b0_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nooral))	g_Brothels.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_b0_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_notitty))	g_Brothels.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_b0_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b0_nohand))	g_Brothels.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_b0_nohand);

		if (g_InterfaceEvents.CheckSlider(p_b0_advslid))
		{
			g_Brothels.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_b0_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b0_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b0_advamnt);
		}
	}

	if (g_Brothels.GetNumBrothels() > 1)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b1_pot_b))	g_Brothels.GetBrothel(1)->m_KeepPotionsStocked = IsCheckboxOn(p_b1_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b1_noanal))	g_Brothels.GetBrothel(1)->m_RestrictAnal = IsCheckboxOn(p_b1_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nobdsm))	g_Brothels.GetBrothel(1)->m_RestrictBDSM = IsCheckboxOn(p_b1_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nonorm))	g_Brothels.GetBrothel(1)->m_RestrictNormal = IsCheckboxOn(p_b1_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nobeast))	g_Brothels.GetBrothel(1)->m_RestrictBeast = IsCheckboxOn(p_b1_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nogroup))	g_Brothels.GetBrothel(1)->m_RestrictGroup = IsCheckboxOn(p_b1_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_noles))	g_Brothels.GetBrothel(1)->m_RestrictLesbian = IsCheckboxOn(p_b1_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nooral))	g_Brothels.GetBrothel(1)->m_RestrictOral = IsCheckboxOn(p_b1_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_notitty))	g_Brothels.GetBrothel(1)->m_RestrictTitty = IsCheckboxOn(p_b1_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b1_nohand))	g_Brothels.GetBrothel(1)->m_RestrictHand = IsCheckboxOn(p_b1_nohand);

		if (g_InterfaceEvents.CheckSlider(p_b1_advslid))
		{
			g_Brothels.GetBrothel(1)->m_AdvertisingBudget = SliderValue(p_b1_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b1_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b1_advamnt);
		}
	}
	if (g_Brothels.GetNumBrothels() > 2)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b2_pot_b))	g_Brothels.GetBrothel(2)->m_KeepPotionsStocked = IsCheckboxOn(p_b2_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b2_noanal))	g_Brothels.GetBrothel(2)->m_RestrictAnal = IsCheckboxOn(p_b2_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nobdsm))	g_Brothels.GetBrothel(2)->m_RestrictBDSM = IsCheckboxOn(p_b2_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nonorm))	g_Brothels.GetBrothel(2)->m_RestrictNormal = IsCheckboxOn(p_b2_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nobeast))	g_Brothels.GetBrothel(2)->m_RestrictBeast = IsCheckboxOn(p_b2_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nogroup))	g_Brothels.GetBrothel(2)->m_RestrictGroup = IsCheckboxOn(p_b2_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_noles))	g_Brothels.GetBrothel(2)->m_RestrictLesbian = IsCheckboxOn(p_b2_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nooral))	g_Brothels.GetBrothel(2)->m_RestrictOral = IsCheckboxOn(p_b2_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_notitty))	g_Brothels.GetBrothel(2)->m_RestrictTitty = IsCheckboxOn(p_b2_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b2_nohand))	g_Brothels.GetBrothel(2)->m_RestrictHand = IsCheckboxOn(p_b2_nohand);

		if (g_InterfaceEvents.CheckSlider(p_b2_advslid))
		{
			g_Brothels.GetBrothel(2)->m_AdvertisingBudget = SliderValue(p_b2_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b2_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b2_advamnt);
		}
	}
	if (g_Brothels.GetNumBrothels() > 3)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b3_pot_b))	g_Brothels.GetBrothel(3)->m_KeepPotionsStocked = IsCheckboxOn(p_b3_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b3_noanal))	g_Brothels.GetBrothel(3)->m_RestrictAnal = IsCheckboxOn(p_b3_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nobdsm))	g_Brothels.GetBrothel(3)->m_RestrictBDSM = IsCheckboxOn(p_b3_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nonorm))	g_Brothels.GetBrothel(3)->m_RestrictNormal = IsCheckboxOn(p_b3_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nobeast))	g_Brothels.GetBrothel(3)->m_RestrictBeast = IsCheckboxOn(p_b3_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nogroup))	g_Brothels.GetBrothel(3)->m_RestrictGroup = IsCheckboxOn(p_b3_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_noles))	g_Brothels.GetBrothel(3)->m_RestrictLesbian = IsCheckboxOn(p_b3_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nooral))	g_Brothels.GetBrothel(3)->m_RestrictOral = IsCheckboxOn(p_b3_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_notitty))	g_Brothels.GetBrothel(3)->m_RestrictTitty = IsCheckboxOn(p_b3_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b3_nohand))	g_Brothels.GetBrothel(3)->m_RestrictHand = IsCheckboxOn(p_b3_nohand);


		if (g_InterfaceEvents.CheckSlider(p_b3_advslid))
		{
			g_Brothels.GetBrothel(3)->m_AdvertisingBudget = SliderValue(p_b3_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b3_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b3_advamnt);
		}
	}
	if (g_Brothels.GetNumBrothels() > 4)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b4_pot_b))	g_Brothels.GetBrothel(4)->m_KeepPotionsStocked = IsCheckboxOn(p_b4_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b4_noanal))	g_Brothels.GetBrothel(4)->m_RestrictAnal = IsCheckboxOn(p_b4_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nobdsm))	g_Brothels.GetBrothel(4)->m_RestrictBDSM = IsCheckboxOn(p_b4_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nonorm))	g_Brothels.GetBrothel(4)->m_RestrictNormal = IsCheckboxOn(p_b4_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nobeast))	g_Brothels.GetBrothel(4)->m_RestrictBeast = IsCheckboxOn(p_b4_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nogroup))	g_Brothels.GetBrothel(4)->m_RestrictGroup = IsCheckboxOn(p_b4_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_noles))	g_Brothels.GetBrothel(4)->m_RestrictLesbian = IsCheckboxOn(p_b4_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nooral))	g_Brothels.GetBrothel(4)->m_RestrictOral = IsCheckboxOn(p_b4_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_notitty))	g_Brothels.GetBrothel(4)->m_RestrictTitty = IsCheckboxOn(p_b4_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b4_nohand))	g_Brothels.GetBrothel(4)->m_RestrictHand = IsCheckboxOn(p_b4_nohand);


		if (g_InterfaceEvents.CheckSlider(p_b4_advslid))
		{
			g_Brothels.GetBrothel(4)->m_AdvertisingBudget = SliderValue(p_b4_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b4_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b4_advamnt);
		}
	}
	if (g_Brothels.GetNumBrothels() > 5)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b5_pot_b))	g_Brothels.GetBrothel(5)->m_KeepPotionsStocked = IsCheckboxOn(p_b5_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b5_noanal))	g_Brothels.GetBrothel(5)->m_RestrictAnal = IsCheckboxOn(p_b5_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nobdsm))	g_Brothels.GetBrothel(5)->m_RestrictBDSM = IsCheckboxOn(p_b5_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nonorm))	g_Brothels.GetBrothel(5)->m_RestrictNormal = IsCheckboxOn(p_b5_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nobeast))	g_Brothels.GetBrothel(5)->m_RestrictBeast = IsCheckboxOn(p_b5_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nogroup))	g_Brothels.GetBrothel(5)->m_RestrictGroup = IsCheckboxOn(p_b5_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_noles))	g_Brothels.GetBrothel(5)->m_RestrictLesbian = IsCheckboxOn(p_b5_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nooral))	g_Brothels.GetBrothel(5)->m_RestrictOral = IsCheckboxOn(p_b5_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_notitty))	g_Brothels.GetBrothel(5)->m_RestrictTitty = IsCheckboxOn(p_b5_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b5_nohand))	g_Brothels.GetBrothel(5)->m_RestrictHand = IsCheckboxOn(p_b5_nohand);


		if (g_InterfaceEvents.CheckSlider(p_b5_advslid))
		{
			g_Brothels.GetBrothel(5)->m_AdvertisingBudget = SliderValue(p_b5_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b5_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b5_advamnt);
		}
	}
	if (g_Brothels.GetNumBrothels() > 6)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_b6_pot_b))	g_Brothels.GetBrothel(6)->m_KeepPotionsStocked = IsCheckboxOn(p_b6_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_b6_noanal))	g_Brothels.GetBrothel(6)->m_RestrictAnal = IsCheckboxOn(p_b6_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nobdsm))	g_Brothels.GetBrothel(6)->m_RestrictBDSM = IsCheckboxOn(p_b6_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nonorm))	g_Brothels.GetBrothel(6)->m_RestrictNormal = IsCheckboxOn(p_b6_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nobeast))	g_Brothels.GetBrothel(6)->m_RestrictBeast = IsCheckboxOn(p_b6_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nogroup))	g_Brothels.GetBrothel(6)->m_RestrictGroup = IsCheckboxOn(p_b6_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_noles))	g_Brothels.GetBrothel(6)->m_RestrictLesbian = IsCheckboxOn(p_b6_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nooral))	g_Brothels.GetBrothel(6)->m_RestrictOral = IsCheckboxOn(p_b6_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_notitty))	g_Brothels.GetBrothel(6)->m_RestrictTitty = IsCheckboxOn(p_b6_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_b6_nohand))	g_Brothels.GetBrothel(6)->m_RestrictHand = IsCheckboxOn(p_b6_nohand);


		if (g_InterfaceEvents.CheckSlider(p_b6_advslid))
		{
			g_Brothels.GetBrothel(6)->m_AdvertisingBudget = SliderValue(p_b6_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_b6_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_b6_advamnt);
		}
	}
	if (g_Studios.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_st_pot_b))	g_Studios.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_st_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_st_noanal))	g_Studios.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_st_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nobdsm))	g_Studios.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_st_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nonorm))	g_Studios.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_st_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nobeast))	g_Studios.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_st_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nogroup))	g_Studios.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_st_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_st_noles))	g_Studios.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_st_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nooral))	g_Studios.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_st_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_st_notitty))	g_Studios.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_st_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_st_nohand))	g_Studios.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_st_nohand);


		if (g_InterfaceEvents.CheckSlider(p_st_advslid))
		{
			g_Studios.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_st_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_st_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_st_advamnt);
		}
	}
	if (g_Arena.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_ar_pot_b))	g_Arena.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_ar_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_ar_noanal))	g_Arena.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_ar_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nobdsm))	g_Arena.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_ar_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nonorm))	g_Arena.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_ar_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nobeast))	g_Arena.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_ar_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nogroup))	g_Arena.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_ar_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_noles))	g_Arena.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_ar_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nooral))	g_Arena.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_ar_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_notitty))	g_Arena.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_ar_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_ar_nohand))	g_Arena.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_ar_nohand);


		if (g_InterfaceEvents.CheckSlider(p_ar_advslid))
		{
			g_Arena.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_ar_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_ar_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_ar_advamnt);
		}
	}
	if (g_Centre.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_ce_pot_b))	g_Centre.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_ce_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_ce_noanal))	g_Centre.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_ce_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nobdsm))	g_Centre.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_ce_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nonorm))	g_Centre.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_ce_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nobeast))	g_Centre.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_ce_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nogroup))	g_Centre.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_ce_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_noles))	g_Centre.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_ce_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nooral))	g_Centre.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_ce_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_notitty))	g_Centre.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_ce_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_ce_nohand))	g_Centre.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_ce_nohand);


		if (g_InterfaceEvents.CheckSlider(p_ce_advslid))
		{
			g_Centre.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_ce_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_ce_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_ce_advamnt);
		}
	}
	if (g_Clinic.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_cl_pot_b))	g_Clinic.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_cl_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_cl_noanal))	g_Clinic.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_cl_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nobdsm))	g_Clinic.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_cl_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nonorm))	g_Clinic.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_cl_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nobeast))	g_Clinic.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_cl_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nogroup))	g_Clinic.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_cl_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_noles))	g_Clinic.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_cl_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nooral))	g_Clinic.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_cl_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_notitty))	g_Clinic.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_cl_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_cl_nohand))	g_Clinic.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_cl_nohand);


		if (g_InterfaceEvents.CheckSlider(p_cl_advslid))
		{
			g_Clinic.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_cl_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_cl_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_cl_advamnt);
		}
	}
	if (g_Farm.GetNumBrothels() > 0)
	{
		if (g_InterfaceEvents.CheckCheckbox(p_fa_pot_b))	g_Farm.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(p_fa_pot_b);

		if (g_InterfaceEvents.CheckCheckbox(p_fa_noanal))	g_Farm.GetBrothel(0)->m_RestrictAnal = IsCheckboxOn(p_fa_noanal);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nobdsm))	g_Farm.GetBrothel(0)->m_RestrictBDSM = IsCheckboxOn(p_fa_nobdsm);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nonorm))	g_Farm.GetBrothel(0)->m_RestrictNormal = IsCheckboxOn(p_fa_nonorm);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nobeast))	g_Farm.GetBrothel(0)->m_RestrictBeast = IsCheckboxOn(p_fa_nobeast);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nogroup))	g_Farm.GetBrothel(0)->m_RestrictGroup = IsCheckboxOn(p_fa_nogroup);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_noles))	g_Farm.GetBrothel(0)->m_RestrictLesbian = IsCheckboxOn(p_fa_noles);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nooral))	g_Farm.GetBrothel(0)->m_RestrictOral = IsCheckboxOn(p_fa_nooral);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_notitty))	g_Farm.GetBrothel(0)->m_RestrictTitty = IsCheckboxOn(p_fa_notitty);
		if (g_InterfaceEvents.CheckCheckbox(p_fa_nohand))	g_Farm.GetBrothel(0)->m_RestrictHand = IsCheckboxOn(p_fa_nohand);


		if (g_InterfaceEvents.CheckSlider(p_fa_advslid))
		{
			g_Farm.GetBrothel(0)->m_AdvertisingBudget = SliderValue(p_fa_advslid) * 50;
			ss.str(""); ss << ("Advertising Budget: ") << (SliderValue(p_fa_advslid) * 50) << (" gold / week");
			EditTextItem(ss.str(), p_fa_advamnt);
		}
	}
}
