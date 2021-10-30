#include "IGame.h"
#include "cRival.h"
#include "character/cPlayer.h"
#include "buildings/cDungeon.h"
#include "cGold.h"
#include "cGangManager.hpp"
#include "cJobManager.h"
#include "cObjectiveManager.hpp"
#include "buildings/IBuilding.h"
#include "sStorage.h"
#include "character/cCustomers.h"
#include "buildings/cBuildingManager.h"
#include "cInventory.h"
#include "character/traits/cTraitsManager.h"
#include "utils/FileList.h"
#include "cTariff.h"
#include "cShop.h"
#include "cGameSettings.h"
#include "scripting/cScriptManager.h"
#include "scripting/GameEvents.h"
#include "xml/util.h"
#include "character/traits/ITraitsCollection.h"
#include "character/cGirlPool.h"
#include "buildings/studio/manager.h"
#include <sstream>
#include "character/cSkillCap.h"

cRivalManager& IGame::rivals()
{
    return *m_Rivals;
}

cRival* IGame::random_rival()
{
    return m_Rivals->GetRandomRival();
}

IGame::IGame() :
    m_Rivals(new cRivalManager()),
    m_ObjectiveManager(new cObjectiveManager()),
    m_Player(nullptr ),
    m_Dungeon(new cDungeon()),
    m_Gold(new cGold()),
    m_Gangs(new cGangManager()),
    m_JobManager(new cJobManager()),
    m_Storage(new sStorage),
    m_Buildings(new cBuildingManager()),
    m_Customers(new cCustomers()),
    m_InvManager(new cInventory()),
    m_Girls(new cGirls()),
    m_Traits( std::make_unique<cTraitsManager>() ),
    m_Shop( new cShop(NUM_SHOPITEMS) ),
    m_ScriptManager( new scripting::cScriptManager() ),
    m_GameSettings(new cGameSettings()),
    m_MarketGirls( new cGirlPool() ),
    m_Prison( new cGirlPool() ),
    m_MovieManager(new cMovieManager),
    m_SkillCaps( std::make_unique<cSkillCapManager>() )
{
    m_Player = std::make_unique<cPlayer>( create_traits_collection() );
}

ITraitsManager& IGame::traits()
{
    return *m_Traits;
}

cObjectiveManager& IGame::objective_manager()
{
    return *m_ObjectiveManager;
}

sObjective * IGame::get_objective()
{
    return objective_manager().GetObjective();
}

cPlayer& IGame::player()
{
    return *m_Player;
}

cDungeon& IGame::dungeon()
{
    return *m_Dungeon;
}

// ----- Bank & money
void IGame::WithdrawFromBank(long amount)
{
    if (m_Bank - amount >= 0)
        m_Bank -= amount;
}

void IGame::DepositInBank(long amount)
{
    if (amount > 0)
        m_Bank += amount;
}

// ----- Runaways
void IGame::RemoveGirlFromRunaways(sGirl* girl)
{
    m_Runaways.remove_if([&](auto& ptr){ return ptr.get() == girl; });
}

void IGame::AddGirlToRunaways(std::shared_ptr<sGirl> girl)
{
    girl->m_DayJob = girl->m_NightJob = JOB_RUNAWAY;
    m_Runaways.push_back(std::move(girl));
}

cGold& IGame::gold()
{
    return *m_Gold;
}

const Date& IGame::date() const
{
    return m_Date;
}

unsigned IGame::get_weeks_played() const {
    const auto date = this->date();
    const unsigned months = (date.year - 1209) * 12 + date.month;
    const unsigned week = (months * 30 + date.day) / 7;
    return week;
}

cGangManager& IGame::gang_manager()
{
    return *m_Gangs;
}

cJobManager& IGame::job_manager()
{
    return *m_JobManager;
}

sStorage& IGame::storage()
{
    return *m_Storage;
}

cMovieManager& IGame::movie_manager()
{
    return *m_MovieManager;
}

IGame::~IGame() = default;


// ----- Drugs & addiction
void IGame::check_druggy_girl(std::stringstream& ss)
{
    if (g_Dice.percent(90)) return;
    sGirl* girl = GetDrugPossessor();
    if (girl == nullptr) return;
    ss << " They also bust a girl named " << girl->FullName() << " for possession of drugs and send her to prison.";
    // TODO clear inventory? The old code is not correct, items are removed but not unequipped!
    /*for (int i = 0; i<girl->m_NumInventory; i++) { girl->m_Inventory[i] = nullptr; }
    girl->m_NumInventory = 0;
    */
    if(girl->m_Building)
        m_Prison->AddGirl(girl->m_Building->remove_girl(girl));
}

