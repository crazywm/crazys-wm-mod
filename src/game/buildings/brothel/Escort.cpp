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

#include "BrothelJobs.h"
#include <character/predicates.h>
#include "jobs/BasicJob.h"
#include "character/sGirl.h"
#include "cGirls.h"

namespace {
    int skill_to_mod(int skill) {
        if (skill >= 95) {
            return 3;
        } else if(skill >= 75) {
            return 2;
        } else if(skill >= 50) {
            return 1;
        } else if(skill >= 25) {
            return 0;
        } else  {
            return -1;
        }
    };
}

bool cEscortJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    Action_Types actiontype = ACTION_WORKESCORT;
    m_Escort = 0;
    m_Prepare = (girl.agility() + girl.service() / 2);

    int fame = 0;
    sImagePreset imagetype = EImageBaseType::ESCORT;

    int roll_a = d100();                            // customer type
    double cust_wealth = 1;
    int cust_type = 0;
    std::string cust_type_text;

    sClientData client;
    bool sex_offer = false;

    //CRAZY
    /*Escort plans
    General idea is to have her arrive to an appoiment with a client.  If she is to late they may turn her away
    resulting in no money if she is on time it goes to the next part.  Next part is they look her over seeing if
    she looks good enough for them or not and looks at what she is wearing.  Fancy dresses help while something like
    slave rags would hurt and may get her sent away again.  Then they are off to do something like go to dinner, a bar,
    maybe a fancy party what have u.  From there its can she hold his intrest and not embarres him.  Traits play the biggest
    part in this.  An elegant girl would do well for a rich person where a aggressive girl might not. On the other hand
    a deadbeat might be turned off by a elegant girl and prefer something more sleazy or such.  If they pass this part
    it goes to a will they have sex roll.  Sometimes they might be okay with not having sex with the girl and be more then
    happy with just her company other times sex is a must.  Do a roll for the sex type while taking into account what
    types of sex are allowed.  Her skill will determine if the client is happy or not.  Sex would be an extra cost.  The
    further a girl makes it and the happier the client the more she makes.  Deadbeat would be super easy to please where
    a king might be almost impossable to make fully happy without the perfect girl.  Sending a virgin should have different
    things happen if it comes to sex where the girl could accept or not.  Maybe have special things happen if the escort is
    your daughter.*/

    //a little pre-game randomness
    add_text("pre-text");

