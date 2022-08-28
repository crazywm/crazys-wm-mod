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

#include "cImageLookup.h"
#include "utils/FileList.h"
#include "utils/streaming_random_selection.hpp"
#include "xml/util.h"
#include "xml/getattr.h"
#include "CLog.h"
#include <tinyxml2.h>
#include <cassert>
#include <set>
#include <utility>
#include <tinyxml2.h>
#include "xml/util.h"
#include "utils/string.hpp"
#include "utils/algorithms.hpp"

namespace {
    const int IMAGE_SELECTION_TOLERANCE = 5;
    const int IMAGE_MINIMUM_VARIETY = 5;
}

template<class T>
cTransitionMatrix<T>::cTransitionMatrix() {
    // disable all transitions -- we use a very large value here, but not limits<int>::max() to make sure we can still
    // safely add values later on. regular costs are in range [0, 100], so this value is large enough.
   fill(10000);
}

template<class T>
int cTransitionMatrix<T>::from_to(T from, T to) const {
    assert((int)from >= 0 && (int)to >= 0);
    return m_Values[(int)from * NUM_ELEMENTS + (int)to];
}

template<class T>
void cTransitionMatrix<T>::set_from_to(T from, T to, int value) {
    assert(value >= 0);
    assert((int)from >= 0 && (int)to >= 0);
    m_Values[(int)from * NUM_ELEMENTS + (int)to] = value;
}

template<class T>
void cTransitionMatrix<T>::fill(int value) {
    m_Values.fill(value);
    // except the diagonal, which is zero
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        set_from_to((T)i, (T)i, 0);
    }
}


namespace {
    boost::optional<ESexParticipants> parse_participant_fallback(const std::string& fb) {
        if(iequals(fb, "keep")) {
            return boost::none;
        } else {
            return get_participant_id(fb);
        }
    }

    boost::optional<ETriValue> parse_optional_trival(const std::string& fb) {
        if(iequals(fb, "keep")) {
            return boost::none;
        } else {
            return parse_tri_value(fb);
        }
    }

    cImageLookup::sFallbackData parse_fallback(const tinyxml2::XMLElement& c) {
        auto target_participant = parse_participant_fallback(GetDefaultedStringAttribute(c, "Participants", "keep"));
        auto source_participant = get_participant_id(GetDefaultedStringAttribute(c, "RequiredParticipants", "any"));
        auto target_tied = parse_optional_trival(GetDefaultedStringAttribute(c, "TiedUp", "keep"));
        return {get_image_id(c.GetText()),
                target_participant,
                source_participant,
                target_tied,
                std::max(0, c.IntAttribute("Cost", 100)),
        };
    }
}

const cImageList& cImageLookup::lookup_files(const std::string& base_path) {
    return m_Loader.lookup_files(base_path);
}

namespace {
    sImageSpec apply_fallback(const sImageSpec& source, const cImageLookup::sFallbackData& fallback) {
        sImageSpec new_spec = source;
        new_spec.BasicImage = fallback.NewImageType;
        // if we have a specific new participant, use that
        if(fallback.TargetParticipants) {
            new_spec.Participants = fallback.TargetParticipants.value();
        }
        if(fallback.TargetTiedUp) {
            new_spec.IsTied = fallback.TargetTiedUp.value();
        }
        return new_spec;
    }
}

