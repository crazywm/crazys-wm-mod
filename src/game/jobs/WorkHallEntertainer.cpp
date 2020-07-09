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
#pragma region //    Includes and Externs            //
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include <sstream>

#pragma endregion

// `J` Job Brothel - Hall
bool WorkHallEntertainer(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKHALL;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.disobey_check(actiontype, JOB_ENTERTAINMENT))
    {
        //SIN - More informative mssg to show *what* she refuses
        //ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        ss << "${name} refused to entertain patrons in the gambling hall " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as an entertainer in the gambling hall.\n \n";

    cGirls::UnequipCombat(&girl);    // put that shit away, you'll scare off the customers!

    sGirl* dealeronduty = random_girl_on_job(*girl.m_Building, JOB_DEALER, Day0Night1);
    string dealername = (dealeronduty ? "Dealer " + dealeronduty->FullName() + "" : "the Dealer");

    int wages = 25, tips = 0;
    int work = 0, fame = 0;
    int imagetype = IMGTYPE_BUNNY;
    int msgtype = Day0Night1;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_ENTERTAINMENT, false);

    //SIN: A little pre-randomness
    if (rng.percent(50))
    {
        if (girl.tiredness() > 75)
        {
            ss << "She was very tired, negatively affecting her performance.\n";
            jobperformance -= 10;
        }
        else if (girl.happiness() > 90)
        {
            ss << "Her cheeriness improved her performance.\n";
            jobperformance += 5;
        }
        if (rng.percent(10))
        {
            if (girl.strip() > 60)
            {
                ss << "A born stripper, ${name} wears her clothes just short of showing flesh, just the way the customers like it.\n";
                jobperformance += 15;
            }
            if (girl.pchate() > girl.pcfear())
            {
                ss << " ${name} opened with some rather rude jokes about you. While this annoys you a little, ";
                if (girl.has_active_trait("Your Daughter"))
                {
                    ss << "she is your daughter, and ";
                }
                ss << "it seems to get the audience on her side.\n";
                jobperformance += 15;
            }
        }
    }

    if (jobperformance >= 245)
    {
        ss << " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n \n";
        wages += 155;
        if (roll_b <= 20)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "The heat of her passion and strength of her fierce nature make ${name}'s songs absolutely riveting to listen to.\n";
            }
            else
            {
                ss << "Every one of ${name}'s jokes elicits a wave of laughter from the crowd of patrons, many of whom have no interest in gambling.\n";
            }
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Elegant"))
            {
                ss << "${name}'s very appearance in the door of the gambling hall leads to applause and the sudden appearance of a clear way to the stage steps.\n";
            }
            else
            {
                ss << "Her voice is that of an angel, and her humor is as sharp and cutting as a blade.\n";
            }
        }
        else if (roll_b <= 60)
        {
            ss << "Each shift, ${name} ends her show by blowing a kiss to her fans, producing waves of applause and cheers.\n";
        }
        else if (roll_b <= 80)
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "There is no way to resist the raw charisma ${name} practically exudes, drawing everybody around her towards her stage.\n";
            }
            else
            {
                ss << "She could probably excel in any club or entertainment center - fortunately, ${name}'s in yours, drawing customers.\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "You feel inexpressibly proud that ${name}, your daughter, is such a skilled entertainer.\n";
            }
            else
            {
                ss << "${name} is downright ideal for this job.  Her presentation and material are flawless\n";
            }
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        wages += 95;
        if (roll_b <= 20)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "${name} isn't perfect, but she's your own flesh and blood, which is more then close enough.\n";
            }
            else
            {
                ss << "${name} begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks.\n";
            }
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "The savagery of ${name}'s temper has honed her wit to a razor's edge, letting her jab away at the audience and still rake in the tips.\n";
            }
            else if (girl.has_active_trait("Elegant"))
            {
                ss << "She cooly paces the edge of the stage as she sings, each step deliberate and perfectly poised, as if ${name} was some noble pacing her seat of power.\n";
            }
            else
            {
                ss << "Her jokes are almost always funny as ${name}'s repertoire develops and expands.\n";
            }
        }
        else if (roll_b <= 60)
        {
            ss << "${name}'s arrival on the stage at the beginning of her shift is greeted by scattered applause by the last few gamblers from the previous shift, and the new clients.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "${name}'s soothing voice seems to glide over the noise and bustling of the gamblers and dealers.\n";
        }
        else
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "While her technical skills could use improvement, ${name}'s delivery is beyond reproach.\n";
            }
            else
            {
                ss << " Each shift the tips ${name} collects seem to grow in size!\n";
            }
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        wages += 55;
        if (roll_b <= 20)
        {
            ss << "${name} managed to provide amusement for anybody who happened to listen to her.\n";
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "Her jokes are a bit more violent then they perhaps should be, but the clients seem to like ${name}.\n";
            }
            else if (girl.has_active_trait("Elegant"))
            {
                ss << "The elegance ${name} brings to all things enhances her already pleasant singing.\n";
            }
            else
            {
                ss << "Most of the jokes ${name} told were amusing.\n";
            }
        }
        else if (roll_b <= 60)
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "While ${name}'s repertoire of jokes and other amusements could stand expansion, she's got the presentation down pat.\n";
            }
            else
            {
                ss << "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
            }
        }
        else if (roll_b <= 80)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "Though you'll never admit it, you're happy that ${name} has carved out a niche that doesn't involve renting her cunt to public servants.\n";
            }
            else
            {
                ss << "${name} earned a decent tip today for her sincere, if not always effective, attempts to entertain the clients.\n";
            }
        }
        else
        {
            ss << "A couple of gamblers parted with some of their hard-earned winnings after swearing that her singing had brought them luck.\n";
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;
        if (roll_b <= 20)
        {
            ss << "${name} managed to elicit a few chuckles, and got a few tips.\n";
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "The tips she received were far more than ${name}'s skills had any reason to bring in.\n";
            }
            else
            {
                ss << "While she won't win any contests, ${name} isn't a terrible singer.\n";
            }
        }
        else if (roll_b <= 60)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "While ${name}'s jokes tend to fall flat, nobody seems to have the bravery to tell her that.\n";
            }
            else
            {
                ss << "${name} monologued the guests for an hour or so.  While nobody seemed particularly interested, nobody looked pissed either.\n";
            }
        }
        else if (roll_b <= 80)
        {
            if (girl.has_active_trait("Elegant"))
            {
                ss << "Her air of sophistication lets ${name} get away with telling boring jokes.\n";
            }
            else
            {
                ss << "While most of the jokes ${name} tells are old and worn out, a few were rather amusing.\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "${name} isn't impressive in any way, but she is your daughter, so you dutifully clap as she descends from the stage.\n";
            }
            else
            {
                ss << "The slow song ${name} sang at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
            }
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
        if (roll_b <= 20)
        {
            ss << "While her jokes aren't actually driving anyone off, nobody seems to find ${name} entertaining either.\n";
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "Even though her jokes were stale and boring, the audience couldn't seem to make themselves let ${name} down, and laughed regardless.\n";
            }
            else
            {
                ss << "Her singing is barely acceptable, but fortunately the bustling of the gambling hall drowns ${name} out for the most part.\n";
            }
        }
        else if (roll_b <= 60)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "${name} cursed the audience roundly for their dislike of her music.\n";
            }
            else
            {
                ss << "${name} tells bland jokes and sings forgettable songs.\n";
            }
        }
        else if (roll_b <= 80)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "You wonder if your daughter, child of your loins, might do better on her back then on the stage.\n";
            }
            else
            {
                ss << "${name} doesn't appear to be enjoying herself, an emotion she has in common with the clientele.\n";
            }
        }
        else
        {
            ss << "For all the reaction she's getting, you could probably replace ${name} with a simple mannequin.\n";
        }
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
        if (roll_b <= 20)
        {
            if (girl.has_active_trait("Clumsy"))
            {
                ss << "${name} trips and falls flat on her face when climbing up the stage stairs.  The crowd cheers loudly.\n";
            }
            else if (girl.has_active_trait("Big Boobs") || girl.has_active_trait("Abnormally Large Boobs"))
            {
                ss << "You have a feeling if she wasn't so well endowed no one would show up for her show.\n";
            }
            else
            {
                ss << "You stuck your head into the hall halfway through shift, and swore off the gin for the third time this week after hearing ${name}'s horrible singing.\n";
            }
        }
        else if (roll_b <= 40)
        {
            if (girl.has_active_trait("Fearless"))
            {
                ss << "Despite the crowd's displeasure, ${name} keeps herself cool and collected, apparently unaffected by their ire.\n";
            }
            else
            {
                ss << "She managed to aggravate several customers with her poor taste in jokes.\n";
            }
        }
        else if (roll_b <= 60)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                ss << "As ${name} butchers another song, you feel bitterly ashamed by your relationship to your worthless child.\n";
            }
            else
            {
                ss << "${name}'s as funny as a peasant uprising and about as appreciated by her audience.\n";
            }
        }
        else if (roll_b <= 80)
        {
            if (girl.has_active_trait("Aggressive"))
            {
                ss << "When the crowd booed her, ${name} responded by throwing a shoe at one of the loudest and most insulting members.\n";
            }
            else
            {
                ss << "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic"))
            {
                ss << "The poor quality of her routine was offset by her natural charisma.\n";
            }
            else
            {
                ss << "For some reason, ${name} attempted a dramatic monologue and feigned suicide on stage.  Needless to say, she didn't receive any tips.\n";
            }
        }
    }


    //base tips, aprox 5-30% of base wages
    tips += (int)(((5 + jobperformance / 8) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n";
        tips += 25;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(15))
    {
        ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n";
        wages -= 15;
    }

    if (girl.has_active_trait("Pessimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n";
            tips -= 10;
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
            tips += 10;
        }
    }

    if (girl.has_active_trait("Optimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
            tips -= 10;
        }
        else
        {
            ss << " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
            tips += 10;
        }
    }

    if (rng.percent(5) && (girl.has_active_trait("Charming") || girl.has_active_trait("Charismatic")))
    {
        if (jobperformance < 125)
        {
            ss << "Despite her poor performance, people still applaud loudly for her.\n";
            tips -= 10;
        }
        else
        {
            ss << "People love to see her perform, and ${name} collects a massive tip!.\n";
            tips += 100;
        }
    }

    if (rng.percent(5) && (girl.has_active_trait("Princess") || girl.has_active_trait("Queen")))
    {
        if (jobperformance < 125)
        {
            ss << "Her royal bitchiness combined with the poor entertainment pisses the audience off.\n";
            tips -= 15;
        }
        else
        {
            ss << "${name} owns the stage, imperious eyes flashing out across the audience.  Tips are very good tonight.\n";
            tips += 25;
        }
    }

    if (brothel->num_girls_on_job(JOB_DEALER, false) >= 1 && rng.percent(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name} tried to distract the patrons but due to her lack of skills she distracted " << dealername << " causing you to lose some money.\n";
            wages -= 10;
        }
        else
        {
            ss << "${name} was able to perfectly distract some patrons while the " << dealername << " cheated to make some more money.\n";
            wages += 25;
        }
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
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


    girl.upd_Enjoyment(actiontype, work);
    girl.AddMessage(ss.str(), imagetype, msgtype);


    wages += (rng % ((int)(((girl.beauty() + girl.charisma()) / 2)*0.5f))) + 10;
    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);


    // Improve girl
    int xp = 10, skill = 2;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 30 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 50 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 70 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    girl.confidence(rng%skill);
    girl.performance(rng%skill + 1);


#pragma endregion
    return false;
}

double JP_HallEntertainer(const sGirl& girl, bool estimate)
{
    //SIN - standardizing job performance per J's instructs
    double jobperformance =
        //Core stats - first 100 - pure performance skill (not stripping so beauty isn't primary)
        girl.performance() +
        //secondary stats - second 100 - beauty, charisma and confidence
        ((girl.charisma() + girl.confidence() + girl.beauty()) / 3) +
        //add level
        girl.level();

    // next up tiredness penalty...
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    if (girl.is_pregnant())                    jobperformance -= 5; //can't move so well
    jobperformance += girl.get_trait_modifier("work.hallentertainer");

    return jobperformance;
}
