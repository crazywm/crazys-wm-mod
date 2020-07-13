/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#include "buildings/cBuildingManager.h"
#include "character/cCustomers.h"
#include "cRng.h"
#include <sstream>
#include "cGold.h"
#include <algorithm>
#include "Game.hpp"
#include "cJobManager.h"

// `J` Job Brothel - General
bool WorkCustService(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKCUSTSERV;
    stringstream ss;
    if (girl.disobey_check(actiontype, JOB_CUSTOMERSERVICE))
    {
        //SIN - More informative mssg to show *what* she refuses
        //ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        ss << "${name} refused to provide Customer Service in your brothel " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as Customer Service.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    // Note: Customer service needs to be done last, after all the whores have worked.

    int numCusts = 0; // The number of customers she can handle
    int serviced = 0;

    // Complications
    int roll = rng%100;
    if (roll <= 5)
    {
        ss << "Some of the patrons abused her during the shift.";
        girl.upd_Enjoyment(actiontype, -1);
    }
#if 1
    if (roll <= 15)
    {
        ss << "A customer mistook her for a whore and was abusive when she wouldn't provide THAT service.";
        girl.upd_Enjoyment(actiontype, -1);
    }
#endif
    else if (roll >= 75) {
        ss << "She had a pleasant time working.";
        girl.upd_Enjoyment(actiontype, +3);
    }
    else
    {
        ss << "The shift passed uneventfully.";
    }
    // Decide how many customers the girl can handle
    if (girl.confidence() > 0)
        numCusts += girl.confidence() / 10; // 0-10 customers for confidence
    if (girl.spirit() > 0)
        numCusts += girl.spirit() / 20; // 0-5 customers for spirit
    if (girl.service() > 0)
        numCusts += girl.service() / 25; // 0-4 customers for service
    numCusts++;
    // A single girl working customer service can take care of 1-20 customers in a week.
    // So she can take care of lots of customers. It's not like she's fucking them.

    // Add a small amount of happiness to each serviced customer
    // First, let's find out what her happiness bonus is
    int bonus = 0;
    if (girl.charisma() > 0)
        bonus += girl.charisma() / 20;
    if (girl.beauty() > 0)
        bonus += girl.beauty() / 20;
    // Beauty and charisma will only take you so far, if you don't know how to do service.
    if (girl.performance() > 0)            // `J` added
        bonus += girl.performance() / 20;
    if (girl.service() > 0)
        bonus += girl.service() / 20;
    // So this means a maximum of 20 extra points of happiness to each
    // customer serviced by customer service, if a girl has 100 charisma,
    // beauty, performance and service.

    // Let's make customers angry if the girl sucks at customer service.
    if (bonus < 5)
    {
        bonus = -20;
        ss << "\n \nHer efforts only made the customers angrier.";
        //And she's REALLY not going to like this job if she's failing at it, so...
        girl.upd_Enjoyment(actiontype, -5);
    }

    // Now let's take care of our neglected customers.
    for (int i=0; i<numCusts; i++)
    {
        if (g_Game->GetNumCustomers() > 0)
        {
            sCustomer Cust = g_Game->GetCustomer(*brothel);
            // Let's find out how much happiness they started with.
            // They're not going to start out very happy. They're seeing customer service, after all.
            Cust.set_stat(STAT_HAPPINESS, 22 + rng%10 + rng%10); // average 31 range 22 to 40
            // Now apply her happiness bonus.
            Cust.happiness(bonus);
            // update how happy the customers are on average
            brothel->m_Happiness += Cust.happiness();
            // And decrement the number of customers to be taken care of
            g_Game->customers().AdjustNumCustomers(-1);
            serviced++;
        }
        else
        {
            //If there aren't enough customers to take care of, time to quit.
            girl.AddMessage(girl.FullName() + " ran out of customers to take care of.", IMGTYPE_PROFILE, Day0Night1);
            break;
        }
    }
    // So in the end, customer service can take care of lots of customers, but won't do it
    // as well as good service from a whore. This is acceptable to me.
    ss << "\n \n${name} took care of " << serviced << " customers this shift.";

    /* Note that any customers that aren't handled by either customer service or a whore count as a 0 in the
     * average for the brothel's customer happiness. So customer service leaving customers with 27-60 in their
     * happiness stat is going to be a huge impact. Again, not as good as if the whores do their job, but better
     * than nothing. */

    // Bad customer service reps will leave the customer with 2-20 happiness. Bad customer service is at least better than no customer service.
#if 0
    string debug = "";
    debug += ("There were " + intstring(g_Game->GetNumCustomers()) + " customers.\n");
    debug += ("She could have handled " + intstring(numCusts) + " customers.\n");
    girl.AddMessage(debug, IMGTYPE_PROFILE, EVENT_DEBUG);
#endif
    // Now pay the girl.

    girl.m_Pay += 50;
    g_Game->gold().staff_wages(50);  // wages come from you
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

    // Raise skills
    int xp = 5 + (serviced / 5), skill = 2 + (serviced / 10);

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.fame(1);
    girl.exp(xp);
    int gain = rng % skill;
    if (gain == 1)        girl.confidence(rng%skill);
    else if(gain == 2)    girl.spirit(rng%skill);
    else                girl.performance(rng%skill);
    girl.service(rng%skill+1);

    return false;
}

double JP_CustService(const sGirl& girl, bool estimate)
{
    //SIN - standardizing job performance per J's instructs
    double jobperformance =
        //main stats - first 100
        ((girl.service() + girl.charisma()) / 2) +
        //secondary stats - second 100
        ((girl.confidence() + girl.spirit() + girl.performance() + girl.beauty()) / 4) +
        //add level
        girl.level();

    //tiredness penalty
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.customerservice");

    return jobperformance;
}
