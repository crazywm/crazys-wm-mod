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

#include <cmath>
#include "character/cPlayer.h"
#include "XmlMisc.h"
#include "cGirls.h"
#include "cInventory.h"
#include "CLog.h"
#include "IGame.h"
#include "character/sGirl.h"
#include "xml/util.h"
#include "ICharacter.h"
#include "character/traits/ITraitsCollection.h"


namespace settings {
    extern const char* USER_SHOW_NUMBERS;
}


cPlayer::cPlayer(std::unique_ptr<ITraitsCollection> tc)    : ICharacter(std::move(tc))        // constructor
{
    m_PlayerGender = GENDER_MALE;

    for (auto& m_Skill : m_Skills)    m_Skill.m_Value = 0;
    for (auto& m_Stat : m_Stats)    m_Stat.m_Value = 0;
    set_stat(STAT_HEALTH, 100);
    set_stat(STAT_HAPPINESS, 100);
    SetToZero();
}

void cPlayer::SetToZero()
{
    m_CustomerFear = m_Disposition = m_Suspicion = 0;
    m_WinGame = false;
}

tinyxml2::XMLElement& cPlayer::SavePlayerXML(tinyxml2::XMLElement& elRoot)
{
    auto& elPlayer = PushNewElement(elRoot, "Player");
    // save the player
    elPlayer.SetAttribute("RealName", m_FullName.c_str());
    elPlayer.SetAttribute("FirstName", m_FirstName.c_str());
    elPlayer.SetAttribute("Surname", m_Surname.c_str());
    elPlayer.SetAttribute("BirthMonth", m_BirthMonth);
    elPlayer.SetAttribute("BirthDay", m_BirthDay);

    // Save their stats
    SaveXML(elPlayer);
    // save other player stuff
    elPlayer.SetAttribute("Suspicion", m_Suspicion);
    elPlayer.SetAttribute("Disposition", m_Disposition);
    elPlayer.SetAttribute("CustomerFear", m_CustomerFear);
    elPlayer.SetAttribute("WinGame", m_WinGame);// have they won the game
    return elPlayer;
}


bool cPlayer::LoadPlayerXML(const tinyxml2::XMLElement* Player)
{
    SetToZero();//init to 0
    if (Player == nullptr) return false;
    if (Player->Attribute("RealName")) m_FullName    = Player->Attribute("RealName");
    if (Player->Attribute("FirstName")) m_FirstName   = Player->Attribute("FirstName");
    if (Player->Attribute("Surname")) m_Surname     = Player->Attribute("Surname");
    if (Player->Attribute("BirthMonth"))        Player->QueryIntAttribute("BirthMonth", &m_BirthMonth);
    if (Player->Attribute("BirthDay"))        Player->QueryIntAttribute("BirthDay", &m_BirthDay);

    // stats
    LoadXML(*Player);
    // load other player stuff
    Player->QueryIntAttribute("Suspicion", &m_Suspicion);
    Player->QueryIntAttribute("Disposition", &m_Disposition);
    Player->QueryIntAttribute("CustomerFear", &m_CustomerFear);
    Player->QueryAttribute("WinGame", &m_WinGame);// have they won the game
    return true;
}

inline int cPlayer::Limit100(int nStat)
{
    if (nStat > 100) nStat = 100;
    else if (nStat < -100) nStat = -100;
    return nStat;
}

int    cPlayer::Scale200(int nValue, int nStat)
{
    /*
    *    WD    Scale the value n so that if adjusting the value will have
    *        less effect as you approch the max or min values.
    *
    *    eg    if you are EVIL additional evil acts will only subtract 1 but
    *        if you are GOOD an evil act will subtract nVal from Disposition
    *
    *        This will slow down the changes in player stats as you near the
    *        end of the ranges.
    */
    //printf("cPlayer::Scale200 nValue = %d, nStat = %d.\n", nValue, nStat);
    if (nValue == 0) return 0;                    // Sanity check
    bool bSign = nValue >= 0;
    nStat += 100;                        // set stat to value between 0 and 200
    if (bSign) nStat = 200 - nStat;                            // Adjust for adding or subtraction
    double    fRatio = nStat / 200.0;
    int        nRetValue = static_cast <int> (nValue * fRatio);

    //printf("cPlayer::Scale200 nRetValue = %d, fRatio = %.2f.\n \n", nRetValue, fRatio);
    if (abs(nRetValue) > 1) return nRetValue;                        // Value is larger than 1                
    return (bSign ? 1 : -1);
}

