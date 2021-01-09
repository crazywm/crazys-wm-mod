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

#ifndef WM_GAME_MOVIES_FWD_H
#define WM_GAME_MOVIES_FWD_H

enum class SceneCategory {
    TEASE,
    SOFT,
    HARD,
    EXTREME,
    NUM_TYPES
};

enum class SceneType {
    // Tease
    ACTION,
    CHEF,
    MUSIC,
    TEASE,
    // Soft
    TITTY,
    STRIP,
    HAND,
    FOOT,
    MAST,
    // Hard
    ORAL,
    ANAL,
    SEX,
    LES,
    GROUP,
    // Extreme
    BEAST,
    BDSM,
    BUK,
    PUB_BDSM,
    THROAT,
    //
    COUNT
};

class sTargetGroup;
class cMovieManager;
class sGirl;
class MovieScene;

SceneCategory get_category(SceneType type);
int get_fluffer_required(SceneType type);
int get_stage_points_required(SceneType type);
const char* get_name(SceneType type);
const MovieScene& film_scene(cMovieManager& mgr, sGirl& girl, int quality, SceneType scene_type, bool forced);

#endif //WM_GAME_MOVIES_FWD_H