template<class T, class U>
void cImageLookup::find_image_internal_imp(const std::string& base_path, const sImageSpec& spec, T&& inner_callback, U&& stopping_criterion) {
    auto& haystack = lookup_files(base_path);
    if(haystack.empty()) return;

    struct sQueueEntry {
        int Cost;
        sImageSpec Node;
        bool operator<(const sQueueEntry& other) const {
            return Cost < other.Cost;
        }
    };

    // OK, need to go to the fallbacks. This is a BFS
    std::set<sImageSpec> visited;
    visited.insert(spec);

    std::multiset<sQueueEntry> queue;
    queue.insert({0, spec});

    // for profile images, also look up based on outfit.
    /// TODO this mechanism needs refinement!!!
    /*
    if(spec.BasicImage == EImageBaseType::PROFILE) {
        switch (spec.Outfit) {
            case EOutfitType::ARMOUR:
                queue.insert({0, EImageBaseType::COMBAT, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::FETISH:
                queue.insert({0, EImageBaseType::DOM, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::MAID:
                queue.insert({0, EImageBaseType::MAID, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::TEACHER:
                queue.insert({0, EImageBaseType::TEACHER, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::NURSE:
                queue.insert({0, EImageBaseType::NURSE, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::FORMAL:
                queue.insert({0, EImageBaseType::FORMAL, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::SWIMWEAR:
                queue.insert({0, EImageBaseType::SWIM, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::LINGERIE:
                queue.insert({0, EImageBaseType::ECCHI, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::FARMER:
                queue.insert({0, EImageBaseType::FARM, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::SORCERESS:
                queue.insert({0, EImageBaseType::MAGIC, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::NUDE:
                queue.insert({0, EImageBaseType::NUDE, spec.Participants, spec.IsPregnant});
                break;
            case EOutfitType::SCHOOLGIRL:
            case EOutfitType::SPORTSWEAR:
            case EOutfitType::RAGS:
            case EOutfitType::CASUAL:
            case EOutfitType::NONE:
            case EOutfitType::COUNT:
                break;
        }
    }
    */
    g_LogFile.info("image", "Looking for image: ", get_image_name(spec.BasicImage), " ",
                   print_tri_flag(spec.IsTied, "tied"), " ",
                   print_tri_flag(spec.IsFuta, "futa"), " ",
                   print_tri_flag(spec.IsPregnant, "preg"));
    while(!queue.empty()) {
        auto it = queue.begin();
        g_LogFile.debug("image", "  Look up '", get_image_name(it->Node.BasicImage), "' with participants ",
                get_participant_name(it->Node.Participants), " at cost ", it->Cost);
        visited.insert(it->Node);

        for(const sImageRecord& image: haystack.iterate(it->Node.BasicImage)) {
            int cost = it->Cost + match_cost(it->Node, image.Attributes);
            if(image.IsFallback)
                cost += IMAGE_SELECTION_TOLERANCE + 1;
            inner_callback(&image, cost);
        }

        // insert does not invalidate iterators for set, so it is still fine
        for(auto& fbs : m_ImageTypes.at((int)it->Node.BasicImage).Fallbacks) {
            int add_cost = 0;
            if(fbs.SourceParticipants != ESexParticipants::ANY) {
                add_cost += m_ParticipantTransition.from_to(it->Node.Participants, fbs.SourceParticipants);
            }

            // std::cout << "CONSIDER FALLBACK: " << get_display_name(it->Node.BasicImage) << " -> " << get_display_name(fbs.NewImageType) << "\n";
            sImageSpec fb_node = apply_fallback(it->Node, fbs);
            // std::cout << (int)fb_node.IsTied << " " << it->Cost + fbs.Cost + add_cost << "\n";
            if(visited.count(fb_node) > 0) continue;
            queue.insert({it->Cost + fbs.Cost + add_cost, fb_node});
        }

        auto next = it;
        ++next;
        if(next != queue.end()) {
            // if the next trial would be less desired, and we already have found one, stop the search
            if(stopping_criterion(next->Cost)) {
                return;
            }
        }
        queue.erase(it);
    }
}

cImageLookup::cImageLookup(std::string def_img_path, const std::string& spec_file) :
    m_DefaultPath(std::move(def_img_path)), m_CostCutoff(250)
{
    m_ImageTypes.resize((int)EImageBaseType::NUM_TYPES);
    load_image_spec(spec_file);
}

std::string cImageLookup::find_image(const std::string& base_path, const sImageSpec& spec, std::uint64_t seed) {
    auto make_path = [](const std::string& file_name, const std::string& directory){
        DirPath path(directory.c_str());
        path << file_name;
        return path.str();
    };

    auto result = find_image_internal(base_path, spec, m_CostCutoff, seed);
    if (!result) {
        // still nothing? retry with default images. Here we go to the end with fallbacks.
        result = find_image_internal(m_DefaultPath, spec, std::numeric_limits<int>::max(), seed);
        if (!result) {
            g_LogFile.warning("image", "Could not find a default image for ", get_image_name(spec.BasicImage));
            return "";
        } else {
            return make_path(result->FileName, m_DefaultPath);
        }
    } else {
        return make_path(result->FileName, base_path);
    }
}

