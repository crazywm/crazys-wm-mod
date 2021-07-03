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

#include "jobs/BasicJob.h"
#include "character/sGirl.h"
#include "character/predicates.h"
#include "character/cCustomers.h"
#include "IGame.h"
#include "cGirls.h"
#include "buildings/IBuilding.h"
#include "cInventory.h"

namespace {
    class Barmaid : public cBasicJob {
    public:
        Barmaid();
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    };

    class Stripper : public cBasicJob {
    public:
        Stripper();
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    };

    class Waitress : public cBasicJob {
    public:
        Waitress();
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    };

}

Barmaid::Barmaid() : cBasicJob(JOB_SLEAZYBARMAID, "StripBarMaid.xml") {

}

IGenericJob::eCheckWorkResult Barmaid::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKCLUB);
}

sWorkJobResult Barmaid::DoWork(sGirl& girl, bool is_night) {
    Action_Types actiontype = ACTION_WORKCLUB;
    add_text("work");

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_ECCHI;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SLEAZYBARMAID, false);



    if (jobperformance >= 245)
    {
        ss << " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n \n";
        m_Earnings = 170;
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        m_Earnings = 110;
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        m_Earnings = 70;
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        m_Earnings = 30;
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        m_Earnings = 10;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        m_Earnings = 0;
    }


    //base tips, aprox 10-20% of base m_Earnings
    m_Tips += (int)(((10 + jobperformance / 22) * m_Earnings) / 100);

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
        if (jobperformance < 150)
        {
            ss << "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n";
        }
        else
        {
            ss << "A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n"; m_Earnings += 15;
        }
    }

    if (girl.has_active_trait("Meek") && chance(5) && jobperformance < 125)
    {
        ss << "${name} spilled a drink all over a man's lap. He told her she had to lick it up and forced her to clean him up which she Meekly accepted and went about licking his cock clean.\n";
        imagetype = IMGTYPE_ORAL;
        enjoy -= 3;
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

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    int roll_a = d100();
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
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Earnings += 10 + uniform(0, roll_max);

    // Improve stats
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 185)        { fame += 1; }

    apply_gains(girl, jobperformance);

    girl.fame(fame);

    //gained
    if (jobperformance < 100 && roll_a <= 2) { cGirls::PossiblyGainNewTrait(girl, "Assassin", 10, actiontype, "${name}'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.", is_night); }
    if (chance(25) && girl.dignity() < 0 && (imagetype == IMGTYPE_SEX || imagetype == IMGTYPE_ORAL))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }


#pragma endregion
    return {false, m_Tips, m_Earnings, 0};
}

Stripper::Stripper() : cBasicJob(JOB_BARSTRIPPER, "StripStripper.xml") {

}

IGenericJob::eCheckWorkResult Stripper::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKSTRIP);
}

sWorkJobResult Stripper::DoWork(sGirl& girl, bool is_night) {
    add_text("work");

    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKSTRIP;
    std::stringstream ss;
    int roll_a = d100(), roll_b = d100(), roll_c = d100();

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    m_Earnings = 30;
    int enjoy = 0;
    int imagetype = IMGTYPE_STRIP;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_BARSTRIPPER, false);
    int lapdance = (girl.intelligence() / 2 +
                    girl.performance() / 2 +
                    girl.strip()) / 2;


    //what is she wearing?
    if (girl.has_item("Rainbow Underwear"))
    {
        ss << "${name} stripped down to reveal her Rainbow Underwear to the approval of the patrons watching her.\n \n";
        brothel->m_Happiness += 5; jobperformance += 5; m_Tips += 10;
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
        jobperformance += 5; m_Tips += 15;
    }


    if (jobperformance >= 245)
    {
        ss << " She must be the perfect stripper, customers go on and on about her and always come to see her when she works.\n \n";
        m_Earnings += 155;
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
            m_Tips += 15;
        }
        else
        {
            ss << "Somehow, ${name} managed to be so sexy that you thought you might need to close just to clean up the mess the crowd made.\n";
            brothel->m_Happiness += 10;
            brothel->m_Filthiness += 5;
            m_Earnings += 10;
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        m_Earnings += 95;
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
        m_Earnings += 55;
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
            m_Tips += 10;
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
        m_Earnings += 15;
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
        m_Earnings -= 5;
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
        m_Earnings -= 15;
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


    //base tips, aprox 5-40% of base m_Earnings
    m_Tips += (int)(((5 + jobperformance / 6) * m_Earnings) / 100);

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
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(actiontype, enjoy);
    girl.AddMessage(ss.str(), imagetype, msgtype);


    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    m_Earnings += uniform(10, 10+roll_max);

    // Improve stats
    int fame = 0;
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 30 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 80 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    apply_gains(girl, jobperformance);

    //gained
    if (jobperformance >= 140 && chance(25))
    {
        cGirls::PossiblyGainNewTrait(girl, "Agile", 40, actiontype, "${name} has been working the pole long enough to become quite Agile.", is_night);
    }

    //lose
    if (jobperformance > 150 && girl.confidence() > 65)
    {
        cGirls::PossiblyLoseExistingTrait(girl, "Shy", 60, actiontype, "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.", is_night);
    }

#pragma endregion
    return {false, std::max(0, m_Tips), std::max(0, m_Earnings), 0};
}

Waitress::Waitress() : cBasicJob(JOB_SLEAZYWAITRESS, "StripWaitress.xml") {

}

IGenericJob::eCheckWorkResult Waitress::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKCLUB);
}

