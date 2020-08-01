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
#include "cRng.h"
#include "cInventory.h"
#include <sstream>
#include "Game.hpp"
#include "character/predicates.h"

#pragma endregion

// `J` Job Brothel - Sleazy Bar
bool WorkBarStripper(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKSTRIP;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
    if (girl.disobey_check(actiontype, JOB_BARSTRIPPER))
    {
        //SIN - More informative mssg to show *what* she refuses
        ss << "${name} refused to strip off in front of the creeps in your club " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a stripper in the club.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int wages = 30, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_STRIP;
    auto msgtype = Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_BARSTRIPPER, false);
    int lapdance = (girl.intelligence() / 2 +
        girl.performance() / 2 +
        girl.strip()) / 2;
    //int drinks;


    //what is she wearing?
    if (girl.has_item("Rainbow Underwear"))
    {
        ss << "${name} stripped down to reveal her Rainbow Underwear to the approval of the patrons watching her.\n \n";
        brothel->m_Happiness += 5; jobperformance += 5; tips += 10;
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
        tips += 5;
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
            ss << "and it helped her tips.\n \n"; tips += 20;
        }
    }
    else if (girl.has_item("Comfortable Underwear"))
    {
        ss << "${name}'s Comfortable Underwear help her move better while on stage.\n \n";
        jobperformance += 5;
    }
    else if (girl.has_item("Plain Underwear"))
    {
        ss << "${name} stripped down to reveal her Plain Underwear which didn't help her performance as the patrons found them boring.\n \n";
        jobperformance -= 5;
    }
    else if (girl.has_item("Sexy Underwear"))
    {
        ss << "${name} stripped down to reveal her Sexy Underwear which brought many people to the stage to watch her.\n \n";
        jobperformance += 5; tips += 15;
    }


    if (jobperformance >= 245)
    {
        ss << " She must be the perfect stripper, customers go on and on about her and always come to see her when she works.\n \n";
        wages += 155;
        if (roll_b <= 20)
        {
            ss << "${name} use of the pole amazes the patrons. They truly have no idea how she does the things she does.\n";
            brothel->m_Happiness += 10;
            brothel->m_Fame += 5;
        }
        else if (roll_b <= 40)
        {
            ss << "${name} throws her bra into the crowd causing a fight to erupt over who got to take it home. They just love her.\n";
            brothel->m_Happiness += 10;
        }
        else if (roll_b <= 60)
        {
            ss << "People push and shove to get to the front of the stage for ${name}'s dance set.\n";
            brothel->m_Fame += 10;
        }
        else if (roll_b <= 80)
        {
            ss << "${name}'s smile is enough to bring in the tips. You sometimes wonder if she even needs to take her clothes off.\n";
            brothel->m_Happiness += 10;
            tips += 15;
        }
        else
        {
            ss << "Somehow, ${name} managed to be so sexy that you thought you might need to close just to clean up the mess the crowd made.\n";
            brothel->m_Happiness += 10;
            brothel->m_Filthiness += 5;
            wages += 10;
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        wages += 95;
        if (roll_b <= 20)
        {
            ss << "${name} use of the pole makes every head in the place turn to watch.\n";
            brothel->m_Fame += 5;
        }
        else if (roll_b <= 40)
        {
            ss << "${name} throws her bra into the crowd causing an uproar of hooting and screaming.  She knows how to drive them crazy.\n";
            brothel->m_Happiness += 10;
        }
        else if (roll_b <= 60)
        {
            ss << "${name} draws just as many females as males to the club when its her shift.  It seems everyone has noticed her skill.\n";
            brothel->m_Fame += 10;
        }
        else if (roll_b <= 80)
        {
            ss << "All male members of ${name}'s audience had trouble standing after she finished her set.\n";
            brothel->m_Happiness += 10;
        }
        else
        {
            ss << "From start to finish, every move ${name} makes practically sweats sexuality.\n";
            brothel->m_Happiness += 10;
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        wages += 55;
        if (roll_b <= 20)
        {
            ss << "${name} can spin on the pole in a way that excites the crowd.\n";
            brothel->m_Happiness += 5;
        }
        else if (roll_b <= 40)
        {
            ss << "${name}'s sly smile and slow striptease drives the crowd crazy.\n";
            brothel->m_Happiness += 5;
        }
        else if (roll_b <= 60)
        {
            ss << "Seeing the large crowd waiting outside, ${name} smirked and slowly walked on stage knowing she was going get good tips today.\n";
            tips += 10;
        }
        else if (roll_b <= 80)
        {
            ss << "Since you let ${name} choose the music she dances to, her erotic dances have improved markedly.\n";
        }
        else
        {
            ss << "You watched with amusement as members of her audience made a disproportionate number of trips to the bathroom after her performance.\n";
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;
        if (roll_b <= 20)
        {
            ss << "While she won't win any contests, ${name} isn't a terrible striper.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "${name} is not the best erotic dancer you've ever seen, but the drunks enjoy the eyecandy.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "${name}'s striptease draws a decent crowd each night.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "${name} is able to use the pole without making any mistakes even if she isn't that great on it.\n";
        }
        else
        {
            ss << "Somehow, ${name} managed to get a decent amount of tips as she slowly took her clothes of during her set on stage.\n";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
        if (roll_b <= 20)
        {
            ss << "${name} spun around on the pole too much and got dizzy, causing her to fall off-stage buck-naked into the crowd. "
                << "A lot of hands grabbed onto her and helped her back up... at least, that's what they said they were doing.\n";
            brothel->m_Fame -= 5;
            brothel->m_Happiness += 5;  //cop a feel
        }
        else if (roll_b <= 40)
        {
            ss << "${name}'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
            brothel->m_Fame -= 5;
            brothel->m_Happiness += 5;  //would be funny to see
        }
        else if (roll_b <= 60)
        {
            ss << "For all the reaction she's getting, you could probably replace ${name} with a simple mannequin.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "${name} at least managed to get all her clothes off this time.\n";
        }
        else
        {
            ss << "${name} isn't qualified as a striper, which might explain why she couldn't successfully detach her bra on stage.\n";
            brothel->m_Happiness -= 10;
        }
    }
    else
    {
        ss << "${name} was stiff and nervous, constantly making mistakes. She really isn't good at this job.\n \n";
        wages -= 15;
        if (roll_b <= 20)
        {
            ss << "${name} slipped of the pole and fell flat on her ass.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "${name}'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
            brothel->m_Fame -= 10;
            brothel->m_Happiness += 5;  //would be funny to see
        }
        else if (roll_b <= 60)
        {
            ss << "Seeing the large crowd waiting outside, ${name}'s nerve broke and she wouldn't take the stage.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "Somehow, ${name} forgot that she was a stripper and finished her set with her clothes still on.\n";
            brothel->m_Fame -= 5;
        }
        else
        {
            ss << "Somehow, ${name} managed to be so unsexy that the audience barely seemed to notice her presence.\n";
            brothel->m_Happiness -= 5;
        }
    }


    //base tips, aprox 5-40% of base wages
    tips += (int)(((5 + jobperformance / 6) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 25;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(5))
    {
        ss << "Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n"; wages -= 15;
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
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n"; tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
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


    // lap dance code.. just test stuff for now
    if (lapdance >= 90)
    {
        ss << "${name} doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
        if (roll_c < 5)
        {
            ss << "She sold a champagne dance.";
            tips += 250;
        }
        if (roll_c < 20)
        {
            ss << "She sold a shower dance.";
            tips += 125;
        }
        if (roll_c < 40)
        {
            ss << "She was able to sell a few VIP dances.";
            tips += 160;
        }
        if (roll_c < 60)
        {
            ss << "She sold a VIP dance.";
            tips += 75;
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
        if (roll_c < 5)
        {
            ss << "She convinced a patron to buy a shower dance.";
            tips += 75;
        }
        if (roll_c < 20)
        {
            ss << "Sold a VIP dance to a patron.";
            tips += 70;
        }
        else
        {
            ss << "Sold a few lap dance.";
            tips += 65;
        }
    }
    else if (lapdance >= 40)
    {
        ss << "${name} tried to sell private dances ";
        if (roll_c < 5)
        {
            ss << "and was able to sell a VIP dance against all odds.";
            tips += 50;
        }
        if (roll_c < 20)
        {
            ss << "and was able to sell a lap dance.";
            tips += 25;
        }
        else
        {
            ss << "but wasn't able to sell any.";
        }
    }
    else
    {
        ss << "${name} doesn't seem to understand the real money in stripping is selling private dances.\n";
    }

    if (wages < 0)
        wages = 0;

    if (is_addict(girl, true) && rng.percent(20))
    {
        string warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
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
    girl.AddMessage(ss.str(), imagetype, msgtype);


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
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 30 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 80 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    girl.performance(rng%skill);
    girl.strip(rng%skill + 2);
    girl.upd_temp_stat(STAT_CONFIDENCE, rng % 2); //SIN - slow boost to confidence

    //gained
    cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, actiontype, "${name} has been stripping and having to be sexy for so long she now reeks of sexyness.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Exhibitionist", 60, actiontype, "${name} has been stripping for so long she loves to be naked now.", Day0Night1);
    if (jobperformance >= 140 && rng.percent(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Agile", 40, actiontype, "${name} has been working the pole long enough to become quite Agile.", Day0Night1);
    }

    //lose
    cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 20, actiontype, "${name} has had so many people see her naked she is no longer nervous about anything.", Day0Night1);
    if (jobperformance > 150 && girl.confidence() > 65)
    {
        cGirls::PossiblyLoseExistingTrait(girl, "Shy", 60, actiontype, "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1);
    }

#pragma endregion
    return false;
}
double JP_BarStripper(const sGirl& girl, bool estimate)// not used
{
#if 1  //SIN - standardizing job performance calc per J's instructs
    double jobperformance =
        //basing this on payout logic from code above
        //main stats - first 100 - charisma and beauty are used above to calc typical payout
        (girl.charisma() + girl.beauty() / 2) +
        //secondary stats - second 100 - these set her lapdance chance (so chance of higher payout)
        ((girl.intelligence() + girl.performance() + girl.strip()) / 3) +
        //add level
        girl.level();

    // next up tiredness penalty
#else
    double jobperformance =
        (girl.charisma() / 2 +
        girl.beauty() / 2 +
        girl.performance() / 2 +
        girl.strip() / 2);
#endif
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    if (girl.fame() > 85)      jobperformance += 10;
    if (girl.is_pregnant())                        jobperformance -= 10; //can't move so well

    jobperformance += girl.get_trait_modifier("work.stripper");

    return jobperformance;
}