const sImageRecord* cImageLookup::find_image_internal(const std::string& base_path, const sImageSpec& spec, int max_cost,
                                              std::uint64_t seed) {
    std::minstd_rand rng(seed);
    std::vector<ScoredImageRef> candidates = find_images(base_path, spec, max_cost);
    if(candidates.empty()) return nullptr;
    int best_cost = candidates.front().second;

    // Select an image. We have to selection processes, the first acts within the tolerance, the second outside it
    RandomSelectorV2<ScoredImageRef> fallback_selector;
    RandomSelectorV2<ScoredImageRef> selector;
    for(auto& cnd : candidates) {
        int offset = std::max(0, cnd.second - best_cost);
        float weight = std::exp(-float(offset) / 100);
        if(offset > IMAGE_SELECTION_TOLERANCE) {
            fallback_selector.process(rng, cnd, 0, weight);
        } else {
            selector.process(rng, cnd, 0, weight);
        }
    }

    // if we have a fallback, add this to the regular images
    if(fallback_selector.selection()) {
        auto cnd = fallback_selector.selection().value();
        int offset = std::max(0, cnd.second - best_cost);
        float weight = std::exp(-float(offset) / 100);
        selector.process(rng, cnd, 0, weight);
    }

    auto selection = selector.selection().value();
    g_LogFile.info("image", "Selected image ", selection.first->FileName, " of type ",
                   get_display_name(selection.first->Attributes.BasicImage), " with score ", selection.second,
                   ". chance: ", int(100 * std::exp(-float(std::max(0, selection.second - best_cost)) / 100) / selector.weight()),
                   "%, best score: ", candidates.front().second, ", total weight: ", selector.weight());

    return selection.first;
}

std::vector<cImageLookup::ScoredImageRef> cImageLookup::find_images(const std::string& base_path, sImageSpec spec, int cutoff) {

    std::vector<ScoredImageRef> candidates;
    int best_cost = std::numeric_limits<int>::max();
    auto inner_callback = [&](const sImageRecord* image, int cost) {
        assert(cost >= 0);
        if(cost - IMAGE_SELECTION_TOLERANCE > cutoff) return;
        candidates.emplace_back(image, cost);
        if(cost < best_cost) {
            best_cost = cost;
        }
    };
    auto stop = [&](int cost_bound) {
        if(cost_bound - IMAGE_SELECTION_TOLERANCE > cutoff) return true;

        if(candidates.size() < IMAGE_MINIMUM_VARIETY) return false;
        // we have at least 10 candidates. Find the 10 best ones.
        auto partitioner = begin(candidates) + IMAGE_MINIMUM_VARIETY;
        std::nth_element(begin(candidates), partitioner, end(candidates));

        // if we have no chance of improving this selection, and we have all images close to the best one,
        // we are done.
        if(partitioner->second < cost_bound && best_cost + IMAGE_SELECTION_TOLERANCE < cost_bound) {
            return true;
        }
        return false;
    };

    find_image_internal_imp(base_path, spec, inner_callback, stop);

    std::sort(begin(candidates), end(candidates), [](const ScoredImageRef& a, const ScoredImageRef& b){
        return a.second < b.second;
    });

    // prune the candidate set
    if(candidates.size() < IMAGE_MINIMUM_VARIETY) {
        return candidates;
    } else {
        auto partitioner = begin(candidates) + IMAGE_MINIMUM_VARIETY;
        int threshold = std::max(best_cost + IMAGE_SELECTION_TOLERANCE, partitioner->second);
        erase_if(candidates, [threshold](const ScoredImageRef& candidate) {
            return candidate.second > threshold;
        });
        return candidates;
    }
}

