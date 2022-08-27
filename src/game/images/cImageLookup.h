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

#ifndef WM_CIMAGELOOKUP_H
#define WM_CIMAGELOOKUP_H

#include "sImageSpec.h"

#include <unordered_map>
#include <vector>
#include <boost/optional.hpp>
#include <random>
#include "cImageList.h"

template<class T>
class cTransitionMatrix {
public:
    using EnumType = T;

    cTransitionMatrix();
    int from_to(T from, T to) const;
    void set_from_to(T from, T to, int value);
    void fill(int value);
private:
    constexpr static const int NUM_ELEMENTS = static_cast<int>(T::COUNT);
    std::array<int, NUM_ELEMENTS * NUM_ELEMENTS> m_Values;
};

class cImageLookup {
public:
    cImageLookup(std::string def_img_path, const std::string& spec_file);
    using ScoredImageRef = std::pair<const sImageRecord*, int>;
    std::string find_image(const std::string& base_path, const sImageSpec& spec, std::uint64_t seed);
    std::vector<ScoredImageRef> find_images(const std::string& base_path, sImageSpec spec, int cutoff=0);

    const std::string& get_display_name(EImageBaseType image);
    sImageRecord get_image_info(const std::string& base_path, const std::string& image_path);

    struct sFallbackData {
        EImageBaseType NewImageType;
        boost::optional<ESexParticipants> TargetParticipants;
        ESexParticipants SourceParticipants;
        boost::optional<ETriValue> TargetTiedUp;
        int Cost = 1;
    };
    struct sImgTypeInfo {
        std::string Display;
        std::vector<sFallbackData> Fallbacks;
    };

    void load_file_name_matchers(const std::string& spec_file);
    void load_transition_costs(const std::string& file_name);
    void load_image_spec(const std::string& file_name);

    const cImageList& lookup_files(const std::string& base_path);
private:
    const sImageRecord* find_image_internal(const std::string& base_path, const sImageSpec& spec, int max_cost,
                                            std::uint64_t seed);

    template<class T, class U>
    void find_image_internal_imp(const std::string& base_path, const sImageSpec& spec, T&& callback, U&& stopping);

    std::vector<sImgTypeInfo> m_ImageTypes;
    std::string m_DefaultPath;

    cImageLoader m_Loader;

    int m_CostCutoff;   // if the lookup cost exceeds this value, default images will be used

    // transition matrices
    cTransitionMatrix<ESexParticipants> m_ParticipantTransition;

    int match_cost(const sImageSpec& spec, const sImageSpec& record) const;
};

#endif //WM_CIMAGELOOKUP_H
