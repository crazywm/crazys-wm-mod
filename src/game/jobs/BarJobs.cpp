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

#include "xml/util.h"
#include "BarJobs.h"
#include "cGirls.h"
#include "Game.hpp"
#include "sStorage.h"
#include "buildings/IBuilding.h"
#include "buildings/cBuildingManager.h"
#include "character/sGirl.h"

bool cBarJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!
    return JobProcessing(girl, *brothel, is_night);
}

cBarJob::cBarJob(JOBS job, const char* xml, sBarJobData data) : cBasicJob(job, xml), m_Data(data) {
    RegisterVariable("Wages", m_Wages);
}

void cBarJob::HandleGains(sGirl& girl, int enjoy, int fame) {
    // update enjoyment
    girl.upd_Enjoyment(m_Data.Action, enjoy);

    if (girl.fame() < 10 && m_Performance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && m_Performance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && m_Performance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && m_Performance >= 185)        { fame += 1; }

    girl.fame(fame);

    apply_gains(girl);
}

IGenericJob::eCheckWorkResult cBarJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (girl.libido() >= 90 && girl.has_active_trait("Nymphomaniac") && chance(20))
    {
        add_text("event.nympho-nowork");
        girl.upd_temp_stat(STAT_LIBIDO, -20);
        girl.AddMessage(ss.str(), IMGTYPE_MAST, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    else if (girl.disobey_check(m_Data.Action, job()))
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    /*else if (brothel->m_TotalCustomers < 1)
    {
        ss << "There were no customers in the bar on the " << (is_night ? "night" : "day") << " shift so ${name} just cleaned up a bit.";
        brothel->m_Filthiness -= 20 + girl.service() * 2;
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;
    }*/
    return eCheckWorkResult::ACCEPTS;
}

void cBarJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    auto wages = job_element.FirstChildElement("WageFunction");
    if(wages) {
        PerformanceToWages = LoadLinearFunction(*wages, "Performance", "Wages");
    }
}

void cBarJob::perf_text() {
    if (m_Performance >= 245)
    {
        add_text("work.perfect") << "\n";
    }
    else if (m_Performance >= 185)
    {
        add_text("work.great") << "\n";
    }
    else if (m_Performance >= 145)
    {
        add_text("work.great") << "\n";
    }
    else if (m_Performance >= 100)
    {
        add_text("work.ok") << "\n";
    }
    else if (m_Performance >= 70)
    {
        add_text("work.bad") << "\n";
    }
    else
    {
        add_text("work.worst") << "\n";
    }
}

struct cBarCookJob : public cBarJob {
    cBarCookJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;


};

cBarCookJob::cBarCookJob() : cBarJob(JOB_BARCOOK, "BarCook.xml", {ACTION_WORKBAR}) {
}

bool cBarCookJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night)
{
    int roll_a = d100(), roll_b = d100();
    add_text("work") << "\n \n";

    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!

    int enjoy = 0, fame = 0;
    m_Wages = PerformanceToWages((float)m_Performance);

    int imagetype = IMGTYPE_COOK;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;


#pragma endregion
#pragma region //    Job Performance            //

    //a little pre-game randomness
    add_text("pre-work-text");

    perf_text();

    add_text("post-work-text");

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

    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Wages += uniform(10, 10 + roll_max);
    // Money
    girl.m_Tips = 0;
    girl.m_Pay = std::max(0, m_Wages);

    // Improve stats
    HandleGains(girl, enjoy, fame);

#pragma endregion
    return false;
}



struct cBarMaidJob : public cBarJob {
    cBarMaidJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarMaidJob::cBarMaidJob() : cBarJob(JOB_BARMAID, "BarMaid.xml", {ACTION_WORKBAR}) {
}

bool cBarMaidJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKBAR;
    std::stringstream ss;
    int roll_jp = d100(), roll_e = d100(), roll_c = d100();
    add_text("work") << "\n \n";

