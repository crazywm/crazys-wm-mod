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

#include "GenericJob.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "cGirls.h"
#include "buildings/IBuilding.h"
#include "Game.hpp"

struct DoctorJob : public cBasicJob {
    DoctorJob();
    bool DoWork(sGirl& girl, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

bool DoctorJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKDOCTOR;
    bool SkipDisobey = true; // summary == "SkipDisobey");
    std::stringstream ss;
    if (!SkipDisobey)    // `J` skip the disobey check because it has already been done in the building flow
    {
        if (girl.disobey_check(actiontype, JOB_DOCTOR))            // they refuse to work
        {
            ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
    }
    ss << "${name} worked as a Doctor.\n";


    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the patients!

    int enjoy = 0, wages = 100, tips = 0;

    // this will be added to the clinic's code eventually - for now it is just used for her pay
    int patients = 0;            // `J` how many patients the Doctor can see in a shift


    // Doctor is a full time job now
    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR;


    double jobperformance = girl.job_performance(JOB_DOCTOR, false);

    //enjoyed the work or not
    int roll = d100();
    if (roll <= 10)
    {
        enjoy -= uniform(1, 3);
        jobperformance *= 0.9;
        ss << "Some of the patients abused her during the shift.\n";
    }
    else if (roll >= 90)
    {
        enjoy += uniform(1, 3);
        jobperformance *= 1.1;
        ss << "She had a pleasant time working.\n";
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "Otherwise, the shift passed uneventfully.\n";
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    patients += (int)(jobperformance / 10);        // `J` 1 patient per 10 point of performance

    /* `J` this will be a place holder until a better payment system gets done
    *  this does not take into account any of your girls in surgery
    */
    int earned = 0;
    for (int i = 0; i < patients; i++)
    {
        earned += uniform(50, 100); // 50-100 gold per customer
    }
    brothel->m_Finance.clinic_income(earned);
    ss.str("");
    ss << "${name} earned " << earned << " gold from taking care of " << patients << " patients.\n";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    wages += (patients * 10);
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    girl.upd_Enjoyment(actiontype, enjoy);
    apply_gains(girl);
    return false;
}

DoctorJob::DoctorJob() : cBasicJob(JOB_DOCTOR, "Doctor.xml") {
    m_Info.FullTime = true;
    m_Info.FreeOnly = true;
}

IGenericJob::eCheckWorkResult DoctorJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_RESTING;
        return eCheckWorkResult::IMPOSSIBLE;
    }
    if (girl.is_slave())
    {
        ss << "Slaves are not allowed to be Doctors so ${name} was reassigned to being a Nurse.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_NURSE;
        return eCheckWorkResult::IMPOSSIBLE;
    }
    if (girl.medicine() < 50 || girl.intelligence() < 50)
    {
        ss << "${name} does not have enough training to work as a Doctor. She has been reassigned to Internship so she can learn what she needs.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_INTERN;
        return eCheckWorkResult::IMPOSSIBLE;
    }
    return eCheckWorkResult::ACCEPTS;
}

struct NurseJob : public cBasicJob {
    NurseJob();
    bool DoWork(sGirl& girl, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

NurseJob::NurseJob() : cBasicJob(JOB_NURSE, "Nurse.xml") {
    m_Info.FullTime = true;
}

bool NurseJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKNURSE;
    std::stringstream ss;
    int roll_a = d100(), roll_b = d100();
    ss << get_text("work") << "\n \n";
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
    if (girl.beauty() > 85 && chance(20))
    {
        tips += 25;
        ss << "Stunned by her beauty a customer left her a great tip.\n";
    }

    if (girl.has_active_trait("Clumsy") && chance(20))
    {
        wages -= 15;
        ss << "Her clumsy nature caused her to spill some medicine everywhere.\n";
    }

    if (girl.has_active_trait("Pessimist") && chance(5))
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

    if (girl.has_active_trait("Optimist") && chance(20))
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

    if (girl.intelligence() < 45 && chance(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        hand = true;
        ss << "An elderly fellow managed to convince ${name} that her touch can heal! She ended up giving him a hand job!\n";
    }

    if (chance(30) && !is_virgin(girl) && !girl.has_active_trait("Lesbian")
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
        girl.libido() > 65 && chance(10))
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
        enjoy -= uniform(1, 3);
        jobperformance *= 0.9;
        ss << "Some of the patrons abused her during the shift.";
    }
    else if (roll_a <= 25)
    {
        enjoy += uniform(1, 3);
        jobperformance *= 1.1;
        ss << "She had a pleasant time working.";
    }
    else
    {
        enjoy += uniform(0, 1);
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
        brothel->m_Happiness += uniform(60, 130);
        girl.handjob(2);
        imagetype = IMGTYPE_HAND;
    }
    else if (les)
    {
        brothel->m_Happiness += uniform(30, 100);
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
#pragma region    //    Finish the shift            //


    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    /* `J` this will be a place holder until a better payment system gets done
    *  this does not take into account any of your girls in surgery
    */
    int earned = 0;
    for (int i = 0; i < patients; i++)
    {
        earned += uniform(5, 40); // 5-40 gold per customer
    }
    brothel->m_Finance.clinic_income(earned);
    ss.str("");    ss << "${name} earned " << earned << " gold from taking care of " << patients << " patients.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 50 && jobperformance >= 185)        { fame += 1; }
    girl.fame(fame);
    apply_gains(girl);
    girl.upd_Enjoyment(actiontype, enjoy);

#pragma endregion
    return false;
}

IGenericJob::eCheckWorkResult NurseJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }

