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

#include "gang_missions.h"

#include <sstream>

#include "IGame.h"
#include "cGangs.h"
#include "character/sGirl.h"
#include "cRival.h"
#include "character/cPlayer.h"
#include "cInventory.h"
#include "buildings/cDungeon.h"
#include "cObjectiveManager.hpp"
#include "sStorage.h"
#include "buildings/IBuilding.h"
#include "buildings/cBuildingManager.h"
#include "CLog.h"
#include "combat/combat.h"
#include "Inventory.h"
#include "cGirlGangFight.h"


namespace settings {
    extern const char* WORLD_CATACOMB_UNIQUE;
}

extern cRng g_Dice;

IGangMission::IGangMission(std::string name, bool potions, bool nets):
    m_Name(std::move(name)), m_RequiresPotions(potions), m_RequiresNets(nets)
{

}


const std::string& IGangMission::name() const
{
    return m_Name;
}

cGangManager& IGangMission::gang_manager()
{
    return g_Game->gang_manager();
}

bool IGangMission::run(sGang& gang)
{
    g_LogFile.log(ELogLevel::DEBUG, "Running mission '", m_Name, "' for gang '", gang.name(), "'");
    std::stringstream event_text;
    return execute_mission(gang, event_text);
}

bool IGangMission::requires_potions() const
{
    return m_RequiresPotions;
}

bool IGangMission::requires_nets() const
{
    return m_RequiresNets;
}

cMissionGrandTheft::cMissionGrandTheft() : IGangMission("Grand Theft", true, false)
{

}

bool cMissionGrandTheft::execute_mission(sGang& gang, std::stringstream& ss)
{
    g_Game->player().disposition(-3);    g_Game->player().customerfear(3);    g_Game->player().suspicion(3);
    bool fightrival = false;        cRival* rival = nullptr;
    std::string place = "place";            int defencechance = 0;            long gold = 1;
    int difficulty = std::max(0, g_Dice.bell(0, 6) - 2);    // 0-4

    if (difficulty <= 0)    { place = "small shop";        defencechance = 10;        gold += 10 + g_Dice % 290; difficulty = 0; }
    if (difficulty == 1)    { place = "smithy";            defencechance = 30;        gold += 50 + g_Dice % 550; }
    if (difficulty == 2)    { place = "jeweler";        defencechance = 50;        gold += 200 + g_Dice % 800; }
    if (difficulty == 3)    { place = "trade caravan";    defencechance = 70;        gold += 500 + g_Dice % 1500; }
    if (difficulty >= 4)    { place = "bank";            defencechance = 90;        gold += 1000 + g_Dice % 4000; difficulty = 4; }

    ss << "Gang   " << gang.name() << "   goes out to rob a " << place << ".\n \n";

    // `J` chance of running into a rival gang updated for .06.02.41
    int gangs = g_Game->rivals().GetNumRivalGangs();
    int chance = 10 + std::max(30, gangs * 2);                // 10% base +2% per gang, 40% max
    ss << "The " << place << " ";
    std::unique_ptr<sGang> defenders = std::make_unique<sGang>();
    if (g_Dice.percent(chance))
    {
        rival = g_Game->rivals().GetRandomRivalWithGangs();
        if (rival && rival->m_NumGangs > 0)
        {
            fightrival = true;
            ss << "is guarded by a gang from " << rival->m_Name;
            *defenders = gang_manager().GetTempGang(rival->m_Power);
        }
    }
    if (defenders == nullptr && g_Dice.percent(defencechance))
    {
        *defenders = gang_manager().GetTempGang(difficulty * 3);
        defenders->give_potions(10);
        ss << "has its own guards";
    }

    if (defenders)
    {
        ss << ". They ";
        int gstart = gang.m_Num;
        if (GangBrawl(gang, *defenders) != EFightResult::VICTORY)
        {
            ss << "put up quite a fight and send your men running.";
            gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
            return false;
        }
        float dif = (float)gstart / (float)gang.m_Num;
        if (gstart == gang.m_Num)    ss << "turn tail and run as soon as they see your men";
        else if (dif > 0.9)/*   */    ss << "put up little resistance as your men walk right over them";
        else if (dif > 0.7)/*   */    ss << "put up a fight but your men win";
        else if (dif > 0.5)/*   */    ss << "fight well but your men defeat them";
        else if (dif > 0.3)/*   */    ss << "fight well but your men still manage to vanquish their foe";
        else/*                  */    ss << "fight valiantly but your men still manage to vanquish their foe";
    } else {
        ss << "is unguarded";
    }
    ss << ".\n \n";

    if (fightrival && defenders->m_Num <= 0) rival->m_NumGangs--;

    // rewards
    ss << "They get away with " << gold << " gold from the " << place << ".\n";

    // `J` zzzzzz - need to add items


    g_Game->player().suspicion(gold / 1000);

    g_Game->gold().grand_theft(gold);
    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

    if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
    {
        g_Game->get_objective()->m_SoFar += gold;
    }
    return true;
}

cMissionKidnap::cMissionKidnap() : IGangMission("Kidnapping", true, true)
{

}

bool cMissionKidnap::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   is kidnapping girls.\n \n";

    if (g_Dice.percent(std::min(75, gang.intelligence())))    // chance to find a girl to kidnap
    {
        auto girl = g_Game->GetRandomGirl();
        if (girl != nullptr)
        {
            return kidnap(gang, ss, std::move(girl));
        }
    }
    ss << "They failed to find any girls to kidnap.";
    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
    return false;
}