    m_Wages = PerformanceToWages(m_Performance);
    m_Tips = 0;
    int enjoy = 0, fame = 0;                // girl
    int Bhappy = 0, Bfame = 0, Bfilth = 0;    // brothel
    int imagetype = IMGTYPE_WAIT;
    int msgtype = is_night;

#pragma endregion
#pragma region //    Job Performance            //

    int numbarmaid = brothel.num_girls_on_job(JOB_BARMAID, is_night);
    int numbarwait = brothel.num_girls_on_job(JOB_WAITRESS, is_night);
    int numbargirls = numbarmaid + numbarwait;
    int numallcust = brothel.m_TotalCustomers;
    int numhercust = (numallcust / numbargirls)
                     + uniform(0, (girl.charisma() / 10) - 3)
                     + uniform(0, (girl.beauty() / 10) - 1);
    if (numhercust < 0) numhercust = 1;
    if (numhercust > numallcust) numhercust = numallcust;

    double drinkssold = 0;                                            // how many drinks she can sell in a shift
    for (int i = 0; i < numhercust; i++)
    {
        drinkssold += 1 + uniform(0, m_Performance / 30);    // 200jp can serve up to 7 drinks per customer
    }
    double drinkswasted = 0;                                        // for when she messes up an order

    if (chance(20))
    {
        // FIXME: We *know* that `actiontype == ACTION_WORKBAR ==
        // 5`. Figure out a condition here that makes sense and isn't
        // too swingy.
        if (actiontype >= 75)
        {
            ss << "Excited to get to work ${name} brings her 'A' game " << (is_night ? "tonight." : "today.");
            m_Performance += 40;
        }
        else if (actiontype <= 25)
        {
            ss << "The thought of working " << (is_night ? "tonight." : "today.") << " made ${name} feel uninspired so she didn't really try.";
            m_Performance -= 40;
        }
    }

    //what is she wearing?

    if (girl.has_item("Bourgeoise Gown") && chance(60))
    {
        int bg = rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        /* */if (bg < 0)    ss << "A few customers did not really like ${name}'s Bourgeoise Gown.";
        else if (bg > 0)    ss << "A few customers complimented ${name}'s Bourgeoise Gown.";
        else/*        */    ss << "${name}'s Bourgeoise Gown didn't really help or hurt her tips.";
        ss << "\n \n";
    }
    else if (girl.has_item("Maid Uniform"))
    {
        int bg = rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        /* */if (bg < 0)    ss << "A few customers teased ${name} for wearing a Maid's Uniform in a bar.";
        else if (bg > 0)    ss << "${name}'s Maid Uniform didn't do much for most of the patrons, but a few of them seemed to really like it.";
        else/*        */    ss << "${name}'s Maid Uniform didn't do much to help her.";
        ss << "\n \n";
    }

    //a little pre-game randomness
    if (girl.has_active_trait("Alcoholic"))
    {
        if (chance(10))
        {
            ss << "${name}'s alcoholic nature caused her to drink several bottles of booze becoming drunk and her serving suffered cause of it.";
            m_Performance -= 50;
            drinkswasted += uniform(10, 20);
        }
        ss << "\n \n";
    }

