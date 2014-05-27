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
#include "libintl.h"
#include "cMovieStudio.h"
#include "cClinic.h"
#include "cArena.h"
#include "cCentre.h"
#include "cHouse.h"

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
extern int g_CurrentScreen;
extern int g_Building;

static cTariff tariff;
static stringstream ss;

bool cScreenBuildingSetup::ids_set = false;

void cScreenBuildingSetup::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	curbrothel_id = get_id("CurrentBrothel");
	potioncost_id = get_id("PotionCost");
	potionavail_id = get_id("AvailablePotions");
	potions10_id = get_id("10PotionsButton");
	potions20_id = get_id("20PotionsButton");
	autopotions_id = get_id("AutoBuyPotionToggle");
	barstaff_id = get_id("BarStaffCost");
	barhire_id = get_id("BarHireButton");
	barfire_id = get_id("BarFireButton");
	casinostaff_id = get_id("CasinoStaffCost");
	casinohire_id = get_id("CasinoHireButton");
	casinofire_id = get_id("CasinoFireButton");
	roomcost_id = get_id("RoomAddCost");
	buyrooms_id = get_id("BuildRoomsButton");
	restrict_id = get_id("SexRestrictions");
	noanal_id = get_id("ProhibitAnalToggleRes");
	nobdsm_id = get_id("ProhibitBDSMToggleRes");
	nonormal_id = get_id("ProhibitNormalToggleRes");
	nobeast_id = get_id("ProhibitBeastToggleRes");
	nogroup_id = get_id("ProhibitGroupToggleRes");
	nolesbian_id = get_id("ProhibitLesbianToggleRes");
	nooral_id = get_id("ProhibitOralToggleRes");
	notitty_id = get_id("ProhibitTittyToggleRes");
	advertsli_id = get_id("AdvertisingSlider");
	advertamt_id = get_id("AdvertisingValue");
}

void cScreenBuildingSetup::init()
{
	g_CurrentScreen = SCREEN_BROTHELMANAGEMENT;
	if(!g_InitWin)
		return;

	Focused();
	g_InitWin = false;
	
	ss.str("");
	ss << gettext("Add Rooms: ") << tariff.add_room_cost(5) << gettext(" gold");
	EditTextItem(ss.str(), roomcost_id);

	ss.str("");
	ss << gettext("Anti-Preg Potions: ") << tariff.anti_preg_price(1) << gettext(" gold each");
	EditTextItem(ss.str(), potioncost_id);

/*	ss.str("");
	ss << gettext("Bar Staff: ") << tariff.bar_staff_wages() << gettext(" gold / week");
	EditTextItem(ss.str(), barstaff_id);

	ss.str("");
	ss << gettext("Casino Staff: ") << tariff.casino_staff_wages() << gettext(" gold / week");
	EditTextItem(ss.str(), casinostaff_id);
*/
	string brothel = "";
	string message = "";
	int number = 0;
	int advert = 0;

	switch(g_Building)
		{
		case BUILDING_STUDIO:
			brothel = gettext("The Movie Studio");
			EditTextItem(brothel, curbrothel_id);
			number = g_Studios.GetBrothel(0)->m_AntiPregPotions;
	
			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_Studios.GetBrothel(0)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Promotion Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_Studios.GetBrothel(0)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_Studios.GetBrothel(0)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_Studios.GetBrothel(0)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_Studios.GetBrothel(0)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_Studios.GetBrothel(0)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_Studios.GetBrothel(0)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_Studios.GetBrothel(0)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_Studios.GetBrothel(0)->m_RestrictOral);
			SetCheckBox(notitty_id, g_Studios.GetBrothel(0)->m_RestrictTitty);
			break;
		case BUILDING_CLINIC:
			brothel = gettext("The Clinic");
			EditTextItem(brothel, curbrothel_id);
			number = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
	
			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_Clinic.GetBrothel(0)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Promotion Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_Clinic.GetBrothel(0)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_Clinic.GetBrothel(0)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_Clinic.GetBrothel(0)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_Clinic.GetBrothel(0)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_Clinic.GetBrothel(0)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_Clinic.GetBrothel(0)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_Clinic.GetBrothel(0)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_Clinic.GetBrothel(0)->m_RestrictOral);
			SetCheckBox(notitty_id, g_Clinic.GetBrothel(0)->m_RestrictTitty);
			break;
		case BUILDING_ARENA:
			brothel = gettext("The Arena");
			EditTextItem(brothel, curbrothel_id);
			number = g_Arena.GetBrothel(0)->m_AntiPregPotions;
	
			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_Arena.GetBrothel(0)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Promotion Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_Arena.GetBrothel(0)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_Arena.GetBrothel(0)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_Arena.GetBrothel(0)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_Arena.GetBrothel(0)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_Arena.GetBrothel(0)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_Arena.GetBrothel(0)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_Arena.GetBrothel(0)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_Arena.GetBrothel(0)->m_RestrictOral);
			SetCheckBox(notitty_id, g_Arena.GetBrothel(0)->m_RestrictTitty);
			break;
		case BUILDING_CENTRE:
			brothel = gettext("The Community Centre");
			EditTextItem(brothel, curbrothel_id);
			number = g_Centre.GetBrothel(0)->m_AntiPregPotions;
	
			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_Centre.GetBrothel(0)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Promotion Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_Centre.GetBrothel(0)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_Centre.GetBrothel(0)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_Centre.GetBrothel(0)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_Centre.GetBrothel(0)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_Centre.GetBrothel(0)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_Centre.GetBrothel(0)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_Centre.GetBrothel(0)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_Centre.GetBrothel(0)->m_RestrictOral);
			SetCheckBox(notitty_id, g_Centre.GetBrothel(0)->m_RestrictTitty);
			break;
		case BUILDING_HOUSE:
			brothel = gettext("Your House");
			EditTextItem(brothel, curbrothel_id);
			number = g_House.GetBrothel(0)->m_AntiPregPotions;
	
			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_House.GetBrothel(0)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Promotion Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_House.GetBrothel(0)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_House.GetBrothel(0)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_House.GetBrothel(0)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_House.GetBrothel(0)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_House.GetBrothel(0)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_House.GetBrothel(0)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_House.GetBrothel(0)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_House.GetBrothel(0)->m_RestrictOral);
			SetCheckBox(notitty_id, g_House.GetBrothel(0)->m_RestrictTitty);
			break;
		case BUILDING_BROTHEL:
		default:
			brothel = gettext("Current Brothel: ");
			brothel += g_Brothels.GetName(g_CurrBrothel);
			EditTextItem(brothel, curbrothel_id);

			// number = g_Brothels.GetNumPotions();
			number = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions;

			_itoa(number,buffer,10);
			message = gettext("You have: ");
			message += buffer;
			EditTextItem(message, potionavail_id);
			DisableCheckBox(autopotions_id, number < 1);

			// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
			advert = g_Brothels.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget / 50;
			advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
			ss.str("");
			ss << gettext("Advertising Budget: ") << (advert * 50) << gettext(" gold / week");
			EditTextItem(ss.str(), advertamt_id);

			// setup check boxes
			SetCheckBox(autopotions_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_KeepPotionsStocked);
			SetCheckBox(noanal_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictAnal);
			SetCheckBox(nobdsm_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBDSM);
			SetCheckBox(nonormal_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictNormal);
			SetCheckBox(nobeast_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBeast);
			SetCheckBox(nogroup_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictGroup);
			SetCheckBox(nolesbian_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictLesbian);
			SetCheckBox(nooral_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictOral);
			SetCheckBox(notitty_id, g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictTitty);
			break;
		}

		HideButton(barhire_id, true);
		HideButton(casinohire_id, true);
		HideButton(barfire_id, true);
		HideButton(casinofire_id, true);
}

