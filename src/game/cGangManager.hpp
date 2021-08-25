#pragma once

#include "cGangs.h"

/*
 * manages all the player gangs
 */
class cGangManager {

    public:
        cGangManager();
        ~cGangManager();

        void AddNewGang(bool boosted = false);    // Adds a new randomly generated gang to the recruitable list
        void HireGang(int gangID);    // hired a recruitable gang, so add it to your gangs
        void FireGang(int gangID);    // fired a gang, so send it back to recruitables (or just delete if full up)
        void AddGang(std::unique_ptr<sGang> newGang);

        // removed a controlled gang completely from service
        void RemoveGang(sGang* gang);
        // sends a gang on a mission
        sGang* GetGang(int gangID);                                            // gets a gang
        // gets a recruitable gang
        sGang* GetGangOnMission(int missID);                                // gets a gang on the current mission
        sGang* GetRandomGangOnMission(int missID);                        // gets a random gang on the current mission
        sGang* GetGangNotFull(int roomfor = 0, bool recruiting = true);        // gets a gang with room to spare
        sGang* GetGangRecruitingNotFull(int roomfor = 0);                    // gets a gang recruiting with room to spare
        void UpdateGangs();

        int GetMaxNumGangs();
        int GetNumGangs();
        int GetNumHireableGangs();

        sGang GetTempGang(int mod);                                            // creates a new gang with stat/skill mod

        tinyxml2::XMLElement& SaveGangsXML(tinyxml2::XMLElement& pRoot);
        bool LoadGangsXML(const tinyxml2::XMLElement* pGangManager);

        int GetNumBusinessExtorted() const    { return m_BusinessesExtort; }
        int NumBusinessExtorted(int n)        { m_BusinessesExtort += n; return m_BusinessesExtort; }

        int GetNets() const {return m_NumNets;}
        int GetNetRestock() const {return m_KeepNetsStocked;}
        void KeepNetStocked(int stocked){m_KeepNetsStocked = stocked;}
        int  BuyNets(int amount, bool autobuy);

        int GetHealingPotions() const {return m_NumHealingPotions;}
        void KeepHealStocked(int stocked){m_KeepHealStocked = stocked;}
        int  GetHealingRestock() const {return m_KeepHealStocked;}
        int  BuyHealingPotions(int amount, bool autobuy);

        bool losegang(const sGang& gang);
        void check_gang_recruit(sGang& gang);
        void GangStartOfShift();
        void RestockNetsAndPots();

        int chance_to_catch(const sGirl& girl);

        std::vector<sGang*> gangs_on_mission(int mission_id);
        std::vector<sGang*> gangs_watching_girls();

        int Gang_Gets_Girls() const    { return m_Gang_Gets_Girls; }
        int Gang_Gets_Items() const    { return m_Gang_Gets_Items; }
        int Gang_Gets_Beast() const    { return m_Gang_Gets_Beast; }
        int Gang_Gets_Girls(int g)    { return m_Gang_Gets_Girls = g; }
        int Gang_Gets_Items(int g)    { return m_Gang_Gets_Items = g; }
        int Gang_Gets_Beast(int g)    { return m_Gang_Gets_Beast = g; }


        std::vector<std::unique_ptr<sGang>>& GetPlayerGangs() { return m_PlayersGangs; }
        std::vector<std::unique_ptr<sGang>>& GetHireableGangs() { return m_HireableGangs; }

    private:
        int m_BusinessesExtort;    // number of businesses under your control

        int m_Gang_Gets_Girls;
        int m_Gang_Gets_Items;
        int m_Gang_Gets_Beast;

        int m_MaxNumGangs;
        std::vector<std::string> m_GangNames;
        std::vector<std::unique_ptr<sGang>> m_PlayersGangs;
        std::vector<std::unique_ptr<sGang>> m_HireableGangs;
        std::vector<std::unique_ptr<IGangMission>> m_Missions;

        // gang armory
        int m_KeepHealStocked = 0;
        int m_NumHealingPotions = 0;
        int m_KeepNetsStocked = 0;
        int m_NumNets = 0;

};