    if (m_Performance >= 245)
    {
        drinkssold *= 1.6;
        roll_e += 10;        // enjoy adj
        add_text("work.perfect");
    }
    else if (m_Performance >= 185)
    {
        drinkssold *= 1.3;
        roll_e += 7;        // enjoy adj
        add_text("work.great");
    }
    else if (m_Performance >= 145)
    {
        drinkssold *= 1.1;
        roll_e += 3;        // enjoy adj
        add_text("work.good");
    }
    else if (m_Performance >= 100)
    {
        drinkssold *= 1.0;
        roll_e += 0;        // enjoy adj
        add_text("work.ok");
    }
    else if (m_Performance >= 70)
    {
        drinkssold *= 0.9;
        roll_e -= 3;        // enjoy adj
        add_text("work.bad");
    }
    else
    {
        drinkssold *= 0.8;
        roll_e -= 7;        // enjoy adj
        add_text("work.worst");
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Tips and Adjustments        //

    m_Tips += (drinkssold - drinkswasted) * ((double)roll_e / 100.0);    //base tips

    add_text("post-work-text");

    // `J` slightly lower percent compared to sleazy barmaid, I would think regular barmaid's uniform is less revealing
    if ((chance(3) && girl.has_active_trait("Busty Boobs")) ||
        (chance(6) && girl.has_active_trait("Big Boobs")) ||
        (chance(9) && girl.has_active_trait("Giant Juggs")) ||
        (chance(12) && girl.has_active_trait("Massive Melons")) ||
        (chance(16) && girl.has_active_trait("Abnormally Large Boobs")) ||
        (chance(20) && girl.has_active_trait("Titanic Tits")))
    {
        ss << "A patron was obviously staring at her large breasts. ";
        if (m_Performance < 150)
        {
            ss << "But she had no idea how to take advantage of it.\n";
        }
        else
        {
            ss << "So she over-charged them for drinks while they were too busy drooling to notice the price.\n";
            m_Wages += 15;
        }
    }

    if (girl.dignity() <= -20 && chance(20) && (girl.has_active_trait("Big Boobs") ||
                                                     girl.has_active_trait("Abnormally Large Boobs") ||
                                                     girl.has_active_trait("Titanic Tits") ||
                                                     girl.has_active_trait("Massive Melons") ||
                                                     girl.has_active_trait("Giant Juggs")))
    {
        m_Tips += 25;
        ss << "${name} got an odd request from a client to carry a small drink he ordered between her tits to his table. "
              "After pouring the drink in a thin glass, ${name} handled the task with minimal difficulty and earned a bigger tip.\n";
    }

    if (girl.morality() <= -20 && chance(10))
    {
        ss << "A patron came up to her and said he wanted to order some milk but that he wanted it straight from the source. ";
        if (girl.lactation() >= 20)
        {
            ss << "With a smile she said she was willing to do it for an extra charge. The patron quickly agreed and ${name} proceed to take out one of her tits and let the patron suck out some milk.\n";
            girl.lactation(-20);
            m_Tips += 40;
        }
        else
        {
            ss << "She was willing to do it but didn't have enough milk production.";
        }
    }

    if (girl.is_pregnant() && chance(10))
    {
        ss << "A customer tried to buy ${name} a drink, but she refused for the sake of her unborn child.";
    }

    if ((girl.has_active_trait("Deep Throat") || girl.has_active_trait("No Gag Reflex")) && chance(5))
    {
        ss << "Some customers were having a speed drinking contest and challenged ${name} to take part.\n";
        if (girl.is_pregnant()) ss << "She refused for the sake of her unborn child.";
        else
        {
            ss << "Her talent at getting things down her throat meant she could pour the drink straight down. She won easily, earning quite a bit of gold.";
            m_Tips += 30;
        }
    }

    if (girl.has_item("Golden Pendant") && chance(10))//zzzzz FIXME need more CRAZY
    {
        ss << "A patron complimented her gold necklace, you're not sure if it was an actual compliment or ";
        if (girl.has_active_trait("Massive Melons") || girl.has_active_trait("Abnormally Large Boobs")
            || girl.has_active_trait("Titanic Tits") || girl.has_active_trait("Big Boobs")
            || girl.has_active_trait("Busty Boobs") || girl.has_active_trait("Giant Juggs"))
        {
            ss << "an excuse to stare at her ample cleavage.";
        }
        else
        {
            ss << "an attempt to get a discount on their bill.";
        }
        girl.happiness(5);//girls like compliments
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_e <= 10)
    {
        enjoy -= uniform(1, 3);
        drinkssold *= 0.9;
        ss << "\nSome of the patrons abused her during the shift.";
    }
    else if (roll_e >= 90)
    {
        drinkssold *= 1.1;
        enjoy += uniform(1, 3);
        ss << "\nShe had a pleasant time working.";
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "\nOtherwise, the shift passed uneventfully.";
    }
    ss << "\n \n";

    if (girl.is_unpaid())
        drinkssold *= 0.9;

#pragma endregion
#pragma region    //    Money                    //

    // drinks are sold for 3gp each, if there are not enough in stock they cost 1gp.
    int ds = std::max(0, (int)drinkssold);
    int dw = std::max(0, (int)drinkswasted);
    int d1 = ds + dw;                                                    // all drinks needed
    int d2 = g_Game->storage().drinks() >= d1 ? d1 : g_Game->storage().drinks();        // Drinks taken from stock
    int d3 = g_Game->storage().drinks() >= d1 ? 0 : d1 - g_Game->storage().drinks();    // Drinks needed to be bought
    int profit = (ds * 3) - (d3 * 1);
    g_Game->storage().add_to_drinks(-d2);
    if (profit < 0) profit = profit;
    else/*       */ profit = profit;
    if ((int)d1 > 0)
    {
        ss << "\n${name}";
        /* */if ((int)drinkssold <= 0)    ss << " didn't sell any drinks.";
        else if ((int)drinkssold == 1)    ss << " only sold one drink.";
        else/*                      */    ss << " sold " << ds << " drinks.";
        /* */if ((int)dw > 0)    ss << "\n" << dw << " were not paid for or were spilled.";
        /* */if (d2 > 0)/*           */ ss << "\n" << d2 << " drinks were taken from the bar's stock.";
        /* */if (d3 > 0)/*           */ ss << "\n" << d3 << " drinks had to be restocked durring the week at a cost of 1 gold each.";
        ss << "\n \n${name}";
        /* */if (profit > 0)/*       */    ss << " made you a profit of " << profit << " gold.";
        else if (profit < 0)/*       */    ss << " cost you " << profit << " gold.";
        else if (d1 > 0)/*           */ ss << " broke even for the week.";
        else/*                       */ ss << " made no money.";
    }

    if (girl.is_unpaid())
    {
        bool keep_tips = girl.keep_tips();
        if (!keep_tips)
        {
            m_Wages += (int)m_Tips;
            m_Tips = 0;
        }
        /* */if ((int)m_Wages > 0)    ss << "\n${name} turned in an extra " << (int)m_Wages << " gold from other sources.";
        else if ((int)m_Wages < 0)    ss << "\nShe cost you " << (int)m_Wages << " gold from other sources.";
        if ((int)m_Tips > 0 && keep_tips)
        {
            ss << "\nShe made " << (int)m_Tips << " gold in tips";
            if ((int)m_Wages < 0)
            {
                ss << " but you made her pay back what she could of the losses";
                int l = (int)m_Tips + (int)m_Wages;
                if (l > 0)        // she can pay it all
                {
                    m_Tips -= l;
                    m_Wages += l;
                }
                else
                {
                    m_Wages += (int)m_Tips;
                    m_Tips = 0;
                }
            }
            ss << ".";
        }
        profit += (int)m_Wages;    // all of it goes to the house
        m_Wages = 0;
    }
    else
    {
        if (profit >= 10)    // base pay is 10 unless she makes less
        {
            ss << "\n \n"<< "${name} made the bar a profit so she gets paid 10 gold for the shift.";
            m_Wages += 10;
            profit -= 10;
        }
        if (profit > 0)
        {
            int b = profit / 50;
            if (b > 0) ss << "\nShe gets 2% of the profit from her drink sales as a bonus totaling " << b << " gold.";
            m_Wages += b;                    // 2% of profit from drinks sold
            profit -= b;
            girl.happiness(b / 5);
        }
        if (dw > 0)
        {
            girl.happiness(-(dw / 5));

            int c = std::min(dw, (int)m_Wages);
            int d = std::min(dw - c, (int)m_Tips);
            int e = std::min(0, dw - d);
            bool left = false;
            if (dw < (int)m_Wages)                    // she pays for all wasted drinks out of wages
            {
                ss << "\nYou take 1 gold out of her pay for each drink she wasted ";
                m_Wages -= c;
                profit += c;
                left = true;
            }
            else if (dw < (int)m_Wages + (int)m_Tips)    // she pays for all wasted drinks out of wages and tips
            {
                ss << "\nYou take 1 gold from her wages and tips for each drink she wasted ";
                m_Wages -= c;
                m_Tips -= d;
                profit += c + d;
                left = true;
            }
            else                                    // no pay plus she has to pay from her pocket
            {
                m_Wages -= c;
                m_Tips -= d;
                profit += c + d;
                if (girl.m_Money < 1)                // she can't pay so you scold her
                {
                    girl.pcfear(rng().bell(-1,5));
                    ss << "\nYou take all her wages and tips and then scold her for wasting so many drinks";
                }
                else if (girl.m_Money >= e)        // she has enough to pay it back
                {
                    girl.pcfear(rng().bell(-1, 2));
                    girl.pchate(rng().bell(-1, 2));
                    ss << "\nYou take all her wages and tips and then make her pay for the rest out of her own money";
                    girl.m_Money -= e;
                    profit += e;
                }
                else                                // she does not have all but can pay some
                {
                    girl.pcfear(rng().bell(-1, 4));
                    girl.pchate(rng().bell(-1, 2));
                    ss << "\nYou take all her wages and tips and then make her pay for what she can of the rest out of her own money";
                    e = girl.m_Money;
                    girl.m_Money -= e;
                    profit += e;
                }
            }

            if (left)
            {
                ss << "leaving her with ";
                /* */if ((int)m_Wages + (int)m_Tips < 1)    ss << "nothing";
                else if ((int)m_Wages + (int)m_Tips < 2)    ss << "just one gold";
                else/*                            */    ss << (int)m_Wages + (int)m_Tips << "gold";
            }
            ss << ".";
        }
    }



    // tiredness
#if 1    // `J` had some issues with tiredness so replacing it with a less complicated method until a better way is found 'Mute' Updated to fix math logic if this doesnt work feel free to switch back
    int t0 = d1;
    int easydrinks = (girl.constitution() + girl.service()) / 4;
    int t1 = easydrinks;                    // 1 tired per 20 drinks
    int t2 = std::max(0, t0 - t1);                // 1 tired per 10 drinks
    int t3 = std::max(0, t0 - (t1+t2));            // 1 tired per 2 drinks
    int tired = std::max(0,(t1/20))+std::max(0,(t2/10))+std::max(0,(t3/2));
#else
    int tired = max(1, (600 - ((int)jobperformance + (girl.constitution() * 3))) / 10);

#endif

    // Money
    girl.m_Tips = std::max(0, (int)m_Tips);
    girl.m_Pay = std::max(0, m_Wages);

    g_Game->gold().bar_income(profit);

#pragma endregion
#pragma region    //    Finish the shift            //

    brothel.m_Happiness += Bhappy;
    brothel.m_Fame += Bfame;
    brothel.m_Filthiness += Bfilth;

    girl.AddMessage(ss.str(), imagetype, msgtype ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    HandleGains(girl, enjoy, fame);

#pragma endregion
    return false;
}

struct cBarWaitressJob : public cBarJob {
    cBarWaitressJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarWaitressJob::cBarWaitressJob() : cBarJob(JOB_WAITRESS, "BarWaitress.xml",{ACTION_WORKBAR}) {
}

bool cBarWaitressJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_a = d100(), roll_c = d100();
    add_text("work") << "\n";

    const sGirl* barmaidonduty = random_girl_on_job(*girl.m_Building, JOB_BARMAID, is_night);
    std::string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->FullName() : "the Barmaid");

    const sGirl* cookonduty = random_girl_on_job(*girl.m_Building, JOB_BARCOOK, is_night);
    std::string cookname = (cookonduty ? "Cook " + cookonduty->FullName() : "the cook");

    int enjoy = 0, fame = 0;

    m_Wages = PerformanceToWages((float)m_Performance);
    m_Tips = 0;

    int imagetype = IMGTYPE_WAIT;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    int HateLove = girl.pclove() - girl.pchate();

#pragma endregion
#pragma region //    Job Performance            //

    //a little pre-game randomness
    add_text("pre-work-text");
    perf_text();
    add_text("post-work-text");

    //base tips, aprox 10-20% of base wages
    m_Tips += (int)(((10 + m_Performance / 22) * m_Wages) / 100);

    //try and add randomness here
    if (girl.libido() > 90 && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus") ||
                               girl.has_active_trait("Slut")))
    {
        ss << "During her shift, ${name} couldn't help but instinctively and excessively rub her ass against the crotches of the clients whenever she got the chance. Her slutty behavior earned her some extra tips, as a couple of patrons noticed her intentional butt grinding.\n";
        m_Tips += 30;
    }

