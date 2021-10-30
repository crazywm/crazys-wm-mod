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

#pragma once

#ifndef __CPLAYER_H
#define __CPLAYER_H

#include <fstream>
#include "Constants.h"
#include <tinyxml2.h>
#include "ICharacter.h"

class sGirl;


class cPlayer : public ICharacter
{
private:
    int Limit100(int nStat);                                // Limit stats to -100 to 100
    int    Scale200(int nStatMod, int nCurrentStatValue);   // Scale stat from 1 to nStatMod
/*
 *    the suspicion level of the authorities.
 *    -100 means they are on players side
 *    +100 means they will probably raid his brothels
 */
    int m_Suspicion;    
/*
 *    How good or evil the player is considered to be:
 *    -100 is evil while +100 is a saint
 */
    int m_Disposition;
/*
 *    how much the customers fear you:
 *    -100 is not at all while 100 means a lot
 */
    int m_CustomerFear;

    std::string m_Title;        // basic title - need to add more titles with more power gained

    int m_PlayerGender;    // `J` added - not going to be changeable yet but adding it in for later
public:
    std::string Title()        { return m_Title; }

    std::string SetTitle(std::string title);

    int Gender()        { return m_PlayerGender; }
    void SetGender(int x);
    void AdjustGender(int male, int female);
    bool CanImpregnateFemale();
    bool CanCarryOwnBaby();
    bool CanCarryNormalBaby();
    bool HasPenis();
    bool HasVagina();
    bool HasTestes();
    bool HasOvaries();


    bool m_WinGame;

    cPlayer(std::unique_ptr<ITraitsCollection> tc);
    void SetToZero();

    tinyxml2::XMLElement& SavePlayerXML(tinyxml2::XMLElement& elRoot);
    bool LoadPlayerXML(const tinyxml2::XMLElement* Player);

    int disposition()        { return m_Disposition; }
    int disposition(int n);
    int evil(int n);
    int suspicion()            { return m_Suspicion; }
    int suspicion(int n);
    int customerfear()        { return m_CustomerFear; }
    int customerfear(int n);

    // text helpers
    std::string disposition_text() const;
    std::string suss_text() const;

    /// automatically use items on the given girl
    void apply_items(sGirl& girl);
    void AutomaticFoodItemUse(sGirl& girl, const char* item_name, std::string message);
    void AutomaticItemUse(sGirl& girl, const char* item_name, std::string message);
    bool AutomaticSlotlessItemUse(sGirl& girl, const char* item_name, std::string message);
};


#endif    /* __CPLAYER_H */