bool cMissionKidnap::kidnap(sGang& gang, std::stringstream& ss, std::shared_ptr<sGirl> girl)
{
    bool captured = false;
    /// TODO kidnap event

    std::string girlName = girl->FullName();
    std::stringstream NGmsg;
    int girlimagetype = IMGTYPE_PROFILE;
    auto eventtype = EVENT_GANG;
    auto gangeventtype = EVENT_GANG;
    int dungeonreason = DUNGEON_GIRLKIDNAPPED;


    /* MYR: For some reason I can't figure out, a number of girl's house percentages
    are at zero or set to zero when they are sent to the dungeon. I'm not sure
    how to fix it, so I'm explicitly setting the percentage to 60 here */
    girl->house(60);

    ss << "Your men find a girl, " << girlName << ", and ";
    if (g_Dice.percent(std::min(75, gang.charisma())))    // convince her
    {
        ss << "convince her that she should work for you.\n";
        NGmsg << girlName << " was talked into working for you by " << gang.name() << ".";
        dungeonreason = DUNGEON_NEWGIRL;
        gang.BoostStat(STAT_CHARISMA, 3);
        captured = true;
        if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
        {
            g_Game->get_objective()->m_SoFar++;                        // `J` Added to make Charisma Kidnapping count
            if (g_Dice.percent(g_Game->get_objective()->m_Target * 10))    // but possibly reduce the reward to gold only
                g_Game->get_objective()->m_Reward = REWARD_GOLD;
        }
    }

    if (!captured)
    {
        switch(AttemptCapture(gang, *girl)) {
            case EAttemptCaptureResult::SUBMITS:
                dungeonreason = DUNGEON_GIRLKIDNAPPED;
                girl->add_temporary_trait("Kidnapped", 3 + g_Dice % 8);
                ss << "kidnap her successfully without a fuss.  She is in your dungeon now.";
                NGmsg << girl->FullName() << " was surrounded by " << gang.name() << " and gave up without a fight.";
                captured = true;
               break;
            case EAttemptCaptureResult::CAPTURED:
                ss << "attempt to kidnap her.\n";
                girlimagetype = IMGTYPE_DEATH;
                dungeonreason = DUNGEON_GIRLKIDNAPPED;
                girl->set_stat(STAT_OBEDIENCE, 0);
                girl->add_temporary_trait("Kidnapped", 10 + g_Dice % 11);
                ss << "She fights back but your men succeed in kidnapping her.\n";
                NGmsg << girl->FullName() << " fought with " << gang.name() << " but lost. She was dragged back to the dungeon.";
                gang.BoostSkill(SKILL_COMBAT, 1);
                captured = true;
                break;
            case EAttemptCaptureResult::ESCAPED:
                ss << "attempt to kidnap her.\n";
                ss << "The girl fights back and defeats your men before escaping into the streets.";
                gangeventtype = EVENT_DANGER;
        }
    }

    if (captured)
    {
        girl->AddMessage(NGmsg.str(), girlimagetype, eventtype);
        g_Game->dungeon().AddGirl(std::move(girl), dungeonreason);
        gang.BoostStat(STAT_INTELLIGENCE);
    }
    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, gangeventtype);
    return captured;
}

cMissionSabotage::cMissionSabotage() : IGangMission("Sabotage", true, false)
{
}

