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
#include <vector>
#include <sstream>
#include "CLog.h"
#include "buildings/cBuildingManager.h"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "cScreenSlaveMarket.h"
#include "cTariff.h"
#include "IGame.h"
#include "character/traits/ITraitSpec.h"
#include "character/traits/ITraitsCollection.h"
#include "character/predicates.h"
#include "character/cGirlPool.h"
#include "xml/util.h"

namespace {
    class cSlaveMarketIF : public IInteractionInterface {
    public:
        bool LookupBoolean(const std::string& name) const final {
            throw std::logic_error("N/A");
        }

        int LookupNumber(const std::string& name) const final {
            if(name == "Disposition") {
                return g_Game->player().disposition();
            } else {
                throw std::logic_error("N/A");
            }
        }

        void TriggerEvent(const std::string& name) const final {
            throw std::logic_error("N/A");
        }
        void SetVariable(const std::string& name, int value) const final {
            throw std::logic_error("N/A");
        }
        void SetVariable(const std::string& name, std::string value) const final {
            throw std::logic_error("N/A");
        }
    };

}

cScreenSlaveMarket::cScreenSlaveMarket() : cGameWindow("slavemarket_screen.xml")
{
    m_SelectedGirl = -1;
    ImageNum       = -1;
    DetailLevel    = 0;
    sel_pos        = 0;

    m_TextRepo = ITextRepository::create();
    DirPath path = DirPath() << "Resources" << "Data" << "SlaveMarket.xml";
    auto doc = LoadXMLDocument(path.c_str());

    m_TextRepo->load(*doc->RootElement());
}

void cScreenSlaveMarket::set_ids()
{
    more_id             = get_id("ShowMoreButton");
    buy_slave_id        = get_id("BuySlaveButton");
    cur_brothel_id      = get_id("CurrentBrothel","*Unused*");
    slave_list_id       = get_id("SlaveList");
    trait_list_id       = get_id("TraitList","*Unused*");
    trait_list_text_id  = get_id("TraitListT");
    details_id          = get_id("SlaveDetails");
    trait_id            = get_id("TraitDesc","*Unused*");
    girl_desc_id        = get_id("GirlDesc");
    image_id            = get_id("GirlImage");
    header_id           = get_id("ScreenHeader","*Unused*");
    gold_id             = get_id("Gold", "*Unused*");
    slave_market_id     = get_id("SlaveMarket");
    releaseto_id        = get_id("ReleaseTo");
    roomsfree_id        = get_id("RoomsFree");

    /// TODO cannot handle dungeon like the others at the moment
    dungeon_id          = get_id("Dungeon");

    // set up structure with all "release girl to ... " buttons
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel0"), BuildingType::BROTHEL, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel1"), BuildingType::BROTHEL, 1});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel2"), BuildingType::BROTHEL, 2});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel3"), BuildingType::BROTHEL, 3});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel4"), BuildingType::BROTHEL, 4});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel5"), BuildingType::BROTHEL, 5});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Brothel6"), BuildingType::BROTHEL, 6});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("House"), BuildingType::HOUSE, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Clinic"), BuildingType::CLINIC, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Studio"), BuildingType::STUDIO, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Arena"), BuildingType::ARENA, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Centre"), BuildingType::CENTRE, 0});
    m_ReleaseButtons.emplace_back(RelBtnData{get_id("Farm"), BuildingType::FARM, 0});

    // set button callbacks
    SetButtonCallback(buy_slave_id, [this](){
        buy_slaves();
        init(true);
    });

    SetButtonHotKey(buy_slave_id, SDLK_SPACE);

    for(const auto& btn: m_ReleaseButtons) {
        SetButtonCallback(btn.id, [this, btn](){
            m_TargetBuilding = g_Game->buildings().building_with_type(btn.type, btn.index);
            update_release_text();});
    }

    SetButtonCallback(dungeon_id, [this]() {
        m_TargetBuilding = nullptr;
        EditTextItem("Send Girl to: The Dungeon", releaseto_id);
        EditTextItem("", roomsfree_id);
    });

    SetButtonCallback(more_id, [this]() {
        sGirl *girl = g_Game->GetSlaveMarket().get_girl(m_SelectedGirl);
        if(!girl) {
            EditTextItem("", details_id, true);
            return;
        }
        if (DetailLevel == 0)        { DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(*girl, true), details_id, true); }
        else if (DetailLevel == 1)    { DetailLevel = 2; EditTextItem(cGirls::GetThirdDetailsString(*girl), details_id, true); }
        else                        { DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(*girl, true), details_id, true); }
    });

    SetListBoxSelectionCallback(slave_list_id, [this](int sel) { change_selected_girl(sel); });
    SetListBoxSelectionCallback(trait_list_id, [this](int sel) { on_select_trait(sel); });
    SetListBoxHotKeys(slave_list_id, SDLK_a, SDLK_d);

    AddKeyCallback(SDLK_s, [this](){
        sGirl *girl = g_Game->GetSlaveMarket().get_girl(m_SelectedGirl);
        if(!girl) {
            EditTextItem("", details_id, true);
            return true;
        }

        if (is_ctrl_held())
        {
            DetailLevel = 2;
            EditTextItem(cGirls::GetThirdDetailsString(*girl), details_id);
        }
        else
        {
            if (DetailLevel == 0)        { DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(*girl, true), details_id); }
            else                        { DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(*girl, true), details_id); }
        }
        return true;
    });
}

