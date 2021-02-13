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

#include "cGirls.h"
#include "cGirlTorture.h"
#include "buildings/cDungeon.h"
#include "character/sGirl.h"
#include "character/cPlayer.h"
#include "cGirlGangFight.h"
#include "IGame.h"
#include "character/traits/ITraitsCollection.h"

extern    cRng            g_Dice;

namespace settings {
    extern const char* TORTURE_INJURY_CHANCE;
}

/*
* ideally, we'd keep a queue of message strings and
* pop them out in order at the end
*/
cGirlTorture::~cGirlTorture()        // deconstructor
{
    int color = COLOR_BLUE;
    if (m_Girl->m_RunAway != 0) color = COLOR_RED;

    // Display any outstanding messages
    if (!m_Message.empty())
    {
        if (m_TorturedByPlayer)
        {
            g_Game->push_message(m_Message, color);
            m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);    // `J` added
        }
        else
        {
            if (m_Girl->health()>0)        // Make sure girl is alive
                m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);
        }
    }
}

cGirlTorture::cGirlTorture(sGirl* a_girl)        // Torture girl by player
{
    // Init for DoTorture()
    m_TorturedByPlayer = true;
    m_Dungeon = &g_Game->dungeon();
    m_Girl = a_girl;
    int nTemp = m_Dungeon->GetGirlPos(m_Girl);
    if (nTemp > -1)
    {
        m_DungeonGirl = m_Dungeon->GetGirl(nTemp);
        DoTorture();
        return;
    }

    // Sanity check for Girl is not in dungeon
    m_Message += m_Girl->FullName();
    m_Message += " cannot be tortured unless in the dungeon!";
}

cGirlTorture::cGirlTorture(sDungeonGirl* a_girl)    // Torture Dungeon girl by player
{
    // Init for DoTorture()
    m_TorturedByPlayer = true;
    m_DungeonGirl = a_girl;
    m_Girl = m_DungeonGirl->m_Girl.get();
    m_Dungeon = &g_Game->dungeon();

    DoTorture();
}

cGirlTorture::cGirlTorture(sDungeonGirl* a_girl, sGirl* Tourturer)
{
    // Init for DoTorture()
    m_TorturedByPlayer = false;
    m_DungeonGirl = a_girl;
    m_Torturer = Tourturer;
    m_Girl = m_DungeonGirl->m_Girl.get();
    m_Dungeon = &g_Game->dungeon();

    DoTorture();
}

void cGirlTorture::DoTorture()
{
    /*
    *    These variables to be initilisied befor calling DoTorture()
    *        bool            m_TorturedByPlayer
    *        sGirl            *m_Girl;
    *        sGirl            *m_Torturer;
    *        sDungeonGirl    *m_DungeonGirl;
    *        Dungeon            *m_Dungeon;
    */

    // Sanity check. Abort on dead girl
    if (m_Girl->is_dead()) return;

    m_Fight = false;

    std::string sGirlName = m_Girl->FullName();
    std::string sMsg;

    // clear down the message and start with her name
    m_Message = "";
    if (m_TorturedByPlayer) m_Message = sGirlName + ": ";

    // WD    Don't allow girls to be tortured by both the Player and the Torturer
    if (m_Girl->m_Tort && !g_Game->allow_cheats())        // only allow this once a week unless cheating
    {
        if (m_TorturedByPlayer) m_Message += "You may only torture someone once per week.\n";
        else m_Message += sGirlName + " has already been tortured this week.\n";
        return;
    }

    // Don't torture new mums
    if (m_Girl->m_JustGaveBirth)
    {
        if (m_TorturedByPlayer)
        {
            m_Message += "She has given birth and has the week off. So she and will not be tortured.\n";
        }
        else
        {
            sMsg = sGirlName + " gave birth and had the week off so was not tortured this week.\n";
            m_Message += "Since " + sGirlName + " gave birth she was not tortured this week.\n";
            //m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
            m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
        }
        return;
    }

    m_Girl->m_Tort = true;
    if (!m_TorturedByPlayer) m_Dungeon->NumGirlsTort(+1);

    /*
    *    OK: she may fight, and if she wins she'll escape
    *
    *    BUG: Intermitent crash if cGirlGangFight() is called when tortured by Girl not Player
    *
    *    WD:    To balance this halve chance of gaining trait in cGirlTorture::add_trait()
    */
    if (m_TorturedByPlayer)
    {
        if (girl_escapes()) return;
    }

    /*
    *    on the grounds that while intending to torture someone is bad actually doing it is worse still...
    *
    *    also if she's not a slave that's a bit worse still
    *    (allowing players who need to discipline their slaves a bit of a break...)
    *
    *    WD    With changes to cPlayer stats how quickly you gain Evil depends on how evil you currently are. Limited to min of 1 point.
    *
    */
    if (m_TorturedByPlayer)
    {
        g_Game->player().evil(m_Girl->is_slave() ? 5 : 10);
    }
    else    // Tortured by Girl
    {
        g_Game->player().evil(m_Girl->is_slave() ? 2 : 4);
    }
    /*
    *    now add one of a number of torture messages...
    */
    if (m_TorturedByPlayer) AddTextPlayer();
    else/*               */    AddTextTorturerGirl();
    /*
    *    check for injury
    */
    if (IsGirlInjured())
    {
        if (m_TorturedByPlayer)
        {
            m_Message += " You were a little too heavy handed and badly injure her.\n";
        }
        else
        {
            sMsg = sGirlName + " was seriously injured in the dungeon this week.\n";
            m_Girl->AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
            m_Torturer->AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
        }
    }
    /*
    *    hmmm... not entirely sure this needs to be a class
    */
    UpdateStats();
    UpdateTraits();

    // `J` wear down rebellion
    m_Girl->raw_traits().decay_temporary_trait("Kidnapped", 1);
    m_Girl->raw_traits().decay_temporary_trait("Emprisoned Customer", 1);

    /*
    *    DANGER DEATH and low health warnings
    *
    */
    if (m_TorturedByPlayer)
    {
        if (m_Girl->is_dead())        // Dead Girl
            m_Message += "She unfortunately died from her wounds.\n";
        else if (m_Girl->health() < 20)
            m_Message += "Also, she is close to death.\n";

    }
    else    // Tortured by Torturer Girl
    {
        if (m_Girl->is_dead())
        {
            sMsg = "While torturing " + sGirlName + " in the dungeon she died from her wounds.";
            m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
        }
        else if (m_Girl->health() < 20)
        {
            // if she is on this low health the tortuer will start feeding again
            if (!m_DungeonGirl->m_Feeding)
            {
                m_DungeonGirl->m_Feeding = true;
                sMsg = m_Torturer->FullName() + " has allowed " + sGirlName +
                       "  to eat because her health was low.\n";
                m_Message += m_Torturer->FullName() +
                             " was allowed her food because her health was low.\n";
                m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_DANGER);
                m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
            }
            else
            {
                sMsg = sGirlName + "'s health is low from ongoing torture.";
                // WD    Low health warnings done as part of cDungeon::Update()
                //m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_DANGER);
                m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
            }

        }
    }
}