bool cMissionSabotage::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   is attacking rivals.\n \n";
    /*
    *    See if they can find any enemy assets to attack
    *
    *    I'd like to add a little more intelligence to this.
    *    Modifiers based on gang intelligence, for instance
    *    Allow a "scout" activity for gangs that improves the
    *    chances of a raid. That sort of thing.
    */
    if (!g_Dice.percent(std::min(90, gang.intelligence())))
    {
        gang.m_Events.AddMessage("They failed to find any enemy assets to hit.", IMGTYPE_PROFILE, EVENT_GANG);
        return false;
    }
    /*
    *    if yes then do damage to a random rival
    *
    *    Something else to consider: rival choice should be
    *    weighted by number of territories controlled
    *    (or - if we go with the ward idea - by territories
    *    controlled in the ward in question
    *
    *    of course, if there is no rival, it's academic
    */
    cRival* rival = g_Game->rivals().GetRandomRivalToSabotage();
    if (!rival)
    {
        gang.m_Events.AddMessage("Scouted the city in vain, seeking would-be challengers to your dominance.", IMGTYPE_PROFILE, EVENT_GANG);
        return false;
    }

    if (rival->m_NumGangs > 0)
    {
        sGang rival_gang = gang_manager().GetTempGang(rival->m_Power);
        rival_gang.give_potions(10);
        ss << "Your men run into a gang from " << rival->m_Name << " and a brawl breaks out.\n";

        switch(GangBrawl(gang, rival_gang)) {
            case EFightResult::VICTORY:
                break;
            case EFightResult::DRAW:
            case EFightResult::DEFEAT:
                if (gang.m_Num > 0)
                {
                    ss << "Your men lost. The ";
                    if (gang.m_Num == 1) ss << "lone survivor fights his";
                    else ss << gang.m_Num << " survivors fight their";
                    ss << " way back to friendly territory.";
                    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
                }
                else
                {
                    ss << "Your gang " << gang.name() << " fails to report back from their sabotage mission.\nLater you learn that they were wiped out to the last man.";
                    g_Game->push_message(ss.str(), COLOR_RED);
                }
                return false;
        }
        ss << "Your men win." << std::endl;
        if (rival_gang.m_Num <= 0)            // clean up the rival gang
        {
            rival->m_NumGangs--;
            ss << "The enemy gang is destroyed. " << rival->m_Name << " has ";
            if (rival->m_NumGangs == 0)            ss << "no more gangs left!\n";
            else if (rival->m_NumGangs <= 3)    ss << "a few gangs left.\n";
            else                                ss << "a lot of gangs left.\n";
        }
    }
    else ss << "\nYour men encounter no resistance when you go after " << rival->m_Name << ".";

    // if we had an objective to attack a rival we just achieved it
    if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_LAUNCHSUCCESSFULATTACK)
        g_Game->objective_manager().PassObjective();

    // If the rival has some businesses under his control he's going to lose some of them
    if (rival->m_BusinessesExtort > 0)
    {
        // mod: brighter goons do better damage they need 100% to be better than before however
        int spread = gang.intelligence() / 4;
        int num = 1 + g_Dice.random(spread);    // get the number of businesses lost
        if (rival->m_BusinessesExtort < num)  // Can't destroy more businesses than they have
            num = rival->m_BusinessesExtort;
        rival->m_BusinessesExtort -= num;

        ss << "\nYour men destroy " << num << " of their businesses. " << rival->m_Name << " have ";
        if (rival->m_BusinessesExtort == 0)            ss << "no more businesses left!\n";
        else if (rival->m_BusinessesExtort <= 10)    ss << "a few businesses left.\n";
        else                                        ss << "a lot of businesses left.\n";
    }
    else ss << rival->m_Name << " have no businesses to attack.\n";

    if (rival->m_Gold > 0)
    {
        // mod: brighter goons are better thieves
        // `J` changed it // they need 100% to be better than before however
        // `J` now based on rival's gold
        // `J` bookmark - your gang sabotage mission gold taken
        int gold = g_Dice.random(
                (int)(((double)gang.intelligence() / 2000.0) * (double)rival->m_Gold))        // 0-5% of rival's gold
                   + g_Dice.random((gang.intelligence() / 5) * gang.m_Num);                    // plus (int/5)*num
        if (gold > rival->m_Gold) gold = rival->m_Gold;
        rival->m_Gold -= gold;

        // some of the money taken 'dissappears' before the gang reports it.
        if (g_Dice.percent(20) && gold > 1000) gold -= g_Dice % 1000;

        ss << "\nYour men steal " << gold << " gold from them. ";
        if (rival->m_Gold == 0)                ss << "Mu-hahahaha!  They're penniless now!\n";
        else if (rival->m_Gold <= 10000)    ss << rival->m_Name << " is looking pretty poor.\n";
        else            ss << "It looks like " << rival->m_Name << " still has a lot of gold.\n";

        /*
        `J` zzzzzz - need to add more and better limiters
        Suggestions from Whitetooth:
        I'm guessing those factors are based on there skills which make sense. For Example:
        Men - Overall number of people able to carry gold after sabotage.
        Combat - total amount of gold each man can hold.
        Magic - Amount of extra gold the gang can carry with magic not relying on combat or men. Magic could be bonus gold that can't be dropped, bribed, or stolen on the way back.
        Intel - Could be a overall factor to check if the gang knows what is valuable and what isn't.
        Agility - Could be a check for clumsiness of the gang; they could drop valuables on the way back.
        Tough - Checks if there tough enough to intimidate any guards or protect the money they have.
        Charisma - Factors how much gold they have to bribe to guards if they get caught and can't intimidate them.
        The order of checks could be -> Intel -> Magic -> Men - > Combat -> Agility -> Tough -> Charisma
        */

        // `J` bookmark - limit gold taken by gang sabotage
        bool limit = false;
        if (gold > 15000)
        {
            limit = true;
            int burnedbonds = (gold / 10000);
            int bbcost = burnedbonds * 10000;
            gold -= bbcost;
            ss << "\nAs your men are fleeing, one of them has to jump through a wall of fire. When he does, he drops ";
            if (burnedbonds == 1)        ss << "a";
            else if (burnedbonds > 4)    ss << "a stack of ";
            else                        ss << burnedbonds;
            ss << " Gold Bearer Bond" << (burnedbonds > 1 ? "s" : "") << " worth 10k gold each. " << bbcost << " gold just went up in smoke.\n";
        }

        if (gold > 5000 && g_Dice.percent(50))
        {
            limit = true;
            int spill = (g_Dice % 4500) + 500;
            gold -= spill;
            ss << "\nAs they are being chased through the streets by " << rival->m_Name << "'s people, one of your gang members cuts open a sack of gold spilling its contents in the street. "
               << "As the throngs of civilians stream in to collect the coins, they block the pursuers and allow you men to get away safely.\n";
        }

        if (gold > 5000)
        {
            limit = true;
            int bribeperc = ((g_Dice % 15) * 5) + 10;
            int bribe = (int)(gold * ((double)bribeperc / 100.0));
            gold -= bribe;
            ss << "\nAs your gang leave your rival's territory on the way back to your brothel, they come upon a band of local police that are hunting them. Their boss demands "
               << bribeperc << "% of what your gang is carrying in order to let them go.  They pay them " << bribe << " gold and continue on home.\n";
        }

        if (limit)
        {
            ss << "\n" << gang.name() << " returns with " << gold << " gold.\n";
        }
        g_Game->gold().plunder(gold);
    }
    else ss << "The losers have no gold to take.\n";

    if (rival->m_NumInventory > 0 && g_Dice.percent(std::min(75, gang.intelligence())))
    {
        cRivalManager r;
        int num = r.GetRandomRivalItemNum(rival);
        sInventoryItem* item = r.GetRivalItem(rival, num);
        if (item)
        {
            ss << "\nYour men steal an item from them, one " << item->m_Name << ".";
            rival->remove_from_inventory(num);
            g_Game->player().inventory().add_item(item);
        }
    }

    if (rival->m_NumBrothels > 0 && g_Dice.percent(gang.intelligence() / std::min(3, 11 - rival->m_NumBrothels)))
    {
        rival->m_NumBrothels--;
        rival->m_Power--;
        ss << "\nYour men burn down one of " << rival->m_Name << "'s Brothels. " << rival->m_Name;
        if (rival->m_NumBrothels == 0)                ss << " has no Brothels left.\n";
        else if (rival->m_NumBrothels <= 3)            ss << " is in control of very few Brothels.\n";
        else                                        ss << " has many Brothels left.\n";
    }
    if (rival->m_NumGamblingHalls > 0 && g_Dice.percent(gang.intelligence() / std::min(1, 9 - rival->m_NumGamblingHalls)))
    {
        rival->m_NumGamblingHalls--;
        ss << "\nYour men burn down one of " << rival->m_Name << "'s Gambling Halls. " << rival->m_Name;
        if (rival->m_NumGamblingHalls == 0)            ss << " has no Gambling Halls left.\n";
        else if (rival->m_NumGamblingHalls <= 3)    ss << " is in control of very few Gambling Halls .\n";
        else                                        ss << " has many Gambling Halls left.\n";
    }
    if (rival->m_NumBars > 0 && g_Dice.percent(gang.intelligence() / std::min(1, 7 - rival->m_NumBars)))
    {
        rival->m_NumBars--;
        ss << "\nYour men burn down one of " << rival->m_Name << "'s Bars. " << rival->m_Name;
        if (rival->m_NumBars == 0)                    ss << " has no Bars left.\n";
        else if (rival->m_NumBars <= 3)                ss << " is in control of very few Bars.\n";
        else                                        ss << " has many Bars left.\n";
    }

    gang.BoostStat(STAT_INTELLIGENCE, 2);
    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

    // See if the rival is eliminated:  If 4 or more are zero or less, the rival is eliminated
    int VictoryPoints = 0;
    if (rival->m_Gold <= 0)                    VictoryPoints++;
    if (rival->m_NumGangs <= 0)                VictoryPoints++;
    if (rival->m_BusinessesExtort <= 0)        VictoryPoints++;
    if (rival->m_NumBrothels <= 0)            VictoryPoints++;
    if (rival->m_NumGamblingHalls <= 0)        VictoryPoints++;
    if (rival->m_NumBars <= 0)                VictoryPoints++;

    if (VictoryPoints >= 4)
    {
        std::stringstream ssVic;
        ssVic << "You have dealt " << rival->m_Name << " a fatal blow.  Their criminal organization crumbles to nothing before you.";
        g_Game->rivals().RemoveRival(rival);
        gang.m_Events.AddMessage(ssVic.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
    }
    return true;
}

cMissionRecapture::cMissionRecapture() : IGangMission("Recapture", true, true)
{
}

