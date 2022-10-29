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
#include "cGirls.h"
#include "buildings/IBuilding.h"
#include "utils/streaming_random_selection.hpp"
#include "IGame.h"

extern const char* const TrainingInteractionId;

namespace {
    struct sTrainingData {
        const char* TargetTrait;
        const char* HasAlreadyMessage;
        const char* ProceedMessage;
        const char* ResistMessage;
        const char* ProgressMessage;
        sImagePreset ImageType;
    };

    class PracticeJob : public cBasicJob {
    public:
        PracticeJob();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    class MistressJob : public cBasicJob {
    public:
        MistressJob();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };
}

MistressJob::MistressJob() : cBasicJob(JOB_MISTRESS, "Mistress.xml") {
    m_Info.FreeOnly = true;
    m_Info.Provides.emplace_back(TrainingInteractionId);
}

sWorkJobResult MistressJob::DoWork(sGirl& girl, bool is_night) {
    if (m_Performance > 150) {
        ProvideInteraction(TrainingInteractionId, 3);
        add_text("work.good");
    } else {
        add_text("work.normal");
        ProvideInteraction(TrainingInteractionId, 2);
    }

    apply_gains(girl, m_Performance);

    if(m_Performance > 100) {
        auto* target_girl = girl.m_Building->girls().get_random_girl([](const sGirl& g) {
            return g.is_slave() && g.obedience() < 50;
        });
        if (target_girl) {
            SetSubstitution("slacker", target_girl->FullName());
            add_text("slacker");
            target_girl->upd_temp_stat(STAT_OBEDIENCE, 4);
            target_girl->upd_temp_stat(STAT_PCFEAR, 2);
        }
    }

    girl.AddMessage(ss.str(), EImageBaseType::TEACHER, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    return {false, 0, 0, 100};
}

IGenericJob::eCheckWorkResult MistressJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKMATRON);
}

PracticeJob::PracticeJob() : cBasicJob(JOB_TRAINING, "Training.xml") {
    m_Info.Consumes.emplace_back(TrainingInteractionId);
}

namespace {
    sImagePreset skill_to_image(SKILLS skill) {
        switch (skill) {
            case SKILL_STRIP:
                return EImageBaseType::STRIP;
            case SKILL_GROUP:
                return EImagePresets::GROUP;
            case SKILL_BDSM:
                return EImageBaseType::BDSM;
            case SKILL_PERFORMANCE:
                return EImagePresets::MASTURBATE;
            case SKILL_ANAL:
                return EImageBaseType::ANAL;
            case SKILL_NORMALSEX:
                return EImageBaseType::VAGINAL;
            case SKILL_ORALSEX:
                return EImagePresets::BLOWJOB;
            case SKILL_TITTYSEX:
                return EImageBaseType::TITTY;
            case SKILL_LESBIAN:
                return EImagePresets::LESBIAN;
            case SKILL_HANDJOB:
                return EImageBaseType::HAND;
            case SKILL_FOOTJOB:
                return EImageBaseType::FOOT;
            case SKILL_BEASTIALITY:
                return EImageBaseType::BEAST;
            default:
                return EImageBaseType::PROFILE;
        }
    }
}


