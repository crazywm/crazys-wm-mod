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

/*
#include "Films.h"
#include "Game.hpp"
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "character/predicates.h"

void FilmPubBDSM::GainTraits(sGirl& girl, int performance) const {
    if (hard) {
        cGirls::PossiblyGainNewTrait(girl, "Mind Fucked", 65, ACTION_SEX,
                                     "${name} was pushed too far in her public torture film and is now completely Mind Fucked.",
                                     false);
        cGirls::GirlInjured(girl, 5);
        cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX,
                                          "${name}'s public degradation has shattered her iron will.", false);
    }
    //Hate
    girl.pchate((impact - result.enjoy));
    girl.pcfear((impact - result.enjoy));
    //Health
    girl.health((4 * impact));
    girl.obedience(impact / 2);
}

void FilmPubBDSM::DoScene(sGirl& girl) {
    ss << "A crowd gathered to watch as ${name}";
    int NUMSCENARIOS = 9;
    int roll = uniform(0, NUMSCENARIOS);

    switch (roll)
    {
        case 0:
            ss << (" was hogtied naked on the floor outside the studio.");
            break;
        case 1:
            ss << (" was stripped and locked in the public stocks in the city square");
            break;
        case 2:
            ss << (" was stripped and strapped over a barrel in the market");
            break;
        case 3:
            ss << (", shackled and sent into the streets in only a maid hat, was quickly surrounded");
            break;
        case 4:
            ss << (" was dragged naked from the studio and tied to a tree");
            break;
        case 5:
            ss << (" was stripped naked and ");
            if (girl.has_active_trait("Pierced Clit") || girl.has_active_trait("Pierced Nipples") ||
                girl.has_active_trait("Pierced Tongue")
                || girl.has_active_trait("Pierced Navel") || girl.has_active_trait("Pierced Nose")) ss << ("tied tightly in place through the rings in her piercings");
            else
            {
                ss << ("had her arms taped-together behind her back, exposing her ");
                if (girl.has_active_trait("Perky Nipples")) ss << ("perky nipples");
                else if (girl.has_active_trait("Puffy Nipples")) ss << ("puffy nipples");
                else if (girl.has_active_trait("Abundant Lactation") || girl.has_active_trait("Cow Tits")) ss << ("large, milky breasts");
                else if (girl.has_active_trait("Flat Chest") || girl.has_active_trait("Petite Breasts") ||
                         girl.has_active_trait("Small Boobs")) ss << ("tiny breasts");
                else if (girl.has_active_trait("Busty Boobs") || girl.has_active_trait("Big Boobs") ||
                         girl.has_active_trait("Giant Juggs")) ss << ("large breasts");
                else if (girl.has_active_trait("Massive Melons") || girl.has_active_trait("Abnormally Large Boobs") ||
                         girl.has_active_trait("Titanic Tits")) ss << ("huge breasts");
                else ss << ("breasts");
            }
            break;
        case 6:
            ss << (" was tied to a fence and had her clothes ripped off");
            break;
        case 7:
            ss << (" was suspended in the air by chains");
            break;
        case 8:
            ss << (" was rendered helpless and given over to the mob");
            break;
        default:
            ss << (" was tied impossibly in the dirt");
            break;
    }
    ss << ". The crowd jeered at her ";

    //Actions
    int NUMACTIONS = 10;
    roll = uniform(0, NUMACTIONS);

    switch (roll)
    {
        case 0:
            if (girl.has_active_trait("Strong Gag Reflex") || girl.has_active_trait("Gag Reflex"))
            {
                ss << ("gags, retches and gasps");
            }
            else ss << ("expressions");
            ss << (" as guys competed to shove their dicks down her throat.");
            girl.oralsex(1);
            throat = true;
            impact = 5;
            break;
        case 1:
            ss << ("yelps as a gang of teenagers slapped her tits and spanked her ass with sticks.");
            girl.bdsm(1);
            impact = 1;
            break;
        case 2:
            ss << ("cries as she was double-fisted by three angry elves.");
            girl.anal(1);
            impact = 5;
            break;
        case 3:
            ss << ("gasps and squeals as a cackling old witch claiming to be her 'fairy godmother' mounted her with a thick double-dildo.");
            girl.lesbian(1);
            impact = 3;
            break;
        case 4:
            ss << ("cries as a demented priest shoved 'Candles of Cleansing' in her 'unclean places', chanting prayers as he lit them.");
            girl.bdsm(1);
            impact = 8;
            break;
        case 5:
            ss << ("screams for help as she was fucked by a pack of wild beasts.");
            girl.beastiality(1);
            fucked = BYBEAST;
            hard = true;
            impact = 8;
            break;
        case 6:
            ss << ("struggles as a pair of ");
            if (girl.beauty() > 80)        ss << "ugly whores, jealous of her looks";
            else if (girl.age() < 25)            ss << "old whores, jealous of her youth";
            else if (girl.intelligence() > 75)    ss << "dumb whores, jealous of her brains";
            else if (girl.is_free())            ss << "slave whores, jealous of her freedom";
            else                                ss << "sadistic rival whores";
            ss << " spit-roasted her with some shockingly large strap-ons.";
            girl.lesbian(1);
            impact = 5;
            break;
        case 7:
            ss << ("cries as a large gang of ");
            if (girl.has_active_trait("Tsundere") || girl.has_active_trait("Yandere"))    ss << ("losers she's rejected over the years");
            else if ((girl.has_active_trait("Lolita") || girl.has_active_trait("Flat Chest") ||
                      girl.has_active_trait("Petite Breasts") ||
                      girl.has_active_trait("Small Boobs"))
                     && (girl.age() < 21)) ss << ("probable pedos");
            else if (girl.has_active_trait("Queen") || girl.has_active_trait("Princess")) ss << ("her former subjects");
            else if (girl.has_active_trait("Idol") || (girl.fame() > 75))    ss << ("men from her fan-club");
            else if (girl.has_active_trait("Teacher")) ss << ("students from her class");
            else if (girl.m_NumCusts > 100) ss << ("her former customers");
            else if (girl.age() < 21) ss << ("guys she went to school with");
            else ss << ("tramps, gypsies and homeless");
            ss << (" took this chance to spank her, grope her and fill her cunt with cum.");
            girl.normalsex(1);
            fucked = BYMAN;
            impact = 3;
            break;
        case 8:
            ss << ("screams as the police captain stubbed out a cigar on her asshole, once he and his men were 'done with that'.");
            girl.bdsm(2);
            fucked = BYMAN;
            hard = true;
            impact = 10;
            break;
        case 9:
            ss << ("agonised screams as a passing battle-mage tested out flame spells on her naked form.");
            girl.bdsm(2);
            hard = true;
            impact = 7;
            break;
        default:
            ss << ("puzzled expression as something impossible happened.");
            break;
    }
    ss << "\nYour film crew rescued her once they had enough footage.\n \n";

    if (result.performance >= 350)
    {
        ss << ("It was an outstanding scene, and definitely boosted her fame and resliience, even if it did wear her out a lot.");
        result.bonus = 12;
        girl.fame(5);
        girl.constitution(3);
        girl.tiredness(10 + impact);
    }
    else if (result.performance >= 245)
    {
        ss << ("It was a great scene and should win her some fans. She looks wrecked now though.");
        result.bonus = 8;
        girl.fame(2);
        girl.constitution(2);
        girl.tiredness(15 + impact);
    }
    else if (result.performance >= 185)
    {
        ss << ("It was a good scene, but not the best.");
        result.bonus = 4;
        girl.fame(1);
        girl.tiredness(15 + impact);
    }
    else if (result.performance >= 145)
    {
        ss << ("It was okay, but something about the scene didn't work.");
        result.bonus = 2;
        girl.tiredness(15 + impact);
    }
    else if (result.performance >= 100)
    {
        ss << ("It wasn't a great public scene.");
        result.bonus = 1;
        girl.tiredness(20 + impact);
    }
    else
    {
        ss << ("Sadly, the scene really didn't work. Even the onlookers weren't looking.");
        girl.fame(-1);
        girl.tiredness(20 + impact);
    }
    ss << ("\n");

    //Enjoyed? If she's deranged, she should have enjoyed it.
    if (girl.has_active_trait("Mind Fucked"))
    {
        result.enjoy += 16;
        ss << ("Being completely mind fucked, ${name} actually gets off on this.\n");
    }
    else if (girl.has_active_trait("Masochist"))
    {
        result.enjoy += 10;
        ss << "${name} enjoys this. It's what she deserves.\n";
    }
    else if (girl.has_active_trait("Broken Will") || girl.has_active_trait("Dependant"))
    {
        result.enjoy += 5;
        ss << "${name} accepts this. It is Master's will.\n";
    }
    else if (girl.has_active_trait("Iron Will") || girl.has_active_trait("Fearless"))
    {
        result.enjoy -= 5;
        ss << "${name} endures in stoic silence, determined not to let you see her suffer.\n";
    }
    else if (girl.has_active_trait("Nymphomaniac"))
    {
        result.enjoy += 2;
        ss << "${name} doesn't much like the pain, but enjoys getting this much sex and attention.\n";
    }

    if (fucked == BYMAN || fucked == BYBEAST)
    {
        if (girl.lose_trait("Virgin"))
        {
            ss << "Somewhere in all this, she lost her virginity.\n";
            result.bonus += 5;
        }
        sCustomer Cust = g_Game->GetCustomer(*girl.m_Building);
        Cust.m_Amount = 1;
        if (fucked == BYMAN)
        {

            if (!girl.calc_pregnancy(Cust, 0.75))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
                ss << "And she's now pregnant.\nCongratulations!\n";
            }
        }
        else
        {
            if (!girl.calc_insemination(cGirls::GetBeast(), 0.75))
            {
                g_Game->push_message(girl.FullName() + " has been inseminated.", 0);
                ss << "And she's been inseminated by a beast.\nCongratulations!\n";
            }
        }
    }
}

void FilmPubBDSM::Reset() {
    GenericFilmJob::Reset();
    hard = false;
    throat = false;
    impact = 0;
    fucked = NONE;
}

FilmPubBDSM::FilmPubBDSM() : GenericFilmJob(JOB_FILMPUBLICBDSM, {
        IMGTYPE_BDSM, ACTION_SEX, 50, SceneType::PUB_BDSM,
}) {
    load_from_xml("FilmPublicBDSM.xml");
}
*/