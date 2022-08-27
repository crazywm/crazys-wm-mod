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
#include <sstream>
#include "character/sGirl.h"
#include "IBuilding.h"
#include "buildings/cBuildingManager.h"
#include "interface/constants.h"

#include "cDungeon.h"
#include "cGangs.h"
#include "cGirlTorture.h"
#include "cObjectiveManager.hpp"
#include "IGame.h"
#include "scripting/GameEvents.h"
#include "CLog.h"
#include "xml/util.h"
#include "cGirlGangFight.h"
#include "character/pregnancy.h"
#include "character/cPlayer.h"

extern cRng                g_Dice;


// strut sDungeonCust
sDungeonCust::sDungeonCust()        // constructor
{
    m_Prev = m_Next = nullptr;
    m_Weeks = 0;
    m_Tort = false;
    m_Feeding = true;
    m_Health = 100;
}

sDungeonCust::~sDungeonCust()        // destructor
{
    if (m_Next) delete m_Next;
    m_Next = nullptr;
}

// strut sDungeonGirl
sDungeonGirl::sDungeonGirl(std::shared_ptr<sGirl> girl) :
    m_Girl(std::move(girl))
{
}

sDungeonGirl::~sDungeonGirl() = default;

// class cDungeon
cDungeon::cDungeon()        // constructor
{
    m_LastDCusts = m_Custs = nullptr;
    m_NumberDied = 0;
    m_NumCusts = 0;
}

cDungeon::~cDungeon()        // destructor
{
    m_Girls.clear();
    delete m_Custs;
    m_LastDCusts = m_Custs = nullptr;
    m_NumberDied = 0;
    m_NumCusts = 0;
}

tinyxml2::XMLElement& cDungeon::SaveDungeonDataXML(tinyxml2::XMLElement * pRoot)// saves all the people (they are stored with the dungeon)
{
    auto& elDungeon = PushNewElement(*pRoot, "Dungeon");

    // save number died
    elDungeon.SetAttribute("NumberDied", m_NumberDied);

    // save girls
    auto& elDungeonGirls = PushNewElement(elDungeon, "Dungeon_Girls");
    for(sDungeonGirl& girl : m_Girls)
    {
        g_LogFile.log(ELogLevel::DEBUG, "Saving Dungeon Girl: ", girl.m_Girl->FullName());

        girl.m_Girl->m_DayJob = girl.m_Girl->m_NightJob = JOB_INDUNGEON;
        auto& elGirl = girl.m_Girl->SaveGirlXML(elDungeonGirls);

        auto& elDungeonData = PushNewElement(elGirl, "Dungeon_Data");
        elDungeonData.SetAttribute("Feeding", girl.m_Feeding);
        elDungeonData.SetAttribute("Reason", girl.m_Reason);
        elDungeonData.SetAttribute("Weeks", girl.m_Weeks);
    }

    g_LogFile.log(ELogLevel::DEBUG, "Saving Customers");
    // save customers
    auto& elDungeonCustomers = PushNewElement(elDungeon, "Dungeon_Customers");
    sDungeonCust* cust = m_Custs;
    while (cust)
    {
        auto& elCustomer = PushNewElement(elDungeonCustomers, "Customer");
        elCustomer.SetAttribute("Feeding", cust->m_Feeding);
        elCustomer.SetAttribute("Tort", cust->m_Tort);
        elCustomer.SetAttribute("HasWife", cust->m_HasWife);
        elCustomer.SetAttribute("NumDaughters", cust->m_NumDaughters);
        elCustomer.SetAttribute("Reason", cust->m_Reason);
        elCustomer.SetAttribute("Weeks", cust->m_Weeks);
        elCustomer.SetAttribute("Health", cust->m_Health);

        cust = cust->m_Next;
    }
    return elDungeon;
}