void cScreenSlaveMarket::init(bool back)
{
    Focused();
    std::stringstream ss;

    //buttons enable/disable
    DisableWidget(more_id, true);
    DisableWidget(buy_slave_id, true);
    m_SelectedGirl = -1;

    ImageNum = -1;
    if (cur_brothel_id >= 0)    EditTextItem(active_building().name(), cur_brothel_id);

    if(!back) {
        m_TargetBuilding = &active_building();
    }
    update_release_text();

    for(const auto& btn: m_ReleaseButtons) {
        HideWidget(btn.id, g_Game->buildings().num_buildings(btn.type) < btn.index + 1);
    }

    ClearListBox(slave_list_id);    // clear the list

    for(std::size_t i = 0; i < g_Game->GetSlaveMarket().num(); ++i) {
        int col = COLOR_BLUE;
        auto girl = g_Game->GetSlaveMarket().get_girl(i);
        if (girl->IsUnique())
        {
            col = COLOR_YELLOW;
        }
        AddToListBox(slave_list_id, i, girl->FullName(), col);
    }

    m_SelectedGirl = 0;

    if (header_id >= 0)
    {
        ss.str(""); ss << "Slave Market, " << g_Game->gold().sval() << " gold";
        EditTextItem(ss.str(), header_id);
    }
    if (gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().sval();
        EditTextItem(ss.str(), gold_id);
    }

    // Finds the first girl in the selection, so she is highlighted. This stops the No girl selected that was normal before. --PP
    m_SelectedGirl = g_Game->GetSlaveMarket().num() - 1;

    // if there is still as selection (a non empty slave list) then highlight the current selection
    if (m_SelectedGirl >= 0) SetSelectedItemInList(slave_list_id, m_SelectedGirl, true);
    // now we need to populate the trait box
    if (trait_list_id >= 0) ClearListBox(trait_list_id);
    if (trait_list_text_id >= 0) EditTextItem("Traits:", trait_list_text_id);
    if (girl_desc_id >= 0)    EditTextItem("", girl_desc_id);
    int tmp = GetLastSelectedItemFromList(slave_list_id);
    // if the last item was -1 I assume the list is empty so we can go home early (and probably should have earlier still)
    if (tmp == -1) return;
    // get the girl under the cursor.
    preparescreenitems(g_Game->GetSlaveMarket().get_girl(tmp));
}

void cScreenSlaveMarket::on_select_trait(int selection)
{
    if (selection != -1 && m_SelectedGirl != -1)
    {
        auto traits = g_Game->GetSlaveMarket().get_girl(m_SelectedGirl)->get_trait_info();
        EditTextItem(traits.at(selection).trait->desc(), trait_id);
    }
    else EditTextItem("", trait_id);
}

void cScreenSlaveMarket::process()
{
    /*
    HideWidget(image_id, (m_SelectedGirl < 0));        // hide/show image based on whether a girl is selected
    if (m_SelectedGirl < 0)                                // if no girl is selected, clear girl info
    {
        EditTextItem("No girl selected", details_id);
        if (trait_id >= 0) EditTextItem("", trait_id);
    }
    // nothing selected == nothing further to do
    */
}

