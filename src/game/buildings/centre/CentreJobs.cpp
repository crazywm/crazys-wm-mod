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

#include "jobs/GenericJob.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "buildings/IBuilding.h"
#include "cGirls.h"
#include "IGame.h"
#include "character/cPlayer.h"

namespace {
    class CommunityService : public cBasicJob {
    public:
        CommunityService();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    class FeedPoor : public cBasicJob {
    public:
        FeedPoor();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };
}

CommunityService::CommunityService() : cBasicJob(JOB_COMUNITYSERVICE, "CommunityService.xml") {

}

IGenericJob::eCheckWorkResult CommunityService::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(ACTION_WORKCENTRE, JOB_COMUNITYSERVICE))            // they refuse to work
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

sWorkJobResult CommunityService::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    ss << "${name} worked doing community service.\n \n";
    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    bool blow = false, sex = false;
    int dispo = 0;
    int wages = 100;
    int enjoy = 0, help = 0, fame = 0;
    int imagetype = IMGTYPE_PROFILE;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_COMUNITYSERVICE, false);

    //Adding cust here for use in scripts...
    sCustomer Cust = cJobManager::GetMiscCustomer(*brothel);

    // `J` merged slave/free messages and moved actual dispo change to after
    if (jobperformance >= 245)
    {
        dispo = 12;
        ss << " She must be perfect at this.\n \n";
    }
    else if (jobperformance >= 185)
    {
        dispo = 10;
        ss << " She's unbelievable at this and is always getting praised by people for her work.\n \n";
    }
    else if (jobperformance >= 145)
    {
        dispo = 8;
        ss << " She's good at this job and gets praised by people often.\n \n";
    }
    else if (jobperformance >= 100)
    {
        dispo = 6;
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
    }
    else if (jobperformance >= 70)
    {
        dispo = 4;
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
    }
    else
    {
        dispo = 2;
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
    }



    //try and add randomness here
    if (girl.has_active_trait("Nymphomaniac") && chance(30) && !is_virgin(girl)
        && !girl.has_active_trait("Lesbian") && girl.libido() > 75
        && (brothel->is_sex_type_allowed(SKILL_NORMALSEX) || brothel->is_sex_type_allowed(SKILL_ANAL)))
    {
        sex = true;
        ss << "Her Nymphomania got the better of her today and she decided the best way to serve her community was on her back!\n \n";
    }

    if (chance(30) && girl.intelligence() < 55)//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        blow = true;
        ss << "An elderly fellow managed to convince ${name} that the best way to serve her community was on her knees. She ended up giving him a blow job!\n \n";
    }



