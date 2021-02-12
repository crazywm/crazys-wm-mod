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

#include "Constants.h"
#include <unordered_map>
#include "character/sAttribute.h"
#include "CLog.h"
#include <cassert>
#include "utils/lookup.h"

const std::array<sAttribute, NUM_STATS>& get_all_stats () {
    static std::array<sAttribute, NUM_STATS> stats {
            sAttribute{"Charisma",     "Cha", ""},
            sAttribute{"Happiness",    "Hap", "", 0, 100, .default_value=100, .permanent=false},
            sAttribute{"Libido",       "Lib", ""},
            sAttribute{"Constitution", "Con", ""},
            sAttribute{"Intelligence", "Int", ""},
            sAttribute{"Confidence",   "Cfd", ""},
            sAttribute{"Mana",         "Ma",  "", 0, 100, 0, .permanent=false},
            sAttribute{"Agility",      "Agl", ""},
            sAttribute{"Fame",         "Fm",  ""},
            sAttribute{"Level",        "Lvl", "", .min= 0, .max = 255, .default_value=0, .permanent=false},
            sAttribute{"AskPrice",     "Ask", "", .min=0, .max=10000, .default_value=0, .permanent=false},
            sAttribute{"Experience",   "Exp", "", .min= 0, .max=32000, .default_value=0, .permanent=false},
            sAttribute{"Age",          "Age", "", .min=18},
            sAttribute{"Obedience",    "Obd", ""},
            sAttribute{"Spirit",       "Spi", ""},
            sAttribute{"Beauty",       "Bty", ""},
            sAttribute{"Tiredness",    "Trd", ""},
            sAttribute{"Health",       "HP",  "", 0, 100, .default_value=100, .permanent=false},
            sAttribute{"PCFear",       "PCF", "", .min=-100},
            sAttribute{"PCLove",       "PCL", "", .min=-100},
            sAttribute{"PCHate",       "PCH", "", .min=-100},
            sAttribute{"Morality",     "Mor", "", .min=-100},
            sAttribute{"Refinement",   "Ref", "", .min=-100},
            sAttribute{"Dignity",      "Dig", "", .min=-100},
            sAttribute{"Lactation",    "Lac", "", .min=-100},
            sAttribute{"Strength",     "Str", ""},
            sAttribute{"NPCLove",      "NPL", ""},
            sAttribute{"Sanity",       "San", ""}
    };
    return stats;
}

const char* get_stat_name(STATS stat) {
    return get_all_stats()[stat].name;
}

const id_lookup_t<STATS>& get_stat_lookup() {
    static id_lookup_t<STATS> lookup = [](){
        id_lookup_t<STATS> lookup;
        // TODO make this case insensitive
        for(std::size_t i = 0; i < NUM_STATS; ++i) {
            lookup[get_stat_name((STATS)i)] = (STATS)i;
            lookup[get_all_stats()[i].short_name] = (STATS)i;
        }
        return std::move(lookup);
    }();
    return lookup;
}

STATS get_stat_id(const std::string& name) {
    return lookup_with_error(get_stat_lookup(), name, "Trying to get invalid STAT");
}

const std::array<sAttribute, NUM_SKILLS>& get_all_skills() {
    static std::array<sAttribute, NUM_SKILLS> skills {
            sAttribute{"Anal",         "Anl", ""},
            sAttribute{"Magic",        "Mgc", ""},
            sAttribute{"BDSM",         "SM",  ""},
            sAttribute{"NormalSex",    "Sex", ""},
            sAttribute{"Beastiality",  "Bst", ""},
            sAttribute{"Group",        "Grp", ""},
            sAttribute{"Lesbian",      "Les", ""},
            sAttribute{"Service",      "Srv", ""},
            sAttribute{"Strip",        "Stp", ""},  // Str is strength
            sAttribute{"Combat",       "Cbt", ""},
            sAttribute{"OralSex",      "BJ",  ""},
            sAttribute{"TittySex",     "Tty", ""},
            sAttribute{"Medicine",     "Med", ""},
            sAttribute{"Performance",  "Prf", ""},
            sAttribute{"Handjob",      "HJb", ""},
            sAttribute{"Crafting",     "Crf", ""},
            sAttribute{"Herbalism",    "Hbl", ""},
            sAttribute{"Farming",      "Frm", ""},
            sAttribute{"Brewing",      "Brw", ""},
            sAttribute{"AnimalHandling", "Aml", ""},
            sAttribute{"Footjob",      "FJb", ""},
            sAttribute{"Cooking",      "Co", "",},
    };
    return skills;
}

const char* get_skill_name(SKILLS stat) {
    return get_all_skills()[stat].name;
}

const id_lookup_t<SKILLS>& get_skill_lookup() {
    static auto lookup = [](){
        id_lookup_t<SKILLS> lookup;
        // TODO make this case insensitive
        for(std::size_t i = 0; i < NUM_SKILLS; ++i) {
            lookup[get_skill_name((SKILLS)i)] = (SKILLS)i;
        }
        return std::move(lookup);
    }();
    return lookup;
}

