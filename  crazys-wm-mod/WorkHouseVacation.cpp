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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cHouse.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cHouseManager g_House;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;
static cPlayer* m_Player = g_Brothels.GetPlayer();

// `J` House Job - General
bool cJobManager::WorkHouseVacation(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname;
	g_Building = BUILDING_HOUSE;

	int roll_a = g_Dice%100;

	// `CRAZY` This is places she can go
	/*default*/	int vac_type = 1;    string vac_type_text = "an onsen";
	/* */if (roll_a <= 20)	{ vac_type = 4; vac_type_text = "a local camp ground"; }
	else if (roll_a <= 40)	{ vac_type = 3; vac_type_text = "a spa and resort"; }
	else if (roll_a <= 60)	{ vac_type = 2; vac_type_text = "a near by beach"; }
	else if (roll_a >= 80)	{ vac_type = 0; vac_type_text = "an amusment park"; } 

	ss << "You send her to " << vac_type_text << " for the week.\n";

	girl->tiredness(-100);
	girl->happiness(100);
	girl->health(100);
	girl->mana(100);
	girl->pclove(10);
	girl->pchate(-5);


	// Improve stats
	int xp = 10, libido = 5;

	if (girl->has_trait("Quick Learner"))		{xp += 3;}
	else if (girl->has_trait("Slow Learner"))	{xp -= 3;}
	if (girl->has_trait("Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_HouseVacation(sGirl* girl, bool estimate)// not used
{
double jp = 0;
if (estimate)// for third detail string
{
}
else// for the actual check
{
}
return jp;
}
