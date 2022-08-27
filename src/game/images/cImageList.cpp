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

#include "cImageList.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/FileList.h"
#include "utils/string.hpp"
#include <tinyxml2.h>
#include <regex>

auto cImageList::get_begin(EImageBaseType image_type) const -> const_iterator_t {
    return m_FileNames.cbegin() + m_Offsets.at((int)image_type);
}

auto cImageList::get_end(EImageBaseType image_type) const -> const_iterator_t {
    return m_FileNames.cbegin() + m_Offsets.at((int)image_type + 1);
}

cImageList::cImageList(const std::vector<records_vector_t>& source) {
    m_Offsets.resize(source.size() + 1);
    m_Offsets[0] = 0;
    // first determine the offsets and the size of the allocation we need to make
    auto offset = m_Offsets.begin();
    for(auto& data : source) {
        int old = *offset;
        *(++offset) = old + static_cast<int>(data.size());
    }

    // then do the actual insertion
    m_FileNames.reserve(m_Offsets.back());
    for(auto& data : source) {
        std::copy(std::begin(data), std::end(data), std::back_inserter(m_FileNames));
    }
}

namespace {
    bool parse_yesno(const std::string& source) {
        if(iequals(source, "yes")) {
            return true;
        } else if (iequals(source, "no")) {
            return false;
        } else {
            g_LogFile.warning("image", "Expected 'yes' or 'no', got '", source, "'");
            throw std::invalid_argument("Expected 'yes' or 'no', got: " + source);
        }
    }

    template<class F>
    void read_record_from_entry(const tinyxml2::XMLElement& entry, F&& callback) {
        std::string file = GetStringAttribute(entry, "File");
        std::string fallback = GetDefaultedStringAttribute(entry, "Fallback", "no");
        std::string source = GetDefaultedStringAttribute(entry, "Source", "");

        ETriValue pregnant = parse_tri_value(GetStringAttribute(entry, "Pregnant"));
        ETriValue futa = parse_tri_value(GetStringAttribute(entry, "Futa"));
        ETriValue tied = parse_tri_value(GetStringAttribute(entry, "TiedUp"));
        ESexParticipants part = get_participant_id( GetDefaultedStringAttribute(entry, "Participants", "any"));
        std::string image_types = GetStringAttribute(entry, "Type");
        std::vector<std::string> split_types;
        split(split_types, image_types, [](char c ){ return c == ';'; });
        for(auto&& type : split_types) {
            sImageSpec spec{get_image_id(type), part, pregnant, futa, tied};
            callback(sImageRecord{file, std::move(source), spec, parse_yesno(fallback)});
        }
    }
}

void cImageLoader::load_file_name_matchers(const std::string& spec_file) {
    auto doc = LoadXMLDocument(spec_file);
    auto& root = *doc->RootElement();

    for(auto& element : IterateChildElements(root, "FileType")) {
        try {
            sImageSpec spec = sImageSpec::load_from_xml(element);
            for (auto& c: IterateChildElements(element, "Pattern")) {
                m_FileNamePatterns.push_back(sFileNameMatcher{c.GetText(), spec});
            }
        } catch (std::exception& error) {
            g_LogFile.error("images", "Could not load pattern matcher from ", spec_file, " on line ", element.GetLineNum());
        }
    }
}


cImageList cImageLoader::load_images(const std::string& base_path) {
    // Option 1: The pack as an images.xml file
    FileList records(DirPath(base_path.c_str()), "images*.xml");
    if(records.size() > 0) {
        for (int i = 0; i < (int) EImageBaseType::NUM_TYPES; ++i) {
            m_RecordsBuffer[i].clear();
        }

        for(int rec_id = 0; rec_id < records.size(); ++rec_id) {
            std::string img = records[rec_id].full();
            auto doc = LoadXMLDocument(img);
            for (auto& entry: IterateChildElements(*doc->RootElement(), "Image")) {
                try {
                    read_record_from_entry(entry, [&](const sImageRecord& rec){
                        m_RecordsBuffer[(int)rec.Attributes.BasicImage].push_back(rec);
                    });

                } catch (const std::exception& error) {
                    g_LogFile.error("image", "Error in image specification in ", img, ": ", error.what(), ". Element start on line ", entry.GetLineNum());
                }
            }
        }
    } else {
        g_LogFile.warning("image", "Image folder '", base_path, "' does not provide images.xml");
        // first get the list of all files
        auto result = FileList(base_path.c_str(), "*.*");
        m_FileNameBuffer.resize(result.size());
        for (int i = 0; i < result.size(); ++i) {
            m_FileNameBuffer[i] = result[i].leaf();
        }

        // then match the files against all patterns to sort
        image_types_from_file_names();
    }

    return cImageList(m_RecordsBuffer);
}

void cImageLoader::image_types_from_file_names() {
    for (int i = 0; i < (int) EImageBaseType::NUM_TYPES; ++i) {
        m_RecordsBuffer[i].clear();
    }
    for (auto& spec: m_FileNamePatterns) {
        std::regex pattern(spec.Pattern, std::regex::icase | std::regex::ECMAScript | std::regex::optimize);
        for (auto& file: m_FileNameBuffer) {
            if (std::regex_match(file, pattern)) {
                m_RecordsBuffer[(int)spec.Attributes.BasicImage].push_back(
                        sImageRecord{file, "", spec.Attributes, false});
            }
        }
    }
}

cImageLoader::cImageLoader() {
    m_RecordsBuffer.resize((int)EImageBaseType::NUM_TYPES);
}

const cImageList& cImageLoader::lookup_files(const std::string& base_path) {
    auto lookup = m_PathCache.find(base_path);
    if(lookup != m_PathCache.end()) {
        return lookup->second;
    } else {
        auto inserted = m_PathCache.insert(std::make_pair(base_path, load_images(base_path)));
        return inserted.first->second;
    }
}
