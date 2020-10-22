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
#include "cInventory.h"
#include <sstream>
#include "Game.hpp"
#include "character/predicates.h"
#include "cGirls.h"

#pragma endregion

// `J` Job Brothel - Brothel
bool WorkBrothelStripper(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKSTRIP;
    std::stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.disobey_check(actiontype, JOB_BROTHELSTRIPPER))
    {
        //SIN - More informative mssg to show *what* she refuses
        ss << "${name} refused to strip for customers in your brothel " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} was stripping in the brothel.\n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int wages = 45, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_STRIP;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_BROTHELSTRIPPER, false);


    int lapdance = (girl.intelligence() / 2 +
        girl.performance() / 2 +
        girl.strip()) / 2;
    bool mast = false, sex = false;

    if (girl.beauty() > 90)
    {
        ss << "\nShe is so hot, customers were waving money to attract her to dance for them.";
        tips += 20;
    }
    if (girl.intelligence() > 75)
    {
        ss << "\nShe was smart enough to boost her pay by playing two customers against one another.";
        tips += 25;
    }
    if (girl.beauty() <= 90 && girl.intelligence() <= 75)
    {
        ss << "\nShe stripped for a customer.";
    }
    ss << "\n \n";


    //Adding cust here for use in scripts...
    sCustomer Cust = g_Game->GetCustomer(*brothel);

    //A little more randomness
    if (Cust.m_IsWoman && (girl.has_active_trait("Lesbian") || girl.lesbian() > 60))
    {
        ss << "${name} was overjoyed to perform for a woman, and gave a much more sensual, personal performance.\n";
        jobperformance += 25;
    }

    if (girl.tiredness() > 70)
    {
        ss << "${name} was too exhausted to give her best tonight";
        if (jobperformance >= 120)
        {
            ss << ", but she did a fairly good job of hiding her exhaustion.\n";
            jobperformance -= 10;
        }
        else
        {
            ss << ". Unable to even mask her tiredness, she moved clumsily and openly yawned around customers.\n";
            jobperformance -= 30;
        }
    }

    if (jobperformance >= 245)
    {
        ss << "She must be the perfect stripper - customers go on and on about her and always come to see her when she works.";
        wages += 155;
    }
    else if (jobperformance >= 185)
    {
        ss << "She's unbelievable at this and is always getting praised by the customers for her work.";
        wages += 95;
    }
    else if (jobperformance >= 145)
    {
        ss << "She's good at this job and gets praised by the customers often.";
        wages += 55;
    }
    else if (jobperformance >= 100)
    {
        ss << "She made a few mistakes but overall she is okay at this.";
        wages += 15;
    }
    else if (jobperformance >= 70)
    {
        ss << "She was nervous and made a few mistakes. She isn't that good at this.";
        wages -= 5;
    }
    else
    {
        ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.";
        wages -= 15;
    }
    ss << "\n \n";

    //base tips, aprox 5-40% of base wages
    tips += (int)(((5 + jobperformance / 6) * wages) / 100);

    // lap dance code.. just test stuff for now
    if (lapdance >= 90)
    {
        ss << "${name} doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
        if (roll_b < 5)
        {
            ss << "She sold a champagne dance.";
            tips += 250;
        }
        else if (roll_b < 20)
        {
            ss << "She sold a shower dance.\n";
            tips += 125;
            if (girl.libido() > 70)
            {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                wages += 50;
                mast = true;
            }
        }
        else if (roll_b < 40)
        {
            ss << "She was able to sell a few VIP dances.\n";
            tips += 160;
            if (rng.percent(20)) sex = true;
        }
        else if (roll_b < 60)
        {
            ss << "She sold a VIP dance.\n";
            tips += 75;
            if (rng.percent(15)) sex = true;
        }
        else
        {
            ss << "She sold several lap dances.";
            tips += 85;
        }
    }
    else if (lapdance >= 65)
    {
        ss << "${name}'s skill at selling private dances is impressive.\n";
        if (roll_b < 10)
        {
            ss << "She convinced a patron to buy a shower dance.\n";
            tips += 75;
            if (girl.libido() > 70)
            {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                tips += 50;
                mast = true;
            }
        }
        if (roll_b < 40)
        {
            ss << "Sold a VIP dance to a patron.\n";
            tips += 75;
            if (rng.percent(20))
            {
                sex = true;
            }
        }
        else
        {
            ss << "Sold a few lap dances.";
            tips += 65;
        }
    }
    else if (lapdance >= 40)
    {
        ss << "${name} tried to sell private dances and ";
        if (roll_b < 5)
        {
            ss << "was able to sell a VIP dance against all odds.\n";
            tips += 75;
            if (rng.percent(10))
            {
                sex = true;
            }
        }
        if (roll_b < 20)
        {
            ss << "was able to sell a lap dance.";
            tips += 25;
        }
        else
        {
            ss << "wasn't able to sell any.";
        }
    }
    else
    {
        ss << "${name} doesn't seem to understand the real money in stripping is selling private dances.\n";
    }


    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 25;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(5))
    {
        ss << " Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n"; wages -= 15;
    }

    if (girl.has_active_trait("Pessimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            ss << " Her pessimistic mood depressed the customers making them tip less.\n"; tips -= 10;
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
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n"; tips -= 10;
        }
        else
        {
            ss << " Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
        }
    }

    if (girl.has_active_trait("Great Figure") && rng.percent(20))
    {
        if (jobperformance < 125)
        {
            ss << "${name} has a great figure so she draws a few extra patrons even if she needed to work more on her stripping.\n"; tips += 5;
        }
        else
        {
            ss << "${name}'s great figure draws a large crowed to the stage and her skill at stripping makes them pay up to see the show up close.\n"; tips += 15;
        }
    }

    //if (rng.percent(10))//ruffe event
    //{
    //    ss << "A patron keep buying her drinks \n";
    //    if (girl.herbalism > 35)
    //    {
    //        ss << "but she noticed an extra taste that she knew was a drug to make her pass out. She reported him to secuirty and he was escorted out. Good news is she made a good amount of money off him before this.\n"; wages += 25;
    //    }
    //    else
    //    {
    //        //guy gets to have his way with her
    //    }
    //}

    if (is_addict(girl, true) && !sex && !mast && rng.percent(60)) // not going to get money or drugs any other way
    {
        const char* warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
        ss << "\n" << warning << "\n";
        if (girl.has_active_trait("Shroud Addict"))            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        if (girl.has_active_trait("Fairy Dust Addict"))        girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        if (girl.has_active_trait("Viras Blood Addict"))    girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        girl.AddMessage(warning, IMGTYPE_ORAL, EVENT_WARNING);
    }

    if (sex)
    {
        int n;
        ss << "In one of the private shows, she ended up ";
        brothel->m_Happiness += 100;
        //int imageType = IMGTYPE_SEX;
        if (Cust.m_IsWoman && brothel->is_sex_type_allowed(SKILL_LESBIAN))
        {
            n = SKILL_LESBIAN;
            ss << "licking the customer's clit until she screamed out in pleasure, making her very happy.";
        }
        else
        {
            switch (rng % 10)
            {
            case 0:        n = SKILL_ORALSEX;   ss << "sucking the customer off";                    break;
            case 1:        n = SKILL_TITTYSEX;  ss << "using her tits to get the customer off";        break;
            case 2:        n = SKILL_HANDJOB;   ss << "using her hand to get the customer off";     break;
            case 3:        n = SKILL_ANAL;      ss << "letting the customer use her ass";            break;
            case 4:        n = SKILL_FOOTJOB;   ss << "using her feet to get the customer off";     break;
            default:       n = SKILL_NORMALSEX; ss << "fucking the customer as well";                break;
            }
            ss << ", making him very happy.\n";
        }
        /* */if (n == SKILL_LESBIAN)    imagetype = IMGTYPE_LESBIAN;
        else if (n == SKILL_ORALSEX)    imagetype = IMGTYPE_ORAL;
        else if (n == SKILL_TITTYSEX)    imagetype = IMGTYPE_TITTY;
        else if (n == SKILL_HANDJOB)    imagetype = IMGTYPE_HAND;
        else if (n == SKILL_FOOTJOB)    imagetype = IMGTYPE_FOOT;
        else if (n == SKILL_ANAL)        imagetype = IMGTYPE_ANAL;
        else if (n == SKILL_NORMALSEX)    imagetype = IMGTYPE_SEX;
        if (n == SKILL_NORMALSEX)
        {
            if (girl.lose_trait("Virgin"))
            {
                ss << "\nShe is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        girl.upd_skill(n, 2);
        girl.upd_temp_stat(STAT_LIBIDO, -25, true);
        girl.upd_Enjoyment(ACTION_SEX, +1);
        // work out the pay between the house and the girl
        wages += girl.askprice();
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        wages += 50 + rng%roll_max;
        fame += 1;
        girl.m_NumCusts++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    }
    else if (mast)
    {
        brothel->m_Happiness += (rng % 70) + 60;
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        wages += 50 + rng%roll_max;
        fame += 1;
        imagetype = IMGTYPE_MAST;
        //girl.m_Events.AddMessage(ss.str(), IMGTYPE_MAST, Day0Night1);
    }
    else
    {
        brothel->m_Happiness += (rng % 70) + 30;
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        wages += 10 + rng%roll_max;
        //girl.m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, Day0Night1);
    }

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\nPole dancing proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\nPole dancing proved to be quite exhausting for a pregnant girl like ${name} .\n";

        }
        girl.tiredness(10 - girl.strength() / 20 );
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
        if (girl.has_active_trait("Exhibitionist"))
        {
            enjoy += 1;
        }
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
        if (girl.has_active_trait("Exhibitionist"))
        {
            enjoy += 1;
        }
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    int xp = 15, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 30 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 80 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    girl.performance(rng%skill);
    girl.strip(rng%skill + 2);

    //gained
    cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, actiontype, "${name} has been stripping and having to be sexy for so long she now reeks of sexiness.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Exhibitionist", 60, actiontype, "${name} has been stripping for so long she loves to be naked now.", Day0Night1);
    if (jobperformance >= 140 && rng.percent(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Agile", 40, actiontype, "${name} has been working the pole long enough to become quite Agile.", Day0Night1);
    }
    if (sex == true && girl.dignity() < 0 && rng.percent(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", Day0Night1, EVENT_WARNING);
    }
    //lose
    cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, "${name} has had so many people see her naked she is no longer nervous about anything.", Day0Night1);
    if (jobperformance > 150 && girl.confidence() > 65)
    {
        cGirls::PossiblyLoseExistingTrait(girl, "Shy", 60, actiontype, "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1);
    }

#pragma endregion
    return false;
}

double JP_BrothelStripper(const sGirl& girl, bool estimate)// not used
{
    //SIN - standardizing job performance per J's instructs
    double jobperformance =
        //basing this on payout logic from code above
        //main stats - first 100 - charisma and beauty are used above to calc typical "roll_max" (max payout)
        (girl.charisma() + girl.beauty() / 2) +
        //secondary stats - second 100 - these set her "lapdance" probabiliy int (so chance of higher payout)
        ((girl.intelligence() + girl.performance() + girl.strip()) / 3) +
        //add level
        girl.level();

    // next up tiredness penalty...
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    if (girl.fame() > 85)        jobperformance += 10; //more people willing to see her
    if (girl.is_pregnant())                        jobperformance -= 10; //SIN - can't move so well
    jobperformance += girl.get_trait_modifier("work.stripper");

    return jobperformance;
}
