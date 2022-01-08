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
#include "BrothelJobs.h"
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

struct cBarCookJob : public cBarJob {
    cBarCookJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarCookJob::cBarCookJob() : cBarJob(JOB_BARCOOK, "BarCook.xml", {ACTION_WORKBAR}) {
}

bool cBarCookJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night)
{
    int fame = 0;
    m_Earnings = 15 + (int)m_PerformanceToEarnings((float)m_Performance);

    int imagetype = IMGTYPE_COOK;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    //a little pre-game randomness
    add_text("pre-work-text");
    add_performance_text();
    add_text("post-work-text");

    // enjoyed the work or not
    shift_enjoyment();

    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    m_Earnings += uniform(10, 10 + roll_max);

    // Improve stats
    HandleGains(girl, fame);
    return false;
}



struct cBarMaidJob : public cBarJob {
    cBarMaidJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarMaidJob::cBarMaidJob() : cBarJob(JOB_BARMAID, "BarMaid.xml", {ACTION_WORKBAR}) {
}

bool cBarMaidJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    //    Job setup                //
    Action_Types actiontype = ACTION_WORKBAR;
    int roll_jp = d100(), roll_e = d100(), roll_c = d100();

    m_Earnings = 15 + (int)m_PerformanceToEarnings(float(m_Performance));
    int fame = 0;                // girl
    int Bhappy = 0, Bfame = 0, Bfilth = 0;    // brothel
    int imagetype = IMGTYPE_WAIT;
    int msgtype = is_night;

    //    Job Performance            //

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

    add_performance_text();
    double drink_factors[] = {0.8, 0.9, 1.0, 1.1, 1.3, 1.6};
    drinkssold *= drink_factors[get_performance_class(m_Performance)];
    if (m_Performance >= 245)
    {
        roll_e += 10;        // enjoy adj
    }
    else if (m_Performance >= 185)
    {
        roll_e += 7;        // enjoy adj
    }
    else if (m_Performance >= 145)
    {
        roll_e += 3;        // enjoy adj
    }
    else if (m_Performance >= 100)
    {
        roll_e += 0;        // enjoy adj
    }
    else if (m_Performance >= 70)
    {
        roll_e -= 3;        // enjoy adj
    }
    else
    {
        roll_e -= 7;        // enjoy adj
    }

    //    Tips and Adjustments        //

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

    //    Money                    //
    shift_enjoyment();
    if (girl.is_unpaid())
        drinkssold *= 0.9;

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
    int t0 = d1;
    int easydrinks = (girl.constitution() + girl.service()) / 4;
    int t1 = easydrinks;                    // 1 tired per 20 drinks
    int t2 = std::max(0, t0 - t1);                // 1 tired per 10 drinks
    int t3 = std::max(0, t0 - (t1+t2));            // 1 tired per 2 drinks
    int tired = std::max(0,(t1/20))+std::max(0,(t2/10))+std::max(0,(t3/2));

    g_Game->gold().bar_income(profit);

    //    Finish the shift            //

    brothel.m_Happiness += Bhappy;
    brothel.m_Fame += Bfame;
    brothel.m_Filthiness += Bfilth;

    girl.AddMessage(ss.str(), imagetype, msgtype ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, fame);

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

    const sGirl* barmaidonduty = random_girl_on_job(*girl.m_Building, JOB_BARMAID, is_night);
    std::string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->FullName() : "the Barmaid");

    const sGirl* cookonduty = random_girl_on_job(*girl.m_Building, JOB_BARCOOK, is_night);
    std::string cookname = (cookonduty ? "Cook " + cookonduty->FullName() : "the cook");

    int fame = 0;

    m_Earnings = 15 + (int)m_PerformanceToEarnings((float)m_Performance);

    int imagetype = IMGTYPE_WAIT;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    int HateLove = girl.pclove() - girl.pchate();

    //    Job Performance            //

    //a little pre-game randomness
    add_text("pre-work-text");
    add_performance_text();
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

    //    Finish the shift            //

    //enjoyed the work or not
    shift_enjoyment();

    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Earnings += uniform(10, 10+roll_max);

    // Improve stats
    HandleGains(girl, fame);
    if (m_Performance > 150 && girl.constitution() > 65)
    {
        cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, m_Data.Action, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet of Foot.", is_night);
    }

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

    const sGirl* singeronduty = random_girl_on_job(brothel, JOB_SINGER, is_night);
    std::string singername = (singeronduty ? "Singer " + singeronduty->FullName() + "" : "the Singer");

    m_Earnings = 20;
    int fame = 0;
    int imagetype = IMGTYPE_PIANO;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    //    Job Performance            //

    m_Tips = (int)((m_Performance / 8.0) * ((rng() % (girl.beauty() + girl.charisma()) / 20.0) + (girl.performance() / 5.0)));

    add_performance_text();

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


    //    Enjoyment                //

    //enjoyed the work or not
    if (roll_b <= 10)
    {
        ss << "Some of the patrons abused her during the shift.";
        m_Enjoyment -= uniform(1, 3);
        m_Tips = int(m_Tips * 0.9);
    }
    else if (roll_b >= 90)
    {
        ss << "She had a pleasant time working.";
        m_Enjoyment += uniform(1, 3);
        m_Tips = int(m_Tips * 1.1);
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        m_Enjoyment += uniform(0, 1);
    }


    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        m_Earnings = 0;
        m_Tips = int(m_Tips * 0.9);
    }
    else
    {
    }

    //    Finish the shift            //

    // Base Improvement and trait modifiers
    HandleGains(girl, fame);

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

    int happy = 0, fame = 0;
    int imagetype = IMGTYPE_SING;
    EventType msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    m_Earnings = 15 + (int)m_PerformanceToEarnings((float)m_Performance);

    const sGirl* pianoonduty = random_girl_on_job(*girl.m_Building, JOB_PIANO, is_night);
    std::string pianoname = (pianoonduty ? "Pianist " + pianoonduty->FullName() + "" : "the Pianist");

    add_text("song-choice");
    ss << " ";
    add_text("song-quality");
    ss << "\n";

    add_performance_text();

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

    shift_enjoyment();

    brothel.m_Fame += fame;
    brothel.m_Happiness += happy;
    
    girl.AddMessage(ss.str(), imagetype, msgtype);
    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    m_Earnings += uniform(10, 10 + roll_max);

    // Improve stats
    HandleGains(girl, fame);
    if (girl.fame() >= 70 && chance(10))
    {
        cGirls::PossiblyGainNewTrait(girl, "Idol", 50, m_Data.Action, "Her fame and singing skills has made ${name} an Idol in Crossgate.", is_night);
    }

    return false;
}


class cDealerJob : public cSimpleJob {
public:
    cDealerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

// TODO rename this, I would expect 'Dealer' to refer to a different job
cDealerJob::cDealerJob() : cSimpleJob(JOB_DEALER, "Dealer.xml", {ACTION_WORKHALL}) {
}

bool cDealerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_a = d100();

    m_Earnings = 25;
    int fame = 0;
    int imagetype = IMGTYPE_CARD;

    //    Job Performance            //

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

    add_performance_text();
    m_Earnings += (int)m_PerformanceToEarnings((float)m_Performance);
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

