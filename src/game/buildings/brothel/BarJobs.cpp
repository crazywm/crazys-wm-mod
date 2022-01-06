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
#include "IGame.h"
#include "sStorage.h"
#include "buildings/IBuilding.h"
#include "buildings/cBuildingManager.h"
#include "character/sGirl.h"
#include "character/predicates.h"
#include "cInventory.h"
#include "character/cCustomers.h"
#include "character/cPlayer.h"

sWorkJobResult cBarJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!
    return {JobProcessing(girl, *brothel, is_night), m_Tips, m_Earnings, m_Earnings};
}

cBarJob::cBarJob(JOBS job, const char* xml, sBarJobData data) : cBasicJob(job, xml), m_Data(data) {
}

void cBarJob::HandleGains(sGirl& girl, int enjoy, int fame) {
    // update enjoyment
    girl.upd_Enjoyment(m_Data.Action, enjoy);

    if (girl.fame() < 10 && m_Performance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && m_Performance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && m_Performance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && m_Performance >= 185)        { fame += 1; }

    girl.fame(fame);

    apply_gains(girl, m_Performance);
}

IGenericJob::eCheckWorkResult cBarJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.libido() >= 90 && girl.has_active_trait("Nymphomaniac") && chance(20))
    {
        add_text("event.nympho-nowork");
        girl.upd_temp_stat(STAT_LIBIDO, -20);
        girl.AddMessage(ss.str(), IMGTYPE_MAST, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    } else {
        return SimpleRefusalCheck(girl, m_Data.Action);
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
        PerformanceToEarnings = LoadLinearFunction(*wages, "Performance", "Wages");
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
        add_text("work.good") << "\n";
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
    m_Earnings = PerformanceToEarnings((float)m_Performance);

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
    m_Earnings += uniform(10, 10 + roll_max);

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

    m_Earnings = PerformanceToEarnings(float(m_Performance));
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
            m_Earnings += 15;
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
            m_Earnings += (int)m_Tips;
            m_Tips = 0;
        }
        /* */if ((int)m_Earnings > 0)    ss << "\n${name} turned in an extra " << (int)m_Earnings << " gold from other sources.";
        else if ((int)m_Earnings < 0)    ss << "\nShe cost you " << (int)m_Earnings << " gold from other sources.";
        if ((int)m_Tips > 0 && keep_tips)
        {
            ss << "\nShe made " << (int)m_Tips << " gold in tips";
            if ((int)m_Earnings < 0)
            {
                ss << " but you made her pay back what she could of the losses";
                int l = (int)m_Tips + (int)m_Earnings;
                if (l > 0)        // she can pay it all
                {
                    m_Tips -= l;
                    m_Earnings += l;
                }
                else
                {
                    m_Earnings += (int)m_Tips;
                    m_Tips = 0;
                }
            }
            ss << ".";
        }
        profit += (int)m_Earnings;    // all of it goes to the house
        m_Earnings = 0;
    }
    else
    {
        if (profit >= 10)    // base pay is 10 unless she makes less
        {
            ss << "\n \n"<< "${name} made the bar a profit so she gets paid 10 gold for the shift.";
            m_Earnings += 10;
            profit -= 10;
        }
        if (profit > 0)
        {
            int b = profit / 50;
            if (b > 0) ss << "\nShe gets 2% of the profit from her drink sales as a bonus totaling " << b << " gold.";
            m_Earnings += b;                    // 2% of profit from drinks sold
            profit -= b;
            girl.happiness(b / 5);
        }
        if (dw > 0)
        {
            girl.happiness(-(dw / 5));

            int c = std::min(dw, (int)m_Earnings);
            int d = std::min(dw - c, (int)m_Tips);
            int e = std::min(0, dw - d);
            bool left = false;
            if (dw < (int)m_Earnings)                    // she pays for all wasted drinks out of wages
            {
                ss << "\nYou take 1 gold out of her pay for each drink she wasted ";
                m_Earnings -= c;
                profit += c;
                left = true;
            }
            else if (dw < (int)m_Earnings + (int)m_Tips)    // she pays for all wasted drinks out of wages and tips
            {
                ss << "\nYou take 1 gold from her wages and tips for each drink she wasted ";
                m_Earnings -= c;
                m_Tips -= d;
                profit += c + d;
                left = true;
            }
            else                                    // no pay plus she has to pay from her pocket
            {
                m_Earnings -= c;
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
                /* */if ((int)m_Earnings + (int)m_Tips < 1)    ss << "nothing";
                else if ((int)m_Earnings + (int)m_Tips < 2)    ss << "just one gold";
                else/*                            */    ss << (int)m_Earnings + (int)m_Tips << "gold";
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

    m_Earnings = PerformanceToEarnings((float)m_Performance);

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
    m_Tips += (int)(((10 + m_Performance / 22) * m_Earnings) / 100);

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
    m_Earnings += uniform(10, 10+roll_max);

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

    m_Earnings = 20;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_PIANO;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    m_Tips = (int)((m_Performance / 8.0) * ((rng() % (girl.beauty() + girl.charisma()) / 20.0) + (girl.performance() / 5.0)));

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
            m_Tips = int(m_Tips * 0.8);
        }
        else
        {
            ss << "${name} played well with " << singername << " increasing tips.\n";
            m_Tips = int(m_Tips * 1.1);
        }
    }

#pragma endregion
#pragma region    //    Enjoyment                //

    //enjoyed the work or not
    if (roll_b <= 10)
    {
        ss << "Some of the patrons abused her during the shift.";
        enjoy -= uniform(1, 3);
        m_Tips = int(m_Tips * 0.9);
    }
    else if (roll_b >= 90)
    {
        ss << "She had a pleasant time working.";
        enjoy += uniform(1, 3);
        m_Tips = int(m_Tips * 1.1);
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
        m_Earnings = 0;
        m_Tips = int(m_Tips * 0.9);
    }
    else
    {
    }

#pragma endregion
#pragma region    //    Finish the shift            //

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

    m_Earnings = PerformanceToEarnings((float)m_Performance);

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
    m_Tips += (int)(((5 + m_Performance / 8) * m_Earnings) / 100);

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
    m_Earnings += uniform(10, 10 + roll_max);

    // Improve stats
    HandleGains(girl, enjoy, fame);
    if (girl.fame() >= 70 && chance(10))
    {
        cGirls::PossiblyGainNewTrait(girl, "Idol", 50, m_Data.Action, "Her fame and singing skills has made ${name} an Idol in Crossgate.", is_night);
    }


#pragma endregion
    return false;
}