bool cMissionRecapture::execute_mission(sGang& gang, std::stringstream& event_text)
{
    std::stringstream ss;
    ss << "Gang   " << gang.name() << "   is looking for escaped girls.\n \n";

    // check if any girls have run away, if no runaway then the gang continues on as normal
    if (g_Game->GetNumRunaways() == 0)    // `J` this should have been replaced by a check in the gang mission list
    {
        ss << "There are none of your girls who have run away, so they have none to look for.";
        gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
        return false;
    }

    std::stringstream RGmsg;
    auto runaway = g_Game->GetRunaways().front();
    std::string girlName = runaway->FullName();
    bool captured = false;
    int girlimagetype = IMGTYPE_PROFILE;
    auto gangeventtype = EVENT_GANG;

    ss << "Your goons find " << girlName << " and ";

    auto result = AttemptCapture(gang, *runaway);
    switch(result) {
        case EAttemptCaptureResult::SUBMITS:
            ss << "she comes quietly without putting up a fight.";
            RGmsg << girlName << " was recaptured by " << gang.name() << ". She gave up without a fight.";
            captured = true;
            break;
        case EAttemptCaptureResult::CAPTURED:
            girlimagetype = IMGTYPE_DEATH;
            ss << "she fights back but your men succeed in capturing her.\n";
            RGmsg << girlName << " fought with " << gang.name() << " but lost. She was dragged back to the dungeon.";
            gang.BoostSkill(SKILL_COMBAT, 1);
            captured = true;
            break;
        case EAttemptCaptureResult::ESCAPED:
            ss << "The girl fights back and defeats your men before escaping into the streets.";
            gangeventtype = EVENT_DANGER;
            break;
    }

    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, gangeventtype);
    if (captured)
    {
        runaway->m_Events.AddMessage(RGmsg.str(), girlimagetype, EVENT_GANG);
        runaway->m_RunAway = 0;
        g_Game->RemoveGirlFromRunaways(runaway.get());
        g_Game->dungeon().AddGirl(runaway, DUNGEON_GIRLRUNAWAY);
        return true;
    }
    return false;
}


cMissionExtortion::cMissionExtortion() : IGangMission("Extortion", true, false)
{
}

bool cMissionExtortion::execute_mission(sGang& gang, std::stringstream& ss)
{
    g_Game->player().disposition(-1);    g_Game->player().customerfear(1);    g_Game->player().suspicion(1);
    ss << "Gang   " << gang.name() << "   is capturing territory.\n \n";

    // Case 1:  Neutral businesses still around
    int numB = g_Game->rivals().GetNumBusinesses();
    int uncontrolled = TOWN_NUMBUSINESSES - gang_manager().GetNumBusinessExtorted() - numB;
    int n = 0;
    int trycount = 1;
    if (uncontrolled > 0)
    {
        trycount += g_Dice % 5;        // 1-5
        while (uncontrolled > 0 && trycount > 0)
        {
            trycount--;
            if (g_Dice.percent(gang.charisma() / 2))                // convince
            {
                uncontrolled--; n++;
                g_Game->player().customerfear(-1);
            }
            else if (g_Dice.percent(gang.intelligence() / 2))        // outwit
            {
                uncontrolled--; n++;
                g_Game->player().disposition(-1);
            }
            else if (g_Dice.percent(gang.combat() / 2))            // threaten
            {
                uncontrolled--; n++;
                g_Game->player().disposition(-1);
                g_Game->player().customerfear(2);
            }
        }

        if (n == 0) ss << "They fail to gain any more neutral territories.";
        else
        {
            ss << " You gain control of " << n << " more neutral territor" << (n > 1 ? "ies." : "y.");
            gang_manager().NumBusinessExtorted(n);
            g_Game->gold().extortion(n * 20);
        }
        ss << "\nThere ";
        if (uncontrolled <= 0)    ss << "are no more";
        if (uncontrolled == 1)    ss << "is one";
        else ss << "are " << uncontrolled;
        ss << " uncontrolled businesses left.";

    }
    else    // Case 2: Steal bussinesses away from rival if no neutral businesses left
    {
        cRival* rival = g_Game->rivals().GetRandomRivalWithBusinesses();
        if (rival && rival->m_BusinessesExtort > 0)
        {
            ss << "They storm into your rival " << rival->m_Name << "'s territory.\n";
            if (rival->m_NumGangs > 0)
            {
                sGang rival_gang = gang_manager().GetTempGang(rival->m_Power);
                rival_gang.give_potions(10);
                ss << "Your men run into one of their gangs and a brawl breaks out.\n";

                int num_before = gang.m_Num;
                switch(GangBrawl(gang, rival_gang)) {
                    case EFightResult::VICTORY:
                        trycount += g_Dice % 3;

                        ss << "They ";
                        if (rival_gang.m_Num <= 0)
                        {
                            ss << "destroy";
                            rival->m_NumGangs--;
                        }
                        else ss << "defeat";
                        ss << " the defenders losing " << num_before - gang.m_Num << " men and ";
                        break;
                    case EFightResult::DRAW:
                    case EFightResult::DEFEAT:
                        ss << "Your gang has been defeated and fail to take control of any new territory.";
                        gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
                        return false;
                }
            }
            else // Rival has no gangs
            {
                ss << "They faced no opposition as they ";
                trycount += g_Dice % 5;
            }

            while (trycount > 0 && rival->m_BusinessesExtort > 0)
            {
                trycount--;
                rival->m_BusinessesExtort--;
                gang_manager().NumBusinessExtorted(1);
                n++;
            }

            if (n > 0)
            {
                ss << "took over ";
                if (n == 1) ss << "one"; else ss << n;
                ss << " of " << rival->m_Name << "'s territor" << (n > 1 ? "ies" : "y");
            }
            else ss << "left. (Error: no territories gained but should have been)";
        }
        else
        {
            ss << "You fail to take control of any of new territories.";
        }
    }

    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

    if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_EXTORTXNEWBUSINESS)
    {
        g_Game->get_objective()->m_SoFar += n;
    }

    return true;
}


cMissionPettyTheft::cMissionPettyTheft() : IGangMission("Petty Theft", true, false)
{

}