    //    Finish the shift            //
    shift_enjoyment();

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // work out the pay between the house and the girl
    m_Earnings += uniform(10, (girl.beauty() + girl.charisma()) / 4 + 10);

    // Improve girl
    if (!girl.has_active_trait("Straight"))    { girl.upd_temp_stat(STAT_LIBIDO, std::min(3, brothel.num_girls_on_job(JOB_XXXENTERTAINMENT, false))); }
    HandleGains(girl, fame);

    return false;
}

class cEntertainerJob : public cSimpleJob {
public:
    cEntertainerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cEntertainerJob::cEntertainerJob() : cSimpleJob(JOB_ENTERTAINMENT, "Entertainer.xml", {ACTION_WORKHALL}) {
}


bool cEntertainerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
#pragma region //    Job setup                //

    const sGirl* dealeronduty = random_girl_on_job(*girl.m_Building, JOB_DEALER, is_night);
    std::string dealername = (dealeronduty ? "Dealer " + dealeronduty->FullName() + "" : "the Dealer");

    m_Earnings = 25;
    int fame = 0;
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

    add_performance_text();
    m_Earnings += (int)m_PerformanceToEarnings((float)m_Performance);


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

    //    Finish the shift            //
    shift_enjoyment();

    HandleGains(girl, fame);
    girl.AddMessage(ss.str(), imagetype, msgtype);

    m_Earnings += uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);

    return false;
}

class cXXXEntertainerJob : public cSimpleJob {
public:
    cXXXEntertainerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};


cXXXEntertainerJob::cXXXEntertainerJob() : cSimpleJob(JOB_XXXENTERTAINMENT, "XXXEntertainer.xml", {ACTION_WORKSTRIP}) {
}

bool cXXXEntertainerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    m_Earnings = 25;
    int fame = 0;
    int imagetype = IMGTYPE_BUNNY;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

     //    Job Performance            //

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

    add_performance_text();
    m_Earnings += m_PerformanceToEarnings((float)m_Performance);


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

    //    Finish the shift            //

    shift_enjoyment();
    HandleGains(girl, fame);
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }

    girl.AddMessage(ss.str(), imagetype, msgtype);


    m_Earnings += uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);
    return false;
}

class cMasseuseJob : public cSimpleJob {
public:
    cMasseuseJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cMasseuseJob::cMasseuseJob() : cSimpleJob(JOB_MASSEUSE, "Masseuse.xml", {ACTION_WORKMASSEUSE}) {
}

bool cMasseuseJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    m_Earnings = girl.askprice() + 40;
    int imageType = IMGTYPE_MASSAGE;
    int fame = 0;

    add_performance_text();
    m_Earnings += m_PerformanceToEarnings((float)m_Performance);

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

    shift_enjoyment();
    HandleGains(girl, fame);
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }

    girl.AddMessage(ss.str(), imageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    return false;
}

class cPeepShowJob : public cSimpleJob {
public:
    cPeepShowJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cPeepShowJob::cPeepShowJob() : cSimpleJob(JOB_PEEP, "PeepShow.xml", {ACTION_WORKSTRIP}) {
}

bool cPeepShowJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_c = d100();
    m_Earnings = girl.askprice() + uniform(0, 50);
    m_Tips = std::max(uniform(-10, 40), 0);
    int fame = 0;
    SKILLS sextype = SKILL_STRIP;
    int imagetype = IMGTYPE_STRIP;

    //    Job Performance            //

    double mods[] = {0.8, 0.9, 1.0, 1.5, 2.0, 3.0};
    double mod = mods[get_performance_class(m_Performance)];
    add_performance_text();

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
                m_Enjoyment -= 5;
                ss << "meekly ran away from it.\n";
            }
            else if (girl.has_active_trait("Lesbian"))
            {
                m_Enjoyment -= 2;
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

    //    Finish the shift            //
    shift_enjoyment();

    // work out the pay between the house and the girl
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, fame);

    //gain traits
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been having to be sexy for so long she now reeks  sexiness.", is_night);
    }
    if (sextype != SKILL_STRIP && girl.dignity() < 0 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }

    return false;
}

class cBrothelStripper : public cSimpleJob {
public:
    cBrothelStripper();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBrothelStripper::cBrothelStripper() : cSimpleJob(JOB_BROTHELSTRIPPER, "BrothelStripper.xml", {ACTION_WORKSTRIP}) {
}

bool cBrothelStripper::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int fame = 0;
    int imagetype = IMGTYPE_STRIP;
    m_Earnings = 45;

    //    Job Performance            //

    int lapdance = (girl.intelligence() / 2 +
                    girl.performance() / 2 +
                    girl.strip()) / 2;
    bool mast = false, sex = false;

    if (girl.beauty() > 90)
    {
        ss << "\nShe is so hot, customers were waving money to attract her to dance for them.";
        m_Tips += 20;
    }
    if (girl.intelligence() > 75)
    {
        ss << "\nShe was smart enough to boost her pay by playing two customers against one another.";
        m_Tips += 25;
    }
    if (girl.beauty() <= 90 && girl.intelligence() <= 75)
    {
        ss << "\nShe stripped for a customer.";
    }
    ss << "\n \n";


    //Adding cust here for use in scripts...
    sCustomer Cust = g_Game->GetCustomer(brothel);

    //A little more randomness
    if (Cust.m_IsWoman && (girl.has_active_trait("Lesbian") || girl.lesbian() > 60))
    {
        ss << "${name} was overjoyed to perform for a woman, and gave a much more sensual, personal performance.\n";
        m_Performance += 25;
    }

    if (girl.tiredness() > 70)
    {
        ss << "${name} was too exhausted to give her best tonight";
        if (m_Performance >= 120)
        {
            ss << ", but she did a fairly good job of hiding her exhaustion.\n";
            m_Performance -= 10;
        }
        else
        {
            ss << ". Unable to even mask her tiredness, she moved clumsily and openly yawned around customers.\n";
            m_Performance -= 30;
        }
    }

    add_performance_text();
    m_Earnings += (int)m_PerformanceToEarnings((float)m_Performance);

    //base tips, aprox 5-40% of base wages
    m_Tips += (int)(((5 + m_Performance / 6) * m_Earnings) / 100);