bool cDungeon::LoadDungeonDataXML(const tinyxml2::XMLElement* pDungeon)    // loads all the people (they are stored with the dungeon)
{
    if (pDungeon == nullptr) { return false; }

    // load number died
    pDungeon->QueryAttribute("NumberDied", &m_NumberDied);

    // load girls
    auto* pDungeonGirls = pDungeon->FirstChildElement("Dungeon_Girls");
    if (pDungeonGirls)
    {
        // load each girl and add her
        for (auto& xml_girl : IterateChildElements(*pDungeonGirls, "Girl"))
        {
            sDungeonGirl girl{std::make_shared<sGirl>(false)};
            bool success = girl.m_Girl->LoadGirlXML(&xml_girl);
            if (success)
            {
                g_LogFile.log(ELogLevel::DEBUG, "Loading Dungeon Girl: ", girl.m_Girl->FullName());
            //    girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_INDUNGEON;

                auto* pDungeonData = xml_girl.FirstChildElement("Dungeon_Data");
                if (pDungeonData)
                {
                    pDungeonData->QueryAttribute("Feeding", &girl.m_Feeding);
                    pDungeonData->QueryIntAttribute("Reason", &girl.m_Reason);
                    pDungeonData->QueryIntAttribute("Weeks", &girl.m_Weeks);
                }

                PlaceDungeonGirl(std::move(girl));
            }
        }
    }

    // load customers
    m_NumCusts = 0;
    g_LogFile.log(ELogLevel::DEBUG, "Loading Customers");
    auto* pDungeonCustomers = pDungeon->FirstChildElement("Dungeon_Customers");
    if (pDungeonCustomers)
    {
        for (auto* pCustomer = pDungeonCustomers->FirstChildElement("Customer"); pCustomer != nullptr; pCustomer = pCustomer->NextSiblingElement("Customer"))
        {
            sDungeonCust* customer = new sDungeonCust();

            pCustomer->QueryAttribute("Feeding", &customer->m_Feeding);
            pCustomer->QueryAttribute("Tort", &customer->m_Tort);
            pCustomer->QueryAttribute("HasWife", &customer->m_HasWife);
            pCustomer->QueryIntAttribute("Reason", &customer->m_Reason);
            pCustomer->QueryIntAttribute("NumDaughters", &customer->m_NumDaughters);
            pCustomer->QueryIntAttribute("Weeks", &customer->m_Weeks);
            pCustomer->QueryIntAttribute("Health", &customer->m_Health);

            PlaceDungeonCustomer(customer);
        }
    }
    return true;
}

void cDungeon::AddGirl(std::shared_ptr<sGirl> girl, int reason)
{
    if (reason == DUNGEON_GIRLKIDNAPPED)
    {
        if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
        {
            g_Game->get_objective()->m_SoFar++;
        }
    }

    girl->m_Building = nullptr;
    girl->m_DayJob = girl->m_NightJob = JOB_INDUNGEON;

    // by this stage they should no longer be a part of any other lists of girls
    sDungeonGirl newPerson{std::move(girl)};
    newPerson.m_Reason = reason;
    PlaceDungeonGirl(std::move(newPerson));
}

void cDungeon::PlaceDungeonGirl(sDungeonGirl newGirl)
{
    m_Girls.emplace_back(std::move(newGirl));
}

void cDungeon::AddCust(int reason, int numDaughters, bool hasWife)
{
    sDungeonCust* newPerson = new sDungeonCust();
    newPerson->m_NumDaughters = numDaughters;
    newPerson->m_HasWife = hasWife;
    newPerson->m_Reason = reason;

    PlaceDungeonCustomer(newPerson);
}

void cDungeon::PlaceDungeonCustomer(sDungeonCust *newCust)
{
    if (m_Custs)
    {
        newCust->m_Prev = m_LastDCusts;
        m_LastDCusts->m_Next = newCust;
        m_LastDCusts = newCust;
    }
    else
    {
        m_LastDCusts = m_Custs = newCust;
    }

    m_NumCusts++;
}

int cDungeon::GetGirlPos(sGirl* girl)
{
    if (girl == nullptr || m_Girls.empty()) return -1;

    int count = 0;
    for(auto& current : m_Girls)
    {
        if (current.m_Girl.get() == girl) return count;
        count++;
    }
    return -1;
}

std::shared_ptr<sGirl> cDungeon::RemoveGirl(sGirl* girl)    // this returns the girl, it must be placed somewhere or deleted
{
    for(auto& current : m_Girls)
    {
        if (current.m_Girl.get() == girl) {
            return RemoveGirl(&current);
        }
    }
    return nullptr;
}