int cPlayer::disposition(int n)
{
    n = Scale200(n, m_Disposition);
    m_Disposition = Limit100(m_Disposition + n);
    return m_Disposition;
}
int cPlayer::evil(int n)
{
    return disposition(-1 * n);
}

int cPlayer::suspicion(int n)
{
    n = Scale200(n, m_Suspicion);
    m_Suspicion = Limit100(m_Suspicion + n);
    return m_Suspicion;
}

int cPlayer::customerfear(int n)
{
    n = Scale200(n, m_CustomerFear);
    m_CustomerFear = Limit100(m_CustomerFear + n);
    return m_CustomerFear;
}


std::string cPlayer::SetTitle(std::string title)
{
    m_Title = std::move(title);
    return m_Title;
}

bool cPlayer::CanImpregnateFemale()
{
    return (m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
            m_PlayerGender == GENDER_HERM || m_PlayerGender == GENDER_MALE);
}
bool cPlayer::CanCarryOwnBaby()
{
    if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FUTA ||
        m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL)
        return true;
    return false;
}
bool cPlayer::CanCarryNormalBaby()
{
    if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FEMALENEUT ||
        m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
        m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
        m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM)
        return true;
    return false;
}
bool cPlayer::HasPenis()
{
    if (m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
        m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
        m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM ||
        m_PlayerGender == GENDER_MALENEUT || m_PlayerGender == GENDER_MALE)
        return true;
    return false;
}
bool cPlayer::HasVagina()
{
    if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FEMALENEUT ||
        m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
        m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
        m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM)
        return true;
    return false;
}
bool cPlayer::HasTestes()
{
    if (m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
        m_PlayerGender == GENDER_HERM || m_PlayerGender == GENDER_MALE)
        return true;
    return false;
}
bool cPlayer::HasOvaries()
{
    if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FUTA ||
        m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL)
        return true;
    return false;
}


void cPlayer::SetGender(int x)
{
    /* */if (x < GENDER_FEMALE)    m_PlayerGender = GENDER_FEMALE;
    else if (x > GENDER_MALE)    m_PlayerGender = GENDER_MALE;
    else /*                 */    m_PlayerGender = x;
}

void cPlayer::AdjustGender(int male, int female)
{
    if (male == 0 && female == 0)            return;                                // no change

    // do the easy stuff first
    else if (male <= 0 && female >= 5)        { m_PlayerGender = GENDER_FEMALE;        return; }    // force female
    else if (male >= 5 && female <= 0)        { m_PlayerGender = GENDER_MALE;            return; }    // force male
    else if (male <= -3 && female <= -3)    { m_PlayerGender = GENDER_NONE;            return; }    // force andro
    else if (male >= 2 && female >= 2)
    {
        if (m_PlayerGender < 6)/*       */    { m_PlayerGender = GENDER_FUTAFULL;        return; }    // everything on base female
        else /*                         */    { m_PlayerGender = GENDER_HERMFULL;        return; }    // everything on base male
    }
    else if (male <= -2 && female <= -2)
    {
        if (m_PlayerGender < 6)/*     */    { m_PlayerGender = GENDER_NONEFEMALE;    return; }    // nothing on base female
        else /*                       */    { m_PlayerGender = GENDER_NONEMALE;        return; }    // nothing on base male
    }

    /*
    GENDER_FEMALE      = 0;     0 0 1 1
    GENDER_FEMALENEUT  = 1;     0 0 1 0
    GENDER_FUTA        = 2;     1 0 1 1
    GENDER_FUTANEUT    = 3;     1 0 1 0
    GENDER_FUTAFULL    = 4;     1 1 1 1
    GENDER_NONEFEMALE  = 5;     0 0 0 0
    GENDER_NONE        = 6;     0 0 0 0
    GENDER_NONEMALE    = 7;     0 0 0 0
    GENDER_HERMFULL    = 8;     1 1 1 1
    GENDER_HERMNEUT    = 9;     1 0 1 0
    GENDER_HERM        = 10; 1 1 1 0
    GENDER_MALENEUT    = 11; 1 0 0 0
    GENDER_MALE        = 12; 1 1 0 0
    */


#if 0 // `J` - this needs a ton more work before all outcomes will work so not using it for now
    switch (m_PlayerGender)
    {
    case GENDER_FEMALE:
    {
        /* */if (male <= 0 && female >= 0)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= 0 && female == -1)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male == 1    && female >= 0)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male == 1    && female == -1)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male == 2    && female >= 0)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= 0    && female == -2)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= -1    && female <= -3)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male == 0    && female <= -3)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male >= #    && female == #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= #    && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= #    && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= #    && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= #    && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_FEMALENEUT:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_FUTA:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_FUTANEUT:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_FUTAFULL:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_NONEFEMALE:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_NONE:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_NONEMALE:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_HERMFULL:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_HERMNEUT:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_HERM:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_MALENEUT:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    case GENDER_MALE:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
    default:
    {
        /* */if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALE;            // 0 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FEMALENEUT;        // 0 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTA;            // 1 0 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTANEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_FUTAFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEFEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONE;            // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_NONEMALE;        // 0 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMFULL;        // 1 1 1 1
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERMNEUT;        // 1 0 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_HERM;            // 1 1 1 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALENEUT;        // 1 0 0 0
        else if (male <= # && female <= #)/*  */    m_PlayerGender = GENDER_MALE;            // 1 1 0 0
        return;
    }
#endif


    }