const std::string& cImageLookup::get_display_name(EImageBaseType image) {
    return m_ImageTypes.at((int)image).Display;
}

namespace {
        /// Gets the cost for an image with pregnancy value `image` in case the looked-for value is `search`.
        int get_preg_cost(ETriValue search, ETriValue image) {
            if(search == image) return 0;
            if(search == ETriValue::Yes) {
                if(image == ETriValue::Maybe) return 5;
                else return 50;
            } else if (search == ETriValue::No) {
                if(image == ETriValue::Maybe) return 1;
                else return 150;
            } else {
                // maybe
                return 0;
            }
        }

        int get_futa_cost(ETriValue search, ETriValue image) {
            if(search == image) return 0;
            if(search == ETriValue::Yes) {
                if(image == ETriValue::Maybe) return 5;
                else return 75;
            } else if (search == ETriValue::No) {
                if(image == ETriValue::Maybe) return 1;
                else return 250;
            } else {
                // maybe
                return 0;
            }
        }

        int get_tied_cost(ETriValue search, ETriValue image) {
            if(search == image) return 0;
            if(search == ETriValue::Yes) {
                if(image == ETriValue::Maybe) return 5;
                else return 100;
            } else if (search == ETriValue::No) {
                if(image == ETriValue::Maybe) return 25;
                else return 100;
            } else {
                // maybe
                return 0;
            }
        }
    }

int cImageLookup::match_cost(const sImageSpec& node, const sImageSpec& record) const {
    int cost = get_preg_cost(node.IsPregnant, record.IsPregnant);
    cost += get_futa_cost(node.IsFuta, record.IsFuta);
    cost += get_tied_cost(node.IsTied, record.IsTied);

    // Participant component
    cost += m_ParticipantTransition.from_to(node.Participants, record.Participants);
    return cost;
}

namespace {
    template<class T>
    void load_transitions(cTransitionMatrix<T>& matrix, const tinyxml2::XMLElement& source) {
        for(auto& entry : IterateChildElements(source, "Entry")) {
            auto from = convert_from_text<T>(GetStringAttribute(entry, "From"));
            auto to = convert_from_text<T>(GetStringAttribute(entry, "To"));
            auto cost = GetIntAttribute(entry, "Cost", 0, 1000);
            matrix.set_from_to(from, to, cost);
        }
    }
}

void cImageLookup::load_transition_costs(const std::string& file_name) {
    auto doc = LoadXMLDocument(file_name);
    auto& root = *doc->RootElement();
    for(auto& el : IterateChildElements(root, "Transition")) {
        std::string which = GetStringAttribute(el, "Name");
        int def_value = el.IntAttribute("Default", 10000);
        if(which == "Participants") {
            m_ParticipantTransition.fill(def_value);
            load_transitions(m_ParticipantTransition, el);
        }
    }
}

sImageRecord cImageLookup::get_image_info(const std::string& base_path, const std::string& image_path) {
    auto& files = lookup_files(base_path);
    auto found = std::find_if(files.begin(), files.end(), [&](const sImageRecord& rec) {
        return rec.FileName == image_path;
    });
    if(found != files.end()) {
        return *found;
    }
    return {};
}

void cImageLookup::load_file_name_matchers(const std::string& spec_file) {
    m_Loader.load_file_name_matchers(spec_file);
}

void cImageLookup::load_image_spec(const std::string& file_name) {
    auto doc = LoadXMLDocument(file_name);
    auto& root = *doc->RootElement();

    for(auto& element : IterateChildElements(root, "Type")) {
        std::string base_name = GetStringAttribute(element, "Name");
        EImageBaseType img = get_image_id(base_name);
        sImgTypeInfo& info = m_ImageTypes.at((int)img);
        for(auto& c : IterateChildElements(element)) {
            std::string el_name = c.Value();
            if(el_name == "Display") {
                info.Display = c.GetText();
            } else if (el_name == "Fallback") {
                info.Fallbacks.push_back(parse_fallback(c));
            }
        }

        if(info.Display.empty())
            info.Display = base_name;
    }
}
