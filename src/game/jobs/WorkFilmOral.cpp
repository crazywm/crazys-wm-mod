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
#include "cGangs.h"

void FilmOral::DoScene(sGirl& girl) {
    //BSIN - not sure what's wrong but this crashes the game.
    sGang* Gang = g_Game->gang_manager().GetGangOnMission(MISS_GUARDING);
    
    if (result.performance >= 350)
    {
        if (Gang && Gang->m_Num > 0 && chance(50))
        {
            int members = std::min(rng().bell(0, Gang->m_Num * 2), Gang->m_Num);
            ss << "When the 'actor' didn't turn up, ${name} expertly sucked off ";
            if (members >= Gang->m_Num)    ss << "everyone";
            else if (members > 1)/*  */    ss << members << " lucky guys";
            else/*                   */    ss << "one lucky guy";
            ss << " from your gang " << Gang->name() << ".";
        }
        else ss << "The lucky guy came uncontrollably, damn near passing out in pleasure as ${name}"
            << " expertly sucked him dry.\nShe savoured the cum in her mouth, rolling it around her lips and tongue before finally swallowing it down.";
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        ss << "${name} sucked off her man like a pro - not once breaking eye-contact - and was rewarded with ";
        if (chance(50) || girl.has_active_trait("Cum Addict")) ss << "a mouthful of semen. She kept her lips clamped to his cock to the last, thirstily swallowing down every drop of hot cum.";
        else ss << "a explosion of cum in her face. As she licked his penis clean, she rubbed cum around her skin and licked it off her fingers.";
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        ss << "${name} gave a fairly pleasant blowjob, and ended up glazed in hot cum.";
        result.bonus = 4;
    }
    else if (result.performance >= 145)
    {
        ss << "${name} gave a passable blowjob, but in the end the actor had to finish himself off, splatting cum on her face.";
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        ss << "It was a pretty awkward and uncomfortable scene, with the actor not getting any kind of pleasure from her clumsy, toothy attempts. ";
        ss << "In the end he gave up and simply wanked in her face, but even then she dodged at the last moment, ruining that scene too.";
        result.bonus = 1;
    }
    else
    {
        ss << "After the fourth time she 'snagged' the actor on her tooth, he cursed and stormed off set. Your gang, " << (Gang ? Gang->name() : "") << ", saved the day by pinning her down wanking on her face one-by-one.\nOverall it's a terrible scene.";
    }
    ss << "\n";

    //Enjoyed? If she performed well, she'd should have enjoyed it.
    if (girl.has_active_trait("Cum Addict")) result.enjoy += 2;
    PerformanceToEnjoyment("She really enjoyed giving head today.",
            "She enjoyed this performance.",
            "She's bad at this, and the whole experience was pretty humiliating.");
    result.bonus += result.enjoy;
}

void FilmOral::GainTraits(sGirl& girl, int performance) const {
    if (chance(5) && (girl.happiness() > 80) && (girl.get_enjoyment(ACTION_WORKMOVIE) > 75))
        cGirls::AdjustTraitGroupGagReflex(girl, 1, true);
}

FilmOral::FilmOral() : GenericFilmJob(JOB_FILMORAL, {
      IMGTYPE_ORAL, ACTION_SEX, SKILL_ORALSEX, 50, -5,
      FilmJobData::NORMAL, SKILL_ORALSEX, "Oral",
      " worked as an actress filming oral scenes.",
      " refused to give head on film today.", {GainPornStar}
}) {
    set_performance_data("work.film.oral", {STAT_CHARISMA, STAT_BEAUTY}, {SKILL_ORALSEX});
}
