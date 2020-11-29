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
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "character/predicates.h"


FilmBdsm::FilmBdsm() : GenericFilmJob(JOB_FILMBONDAGE, {
    IMGTYPE_BDSM, ACTION_SEX, 50, 8,
    FilmJobData::EVIL, SKILL_BDSM, "Bondage"
}) {
    load_from_xml("FilmBondage.xml");
}


void FilmBdsm::DoScene(sGirl& girl) {
    PrintPerfSceneEval();
    ss << "\n";

    add_text("post-work-event");

    //Feedback enjoyment
    if (result.enjoy > 10)
    {
        ss << "She won't say it, but you suspect she secretly gets off on the attention, sin and degradation.\n \n";
    }
    else if (result.enjoy > 0)
    {
        ss << "She's only a little traumatised.\n \n";
    }
    else
    {
        ss << "From the way she's crouched, rocking in a corner and silently weeping... you figure she didn't enjoy this.\n \n";
    }

    if (girl.lose_trait("Virgin"))
    {
        result.performance += 50;
        ss << "She is no longer a virgin.\n";
    }
    sCustomer Cust = g_Game->GetCustomer(*girl.m_Building);
    Cust.m_Amount = 1;
    if (Cust.m_IsWoman)    // FemDom
    {
        result.performance += 20;
        /* */if (girl.has_active_trait("Lesbian")) result.performance += 20;
        else if (girl.has_active_trait("Straight")) result.performance -= 20;
    }
    else
    {
        if (!girl.calc_pregnancy(Cust, 0.75))
            g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
        /* */if (girl.has_active_trait("Lesbian")) result.performance -= 10;
        else if (girl.has_active_trait("Straight")) result.performance += 10;
    }

    // TODO what's the point of all these performance updates after the scene has been filmed?
}

bool FilmBdsm::CheckRefuseWork(sGirl& girl) {
    int roll = d100();
    if (girl.health() < 40)
    {
        add_text("crew.refuse.health");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.is_pregnant())
    {
        add_text("crew.refuse.pregnant");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.has_active_trait("Mind Fucked"))
    {
        ss << "Mind fucked ${name} seemed at home in the dungeon, and started collecting together tools she should be punished with.\n";
        result.bonus += 10;
    }
    else if (girl.has_active_trait("Masochist"))
    {
        ss << "Masochist ${name} was pleased to in the dungeon. It is her place.\n";
        result.bonus += 6;
    }
    else if (roll <= 10 && girl.disobey_check(ACTION_WORKMOVIE, JOB_FILMBONDAGE))
    {
        return RefusedTieUp(girl);
    }
    else add_text("work") << "\n \n";
    return false;
}