class cDealerJob : public cBarJob {
public:
    cDealerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

// TODO rename this, I would expect 'Dealer' to refer to a different job
cDealerJob::cDealerJob() : cBarJob(JOB_DEALER, "Dealer.xml", sBarJobData{ACTION_WORKHALL}) {
}

bool cDealerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    add_text("work") << "\n \n";
    int roll_a = d100();

    m_Earnings = 25;
    int work = 0, fame = 0;
    int imagetype = IMGTYPE_CARD;

#pragma endregion
#pragma region //    Job Performance            //

    const sGirl* enteronduty = random_girl_on_job(*girl.m_Building, JOB_ENTERTAINMENT, is_night);
    std::string entername = (enteronduty ? "Entertainer " + enteronduty->FullName() + "" : "the Entertainer");
    const sGirl* xxxenteronduty = random_girl_on_job(*girl.m_Building, JOB_XXXENTERTAINMENT, is_night);
    std::string xxxentername = (xxxenteronduty ? "Entertainer " + xxxenteronduty->FullName() + "" : "the Sexual Entertainer");


    //a little pre-game randomness
    if (chance(10))
    {
        if (girl.has_active_trait("Strange Eyes"))
        {
            ss << " ${name}'s strange eyes were somehow hypnotic, giving her some advantage.";
            m_Performance += 15;
        }
        if (girl.has_active_trait("Nymphomaniac") && girl.libido() > 75)
        {
            ss << " ${name} had very high libido, making it hard for her to concentrate.";
            m_Performance -= 10;
        }
        if (girl.footjob() > 50)
        {
            ss << " ${name} skillfully used her feet under the table to break customers' concentration.";
            m_Performance += 5;
        }
    }
    if (is_addict(girl, true) && chance(20))
    {
        ss << "\nNoticing her addiction, a customer offered her drugs. She accepted, and had an awful day at the card table.\n";
        if (girl.has_active_trait("Shroud Addict"))
        {
            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        }
        if (girl.has_active_trait("Fairy Dust Addict"))
        {
            girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        }
        if (girl.has_active_trait("Viras Blood Addict"))
        {
            girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        }
        m_Performance -= 50;
    }

    perf_text();
    m_Earnings += PerformanceToEarnings((float)m_Performance);

    //I'm not aware of tipping card dealers being a common practice, so no base tips


    // try and add randomness here
    add_text("after-work") << "\n";

    if (brothel.num_girls_on_job(JOB_ENTERTAINMENT, false) >= 1 && chance(25))
    {
        if (m_Performance < 125)
        {
            ss << "${name} wasn't good enough at her job to use " << entername << "'s distraction to make more money.\n";
        }
        else
        {
            ss << "${name} used " << entername << "'s distraction to make you some extra money.\n"; m_Earnings += 25;
        }
    }

