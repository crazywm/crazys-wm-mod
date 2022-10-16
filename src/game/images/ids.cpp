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

#include "ids.h"
#include "utils/lookup.h"

const std::array<const char*, (int)EImageBaseType::NUM_TYPES>& get_imgtype_names() {
    static std::array<const char*, (int)EImageBaseType::NUM_TYPES> names {
            "anal", "bdsm", "spanking", "sex", "beast", "tentacle", "oral",
            "titty", "hand", "foot", "deepthroat", "dildo", "strapon",
            "sixtynine", "lick", "finger", "fisting", "suckballs", "rape", "rapebeast", "cumshot", "creampie",
            "cowgirl", "revcowgirl", "doggy", "missionary", "scissor",
            "strip", "kiss", "fondle", "chastity", "ecchi", "nude", "bunny", "escort", "massage", "milk", "dom", "sub",
            "maid", "sing", "wait", "card", "nurse", "piano", "music", "stagehand",
            "crystalpurifier", "director", "cameramage", "studiocrew", "security", "cook", "bake",
            "farm", "herd", "advertise",
            "sport", "study", "teacher", "combat", "magic", "bed", "rest", "bath", "swim",
            "shop", "craft", "dance", "dinner", "drinks",
            "birthhuman", "birthbeast",
            "refuse", "death", "profile", "portrait", "jail", "torture", "formal", "presented",
            "brand", "puppygirl", "ponygirl", "catgirl"
    };
    return names;
}

const id_lookup_t<EImageBaseType>& get_image_lookup() {
    static auto lookup = create_lookup_table<EImageBaseType>(get_imgtype_names());
    return lookup;
}

EImageBaseType get_image_id(const std::string& name) {
    return lookup_with_error(get_image_lookup(), name, "Trying to get invalid Image Type");
}

const char* get_image_name(EImageBaseType image) {
    return get_imgtype_names().at((int)image);
}

const std::array<const char*, (int)ESexParticipants::COUNT>& get_participant_names() {
    static std::array<const char*, (int)ESexParticipants::COUNT> names {
            "solo", "lesbian", "hetero", "ffm", "mmf", "gangbang", "lesgroup",
            "orgy", "other", "any", "anygroup"
    };
    return names;
}

const id_lookup_t<ESexParticipants>& get_participant_lookup() {
    static auto lookup = create_lookup_table<ESexParticipants>(get_participant_names());
    return lookup;
}

ESexParticipants get_participant_id(const std::string& name) {
    return lookup_with_error(get_participant_lookup(), name, "Trying to get invalid ESexParticipants");
}

const char* get_participant_name(ESexParticipants participant) {
    return get_participant_names().at((int)participant);
}


const std::array<const char*, (int)EImagePresets::COUNT>& get_image_preset_names() {
    static std::array<const char*, (int)EImagePresets::COUNT> names {
            "group", "ffm", "mmf", "gangbang", "orgy", "blowjob", "lesbian", "eatout", "les69", "masturbate",
            "mast_dildo", "bukkake", "group_bdsm"
    };
    return names;
}

const id_lookup_t<EImagePresets>& get_image_preset_lookup() {
    static auto lookup = create_lookup_table<EImagePresets>(get_image_preset_names());
    return lookup;
}

EImagePresets get_image_preset_id(const std::string& name) {
    return lookup_with_error(get_image_preset_lookup(), name, "Trying to get invalid EImagePresets");
}

const char* get_image_preset_name(EImagePresets preset) {
    return get_image_preset_names().at((int)preset);
}

template<>
ESexParticipants convert_from_text<ESexParticipants>(const std::string& name) {
    return get_participant_id(name);
}

ETriValue parse_tri_value(const std::string& source) {
    if(iequals(source, "yes")) {
        return ETriValue::Yes;
    } else if (iequals(source, "no")) {
        return ETriValue::No;
    }  else if (iequals(source, "maybe")) {
        return ETriValue::Maybe;
    } else {
        g_LogFile.warning("image", "Expected 'yes', 'no', or 'maybe', got '", source, "'");
        throw std::invalid_argument("Expected  'yes', 'no', or 'maybe', got: " + source);
    }
}

std::ostream& print_tri_flag(std::ostream& target, ETriValue v, const char* val) {
    switch (v) {
        case ETriValue::Yes:
            target << '[' << val << "] ";
            return target;
        case ETriValue::Maybe:
            target << "[?" << val << "] ";
            return target;
        case ETriValue::No:
            return target;
    }
}

std::string print_tri_flag(ETriValue v, const char* val) {
    std::stringstream target;
    print_tri_flag(target, v, val);
    return target.str();
}