void cGirlTorture::AddTextPlayer()
{
    bool was, is;

    int mes = g_Dice.in_range(0, 4);
    switch (mes) {
    case 0:
        m_Message += "you torture her for hours leaving her sobbing.\n";
        break;
    case 1:
        m_Message += "you enjoy giving out all manners of pain imaginable.\n";
        break;
    case 2:
        m_Message += "you place a small wormlike creature called a vorm in her pussy and watch as it painfully sucks fluid from her.\n";
        break;
    case 3:
        m_Message += "after beating her around and using a few torture devices, you sit and watch her crying and cowering in the corner for a time.\n";
        break;
    case 4:
    default:
        m_Message += "you rape her many times making sure she is as uncomfotable as possible.\n";
        /*
        *        see if she was preggers before the rape
        *        check to see if the rape made her pregnant
        *        then check to see if she is afterward
        */
        was = m_Girl->is_pregnant();
        /*
        *        supposed to be checking to see if
        *        she's preg by the goons (at group sex rates)
        *        then again by player. But I don't have a handy
        *        sCustomer so I'm going group rate
        *        and assigning any progeny to the player.
        *        Lazy, I know :)
        */
        m_Girl->calc_pregnancy(&g_Game->player(), 1.5, true);
        is = m_Girl->is_pregnant();
        /*
        *        if she was not, but is now, then the player
        *        just knocked her up. We should mention this.
        */
        if (is && !was)
        {
            m_Message += " She is now pregnant.";
        }
        m_Girl->lose_trait("Virgin");
        break;
    }
}

void cGirlTorture::AddTextTorturerGirl()
{
    /*
    *        Sumary messages for Torture by girl
    */
    if (m_DungeonGirl->m_Feeding)
        m_Message += m_Girl->FullName() + " has been tortured.\n";

    else
        m_Message += m_Girl->FullName() + " has been tortured and starved.\n";

}

void cGirlTorture::UpdateStats()
{
    // do heavy torture
    if (m_Girl->health() > 10)
    {
        m_Girl->health(-5);
        m_Girl->happiness(-5);
        m_Girl->constitution(1);
        m_Girl->confidence(-5);
        m_Girl->obedience(10);
        m_Girl->spirit(-5);
        m_Girl->tiredness(-5);
        m_Girl->pchate(3);
        m_Girl->pclove(-5);
        m_Girl->pcfear(5);
        m_Girl->bdsm(1);
    }

    // do safer torture
    else    //    (m_Girl->health() <= 10)
    {
        m_Girl->happiness(-2);
        m_Girl->confidence(-2);
        m_Girl->obedience(4);
        m_Girl->spirit(-2);
        m_Girl->tiredness(-2);
        m_Girl->pchate(1);
        m_Girl->pclove(-2);
        m_Girl->pcfear(3);
    }
}