    if (girl.disobey_check(ACTION_WORKNURSE, JOB_NURSE))            // they refuse to work
    {
        ss << "${name} refused to see any patients during the " << (is_night ? "night" : "day") << " shift.";
        if (girl.tiredness() > 50 && chance(girl.tiredness() - 30))
        {
            ss << "\nShe was found sleeping " << rng().select_text({"in a supply closet.", "in an empty patient bed."});
            girl.tiredness(-uniform(0, 40));
        }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;;
    }
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}


struct MechanicJob : public cBasicJob {
    MechanicJob();
    bool DoWork(sGirl& girl, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

MechanicJob::MechanicJob() : cBasicJob(JOB_MECHANIC, "Mechanic.xml") {
    m_Info.FullTime = true;
}


bool MechanicJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKMECHANIC;
    std::stringstream ss;
    int roll_a = d100();
    if (girl.disobey_check(actiontype, JOB_MECHANIC))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << get_text("work") << "\n \n";


    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!
    int wages = 25, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_PROFILE;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_MECHANIC, false);


    if (jobperformance >= 245)
    {
        wages += 155;
        ss << "She must be the perfect mechanic patients go on and on about her and always come to see her when she works.\n \n";
    }
    else if (jobperformance >= 185)
    {
        wages += 95;
        ss << "She's unbelievable at this and is always getting praised by the patients for her work.\n \n";
    }
    else if (jobperformance >= 135)
    {
        wages += 55;
        ss << "She's good at this job and gets praised by the patients often.\n \n";
    }
    else if (jobperformance >= 85)
    {
        wages += 15;
        ss << "She made a few mistakes but overall she is okay at this.\n \n";
    }
    else if (jobperformance >= 65)
    {
        wages -= 5;
        ss << "She was nervous and made a few mistakes. She isn't that good at this.\n \n";
    }
    else
    {
        wages -= 15;
        ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
    }


    //try and add randomness here
    if (girl.beauty() > 85 && chance(20))
    {
        wages += 25;
        ss << " Stunned by her beauty a patient left her a great tip.\n \n";
    }
    if (girl.has_active_trait("Construct") && chance(15))
    {
        wages += 15;
        ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
    }
    if (girl.has_active_trait("Half-Construct") && chance(15))
    {
        wages += 15;
        ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
    }
    if (girl.has_active_trait("Clumsy") && chance(15))
    {
        wages -= 15;
        ss << " Her clumsy nature caused her to drop parts everywhere.\n";
    }
    if (girl.has_active_trait("Pessimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            wages -= 10;
            ss << " Her pessimistic mood depressed the patients making them tip less.\n";
        }
        else
        {
            wages += 10;
            ss << "${name} was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
        }
    }
    if (girl.has_active_trait("Optimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            wages -= 10;
            ss << "${name} was in a cheerful mood but the patients thought she needed to work more on her services.\n";
        }
        else
        {
            wages += 10;
            ss << " Her optimistic mood made patients cheer up increasing the amount they tip.\n";
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
#pragma region    //    Finish the shift            //


    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    int roll_max = (girl.intelligence() + girl.service());
    roll_max /= 4;
    wages += uniform(10, 10+roll_max);
    wages += 5 * brothel->num_girls_on_job(JOB_GETREPAIRS, is_night);    // `J` pay her 5 for each patient you send to her
    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 50 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);

    apply_gains(girl);

    girl.upd_Enjoyment(actiontype, enjoy);

#pragma endregion
    return false;
}

IGenericJob::eCheckWorkResult MechanicJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(ACTION_WORKMECHANIC, JOB_MECHANIC))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;
    }
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}