    //SIN: a bit more randomness
    if (chance(20) && m_Earnings < 20 && girl.charisma() > 60)
    {
        ss << "${name} did so badly, a customer felt sorry for her and left her a few coins from his winnings.\n";
        m_Earnings += uniform(3, 20);
    }
    if (chance(5) && girl.normalsex() > 50 && girl.fame() > 30)
    {
        ss << "A customer taunted ${name}, saying the best use for a dumb whore like her is bent over the gambling table.";
        bool spirited = (girl.spirit() + girl.spirit() > 80);
        if (spirited)
        {
            ss << "\n\"But this way\"${name} smiled, \"I can take your money, without having to try and find your penis.\"";
        }
        else
        {
            ss << "She didn't acknowledge it in any way, but inwardly determined to beat him.";
        }
        if (m_Performance >= 145)
        {
            ss << "\nShe cleaned him out, deliberately humiliating him and taunting him into gambling more than he could afford. ";
            ss << "He ended up losing every penny and all his clothes to this 'dumb whore'. He was finally kicked out, naked into the streets.\n \n";
            ss << "${name} enjoyed this. A lot.";
            girl.upd_Enjoyment(ACTION_WORKHALL, 3);
            girl.happiness(5);
            m_Earnings += 100;
        }
        else if (m_Performance >= 99)
        {
            ss << "\nShe managed to hold her own, and in the end was just happy not to lose to a guy like this.";
        }
        else
        {
            ss << "\nSadly her card skills let her down and he beat her in almost every hand. He finally stood up pointing at the table:";
            ss << "\n\"If you wanna make your money back, whore, you know what to do.\"";
            if (spirited)
            {
                ss << "\"Bend over it then,\" she scowled. \"I'll show you where you can shove those gold coins.\"\nHe left laughing.";
            }
            else
            {
                ss << "\"I'm not doing that today, sir,\" she mumbled. \"But there are other girls.\"\nHe left for the brothel.";
            }
            ss << "\n \nShe really hated losing at this stupid card game.";
            girl.upd_Enjoyment(ACTION_WORKHALL, -3);
            girl.happiness(-5);
            m_Earnings -= 50;
        }
    }

    if (brothel.num_girls_on_job(JOB_XXXENTERTAINMENT, false) >= 1)
    {
        if (m_Performance < 125)
        {
            if (!girl.has_active_trait("Straight"))
            {
                if (girl.libido() > 90)
                {
                    ss << "${name} found herself looking at " << xxxentername << "'s performance often, losing more times than usual.\n";
                    m_Earnings = int(m_Earnings * 0.9);
                }
                else
                {
                    ss << "${name} wasn't good enough at her job to use " << xxxentername << "'s distraction to make more money.\n";
                }
            }
            else
            {
                ss << "${name} wasn't good enough at her job to use " << xxxentername << "'s distraction to make more money.\n";
            }
        }
        else
        {
            ss << "${name} took advantage of " << xxxentername << "'s show to win more hands and make some extra money.\n";
            m_Earnings = int(m_Earnings * 1.2);
        }
    }

    if (m_Earnings < 0) m_Earnings = 0;

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
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // work out the pay between the house and the girl
    m_Earnings += uniform(10, (girl.beauty() + girl.charisma()) / 4 + 10);

    // Improve girl
    if (!girl.has_active_trait("Straight"))    { girl.upd_temp_stat(STAT_LIBIDO, std::min(3, brothel.num_girls_on_job(JOB_XXXENTERTAINMENT, false))); }
    HandleGains(girl, work, fame);

    return false;
}

IGenericJob::eCheckWorkResult cDealerJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKHALL);
}


class cEntertainerJob : public cBarJob {
public:
    cEntertainerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

cEntertainerJob::cEntertainerJob() : cBarJob(JOB_ENTERTAINMENT, "Entertainer.xml", sBarJobData{ACTION_WORKHALL}) {
}


bool cEntertainerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    add_text("work") << "\n \n";

#pragma region //    Job setup                //

    const sGirl* dealeronduty = random_girl_on_job(*girl.m_Building, JOB_DEALER, is_night);
    std::string dealername = (dealeronduty ? "Dealer " + dealeronduty->FullName() + "" : "the Dealer");

    m_Earnings = 25;
    int work = 0, fame = 0;
    int imagetype = IMGTYPE_BUNNY;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    //SIN: A little pre-randomness
    if (chance(50))
    {
        if (girl.tiredness() > 75)
        {
            ss << "She was very tired, negatively affecting her performance.\n";
            m_Performance -= 10;
        }
        else if (girl.happiness() > 90)
        {
            ss << "Her cheeriness improved her performance.\n";
            m_Performance += 5;
        }
        if (chance(10))
        {
            if (girl.strip() > 60)
            {
                ss << "A born stripper, ${name} wears her clothes just short of showing flesh, just the way the customers like it.\n";
                m_Performance += 15;
            }
            if (girl.pchate() > girl.pcfear())
            {
                ss << " ${name} opened with some rather rude jokes about you. While this annoys you a little, ";
                if (girl.has_active_trait("Your Daughter"))
                {
                    ss << "she is your daughter, and ";
                }
                ss << "it seems to get the audience on her side.\n";
                m_Performance += 15;
            }
        }
    }

