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

#pragma endregion

// `J` Job Brothel - Sleazy Bar
bool WorkSleazyWaitress(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKCLUB;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
    if (girl.disobey_check(actiontype, JOB_SLEAZYWAITRESS))
    {
        //SIN - More informative mssg to show *what* she refuses
        ss << "${name} refused to be a waitress for the creeps in your strip club " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} is instructed to work at your sleazy restaurant as a waitress. She is informed that the customers here want good service, but they chose this place because of the promise of attractive women in skimpy clothing. If she wants to be successful, she will need to impress them with her body as well as her service.\n \n";
    ss << "She worked as a waitress in the strip club.\n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int wages = 25;
    double tips = 0;
    int enjoy = 0, anal = 0, oral = 0, hand = 0, fame = 0;
    int imagetype = IMGTYPE_ECCHI;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SLEAZYWAITRESS, false);

    const sGirl* barmaidonduty = random_girl_on_job(*girl.m_Building, JOB_SLEAZYBARMAID, Day0Night1);
    string barmaidname = barmaidonduty ? "Barmaid " + barmaidonduty->FullName() + "" : "the Barmaid";

    // `CRAZY`
    /*default*/ string dick_type_text = "normal sized";
    /* */if (roll_c <= 10)    { dick_type_text = "huge"; }
    else if (roll_c >= 90)    { dick_type_text = "small"; }

    //a little pre-game randomness
    if (girl.has_active_trait("Cum Addict") && rng.percent(30))
    {
        ss << "${name} is addicted to cum, and she cannot serve her shift without taking advantage of a room full of cocks. Since most of your patrons are already sexually primed with all this nubile flesh walking around in skimpy uniforms, she does not need to be very persuasive to convince various men to satisfy her addiction. You see her feet sticking out from under the tables from time to time as a satisfied customer smiles at the ceiling. Her service with the other tables suffers, but her tips are still quite high.\n";
        jobperformance -= 10;
        tips += 40;
        imagetype = IMGTYPE_ORAL;
    }
    else if ((girl.has_active_trait("Shy") || girl.has_active_trait("Nervous")) && rng.percent(20))
    {
        ss << "${name} has serious difficulty being around all these new people, and the fact that they are all so forward about her body does nothing to help. She spends a lot of time hiding in the kitchen, petrified of going back out and talking to all those people.";
        jobperformance -= 20;
    }


    if (jobperformance >= 245)
    {
        ss << " She must be the perfect waitress customers go on and on about her and always come to see her when she works.\n \n";
        wages += 155;

        if (roll_b >= 50)
        {
            ss << "${name} is amazing as a waitress, and dances about the room from table to table keeping all the customers satisfied. She recalls their orders by perfect memory, watches everyone to make sure they have everything they want, and appears as if by magic the moment anyone places a menu back on the table or returns their fork to the plate. Her sexy body keeps everyone thinking about her long after she walks away.\n";
            brothel->m_Fame += 10;
            tips += 10;
        }
        else
        {
            ss << "${name} does a great job, and her memorization of the menu and the specials, paired with her timely service and polite demeanor, pays dividends as she moves from table to table collecting orders. The patrons are pleased, and while ${name} knows better than to interrupt anyone while they are eating, the customers have no complaints when she is forced to do so. She walks with just the right sexy strut that she is a welcome sight whenever she appears.\n";
            brothel->m_Fame += 5;
            tips += 15;
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        wages += 95;

        if (roll_b >= 50)
        {
            ss << "${name} is a very good waitress, and collects the orders promptly and politely. She has almost everything on the menu memorized and can speak knowledgeably about the beer list and customer favorites. She occasionally spends too much time at each table or appears while customers are still considering their orders, but this does not seriously detract from her service, and the patrons who enjoy her extra attention flirt with her constantly.\n";
            tips += 5;
        }
        else
        {
            ss << "${name} impresses the patrons with her efficiency, politeness, and knowledge. She deftly deters the patrons away from dishes that are not perfected, and also from dishes that are too inexpensive. She talks a number of customers into ordering a more expensive special, and they are all happy with the results, especially as she hovers long and low over them, brushing them repeatedly with her breasts as she sets it on the table.\n";
            tips += 10;
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        wages += 55;

        if (roll_b >= 50)
        {
            ss << "${name} provides satisfactory service. She tends to show up too early, and occasionally the customers feel rushed, but ${name} is nevertheless polite and courteous and everyone is generally pleased with her work. She needs to learn more about the menu items and the specials, and her flirtations with the customers feel forced and unnatural, but all in all, her work is good.\n";
        }
        else
        {
            ss << "${name} is fairly good as a waitress. She is attentive and prompt, and while she has to write all the orders down in order not to forget, the customers are not too put out by her efforts. She does not understand the cooks' capabilities well enough to steer customers away from poorer dishes, but she is good enough at flirting with them and occasionally grinding against them that they are distracted from the meal.\n";
        }
        tips += 5;
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;

        if (roll_b >= 50)
        {
            ss << "${name} manages to get all the food ordered and delivered, but that is the extent of her skills. Her unease at this job translates into customers that feel rushed to order sooner than they would like, and her inability to speak with knowledge about the menu items and specials leads them to make hasty and usually inexpensive selections. When she tries to bend low and give them a good view of her tits as they order, it ends up looking awkward and forced. The patrons are satisfied by her work, but only just.\n";
        }
        else
        {
            ss << "${name} is still learning how to be a decent waitress. She is able to get the right food to the right patron on time, but it is clear that she is still training in her job. Her attempts at grinding her ass against them are badly timed and usually come across as clumsiness rather than flirtation, and the customers show limited patience while she asks them to repeat their orders as she writes everything down.\n";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;

        if (roll_b >= 50)
        {
            ss << "${name} is not very good at working as a waitress. She forgets orders and has to return to the tables for clarification regularly. Some patrons get the wrong meal, and a few choose to leave without paying rather than wait for her to place the corrected order. She stumbles over the menu options and her flirtatious comments sound like a terrible actress rehearsing even worse lines. The customers are not very pleased with her work, and her tips reflect this.\n";
        }
        else
        {
            ss << "${name} barely makes it through her shift without running every customer out of the restaurant. Her skills need a lot of work. She asks patrons for their orders before they even pick up the menu, and rather than giving them time to look it over and select, she stands there staring until they place the order. She tries to make up for this by \"gently\" brushing her tits against their heads when she adjusts something on the table, but it ends up being a weird boob mash in their ears. Most patrons find this very unnerving, but it would probably be forgiven if she brought them the right orders. She does not, usually. Some patrons walk out, promising that they will tell their friends about the poor service. Almost nobody tips her.\n";
        }
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;

        if (roll_b >= 50)
        {
            ss << "${name} is really bad at serving as a waitress. Awful, really. She hassles customers for food orders, interrupts them constantly while they eat, and constantly screws up their order. She makes practically no effort to flirt or entice the customers with her body. The kitchen is constantly compelled to send complimentary food out to cover for her poor service, but most customers are not so easily placated and deride ${name} to her face. They leave, promising that they will never come back. Nobody tips her.\n";
            brothel->m_Happiness -= 10;
        }
        else
        {
            ss << "${name} understands nothing about being a waitress. The menu confuses her, the specials are completely outside her understanding, and she has no idea how to use her body to her advantage. Those customers that stick around after her terrible initial service are not very surprised to find that a completely different meal than the one they ordered finds its way to their table. She hovers over them while they eat, clearly waiting for them to finish and pay so she can bus the table. Nobody tips her, and most tell her that they will not come back again as long as she works here.\n";
            brothel->m_Fame -= 10;
        }
    }


    //base tips, aprox 10-20% of base wages
    tips += (((10.0 + jobperformance / 22.0) * (double)wages) / 100.0);

    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 25;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(15))
    {
        if (rng.percent(50))
        {
            ss << "Her clumsy nature cause her to spill food on a customer resulting in them storming off without paying.\n";
        }
        else
        {
            ss << "${name} is clumsy, which is not a great attribute for someone that carries food for others all day. All the customers are jarred to hear a pile of plates come crashing down to the floor, as ${name} sheepishly stares at the heap of broken dishes and scattered food. A customer is forced to wait while she apologizes and places a new order.";
        }
        wages -= 25;
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

    if (girl.has_active_trait("Great Arse") && rng.percent(15))
    {
        if (jobperformance >= 185) //great
        {
            ss << "A patron reached out to grab her ass. But she skillfully avoided it";
            if (girl.libido() > 70 && !girl.has_active_trait("Lesbian"))
            {
                ss << " and said that's only on the menu if your willing to pay up. He jumped at the chance to get to try her ass out and bent her over the table and whiping out his " << dick_type_text << " dick.";
                wages += girl.askprice() + 50;
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
                        ss << " It slide right in her well trained ass with no problems."; anal += 1;
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
                ss << " and with a laugh and told him that her ass wasn't on the menu. He laughed so hard he increased her tip!\n"; tips += 25;
            }
        }
        else if (jobperformance >= 135) //decent or good
        {
            ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything.\n";
        }
        else if (jobperformance >= 85) //bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n"; wages -= 10;
        }
        else  //very bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n"; wages -= 15;
        }
    }

    if (girl.has_active_trait("Great Figure") && rng.percent(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name}'s amazing figure wasn't enough to keep the patrons happy when her service was so bad.\n"; tips -= 10;
        }
        else
        {
            ss << "Not only does she have an amazing figure but she is also an amazing waitress the patrons really love her and her tips prove it.\n"; tips += 20;
        }
    }

    if ((girl.has_active_trait("Meek") || girl.has_active_trait("Shy")) && rng.percent(5))
    {
        ss << "${name} was taking an order from a rather rude patron when he decide to grope her. She isn't the kind of girl to resist this and had a bad day at work because of this.\n";
        enjoy -= 5;
    }

    if (girl.libido() > 90 && rng.percent(25) && !girl.has_active_trait("Lesbian") && (girl.has_active_trait(
            "Nymphomaniac") || girl.has_active_trait("Succubus")) && (girl.oralsex() > 80 ||
                                                                      girl.has_active_trait("Cum Addict")))
    {
        ss << "${name} thought she deserved a short break and disappeared under one of the tables when nobody was looking, in order to give one of the clients a blowjob. Kneeling under the table, she devoured his cock with ease and deepthroated him as he came to make sure she didn't make a mess. The client himself was wasted out of his mind and didn't catch as much as a glimpse of her, but he left the locale with a big tip on the table.\n";
        tips += 50;
        imagetype = IMGTYPE_ORAL;
        oral += 2;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
    }

    if (girl.libido() > 90 && rng.percent(25) && !girl.has_active_trait("Lesbian") && (girl.has_active_trait(
            "Nymphomaniac") || girl.has_active_trait("Succubus")) && (girl.oralsex() > 80 ||
                                                                      girl.has_active_trait("Cum Addict")))
    {
        ss << "During her shift, ${name} unnoticeably dove under the table belonging to a lonely-looking fellow, quickly unzipped his pants and started jacking him off enthusiastically. She skillfully wiped herself when he came all over her face. The whole event took no longer than two minutes, but was well worth the time spent on it, since the patron left with a heavy tip.\n";
        tips += 50;
        imagetype = IMGTYPE_HAND;
        hand += 2;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
    }

    if (rng.percent(20) && (girl.has_active_trait("Exhibitionist") || girl.dignity() <= -20))
    {
        if (roll_b <= 50)
        {
            ss << "During her shift, ${name} deliberately dropped the pen she uses to write down orders in front of one of the customers. Exploiting her skimpy outfit, she made sure to bend over to pick it up in a way that allowed her to directly flash her butt on the sitting client's eye level. This earned her an extra tip.\n"; tips += 20;
        }
        else
        {
            ss << "An inebriated patron said half-jokingly to ${name} that he'll leave a heavy tip if she takes his order while sitting on his lap. Much to his surprise, ${name} was almost too eager to comply, sitting directly on his crotch instead, making sure to grind her butt into it. The customer lived up to his word and ${name} left the table with some extra cash.\n"; tips += 30;
        }
    }

    if (girl.dignity() <= -20 && rng.percent(20) && girl.breast_size() > 5) //updated for the new breast traits
    {
        ss << "A drunk patron \"accidentally\" fell onto ${name} and buried his face between her breasts. To his joy and surprise, ${name} flirtatiously encouraged him to motorboat them for awhile, which he gladly did, before slipping some cash between the titties and staggering out on his way.\n"; tips += 40;
    }

    if (girl.has_active_trait("Futanari") && girl.libido() > 80 && rng.percent(5))
    {
        if (girl.has_active_trait("Open Minded") || girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Slut") ||
            girl.has_active_trait("Succubus") || (girl.confidence() > 35 && girl.dignity() < 35))
        {
            ss << "Noticing the bulge under her skirt one of the customers asked for a very special service: He wanted some \"cream\" in his drink. ${name} took her already hard cock out and sprinkled the drink with some of her jizz. The customer thanked her and slipped a good tip under her panties.\n";
            girl.upd_skill(SKILL_SERVICE, 2);
            girl.upd_temp_stat(STAT_LIBIDO, -30, true);;
            tips += 30 + (int)(girl.service() * 0.2); // not sure if this will work fine
            imagetype = IMGTYPE_MAST;
        }
        else
        {
            ss << "Noticing the bulge under her skirt one of the customers asked ${name} to spill some of her \"cream\" in his drink, but she refused, blushing.\n";
            switch (rng % 10)
            {
            case 1:        girl.sanity(-(rng % 5));        if (rng % 2) break;
            case 2:        girl.confidence(-(rng % 5));    if (rng % 2) break;
            case 3:        girl.dignity(-(rng % 5));        if (rng % 2) break;
            default:    enjoy -= rng % 5;    break;
            }
        }
    }

    if ((girl.has_active_trait("Busty Boobs") && rng.percent(5)) ||
        (girl.has_active_trait("Big Boobs") && rng.percent(10)) ||
        (girl.has_active_trait("Giant Juggs") && rng.percent(15)) ||
        (girl.has_active_trait("Massive Melons") && rng.percent(20)) ||
        (girl.has_active_trait("Abnormally Large Boobs") && rng.percent(25)) ||
        (girl.has_active_trait("Titanic Tits") && rng.percent(30)))
    {
        if (rng.percent(30) && (girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Bimbo")))
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
            tips += t;
            /* */if (t < 10)     ss << "small";
            else if (t < 20)     ss << "good";
            else if (t < 35)     ss << "great";
            else  ss << "gigantic";
            ss << " tip.\n";
        }
        else if (rng.percent(20) && girl.has_active_trait("Lesbian") && (girl.has_active_trait("Slut") ||
                                                                         girl.has_active_trait("Succubus") || girl.libido() > 60))
        {
            ss << "A female patron was staring obviously at her large breasts, so she grabbed her hand, slipped it under her clothes and let her play with her boobs. ";
            if (girl.has_active_trait("Pierced Nipples"))
            {
                tips += 3;
                girl.upd_temp_stat(STAT_LIBIDO, 1, true);
                ss << "Her nipple piercings were a pleasant surprise to her, and she \n";
            }
            else ss << "She";
            ss << "slipped a small tip between her tits afterwards.\n";
            girl.upd_temp_stat(STAT_LIBIDO, 2, true);
            tips += 15;
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
                tips += 3;
                girl.upd_temp_stat(STAT_LIBIDO, 1, true);
                ss << "Her nipple piercings were a pleasant surprise to him, and he \n";
            }
            else ss << "He";
            ss << "slipped a small tip between her tits afterwards.\n";
            girl.upd_temp_stat(STAT_LIBIDO, 2, true);
            tips += 15;
            if (girl.dignity() > 30)
            {
                girl.dignity(-1);
            }
        }
    }

    if (girl.is_pregnant() && rng.percent(15))
    {
        if (girl.lactation() > 50 &&
            (girl.has_active_trait("Open Minded") || girl.has_active_trait("Exhibitionist") || girl.has_active_trait("Slut")
             || girl.has_active_trait("Succubus") ||

             (girl.confidence() > 35 && girl.dignity() < 50)))
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his coffee. ${name} took one of her breasts out and put some of her milk in the drink. The customer thanked her and slipped a good tip under her skirt.\n";
            girl.service(2);
            girl.lactation(-30);
            tips += 30 + (int)(girl.service() *0.2);
        }
        else if (girl.lactation() < 50)
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but ${name} said that she didn't have enough.\n";
        }
        else
        {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but she refused, blushing.\n";
            switch (rng % 10)
            {
            case 1:        girl.sanity(-(rng % 5));        if (rng % 2) break;
            case 2:        girl.confidence(-(rng % 5));    if (rng % 2) break;
            case 3:        girl.dignity(-(rng % 5));        if (rng % 2) break;
            default:    enjoy -= rng % 5;    break;
            }
        }
    }

    if (girl.has_active_trait("Alcoholic") && rng.percent(10) && girl.health() > 5)
    {
        ss << "${name} couldn't resist the offer of some patrons who invited her for a drink. And another one. And another one... When she came back to her senses she was lying on the floor half naked and covered in cum...\n";
        tips -= 10;
        wages -= 50;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.anal(max(0, rng % 4 + 1));
        girl.bdsm(max(0, rng % 4 - 1));
        girl.normalsex(max(0, rng % 4 + 1));
        girl.group(max(2, rng % 4 + 2));
        girl.oralsex(max(0, rng % 4 + 0));
        girl.tittysex(max(0, rng % 4 - 1));
        girl.handjob(max(0, rng % 4 + 0));
        girl.footjob(max(0, rng % 4 - 1));
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
        sCustomer Cust = g_Game->GetCustomer(*brothel);
        Cust.m_Amount = min(1, rng % 11);
        if (!girl.calc_group_pregnancy(Cust, false, 1.0))
        {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
    }

    if (girl.has_active_trait("Fleet Of Foot") && rng.percent(30))
    {
        ss << "${name} is fast on her feet, and makes great time navigating from table to table. She is able to serve almost twice as many customers in her shift.\n";
        tips += 50;
    }

    if ((girl.has_active_trait("Long Legs") || girl.has_active_trait("Great Figure") ||
            girl.has_active_trait("Hourglass Figure")) && rng.percent(30))
    {
        ss << "${name}'s body is incredible, and the customers fixate on her immediately. Her tips reflect their attention.";
        tips += 20;
    }

    if (girl.has_active_trait("Dojikko") && rng.percent(35))
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
                tips += 35;
                brothel->m_Happiness += 5;
            }
            else
            {
                ss << "The customer initially thinks that this is way better than having his drink, but then realizes that ${name} is a bit inexperienced with her efforts around his shaft. \"Looks like you're clumsy in just about everything you do,\" he comments, \"but somehow, that's still super adorable. Keep going, and I'll finish in a few minutes.\" She takes a little longer than she should to bring him to orgasm, and then stands, wipes the cum off her lips, and picks up the dropped tray. \"I'll get you another drink immediately,\" she announces as she walks back to the kitchen.\n";
                tips += 15;
            }
            girl.oralsex(2);
            imagetype = IMGTYPE_ORAL;
            girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        }
    }

    if (girl.has_active_trait("Sexy Air") && rng.percent(35))
    {
        ss << "Customers enjoy watching ${name} work. Her sexy body and her indefinably attractive way of carrying herself draw attention, whether she likes it or not. It is uncanny how many drinks the customers accidentally spill on their laps, and they would all like her help drying themselves off.\n";
        if (girl.dignity() <= 0 || girl.libido() >= 60)
        {
            ss << "\"What a terrible spill,\" she cries in mock indignation as she kneels down beside one of them. \"Maybe I can deal with all of this.. wetness..\" she continues, quickly working her hand down his pants, stroking vigorously and using the spilled drink as lubrication.\n";
            if (girl.handjob() >= 50)
            {
                ss << "The customer sighs with satisfaction and then erupts with an ecstatic cry as she finishes him with her skilled fingers. ${name} stands back up and smiles as she asks him if that solves the problem. He assures her that it did and thanks her by placing a handful of coins on the table.\n";
                tips += 35;
                brothel->m_Happiness += 5;
            }
            else
            {
                ss << "The customer is thrilled until he realizes that ${name} is not very good at handjobs. Still, she is so sexy that he does not have to make a lot of effort to cum on her palm. \"Thank you,\" he smiles. \"I think it's all dry now.\"\n";
                brothel->m_Happiness += 5;
            }
            girl.handjob(2);
            imagetype = IMGTYPE_HAND;
            girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        }
    }

    if (girl.has_active_trait("Exhibitionist") && rng.percent(50))
    {
        ss << "${name} is a rampant exhibitionist, and this job gives her a lot of opportunities to flash her goods at select customers. She has cut her uniform top to be even shorter than usual, and her nipples constantly appear when she flashes her underboobs. ${name} does a great job of increasing the atmosphere of sexual tension in your restaurant.";
        brothel->m_Happiness += 15;
    }

    if (rng.percent(35) && (girl.breast_size() >= 5 || girl.has_active_trait("Sexy Air")))
    {
        if (girl.dignity() >= 50)
        {
            ss << "${name} draws a lot of attention when she walks and bends towards patrons, and everybody strains their necks for a look down her shirt at her heavy swinging breasts. They openly make lewd comments about the things they would do to her tits, and ${name} shies away and tries to cover them more fully with a menu. She swerves to avoid the many groping hands that \"accidentally\" find themselves brushing against her mammaries. The customers think this is just great fun and catcall after her whenever she tries to retreat.\n";
            brothel->m_Happiness += 15;
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
                    tips += 35;
                }
                else
                {
                    ss << "The titfuck is not the best, but he�s hardly one to complain. \"I don�t know if I�ll order the special regularly,\" he says to her when she crawls back up and finishes wiping off the cum, \"but it was certainly a bonus for today!\"\n";
                }
                girl.tittysex(2);
                imagetype = IMGTYPE_TITTY;
                girl.upd_temp_stat(STAT_LIBIDO, -20, true);
            }
            tips += 15;
        }
    }

    if (brothel->num_girls_on_job(JOB_SLEAZYBARMAID, false) >= 1 && rng.percent(25))
    {
        if (jobperformance > 100)
        {
            ss << "\nWith the help from " << barmaidname << ", ${name} provided better service to the customers, increasing her tips.\n";
            tips *= 1.2;
        }
    }
    else if (brothel->num_girls_on_job(JOB_SLEAZYBARMAID, false) == 0 && jobperformance <= 100)
    {
        ss << "\n${name} had a hard time attending all the customers without the help of a barmaid.\n";
        tips *= 0.9;
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
        ss << "\nShe had a pleasant time working."; enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully."; enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += 10 + rng%roll_max;

    girl.m_Tips = max(0, (int)tips);
    girl.m_Pay = max(0, wages);

#pragma endregion
#pragma region    //    Finish the shift            //


    girl.oralsex(oral);
    girl.handjob(hand);
    girl.anal(anal);
    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, Day0Night1);

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


    //gained traits
    cGirls::PossiblyGainNewTrait(girl, "Charming", 70, actiontype, "${name} has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", Day0Night1);
    if (jobperformance > 150 && girl.constitution() > 65) { cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, actiontype, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet Of Foot.", Day0Night1); }
    if (rng.percent(25) && girl.dignity() < 0 && (anal > 0 || oral > 0 || hand > 0))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", Day0Night1, EVENT_WARNING);
    }

    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her breaking hundreds of dishes, and just as many reprimands, but ${name} has finally stopped being so Clumsy.", Day0Night1);


#pragma endregion
    return false;
}


double JP_SleazyWaitress(const sGirl& girl, bool estimate)// not used
{
    //SIN - standardizing job performance calc per J's instructs
    double jobperformance =
        //main stats - first 100 - service skills and beauty (due to sleazy bar)
        (girl.service() + girl.beauty() / 2) +
        //secondary stats - second 100 - charisma to charm customers, agility and performance to entertain them
        ((girl.charisma() + girl.performance() + girl.agility()) / 3) +
        //add level
        girl.level();

    // next up tiredness penalty
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.sleazywaitress");

    return jobperformance;
}
