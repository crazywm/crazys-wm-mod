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


#ifndef CRAZYS_WM_MOD_CGAME_H
#define CRAZYS_WM_MOD_CGAME_H

#include "IGame.h"
#include <memory>
#include <list>
#include <string>
#include <functional>
#include <unordered_set>
#include "Constants.h"
#include <vector>
#include "scripting/fwd.hpp"
#include "interface/constants.h"
#include "IKeyValueStore.h"

namespace tinyxml2
{
    class XMLElement;
}

/*!
 * \brief Collects all information about a single game.
 * \details This class manages all global objects of a single game. This includes the girls,
 *          buildings, gangs, player, rivals etc.
 */
class cGame : public IGame {
public:
    cGame();

    void NextWeek() override;

    void NewGame(const std::function<void(std::string)>& callback) override;
    void LoadGame(const tinyxml2::XMLElement& source, const std::function<void(std::string)>& callback) override;
    void SaveGame(tinyxml2::XMLElement& root) override;

    void PushEvent(const scripting::sEventID& event) override;

    void push_message(std::string text, int color) override;
    void error(std::string message) override;
    cErrorContext push_error_context(std::string text) override;

private:
    // Load / Save
    void ReadGameAttributesXML(const tinyxml2::XMLElement& el);

    void LoadGirlFiles(const DirPath& location, const std::function<void(const std::string&)>& error_handler);
    void LoadItemFiles(DirPath location);
    void LoadTraitFiles(DirPath location);

    // Game Processing
    void HandleTaxes();
    void UpdateRunaways();
    void UpdateMarketSlaves();
    void CheckRaid();
    void UpdateBribeInfluence();

    std::vector<std::string> m_ErrorContextStack;

    // This keeps track of all unqiue girl files that are used for this game.
    std::unordered_set<std::string> m_GirlFiles;

    // event stack
    struct sScriptEventStack;
    std::unique_ptr<sScriptEventStack> m_EventStack;

    void RunNextEvent(const scripting::sEventID& event);
};

#endif //CRAZYS_WM_MOD_CGAME_H
