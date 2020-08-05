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

namespace tinyxml2
{
    class XMLElement;
}

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
class Game {
public:
    Game();
    ~Game();

    void next_week();

    void load(tinyxml2::XMLElement& root);
    void save(tinyxml2::XMLElement& root);

    void NewGame(const std::function<void(std::string)>& callback);
    void LoadGame(const tinyxml2::XMLElement& source, const std::function<void(std::string)>& callback);

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

    // gold
    cGold& gold();

    // traits
    ITraitsManager& traits();
    std::unique_ptr<ITraitsCollection> create_traits_collection();

    // tariffs
    cTariff& tariff();

    // settings
    IKeyValueStore& settings();

    // messages
    void push_message(std::string text, int color);
    void error(std::string message);

    // customers
    cCustomers& customers();
    int GetNumCustomers() const;
    sCustomer GetCustomer(IBuilding& brothel); // Creates a new customer.

    // girls
    cGirls& girl_pool();
    std::shared_ptr<sGirl> GetRandomGirl(bool slave = false, bool catacomb = false, bool arena = false, bool daughter = false, bool isdaughter = false);
    std::shared_ptr<sGirl>
    CreateRandomGirl(int age, bool slave = false, bool undead = false, bool Human0Monster1 = false,
                     bool childnaped = false, bool arena = false, bool daughter = false, bool isdaughter = false,
                     std::string findbyname = "");

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
    void PushEvent(const scripting::sEventID& event);

    // time keeping
    const Date& date() const;

    // prison
    cGirlPool& GetPrison()                { return *m_Prison; }

    // runaways
    const std::list<std::shared_ptr<sGirl>>& GetRunaways() const { return m_Runaways; }
    void AddGirlToRunaways(std::shared_ptr<sGirl> girl);
    void RemoveGirlFromRunaways(sGirl* girl);
    int  GetNumRunaways()                        { return m_Runaways.size(); }

    void check_druggy_girl(std::stringstream& ss);
    sGirl* GetDrugPossessor();
    void check_raid();

    // global stuff
    long GetBribeRate() const           { return m_BribeRate; }
    void SetBribeRate(long rate)        { m_BribeRate = rate; }
    void UpdateBribeInfluence();
    int  GetInfluence()    const           { return m_Influence; }

    void WithdrawFromBank(long amount);
    void DepositInBank(long amount);
    long GetBankMoney() const           { return m_Bank; }

    void UpgradeSupplySheds()            { m_SupplyShedLevel++; }
    int  GetSupplyShedLevel()            { return m_SupplyShedLevel; }
    int MaxSupplies()                   { return m_SupplyShedLevel * 700; }

    cGirlPool& GetSlaveMarket() { return *m_MarketGirls; }
    void UpdateMarketSlaves();

    bool allow_cheats() const;
    void enable_cheating();

    // utilities
    bool NameExists(const std::string& name) const;
    bool SurnameExists(const std::string& surname) const;

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

    // slave market stuff
    std::unique_ptr<cGirlPool> m_MarketGirls;

    bool m_IsCheating = false;



    int m_BribeRate       = 0;            // the amount of money spent bribing officials per week
    int m_Influence       = 0;            // based on the bribe rate this is the percentage of influence you have
    int m_Bank            = 0;            // how much is stored in the bank

    int m_SupplyShedLevel = 1;            // the level of the supply sheds. the higher the level, the more alcohol and antipreg potions can hold

    std::unique_ptr<cGirlPool> m_Prison;
    std::list<std::shared_ptr<sGirl>> m_Runaways;

    Date m_Date = {1209, 1, 1};

    // helper functions
    void do_tax();
    void read_attributes_xml(const tinyxml2::XMLElement& el);

    void UpdateRunaways();

    // This keeps track of all unqiue girl files that are used for this game.
    std::unordered_set<std::string> m_GirlFiles;

    // event stack
    struct sScriptEventStack;
    std::unique_ptr<sScriptEventStack> m_EventStack;

    void RunNextEvent(const scripting::sEventID& event);

private:
    void LoadGirlFiles(const DirPath& location, const std::function<void(const std::string&)>& error_handler);
    void LoadItemFiles(DirPath location);
    void LoadTraitFiles(DirPath location);
};

// the global game instance.
extern std::unique_ptr<Game> g_Game;

#endif //CRAZYS_WM_MOD_GAME_HPP