bool cScreenSlaveMarket::buy_slaves()
{
    std::stringstream ss;
    std::stringstream sendtotext;

    // set the brothel
    if (m_TargetBuilding == nullptr) sendtotext << "the Dungeon";
    else
    {
        switch(m_TargetBuilding->type()) {
        case BuildingType::BROTHEL:
            sendtotext << "your brothel: " << m_TargetBuilding->name();
            break;
        case BuildingType::ARENA:
            sendtotext << "the Arena";
            break;
        case BuildingType::STUDIO:
            sendtotext << "the Studio";
            break;
        case BuildingType::CENTRE:
            sendtotext << "the Community Centre";
            break;
        case BuildingType::FARM:
            sendtotext << "the Farm";
            break;
        case BuildingType::CLINIC:
            sendtotext << "the Clinic";
            break;
        case BuildingType::HOUSE:
            sendtotext << "your House";
            break;
        }
    }

    // set the girls
    int numgirls;

    int totalcost = 0;
    std::vector<sGirl*> girls_bought;
    ForAllSelectedItems(slave_list_id, [&](int sel) {
        auto girl = g_Game->GetSlaveMarket().get_girl(sel);
        girls_bought.push_back(girl);
        totalcost += g_Game->tariff().slave_buy_price(*girl);
    });
    numgirls = girls_bought.size();

    // Check if there is enough room where we want to send her
    if (m_TargetBuilding)    // Dungeon has no limit so don't bother checking if sending them there.
    {
        if (m_TargetBuilding->free_rooms() < 0)
        {
            push_message("The current building has no more room.\nChoose another building to send them to.", 0);
            return false;
        }
        if (m_TargetBuilding->free_rooms() < numgirls)
        {
            push_message("The current building does not have enough room for all the girls you want to send there.\nChoose another building or select fewer girls at a time to buy.", 0);
            return false;
        }
    }

    // `J` check if we can afford all the girls selected
    if (!g_Game->gold().slave_cost(totalcost))    // this pays for them if you can afford them
    {                                    // otherwise this part runs and returns a fail message.
        std::stringstream text;
        if (numgirls > 4 && g_Game->gold().ival() < totalcost / 2) text << "Calm down!  ";
        text << "You don't have enough money to purchase ";
        switch (numgirls)
        {
        case 0: text << "... noone ... Wait? What? Something went wrong.\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org"; break;
        case 1: text << girls_bought.front()->FullName(); break;
        case 2: text << "these two"; break;
        case 3: text << "these three"; break;
        default: text << numgirls; break;
        }
        text << (numgirls <= 1 ? "" : " girls") << ".";
        push_message(text.str(), 0);
        return false;
    }

    // `J` we could afford the girls so lets get to it
    ss << "You buy ";

    if (numgirls == 1)       ss << "a girl,   " << girls_bought.front()->FullName() << "   and send her to " << sendtotext.str();
    else if (numgirls == 2)  ss << "two girls,   " << girls_bought.front()->FullName() << "   and   " << girls_bought[1]->FullName() << ". You send them to " << sendtotext.str();
    else                     ss << numgirls << " girls and send them to " << sendtotext.str();
    ss << ".\n\n";

    // `J` zzzzzz - add in flavor texts here
    if (numgirls < 1) ss << "(error, no girls)";

    else if (!m_TargetBuilding) {
#pragma region //    Send them to the Dungeon        //
        if (g_Game->player().disposition() >= 80)                //Benevolent
        {
            ss << "\"Don't worry " << (numgirls == 1 ? "my dear" : "ladies")
               << ", I'm only sending you there until I find room for you somewhere better.\"\n";
        } else if (g_Game->player().disposition() >= 50)            // nice
        {
            ss << "\"Don't worry " << (numgirls == 1 ? "my dear" : "ladies")
               << ", you will not be in there long, I promise.\"\n";
        } else if (g_Game->player().disposition() >= 10)            //Pleasant
        {
            ss << "\"Try to make " << (numgirls == 1 ? "yourself" : "yourselves")
               << " comfortable, you should not be in there too long.\"\n";
        } else if (g_Game->player().disposition() >= -10)            // neutral
        {
            ss << "\"To the Dungeon with " << (numgirls == 1 ? "you" : "them") << ", I'll see you there shortly.\"\n";
        } else if (g_Game->player().disposition() >= -50)            // not nice
        {
            ss << "\"To the Dungeon with " << (numgirls == 1 ? "you" : "them")
               << ". I'll enjoy this, but you may not.\"\n";

        } else if (g_Game->player().disposition() >= -80)            //Mean
        {
            ss << "\"To the Dungeon with " << (numgirls == 1 ? "you" : "them") << ". Put "
               << (numgirls == 1 ? "her" : "them") << " in chains and I'll get to them when I am done here.\"\n";
        } else/*                                        */    //Evil
        {
            ss
                    << "\"You are off to your new home, the Dungeon, where my dreams happen and your dreams become nightmares.\"\n";

        }
#pragma endregion
#pragma region //    Send them to a Brothel            //
    }
    else if (m_TargetBuilding->type() == BuildingType::BROTHEL)
    {
        if(numgirls == 1) {
            ss << m_TextRepo->get_text("brothel-one", cSlaveMarketIF{});
        } else {
            ss << m_TextRepo->get_text("brothel-many", cSlaveMarketIF{});
        }
        ss << "\n";
    }
#pragma endregion
#pragma region //    Send them to Your House        //
    else if  (m_TargetBuilding->type() == BuildingType::HOUSE)
    {
        if (g_Game->player().disposition() >= 80)                //Benevolent
        {
            ss << "";
        }
        else if (g_Game->player().disposition() >= 50)            // nice
        {
            ss << "";
        }
        else if (g_Game->player().disposition() >= 10)            //Pleasant
        {
            ss << "";
        }
        else if (g_Game->player().disposition() >= -10)            // neutral
        {
            ss << "";
        }
        else if (g_Game->player().disposition() >= -50)            // not nice
        {
            ss << "";
        }
        else if (g_Game->player().disposition() >= -80)            //Mean
        {
            ss << "";
        }
        else                                            //Evil
        {
            ss << "";
        }
    }
#pragma endregion
#pragma region //    Send them to the Clinic        //
    else if  (m_TargetBuilding->type() == BuildingType::CLINIC)
    {
        ss << (numgirls == 1 ? "She is" : "They are") << " brought to the Clinic where they are given a full checkup.\n";
        for (auto& girl : girls_bought)
        {
            girl->health(100);
            girl->happiness(100);
            girl->tiredness(-100);
            girl->service(std::max(0, g_Dice.bell(-1, 3)));
            girl->medicine(std::max(0, g_Dice.bell(-2, 3)));
            girl->morality(std::max(0, g_Dice.bell(-2, 2)));
            if (has_disease(*girl))
            {
                std::vector<std::string> diseases;
                if (girl->has_active_trait("AIDS"))        diseases.emplace_back("AIDS");
                if (girl->has_active_trait("Chlamydia"))    diseases.emplace_back("Chlamydia");
                if (girl->has_active_trait("Herpes"))        diseases.emplace_back("Herpes");
                if (girl->has_active_trait("Syphilis"))    diseases.emplace_back("Syphilis");
                ss << girl->FullName() << " has been diagnosed with ";
                if (diseases.empty())    ss << "an unknown disease.";
                if (!diseases.empty())    ss << diseases[0];
                if (diseases.size() >= 2)    ss << " and " << diseases[1];
                if (diseases.size() >= 3)    ss << " and " << diseases[2];
                if (diseases.size() >= 4)    ss << " and " << diseases[3];
                ss << ".\n";
            }
        }
    }
#pragma endregion
#pragma region //    Send them to the Studio        //
    else if  (m_TargetBuilding->type() == BuildingType::STUDIO)
    {
        ss << "\"Ok " << (numgirls == 1 ? "my dear" : "ladies") << ", here are a couple of scripts to practice with on your way to the Studio.\"\n";
        if (numgirls == 1)        // single girl flavor texts
        {
            for (auto& girl : girls_bought)
            {
                /* */if (girl->has_active_trait("Porn Star"))
                {
                    ss << girl->FullName() << ": \"Script? I don't need a script, just tell me who to fuck and where the camera is and I'll get it done.\"\n";
                }
                else if (girl->has_active_trait("Actress"))
                {
                    ss << girl->FullName() << ": \"This is a rather short script, I'm used to long dialogues. I don't seem to get a lot of lines here, how much camera time do I get?\"\nGuard: \"Don't worry sweetheart, your mouth will get a lot to do.\"\n";
                }
                else if (girl->has_active_trait("Shy"))
                {
                    ss << girl->FullName() << ": \"I can't act, I can barely talk to other people. I get tongue tied whenever I'm around people.\"\nGuard: \"Tongued? Tied? That shouldn't be a problem sweetheart, they can work with anything where you are going.\"\n";
                    girl->happiness(-10);
                }
                else
                {
                    ss << girl->FullName() << " spends her time going over the scripts, disgusted at times, turned on at others.";
                    girl->performance(g_Dice % 4);
                    girl->libido(g_Dice % 10);
                }
            }
        }
        else                    // multiple girl flavor texts
        {
            std::string pornstarname;    int pornstarnum = 0;
            std::string actressname;    int actressnum = 0;
            std::string shygirlname;    int shygirlnum = 0;

            for (auto& girl : girls_bought)
            {
                /* */if (girl->has_active_trait("Porn Star"))    { pornstarnum++; pornstarname = girl->FullName(); }
                else if (girl->has_active_trait("Actress"))    { actressnum++; actressname = girl->FullName(); }
                else if (girl->has_active_trait("Shy"))        { shygirlnum++; shygirlname = girl->FullName(); }
                girl->lesbian(g_Dice % numgirls);
                girl->performance(g_Dice % numgirls);
            }
            if (actressnum > 0 && pornstarnum > 0)
            {
                ss << actressname << ": \"I've been in a few films already, this will be fun.\"\n" << pornstarname << ": \"Its not that kind of film honey.\"\n";
            }
            ss << "The girls practice their scripts, playing different roles and with eachother.\n";
        }
    }
#pragma endregion
#pragma region //    Send them to the Arena            //
    else if (m_TargetBuilding->type() == BuildingType::ARENA)
    {
        ss << "Guard: \"Ok " << (numgirls == 1 ? "sweetheart" : "ladies") << ", we are headed for the Arena where you will train and eventually fight for your new master, " << g_Game->player().FullName() << ". Your first lesson, to carry these heavy packages to the Arena. Load up and march.\"\n";
        for (auto& girl : girls_bought)
        {
            girl->strength(g_Dice % 6);
            girl->constitution(g_Dice % 3);
            girl->tiredness(g_Dice % 30);
        }
    }
#pragma endregion
#pragma region //    Send them to the Centre        //
    else if (m_TargetBuilding->type() == BuildingType::CENTRE)
    {
        ss << "When " << (numgirls == 1 ? "she arrives" : "the girls arrive") << " at the Centre " << (numgirls == 1 ? "she is" : "they are") << " shown around and then assigned to " << (numgirls == 1 ? "clean the dishes.\n" : "various tasks around the Centre.\n");
        for (auto& girl : girls_bought)
        {
            girl->service(g_Dice % 5);
            girl->morality(g_Dice % 5);
            girl->cooking(g_Dice % 3);
        }
    }
#pragma endregion
#pragma region //    Send them to the Farm            //
    else if (m_TargetBuilding->type() == BuildingType::FARM)
    {
        ss << (numgirls == 1 ? "She is" : "The girls are") << " brought to your stall at the town's farmers market until they are ready to go to the Farm. While there, " << (numgirls == 1 ? "she is" : "they are") << " shown the various animals and goods that they will be producing on the farm.\n";
        for (auto& girl : girls_bought)
        {
            girl->farming(g_Dice % 5 + 1);
            girl->animalhandling(g_Dice % 5 + 1);
            girl->herbalism(g_Dice % 3);
        }
    }
#pragma endregion
    // `J` end flavor texts

    // `J` send them where they need to go
    for (auto& girl : girls_bought)
    {
        std::stringstream sss;
        sss << "Purchased from the Slave Market for " << g_Game->tariff().slave_buy_price(*girl) << " gold.";
        girl->AddMessage(sss.str(), EImageBaseType::PROFILE, EVENT_GOODNEWS);
        girl->set_default_house_percent();

        if(m_TargetBuilding) {
            m_TargetBuilding->add_girl(g_Game->GetSlaveMarket().TakeGirl(girl));
            affect_girl_by_disposition(*girl);
        } else    // if something fails this will catch it and send them to the dungeon
        {
            g_Game->dungeon().AddGirl(g_Game->GetSlaveMarket().TakeGirl(girl), DUNGEON_NEWSLAVE);
            affect_dungeon_girl_by_disposition(*girl);
        }
    }

    // `J` now tell the player what happened
    if (numgirls <= 0)    push_message("Error, no girls names in the list", 0);
    else push_message(ss.str(), 0);

    // finish it
    m_SelectedGirl = -1;
    HideWidget(image_id, true);        // hide/show image based on whether a girl is selected
    if (m_SelectedGirl < 0)                                // if no girl is selected, clear girl info
    {
        EditTextItem("No girl selected", details_id);
        if (trait_id >= 0) EditTextItem("", trait_id);
    }

    return true;
}

