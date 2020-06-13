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
#include "Game.hpp"
#include "character/cPlayer.h"

void FilmMusic::DoScene(sGirl& girl) {
    //What's she best at?
    enum { PERFORMANCE, LOOKS };
    int TopSkill = (girl.performance() > (girl.beastiality() + girl.charisma() / 2)) ? PERFORMANCE : LOOKS;

    struct BonusItem {
        const char* name;
        int bonus;
    };

    std::initializer_list<BonusItem> items = {
            {"Liquid Dress", 50}, {"Dark Liquid Dress", 50},
            {"Gemstone Dress", 50}, {"Hime Dress", 60},
            {"Enchanted Dress", 50}, {"Leopard Lingerie", 50},
            {"Designer Lingerie", 50}, {"Sheer Lingerie", 30},
            {"Oiran Dress", 25}, {"Silk Lingerie", 15},
            {"Empress' New Clothes", 50}, {"Faerie Gown", 50},
            {"Royal Gown", 50}, {"Classy Underwear", 50}
    };

    for(auto& item : items) {
        if(girl.has_item(item.name)) {
            result.performance += item.bonus;
            ss << "To improve the scene, ${name} wore her " << item.name << ".\n";
            break;
        }
    }

    if (result.performance >= 350)
    {
        ss << "${name} created a legendary music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing was outstanding, and she herself was truly breathtaking.";
        else ss << "She was stunning and gave a fantastic performance.";
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        ss << "${name} created a superb music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were top-notch and she looked amazing on camera.";
        else ss << "She was beautiful and she gave a very touching performance.";
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        ss << "${name} created a very good music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were excellent and she looked okay.";
        else ss << "She looked fantastic and gave a nice performance.";
        result.bonus = 4;
    }
    else if (result.performance >= 145)
    {
        ss << "${name} created an okay music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were decent.";
        else ss << "She looked pretty good.";
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << "${name} created a weak music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were decent but she didn't look all that.";
        else ss << "She looked pretty good but the performance let her down.";
        result.bonus = 1;
        ss << "\nThe Studio Director advised ${name} how to spice up the performance";
        if (chance(40))
        {
            ss << ". The scene definitely got better after this.";
            result.bonus++;
        }
        else
        {
            ss << ", but she wouldn't listen.";
        }
    }
    else if(result.performance >= 50)
    {
        ss << "${name} created a bad music video. ";
        if (TopSkill == PERFORMANCE) ss << "Her singing and dancing weren't great and she had zero charisma on camera.";
        else ss << "She was just about likeable on camera, but her performance was painful.";
    }
    else
    {
        ss << "${name} created a terrible music video. You even considered playing it in the dungeon as a kind of torture. But no. That would be inhumane.";
        if (g_Game->player().disposition() < -20) ss << ".. Even for you.";
    }

    ss << "\n";

    //Enjoyed? If she performed well, she'd should have enjoyed it.
    PerformanceToEnjoyment("She loved singing and performing today.",
                           "She enjoyed this performance.",
                           "She isn't much of a performer and did not enjoy making this scene."
    );
    result.bonus += result.enjoy;
}

void FilmMusic::GainTraits(sGirl& girl, int performance) const {
    //gain traits
    if      (performance >= 100 && chance(25)) cGirls::PossiblyGainNewTrait(&girl, "Charming", 80, ACTION_WORKMOVIE, "Singing and dancing on film has made her more Charming.", false);
    else if (performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(&girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks sexiness.", false);
    }
    if (girl.has_active_trait("Singer"))
    {
        if (performance >= 245 && chance(30))
        {
            cGirls::PossiblyGainNewTrait(&girl, "Idol", 80, ACTION_WORKMOVIE, "Her talented and charismatic performances have got a large number of fans Idolizing her.", false);
        }
    }
    else if (performance >= 200 && chance(30)) cGirls::PossiblyGainNewTrait(&girl, "Singer", 80, ACTION_WORKMOVIE, "Her singing has become quite excellent.", false);
}

FilmMusic::FilmMusic() : GenericFilmJob(JOB_FILMMUSIC, {
        IMGTYPE_ORAL, ACTION_WORKMOVIE, SKILL_STRIP, 40, -3,
        FilmJobData::NICE, SKILL_PERFORMANCE, "Music",
        " worked on a music video showcasing her singing and dancing talent.",
        " refused to shoot a music video scene today."
}) {
    set_performance_data("work.film.music", {STAT_CHARISMA, STAT_BEAUTY, STAT_CONFIDENCE}, {SKILL_PERFORMANCE});
}
