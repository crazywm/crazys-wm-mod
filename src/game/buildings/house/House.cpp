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

#include "jobs/SimpleJob.h"
#include "character/sGirl.h"
#include "character/cPlayer.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "character/traits/ITraitsCollection.h"
#include "buildings/cBuildingManager.h"
#include "buildings/cDungeon.h"
#include "cGirls.h"
#include "IGame.h"

namespace {
    struct HouseCook: public cSimpleJob {
        HouseCook();
        bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    };

    struct HousePet: public cBasicJob {
        HousePet();
        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    struct Recruiter: public cSimpleJob {
        Recruiter();
        bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    };

    struct PersonalTraining: public cBasicJob {
        PersonalTraining();
        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
        double GetPerformance(const sGirl& girl, bool estimate) const override;
    };
}

HouseCook::HouseCook() : cSimpleJob(JOB_HOUSECOOK, "HouseCook.xml", {ACTION_WORKCOOKING, 20, EImageBaseType::COOK}) {

}

bool HouseCook::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    m_Wages += (int)m_PerformanceToEarnings((float)m_Performance);
    brothel.update_all_girls_stat(STAT_HAPPINESS, get_performance_class(m_Performance) - 2);
    add_performance_text();

    // enjoyed the work or not
    shift_enjoyment();

