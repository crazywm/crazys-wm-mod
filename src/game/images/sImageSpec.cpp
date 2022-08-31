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

#include "sImageSpec.h"
#include "xml/util.h"
#include "tinyxml2.h"
#include "xml/getattr.h"

namespace {
    ESexParticipants get_participant_for_preset(EImagePresets type) {
        switch(type) {
            case EImagePresets::GROUP:
                return ESexParticipants::GANGBANG;
            case EImagePresets::ORGY:
                return ESexParticipants::ORGY;
            case EImagePresets::LESBIAN:
                return ESexParticipants::LESBIAN;
            case EImagePresets::FFM:
                return ESexParticipants::THREESOME_FFM;
            case EImagePresets::MMF:
                return ESexParticipants::THREESOME_MMF;
            case EImagePresets::GANGBANG:
                return ESexParticipants::GANGBANG;
            case EImagePresets::LES69:
            case EImagePresets::EAT_OUT:
                return ESexParticipants::LESBIAN;
            case EImagePresets::BLOWJOB:
                return ESexParticipants::HETERO;
            case EImagePresets::MASTURBATE:
            case EImagePresets::MAST_DILDO:
                return ESexParticipants::SOLO;
            case EImagePresets::BUKKAKE:
            case EImagePresets::GROUP_BDSM:
                return ESexParticipants::GANGBANG;
            default:
                return ESexParticipants::ANY;
        }
    }

    EImageBaseType get_base_for_preset(EImagePresets type) {
        switch(type) {
            case EImagePresets::GROUP:
            case EImagePresets::ORGY:
            case EImagePresets::FFM:
            case EImagePresets::MMF:
            case EImagePresets::GANGBANG:
                return EImageBaseType::VAGINAL;
            case EImagePresets::LESBIAN:
                return EImageBaseType::LICK;
            case EImagePresets::LES69:
                return EImageBaseType::SIXTY_NINE;
            case EImagePresets::EAT_OUT:
            case EImagePresets::BLOWJOB:
                return EImageBaseType::ORAL;
            case EImagePresets::MASTURBATE:
                return EImageBaseType::FINGER;
            case EImagePresets::MAST_DILDO:
                return EImageBaseType::DILDO;
            case EImagePresets::BUKKAKE:
                return EImageBaseType::CUMSHOT;
            case EImagePresets::GROUP_BDSM:
                return EImageBaseType::BDSM;
            default:
                return EImageBaseType::PROFILE;
        }
    }

    ESexParticipants get_participant_for_type(EImageBaseType type) {
        switch (type) {
            case EImageBaseType::VAGINAL:
            case EImageBaseType::ANAL:
            case EImageBaseType::BDSM:
            case EImageBaseType::BEAST:
            case EImageBaseType::ORAL:
            case EImageBaseType::TITTY:
            case EImageBaseType::HAND:
            case EImageBaseType::FOOT:
            case EImageBaseType::DEEPTHROAT:
            case EImageBaseType::SUCKBALLS:
            case EImageBaseType::RAPE:
            case EImageBaseType::RAPE_BEAST:
            case EImageBaseType::STRAPON:
            case EImageBaseType::SIXTY_NINE:
            case EImageBaseType::LICK:
            case EImageBaseType::FINGER:
            case EImageBaseType::FISTING:
                return ESexParticipants::HETERO;
            case EImageBaseType::DILDO:
            case EImageBaseType::TENTACLE:
                return ESexParticipants::SOLO;
            default:
                return ESexParticipants::ANY;
        }
    }

    ETriValue get_tied_up_for_type(EImageBaseType type) {
        switch (type) {
            case EImageBaseType::BDSM:
            case EImageBaseType::SPANKING:
            case EImageBaseType::TORTURE:
            case EImageBaseType::JAIL:
            case EImageBaseType::PRESENTED:
            case EImageBaseType::RAPE:
            case EImageBaseType::RAPE_BEAST:
            case EImageBaseType::SUB:
            case EImageBaseType::CHASTITY:
            case EImageBaseType::TENTACLE:
                return ETriValue::Maybe;
            default:
                return ETriValue::No;
        }
    }
    ETriValue get_tied_up_for_preset(EImagePresets type) {
        switch (type) {
            case EImagePresets::GROUP_BDSM:
                return ETriValue::Maybe;
            default:
                return ETriValue::No;
        }
    }
}

EImageBaseType sImagePreset::base_image() const {
    if(Value < (int)EImageBaseType::NUM_TYPES) {
        return (EImageBaseType)Value;
    } else {
        return get_base_for_preset(EImagePresets(Value - (int)EImageBaseType::NUM_TYPES));
    }
}

ESexParticipants sImagePreset::participants() const {
    if(Value < (int)EImageBaseType::NUM_TYPES) {
        return get_participant_for_type((EImageBaseType) Value);
    } else {
        return get_participant_for_preset(EImagePresets(Value - (int)EImageBaseType::NUM_TYPES));
    }
}

ETriValue sImagePreset::tied_up() const {
    if(Value < (int)EImageBaseType::NUM_TYPES) {
        return get_tied_up_for_type((EImageBaseType) Value);
    } else {
        return get_tied_up_for_preset(EImagePresets(Value - (int)EImageBaseType::NUM_TYPES));
    }
}

sImageSpec sImageSpec::load_from_xml(const tinyxml2::XMLElement& element) {
    EImageBaseType image = get_image_id(GetOnlyChildText(element, "Type"));
    ETriValue pregnant = parse_tri_value(GetOnlyChildText(element, "Pregnant"));
    ETriValue futa = parse_tri_value(GetOnlyChildText(element, "Futa"));
    ETriValue tied = parse_tri_value(GetOnlyChildText(element, "TiedUp"));
    auto participant_el = element.FirstChildElement("Participants");
    ESexParticipants part = ESexParticipants::ANY;
    if(participant_el) {
        part = get_participant_id(participant_el->GetText());
    }

    return {image, part, pregnant, futa, tied};
}

sImageSpec::sImageSpec(EImageBaseType type, ESexParticipants parts, ETriValue preg, ETriValue futa, ETriValue tied) :
        BasicImage(type), Participants(parts), IsPregnant(preg), IsFuta(futa),
        IsTied(tied)
{

}

namespace {
    auto to_tuple(const sImageSpec& spec) {
        return std::tie(spec.BasicImage, spec.Participants, spec.IsPregnant, spec.IsFuta, spec.IsTied);
    }
}

bool operator<(const sImageSpec& first, const sImageSpec& second) {
    return to_tuple(first) < to_tuple(second);
}