sWorkJobResult PracticeJob::DoWork(sGirl& girl, bool is_night) {
    auto building = girl.m_Building;
    sGirl* mistress = RequestInteraction(TrainingInteractionId);
    SetSubstitution("mistress", mistress ? mistress->FullName() : "The Mistress");
    girl.tiredness(2);

    if(!mistress) {
        add_text("no-mistress");
        RandomSelector<SKILLS> selector;
        SKILLS skills[] = {SKILL_NORMALSEX, SKILL_ANAL, SKILL_FOOTJOB, SKILL_HANDJOB, SKILL_LESBIAN,
                           SKILL_TITTYSEX, SKILL_ORALSEX, SKILL_PERFORMANCE, SKILL_STRIP};
        for(auto& skill : skills) {
            if(girl.get_skill(skill) < 50 && girl.m_Building->is_sex_type_allowed(skill)) {
                selector.process(&skill, 50 - girl.get_skill(skill));
            }
        }

        girl.exp(5);
        sImagePreset image = EImagePresets::MASTURBATE;
        if(selector.selection()) {
            auto target = *selector.selection();
            image = skill_to_image(target);
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                ss << "\n" << get_skill_name(target) << " + 1";
            }
        }

        girl.AddMessage(ss.str(), image, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        return sWorkJobResult{false, 0, 0, 20};
    } else {
        mistress->tiredness(2);

        // The mistress trains her. First, settle on a skill to train
        RandomSelector<SKILLS> selector;
        SKILLS skills[] = {SKILL_NORMALSEX, SKILL_ANAL, SKILL_BDSM, SKILL_BEASTIALITY, SKILL_FOOTJOB,
                           SKILL_HANDJOB, SKILL_LESBIAN, SKILL_TITTYSEX, SKILL_ORALSEX, SKILL_GROUP,
                           SKILL_PERFORMANCE, SKILL_STRIP};

        for(auto& skill : skills) {
            if(!girl.m_Building->is_sex_type_allowed(skill))
                continue;

            int my_value = girl.get_skill(skill);
            int other_value = mistress->get_skill(skill);
            float weight = 5;       // base chance
            // is it important to train that value
            if(my_value < 20)
                weight += 5;
            if(my_value < 50)
                weight += 5;

            // can I train that value well
            if(my_value < other_value) {
                weight += float(other_value - my_value) / 2;
            }

            // don't train values close to skill cap
            int cap = g_Game->get_skill_cap(skill, girl);
            if(my_value >= cap - 5) {
                weight = 2;
            }

            selector.process(&skill, weight);
        }

        // this will always be non NULL, because SKILL_PERFORMANCE cannot be forbidden
        SKILLS target = *selector.selection();
        sImagePreset image = skill_to_image(target);

        SetSubstitution("skill", get_skill_name(target));
        int my_value = girl.get_skill(target);
        int other_value = mistress->get_skill(target);
        if(my_value >= other_value) {
            add_text("lack-of-skill");
            girl.exp(5);
            // if total skill < 50, we still get some update
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                ss << "\n" << get_skill_name(target) << " + 1";
            }
            if(chance(5)) {
                mistress->upd_skill(target, 1);
            }
        } else {
            add_text(std::string("train.") + get_skill_name(target));
            girl.exp(5);
            int perf = mistress->job_performance(JOB_MISTRESS, false);
            int min_gain = 1;
            int max_gain = 3;
            if(my_value < 20 && perf > 75) {
                min_gain += 1;
            }
            if(other_value > 66 && my_value < 40 && perf > 100) {
                min_gain += 1;
            }

            if(my_value < 40 && perf > 140) {
                max_gain += 1;
            }

            if (girl.has_active_trait(traits::QUICK_LEARNER))     max_gain += 1;
            else if (girl.has_active_trait(traits::SLOW_LEARNER)) max_gain -= 1;

            max_gain += std::min((other_value - my_value) / 10, 3);
            int amount = uniform(min_gain, std::max(max_gain, min_gain));
            girl.upd_skill(target, amount);
            ss << "\n${name}'s " << get_skill_name(target) << " skill increased by " << amount << " points.";
        }

        girl.AddMessage(ss.str(), image, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        return sWorkJobResult{false, 0, 0, 20};
    }
}