    perf_text();
    m_Earnings += PerformanceToEarnings((float)m_Performance);


    //base tips, aprox 5-30% of base wages
    m_Tips += (int)(((5 + m_Performance / 8) * m_Earnings) / 100);

    // try and add randomness here
    add_text("after-work") << "\n";

    if (brothel.num_girls_on_job(JOB_DEALER, false) >= 1 && chance(25))
    {
        if (m_Performance < 125)
        {
            ss << "${name} tried to distract the patrons but due to her lack of skills she distracted " << dealername << " causing you to lose some money.\n";
            m_Earnings -= 10;
        }
        else
        {
            ss << "${name} was able to perfectly distract some patrons while the " << dealername << " cheated to make some more money.\n";
            m_Earnings += 25;
        }
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    int roll_a = d100();
    if (roll_a <= 5)
    {
        ss << "\nSeveral patrons heckled her and made her shift generally unpleasant.";
        work -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a great time working.";
        work += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        work += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //


    HandleGains(girl, work, fame);
    girl.AddMessage(ss.str(), imagetype, msgtype);


    m_Earnings += uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);

#pragma endregion
    return false;
}

IGenericJob::eCheckWorkResult cEntertainerJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKHALL);
}

class cXXXEntertainerJob : public cBarJob {
public:
    cXXXEntertainerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};


cXXXEntertainerJob::cXXXEntertainerJob() : cBarJob(JOB_XXXENTERTAINMENT, "XXXEntertainer.xml", sBarJobData{ACTION_WORKSTRIP}) {
}

bool cXXXEntertainerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    add_text("work") << "\n \n";

