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

#include "cGirls.h"
#include "character/sGirl.h"
#include "doctest.h"
#include "IGame.h"

TEST_CASE("trait group gag reflex") {
    if(!g_Game) {
        g_Game = IGame::CreateGame();
        g_Game->NewGame([](std::string) {});
    }

    sGirl girl(true);
    SUBCASE("increase neutral") {
        cGirls::AdjustTraitGroupGagReflex(girl, 1);
        CHECK(girl.has_active_trait(traits::NO_GAG_REFLEX));
    }
    SUBCASE("decrease neutral") {
        cGirls::AdjustTraitGroupGagReflex(girl, -1);
        CHECK(girl.has_active_trait(traits::GAG_REFLEX));
    }

    SUBCASE("increase no") {
        girl.gain_trait(traits::NO_GAG_REFLEX);
        cGirls::AdjustTraitGroupGagReflex(girl, 1);
        CHECK(girl.has_active_trait(traits::DEEP_THROAT));
        CHECK(!girl.has_active_trait(traits::NO_GAG_REFLEX));
    }
    SUBCASE("increase gag reflex") {
        girl.gain_trait(traits::GAG_REFLEX);
        cGirls::AdjustTraitGroupGagReflex(girl, 1);
        CHECK(!girl.has_active_trait(traits::GAG_REFLEX));
    }
    SUBCASE("increase strong gag reflex") {
        girl.gain_trait(traits::STRONG_GAG_REFLEX);
        cGirls::AdjustTraitGroupGagReflex(girl, 1);
        CHECK(girl.has_active_trait(traits::GAG_REFLEX));
        CHECK(!girl.has_active_trait(traits::STRONG_GAG_REFLEX));
    }
}