void cScreenSlaveMarket::affect_girl_by_disposition(sGirl& girl) const
{
    if (g_Game->player().disposition() >= 80)                // Benevolent
    {
        girl.health(g_Dice % 10);
        girl.happiness(g_Dice % 20);
        girl.tiredness(-(g_Dice % 10));
        girl.pclove(g_Dice.bell(-2, 10));
        girl.pcfear(g_Dice.bell(-5, 1));
        girl.obedience(g_Dice.bell(-1, 5));
        girl.confidence(g_Dice.bell(-1, 5));
        girl.spirit(g_Dice.bell(-2, 10));
        girl.dignity(g_Dice.bell(-2, 5));
        girl.morality(g_Dice.bell(-2, 5));
        girl.refinement(g_Dice.bell(-2, 5));
        girl.sanity(g_Dice.bell(-1, 5));
        girl.fame(std::max(0, g_Dice.bell(-1, 1)));
    }
    else if (g_Game->player().disposition() >= 50)            // Nice
    {
        girl.health(g_Dice % 5);
        girl.happiness(g_Dice % 10);
        girl.tiredness(-(g_Dice % 5));
        girl.pclove(g_Dice.bell(-2, 6));
        girl.pcfear(g_Dice.bell(-3, 1));
        girl.obedience(g_Dice.bell(-1, 3));
        girl.confidence(g_Dice.bell(-1, 3));
        girl.spirit(g_Dice.bell(-1, 5));
        girl.dignity(g_Dice.bell(-1, 3));
        girl.morality(g_Dice.bell(-1, 3));
        girl.refinement(g_Dice.bell(-1, 3));
        girl.sanity(g_Dice.bell(-1, 3));
    }
    else if (g_Game->player().disposition() >= 10)            // Pleasant
    {
        girl.happiness(g_Dice % 5);
        girl.pclove(g_Dice.bell(-2, 2));
        girl.pcfear(g_Dice.bell(-1, 1));
        girl.obedience(g_Dice.bell(-1, 1));
        girl.confidence(g_Dice.bell(-1, 1));
        girl.spirit(g_Dice.bell(-1, 1));
        girl.dignity(g_Dice.bell(-1, 1));
        girl.morality(g_Dice.bell(-1, 1));
        girl.refinement(g_Dice.bell(-1, 1));
        girl.sanity(g_Dice.bell(-1, 1));
    }
    else if (g_Game->player().disposition() >= -10)            // Neutral
    {
    }
    else if (g_Game->player().disposition() >= -50)            // Not nice
    {
        girl.health(-(g_Dice % 2));
        girl.happiness(-(g_Dice % 10));
        girl.tiredness(-(g_Dice % 3));
        girl.pclove(-(g_Dice % 10));
        girl.pcfear(g_Dice % 5);
        girl.obedience(g_Dice.bell(-1, 2));
        girl.confidence(-(g_Dice % 3));
        girl.spirit(-(g_Dice % 5));
        girl.dignity(-(g_Dice % 3));
        girl.morality(-(g_Dice % 2));
        girl.refinement(-(g_Dice % 2));
        girl.sanity(g_Dice.bell(-2, 2));
        girl.bdsm(std::max(0, g_Dice.bell(-2, 5)));
    }
    else if (g_Game->player().disposition() >= -80)            //Mean
    {
        girl.health(-(g_Dice % 3));
        girl.happiness(-(g_Dice % 20));
        girl.tiredness(-(g_Dice % 5));
        girl.pclove(-(g_Dice % 20));
        girl.pcfear(g_Dice % 10);
        girl.obedience(g_Dice.bell(-1, 4));
        girl.confidence(-(g_Dice % 5));
        girl.spirit(-(g_Dice % 10));
        girl.dignity(-(g_Dice % 10));
        girl.morality(-(g_Dice % 2));
        girl.refinement(-(g_Dice % 3));
        girl.sanity(g_Dice.bell(-3, 1));
        girl.bdsm(3 + g_Dice % 10);
    }
    else                                            // Evil
    {
        girl.health(-(g_Dice % 5));
        girl.happiness(-(g_Dice % 40));
        girl.tiredness(-(g_Dice % 10));
        girl.pclove(-(g_Dice % 40));
        girl.pcfear(g_Dice % 20);
        girl.obedience(g_Dice.bell(-2, 5));
        girl.confidence(-(g_Dice % 10));
        girl.spirit(-(g_Dice % 20));
        girl.dignity(-(g_Dice % 20));
        girl.morality(-(g_Dice % 3));
        girl.refinement(-(g_Dice % 5));
        girl.sanity(g_Dice.bell(-5, 1));
        girl.bdsm(5 + g_Dice % 12);
    }
}