#pragma region //    Job setup                //

    m_Earnings = 25;
    int work = 0, fame = 0;
    int imagetype = IMGTYPE_BUNNY;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    // SIN: A little pre-show randomness - temporary stats that may affect show
    if (chance(20))
    {
        if (girl.tiredness() > 75)
        {
            ss << "${name} was very tired. This affected her performance. ";
            m_Performance -= 10;
        }
        else if (girl.libido() > 30)
        {
            ss << "${name}'s horniness improved her performance. ";
            m_Performance += 10;
        }

        if (girl.has_active_trait("Demon") || girl.has_active_trait("Shape Shifter") || girl.has_active_trait("Construct") ||
            girl.has_active_trait("Cat Girl") || girl.has_active_trait("Succubus") || girl.has_active_trait("Reptilian"))
        {
            ss << "Customers are surprised to see such an unusual girl giving sexual entertainment. ";
            ss << "Some are disgusted, some are turned on, but many can't help watching.\n";
            ss << "The dealers at the tables make a small fortune from distracted guests. ";
            m_Earnings += 30;
            fame += 1;
        }
        else if (girl.age() > 30 && chance(std::min(90, std::max((girl.age() - 30) * 3, 1))) && girl.beauty() < 30)
        {    //"Too old!" - chance of heckle: age<30y= 0%, then 4%/year (32y - 6%, 40y - 30%...) max 90%... (but only a 20% chance this bit even runs)
            // note: demons are exempt as they age differently
            ss << "Some customers heckle ${name} over her age.";
            ss << "\n\"Gross!\" \"Grandma is that you!?\"\n";
            ss << "This makes it harder for her to work this shift. ";
            m_Performance -= 20;
        }
        else if (girl.has_active_trait("Exotic"))
        {
            ss << "The customers were pleasantly surprised to see such an exotic girl giving sexual entertainment.";
            m_Earnings += 15;
            fame += 1;
        }
        if ((girl.has_active_trait("Syphilis") || girl.has_active_trait("Herpes"))
            && chance(100 - girl.health()))
        {
            ss << "She's unwell. A man in the audience recognises ${name}'s symptoms and heckles her about her ";
            if (girl.has_active_trait("Syphilis") && girl.has_active_trait("Herpes"))
            {
                ss << "diseases";
            }
            else if (girl.has_active_trait("Herpes"))
            {
                ss << "Herpes";
            }
            else if (girl.has_active_trait("Syphilis"))
            {
                ss << "Syphilis";
            }
            else
            {
                ss << "diseases";
            }
            ss << ". This digusts some in the audience and results in further heckling which disrupts ";
            ss << "her performance and makes her very uncomfortable. ";
            m_Performance -= 60;
            girl.happiness(-10);
            fame -= 3;
        }
        ss << "\n";
    }

    perf_text();
    m_Earnings += PerformanceToEarnings((float)m_Performance);


    //base tips, aprox 5-30% of base wages
    m_Tips += (int)(((5 + m_Performance / 6) * m_Earnings) / 100);

    // try and add randomness here
    add_text("after-work") << "\n";

    //try and add randomness here

    if (girl.has_active_trait("Your Daughter") && chance(20))
    {
        ss << "Word got around that ${name} is your daughter, so more customers than normal came to watch her perform.\n";
        m_Earnings += (m_Earnings / 5);
        if (g_Game->player().disposition() > 0)
        {
            ss << "This is about the nicest job you can give her. She's safe here and the customers can only look - ";
        }
        else
        {
            ss << "At the end of the day, she's another whore to manage, it's a job that needs doing and ";
        }
        if (m_Performance >= 120)
        {
            ss << " she shows obvious talent at this.\n";
            fame += 5;
        }
        else
        {
            ss << " it's just a damn shame she sucks at it.\n";
        }
    }

    if (girl.libido() > 90)
    {
        if (girl.has_active_trait("Futanari"))
        {
            //Addiction bypasses confidence check
            if (girl.has_active_trait("Cum Addict"))
            {
                //Autofellatio, belly gets in the way if pregnant, requires extra flexibility
                if (girl.has_active_trait("Flexible") && !girl.is_pregnant() && chance(50))
                {
                    ss << "During her shift ${name} couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
                    girl.oralsex(1);
                    girl.happiness(1);
                    fame += 1;
                    m_Tips += 30;
                }
                else
                {
                    //default Cum Addict
                    ss << "${name} won't miss a chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
                    girl.happiness(1);
                    m_Tips += 10;
                }
                cJobManager::GetMiscCustomer(brothel);
                brothel.m_Happiness += 100;
                girl.upd_temp_stat(STAT_LIBIDO, -30, true);
                // work out the pay between the house and the girl
                m_Earnings += girl.askprice() + 60;
                fame += 1;
                imagetype = IMGTYPE_MAST;
            }
                //Let's see if she has what it takes to do it: Confidence > 65 or Exhibitionist trait, maybe shy girls should be excluded
            else if (!girl.has_active_trait("Cum Addict") && girl.has_active_trait("Exhibitionist") || !girl.has_active_trait(
                    "Cum Addict") && girl.confidence() > 65)
            {
                //Some variety
                //Autopaizuri, requires very big breasts
                if (chance(25) && girl.has_active_trait("Abnormally Large Boobs") || chance(25) && (girl.has_active_trait(
                        "Titanic Tits")))
                {
                    ss << "${name} was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
                    girl.tittysex(1);
                    fame += 1;
                    m_Tips += 30;
                }
                    //cums over self
                else if (girl.dignity() < -40 && chance(25))
                {
                    ss << "The customers were really impressed when ${name} finished her show by cumming all over herself";
                    m_Tips += 10;
                }
                    //Regular futa masturbation
                else
                {
                    ss << "${name}'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
                    brothel.m_Filthiness += 1;
                }
                cJobManager::GetMiscCustomer(brothel);
                brothel.m_Happiness += 100;
                girl.upd_temp_stat(STAT_LIBIDO, -30, true);
                // work out the pay between the house and the girl
                m_Earnings += girl.askprice() + 60;
                fame += 1;
                imagetype = IMGTYPE_MAST;
            }
            else
            {
                ss << "There was a noticeable bulge in ${name}'s panties but she didn't have enough confidence to masturbate in public.";
            }
        }
            //regular masturbation code by Crazy tweaked to exclude futas and keep the original Libido > 90 requirement
        else if (!girl.has_active_trait("Futanari") && girl.libido() > 90)
        {
            ss << "She was horny and ended up masturbating for the customers making them very happy.";
            cJobManager::GetMiscCustomer(brothel);
            brothel.m_Happiness += 100;
            girl.upd_temp_stat(STAT_LIBIDO, -20, true);
            // work out the pay between the house and the girl
            m_Earnings += girl.askprice() + 60;
            fame += 1;
            imagetype = IMGTYPE_MAST;
        }
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    int roll_a = d100();
    if (roll_a <= 5)
    {
        ss << "\nSeveral patrons heckled her and made her shift generally unpleasant.";
        work -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a great time working.";
        work += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        work += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //


    HandleGains(girl, work, fame);
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }

    girl.AddMessage(ss.str(), imagetype, msgtype);


    m_Earnings += uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);

#pragma endregion
    return false;
}

IGenericJob::eCheckWorkResult cXXXEntertainerJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKSTRIP);
}

class cMasseuseJob : public cBarJob {
public:
    cMasseuseJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

cMasseuseJob::cMasseuseJob() : cBarJob(JOB_MASSEUSE, "Masseuse.xml", sBarJobData{ACTION_WORKMASSEUSE}) {
}

IGenericJob::eCheckWorkResult cMasseuseJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKMASSEUSE);
}

