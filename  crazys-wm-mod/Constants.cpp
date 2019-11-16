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

#include "Constants.h"
#include <unordered_map>

template<class T>
using id_lookup_t = std::unordered_map<std::string, T>;

template<class T, std::size_t N>
id_lookup_t<T> create_lookup_table(const std::array<const char*, N>& names) {
    id_lookup_t<T> lookup;
    // TODO make this case insensitive
    for(std::size_t i = 0; i < N; ++i) {
        lookup[names[i]] = (T)i;
    }
    return std::move(lookup);
}

const std::array<const char*, NUM_STATS>& get_stat_names () {
    static std::array<const char*, NUM_STATS> names {
            "Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility",
            "Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty", "Tiredness", "Health",
            "PCFear", "PCLove", "PCHate", "Morality", "Refinement", "Dignity", "Lactation", "Strength",
            "NPCLove", "Sanity"
    };
    return names;
}

const char* get_stat_name(STATS stat) {
    return get_stat_names()[stat];
}

const id_lookup_t<STATS>& get_stat_lookup() {
    static auto lookup = create_lookup_table<STATS>(get_stat_names());
    return lookup;
}

STATS get_stat_id(const std::string& name) {
    return get_stat_lookup().at(name);
}

const std::array<const char*, NUM_SKILLS>& get_skill_names() {
    static std::array<const char*, NUM_SKILLS> names {
                    "Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat", "OralSex", "TittySex",
                    "Medicine", "Performance", "Handjob", "Crafting", "Herbalism", "Farming", "Brewing", "AnimalHandling", "Footjob", "Cooking"
            };
    return names;
}

const char* get_skill_name(SKILLS stat) {
    return get_skill_names()[stat];
}

const id_lookup_t<SKILLS>& get_skill_lookup() {
    static auto lookup = create_lookup_table<SKILLS >(get_skill_names());
    return lookup;
}

SKILLS get_skill_id(const std::string& name) {
    return get_skill_lookup().at(name);
}

const std::array<const char*, NUM_STATUS>& get_status_names() {
    static std::array<const char*, NUM_STATUS> names {
        "None", "Poisoned", "Badly Poisoned", "Pregnant", "Pregnant By Player", "Slave", "Has Daughter", "Has Son",
                "Inseminated", "Controlled", "Catacombs", "Arena", "Your Daughter", "Is Daughter",
                "DatingPerv", "DatingMean", "DatingNice"
    };
    return names;
}

const char* get_status_name(STATUS stat) {
    return get_stat_names()[stat];
}

const id_lookup_t<STATUS>& get_status_lookup() {
    static auto lookup = create_lookup_table<STATUS>(get_status_names());
    return lookup;
}

STATUS get_status_id(const std::string& name) {
    return get_status_lookup().at(name);
}

const std::array<const char*, NUM_ACTIONTYPES>& get_action_names() {
    static std::array<const char*, NUM_ACTIONTYPES> names {
            "COMBAT", "SEX", "WORKESCORT", "WORKCLEANING", "WORKMATRON", "WORKBAR", "WORKHALL", "WORKSHOW", "WORKSECURITY",
            "WORKADVERTISING", "WORKTORTURER", "WORKCARING", "WORKDOCTOR", "WORKMOVIE", "WORKCUSTSERV", "WORKCENTRE", "WORKCLUB",
            "WORKHAREM", "WORKRECRUIT", "WORKNURSE", "WORKMECHANIC", "WORKCOUNSELOR", "WORKMUSIC", "WORKSTRIP", "WORKMILK",
            "WORKMASSEUSE", "WORKFARM", "WORKTRAINING", "WORKREHAB", "MAKEPOTIONS", "MAKEITEMS", "COOKING", "GETTHERAPY",
            "WORKHOUSEPET", "GENERAL"
    };
    return names;
}

const std::array<const char*, NUM_ACTIONTYPES>& get_action_descriptions() {
    static std::array<const char*, NUM_ACTIONTYPES> names {
            "combat",							// ACTION_COMBAT
            "working as a whore",				// ACTION_SEX
            "working as an Escort",				// ACTION_WORKESCORT
            "cleaning",							// ACTION_WORKCLEANING
            "acting as a matron",				// ACTION_WORKMATRON
            "working in the bar",				// ACTION_WORKBAR
            "working in the gambling hall",		// ACTION_WORKHALL
            "producing movies",					// ACTION_WORKSHOW
            "providing security",				// ACTION_WORKSECURITY
            "doing advertising",				// ACTION_WORKADVERTISING
            "torturing people",					// ACTION_WORKTORTURER
            "caring for beasts",				// ACTION_WORKCARING
            "working as a doctor",				// ACTION_WORKDOCTOR
            "producing movies",					// ACTION_WORKMOVIE
            "providing customer service",		// ACTION_WORKCUSTSERV
            "working in the centre",			// ACTION_WORKCENTRE
            "working in the club",				// ACTION_WORKCLUB
            "being in your harem",				// ACTION_WORKHAREM
            "being a recruiter",				// ACTION_WORKRECRUIT
            "working as a nurse",				// ACTION_WORKNURSE
            "fixing things",					// ACTION_WORKMECHANIC
            "counseling people",				// ACTION_WORKCOUNSELOR
            "performing music",					// ACTION_WORKMUSIC
            "stripping",						// ACTION_WORKSTRIP
            "having her breasts milked",		// ACTION_WORKMILK
            "working as a masseuse",			// ACTION_WORKMASSEUSE
            "working on the farm",				// ACTION_WORKFARM
            "training",							// ACTION_WORKTRAINING
            "counseling",						// ACTION_WORKREHAB
            "making potions",					// ACTION_WORKMAKEPOTIONS
            "making items",						// ACTION_WORKMAKEITEMS
            "cooking",							// ACTION_WORKCOOKING
            "therapy",							// ACTION_WORKTHERAPY
            "puppy training",					// ACTION_WORKHOUSEPET
            "doing miscellaneous tasks"			// ACTION_GENERAL
    };
    return names;
}

const char* get_action_name(Action_Types stat) {
    return get_action_names()[stat];
}

const char* get_action_descr(Action_Types action) {
    return get_action_descriptions()[action];
}

const id_lookup_t<Action_Types >& get_action_lookup() {
    static auto lookup = create_lookup_table<Action_Types >(get_action_names());
    return lookup;
}

Action_Types get_action_id(const std::string& name) {
    return get_action_lookup().at(name);
}

const std::array<const char*, NUM_FETISH>& get_fetish_names() {
    static std::array<const char*, NUM_FETISH> names {
            "TryAnything", "SpecificGirl", "BigBoobs", "Sexy", "CuteGirl", "NiceFigure", "Lolita", "NiceArse",
            "Cool", "Elegant", "Nerd", "SmallBoobs", "Dangerous", "NonHuman", "Freak", "Futa",
            "Tall", "Short", "Fat"
    };
    return names;
}

const char* get_fetish_name(Fetishs stat) {
    return get_fetish_names()[stat];
}

const id_lookup_t<Fetishs>& get_fetish_lookup() {
    static auto lookup = create_lookup_table<Fetishs>(get_fetish_names());
    return lookup;
}

Fetishs get_fetish_id(const std::string& name) {
    return get_fetish_lookup().at(name);
}
