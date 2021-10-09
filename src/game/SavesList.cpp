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

#include "SavesList.h"
#include "IGame.h"
#include "cGold.h"
#include <chrono>
#include <algorithm>
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/DirPath.h"
#include "utils/FileList.h"
#include <boost/numeric/conversion/cast.hpp>
#include "sConfig.h"
#include "Revision.h"

void SavesList::NotifySaveGame(const std::string& file_name, IGame& game) {
    auto& target = m_SaveData[file_name];
    target.Money = game.gold().ival();
    target.WeeksPlayed = game.get_weeks_played();

    auto now_time = std::chrono::system_clock::now().time_since_epoch();
    long sec_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now_time).count();
    target.LastSaveTime = boost::numeric_cast<int>(sec_since_epoch);
}

void SavesList::SaveXML(const std::string& target_file) {
    tinyxml2::XMLDocument doc;
    doc.LinkEndChild(doc.NewDeclaration());
    auto* root = doc.NewElement("SavesList");
    doc.LinkEndChild(root);

    for(auto& entry : m_SaveData) {
        auto& entry_tag = PushNewElement(*root, "SaveGame");
        entry_tag.SetAttribute("File", entry.first.c_str());
        entry_tag.SetAttribute("Money", entry.second.Money);
        entry_tag.SetAttribute("WeeksPlayed", entry.second.WeeksPlayed);
        entry_tag.SetAttribute("LastSaveTime", entry.second.LastSaveTime);
    }

    doc.SaveFile(target_file.c_str());
}

void SavesList::LoadXML(const std::string& source_file) {
    auto doc = LoadXMLDocument(source_file);
    auto root = doc->RootElement();
    if(!root) {
        throw std::runtime_error("Could not find root element in saves list");
    }
    for(auto& entry : IterateChildElements(*root)) {
        std::string file = GetStringAttribute(entry, "File");
        sSaveData data;
        data.Money = GetIntAttribute(entry, "Money");
        data.WeeksPlayed = GetIntAttribute(entry, "WeeksPlayed");
        data.LastSaveTime = GetIntAttribute(entry, "LastSaveTime");
        m_SaveData.emplace(file, data);
    }
}

void SavesList::BuildSaveGameList(const std::string& saves_path) {
    const char* pattern = "*.gam";
    FileList fl = FileList(saves_path.c_str(), pattern);

    DirPath list_file;
    list_file << saves_path << ".saves.xml";
    // TODO check that file exists
    try {
        LoadXML(list_file.c_str());
    } catch (std::runtime_error& rte) {
        // can ignore, we are going to build it anyway
    }

    bool built = false;
    for (int i = 0; i < fl.size(); i++)
    {
        // ignore autosave, and all the save games for which we have data
        if(fl[i].leaf() == "autosave.gam") continue;
        if(m_SaveData.count(fl[i].leaf()) > 0) continue;

        auto temp_game = IGame::CreateGame();
        try {
            LoadGame(fl[i].full(), [](const std::string&) {}, *temp_game);
            NotifySaveGame(fl[i].leaf(), *temp_game);
            built = true;
        } catch (const std::runtime_error& rte) {
            // print error, but otherwise ignore
        }
    }

    if(built) {
        SaveXML(list_file.c_str());
    }
}

bool SavesList::LoadGame(const std::string& source_file, const std::function<void(std::string)>& callback, IGame& target) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(source_file.c_str()) != tinyxml2::XML_SUCCESS) {
        callback(doc.ErrorStr());
        return false;
    }
    auto pRoot = doc.FirstChildElement("Root");
    if (pRoot == nullptr) {
        return false;
    }

    // load the version
    int major = GetIntAttribute(*pRoot, "MajorVersion");
    if (major != g_MajorVersion) {
        callback("You must start a new game with this version");
        return false;
    }
    std::string version("<blank>");
    if (pRoot->Attribute("ExeVersion")) { version = pRoot->Attribute("ExeVersion"); }
    if (version != "official") {
        callback("Warning, the exe was not detected as official, it was detected as " + version + ".  Attempting to load anyways.");
    }

    std::unique_ptr<IGame> cache = std::move(g_Game);
    g_Game.reset(&target);
    try {
        target.LoadGame(*pRoot, callback);
    } catch(...) {
        g_Game.release();
        g_Game = std::move(cache);
        throw;
    }
    g_Game.release();
    g_Game = std::move(cache);
    return true;
}

std::vector<SavesList::sSaveEntry> SavesList::get_saves() const {
    extern cConfig cfg;
    DirPath::expand_path(cfg.saves());
    std::vector<sSaveEntry> target;
    for(auto& entry : m_SaveData) {
        std::ifstream in("Saves/" + entry.first);
        if (!in.good()) continue; // don't show deleted saves
        target.emplace_back(sSaveEntry{entry.first, entry.second});
    }
    std::sort(begin(target), end(target), [](const sSaveEntry& a, const sSaveEntry& b) -> bool {
        return a.Data.LastSaveTime >= b.Data.LastSaveTime; });
    return target;
}
