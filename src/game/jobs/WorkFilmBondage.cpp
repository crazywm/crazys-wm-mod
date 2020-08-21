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
    IMGTYPE_BDSM, ACTION_SEX, SKILL_BDSM, 50, 8,
    FilmJobData::EVIL, SKILL_BDSM, "Bondage",
    nullptr, nullptr
}) {
    set_performance_data("work.film.bondage", {STAT_CHARISMA, STAT_BEAUTY, STAT_DIGNITY}, {});
    add_trait_chance(GainFaker);
    add_trait_chance(GainSlut);
    add_trait_chance(GainPornStar);
    add_trait_chance(GainMasochist);
}


void FilmBdsm::DoScene(sGirl& girl) {
    PrintPerfSceneEval();
    ss << "\n";


    //Enjoyed? If she's deranged, she'd should have enjoyed it.
    if (girl.has_active_trait("Mind Fucked"))
    {
        result.enjoy += 16;
        ss << "Being completely mind fucked, ${name} actually gets off on this.\n";
    }
    else if (girl.has_active_trait("Masochist"))
    {
        result.enjoy += 10;
        ss << "${name} enjoys this. It's what she deserves.\n";
    }
    else if (girl.has_active_trait("Broken Will") || girl.has_active_trait("Dependant"))
    {
        result.enjoy += 5;
        ss << "${name} accepts this. It is Master's will.\n";
    }
    else if (girl.has_active_trait("Iron Will") || girl.has_active_trait("Fearless"))
    {
        result.enjoy -= 5;
        ss << "${name} endures in stoic silence, determined not to let you see her suffer.\n";
    }
    else if (girl.has_active_trait("Nymphomaniac"))
    {
        result.enjoy += 2;
        ss << "${name} doesn't much like the pain, but loves the sex and attention.\n";
    }

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
        ss << "The crew refused to film a dungeon scene with ${name} because she is not healthy enough.\n\"We are NOT filming snuff.\"";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.is_pregnant())
    {
        ss << "The crew refused to do a BDSM scene with ${name} due to her pregnancy.";
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
        ss << "${name} was horrified and refused to be beaten and sexually tortured in this ";
        if (girl.is_slave())
        {
            if (g_Game->player().disposition() > 30)  // nice
            {
                ss << "\"monstrous\" place.\nShe was starting to panic, so you allowed her the day off.\n";
                girl.pclove(2);
                girl.pchate(-1);
                girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
                return true;
            }
            else if (g_Game->player().disposition() > -30) //pragmatic
            {
                ss << "\"monstrous\" place.\nShe was starting to panic, so you ordered your men to grab her and bind her for action.\n";
                girl.pclove(-1);
                girl.pchate(2);
                girl.pcfear(2);
                g_Game->player().disposition(-1);
                result.enjoy -= 2;
            }
            else
            {
                ss << "\"monstrous\" place.\nShe was starting to panic so you ordered your men to quickly grab, strip and bind her. Finally, ";
                if (girl.has_active_trait("Pierced Nipples"))
                {
                    ss << "noticing her pierced nipples";
                    if (girl.has_active_trait("Pierced Clit"))
                    {
                        ss << " and clit";
                    }
                    ss << ", ";
                }
                else if (girl.has_active_trait("Pierced Clit")) ss << "noticing her clittoral piercing, ";
                ss << "you personally selected some 'fun tools' for your actor, instructing him to train your slave in humility and obedience.\n\"Master her. Intimately.\"";
                girl.pclove(-4);
                girl.pchate(+5);
                girl.pcfear(+5);
                g_Game->player().disposition(-2);
                result.enjoy -= 6;
            }
        }
        else // not a slave
        {
            ss << " \"monstrous\" place.\n";
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
    }
    else ss << "${name} was taken for bondage and torture scenes in your dungeon.\n \n";
    return false;
}