sGirl* IGame::GetDrugPossessor()
{
    /*for(auto& current : buildings())
    {
        for(sGirl* girl : current->girls())
        {
            if (!g_Dice.percent(girl->intelligence()))    // girls will only be found out if low intelligence
            {
                if (girl->has_item("Shroud Mushroom") || girl->has_item("Fairy Dust") || girl->has_item("Vira Blood"))
                    return girl;
            }
        }
    }*/

    return nullptr;
}

cBuildingManager& IGame::buildings()
{
    return *m_Buildings;
}

bool IGame::has_building(BuildingType type) const
{
    return m_Buildings->has_building(type);
}

bool IGame::NameExists(const std::string& name) const
{
    auto girl = m_Girls->GetGirl(0);
    for(int i = 0; girl;)
    {
        if (girl->FullName() == name)    return true;
        girl = m_Girls->GetGirl(++i);
    }

    if (m_Buildings->NameExists(name))        return true;
    for(int i = 0; i < m_MarketGirls->num(); ++i)
        if (m_MarketGirls->get_girl(i)->FullName() == name)    return true;

    return false;
}

bool IGame::SurnameExists(const std::string& name) const
{
    auto girl = m_Girls->GetGirl(0);
    for(int i = 0; girl;)
    {
        if (girl->Surname() == name)    return true;
        girl = m_Girls->GetGirl(++i);
    }

    if (m_Buildings->SurnameExists(name))    return true;
    for(int i = 0; i < m_MarketGirls->num(); ++i)
        if (m_MarketGirls->get_girl(i)->Surname() == name)    return true;

    return false;
}

cCustomers& IGame::customers()
{
    return *m_Customers;
}

int IGame::GetNumCustomers() const
{
    return m_Customers->GetNumCustomers();
}

sCustomer IGame::GetCustomer(IBuilding& brothel)
{
    return m_Customers->GetCustomer(brothel);
}

cInventory& IGame::inventory_manager()
{
    return *m_InvManager;
}

std::shared_ptr<sGirl> IGame::GetRandomGirl(bool slave, bool catacomb, bool arena, bool daughter, bool isdaughter, bool require_unique)
{
    return m_Girls->GetRandomGirl(slave, catacomb, arena, daughter, isdaughter, require_unique);
}

cGirls& IGame::girl_pool()
{
    return *m_Girls;
}

std::shared_ptr<sGirl> IGame::CreateRandomGirl(int age, bool slave, bool undead, bool Human0Monster1, bool childnaped,
                                               bool arena, bool daughter, bool isdaughter, std::string findbyname)
{
    return m_Girls->CreateRandomGirl(age, slave, undead, Human0Monster1, childnaped, arena, daughter, isdaughter, findbyname);
}

cTariff& IGame::tariff()
{
    return *m_Tariff;
}

int IGame::get_skill_cap(SKILLS target, const ICharacter& character) const {
    return m_SkillCaps->get_cap(target, character);
}

bool IGame::allow_cheats() const
{
    return m_IsCheating;
}

void IGame::enable_cheating()
{
    gold().cheat();
    inventory_manager().GivePlayerAllItems();
    gang_manager().NumBusinessExtorted(500);
    m_IsCheating = true;
}

cShop &IGame::shop() {
    return *m_Shop;
}

scripting::cScriptManager &IGame::script_manager() {
    return *m_ScriptManager;
}

void IGame::TalkToGirl(sGirl &target) {
    if (m_TalkCount <= 0) return;    // if we have no talks left, we can go home

    /*
    *    is she dead? that would make life simpler.
    *
    *    (actually, I'd like to be able to view her stats in read-only mode
    *    after she dies, just so I can do a post-mortem. But for now...)
    */
    if (target.is_dead())
    {
        push_message("Though you have many skills, speaking to the dead is not one of them.", 1);
        return;
    }

    if(target.m_Building != nullptr) {
        target.TriggerEvent(EDefaultEvent::GIRL_INTERACT_BROTHEL);
    } else {
        target.TriggerEvent(EDefaultEvent::GIRL_INTERACT_DUNGEON);
    }
    if (!allow_cheats()) m_TalkCount--;

}

IKeyValueStore& IGame::settings()
{
    return *m_GameSettings;
}

std::unique_ptr<ITraitsCollection> IGame::create_traits_collection() {
    return m_Traits->create_collection();
}

bool IGame::CanWalkAround() const {
    return !m_WalkAround || allow_cheats();
}

void IGame::DoWalkAround() {
    m_WalkAround = true;
}


cErrorContext::~cErrorContext() {
    if(m_Unstack) {
        if(std::uncaught_exceptions() > 0) {
            m_Game->error("");
        }
        m_Unstack();
    }
}