    if (brothel.num_girls_on_job( JOB_BARMAID, false) >= 1 && chance(25))
    {
        if (m_Performance < 125)
        {
            ss << "${name} wasn't good enough at her job to use " << barmaidname << " to her advantage.\n";
        }
        else
        {
            ss << "${name} used " << barmaidname << " to great effect speeding up her work and increasing her tips.\n";
            m_Tips += 25;
        }
    }

    //BIRTHDAY /**/
    if (girl.m_BDay >= 51)
    {
        if (girl.is_slave())
        {
        }
        else
        {
            if (HateLove >= 80)
            {
                if (girl.libido() >= 60)
                {
                    ss << "${name} has barely finished her shift before she is changed into her sexiest dress and standing before you. \"I have a little birthday wish,\" she whispers, drawing closer to you. \"I thought maybe, as your gift to me, you would allow me to serve you alone tonight. I asked " << cookname << " to cook all your favorite dishes, and I've prepared the upper dining area so it will just be the two of us.\" She leads you upstairs and seats you before disappearing for a moment and returning with the first course. ${name} feeds you with her own hands, giggling with every few bites. \"We have a cake, of course,\" she smiles as you finish everything, \"but that isn't the only dessert.\"\n";
                    if (roll_c >= 80)//ANAL
                    {
                        ss << "${name} lifts up her skirt so you can see that she is not wearing underwear. \"I was hoping that you might put your birthday present in my ass,\" she whispers into your ear, deftly opening your pants and lowering herself onto your suddenly and ragingly erect cock. She whimpers briefly as your dick penetrates her, then she spits on her hand and rubs the lubricant onto your tip before impaling herself again. \"You have no idea how often I fantasize about this when dealing with those stodgy customers all day,\" she pants, reveling as you ream her ass. \"Use me like a dirty backstreet whore,\" she continues, wrapping her asshole around you and bouncing up and down. It does not take long to cum for both of you. ${name} smiles";
                        ss << " with fulfillment as she lifts herself off your cock, semen leaking onto the table. \"I guess I'll need to clean that up,\" she comments, before turning back to you. \"Happy birthday to me,\" she grins. \"Let's do it again sometime.\"";
                        imagetype = IMGTYPE_ANAL;
                        girl.anal(1);
                    }
                    else if (roll_c >= 50)//SEX
                    {
                        ss << "${name} lies flat on her back on the cleared table, hiking up her dress so you have direct access to her wet pussy and placing the cake on her stomach. \"If you want dessert, I think you should come and get it,\" she purrs. You insert your hard-on into her and slowly fuck her as she moans, stopping only for a moment to take a piece of cake. You eat a bite and then feed her the rest as you pump with increasing speed, and as she takes the last bite, you spurt deep inside her. \"Happy birthday to me,\" she smiles.";
                        imagetype = IMGTYPE_SEX;
                        girl.normalsex(1);
                    }
                    else//ORAL
                    {
                        if (girl.oralsex() >= 50 && girl.has_active_trait("Deep Throat"))
                        {
                            ss << "${name} does not even wait for a reply before she moves her hand to your cock, deftly opening your pants and working you to a raging hard-on. She smiles mischievously at you and then dives down, swallowing your whole cock with one quick motion. She stays there, locked with her tongue on your balls and your shaft buried in her throat, massaging your cock with swallowing motions while staring with watering eyes into yours, until she begins to lose oxygen. You cum buckets straight down her throat as she begins to choke herself on you, and when she has secured every drop in her stomach, she pulls back, takes a deep breath, and smiles. \"Happy birthday to me,\" she says.";
                        }
                        else if (girl.oralsex() >= 50)
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is excellent, and you relish every moment, taking an occasional bite of cake as she latches onto your dick.";
                        }
                        else
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is not amazing, but it is delivered with such tenderness and love that you find yourself very satisfied regardless.";
                        }
                        imagetype = IMGTYPE_ORAL;
                        girl.oralsex(1);
                    }
                }
                else
                {
                    ss << "${name} finished her work and came to you with a shy grin. \"Did you know that it's my birthday?\" she asks, brushing against you gently and continuing without waiting for a response. \"I asked " << cookname << " to make a little something special, and I thought maybe we could share it together.\" The two of you enjoy a delicious light dinner, followed by an adorable little cake, as she giggles appreciably at your jokes and flirts with you. \"Maybe we should do this again without waiting a whole year,\" she suggests at the end of the evening, eyes flashing with unspoken promises. \"I'd love to thank you more fully for everything you have done for me.\"\n";
                }
            }
            else
            {
                ss << "${name} finished her work as a waitress and returned home for a private birthday celebration with her friends.\n";
            }
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


    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Wages += uniform(10, 10+roll_max);
    // Money
    girl.m_Tips = std::max(0, m_Tips);
    girl.m_Pay = std::max(0, m_Wages);