std::shared_ptr<sGirl> cDungeon::RemoveGirl(sDungeonGirl* girl)    // this returns the girl, it must be placed somewhere or deleted
{
    girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_RESTING;

    std::shared_ptr<sGirl> girlData = std::move(girl->m_Girl);
    girl->m_Girl = nullptr;

    // remove from girls list
    m_Girls.erase(std::remove_if(begin(m_Girls), end(m_Girls), [girl](sDungeonGirl& g){ return &g == girl; }),
            end(m_Girls));

    return std::move(girlData);
}

void cDungeon::RemoveCust(sDungeonCust* cust)
{
    if (cust == nullptr) return;
    if (cust->m_Prev)            cust->m_Prev->m_Next = cust->m_Next;
    if (cust->m_Next)            cust->m_Next->m_Prev = cust->m_Prev;
    if (cust == m_LastDCusts)    m_LastDCusts = cust->m_Prev;
    if (cust == m_Custs)        m_Custs = cust->m_Next;
    cust->m_Next = cust->m_Prev = nullptr;
    delete cust;
    cust = nullptr;
    m_NumCusts--;
}

/// Given a name of a detail (stat, skill, trait, etc.), returns its
/// value as itself (`.val_`) and as a formatted string (`.fmt_`).
FormattedCellData sDungeonGirl::GetDetail(const std::string& detailName) const
{
    if (detailName == "Rebelliousness")
    {
        // `J` Dungeon "Matron" can be a Torturer from any brothel
        bool has_torturer = (random_girl_on_job(g_Game->buildings(), JOB_TORTURER, false) != nullptr);
        return mk_num(cGirls::GetRebelValue(*m_Girl));
    }
    else if (detailName == "Reason")
    {
        switch (m_Reason)
        {
           case DUNGEON_GIRLCAPTURED:     return mk_text("Newly Captured.");
           case DUNGEON_GIRLKIDNAPPED:    return mk_text("Taken from her family.");
           case DUNGEON_GIRLWHIM:         return mk_text("Your whim.");
           case DUNGEON_GIRLSTEAL:        return mk_text("Not reporting true earnings.");
           case DUNGEON_GIRLRUNAWAY:      return mk_text("Ran away and re-captured.");
           case DUNGEON_NEWSLAVE:         return mk_text("This is a new slave.");
           case DUNGEON_NEWGIRL:          return mk_text("This is a new girl.");
           case DUNGEON_KID:              return mk_text("Child of one of your girls.");
           case DUNGEON_NEWARENA:         return mk_text("This is a girl won in the arena.");
           case DUNGEON_RECRUITED:        return mk_text("This girl was recruited for you.");
           case DUNGEON_CUSTNOPAY:        return mk_text("Not paying.");
           case DUNGEON_DEAD:             return mk_text("DEAD");
           default:                       return mk_error("(error)");
        }
    }
    else if (detailName == "Duration")    return mk_num(m_Weeks);
    else if (detailName == "Feeding")     return mk_yesno(m_Feeding);
    else if (detailName == "Tortured")    return mk_yesno(m_Girl->m_Tort);
    else if (detailName == "Kidnapped")
    {
        // TODO (traits) figure out something here!
#if 0
        std::ostringstream ss;

        auto info = m_Girl->raw_traits().get_trait_info("Kidnapped");
        if (info.trait && info.remaining_time > 0) ss << info.remaining_time;
        else ss << "-";
#else
        return mk_text("-");
#endif
    }
    else
    {
        return m_Girl->GetDetail(detailName);
    }
}

/// Given a name of a detail (stat, skill, trait, etc.), returns its
/// value as itself (`.val_`) and as a formatted string (`.fmt_`).
FormattedCellData sDungeonCust::GetDetail(const std::string& detailName) const
{
    if (detailName == "Name")           return mk_text("Customer");
    else if (detailName == "Health")    return mk_health(m_Health);
    else if (detailName == "Reason")
    {
        switch (m_Reason)
        {
        case DUNGEON_CUSTNOPAY:         return mk_text("Not paying.");
        case DUNGEON_CUSTBEATGIRL:      return mk_text("Beating your girls.");
        case DUNGEON_CUSTSPY:           return mk_text("Being a rival's spy.");
        case DUNGEON_RIVAL:             return mk_text("Is a rival.");
        case DUNGEON_DEAD:             return mk_text("DEAD");
        default:                        return mk_error("(error)");
        }
    }
    else if (detailName == "Duration")  return mk_num((int)m_Weeks);
    else if (detailName == "Feeding")   return mk_yesno(m_Feeding);
    else if (detailName == "Tortured")  return mk_yesno(m_Tort);
    else                                return mk_error("---");
}