struct InternJob : public cBasicJob {
    InternJob();
    bool DoWork(sGirl& girl, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

InternJob::InternJob() : cBasicJob(JOB_INTERN, "Intern.xml") {

}

double InternJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (estimate)// for third detail string
    {
        double jobperformance =
                (100 - girl.medicine()) +
                (100 - girl.intelligence()) +
                (100 - girl.charisma());

        // traits she could gain/lose
        if (girl.has_active_trait("Nervous")) jobperformance += 20;
        if (!girl.has_active_trait("Charismatic")) jobperformance += 20;
        return jobperformance;
    }
    return 0.0;
}

bool InternJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKTRAINING;
    std::stringstream ss;
    ss << get_text("work") << "\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away

    int enjoy = 0;                                                //
    int wages = 0;                                                //
    int tips = 0;                                                //
    int train = 0;                                                // main skill trained
    int tmed = girl.medicine();                                // Starting level - train = 1
    int tint = girl.intelligence();                            // Starting level - train = 2
    int tcha = girl.charisma();                                // Starting level - train = 3
    bool gaintrait = false;                                        // posibility of gaining a trait
    bool promote = false;                                        // posibility of getting promoted to Doctor or Nurse
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = brothel->m_Filthiness / 100;                // training time wasted with bad equipment
    int sgMed = 0, sgInt = 0, sgCha = 0;                        // gains per skill
    int roll_a = d100();                                    // roll for main skill gain
    int roll_b = d100();                                    // roll for main skill trained
    int roll_c = d100();                                    // roll for enjoyment


    /* */if (roll_a <= 5)    skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else /*             */    skill = 3;
    /* */if (girl.has_active_trait("Quick Learner"))    { skill += 1; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Clinic is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 40  && tmed < 100)    train = 1;    // medicine
        else if (roll_b < 70  && tint < 100)    train = 2;    // intelligence
        else if (roll_b < 100 && tcha < 100)    train = 3;    // charisma
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || chance(5)) gaintrait = true;
    if (train == 0 && girl.medicine() > 70 && girl.intelligence() > 70)    promote = true;

    if (train == 1) { sgMed = skill; ss << "She learns how to work with medicine better.\n"; }    else sgMed = uniform(0, 2);
    if (train == 2) { sgInt = skill; ss << "She got smarter today.\n"; }                        else sgInt = uniform(0, 1);
    if (train == 3) { sgCha = skill; ss << "She got more charismatic today.\n"; }                else sgCha = uniform(0, 1);

    if (sgMed + sgInt + sgCha > 0)
    {
        ss << "She managed to gain:\n";
        if (sgMed > 0) { ss << sgMed << " Medicine.\n";        girl.medicine(sgMed); }
        if (sgInt > 0) { ss << sgInt << " Intelligence.\n";    girl.intelligence(sgInt); }
        if (sgCha > 0) { ss << sgCha << " Charisma.\n";        girl.charisma(sgCha); }
    }

    int trycount = 10;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (uniform(0, 10))
        {
            case 0:
                if (girl.lose_trait( "Nervous"))
                {
                    ss << "She seems to be getting over her Nervousness with her training.";
                    gaintrait = false;
                }
                break;
            case 1:
                if (girl.lose_trait( "Meek"))
                {
                    ss << "She seems to be getting over her Meekness with her training.";
                    gaintrait = false;
                }
                break;
            case 2:
                if (girl.lose_trait( "Dependant"))
                {
                    ss << "She seems to be getting over her Dependancy with her training.";
                    gaintrait = false;
                }
                break;
            case 3:
                if (girl.gain_trait( "Charismatic"))
                {
                    ss << "Dealing with patients and talking with them about their problems has made ${name} more Charismatic.";
                    gaintrait = false;
                }
                break;
            case 4:
                break;
            case 5:

                break;
            case 6:

                break;

            default:    break;    // no trait gained
        }
    }



    //enjoyed the work or not
    /* */if (roll_c <= 10)    { enjoy -= uniform(1, 3);    ss << "Some of the patrons abused her during the shift."; }
    else if (roll_c >= 90)    { enjoy += uniform(1, 3);    ss << "She had a pleasant time working."; }
    else /*             */    { enjoy += uniform(0, 1);        ss << "Otherwise, the shift passed uneventfully."; }
    girl.upd_Enjoyment(actiontype, enjoy);

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    if (girl.is_unpaid()) { wages = 0; }
    else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait("Quick Learner"))        { xp += 2; }
    else if (girl.has_active_trait("Slow Learner"))    { xp -= 2; }

    girl.exp(uniform(1, xp));
    girl.upd_temp_stat(STAT_LIBIDO, skill / 2);

    if (girl.medicine() + girl.intelligence() + girl.charisma() >= 300) promote = true;
    if (promote)
    {
        ss.str("");
        ss << "${name} has completed her Internship and has been promoted to ";
        if (girl.is_slave())    { ss << "Nurse.";    girl.m_DayJob = girl.m_NightJob = JOB_NURSE; }
        else /*            */    { ss << "Doctor.";    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR; }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
    }

    return false;
}

