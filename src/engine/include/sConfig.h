/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Development Team are defined as the game's coders
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

#include <string>
#include "utils/cKeyValueStore.h"

struct sConfigData;

class cConfig
{
    static    sConfigData    *data;
public:
    cConfig();

    struct font_data {
        const std::string&    normal();
    } fonts;

    struct Folders {
        const std::string&    characters();
        const std::string&    saves();
        const std::string&    items();
        const std::string&    defaultimageloc();
        bool                  backupsaves();
        bool                  preferdefault();
    } folders;

    struct Resolution{
        const std::string& resolution();
        int                width();
        int                height();
        bool               fullscreen();
        int                list_scroll();
        int                text_scroll();
    } resolution;

    void set_value(const char* id, std::string value);
    void set_value(const char* id, bool value);

    void save();
};