sDungeonGirl* cDungeon::GetGirl(int i)
{
    if (i < 0)                    { i = i + m_Girls.size(); }
    if (i >= m_Girls.size())    { i = i - m_Girls.size(); }

    int tmp = 0;
    for(auto& current : m_Girls)
    {
        if (tmp == i) { return &current; };
        tmp++;
    }
    return nullptr;
}

sDungeonGirl* cDungeon::GetGirlByID(std::uint64_t id)
{
    auto result = std::find_if(begin(m_Girls), end(m_Girls),
                               [&](auto&& g){ return id == g.m_Girl->GetID(); });
    if(result != end(m_Girls)) {
        return &(*result);
    }
    return nullptr;
}

sDungeonCust* cDungeon::GetCust(int i)
{
    sDungeonCust* cust = m_Custs;
    int tmp = 0;
    while (cust)
    {
        if (tmp == i) break;
        tmp++;
        cust = cust->m_Next;
    }
    return cust;
}

void cDungeon::ClearDungeonGirlEvents()
{
    for(auto& current : m_Girls) {
        // Clear the girls' events from the last turn
        current.m_Girl->GetEvents().Clear();
    }
}


void cDungeon::Update()
{
    /*
    *    WD: GetNumGirlsOnJob() not testing if the girl worked
    *
    */
    sGirl* TorturerGirlref = nullptr;
    std::string girlName;
    std::stringstream msg;
    std::stringstream ss;

    // Reset counters
    m_NumGirlsTort = m_NumCustsTort = 0;

    // WD:    Did we torture the girls
    // WD: If so, who is the Torturer
    if (m_TortureDone) {
        TorturerGirlref = random_girl_on_job(g_Game->buildings(), JOB_TORTURER, false);
        if(!TorturerGirlref) {
            g_Game->error("ERROR: Cannot find the torturer! How did this happen?");
            m_TortureDone = false;
        }
    }

    /*********************************
    *    DO ALL DUNGEON GIRLS
    *********************************/
    for(auto& current : m_Girls)
    {
        sGirl* girl = current.m_Girl.get();
        // TODO when can this happen???
        if(!girl) continue;
        girl->save_statistics();

        //            girl->m_Tort = false;// WD: Move till after Girls have been tortured so that we dont torture twice week
        girlName = girl->FullName();
        ss.str("");
        msg.str("");

        // Check for dead girls
        if (girl->is_dead())
        {
            // remove dead bodies from last week
            if (current.m_Reason == DUNGEON_DEAD)
            {
                sDungeonGirl* temp = &current;
                msg << girlName << "'s body has been removed from the dungeon since she was dead.";
                g_Game->push_message(msg.str(), COLOR_RED);
                RemoveGirl(temp);
                continue;
            }
            // Mark as dead
            else
            {
                m_NumberDied++;
                OnGirlDead(current);
                continue;
            }
        }
        /*
        *            DAILY Processing
        */
        std::string summary;

        current.m_Weeks++;                        // the number of weeks they have been in the dungeon
        cGirls::CalculateGirlType(*girl);        // update the fetish traits
        cGirls::updateGirlAge(*girl, true);        // update birthday counter and age the girl
        cGirls::updateTemp(*girl);            // update temp stuff
        cGirls::EndDayGirls(g_Game->buildings().get_building(0), *girl);
        handle_children(*girl, summary, true);            // handle pregnancy and children growing up
        cGirls::updateSTD(*girl);                // health loss to STD's - NOTE: Girl can die
        cGirls::updateHappyTraits(*girl);        // Update happiness due to Traits - NOTE: Girl can die
        updateGirlTurnDungeonStats(&current);    // Update stats
        cGirls::updateGirlTurnStats(*girl);        // Stat Code common to Dugeon and Brothel

        // Check again for dead girls
        if (girl->health() <= 0)
        {
            m_NumberDied++;
            OnGirlDead(current);

            msg.str(""); ss.str("");
            girl->AddMessage("${name} has died in the dungeon.", EImageBaseType::DEATH, EVENT_DANGER);
            girl->AddMessage("${name} has died.  Her body will be removed by the end of the week.\n", EImageBaseType::DEATH, EVENT_SUMMARY);

            // if there is a torturer send her a message
            if (m_TortureDone)
            {
                msg.str("");
                msg << girlName << " has died in the dungeon under her care!";
                TorturerGirlref->AddMessage(msg.str(), EImageBaseType::DOM, EVENT_DUNGEON);
            }

            continue;
        }

        // Have dungeon girls tortured by the Torturer
        if (m_TortureDone)
        {
            cGirlTorture gt(&current, TorturerGirlref);
        }

        /*        `J` merged WARNING MESSAGES and SUMMARY MESSAGES
        *            Allow girl sorting in turn summary
        */
        //    `J` set the basics
        msg.str(""); ss.str("");
        msg << girlName << " is languishing in the dungeon.\n \n";
        EventType msgtype = EVENT_DUNGEON;
        EImageBaseType imgtype = EImageBaseType::JAIL;
        int    nHealth = girl->health();
        int    nTired = girl->tiredness();

        //    `J` check them for dangers or warnings
        if (nHealth < 20 || nTired > 80)
        {
            msg << "DANGER: " << girlName;
            msgtype = EVENT_DANGER;
        }
        else if (nHealth < 40 || nTired > 60)
        {
            msg << "WARNING: " << girlName;
            msgtype = EVENT_WARNING;
        }

        //    `J` did msgtype change?
        if (msgtype != EVENT_DUNGEON)
        {
            if (girl->m_Tort)
            {
                msg << " was tortured this week.";
                imgtype = EImageBaseType::TORTURE;
                if (nHealth < 40 || nTired > 60)    { msg << "\nShe"; }
            }
            if (nHealth < 20)                        { msg << " is severely injured"; }
            else if (nHealth < 40)                    { msg << " is injured"; }
            if (nHealth < 40 && nTired > 60)        { msg << " and"; }
            else if (nTired > 60)                    { msg << " is"; }
            else                                    { msg << "."; }
            if (nTired > 80)                        { msg << " exhausted, it may effect her health."; }
            else if (nTired > 60)                    { msg << " tired."; }
            msg << "\n \nHer health is " << nHealth << ".\nHer tiredness is " << nTired << ".";
        }
        girl->AddMessage(msg.str(), imgtype, msgtype);

        girl->m_Tort = false;
    }

    /*
    *            WD:  Torturer Girl summary
    *                Processed after all dGirls
    */
    if (m_TortureDone && !m_Girls.empty())
    {
        msg.str("");
        msg << TorturerGirlref->FullName() << " has tortured " << m_NumGirlsTort << " girls in the Dungeon.";
        TorturerGirlref->AddMessage(msg.str(), EImageBaseType::DOM, EVENT_DUNGEON);
    }



    /*********************************
    *    DO ALL CUSTOMERS
    *********************************/
    if (m_Custs)
    {
        sDungeonCust* current = m_Custs;
        while (current)
        {
            current->m_Tort = false;
            if (current->m_Health <= 0)
            {
                OnCustomerDead(current);
            }
            if (current->m_Reason == DUNGEON_DEAD)
            {
                sDungeonCust* temp = current;
                current = current->m_Next;
                RemoveCust(temp);
                continue;
            }

            /*
            *            lose health if not feeding
            *
            *            Mod: removed "no-effect" branch to silence
            *            compiler
            */
            if (!current->m_Feeding) current->m_Health -= 5;

            current->m_Weeks++;
            if (current->m_Health <= 0)
            {
                m_NumberDied++;
                current->m_Reason = DUNGEON_DEAD;
                OnCustomerDead(current);
            }
            current = current->m_Next;
        }
    }
}