    // do all the output
    girl.AddMessage(ss.str(), m_ImageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    HandleGains(girl, 0);

    return false;
}


HousePet::HousePet() : cBasicJob(JOB_HOUSEPET, "HousePet.xml") {
    m_Info.FullTime = true;
}

sWorkJobResult HousePet::DoWork(sGirl& girl, bool is_night) {
    if (is_night) return {false, 0, 0, 0};
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKHOUSEPET;
    int roll_a = d100(), roll_b = d100(), roll_c = d100();
    int train = roll_a - girl.obedience() - girl.get_training(TRAINING_PUPPY);

    int enjoy = 0, training = 0, ob = 0;

    // `J` add in player's disposition so if the girl has heard of you
    int dispmod = 0;
    /* */if (g_Game->player().disposition() >= 100)    dispmod = 3;    // "Saint"
    else if (g_Game->player().disposition() >= 80)    dispmod = 2;    // "Benevolent"
    else if (g_Game->player().disposition() >= 50)    dispmod = 1;    // "Nice"
    else if (g_Game->player().disposition() >= 10)    dispmod = 0;    // "Pleasant"
    else if (g_Game->player().disposition() >= -10)    dispmod = 0;    // "Neutral"
    else if (g_Game->player().disposition() >= -50)    dispmod = -1;    // "Not nice"
    else if (g_Game->player().disposition() >= -80)    dispmod = -2;    // "Mean"
    else /*                                  */    dispmod = -3;    // "Evil"

    sImageSpec image_spec = girl.MakeImageSpec(EImageBaseType::PUPPYGIRL);

    const sGirl* headonduty = random_girl_on_job(*girl.m_Building, JOB_HEADGIRL, is_night);
    SetSubstitution("headgirl", headonduty ? "Head Girl " + headonduty->FullName() + "" : "the Head girl");

    // torturer can be in any brothel. need not be in house
    const sGirl* tortureronduty = random_girl_on_job(g_Game->buildings(), JOB_CLEANHOUSE, is_night);
    SetSubstitution("torturer", tortureronduty ? "Torturer " + tortureronduty->FullName() + "" : "the Torturer");

    const sGirl* recruiteronduty = random_girl_on_job(*girl.m_Building, JOB_RECRUITER, is_night);
    SetSubstitution("recruiter", recruiteronduty ? "Recruiter " + recruiteronduty->FullName() + "" : "the Recruiter");

    const sGirl* bedwarmeronduty = random_girl_on_job(*girl.m_Building, JOB_PERSONALBEDWARMER, is_night);
    SetSubstitution("bedwarmer", bedwarmeronduty ? "Bed warmer " + bedwarmeronduty->FullName() + "" : "the Bed warmer");

    const sGirl* cleaneronduty = random_girl_on_job(*girl.m_Building, JOB_CLEANHOUSE, is_night);
    SetSubstitution("cleaner", cleaneronduty ? "House cleaner " + cleaneronduty->FullName() + "" : "the house cleaner");

    const sGirl* traningonduty = random_girl_on_job(*girl.m_Building, JOB_PERSONALTRAINING, is_night);
    SetSubstitution("trainee", traningonduty ? traningonduty->FullName() : "the Personal trained");

    const sGirl* puppyonduty = random_girl_on_job(*girl.m_Building, JOB_HOUSEPET, is_night);
    SetSubstitution("puppy", puppyonduty ? "Puppy girl " + puppyonduty->FullName() + "" : "the Puppy girl");

    if (train >= 50)            // they refuse to train
    {
        ss << "${name} is still in training, and is having difficulty accepting her new role. Today she was a bad girl!\n";
        if (girl.get_training(TRAINING_PUPPY) >= 50)
        {
            if (roll_b >= 66)
            {
                if (dispmod >= 0)
                {
                    add_text("refused.skilled.nice");
                    training += 2;
                    ob += 2;
                }
                else
                {
                    add_text("refused.skilled.evil");
                    training += 4;
                    ob += 4;
                }
            }
            else if (roll_b >= 33)
            {
                add_text("refused.skilled.neutral");
                training += 2;
                ob += 2;
            }
            else
            {
                if (dispmod >= 0)//GOOD
                {
                    add_text("refused.skilled.nice");
                    training += 2;
                    ob += 2;
                }
                else//BAD
                {
                    add_text("refused.skilled.evil");
                    training += 3;
                    ob += 3;
                }
            }
        }
        else//low skill
        {
            if (dispmod >= 0)/*NICE DISP*/
            {
                add_text("refused.unskilled.nice");
                training += 2;
                ob += 2;
            }
            else if (dispmod < 0)/*BAD DISP*/
            {
                add_text("refused.unskilled.evil");
                training += 3;
                ob += 3;
            }
        }
    }
    else /*She did the training*/
    {
        ss << "${name} trained to be a house pet.\n \n";
        if (girl.get_training(TRAINING_PUPPY) >= 70)
        {
            if (girl.m_WeeksPreg >= 38 && headonduty) {
                add_text("train.skilled.preg");
            }else if (roll_b >= 90 && is_virgin(girl))
            {
                if (roll_c >= 85)
                {
                    if (dispmod >= 0)/*NICE DISP*/
                    {
                        add_text("train.skilled.virgin.nice");
                        training += 4;
                        ob += 4;
                        girl.lose_trait(traits::VIRGIN);
                        image_spec.BasicImage = EImageBaseType::VAGINAL;
                    }
                    else
                    {
                        add_text("train.virgin.evil");
                        training += 4;
                        ob += 4;
                        girl.lose_trait(traits::VIRGIN);
                        image_spec.BasicImage = EImageBaseType::BEAST;
                    }
                }
                else
                {
                    add_text("train.skilled.virgin.generic");
                    training += 4;
                    ob += 4;
                    image_spec.BasicImage = EImageBaseType::ORAL;
                    image_spec.Participants = ESexParticipants::HETERO;
                }
            }
            else if (roll_b >= 80 && puppyonduty)
            {
                add_text("train.puppies");
                training += 2;
            }
            else if (roll_b >= 60 && g_Game->has_building(BuildingType::STUDIO))
            {
                add_text("train.skilled.studio");
                if (brothel->is_sex_type_allowed(SKILL_BEASTIALITY) && !is_virgin(girl))
                {
                    ss << "Your bitch looked up, and began to crawl back over to you, but the mastiff had other ideas.";
                    ss << " ${name} suddenly yelped and began to let out a series of piercing, pleading barks as the mastiff mounted her from behind, and drove";
                    ss << " it's large red cock into her welcoming cunt. \"Should we do something?\" someone on stage asked, more out of fascination rather";
                    ss << " than actual desire. Almost certainly the cameras were still rolling at this point. \"No, he deserves his reward.\"";
                    ss << " you reply, smiling as the dog plowed heavily into ${name}, and her tits began to sway violent, her tongue panting like a good dog.";
                    girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                    girl.beastiality(2);
                    image_spec.BasicImage = EImageBaseType::BEAST;
                    if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
                    {
                        g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
                    }
                }
                else
                {
                    ss << "Happy with ${name} you promise her a reward later.";
                }
                training += 2;
            }
            else if (roll_b >= 40 && brothel->num_girls_on_job(JOB_HEADGIRL, false) >= 1)
            {
                add_text("train.skilled.headgirl");
                training += 2;
            }
            else
            {
                add_text("train.skilled.generic");
                training += 2;
            }
        }
        else if (girl.get_training(TRAINING_PUPPY) >= 50)
        {
            if (girl.m_WeeksPreg >= 38 && headonduty)
            {
                add_text("train.medium.preg");
            }
            else if (roll_b >= 85 && headonduty &&
                !girl.has_active_trait(traits::YOUR_DAUGHTER) && girl.has_active_trait(traits::KIDNAPPED))
            {
                add_text("train.medium.kidnapped");
            }
            else if (roll_b >= 70 && headonduty)
            {
                add_text("train.medium.head");
                training += 2;
            }else if (roll_b >= 60 && recruiteronduty)
            {
                add_text("train.medium.recruiter");
                training += 2;
            }
            else if (roll_b >= 45 && bedwarmeronduty)
            {
                add_text("train.medium.bedwarmer");
                training += 2;
            }
            else if (roll_b >= 25 && cleaneronduty)
            {
                add_text("train.medium.cleaner");
                training += 2;
            }
            else
            {
                add_text("train.medium.generic");
                training += 2;
            }
        }
        else/*LOW SKILL*/
        {
            ss << "You clip a leash to ${name}'s collar and she barks uneasily. She knows she's just your pet bitch, but she remembers when she wasn't. She hasn't quite embraced this life yet.\n";
            if (roll_b >= 66)
            {
                add_text("train.low.generic");
                training += 2;
            }
            else
            {
                if (girl.oralsex() < 35)
                {
                    add_text("train.low.bad-oral");
                    training += 2;
                    girl.oralsex(2);
                    image_spec.BasicImage = EImageBaseType::ORAL;
                    image_spec.Participants = ESexParticipants::HETERO;
                }
                else
                {
                    add_text("train.low.good-oral");
                    training += 2;
                    girl.oralsex(1);
                    image_spec.BasicImage = EImageBaseType::ORAL;
                    image_spec.Participants = ESexParticipants::HETERO;
                }
            }
        }
    }


#pragma endregion
#pragma region //    Job Performance            //


    //enjoyed the work or not
    int roll = d100();
    if (roll <= 5)
    {
        ss << "Some of the girls made fun of her for been a puppy during the shift.";
        enjoy -= 1;
    }
    else if (roll <= 25)
    {
        ss << "She had a pleasant time training.";
        enjoy += 3;
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += 1;
    }


    girl.AddMessage(ss.str(), image_spec, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    girl.exp(15);
    girl.upd_Enjoyment(actiontype, enjoy);
    girl.upd_Training(TRAINING_PUPPY, training);
    girl.obedience(ob);

    return sWorkJobResult{false, 0, 0, 0};
}

IGenericJob::eCheckWorkResult HousePet::CheckWork(sGirl& girl, bool is_night) {
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

PersonalTraining::PersonalTraining() : cBasicJob(JOB_PERSONALTRAINING, "PersonalTraining.xml") {
    m_Info.FullTime = true; // TODO for now, maybe later we can remove this and make it a per-shift singleton
}

sWorkJobResult PersonalTraining::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    add_text("work") << "\n\n";

    cGirls::UnequipCombat(girl);    // put that shit away, not needed for sex training

    // first set sex restrictions
    bool Allow_BEAST = brothel->is_sex_type_allowed(SKILL_BEASTIALITY);
    bool Allow_BDSM = brothel->is_sex_type_allowed(SKILL_BDSM);
    bool Allow_GROUP = brothel->is_sex_type_allowed(SKILL_GROUP);
    bool Allow_NORMALSEX = brothel->is_sex_type_allowed(SKILL_NORMALSEX);
    bool Allow_ANAL = brothel->is_sex_type_allowed(SKILL_ANAL);
    bool Allow_LESBIAN = brothel->is_sex_type_allowed(SKILL_LESBIAN);
    bool Allow_FOOTJOB = brothel->is_sex_type_allowed(SKILL_FOOTJOB);
    bool Allow_HANDJOB = brothel->is_sex_type_allowed(SKILL_HANDJOB);
    bool Allow_ORALSEX = brothel->is_sex_type_allowed(SKILL_ORALSEX);
    bool Allow_TITTYSEX = brothel->is_sex_type_allowed(SKILL_TITTYSEX);
    bool Allow_STRIP = brothel->is_sex_type_allowed(SKILL_STRIP);

    // if everything is banned do nothing
    if (!Allow_BEAST && !Allow_BDSM && !Allow_GROUP && !Allow_NORMALSEX && !Allow_ANAL && !Allow_LESBIAN &&
        !Allow_FOOTJOB && !Allow_HANDJOB && !Allow_ORALSEX && !Allow_TITTYSEX && !Allow_STRIP)
    {
        ss << "All sex is banned in this building so you just talk to her.";
        girl.pcfear(-uniform(-1, 2));
        girl.pclove(+uniform(-2, 4));
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
        return {false, 0, 0, 0};
    }

    enum {
        PT_BEAST,
        PT_BDSM,
        PT_GROUP,
        PT_NORMALSEX,
        PT_ANAL,
        PT_LESBIAN,
        PT_FOOTJOB,
        PT_HANDJOB,
        PT_ORALSEX,
        PT_TITTYSEX,
        PT_STRIP
    };



    double roll_a = d100();
    double roll_b = d100();

    int Disp = g_Game->player().disposition();

    int xp = uniform(10, 20);
    if (girl.has_active_trait(traits::QUICK_LEARNER))        { xp += 3; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { xp -= 3; }

    int skill = 0;
    if (roll_a <= 1)          skill = 14;
    else if (roll_a <= 3)     skill = 13;
    else if (roll_a <= 6)     skill = 12;
    else if (roll_a <= 10)    skill = 11;
    else if (roll_a <= 15)    skill = 10;
    else if (roll_a <= 21)    skill = 9;
    else if (roll_a <= 28)    skill = 8;
    else if (roll_a <= 37)    skill = 7;
    else if (roll_a <= 47)    skill = 6;
    else if (roll_a <= 58)    skill = 5;
    else if (roll_a <= 70)    skill = 4;
    else if (roll_a <= 83)    skill = 3;
    else /*             */    skill = 2;

    if (girl.has_active_trait(traits::QUICK_LEARNER))     skill += 1;
    else if (girl.has_active_trait(traits::SLOW_LEARNER)) skill -= 1;

    // roll_b random from 1-100 then is modified by player disposition and if is less than:
    // 7 strip | 15 les | 30 tit | 50 oral | 70 normal | 80 anal | 90 group | 100 bdsm | +beast
    // the nicer the player, the lower the roll, meaner is higher, only evil will do beast
    // will also skip down the list if the girl has 100 in the skill
    if (Disp >= 80)                //Benevolent
    {
        if (is_virgin(girl))        // 25% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\nShe ";
            if (roll_b <= 25)    { ss << "declines so "; roll_b *= 2; }
            else                { ss << "agrees so "; roll_b = 60; } // normal sex
        }
        else if (roll_b > 75) roll_b -= 8;
    }
    else if (Disp >= 50)            // nice
    {
        if (is_virgin(girl))        // 50 % decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\nShe ";
            if (roll_b <= 50)        { ss << "declines so "; }
            else                    { ss << "agrees so ";    roll_b = 60; } // normal sex
        }
        else if (roll_b > 90) roll_b -= 3;
    }
    else if (Disp > 10)            //Pleasant
    {
        if (is_virgin(girl))        // 70% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
            if (roll_b <= 50)        { ss << "She declines so "; }
            else if (roll_b <= 80)    { ss << "She agrees so ";            roll_b = 60; } // normal sex
            else                    { ss << "She declines so ";            roll_b = 75; } // anal sex
        }
        else if (roll_b > 95) roll_b *= 0.98;
    }
    else if (Disp >= -10)            // neutral
    {
        if (is_virgin(girl))        // 80% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
            if (roll_b <= 50)        { ss << "She declines so "; }
            else if (roll_b <= 70)    { ss << "She agrees so ";            roll_b = 60; } // normal sex
            else if (roll_b <= 90)    { ss << "She declines so ";            roll_b = 75; } // anal sex
            else                    { ss << "She refuses so instead ";    roll_b = 95; } // BDSM
        }
    }
    else if (Disp > -50)            // not nice
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 70)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 85)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else if (roll_b < 30) roll_b *= 2;
    }
    else if (Disp > -80)            //Mean
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 60)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 80)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else if (roll_b < 50) roll_b *= 2;
    }
    else                                            //Evil
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 50)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 70)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else if (roll_b <= 90)    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
            else                    { ss << "Wanting her to know she is just an animal to you, ";    roll_b = 105; } // beast
        }
        else if (roll_b < 70) roll_b *= 2;
    }


    if (roll_b <= 10 && girl.strip() < 100)
    {
        girl.strip(skill);
        ss << "You decide to have her strip for you.\n \n";
        ss << "She managed to gain " << skill << " Strip.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::STRIP, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 20 && girl.lesbian() < 100 && brothel->is_sex_type_allowed(SKILL_LESBIAN))
    {
        girl.lesbian(skill);
        ss << "You decide to bring in another girl for her.\n \n";
        ss << "She managed to gain " << skill << " Lesbian.\n \n";
        girl.AddMessage(ss.str(), EImagePresets::LESBIAN, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 27 && girl.tittysex() < 100 && brothel->is_sex_type_allowed(SKILL_TITTYSEX))
    {
        girl.tittysex(skill);
        ss << "You decide to have her use her tits on you.\n \n";
        ss << "She managed to gain " << skill << " Titty.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::TITTY, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 34 && girl.handjob() < 100 && brothel->is_sex_type_allowed(SKILL_HANDJOB))
    {
        girl.handjob(skill);
        ss << "You decide to teach her the art of manual stimulation.\n \n";
        ss << "She managed to gain " << skill << " Hand Job.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::HAND, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 40 && girl.footjob() < 100 && brothel->is_sex_type_allowed(SKILL_FOOTJOB))
    {
        girl.footjob(skill);
        ss << "You decide to teach her the art of manual stimulation with her feet.\n \n";
        ss << "She managed to gain " << skill << " Foot Job.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::FOOT, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 50 && girl.oralsex() < 100 && brothel->is_sex_type_allowed(SKILL_ORALSEX))
    {
        girl.oralsex(skill);
        ss << "You decide to teach her the art of sucking a cock.\n \n";
        ss << "She managed to gain " << skill << " Oral.\n \n";
        girl.AddMessage(ss.str(), EImagePresets::BLOWJOB, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 70 && girl.normalsex() < 100 && brothel->is_sex_type_allowed(SKILL_NORMALSEX))
    {
        girl.normalsex(skill);
        ss << "You decide to teach her how to ride a dick like a pro.\n \n";
        ss << "She managed to gain " << skill << " Normal Sex.\n \n";
        if (girl.lose_trait(traits::VIRGIN))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), EImageBaseType::VAGINAL, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (roll_b <= 80 && girl.anal() < 100 && brothel->is_sex_type_allowed(SKILL_ANAL))
    {
        girl.anal(skill);
        ss << "You decide to teach her how to use her ass.\n \n";
        ss << "She managed to gain " << skill << " Anal Sex.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::ANAL, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else if (roll_b <= 90 && girl.group() < 100 && brothel->is_sex_type_allowed(SKILL_GROUP))
    {
        girl.group(skill);
        ss << "You decide to over see her skill in a gang bang.\n \n";
        ss << "She managed to gain " << skill << " Group Sex.\n \n";
        auto spec = girl.MakeImageSpec(EImagePresets::GANGBANG);
        if (girl.lose_trait(traits::VIRGIN))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), spec, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        // TODO chance to get pregnant by non-player!
        // TODO if we remove the virgin trait before the pregnancy calculation, it cannot affect preg chance!
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (roll_b <= 100 && girl.bdsm() < 100 && brothel->is_sex_type_allowed(SKILL_BDSM))
    {
        girl.bdsm(skill);
        ss << "You decide to teach her the fine art of BDSM.\n \n";
        ss << "She managed to gain " << skill << " BDSM.\n \n";
        if (girl.lose_trait(traits::VIRGIN))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), EImageBaseType::BDSM, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (girl.beastiality() < 100 && brothel->is_sex_type_allowed(SKILL_BEASTIALITY))
    {
        girl.beastiality(skill);
        ss << "You decide to have her get acquainted with some animals.\n \n";
        ss << "She managed to gain " << skill << " Beastiality.\n \n";
        if (girl.lose_trait(traits::VIRGIN))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), EImageBaseType::BEAST, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }
    else
    {
        girl.strip(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_HANDJOB))        girl.handjob(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_ORALSEX))        girl.oralsex(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_TITTYSEX))        girl.tittysex(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_LESBIAN))        girl.lesbian(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_NORMALSEX))        girl.normalsex(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_ANAL))            girl.anal(uniform(0, 2));
        if (brothel->is_sex_type_allowed(SKILL_BDSM))            girl.bdsm(uniform(0, 2));
        ss << "You couldn't decide what to teach her so you just fooled around with her.\n \n";
        girl.AddMessage(ss.str(), EImageBaseType::ECCHI, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }

    int pay = 0;
    if (girl.is_slave())
    {
        ss << "\nYou own her so you don't have to pay her.";
    }
    else
    {
        pay = 50;
    }


    // Improve stats
    girl.exp(xp);

    return sWorkJobResult{false, 0, 0, pay};
}

