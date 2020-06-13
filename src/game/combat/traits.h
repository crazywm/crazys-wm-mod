/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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

#ifndef WM_COMBAT_TRAITS_H
#define WM_COMBAT_TRAITS_H

// these traits can affect combat actions
namespace ct {
    // modifier for receiving physical damage
    constexpr const char* PHYS_DMG_MOD  = "combat.hurt.physical";
    constexpr const char* MAGIC_DMG_MOD = "combat.hurt.magical";
    constexpr const char* RETREAT_MOD   = "combat.retreat";
    constexpr const char* REST_MOD      = "combat.rest";
    constexpr const char* PHYS_CRIT_MOD = "combat.melee.crit";
/*
    const char* MEEK = "Meek";
    const char* INCORPOREAL = "Incorporeal";
    if (m_girl->has_trait("Clumsy"))        m_odds -= 0.05;
    if (m_girl->has_trait("Meek"))            m_odds -= 0.05;
    if (m_girl->has_trait("Dependant"))        m_odds -= 0.10;
    if (m_girl->has_trait("Fearless"))        m_odds += 0.10;
    if (m_girl->has_trait("Fleet of Foot"))    m_odds += 0.10;
    if (m_girl->has_trait("Brawler"))        m_odds += 0.15;
    if (m_girl->has_trait("Assassin"))        casualties += g_Dice.in_range(1, 6);
    if (m_girl->has_trait("Adventurer"))    casualties += 2;    // some level clearing instincts
    if (m_girl->has_trait("Merciless"))        casualties++;
    if (m_girl->has_trait("Yandere"))        casualties++;
    if (m_girl->has_trait("Tsundere"))        casualties++;
    if (m_girl->has_trait("Meek"))            casualties--;
    if (m_girl->has_trait("Dependant"))        casualties -= 2;
    if (m_girl->has_trait("Fleet of Foot")) casualties -= 2;
    */
}


#endif //WM_COMBAT_TRAITS_H
