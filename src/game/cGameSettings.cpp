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

#include "cGameSettings.h"
#include "tinyxml2.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include <utility>
#include "CLog.h"

// pre-defined setting constants
/*!
 * These are defined here, as constants in the settings namespace.
 * This allows them to be forward-declared in the files in which they
 * are needed, and the linker makes sure there are not typos. Compared
 * to defining all these constants in a header, this has the advantage
 * that adding or changing a constant does not trigger a recompilation
 * of the entire project.
 */
namespace settings {
    const char* INITIAL_GOLD          = "initial.gold";
    const char* INITIAL_BOOSTED_GANGS = "initial.gang_boosted";
    const char* INITIAL_RANDOM_GANGS  = "initial.gang_random";

    const char* WORLD_ENCOUNTER_CHANCE   = "world.encounter_chance";
    const char* WORLD_CATACOMB_UNIQUE    = "world.catacombs.unique_chance";
    const char* WORLD_ENCOUNTER_UNIQUE   = "world.encounter.unique_chance";
    const char* WORLD_RAPE_STREETS       = "world.rape-chance.streets";
    const char* WORLD_RAPE_BROTHEL       = "world.rape-chance.brothel";

    const char* GANG_MAX_RECRUIT_LIST    = "gang.max_recruit_list";
    const char* GANG_MIN_START_MEMBERS   = "gang.min_start_members";
    const char* GANG_MAX_START_MEMBERS   = "gang.max_start_members";
    const char* GANG_MAX_MEMBERS         = "gang.max_members";
    const char* GANG_REMOVE_CHANCE       = "gang.chance_remove";
    const char* GANG_MIN_WEEKLY_NEW      = "gang.min_weekly_new";
    const char* GANG_MAX_WEEKLY_NEW      = "gang.max_weekly_new";
    const char* GANG_WEAPON_UPGRADE_COST = "gang.weapon_upgrade_cost";
    const char* GANG_WAGES_FACTOR        = "gang.wages_factor";
    const char* GANG_MIN_RIVAL_MEMBERS   = "gang.min_rival_members";
    const char* GANG_MAX_RIVAL_MEMBERS   = "gang.max_rival_members";
    const char* GANG_MIN_RIVAL_SKILL     = "gang.min_rival_skill";
    const char* GANG_MAX_RIVAL_SKILL     = "gang.max_rival_skill";

    const char* USER_HOUSE_PERCENT_FREE  = "user.house_percent.free";
    const char* USER_HOUSE_PERCENT_SLAVE = "user.house_percent.slave";
    const char* USER_ACCOMODATION_FREE   = "user.accommodation.free";
    const char* USER_ACCOMODATION_SLAVE  = "user.accommodation.slave";
    const char* USER_KEEP_TIPS_FREE      = "user.keep_tips.free";
    const char* USER_KEEP_TIPS_SLAVE     = "user.keep_tips.slave";
    const char* USER_PAY_SLAVE           = "user.pay.slave";
    const char* USER_ITEMS_AUTO_USE      = "user.items.auto_use";
    const char* USER_ITEMS_AUTO_EQUIP_COMBAT = "user.items.auto_equip_combat";
    const char* USER_HOROSCOPE           = "user.horoscopetype";
    const char* USER_MOVIES_AUTO         = "user.movies.auto";
    const char* USER_COMBAT_POPUP        = "user.combat-popup";
    const char* USER_SHOW_NUMBERS        = "user.show-numbers";

    const char* SLAVE_MARKET_MIN = "slave_market.min_weekly_new";
    const char* SLAVE_MARKET_MAX = "slave_market.max_weekly_new";
    const char* SLAVE_MARKET_UNIQUE_CHANCE  = "slave_market.unique_chance";
    const char* SLAVE_MARKET_TURNOVER_RATE  = "slave_market.turnover";

    const char* TAXES_RATE    = "tax.rate";
    const char* TAXES_MINIMUM = "tax.minimum";
    const char* TAXES_LAUNDRY = "tax.laundry";