bool cScreenSlaveMarket::change_selected_girl(int selected)
{
    ImageNum       = -1;
    /*
     *    Since this is a multiselect box, GetLastSelectedItemFromList
     *    returns the last clicked list item, even if it's deselected.
     *    So, we'll check for that and show first truly selected item
     *    if the last clicked one is actually deselected.
     */
    m_SelectedGirl = selected;
    HideWidget(image_id, (m_SelectedGirl < 0));        // hide/show image based on whether a girl is selected
    if (m_SelectedGirl < 0)
    {
        // if no girl is selected, clear girl info
        EditTextItem("No girl selected", details_id);
        if (trait_id >= 0) EditTextItem("", trait_id);

    }
    bool MatchSel = false;
    int first_candidate = -1;

    ForAllSelectedItems(slave_list_id, [&](int sel) {
        MatchSel |= sel == m_SelectedGirl;
        if(first_candidate == -1)  first_candidate = sel;
    });

    if (!MatchSel) m_SelectedGirl = first_candidate;
    // if the player selected an empty slot make that into "nothing selected" and return
    //if (MarketSlaveGirls[selection] == nullptr) selection = -1;
    // disable/enable buttons based on whether a girl is selected
    DisableWidget(more_id, (m_SelectedGirl == -1));
    DisableWidget(buy_slave_id, (m_SelectedGirl == -1));
    if (trait_list_id >= 0) ClearListBox(trait_list_id);
    if (trait_list_text_id >= 0) EditTextItem("Traits:", trait_list_text_id);
    if (girl_desc_id >= 0)    EditTextItem("", girl_desc_id);
    // selection of -1 means nothing selected, so we get to go home early
    if (m_SelectedGirl == -1) return true;
    /*
     *    otherwise, we have (potentially) a new girl:
     *    set the global girl pointer
     *
     *    if we can't find the pointer. log an error and go home
     */
    sGirl *girl = g_Game->GetSlaveMarket().get_girl(m_SelectedGirl);
    if (!girl)
    {
        g_LogFile.log(ELogLevel::ERROR, "cScreenSlaveMarket::change_selected_girl: can't find girl data for selection (",
                m_SelectedGirl, ')');
        return true;
    }
    std::string detail;

    if (DetailLevel == 0)        detail = cGirls::GetDetailsString(*girl, true);
    else if (DetailLevel == 1)    detail = cGirls::GetMoreDetailsString(*girl, true);
    else                        detail = cGirls::GetThirdDetailsString(*girl);
    EditTextItem(detail, details_id, true);
    ImageNum = -1;                                        // I don't understand where this is used...

    preparescreenitems(girl);
    PrepareImage(image_id, *girl, EImageBaseType::PRESENTED);
    HideWidget(image_id, false);

    return true;
}

