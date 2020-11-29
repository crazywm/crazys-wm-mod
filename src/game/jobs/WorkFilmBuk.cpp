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

FilmBuk::FilmBuk() : GenericFilmJob(JOB_FILMBUKKAKE, {
        IMGTYPE_ORAL, Action_Types::NUM_ACTIONTYPES, 50, 5,
        FilmJobData::EVIL, SKILL_GROUP, "Bukkake"
}) {
    load_from_xml("FilmBuk.xml");
}

void FilmBuk::DoScene(sGirl& girl) {
    //Lights, camera...
    add_text("work.description");
    if(tied) {
        add_text("work.description.tied");
    } else {
        add_text("work.description.untied");
    }

    PrintPerfSceneEval();
    ss << "\n";
    add_text("post-work-event");

    //For final calc
    result.bonus += result.enjoy;

    // now we've got that done, most girls shouldn't like this...
    result.enjoy -= 10;
}

void FilmBuk::GainTraits(sGirl& girl, int performance) const {
    if (tied) {
        cGirls::PossiblyGainNewTrait(girl, "Mind Fucked", 75, ACTION_SEX,
                                     "${name} has become Mind Fucked from the forced degradation.", false);
        cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX,
                                          "${name}'s unwilling degradation has shattered her iron will.", false);
    }
}

bool FilmBuk::CheckRefuseWork(sGirl& girl) {
    int roll = d100();
    if (girl.has_active_trait("Cum Addict"))
    {
        ss << "Cum-craving ${name} couldn't wait to get sticky in this bukkake scene, and was sucking guys off before the lighting was even set up.";
        result.bonus += 10;
    }
    else if (roll <= 10 && !girl.has_active_trait("Mind Fucked") && girl.disobey_check(ACTION_WORKMOVIE, JOB_FILMBUKKAKE))
    {
        tied = !RefusedTieUp(girl);
        return !tied;
    }
    else ss << get_text("work") << "\n \n";
    return false;
}

void FilmBuk::Reset() {
    GenericFilmJob::Reset();
    tied = false;
}
