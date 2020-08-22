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
#include "cScreenSettings.h"
#include "utils/FileList.h"
#include "CLog.h"
#include "sConfig.h"

extern cConfig cfg;

void cScreenSettings::set_ids()
{
    back_id            = get_id("BackButton", "Back");
    ok_id              = get_id("OkButton");
    revert_id          = get_id("RevertButton");

    // Folders
    characters_id      = get_id("Characters");
    saves_id           = get_id("Saves");
    defaultimages_id   = get_id("DefaultImages");
    items_id           = get_id("Items");
    backupsaves_id     = get_id("BackupSaves");
    preferdefault_id   = get_id("PreferDefault");

    theme_id           = get_id("ThemeList");
    fullscreen_id      = get_id("Fullscreen");

    SetButtonNavigation(back_id, "Main Menu");
    SetButtonCallback(revert_id, [this]() { init(false); });
    SetButtonCallback(ok_id, [this]() {
        update_settings();
        pop_window();
    });

    auto themes = FileList::ListSubdirs("Resources/Interface");
    for(auto& theme : themes) {
        if(theme == cfg.resolution.resolution()) {
            AddToListBox(theme_id, 1, theme, COLOR_DARKBLUE);
        } else {
            AddToListBox(theme_id, 0, theme);
        }
    }
    SetSelectedItemInList(theme_id, 1, false, true);
}

cScreenSettings::cScreenSettings() : cInterfaceWindowXML("settings.xml")
{
}

void cScreenSettings::init(bool back)
{
    Focused();
    if (!back)
    {
        std::stringstream ss;
        SetEditBoxText(characters_id, cfg.folders.characters());
        SetEditBoxText(saves_id, cfg.folders.saves());
        SetEditBoxText(defaultimages_id, cfg.folders.defaultimageloc());
        SetEditBoxText(items_id, cfg.folders.items());
        SetCheckBox(backupsaves_id, cfg.folders.backupsaves());
        SetCheckBox(preferdefault_id, cfg.folders.preferdefault());
        SetCheckBox(fullscreen_id, cfg.resolution.fullscreen());
        SetSelectedItemInList(theme_id, 1, false, true);
    }
}

void cScreenSettings::update_settings()
{
    cfg.set_value("folders.characters", GetEditBoxText(characters_id));
    cfg.set_value("folders.saves", GetEditBoxText(saves_id));
    cfg.set_value("folders.default_images", GetEditBoxText(defaultimages_id));
    cfg.set_value("folders.items", GetEditBoxText(items_id));

    cfg.set_value("folders.backup_saves", IsCheckboxOn(backupsaves_id));
    cfg.set_value("folders.prefer_defaults", IsCheckboxOn(preferdefault_id));

    cfg.set_value("interface.theme", GetSelectedTextFromList(theme_id));
    cfg.set_value("interface.fullscreen", IsCheckboxOn(fullscreen_id));

    cfg.save();
}
