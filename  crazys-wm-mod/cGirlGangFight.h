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

#pragma once

#ifndef __CGIRLGANGFIGHT_H
#define __CGIRLGANGFIGHT_H

#include <string>

#include "CLog.h"

using namespace std;

struct	sGirl;
struct	sGang;


class cGirlGangFight {
	sGirl	*m_girl;
	CLog	l;

	int		m_girl_stats;
	int		m_goon_stats;
	int		m_max_goons;
//	double	m_ratio;
//	int		m_dead_goons;

	bool	m_girl_fights;
	bool	m_girl_wins;
	bool	m_player_wins;
	bool	m_wipeout;
	bool	m_unopposed;

	double	m_odds;

	void lose_vs_own_gang(sGang* gang);
	void win_vs_own_gang(sGang* gang);
	int use_potions(sGang *gang, int casualties);
public:
	cGirlGangFight(sGirl *girl);

	bool girl_fights()	{ return m_girl_fights; }
	bool girl_submits()	{ return !m_girl_fights; }
	bool girl_won()		{ return m_girl_wins; }
	bool girl_lost()	{ return !m_girl_wins; }
	bool player_won()	{ return m_player_wins; }

	bool wipeout()		{ return m_wipeout; }

//	int dead_goons()	{ return m_dead_goons; }
};

#endif  /* __CGIRLGANGFIGHT_H */