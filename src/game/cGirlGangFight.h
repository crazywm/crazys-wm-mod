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

struct sGirl;
struct sGang;

enum class EGirlEscapeAttemptResult {
    SUBMITS,
    STOPPED_BY_GOONS,
    STOPPED_BY_PLAYER,
    SUCCESS
};

EGirlEscapeAttemptResult AttemptEscape(sGirl& girl);

enum class EAttemptCaptureResult {
    SUBMITS,
    CAPTURED,
    ESCAPED
};

EAttemptCaptureResult AttemptCapture(sGang& gang, sGirl& girl);

// TODO unify this with ECombatResult!
enum class EFightResult {
    VICTORY,
    DRAW,
    DEFEAT
};

EFightResult GangBrawl(sGang& a, sGang& b);
EFightResult GirlFightsGirl(sGirl& a, sGirl& b);


#endif  /* __CGIRLGANGFIGHT_H */