bool cMasseuseJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    add_text("work") << "\n \n";

    m_Earnings = girl.askprice() + 40;
    int imageType = IMGTYPE_MASSAGE;
    int fame = 0;
    int work = 0;

    perf_text();
    m_Earnings += PerformanceToEarnings((float)m_Performance);

    //base tips, aprox 5-30% of base wages
    m_Tips += (int)(((5 + m_Performance / 8) * m_Earnings) / 100);

    if (girl.libido() > 90)
        //ANON: TODO: sanity check: not gonna give 'perks' to the cust she just banned for wanting perks!
    {
        int n;
        ss << "Because she was quite horny, she ended up ";
        sCustomer Cust = g_Game->GetCustomer(brothel);
        brothel.m_Happiness += 100;
        if (Cust.m_IsWoman && brothel.is_sex_type_allowed(SKILL_LESBIAN))
        {
            n = SKILL_LESBIAN, ss << "intensely licking the female customer's clit until she got off, making the lady very happy.\n";
        }
        else
        {
            switch (uniform(0, 10))
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
        m_Earnings += 225;
        m_Tips += 30 + girl.get_skill(n) / 5;
        girl.upd_Enjoyment(ACTION_SEX, +1);
        fame += 1;
        girl.m_NumCusts++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    } //SIN - bit more spice - roll_c doesn't seem to be used anywhere else so ok here
    else if (girl.has_active_trait("Doctor") && chance(5))
    {
        ss << "Due to ${name}'s training as a Doctor, she was able to discover an undetected medical condition in her client during the massage. ";
        if (girl.charisma() < 50)
        {
            ss << "The customer was devastated to get such news from a massage and numbly accepted the referral for treatment.\n";
        }
        else
        {
            ss << "The customer was shocked to get such news, but was calmed by ${name}'s kind explanations, and happily accepted the referral for treatment.\n";
            brothel.m_Happiness += 20;
        }
    }
    else
    {
        brothel.m_Happiness += uniform(30, 100);
        brothel.m_MiscCustomers++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    }

#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    int roll_a = d100();
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

    HandleGains(girl, work, fame);
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }

    girl.AddMessage(ss.str(), imageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    return false;
}

class cPeepShowJob : public cBarJob {
public:
    cPeepShowJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

cPeepShowJob::cPeepShowJob() : cBarJob(JOB_PEEP, "PeepShow.xml", sBarJobData{ACTION_WORKSTRIP}) {
}

IGenericJob::eCheckWorkResult cPeepShowJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKSTRIP);
}

bool cPeepShowJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_c = d100();
    m_Earnings = girl.askprice() + uniform(0, 50);
    m_Tips = std::max(uniform(-10, 40), 0);
    int enjoy = 0, fame = 0;
    SKILLS sextype = SKILL_STRIP;
    int imagetype = IMGTYPE_STRIP;

