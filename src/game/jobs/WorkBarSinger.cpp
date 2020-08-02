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
#include <sstream>
#include "cGirls.h"

#pragma endregion

// `J` Job Brothel - Bar
bool WorkBarSinger(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKMUSIC;
    std::stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.disobey_check(actiontype, JOB_SINGER))
    {
        ss << "${name} refused to sing in your bar " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (brothel->m_TotalCustomers < 1)
    {
        ss.str("");
        ss << "There were no customers in the bar on the " << (Day0Night1 ? "night" : "day") << " shift so ${name} just cleaned up a bit.";
        brothel->m_Filthiness -= 20 + girl.service() * 2;
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;
    }

    ss << "${name} worked as a singer in the bar.\n \n";

    cGirls::UnequipCombat(girl); // put that shit away, you'll scare off the customers!

    int wages = 20, tips = 0;
    int enjoy = 0, happy = 0, fame = 0;
    int imagetype = IMGTYPE_SING;
    EventType msgtype = Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SINGER, false);

    const sGirl* pianoonduty = random_girl_on_job(*girl.m_Building, JOB_PIANO, Day0Night1);
    std::string pianoname = (pianoonduty ? "Pianist " + pianoonduty->FullName() + "" : "the Pianist");

    //dont effect things but what she sings
    if (rng.percent(60) && (girl.has_active_trait("Country Gal") || girl.has_active_trait("Farmers Daughter")))
        roll_a = 60;    // country
    else if (rng.percent(60) && (girl.has_active_trait("Elegant")))
        roll_a = 30;    // classical
    else if (rng.percent(60) && (girl.has_active_trait("Aggressive")))
        roll_a = 20;    // death metal
    else if (rng.percent(60) && (girl.has_active_trait("Bimbo")))
        roll_a = 90;    // Pop Songs

    // `CRAZY` The type of music she sings
    std::string song_type_text = "Various types of music";
    /* */if (roll_a <= 10)    { song_type_text = "Goth Rock songs"; }
    else if (roll_a <= 20)    { song_type_text = "Death Metal songs"; }
    else if (roll_a <= 30)    { song_type_text = "Classical songs"; }
    else if (roll_a <= 40)    { song_type_text = "Metal songs"; }
    else if (roll_a <= 50)    { song_type_text = "Rock songs"; }
    else if (roll_a <= 60)    { song_type_text = "Country songs"; }
    else if (roll_a >= 90)    { song_type_text = "Pop songs"; }

    // `CRAZY` How well she sings
    /*default*/    std::string sing_pre_text;
    /* */if (jobperformance >= 245)    { sing_pre_text = " perfectly"; }
    else if (jobperformance >= 185)    { sing_pre_text = " great"; }
    else if (jobperformance >= 145)    { sing_pre_text = " good"; }
    else if (jobperformance >= 100)    { sing_pre_text = " decent"; }
    else if (jobperformance >= 70)    { sing_pre_text = " poorly"; }
    else                            { sing_pre_text = " very poorly"; }

    ss << "She sang " << song_type_text << sing_pre_text << ".\n";


    if (jobperformance >= 245)
    {
        ss << "She must have the voice of an angel, the customers go on and on about her and always come to listen when she sings.\n \n";
        wages += 155;
        if (roll_b <= 20)
        {
            ss << "${name}'s voice brought many patrons to tears as she sang a song full of sadness.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "Wanting to have some fun she encouraged some listeners to sing-along with her.\n";
            happy += 10;
        }
        else if (roll_b <= 60)
        {
            ss << "Feeling a little blue she only sang sad ballads today. You could swear that some customers were crying from the emotion.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "The whole room was quiet when ${name} was singing. After she finished, she gathered listeners applauded for minutes.\n";
            fame += 5;
        }
        else
        {
            ss << "${name}'s soothing voice seems to glide over the noise and bustling of the bar.\n";
        }
    }
    else if (jobperformance >= 185)
    {
        ss << "She's unbelievable at this and is always getting praised by the customers for her voice.\n \n";
        wages += 95;
        if (roll_b <= 25)
        {
            ss << "${name} begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
            fame += 10; wages += 10;
        }
        else if (roll_b <= 50)
        {
            ss << "Her performance was really great, giving the listeners a pleasant time.\n";
            happy += 5;
        }
        else if (roll_b <= 75)
        {
            ss << "When ${name} got on stage the crowd went wild. She didn't disappoint her fans giving one of the best performances in her life.\n";
            happy += 5;
        }
        else
        {
            ss << "${name}'s soothing voice seems to glide over the noise and bustling of the bar.\n";
        }
    }
    else if (jobperformance >= 145)
    {
        ss << "Her voice is really good and gets praised by the customers often.\n \n";
        wages += 55;
        if (roll_b <= 20)
        {
            ss << "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "She sang every part of the song clearly. ${name} is a really good singer.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "This wasn't the best performance of her life time, but in general she did well.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "She is good at this. With some work she could be a star.\n";
            fame += 5;
        }
        else
        {
            ss << "The slow song ${name} sang at the end of shift really had her full emotion and heart.\n";
        }
    }
    else if (jobperformance >= 100)
    {
        ss << "She hits a few right notes but she still has room to improve.\n \n";
        wages += 15;
        if (roll_b <= 25)
        {
            ss << "While she won't win any contests, ${name} isn't a terrible singer.\n";
        }
        else if (roll_b <= 50)
        {
            ss << "She didn't sing every part clearly but overall she was good.\n";
        }
        else if (roll_b <= 75)
        {
            ss << "Maybe she isn't the best but at least she doesn't scare away the customers.\n";
        }
        else
        {
            ss << "The slow song ${name} sang at the end of shift really had her full emotion and heart.  A pity she was bored and tired.\n";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << "She almost never hits a right note. Lucky for you most of your customers are drunks.\n \n";
        wages -= 5;
        if (roll_b <= 20)
        {
            ss << "Her singing is barely acceptable, but fortunately the bustling of the bar drowns ${name} out for the most part.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "After hearing today's performance, you order your guards to gag her for a week.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "Some customers left immediately after ${name} started to sing.\n";
            wages -= 10;
        }
        else if (roll_b <= 80)
        {
            ss << "${name} singing was awful. Not a single line was sung clearly.\n";
            happy -= 5;
        }
        else
        {
            ss << "${name}'s voice is all over the place as she sings.\n";
        }
    }
    else
    {
        ss << "Her voice sounds like nails on a chalk board.  She could be the worst singer ever.\n \n";
        wages -= 15;
        if (roll_b <= 14)
        {
            ss << "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
            happy -= 5;
        }
        else if (roll_b <= 28)
        {
            ss << "It's tragic, ${name} really can't sing.\n";
        }
        else if (roll_b <= 42)
        {
            ss << "She is just terrible. You could swear that your singing under the shower is far better.\n";
        }
        else if (roll_b <= 56)
        {
            ss << "${name} was the first to get on stage before today's star performance. Seeing the gathered crowd, she froze up being able to let out a single word.\n";
        }
        else if (roll_b <= 70)
        {
            ss << "${name} singing was awful. Angry listeners throw random objects at her.\n";
            happy -= 5;
        }
        else if (roll_b <= 84)
        {
            ss << "Hearing ${name}'s singing gave you a thought to use it as a new torture method.\n";
        }
        else
        {
            ss << "${name} bellowed out a melody that caused the bar to go into a panic clearing it quickly.\n";
            happy -= 10;
        }
    }

    //base tips, aprox 5-30% of base wages
    tips += (int)(((5 + jobperformance / 8) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() >85 && rng.percent(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 15;
    }

    if (girl.charisma() > 80 && rng.percent(15))
    {
        ss << "Her charisma shone through as she chatted to customers between songs.\n \n"; tips += 15; happy += 5;
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(5))
    {
        ss << "Her clumsy nature caused her to trip coming on stage causing the crowed to go wild with laughter. She went to the back and hide refusing to take the stage for her song set.\n"; wages -= 15;
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
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her on her singing.\n"; tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
        }
    }

    if (girl.has_active_trait("Psychic") && rng.percent(20))
    {
        ss << "She knew just what songs to sing to get better tips by using her Psychic powers.\n"; tips += 15;
    }

    if (girl.has_active_trait("Assassin") && rng.percent(5))
    {
        if (jobperformance < 150)
        {
            ss << "A patron booed her. She saw red and using her Assassin skills killed him instantly. Other patrons fled out without paying.\n"; wages -= 50;
        }
        else
        {
            ss << "A patron booed her. But was drunk and started crying a moment later so she ignored them.\n";
        }
    }

    if (girl.has_active_trait("Horrific Scars") && rng.percent(15))
    {
        if (jobperformance < 150)
        {
            ss << "A patron gasped and pointed at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
        }
        else
        {
            ss << "A patron gasped and pointed at her Horrific Scars making her sad. Her singing was so wonderful that at the end of the performance they personally apologized and thanked her, leaving her a good tip.\n"; tips += 15;
        }
    }

    if (girl.has_active_trait("Idol") && girl.fame() > 75 && rng.percent(25))
    {
        ss << "Today a large group of ${name}'s followers came to listen to her sing, leaving very generous tips behind.\n";
        wages += 15;
        tips += 25 + girl.fame() / 4;
        girl.fame(1);
    }
    else if (girl.has_active_trait("Idol") && rng.percent(25))
    {
        ss << "A group of ${name}'s fans came to listen to her sing, leaving good tips behind.\n";
        wages += 10;
        tips += 20 + girl.fame() / 5;
        girl.fame(1);
    }
    else if (!girl.has_active_trait("Idol") && girl.fame() > 75 && rng.percent(15))
    {
        ss << "${name} is quite popular in Crossgate so a small crowd of people came in just to listen to her.\n";
        wages += 5;
        tips += 15;
        girl.fame(1);
    }

    if (brothel->num_girls_on_job(JOB_PIANO, Day0Night1) >= 1 && rng.percent(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name}'s singing was out of tune with " << pianoname << " causing customers to leave with their fingers in their ears.\n"; tips -= 10;
        }
        else
        {
            ss << pianoname << " took her singing to the next level causing the tips to flood in.\n"; tips += 40;
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

    brothel->m_Fame += fame;
    brothel->m_Happiness += happy;

    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, msgtype);
    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += 10 + rng%roll_max;

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    int xp = 10, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 30 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 90 && jobperformance >= 185)        { fame += 1; }

    girl.exp(xp);
    if (rng % 2 == 1)
        girl.confidence(rng%skill + 1);
    girl.performance(rng%skill + 1);

    //gain traits
    cGirls::PossiblyGainNewTrait(girl, "Charismatic", 70, actiontype, "Singing on a daily basis has made ${name} more Charismatic.", Day0Night1);
    if (girl.fame() >= 70 && rng.percent(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Idol", 50, actiontype, "Her fame and singing skills has made ${name} an Idol in Crossgate.", Day0Night1);
    }

    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, "${name} seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
    cGirls::PossiblyLoseExistingTrait(girl, "Meek", 50, actiontype, "${name}'s having to sing every day has forced her to get over her meekness.", Day0Night1);


#pragma endregion
    return false;
}
double JP_BarSinger(const sGirl& girl, bool estimate)// not used
{
    double jobperformance =
        // primary - first 100
        girl.performance() +
        // secondary - second 100
        ((girl.charisma() + girl.confidence() + girl.constitution()) / 3) +
        // level bonus
        girl.level();

    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.singer");

    return jobperformance;
}

