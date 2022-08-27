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

#ifndef CRAZYS_WM_MOD_GAME_HPP
#define CRAZYS_WM_MOD_GAME_HPP

#include <memory>
#include <list>
#include <string>
#include <functional>
#include <unordered_set>
#include "Constants.h"
#include <vector>
#include "scripting/fwd.hpp"
#include "interface/constants.h"
#include "IKeyValueStore.h"

class sGirl;
class cGirls;
class cBuildingManager;
class cGangManager;
class DirPath;

class sStorage;
class cRival;
class cRivalManager;
class cObjectiveManager;
class sObjective;
class cPlayer;
class cDungeon;
class cGold;
class cJobManager;
class cCustomers;
class sCustomer;
class cInventory;
class IBuilding;
class ITraitsManager;
class ITraitsCollection;
class cTariff;
class cShop;
class cGirlPool;
class cMovieManager;
class ICharacter;
class cSkillCapManager;
class cImageLookup;

namespace tinyxml2
{
    class XMLElement;
}

class IGame;
class cErrorContext {
public:
    ~cErrorContext();
    cErrorContext(const cErrorContext&) = delete;
    cErrorContext(cErrorContext&& o)  noexcept : m_Game(o.m_Game), m_Unstack(std::move(o.m_Unstack)) {
        o.m_Unstack = {};
    };

    friend class cGame;
private:
    cErrorContext(IGame* g, std::function<void()> unstack) :
            m_Game(g), m_Unstack(std::move(unstack)) {
    }

    IGame* m_Game;
    std::function<void()> m_Unstack;
};

struct Date {
    /// An in-game date
    int year;
    int month;
    int day;
};

/*!
 * \brief Collects all information about a single game.
 * \details This class manages all global objects of a single game. This includes the girls,
 *          buildings, gangs, player, rivals etc.
 */
class IGame {
public:
    IGame();
    ~IGame();

    static std::unique_ptr<IGame> CreateGame();

    virtual void NewGame(const std::function<void(std::string)>& callback) = 0 ;
    virtual void LoadGame(const tinyxml2::XMLElement& source, const std::function<void(std::string)>& callback) = 0;
    virtual void SaveGame(tinyxml2::XMLElement& root) = 0;

    virtual void NextWeek() = 0;

    // rivals
    cRivalManager& rivals();
    cRival* random_rival();

    // objectives
    cObjectiveManager& objective_manager();
    sObjective* get_objective();

    // player
    cPlayer& player();

    // dungeon
    cDungeon& dungeon();

    // gangs
    cGangManager& gang_manager();

    // storage
    sStorage& storage();

    // movies
    cMovieManager& movie_manager();

    // gold
    cGold& gold();

    // image mgr
    cImageLookup& image_lookup() {
        return *m_ImageLookup;
    }

    // traits
    ITraitsManager& traits();
    std::unique_ptr<ITraitsCollection> create_traits_collection();

    // skills
    int get_skill_cap(SKILLS target, const ICharacter& character) const;

    // tariffs
    cTariff& tariff();

    // settings
    IKeyValueStore& settings();

    // messages
    virtual void push_message(std::string text, int color) = 0;
    virtual void error(std::string message) = 0;
    virtual cErrorContext push_error_context(std::string text) = 0;


    // customers
    cCustomers& customers();
    int GetNumCustomers() const;
    sCustomer GetCustomer(IBuilding& brothel); // Creates a new customer.

    // girls
    cGirls& girl_pool();
    std::shared_ptr<sGirl> GetRandomUniqueGirl(bool slave = false, bool catacomb = false, bool arena = false,
                                         bool daughter = false, bool isdaughter = false);
    std::shared_ptr<sGirl> CreateRandomGirl(SpawnReason reason, int age=-1);

    // inventory
    cInventory& inventory_manager();

    // shop
    cShop& shop();

    // building management and shortcuts
    cBuildingManager& buildings();
    /// returns true if the player owns a building of the given type.
    bool has_building(BuildingType type) const;