bool cGirlTorture::IsGirlInjured()
{
    // Sanity check, Can't get injured
    if (m_Girl->has_active_trait("Incorporeal")) return false;


    /*
    *    WD    Injury was only possible if girl is pregnant or
    *        hasn't got the required traits.
    *
    *        Now check for injury first
    *        Use usigned int so can't pass negative chance
    */
    std::string    sMsg;
    std::string    sGirlName = m_Girl->FullName();
    int nMod = int(100 * (float)g_Game->settings().get_percent(settings::TORTURE_INJURY_CHANCE));

    if (m_Girl->has_active_trait("Fragile")) nMod += nMod;    // nMod *= 2;
    if (m_Girl->has_active_trait("Tough")) nMod /= 2;
    if (nMod < 1) nMod = 1;        // `J` always at least a 1% chance

    // Did the girl get injured
    if (!g_Dice.percent(nMod)) return false;
    /*
    *    INJURY PROCESSING
    *    Only injured girls continue past here
    */

    // Post any outstanding Player messages
    if (m_TorturedByPlayer && !m_Message.empty())
    {
        g_Game->push_message(m_Message, 0);
        m_Girl->AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);    // `J` added

        m_Message = sGirlName + ": ";
    }

    if(m_TorturedByPlayer) {
        cGirls::GirlInjured(*m_Girl, 3, [&](std::string msg){ m_Message += std::move(msg); });
    } else {
        cGirls::GirlInjured(*m_Girl, 3, [&](std::string msg){ MakeEvent(std::move(msg)); });
    }

    // Post any new Player messages in Red Message Box Colour 1
    if (m_TorturedByPlayer && !m_Message.empty() && m_Message != sGirlName + ": ")
    {
        g_Game->push_message(m_Message, COLOR_RED);
        m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_DAYSHIFT);    // `J` added

        m_Message = sGirlName + ": ";
    }

    return true;
}

bool cGirlTorture::girl_escapes()
{
    auto result = AttemptEscape(*m_Girl);
    switch(result) {
        case EGirlEscapeAttemptResult::SUBMITS:
            m_Message += "She doesn't put up a fight so ";
            return false;    // she does NOT escape
        case EGirlEscapeAttemptResult::STOPPED_BY_PLAYER:
            m_Message += "She puts up a fight";
            m_Fight = true;
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_GOONS:
        case EGirlEscapeAttemptResult::SUCCESS:
            break;
    }

    switch (result) {
        case EGirlEscapeAttemptResult::STOPPED_BY_PLAYER:
            m_Message += " and would have escaped but for your personal intervention; ";
            return false;
        case EGirlEscapeAttemptResult::STOPPED_BY_GOONS:
            m_Message += "but goons defeat her.";
            return false;
        case EGirlEscapeAttemptResult::SUCCESS:
            // If girl wins she escapes and leaves the brothel
            m_Message += "And after defeating you as well she escapes to the outside world.\n";
            m_Girl->run_away();
            g_Game->player().evil(5);                            // Add evilness for girl telling the tale
            g_Game->player().suspicion(15);
            return true;
        default:
            assert(0);
    }
}

void cGirlTorture::UpdateTraits()
{
    int nWeekMod = m_DungeonGirl->m_Weeks;
    if (nWeekMod < 1) nWeekMod = 1;

    if (g_Dice.percent(30) && m_Girl->spirit() < 20 && m_Girl->health() < 20)
    {
        m_Girl->add_temporary_trait("Broken Will", int(5 + nWeekMod / 2));
    }
    if (g_Dice.percent(30) && m_Girl->bdsm() > 30)
    {
        m_Girl->add_temporary_trait("Masochist", 10 + nWeekMod);
    }
    if (g_Dice.percent(30) && m_Girl->health() < 10)
    {
        m_Girl->add_temporary_trait("Mind Fucked", 10 + nWeekMod);
    }
}

void cGirlTorture::add_trait(const std::string& trait, int pc)
{
    if (m_Girl->has_active_trait(trait.c_str())) return;
    /*
    *    WD:    To balance a crash bug workaround for Job Torturer
    *        unable to call GirlGangFight()
    *        Halve chance of gaining trait
    */
    if (!m_TorturedByPlayer) pc /= 2;
    if (!g_Dice.percent(pc)) return;

    std::string sMsg = "${name} has gained trait \"" + trait + "\" from being tortured.";

    if (m_TorturedByPlayer)
    {
        g_Game->push_message(sMsg, 2);
        m_Girl->AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
    }
    else MakeEvent(sMsg);

    // Add trait
    m_Girl->gain_trait(trait.c_str());
}

inline void cGirlTorture::MakeEvent(const std::string& sMsg)
{
    m_Girl->AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
    m_Torturer->AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
}