std::string cPlayer::disposition_text() const
{
    std::stringstream ss;
    if (m_Disposition >= 100)    ss << "Saint";
    else if (m_Disposition >= 80)    ss << "Benevolent";
    else if (m_Disposition >= 50)    ss << "Nice";
    else if (m_Disposition >= 10)    ss << "Pleasant";
    else if (m_Disposition >= -10)    ss << "Neutral";
    else if (m_Disposition >= -50)    ss << "Not nice";
    else if (m_Disposition >= -80)    ss << "Mean";
    else ss << "Evil";
    if (g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS)) ss << " (" << m_Disposition << ")";
    return ss.str();
}

std::string cPlayer::suss_text() const
{
    std::stringstream ss;
    if (m_Suspicion >= 80)        ss << "Town Scum";
    else if (m_Suspicion >= 50)        ss << "Miscreant";
    else if (m_Suspicion >= 10)        ss << "Suspect";
    else if (m_Suspicion >= -10)    ss << "Unsuspected";
    else if (m_Suspicion >= -50)    ss << "Lawful";
    else if (m_Suspicion >= -80)    ss << "Philanthropist";
    else /*                               */    ss << "Town Hero";
    if (g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS))            ss << " (" << m_Suspicion << ")";
    return ss.str();
}

void cPlayer::AutomaticFoodItemUse(sGirl& girl, const char* item_name, std::string message)
{
    if(!has_item(item_name))
        return;

    auto item = g_Game->inventory_manager().GetItem(item_name);
    girl.add_item(item);
    girl.equip(item, true);
    remove_item(item);
    girl.AddMessage(message, EImageBaseType::DEATH, EVENT_WARNING);
}

void cPlayer::AutomaticItemUse(sGirl& girl, const char* item_name, std::string message)
{
    if(!has_item(item_name))
        return;

    auto item = g_Game->inventory_manager().GetItem(item_name);

    if (girl.can_equip(item))  // Don't force equipment
    {
        girl.add_item(item);
        remove_item(item);
        girl.equip(item, false);
        girl.AddMessage(message, EImageBaseType::PROFILE, EVENT_WARNING);
    }
}

bool cPlayer::AutomaticSlotlessItemUse(sGirl& girl, const char* item_name, std::string message)
{
    // Slotless items include manuals, stripper poles, free weights, etc...
    if(!has_item(item_name))
        return false;

    auto item = g_Game->inventory_manager().GetItem(item_name);
    girl.add_item(item);
    remove_item(item);
    girl.equip(item, false);
    girl.AddMessage(message, EImageBaseType::DEATH, EVENT_WARNING);
    return true;
}

