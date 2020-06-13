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

FilmChef::FilmChef() : GenericFilmJob(JOB_FILMCHEF, {
      IMGTYPE_COOK, ACTION_WORKCOOKING, SKILL_COOKING, 40, -8,
      FilmJobData::NICE, SKILL_COOKING, "Cooking",
      " worked filming sexy chef scenes to educate and entertain viewers.",
      " refused to make a kitchen show today.",
      {{true, "Charming", 80, ACTION_WORKMOVIE, "She has become quite Charming."},
       {true, "Actress", 80, ACTION_WORKMOVIE, "Working on camera has improved her acting skills."}}
}) {
    set_performance_data("work.film.chef", {STAT_CHARISMA, STAT_BEAUTY, STAT_CONFIDENCE}, {SKILL_COOKING});
}

void FilmChef::DoScene(sGirl& girl) {
    //What's she best at?
    enum { COOKING, LOOKS };
    int TopSkill = (girl.cooking() > ((girl.beauty() + girl.charisma()) / 2)) ? COOKING : LOOKS;

    if (result.performance >= 350)
    {
        ss << "${name} created an outstanding show. She's so competent and flirtatious that the show is a hit with everyone.";
        result.bonus = 12;
        girl.fame(3);
    }
    else if (result.performance >= 245)
    {
        ss << "${name} created a great show. Women are impressed with her competence. Men are impressed with her... competence. It doesn't hurt that she's hot and flirtatious.";
        result.bonus = 6;
        girl.fame(2);
    }
    else if (result.performance >= 185)
    {
        ss << "${name} created a good show. ";
        if (TopSkill == COOKING) ss << "Her skills in the kitchen are enough to carry the show.";
        else ss << "She's sexy enough that her fumbles in the kitchen go mostly unnoticed.";
        result.bonus = 4;
        girl.fame(1);
    }
    else if (result.performance >= 145)
    {
        ss << "${name} created an okay cooking show. ";
        if (TopSkill == COOKING)  ss << "She was skilled enough in the kitchen to mostly entertain.";
        else/*                 */ ss << "She's cute, so it doesn't really matter that she can't cook.";
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << "${name} created a bad cooking show. ";
        if (TopSkill == COOKING)  ss << "The little skill she had in the kitchen wasn't nearly enough to save it.";
        else /*                 */ss << "Perhaps if she'd quit breaking all the utensils and had instead taken her clothes off it'd would've been worth watching.\nPerhaps.";
        result.bonus = 1;
    }
    else
    {
        ss << "${name} created a really bad cooking show. ";
        ss << "She had no charisma and burned or broke basically everything she touched. It was almost funny.\nAlmost.";
    }
    ss << "\n";

    //Enjoyed? If she performed well, she'd should have enjoyed it.
    PerformanceToEnjoyment("She loved cooking and flirting on camera.",
                           "She enjoyed making this show.",
                           "She was awful at this and did not enjoy making it.");
    result.bonus += result.enjoy;
}

void FilmChef::GainTraits(sGirl& girl, int performance) const {
    if (performance >= 220 && chance(30))
    {
        cGirls::PossiblyGainNewTrait(&girl, "Chef", 80, ACTION_WORKCOOKING, "${name} has become a competent Chef.", false);
    }
    if (performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(&girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks sexiness.", false);
    }
}
