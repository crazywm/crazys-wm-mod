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

#ifndef WM_DATA_H
#define WM_DATA_H

#include "fwd.h"
#include <array>
#include <string>
#include <vector>

namespace tinyxml2 {
    class XMLElement;
}

struct sTargetGroup {
    template<class T>
    using CategoryArray = std::array<T, (int)SceneCategory::NUM_TYPES>;
public:
    std::string Name;
    int Amount;                       // How many of these people are there
    CategoryArray<float> TurnOffs;      // Chance that a scene type is a turn-off to this group of people
    float ForcedIsTurnOff;
    CategoryArray<float> Desires;       // How much of a certain scene type is desired.
    SceneType Favourite;

    float RequiredScore;                // threshold position for sigmoid that decides whether the movie is good enough
    float SpendingPower;                // threshold position for sigmoid that decides whether they can afford the movie

    int Saturation = 0;                 // The amount of people who have seen a movie just recently and aren't going to watch a new one soon.

    void load_xml(const tinyxml2::XMLElement& root);
    void save_xml(tinyxml2::XMLElement& target) const;
};

struct sTargetGroupSpec {
    using CategoryArray = std::array<float, (int)SceneCategory::NUM_TYPES>;

    std::string Name;
    int MinAmount;
    int MaxAmount;
    CategoryArray TurnOffs;
    float ForcedIsTurnOff;
    CategoryArray DesiresMin;
    CategoryArray DesiresMax;

    std::vector<SceneType> Favourites;

    int RequiredScoreMin;
    int RequiredScoreMax;

    int SpendingPowerMin;
    int SpendingPowerMax;

    void load_xml(const tinyxml2::XMLElement& root);
};

struct MovieScene {
    SceneCategory Category;
    SceneType Type;
    int ContentQuality;
    int TechnicalQuality;
    bool WasForced;

    // Cast Data
    std::string Actress;

    // Crew Data
    std::string Director;
    std::string CameraMage;
    std::string CrystalPurifier;

    int Budget;

    void load_xml(const tinyxml2::XMLElement& root);
    void save_xml(tinyxml2::XMLElement& target) const;
};

struct Movie {
    std::string Name;
    std::vector<MovieScene> Scenes;
    int TicketPrice = 1;
    float Hype = 0;
    int Age = 0;

    int TotalEarnings = 0;
    int TotalCost = 0;

    // track what happened last week
    int EarningsLastWeek = 0;

    void load_xml(const tinyxml2::XMLElement& root);
    void save_xml(tinyxml2::XMLElement& target) const;
};

#endif //WM_DATA_H