    // lap dance code.. just test stuff for now
    int roll_b = d100();
    if (lapdance >= 90)
    {
        ss << "${name} doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
        if (roll_b < 5)
        {
            ss << "She sold a champagne dance.";
            m_Tips += 250;
        }
        else if (roll_b < 20)
        {
            ss << "She sold a shower dance.\n";
            m_Tips += 125;
            if (girl.libido() > 70)
            {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                m_Earnings += 50;
                mast = true;
            }
        }
        else if (roll_b < 40)
        {
            ss << "She was able to sell a few VIP dances.\n";
            m_Tips += 160;
            if (chance(20)) sex = true;
        }
        else if (roll_b < 60)
        {
            ss << "She sold a VIP dance.\n";
            m_Tips += 75;
            if (chance(15)) sex = true;
        }
        else
        {
            ss << "She sold several lap dances.";
            m_Tips += 85;
        }
    }
    else if (lapdance >= 65)
    {
        ss << "${name}'s skill at selling private dances is impressive.\n";
        if (roll_b < 10)
        {
            ss << "She convinced a patron to buy a shower dance.\n";
            m_Tips += 75;
            if (girl.libido() > 70)
            {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                m_Tips += 50;
                mast = true;
            }
        }
        if (roll_b < 40)
        {
            ss << "Sold a VIP dance to a patron.\n";
            m_Tips += 75;
            if (chance(20))
            {
                sex = true;
            }
        }
        else
        {
            ss << "Sold a few lap dances.";
            m_Tips += 65;
        }
    }
    else if (lapdance >= 40)
    {
        ss << "${name} tried to sell private dances and ";
        if (roll_b < 5)
        {
            ss << "was able to sell a VIP dance against all odds.\n";
            m_Tips += 75;
            if (chance(10))
            {
                sex = true;
            }
        }
        if (roll_b < 20)
        {
            ss << "was able to sell a lap dance.";
            m_Tips += 25;
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
    add_text("event.post");

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

    if (is_addict(girl, true) && !sex && !mast && chance(60)) // not going to get money or drugs any other way
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
        brothel.m_Happiness += 100;
        //int imageType = IMGTYPE_SEX;
        if (Cust.m_IsWoman && brothel.is_sex_type_allowed(SKILL_LESBIAN))
        {
            n = SKILL_LESBIAN;
            ss << "licking the customer's clit until she screamed out in pleasure, making her very happy.";
        }
        else
        {
            switch (uniform(0, 10))
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
        m_Earnings += girl.askprice();
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        m_Earnings += uniform(0, roll_max) + 50;
        fame += 1;
        girl.m_NumCusts++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    }
    else if (mast)
    {
        brothel.m_Happiness += uniform(60, 130);
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        m_Earnings += 50 + uniform(0, roll_max);
        fame += 1;
        imagetype = IMGTYPE_MAST;
        //girl.m_Events.AddMessage(ss.str(), IMGTYPE_MAST, Day0Night1);
    }
    else
    {
        brothel.m_Happiness += uniform(30, 100);
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        m_Earnings += 10 + uniform(0, roll_max);
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

    //    Finish the shift            //

    if (girl.has_active_trait("Exhibitionist"))
    {
        m_Enjoyment += 1;
    }
    shift_enjoyment();
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, fame);

    //gained
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Agile", 40, ACTION_WORKSTRIP, "${name} has been working the pole long enough to become quite Agile.", is_night);
    }
    if (sex && girl.dignity() < 0 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }
    //lose
    if (m_Performance > 150 && girl.confidence() > 65)
    {
        cGirls::PossiblyLoseExistingTrait(girl, "Shy", 60, ACTION_WORKSTRIP, "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.", is_night);
    }
    return false;
}

class ClubBarmaid : public cSimpleJob {
public:
    ClubBarmaid();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};


ClubBarmaid::ClubBarmaid() : cSimpleJob(JOB_SLEAZYBARMAID, "StripBarMaid.xml", {ACTION_WORKCLUB}) {

}

bool ClubBarmaid::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int fame = 0;
    int imagetype = IMGTYPE_ECCHI;

    add_performance_text();
    m_Earnings = 15 + (int)m_PerformanceToEarnings((float)m_Performance);

    //base tips, aprox 10-20% of base m_Earnings
    m_Tips += (int)(((10 + m_Performance / 22) * m_Earnings) / 100);

    //try and add randomness here
    add_text("event.post");

    if (girl.has_active_trait("Clumsy") && chance(15))
    {
        ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n"; m_Earnings -= 15;
    }

    // `J` slightly higher percent compared to regular barmaid, I would think sleazy barmaid's uniform is more revealing
    if ((chance(5) && girl.has_active_trait("Busty Boobs")) ||
        (chance(10) && girl.has_active_trait("Big Boobs")) ||
        (chance(15) && girl.has_active_trait("Giant Juggs")) ||
        (chance(20) && girl.has_active_trait("Massive Melons")) ||
        (chance(25) && girl.has_active_trait("Abnormally Large Boobs")) ||
        (chance(30) && girl.has_active_trait("Titanic Tits")))
    {
        if (m_Performance < 150)
        {
            ss << "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n";
        }
        else
        {
            ss << "A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n"; m_Earnings += 15;
        }
    }

    if (girl.has_active_trait("Meek") && chance(5) && m_Performance < 125)
    {
        ss << "${name} spilled a drink all over a man's lap. He told her she had to lick it up and forced her to clean him up which she Meekly accepted and went about licking his cock clean.\n";
        imagetype = IMGTYPE_ORAL;
        m_Enjoyment -= 3;
    }

    if (chance(5)) //may get moved to waitress
    {
        add_text("event.grab-boob");
    }

    if ((girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus")) && girl.libido() > 80 && chance(20) && !is_virgin(girl) && !girl.has_active_trait(
            "Lesbian"))
    {
        add_text("event.nympho");
        imagetype = IMGTYPE_SEX;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.normalsex(1);
        sCustomer Cust = g_Game->GetCustomer(*girl.m_Building);
        Cust.m_Amount = 1;
        if (!girl.calc_pregnancy(Cust, 1.0))
        {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
        girl.m_NumCusts++;
    }

    //enjoyed the work or not
    shift_enjoyment();
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    m_Earnings += 10 + uniform(0, roll_max);

    // Improve stats
    HandleGains(girl, fame);

    //gained
    if (m_Performance < 100 && chance(2)) { cGirls::PossiblyGainNewTrait(girl, "Assassin", 10, ACTION_WORKCLUB, "${name}'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.", is_night); }
    if (chance(25) && girl.dignity() < 0 && (imagetype == IMGTYPE_SEX || imagetype == IMGTYPE_ORAL))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }
    return false;
}

class ClubStripper : public cSimpleJob {
public:
    ClubStripper();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

ClubStripper::ClubStripper() : cSimpleJob(JOB_BARSTRIPPER, "StripStripper.xml", {ACTION_WORKSTRIP}) {
}

bool ClubStripper::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_c = d100();

    m_Earnings = 30;
    int imagetype = IMGTYPE_STRIP;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    int lapdance = (girl.intelligence() / 2 +
                    girl.performance() / 2 +
                    girl.strip()) / 2;


