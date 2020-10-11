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
#pragma once
/*
 * #pragma once does the same thing as #ifndef __FOO_H etc etc
 */

#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <iomanip>

enum class ELogLevel {
    VERBOSE,            // written only to log file in verbose mode
    DEBUG,              // written only to log file
    INFO,               // written to file and stdout
    WARNING,            // written to file and to stderr
    ERROR               // written to file and stderr
};

class CLog
{
public:
    explicit CLog();

    // logging level
    void enable_verbose();
    void disable_verbose();

    /// General purpose logging function. Logs `data` with level `ll`
    /// on `channel`.
    template<class... Args>
    void log(const char* channel, ELogLevel ll, Args&&... data) {
        write_log_level(ll, channel);
        using expand_type = int[];
        (void)expand_type{0, (log_raw(ll, std::forward<Args>(data)), 0)...};
        end_log_entry(ll);
    }

    // legacy support -- try to reduce usage in the future
    template<class... Args>
    void log(ELogLevel ll, Args&&... data) {
        write_log_level(ll, nullptr);
        using expand_type = int[];
        (void)expand_type{0, (log_raw(ll, std::forward<Args>(data)), 0)...};
        end_log_entry(ll);
    }

    template<class... Args>
    void error(const char* channel, Args&&... data) {
        log(channel, ELogLevel::ERROR, std::forward<Args>(data)...);
    }

    template<class... Args>
    void warning(const char* channel, Args&&... data) {
        log(channel, ELogLevel::WARNING, std::forward<Args>(data)...);
    }

    template<class... Args>
    void info(const char* channel, Args&&... data) {
        log(channel, ELogLevel::INFO, std::forward<Args>(data)...);
    }

    template<class... Args>
    void debug(const char* channel, Args&&... data) {
        log(channel, ELogLevel::DEBUG, std::forward<Args>(data)...);
    }

    template<class... Args>
    void verbose(const char* channel, Args&&... data) {
        log(channel, ELogLevel::VERBOSE, std::forward<Args>(data)...);
    }

    template<class... Args>
    void debug_table(const char* channel, const char* header, Args&&... values) {
        write_log_level(ELogLevel::VERBOSE, channel);
        log_raw(ELogLevel::VERBOSE, header);
        log_raw(ELogLevel::VERBOSE, ":\n");
        write_table(ELogLevel::VERBOSE, std::forward<Args>(values)...);
        end_log_entry(ELogLevel::VERBOSE);
    }
private:
    template<class T>
    void log_raw(ELogLevel ll, T&& content) {
        for(auto& stream: get_streams(ll)) {
            (*stream) << content;
        }
    }

    // logging table entries for debug
    // TODO might turn this into a general utility
    template<class First, class... Args>
    void write_table(ELogLevel ll, const char* key, First&& first, Args&&... args) {
        for(auto& stream: get_streams(ll)) {
            write_table_key(*stream, key);
            (*stream) << first << '\n';
        }

        write_table(ll, std::forward<Args>(args)...);
    }

    void write_table_key(std::ostream& stream, const char* key);

    // end of recursion
    void write_table(ELogLevel ll) { }

    void write_log_level(ELogLevel ll, const char* channel);
    void end_log_entry(ELogLevel ll);

    //! Gets all the streams for the relevant log level.
    const std::vector<std::ostream*>& get_streams(ELogLevel ll);

    //! The target file for the logs.
    std::ofstream m_ofile;

    //! stream matching: Every output level will relay its message to all the streams
    //! given here.
    std::array<std::vector<std::ostream*>, 5> m_Streams;
};

extern CLog g_LogFile;
