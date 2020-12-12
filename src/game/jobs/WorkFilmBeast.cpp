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
#include "Game.hpp"
#include "character/cPlayer.h"
#include "sStorage.h"
#include "Films.h"
#include "character/predicates.h"
#include "character/cCustomers.h"

void FilmBeast::DoScene(sGirl& girl) {
    PrintPerfSceneEval();
    PrintEnjoyFeedback();
    if (girl.lose_trait("Virgin"))
    {
        result.performance += 50;
        ss << "She is no longer a virgin.\n";
    }
}

bool FilmBeast::CheckCanWork(sGirl& girl) {
    if(GenericFilmJob::CheckCanWork(girl)) {
        // no beasts = no scene
        if (g_Game->storage().beasts() < 1)
        {
            girl.AddMessage("You have no beasts for this scene, so she had the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
            return false;
        }
        return true;
    }
    return false;
}

bool FilmBeast::CheckRefuseWork(sGirl& girl) {
    if (girl.health() < 20)
    {
        ss << "The crew refused to film a Bestiality scene with ${name} because she is not healthy enough.\n\"She could get hurt.\"";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.has_active_trait("Nymphomaniac"))
    {
        ss << "\"Bestiality? Aren't we all animals?!\"\nAs long as it will fuck her, sex addict ${name} really doesn't care WHAT it is.";
    }
    else if (!girl.has_active_trait("Mind Fucked") && girl.disobey_check(ACTION_WORKMOVIE, JOB_FILMBEAST))
    {
        tied = !RefusedTieUp(girl);
        return !tied;
    }
    else add_text("work") << "\n \n";
    return false;
}

void FilmBeast::GainTraits(sGirl& girl, int performance) const {
    if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
        g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
}

void FilmBeast::Reset() {
    GenericFilmJob::Reset();
    tied = false;
}

FilmBeast::FilmBeast() : GenericFilmJob(JOB_FILMBEAST, {
    IMGTYPE_BEAST, ACTION_SEX, 50, 5,
    FilmJobData::EVIL, SKILL_BEASTIALITY, "Beast"}) {
    load_from_xml("FilmBeast.xml");
}
