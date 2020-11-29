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
    if (result.performance >= 350)
    {
        add_text("work.perfect");
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        add_text("work.great");
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        add_text("work.good");
        result.bonus = 4;
    }
    else if (result.performance >= 145)
    {
        add_text("work.ok");
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        add_text("work.bad");
        result.bonus = 1;
    }
    else
    {
        add_text("work.worst");
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
      IMGTYPE_ORAL, ACTION_SEX, 50, -5,
      FilmJobData::NORMAL, SKILL_ORALSEX, "Oral"
}) {
    load_from_xml("FilmOral.xml");
}