    //what is she wearing?
    if (girl.has_item("Rainbow Underwear"))
    {
        ss << "${name} stripped down to reveal her Rainbow Underwear to the approval of the patrons watching her.\n \n";
        brothel.m_Happiness += 5; m_Performance += 5; m_Tips += 10;
    }
    else if (girl.has_item("Black Leather Underwear"))
    {
        ss << "${name} stripped down to reveal her Black Leather Underwear ";
        if (girl.has_active_trait("Sadistic"))
        {
            ss << "and broke out a whip asking who wanted to join her on stage for a spanking.\n \n";
        }
        if (girl.has_active_trait("Masochist"))
        {
            ss << "and asked a patron to come on stage and give her a spanking.\n \n";
        }
        else
        {
            ss << "which the patrons seemed to enjoy.\n \n";
        }
    }
    else if (girl.has_item("Adorable Underwear"))
    {
        ss << "${name} stripped down to reveal her Adorable Underwear which slightly help her out on tips.\n \n";
        m_Tips += 5;
    }
    else if (girl.has_item("Classy Underwear"))
    {
        ss << "${name} stripped down to reveal her Classy Underwear which some people seemed to like ";
        if (roll_c <= 50)
        {
            ss << "but others didn't seem to care for.\n \n";
        }
        else
        {
            ss << "and it helped her tips.\n \n"; m_Tips += 20;
        }
    }
    else if (girl.has_item("Comfortable Underwear"))
    {
        ss << "${name}'s Comfortable Underwear help her move better while on stage.\n \n";
        m_Performance += 5;
    }
    else if (girl.has_item("Plain Underwear"))
    {
        ss << "${name} stripped down to reveal her Plain Underwear which didn't help her performance as the patrons found them boring.\n \n";
        m_Performance -= 5;
    }
    else if (girl.has_item("Sexy Underwear"))
    {
        ss << "${name} stripped down to reveal her Sexy Underwear which brought many people to the stage to watch her.\n \n";
        m_Performance += 5; m_Tips += 15;
    }

    m_Earnings += (int)m_PerformanceToEarnings((float)m_Performance);
    add_performance_text();
    if (m_Performance >= 245)
    {
        brothel.m_Fame += 5;
        m_Tips += 15;
    }
    else if (m_Performance >= 185)
    {
        m_Tips += 10;
    }
    else if (m_Performance >= 145)
    {
        m_Tips += 5;
    }
    else if (m_Performance < 70)
    {
        brothel.m_Happiness -= 5;
        brothel.m_Fame -= 5;
    }

    //base tips, aprox 5-40% of base m_Earnings
    m_Tips += (int)(((5 + m_Performance / 6) * m_Earnings) / 100);

    add_text("event.post");

    // lap dance code.. just test stuff for now
    if (lapdance >= 90)
    {
        add_text("lapdance.great");
    }
    else if (lapdance >= 65)
    {
        add_text("lapdance.good");
    }
    else if (lapdance >= 40)
    {
        add_text("lapdance.ok");
    }
    else
    {
        add_text("lapdance.bad");
    }

    if (m_Earnings < 0)
        m_Earnings = 0;

    if (is_addict(girl, true) && chance(20))
    {
        const char* warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
        ss << "\n" << warning << "\n";
        if (girl.has_active_trait("Shroud Addict"))            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        if (girl.has_active_trait("Fairy Dust Addict"))        girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        if (girl.has_active_trait("Viras Blood Addict"))        girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        girl.AddMessage(warning, IMGTYPE_ORAL, EVENT_WARNING);
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
        girl.tiredness(10 - girl.strength() / 20);
    }

    if (girl.has_active_trait("Exhibitionist"))
    {
        m_Enjoyment += 1;
    }
    shift_enjoyment();

    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    m_Earnings += uniform(10, 10+roll_max);

    HandleGains(girl, 0);

    //gained
    if (m_Performance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Agile", 40, ACTION_WORKSTRIP, "${name} has been working the pole long enough to become quite Agile.", is_night);
    }

    //lose
    if (m_Performance > 150 && girl.confidence() > 65)
    {
        cGirls::PossiblyLoseExistingTrait(girl, "Shy", 60, ACTION_WORKSTRIP, "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.", is_night);
    }

    return false;
}

class ClubWaitress : public cSimpleJob {
public:
    ClubWaitress();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

ClubWaitress::ClubWaitress() : cSimpleJob(JOB_SLEAZYWAITRESS, "StripWaitress.xml", {ACTION_WORKCLUB}) {

}

bool ClubWaitress::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    m_Earnings = 25;
    int anal = 0, oral = 0, hand = 0, fame = 0;
    int imagetype = IMGTYPE_ECCHI;

    //    Job Performance            //

    auto undignified = [&](){
        switch (uniform(0, 10))
        {
            case 1:        girl.sanity(-uniform(0, 5));        if (chance(50)) break;
            case 2:        girl.confidence(-uniform(0, 5));    if (chance(50)) break;
            case 3:        girl.dignity(-uniform(0, 5));        if (chance(50)) break;
            default:       m_Enjoyment -= uniform(0, 5);    break;
        }
    };

    //a little pre-game randomness
    if (girl.has_active_trait("Cum Addict") && chance(30))
    {
        ss << "${name} is addicted to cum, and she cannot serve her shift without taking advantage of a room full of cocks. Since most of your patrons are already sexually primed with all this nubile flesh walking around in skimpy uniforms, she does not need to be very persuasive to convince various men to satisfy her addiction. You see her feet sticking out from under the tables from time to time as a satisfied customer smiles at the ceiling. Her service with the other tables suffers, but her tips are still quite high.\n";
        m_Performance -= 10;
        m_Tips += 40;
        imagetype = IMGTYPE_ORAL;
    }
    else if ((girl.has_active_trait("Shy") || girl.has_active_trait("Nervous")) && chance(20))
    {
        ss << "${name} has serious difficulty being around all these new people, and the fact that they are all so forward about her body does nothing to help. She spends a lot of time hiding in the kitchen, petrified of going back out and talking to all those people.";
        m_Performance -= 20;
    }

    add_performance_text();
    m_Earnings += (int)m_PerformanceToEarnings((float)m_Performance);
    if (m_Performance >= 245)
    {
        brothel.m_Fame += 5;
        m_Tips += 15;
    }
    else if (m_Performance >= 185)
    {
        m_Tips += 10;
    }
    else if (m_Performance >= 145)
    {
        m_Tips += 5;
    }
    else if (m_Performance < 70)
    {
        brothel.m_Happiness -= 5;
        brothel.m_Fame -= 5;
    }


    //base tips, aprox 10-20% of base wages
    m_Tips += (((10.0 + m_Performance / 22.0) * (double)m_Earnings) / 100.0);

    //try and add randomness here
    add_text("event.post");