bool cMissionPettyTheft::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   is performing petty theft.\n \n";
    g_Game->player().disposition(-1);
    g_Game->player().customerfear(1);
    g_Game->player().suspicion(1);

    int startnum = gang.m_Num;
    int numlost = 0;

    // `J` chance of running into a rival gang updated for .06.02.41
    int gangs = g_Game->rivals().GetNumRivalGangs();
    int chance = 5 + std::max(20, gangs * 2);                // 5% base +2% per gang, 25% max

    if (g_Dice.percent(chance))
    {
        cRival* rival = g_Game->rivals().GetRandomRivalWithGangs();
        ss << "Your men run into ";
        if (rival && rival->m_NumGangs > 0)    ss << "a gang from " << rival->m_Name;
        else/*                           */    ss << "group of thugs from the streets";
        ss << " and a brawl breaks out.\n";

        sGang rival_gang = gang_manager().GetTempGang();
        rival_gang.give_potions(10);
        int cur_members = gang.m_Num;

        switch(GangBrawl(gang, rival_gang)) {
            case EFightResult::VICTORY:
                ss << "Your men win, but you lost " << cur_members - gang.m_Num << " men. ";
                ss << "\n \n";
                if (rival && rival->m_NumGangs > 0 && rival_gang.m_Num <= 0) rival->m_NumGangs--;

                numlost += startnum - gang.m_Num;
                break;
            case EFightResult::DRAW:
                ss << "After a few bloody exchanges, both gangs decide to look for easier prey. You lost " <<
                cur_members - gang.m_Num << " men. ";
                break;
            case EFightResult::DEFEAT:
                ss << "Your men lose the fight.";
                gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
                return false;
        }
    }
    else if (g_Dice.percent(0))        // `J` added for .06.02.41
    {
        auto girl = g_Game->GetRandomGirl();
        if (girl->has_active_trait("Incorporeal")) girl = g_Game->GetRandomGirl();        // try not to get an incorporeal girl but only 1 check
        if (girl)
        {
            std::string girlName = girl->FullName();
            std::stringstream NGmsg;

            // `J` make sure she is ready for a fight
            if (girl->combat() < 50)        girl->combat(10 + g_Dice % 30);
            if (girl->magic() < 50)            girl->magic(10 + g_Dice % 20);
            if (girl->constitution() < 50)    girl->constitution(10 + g_Dice % 20);
            if (girl->agility() < 50)        girl->agility(10 + g_Dice % 20);
            if (girl->confidence() < 50)    girl->agility(10 + g_Dice % 40);
            girl->health(100);
            girl->tiredness(-100);
            auto health_potion = g_Game->inventory_manager().GetItem("Healing Salve (S)");
            girl->add_item(health_potion, g_Dice.in_range(2, 4));

            ss << "Your men are confronted by a masked vigilante.\n";

            Combat combat(ECombatObjective::KILL, ECombatObjective::CAPTURE);
            combat.add_combatants(ECombatSide::DEFENDER, gang, 50);
            combat.add_combatant(ECombatSide::ATTACKER, *girl);

            // TODO we can add a chance for a sidekick now.

            auto result = combat.run(10);
            auto report = std::make_shared<CombatReport>();
            report->rounds = combat.round_summaries();
            gang.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);
            girl->m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);


            // if the girl has lost
            if (result == ECombatResult::DEFEAT)
            {
                numlost += startnum - gang.m_Num;
                long gold = girl->m_Money > 0 ? girl->m_Money : g_Dice % 100 + 1;    // take all her money or 1-100 if she has none
                girl->m_Money = 0;
                g_Game->gold().petty_theft(gold);

                ss << "She fights ";
                if (numlost > startnum / 2)    ss << "well but your men still manage to capture her";
                else if (numlost == 0)/* */    ss << "your men but loses quickly";
                else/*                   */    ss << "your men but they take her down with only " << (numlost == 1 ? "one casualty" : "a few casualties");
                ss << ".\nThey unmask " << girlName << ", take all her gold (" << gold << ") from her and drag her to the dungeon.\n \n";
                girl->set_stat(STAT_OBEDIENCE, 0);
                girl->add_temporary_trait("Kidnapped", 5 + g_Dice % 11);

                NGmsg << girl->FullName() << " tried to stop " << gang.name() << " from committing petty theft but lost. She was dragged back to the dungeon.";
                gang.BoostSkill(SKILL_COMBAT, 1);

                if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
                {
                    g_Game->get_objective()->m_SoFar += gold;
                }
                if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
                {
                    g_Game->get_objective()->m_SoFar++;    // `J` You are technically kidnapping her
                }
                g_Game->dungeon().AddGirl(girl, DUNGEON_GIRLKIDNAPPED);
                return true;
            }
            else if (result == ECombatResult::DRAW) {
                ss << "After a few bloody exchanges, neither your goons nor the vigilante seem to be eager to see "
                      "this battle to conclusion.";
                gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
                return false;
            }
            else
            {
                ss << "She defeats your men and disappears back into the shadows.";
                gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
                return false;
            }
        }
    }

    int difficulty = std::max(0, g_Dice.bell(1, 6) - 2);    // 0-4
    std::string who = "people";
    int fightbackchance = 0;
    int numberoftargets = 2 + g_Dice % 9;
    int targetfight = numberoftargets;
    long gold = 0;
    long goldbase = 1;

    if (difficulty <= 0)    { who = "kids";                    fightbackchance = 50;    goldbase += 20;        difficulty = 0; }
    if (difficulty == 1)    { who = "little old ladies";    fightbackchance = 40;    goldbase += 40; }
    if (difficulty == 2)    { who = "noble men and women";    fightbackchance = 30;    goldbase += 60; }
    if (difficulty == 3)    { who = "small stalls";            fightbackchance = 50;    goldbase += 80; }
    if (difficulty >= 4)    { who = "traders";                fightbackchance = 70;    goldbase += 100;    difficulty = 4; }

    for (int i = 0; i < numberoftargets; i++)    gold += g_Dice % goldbase;

    if (g_Dice.percent(fightbackchance))    // determine losses if they fight back
    {
        while (gang.m_Num > 0 && targetfight > 0)    // fight until someone wins
        {
            if (g_Dice.percent(gang.combat()))        targetfight--;        // you win so lower their numbers
            else if (g_Dice.percent(g_Dice % 11 + (difficulty * 10)))        // or they win
            {
                if (gang.num_potions() > 0) { gang.use_potion(); }    // but you heal
                else { gang.m_Num--; numlost++; }                            // otherwise lower your numbers
            }
        }
    }

    if (gang.m_Num <= 0) return false;    // they all died so return and the message will be taken care of in the losegang function

    ss << "Your gang robs " << numberoftargets << " " << who << " and get " << gold << " gold from them.";
    if (numlost > 0) { ss << "\n \n" << gang.name() << " lost "; if (numlost == 1) ss << "one man."; else ss << numlost << " men."; }

    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

    g_Game->gold().petty_theft(gold);

    if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
    {
        g_Game->get_objective()->m_SoFar += gold;
    }
    return true;
}


cMissionCatacombs::cMissionCatacombs() : IGangMission("Explore Catacombs", true, true)
{

}

