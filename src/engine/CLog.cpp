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
#include <iostream>
#include <iomanip>
#include "CLog.h"
#include "Revision.h"

CLog::CLog() {
    m_ofile.open("gamelog.txt", std::fstream::out);
    if(!m_ofile.is_open()) {
        std::cerr << "Could not open game log file!" << std::endl;
    }

    m_ofile << VERSION_STRING << '\n';
    m_ofile << "------------------------------------------------------------------------------------------------------------------------" << std::endl;

    // set up streams
    // all modes log to the output file
    for(auto& s : m_Streams) {
        s.push_back(&m_ofile);
    }
    m_Streams[static_cast<int>(ELogLevel::NOTIFY)].push_back(&std::cout);
    m_Streams[static_cast<int>(ELogLevel::WARNING)].push_back(&std::cerr);
    m_Streams[static_cast<int>(ELogLevel::ERROR)].push_back(&std::cerr);
}

void CLog::write_log_level(ELogLevel ll, const char* channel) {
    for(auto& stream : get_streams(ll)) {
        (*stream) << std::setw(9) << std::left;
        switch (ll) {
            case ELogLevel::VERBOSE:
                (*stream) << "VERB:";
                break;
            case ELogLevel::DEBUG:
                (*stream) << "DEBUG:";
                break;
            case ELogLevel::NOTIFY:
                (*stream) << "INFO:";
                break;
            case ELogLevel::WARNING:
                (*stream) << "WARNING:";
                break;
            case ELogLevel::ERROR:
                (*stream) << "ERROR:";
        }

        (*stream) << std::setw(12);
        if(channel) {
            std::string channel_str = std::string("[") + channel + ']';
            (*stream) << channel_str;
        } else {
            (*stream) << ' ';
        }
    }
}

void CLog::end_log_entry(ELogLevel ll) {
    for(auto& stream: get_streams(ll)) {
        (*stream) << std::endl;
    }
}

const std::vector<std::ostream*>& CLog::get_streams(ELogLevel ll) {
    return m_Streams[static_cast<int>(ll)];
}

void CLog::write_table_key(std::ostream& stream, const char* key) {
    stream << std::setw(9) << "" << std::setw(11) << std::left << key <<"|" << std::setw(15) << std::right;
}

void CLog::disable_verbose() {
    m_Streams[static_cast<int>(ELogLevel::VERBOSE)].clear();
}

void CLog::enable_verbose() {
    m_Streams[static_cast<int>(ELogLevel::VERBOSE)].clear();
    m_Streams[static_cast<int>(ELogLevel::VERBOSE)].push_back(&m_ofile);
}
