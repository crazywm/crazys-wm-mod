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

void FilmAction::DoScene(sGirl& girl) {
    //JOB
    if (result.performance >= 350)
    {
        ss << "${name} made an outstanding action scene, " << get_text("story");
        ss << " It will definitely win her some fans.";
        girl.fame(3);
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        ss << "${name} made a great action scene, " << get_text("story");
        ss << " It should win her some fans.";
        girl.fame(2);
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        ss << "${name} made a fairly good action scene, " << get_text("story");
        ss << " Her fans will enjoy it.";
        result.bonus = 4;
        girl.fame(1);
    }
    else if (result.performance >= 145)
    {
        ss << "${name} made an uninspired action scene, " << get_text("story");
        ss << " Her diehard fans might enjoy it.";
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << "${name} made a weak action scene, " << get_text("story");
        result.bonus = 1;
        ss << "\nThe CamerMage advised ${name} how to improve her performance";
        if (chance(40))
        {
            ss << " and her scene got a little better after this.";
            result.bonus++;
        }
        else
        {
            ss << ", but she wouldn't listen.";
        }
    }
    else
    {
        ss << "${name} made an awful action scene, " << get_text("story");
        ss << " Even her fans will hate it.";
        girl.fame(-1);
    }

    ss << "\n";

    PerformanceToEnjoyment("She loved performing on film today.",
                           "She enjoyed this performance.",
                           "She found this awkward and did not enjoy it.");
}

void FilmAction::GainTraits(sGirl& girl, int performance) const {
    if (performance >= 140 && chance(50))
    {
        if (cGirls::PossiblyGainNewTrait(girl, "Brawler", 60, ACTION_COMBAT, "She has performed in enough fight scenes that she has become quite a brawler.", false));
        else if (cGirls::PossiblyGainNewTrait(girl, "Tough", 70, ACTION_COMBAT, "She has performed in enough fight scenes that she has become quite tough.", false));
        else if (cGirls::PossiblyGainNewTrait(girl, "Agile", 80, ACTION_COMBAT, "She has performed martial arts scenes that she is becoming agile.", false));
        else (cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 90, ACTION_COMBAT, "She has performed so many action scenes that she has become fast on her feet.", false));
    }

    if (performance >= 140 && chance(50))
    {
        if (cGirls::PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKMOVIE, "She has performed in enough movie scenes that she has become charismatic.", false));
        else if (cGirls::PossiblyGainNewTrait(girl, "Actress", 70, ACTION_WORKMOVIE, "She has performed in enough movie scenes that she has become a good actress.", false));
        else cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks sexiness.", false);
    }
}

FilmAction::FilmAction() : GenericFilmJob(JOB_FILMACTION, {
      IMGTYPE_COMBAT, ACTION_COMBAT, 50, -8,
      FilmJobData::NORMAL, SKILL_COMBAT, "Action"
}) {
    load_from_xml("FilmAction.xml");
}


double FilmAction::GetPerformance(const sGirl& girl, bool estimate) const
{
    /*BSIN: She can work to her skills: if she's good at combat, make it combat focussed, if magic make it mage craft
    //So calc is as follows:
            total/4                        (magic + combat)/4                - gives a max of 50 for combined score
        +    best_skill/2            +    (whichever is higher)/2            - max of 50 again for highest skill

        So if comb = magic = 100, then total = 100
        if comb = 100, magic = 0, then total = (100 + 0)/4 + (100/2) = 25 + 50 = 75
        if comb = 50, magic = 100, then total = (50 + 100)/4 + (100/2) = 37 + 50 = 87
        if comb = 50, magic = 50, then total = (50 + 50)/4 + (50/2) = 25 + 25 = 50

        Second factor is looks: (beau + char)/ 2
    */
    int    CombatSkill = girl.combat();
    int    MagicSkill = girl.magic();
    int TopSkill = ((CombatSkill > MagicSkill) ? CombatSkill : MagicSkill);
    double jobperformance =
        (((girl.charisma() + girl.beauty()) / 2)        //looks component
        + (((CombatSkill + MagicSkill)/4) + (TopSkill/2)));                                        //Skills component

    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.film.action");

    return jobperformance;
}