IGenericJob::eCheckWorkResult PracticeJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(ACTION_WORKTRAINING, JOB_TRAINING))            // they refuse to work
    {

        sGirl* mistress = nullptr;
        if(girl.is_slave() && (mistress = RequestInteraction(TrainingInteractionId))) {
            SetSubstitution("mistress", mistress->FullName() );
            add_text("refuse.forced");
            // smaller changes than for dungeon torture, but still we should combine the code at some point
            girl.obedience(2);
            girl.upd_temp_stat(STAT_OBEDIENCE, 5);
            girl.pcfear(1);
            girl.pclove(-1);
            girl.spirit(-1);
            girl.happiness(-2);
            girl.tiredness(2);
            girl.health(-2);

            girl.AddMessage(ss.str(), EImageBaseType::TORTURE, EVENT_NOWORK);
        } else {
            add_text("refuse");
            girl.AddMessage(ss.str(), EImageBaseType::REFUSE, EVENT_NOWORK);
        }

        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

class TrainingJob : public IGenericJob {
public:
    TrainingJob(JOBS job, const char* brief, sTrainingData dat) : IGenericJob(job), m_Data(dat) {
        m_Info.FullTime = true;
        m_Info.ShortName = brief;
        m_Info.Consumes.emplace_back(TrainingInteractionId);
    }

    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;

    virtual void HandleTraining(sGirl& girl, bool is_night) = 0;
    virtual void OnComplete(sGirl& girl) = 0;
    virtual void OnNoProgress(sGirl& girl);
    virtual void OnRegularProgress(sGirl& girl, bool is_night);

protected:
    void update_progress(sGirl& girl);
    void CountTheDays(sGirl& girl, bool is_night);

    EventType TypeOfEvent;
    int Tiredness;
    int Enjoyment;
    int Wages;

    sTrainingData m_Data;

private:
    int m_StartProgress = 0;

    sGirl* m_Mistress = nullptr;
};

void TrainingJob::update_progress(sGirl& girl) {
    girl.m_WorkingDay += uniform(10, 20);
    girl.m_WorkingDay += girl.obedience() / 20;
    if (girl.pcfear() > 50)                girl.m_WorkingDay +=uniform(0, girl.pcfear() / 20);        // She will do as she is told
    if (girl.pclove() > 50)                girl.m_WorkingDay += uniform(0, girl.pclove() / 20);        // She will do what you ask

    // Negative Stats/Skills
    girl.m_WorkingDay -= girl.spirit() / 25;
    if (girl.pclove() < -30)               girl.m_WorkingDay -= uniform(0, -girl.pclove() / 10);        // She will not do what you want
    if (girl.happiness() < 50)             girl.m_WorkingDay -= uniform(1, 5);                        // She is not feeling like it
    if (girl.health() < 50)                girl.m_WorkingDay -= uniform(1, 5);                        // She is feeling sick
    if (girl.tiredness() > 50)             girl.m_WorkingDay -= uniform(1, 5);                        // She is tired
}

sWorkJobResult TrainingJob::DoWork(sGirl& girl, bool is_night) {
    // TODO pull this up into generic job?
    if (girl.m_YesterDayJob != job()) girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
    if (girl.m_WorkingDay < 0) girl.m_WorkingDay = 0;

    // Base adjustment
    Wages = 0;
    Tiredness = uniform(5, 15);
    m_StartProgress = girl.m_WorkingDay;

    cGirls::UnequipCombat(girl);

    ss << m_Data.ProceedMessage << "\n";

    TypeOfEvent = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    HandleTraining(girl, is_night);

    girl.upd_Enjoyment(ACTION_WORKTRAINING, Enjoyment);
    girl.tiredness(Tiredness);

    girl.AddMessage(ss.str(), m_Data.ImageType, TypeOfEvent);

    return {false, 0, 0, Wages};
}

double TrainingJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait(m_Data.TargetTrait))    return -1000;
    return 250;
}