#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    int roll_a = d100();
    int roll_b = d100();
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

    girl.upd_Enjoyment(ACTION_WORKCENTRE, enjoy);


    if (sex)
    {
        if (brothel->is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || brothel->is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            girl.normalsex(2);
            imagetype = IMGTYPE_SEX;
            if (girl.lose_trait("Virgin"))
            {
                ss << "\nShe is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        else if (brothel->is_sex_type_allowed(SKILL_ANAL))
        {
            girl.anal(2);
            imagetype = IMGTYPE_ANAL;
        }
        brothel->m_Happiness += 100;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.upd_Enjoyment(ACTION_SEX, +3);
        fame += 1;
        dispo += 6;
    }
    else if (blow)
    {
        brothel->m_Happiness += uniform(60, 130);
        dispo += 4;
        girl.oralsex(2);
        fame += 1;
        imagetype = IMGTYPE_ORAL;
    }

    if (girl.is_slave())
    {
        ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
        wages = 0;
    }
    else
    {
        ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
        g_Game->gold().staff_wages(wages);  // wages come from you
        dispo = int(dispo*1.5);
    }

#pragma endregion

    g_Game->player().disposition(dispo);
    girl.AddMessage(ss.str(), imagetype, msgtype);

    help += (int)(jobperformance / 10);        //  1 helped per 10 point of performance

    ss.str("");
    ss << "${name} helped " << help << " people today.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    apply_gains(girl, m_Performance);

    return {false, 0, wages, 0};
}

FeedPoor::FeedPoor() : cBasicJob(JOB_FEEDPOOR, "FeedPoor.xml") {

}

sWorkJobResult FeedPoor::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    ss << "${name} worked feeding the poor.";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    bool blow = false, sex = false;
    int wages = 20;
    int enjoy = 0, feed = 0, fame = 0;
    int roll_b = d100();

    int imagetype = IMGTYPE_PROFILE;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_FEEDPOOR, false);


    //Adding cust here for use in scripts...
    sCustomer Cust = cJobManager::GetMiscCustomer(*brothel);


    int dispo;
    if (jobperformance >= 245)
    {
        add_text("work.perfect");
        dispo = 12;
    }
    else if (jobperformance >= 185)
    {
        add_text("work.great");
        dispo = 10;
    }
    else if (jobperformance >= 145)
    {
        add_text("work.good");
        ss << " She's good at this job and gets praised by people often.\n \n";
        dispo = 8;
    }
    else if (jobperformance >= 100)
    {
        add_text("work.ok");
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        dispo = 6;
    }
    else if (jobperformance >= 70)
    {
        add_text("work.bad");
        dispo = 4;
    }
    else
    {
        add_text("work.worst");
        ss << " \n \n";
        dispo = 2;
    }


    //try and add randomness here
    if (girl.intelligence() < 55 && chance(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        blow = true;    ss << "An elderly fellow managed to convince ${name} that he was full and didn't need anymore food but that she did. He told her his cock gave a special treat if she would suck on it long enough. Which she did man she isn't very smart.\n \n";
    }

    if (girl.has_active_trait("Nymphomaniac") && chance(30) && girl.libido() > 75
        && !girl.has_active_trait("Lesbian") && !is_virgin(girl)
        && (brothel->is_sex_type_allowed(SKILL_NORMALSEX) || brothel->is_sex_type_allowed(SKILL_ANAL)))
    {
        sex = true;
        ss << "Her Nymphomania got the better of her today and she decided to let them eat her pussy!  After a few minutes they started fucking her.\n";
    }



    if (girl.is_slave())
    {
        ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
        g_Game->player().disposition(dispo);
        wages = 0;
    }
    else
    {
        ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
        g_Game->gold().staff_wages(100);  // wages come from you
        g_Game->player().disposition(int(dispo*1.5));
    }




#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    int roll_a = d100();
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


    if (sex)
    {
        if (brothel->is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || brothel->is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            girl.AddMessage(ss.str(), IMGTYPE_SEX, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            girl.normalsex(2);
            if (girl.lose_trait("Virgin"))
            {
                ss << "She is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        else if (brothel->is_sex_type_allowed(SKILL_ANAL))
        {
            girl.AddMessage(ss.str(), IMGTYPE_ANAL, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            girl.anal(2);
        }
        brothel->m_Happiness += 100;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.upd_Enjoyment(ACTION_SEX, +3);
        fame += 1;
        dispo += 6;
    }
    else if (blow)
    {
        brothel->m_Happiness += uniform(60, 130);
        dispo += 4;
        girl.oralsex(2);
        fame += 1;
        girl.AddMessage(ss.str(), IMGTYPE_ORAL, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else
    {
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    feed += (int)(jobperformance / 10);        //  1 feed per 10 point of performance

    int cost = 0;
    for (int i = 0; i < feed; i++)
    {
        cost += uniform(2, 5); // 2-5 gold per customer
    }
    brothel->m_Finance.centre_costs(cost);
    ss.str("");
    ss << "${name} feed " << feed << " costing you " << cost << " gold.";
    girl.AddMessage(ss.str(), imagetype, msgtype);


    apply_gains(girl, m_Performance);

    girl.upd_Enjoyment(ACTION_WORKCENTRE, enjoy);

#pragma endregion
    return {false, 0, 0, std::max(0, wages)};
}

auto FeedPoor::CheckWork(sGirl& girl, bool is_night) -> eCheckWorkResult {
    if (girl.disobey_check(ACTION_WORKCENTRE, JOB_FEEDPOOR))
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

void RegisterCentreJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CommunityService>());
    mgr.register_job(std::make_unique<FeedPoor>());
}