#pragma endregion
#pragma region //    Job Performance            //

    double mod = 0.0;

    if (m_Performance >= 245)
    {
        mod = 3.0;
    }
    else if (m_Performance >= 185)
    {
        mod = 2.0;
    }
    else if (m_Performance >= 145)
    {
        mod = 1.5;
    }
    else if (m_Performance >= 100)
    {
        mod = 1.0;
    }
    else if (m_Performance >= 70)
    {
        mod = 0.9;
    }
    else
    {
        mod = 0.8;
    }


    //try and add randomness here
    if (girl.libido() > 80)
    {
        if (girl.has_active_trait("Lesbian") && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus")))
        {
            ss << "\nShe was horny and she loves sex so she brought in another girl and had sex with her while the customers watched.\n";
            sextype = SKILL_LESBIAN;
            /* `J` cGirls::GirlFucks handles skill gain from sex
            girl.lesbian(1);
            //*/
        }
        else if (girl.has_active_trait("Bisexual") && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus")))
        {
            if (roll_c <= 50)
            {
                ss << "\nShe was horny and she loves sex so she brought in another girl and had sex with her while the customers watched.\n";
                sextype = SKILL_LESBIAN;
                /* `J` cGirls::GirlFucks handles skill gain from sex
                girl.lesbian(1);
                //*/
            }
            else
            {
                ss << "\nShe was horny and she loves sex so she brought in one of the customers and had sex with him while the others watched.";
                sextype = SKILL_NORMALSEX;
            }
        }
        else if (girl.has_active_trait("Straight") && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus")))
        {
            ss << "\nShe was horny and she loves sex so she brought in one of the customers and had sex with him while the others watched.\n";
            sextype = SKILL_NORMALSEX;
        }
            // new stuff
        else if (girl.has_active_trait("Futanari"))
        {
            //Addiction bypasses confidence check
            if (girl.has_active_trait("Cum Addict"))
            {
                //Autofellatio, belly gets in the way if pregnant, requires extra flexibility
                if (girl.has_active_trait("Flexible") && !(girl.is_pregnant()) && chance(50))
                {
                    ss << "\nDuring her shift ${name} couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
                    girl.oralsex(1);
                    girl.happiness(1);
                    fame += 1;
                    m_Tips += 30;
                }
                else
                {
                    //default Cum Addict
                    ss << "\n${name} won't miss a chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
                    girl.happiness(1);
                    m_Tips += 10;
                }
                cJobManager::GetMiscCustomer(brothel);
                brothel.m_Happiness += 100;
                girl.upd_temp_stat(STAT_LIBIDO, -30, true);
                // work out the pay between the house and the girl
                m_Earnings += girl.askprice() + 60;
                fame += 1;
                imagetype = IMGTYPE_MAST;
            }
                //Let's see if she has what it takes to do it: Confidence > 65 or Exhibitionist trait, maybe shy girls should be excluded
            else if (!girl.has_active_trait("Cum Addict") && girl.has_active_trait("Exhibitionist") || !girl.has_active_trait(
                    "Cum Addict") && girl.confidence() > 65)
            {
                //Some variety
                //Autopaizuri, requires very big breasts
                if (chance(25) && girl.has_active_trait("Abnormally Large Boobs") || chance(25) && (girl.has_active_trait(
                        "Titanic Tits")))
                {
                    ss << "\n${name} was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
                    girl.tittysex(1);
                    fame += 1;
                    m_Tips += 30;
                }
                    //cums over self
                else if (girl.dignity() < -40 && chance(25))
                {
                    ss << "\nThe customers were really impressed when ${name} finished her show by cumming all over herself";
                    m_Tips += 10;
                }
                    //Regular futa masturbation
                else
                {
                    ss << "\n${name}'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
                    brothel.m_Filthiness += 1;
                }
                cJobManager::GetMiscCustomer(brothel);
                brothel.m_Happiness += 100;
                girl.upd_temp_stat(STAT_LIBIDO, -30, true);
                // work out the pay between the house and the girl
                m_Earnings += girl.askprice() + 60;
                fame += 1;
                imagetype = IMGTYPE_MAST;
            }
            else
            {
                ss << "\nThere was a noticeable bulge in ${name}'s panties but she didn't have enough confidence to masturbate in public.";
            }
        }
        else
        {
            //GirlFucks handles all the stuff for the other events but it isn't used here so everything has to be added manually
            //It's is the same text as the XXX entertainer masturbation event, so I'll just copy the rest
            ss << "\nShe was horny and ended up masturbating for the customers, making them very happy.";
            cJobManager::GetMiscCustomer(brothel);
            brothel.m_Happiness += 100;
            girl.upd_temp_stat(STAT_LIBIDO, -30, true);
            // work out the pay between the house and the girl
            m_Earnings += girl.askprice() + 60;
            fame += 1;
            imagetype = IMGTYPE_MAST;
        }
    }
    else if (chance(5))  //glory hole event
    {
        ss << "A man managed to cut a hole out from his booth and made himself a glory hole, ${name} saw his cock sticking out and ";
        {
            if (girl.has_active_trait("Meek") || girl.has_active_trait("Shy"))
            {
                enjoy -= 5;
                ss << "meekly ran away from it.\n";
            }
            else if (girl.has_active_trait("Lesbian"))
            {
                enjoy -= 2;
                girl.upd_temp_stat(STAT_LIBIDO, -10, true);
                ss << "she doesn't understand the appeal of them, which turned her off.\n";
            }
            else if (brothel.is_sex_type_allowed(SKILL_NORMALSEX) && !is_virgin(girl) && (girl.has_active_trait(
                    "Nymphomaniac") ||
                                                                                           girl.has_active_trait("Succubus")) && girl.libido() >= 80) //sex
            {
                sextype = SKILL_NORMALSEX;
                ss << "decided she needed to use it for her own entertainment.\n";
            }
            else if (brothel.is_sex_type_allowed(SKILL_ORALSEX) && (girl.has_active_trait("Nymphomaniac") ||
                                                                     girl.has_active_trait("Succubus") ||
                                                                     girl.has_active_trait("Cum Addict")) && girl.libido() >= 60) //oral
            {
                sextype = SKILL_ORALSEX;
                ss << "decided she needed to taste it.\n";
            }
            else if (brothel.is_sex_type_allowed(SKILL_FOOTJOB) && (girl.has_active_trait("Nymphomaniac") ||
                                                                     girl.has_active_trait("Succubus") || girl.dignity() < -30) && girl.libido() >= 40) //foot
            {
                sextype = SKILL_FOOTJOB;
                imagetype = IMGTYPE_FOOT;
                ss << "decided she would give him a foot job for being so brave.\n";
            }
            else if (brothel.is_sex_type_allowed(SKILL_HANDJOB) && (girl.has_active_trait("Nymphomaniac") ||
                                                                     girl.has_active_trait("Succubus") || girl.dignity() < -30))    //hand job
            {
                sextype = SKILL_HANDJOB;
                ss << "decided she would give him a hand job for being so brave.\n";
            }
            else
            {
                ss << "pointed and laughed.\n";
            }

            /* `J` suggest adding bad stuff,
            else if (girl.has_trait( "Merciless") && girl.has_item("Dagger") != -1 && chance(10))
            {
            imagetype = IMGTYPE_COMBAT;
            ss << "decided she would teach this guy a lesson and cut his dick off.\n";
            }
            *
            * This would probably require some other stuff and would deffinatly reduce her pay.
            * It may even get the girl arrested if the players suspicion were too high
            *
            //*/
        }
    }

    // `J` calculate base pay and tips with mod before special pay and tips are added
    m_Tips = std::max(0, int(m_Tips * mod));
    m_Earnings = std::max(0, int(m_Earnings * mod));

    if (girl.beauty() > 85 && chance(20))
    {
        ss << "Stunned by her beauty, a customer left her a great tip.\n \n";
        m_Tips += uniform(10, 60);
    }

    if (sextype != SKILL_STRIP)
    {
        // `J` get the customer and configure them to what is already known about them
        sCustomer Cust = cJobManager::GetMiscCustomer(brothel);
        Cust.m_Amount = 1;                                        // always only 1
        Cust.m_SexPref = sextype;                                // they don't get a say in this
        if (sextype == SKILL_LESBIAN) Cust.m_IsWoman = true;    // make sure it is a lesbian

        std::string message = ss.str();
        cGirls::GirlFucks(&girl, is_night, &Cust, false, message, sextype);
        ss.str(""); ss << message;
        brothel.m_Happiness += Cust.happiness();

        /* `J` cGirls::GirlFucks handles libido and customer happiness
        Cust.m_Stats[STAT_HAPPINESS] = max(100, Cust.m_Stats[STAT_HAPPINESS] + 50);
        girl.upd_temp_stat(STAT_LIBIDO, -20);
        //*/

        int sexwages = std::min(uniform(0, Cust.m_Money / 4) + girl.askprice(), int(Cust.m_Money));
        Cust.m_Money -= sexwages;
        int sextips = std::max(0, int(uniform(0, Cust.m_Money) - (Cust.m_Money / 2)));
        Cust.m_Money -= sextips;
        m_Earnings += sexwages;
        m_Tips += sextips;

        ss << "The customer she had sex with gave her " << sexwages << " gold for her services";
        if (sextips > 0) ss << " and slipped her another " << sextips << " under the table.\n \n";
        else ss << ".\n \n";

        if (imagetype == IMGTYPE_STRIP)
        {
            /* */if (sextype == SKILL_ANAL)            imagetype = IMGTYPE_ANAL;
            else if (sextype == SKILL_BDSM)            imagetype = IMGTYPE_BDSM;
            else if (sextype == SKILL_NORMALSEX)    imagetype = IMGTYPE_SEX;
            else if (sextype == SKILL_BEASTIALITY)    imagetype = IMGTYPE_BEAST;
            else if (sextype == SKILL_GROUP)        imagetype = IMGTYPE_GROUP;
            else if (sextype == SKILL_LESBIAN)        imagetype = IMGTYPE_LESBIAN;
            else if (sextype == SKILL_ORALSEX)        imagetype = IMGTYPE_ORAL;
            else if (sextype == SKILL_TITTYSEX)        imagetype = IMGTYPE_TITTY;
            else if (sextype == SKILL_HANDJOB)        imagetype = IMGTYPE_HAND;
            else if (sextype == SKILL_FOOTJOB)        imagetype = IMGTYPE_FOOT;
        }
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    int roll_a = d100();
    //enjoyed the work or not
    /* */if (roll_a <= 5)    { enjoy -= 1; ss << "\nSome of the patrons abused her during the shift."; }
    else if (roll_a <= 25)    { enjoy += 3; ss << "\nShe had a pleasant time working."; }
    else /*           */    { enjoy += 1; ss << "\nOtherwise, the shift passed uneventfully."; }

#pragma endregion
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(ACTION_WORKSTRIP, enjoy);

    // work out the pay between the house and the girl
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, enjoy, fame);

    //gain traits
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }
    if (sextype != SKILL_STRIP && girl.dignity() < 0 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
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
    mgr.register_job(std::make_unique<cEscortJob>());
    mgr.register_job(std::make_unique<cDealerJob>());
    mgr.register_job(std::make_unique<cEntertainerJob>());
    mgr.register_job(std::make_unique<cXXXEntertainerJob>());
    mgr.register_job(std::make_unique<cMasseuseJob>());
    mgr.register_job(std::make_unique<cPeepShowJob>());
}
