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
#include "widgets/cListBox.h"
#include "CLog.h"
#include "Game.hpp"
#include <algorithm>

std::string get_class(const sKeyValueEntry* var) {
    // this const cast should not be necessary, but is required for mingw32
    return std::string(var->tag, const_cast<const char*>(std::strchr(var->tag, '.')));
}

void cScreenGameConfig::init(bool back) {
    ClearListBox(list_id);

    m_SettingsList = m_Settings.list_all_settings();

    std::sort(begin(m_SettingsList), end(m_SettingsList), [](const auto& a, const auto& b){
        return std::strcmp(a->tag, b->tag) <= 0;
    });

    std::map<std::string, std::pair<std::string, std::string>> headings =
            {{"initial", {"Initial", "Defines the starting setup of the game"}},
             {"gang", {"Gangs", "Defines gang behaviour"}},
             {"user", {"User Settings", "These can be changed by the player during the game"}},
             {"slave_market", {"Slave Market", "Slave Market"}},
             {"pregnancy", {"Pregnancy", "Pregnancy Settings"}},
             {"tax", {"Taxation", "Tax and Money Laundering"}}
            };

    std::string cls;
    for(int i = 0; i < m_SettingsList.size(); ++i) {
        auto& setting = m_SettingsList[i];
        std::string value;
        if(cls != get_class(setting)) {
            cls = get_class(setting);
            try {
                auto& heading = headings.at(cls);
                AddToListBox(list_id, -1, {heading.first, heading.second, ""}, COLOR_DARKBLUE);
            } catch(std::logic_error&) {
                g_LogFile.error("interface", "Could not find heading for tag ", setting->tag);
                AddToListBox(list_id, -1, {cls, "???", ""}, COLOR_DARKBLUE);
            }
        }
        switch(setting->value.which()) {
            case 0:
                value = boost::get<bool>(setting->value) ? "Yes" : "No";
                break;
            case 1:
                value = std::to_string(boost::get<int>(setting->value));
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

        AddToListBox(list_id, i, {setting->name, setting->description, value});
    }
}

cScreenGameConfig::cScreenGameConfig()  : cInterfaceWindowXML("game_setup.xml") {

}

void cScreenGameConfig::set_ids() {
    back_id                     = get_id("BackButton");
    ok_id                     = get_id("OkButton");
    revert_id                 = get_id("RevertButton");
    list_id                   = get_id("SettingsList");

    SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(revert_id, [this](){ m_Settings = dynamic_cast<cGameSettings&>(g_Game->settings()); this->init(false); });
    SetButtonCallback(ok_id, [this](){
        dynamic_cast<cGameSettings&>(g_Game->settings()) = m_Settings; pop_window();
    });
    SetListBoxDoubleClickCallback(list_id, [this](int sel){
        if(sel < 0) return;
        auto& setting = m_SettingsList.at(sel);
        switch(setting->value.which()) {
            case 0:
                input_choice(setting->name, {"Yes", "No"},
                        [this, setting](int c){ m_Settings.set_value(setting->tag, c == 0); init(false); });
                break;
            case 1:
                input_integer([this, setting](int c){ m_Settings.set_value(setting->tag, c); init(false); },
                        m_Settings.get_integer(setting->tag));
                break;
            case 2:
            {
                input_string([this, setting](const std::string& v){
                    float val = std::strtof(v.c_str(), nullptr);
                    m_Settings.set_value(setting->tag, val);
                    init(false); });
                break;
            }case 3:
            {
                input_string([this, setting](const std::string& v){
                    float val = std::strtof(v.c_str(), nullptr);
                    val /= 100.f;
                    m_Settings.set_value(setting->tag, sPercent(val));
                    init(false); });
                break;
            }
            case 4:
                input_string([this, setting](std::string v){ m_Settings.set_value(setting->tag, std::move(v)); init(false); });
                break;
        }
    });
}
