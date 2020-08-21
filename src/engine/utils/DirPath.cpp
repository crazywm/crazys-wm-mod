/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#include "utils/DirPath.h"
#include "CLog.h"

/*
 * kind of trivial...
 */
#ifdef LINUX
#include <dirent.h>
#include <cerrno>
#include <cstring>

const char    DirPath::sep[] = "/";

bool DirPath::is_directory() const {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
       return false;
    } else {
        /* opendir() failed for some other reason. */
        g_LogFile.warning("io", "Error (", errno, ") when opening '", path, "': ", std::strerror(errno));
        return false;
    }
}

#else
#include <shlwapi.h>
const char    DirPath::sep[] = "\\";

bool DirPath::is_directory() const {
    return PathIsDirectoryA(path.c_str());
}

#endif

ImagePath::ImagePath(const std::string& filename)
        : DirPath()
{
    (*this) << "Resources" << "Images" << filename;
}

ButtonPath::ButtonPath(const std::string& filename)
        : DirPath()
{
    (*this) << "Resources" << "Buttons" << filename;
}