    if (girl.has_active_trait("Great Arse") && chance(15))
    {
        if (m_Performance >= 185) //great
        {
            ss << "A patron reached out to grab her ass. But she skillfully avoided it";
            if (girl.libido() > 70 && !girl.has_active_trait("Lesbian"))
            {
                int roll_c = d100();
                std::string dick_type_text = "normal sized";
                if (roll_c <= 10)    { dick_type_text = "huge"; }
                else if (roll_c >= 90)    { dick_type_text = "small"; }

                ss << " and said that's only on the menu if your willing to pay up. "
                      "He jumped at the chance to get to try her ass out and bent "
                      "her over the table and whipping out his " << dick_type_text << " dick.";
                m_Earnings += girl.askprice() + 50;
                imagetype = IMGTYPE_ANAL;
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                bool fast_orgasm = girl.has_active_trait("Fast Orgasms");
                bool slow_orgasm = girl.has_active_trait("Slow Orgasms");
                // this construct is used many times below, so we write it out here once.
                auto OrgasmSelectText = [&](const char* fast, const char* slow, const char* def) {
                    if (fast_orgasm) { ss << fast; }
                    else if (slow_orgasm) { ss << slow; }
                    else { ss << def; }
                };

                if (roll_c >= 90)//small
                {
                    if (girl.anal() >= 70)
                    {
                        ss << " It slid right in her well trained ass with no problems."; anal += 1;
                        OrgasmSelectText(" Thankfully she is fast to orgasms or she wouldn't have got much out of this.\n",
                                         " She got nothing out of this as his dick was to small and its hard to get her off anyway.\n",
                                         " She slightly enjoyed herself.\n");
                    }
                    else if (girl.anal() >= 40)
                    {
                        ss << " It slide into her ass with little trouble as she is slight trained in the anal arts."; anal += 2;
                        OrgasmSelectText(" She was able to get off on his small cock a few times thanks to her fast orgasm ability.\n",
                                         " Didn't get much out of his small cock as she is so slow to orgasm.\n",
                                         " Enjoyed his small cock even if she didn't get off.\n");
                    }
                    else
                    {
                        ss << " Despite the fact that it was small it was still a tight fit in her inexperienced ass."; anal += 3;
                        OrgasmSelectText(" Her lack of skill at anal and the fact that she is fast to orgasm she had a great time even with the small cock.\n",
                                         " Her tight ass help her get off on the small cock even though it is hard for her to get off.\n",
                                         " Her tight ass help her get off on his small cock.\n");
                    }
                }
                else if (roll_c <= 10)//huge
                {
                    if (girl.anal() >= 70)
                    {
                        ss << " Her well trained ass was able to take the huge dick with little trouble."; anal += 3;
                        OrgasmSelectText(" She orgasmed over and over on his huge cock and when he finally finished she was left a gasping for air in a state of ecstasy.\n",
                                         " Despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n",
                                         " She orgasmed many times and loved every inch of his huge dick.\n");
                    }
                    else if (girl.anal() >= 40)
                    {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) && girl.intelligence() >= 60)
                        {
                            ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in easier."; anal += 3;
                            OrgasmSelectText(" With the help of her Booty Lube she was able to enjoy every inch of his huge dick and orgasmed many times. When he was done she was left shacking with pleasure.\n",
                                             " With the help of her Booty Lube and despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n",
                                             " With the help of her Booty Lube she was able to orgasm many times and loved every inch of his huge dick.\n");
                        }
                        else
                        {
                            ss << " Her slighted trained ass was able to take the huge dick with only a little pain at the start."; anal += 2;
                            OrgasmSelectText(" After a few minutes of letting her ass get used to his big cock she was finally able to enjoy it and orgasmed many times screaming in pleasure.\n",
                                             " After a few minutes of letting her ass get used to his big cock she was able to orgasm.\n",
                                             " After a few minutes of letting her ass get used to his big cock she was able to take the whole thing and orgasmed a few times.\n");
                        }
                    }
                    else
                    {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) && girl.intelligence() >= 60)
                        {
                            ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
                            OrgasmSelectText(" Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed many times.\n",
                                             " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed one time.\n",
                                             " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed a few times.\n");
                        }
                        else
                        {
                            ss << " She screamed in pain as he stuffed his huge dick in her tight ass.\n"; anal += 1;
                            OrgasmSelectText(" She was able to get some joy out of it in the end as she is fast to orgasm.\n",
                                             " It was nothing but a painful experience for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n",
                                             " It was nothing but a painful experience for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n");

                        }
                    }
                }
                else// normal
                {
                    if (girl.anal() >= 70)
                    {
                        ss << " It slide right in her well trained ass."; anal += 2;
                        OrgasmSelectText(" She was able to get off a few times as she is fast to orgasm.\n",
                                         " She was getting close to done when he pulled out and shot his wade all over her back. Its to bad she is slow to orgasm.\n",
                                         " She was able to get off by the end.\n");
                    }
                    else if (girl.anal() >= 40)
                    {
                        ss << " It was a good fit for her slightly trained ass."; anal += 3;
                        OrgasmSelectText(" His cock being a good fit for her ass she was able to orgasm many times and was screaming in pleasure before to long.\n",
                                         " His cock being a good fit for her ass he was able to bring her to orgasm if a bit slowly.\n",
                                         " His cock being a good fit for her ass she orgasmed a few times. When he was done she was left with a smile on her face.\n");
                    }
                    else
                    {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) && girl.intelligence() >= 60)
                        {
                            ss << " Upon seeing his dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
                            OrgasmSelectText(" Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n",
                                             " Thanks to her lube she was able to enjoy it much faster and was able to orgasm.\n",
                                             " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n");
                        }
                        else
                        {
                            ss << " Despite the fact that it was normal sized it was still a very tight fit in her inexperienced ass."; anal += 2;
                            OrgasmSelectText(" It was slightly painful at first but after a few minutes it wasn't a problem and she was able to orgasm.\n",
                                             " It was slightly painful at first but after a few minutes it wasn't a problem. But she wasn't able to orgasm in the end.\n",
                                             " It was slightly painful at first but after a few minutes it wasn't a problem she enjoyed it in the end.\n");
                        }
                    }
                }
            }
            else
            {
                ss << " and with a laugh and told him that her ass wasn't on the menu. He laughed so hard he increased her tip!\n"; m_Tips += 25;
            }
        }
        else if (m_Performance >= 135) //decent or good
        {
            ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything.\n";
        }
        else if (m_Performance >= 85) //bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n"; m_Earnings -= 10;
        }
        else  //very bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n"; m_Earnings -= 15;
        }
    }

    if ((girl.has_active_trait("Meek") || girl.has_active_trait("Shy")) && chance(5))
    {
        ss << "${name} was taking an order from a rather rude patron when he decide to grope her. She isn't the kind of girl to resist this and had a bad day at work because of this.\n";
        m_Enjoyment -= 5;
    }

    if (girl.libido() > 90 && chance(25) && !girl.has_active_trait("Lesbian") && (girl.has_active_trait(
            "Nymphomaniac") || girl.has_active_trait("Succubus")) && (girl.oralsex() > 80 ||
                                                                      girl.has_active_trait("Cum Addict")))
    {
        ss << "${name} thought she deserved a short break and disappeared under one of the tables when nobody was looking, in order to give one of the clients a blowjob. Kneeling under the table, she devoured his cock with ease and deepthroated him as he came to make sure she didn't make a mess. The client himself was wasted out of his mind and didn't catch as much as a glimpse of her, but he left the locale with a big tip on the table.\n";
        m_Tips += 50;
        imagetype = IMGTYPE_ORAL;
        oral += 2;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
    }

    if (girl.libido() > 90 && chance(25) && !girl.has_active_trait("Lesbian") && (girl.has_active_trait(
            "Nymphomaniac") || girl.has_active_trait("Succubus")) && (girl.oralsex() > 80 ||
                                                                      girl.has_active_trait("Cum Addict")))
    {
        ss << "During her shift, ${name} unnoticeably dove under the table belonging to a lonely-looking fellow, quickly unzipped his pants and started jacking him off enthusiastically. She skillfully wiped herself when he came all over her face. The whole event took no longer than two minutes, but was well worth the time spent on it, since the patron left with a heavy tip.\n";
        m_Tips += 50;
        imagetype = IMGTYPE_HAND;
        hand += 2;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
    }


    if (girl.dignity() <= -20 && chance(20) && girl.breast_size() > 5) //updated for the new breast traits
    {
        ss << "A drunk patron \"accidentally\" fell onto ${name} and buried his face between her breasts. To his joy and surprise, ${name} flirtatiously encouraged him to motorboat them for awhile, which he gladly did, before slipping some cash between the titties and staggering out on his way.\n"; m_Tips += 40;
    }

    if (girl.has_active_trait("Futanari") && girl.libido() > 80 && chance(5))
    {
        if (girl.has_active_trait("Open Minded") || girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Slut") ||
            girl.has_active_trait("Succubus") || (girl.confidence() > 35 && girl.dignity() < 35))
        {
            ss << "Noticing the bulge under her skirt one of the customers asked for a very special service: He wanted some \"cream\" in his drink. ${name} took her already hard cock out and sprinkled the drink with some of her jizz. The customer thanked her and slipped a good tip under her panties.\n";
            girl.upd_skill(SKILL_SERVICE, 2);
            girl.upd_temp_stat(STAT_LIBIDO, -30, true);;
            m_Tips += 30 + (int)(girl.service() * 0.2); // not sure if this will work fine
            imagetype = IMGTYPE_MAST;
        }
        else
        {
            ss << "Noticing the bulge under her skirt one of the customers asked ${name} to spill some of her \"cream\" in his drink, but she refused, blushing.\n";
            undignified();
        }
    }

    if ((girl.has_active_trait("Busty Boobs") && chance(5)) ||
        (girl.has_active_trait("Big Boobs") && chance(10)) ||
        (girl.has_active_trait("Giant Juggs") && chance(15)) ||
        (girl.has_active_trait("Massive Melons") && chance(20)) ||
        (girl.has_active_trait("Abnormally Large Boobs") && chance(25)) ||
        (girl.has_active_trait("Titanic Tits") && chance(30)))
    {
        if (chance(30) && (girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Bimbo")))
        {
            ss << "A patron was staring obviously at her large breasts, so she took off her top to show him her tits, which earned her a ";
            double t = 10.0;
            if (girl.has_active_trait("Big Boobs")) { t *= 1.5; }
            if (girl.has_active_trait("Giant Juggs")) { t *= 2; }
            if (girl.has_active_trait("Massive Melons")) { t *= 3; }
            if (girl.has_active_trait("Abnormally Large Boobs")) { t *= 4; }
            if (girl.has_active_trait("Titanic Tits")) { t *= 5; }
            if (girl.has_active_trait("No Nipples")) { t /= 3; }
            if (girl.has_active_trait("Missing Nipple")) { t /= 2; }
            if (girl.has_active_trait("Puffy Nipples")) { t += 1; }
            if (girl.has_active_trait("Perky Nipples")) { t += 1; }
            if (girl.has_active_trait("Pierced Nipples")) { t += 2; }
            if (girl.dignity() > 60)
            {
                girl.dignity(-1);
            }
            m_Tips += t;
            /* */if (t < 10)     ss << "small";
            else if (t < 20)     ss << "good";
            else if (t < 35)     ss << "great";
            else  ss << "gigantic";
            ss << " tip.\n";
        }
        else if (chance(20) && girl.has_active_trait("Lesbian") && (girl.has_active_trait("Slut") ||
                                                                    girl.has_active_trait("Succubus") || girl.libido() > 60))
        {
            ss << "A female patron was staring obviously at her large breasts, so she grabbed her hand, slipped it under her clothes and let her play with her boobs. ";
            if (girl.has_active_trait("Pierced Nipples"))
            {
                m_Tips += 3;
                girl.upd_temp_stat(STAT_LIBIDO, 1, true);
                ss << "Her nipple piercings were a pleasant surprise to her, and she \n";
            }
            else ss << "She";
            ss << "slipped a small tip between her tits afterwards.\n";
            girl.upd_temp_stat(STAT_LIBIDO, 2, true);
            m_Tips += 15;
            if (girl.dignity() > 30)
            {
                girl.dignity(-1);
            }
        }
        else if (!girl.has_active_trait("Lesbian") && (girl.has_active_trait("Slut") || girl.has_active_trait("Succubus")))
        {
            ss << "A patron was staring obviously at her large breasts, so she grabbed his hand, slipped it under her clothes and let him play with her boobs. ";
            if (girl.has_active_trait("Pierced Nipples"))
            {
                m_Tips += 3;
                girl.upd_temp_stat(STAT_LIBIDO, 1, true);
                ss << "Her nipple piercings were a pleasant surprise to him, and he \n";
            }
            else ss << "He";
            ss << "slipped a small tip between her tits afterwards.\n";
            girl.upd_temp_stat(STAT_LIBIDO, 2, true);
            m_Tips += 15;
            if (girl.dignity() > 30)
            {
                girl.dignity(-1);
            }
        }
    }

    if (girl.is_pregnant() && chance(15))
    {
        if (girl.lactation() > 50 &&
            (girl.has_active_trait("Open Minded") || girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Slut")
             || girl.has_active_trait("Succubus") ||

             (girl.confidence() > 35 && girl.dignity() < 50)))
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his coffee. ${name} took one of her breasts out and put some of her milk in the drink. The customer thanked her and slipped a good tip under her skirt.\n";
            girl.service(2);
            girl.lactation(-30);
            m_Tips += 30 + (int)(girl.service() *0.2);
        }
        else if (girl.lactation() < 50)
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but ${name} said that she didn't have enough.\n";
        }
        else
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but she refused, blushing.\n";
            undignified();
        }
    }

    if (girl.has_active_trait("Alcoholic") && chance(10) && girl.health() > 5)
    {
        ss << "${name} couldn't resist the offer of some patrons who invited her for a drink. And another one. And another one... When she came back to her senses she was lying on the floor half naked and covered in cum...\n";
        m_Tips -= 10;
        m_Wages -= 50;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.anal(uniform(1, 4));
        girl.bdsm(uniform(0, 3));
        girl.normalsex(uniform(1, 4));
        girl.group(uniform(2, 5));
        girl.oralsex(uniform(0, 3));
        girl.tittysex(uniform(0, 3));
        girl.handjob(uniform(0, 3));
        girl.footjob(uniform(0, 3));
        girl.happiness(-5);
        girl.health(-1);
        girl.upd_Enjoyment(ACTION_SEX, -2);
        girl.spirit(-2);
        imagetype = IMGTYPE_GROUP;
        girl.AddMessage(ss.str(), IMGTYPE_GROUP, EVENT_DANGER);
        if (girl.lose_trait("Virgin"))
        {
            ss << "\nShe is no longer a virgin.\n";
            girl.happiness(-10);
            girl.upd_Enjoyment(ACTION_SEX, -2);
            girl.health(-1);
            girl.spirit(-1);
        }
        sCustomer Cust = g_Game->GetCustomer(brothel);
        Cust.m_Amount = uniform(1, 10);
        if (!girl.calc_group_pregnancy(Cust, 1.0))
        {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
    }

    if (girl.has_active_trait("Fleet Of Foot") && chance(30))
    {
        ss << "${name} is fast on her feet, and makes great time navigating from table to table. She is able to serve almost twice as many customers in her shift.\n";
        m_Tips += 50;
    }

    if ((girl.has_active_trait("Long Legs") || girl.has_active_trait("Great Figure") ||
         girl.has_active_trait("Hourglass Figure")) && chance(30))
    {
        ss << "${name}'s body is incredible, and the customers fixate on her immediately. Her tips reflect their attention.";
        m_Tips += 20;
    }

    if (girl.has_active_trait("Dojikko") && chance(35))
    {
        ss << "${name}  is clumsy in the most adorable way, and when she trips and falls face-first into a patron's lap, spilling a tray all over the floor, he just laughs and asks if there is anything he can do to help.\n";
        if (girl.dignity() >= 50 || girl.libido() <= 50)
        {
            ss << "${name} gives him a nervous smile as she gets back up and dusts herself off. \"I'm so sorry, sir,\" she mutters, and he waves the whole thing away as if nothing happened. \"I'm happy to wait for another drink, for you, cutie,\" he says.\n";
        }
        else
        {
            ss << "\"There might be something I can do for you while I'm here,\" replies ${name}. She places her mouth over the bulge in his pants and starts sucking the pants until he unzips himself and pulls out his dick for her. \"I hope this makes up for me spilling that drink,\" she says before she locks her mouth around his dick.\n";
            if (girl.oralsex() >= 50)
            {
                ss << "\"It certainly does,\" responds the customer. \"You're very good at this, you clumsy little minx.\" ${name} murmurs appreciably around his cock but does not stop until she has milked out his cum and swallowed it. She stands back up, dusts herself off, and starts picking up the dropped tray and the glasses. \"Allow me to get you another drink, sir,\" she states as she saunters away.\n";
                m_Tips += 35;
                brothel.m_Happiness += 5;
            }
            else
            {
                ss << "The customer initially thinks that this is way better than having his drink, but then realizes that ${name} is a bit inexperienced with her efforts around his shaft. \"Looks like you're clumsy in just about everything you do,\" he comments, \"but somehow, that's still super adorable. Keep going, and I'll finish in a few minutes.\" She takes a little longer than she should to bring him to orgasm, and then stands, wipes the cum off her lips, and picks up the dropped tray. \"I'll get you another drink immediately,\" she announces as she walks back to the kitchen.\n";
                m_Tips += 15;
            }
            girl.oralsex(2);
            imagetype = IMGTYPE_ORAL;
            girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        }
    }

    if (girl.has_active_trait("Sexy Air") && chance(35))
    {
        ss << "Customers enjoy watching ${name} work. Her sexy body and her indefinably attractive way of carrying herself draw attention, whether she likes it or not. It is uncanny how many drinks the customers accidentally spill on their laps, and they would all like her help drying themselves off.\n";
        if (girl.dignity() <= 0 || girl.libido() >= 60)
        {
            ss << "\"What a terrible spill,\" she cries in mock indignation as she kneels down beside one of them. \"Maybe I can deal with all of this.. wetness..\" she continues, quickly working her hand down his pants, stroking vigorously and using the spilled drink as lubrication.\n";
            if (girl.handjob() >= 50)
            {
                ss << "The customer sighs with satisfaction and then erupts with an ecstatic cry as she finishes him with her skilled fingers. ${name} stands back up and smiles as she asks him if that solves the problem. He assures her that it did and thanks her by placing a handful of coins on the table.\n";
                m_Tips += 35;
                brothel.m_Happiness += 5;
            }
            else
            {
                ss << "The customer is thrilled until he realizes that ${name} is not very good at handjobs. Still, she is so sexy that he does not have to make a lot of effort to cum on her palm. \"Thank you,\" he smiles. \"I think it's all dry now.\"\n";
                brothel.m_Happiness += 5;
            }
            girl.handjob(2);
            imagetype = IMGTYPE_HAND;
            girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        }
    }

    if (girl.has_active_trait("Exhibitionist") && chance(50))
    {
        ss << "${name} is a rampant exhibitionist, and this job gives her a lot of opportunities to flash her goods at select customers. She has cut her uniform top to be even shorter than usual, and her nipples constantly appear when she flashes her underboobs. ${name} does a great job of increasing the atmosphere of sexual tension in your restaurant.";
        brothel.m_Happiness += 15;
    }

    if (chance(35) && (girl.breast_size() >= 5 || girl.has_active_trait("Sexy Air")))
    {
        if (girl.dignity() >= 50)
        {
            ss << "${name} draws a lot of attention when she walks and bends towards patrons, and everybody strains their necks for a look down her shirt at her heavy swinging breasts. They openly make lewd comments about the things they would do to her tits, and ${name} shies away and tries to cover them more fully with a menu. She swerves to avoid the many groping hands that \"accidentally\" find themselves brushing against her mammaries. The customers think this is just great fun and catcall after her whenever she tries to retreat.\n";
            brothel.m_Happiness += 15;
        }
        else if (girl.dignity() <= 0)
        {
            ss << "${name} has been blessed with great tits and the wisdom to know it. She leans deep over the tables to give customers a good view down her cleavage as she takes their orders. When a customer \"accidentally\" grabs her left tit instead of his glass, she pushes the tit deeper into his hands, stares into his eyes, and smiles. \"These aren�t on the menu,\" she purrs.\n";
            if (girl.libido() >= 60)
            {
                ss << "\"But they could be the daily special,\" she continues, grinding the breast against his hand. The customer grins and places a handful of coins on the table. \"That looks about right,\" ${name} says as she gets down on the floor and crawls under the table. He is laughing and high-fiving his buddies in no time as she wraps his dick around her tits and starts mashing them together for his pleasure.\n";
                if (girl.tittysex() >= 50)
                {
                    ss << "He enjoys it immensely, and adds a few more coins to the table as ${name} crawls back from under the table and sexily wipes the cum from her face.\n";
                    m_Tips += 35;
                }
                else
                {
                    ss << "The titfuck is not the best, but he�s hardly one to complain. \"I don�t know if I�ll order the special regularly,\" he says to her when she crawls back up and finishes wiping off the cum, \"but it was certainly a bonus for today!\"\n";
                }
                girl.tittysex(2);
                imagetype = IMGTYPE_TITTY;
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
            }
            m_Tips += 15;
        }
    }

    if (brothel.num_girls_on_job(JOB_SLEAZYBARMAID, false) >= 1 && chance(25))
    {
        if (m_Performance > 100)
        {
            ss << "\nWith help from the Barmaid, ${name} provided better service to the customers, increasing her tips.\n";
            m_Tips *= 1.2;
        }
    }
    else if (brothel.num_girls_on_job(JOB_SLEAZYBARMAID, false) == 0 && m_Performance <= 100)
    {
        ss << "\n${name} had a hard time attending all the customers without the help of a barmaid.\n";
        m_Tips *= 0.9;
    }

    //    Finish the shift            //

    shift_enjoyment();

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    m_Earnings += uniform(10, 10 + roll_max);

    girl.oralsex(oral);
    girl.handjob(hand);
    girl.anal(anal);
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, fame);

    if (!girl.has_active_trait("Straight"))    {
        girl.upd_temp_stat(STAT_LIBIDO, std::min(3, brothel.num_girls_on_job(JOB_BARSTRIPPER, false)));
    }

    //gained traits
    if (m_Performance > 150 && girl.constitution() > 65) { cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, ACTION_WORKCLUB, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet Of Foot.", is_night); }
    if (chance(25) && girl.dignity() < 0 && (anal > 0 || oral > 0 || hand > 0))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }

    return false;
}

