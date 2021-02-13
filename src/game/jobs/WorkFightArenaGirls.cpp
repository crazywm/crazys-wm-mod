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
#include "IGame.h"
#include <sstream>
#include "CLog.h"
#include "cGirlGangFight.h"
#include "buildings/cDungeon.h"

#pragma endregion

// `J` Job Arena - Fighting
bool WorkFightArenaGirls(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_COMBAT;
    std::stringstream ss;
    if (girl.disobey_check(actiontype, JOB_FIGHTARENAGIRLS))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} was assigned to fight other girls in the arena.\n \n";


    int wages = 0, tips = 0;
    int enjoy = 0, fame = 0;

    int imagetype = IMGTYPE_COMBAT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_FIGHTARENAGIRLS, false);

    cGirls::EquipCombat(girl);        // ready armor and weapons!
    int fightxp = 1;
    auto tempgirl = g_Game->CreateRandomGirl(18, false, false, false, false, true);
    if (tempgirl) {
        auto fight_outcome = GirlFightsGirl(girl, *tempgirl);
        if (fight_outcome == EFightResult::VICTORY)    // she won
        {
            fightxp = 3;
            enjoy = rng % 3 + 1;
            fame = rng % 3 + 1;
            std::shared_ptr<sGirl> ugirl = nullptr;
            if (rng.percent(10))        // chance of getting unique girl
            {
                ugirl = g_Game->GetRandomGirl(false, false, true);
            }
            if (ugirl)
            {
                std::stringstream msg;    // goes to the girl and the g_MessageQue
                std::stringstream Umsg;    // goes to the new girl
                std::stringstream Tmsg;    // temp msg
                ugirl->set_stat(STAT_HEALTH, rng.in_range(1, 50));
                ugirl->set_stat(STAT_HAPPINESS, rng.in_range(1, 80));
                ugirl->set_stat(STAT_TIREDNESS, rng.in_range(50, 100));
                ugirl->set_status(STATUS_ARENA);
                msg << "${name} won her fight against " << ugirl->FullName() << ".\n \n";
                Umsg << ugirl->FullName() << " lost her fight against your girl ${name}.\n \n";
                Tmsg << ugirl->FullName();
                if (rng.percent(50))
                {
                    ugirl->set_status(STATUS_SLAVE);
                    Tmsg << "'s owner could not afford to pay you your winnings so he gave her to you instead.\n \n";
                }
                else
                {
                    Tmsg << " put up a good fight so you let her live as long as she came work for you.\n \n";
                    wages = 100 + rng % (girl.fame() + girl.charisma());
                }
                msg << Tmsg.str();
                Umsg << Tmsg.str();
                ss << msg.str();
                g_Game->push_message(msg.str(), 0);
                ugirl->m_Events.AddMessage(Umsg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);

                g_Game->dungeon().AddGirl(ugirl, DUNGEON_NEWARENA);
            }
            else
            {
                if (girl.has_active_trait("Exhibitionist"))
                {
                    ss << "As she enjoys showing off her body, ${name} lets her opponent cut away her already-skimpy clothing, ending her match all but naked - but victorious.  Many of the patrons seem to enjoy the show.\n";
                }
                else if (girl.has_active_trait("Sadistic"))
                {
                    ss << "As the match draws to a close, ${name} stops trying to win and simply toys with her opponent, practically torturing her in front of the audience.  Many patrons disapprove, although a few respond favorably.\n";
                }
                else if (girl.has_active_trait("Dominatrix"))
                {
                    ss << "${name}'s verbal abuse, along with her combat prowess, lead to a dramatic defeat of her opponent, although her behavior towards the loser disappoints many patrons.\n";
                }
                else if (girl.has_active_trait("Brawler"))
                {
                    ss << "${name}'s preference for grappling and clinch-fighting throws her opponent off-balance and gives her an easy, if somewhat boring, victory.\n";
                }
                else if (girl.has_active_trait("Flight"))
                {
                    ss << "Being able to fly is an almost-unfair advantage that most opponents can't counter.  Thanks to that, ${name} wins handily.\n";
                }
                else if (girl.has_active_trait("Assassin"))
                {
                    ss << "Utilizing a no-rules, victory-at-any-cost combat style, ${name} won a decisive victory over her orthodox opponent.  This was not unnoticed by some people in the audience.\n";
                }
                else if (girl.has_active_trait("Masochist"))
                {
                    ss << "${name}'s cries of joy and outright pleasure upon being injured distract her opponent, and the pain-loving girl comes back from apparent defeat to achieve a dramatic victory.  Despite her many injuries, she smiles happily as she thanks her opponent for the match.\n";
                }
                else if (girl.magic() >= 50 && girl.combat() < 50)
                {
                    ss << "${name}'s powerful magic demonstrates precisely why mages are feared by many.  She even manages to look good while doing it!\n";
                }
                else if (girl.magic() >= 75 && girl.combat() >= 75)
                {
                    ss << "Having mastered both weapons and sorcery, ${name} is a nearly unstoppable force of nature in the Arena, easily dispatching opponents who focus on one branch of combat over the other.\n";
                }
                else
                {
                    ss << "${name} won her fight.";
                }
                wages = 100 + rng % (girl.fame() + girl.charisma());
            }
        }
        else if (fight_outcome == EFightResult::DEFEAT) // she lost
        {
            enjoy = -(rng % 3 + 1);
            fame = -(rng % 3 + 1);
            if (girl.has_active_trait("Exhibitionist"))
            {
                ss << "As she enjoys showing off her body, ${name} lets her opponent cut away her already-skimpy clothing, but either because of her lack of skill or just bad luck, she takes a real hit and is defeated.\n";
            }
            else if (girl.has_active_trait("Sadistic"))
            {
                ss << "${name} tries to torment her opponent, but her overconfidence leads to a stunning defeat when the other fighter feigns injury.\n";
            }
            else if (girl.has_active_trait("Flight"))
            {
                ss << "${name} was unable to leverage her greater maneuverability and yielded the match after being forced to the ground.\n";
            }
            else if (girl.has_active_trait("Masochist"))
            {
                ss << "Overwhelmed by pleasure, ${name} is unable to defend herself from her opponent and is easily defeated.  After the match ends, she begs the other fighter for another match - 'just like this one.'\n";
            }
            else if (girl.magic() >= 50 && girl.combat() < 50)
            {
                ss << "Lacking the physical prowess to hold her opponent off while she readies her spells, ${name} is quickly defeated by her opponent.\n";
            }
            else if (girl.magic() >= 75 && girl.combat() >= 75)
            {
                ss << "You can't belive, ${name} lost. With her skill in combat and magic you thought her unbeatable.\n";
            }
            else
            {
                ss << "She lost the fight.";
            }
            int cost = 150;
            brothel->m_Finance.arena_costs(cost);
            ss << " You had to pay " << cost << " gold cause your girl lost.";
            /*that should work but now need to make if you lose the girl if you dont have the gold zzzzz FIXME*/
        }
        else  // it was a draw
        {
            enjoy = rng % 3 - 2;
            fame = rng % 3 - 2;
            ss << "The fight ended in a draw.";
        }
    }
    else {
        g_LogFile.log(ELogLevel::ERROR, "You have no Arena Girls for your girls to fight\n");
        ss << "There were no Arena Girls for her to fight.\n \n(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
        imagetype = IMGTYPE_PROFILE;
    }

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }

    if (girl.has_active_trait("Exhibitionist") && rng.percent(15))
    {
        ss << "A flamboyant fighter, ${name} fights with as little armor and clothing as possible, and sometimes takes something off in the middle of a match, to the enjoyment of many fans.\n";
    }

    if (girl.has_active_trait("Idol") && rng.percent(15))
    {
        ss << "${name} has quite the following, and the Arena is almost always packed when she fights.  People just love to watch her in action.\n";
    }

    // Improve girl
    int xp = 5 * fightxp, libido = 3, skill = 1;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 5; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 5; }

    if (girl.is_unpaid())
    {
        wages = 0;
    }

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);


    girl.AddMessage(ss.str(), imagetype, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    girl.fame(fame);
    girl.exp(xp);
    girl.combat(rng%fightxp + skill);
    girl.magic(rng%fightxp + skill);
    girl.agility(rng%fightxp + skill);
    girl.constitution(rng%fightxp + skill);
    girl.upd_temp_stat(STAT_LIBIDO, libido);
    girl.upd_Enjoyment(actiontype, enjoy);

    /* `J` this will be a place holder until a better payment system gets done
    *
    */
    int earned = 0;
    for (int i = 0; i < jobperformance; i++)
    {
        earned += rng % 10 + 5; // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    brothel->m_Finance.arena_income(earned);
    ss.str("");
    ss << "${name} drew in " << jobperformance << " people to watch her and you earned " << earned << " from it.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    //gain traits
    cGirls::PossiblyGainNewTrait(girl, "Tough", 65, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 55, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Aggressive", 70, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
    if (rng.percent(25) && girl.strength() >= 65 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, "${name} has become pretty Strong from all of the fights she's been in.", Day0Night1);
    }
    if (rng.percent(25) && girl.combat() >= 60 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Brawler", 60, ACTION_COMBAT, "${name} has become pretty good at fighting.", Day0Night1);
    }
    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Fragile", 35, actiontype, "${name} has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

#pragma endregion
    return false;
}

double JP_FightArenaGirls(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;

    if (estimate)// for third detail string
    {
        jobperformance +=
            (girl.fame() / 2) +
            (girl.charisma() / 2) +
            (girl.combat() / 2) +
            (girl.magic() / 2) +
            (girl.level());
    }
    else// for the actual check
    {
        jobperformance += (girl.fame() + girl.charisma()) / 2;
        if (!estimate)
        {
            int t = girl.tiredness() - 80;
            if (t > 0)
                jobperformance -= (t + 2) * (t / 2);
        }
    }

    jobperformance += girl.get_trait_modifier("work.fightarena");

    return jobperformance;
}