IGenericJob::eCheckWorkResult TrainingJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.has_active_trait(m_Data.TargetTrait))
    {
        girl.AddMessage(m_Data.HasAlreadyMessage, EImageBaseType::PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    m_Mistress = RequestInteraction(TrainingInteractionId);
    if(!m_Mistress) {
        ss << "There is no Mistress available to train ${name}\n";
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        return eCheckWorkResult::IMPOSSIBLE;
    }

    return eCheckWorkResult::ACCEPTS;
}

void TrainingJob::CountTheDays(sGirl& girl, bool is_night)
{
    if (girl.disobey_check(ACTION_WORKTRAINING, job())) girl.m_WorkingDay /= 2;    // if she disobeys, half her time is wasted

    int total = girl.m_WorkingDay - m_StartProgress;
    if (total <= 0)                                // she lost time so more tired
    {
        Tiredness += uniform(5, 5-total);
        Enjoyment -= uniform(0, 2);
    }
    else if (total > 40)                        // or if she trained a lot
    {
        Tiredness += uniform(total / 4, total / 2);
        Enjoyment += uniform(0, 2);
    }
    else                                        // otherwise just a bit tired
    {
        Tiredness += uniform(0, total / 3);
        Enjoyment -= uniform(-2, 2);
    }

    if (girl.m_WorkingDay <= 0)
    {
        girl.m_WorkingDay = 0;
        TypeOfEvent = EVENT_WARNING;
        OnNoProgress(girl);
    }
    else if (girl.m_WorkingDay >= 100 && is_night)
    {
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        TypeOfEvent = EVENT_GOODNEWS;
        OnComplete(girl);
        girl.FullJobReset(JOB_RESTING);
        Wages = 200;
    }
    else
    {
        OnRegularProgress(girl, is_night);
        Wages = std::min(100, girl.m_WorkingDay);
    }

    if (girl.is_slave()) Wages /= 2;
}

void TrainingJob::OnNoProgress(sGirl& girl) {
    ss << m_Data.ResistMessage << "\n";
    Tiredness += uniform(5, 15);
}

void TrainingJob::OnRegularProgress(sGirl& girl, bool is_night) {
    ss << m_Data.ProgressMessage;
    if (girl.m_WorkingDay >= 100)
    {
        ss << "almost complete.";
        Tiredness -= (girl.m_WorkingDay - 100) / 2;    // her last day so she rested a bit
    }
    else ss << "in progress (" << girl.m_WorkingDay << "%).";
}

class SoStraight : public TrainingJob {
public:
    SoStraight() : TrainingJob(JOB_SO_STRAIGHT, "SOSt", {
            traits::STRAIGHT, "${name} is already Straight.",
            "You proceed to change ${name}'s sexual orientation to Straight.",
            "She resisted all attempts to make her Straight.",
            "Her Sexual Orientation conversion to Straight is ",
            EImageBaseType::VAGINAL}) {
        m_Info.Description = "You will make sure she only likes having sex with men.";
    }
    void HandleTraining(sGirl& girl, bool is_night) override;
    void OnComplete(sGirl& girl) override;
};

void SoStraight::HandleTraining(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    // Positive Stats/Skills
    girl.m_WorkingDay += girl.normalsex() / 5;
    girl.m_WorkingDay += girl.group() / 10;
    girl.m_WorkingDay += girl.oralsex() / 20;
    girl.m_WorkingDay += girl.tittysex() / 20;
    girl.m_WorkingDay += girl.anal() / 20;

    update_progress(girl);

    if (girl.has_active_trait(traits::LESBIAN))
    {
        ss << "Her innate disgust of balls and shaft made her pull away from you while trying to teach her to suck it.\n";
        girl.m_WorkingDay -= girl.lesbian() / 5;                    // it is hard to change something you are good at
        Tiredness += girl.lesbian() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) girl.m_WorkingDay -= girl.lesbian() / 20;    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier("so.straight");
    girl.m_WorkingDay += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))        { ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }

    if (!brothel->is_sex_type_allowed(SKILL_NORMALSEX))      girl.m_WorkingDay -= uniform(10, 30);

    CountTheDays(girl, is_night);
    int xp = 1 + std::max(0, girl.m_WorkingDay / 20);

    // Improve girl
    int I_lesbian = uniform(-15, -2);
    int I_normalsex = uniform(2, 15);
    int I_group = uniform(1, 5);
    int I_anal = uniform(1, 5);
    int I_oralsex = uniform(1, 5);
    int I_handjob = uniform(1, 5);
    int I_tittysex = uniform(1, 5);

    girl.exp(xp);
    girl.lesbian(I_lesbian);
    girl.normalsex(I_normalsex);
    girl.group(I_group);
    girl.anal(I_anal);
    girl.oralsex(I_oralsex);
    girl.handjob(I_handjob);
    girl.tittysex(I_tittysex);
}

