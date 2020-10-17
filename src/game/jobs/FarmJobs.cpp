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

#include "cGirlGangFight.h"
#include "FarmJobs.h"
#include "character/sGirl.h"
#include "character/cPlayer.h"
#include "character/predicates.h"
#include "character/cCustomers.h"
#include "Game.hpp"
#include "cGirls.h"
#include "sStorage.h"
#include "cInventory.h"
#include "cGold.h"
#include "buildings/cDungeon.h"
#include "buildings/IBuilding.h"
#include "buildings/cBuildingManager.h"

namespace settings {
    extern const char* SLAVE_MARKET_UNIQUE_CHANCE;
}

cFarmJob::cFarmJob(JOBS job, sFarmJobData data) :
    cBasicJob(job), m_Data(data) {

}

cFarmJob::cFarmJob(JOBS job, const char* xml, sFarmJobData data) :
    cBasicJob(job, xml), m_Data(data) {

}


bool cFarmJob::DoWork(sGirl& girl, bool is_night) {
    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    ss << get_text("work") << "\n \n";

    // farm jobs are generally not tipping jobs
    girl.m_Tips = 0;
    wages = m_Data.BaseWages;

    double performance = girl.job_performance(job(), false);
    return JobProcessing(girl, *girl.m_Building, is_night, performance);
}

void cFarmJob::HandleGains(sGirl& girl, int enjoy) {
    // update enjoyment
    girl.upd_Enjoyment(m_Data.Action, enjoy);
    apply_gains(girl);
}

IGenericJob::eCheckWorkResult cFarmJob::CheckWork(sGirl& girl, bool is_night) {
    if (girl.disobey_check(m_Data.Action, job()))
    {
        ss << get_text("refuse") << " " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;
    }
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}


class cFarmJobFarmer : public cFarmJob {
public:
    cFarmJobFarmer();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance);
};