void cDungeon::OnGirlDead(sDungeonGirl& current) const {
    current.m_Reason = DUNGEON_DEAD;
    g_Game->push_message("A girl has died in the dungeon.\nHer body will be removed by the end of the week.", COLOR_RED);

    if (g_Dice.percent(10))	// only 10% of being discovered
    {
        g_Game->player().suspicion(1);
    }
    g_Game->player().disposition(-1);
    for(auto& bld : g_Game->buildings().buildings()) {
        bld->girls().apply([](sGirl& g){
            g.pcfear(2);
        });
    };
}

void cDungeon::OnCustomerDead(sDungeonCust* current) {
    m_NumberDied++;
    current->m_Reason = DUNGEON_DEAD;
    g_Game->push_message("A customer has died in the dungeon.\nTheir body will be removed by the end of the week.", COLOR_RED);

    if (g_Dice.percent(10))	// only 10% chance of being found out
    {
        g_Game->player().suspicion(1);
    }
    g_Game->player().disposition(-1);
    g_Game->player().customerfear(1);
}

void cDungeon::updateGirlTurnDungeonStats(sDungeonGirl* d_girl)
{
    /*
    *    WD: Update each turn the stats for girls in dudgeon
    */

    sGirl* girl = d_girl->m_Girl.get();
    std::string msg;
    std::string girlName = girl->FullName();

    // Sanity check. Abort on dead girl
    if (girl->is_dead()) return;

    if (d_girl->m_Feeding)
    {
        girl->tiredness(-10);
        girl->pcfear(4);
        girl->pclove(-1);
        girl->mana(5);
        girl->bdsm(1);

        if (girl->is_slave())    // Slave being fed
        {
            girl->confidence(-2);
            girl->obedience(2);
            girl->spirit(-2);
            girl->pclove(-1);
            girl->happiness(-1);
            girl->health(4);
        }
        else    // Free girl being fed
        {
            girl->confidence(-1);
            girl->obedience(1);
            girl->spirit(-1);
            girl->pclove(-4);
            girl->happiness(-5);
            girl->health(1);
        }
    }
    else    // feeding off
    {
        girl->confidence(-2);
        girl->obedience(2);
        girl->spirit(-2);
        girl->health(-5);
        girl->mana(1);
        girl->bdsm(2);
        if (girl->is_slave())    // Slave being starved
        {
            girl->pclove(-1);
            girl->pclove(-2);
            girl->pcfear(4);
            girl->tiredness(1);
            girl->happiness(-3);
        }
        else    // Free girl being starved
        {
            girl->pclove(-4);
            girl->pclove(-5);
            girl->pcfear(6);
            girl->tiredness(2);
            girl->happiness(-5);
        }
    }
}