void SoStraight::OnComplete(sGirl& girl) {
    ss << "Her Sexual Orientation conversion is complete. She is now Straight.\n";
    girl.lose_trait(traits::LESBIAN);    girl.gain_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

class SoLesbian : public TrainingJob {
public:
    SoLesbian() : TrainingJob(JOB_SO_LESBIAN, "SOLe",
                              {traits::LESBIAN, "${name} is already a Lesbian.",
                               "You proceed to change ${name}'s sexual orientation to Lesbian.",
                               "She resisted all attempts to make her a Lesbian.",
                               "Her Sexual Orientation conversion to Lesbian is ",
                               EImageBaseType::VAGINAL}) {
        m_Info.Description = "You will make sure she only likes having sex with women.";
    }
    void HandleTraining(sGirl& girl, bool is_night) override;
    void OnComplete(sGirl& girl) override;
};

void SoLesbian::HandleTraining(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    // Positive Stats/Skills
    girl.m_WorkingDay += girl.lesbian() / 5;
    girl.m_WorkingDay += girl.group() / 20;
    girl.m_WorkingDay += girl.oralsex() / 25;
    update_progress(girl);

    if (girl.has_active_trait(traits::STRAIGHT))
    {
        ss << "Being used to working with something long and hard, she wasn't really sure what she was doing with her partner.\n";
        girl.m_WorkingDay -= girl.normalsex() / 5;                // it is hard to change something you are good at
        Tiredness += girl.normalsex() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) girl.m_WorkingDay -= girl.normalsex() / 20;                    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier("so.lesbian");
    girl.m_WorkingDay += uniform(trait / 2, trait + trait / 2);

    if (girl.has_active_trait(traits::BROKEN_WILL))    { ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }

    //    if (girl.check_virginity())                {}

    if (!brothel->is_sex_type_allowed(SKILL_LESBIAN))        girl.m_WorkingDay -= uniform(10, 30);

    int xp = 1 + std::max(0, girl.m_WorkingDay / 20);
    CountTheDays(girl, is_night);

    // Improve girl
    int I_lesbian = uniform(2, 15);
    int I_normalsex = uniform(-2, -15);
    int I_group = uniform(1, -3);
    int I_anal = uniform(-1, -5);
    int I_oralsex = uniform(1, -3);
    int I_handjob = uniform(1, -3);
    int I_tittysex = uniform(-1, -5);

    girl.exp(xp);
    girl.lesbian(I_lesbian);
    girl.normalsex(I_normalsex);
    girl.group(I_group);
    girl.anal(I_anal);
    girl.oralsex(I_oralsex);
    girl.handjob(I_handjob);
    girl.tittysex(I_tittysex);
}

void SoLesbian::OnComplete(sGirl& girl) {
    ss << "\nHer Sexual Orientation conversion is complete. She is now a Lesbian.";
    girl.gain_trait(traits::LESBIAN);    girl.lose_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

class SoBi : public TrainingJob {
public:
    SoBi() : TrainingJob(JOB_SO_BISEXUAL, "SOBi",
                              {traits::BISEXUAL, "${name} is already Bisexual.",
                               "You proceed to change ${name}'s sexual orientation to Bisexual.",
                               "She resisted all attempts to make her Bisexual.",
                               "Her Sexual Orientation conversion to Bisexual is ",
                               EImagePresets::LESBIAN}) {
        m_Info.Description = "You will make sure she likes having sex with both men and women.";
    }
    void HandleTraining(sGirl& girl, bool is_night) override;
    void OnComplete(sGirl& girl) override;
};

void SoBi::HandleTraining(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (girl.has_active_trait(traits::STRAIGHT))
    {
        girl.m_WorkingDay += girl.group() / 10;
        girl.m_WorkingDay += girl.normalsex() / 20;
        girl.m_WorkingDay += girl.lesbian() / 5;
        girl.m_WorkingDay += girl.oralsex() / 20;
        girl.m_WorkingDay += girl.tittysex() / 20;
        girl.m_WorkingDay += girl.anal() / 20;
    }
    else if (girl.has_active_trait(traits::LESBIAN))
    {
        girl.m_WorkingDay += girl.group() / 10;
        girl.m_WorkingDay += girl.normalsex() / 5;
        girl.m_WorkingDay += girl.lesbian() / 20;
        girl.m_WorkingDay += girl.oralsex() / 15;
        girl.m_WorkingDay += girl.tittysex() / 15;
        girl.m_WorkingDay += girl.anal() / 15;
    }
    else
    {
        girl.m_WorkingDay += girl.group() / 5;
        girl.m_WorkingDay += girl.normalsex() / 10;
        girl.m_WorkingDay += girl.lesbian() / 10;
        girl.m_WorkingDay += girl.oralsex() / 20;
        girl.m_WorkingDay += girl.tittysex() / 20;
        girl.m_WorkingDay += girl.anal() / 20;
    }
    update_progress(girl);

    int trait = girl.get_trait_modifier("so.bi");
    girl.m_WorkingDay += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    { ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }

    if (!brothel->is_sex_type_allowed(SKILL_LESBIAN))        girl.m_WorkingDay -= uniform(5, 15);
    if (!brothel->is_sex_type_allowed(SKILL_NORMALSEX))      girl.m_WorkingDay -= uniform(5, 15);
    CountTheDays(girl, is_night);
    int xp = 1 + std::max(0, girl.m_WorkingDay / 20);
    int I_lesbian = uniform(1, 10);
    int I_normalsex = uniform(1, 10);
    int I_group = uniform(2, 15);
    int I_anal = uniform(0, 5);
    int I_oralsex = uniform(0, 5);
    int I_handjob = uniform(0, 5);
    int I_tittysex = uniform(0, 3);

    girl.exp(xp);
    girl.lesbian(I_lesbian);
    girl.normalsex(I_normalsex);
    girl.group(I_group);
    girl.anal(I_anal);
    girl.oralsex(I_oralsex);
    girl.handjob(I_handjob);
    girl.tittysex(I_tittysex);
}

void SoBi::OnComplete(sGirl& girl) {
    ss << "\nHer Sexual Orientation conversion is complete. She is now Bisexual.";
    girl.lose_trait(traits::LESBIAN);    girl.gain_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

class FakeOrg : public TrainingJob {
public:
    FakeOrg() : TrainingJob(JOB_FAKEORGASM, "FOEx",
                         {traits::FAKE_ORGASM_EXPERT, "${name} is already a \"Fake Orgasm Expert\".",
                          "You teach ${name} how to fake her orgasms.",
                          "She resisted all attempts to make her Bisexual.",
                          "Her Sexual Orientation conversion to Bisexual is ",
                          EImagePresets::MASTURBATE}) {
        m_Info.Description = "You will teach her how to fake her orgasms.";
    }
    void HandleTraining(sGirl& girl, bool is_night) override;
    void OnComplete(sGirl& girl) override;
    void OnNoProgress(sGirl& girl) override;
    void OnRegularProgress(sGirl& girl, bool is_night) override;
};

void FakeOrg::HandleTraining(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    // Positive Stats/Skills
    girl.m_WorkingDay += girl.performance() / 5;
    girl.m_WorkingDay += girl.group() / 20;
    girl.m_WorkingDay += girl.normalsex() / 20;
    girl.m_WorkingDay += girl.lesbian() / 20;

    update_progress(girl);
    int trait = girl.get_trait_modifier("fake-orgasm");
    girl.m_WorkingDay += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    { ss << "She just sits there doing exactly what you tell her to do, You don't think it is really getting through to her.\n"; }

    if (!brothel->is_sex_type_allowed(SKILL_NORMALSEX))      girl.m_WorkingDay -= uniform(5, 15);

    CountTheDays(girl, is_night);

    // Improve girl
    int xp = 1 + std::max(0, girl.m_WorkingDay / 20);
    int I_performance = uniform(3, 15);
    int I_confidence = uniform(-1, 5);
    int I_constitution = std::max(0, uniform(-2, 1));
    int I_spirit = uniform(-5, 5);
    int I_lesbian = uniform(0, 5);
    int I_normalsex = uniform(0, 5);
    int I_group = uniform(0, 5);
    int I_anal = std::max(0, uniform(-2, 2));

    girl.exp(xp);
    girl.lesbian(I_lesbian);
    girl.normalsex(I_normalsex);
    girl.group(I_group);
    girl.anal(I_anal);
    girl.performance(I_performance);
    girl.confidence(I_confidence);
    girl.constitution(I_constitution);
    girl.spirit(I_spirit);
}

void FakeOrg::OnNoProgress(sGirl& girl) {
    if (girl.any_active_trait({traits::SLOW_LEARNER, traits::BROKEN_WILL, traits::MIND_FUCKED, traits::RETARDED}))
    {
        ss << "She was not mentally able to learn";
    }
    else if (girl.any_active_trait({traits::BIMBO, traits::FAST_ORGASMS, traits::NYMPHOMANIAC}))
    {
        ss << "She was too focused on the sex to learn";
        Tiredness += uniform(5, 15);
    }
    else if (girl.any_active_trait({traits::BLIND, traits::DEAF}))
    {
        ss << "Her handicap kept her from learning";
    }
    else
    {
        ss << "She resisted all attempts to teach her";
        Tiredness += uniform(5, 15);
    }
    ss << " to fake her orgasms.";
}

void FakeOrg::OnComplete(sGirl& girl) {
    ss << "With her training complete, she is now a \"Fake Orgasm Expert\".";
    girl.lose_trait(traits::SLOW_ORGASMS);    girl.lose_trait(traits::FAST_ORGASMS);    girl.gain_trait(traits::FAKE_ORGASM_EXPERT);
}

void FakeOrg::OnRegularProgress(sGirl& girl, bool is_night) {
    if (girl.m_WorkingDay >= 100)        Tiredness -= (girl.m_WorkingDay - 100) / 2;    // her last day so she rested a bit
    else    ss << "Training in progress (" << girl.m_WorkingDay << "%).\n \n";
    if (girl.m_WorkingDay < 25)      ss << "She has no idea what she sounds like during sex but it ain't orgasmic.";
    else if (girl.m_WorkingDay < 50) ss << "When she realizes she should finish, you can see it click in her mind and easily notice her changing things up.";
    else if (girl.m_WorkingDay < 75) ss << "She is still not getting into rhythm with " << (chance(33) ? "you" : "her partner") << " but it still seems enjoyable.";
    else                             ss << "She is almost there but you want her to practice a little more to get it perfect.";

    if (!is_night)
    {
        ss << "\nYou tell her to take a break for lunch and ";
        if (girl.m_WorkingDay < 50)      ss << "clear her mind, she has a lot more work to do.";
        else if (girl.m_WorkingDay < 75) ss << "relax, she has a bit more training to do.";
        else                             ss << "see if she can make anyone say \"I'll have what she's having\".";
    }
    else
    {
        ss << "\nThats all for tonight, ";
        if (girl.m_WorkingDay < 50)    ss << "we have a lot more to do tomorrow (and probably the next few weeks).";
        else                           ss << "we'll pick things up in the morning.";
    }
}

void RegisterTrainingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<SoStraight>());
    mgr.register_job(std::make_unique<SoLesbian>());
    mgr.register_job(std::make_unique<SoBi>());
    mgr.register_job(std::make_unique<FakeOrg>());
    mgr.register_job(std::make_unique<PracticeJob>());
    mgr.register_job(std::make_unique<MistressJob>());
}