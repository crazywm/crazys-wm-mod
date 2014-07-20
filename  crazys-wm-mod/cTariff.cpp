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
#include "CLog.h"
#include "cTariff.h"
#include "cGirls.h"
#include "cBrothel.h"

extern	cGirls	g_Girls;
extern	CLog	g_LogFile;

static CLog &lf = g_LogFile;

double cTariff::slave_base_price(sGirl *girl)
{
	double cost;
/*
 *	The ask price is the base price for the girl
 *	It changes with her stats, so we need to refresh it
 */
	g_Girls.CalculateAskPrice(girl, false);
/*
 *	base price is the girl's ask price stat
 */
	cost = girl->askprice() * 15;
/*
 *	add to that the sum of her skills
 */
	for(u_int i=0; i<NUM_SKILLS; i++) {
		cost += (unsigned int)girl->m_Skills[i];
	}
/*
 *	virgins fetch a premium
 *	MOD: not sure of the rationale behind 158
 *	but a multiplier makes more sense to me.
 *	Let's say virgins go for half as much again
 */
	if (g_Girls.CheckVirginity(girl)) {
		cost *= 1.5;
	}
	lf.ss() << "CTariff: base price for slave '"
	  	 << girl->m_Name
		 << "' = "
		 << int(cost)
	;
	lf.ssend();
	return cost;
}

int cTariff::slave_buy_price(sGirl *girl)
{
	cConfig cfg;
	double cost = slave_base_price(girl);
	double factor = cfg.out_fact.slave_cost();

	lf.ss() << "CTariff: buy price config factor '"
	  	 << girl->m_Name
		 << "' = "
		 << factor
	;
	lf.ssend();
/*
 *	multiply by the config factor for buying slaves
 */
	cost *= cfg.out_fact.slave_cost();
/*
 *	a bit of debug chatter
 */
	lf.ss() << "CTariff: buy price for slave '"
	  	 << girl->m_Name
		 << "' = "
		 << int(cost)
	;
	lf.ssend();
	return int(cost);
}

int cTariff::slave_sell_price(sGirl *girl)
{
	cConfig cfg;
	double cost = slave_base_price(girl);
/*
 *	multiply by the config factor for buying slaves
 */
	return int(cost * cfg.in_fact.slave_sales());
}

int cTariff::empty_room_cost(sBrothel *brothel)
{
	cConfig cfg;
	double cost;
/*
 *	basic cost is number of empty rooms
 *	nominal cost is 2 gold per
 *	modified by brothel support multiplier
 */
	cost = brothel->m_NumRooms - brothel->m_NumGirls;
	cost *= 2;
	cost *= cfg.out_fact.brothel_support();
	return int(cost);
}

int cTariff::goon_weapon_upgrade(int level)
{
	cConfig cfg;
	return int((level + 1) * 1200 * cfg.out_fact.item_cost());
}

int cTariff::goon_mission_cost(int mission)
{
	cConfig cfg;
	double cost = 0.0;
	double factor = cfg.out_fact.goon_wages();

	switch(mission) {
	case MISS_SABOTAGE:	cost = factor * 150;	break;
	case MISS_SPYGIRLS:	cost = factor * 40;	break;
	case MISS_CAPTUREGIRL:	cost = factor * 125;	break;
	case MISS_EXTORTION:	cost = factor * 116;	break;
	case MISS_PETYTHEFT:	cost = factor * 110;	break;
	case MISS_GRANDTHEFT:	cost = factor * 250;	break;
	case MISS_KIDNAPP:	cost = factor *	150;	break;
	case MISS_CATACOMBS:	cost = factor * 300;	break;
	case MISS_TRAINING:	cost = factor * 90;	break;
	case MISS_RECRUIT:	cost = factor * 80;	break;
//	case MISS_SAIGON:	just kidding
	default:
		lf.ss() << "Warning: cTariff: unrecogised goon mission "
			<< mission
			<< " charging as guard mission"
		;
		lf.ssend();
		// drop through ...
	case MISS_GUARDING:	cost = factor * 60;	break;
	}

	if(cost > 0.0) {
		return int(cost);
	}

/*
 *	if we get here, the default didn't work
 *	which shouldn't happen. I HAVE put it in a
 *	non standard place, but it should be legal.
 *
 *	Still, just to be sure...
 */
	lf.ss() << "Warning: cTariff: logic error in gang_mission_cost("
		<< mission
		<< ") - charging as guard mission"
	;
	lf.ssend();
	return int(factor * 60);
}


/*

g++ -g CAnimatedSprite.o cBrothel.o cButton.o cCheckBox.o cChoiceMessage.o cCustomers.o cEditBox.o cEvents.o cFont.o CGameObject.o cGameScript.o cGirls.o CGraphics.o cImageItem.o cInterfaceWindow.o cInventory.o cListBox.o CLog.o cMessageBox.o CRenderQue.o CResourceManager.o cRival.o cScripts.o CSurface.o cTraits.o cTriggers.o GameFlags.o InterfaceGlobals.o InterfaceProcesses.o main.o FileList.o DirPath.o cRng.o cDungeonScreenManager.o sConfig.o cTariff.o cGold.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o -lSDL -lSDL_ttf -lSDL_gfx -lSDL_image -o WhoreMaster

 */