class AdvertisingJob : public cSimpleJob {
public:
    AdvertisingJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

AdvertisingJob::AdvertisingJob() : cSimpleJob(JOB_ADVERTISING, "Advertising.xml", {ACTION_WORKADVERTISING}) {

}

bool AdvertisingJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    //    Job setup                //
    int fame = 0;
    int imagetype = IMGTYPE_SIGN;
    auto msgtype = EVENT_SUMMARY;

    //    Job Performance            //

    if(girl.is_unpaid())
        m_Performance = m_Performance * 90 / 100;    // unpaid slaves don't seem to want to advertise as much.
    if (girl.is_free())
        m_Performance = m_Performance * 110 / 100;    // paid free girls seem to attract more business

    // add some more randomness
#if 0 // work in progress

    ss << "She gave a tour of building making sure not to show the rooms that are messy.\n";
    if ()
    {
        ss << "as she was walking the people around she would make sure to slip in a few recommendations for girls the people would enjoy getting services from.";
    }
    else
    {
        ss << "She spent more time trying to flirt with the customers then actually getting anywhere with showing them around,\n";
        ss << "She still got the job done but it was nowhere as good as it could have been";
    }
    if ()
    {
        ss << "Most of the time she spent reading or doing anything else to spend her time to pass the time.\n";
    }
    else
    {
        ss << "She decided not to get in trouble so she just waited there silently for someone to come so she could do her job properly.\n";
    }

#endif

