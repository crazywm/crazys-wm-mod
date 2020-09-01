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
#include "Films.h"
#include "buildings/IBuilding.h"

void FilmTease::DoScene(sGirl& girl) {
    struct BonusItem {
        const char* name;
        int bonus;
    };

    std::initializer_list<BonusItem> items = {
            {"Cow-Print Lingerie", 6}, {"Designer Lingerie", 10},
            {"Sequin Lingerie", 7}, {"Sexy X-Mas Lingerie", 7},
            {"Leather Lingerie", 6}, {"Leopard Lingerie", 5},
            {"Organic Lingerie", 5}, {"Sheer Lingerie", 5},
            {"Lace Lingerie", 3}, {"Silk Lingerie", 2},
            {"Plain Lingerie", 1}
    };

    for(auto& item : items) {
        if(girl.has_item(item.name)) {
            result.performance += 2*item.bonus;
            ss << "To improve the scene, ${name} wore her " << item.name << ".\n";
            break;
        }
    }

    if (result.performance >= 350)
    {
        ss << ("She created an outstanding teaser scene. She's so stunning and flirtatious that it's somehow hotter that she never gets naked.");
        girl.fame(4);
        girl.charisma(2);
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        ss << ("She created an amazing teaser scene. Somehow she makes the few little she has on look hotter than being naked.");
        girl.fame(2);
        girl.charisma(1);
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        ss << ("She created a decent teaser scene. Not the best, but still arousing.");
        girl.fame(1);
        girl.charisma(1);
        result.bonus = 4;
    }
    else if (result.performance >= 145)
    {
        ss << ("It wasn't a great teaser scene. By the end you wish she'd get naked and do something.");
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << ("It was a bad teaser scene. Could someone just fuck her already?");
        result.bonus = 1;
        ss << ("\nThe Studio Director advised ${name} how to spice up the scene");
        if (chance(40))
        {
            ss << (" and improve her performance. The scene definitely got better after this.");
            result.bonus++;
        }
        else
        {
            ss << (", but she wouldn't listen.");
        }
    }
    else
    {
        ss << ("That was nothing 'teasing' about this awkward, charmless performance. Even the CameraMage seemed to lose interest.");
        if (girl.m_Building->num_girls_on_job(JOB_DIRECTOR, false) > 0)
        {
            ss << ("\nThe Studio Director stepped in and took control ");
            if (chance(50))
            {
                ss << ("significantly improving ${name}'s performance in the scene.");
                result.bonus++;
            }
            else
            {
                ss << ("but wasn't able to save this trainwreck of a scene.");
            }
        }
    }
    ss << ("\n");

    //Enjoyed? If she performed well, she'd should have enjoyed it.
    PerformanceToEnjoyment("She loved flirting with the camera today.",
                           "She enjoyed this performance.",
                           "She didn't really get what she was supposed to do, and did not enjoy making this scene."
    );
    result.bonus += result.enjoy;
}

void FilmTease::GainTraits(sGirl& girl, int performance) const {
    //gain traits
    if (performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks sexiness.", false);
    }
}

FilmTease::FilmTease() : GenericFilmJob(JOB_FILMTEASE, {
        IMGTYPE_ECCHI, ACTION_WORKSTRIP, SKILL_STRIP, 40, 0,
        FilmJobData::NICE, SKILL_STRIP, "Teaser",
        " worked filming teaser scenes to arouse viewers.",
        " refused to make a teaser clip today.",
}) {
    load_from_xml("FilmTease.xml");
}

