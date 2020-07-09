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
#pragma region //    Includes and Externs            //
#include "buildings/cBuildingManager.h"
#include "character/cCustomers.h"
#include "cRng.h"
#include <sstream>
#include "cGold.h"
#include "Game.hpp"
#include "character/predicates.h"

#pragma endregion

// `J` Job Brothel - Sleazy Bar
bool WorkSleazyBarmaid(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKCLUB;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.disobey_check(actiontype, JOB_SLEAZYBARMAID))
    {
        //Making mssg more informative (what was refused?)
        ss << "${name} refused to be a barmaid in your sleazy strip club " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a bartender in the strip club.\n \n";

    cGirls::UnequipCombat(&girl);    // put that shit away, you'll scare off the customers!

    int HateLove = 0;
    HateLove = girl.pclove() - girl.pchate();
    int wages = 15, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_ECCHI;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SLEAZYBARMAID, false);



    if (jobperformance >= 245)
    {
        ss << " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n \n";
        wages += 155;
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        wages += 95;
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        wages += 55;
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
    }


    //base tips, aprox 10-20% of base wages
    tips += (int)(((10 + jobperformance / 22) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 25;
    }

    if (girl.beauty() > 99 && rng.percent(5))
    {
        ss << "${name} looked absolutely stunning during her shift and was unable to hide it. Instead of her ass or tits, the patrons couldn't glue their eyes off her face, and spent a lot more than usual on tipping her.\n"; tips += 50;
    }

    if (girl.charisma() > 85 && rng.percent(20))
    {
        ss << "${name} surprised a couple of gentlemen discussing some complicated issue by her insightful comments when she was taking her order. They decided her words were worth a heavy tip.\n"; tips += 35;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(15))
    {
        ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n"; wages -= 15;
    }

    if (girl.has_active_trait("Pessimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n"; tips -= 10;
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; tips += 10;
        }
    }

    if (girl.has_active_trait("Optimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
        }
    }
    // `J` slightly higher percent compared to regular barmaid, I would think sleazy barmaid's uniform is more revealing
    if ((rng.percent(5) && girl.has_active_trait("Busty Boobs")) ||
        (rng.percent(10) && girl.has_active_trait("Big Boobs")) ||
        (rng.percent(15) && girl.has_active_trait("Giant Juggs")) ||
        (rng.percent(20) && girl.has_active_trait("Massive Melons")) ||
        (rng.percent(25) && girl.has_active_trait("Abnormally Large Boobs")) ||
        (rng.percent(30) && girl.has_active_trait("Titanic Tits")))
    {
        if (jobperformance < 150)
        {
            ss << "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n";
        }
        else
        {
            ss << "A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n"; wages += 15;
        }
    }

    if (girl.has_active_trait("Meek") && rng.percent(5) && jobperformance < 125)
    {
        ss << "${name} spilled a drink all over a man's lap. He told her she had to lick it up and forced her to clean him up which she Meekly accepted and went about licking his cock clean.\n";
        imagetype = IMGTYPE_ORAL;
        enjoy -= 3;
    }

    if (rng.percent(5)) //may get moved to waitress
    {
        ss << "A drunken patron decide to grab ${name}'s boob. ";
        if (girl.has_active_trait("Meek"))
        {
            ss << "She was shocked and didn't react. The man molested her for a few minutes!\n";
        }
        else if (girl.has_active_trait("Tough"))
        {
            ss << "She knocked him out! You could swear that she knocked a couple of his teeth out too!\n";
        }
        else if (girl.has_active_trait("Your Daughter"))
        {
            ss << "She screamed do you know who my dad is? He will have your head for this!\n";
        }
        else if (girl.has_active_trait("Your Wife"))
        {
            ss << "She screamed do you know who my husband is? He will have your head for this!\n";
        }
        else if (HateLove >= 80) //loves you
        {
            ss << "She screamed do you know who my love is? He will have your head for this!\n";
        }
        else
        {
            ss << "She screamed and shook his hand off.\n";
        }
    }

    if ((girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus")) && girl.libido() > 80 && rng.percent(20) && !is_virgin(girl) && !girl.has_active_trait(
            "Lesbian"))
    {
        if (roll_b <= 50)
        {
            ss << "${name} dragged one of the less sober clients outside into a back alley and ordered him to fuck her senseless against a wall. After the deed, the poor fellow passed out and ${name} returned to her job as if nothing happened.\n";
        }
        else
        {
            ss << "${name} spotted a relatively good-looking guy walking into the bathroom alone. She followed him inside, and as he tried to exit the bathroom stall, he got pushed back in by her. ${name} didn't waste any time and in a matter of seconds was vigorously fucking the client. After the deed, the client made sure ${name} had a pretty hefty wad of money stuck behind her skirt.\n"; tips += 50;
        }
        imagetype = IMGTYPE_SEX;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.normalsex(1);
        sCustomer Cust = g_Game->GetCustomer(*brothel);
        Cust.m_Amount = 1;
        if (!girl.calc_pregnancy(Cust, 1.0))
        {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }


#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, Day0Night1);


    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += 10 + rng%roll_max;
    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 15, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (!girl.has_active_trait("Straight"))    {
        girl.upd_temp_stat(STAT_LIBIDO, min(3, brothel->num_girls_on_job(JOB_BARSTRIPPER, false)));
    }
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    girl.performance(rng%skill);
    girl.service(rng%skill + 1);

    //gained
    cGirls::PossiblyGainNewTrait(&girl, "Charismatic", 60, actiontype, "Dealing with customers at the bar and talking with them about their problems has made ${name} more Charismatic.", Day0Night1);
    if (jobperformance < 100 && roll_a <= 2) { cGirls::PossiblyGainNewTrait(&girl, "Assassin", 10, actiontype, "${name}'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.", Day0Night1); }
    if (rng.percent(25) && girl.dignity() < 0 && (imagetype == IMGTYPE_SEX || imagetype == IMGTYPE_ORAL))
    {
        cGirls::PossiblyGainNewTrait(&girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", Day0Night1, EVENT_WARNING);
    }

    //lose
    cGirls::PossiblyLoseExistingTrait(&girl, "Nervous", 20, actiontype, "${name} seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);


#pragma endregion
    return false;
}

double JP_SleazyBarmaid(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = (girl.service() +
        (girl.charisma() / 3) + (girl.beauty() / 3) + (girl.performance() / 3) +
        girl.level());
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }
    jobperformance += girl.get_trait_modifier("work.sleazybarmaid");

    return jobperformance;
}