sWorkJobResult Waitress::DoWork(sGirl& girl, bool is_night) {
    add_text("work");
    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    auto brothel = girl.m_Building;

    m_Earnings = 25;
    int enjoy = 0, anal = 0, oral = 0, hand = 0, fame = 0;
    int imagetype = IMGTYPE_ECCHI;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SLEAZYWAITRESS, false);

    auto undignified = [&](){
        switch (uniform(0, 10))
        {
            case 1:        girl.sanity(-uniform(0, 5));        if (chance(50)) break;
            case 2:        girl.confidence(-uniform(0, 5));    if (chance(50)) break;
            case 3:        girl.dignity(-uniform(0, 5));        if (chance(50)) break;
            default:    enjoy -= uniform(0, 5);    break;
        }
    };

    //a little pre-game randomness
    if (girl.has_active_trait("Cum Addict") && chance(30))
    {
        ss << "${name} is addicted to cum, and she cannot serve her shift without taking advantage of a room full of cocks. Since most of your patrons are already sexually primed with all this nubile flesh walking around in skimpy uniforms, she does not need to be very persuasive to convince various men to satisfy her addiction. You see her feet sticking out from under the tables from time to time as a satisfied customer smiles at the ceiling. Her service with the other tables suffers, but her tips are still quite high.\n";
        jobperformance -= 10;
        m_Tips += 40;
        imagetype = IMGTYPE_ORAL;
    }
    else if ((girl.has_active_trait("Shy") || girl.has_active_trait("Nervous")) && chance(20))
    {
        ss << "${name} has serious difficulty being around all these new people, and the fact that they are all so forward about her body does nothing to help. She spends a lot of time hiding in the kitchen, petrified of going back out and talking to all those people.";
        jobperformance -= 20;
    }


    if (jobperformance >= 245)
    {
        add_text("work.perfect");
        m_Earnings += 155;
        brothel->m_Fame += 5;
        m_Tips += 15;
    }
    else if (jobperformance >= 185)
    {
        add_text("work.great");
        m_Earnings += 95;
        m_Tips += 10;
    }
    else if (jobperformance >= 145)
    {
        add_text("work.good");
        m_Earnings += 55;
        m_Tips += 5;
    }
    else if (jobperformance >= 100)
    {
        add_text("work.ok");
        m_Earnings += 15;
    }
    else if (jobperformance >= 70)
    {
        add_text("work.bad");
        m_Earnings -= 5;
    }
    else
    {
        add_text("work.worst");
        m_Earnings -= 15;
        brothel->m_Happiness -= 5;
        brothel->m_Fame -= 5;
    }


    //base tips, aprox 10-20% of base wages
    m_Tips += (((10.0 + jobperformance / 22.0) * (double)m_Earnings) / 100.0);

    //try and add randomness here
    add_text("event.post");

    if (girl.has_active_trait("Great Arse") && chance(15))
    {
        if (jobperformance >= 185) //great
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
        else if (jobperformance >= 135) //decent or good
        {
            ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything.\n";
        }
        else if (jobperformance >= 85) //bad
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
        enjoy -= 5;
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
        sCustomer Cust = g_Game->GetCustomer(*brothel);
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
                brothel->m_Happiness += 5;
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

    if (girl.has_active_trait("Exhibitionist") && chance(50))
    {
        ss << "${name} is a rampant exhibitionist, and this job gives her a lot of opportunities to flash her goods at select customers. She has cut her uniform top to be even shorter than usual, and her nipples constantly appear when she flashes her underboobs. ${name} does a great job of increasing the atmosphere of sexual tension in your restaurant.";
        brothel->m_Happiness += 15;
    }

    if (chance(35) && (girl.breast_size() >= 5 || girl.has_active_trait("Sexy Air")))
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

    if (brothel->num_girls_on_job(JOB_SLEAZYBARMAID, false) >= 1 && chance(25))
    {
        if (jobperformance > 100)
        {
            ss << "\nWith help from the Barmaid, ${name} provided better service to the customers, increasing her tips.\n";
            m_Tips *= 1.2;
        }
    }
    else if (brothel->num_girls_on_job(JOB_SLEAZYBARMAID, false) == 0 && jobperformance <= 100)
    {
        ss << "\n${name} had a hard time attending all the customers without the help of a barmaid.\n";
        m_Tips *= 0.9;
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    int roll_a = d100();
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
    m_Earnings += uniform(10, 10 + roll_max);

#pragma endregion
#pragma region    //    Finish the shift            //


    girl.oralsex(oral);
    girl.handjob(hand);
    girl.anal(anal);
    girl.upd_Enjoyment(ACTION_WORKCLUB, enjoy);
    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    apply_gains(girl, m_Performance);

    if (!girl.has_active_trait("Straight"))    {
        girl.upd_temp_stat(STAT_LIBIDO, std::min(3, brothel->num_girls_on_job(JOB_BARSTRIPPER, false)));
    }

    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);

    apply_gains(girl, jobperformance);

    //gained traits
    if (jobperformance > 150 && girl.constitution() > 65) { cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, ACTION_WORKCLUB, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet Of Foot.", is_night); }
    if (chance(25) && girl.dignity() < 0 && (anal > 0 || oral > 0 || hand > 0))
    {
        cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", is_night, EVENT_WARNING);
    }


#pragma endregion
    return {false, m_Tips, m_Earnings, 0};
}

void RegisterStripClubJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<Barmaid>());
    mgr.register_job(std::make_unique<Stripper>());
    mgr.register_job(std::make_unique<Waitress>());
}

