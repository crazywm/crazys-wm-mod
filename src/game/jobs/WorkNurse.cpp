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
#include "character/cCustomers.h"
#include "Game.hpp"
#include "character/predicates.h"
#include "cJobManager.h"

#pragma endregion

// `J` Job Clinic - Staff
bool WorkNurse(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKNURSE;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_CLINICREST);
        return false;
    }

    if (girl.disobey_check(actiontype, JOB_NURSE))            // they refuse to work
    {
        ss << "${name} refused to see any patients during the " << (Day0Night1 ? "night" : "day") << " shift.";
        if (girl.tiredness() > 50 && rng.percent(girl.tiredness() - 30))
        {
            ss << "\nShe was found sleeping ";
            if (rng % 2)        ss << "in a supply closet.";
            else                ss << "in an empty patient bed.";
            girl.tiredness(-(rng % 40));
        }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a nurse.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the patients!

    int wages = 25, tips = 0;
    int enjoy = 0, fame = 0;
    bool hand = false, sex = false, les = false;

    int imagetype = IMGTYPE_NURSE;

#pragma endregion
#pragma region //    Job Performance            //



    // this will be added to the clinic's code eventually - for now it is just used for her pay
    int patients = 0;            // `J` how many patients the Doctor can see in a shift


    double jobperformance = girl.job_performance(JOB_NURSE, false);

    //Adding cust here for use in scripts...
    sCustomer Cust = cJobManager::GetMiscCustomer(*brothel);

    if (jobperformance >= 245)
    {
        ss << "She must be the perfect nurse, patients go on and on about her and always come to see her when she works.\n \n";
        wages += 155;
        if (roll_b <= 20)
        {
            ss << "Just the sight of ${name} brings happiness to the patients. She is a blessing to the entire ward.";
        }
        else if (roll_b <= 40)
        {
            ss << "Today the knowledge and swift reaction from ${name} saved a patient's life!";
        }
        else if (roll_b <= 60)
        {
            ss << "She was doing her routine with a wide, beautiful smile and humming a pleasant tune.";
        }
        else if (roll_b <= 80)
        {
            ss << "Her work is her life. She finds fulfillment in every day working in the clinic.";
        }
        else
        {
            ss << "She spent her day supervising new nurses, showing them how to do the work properly and answering tons of questions.";
        }
    }
    else if (jobperformance >= 185)
    {
        ss << "She's unbelievable at this and is always getting praised for her work by the patients.\n \n";
        wages += 95;
        if (roll_b <= 20)
        {
            ss << "${name} is good at her work. Besides helping patients, she gave some pointers to younger, less experience coworkers.";
        }
        else if (roll_b <= 40)
        {
            ss << "${name} was on her feet all day long. She was really tired but also felt fulfilled.";
        }
        else if (roll_b <= 60)
        {
            ss << "She managed to calm a patient before a major surgery.";
        }
        else if (roll_b <= 80)
        {
            ss << "When assisting in a surgery, she knew what tool will be needed, before the surgeon called for them.";
        }
        else
        {
            ss << "${name} is really committed to her job as a nurse. She's starting to feel enjoyment after every hard worked day.";
        }
    }
    else if (jobperformance >= 135)
    {
        ss << "She's good at this job and gets praised by the patients often.\n \n";
        wages += 55;
        if (roll_b <= 20)
        {
            ss << "She made one of the patients heart beat dangerously high, while leaning over his face to rearrange the pillow.";
        }
        else if (roll_b <= 40)
        {
            ss << "${name} made her rounds and spend the rest of the day chatting with other staff members.";
        }
        else if (roll_b <= 60)
        {
            ss << "When giving medicine to the patient, ${name} noticed that the dosage for one of them was wrong. She corrected her mistake immediately.";
        }
        else if (roll_b <= 80)
        {
            ss << "Today she was assigned to the surgery room. She learned something new.";
        }
        else
        {
            ss << "It was a busy day for ${name}, but she manage to get thru it without any slipups.";
        }
    }
    else if (jobperformance >= 85)
    {
        ss << "She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;
        if (roll_b <= 20)
        {
            ss << "She accidentally gave a patient the wrong dosage of their drugs! Fortunately she understated them.";
        }
        else if (roll_b <= 40)
        {
            ss << "${name} made a mistake while changing bandages of one of her patients. The head nurse scolded her.";
        }
        else if (roll_b <= 60)
        {
            ss << "She spent her shift following the doctor and learning more about the job.";
        }
        else if (roll_b <= 80)
        {
            ss << "She slapped an older patient for groping her ass! Good thing that the doctor was near to help him stay alive!";
        }
        else
        {
            ss << "When giving a sponge bath to one of male patients she accidentally aroused his manhood";
            if (girl.has_active_trait("Shy") || girl.has_active_trait("Meek"))
            {
                ss << ". Ashamed from the sight, she run out the room!";
            }
            else
            {
                ss << " but it didn't bother her any.";
            }
        }
    }
    else if (jobperformance >= 65)
    {
        ss << "She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
        if (roll_b <= 20)
        {
            ss << "When prepping her patient before a mayor surgery she described all possible bad outcomes forgetting to mentioning the low risk percentage and good sides of this treatment.";
        }
        else if (roll_b <= 40)
        {
            ss << "${name} took the temperature of all patients in her block with the same thermometer without sterilizing it!";
        }
        else if (roll_b <= 60)
        {
            ss << "She got scolded today by the doctor for laughing at the patients that came in for liposuction about their weight.";
        }
        else if (roll_b <= 80)
        {
            ss << "Being responsible for utilizing today's medical waste, ${name} just flashed them down the toilet.";
        }
        else
        {
            ss << "She spend most of her shift snoozing in the basement medical supply storage.";
        }
    }
    else
    {
        ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
        if (roll_b <= 20)
        {
            ss << "Some could say that her technique of drawing blood is the same way people play pin the tail on the donkey.";
        }
        else if (roll_b <= 40)
        {
            ss << "Sponge bath, skinning someone alive, no difference for ${name}.";
        }
        else if (roll_b <= 60)
        {
            ss << "${name} covered her mouth while sneezing several times. After that she went to assist in the ongoing surgery without changing her gloves!";
        }
        else if (roll_b <= 80)
        {
            ss << "While assisting the doctor, ${name} giggled when the patient was telling about their health problems.";
        }
        else
        {
            ss << "${name} fainted at the first sight of blood. She wasn't helpful today.";
        }
    }
    ss << "\n \n";

    //try and add randomness here
    if (girl.beauty() > 85 && rng.percent(20))
    {
        tips += 25;
        ss << "Stunned by her beauty a customer left her a great tip.\n";
    }

    if (girl.has_active_trait("Clumsy") && rng.percent(20))
    {
        wages -= 15;
        ss << "Her clumsy nature caused her to spill some medicine everywhere.\n";
    }

    if (girl.has_active_trait("Pessimist") && rng.percent(5))
    {
        if (jobperformance < 125)
        {
            wages -= 10;
            ss << "Her pessimistic mood depressed the patients making them tip less.\n";
        }
        else
        {
            tips += 10;
            ss << "${name} was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
        }
    }

    if (girl.has_active_trait("Optimist") && rng.percent(20))
    {
        if (jobperformance < 125)
        {
            wages -= 10;
            ss << "${name} was in a cheerful mood but the patients thought she needed to work more on her services.\n";
        }
        else
        {
            tips += 10;
            ss << "Her optimistic mood made patients cheer up increasing the amount they tip.\n";
        }
    }

    if (girl.intelligence() < 45 && rng.percent(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        hand = true;
        ss << "An elderly fellow managed to convince ${name} that her touch can heal! She ended up giving him a hand job!\n";
    }

    if (rng.percent(30) && !is_virgin(girl) && !girl.has_active_trait("Lesbian")
        && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Slut") || girl.has_active_trait("Succubus") ||
            girl.has_active_trait("Bimbo")))
    {
        if (girl.libido() > 65 && (brothel->is_sex_type_allowed(SKILL_NORMALSEX) || brothel->is_sex_type_allowed(SKILL_ANAL)))
        {
            tips += 50;
            sex = true;
            enjoy += 1;
            ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. The man took advantage and fucked her brains out!\n";
        }
        else
        {
            ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. But she wasn't in the mood so she left.\n";
        }
    }

    if (girl.has_active_trait("Lesbian") && girl.has_active_trait("Aggressive") &&
        girl.libido() > 65 && rng.percent(10))
    {
        les = true;
        enjoy += 1;
        ss << "When giving a sponge bath to one of her female patients she couldn't help herself and took advantage of the situation.\n";
    }
    ss << "\n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //
    //enjoyed the work or not
    if (roll_a <= 5)
    {
        enjoy -= rng % 3 + 1;
        jobperformance *= 0.9;
        ss << "Some of the patrons abused her during the shift.";
    }
    else if (roll_a <= 25)
    {
        enjoy += rng % 3 + 1;
        jobperformance *= 1.1;
        ss << "She had a pleasant time working.";
    }
    else
    {
        enjoy += rng % 2;
        ss << "Otherwise, the shift passed uneventfully.";
    }

    if (sex)
    {
        if (brothel->is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || brothel->is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            imagetype = IMGTYPE_SEX;
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
            imagetype = IMGTYPE_ANAL;
            girl.anal(2);
        }
        brothel->m_Happiness += 100;
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        girl.upd_Enjoyment(ACTION_SEX, +3);
    }
    else if (hand)
    {
        brothel->m_Happiness += (rng % 70) + 60;
        girl.handjob(2);
        imagetype = IMGTYPE_HAND;
    }
    else if (les)
    {
        brothel->m_Happiness += (rng % 70) + 30;
        imagetype = IMGTYPE_LESBIAN;
        girl.lesbian(2);
    }

    if (girl.is_unpaid())
    {
        jobperformance *= 0.9;
        patients += (int)(jobperformance / 5);        // `J` 1 patient per 5 point of performance
        wages = 0;
    }
    else
    {
        patients += (int)(jobperformance / 5);        // `J` 1 patient per 5 point of performance
        wages += patients * 2;                // `J` pay her 2 for each patient you send to her
    }



#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //


    girl.AddMessage(ss.str(), imagetype, Day0Night1);

    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    /* `J` this will be a place holder until a better payment system gets done
    *  this does not take into account any of your girls in surgery
    */
    int earned = 0;
    for (int i = 0; i < patients; i++)
    {
        earned += rng % 35 + 5; // 5-40 gold per customer
    }
    brothel->m_Finance.clinic_income(earned);
    ss.str("");    ss << "${name} earned " << earned << " gold from taking care of " << patients << " patients.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);


    // Improve stats
    int xp = 10 + patients / 2, libido = 0, skill = 2 + patients / 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.has_active_trait("Lesbian"))                { libido += patients / 2; }
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 50 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);
    girl.exp(xp);
    if (rng % 2 == 1)    girl.intelligence(rng%skill);
    else                girl.charisma(rng%skill);
    girl.medicine(rng%skill + 1);
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    girl.upd_Enjoyment(actiontype, enjoy);
    //gain traits
    cGirls::PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made ${name} more Charismatic.", Day0Night1);
    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, "${name} seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);

#pragma endregion
    return false;
}

double JP_Nurse(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = (girl.intelligence() / 2 +
        girl.charisma() / 2 +
        girl.medicine() +
        girl.level() / 5);
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.nurse");

    return jobperformance;
}