std::string cScreenSlaveMarket::get_buy_slave_string(sGirl* girl)
{
    std::string text = girl->FullName();

    if (active_building().free_rooms() <= 0)
    {
        text += " has been sent to your dungeon, since your current brothel is full.";
    }
    else if (cGirls::GetRebelValue(*girl) >= 35)
    {
        if (g_Game->player().disposition() >= 80)                //Benevolent
        {
            text += " accepting her own flaws that to needed be corrected, she goes to the dungeon where she will be waiting for your guidance.";
        }
        else if (g_Game->player().disposition() >= 50)            // nice
        {
            text += " in your opinion needs to work on her attitude, she has been guided to the dungeon.";
        }
        else if (g_Game->player().disposition() >= 10)            //Pleasant
        {
            text += " as your newest investment, she was sent to the dungeon to work on her rebellious nature.";
        }
        else if (g_Game->player().disposition() >= -10)            // neutral
        {
            text += " has been sent to your dungeon, as she is rebellious and poorly trained.";
        }
        else if (g_Game->player().disposition() >= -50)            // not nice
        {
            text += " as your newest investment that needs your special touch, she was sent to the dungeon.";
        }
        else if (g_Game->player().disposition() >= -80)            //Mean
        {
            text += " still had some spirit in her eyes left that you decided to stub out. She was dragged to a dungeon cell.";
        }
        else                                            //Evil
        {
            text += " put up a fight. She was beaten and dragged to your dungeon where you can have some private fun time with her.";
        }
    }
    return text;
}