    //    Enjoyment and Tiredness        //

    // Complications
    int roll_a = d100();
    if (roll_a <= 10)
    {
        m_Enjoyment -= uniform(1, 3);
        ss << "She was harassed and made fun of while advertising.\n";
        if (girl.happiness() < 50)
        {
            m_Enjoyment -= 1;
            ss << "Other then that she mostly just spent her time trying to not breakdown and cry.\n";
            fame -= uniform(0, 1);
        }
        m_Performance = m_Performance * 80 / 100;
        fame -= uniform(0, 1);
    }
    else if (roll_a >= 90)
    {
        m_Enjoyment += uniform(1, 3);
        ss << "She made sure many people were interested in the buildings facilities.\n";
        m_Performance = m_Performance * 100 / 100;
        fame += uniform(0, 2);
    }
    else
    {
        m_Enjoyment += uniform(0, 1);
        ss << "She had an uneventful day advertising.\n";
    }

    /* `J` If she is not happy at her job she may ask you to change her job.
    *    Submitted by MuteDay as a subcode of bad complications but I liked it and made it as a separate section
    *    I will probably make it its own function when it works better.
    */
    if (girl.m_Enjoyment[ACTION_WORKADVERTISING] < -10)                         // if she does not like the job
    {
        int enjoyamount = girl.m_Enjoyment[ACTION_WORKADVERTISING];
        int saysomething = uniform(0, girl.confidence()) - enjoyamount;    // the more she does not like the job the more likely she is to say something about it
        saysomething -= girl.pcfear() / (girl.is_free() ? 2 : 1);    // reduce by fear (half if free)

        if (saysomething > 50)
        {
            girl.AddMessage("${name} comes up to you and asks you to change her job, She does not like advertizing.\n",
                            IMGTYPE_PROFILE, EVENT_WARNING);
        }
        else if (saysomething > 25)
        {
            ss << "She looks at you like she has something to say but then turns around and walks away.\n";
        }
    }

