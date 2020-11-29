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

void FilmThroat::DoScene(sGirl& girl) {
    int hate = 0, tired = 0;
    int OPTION = uniform(0, 3);
    if (result.performance >= 350)
    {
        add_text("work.perfect");
        result.bonus = 12;
        hate = 1;
        tired = 1;
    }
    else if (result.performance >= 245)
    {
        add_text("work.great");
        hate = 2;
        tired = 4;
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        add_text("work.good");
        result.bonus = 4;
        hate = 3;
        tired = 8;
    }
    else if (result.performance >= 145)
    {
        add_text("work.ok");
        result.bonus = 2;
        hate = 5;
        tired = 10;
    }
    else if (result.performance >= 100)
    {
        add_text("work.bad");
        result.bonus = 1;
        hate = 8;
        tired = 12;
    }
    else
    {
        add_text("work.worst");
        hate = 8;
        tired = 12;
    }
    ss << ("\n");

    //Enjoyed?! No, but some will hate it less.
    PerformanceToEnjoyment("She won't say it, but you suspect she secretly gets off on the degradation.",
                           "She's pretty raw and her makeup is everywhere.",
                           "From the way she's coughing and gagging and has bright red eyes, you suspect this wasn't much fun for her."
                           );
    result.bonus += result.enjoy;


    int impact = uniform(0, 10);
    if (girl.has_active_trait("Strong Gag Reflex"))
    {
        ss << "She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterward.\n \n";
        girl.health((10 + impact));
        girl.tiredness((10 + impact + tired));
        girl.pchate((2 + hate));
        girl.pcfear((2 + hate));
    }
    else if (girl.has_active_trait("Gag Reflex"))
    {
        ss << "She gagged and retched a lot. It was exhausting and left her feeling sick.\n \n";
        girl.health((2 + impact));
        girl.tiredness((5 + impact + tired));
        girl.pchate(hate);
        girl.pcfear(hate);
    }
}

bool FilmThroat::CheckRefuseWork(sGirl& girl) {
    if (girl.health() < 20)
    {
        ss << ("The crew refused to film a throatjob scene because ${name} is not healthy enough.\n\"We are NOT filming snuff\".");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.has_active_trait("Mind Fucked"))
    {
        ss << ("Mind fucked ${name} was pleased to be paraded naked in public and tortured and degraded. It is natural.\n");
        result.bonus += 10;
    }
    else if (girl.has_active_trait("Masochist"))
    {
        ss << ("Masochist ${name} was pleased to be tortured, used and degraded. It is her place.\n");
        result.bonus += 6;
    }
    else if (girl.disobey_check(ACTION_WORKMOVIE, JOB_FILMFACEFUCK))
    {
        return RefusedTieUp(girl);
    }
    else add_text("work");
    ss << "\n \n";
    return false;
}

void FilmThroat::GainTraits(sGirl& girl, int performance) const {
    if (result.performance > 200) cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", false);
    if (chance(5) && (girl.happiness() > 80) && (girl.get_enjoyment(ACTION_WORKMOVIE) > 75))
        cGirls::AdjustTraitGroupGagReflex(girl, 1, true);

    //lose
    if (chance(5)) cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, "Somewhere between having a dick in her throat, balls slapping her eyes and a camera watching her retch, ${name} has lost her iron will.", false);
}

FilmThroat::FilmThroat() : GenericFilmJob(JOB_FILMFACEFUCK, {
      IMGTYPE_ORAL, ACTION_SEX, 50, 5,
      FilmJobData::EVIL, SKILL_ORALSEX, "Facefucking"
}) {
    load_from_xml("FilmFaceFuck.xml");
}
