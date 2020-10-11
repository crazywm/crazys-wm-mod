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
#include <cmath>
#include "Game.hpp"
#include "character/cCustomers.h"
#include "character/predicates.h"

FilmGroup::FilmGroup() : GenericFilmJob(JOB_FILMGROUP, {
    IMGTYPE_GROUP, ACTION_SEX, SKILL_GROUP, 50, 5,
    FilmJobData::NORMAL, SKILL_GROUP, "Group",
    "", " refused to do an orgy on film today.",
}) {
    load_from_xml("FilmGroup.xml");
}

void FilmGroup::DoScene(sGirl& girl) {
    int guys = d100();
    if (guys < 50)
    {
        guys = 2;
        result.performance += 5;
        ss << "${name} worked as an actress filming a three-way.\n \n";
    }
    else if (guys < 95)
    {
        result.performance += 10;
        guys = d100();
        /* */if (guys < 40)        guys = 3;
        else if (guys < 60)        guys = 4;
        else if (guys < 75)        guys = 5;
        else if (guys < 85)        guys = 6;
        else if (guys < 90)        guys = 7;
        else if (guys < 94)        guys = 8;
        else if (guys < 98)        guys = 9;
        else /*          */        guys = 10;
        ss << "${name} worked in a gang-bang scene with " << guys << " other people.\n \n";
    }
    else if (!girl.has_active_trait("Lesbian") && girl.has_active_trait("Nymphomaniac") && girl.has_active_trait("Porn Star"))
    {
        result.performance += 50;
        guys = uniform(120, 300);
        ss << "${name} worked in a scene where she tried to set a new record by taking part in a " << guys << " men gangbang.\n \n";
    }
    else
    {
        result.performance += 20;
        guys = uniform(10, 30);
        ss << " worked in a orgy scene with " << guys << " other people.\n \n";
    }

    girl.tiredness(guys - 2);    // Extra tiredness per guy

    int roll = d100();
    if (roll <= 10)
    {
        result.enjoy -= ((guys > 10 ? (guys / 10) : (guys / 2)) + 1);
        ss << "She found it unpleasant fucking that many people.\n \n";
    }
    else if (roll >= 90 || (girl.has_active_trait("Nymphomaniac") && roll >= 50))
    {
        result.enjoy += (guys > 10 ? (guys / 10) : (guys / 2)) + 1;
        ss << "She loved getting so much action, and wants more!\n \n";
    }
    else
    {
        result.enjoy += uniform(0, 2);
        ss << "She wasn't really into having so much sex today, but managed to get through.\n \n";
    }
    result.performance += result.enjoy * 2;

    if (girl.has_active_trait("Lolita") && chance(50))
    {
        ss << "With such a young looking girl on the cover, this movie is simply sure to sell out.\n \n";
        result.performance += 15;
    }
    if (girl.has_active_trait("Bimbo") && chance(50))
    {
        ss << "Her performance couldn't be considered acting. She just enjoyed a good dicking with a blissfully stupid smile on her face.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Cute") && chance(50))
    {
        ss << "Such a cute girl doing her best to handle the rough ponding her co-stars gave her will surely make a few hearts skip a beat.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Natural Pheromones") && chance(50))
    {
        ss << "Her scent drove the actors mad with lust. They went at it harder and wilder than the director had ordered.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Fast Orgasms") && chance(50))
    {
        ss << "She came the second she got two dicks inside her. That was the first of many orgasms. She barely remembered her own name by the end of the scene.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Great Arse") && chance(50))
    {
        ss << "Her ass jiggling with each thrust was a sight to behold. The camera mage made sure to capture its wiggly glory in slow motion.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Plump") && chance(50))
    {
        ss << "She sure offered a lot of meat to work with. The wet and fleshy sound she made with each thrust is sure to drive the audience wild.\n \n";
        result.performance += 5;
    }
    if ((girl.has_active_trait("Fragile") || girl.has_active_trait("Delicate")) && chance(50))
    {
        ss << "This was somewhat rough for her, she was barely able to move by the end of the scene. Some people find that hot though.\n \n";
        result.performance += 5;
        result.enjoy -= uniform(-2, 3);
    }
    if (girl.breast_size() > 5 && chance(50))
    {
        ss << "Her gigantic breasts stole the spotlight towards the end of the scene when she pushed them together to collect her co-star's seed.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Cum Addict") && chance(50))
    {
        ss << "She insisted that her co-stars collect their semen in a cup for her to drink at the end of the scene.\n \n";
        result.performance += 5;
    }
    if (girl.has_active_trait("Succubus") && chance(50))
    {
        ss << "After the camera stopped running, she grabbed two actors and had another round with them.\n \n";
        result.performance += 5;
    }

    if (girl.lose_trait("Virgin"))
    {
        result.performance += 50;
        ss << "She is no longer a virgin.\n";
    }

    sCustomer Cust = g_Game->GetCustomer(*girl.m_Building);
    Cust.m_Amount = guys;
    if (!girl.calc_group_pregnancy(Cust, 1.0))
    {
        g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
    }

    // Extra pay per guy -- but scale nonlinearly, so it does not explode too much
    double guy_bonus = std::log(guys);
    result.wages += int(guy_bonus * 30);
    result.bonus = (int)(result.performance / 10);

    // Improve stats
    /*
    int xp = guy_bonus * 5;
    int skill = guy_bonus;
    */
}

double FilmGroup::GetPerformance(const sGirl& girl, bool estimate) const
{
    return 0;
}