    //    Money                    //

    ss << "She managed to stretch the effectiveness of your advertising budget by about " << int(m_Performance) << "%.";
    // if you pay slave girls out of pocket  or if she is a free girl  pay them
    if (!girl.is_unpaid())
    {
        m_Wages += 70;
        g_Game->gold().advertising_costs(70);
        ss << " You paid her 70 gold for her advertising efforts.";
    }
    else
    {
        ss << " You do not pay your slave for her advertising efforts.";
    }

    //    Finish the shift            //

    girl.AddMessage(ss.str(), imagetype, msgtype);

    // now to boost the brothel's advertising level accordingly
    brothel.m_AdvertisingLevel += (m_Performance / 100);

    HandleGains(girl, fame);

    if (girl.strip() > 50)
        cGirls::PossiblyGainNewTrait(girl, "Exhibitionist", 50, ACTION_WORKADVERTISING, "${name} has become quite the Exhibitionist, she seems to prefer Advertising topless whenever she can.", is_night);

    return false;
}


class CustServiceJob : public cSimpleJob {
public:
    CustServiceJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

CustServiceJob::CustServiceJob() : cSimpleJob(JOB_CUSTOMERSERVICE, "CustService.xml", {ACTION_WORKCUSTSERV}) {

}

bool CustServiceJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    Action_Types actiontype = ACTION_WORKCUSTSERV;
    // Note: Customer service needs to be done last, after all the whores have worked.

    int numCusts = 0; // The number of customers she can handle
    int serviced = 0;

    // Complications
    int roll = d100();
    if (roll <= 5)
    {
        ss << "Some of the patrons abused her during the shift.";
        m_Enjoyment -= 1;
    } else if (roll <= 15)
    {
        ss << "A customer mistook her for a whore and was abusive when she wouldn't provide THAT service.";
        m_Enjoyment -= 1;
    }
    else if (roll >= 75) {
        ss << "She had a pleasant time working.";
        m_Enjoyment += 3;
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
        m_Enjoyment -= 5;
    }

    // Now let's take care of our neglected customers.
    for (int i=0; i<numCusts; i++)
    {
        if (g_Game->GetNumCustomers() > 0)
        {
            sCustomer Cust = g_Game->GetCustomer(brothel);
            // Let's find out how much happiness they started with.
            // They're not going to start out very happy. They're seeing customer service, after all.
            Cust.set_stat(STAT_HAPPINESS, 22 + uniform(0, 9) + uniform(0, 9)); // average 31 range 22 to 40
            // Now apply her happiness bonus.
            Cust.happiness(bonus);
            // update how happy the customers are on average
            brothel.m_Happiness += Cust.happiness();
            // And decrement the number of customers to be taken care of
            g_Game->customers().AdjustNumCustomers(-1);
            serviced++;
        }
        else
        {
            //If there aren't enough customers to take care of, time to quit.
            girl.AddMessage(girl.FullName() + " ran out of customers to take care of.", IMGTYPE_PROFILE,
                            is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
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
    // Now pay the girl.
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Raise skills
    HandleGains(girl, 1);
    // additional XP bonus for many customers
    girl.exp(serviced / 5);

    m_Wages = 50;
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
    mgr.register_job(std::make_unique<cBrothelStripper>());
    mgr.register_job(std::make_unique<ClubBarmaid>());
    mgr.register_job(std::make_unique<ClubStripper>());
    mgr.register_job(std::make_unique<ClubWaitress>());
    mgr.register_job(std::make_unique<AdvertisingJob>());
    mgr.register_job(std::make_unique<CustServiceJob>());
}