bool cMissionCatacombs::execute_mission(sGang& gang, std::stringstream& ss)
{
    gang.m_Combat = true;
    int num = gang.m_Num;
    ss << "Gang   " << gang.name() << "   is exploring the catacombs.\n \n";

    // use new code
    int totalgirls = 0; int totalitems = 0; int totalbeast = 0;
    int bringbacknum = 0;
    int gold = 0;

    // do the intro text
    ss << cGirls::catacombs_look_for(gang_manager().Gang_Gets_Girls(), gang_manager().Gang_Gets_Items(), gang_manager().Gang_Gets_Beast());

    // do the bring back loop
    while (gang.m_Num >= 1 && bringbacknum < gang.m_Num * std::max(1, gang.strength() / 20))
    {
        double choice = (g_Dice % 10001) / 100.0;
        gold += g_Dice % (gang.m_Num * 20);

        if (choice < gang_manager().Gang_Gets_Girls())                    // get girl = 10 point
        {
            bool gotgirl = false;
            // TODO make this the girl we actually want to get!
            auto tempgirl = g_Game->CreateRandomGirl(18, false, false, true);        // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
            if(AttemptCapture(gang, *tempgirl) == EAttemptCaptureResult::CAPTURED) gotgirl = true;
            if (gotgirl)
            {
                bringbacknum += 10;
                totalgirls++;
            }
            else bringbacknum += 5;
        }
        else if (choice < gang_manager().Gang_Gets_Girls() + gang_manager().Gang_Gets_Items())    // get item = 4 points
        {
            bool gotitem = false;
            if (g_Dice.percent(33))    // item is guarded
            {
                auto tempgirl = g_Game->CreateRandomGirl(18, false, false, true);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
                Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
                combat.add_combatants(ECombatSide::ATTACKER, gang);
                combat.add_combatant(ECombatSide::DEFENDER, *tempgirl);
                if(combat.run(10) == ECombatResult::VICTORY) {
                    gotitem = true;
                }
                auto report = std::make_shared<CombatReport>();
                report->rounds = combat.round_summaries();
                gang.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

                if (g_Dice.percent(20))        { totalitems++; bringbacknum += 2; }
                else if (g_Dice.percent(50))    gold += 1 + g_Dice % 200;
            }
            else gotitem = true;
            if (gotitem)
            {
                bringbacknum += 4;
                totalitems++;
            }
            else bringbacknum += 2;
        }
        else                                    // get beast = 2 point
        {
            bool gotbeast = false;
            if (gang.num_nets() > 0)    // try to capture using net
            {
                while (gang.num_nets() > 0 && !gotbeast)
                {
                    int damagechance = 50;    // higher damage net chance in the catacombs
                    if (g_Dice.percent(gang.combat()))    // hit it with the net
                    {
                        if (!g_Dice.percent(60)) gotbeast = true;
                        else damagechance = 80;
                    }
                    gang.damage_net(damagechance);
                }
            }
            if (!gotbeast)    // fight it
            {
                // the last few members will runaway or allow the beast to run away so that the can still bring back what they have
                while (gang.m_Num > 1 + g_Dice % 3 && !gotbeast)
                {
                    if (g_Dice.percent(std::min(90, gang.combat())))
                    {
                        gotbeast = true;
                        continue;
                    }
                    if (gang.num_potions() == 0)
                    {
                        gang.m_Num--;
                        continue;
                    }
                    // `J` 5% chance they will not get the healing potion in time.
                    // needed to have atleast some chance or else they are totally invincable.
                    if (g_Dice.percent(5)) gang.m_Num--;
                    gang.use_potion();
                }
            }
            if (gotbeast)
            {
                int numbeasts = 1 + g_Dice % 3;
                bringbacknum += numbeasts * 2;
                totalbeast += numbeasts;
            }
            else bringbacknum++;
        }
    }

    // determine loot
    if (gang.m_Num < 1) return false;    // they all died
    else
    {
        if (num == gang.m_Num)    ss << "All " << gang.m_Num << " of them return.\n \n";
        else ss << gang.m_Num << " of the " << num << " who went out return.\n \n";

        if (gold > 0)
        {
            ss << "They bring back with them:   " << gold << " gold\n \n";
            g_Game->gold().catacomb_loot(gold);
        }

        // get catacomb girls (is "monster" if trait not human)
        if (totalgirls > 0)
        {
            ss << "Your men captured " << totalgirls << " girl" << (totalgirls > 1 ? "s" : "") << ":\n";
            for (int i = 0; i < totalgirls; i++)
            {
                std::shared_ptr<sGirl> ugirl = nullptr;
                bool unique = g_Dice.percent(g_Game->settings().get_percent(settings::WORLD_CATACOMB_UNIQUE));    // chance of getting unique girl
                if (unique)
                {
                    ugirl = g_Game->GetRandomGirl(false, true);
                    if (ugirl == nullptr) unique = false;
                }
                if (unique)
                {
                    ss << "   " << ugirl->FullName() << "   (u)\n";
                    ugirl->remove_status(STATUS_CATACOMBS);
                    std::stringstream NGmsg;
                    ugirl->add_temporary_trait("Kidnapped", 2 + g_Dice % 10);
                    NGmsg << ugirl->FullName() << " was captured in the catacombs by " << gang.name() << ".";
                    ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
                    g_Game->dungeon().AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
                }
                else
                {
                    ugirl = g_Game->CreateRandomGirl(0, false, false, true);
                    if (ugirl != nullptr)  // make sure a girl was returned
                    {
                        ss << "   " << ugirl->FullName() << "\n";
                        std::stringstream NGmsg;
                        ugirl->add_temporary_trait("Kidnapped", 2 + g_Dice % 10);
                        NGmsg << ugirl->FullName() << " was captured in the catacombs by " << gang.name() << ".";
                        ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
                        g_Game->dungeon().AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
                    }
                }
                if (ugirl && g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS)
                {
                    g_Game->get_objective()->m_SoFar++;
                }
            }
        }
        ss << "\n";

        // get items
        if (totalitems > 0)
        {
            ss << "Your men bring back " << totalitems << " item" << (totalitems > 1 ? "s" : "") << ":\n";
            for (int i = 0; i < totalitems; i++) {
                sInventoryItem* temp = g_Game->inventory_manager().GetRandomCatacombItem();
                if (temp) {
                    ss << ",\n";
                    if (g_Game->player().inventory().add_item(temp)) {
                        ss << "a " << temp->m_Name;
                    } else {
                        ss << "Your inventory is full\n";
                        break;
                    }
                }
            }
        }
        ss << "\n";

        // bring back any beasts
        if (totalbeast > 0)
        {
            ss << "Your men " << (totalgirls + totalitems > 0 ? "also " : "") << "bring back " << totalbeast << " beasts.";
            g_Game->storage().add_to_beasts(totalbeast);
        }
    }


    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
    return true;
}


cMissionService::cMissionService() : IGangMission("Service", false, false)
{

}