void cScreenSlaveMarket::preparescreenitems(sGirl* girl)
{

    int trait_count = 0;
    std::stringstream traits_text;
    traits_text << "Traits:      ";

    // loop through her traits, populating the box
    auto traits = girl->get_trait_info();
    for (int i = 0; i < traits.size(); i++)
    {
        if (!traits[i].active) continue;
        trait_count++;
        if (trait_list_id >= 0) AddToListBox(trait_list_id, i, traits[i].trait->display_name());
        if (trait_list_text_id)
        {
            if (trait_count > 1) traits_text << ",   ";
            traits_text << traits[i].trait->display_name();
        }
    }
    // and finally, highlight the selected entry?
    if (trait_list_id >= 0) SetSelectedItemInList(trait_list_id, 0);
    if (trait_list_text_id >= 0) EditTextItem(traits_text.str(), trait_list_text_id);
    if (girl_desc_id >= 0)    EditTextItem(girl->m_Desc, girl_desc_id);
}

void cScreenSlaveMarket::affect_dungeon_girl_by_disposition(sGirl& girl) const
{
    if (g_Game->player().disposition() >= 80)                //Benevolent
    {
            // you have a reputation for not torturing so they are not afraid (but you are sending them to a dungeon so...)
            girl.m_AccLevel = 0;
            girl.pcfear(g_Dice.bell(-3, 1));
            girl.pclove(g_Dice.bell(-2, 6));
            girl.happiness(g_Dice.bell(-5, 10));
            girl.spirit(g_Dice.bell(-2, 4));
            girl.obedience(g_Dice.bell(-2, 5));

    } else if (g_Game->player().disposition() >= 50)            // nice
    {
        // you have a reputation for not torturing much so they are less afraid (but you are sending them to a dungeon so...)
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice.bell(-2, 2));
        girl.pclove(g_Dice.bell(-4, 4));
        girl.happiness(g_Dice.bell(-10, 5));
        girl.spirit(g_Dice.bell(-2, 2));
        girl.obedience(g_Dice.bell(-2, 2));

    } else if (g_Game->player().disposition() >= 10)            //Pleasant
    {
        bool mas = girl.has_active_trait("Masochist");
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice % 5 + (mas ? 0 : 3));
        girl.pclove(-(g_Dice % 9) + (mas ? 2 : -2));
        girl.happiness(-(g_Dice % 6 - (mas ? 2 : 0)));
        girl.spirit(-(g_Dice % 4 - (mas ? 2 : 0)));
        girl.obedience(g_Dice % 3 - 1);
    } else if (g_Game->player().disposition() >= -10)            // neutral
    {
        bool mas = girl.has_active_trait("Masochist");
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice % 5 + (mas ? 0 : 3));
        girl.pclove(-(g_Dice % 9) + (mas ? 2 : -2));
        girl.happiness(-(g_Dice % 6 - (mas ? 2 : 0)));
        girl.spirit(-(g_Dice % 4 - (mas ? 2 : 0)));
        girl.obedience(g_Dice % 3 - 1);
    } else if (g_Game->player().disposition() >= -50)            // not nice
    {
        bool mas = girl.has_active_trait("Masochist");
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice % 10 + (mas ? 0 : 5));
        girl.pclove(-(g_Dice % 17 - (mas ? 3 : -3)));
        girl.happiness(-(g_Dice % 12 - (mas ? 3 : 0)));
        girl.spirit(-(g_Dice % 7 - (mas ? 2 : 0)));
        girl.obedience(g_Dice % 5 - 2);

    } else if (g_Game->player().disposition() >= -80)            //Mean
    {

        bool mas = girl.has_active_trait("Masochist");
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice % 20 + (mas ? 0 : 10));
        girl.pclove(-(g_Dice % 35 - (mas ? 5 : -5)));
        girl.happiness(-(g_Dice % 25 - (mas ? 5 : 0)));
        girl.spirit(-(g_Dice % 15 - (mas ? 3 : 0)));
        girl.obedience(g_Dice % 10 - 3);

    } else/*                                        */    //Evil
    {
        bool mas = girl.has_active_trait("Masochist");
        girl.m_AccLevel = 0;
        girl.pcfear(g_Dice % 40 + (mas ? 0 : 20));
        girl.pclove(-(g_Dice % 70 - (mas ? 10 : 10)));
        girl.happiness(-(g_Dice % 50 - (mas ? 10 : 0)));
        girl.spirit(-(g_Dice % 30 - (mas ? 5 : 0)));
        girl.obedience(g_Dice % 20 - 5);
    }
}

void cScreenSlaveMarket::update_release_text()
{
    if(!m_TargetBuilding)
        return;
    std::stringstream  ss;
    ss << "Send Girl to: " << m_TargetBuilding->name();
    EditTextItem(ss.str(), releaseto_id);
    ss.str("");
    ss << "Room for " << m_TargetBuilding->free_rooms() << " more girls.";
    EditTextItem(ss.str(), roomsfree_id);
}