    cJobManager& job_manager();

    scripting::cScriptManager& script_manager();
    virtual void PushEvent(const scripting::sEventID& event) = 0;

    // time keeping
    const Date& date() const;
    unsigned int get_weeks_played() const;

    // prison
    cGirlPool& GetPrison()                { return *m_Prison; }

    // runaways
    const std::list<std::shared_ptr<sGirl>>& GetRunaways() const { return m_Runaways; }
    void AddGirlToRunaways(std::shared_ptr<sGirl> girl);
    void RemoveGirlFromRunaways(sGirl* girl);
    int  GetNumRunaways()                        { return m_Runaways.size(); }

    void check_druggy_girl(std::stringstream& ss);
    sGirl* GetDrugPossessor();

    // global stuff
    int GetBribeRate() const           { return m_BribeRate; }
    void SetBribeRate(int rate)        { m_BribeRate = rate; }
    int  GetInfluence()    const           { return m_Influence; }

    void WithdrawFromBank(long amount);
    void DepositInBank(long amount);
    long GetBankMoney() const           { return m_Bank; }

    void UpgradeSupplySheds()            { m_SupplyShedLevel++; }
    int  GetSupplyShedLevel()            { return m_SupplyShedLevel; }
    int MaxSupplies()                   { return m_SupplyShedLevel * 700; }

    cGirlPool& GetSlaveMarket() { return *m_MarketGirls; }

    // other data
    int GetTalkCount() const { return m_TalkCount; }
    void AddTalkCount(int value) { m_TalkCount += value; }

    bool CanWalkAround() const;
    void DoWalkAround();

    bool allow_cheats() const;
    void enable_cheating();

    // Global actions
    void TalkToGirl(sGirl& target);
private:
    // managers
    std::unique_ptr<cRivalManager> m_Rivals;
    std::unique_ptr<cObjectiveManager> m_ObjectiveManager;
    std::unique_ptr<cPlayer> m_Player;
    // TODO can we treat the dungeon as a normal building?
    std::unique_ptr<cDungeon> m_Dungeon;
    std::unique_ptr<cGold> m_Gold;
    std::unique_ptr<cGangManager> m_Gangs;
    std::unique_ptr<cJobManager> m_JobManager;
    std::unique_ptr<sStorage> m_Storage;
    std::unique_ptr<cBuildingManager> m_Buildings;
    std::unique_ptr<cCustomers> m_Customers;
    std::unique_ptr<cInventory> m_InvManager;
    std::unique_ptr<cGirls> m_Girls;
    std::unique_ptr<ITraitsManager> m_Traits;
    std::unique_ptr<cTariff> m_Tariff;
    std::unique_ptr<cShop> m_Shop;
    std::unique_ptr<IKeyValueStore> m_GameSettings;
    std::unique_ptr<scripting::cScriptManager> m_ScriptManager;
    std::unique_ptr<cMovieManager> m_MovieManager;
    // TODO should this be here or in the interface code?
    std::unique_ptr<cImageLookup> m_ImageLookup;

    // slave market stuff
    std::unique_ptr<cGirlPool> m_MarketGirls;

    std::unique_ptr<cGirlPool> m_Prison;

protected:
    std::unique_ptr<cSkillCapManager> m_SkillCaps;

    Date m_Date = {1209, 1, 1};

    bool m_IsCheating = false;

    int m_BribeRate       = 0;            // the amount of money spent bribing officials per week
    int m_Influence       = 0;            // based on the bribe rate this is the percentage of influence you have
    int m_Bank            = 0;            // how much is stored in the bank

    int m_SupplyShedLevel = 1;            // the level of the supply sheds. the higher the level, the more alcohol and antipreg potions can hold

    std::list<std::shared_ptr<sGirl>> m_Runaways;

    int m_TalkCount = 0;
    bool m_WalkAround = false;
};

// the global game instance.
extern std::unique_ptr<IGame> g_Game;

#endif //CRAZYS_WM_MOD_GAME_HPP
