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
#include "cScriptUtils.h"
#include "cGirls.h"
#include "cMessageBox.h"
#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cClinic.h"
#include "cArena.h"
#include "cCentre.h"
#include "cHouse.h"
#include "cFarm.h"
#include "libintl.h"

extern cGirls g_Girls;
extern cMessageQue g_MessageQue;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager		g_Studios;
extern cClinicManager g_Clinic;
extern cArenaManager g_Arena;
extern cCentreManager g_Centre;
extern cHouseManager g_House;
extern cFarmManager g_Farm;
extern int g_CurrBrothel;
extern int g_Building;

void cScriptUtils::add_girl_to_brothel(sGirl *girl)
{
/*
 *	no girl -> nothing to do -> go home early
 */
	if(!girl) {
		g_Building = BUILDING_BROTHEL;
		return;
	}

	/* MYR: For some reason I can't figure out, a number of girl's house percentages
            are at zero or set to zero when they are sent to the brothel. I'm not sure
			how to fix it, so I'm explicitly setting the percentage to 60 here */
	girl->m_Stats[STAT_HOUSE] = 60;

	string text = girl->m_Realname;
/*
 *	OK: how rebellious is this floozy?
 */
	if(g_Girls.GetRebelValue(girl, false) >= 35) {
		text += " has been sent to your dungeon, as she is rebellious and poorly trained.";
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWGIRL);
		g_Building = BUILDING_BROTHEL;
		return;
	}
/*
 *	She qualifies for brothel duty - is there room?
 *	let's get some numbers
 */
	int total_rooms = 0;
	int rooms_used = 0;

	switch (g_Building)
		{
	case BUILDING_STUDIO:
		total_rooms = g_Studios.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_Studios.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_CLINIC:
		total_rooms = g_Clinic.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_Clinic.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_ARENA:
		total_rooms = g_Arena.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_Arena.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_CENTRE:
		total_rooms = g_Centre.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_Centre.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_HOUSE:
		total_rooms = g_House.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_House.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_FARM:
		total_rooms = g_Farm.GetBrothel(0)->m_NumRooms;
		rooms_used  = g_Farm.GetBrothel(0)->m_NumGirls;
		break;
	case BUILDING_BROTHEL:
	default: // regular brothel
		total_rooms = g_Brothels.GetBrothel(g_CurrBrothel)->m_NumRooms;
		rooms_used  = g_Brothels.GetBrothel(g_CurrBrothel)->m_NumGirls;
		break;
		}
	int diff = total_rooms - rooms_used;
/*
 *	now then...
 */
	if(diff <= 0) {
		text += (" has been sent to your dungeon, since current brothel is full.");
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWGIRL);
		g_Building = BUILDING_BROTHEL;
		return;
	}
/*
 *	otherwise, it's very simple
 */
	text += (" has been sent to your current brothel.");

	switch (g_Building)
		{
	case BUILDING_STUDIO:
		g_Studios.AddGirl(0, girl);
		break;
	case BUILDING_CLINIC:
		g_Clinic.AddGirl(0, girl);
		break;
	case BUILDING_ARENA:
		g_Arena.AddGirl(0, girl);
		break;
	case BUILDING_CENTRE:
		g_Centre.AddGirl(0, girl);
		break;
	case BUILDING_HOUSE:
		g_House.AddGirl(0, girl);
		break;
	case BUILDING_FARM:
		g_Farm.AddGirl(0, girl);
		break;
	case BUILDING_BROTHEL:
	default: // regular brothel
		g_Brothels.AddGirl(g_CurrBrothel, girl);
		break;
		}

	g_MessageQue.AddToQue(text, 0);
	g_Building = BUILDING_BROTHEL;
	return;
}