// The type of customer She Escorts
///*default*/    int cust_type = 2;    string cust_type_text = "Commoner";
//{ cust_type = 10; cust_type_text = "King"; }
//{ cust_type = 9; cust_type_text = "Prince"; }
//{ cust_type = 8; cust_type_text = "Noble"; }
//{ cust_type = 7; cust_type_text = "Judge"; }
//{ cust_type = 6; cust_type_text = "Mayor"; }
//{ cust_type = 5; cust_type_text = "Sheriff"; }
//{ cust_type = 4; cust_type_text = "Bureaucrat"; }
//{ cust_type = 3; cust_type_text = "Regular"; }
//{ cust_type = 1; cust_type_text = "Deadbeat"; }

    enum escortchoice
    {
        /*Es_King,
        Es_Prince,
        Es_Noble,
        Es_Judge,
        Es_Mayor,
        Es_Sheriff,
        Es_Bureaucrat,*/
        Es_Regular,
        Es_Commoner,
        Es_DeadBeat,            //


        MD_NumberOfEscortChoices    // Leave this as the last thing on the list to allow for random choices.
    };                                // When the choice gets handled, the switch will use the "MD_name" as the case.

    // the test for if the girl can act on that choice is done next
    int choice = 0;    bool choicemade = false;

    while (!choicemade)
    {
        if (girl.fame() >= 20 && roll_a <= 65)
        {
            choice = Es_Regular;
            choicemade = true;    // ready so continue
        }
        else if (girl.fame() >= 10 && roll_a <= 90)
        {
            choice = Es_Commoner;
            choicemade = true;    // ready so continue
        }
        else
        {
            choice = uniform(0, MD_NumberOfEscortChoices-1);    // randomly choose from all of the choices
            if (choice == Es_DeadBeat) {
                choicemade = true;    // ready so continue
            }
        }
    };

    auto sex_event_fn = [&](const std::string& prefix) {
        auto type = choose_sex(prefix, girl, client);
        if (type != SexType::NONE) {
            imagetype = handle_sex(prefix, fame, girl, type);
        }
    };

    auto traits_check_fn = [&]() {
        // boob event
        if (girl.breast_size() > BreastSize::BIG_BOOBS)
        {
            add_text("client.large-boobs") << "\n";
            client.TittyLover = true;
        }
        else
        {
            if (chance(40))
            {
                add_text("client.small-boobs") << "\n";
                client.TittyLover = true;
            }
            else
            {
                add_text("client.disappointed-boobs") << "\n";
                m_Escort -= 1;
            }
        }

        // ass
        if (girl.any_active_trait({traits::GREAT_ARSE, traits::DELUXE_DERRIERE, traits::PHAT_BOOTY}) && !client.TittyLover)
        {
            m_Escort += 1;
            client.AssLover = true;
            add_text("client.great-ass") << "\n";
        }

        if (girl.any_active_trait({traits::SEXY_AIR, traits::GREAT_FIGURE, traits::HOURGLASS_FIGURE}))
        {
            add_text("client.sexy");
            m_Escort += 1;
        }

        if (girl.has_active_trait(traits::BRUISES))
        {
            add_text("client.bruises");
        }
    };

    switch (choice)
    {
        //case Es_Bureaucrat:
        //        {
        //        }
        //        break;    // end Es_Bureaucrat

        case Es_Regular:
        {
            cust_type = 3; cust_type_text = "Regular";
            //Regular event

            // telling what she is meeting and where; how prepared and when does she arrive...
            add_text("regular") << "\n";
            m_Escort += skill_to_mod(m_Prepare);

            // beauty check
            add_text("regular.beauty-check") << "\n";
            m_Escort += skill_to_mod(girl.beauty());

            //boob event
            if (girl.breast_size() > BreastSize::BIG_BOOBS && chance(75))
            {
                m_Escort += 1;
                add_text("regular.boobs");
            }

            add_text("regular.beauty-traits");

            if (girl.has_item("Bad Nurse Costume") || girl.has_item("Sexy Nurse Costume"))
            {
                ss << "The three of them do spend a moment staring at the revealing \"nurse\" costume that ${name} has decided to wear to this assignation. \"You didn't tell me that she was in the medical profession,\" says the boss, his gaze lingering on her very short skirt while his wife chokes on her drink.\n";
                m_Escort -= 1;
            }
            else if (girl.has_item("Black Knight Uniform Coat"))
            {
                ss << "\"My goodness, you didn't tell me that your lovely girlfriend was with the Royal Guards,\" states the boss as they all pass their eyes over ${name}'s Black Knight Uniform Coat. \"We must be on good behavior this evening,\" he jokes.\n";
            }
            else if (girl.has_item("Chinese Dress") || girl.has_item("Enchanted Dress") || girl.has_item("Hime Dress") || girl.has_item("Linen Dress") || girl.has_item("Miko Dress") || girl.has_item("Bourgeoise Gown") || girl.has_item("Faerie Gown") || girl.has_item("Peasant Gown"))
            {
                ss << "\"What a lovely dress, darling,\" the wife begins with a compliment. ${name} is happy to see that the client is pleased with her choice of attire.";
                m_Escort += 1;
            }
            else if (girl.has_item("Brothel Gown") || girl.has_item("Trashy Dress"))
            {
                ss << "\"What a.. lovely.. dress,\" says the wife, elbowing her husband, who is staring with an open mouth at ${name}'s trashy brothel attire. The client winces slightly.";
                m_Escort -= 1;
            }
            else if (girl.has_item("Bunny Outfit"))
            {
                ss << "All three patrons stare with consternation at the revealing bunny outfit that ${name} is wearing. The boss sweats a little while locking eyes on her cleavage, and his wife politely inquires if perhaps she just got off of work at a casino. The client hides his face and deep embarrassment in his hands.";
                m_Escort -= 2;
            }
            else if (girl.has_item("Dancer Costume"))
            {
                ss << "\"Oh, you're a dancer!\" cries the wife with pleasure, looking at ${name}'s dancer costume. \"How wonderful! I always wanted to be a dancer when I was younger. You must tell me everything about it!\"\n";
                m_Escort += 1;
            }
            else if (girl.has_item("Gantz Suit") || girl.has_item("Plug Suit"))
            {
                ss << "Nobody says anything as ${name} sits, but they are all staring speechless at the skin-tight leather/latex dress she is wearing. \"Oh my,\" whispers the boss, loosening his collar and turning red. His wife just continues staring as the client buries his face in his hands.\n";
                m_Escort -= 3;
            }
            else if (girl.has_item("Gemstone Dress") || girl.has_item("Noble Gown") || girl.has_item("Royal Gown") || girl.has_item("Silken Dress") || girl.has_item("Velvet Dress"))
            {
                ss << "\"My goodness, what a beautiful dress!\" cries the wife, gazing at ${name}'s expensive attire. While she is clearly overdressed for the occasion, both the boss and his wife simply chalk it up as a charming attempt to support her boyfriend by being extra presentable.\n";
            }
            else if (girl.has_item("Maid Uniform"))
            {
                ss << "\"Do you work at one of the nearby estates?\" inquires the wife, looking at ${name}'s maid uniform. \"I'm so glad you were able to join us with your work schedule.\" The client is surprised, but not disappointed, with the choice of attire, and quickly invents a story about her job.\n";
            }
            else if (girl.has_item("Nurse Uniform"))
            {
                ss << "\"Oh, you must be coming straight from the hospital,\" exclaims the wife, looking over ${name}'s nurse uniform. \"I'm so glad you could join us. I know how demanding those poor nurses work over there.\" The client is surprised at the choice of attire, but quickly runs with it, inventing a story about her job.\n";
            }
            else if (girl.has_item("School Uniform"))
            {
                ss << "\"Aren't you a pretty young thing,\" says the wife, looking over ${name}'s school uniform, and then back at the client. \"You didn't tell me she was continuing her education! What school do you attend, darling?\" she inquires, and the client quickly invents a story to justify the outfit.\n";
            }
            else if (girl.has_item("Slave Rags"))
            {
                ss << "\"Oh, you poor thing,\" says the wife softly, taking in the slave rags that ${name} is wearing. \"Is everything.. all right.. with your, um, job?\" asks the boss, awkwardly trying to navigate around the fact that ${name} is apparently a slave who does not possess anything more than these revealing rags. The client struggles valiantly to explain how he has come to fall in love with a poor slave and is trying to buy her freedom. Everyone sits in uncomfortable silence for a moment.\n";
                m_Escort -= 1;
            }

            //CONVERSATION PHASE (CHARISMA CHECK)
            ss << "\n";
            add_text("regular.service") << "\n";
            add_text("regular.performance") << "\n";
            m_Escort += skill_to_mod(girl.service());
            m_Escort += skill_to_mod(girl.performance());

            //random stuff
            add_text("regular.conversation-traits") << "\n";

            //RESULTS PHASE (POINT CHECK)
            bool group_offer = false;
            if (m_Escort >= 9)
            {
                add_text("regular.best") << "\n";
                fame += 2;
                group_offer = true;
                cust_wealth = 2;
            }
            else if (m_Escort >= 6)
            {
                add_text("regular.good") << "\n";
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (m_Escort >= 1)
            {
                add_text("regular.neutral") << "\n";
            }
            else
            {
                add_text("regular.bad") << "\n";
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (group_offer)
            {
                if (girl.morality() >= 50)
                {
                    add_text("regular.group.moral");
                    sex_offer = true;
                }
                else if (girl.libido() <= 50)
                {
                    add_text("regular.group.no-lust");
                }
                else
                {
                    if (is_virgin(girl))
                    {
                        add_text("regular.group.virgin") << "\n";
                        fame += skill_to_mod(girl.lesbian());
                        imagetype = EImagePresets::LESBIAN;
                        girl.lesbian(2);
                        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                    }
                    else
                    {
                        add_text("regular.group.sex") << "\n";
                        fame += skill_to_mod(girl.group());
                        imagetype = EImagePresets::ORGY;
                        girl.group(2);
                        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
                    }
                    add_text("regular.group.after-sex") << "\n";
                }
            }

            if (sex_offer)
            {
                if (girl.morality() >= 50) {
                    add_text("regular.client.moral") << "\n";
                    client.SexOffer = SexType::ORAL;
                } else if (girl.libido() <= 50) {
                    add_text("regular.client.no-lust") << "\n";
                } else {
                    client.SexOffer = SexType::ANY;
                }
            }

            sex_event_fn("regular");

        }
            break;    // end Es_Regular

        case Es_Commoner:
        {
            cust_type = 2; cust_type_text = "Commoner";
            //COMMENER
            // telling what she is meeting and where; how prepared and when does she arrive...
            add_text("commoner") << "\n";
            m_Escort += skill_to_mod(m_Prepare);


            // beauty check
            add_text("commoner.beauty-check") << "\n";
            m_Escort += skill_to_mod(girl.beauty());

            traits_check_fn();

            //CONVERSATION PHASE (REFINEMENT  CHECK)
            add_text("commoner.refinement") << "\n";
            m_Escort += skill_to_mod(girl.refinement());
            if (girl.has_active_trait(traits::ELEGANT))
            {
                add_text("commoner.refinement.elegant") << "\n";
                m_Escort += 1;
            } else if (girl.has_active_trait(traits::DOMINATRIX))
            {
                add_text("commoner.refinement.dominant") << "\n";
                m_Escort += (2 * skill_to_mod(girl.refinement()) - 1) / 2;
            }
            if (girl.has_active_trait(traits::SOCIAL_DRINKER))
            {
                add_text("commoner.refinement.social-drinker") << "\n";
                m_Escort -= 1;
            }

            //RESULTS PHASE (POINT CHECK)
            if (m_Escort >= 9)
            {
                add_text("commoner.best") << "\n";
                fame += 2;
                sex_offer = true;
                cust_wealth = 2;
            }
            else if (m_Escort >= 6)
            {
                add_text("commoner.good") << "\n";
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (m_Escort >= 1)
            {
                add_text("commoner.neutral") << "\n";
            }
            else
            {
                add_text("commoner.bad") << "\n";
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (sex_offer)
            {
                if (girl.morality() >= 50)
                {
                    add_text("commoner.sex.moral") << "\n";
                }
                else if (girl.libido() <= 50)
                {
                    add_text("commoner.sex.no-lust") << "\n";
                }
                else
                {
                    add_text("commoner.sex.accept") << "\n";
                    client.SexOffer = SexType::ANY;
                }
            }

            //sex event
            sex_event_fn("commoner");
        }
            break;    // end Es_Commoner


        case Es_DeadBeat:
        {
            cust_type = 1; cust_type_text = "Dead Beat";
            // telling what she is meeting and where; how prepared and when does she arrive...
            add_text("deadbeat") << "\n";
            m_Escort += skill_to_mod(m_Prepare);

            // beauty check
            add_text("deadbeat.beauty-check") << "\n";
            m_Escort += skill_to_mod(girl.beauty());

            traits_check_fn();

            //CONVERSATION PHASE (CHARISMA CHECK)
            add_text("deadbeat.charisma") << "\n";
            m_Escort += skill_to_mod(girl.charisma());
            if (girl.has_active_trait(traits::ALCOHOLIC) && chance(50))
            {
                add_text("deadbeat.alcoholic") << "\n";
                girl.upd_temp_stat(STAT_LIBIDO, +20, true);
            }

            //RESULTS PHASE (POINT CHECK)
            if (m_Escort >= 9)
            {
                add_text("deadbeat.best") << "\n";
                fame += 2;
                sex_offer = true;
                cust_wealth = 2;
            }
            else if (m_Escort >= 6)
            {
                add_text("deadbeat.good") << "\n";
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (m_Escort >= 1)
            {
                add_text("deadbeat.neutral") << "\n";
            }
            else
            {
                add_text("deadbeat.bad") << "\n";
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (sex_offer)
            {
                if (girl.morality() >= 50)
                {
                    add_text("deadbeat.sex.moral");
                }
                else if (girl.libido() <= 50)
                {
                    add_text("deadbeat.sex.no-lust");
                }
                else
                {
                    add_text("deadbeat.sex.accept") << "\n";
                    client.SexOffer = SexType::ANY;
                }
            }

            //sex event
            sex_event_fn("deadbeat");
        }
            break;    // end Es_DeadBeat
    };


    // work out the pay between the house and the girl
    m_Earnings = (int)(girl.askprice() * cust_type * cust_wealth);
    // m_Tips = (jobperformance > 0) ? (rng%jobperformance) * cust_type * cust_wealth : 0;
    ss << "\n \n${name} receives " << m_Earnings << " in payment for her work as an Escort for a " << cust_type_text << " client. Her fame as an Escort has changed by " << fame << ".";

    // Improve stats
    apply_gains(girl, m_Performance);
    girl.fame(uniform(0, 2));

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    return false;
}

IGenericJob::eCheckWorkResult cEscortJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(ACTION_WORKESCORT, JOB_ESCORT))
    {
        ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }

    if (girl.has_active_trait(traits::DEAF) && chance(50))
    {
        ss << "${name} is deaf, meaning she would be unable to hear the conversation that is so critical to being successful as an escort. "
              "As there is practically no chance that a client will want to have an entire date in sign language, assuming he even knows it, "
              "${name} is particularly unsuited to work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
        return eCheckWorkResult::IMPOSSIBLE;
    }
    else if (girl.has_active_trait(traits::MUTE) && chance(50))
    {
        ss << "${name} is mute, and while some men enjoy a woman who stays silent, these men are not paying escorts to engage them in conversation. "
              "As it is severely unlikely that a client will want to spend the entire date deciphering sign language, even if they do know it,"
              " ${name} is particularly unsuited for work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
        return eCheckWorkResult::IMPOSSIBLE;
    }
    return eCheckWorkResult::ACCEPTS;
}

sImagePreset cEscortJob::handle_sex(const std::string& prefix, int& fame, sGirl& girl, SexType type) {
    switch(type) {
    case SexType::SEX:
        add_text("client.normal-sex") << "\n";
        add_text(prefix + ".normal-sex.outro");
        fame += skill_to_mod(girl.normalsex());
        girl.normalsex(2);
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        return EImageBaseType::VAGINAL;
    break;
    case SexType::ANAL:
        if (girl.has_item("Compelling Buttplug"))
        {
            add_text("client.buttplug.compelling");
            fame += 1;
        }
        else if (girl.has_item("Large Buttplug"))
        {
            add_text("client.buttplug.large");
            fame += 2;
        }
        fame += skill_to_mod(girl.anal());
        add_text("client.anal-sex") << "\n";
        add_text(prefix + ".anal-sex.outro");
        girl.anal(2);
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        return EImageBaseType::ANAL;
    break;
    case SexType::HAND:
        fame += skill_to_mod(girl.handjob());
        add_text("client.handjob") << "\n";
        add_text(prefix + ".handjob.outro");
        girl.handjob(2);
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        return EImageBaseType::HAND;
    break;
    case SexType::TITTY:
        fame += skill_to_mod(girl.tittysex());
        add_text(prefix + ".titty-sex.intro") << "\n";
        add_text("client.titty-sex") << "\n";
        add_text(prefix + ".titty-sex.outro");
        girl.tittysex(2);
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        return EImageBaseType::TITTY;
    break;
    case SexType::ORAL:
        if (girl.any_active_trait({traits::STRONG_GAG_REFLEX, traits::GAG_REFLEX}))
        {
            add_text("client.oral-sex-gag") << "\n";
            fame -= 1;
        } else {
            add_text("client.oral-sex-no-gag") << "\n";
        }

        fame += skill_to_mod(girl.oralsex());
        add_text(prefix + ".oral-sex.outro");

        girl.oralsex(2);
        girl.upd_temp_stat(STAT_LIBIDO, -20, true);
        if(girl.oralsex() > 60 && chance(25)) {
            return EImageBaseType::SUCKBALLS;
        }
        return EImagePresets::BLOWJOB;
        break;
    default:
        return EImageBaseType::PROFILE;
        break;
    }
}

auto cEscortJob::choose_sex(const std::string& prefix, const sGirl& girl, const sClientData& client) -> SexType {
    SexType type = client.SexOffer;
    if(type == SexType::NONE) return type;

    if (client.TittyLover && chance(50)) {
        type = SexType::TITTY;
    }
    else if (client.AssLover && chance(50))
    {
        add_text("client.ass-lover") << "\n";
        if (is_virgin(girl)) {
            add_text("client.ass-lover.virgin") << "\n";
            type = SexType::ANAL;
        } else {
            if(chance(50)) {
                type = SexType::ANAL;
            } else {
                type = SexType::SEX;
            }
        }
    }
    else
    {
        add_text(prefix + ".init-sex") << "\n";
    }

    if(type == SexType::ANY) {
        int roll_sex = d100();
        if (roll_sex >= 50 && !is_virgin(girl)) {
            return SexType::SEX;
        } else if(roll_sex > 40) {
            return SexType::ANAL;
        } else if(roll_sex > 25) {
            return SexType::HAND;
        } else {
            return SexType::ORAL;
        }
    }

    return type;
}

cEscortJob::cEscortJob() : cSimpleJob(JOB_ESCORT, "Escort.xml", {ACTION_WORKESCORT, 0, EImageBaseType::ESCORT}) {
    RegisterVariable("Escort", m_Escort);
    RegisterVariable("Prepare", m_Prepare);
}