SKILLS get_skill_id(const std::string& name) {
    return lookup_with_error(get_skill_lookup(), name, "Trying to get invalid SKILL");
}

const std::array<const char*, NUM_STATUS>& get_status_names() {
    static std::array<const char*, NUM_STATUS> names {
        "None", "Poisoned", "Badly Poisoned", "Pregnant", "Pregnant By Player", "Slave",
        "Inseminated", "Controlled", "Catacombs", "Arena", "Is Daughter", "Dating Pervert",
        "Dating Mean", "Dating Nice"
    };
    return names;
}

const char* get_status_name(STATUS stat) {
    return get_status_names()[stat];
}

const id_lookup_t<STATUS>& get_status_lookup() {
    static auto lookup = create_lookup_table<STATUS>(get_status_names());
    return lookup;
}

STATUS get_status_id(const std::string& name) {
    return lookup_with_error(get_status_lookup(), name, "Trying to get invalid STATUS");
}

const std::array<const char*, NUM_ACTIONTYPES>& get_action_names() {
    static std::array<const char*, NUM_ACTIONTYPES> names {
            "COMBAT", "SEX", "WORKESCORT", "WORKCLEANING", "WORKMATRON",
            "WORKBAR", "WORKHALL", "WORKSHOW", "WORKSECURITY",
            "WORKADVERTISING", "WORKTORTURER", "WORKCARING", "WORKDOCTOR",
            "ACTRESS", "PORNSTAR", "MOVIECREW", "WORKCUSTSERV", "WORKCENTRE", "WORKCLUB",
            "WORKHAREM", "WORKRECRUIT", "WORKNURSE", "WORKMECHANIC",
            "WORKCOUNSELOR", "WORKMUSIC", "WORKSTRIP", "WORKMILK",
            "WORKMASSEUSE", "WORKFARM", "WORKTRAINING", "WORKREHAB",
            "MAKEPOTIONS", "MAKEITEMS", "COOKING", "GETTHERAPY",
            "WORKHOUSEPET", "GENERAL"
    };
    return names;
}