    const char* PREG_CONTRA_FAIL   = "pregnancy.chance.contraception_fail";
    const char* PREG_MULTI_CHANCE  = "pregnancy.chance.multiplets";
    const char* PREG_MISS_MONSTER  = "pregnancy.chance.miscarriage.beast";
    const char* PREG_MISS_HUMAN    = "pregnancy.chance.miscarriage.human";
    const char* PREG_CHANCE_PLAYER = "pregnancy.chance.player";
    const char* PREG_CHANCE_CUST   = "pregnancy.chance.customer";
    const char* PREG_CHANCE_BEAST  = "pregnancy.chance.beast";
    const char* PREG_CHANCE_GIRL  = "pregnancy.chance.girl";
    const char* PREG_GIRL_CHILD   = "pregnancy.child.girl-chance";
    const char* PREG_DURATION_HUMAN    = "pregnancy.duration.human";
    const char* PREG_DURATION_MONSTER  = "pregnancy.duration.monster";
    const char* PREG_WEEKS_GROW    = "pregnancy.duration.growth";
    const char* PREG_COOL_DOWN     = "pregnancy.duration.cooldown";
    const char* PREG_GOOD_FACTOR   = "pregnancy.good_sex_factor";

    const char* MONEY_SELL_ITEM    = "money.sell.item";
    const char* MONEY_SELL_SLAVE   = "money.sell.slave";
    const char* MONEY_BUY_SLAVE    = "money.buy.slave";
    const char* MONEY_COST_ROOM    = "money.cost.room";
    const char* MONEY_COST_CONTRA  = "money.cost.contraception";
    const char* MONEY_COST_HP      = "money.cost.healing_potion";
    const char* MONEY_COST_NET     = "money.cost.net";

    const char* TORTURE_INJURY_CHANCE   = "interact.torture_injury_chance";

    const char* BALANCING_HEALTH_REGAIN = "balancing.health.regain";
    const char* BALANCING_FATIGUE_REGAIN = "balancing.fatigue.regain";

    const char* MOVIES_SATURATION_DECAY = "movies.saturation-decay";
    const char* MOVIES_HYPE_DECAY = "movies.hype-decay";
    const char* MOVIES_AUDIENCE_UPDATE_CHANCE = "movies.audience.update-chance";
    const char* MOVIES_AUDIENCE_BASE_FRACTION = "movies.audience.base-fraction";
    const char* MOVIES_REPEATED_SCENE_FACTOR = "movies.movie.repeated-factor";
    const char* MOVIES_RUNNING_WEEKS = "movies.running-weeks";
    const char* MOVIES_AUDIENCE_SATED_CHANCE = "movies.audience.sated-chance";
}