    // Improve stats
    HandleGains(girl, enjoy, fame);
    if (m_Performance > 150 && girl.constitution() > 65)
    {
        cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, m_Data.Action, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet of Foot.", is_night);
    }
    
#pragma endregion
    return false;
}

struct cBarPianoJob : public cBarJob {
    cBarPianoJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarPianoJob::cBarPianoJob() : cBarJob(JOB_PIANO, "BarPiano.xml", {ACTION_WORKMUSIC}) {
}

bool cBarPianoJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_a = d100(), roll_b = d100();
    
    add_text("work");

    const sGirl* singeronduty = random_girl_on_job(brothel, JOB_SINGER, is_night);
    std::string singername = (singeronduty ? "Singer " + singeronduty->FullName() + "" : "the Singer");

    int wages = 20, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_PROFILE;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    tips = (int)((m_Performance / 8.0) * ((rng() % (girl.beauty() + girl.charisma()) / 20.0) + (girl.performance() / 5.0)));

    perf_text();

    //SIN - bit of randomness.
    if (chance(brothel.m_Filthiness / 50))
    {
        ss << "Soon after she started her set, some rats jumped out of the piano and fled the building. Patrons could be heard laughing.";
        brothel.m_Fame -= uniform(0, 1);            // 0 to -1
    }
    ss << "\n \n";

