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
        ss << "${name} made an outstanding action scene,";
        AddStory();
        ss << " It will definitely win her some fans.";
        girl.fame(3);
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        ss << "${name} made a great action scene,";
        AddStory();
        ss << " It should win her some fans.";
        girl.fame(2);
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        ss << "${name} made a fairly good action scene,";
        AddStory();
        ss << " Her fans will enjoy it.";
        result.bonus = 4;
        girl.fame(1);
    }
    else if (result.performance >= 145)
    {
        ss << "${name} made an uninspired action scene,";
        AddStory();
        ss << " Her diehard fans might enjoy it.";
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << "${name} made a weak action scene,";
        AddStory();
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
        ss << "${name} made an awful action scene,";
        AddStory();
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

void FilmAction::AddStory()
{
    ss << " in which she ";
    ss << rng().select_text({
        "battles a gang of demons.",
        "defends a village against twisted, raping marauders.",
        "avenges her mentor and defends her family's honour.",
        "battles her evil step-brother for control of the Crimson Sceptre.",
        "saves a twisted nunnery from the evil within.",
        "opens hella whup-ass.",
        "protects the Elven Princess from the Orc Prince's evil magics.",
        "struggles to survive an island deathmatch sponsored by a corrupt state.",
        "dies unfairly, and is forced to beat the Challenge of the Eight Divines to earn back her place among the living.",
        "protects a handsome, kindly slave-master from his slaves' vicious mutiny.",
        "is a bounty hunter, hunting down desperate criminals and dangerous escaped slaves.",
        "battles her older sister who has been corrupted by the dark power of the Ninth Ward of Amocles.",
        "is the last of a race of female warriors, taking vengeance against a dark Prince.",
        "stars as a female monk defending a mountain temple from marauding Centaurs.",
        "hunts down the sadistic pirate gang who kidnapped her sister."
    });
}

FilmAction::FilmAction() : GenericFilmJob(JOB_FILMACTION, {
      IMGTYPE_COMBAT, ACTION_COMBAT, SKILL_COMBAT, 50, -8,
      FilmJobData::NORMAL, SKILL_COMBAT, "Action",
      " worked as an actress in an action scene.",
      " refused to shoot an action scene today."
}) {}


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


