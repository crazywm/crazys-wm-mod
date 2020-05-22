#ifndef CRAZYS_WM_MOD_GAME_HPP
#define CRAZYS_WM_MOD_GAME_HPP

#include <string>
#include <memory>
#include <list>
#include <Constants.h>
#include <vector>

class sGirl;
class cGirls;
class cBuildingManager;
class cGangManager;

class sStorage;
class cRival;
class cRivalManager;
class cObjectiveManager;
class sObjective;
class cPlayer;
class cDungeon;
class cTriggerList;
class cGold;
class cJobManager;
class cCustomers;
class sCustomer;
class cInventory;
class IBuilding;

class TiXmlElement;

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

    void load(TiXmlElement& root);
    void save(TiXmlElement& root);

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

    // triggers
    cTriggerList& global_triggers();

    // gangs
    cGangManager& gang_manager();

    // storage
    sStorage& storage();

    // gold
    cGold& gold();

    // messages
    void push_message(std::string text, int color);

    // customers
    cCustomers& customers();
    int GetNumCustomers() const;
    sCustomer GetCustomer(IBuilding& brothel); // Creates a new customer.

    // girls
    cGirls& girl_pool();
    sGirl* GetRandomGirl(bool slave = false, bool catacomb = false, bool arena = false, bool daughter = false, bool isdaughter = false);
    sGirl* CreateRandomGirl(int age, bool addToGGirls, bool slave = false, bool undead = false,
            bool Human0Monster1 = false, bool childnaped = false, bool arena = false, bool daughter = false,
            bool isdaughter = false, std::string findbyname = "");

    // inventory
    cInventory& inventory_manager();

    // building management and shortcuts
    cBuildingManager& buildings();
    /// returns true if the player owns a building of the given type.
    bool has_building(BuildingType type) const;

    cJobManager& job_manager();

    // time keeping
    const Date& date() const;

    // prison
    std::list<sGirl*>& GetPrison()				{ return m_Prison; }
    void AddGirlToPrison(sGirl* girl);
    void RemoveGirlFromPrison(sGirl* girl);
    int  GetNumInPrison()			            { return m_Prison.size(); }

    // runaways
    const std::list<sGirl*>& GetRunaways() const { return m_Runaways; }
    void AddGirlToRunaways(sGirl* girl);
    void RemoveGirlFromRunaways(sGirl* girl);
    int  GetNumRunaways()			            { return m_Runaways.size(); }

    void check_druggy_girl(std::stringstream& ss);
    sGirl* GetDrugPossessor();
    void check_raid();

    // global stuff
    long GetBribeRate() const           { return m_BribeRate; }
    void SetBribeRate(long rate)        { m_BribeRate = rate; }
    void UpdateBribeInfluence();
    int  GetInfluence()	const           { return m_Influence; }

    void WithdrawFromBank(long amount);
    void DepositInBank(long amount);
    long GetBankMoney() const           { return m_Bank; }

    void UpgradeSupplySheds()			{ m_SupplyShedLevel++; }
    int  GetSupplyShedLevel()			{ return m_SupplyShedLevel; }
    int MaxSupplies()                   { return m_SupplyShedLevel * 700; }

    std::size_t GetNumMarketSlaves() const;
    sGirl * GetMarketSlave(std::size_t index);
    bool IsMarketUniqueGirl(int index) const;
    void RemoveMarketSlave(const sGirl& target);
    void UpdateMarketSlaves();

    // utilities
    bool NameExists(std::string name) const;
    bool SurnameExists(std::string surname) const;
private:
    // managers
    std::unique_ptr<cRivalManager> m_Rivals;
    std::unique_ptr<cObjectiveManager> m_ObjectiveManager;
    std::unique_ptr<cPlayer> m_Player;
    // TODO can we treat the dungeon as a normal building?
    std::unique_ptr<cDungeon> m_Dungeon;
    std::unique_ptr<cTriggerList> m_GlobalTriggers;
    std::unique_ptr<cGold> m_Gold;
    std::unique_ptr<cGangManager> m_Gangs;
    std::unique_ptr<cJobManager> m_JobManager;
    std::unique_ptr<sStorage> m_Storage;
    std::unique_ptr<cBuildingManager> m_Buildings;
    std::unique_ptr<cCustomers> m_Customers;
    std::unique_ptr<cInventory> m_InvManager;
    std::unique_ptr<cGirls> m_Girls;

    // slave market stuff
    std::vector<sGirl*> m_MarketGirls;
    std::vector<bool> m_MarketUniqueGirl;
    sGirl* generate_unique_girl();



    long m_BribeRate      = 0;			// the amount of money spent bribing officials per week
    int  m_Influence      = 0;			// based on the bribe rate this is the percentage of influence you have
    long m_Bank           = 0;			// how much is stored in the bank

    int m_SupplyShedLevel = 1;		    // the level of the supply sheds. the higher the level, the more alcohol and antipreg potions can hold

    std::list<sGirl*> m_Prison;
    std::list<sGirl*> m_Runaways;

    Date m_Date = {1209, 1, 1};

    // helper functions
    void do_tax();
    void read_attributes_xml(TiXmlElement& el);
};

// the global game instance.
extern Game g_Game;

#endif //CRAZYS_WM_MOD_GAME_HPP