cGameSettings::cGameSettings() : cKeyValueBase("Setting", "Name", "Value")
{
    using namespace settings;
    // game specific settings
    add_setting(INITIAL_GOLD, "Initial Gold", "The amount of gold available to the player at the beginning of the game", 4000);
    add_setting(INITIAL_BOOSTED_GANGS, "Start Gangs Boosted", "The number of boosted gangs available at game start", 2);
    add_setting(INITIAL_RANDOM_GANGS, "Start Gangs Random", "The number of random gangs available at game start", 2);

    add_setting(WORLD_ENCOUNTER_CHANCE, "Encounter Chance", "Chance to meet a girl when walking in town/arena tryouts/casting",
                sPercent(30));
    add_setting(WORLD_CATACOMB_UNIQUE, "Catacombs Unique Chance", "Percentage of unique girls in catacomb encounters",
                sPercent(50));
    add_setting(WORLD_ENCOUNTER_UNIQUE, "Town Unique Chance", "Percentage of unique girls in town encounters",
                sPercent(50));
    add_setting(WORLD_RAPE_STREETS, "Rape Streets", "Chance that a customer will try to rape a girl when working the streets",
                sPercent(5));
    add_setting(WORLD_RAPE_BROTHEL, "Rape Brothel", "Chance that a customer will try to rape a girl when whoring in the brothel",
                sPercent(1));

    add_setting(GANG_MAX_RECRUIT_LIST, "Max Recruit List", "The maximum amount of gangs available for recruitment.", 6);
    add_setting(GANG_MIN_START_MEMBERS, "Init Member Min", "The minimum number of members in a newly created gang.", 1);
    add_setting(GANG_MAX_START_MEMBERS, "Init Member Max", "The maximum number of members in a newly created gang.", 8);
    add_setting(GANG_MAX_MEMBERS, "Member Max", "The maximum number of members in a newly created gang.", 10);
    add_setting(GANG_REMOVE_CHANCE, "Remove Unwanted", "The chance that an unrecruited gang will be removed.", sPercent(0.25f));
    add_setting(GANG_MIN_WEEKLY_NEW, "Add New Weekly Min", "The minimum number of new gangs created each week.", 0);
    add_setting(GANG_MAX_WEEKLY_NEW, "Add New Weekly Max", "The maximum number of new gangs created each week.", 2);
    add_setting(GANG_WEAPON_UPGRADE_COST, "Weapon Upgrade Cost", "The base cost for a weapon upgrade.", 150);
    add_setting(GANG_WAGES_FACTOR, "Wages Factor", "Multiplier for all gang wages (mission costs).", 1.f);
    add_setting(GANG_MIN_RIVAL_MEMBERS, "Rival Member Min", "The minimum number of members in a rival gang.", 2);
    add_setting(GANG_MAX_RIVAL_MEMBERS, "Rival Member Max", "The maximum number of members in a rival gang.", 8);
    // This is the base skill. There might be modifiers that change the effective values of these.
    add_setting(GANG_MIN_RIVAL_SKILL, "Rival Skill Min", "The minimum skill for rival gangs.", 20);
    add_setting(GANG_MIN_RIVAL_SKILL, "Rival Skill Max", "The maximum skill for rival gangs.", 60);

    // user adjustable settings
    add_setting(USER_HOUSE_PERCENT_FREE, "Free House %", "The percentage of earnings that are taken from free girls by default.", 60);
    add_setting(USER_HOUSE_PERCENT_SLAVE, "Slave House %", "The percentage of earnings that are taken from slave girls by default.", 100);
    add_setting(USER_ACCOMODATION_FREE, "Base Free Accom", "The default accommodation level for free girls.", 5);
    add_setting(USER_ACCOMODATION_SLAVE, "Base Slave Accom", "The default accommodation level for slave girls.", 1);
    add_setting(USER_KEEP_TIPS_FREE, "Girls Keep Tips", "Whether free girls are allowed to keep their tips.", true);
    add_setting(USER_KEEP_TIPS_SLAVE, "Slave Keep Tips", "Whether slaves are allowed to keep their tips.", false);
    add_setting(USER_PAY_SLAVE, "Pay Slaves", "Whether you pay your slave girls.", false);

    add_setting(USER_ITEMS_AUTO_USE, "Auto Use Items", "Whether items from the player's inventory should be applied automatically to suitable girls.", true);
    add_setting(USER_ITEMS_AUTO_EQUIP_COMBAT, "Auto Equip Combat", "If this is enabled, girls will equip their combat gear before doing any combat-related jobs.", true);
    add_setting(USER_HOROSCOPE, "Horoscope", "Which horoscope to use", 1);
    add_setting(USER_MOVIES_AUTO, "Auto Make Movies", "Whether scenes should automatically be combined to movies", true);
    add_setting(USER_COMBAT_POPUP, "Combat Popup", "Whether combat details are shown in a popup", true);
    add_setting(USER_SHOW_NUMBERS, "Show Numbers", "Whether to show additional numbers in certain game texts.", false);

    add_setting(SLAVE_MARKET_MIN, "Minimum Slaves", "Minimum amount of girls at the slave market", 5);
    add_setting(SLAVE_MARKET_MAX, "Maximum Slaves", "Maximum amount of girls at the slave market", 12);
    add_setting(SLAVE_MARKET_UNIQUE_CHANCE, "Unique Market", "Chance that a slave market girl will be a unique girl", sPercent(0.35f));
    add_setting(SLAVE_MARKET_TURNOVER_RATE, "Turnover", "Rate with which the girls will be replaced each week", sPercent(0.5f));

    add_setting(TAXES_RATE, "Rate", "The default tax rate. The effective tax rate can be lower due to influence at city hall.", sPercent(0.06f));
    add_setting(TAXES_MINIMUM, "Minimum", "Minimum tax rate that can be achieved due to city hall influence.", sPercent(0.01f));
    add_setting(TAXES_LAUNDRY, "Laundry", "Maximum percentage of you income that you can protect from taxation by money laundering.", sPercent(0.25f));

    add_setting(PREG_CONTRA_FAIL, "Contraception Failure", "Chance for a contraceptive to fail", sPercent(0.00f));
    add_setting(PREG_MULTI_CHANCE, "Multi Birth Chance", "Chance for multiple babies in one pregnancy", sPercent(0.01f));
    add_setting(PREG_MISS_HUMAN, "Miscarriage Chance", "Chance for miscarriage in monster pregnancy", sPercent(0.01f));
    add_setting(PREG_MISS_MONSTER, "Miscarriage Monster", "Chance for miscarriage in normal pregnancy", sPercent(0.001f));

    add_setting(PREG_CHANCE_PLAYER, "Player Chance", "Base chance for getting impregnated by the player.", sPercent(0.25f));
    add_setting(PREG_CHANCE_CUST, "Customer Chance", "Base chance for getting impregnated by a customer.", sPercent(0.25f));
    add_setting(PREG_CHANCE_BEAST, "Monster Chance", "Base chance for getting impregnated by a monster.", sPercent(0.25f));
    add_setting(PREG_CHANCE_GIRL, "Girl Chance", "Base chance that the girl is fertile.", sPercent(0.25f));

    add_setting(PREG_GIRL_CHILD, "Chance Of Girl", "Percentage of children that will be girls.", sPercent(0.5f));
    add_setting(PREG_DURATION_HUMAN, "Weeks Pregnant", "Pregnancy duration (in weeks) for human babies.", 38);
    add_setting(PREG_DURATION_MONSTER, "Weeks Monster P", "Pregnancy duration (in weeks) for monster babies.", 20);
    add_setting(PREG_WEEKS_GROW, "Weeks Till Grown", "How long (weeks) it takes for a child to grow up.", 60);
    add_setting(PREG_COOL_DOWN, "Cool Down", "How long it take after birth until the next pregnancy is possible.", 4);
    add_setting(PREG_GOOD_FACTOR, "Good Preg Factor", "??? FIGURE OUT WHAT THIS DOES ???", 2.f);

    add_setting(MONEY_SELL_ITEM, "Sell Item Factor", "Percentage of item's worth (buy price) you get when selling", sPercent(.5f));
    add_setting(MONEY_SELL_SLAVE, "Sell Slave Factor", "Percentage of slave's worth you get when selling", sPercent(.9f));
    add_setting(MONEY_BUY_SLAVE, "Buy Slave Factor", "Factor of slave's worth used when buying", sPercent(.9f));
    add_setting(MONEY_COST_ROOM, "Room Cost", "The cost for buying a new room.", 1000);
    add_setting(MONEY_COST_CONTRA, "Contraception Cost", "The cost of a contraception potion.", 2);
    add_setting(MONEY_COST_HP, "HP Potion Cost", "The cost of a healing potion (for gangs).", 10);
    add_setting(MONEY_COST_NET, "Net Cost", "The cost of a net (for gangs).", 5);

    add_setting(TORTURE_INJURY_CHANCE, "Torture Injury Chance", "Base chance for inflicting permanent damage on a girl during torture.",
                sPercent(.03f));

    add_setting(BALANCING_HEALTH_REGAIN, "Weekly Girl Health Regain", "Home many health points girls regain each week.", 2);
    add_setting(BALANCING_FATIGUE_REGAIN, "Weekly Girl Fatigue Regain", "Home many fatigue points girls regain each week.", 2);

    add_setting(MOVIES_SATURATION_DECAY, "Audience Saturation Decay", "Percentage of sated moviegoers that will be reset each week.", sPercent(0.02f));
    add_setting(MOVIES_HYPE_DECAY, "Hype Decay", "Percentage of hype points a movie loses each week", sPercent(0.05f));
    add_setting(MOVIES_AUDIENCE_UPDATE_CHANCE, "Audience Update Chance", "Weekly chance to re-randomize a target group.", sPercent(0.02f));
    add_setting(MOVIES_REPEATED_SCENE_FACTOR, "Repeated Scene Factor", "Decrease in value if multiple scenes of the same type are used in one movie.", sPercent(0.2f));
    add_setting(MOVIES_RUNNING_WEEKS, "Movie Running Weeks", "How many weeks a movie will be shown.", 25);
    add_setting(MOVIES_AUDIENCE_BASE_FRACTION, "Base Audience", "Base chance for viewers to go see a movie.", sPercent(0.05f));
    add_setting(MOVIES_AUDIENCE_SATED_CHANCE, "Audience Sated Chance", "Chance that a moviegoer will become sated after seeing a movie.",sPercent(0.5f));
}

void cGameSettings::add_setting(const char* tag, const char* name, const char* desc, settings_value_t default_value)
{
    m_Settings[tag] = sKeyValueEntry{tag, name, desc, std::move(default_value)};
}

const settings_value_t & cGameSettings::get_value(const char* name) const
{
    return m_Settings.at(std::string(name)).value;
}

void cGameSettings::save_xml(tinyxml2::XMLElement& target) const {
    auto& root = PushNewElement(target, "Settings");
    for(auto& s : m_Settings) {
       auto& el = PushNewElement(root, m_ElementName);
       el.SetAttribute(m_KeyName, s.first.c_str());
       save_value_xml(el, s.second.value);
    }
}

std::vector<const sKeyValueEntry*> cGameSettings::list_all_settings() const {
    std::vector<const sKeyValueEntry*> result;
    result.reserve(m_Settings.size());
    for(auto& setting : m_Settings)
        result.push_back(&setting.second);
    return std::move(result);
}

settings_value_t& cGameSettings::get_value(const char* name) {
    return m_Settings.at(std::string(name)).value;
}

