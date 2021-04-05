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
}

Barmaid::Barmaid() : cBasicJob(JOB_SLEAZYBARMAID) {

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

void RegisterStripClubJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<Barmaid>());
    mgr.register_job(std::make_unique<Stripper>());
}