IGenericJob::eCheckWorkResult InternJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }
    if (girl.medicine() + girl.intelligence() + girl.charisma() >= 300)
    {
        ss << "There is nothing more she can learn here so she is promoted to ";
        if (girl.is_slave())    { ss << "Nurse.";    girl.m_DayJob = girl.m_NightJob = JOB_NURSE; }
        else /*            */    { ss << "Doctor.";    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR; }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }

    if (girl.disobey_check(ACTION_WORKTRAINING, JOB_INTERN))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

struct JanitorJob : public cBasicJob {
    JanitorJob();
    bool DoWork(sGirl& girl, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

JanitorJob::JanitorJob() : cBasicJob(JOB_JANITOR, "Janitor.xml") {
}

void CleaningUpdateGirl(sGirl& girl, cRng& rng, bool is_night, int enjoy, int tips, int wages, int clean_amount);

bool JanitorJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKCLEANING;
    std::stringstream ss;
    int roll_a = d100(), roll_b = d100();
    ss << "${name} worked cleaning the Clinic.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away

    double CleanAmt = girl.job_performance(JOB_JANITOR, false);
    int enjoy = 0;
    int wages = 0;
    int tips = 0;
    bool playtime = false;

    if (roll_a <= 10)
    {
        enjoy -= uniform(1, 3);
        CleanAmt *= 0.8;
        if (roll_b < 50)    ss << "She spilled a bucket of something unpleasant all over herself.";
        else                ss << "She did not like cleaning the Clinic today.";
    }
    else if (roll_a >= 90)
    {
        enjoy += uniform(1, 3);
        CleanAmt *= 1.1;
        if (roll_b < 50)    ss << "She cleaned the building while humming a pleasant tune.";
        else                ss << "She had a great time working today.";
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt *= 0.9;
        wages = 0;
    }
    else
    {
        wages = std::min(30, int(30 + (CleanAmt / 10))); // `J` Pay her based on how much she cleaned
    }

    // `J` if she can clean more than is needed, she has a little free time after her shift
    if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
    ss << "\n \nCleanliness rating improved by " << (int)CleanAmt;
    if (playtime)    // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she ";
        if (girl.is_pregnant() && girl.health() < 90)
        {
            ss << "got a quick checkup and made sure her unborn baby was doing OK.";
            girl.health(10);
            girl.happiness(uniform(2, 5));
        }
        else if (girl.health() < 80)
        {
            ss << "got a quick checkup.";
            girl.health(10);
        }
        else if (girl.is_pregnant() || chance(40))
        {
            ss << "hung out in the maternity ward watching the babies.";
            girl.happiness(uniform(-2, 3));
        }
        else if (girl.tiredness() > 50 && brothel->free_rooms() > 10)
        {
            ss << "found an empty room and took a nap.";
            girl.tiredness(-uniform(5, 15));
        }
        else
        {
            int d = brothel->num_girls_on_job(JOB_DOCTOR, is_night);
            int n = brothel->num_girls_on_job(JOB_NURSE, is_night);
            ss << "watched the ";
            if (d + n < 1)
            {
                ss << "people wander about the Clinic.";
                girl.happiness(uniform(-1, 1));
            }
            else
            {
                if (d > 0)            ss << "doctor" << (d > 1 ? "s" : "");
                if (d > 0 && n > 0)    ss << " and ";
                if (n > 0)            ss << "nurse" << (n > 1 ? "s" : "");
                ss << " do their job" << (d + n > 1 ? "s" : "");
                girl.medicine(uniform(0, 1));
            }
        }
    }

    // do all the output
    girl.AddMessage(ss.str(), IMGTYPE_MAID, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve girl
    CleaningUpdateGirl(girl, rng(), is_night, enjoy, tips, wages, CleanAmt);

    return false;
}

IGenericJob::eCheckWorkResult JanitorJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(ACTION_WORKCLEANING, JOB_JANITOR))
    {
        girl.AddMessage("${name} refused to clean the Clinic.", IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;;
    }
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}


void RegisterClinicJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<DoctorJob>());
    mgr.register_job(std::make_unique<NurseJob>());
    mgr.register_job(std::make_unique<MechanicJob>());
    mgr.register_job(std::make_unique<InternJob>());
    mgr.register_job(std::make_unique<JanitorJob>());
}