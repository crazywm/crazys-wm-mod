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

#ifndef WM_IMAGE_IDS_H
#define WM_IMAGE_IDS_H

#include <array>
#include <string>

/*!
 * \brief Enum that can in be three states, corresponding certain yes/true, certain no/false, and an intermediate state
 * that means maybe. This is used for e.g. for image features which in some cases may not be decidable. For example, for
 * pregnancy, maybe would correspond to an image in which the belly is not visible.
 */
enum class ETriValue {
    Yes,
    Maybe,
    No
};

ETriValue parse_tri_value(const std::string& source);
std::string print_tri_flag( ETriValue v, const char* val);
std::ostream& print_tri_flag(std::ostream& target, ETriValue v, const char* val);

// This is the data the is submitted to an event when determining an image
enum class EImageBaseType {
    // Basic Sex types
    ANAL,
    BDSM,
    SPANKING,
    VAGINAL,
    BEAST,
    TENTACLE,
    ORAL,
    TITTY,
    HAND,
    FOOT,
    DEEPTHROAT,
    DILDO,
    STRAPON,
    SIXTY_NINE,
    LICK,                   // getting her pussy licked
    FINGER,                 // getting her pussy fingered
    FISTING,                // getting her pussy fisted
    SUCKBALLS,
    RAPE,
    RAPE_BEAST,
    CUMSHOT,

    // special positions
    COWGIRL,
    REVCOWGIRL,
    SEXDOGGY,
    MISSIONARY,
    SCISSOR,

    // NSFW
    STRIP,
    KISS,
    FONDLE,
    CHASTITY,
    ECCHI,
    NUDE,
    BUNNY,
    ESCORT,                         // clothed with a client
    MASSAGE,                        // massaging a client
    MILK,                           // getting milked
    DOM,                            // acting as a dominatrix (torturer, dominatrix job, ...)
    SUB,                            // acting as a submissive

    // Jobs
    MAID,
    SINGING,                        // for some reason, using the name SING causes an error under windows
    WAIT,
    CARD,
    NURSE,
    PIANO,                          // Playing the piano
    MUSIC,                          // Playing another instrument
    STAGEHAND,
    PURIFIER,
    DIRECTOR,
    CAMERA_MAGE,
    STUDIO_CREW,
    SECURITY,                       // Security
    COOK,                           // Cooking
    BAKE,                           // Baking
    FARM,                           // Generic farm work
    HERD,                           // working with animals
    ADVERTISE,                      // (prvls. SIGN)

    // Activities
    SPORT,                          // for the free time job, doing sports
    STUDY,                          // reading a book or looking clever
    TEACHER,                        // dressed or acting as a teacher
    COMBAT,                         // fighting/with weapons
    MAGIC,                          // performing a spell
    BED,                            // On a bed. (Used e.g. for bedroom interactions)
    REST,                           // free time resting
    BATH,                           // taking a bath
    SWIM,                           // going swimming or dressed in a swimsuit
    SHOP,                           // free time going shopping
    CRAFT,                          // creating items
    DANCE,                          // dancing
    DINNER,
    DRINKS,

    BIRTH_HUMAN,                    // Giving birth to a human
    BIRTH_BEAST,                    // Giving birth to a beast

    // other
    REFUSE,                         // refuse to do a job
    DEATH,                          // she died
    PROFILE,                        // generic profile image
    PORTRAIT,                       // portrait-style profile image
    JAIL,                           // In jail or dungeon
    TORTURE,                        // Getting tortured
    FORMAL,                         // wearing formal clothes
    PRESENTED,                      // used for Slave Market
    BRAND,                          // getting branded
    PUPPYGIRL,                      // acting or looking like a puppy
    PONYGIRL,                       // acting or looking like a pony
    CATGIRL,                        // acting or looking like a cat

    NUM_TYPES
};

const std::array<const char*, (int)EImageBaseType::NUM_TYPES>& get_imgtype_names();
EImageBaseType get_image_id(const std::string& name);
const char* get_image_name(EImageBaseType image);

enum class ESexParticipants {
    SOLO,                       ///< She is alone / masturbating
    LESBIAN,                    ///< With another woman
    HETERO,                     ///< With another man
    THREESOME_FFM,              ///< With a woman and a man
    THREESOME_MMF,              ///< With two women
    GANGBANG,                   ///< With lots of men
    LESBIAN_GROUP,              ///< With lots of women
    ORGY,                       ///< Multiple; mixed gender
    OTHER,                      ///< With a single other person that is not identifiable as man/woman
    ANY,                        ///< The value to be used for non-sex images. It means match everything.
    COUNT,
};

const std::array<const char*, (int)ESexParticipants::COUNT>& get_participant_names();
ESexParticipants get_participant_id(const std::string& name);
const char* get_participant_name(ESexParticipants image);
/*
IMGTYPE_IMPREGSEX,                    // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGGROUP,                // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGBDSM,                    // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGBEAST,                // `J` new .06.03.01 for DarkArk
IMGTYPE_WATER            - "water*."            - "Watersports"        - watersports
IMGTYPE_PETPROFILE        - "pet*."            - "Pet"                - profile, nude
IMGTYPE_PETORAL            - "petoral*."        - "PetOral"            - oral, lick, deepthroat, titty, petprofile, nude, bdsm
IMGTYPE_PETSEX            - "petsex*."        - "PetSex"            - sex, nude, anal, petprofile, bdsm
IMGTYPE_PETBEAST        - "petbeast*."        - "PetBeastiality"    - beast, sex, anal, bdsm, nude
IMGTYPE_PETFEED            - "petfeed*."        - "PetFeed"            - oral, lick, petprofile, bdsm, nude
IMGTYPE_PETPLAY            - "petplay*."        - "PetPlay"            - petprofile, bdsm, nude
IMGTYPE_PETTOY            - "pettoy*."        - "PetToy"            - dildo, petprofile, oral, mast, bdsm, nude
IMGTYPE_PETWALK            - "petwalk*."        - "PetWalk"            - petprofile, bdsm, nude
IMGTYPE_PETLICK            - "petlick*."        - "PetLick"            - lick, oral, petprofile, bdsm, nude, titty
*/

enum class EImagePresets {
    GROUP,
    FFM,
    MMF,
    GANGBANG,
    ORGY,
    BLOWJOB,
    LESBIAN,
    EAT_OUT,
    LES69,
    MASTURBATE,
    MAST_DILDO,
    BUKKAKE,
    GROUP_BDSM,
    COUNT
};

const std::array<const char*, (int)EImagePresets::COUNT>& get_image_preset_names();
EImagePresets get_image_preset_id(const std::string& name);
const char* get_image_preset_name(EImagePresets image);

template<class T>
T convert_from_text(const std::string& name);

#endif //WM_IMAGE_IDS_H