void cPlayer::apply_items(sGirl& girl) {
    int PolishCount = 0;
    
    /* Automatic item use - to stop the monotonous work.
    (I started writing this for my test game where I had 6 brothels with
    125+ girls in each. 16 of them were full time catacombs explorers.)

    Food type items are forced. Actual pieces of equipment are not.
    The players equipment choices should always be respected.

    There are a number of things this function specifically DOES NOT do:
    1. Use skill raising items.
    2. Cure diseases like aids and syphilis.
    3. Cure addictions like shroud and fairy dust.
    4. Use temporary items.
    5. Use items related to pregnancy, insemenation or children

    I should qualify this by saying, "It doesn't directly raise stats, cure
    diseases and addictions." They can happen indirectly as a piece of equipment
    equipped for a stat boost or trait may also raise skills. Similarily a
    item used to cure some condition (like an Elixir of Ultimate Regeneration
    curing one-eye or scars)  may also cure a disease or addiction as well.

    The way this is currently written it shouldn't be released as part
    of the game. It makes too many choices for the player. Perhaps we can
    make it into a useful game function somehow. Regardless, this can be
    disabled by commenting out a single line in UpdateGirls.
    */

    // ------------ Part 1: Stats -------------

# pragma region automation_stats

    // Health

    // Healing items are wasted on constructs as the max. 4% applies to both damage and
    // healing
    bool is_constructed = girl.has_active_trait("Construct");
    if (girl.health() <= 25 && !is_constructed)
        AutomaticFoodItemUse(girl, "Healing Salve (L)", "Used a large healing salve to stay healthy.");

    if (girl.health() <= 50 && !is_constructed)
        AutomaticFoodItemUse(girl, "Healing Salve (M)", "Used a medium healing salve to stay healthy.");

    if (girl.health() <= 75 && !is_constructed)
        AutomaticFoodItemUse(girl, "Healing Salve (S)", "Used a small healing salve to stay healthy.");

    // Tiredness/fatigue
    if (girl.tiredness() >= 75 && !is_constructed)
        AutomaticFoodItemUse(girl, "Incense of Serenity (L)", "Used a large incense of serenity to stay awake.");

    if (girl.tiredness() >= 50 && !is_constructed)
        AutomaticFoodItemUse(girl, "Incense of Serenity (M)", "Used a medium incense of serenity to stay awake.");

    if (girl.tiredness() >= 25 && !is_constructed)
        AutomaticFoodItemUse(girl, "Incense of Serenity (S)", "Used a small incense of serenity to stay awake.");

    // Mana

    // Set threshold at 20 as that is what is required to charm a customer to sleep with a girl
    if (girl.mana() < 20 && g_Dice.percent(5))
    {
        AutomaticFoodItemUse(girl, "Mana Crystal", "Used a mana crystal to restore 25 mana.");
    }
    if (girl.mana() < 20 && g_Dice.percent(5) )
    {
        AutomaticFoodItemUse(girl, "Eldritch Cookie", "Used an eldritch cookie to restore 30 mana.");
    }
    if (girl.mana() < 20 && g_Dice.percent(5))
    {
        AutomaticFoodItemUse(girl, "Mana Potion", "Used a mana potion to restore 100 mana.");
    }

    // Libido - ordered big to small

    // Succubus Milk [100 pts]
    if (girl.libido() < 5)  // Lower threshold
        AutomaticFoodItemUse(girl, "Succubus Milk", "Used succubus milk to restore 100 libido.");

    // Sinspice [75 pts]
    if (girl.libido() < 10)
        AutomaticFoodItemUse(girl, "Sinspice", "Used sinspice to restore 75 libido.");

    //Empress' New Clothes [50 pts] (Piece of equipment)  (This is a tossup between charisma & libido)
    if (girl.libido() <= 10)
        AutomaticItemUse(girl, "Empress' New Clothes", "You had her put on the empress' new clothes to get her libido up.");

    // Red Rose Extravaganza [50 pts?]
    if (girl.libido() < 10)
        AutomaticFoodItemUse(girl, "Red Rose Extravaganza", "Gave her a red rose extravaganza to get her libido going again.");

    // Ring of the Horndog [50 pts] (Piece of equipment)
    if (girl.libido() <= 10 && !girl.has_item("Minor Ring of the Horndog")
        && !girl.has_item("Ring of the Horndog") && !girl.has_item("Organic Lingerie"))
        AutomaticItemUse(girl, "Ring of the Horndog", "You had her equip a ring of the horndog to better serve her customers. (Libido up.)");

    // Gemstone Dress [42 pts] (Piece of equipment)
    if (girl.libido() <= 10)
        AutomaticItemUse(girl, "Gemstone Dress", "She put on a gemstone dress for that million-dollar feeling. (Libido up.)");

    // Silken Dress [34 pts] (Piece of equipment)
    if (girl.libido() <= 10)
        AutomaticItemUse(girl, "Silken Dress", "She put on a silken dress to better slide with her customers. (Libido up.)");

    // Minor Ring of the Horndog [30 pts] (Piece of equipment)
    if (girl.libido() <= 10 && !girl.has_item("Minor Ring of the Horndog")
        && !girl.has_item("Ring of the Horndog") && !girl.has_item("Organic Lingerie"))
        AutomaticItemUse(girl, "Minor Ring of the Horndog", "She was lookin a little listless so you had her equip a minor ring of the horndog. (Libido up.)");

    // Velvet Dress [34 pts] (Piece of equipment)
    if (girl.libido() <= 10)
        AutomaticItemUse(girl, "Velvet Dress", "She put on a velvet dress to feel even more sexy. (Libido up.)");

    // Designer Lingerie [20 pts] (Piece of equipment)
    if (girl.libido() <= 10)
        AutomaticItemUse(girl, "Designer Lingerie", "She put on designer lingerie to feel more at home. (Libido up.)");

    // Charisma

    //Ring of Charisma [50 pts] (Piece of equipment)
    if (girl.charisma() <= 50 && !girl.has_item("Ring of Charisma"))
        AutomaticItemUse(girl, "Ring of Charisma", "You had her put on a ring of charisma to overcome her speaking difficulties.");

    // Minor Ring of Charisma [30 pts] (Piece of equipment)
    if (girl.charisma() <= 70 && !girl.has_item("Minor Ring of Charisma"))
        AutomaticItemUse(girl, "Minor Ring of Charisma", "Put on a minor ring of charisma.");

    // Beauty

    // Ring of Beauty [50 pts] (Piece of equipment)
    if (girl.beauty() <= 50 && !girl.has_item("Ring of Beauty"))
        AutomaticItemUse(girl, "Ring of Beauty", "You had her put on a ring of beauty to overcome her ugly-stick disadvantage.");

    // Minor Ring of Beauty [30 pts] (Piece of equipment)
    if (girl.beauty() <= 70 && !girl.has_item("Minor Ring of Beauty"))
        AutomaticItemUse(girl, "Minor Ring of Beauty", "She put on a minor ring of beauty to cover some flaws.");

    // Rainbow Ring [15 pts] (Piece of equipment)
    if (girl.beauty() <= 85 && !girl.has_item("Rainbow Ring"))
        AutomaticItemUse(girl, "Rainbow Ring", "She put on a rainbow ring, to match her rainbow personality.");

    // Happiness - ordered from big values to small

    // Heaven-and-Earth Cake [100 pts]
    if (girl.happiness() <= 10)
        AutomaticFoodItemUse(girl, "Heaven-and-Earth Cake", "Had a heaven-and-earth cake to stave off serious depression.");

    // Eldritch cookie [70 pts]
    if (girl.happiness() <= 30)
        AutomaticFoodItemUse(girl, "Eldritch Cookie", "Had an eldritch cookie to improve her mood.");

    // Expensive Chocolates [50 pts]
    if (girl.happiness() <= 50)
        AutomaticFoodItemUse(girl, "Expensive Chocolates", "Had some expensive chocolates to improve her mood.");

    // Apple Tart [30 pts]
    if (girl.happiness() <= 70)
        AutomaticFoodItemUse(girl, "Apple Tart", "Had an apple tart to improve her mood.");

    // Honeypuff Scones [30 pts]
    if (girl.happiness() <= 70)
        AutomaticFoodItemUse(girl, "Honeypuff Scones", "Had a honeypuff scone for lunch.");

    // Fancy breath mints [10 pts]
    if (girl.happiness() <= 90)
        AutomaticFoodItemUse(girl, "Fancy Breath Mints", "Had fancy breath mints. (Why not? They were lying around.)");

    // Exotic Bouquet [10 pts]
    if (girl.happiness() <= 90)
        AutomaticFoodItemUse(girl, "Exotic Bouquet", "You gave her an exotic bouquet for work well done.");

    // Wild Flowers [5 pts]
    if (girl.happiness() <= 95)
        AutomaticFoodItemUse(girl, "Wild Flowers", "You gave her some wild flowers.");

    // Age

    //Do this before boobs b/c lolly wand makes them small
    // My arbitrary rule is, once they hit 30, make 'em young again.

    // To prevent using an elixir, then a wand, set an arbitrary upper age limit of 35 for elixirs
    if ((girl.age() >= 30) && (girl.age() <= 35))
        AutomaticFoodItemUse(girl, "Elixir of Youth", "Used a elixir of youth to remove ten years of age.");

    if (girl.age() >= 30 && girl.age() <= 80)
        AutomaticFoodItemUse(girl, "Lolita Wand", "Used a lolita wand to become seventeen again.");

    // XP: Nuts & tomes & mangos of knowledge, etc...

    // `J` xp can now be above 255 so removing restriction
    if (g_Dice.percent(5))    AutomaticFoodItemUse(girl, "Nut of Knowledge", "Used a small nut of knowledge.");
    if (g_Dice.percent(5))    AutomaticFoodItemUse(girl, "Mango of Knowledge", "She ate a mango of knowledge.");
    if (g_Dice.percent(5))    AutomaticFoodItemUse(girl, "Watermelon of Knowledge", "She had a watermelon of knowledge for lunch.");

    // Constitution (Items in reverse order. That is, the items offering the largest increases are first)

    // Ring of the Schwarzenegger [50 pts] (Piece of equipment)
    if (girl.constitution() <= 50)
        AutomaticItemUse(girl, "Ring of the Schwarzenegger", "She put on a Ring of the Schwarzenegger for the constitution boost.");

    // Bracer of Toughness [40 pts] (Piece of equipment)
    if (girl.constitution() <= 60)
        AutomaticItemUse(girl, "Bracer of Toughness", "She put on a bracer of toughness for the constitution boost.");

    // Minor Ring of the Schwarzenegger [30 pts] (Piece of equipment)
    if (girl.constitution() <= 70)
        AutomaticItemUse(girl, "Minor Ring of the Schwarzenegger", "She put on a Minor Ring of the Schwarzenegger for the constitution boost.");

    // Necklace of Pain Reversal [25 pts net: +40 for masochist -15 on necklace] (Piece of equipment)
    if (girl.constitution() <= 75 && !girl.has_active_trait("Masochist"))
        AutomaticItemUse(girl, "Necklace of Pain Reversal", "You had her put on a necklace of pain reversal for the constitution boost.");

    // Tiger Leotard [20 pts] (Piece of equipment)
    if (girl.constitution() <= 80)
        AutomaticItemUse(girl, "Tiger Leotard", "She put on a tiger leotard to feel it's strength and power.");

    // Manual of health [10 pts] (Piece of equipment, but slotless)
    // Lets be reasonable and only allow only one of each slotless item to be given to a girl.
    // (Having 8 stripper poles in a girl's inventory looks silly IMO.)
    if (girl.constitution() <= 90 && girl.strength() <= 90 && !girl.has_item("Manual of Health"))
        AutomaticSlotlessItemUse(girl, "Manual of Health", "You gave her a manual of health to read.");

    // Free Weights [10 pts] (Piece of equipment, but slotless)
    if (girl.constitution() <= 90 && girl.strength() <= 90 && !girl.has_item("Free Weights"))
        AutomaticSlotlessItemUse(girl, "Free Weights", "You gave her free weights to work with.");

    // Stripper Pole [5 pts] (Piece of equipment, but slotless)
    if (girl.constitution() <= 95 && girl.strength() <= 95 && !girl.has_item("Stripper Pole"))
        AutomaticSlotlessItemUse(girl, "Stripper Pole", "You gave her a stripper pole to practice with.");

    // Obedience

    // Necklace of Control (piece of equipment)
    if (girl.obedience() <= 10)
        AutomaticItemUse(girl, "Necklace of Control", "Her obedience is a problem so you had her put on a necklace of control.");

    if (girl.obedience() <= 50)
        AutomaticItemUse(girl, "Disguised Slave Band", "You had her put on a disguised slave band, claiming it was something else.");

    if (girl.obedience() <= 50)
        AutomaticItemUse(girl, "Slave Band", "You dealth with her obedience problems by forcing her to wear a slave band.");

    if (girl.obedience() <= 90)
        AutomaticFoodItemUse(girl, "Willbreaker Spice", "You slipped some willbreaker spice in to her food.");

#pragma endregion automation_stats

    // ---------- Part 2: Traits ----------------

# pragma region automation_traits

    // Perfection. This is an uber-valuable I put in. Ideally it should be Catacombs01, not Catacombs15.
    // It changes so many traits that it's hard to decide on a rule. In the end I kept it simple.
    // (Players will justifiably hate me if I made this decision for them.)
    // Do this first as it covers/replaces 90% of what follows
    /*if (girl.m_NumTraits <= 8)
        AutomaticFoodItemUse(girl, "Perfection", "Used perfection to become a near perfect being.");
    */
    // Tough

    // Aoshima beef
    if (!girl.has_active_trait("Tough"))
        AutomaticFoodItemUse(girl, "Aoshima BEEF!!", "Bulked up on Aoshima Beef to get the tough trait.");

    // Oiran Dress (Piece of equipment)
    if (!girl.has_active_trait("Tough"))
        AutomaticItemUse(girl, "Oiran Dress", "Put on an Oiran Dress.");

    // Nymphomaniac

    // Do this before quick learner b/c taking the shroud cola gives the girl the slow learner trait
    /*has = m_Inventory.has_item("Shroud Cola");
    has2 = m_Inventory.has_item("Cure for Shroud Addiction");
    if (!girl.has_trait("Nymphomaniac") && (has != -1 && has2 != -1))
    {
        // If one succeeds, the other should too
        // Note the order is important here: Shroud cola has to be first
        stringstream si; si << "You had her down a shround cola for the nymphomaniac side-effect.\n \n Unfortunately she also gains the slow-learner trait.";
        AutomaticFoodItemUse(girl, has, si.str());
        AutomaticFoodItemUse(girl, has2, "You had her take the shroud addiction cure.");
    }*/

    // Quick learner

    // Scroll of transcendance
    if (!girl.has_active_trait("Quick Learner") && !girl.has_active_trait("Optimist"))
        AutomaticFoodItemUse(girl, "Scrolls of Transcendance", "Read a Scroll of Transcendence to gain the quick learner and optimist traits.");

    // Book of enlightenment
    if (!girl.has_active_trait("Quick Learner"))
        AutomaticFoodItemUse(girl, "Book of Enlightenment", "Read a book of enlightenment for the quick learner trait.");

    // Ring of Enlightenment
    if (!girl.has_active_trait("Quick Learner"))
        AutomaticItemUse(girl, "Ring of Enlightenment", "Put on a ring of enlightenment for the quick learner trait.");

    // Amulet of the Cunning Linguist
    if (!girl.has_active_trait("Quick Learner"))
        AutomaticItemUse(girl, "Amulet of the Cunning Linguist", "Put on an amulet of the cunning linguist for the quick learner trait.");

    // Optimist: Good fortune, leprechaun biscuit, chatty flowers, etc...

    // Good Fortune
    if (!girl.has_active_trait("Optimist"))
        AutomaticFoodItemUse(girl, "Good Fortune", "Read a good fortune and feels more optimistic for it.");

    // Leprechaun Biscuit
    if (!girl.has_active_trait("Optimist"))
        AutomaticFoodItemUse(girl, "Leprechaun Biscuit", "Had a leprechaun biscuit and feels more optimistic for it.");

    // Chatty Flowers
    if (!girl.has_active_trait("Optimist"))
        AutomaticFoodItemUse(girl, "Chatty Flowers", "Talked with the chatty flowers and feels more optimistic for it.");

    // Glass shoes (piece of equipment)
    if (!girl.has_active_trait("Optimist") && !girl.has_item("Sandals of Mercury"))
        AutomaticItemUse(girl, "Glass Shoes", "Slipped on glass shoes for the optimist trait.");

    // Elegant (Obsidian Choker, piece of equipment)
    if (!girl.has_active_trait("Elegant"))
        AutomaticItemUse(girl, "Obsidian Choker", "Put on an obsidian choker for the elegant trait.");

    // Fleet of foot (Sandals of Mercury, piece of equipment)
    if (!girl.has_active_trait("Fleet of Foot") && !girl.has_item("Glass Shoes"))
        AutomaticItemUse(girl, "Sandals of Mercury", "Put on Sandals of Mercury for the fleet of foot trait.");

    // Fast Orgasms & Nymphomaniac (Organic Lingerie, piece of equipment)
    if (!girl.has_active_trait("Fast orgasms") && !girl.has_active_trait("Fast Orgasms") && !girl.has_active_trait("Nymphomaniac"))
        AutomaticItemUse(girl, "Organic Lingerie", "You had her wear organic lingerie.");

    // Fast Orgasms (Ring of Pleasure, piece of equipment)
    if (!girl.has_active_trait("Fast orgasms") && !girl.has_active_trait("Fast Orgasms"))
        AutomaticItemUse(girl, "Ring of Pleasure", "You had her put on a ring of pleasure for the fast orgasms trait.");

    // Lets try and cure mind fucked & retarted
    // The amulet of the sex elemental gives you the mind fucked trait. It can be "cured" until the amulet is taken off and put on again.
    // Regardless, we'll not try to cure the amulet case.
    if (((girl.has_active_trait("Mind Fucked") && !girl.has_item("Amulet of the Sex Elemental"))
         || girl.has_active_trait("Retarded")))
        AutomaticFoodItemUse(girl, "Refined Mandragora Extract", "You had her use refined mandragora extract to remove mental damage.");

    // Malformed

//    has = m_Inventory.has_item("Elixir of Ultimate Regeneration");
//    if (girl.has_trait("Malformed") && has != -1)
//        AutomaticFoodItemUse(girl, has, "Used an elixir of ultimate regeneration to cure her malformities.");

    // Tsundere & yandere
    if ((girl.has_active_trait("Yandere") || girl.has_active_trait("Tsundere")))
        AutomaticFoodItemUse(girl, "Attitude Reajustor", "You had her take an attitude reajustor pill.");

    // Eyes
    if ((girl.has_active_trait("One Eye") || girl.has_active_trait("Eye Patch")))
        AutomaticFoodItemUse(girl, "Eye Replacement Candy", "Used an eye replacement candy to restore her eye.");

    // Last ditch eye check.  Use the big guns if you don't have anything else.
//    has = m_Inventory.has_item("Elixir of Ultimate Regeneration");
//    if ((girl.has_trait("One Eye") || girl.has_trait("Eye Patch")) && has != -1)
//        AutomaticFoodItemUse(girl, has, "Used an elixir of ultimate regeneration to restore her eye.");

    // Scars - start with the least powerful cures and work up
    if ((girl.has_active_trait("Small Scars") || girl.has_active_trait("Cool Scars")))
        AutomaticFoodItemUse(girl, "Oil of Lesser Scar Removing", "Used an oil of lesser scar removal to remove work-related damage.");

    if ((girl.has_active_trait("Small Scars") || girl.has_active_trait("Cool Scars") || girl.has_active_trait("Horrific Scars")))
        AutomaticFoodItemUse(girl, "Oil of Greater Scar Removing", "Used an oil of greater scar removal to remove her scars.");

//    has = m_Inventory.has_item("Elixir of Ultimate Regeneration");
//    if ((girl.has_trait("Small Scars") || girl.has_trait("Cool Scars") || girl.has_trait("Horrific Scars")) && has != -1)
//        AutomaticFoodItemUse(girl, has, "Used an elixir of ultimate regeneration to remove her scars.");

    // Big boobs
    if (!girl.has_active_trait("Big Boobs") && !girl.has_active_trait("Abnormally Large Boobs"))
        AutomaticFoodItemUse(girl, "Oil of Extreme Breast Growth", "She uses an oil of extreme breast growth to gain the abnormally large boobs trait.");

    if (!girl.has_active_trait("Big Boobs") && !girl.has_active_trait("Abnormally Large Boobs"))
        AutomaticFoodItemUse(girl, "Oil of Greater Breast Growth", "She uses an oil of greater breast growth to gain the big boobs trait.");

    // Nipple Rings of Pillowy Softness (piece of [ring slot] equipment)
    if (!girl.has_active_trait("Big Boobs") && !girl.has_active_trait("Abnormally Large Boobs"))
        AutomaticSlotlessItemUse(girl, "Nipple Rings of Pillowy Softness", "You had her put on a nipple rings of pillowy softness.");

    // Nipple Rings of Breast Expansion, (piece of [ring slot] equipment)
    if (!girl.has_active_trait("Big Boobs") && !girl.has_active_trait("Abnormally Large Boobs"))
        AutomaticItemUse(girl, "Nipple Rings of Breast Expansion", "You had her put on nipple rings of breast expansion for the big boobs trait.");

    // Polish
// `J` zzzzzz - This should check stats not traits
    // If the girl doesn't have 4 of these 5 traits she will use polish
    if (!girl.has_active_trait("Good Kisser"))            PolishCount++;
    if (!girl.has_active_trait("Great Figure"))            PolishCount++;
    if (!girl.has_active_trait("Great Arse"))            PolishCount++;
    if (!girl.has_active_trait("Long Legs"))            PolishCount++;
    if (!girl.has_active_trait("Puffy Nipples"))        PolishCount++;

    if (PolishCount >= 4)
        AutomaticFoodItemUse(girl, "Polish", "Used polish to make herself more attractive to clients.");


    // Masochist

    // Put this at the bottom as there are better neck slot items that could be equipped above
    // Unlike the case of raising the constitution score in part one, we're only concerned with the trait here
    if (!girl.has_active_trait("Masochist"))
        AutomaticItemUse(girl, "Necklace of Pain Reversal", "You have this thing for masochism, so you had her put on a necklace of pain reversal.");

    // Iron Will

    // Disguised Slave band (piece of equipment)
    // (Statuses like 'controlled' on the Disguised Slave Band (amongst others) don't appear to do anything.)
    if (girl.has_active_trait("Iron Will"))
        AutomaticItemUse(girl, "Disguised Slave Band", "Her iron will is a problem so you had her put on a disguised slave band, claiming it was something else.");

    if (girl.has_active_trait("Iron Will"))
        AutomaticItemUse(girl, "Slave Band", "You dealt with her iron will by forcing her to wear a slave band.");

    // Necklace of Control (piece of equipment)
    if (girl.has_active_trait("Iron Will"))
        AutomaticItemUse(girl, "Necklace of Control", "Her iron will is a problem so you had her put on a necklace of control.");

# pragma endregion automation_traits
}
