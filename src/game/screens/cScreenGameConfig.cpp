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

#include "cScreenGameConfig.h"
#include "widgets/IListBox.h"
#include "CLog.h"
#include "IGame.h"
#include <algorithm>
#include "utils/string.hpp"
#include "utils/algorithms.hpp"

std::string get_class(const sKeyValueEntry* var) {
    // this const cast should not be necessary, but is required for mingw32
    return std::string(var->tag, const_cast<const char*>(std::strchr(var->tag, '.')));
}

cScreenGameConfig::cScreenGameConfig(bool use_in_game_mode) :
        cInterfaceWindowXML("game_setup.xml") ,
        m_InGameMode(use_in_game_mode) {

}

void cScreenGameConfig::init(bool back) {
    if(!back) {
        m_Settings = dynamic_cast<cGameSettings&>(g_Game->settings());
    }

    int last_selection = GetSelectedItemFromList(list_id);
    ClearListBox(list_id);

    m_SettingsList = m_Settings.list_all_settings();

    std::sort(begin(m_SettingsList), end(m_SettingsList), [](const auto& a, const auto& b) {
        return std::strcmp(a->tag, b->tag) <= 0;
    });

    if(m_InGameMode) {
        erase_if(m_SettingsList, [](const sKeyValueEntry* kve){
            return !starts_with(kve->tag, "user");
        });
    }

    std::map<std::string, std::pair<std::string, std::string>> headings =
            {{"initial", {"Initial", "Defines the starting setup of the game"}},
             {"gang", {"Gangs", "Defines gang behaviour"}},
             {"user", {"User Settings", "These can be changed by the player during the game"}},
             {"slave_market", {"Slave Market", "Slave Market"}},
             {"pregnancy", {"Pregnancy", "Pregnancy Settings"}},
             {"tax", {"Taxation", "Tax and Money Laundering"}},
             {"movies", {"Movies", "Movies and Studio"}},
             {"balancing", {"Balancing", "Other Settings that affect game balance"}},
            };

    std::string cls;
    for(int i = 0; i < m_SettingsList.size(); ++i) {
        auto& setting = m_SettingsList[i];
        std::string value;
        if(cls != get_class(setting)) {
            cls = get_class(setting);
            try {
                auto& heading = headings.at(cls);
                AddToListBox(list_id, -1, std::vector<FormattedCellData>({mk_text(heading.first), mk_text(heading.second), mk_text("")}), COLOR_DARKBLUE);
            } catch(std::logic_error&) {
                g_LogFile.error("interface", "Could not find heading for tag ", setting->tag);
                AddToListBox(list_id, -1, std::vector<FormattedCellData>({mk_text(cls), mk_text("-"), mk_text("")}), COLOR_DARKBLUE);
            }
        }
        switch(setting->value.which()) {
            case 0:
                value = boost::get<bool>(setting->value) ? "Yes" : "No";
                break;
            case 1:
                value = std::to_string(boost::get<sIntWithBounds>(setting->value).value);
                break;
            case 2:
            {
                float raw = boost::get<float>(setting->value);
                value = std::to_string(raw);
                break;
            }
            case 3:
            {
                sPercent raw = boost::get<sPercent>(setting->value);
                std::stringstream conv;
                conv.precision(1);
                conv.imbue(std::locale(""));
                conv << std::fixed << (raw * 100.0) << "%";
                value = conv.str();
                break;
            }

            case 4:
                value = boost::get<std::string>(setting->value);
                break;
        }

        AddToListBox(list_id, i, std::vector<FormattedCellData>({mk_text(setting->name), mk_text(setting->description), mk_text(value)}));
    }

    if(last_selection != -1) {
        SetSelectedItemInList(list_id, last_selection);
    }
}

void cScreenGameConfig::set_ids() {
    ok_id          = get_id("OkButton");
    revert_id      = get_id("RevertButton");
    list_id        = get_id("SettingsList");

    SetButtonCallback(revert_id, [this](){ this->init(false); });
    SetButtonCallback(ok_id, [this]() {
        dynamic_cast<cGameSettings&>(g_Game->settings()) = m_Settings;
        pop_window();
    });
    SetListBoxDoubleClickCallback(list_id, [this](int sel){
        if(sel < 0) return;
        auto& setting = m_SettingsList.at(sel);
        switch(setting->value.which()) {
            case 0:
                input_choice(setting->name, {"Yes", "No"},
                        [this, setting](int c){ m_Settings.set_value(setting->tag, c == 0); init(true); });
                break;
            case 1:
                input_integer([this, setting](int c){ m_Settings.set_value(setting->tag, c); init(true); },
                        m_Settings.get_integer(setting->tag));
                break;
            case 2:
            {
                input_string([this, setting](const std::string& v){
                    float val = std::strtof(v.c_str(), nullptr);
                    m_Settings.set_value(setting->tag, val);
                    init(true); }, std::to_string(m_Settings.get_float(setting->tag)));
                break;
            }case 3:
            {
                input_string([this, setting](const std::string& v){
                    float val = std::strtof(v.c_str(), nullptr);
                    val /= 100.f;
                    m_Settings.set_value(setting->tag, sPercent(val));
                    init(true); }, std::to_string(100.f * float(m_Settings.get_percent(setting->tag))));
                break;
            }
            case 4:
                input_string([this, setting](std::string v){ m_Settings.set_value(setting->tag, std::move(v)); init(true); });
                break;
        }
    });
}
