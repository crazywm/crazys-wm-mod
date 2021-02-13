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
#include "IGame.h"
#include "character/predicates.h"
#include "cGirls.h"

#pragma endregion

// `J` Job Brothel - Brothel
bool WorkBrothelMasseuse(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKMASSEUSE;
    std::stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
    if (girl.disobey_check(actiontype, JOB_MASSEUSE))
    {
        //SIN - More informative mssg to show *what* she refuses
        ss << "${name} refused to massage customers in your brothel " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked massaging customers.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int wages = girl.askprice() + 40;
    int tips = 0;
    int work = 0, fame = 0;
    int imageType = IMGTYPE_MASSAGE;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_MASSEUSE, false);

    bool bannedCustomer = false; //ANON: in case she bans cust as per msg



    if (jobperformance >= 245)
    {
        ss << " She must be the perfect masseuse she never goes too hard but never too soft. She knows just what to do and the customers can't get enough of her.\n \n";
        wages += 155;

        if (roll_b <= 20)
        {
            ss << "${name} hands are the best thing that could touch a man. Customers left big tips after getting a massage from her.\n";
            tips += 20;
        }
        else if (roll_b <= 40)
        {
            ss << "She is well known and has a steady base of faithful customers.\n";
            brothel->m_Fame += 10;
        }
        else if (roll_b <= 60)
        {
            ss << "A client wanted ${name} to focus on a tight region. Doing as he asked, she left him with a boner.\n";
            brothel->m_Happiness += 5;
        }
        else if (roll_b <= 80)
        {
            ss << "Every man left ${name}'s room with a wide smile.\n";
            brothel->m_Fame += 5;
        }
        else
        {
            ss << "Being asked to perfume more sensual massage, ${name} massage the customer with her breasts for some extra coin.\n";
            tips += 20;
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always finding new ways to please the customer.\n \n";
        wages += 95;

        if (roll_b <= 20)
        {
            ss << "Knowing most of masseur's techniques, she is one of the best.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "Her customer wanted something more than the usual service. She declined and banned him from her customers list.\n";
            bannedCustomer = true;
        }
        else if (roll_b <= 60)
        {
            ss << "Trying a new technique, ${name} performed a massage with her feet. The customer really liked it and suggested she should put this on the regular menu.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "She certainly knows how and where to press on a man's body. Today, customer moans were heard in the corridor.\n";
            brothel->m_Happiness += 5;
        }
        else
        {
            ss << "Today she used some new oils. The customers liked them and paid some extra for the service.\n";
            wages += 15;
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and knows a few tricks to drive the customers wild.\n \n";
        wages += 55;

        if (roll_b <= 20)
        {
            ss << "${name} helped a customer with a really painful back pain. The man bowed several times, thanking her for the treatment.\n";
            brothel->m_Happiness += 5;
            brothel->m_Fame += 5;
        }
        else if (roll_b <= 40)
        {
            ss << "She's making less mistakes with every day and earning good money.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "When massaging a customer, she had a pleasant conversation with him, leaving a very good impression.\n";
            brothel->m_Happiness += 5;
        }
        else if (roll_b <= 80)
        {
            ss << "She certainly knows how to do her job and is getting better at it every day.\n";
        }
        else
        {
            ss << "A female customer requested a face massage. ${name} managed to meet her expectations.\n";
            brothel->m_Happiness += 5;
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;

        if (roll_b <= 20)
        {
            ss << "Maybe she isn't the best, but at least she won't kill anyone.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "She got the job done. No complaints were noted.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "She was good. A few of ${name}'s customers praised her.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "She made few mistakes, but managed not to hurt anyone today.\n";
        }
        else
        {
            ss << "Only a few customers came today. She didn't earn much.\n";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;

        if (roll_b <= 20)
        {
            ss << "${name} almost killed a customer today!\n";
        }
        else if (roll_b <= 40)
        {
            ss << "Her performance was bad. The customer left a complaint and didn't pay for the service.\n";
            brothel->m_Happiness -= 5;
        }
        else if (roll_b <= 60)
        {
            ss << "Thing that she did today could be called a backrub, not a massage.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "She argued with a client and kicked him out on the street wearing only a towel around his waist.\n";
            brothel->m_Happiness -= 5;
        }
        else
        {
            ss << "Not knowing what to do, she tried her best and failed.\n";
        }
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;

        if (roll_b <= 20)
        {
            ss << "${name} massaged a client. He could barely walk out after her service.\n";
            brothel->m_Fame -= 5;
        }
        else if (roll_b <= 40)
        {
            ss << "Getting kicked by a horse or getting a massage from ${name}. Equally deadly but the first you could get for free.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "A loud scream was heard in your building shortly after ${name} started her shift.\n";
            brothel->m_Happiness -= 5;
        }
        else if (roll_b <= 80)
        {
            ss << "Not having any experience in this kind of work she fails miserably.\n";
        }
        else
        {
            ss << "She tries to massage a client in the best way she knows. After ten minutes he leaves without paying and frustrated.\n";
            brothel->m_Happiness -= 5;
        }
    }


    //base tips, aprox 5-30% of base wages
    tips += int(((5.0 + jobperformance / 8.0) * wages) / 100.0);

    if ((girl.libido() > 90) && !bannedCustomer)
        //ANON: sanity check: not gonna give 'perks' to the cust she just banned for wanting perks!
    {
        int n;
        ss << "Because she was quite horny, she ended up ";
        sCustomer Cust = g_Game->GetCustomer(*brothel);
        brothel->m_Happiness += 100;
        if (Cust.m_IsWoman && brothel->is_sex_type_allowed(SKILL_LESBIAN))
        {
            n = SKILL_LESBIAN, ss << "intensely licking the female customer's clit until she got off, making the lady very happy.\n";
        }
        else
        {
            switch (rng % 10)
            {
            case 0:        n = SKILL_ORALSEX;   ss << "massaging the customer's cock with her tongue";                    break;
            case 1:        n = SKILL_TITTYSEX;  ss << "using her tits to get the customer off";                            break;
            case 2:        n = SKILL_HANDJOB;   ss << "giving him a cock-rub as well";                                    break;
            case 3:        n = SKILL_ANAL;      ss << "oiling the customer's cock and massaging it with her asshole.";    break;
            case 4:        n = SKILL_FOOTJOB;   ss << "using her feet to get the customer off";                            break;
            default:       n = SKILL_NORMALSEX; ss << "covered in massage oil and riding the customer's cock";            break;
            }
            ss << ", making him very happy.\n";
        }
        /* */if (n == SKILL_LESBIAN)    imageType = IMGTYPE_LESBIAN;
        else if (n == SKILL_ORALSEX)    imageType = IMGTYPE_ORAL;
        else if (n == SKILL_TITTYSEX)    imageType = IMGTYPE_TITTY;
        else if (n == SKILL_HANDJOB)    imageType = IMGTYPE_HAND;
        else if (n == SKILL_FOOTJOB)    imageType = IMGTYPE_FOOT;
        else if (n == SKILL_ANAL)        imageType = IMGTYPE_ANAL;
        else if (n == SKILL_NORMALSEX)    imageType = IMGTYPE_SEX;
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
        wages += 225;
        tips += 30 + girl.get_skill(n) / 5;
        girl.upd_Enjoyment(ACTION_SEX, +1);
        fame += 1;
        girl.m_NumCusts++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    }
    //SIN - bit more spice - roll_c doesn't seem to be used anywhere else so ok here
    else if (girl.has_active_trait("Doctor") && roll_c > 95)
    {
        ss << "Due to ${name}'s training as a Doctor, she was able to discover an undetected medical condition in her client during the massage. ";
        if (girl.charisma() < 50)
        {
            ss << "The customer was devastated to get such news from a massage and numbly accepted the referral for treatment.\n";
        }
        else
        {
            ss << "The customer was shocked to get such news, but was calmed by ${name}'s kind explanations, and happily accepted the referral for treatment.\n";
            brothel->m_Happiness += 20;
        }
    }
    else
    {
        brothel->m_Happiness += (rng % 70) + 30;
        brothel->m_MiscCustomers++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift."; work -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working."; work += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    girl.upd_Enjoyment(actiontype, work);

    girl.AddMessage(ss.str(), imageType, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    int xp = 15, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    girl.medicine(rng%skill);
    girl.service(rng%skill + 1);


#pragma endregion
    return false;
}

double JP_BrothelMasseuse(const sGirl& girl, bool estimate)// not used
{
    //SIN - standardizing job performance per J's instructs
    double jobperformance =
        //Core stats - first 100: how well she serves customers and understanding of body
        ((girl.service() + girl.medicine()) / 2) +
        //secondary stats - second 100 - strength to do this all night, and attractiveness
        ((girl.strength() + girl.beauty() + girl.charisma()) / 3) +
        //add level
        girl.level();

    // next up tiredness penalty...
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.masseuse");

    return jobperformance;
}