    add_text("post-work-text");

    if (brothel.num_girls_on_job(JOB_SINGER, false) >= 1 && chance(25))
    {
        if (m_Performance < 125)
        {
            ss << "${name} played poorly with " << singername << " making people leave.\n";
            tips = int(tips * 0.8);
        }
        else
        {
            ss << "${name} played well with " << singername << " increasing tips.\n";
            tips = int(tips * 1.1);
        }
    }

#pragma endregion
#pragma region    //    Enjoyment                //

    //enjoyed the work or not
    if (roll_b <= 10)
    {
        ss << "Some of the patrons abused her during the shift.";
        enjoy -= uniform(1, 3);
        tips = int(tips * 0.9);
    }
    else if (roll_b >= 90)
    {
        ss << "She had a pleasant time working.";
        enjoy += uniform(1, 3);
        tips = int(tips * 1.1);
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += uniform(0, 1);
    }

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        wages = 0;
        tips = int(tips * 0.9);
    }
    else
    {
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Update Enjoyment
    if (m_Performance < 50) enjoy -= 1;
    if (m_Performance < 0) enjoy -= 1;    // if she doesn't do well at the job, she enjoys it less
    if (m_Performance > 200) enjoy *= 2;        // if she is really good at the job, her enjoyment (positive or negative) is doubled

    // Base Improvement and trait modifiers
    HandleGains(girl, enjoy, fame);

    // Push out the turn report
    girl.AddMessage(ss.str(), imagetype, msgtype);
    return false;
}