bool cMissionService::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   spend the week helping out the community.";

    int susp = g_Dice.bell(0, 2), fear = g_Dice.bell(0, 2), disp = g_Dice.bell(0, 3), serv = g_Dice.bell(0, 3);
    int cha = 0, intl = 0, agil = 0, mag = 0, gold = 0, sec = 0, dirt = 0, beasts = 0;
    int percent = std::max(1, std::min(gang.m_Num * 5, gang.service()));

    for (int i = 0; i < gang.m_Num / 2; i++)
    {
        if (g_Dice.percent(percent))
        {
            switch (g_Dice % 9)
            {
            case 0:        susp++;        break;
            case 1:        fear++;        break;
            case 2:        disp++;        break;
            case 3:        cha++;        break;
            case 4:        intl++;        break;
            case 5:        agil++;        break;
            case 6:        mag++;        break;
            case 7:        gold += g_Dice % 10 + 1;    break;
            default:    serv++;        break;
            }
        }
    }

    if (gang.m_Num < 15 && g_Dice.percent(std::min(25 - gang.m_Num, gang.charisma())))
    {
        int addnum = std::max(1, g_Dice.bell(-2, 4));
        if (addnum + gang.m_Num > 15)    addnum = 15 - gang.m_Num;
        ss << "\n \n";
        /* */if (addnum <= 1)    { addnum = 1;    ss << "A local boy"; }
        else if (addnum == 2)    { ss << "Two locals"; }
        else /*            */    { ss << "Some locals"; }
        ss << " decided to join your gang to help out their community.";
        gang.m_Num += addnum;
    }

    if (g_Dice.percent(std::max(10, std::min(gang.m_Num * 6, gang.intelligence()))))
    {
        IBuilding* brothel = g_Game->buildings().random_building_with_type(BuildingType::BROTHEL);
        sec = std::max(5 + g_Dice % 26, gang.intelligence() / 4);
        dirt = std::max(5 + g_Dice % 26, gang.service() / 4);
        brothel->m_SecurityLevel += sec;
        brothel->m_Filthiness -= dirt;
        ss << "\n \nThey cleaned up around " << brothel->name()
           << "; fixing lights, removing debris and making sure the area is secure.";
    }
    if (g_Dice.percent(std::max(10, std::min(gang.m_Num * 6, gang.intelligence()))))
    {
        beasts += std::max(1, g_Dice.bell(-4, 4));
        ss << "\n \nThey rounded up ";
        if (beasts <= 1)        { beasts = 1;    ss << "a"; }
        else if (beasts == 2)    { ss << "two"; }
        else/*             */    { ss << "some"; }
        ss << " stray beast" << (beasts > 1 ? "s" : "") << " and brought " << (beasts > 1 ? "them" : "it")
           << " to the brothel" << (g_Game->buildings().num_buildings(BuildingType::BROTHEL) > 1 ? "s" : "") << ".";

        if (g_Dice.percent(beasts * 5))
        {
            std::string itemfound;
            switch (g_Dice % 4)
            {
            case 0:        itemfound = "Black Cat";        break;
            case 1:        itemfound = "Guard Dog";        break;
            default:    itemfound = "Cat";                break;
            }

            sInventoryItem* item = g_Game->inventory_manager().GetItem(itemfound);
            if (item)
            {
                IBuilding* brothel = g_Game->buildings().random_building_with_type(BuildingType::BROTHEL);
                sGirl* girl = brothel->girls().get_random_girl();
                if (girl->add_item(item))                        // see if a girl can take it
                {
                    std::stringstream gss;
                    gss << "While " << gang.name() << " was bringing in the ";
                    if (beasts == 1)
                    {
                        gss << item->m_Name << " they caught, " << girl->FullName() << " stopped them and begged them to let her keep it.";
                    }
                    else
                    {
                        gss << "beasts they captured, " << girl->FullName() << " picked out a ";
                        if (item->m_Name == "Guard Dog")    gss << "dog from the pack";
                        else/*                        */    gss << item->m_Name;
                        gss << " and claimed it for herself.";
                    }
                    girl->m_Events.AddMessage(gss.str(), IMGTYPE_PROFILE, EVENT_GANG);
                    beasts--;
                    ss << "\n" << gss.str() << "\n";
                }
                else if (g_Game->player().inventory().add_item(item))                // otherwise put it in inventory
                {
                    if (beasts == 1)    ss << "\nYou take the " << item->m_Name << " that they captured and put it in a cage in your office.\n";
                    else/*        */    ss << "\nOne of the beasts they catch is a " << item->m_Name << " that looks healthy enough to give to a girl. You have it put in a cage and taken to your office.\n";
                    beasts--;
                }
            }
        }
    }


    ss << "\n";
    if (sec > 0)    { ss << "\nSecurity + " << sec; }
    if (dirt > 0)    { ss << "\nFilthiness - " << dirt; }
    if (beasts > 0)    { ss << "\nBeasts +" << beasts; }
    if (susp > 0)    { g_Game->player().suspicion(-susp);                        ss << "\nSuspicion -" << susp; }
    if (fear > 0)    { g_Game->player().customerfear(-fear);                    ss << "\nCustomer Fear -" << fear; }
    if (disp > 0)    { g_Game->player().disposition(disp);                    ss << "\nDisposition +" << disp; }
    if (serv > 0)    { gang.AdjustGangSkill(SKILL_SERVICE, serv);        ss << "\nService +" << serv; }
    if (cha > 0)    { gang.AdjustGangStat(STAT_CHARISMA, cha);            ss << "\nCharisma +" << cha; }
    if (intl > 0)    { gang.AdjustGangStat(STAT_INTELLIGENCE, intl);    ss << "\nIntelligence +" << intl; }
    if (agil > 0)    { gang.AdjustGangStat(STAT_AGILITY, agil);            ss << "\nAgility +" << agil; }
    if (mag > 0)    { gang.AdjustGangSkill(SKILL_MAGIC, mag);            ss << "\nMagic +" << mag; }
    if (gold > 0)    { g_Game->gold().misc_credit(gold);    ss << "\nThey recieved " << gold << " gold in tips from grateful people."; }

    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
    return true;
}


cMissionTraining::cMissionTraining() : IGangMission("Training", false, false)
{

}