const std::array<const char*, NUM_ACTIONTYPES>& get_action_descriptions() {
    static std::array<const char*, NUM_ACTIONTYPES> names {
            "combat",                            // ACTION_COMBAT
            "working as a whore",                // ACTION_SEX
            "working as an Escort",                // ACTION_WORKESCORT
            "cleaning",                            // ACTION_WORKCLEANING
            "acting as a matron",                // ACTION_WORKMATRON
            "working in the bar",                // ACTION_WORKBAR
            "working in the gambling hall",        // ACTION_WORKHALL
            "producing movies",                    // ACTION_WORKSHOW
            "providing security",                // ACTION_WORKSECURITY
            "doing advertising",                // ACTION_WORKADVERTISING
            "torturing people",                    // ACTION_WORKTORTURER
            "caring for beasts",                // ACTION_WORKCARING
            "working as a doctor",                // ACTION_WORKDOCTOR
            "being an actress",                    // ACTION_ACTRESS
            "doing porn",                          // ACTION_PORNSTAR
            "producing movies",                  // ACTION_MOVIECREW
            "providing customer service",        // ACTION_WORKCUSTSERV
            "working in the centre",            // ACTION_WORKCENTRE
            "working in the club",                // ACTION_WORKCLUB
            "being in your harem",                // ACTION_WORKHAREM
            "being a recruiter",                // ACTION_WORKRECRUIT
            "working as a nurse",                // ACTION_WORKNURSE
            "fixing things",                    // ACTION_WORKMECHANIC
            "counseling people",                // ACTION_WORKCOUNSELOR
            "performing music",                    // ACTION_WORKMUSIC
            "stripping",                        // ACTION_WORKSTRIP
            "having her breasts milked",        // ACTION_WORKMILK
            "working as a masseuse",            // ACTION_WORKMASSEUSE
            "working on the farm",                // ACTION_WORKFARM
            "training",                            // ACTION_WORKTRAINING
            "counseling",                        // ACTION_WORKREHAB
            "making potions",                    // ACTION_WORKMAKEPOTIONS
            "making items",                        // ACTION_WORKMAKEITEMS
            "cooking",                            // ACTION_WORKCOOKING
            "therapy",                            // ACTION_WORKTHERAPY
            "puppy training",                    // ACTION_WORKHOUSEPET
            "doing miscellaneous tasks"            // ACTION_GENERAL
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
    return lookup_with_error(get_action_lookup(), name, "Trying to get invalid ACTION");
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
    return lookup_with_error(get_fetish_lookup(), name, "Trying to get invalid FETISH");
}

const std::array<const char*, NUM_IMGTYPES>& get_imgtype_names() {
    static std::array<const char*, NUM_IMGTYPES> names {
            "anal", "bdsm", "sex", "beast", "group", "lesbian", "torture", "death",
            "profile", "combat", "oral", "ecchi", "strip", "maid", "sing", "wait",
            "card", "bunny", "nude", "mast", "titty", "milk", "hand", "foot",
            "bed", "farm", "herd", "cook", "craft", "swim", "bath", "nurse",
            "formal", "shop", "magic", "sign", "presented", "dom", "deepthroat", "eatout",
            "dildo", "sub", "strapon", "les69", "lick", "suckballs", "cowgirl", "revcowgirl",
            "sexdoggy", "jail", "puppygirl", "ponygirl", "catgirl", "brand", "rape", "rapebeast",
            "birthhuman", "birthhumanmultiple", "birthbeast", "impregsex", "impreggroup", "impegbdsm",
            "impregbeast", "virginsex", "virgingroup", "virginbdsm", "virginbeast", "escort",
            "sport", "study", "teacher", "massage", "pregnant", "preganal", "pregbdsm", "pregsex", "pregbeast", "preggroup",
            "preglesbian", "pregtorture", "pregdeath", "pregprofile", "pregcombat", "pregoral",
            "pregecchi", "pregstrip", "pregmaid", "pregsing", "pregwait", "pregcard", "pregbunny",
            "pregnude", "pregmast", "pregtitty", "pregmilk", "preghand", "pregfoot", "pregbed",
            "pregfarm", "pregherd", "pregcook", "pregcraft", "pregswim", "pregbath", "pregnurse",
            "pregformal", "pregshop", "pregmagic", "pregsign", "pregpresented", "pregdom",
            "pregdeepthroat", "pregeatout", "pregdildo", "pregsub", "pregstrapon", "pregles69",
            "preglick", "pregsuckballs", "pregcowgirl", "pregrevcowgirl", "pregdoggy", "pregjail",
            "pregpuppy", "pregpony", "pregcat", "pregbrand", "pregrape", "pregrapebeast",
            "", "", "", "", "", "", "", "", "", "", "", "pregescort", "pregsport", "pregstudy",
            "pregteacher", "pregmassage"
    };
    return names;
}

const std::array<const char*, NUM_JOBS>& get_job_names() {
    static std::array<const char*, NUM_JOBS> names {
            "Free Time", "Practice Skills", "Cleaning", "Security",
            "Advertising", "Customer Service", "Matron", "Torturer",
            "Explore Catacombs", "Beast Carer", "Barmaid", "Waitress",
            "Singer", "Piano", "Escort", "Bar Cook",
            "Game Dealer", "Entertainer", "XXX Entertainer", "Hall Whore",
            "Strip Club Barmaid", "Strip Club Waitress", "Strip Club Stripper",
            "Strip Club Whore", "Masseuse", "Brothel Stripper", "Peep Show",
            "Brothel Whore", "Whore on Streets",
            "Executive", "Director", "Market Research", "Promoter",
            "Camera Mage", "Crystal Purifier", "Fluffer",
            "Stagehand", "Action", "The Naked Chef", "Music", "Teaser Video",
            "Film Masturbation", "Film Strip tease", "Film Foot Job", "Film Hand Job",
            "Film Titty Fuck", "Film Anal", "Film Lesbian",
            "Film Oral Sex", "Film Sex", "Film Group",
            "Film Bestiality", "Film Bondage", "Cumslut/Bukkake", "Face-fuck",
            "Public Torture", "Film a random scene",
            "Fight Beasts", "Cage Match", "Combat Training", "Doctore",
            "City Guard", "Blacksmith", "Cobbler", "Jeweler",
            "Grounds Keeper", "Centre Manager", "Feed Poor",
            "Community Service", "Clean Centre", "Counselor", "Rehab",
            "Therapy", "Extreme Therapy", "Anger Management", "Get Healing",
            "Cure Diseases", "Abortion", "Cosmetic Surgery",
            "Liposuction", "Breast Reduction Surgery", "Boob Job",
            "Vaginal Rejuvenation", "Face Lift", "Arse Job", "Tubes Tied",
            "Fertility Treatment", "Chairman", "Doctor",
            "Nurse", "Intern", "Janitor",
            "Farm Manager", "Veterinarian", "Marketer", "Researcher",
            "Farm Hand", "Farmer", "Gardener", "Shepherd",
            "Rancher", "Catacombs Rancher", "Beast Capture", "Milker",
            "Get Milked", "Butcher", "Baker", "Brewer",
            "Tailor", "Make Items", "Make Potions",
            "Head Girl", "Recruiter", "Bed Warmer", "House Cook",
            "Clean House", "Personal Training", "Fake Orgasm Expert",
            "SO Straight", "SO Bisexual", "SO Lesbian",
            "House Pet", "In the Dungeon", "Runaway"
    };
    return names;
}

const char* get_job_name(JOBS job) {
    return get_job_names()[job];
}

auto get_job_lookup() -> const auto& {
    static auto lookup = create_lookup_table<JOBS>(get_job_names());
    return lookup;
}

JOBS get_job_id(const std::string& name) {
    return lookup_with_error(get_job_lookup(), name, "Trying to get invalid JOB");
}