void cDungeon::SetTortureDone()
{
    m_TortureDone = true;
}

bool cDungeon::SendGirlToDungeon(std::shared_ptr<sGirl> girl)
{
    std::stringstream ss;
    bool success = true;
    int reason = girl->m_Spotted ? DUNGEON_GIRLSTEAL : DUNGEON_GIRLWHIM;

    auto result = AttemptEscape(*girl);
    switch(result) {
        case EGirlEscapeAttemptResult::SUBMITS:
            ss << "She goes quietly with a sullen look on her face.";
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_GOONS:
            ss << "She puts up a fight ";
            ss << "but your goons manage to drag her unconscious to the dungeon.";
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_PLAYER:
            ss << "She manages to defeat your goons, but your personal intervention prevented her from escaping! ";
            ss << "You place her unconscious body in the dungeon.";
            break;
        case EGirlEscapeAttemptResult::SUCCESS:
            ss << "After defeating you goons and you, she escapes to the outside.\n";
            ss << "She will escape for good in 6 weeks if you don't send someone after her.";
            girl->run_away();
            success = false;
            std::stringstream smess;
            smess << girl->FullName() << " has run away";
            g_Game->push_message(smess.str(), 1);
    }
    g_Game->push_message(ss.str(), 0);

    if (success)
        g_Game->dungeon().AddGirl(std::move(girl), reason);
    return success;
}

void cDungeon::SetFeeding(int num, bool allow) {
    if ((num - GetNumGirls()) >= 0)    // it is a customer
    {
        GetCust(num - GetNumGirls())->m_Feeding = allow;
    }
    else
    {
        GetGirl(num)->m_Feeding = allow;
    }
}

void cDungeon::ReleaseGirl(int index, IBuilding& target) {
    auto girl = RemoveGirl(GetGirl(index));
    target.add_girl(std::move(girl));
}