cFarmJobFarmer::cFarmJobFarmer() : cFarmJob(
        JOB_FARMER, "Farmer.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobFarmer::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100(), roll_b = d100(), roll_c = d100();

    int enjoy = 0;
    int imagetype = IMGTYPE_FARM;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    double foodproduced = performance;
    int alchemyproduced = 0;
    int goodsproduced = 0;

    if (performance >= 245)
    {
        ss << "Her basket practically fills itself as she walks down the rows of crops.";
        foodproduced *= 5; roll_a += 10; roll_b += 25;
    }
    else if (performance >= 185)
    {
        ss << "Her hands moved like lightning as she picked only the best crops.";
        foodproduced *= 4; roll_a += 5; roll_b += 18;
    }
    else if (performance >= 145)
    {
        ss << "She knows exactly when the crops are ready to be picked and how to best collect them.";
        foodproduced *= 3; roll_a += 2; roll_b += 10;
    }
    else if (performance >= 100)
    {
        ss << "She can pick the crops fairly well without too many culls.";
        foodproduced *= 2;
    }
    else if (performance >= 70)
    {
        ss << "She isn't very good at knowing which plants are ripe and which should have been left a little longer.";
        roll_a -= 2; roll_b -= 5;
    }
    else
    {
        ss << "She seems to take more of the unuseable parts of the plants than she takes the edible parts.";
        wages -= 10; foodproduced *= 0.8; roll_a -= 5; roll_b -= 10;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    int tired = (300 - (int)performance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    if (roll_a <= 10)
    {
        tired /= 8;
        enjoy -= uniform(0, 2);
        if (roll_b < 20)    // injury
        {
            girl.health(-uniform(1, 5));
            foodproduced *= 0.8;
            if (chance(girl.magic() / 2))
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to use magic to do her work for her, the magic rebounded on her";
            }
            else ss << "She cut herself while working";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident at the Farm.", COLOR_RED);
                return false;    // not refusing, she is dead
            }
            else ss << ".";
        }
        else    // unhappy
        {
            foodproduced *= 0.9;
            ss << "She did not like working in the fields today.";
            girl.happiness(-uniform(0, 10));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        foodproduced *= 1.1;
        enjoy += uniform(0, 2);
        /* */if (roll_b < 50)    ss << "She kept a steady pace by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        enjoy += uniform(0, 1);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Create Items                //

    if (chance((girl.farming() + girl.magic()) / 10) && chance(performance / 10))
    {
        std::string itemname; int itemnumber = 1;
        /* */if (roll_c > 30)    { itemname = "Nut of Knowledge";        itemnumber = (roll_c > 90 ? uniform(2, 4) : 1); }
        else if (roll_c > 10)    { itemname = "Mango of Knowledge";        itemnumber = (roll_c > 28 ? 2 : 1); }
        else/*            */    { itemname = "Watermelon of Knowledge"; itemnumber = (roll_c == 9 ? 2 : 1); }

        sInventoryItem* item = g_Game->inventory_manager().GetItem(itemname);
        if (item)
        {
            g_Game->player().add_item(item, itemnumber);
            ss << "While picking crops, ${name} sensed a magical aura and found ";
            if (itemnumber == 1) ss << "a"; else ss << itemnumber;
            ss << " " << itemname << ".\n";
            foodproduced -= itemnumber;
        }
    }
    if (chance(girl.herbalism() / 2) && chance(performance / 10))
    {
        alchemyproduced = uniform(1, girl.herbalism() / 10);
        ss << "While sorting the day's haul, ${name} came across ";
        if (alchemyproduced == 1) ss << "a specimen";
        else ss << alchemyproduced << " specimens";
        ss << " that would work well in potions.\n";
        foodproduced -= alchemyproduced;
    }
    if (chance(girl.crafting() / 2) && chance(performance / 10))
    {
        goodsproduced = uniform(1, girl.crafting() / 10);
        ss << "${name} created ";
        if (goodsproduced == 1) ss << "a little toy";
        else ss << goodsproduced << " little toys";
        ss << " from the unusable parts of her crops.\n";
    }
    if (foodproduced <= 0) foodproduced = 0;

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        foodproduced *= 0.9;
        wages = 0;
    }
    else
    {
        wages += (int)foodproduced / 100; // `J` Pay her based on how much she brought in
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    if (foodproduced > 0)
    {
        g_Game->storage().add_to_food((int)foodproduced);
        ss << "\n${name} brought in " << (int)foodproduced << " units of food.";
    }
    if (alchemyproduced > 0)    g_Game->storage().add_to_alchemy((int)alchemyproduced);
    if (goodsproduced > 0)        g_Game->storage().add_to_goods((int)goodsproduced);

    // Money
    girl.m_Pay = std::max(0, wages);

    // Update Enjoyment
    if (performance < 50) enjoy -= 1;
    if (performance < 0) enjoy -= 1;    // if she doesn't do well at the job, she enjoys it less
    if (performance > 200) enjoy *= 2;        // if she is really good at the job, her enjoyment (positive or negative) is doubled
    HandleGains(girl, enjoy);
    if (tired > 0) girl.tiredness(tired);

    // Push out the turn report
    girl.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
    return false;
}

class cFarmJobMarketer : public cFarmJob {
public:
    cFarmJobMarketer();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

cFarmJobMarketer::cFarmJobMarketer() : cFarmJob(JOB_MARKETER, "Marketer.xml",
    {ACTION_WORKCUSTSERV, 20}) {
}

double cFarmJobMarketer::GetPerformance(const sGirl& girl, bool estimate) const {
    double basic = cBasicJob::GetPerformance(girl, estimate);
    if (girl.morality() > 50)          basic -= 5;    // too honest to cheat the customer
    else if (girl.morality() < -50)    basic -= 5;    // too crooked to be trusted with an honest price
    return basic;
}

bool cFarmJobMarketer::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100();
    int enjoy = 0;
    int imagetype = IMGTYPE_PROFILE;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region    //    Create Items                //

    int ForSale_HandmadeGoods = g_Game->storage().get_excess_goods();
    int ForSale_Beasts = g_Game->storage().get_excess_beasts();
    int ForSale_Food = g_Game->storage().get_excess_food();
    int ForSale_Drinks = g_Game->storage().get_excess_drinks();
    int ForSale_Alchemy =g_Game->storage().get_excess_alchemy();

    double pricemultiplier = 1.0;

    // `J` Farm Bookmark - adding in items that can be created in the farm
    if (ForSale_Food >= 10000 && chance(5))
    {
        std::shared_ptr<sGirl> ugirl = nullptr;
        int cost = 10000;
        if (ForSale_Food >= 15000 && chance( 100.f * g_Game->settings().get_percent( settings::SLAVE_MARKET_UNIQUE_CHANCE ) ))
        {
            cost = 15000;
            ugirl = g_Game->GetRandomGirl();                // Unique girl type
        }
        if (ugirl == nullptr)        // if not unique or a unique girl can not be found
        {
            cost = 10000;
            ugirl = g_Game->CreateRandomGirl(0);    // create a random girl
        }
        if (ugirl)
        {
            std::stringstream Umsg;
            // improve her a bit because she should be a good girl to be traded
            ugirl->tiredness(-100);                    ugirl->happiness(100);        ugirl->health(100);
            ugirl->charisma(uniform(10, 40));            ugirl->beauty(uniform(10, 20));
            ugirl->constitution(uniform(-10, 30));        ugirl->intelligence(uniform(-5, 10));
            ugirl->confidence(uniform(-20, 10));        ugirl->agility(uniform(-5, 20));
            ugirl->strength(uniform(1, 20));            ugirl->obedience(uniform(10, 50));
            ugirl->spirit(uniform(-50, 50));            ugirl->morality(uniform(10, 50));
            ugirl->refinement(uniform(-10, 30));        ugirl->dignity(uniform(-20, 20));
            ugirl->npclove(rng().bell(-100, 100));        // she may have had a boyfriend before she got sold to you
            ugirl->service(uniform(10, 40));            ugirl->performance(uniform(0, 5));
            ugirl->crafting(uniform(0, 10));                ugirl->herbalism(uniform(0, 5));
            ugirl->farming(uniform(0, 10));                ugirl->brewing(uniform(0, 5));
            ugirl->animalhandling(uniform(0, 10));            ugirl->cooking(uniform(0, 20));

            Umsg << "${name} was purchased by Farm Marketer ${name} in exchange for " << cost << " units of food.\n";
            ugirl->AddMessage(Umsg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
            ss << "\n \nA merchant from a far off village brought a girl from his village to trade for " << cost << " units of food.\n" << ugirl->FullName() << " has been sent to your dungeon.\n";

            g_Game->dungeon().AddGirl(std::move(ugirl), DUNGEON_NEWGIRL);    // Either type of girl goes to the dungeon
            g_Game->storage().add_to_food(-cost);
        }
    }

#pragma endregion
#pragma region //    Job Performance            //

    if (performance >= 245)
    {
        ss << " She must be the perfect at this.\n \n";
        pricemultiplier += 1.0;
    }
    else if (performance >= 185)
    {
        ss << " She's unbelievable at this.\n \n";
        pricemultiplier += 0.5;
    }
    else if (performance >= 145)
    {
        ss << " She's good at this job.\n \n";
        pricemultiplier += 0.2;
    }
    else if (performance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
    }
    else if (performance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        pricemultiplier -= 0.2;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        pricemultiplier -= 0.5;
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    if (roll_a <= 10)
    {
        enjoy -= uniform(0, 2);
        pricemultiplier -= 0.1;
        girl.happiness(-uniform(0, 10));
        ss << "She did not like selling things today.";
    }
    else if (roll_a >= 90)
    {
        pricemultiplier += 0.1;
        enjoy += uniform(0, 2);
        girl.happiness(uniform(0, 7));
        ss << "She had a great time selling today.";
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Money                    //

    double gold = 0.0;
    // start with how many of each she can sell
    int Sell_HandmadeGoods    = std::min(ForSale_HandmadeGoods,    (int)(performance / 2.0 * ForSale_HandmadeGoods));
    int Sell_Beasts            = std::min(ForSale_Beasts,            (int)(performance / 2.5 * ForSale_Beasts));
    int Sell_Food            = std::min(ForSale_Food,                (int)(performance / 2.0 * ForSale_Food));
    int Sell_Drinks            = std::min(ForSale_Drinks,            (int)(performance / 2.0 * ForSale_Drinks));
    int Sell_Alchemy        = std::min(ForSale_Alchemy,            (int)(performance / 3.0 * ForSale_Alchemy));
    // for how much
    gold += (pricemultiplier * 1 * Sell_HandmadeGoods);
    gold += (pricemultiplier * uniform(50, 500) * Sell_Beasts);
    gold += (pricemultiplier * 0.1 * Sell_Food);
    gold += (pricemultiplier * 0.1 * Sell_Drinks);
    gold += (pricemultiplier * uniform(10, 150) * Sell_Alchemy);
    // remove them from the count
    g_Game->storage().add_to_goods(-Sell_HandmadeGoods);
    g_Game->storage().add_to_beasts(-Sell_Beasts);
    g_Game->storage().add_to_food(-Sell_Food);
    g_Game->storage().add_to_drinks(-Sell_Drinks);
    g_Game->storage().add_to_alchemy(-Sell_Alchemy);
    // tell the player
    if (gold > 0)    // something was sold
    {
        ss << "${name} was able to sell:\n";

        if (Sell_HandmadeGoods > 0)
        {
            if (Sell_HandmadeGoods == ForSale_HandmadeGoods) ss << "All " << Sell_HandmadeGoods;
            else ss << Sell_HandmadeGoods << " of the " << ForSale_HandmadeGoods;
            ss << " Handmade Goods.\n";
        }
        if (Sell_Beasts > 0)
        {
            if (Sell_Beasts == ForSale_Beasts) ss << "All " << Sell_Beasts;
            else ss << Sell_Beasts << " of the " << ForSale_Beasts;
            ss << " Beasts.\n";

        }
        if (Sell_Food > 0)
        {
            if (Sell_Food == ForSale_Food) ss << "All " << Sell_Food;
            else ss << Sell_Food << " of the " << ForSale_Food;
            ss << " Food.\n";
        }
        if (Sell_Drinks > 0)
        {
            if (Sell_Drinks == ForSale_Drinks) ss << "All " << Sell_Drinks;
            else ss << Sell_Drinks << " of the " << ForSale_Drinks;
            ss << " Drinks.\n";
        }
        if (Sell_Alchemy > 0)
        {
            if (Sell_Alchemy == ForSale_Alchemy) ss << "All " << Sell_Alchemy;
            else ss << Sell_Alchemy << " of the " << ForSale_Alchemy;
            ss << " Alchemy Items.\n";
        }
        ss << "She made a total of " << (int)gold << " from it all.\nShe gets 1% of the sales: " << (int)(gold / 100)<<".\nThe rest goes directly into your coffers.\n \n";
        wages += (int)(gold / 100); // `J` Pay her based on how much she brought in
        gold -= (int)(gold / 100);
        enjoy += (int)(wages / 100);        // the more she gets paid, the more she likes selling
    }

#pragma endregion
#pragma region    //    Finish the shift            //


    // Money
    girl.m_Pay = std::max(0, wages);
    g_Game->gold().farm_income(gold);
    HandleGains(girl, enjoy);

    // Push out the turn report
    girl.AddMessage(ss.str(), imagetype, msgtype);

    return false;
}


class cFarmJobVeterinarian : public cFarmJob {
public:
    cFarmJobVeterinarian();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
private:
};

cFarmJobVeterinarian::cFarmJobVeterinarian() : cFarmJob(
        JOB_VETERINARIAN, "Veterinarian.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobVeterinarian::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100();
    int enjoy = 0;

#pragma region //    Job Performance            //

    int fame = 0;
    if (performance >= 245)
    {
        wages += 155;    fame += 2;
        ss << " She must be the perfect at this.\n \n";
    }
    else if (performance >= 185)
    {
        wages += 95;    fame += 1;
        ss << " She's unbelievable at this.\n \n";
    }
    else if (performance >= 145)
    {
        wages += 55;
        ss << " She's good at this job.\n \n";
    }
    else if (performance >= 100)
    {
        wages += 15;
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
    }
    else if (performance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Create Items                //

    // `J` Farm Bookmark - adding in items that can be created in the farm


#pragma endregion
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Money
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    girl.fame(fame);
    HandleGains(girl, enjoy);

#pragma endregion
    return false;
}

class cFarmJobShepherd : public cFarmJob {
public:
    cFarmJobShepherd();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
};

cFarmJobShepherd::cFarmJobShepherd() : cFarmJob(
        JOB_SHEPHERD, "Shepherd.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobShepherd::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100(), roll_b = d100();
    int enjoy = 0;
    int imagetype = IMGTYPE_HERD;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    // brings in food and rarely beasts
    double beasts = performance / 100;
    double food = performance;

    if (performance >= 245)
    {
        ss << "The animals come to ${name} when they are ready for market.";
        beasts *= 2.0; food *= 2.0; roll_a += 10; roll_b += 25;
    }
    else if (performance >= 185)
    {
        ss << "${name} seems to know just when to choose the best animals to send to market.";
        beasts *= 1.6; food *= 1.6; roll_a += 5; roll_b += 18;
    }
    else if (performance >= 145)
    {
        ss << "${name} has little trouble finding animals to send to market.";
        beasts *= 1.2; food *= 1.2; roll_a += 2; roll_b += 10;
    }
    else if (performance >= 100)
    {
        ss << "${name} usually sends the right animals to market.";
    }
    else if (performance >= 70)
    {
        ss << "${name} doesn't know what animals to send to the market so she just guesses.";
        beasts *= 0.8; food *= 0.8; roll_a -= 2; roll_b -= 5;
    }
    else
    {
        ss << "${name} has no idea what she is doing.";
        beasts *= 0.5; food *= 0.5; wages -= 10; roll_a -= 5; roll_b -= 10;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    // Complications
    if (roll_a <= 10)
    {
        enjoy -= uniform(1, 3);
        ss << "The animals were uncooperative and some didn't even let her get near them.\n";
        if (chance(20))
        {
            enjoy--;
            ss << "Several animals got out and ${name} had to chase them down.\n";
            girl.happiness(-uniform(1, 5));
            girl.tiredness(uniform(1, 15));
            beasts *= 0.8;
            food *= 0.9;
        }
        if (chance(20))
        {
            enjoy--;
            int healthmod = uniform(1, 10);
            girl.health(-healthmod);
            girl.happiness(-uniform(healthmod, 2*healthmod));
            ss << "One of the animals kicked ${name} and ";
            if (girl.health() < 1)
            {
                ss << "killed her.\n";
                g_Game->push_message(girl.FullName() + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
                return false;    // not refusing, she is dead
            }
            else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
            beasts *= 0.9;
            food *= 0.8;
        }
    }
    else if (roll_a >= 90)
    {
        enjoy += uniform(1, 3);
        ss << "The animals were pleasant and cooperative today.\n";
        beasts *= 1.1;
        food *= 1.1;
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "She had an uneventful day tending the animals.\n";
    }

#pragma endregion
#pragma region    //    Create Items                //




    // `J` Farm Bookmark - adding in items that can be created in the farm

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        beasts *= 0.9;
        food *= 0.9;
        wages = 0;
    }
    else    // `J` Pay her based on how much she brought in
    {
        if (food > 0)        wages += (int)food / 100;
        if (beasts > 0)        wages += (int)beasts;
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    ss << "\n${name}";
    if ((int)beasts > 0)
    {
        g_Game->storage().add_to_beasts((int)beasts);
        ss << " brought " << (int)beasts << " beasts to work in the brothels";
    }
    if ((int)beasts > 0 && (int)food > 0)
    {
        ss << " and";
    }
    if ((int)food > 0)
    {
        g_Game->storage().add_to_food((int)food);
        ss << " sent " << (int)food << " units of food worth of animals to slaughter";
    }
    if ((int)beasts <= 0 && (int)food <= 0)
    {
        ss << " did not bring in any animals";
    }
    ss << ".";
    
    girl.AddMessage(ss.str(), imagetype, msgtype);

    // Money
    girl.m_Pay = std::max(0, wages);
    
    HandleGains(girl, enjoy);

#pragma endregion
    return false;
}

class cFarmJobRancher : public cFarmJob {
public:
    cFarmJobRancher();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
};

cFarmJobRancher::cFarmJobRancher() : cFarmJob(
        JOB_RANCHER, "Rancher.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobRancher::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100(), roll_b = d100();

    int enjoy = 0;
    int imagetype = IMGTYPE_FARM;

#pragma region //    Job Performance            //

    // brings in beasts and food
    double beasts = performance / 50;
    double food = performance / 5;

    if (performance >= 245)
    {
        ss << "The animals come to ${name} when they are ready for market.";
        beasts *= 2.0; food *= 2.0; roll_a += 10; roll_b += 25;
    }
    else if (performance >= 185)
    {
        ss << "${name} seems to know just when to choose the best animals to send to market.";
        beasts *= 1.6; food *= 1.6; roll_a += 5; roll_b += 18;
    }
    else if (performance >= 145)
    {
        ss << "${name} has little trouble finding animals to send to market.";
        beasts *= 1.2; food *= 1.2; roll_a += 2; roll_b += 10;
    }
    else if (performance >= 100)
    {
        ss << "${name} usually sends the right animals to market.";
    }
    else if (performance >= 70)
    {
        ss << "${name} doesn't know what animals to send to the market so she just guesses.";
        beasts *= 0.8; food *= 0.8; roll_a -= 2; roll_b -= 5;
    }
    else
    {
        ss << "${name} has no idea what she is doing.";
        beasts *= 0.5; food *= 0.5; wages -= 10; roll_a -= 5; roll_b -= 10;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    // Complications
    if (roll_a <= 10)
    {
        enjoy -= uniform(1, 3);
        ss << "The animals were uncooperative and some didn't even let her get near them.\n";
        if (chance(20))
        {
            enjoy--;
            ss << "Several animals got out and ${name} had to chase them down.\n";
            girl.happiness(-uniform(1, 5));
            girl.tiredness(uniform(1, 15));
            beasts *= 0.8;
            food *= 0.9;
        }
        if (chance(20))
        {
            enjoy--;
            int healthmod = uniform(1, 10);
            girl.health(-healthmod);
            girl.happiness(-uniform(healthmod, 2*healthmod));
            ss << "One of the animals kicked ${name} and ";
            if (girl.health() < 1)
            {
                ss << "killed her.\n";
                g_Game->push_message(girl.FullName() + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
                return false;    // not refusing, she is dead
            }
            else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
            beasts *= 0.9;
            food *= 0.8;
        }
    }
    else if (roll_a >= 90)
    {
        enjoy += uniform(1, 3);
        ss << "The animals were pleasant and cooperative today.\n";
        beasts *= 1.1;
        food *= 1.1;
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "She had an uneventful day tending the animals.\n";
    }


#pragma endregion
#pragma region    //    Create Items                //

    // `J` Farm Bookmark - adding in items that can be created in the farm


#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        beasts *= 0.9;
        food *= 0.9;
        wages = 0;
    }
    else    // `J` Pay her based on how much she brought in
    {
        if (food > 0)        wages += (int)food / 100;
        if (beasts > 0)        wages += (int)beasts;
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    ss << "\n${name}";
    if ((int)beasts > 0)
    {
        g_Game->storage().add_to_beasts((int)beasts);
        ss << " brought " << (int)beasts << " beasts to work in the brothels";
    }
    if ((int)beasts > 0 && (int)food > 0)
    {
        ss << " and";
    }
    if ((int)food > 0)
    {
        g_Game->storage().add_to_food((int)food);
        ss << " sent " << (int)food << " units of food worth of animals to slaughter";
    }
    if ((int)beasts <= 0 && (int)food <= 0)
    {
        ss << " did not bring in any animals";
    }
    ss << ".";

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Money
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    HandleGains(girl, enjoy);

#pragma endregion
    return false;
}


class cFarmJobMilker : public cFarmJob {
public:
    cFarmJobMilker();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
};

cFarmJobMilker::cFarmJobMilker() : cFarmJob(
        JOB_MILKER, "Milker.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobMilker::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100(), roll_b = d100();
    int enjoy = 0;
    int imagetype = IMGTYPE_FARM;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    double drinks = performance / 2;

    if (performance >= 245)
    {
        ss << "Her milk bucket practically fills itself as she walks down the rows of cows.";
        drinks *= 5; roll_a += 10; roll_b += 25;
    }
    else if (performance >= 185)
    {
        ss << "Her hands moved like lightning as she gracefully milks the cows teats.";
        drinks *= 4; roll_a += 5; roll_b += 18;
    }
    else if (performance >= 145)
    {
        ss << "She knows exactly when the cows are ready to be milked and how to best milk them.";
        drinks *= 3; roll_a += 2; roll_b += 10;
    }
    else if (performance >= 100)
    {
        ss << "She can milk the cows without spilling much.";
        drinks *= 2;
    }
    else if (performance >= 70)
    {
        ss << "She isn't very good at aiming the teats into the bucket.";
        roll_a -= 2; roll_b -= 5;
    }
    else
    {
        ss << "She can't seem to get the hang of this.";
        wages -= 10; drinks *= 0.8; roll_a -= 5; roll_b -= 10;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    // Complications
    if (roll_a <= 10)
    {
        enjoy -= uniform(1, 3);
        ss << "The animals were uncooperative and some didn't even let her get near them.\n";
        drinks *= 0.8;
        if (chance(20))
        {
            enjoy--;
            ss << "Several animals kicked over the milking buckets and soaked ${name}.\n";
            girl.happiness(-uniform(1, 5));
            drinks -= uniform(5, 10);
        }
        if (chance(20))
        {
            enjoy--;
            ss << "One of the animals urinated on ${name} and contaminated the milk she had collected.\n";
            girl.happiness(-uniform(1, 3));
            drinks -= uniform(5, 10);
        }
        if (chance(20))
        {
            enjoy--;
            int healthmod = uniform(1, 10);
            girl.health(-healthmod);
            girl.happiness(-uniform(healthmod, 2 * healthmod));
            ss << "One of the animals kicked ${name} and ";
            if (girl.health() < 1)
            {
                ss << "killed her.\n";
                g_Game->push_message(girl.FullName() + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
                return false;    // not refusing, she is dead
            }
            else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
            drinks -= uniform(5, 10);
        }
    }
    else if (roll_a >= 90)
    {
        enjoy += uniform(1, 3);
        ss << "The animals were pleasant and cooperative today.\n";
        drinks *= 1.2;
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "She had an uneventful day milking.\n";
    }

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        drinks *= 0.9;
        wages = 0;
    }
    else
    {
        wages += (int)drinks / 100; // `J` Pay her based on how much she brought in
    }

#pragma endregion
#pragma region    //    Create Items                //

    int milk = int(drinks / 10);    // plain milk is made here, breast milk from girls is made in WorkMilk
    if (milk > 0)
    {
        int milkmade[5] = { 0, 0, 0, 0, 0 };    // (total,gallon,quart,pint,shots}
        // check if the milk items exist and only do the checks if at least one of them does
        sInventoryItem* milkitems[4] = { g_Game->inventory_manager().GetItem("Milk"), g_Game->inventory_manager().GetItem("Milk (pt)"), g_Game->inventory_manager().GetItem("Milk (qt)"), g_Game->inventory_manager().GetItem("Milk (g)") };
        if (milkitems[0] != nullptr || milkitems[1] != nullptr || milkitems[2] != nullptr || milkitems[3] != nullptr)
        {
            while (milk > 0)    // add milk
            {
                std::string itemname;
                /* */if (milkitems[3] && milk > 3 && chance(30))
                {
                    milk -= 4;
                    milkmade[0]++;
                    milkmade[4]++;
                    g_Game->player().add_item(milkitems[3]);
                }
                else if (milkitems[2] && milk > 2 && chance(50))
                {
                    milk -= 3;
                    milkmade[0]++;
                    milkmade[3]++;
                    g_Game->player().add_item(milkitems[2]);
                }
                else if (milkitems[1] && milk > 1 && chance(70))
                {
                    milk -= 2;
                    milkmade[0]++;
                    milkmade[2]++;
                    g_Game->player().add_item(milkitems[1]);
                }
                else if (milkitems[0])
                {
                    milk -= 1;
                    milkmade[0]++;
                    milkmade[1]++;
                    g_Game->player().add_item(milkitems[0]);
                }
                else milk--;    // add a reducer just in case.
            }
        }
        if (milkmade[0] > 0)
        {
            ss << "${name} produced " << milkmade[0] << " bottles of milk for you, \n";
            if (milkmade[4] > 0) ss << milkmade[4] << " gallons\n";
            if (milkmade[3] > 0) ss << milkmade[3] << " quarts\n";
            if (milkmade[2] > 0) ss << milkmade[2] << " pints\n";
            if (milkmade[1] > 0) ss << milkmade[1] << " shots\n";
        }
    }

    // `J` zzzzzz - need to add the girl milking herself - can be done easier after WorkMilk is done




#pragma endregion
#pragma region    //    Finish the shift            //

    ss << "\n${name}";
    if ((int)drinks > 0)
    {
        g_Game->storage().add_to_drinks((int)drinks);
        ss << " brought in " << (int)drinks << " units of milk.";
    }
    else { ss << " was unable to collect any milk."; }

    girl.AddMessage(ss.str(), imagetype, msgtype);

    // Money
    girl.m_Pay = std::max(0, wages);

    HandleGains(girl, enjoy);


#pragma endregion
    return false;
}


class cFarmJobBeastCapture : public cFarmJob {
public:
    cFarmJobBeastCapture();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
};

cFarmJobBeastCapture::cFarmJobBeastCapture() : cFarmJob(
        JOB_BEASTCAPTURE, "BeastCapture.xml", {ACTION_COMBAT, 40}) {
}

bool cFarmJobBeastCapture::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {

    cGirls::EquipCombat(girl);    // ready armor and weapons!

    int enjoy = 0;
    int imagetype = IMGTYPE_COMBAT;
    int msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    The Fight to get the Beasts        //

    int tired = 0;
    int gainmax = (int)(performance / 30) + 1;
    int gain = uniform(1, gainmax);
    // TODO make this a real MONSTER, not a girl the player never sees anyway
    auto tempgirl = g_Game->CreateRandomGirl(18, false, false, true);
    if (tempgirl)        // `J` reworked in case there are no Non-Human Random Girls
    {
        auto fight_outcome = GirlFightsGirl(girl, *tempgirl);
        if (fight_outcome == EFightResult::VICTORY)    // she won
        {
            if (gain <= 2)    gain = 2;
            if (gain >= gainmax)    gain = gainmax;
            ss << "She had fun hunting today and came back with " << gain << " new beasts.";;
            imagetype = IMGTYPE_COMBAT;
            tired = uniform(0, 3*gain);
            enjoy += uniform(2, 5);
        }
        else        // she lost or it was a draw
        {
            gain = rng().bell(-gainmax / 3, gainmax / 2);
            ss << " The animals were difficult to track today. ${name} eventually returned worn out and frustrated, ";
            if (gain <= 0)
            {
                gain = 0;
                ss << "empty handed.";
            }
            else
            {
                ss << "dragging ";
                if (gain == 1)    ss << "one";
                if (gain > 1)    ss << gain;
                ss << " captured beast" << (gain > 1 ? "s" : "") << " behind her.";
            }
            imagetype = IMGTYPE_COMBAT;
            enjoy -= uniform(1, 3);
            tired = uniform(20, 20 + 10 * gain);
        }
    }
    else
    {
        g_LogFile.log(ELogLevel::ERROR, "You have no Non-Human Random Girls for your girls to fight!");
        ss << "She came back with just one animal today.\n \n";
        ss << "(Error: You need a Non-Human Random Girl to allow WorkBeastCapture randomness)";
        gain = 1;
        msgtype = EVENT_WARNING;
        tired = 15;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    A Little Randomness            //

    //SIN: A little randomness
    if (((girl.animalhandling() + girl.charisma()) > 125) && chance(30))
    {
        ss << "${name} has a way with animals, a" << (gain > 1 ? "nother" : "") << " beast freely follows her back.\n";
        gain++;
    }
    //SIN: most the rest rely on more than one cap so might as well skip the lot if less than this...
    if (gain > 1)
    {
        // `J` added a switch with a use next if check fails and changed percents to (gain * 5)
        switch (uniform(0, 9))
        {
            case 0:
                if (girl.has_active_trait("Twisted") && girl.has_active_trait("Nymphomaniac") && (girl.libido() >= 80))
                {
                    ss << "Being a horny, twisted nymphomaniac, ${name} had some fun with the beasts before she handed them over.\n";
                    girl.beastiality(uniform(0, gain));
                    girl.libido(-uniform(0, gain));
                    tired += gain;
                    break;
                }
            case 1:
                if (girl.has_active_trait("Psychic") && (girl.libido() >= 90) && chance(gain * 5))
                {
                    ss << "${name}'s Psychic sensitivity caused her mind be overwhelmed by the creatures' lusts";
                    if (is_virgin(girl))
                    {
                        ss << " but, things were moving too fast and she regained control before they could take her virginity.\n";
                        girl.beastiality(1);
                        girl.libido(2); // no satisfaction!
                    }
                    else
                    {
                        ss << ". Many hours later she staggered in to present the creatures to you.\n";
                        girl.beastiality(uniform(0, gain));
                        girl.libido(-uniform(0, 2*gain));
                        girl.tiredness(gain);
                        girl.calc_insemination(cGirls::GetBeast());
                    }
                    tired += gain;
                    break;
                }
            case 2:
                if (girl.has_active_trait("Assassin") && chance(gain * 5))
                {
                    ss << " One of the captured creatures tried to escape on the way back. Trained assassin, ${name}, instantly killed it as an example to the others.\n";
                    girl.combat(1);
                    gain--;
                    break;
                }
            case 3:
                if (girl.tiredness() > 50 && chance(gain * 5))
                {
                    ss << "${name} was so exhausted she couldn't concentrate. One of the creatures escaped.\n";
                    gain--;
                    break;
                }
            default: break;

        }
    }

#pragma endregion
#pragma region    //    Collect Pets                //

    // `J` Farm Bookmark - adding in items that can be gathered in the farm
    if (chance(5))
    {
        std::string itemfound; std::string itemfoundtext;
        int chooseitem = d100();

        if (chooseitem < 25)
        {
            itemfound = chance(50) ? "Black Cat" : "Cat";
            itemfoundtext = "a stray cat and brought it back with her.";
        }
        else if (chooseitem < 50)
        {
            itemfound = "Guard Dog";
            itemfoundtext = "a tough looking stray dog and brought it back with her.";
        }
        else if (chooseitem < 60)
        {
            itemfound = "A Bunch of Baby Ducks";
            itemfoundtext = "a bunch of baby ducks without a mother, so she gathered them up in her shirt and brought them home.";
        }
        else if (chooseitem < 65)
        {
            itemfound = "Death Bear";
            itemfoundtext = "a large bear that seems to have had some training, so she brought it home.";
        }
        else if (chooseitem < 80)
        {
            itemfound = "Pet Spider";
            itemfoundtext = "a strange looking spider so she collected it in a bottle and brought it back with her.";
        }
        else if (chooseitem < 95)
        {
            itemfound = chance(25) ? "Fox Stole" : "Fur Stole";
            itemfoundtext = "a dead animal that was not too badly damaged. She brought it home, skinned it, cleaned it up and made a lovely stole from it.";
        }
        else if (chooseitem < 96)
        {
            itemfound = "Echidna's Snake";
            itemfoundtext = "a rather obedient and psychic snake. It wrapped itself around her crotch and let her take it home.";
        }
        else
        {
            itemfound = "Tiger Shark Tooth";
            itemfoundtext = "a rather large shark tooth and brought it home.";
        }

        sInventoryItem* item = g_Game->inventory_manager().GetItem(itemfound);
        if (item)
        {
            ss << "${name} found " << itemfoundtext;
            g_Game->player().add_item(item);
        }
    }

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        wages = 0;
    }
    else
    {
        wages += gain * 10; // `J` Pay her based on how much she brings back
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    g_Game->storage().add_to_beasts(gain);

    // Money
    girl.m_Pay = std::max(0, wages);

    // Improve girl
    if (tired > 0) girl.tiredness(tired);

    HandleGains(girl, enjoy);

    if (chance(25) && girl.strength() >= 60 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, "${name} has become pretty Strong from all of the fights she's been in.", is_night);
    }

    girl.AddMessage(ss.str(), imagetype, msgtype ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

#pragma endregion
    return false;
}


class cFarmJobGetMilked : public cFarmJob { 
public:
    cFarmJobGetMilked();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
private:
    double toOz(int ml)            { return (0.0338 * ml); }
};

cFarmJobGetMilked::cFarmJobGetMilked() : cFarmJob(
        JOB_MILK, "GetMilked.xml", {ACTION_WORKMILK, 0}) {
}

bool cFarmJobGetMilked::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    const sGirl* farmmanonduty = random_girl_on_job(*girl.m_Building, JOB_FARMMANGER, is_night);
    std::string farmmanname = (farmmanonduty ? "Farm Manager " + farmmanonduty->FullName() + "" : "the Farm Manager");

    int enjoy = 0;
    int roll = d100();
    /*int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();*/


    //\\BSIN - doesn't look like this has been touched in the last year - completing
    //\\On testing, decided to half the output volume on virgins and non-pregs without MILF trait.
    // `J` New job function - needs work - commenting out for now
    /*
    100 lactation + preg + notrait = volume should be 20-30oz per day, 140-210oz per week

    *Breast size*            *L*        *P*        *B*        *ml*    *oz*    *pt*            *L* = Lactation
    Flat Chest                100        2        1        200         6.8    0.4                *P* = Pregnant
    Petite Breasts            100        2        2        400        13.5    0.8                *B* = Breast size multiplier
    Small Boobs                100        2        3        600        20.3    1.3                *ml* = milliliters
    (Normal)                100        2        4        800        27.1    1.7                *oz* = ounces
    Busty Boobs                100        2        5        1000    33.8    2.1                *Pt* = pints
    Big Boobs                100        2        6        1200    40.6    2.5
    Giant Juggs                100        2        7        1400    47.3    3.0                //// Lactation traits increase or decrease lactation up to 40%
    Massive Melons            100        2        8        1600    54.1    3.4
    Abnormally Large Boobs    100        2        9        1800    60.9    3.8
    Titanic Tits            100        2        10        2000    67.6    4.2

    //*/

    //BSIN
    //
    //MAGIC NUMBER COMING NEXT - milk seems to sell for 50G/oz, so trade/wholesale value of 10 seems reasonable and produces fair output.
    //Gives less than current for small breasted, non-pregs, and more for pregnant breasted girls.
    const int MILKWHOLESALE = 10;
    const int CATGIRLBONUS = 2;            //CG milk is four times as much in shop - trade boost of 2 seems right (shop pockets difference)
    //
    //
    //
    //INITIATE THINGS I NEED
    int volume = 0;                        // in milliliters
    int lactation = girl.lactation();    // how much?
    bool isPregnant = girl.is_pregnant();
    int pregMultiplier = ((isPregnant) ? 2 : 1);
    bool yours = girl.has_status(STATUS_PREGNANT_BY_PLAYER);
    int HateLove = 0;
    HateLove = girl.pclove() - girl.pchate();
    //BSIN: Update: to generate an extra message in case of certain event
    std::stringstream ssextra;
    int extraimage = 0;
    bool extraEvent = false;
    bool noAnti = girl.m_UseAntiPreg = false;

    int breastsize = girl.breast_size();

    // Milk - a multiplier on base lactation
    if (girl.has_active_trait("Dry Milk"))                        { lactation = (lactation * 4) / 10; }
    if (girl.has_active_trait("Scarce Lactation"))                { lactation = (lactation * 6) / 10; }
    if (girl.has_active_trait("Abundant Lactation"))            { lactation = (lactation * 14) / 10; }
    if (girl.has_active_trait("Cow Tits"))                        { lactation = (lactation * 16) / 10; }



    //test code for auto preg
    if (girl.m_WeeksPreg < 0 && brothel.num_girls_on_job(JOB_FARMMANGER, false) >= 1 && noAnti && !is_virgin(girl))
    {
        sCustomer Cust = g_Game->GetCustomer(brothel);
        ss << farmmanname <<" noticing that ${name} wasn't pregnant decided to take it upon herself to make sure she got knocked up.\n";
        if (brothel.is_sex_type_allowed(SKILL_BEASTIALITY) && g_Game->storage().beasts() >= 1 && chance(50))
        {
            ss << "She sends in one of your beasts to get the job done.";
            girl.beastiality(2);
            girl.AddMessage(ss.str(), IMGTYPE_BEAST, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            }
        }
        else
        {
            ss << "She found a random man off the street and offered him the chance to have sex with ${name} for free as long as he cummed inside her. He jumped at the chance for it.";
            girl.normalsex(2);
            girl.AddMessage(ss.str(), IMGTYPE_SEX, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
    }


    //Lets get some scenario...
    // TODO Milk is currently not a valid action!
    int predisposition = 0; //girl.get_enjoyment(JOB_MILK);
    /**/ if (predisposition <= -50)    ss << "${name} froze when she saw the milking area. She had to be strapped in securely for milking.";
    else if (predisposition <= 10)    ss << "${name} was led into the stall, strapped in and milked without incident.";
    else if (predisposition <= 50)    ss << "${name} walked into the stall, sat down and got her breasts out. She massaged her breasts in preparation for being milked.";
    else                            ss << "${name} ran to the stall, stripped off all her clothes, and waited impatiently for the milker to get to her. She seems to like this.";
    ss << "\n \n";


    //Milking action - adapted from below
    if (breastsize <= 3)   // Using breastsize figure found above to reduce repeated trait calls
    {
        if (isPregnant)
        {
            ss << "${name} only has little breasts, but her body still produces milk in anticipation of nursing " << ((yours) ? "your" : "her") << " child.";
        }
        else
        {
            ss << "${name} has small breasts, which only produce a small yield.";
        }
    }
    else if (breastsize == 4)
    {
        if (isPregnant)
        {
            ss << "${name} has average sized breasts, which yield a fair amount of milk " << ((yours) ? "now you've knocked her up." : "with the help of her pregnancy.");
        }
        else
        {
            ss << "${name} has average sized breasts, perfect handfuls, which yield an okay amount of milk.";
        }
    }
    else if (breastsize <= 7)
    {
        if (isPregnant)
        {
            ss << "${name}'s already sizable breasts have become engorged with milk in preparation for " << ((yours) ? "your" : "her") << " child.";
        }
        else
        {
            ss << "${name} has large breasts, that yield a good amount of milk to the suction machine even without pregnancy.";
        }
    }
    else
    {
        if (isPregnant)
        {
            ss << "${name} had ridiculously large breasts already. Now she's pregnant " << ((yours) ? "with your child " : "")
               << "her tits are each larger than her head, and are dripping with milk near continuously.";
        }
        else
        {
            ss << "${name}'s massive globes don't need pregnancy to yield a profitable quantity of milk!";
        }
    }
    ss << "\n \n";

    //Calculating base volume per above formula
    volume = lactation * pregMultiplier * breastsize;    //vol in ml
    //some randomization to prevent identical production numbers every time
    int randVol = volume / 10;            // rand vol = 10 percent of vol
    volume -= randVol;                    // removing the 10 percent
    volume += uniform(0, 2 * randVol);    // adding back between 0 and 20%


    //Testing and seems weird that virgins and never-pregs can produce so much, so halving this
    //This is every way I can find of asking if she's had a kid - MILF needed as this will register children prior to employment
    if (is_virgin(girl) || (!isPregnant && !girl.has_active_trait("MILF") && girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS] < 1))
    {
        volume /= 2;                                            // never preg, so not producing much
        girl.lactation(uniform(0, 2));    //all this pumping etc induces lactation
    }

    ///////////////////
    //let's see if there's a milker, and if so, influence this a little.
    const sGirl* milker = random_girl_on_job(brothel, JOB_MILKER, is_night);;

    //O/P for info and debug

    if (milker)
    {
        ss << "She was milked by ";
        ss << milker->FullName();
        if (milker->has_active_trait("No Hands") || milker->has_active_trait("No Arms"))
        {
            ss << ", who really struggled. Why would you pick someone with no hands to be a milker?";
            volume /= 2;
        }
        else if (milker->has_active_trait("MILF") && chance(50))
        {
            ss << ", a mother, who has experience extracting milk effectively.";
            volume += (volume / 5);
        }
        else if (milker->has_active_trait("Sadistic") && chance(35))
        {
            ss << ", who seemed more interested in slapping ${name}'s breasts and twisting her nipples than in actually trying to get milk out.";
            volume -= (volume / 5);
        }
        else if (milker->has_active_trait("Lesbian") && chance(40))
        {
            ss << ", who massaged ${name}'s breasts thoroughly and was careful to thoroughly arouse the nipple with her tongue before attaching the cup. This helped with milking.";
            volume += (volume / 10);
            girl.upd_temp_stat(STAT_LIBIDO, 5, true);
        }
        else if (girl.has_active_trait("Clumsy") && chance(40))
        {
            ss << ", who did a great job milking ${name}'s breasts, but then tripped over the bucket, spilling quite a lot.";
            volume -= (volume / 4);
        }
        else if (milker->has_active_trait("Straight") && chance(40))
        {
            ss << ", who clearly didn't want to touch another woman's breasts. This made the milking akward and inefficient.";
            volume -= (volume / 10);
        }
        else if (milker->has_active_trait("Cum Addict") && chance(45))
        {
            ss << ", who kept compaining that she'd rather be 'milking' men.";
        }
        else ss << ".";
    }
    else ss << "She was milked by some farmhand.";
    ss << "\n";

    //to ounces
    double milkProduced = toOz(volume);                    //vol in oz

    //Creating new var to reflect the ease of milking her. Basing this on lactation, rather than dice roll, as seems more real
    //if they lactate a lot, the machines draining off the weight will be good
    //if not, the dry pulling on the nipples would hurt
    //
    int ease = volume;
    //
    // Nipples affects ease which is used to adjust the girls enjoyment and damage
    if (girl.has_active_trait("Inverted Nipples")) ease -= 20;
    if (girl.has_active_trait("Puffy Nipples")) ease += 40;
    if (girl.has_active_trait("Perky Nipples")) ease += 20;


    if (ease < 75)
    {
        enjoy -= 2 * (roll % 3 + 2);                                // -8 to -4
        girl.health(-(roll % 6));            // 0 to 5 damage
        ss << "She's barely lactating, so this was a slow, painful process that left her with raw, ";
        if (girl.has_active_trait("Missing Nipple") || girl.has_active_trait("No Nipples")) ss << "aching breasts.";
        else ss << "bleeding nipples.";
    }
    else if (ease < 150)
    {
        enjoy -= 2 * (roll % 3 + 1);                                // -6 to -2
        girl.health(-(roll % 3));            // 0 to 2 damage
        ss << "She's barely producing so all the squeezing, tugging and suction-cup just left her breasts raw and painful.";
    }
    else if (ease < 300)
    {
        enjoy -= roll % 3 + 1;                                        // -3 to -1
        ss << "It was unpleasant. She produced little milk and the suction-cup left her breasts aching.";
    }
    else if (ease < 600)
    {
        enjoy++;                                                    // +1
        ss << "Being milked was okay for her.";
    }
    else if (ease < 1200)
    {
        enjoy += roll % 3 + 1;                                        // +1 to +3
        ss << "Being milked felt good today.";
    }
    else if (ease < 1600)
    {
        enjoy += 2 * (roll % 3 + 1);                                // +2 to +6
        girl.happiness((roll % 3));        // 0 to 2 happiness
        ss << "Her breasts were uncomfortably full. Getting that weight off felt great.";
    }
    else
    {
        enjoy += 2 * (roll % 3 + 2);                                // +4 to +8
        girl.happiness((roll % 6));        // 0 to 5 happiness
        ss << "Her breasts were so full milk was leaking through her clothes. Finally getting milked felt incredible.";
    }
    ss << "\n \n";

    //value calculations
    int milkValue = int(milkProduced * MILKWHOLESALE);        //Base value
    int traitBoost = milkValue;                                // now basing the boost on base value, not on inflated CG value.

    if (girl.has_active_trait("Cat Girl"))
    {
        ss << "Cat-Girl breast-milk has higher value.\n";
        milkValue *= CATGIRLBONUS;
    }

    //finally a little randomness
    if (volume > 0)   // no point mentioning this if she doesn't produce anything
    {
        if (chance(60) && (girl.has_active_trait("Fallen Goddess") || girl.has_active_trait("Goddess")))
        {
            ss << "Customers are willing to pay much more to sup from the breast of a Goddess.\n";
            milkValue += (2 * traitBoost);
        }
        else if (chance(40) && girl.has_active_trait("Demon"))
        {
            ss << "Customers are thrilled at the chance to consume the milk of a Demon.\n";
            milkValue += (2 * traitBoost);
        }
        else if (chance(50) && girl.has_active_trait("Queen"))
        {
            ss << "Customers are willing to pay more to enjoy the breast-milk of a Queen.\n";
            traitBoost *= uniform(1, 2);
            milkValue += traitBoost;
        }
        else if (chance(50) && girl.has_active_trait("Princess"))
        {
            ss << "Customers are willing to pay more to enjoy the breast-milk of a Princess.\n";
            traitBoost *= uniform(1, 2);
            milkValue += traitBoost;
        }
        else if (chance(30) && (girl.has_active_trait("Priestess")))
        {
            ss << "Customers pay more to drink the breast-milk of a religious holy-woman.\n";
            milkValue += traitBoost;
        }
        else if (chance(40) && (girl.fame() >= 95))
        {
            ss << "Your customers eagerly gulp down the breast-milk of such a famous and well-loved girl.\n";
            milkValue += traitBoost;
        }
        else if (girl.has_active_trait("Vampire"))
        {
            ss << "Customers pay more to try the breast-milk of a Vampire. Perhaps they hope for eternal life.\n";
            milkValue += uniform(5, 35);
        }

        if (chance(30) && (girl.has_active_trait("Shroud Addict") || girl.has_active_trait("Fairy Dust Addict") ||
                                girl.has_active_trait("Viras Blood Addict")))
        {
            ss << "Her breast-milk has a strangely bitter flavour. However, customers find it quite addictive and end up paying extra for more.\n";
            milkValue += uniform(10, 40);
        }

        if (chance(15) && (girl.has_active_trait("Strong Magic") || girl.has_active_trait("Powerful Magic")))
        {
            if ((girl.magic() > 75) && (girl.mana() > 50))
            {
                ss << "Her milk pulsates with magical healing energies. It can cure colds, heal injuries and 'improve performance.' Customers pay significantly more for it.\n";
                milkValue += traitBoost;
                girl.mana(-25); //Mana passes into milk
            }
            else
            {
                ss << "Her milk has mild magical healing properties and leaves customers feeling upbeat. Customers pay a little more for this.\n";
                milkValue += uniform(10, 40);
            }
        }
        if (girl.has_active_trait("Undead") || girl.has_active_trait("Zombie"))
        {
            ss << "Customers are very reluctant to drink the milk of the undead. You can barely give the stuff away.\n";
            milkValue /= 10;
        }
    }
    //update to add options based on how good you are...
    if (chance(3) && (girl.has_active_trait("Great Arse") || girl.has_active_trait("Tight Butt") ||
                           girl.has_active_trait("Phat Booty") ||
                           girl.has_active_trait("Deluxe Derriere")))
    {
        extraEvent = true;
        ssextra << "\nAs you survey the milking area from the doorway, you can't help noticing ${name}'s butt rising into the air from a milking stall";
        if (g_Game->player().disposition() < -30)  //more than a bit evil
        {
            ssextra << ". Looking closer you find her strapped to a milking bench, that butt pointed right at you while her breasts hang beneath, pumped by suction cups. "
                    << "It's too damn good an opportunity";
            if (roll % 2)
            {
                ssextra << ", you decide, as you squeeze your dick into her ass.\n";
                girl.anal(1);
                girl.bdsm(1);
                extraimage = IMGTYPE_ANAL;
            }
            else if (is_virgin(girl))
            {
                ssextra << " and you are about to enter her when you remember she is a virgin. Reluctantly, you switch and instead fuck her ass.\n";
                girl.anal(1);
                girl.bdsm(1);
                extraimage = IMGTYPE_ANAL;
            }
            else
            {
                ssextra << " so you clamp your hands around that booty and fuck her pussy hard. ";
                girl.normalsex(1);
                girl.bdsm(1);
                extraimage = IMGTYPE_SEX;

                if (!girl.calc_pregnancy(&g_Game->player(), 1.0))
                {
                    ssextra << "${name} has gotten pregnant. This should help with her milk production.";
                }
                ssextra << "\n";
            }
            if (HateLove <= -50)
            {
                ssextra << "She's pissed now, telling the other girls you raped her";
                if (girl.is_slave()) ssextra << ", almost like she's forgotten who owns her";
                ssextra << ".\n \n";
                girl.pchate(5);
                enjoy -= 5;
            }
            else if (HateLove <= 0)
            {
                ssextra << "She's upset you took advantage of her. She thought you were better than that.\n \n";
                girl.pchate(2);
                enjoy -= 2;
            }
            else if (HateLove <= 50)
            {
                ssextra << "She was surprised, but pleased you noticed her. She enjoyed it.\n \n";
                girl.pclove(1);
                enjoy += 2;
            }
            else
            {
                ssextra << "She loved it! It made milking much more enjoyable.\n \n";
                girl.pclove(4);
                enjoy += 4;
            }
        }
        else if (g_Game->player().disposition() < 40)   // not that good
        {
            ssextra << ". Looking closer, she really does have a great butt. You stop for a moment, but decide that doing anything more just wouldn't be right. "
                    << "You give her butt a gentle pat and walk away.\n \n";
            extraimage = IMGTYPE_COWGIRL;
        }
        else //you are good
        {
            ssextra << " and, right behind her, one of the delivery-boys from the market. He knows he shouldn't be in the production area. As you move closer, "
                    << "you are shocked to find him taunting and abusing ${name} as he roughly shoves a finger in and out of her asshole. You watch horrified as he "
                    << "starts to take down his pants, clearly readying to rape her. She wriggles and squeals but is so tightly strapped in she is powerless to stop him.\nFurious you "
                    << "grab a heavy, wooden milk-bucket and smash it across his head, knocking him out cold. Your gang drag the unconscious wretch to your dungeon as you write a "
                    << "note to the market trader, explaining this.\n";
            switch (roll % 3)
            {
                case 0:
                    ssextra << "The trader sends back 100 gold and a note apologizing, and promising this will never happen again. He says you can do what you like with the "
                            << "delivery boy, \"But if you ever release the fool, tell him he'll need a new job.\"\n";
                    g_Game->dungeon().AddCust(DUNGEON_CUSTBEATGIRL, 0, false);
                    wages += 100;
                    break;
                case 1:
                    ssextra << "The trader arrives soon after, begging for his son's release. Eventually you agree, adding that if his boy EVER enters your farm again "
                            << "his balls will be staying here. The trader thanks you repeatedly for your kindness and apologizes to ${name}, giving her 200 extra gold for her... discomfort.\n";
                    wages += 200;
                    break;
                case 2:
                    ssextra << "You never hear a word from the market trader.\n";
                    g_Game->dungeon().AddCust(DUNGEON_CUSTBEATGIRL, 0, false);
                    break;
            }
            if (is_virgin(girl)) ssextra << "Thanks to you, her virginity is intact so ";
            ssextra << "${name} comes to your office after her shift";
            if ((HateLove <= 50) || girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Cum Addict") ||
                girl.has_active_trait("Slut"))
            {
                ssextra << ", pulls down your pants, and 'thanks'";
                extraimage = IMGTYPE_ORAL;
            }
            else
            {
                ssextra << " and earnestly thanks";
                extraimage = IMGTYPE_PROFILE;
            }
            ssextra << " you for your intervention.\n";
        }
        //Add whatever is in here onto the original message too.
        //ss << ssextra.str();
        //Ditched the above - too cumbersome.
        ss << "Something happened here today (see extra message).\n";
    }
    wages += milkValue;

    //Output
    //
    ss << "In the end, ${name} produced ";
    if (milkValue <= 0)
    {
        ss << "no milk at all, earning nothing for her pains.";
    }
    else if ((int)milkProduced == 0)
    {
        ss << "a trickle of ";
        if (girl.has_active_trait("Cat Girl"))    ss << "Cat-Girl ";
        ss << "breast-milk, earning just " << wages << " gold.";
    }
    else
    {
        ss << "just over " << (int)milkProduced << " ounces. This fine, freshly-squeezed ";
        if (girl.has_active_trait("Cat Girl"))    ss << "Cat-Girl ";
        ss << "breast-milk earns " << wages << " gold.";
    }

    // `J` Farm Bookmark - adding in items that can be created in the farm


    girl.m_Pay = std::max(0, wages);

    girl.AddMessage(ss.str(), IMGTYPE_MILK, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    //generate extra message
    if (extraEvent) girl.AddMessage(ssextra.str(), extraimage, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, enjoy);

    return false;
}

double cFarmJobGetMilked::GetPerformance(const sGirl& girl, bool estimate) const {
    // TODO this does not fit in the current framework!
    double jobperformance = 0.0;
    jobperformance += girl.lactation();
    jobperformance += 5 * girl.breast_size();

    if (girl.has_active_trait("Dry Milk")) jobperformance /= 5;
    else if (girl.has_active_trait("Scarce Lactation")) jobperformance /= 2;
    else if (girl.has_active_trait("Abundant Lactation")) jobperformance *= 1.5;
    else if (girl.has_active_trait("Cow Tits")) jobperformance *= 2;

    if (girl.is_pregnant()) jobperformance *= 2;

    return jobperformance;
}

class cFarmJobCatacombRancher : public cFarmJob {
public:
    cFarmJobCatacombRancher();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
private:
};

cFarmJobCatacombRancher::cFarmJobCatacombRancher() : cFarmJob(
        JOB_CATACOMBRANCHER, "CatacombRancher.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobCatacombRancher::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    cGirls::EquipCombat(girl);    // This job can be dangerous so any protection is good.

    int enjoy = 0;

    if (performance >= 245)
    {
        ss << " She must be the perfect at this.";
        wages += 155;
    }
    else if (performance >= 185)
    {
        ss << " She's unbelievable at this.";
        wages += 95;
    }
    else if (performance >= 145)
    {
        ss << " She's good at this job.";
        wages += 55;
    }
    else if (performance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.";
        wages += 15;
    }
    else if (performance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.";
        wages -= 5;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
        wages -= 15;
    }
    ss << "\n \n";


    //enjoyed the work or not
    int roll = d100();
    if (roll <= 5)
    {
        ss << "Some of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll <= 25)
    {
        ss << "She had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += 1;
    }

    //    Finish the shift            //

    girl.AddMessage(ss.str(), IMGTYPE_HERD, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += uniform(10, 10 + roll_max);
    girl.m_Pay = std::max(0, wages);

    HandleGains(girl, enjoy);
    
    return false;
}

class cFarmJobResearch : public cFarmJob {
public:
    cFarmJobResearch();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

cFarmJobResearch::cFarmJobResearch() : cFarmJob(JOB_RESEARCH, {ACTION_WORKTRAINING, 20}) {
    load_from_xml("FarmResearch.xml");
}

bool cFarmJobResearch::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    cGirls::UnequipCombat(girl);    // put that shit away

    int enjoy = 0;
    int roll_a = d100(), roll_b = d100(), roll_c = d100();
    
#pragma region //    Job Performance            //

    int train = 0;                                                // main skill trained
    int tanm = girl.animalhandling();                            // Starting level - train = 1
    int tfar = girl.farming();                                    // Starting level - train = 2
    int tmag = girl.magic();                                    // Starting level - train = 3
    int ther = girl.herbalism();                                // Starting level - train = 4
    int tint = girl.intelligence();                            // Starting level - train = 5
    bool gaintrait = false;                                        // posibility of gaining a trait
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = brothel.m_Filthiness / 100;                // training time wasted with bad equipment
    int sgAnm = 0, sgFar = 0, sgMag = 0, sgHer = 0, sgInt = 0;    // gains per skill


    /* */if (roll_a <= 5)    skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else /*             */    skill = 3;
    /* */if (girl.has_active_trait("Quick Learner"))    { skill += 1; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Farm Lab is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 20  && tanm < 100)    train = 1;    // animalhandling
        else if (roll_b < 40  && tfar < 100)    train = 2;    // farming
        else if (roll_b < 60  && tmag < 100)    train = 3;    // magic
        else if (roll_b < 80  && ther < 100)    train = 4;    // herbalism
        else if (roll_b < 100 && tint < 100)    train = 5;    // intelligence
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || chance(5)) gaintrait = true;

    if (train == 1) { sgAnm = skill; ss << "She researches animals.\n"; }                else sgAnm = uniform(0, 1);
    if (train == 2) { sgFar = skill; ss << "She researches farming techniques.\n"; }    else sgFar = uniform(0, 1);
    if (train == 3) { sgMag = skill; ss << "She researches magical techniques.\n"; }    else sgMag = uniform(0, 1);
    if (train == 4) { sgHer = skill; ss << "She researches plants and their uses.\n"; }    else sgHer = uniform(0, 1);
    if (train == 5) { sgInt = skill; ss << "She researches general topics.\n"; }        else sgInt = uniform(0, 1);

    if (sgAnm + sgFar + sgMag + sgHer + sgInt > 0)
    {
        ss << "She managed to gain:\n";
        if (sgAnm > 0) { ss << sgAnm << " Animal Handling\n";    girl.animalhandling(sgAnm); }
        if (sgFar > 0) { ss << sgFar << " Farming\n";            girl.farming(sgFar); }
        if (sgMag > 0) { ss << sgMag << " Magic\n";                girl.magic(sgMag); }
        if (sgHer > 0) { ss << sgHer << " Herbalism\n";            girl.herbalism(sgHer); }
        if (sgInt > 0) { ss << sgInt << " Intelligence\n";        girl.intelligence(sgInt); }
    }

    int trycount = skill;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (uniform(0, 9))
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                if (girl.lose_trait( "Dependant"))
                {
                    ss << "She seems to be getting over her Dependancy with her training.";
                    gaintrait = false;
                }
                break;
            default:    break;    // no trait gained
        }
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //


    //enjoyed the work or not
    /* */if (roll_c <= 10)    { enjoy -= uniform(1, 3);    ss << "She did not enjoy her time training."; }
    else if (roll_c >= 90)    { enjoy += uniform(1, 3);    ss << "She had a pleasant time training."; }
    else /*             */    { enjoy += uniform(0, 1);        ss << "Otherwise, the shift passed uneventfully."; }

    ss << "\n \n";

#pragma endregion
#pragma region    //    Create Items                //



    // `J` Farm Bookmark - adding in items that can be created in the farm
    if (girl.intelligence() + girl.crafting() > 100 && chance(girl.intelligence() + girl.crafting() / 10))    // 10-20%
    {
        sInventoryItem* item = nullptr;
        std::string itemname;
        int tries = skill;
        while (itemname.empty() && tries > 0)
        {
            switch (uniform(0, 20))
            {
                /*    For each item available, the girl making it must have:
                Skills:
                *    If an item gives less than 40 skill points, the girl must have 40 more than the amount given in that skill
                *    If it gives more than 40 points, there will need to be some magic included.
                *    If there are multiple skills, add 40 to each bonus and if skills (a+b+c) > bonus total
                Stats:
                *    Stats will need more variation in how high they are needed because it is less knowledge than conditioning
                Traits:
                *    Most traits will require some magic
                Randomness:
                *    There also should be a chance() with the main Skills averaged and divided by 10
                *        If there are 3 skills, then chance((a+b+c)/30)
                */
                case 0:
                    if (girl.farming() > 50 && girl.animalhandling() > 50
                        && chance(girl.farming() + girl.animalhandling() / 20))
                        itemname = "Farmer's Guide";        // +10 Farming, +10 AnimalHandling
                    break;
                case 1:
                    if (girl.constitution() > 60 && chance(girl.constitution() / 10))
                        itemname = "Manual of Health";        // (+15 Beast/Group, +10 Cons/Str, +5 BDSM/Comb)
                    break;
                case 2:
                    if (girl.magic() > 60 && chance(girl.magic() / 10))
                        itemname = "Manual of Magic";        // +20 magic
                    break;
                case 3:
                    if (girl.magic() > 80 && chance(girl.magic() / 10))
                        itemname = "Codex of the Arcane";    // +40 magic
                    break;
                case 4:
                    if (girl.lesbian() > 40 && chance(girl.lesbian() / 10))
                        itemname = "Manual of Two Roses";    // +20 Lesbian
                    break;
                case 5:
                {
                    if (girl.lesbian() > 80 && chance(girl.lesbian() / 10))
                        itemname = "Codex of Sappho";        // +40 Lesbian
                }break;
                case 6:
                    if (girl.bdsm() > 60 && chance(girl.bdsm() / 10))
                        itemname = "Manual of Bondage";        // (+20 BDSM, +5 Cons)
                    break;
                case 7:
                    if (girl.combat() > 60 && chance(girl.combat() / 10))
                        itemname = "Manual of Arms";        // (+20 Com)
                    break;
                case 8:
                    if (girl.performance() + girl.strip() > 100 && chance((girl.performance() + girl.strip()) / 20))
                        itemname = "Manual of the Dancer";    // (+15 Serv/Strip/Perf, +5 Norm/Agi)
                    break;
                case 9:
                    if (girl.normalsex() + girl.oralsex() + girl.anal() > 150 && chance((girl.normalsex() + girl.oralsex() + girl.anal()) / 30))
                        itemname = "Manual of Sex";            // (+15 Norm, +10 Oral, +5 Anal)
                    break;
                case 10:
                {
                    if (girl.magic() < 80 && girl.mana() < 20) break;
                    int manacost = 60;
                    /* */if (girl.has_active_trait("Sterile")) manacost = 80;
                    else if (girl.has_active_trait("Fertile")) manacost = 40;
                    else if (girl.has_active_trait("Broodmother")) manacost = 20;
                    if (girl.mana() >= manacost && chance(girl.magic() - manacost))
                    {
                        girl.mana(-manacost);
                        itemname = "Fertility Tome";                // (-Sterile, +Fertile, +50 Normal Sex, +100 Libido)
                    }
                }break;
                case 11:
                {
                    // Noble, Princess and Queen needs 40 in all the
                    // three stats, everyone else needs 60 in one stat
                    // of the three to make this
                    if (girl.has_active_trait("Noble") || girl.has_active_trait("Princess") || girl.has_active_trait("Queen"))
                    {
                        if (girl.refinement() < 40 || girl.service() < 40 || girl.intelligence() < 40)
                            break;
                        else if (girl.refinement() < 60 && girl.service() < 60 && girl.intelligence() < 60)
                            break;
                    }
                    // she can make it, now does she?
                    if (chance((girl.refinement() + girl.service() + girl.intelligence()) / 30))
                        itemname = "Codex of the Courtesan";        // (+20 Serv/Strip/Refin, +10 Mor/Dig/Cha/Int/Lib/Conf/Oral)
                }break;
                case 12:
                {
                    int manacost = 70;
                    // Dominatrix, Masochist and Sadistic needs 50, everyone else needs 70 to make this
                    if (girl.has_active_trait("Dominatrix") || girl.has_active_trait("Masochist") || girl.has_active_trait("Sadistic"))
                    {
                        if (girl.bdsm() < 50 || girl.magic() < 50 || girl.mana() < 50)        break;
                        manacost = 50;
                    }
                    else if (girl.bdsm() < 70 && girl.magic() < 70 && girl.mana() < 70)        break;
                    // she can make it, now does she?
                    if (chance((girl.bdsm() + girl.magic()) / 20))
                    {
                        girl.mana(-manacost);
                        itemname = "Codex of Submission";        // (+30 Obed, -30 Spi/Dig, +20 BDSM, +10 Anal/Group/Oral/Hand/Foot)
                    }
                }break;
                case 13:
                {
                    if (girl.combat() > 80 && chance(girl.combat() / 10))
                        itemname = "Codex of Mars";            // (+40 Com, Adds Brawler)
                }break;
                case 14:
                {
                    if (girl.normalsex() + girl.oralsex() + girl.anal() > 170 && chance((girl.normalsex() + girl.oralsex() + girl.anal()) / 30))
                        itemname = "Codex of Eros";            // (+30 Norm, +10 Anal/Oral)
                }break;
                case 15:
                {
                    if (girl.medicine() + girl.intelligence() > 110 && chance((girl.medicine() + girl.intelligence()) / 20))
                        itemname = "Codex of Asclepius";    // (+20 Med, +10 Int)
                }break;
                case 16:
                {
                    /*
    
                    */
                }break;
                case 17:
                {
                    /*
    
                    */
                }break;
                case 18:
                {
                    /*
    
                    */
                }break;

                default:
                    break;
            }
        }

        item = g_Game->inventory_manager().GetItem(itemname);
        if (item)
        {
            g_Game->player().add_item(item);
            ss << "${name} managed to create a " << itemname << " by compiling her notes together.\n";
        }
    }



#pragma endregion
#pragma region    //    Money                    //

    if (girl.is_unpaid()) { wages = 0; }
    else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more


#pragma endregion
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(m_Data.Action, enjoy);

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Money
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait("Quick Learner"))        { xp += 2; }
    else if (girl.has_active_trait("Slow Learner"))    { xp -= 2; }

    girl.exp(uniform(1, xp) );
    girl.upd_temp_stat(STAT_LIBIDO, skill / 2);

#pragma endregion
    return false;
}

double cFarmJobResearch::GetPerformance(const sGirl& girl, bool estimate) const {
    // this has completely different handling for
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance +=
                (100 - girl.animalhandling()) +
                (100 - girl.farming()) +
                (100 - girl.magic()) +
                (100 - girl.herbalism()) +
                (100 - girl.intelligence());
    }
    else// for the actual check
    {
        jobperformance = (girl.intelligence() / 2 +
                          girl.herbalism() / 2 +
                          girl.brewing());
        if (!estimate)
        {
            int t = girl.tiredness() - 80;
            if (t > 0)
                jobperformance -= (t + 2) * (t / 3);
        }
    }

    jobperformance += girl.get_trait_modifier("work.research");

    return jobperformance;
}

class cFarmJobFarmHand : public cFarmJob {
public:
    cFarmJobFarmHand();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance);
};

cFarmJobFarmHand::cFarmJobFarmHand() : cFarmJob(
        JOB_FARMHAND, "FarmHand.xml", {ACTION_WORKFARM, 20}) {
}

bool cFarmJobFarmHand::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) {
    int roll_a = d100();
    int roll_b = d100();
    int roll_c = d100();
    cGirls::UnequipCombat(girl);
    Action_Types actiontype = ACTION_WORKFARM; Action_Types actiontype2 = ACTION_WORKCLEANING;
    double CleanAmt = girl.job_performance(JOB_FARMHAND, false);
    int enjoyC = 0, enjoyF = 0;
    int wages = 0;
    int tips = 0;
    bool playtime = false;

    if (roll_a <= 10)
    {
        enjoyC -= uniform(0, 2); enjoyF -= uniform(0, 2);
        CleanAmt = CleanAmt * 0.8;
        if (roll_b < 30)    ss << "She spilled a bucket of something unpleasant all over herself.";
        else if (roll_b < 60)    ss << "She stepped in something unpleasant.";
        else    ss << "She did not like working on the farm today.";
    }
    else if (roll_a >= 90)
    {
        enjoyC += uniform(0, 2); enjoyF += uniform(0, 2);
        CleanAmt = CleanAmt * 1.1;
        /* */if (roll_b < 50)    ss << "She cleaned the building while humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        enjoyC += uniform(0, 1); enjoyF += uniform(0, 1);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt = CleanAmt * 0.9;
        wages = 0;
    }
    else
    {
        wages = int(CleanAmt); // `J` Pay her based on how much she cleaned
    }

    // `J` if she can clean more than is needed, she has a little free time after her shift
    if (brothel.m_Filthiness < CleanAmt / 2) playtime = true;
    ss << "\n \nCleanliness rating improved by " << int(CleanAmt);
    if (playtime)    // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she ";
        if (!is_night && roll_c % 3 == 1)    // 33% chance she will watch the sunset when working day shift
        {
            ss << "sat beneath an oak tree and watched the sunset.";
            girl.tiredness(-uniform(2, 6));
        }
        else if (roll_c < 25)
        {
            ss << "played with the baby animals a bit.";
            girl.animalhandling(uniform(1, 3));
        }
        else if (roll_c < 50)
        {
            ss << "played in the dirt a bit.";
            girl.farming(uniform(0, 1));
        }
        else
        {
            ss << "sat in a rocking chair on the farm house front porch whittling.";
            girl.crafting(uniform(0, 2));
            girl.tiredness(-uniform(0, 2));
        }
        girl.happiness(uniform(1, 5));
    }

#if 0

    // `J` Farm Bookmark - adding in items that can be created in the farm


#endif

    // do all the output
    girl.AddMessage(ss.str(), IMGTYPE_MAID, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    brothel.m_Filthiness -= int(CleanAmt);

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve girl
    int xp = 5, libido = 1, skill = 3;

    if (enjoyC + enjoyF > 2)                            { xp += 1; skill += 1; }
    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.has_active_trait("Nymphomaniac"))            { libido += 2; }

    girl.exp(uniform(1, xp+1));
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    // primary (+2 for single or +1 for multiple)
    girl.service(uniform(0, skill));
    // secondary (-1 for one then -2 for others)
    girl.crafting(std::max(0, uniform(-1, skill-1)));
    girl.farming(std::max(0, uniform(-2, skill-2)));
    girl.strength(std::max(0, uniform(-2, skill-2)));

    girl.upd_Enjoyment(actiontype, enjoyF);
    girl.upd_Enjoyment(actiontype2, enjoyC);
    // Gain Traits
    if (chance(girl.service()))
        cGirls::PossiblyGainNewTrait(girl, "Maid", 90, actiontype2, "${name} has cleaned enough that she could work professionally as a Maid anywhere.", is_night);
    // Lose Traits
    if (chance(girl.service()))
        cGirls::PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype2, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.", is_night);

    return false;
}

void RegisterFarmJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cFarmJobFarmer>());
    mgr.register_job(std::make_unique<cFarmJobMarketer>());
    mgr.register_job(std::make_unique<cFarmJobVeterinarian>());
    mgr.register_job(std::make_unique<cFarmJobShepherd>());
    mgr.register_job(std::make_unique<cFarmJobRancher>());
    mgr.register_job(std::make_unique<cFarmJobBeastCapture>());
    mgr.register_job(std::make_unique<cFarmJobGetMilked>());
    mgr.register_job(std::make_unique<cFarmJobMilker>());
    mgr.register_job(std::make_unique<cFarmJobCatacombRancher>());
    mgr.register_job(std::make_unique<cFarmJobResearch>());
    mgr.register_job(std::make_unique<cFarmJobFarmHand>());
}