bool cMissionTraining::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   spend the week training and improving their skills.\n \n";

    int old_combat = gang.combat();
    int old_magic = gang.magic();
    int old_intel = gang.intelligence();
    int old_agil = gang.agility();
    int old_const = gang.constitution();
    int old_char = gang.charisma();
    int old_str = gang.strength();
    int old_serv = gang.service();

    std::vector<int*> possible_skills;
    possible_skills.push_back(&gang.m_Skills[SKILL_COMBAT]);
    possible_skills.push_back(&gang.m_Skills[SKILL_MAGIC]);
    possible_skills.push_back(&gang.m_Stats[STAT_INTELLIGENCE]);
    possible_skills.push_back(&gang.m_Stats[STAT_AGILITY]);
    possible_skills.push_back(&gang.m_Stats[STAT_CONSTITUTION]);
    possible_skills.push_back(&gang.m_Stats[STAT_CHARISMA]);
    possible_skills.push_back(&gang.m_Stats[STAT_STRENGTH]);
    possible_skills.push_back(&gang.m_Skills[SKILL_SERVICE]);

    int count = (g_Dice % 3) + 2;  // get 2-4 potential skill/stats to boost
    for (int i = 0; i < count; i++)
    {
        int boost_count = (g_Dice % 3) + 1;  // boost each 1-3 times
        gang.BoostRandomSkill(possible_skills, 1, boost_count);
    }
    possible_skills.clear();

    if (gang.m_Skills[SKILL_COMBAT] > old_combat)        ss << "\n+" << (gang.m_Skills[SKILL_COMBAT] - old_combat) << " Combat";
    if (gang.m_Skills[SKILL_MAGIC] > old_magic)        ss << "\n+" << (gang.m_Skills[SKILL_MAGIC] - old_magic) << " Magic";
    if (gang.m_Stats[STAT_INTELLIGENCE] > old_intel)    ss << "\n+" << (gang.m_Stats[STAT_INTELLIGENCE] - old_intel) << " Intelligence";
    if (gang.m_Stats[STAT_AGILITY] > old_agil)            ss << "\n+" << (gang.m_Stats[STAT_AGILITY] - old_agil) << " Agility";
    if (gang.m_Stats[STAT_CONSTITUTION] > old_const)    ss << "\n+" << (gang.m_Stats[STAT_CONSTITUTION] - old_const) << " Toughness";
    if (gang.m_Stats[STAT_CHARISMA] > old_char)        ss << "\n+" << (gang.m_Stats[STAT_CHARISMA] - old_char) << " Charisma";
    if (gang.m_Stats[STAT_STRENGTH] > old_str)            ss << "\n+" << (gang.m_Stats[STAT_STRENGTH] - old_str) << " Strength";
    if (gang.m_Skills[SKILL_SERVICE] > old_serv)        ss << "\n+" << (gang.m_Skills[SKILL_SERVICE] - old_serv) << " Service";

    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
    gang.m_Combat = false;
    return false;
}

cMissionRecruiting::cMissionRecruiting() : IGangMission("Recruiting", false, false)
{

}

bool cMissionRecruiting::execute_mission(sGang& gang, std::stringstream& ss)
{
    ss << "Gang   " << gang.name() << "   is recruiting.\n \n";
    int recruit = 0;
    int start = g_Dice.bell(1, 6);        // 1-6 people are available for recruitment
    int available = start;
    int add = std::max(0, g_Dice.bell(0, 4) - 1);        // possibly get 1-3 without having to ask
    start += add;
    int disp = g_Game->player().disposition();
    while (available > 0)
    {
        int chance = gang.charisma();
        if (g_Dice.percent(gang.magic() / 4))            chance += gang.magic() / 10;
        if (g_Dice.percent(gang.combat() / 4))            chance += gang.combat() / 10;
        if (g_Dice.percent(gang.intelligence() / 4))    chance += gang.intelligence() / 10;
        if (g_Dice.percent(gang.agility() / 4))        chance += gang.agility() / 10;
        if (g_Dice.percent(gang.constitution() / 4))    chance += gang.constitution() / 10;
        if (g_Dice.percent(gang.strength() / 4))        chance += gang.strength() / 10;

        // less chance of them wanting to work for really evil or really good player
        if (disp < -50)                chance += (disp + 50) / 2;    // -25 for -100 disp
        if (disp > -20 && disp < 0)    chance += (22 + disp) / 2;    // +1 for -19  to +10 for -2
        if (disp == 0)                chance += 10;                // +10 for -2,-1,0,1,2
        if (disp < 20 && disp > 0)    chance += (22 - disp) / 2;    // +1 for 19   to +10 for 2
        if (disp > 50)                chance -= (disp - 50) / 3;    // -16 for > 98 disp

        if (chance > 90) chance = 90;
        if (chance < 20) chance = 20;    // 20-90% chance
        if (g_Dice.percent(chance)) add++;
        available--;
    }

    while (add > recruit && gang.m_Num < 15)
    {
        recruit++;
        gang.m_Num++;
    }

    if (start < 1) ss << "They were unable to find anyone to recruit.";
    else
    {
        ss << "They found "; if (start == 1) ss << "one person"; else ss << start << " people"; ss << " to try to recruit";

        if (start == 1) { if (add == start) ss << " and they got him"; else ss << " but he didn't want"; ss << " to join."; }
        else if (add <= 0)        ss << " but were unable to get any to join.";
        else if (add == start)    ss << " and managed to get all of them to join.";
        else if (add == 1)        ss << " but were only able to convince one of them to join.";
        else                    ss << " and were able to convince " << add << " of them to join.";

        if (gang.m_Num >= 15 && add == recruit) ss << "\nThey got as many as they needed to fill their ranks.";
        else if (gang.m_Num >= 15 && add > recruit)
        {
            gang.m_Num = 15;
            ss << "\nThey only had room for ";
            if (recruit == 1) ss << "one"; else ss << recruit;
            ss << " more in their gang so they ";
            int passnum = add - recruit;
            sGang* passto = gang_manager().GetGangRecruitingNotFull(passnum);
            if (passto)
            {
                ss << "sent the rest to join " << passto->name() << ".";
                std::stringstream pss;
                pss << gang.name() << " sent " << passnum << " recruit" << (passnum > 1 ? "s" : "") << " that they had no room for to " << passto->name();
                int passnumgotthere = 0;
                for (int i = 0; i < passnum; i++)
                {
                    if (passto->m_MissionID == MISS_RECRUIT)    if (g_Dice.percent(75)) passnumgotthere++;
                    if (passto->m_MissionID == MISS_TRAINING)    if (g_Dice.percent(50)) passnumgotthere++;
                    if (passto->m_MissionID == MISS_SPYGIRLS)    if (g_Dice.percent(95)) passnumgotthere++;
                    if (passto->m_MissionID == MISS_GUARDING)    if (g_Dice.percent(30)) passnumgotthere++;
                    if (passto->m_MissionID == MISS_SERVICE)    if (g_Dice.percent(90)) passnumgotthere++;
                }
                if (passnumgotthere > 0)
                {
                    if (passnumgotthere == passnum) pss << ".\nThey " << (passnum > 1 ? "all " : "") << "arrived ";
                    else pss << ".\nOnly " << passnumgotthere << " arrived ";
                    if (passto->m_Num + passnumgotthere <= 15)
                        pss << "and got accepted into the gang.";
                    else
                    {
                        passnumgotthere = 15 - passto->m_Num;
                        pss << "but " << passto->name() << " could only take " << passnumgotthere << " of them.";
                    }
                    passto->m_Num += passnumgotthere;
                }
                else pss << " but none showed up.";
                passto->m_Events.AddMessage(pss.str(), IMGTYPE_PROFILE, EVENT_GANG);
            }
            else ss << "had to turn away the rest.";
        }
    }
    gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
    gang.m_Combat = true;    // though not actually combat, this prevents the automatic +1 member at the end of the week
    return false;
}

cMissionGuarding::cMissionGuarding() : IGangMission("Guarding", false, false)
{

}

cMissionSpyGirls::cMissionSpyGirls() : IGangMission("Spy Girls", false, false)
{

}