void cScreenBuildingSetup::process()
{
	// we need to make sure the ID variables are set
	if(!ids_set)
		set_ids();

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}


void cScreenBuildingSetup::check_events()
{
	// no events means we can go home
	if(g_InterfaceEvents.GetNumEvents() == 0)
		return;

	// if it's the back button, pop the window off the stack and we're done
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_Building = BUILDING_BROTHEL;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if(g_InterfaceEvents.CheckButton(buyrooms_id))
	{
		if(!g_Gold.brothel_cost(5000))
			g_MessageQue.AddToQue(gettext("You need 5000 gold to add 5 rooms"), 1);
		else
		{
			switch(g_Building)
				{
			case BUILDING_STUDIO:
				g_Studios.GetBrothel(0)->m_NumRooms += 5;
				break;
			case BUILDING_CLINIC:
				g_Clinic.GetBrothel(0)->m_NumRooms += 5;
				break;
			case BUILDING_ARENA:
				g_Arena.GetBrothel(0)->m_NumRooms += 5;
				break;
			case BUILDING_CENTRE:
				g_Centre.GetBrothel(0)->m_NumRooms += 5;
				break;
			case BUILDING_HOUSE:
				g_House.GetBrothel(0)->m_NumRooms += 5;
				break;

			case BUILDING_BROTHEL:
			default:
				g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms += 5;
				break;
			}
		}
	}
	if(g_InterfaceEvents.CheckButton(potions10_id))
	{
		int MaxSupplies = 0;
		int number = 0;
		string message = "";

		switch(g_Building)
			{
		case BUILDING_STUDIO:
			MaxSupplies = g_Studios.GetSupplyShedLevel()*700;
			number = g_Studios.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Studios.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_CLINIC:
			MaxSupplies = g_Clinic.GetSupplyShedLevel()*700;
			number = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Clinic.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_ARENA:
			MaxSupplies = g_Arena.GetSupplyShedLevel()*700;
			number = g_Arena.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Arena.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_CENTRE:
			MaxSupplies = g_Centre.GetSupplyShedLevel()*700;
			number = g_Centre.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Centre.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_HOUSE:
			MaxSupplies = g_House.GetSupplyShedLevel()*700;
			number = g_House.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_House.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_BROTHEL:
		default:
			MaxSupplies = g_Brothels.GetSupplyShedLevel()*700;
			number = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message += buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+10 > MaxSupplies)
					number = (number+10) - MaxSupplies;
				else
					number = 10;

				// afford returns a bool
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
			}
		g_InitWin = true;
		return;
	}
	if(g_InterfaceEvents.CheckButton(potions20_id))
	{
		int MaxSupplies = 0;
		int number = 0;
		string message = "";

		switch(g_Building)
		{
		case BUILDING_STUDIO:
			MaxSupplies = g_Studios.GetSupplyShedLevel()*700;
			number = g_Studios.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Studios.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_CLINIC:
			MaxSupplies = g_Clinic.GetSupplyShedLevel()*700;
			number = g_Clinic.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Clinic.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_ARENA:
			MaxSupplies = g_Arena.GetSupplyShedLevel()*700;
			number = g_Arena.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Arena.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_CENTRE:
			MaxSupplies = g_Centre.GetSupplyShedLevel()*700;
			number = g_Centre.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Centre.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_HOUSE:
			MaxSupplies = g_House.GetSupplyShedLevel()*700;
			number = g_House.GetBrothel(0)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_House.GetBrothel(0)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
		case BUILDING_BROTHEL:
		default:
			MaxSupplies = g_Brothels.GetSupplyShedLevel()*700;
			number = g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions;
			if(number == MaxSupplies)
			{
				_itoa(MaxSupplies,buffer,10);
				message = gettext("You can only store up to ");
				message+=buffer;
				message += gettext(" potions");
				g_MessageQue.AddToQue(message, 0);
			}
			else
			{
				if(number+20 > MaxSupplies)
					number = (number+20) - MaxSupplies;
				else
					number = 20;
	
				if(g_Gold.afford(number*2))
				{
					g_Gold.girl_support(number*2);
					g_Brothels.GetBrothel(g_CurrBrothel)->m_AntiPregPotions += number;
				}
				else
					g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
			}
			break;
			break;

		} //end switch
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

	if(g_InterfaceEvents.CheckSlider(advertsli_id))
	{
		// set advertising budget based on slider
	switch(g_Building)
		{
	case BUILDING_STUDIO:
		g_Studios.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
	case BUILDING_CLINIC:
		g_Clinic.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
	case BUILDING_ARENA:
		g_Arena.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
	case BUILDING_CENTRE:
		g_Centre.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
	case BUILDING_HOUSE:
		g_House.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
	case BUILDING_BROTHEL:
	default:
		g_Brothels.GetBrothel(g_CurrBrothel)->m_AdvertisingBudget = SliderValue(advertsli_id) * 50;
		break;
		}
		ss.str("");
		ss << gettext("Advertising Budget: ") << (SliderValue(advertsli_id) * 50) << gettext(" gold / week");
		EditTextItem(ss.str(), advertamt_id);
	}
	switch(g_Building)
	{
	case BUILDING_STUDIO:
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_Studios.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_Studios.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_CLINIC:
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_Clinic.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_Clinic.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_ARENA:
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_Arena.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_Arena.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_CENTRE:
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_Centre.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_Centre.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_HOUSE:
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_House.GetBrothel(0)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_House.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		break;
	case BUILDING_BROTHEL:
	default:
		{
		if(g_InterfaceEvents.CheckCheckbox(autopotions_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_KeepPotionsStocked = IsCheckboxOn(autopotions_id);
		if(g_InterfaceEvents.CheckCheckbox(noanal_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictAnal = IsCheckboxOn(noanal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobdsm_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBDSM = IsCheckboxOn(nobdsm_id);
		if(g_InterfaceEvents.CheckCheckbox(nonormal_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictNormal = IsCheckboxOn(nonormal_id);
		if(g_InterfaceEvents.CheckCheckbox(nobeast_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictBeast = IsCheckboxOn(nobeast_id);
		if(g_InterfaceEvents.CheckCheckbox(nogroup_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictGroup = IsCheckboxOn(nogroup_id);
		if(g_InterfaceEvents.CheckCheckbox(nolesbian_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictLesbian = IsCheckboxOn(nolesbian_id);
		if(g_InterfaceEvents.CheckCheckbox(nooral_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictOral = IsCheckboxOn(nooral_id);
		if(g_InterfaceEvents.CheckCheckbox(notitty_id))
			g_Brothels.GetBrothel(g_CurrBrothel)->m_RestrictTitty = IsCheckboxOn(notitty_id);
		}
	}
}