struct cBarSingerJob : public cBarJob {
    cBarSingerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarSingerJob::cBarSingerJob() : cBarJob(JOB_SINGER, "BarSinger.xml",{ACTION_WORKMUSIC}) {
}

bool cBarSingerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) 
{
    int roll_a = d100(), roll_b = d100();
    add_text("work") << "\n \n";

    int enjoy = 0, happy = 0, fame = 0;
    int imagetype = IMGTYPE_SING;
    EventType msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    m_Wages = PerformanceToWages((float)m_Performance);
    m_Tips = 0;

#pragma endregion
#pragma region //    Job Performance            //

    const sGirl* pianoonduty = random_girl_on_job(*girl.m_Building, JOB_PIANO, is_night);
    std::string pianoname = (pianoonduty ? "Pianist " + pianoonduty->FullName() + "" : "the Pianist");

    add_text("song-choice");
    ss << " ";
    add_text("song-quality");
    ss << "\n";

    perf_text();

    //base tips, aprox 5-30% of base wages
    m_Tips += (int)(((5 + m_Performance / 8) * m_Wages) / 100);

    //try and add randomness here
    add_text("post-work-text");

    if (brothel.num_girls_on_job(JOB_PIANO, is_night) >= 1 && chance(25))
    {
        if (m_Performance < 125)
        {
            ss << "${name}'s singing was out of tune with " << pianoname << " causing customers to leave with their fingers in their ears.\n"; m_Tips -= 10;
        }
        else
        {
            ss << pianoname << " took her singing to the next level causing the tips to flood in.\n"; m_Tips += 40;
        }
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift."; enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working."; enjoy += 3; girl.confidence(1);
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully."; enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    brothel.m_Fame += fame;
    brothel.m_Happiness += happy;
    
    girl.AddMessage(ss.str(), imagetype, msgtype);
    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Wages += uniform(10, 10 + roll_max);

    // Money
    girl.m_Tips = std::max(0, m_Tips);
    girl.m_Pay = std::max(0, m_Wages);

    // Improve stats
    HandleGains(girl, enjoy, fame);
    if (girl.fame() >= 70 && chance(10))
    {
        cGirls::PossiblyGainNewTrait(girl, "Idol", 50, m_Data.Action, "Her fame and singing skills has made ${name} an Idol in Crossgate.", is_night);
    }


#pragma endregion
    return false;
}

void RegisterBarJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cBarCookJob>());
    mgr.register_job(std::make_unique<cBarMaidJob>());
    mgr.register_job(std::make_unique<cBarWaitressJob>());
    mgr.register_job(std::make_unique<cBarPianoJob>());
    mgr.register_job(std::make_unique<cBarSingerJob>());
}