IGenericJob::eCheckWorkResult PersonalTraining::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_SEX);
}

double PersonalTraining::GetPerformance(const sGirl& girl, bool estimate) const {
    if (estimate) {
        return (100 - cGirls::GetAverageOfSexSkills(girl)) * 3;
    }
    return 0;
}

Recruiter::Recruiter() : cSimpleJob(JOB_RECRUITER, "Recruiter.xml", {ACTION_WORKRECRUIT, 100, EImageBaseType::PROFILE}) {
    m_Info.FullTime = true;
    m_Info.FreeOnly = true;
}

bool Recruiter::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int fame = 0;

    int HateLove = girl.pclove();

    if (HateLove < -80)         ss << "She hates you more then anything so she doesn't try that hard.";
    else if (HateLove < -60)    ss << "She hates you.";
    else if (HateLove < -40)    ss << "She doesn't like you.";
    else if (HateLove < -20)    ss << "She finds you to be annoying.";
    else if (HateLove <   0)    ss << "She finds you to be annoying.";
    else if (HateLove <  20)    ss << "She finds you to be decent.";
    else if (HateLove <  40)    ss << "She finds you to be a good person.";
    else if (HateLove <  60)    ss << "She finds you to be a good person.";
    else if (HateLove <  80)    ss << "She has really strong feelings for you so she trys really hard for you.";
    else                        ss << "She loves you more then anything so she gives it her all.";
    ss << "\n \n";

    add_performance_text();
    int findchance = performance_based_lookup(4, 8, 10, 12, 15, 20);

    // `J` add in player's disposition so if the girl has heard of you
    int dispmod = 0;
    /* */if (g_Game->player().disposition() >= 100)    dispmod = 3;    // "Saint"
    else if (g_Game->player().disposition() >= 80)    dispmod = 2;    // "Benevolent"
    else if (g_Game->player().disposition() >= 50)    dispmod = 1;    // "Nice"
    else if (g_Game->player().disposition() >= 10)    dispmod = 0;    // "Pleasant"
    else if (g_Game->player().disposition() >= -10)    dispmod = 0;    // "Neutral"
    else if (g_Game->player().disposition() >= -50)    dispmod = -1;    // "Not nice"
    else if (g_Game->player().disposition() >= -80)    dispmod = -2;    // "Mean"
    else /*                                  */    dispmod = -3;    // "Evil"

    int findroll = (d100());
    if (findroll < findchance + 10)    // `J` While out recruiting she does find someone...
    {
        std::shared_ptr<sGirl> newgirl = g_Game->CreateRandomGirl(SpawnReason::RECRUITED);
        if (newgirl)
        {
            bool add = false;
            ss << "She finds a girl, ";
            ss << newgirl->FullName();
            if (findroll < findchance - 5)
            {        // `J` ... and your disposition did not come up.
                add = true;
                ss << " and convinces her that she should work for you.";
            }
            else if (findroll < findchance + 5)    // `J` ... and your disposition did come up...
            {
                if (findroll < findchance + dispmod)    // `J` ... and she was recruited
                {
                    add = true;
                    if (dispmod > 0)
                    {
                        ss << "\nYour nice reputation has helped you today as she agrees to come work for you.";
                    }
                    else if (dispmod < 0)
                    {
                        ss << "\nScared of what you might to do to her if you don't come work for her she agrees to taking the job.";
                    }
                    else
                    {
                        ss << "\nThe fact that your neither good nor evil seems to have helped you today.  As the girl doesn't think your evil nor a \"pussy\" and comes to work for you.";
                    }
                    if (dispmod == 3)
                    {
                        if (chance(25))    newgirl->gain_trait(traits::OPTIMIST);
                    }
                    if (dispmod == -3)
                    {
                        int rollt = uniform(0, 3);
                        if (rollt == 0)    newgirl->raw_traits().add_inherent_trait(traits::DEMON);
                        if (rollt == 1)    newgirl->gain_trait(traits::FEARLESS);
                    }
                }
                else    // `J` ... and she was not recruited
                {

                    if (dispmod > 0)
                    {
                        ss << "\nBeing so nice she is worried you will get yourself killed and someone else will take over that she won't like.  So she won't come work for you.";
                    }
                    else if (dispmod < 0)
                    {
                        ss << "\nShe fears you to much to come and work for you.";
                    }
                    else
                    {
                        ss << "\nAs you are on the fence with your reputation she doesn't know what to think about you and won't work for you.";
                    }
                }
            }
            else    // `J` ... She was not recruited.
            {
                ss << " but was unable to convince her that she should work for you.";
            }
            if (add)
            {
                newgirl->house(60);
                std::stringstream NGmsg;
                NGmsg << "${name} was recruited by " << girl.FullName() << " to work for you.";
                newgirl->AddMessage(NGmsg.str(), m_ImageType, EVENT_GANG);

                g_Game->dungeon().AddGirl(std::move(newgirl), DUNGEON_RECRUITED);
            }
        }
        else
        {
            ss << "But was unable to find anyone to join.";
        }
    }
    else
    {
        ss << "But was unable to find anyone to join.";
    }

    ss << "\n \n";

    shift_enjoyment();

    girl.AddMessage(ss.str(), m_ImageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    int roll_max = (girl.charisma() + girl.service()) / 4;
    m_Wages += 10 + uniform(0, roll_max);

    // Improve stats
    HandleGains(girl, fame);

    return false;
}

void RegisterHouseJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<HouseCook>());
    mgr.register_job(std::make_unique<HousePet>());
    mgr.register_job(std::make_unique<PersonalTraining>());
    mgr.register_job(std::make_unique<Recruiter>());
}