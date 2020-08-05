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
#include "utils/FileList.h"
#include "CLog.h"
#include <string>

FileList::FileList(DirPath dp, const char *pattern) : folder( std::move(dp) )
{
    scan(pattern);
}

void FileList::scan(const char * pattern)
{
    files.clear();
    add(pattern);
}

#ifdef LINUX

#include <dirent.h>
#include <cerrno>
#include <regex.h>
#include <cstring>
#include <sys/stat.h>

static std::string& gsub(std::string& str, const char *pat_pt, const char *repl_pt)
{
    size_t pat_len = strlen(pat_pt);
    size_t repl_len = strlen(repl_pt);

    size_t siz = 0;
    for (int i = 0; i < 10; i++) {
        siz = str.find(pat_pt, siz);
        if (siz == std::string::npos) {
            break;
        }
        str.replace(siz, pat_len, repl_pt);
        siz += repl_len;
    }
    return str;
}

/*
* scan the directory, dp, looking for files that match the
* pattern, pattern.
*
* This is the LINUX version of this func. The windows one
* is underneath
*/

void FileList::add(const char *pattern)
{
    DIR        *dpt;
    struct dirent *dent;
    const char* base_path = folder.c_str();

    /*
     *    open the directory. Print an error to the console if it fails
     */
    if ((dpt = opendir(base_path)) == nullptr) {
        g_LogFile.error("engine", "Could not open directory '", base_path, "' (", errno, ")");
        return;
    }

    std::string        s_bp(folder.c_str());
    std::string        s_pat(pattern);
                //match from beginning of string - stops preg* images from being sorted into normal image groups
    s_pat = "^" + s_pat + "$"; 
    
    /*
    *    we'll need to match regular expressions against the file name
    *    to do that we need to turn the "." into "\\." sequences and similarly
    *    "*" into ".*"
    */
    gsub(s_pat, ".", "\\.");
    gsub(s_pat, "*", ".*");
    gsub(s_pat, "[", "\\[");
    
    /*
    *    now make a regex
    */
    regex_t r;
    int res = regcomp(&r, s_pat.c_str(), REG_NOSUB | REG_ICASE); //ignoring case so images work properly
    if(res != 0) {
        g_LogFile.error("regex", "Could not compile regex");
    }

    /*
    *    loop through the files
    */
    while ((dent = readdir(dpt)) != nullptr) {
        int nmatch = strlen(dent->d_name);
        if (regexec(&r, dent->d_name, nmatch, nullptr, 0) == REG_NOMATCH) {
            continue;
        }
        
        files.emplace_back(s_bp, std::string(dent->d_name));
    }
    regfree(&r);
    closedir(dpt);
}

std::vector<std::string> FileList::ListSubdirs(std::string path) {
    std::vector<std::string> result;
    DIR        *dpt;
    struct dirent *dent;
    const char* base_path = path.c_str();

    /*
     *    open the directory. Print an error to the console if it fails
     */
    if ((dpt = opendir(base_path)) == nullptr) {
        g_LogFile.error("engine", "Could not open directory '", base_path, "' (", errno, ")");
        return {};
    }

    struct stat buf{};
    while ((dent = readdir(dpt)) != nullptr) {
        if(stat((path + "/" + dent->d_name).c_str(), &buf) == -1) {
            g_LogFile.error("engine", "Could calling stat on '", path, "/", dent->d_name, "' (", errno, ")");
            continue;
        };
        if(S_ISDIR(buf.st_mode)) {
            std::string dir_name = dent->d_name;
            // exclude special directories
            if(dir_name.front() != '.') {
                result.emplace_back(dent->d_name);
            }
        }
    }
    return std::move(result);
}

#else
#include<windows.h>


void FileList::add(const char * pattern)
{
    WIN32_FIND_DATAA FindFileData;
    DirPath loc = folder.c_str();
    loc << pattern;
    std::string base = folder.c_str();
    std::string filename;
    HANDLE hFind = FindFirstFileA(loc.c_str(), &FindFileData);

    int i = 0;
    while (hFind != INVALID_HANDLE_VALUE) {
        filename = FindFileData.cFileName;
        FileListEntry tempfile(base, filename);
        files.push_back(tempfile);
        if (FindNextFileA(hFind, &FindFileData) == 0) {
            break;
        }
    }
    FindClose(hFind);
}

std::vector<std::string> FileList::ListSubdirs(std::string path) {
    // https://www.bfilipek.com/2019/04/dir-iterate.html#on-windows-winapi
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile((path + "/*").c_str(), &FindFileData);
    std::vector<std::string> result;
    if (hFind == INVALID_HANDLE_VALUE) {
        g_LogFile.error("engine", "FindFirstFile failed (", GetLastError(), ")");
        return {};
    }

    do {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if(FindFileData.cFileName[0] != '.') {
                result.emplace_back(FindFileData.cFileName);
            }
        }
    } while (FindNextFile(hFind, &FindFileData) != 0);

    FindClose(hFind);

    return std::move(result);
}

#endif