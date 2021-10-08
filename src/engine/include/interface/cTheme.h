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

#ifndef WM_CTHEME_H
#define WM_CTHEME_H

#include <string>
#include <unordered_map>
#include "sColor.h"

class cTheme {
public:
    const std::string& directory() const;
    const std::string& image_dir() const;
    int base_width() const;
    int base_height() const;
    int list_scroll() const;
    int text_scroll() const;

    std::string normal_font() const;

    void load(std::string source);
    void set_screen_size(int width, int height);

    std::string get_image(const std::string& dir, const std::string& filename) const;

    /// Tries to get the color for a UI element. If lookup is not found, fallback is returned
    sColor get_color(const std::string& lookup, const sColor& fallback) const;

    // queries
    /// Calculate an x coordinate in screen space
    int calc_x(int relative_x) const;
    /// Calculate a y coordinate in screen space
    int calc_y(int relative_y) const;
    /// Calculate a width in screen space
    int calc_w(int relative_w) const;
    /// Calculate the height in screen space
    int calc_h(int relative_h) const;

private:
    std::string m_Directory;
    std::string m_ImageDirectory;

    int m_BaseWidth;
    int m_BaseHeight;
    int m_ScreenWidth;
    int m_ScreenHeight;

    std::string m_Font;

    int m_ListScroll;
    int m_TextScroll;

    std::unordered_map<std::string, sColor> m_Colors;
};

#endif //WM_CTHEME_H
