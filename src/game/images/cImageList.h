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

#ifndef WM_CIMAGELIST_H
#define WM_CIMAGELIST_H

#include <vector>
#include <string>
#include <unordered_map>
#include "ids.h"
#include "sImageSpec.h"

struct sImageRecord {
    std::string FileName;
    std::string Source;
    sImageSpec Attributes;
    bool IsFallback : 1;
};

class cImageList {
    using records_vector_t = std::vector<sImageRecord>;
public:
    explicit cImageList(const std::vector<records_vector_t>& source);

    using const_iterator_t = records_vector_t::const_iterator;

    const_iterator_t get_begin(EImageBaseType image_type) const;
    const_iterator_t get_end(EImageBaseType image_type) const;
    struct sImageRange {
        const cImageList* Target;
        EImageBaseType Image;

        const_iterator_t begin() const {
            return Target->get_begin(Image);
        }
        const_iterator_t end() const {
            return Target->get_end(Image);
        }

    };

    bool empty() const {
        return m_FileNames.empty();
    }

    records_vector_t::const_iterator begin() const { return m_FileNames.begin(); }
    records_vector_t::const_iterator end() const { return m_FileNames.end(); }

    sImageRange iterate(EImageBaseType img) const { return sImageRange{this, img}; }

private:
    records_vector_t m_FileNames;
    std::vector<int> m_Offsets;
};

class cImageLoader {
public:
    cImageLoader();
    void load_file_name_matchers(const std::string& spec_file);

    const cImageList& lookup_files(const std::string& base_path);
private:
    struct sFileNameMatcher {
        std::string Pattern;
        sImageSpec Attributes;
    };

    cImageList load_images(const std::string& base_path);
    void image_types_from_file_names();

    std::unordered_map<std::string, cImageList> m_PathCache;

    // cache variables, so we don't need to repeatedly allocate memory
    std::vector<std::string> m_FileNameBuffer;
    std::vector<std::vector<sImageRecord>> m_RecordsBuffer;
    std::vector<sFileNameMatcher> m_FileNamePatterns;
};

#endif //WM_CIMAGELIST_H
