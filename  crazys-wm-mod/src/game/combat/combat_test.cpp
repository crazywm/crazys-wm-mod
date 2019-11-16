#include "combat.h"
#include "combatant.h"
#include <iostream>
#include <cGangs.h>

#include <functional>
#include <map>

// standard combatants

struct CombatResultAggregate {
    int wins = 0;
    int draws = 0;
    int defeats = 0;
};

struct CombatTest {
    static auto HighLevelPhysical() {
        return std::make_unique<Combatant>("High Level Physical", 90, 0, 0, 90, 90, 0, 90, 90);
    }

    static auto MidLevelPhysical() {
        return std::make_unique<Combatant>("Mid Level Physical", 50, 0, 0, 50, 50, 0, 50, 50);
    }

    static auto LowLevelPhysical() {
        return std::make_unique<Combatant>("Low Level Physical", 20, 0, 0, 20, 20, 0, 20, 20);
    }

    static auto HighLevelMagical() {
        return std::make_unique<Combatant>("High Level Magical", 70, 90, 0, 50, 50, 90, 50, 20);
    }

    static auto MidLevelMagical() {
        return std::make_unique<Combatant>("Mid Level Magical", 40, 50, 0, 30, 30, 50, 30, 20);
    }

    static auto LowLevelMagical() {
        return std::make_unique<Combatant>("Low Level Magical", 20, 20, 0, 20, 30, 20, 30, 20);
    }

    CombatTest() {
        m_Rooster["HLP"] = CombatTest::HighLevelPhysical;
        m_Rooster["MLP"] = CombatTest::MidLevelPhysical;
        m_Rooster["LLP"] = CombatTest::LowLevelPhysical;

        m_Rooster["HLM"] = CombatTest::HighLevelMagical;
        m_Rooster["MLM"] = CombatTest::MidLevelMagical;
        m_Rooster["LLM"] = CombatTest::LowLevelMagical;
    }

    ECombatResult run_single( std::initializer_list<const char*> attackers, std::initializer_list<const char*> defenders,
            bool narrate = false ) {
        Combat c{ECombatObjective::KILL, ECombatObjective::KILL};
        for (auto& p : attackers) {
            c.add_combatant(ECombatSide::ATTACKER, m_Rooster[p]());
        }

        for (auto& p : defenders) {
            c.add_combatant(ECombatSide::DEFENDER, m_Rooster[p]());
        }

        auto res = c.run();
        if(narrate)
            std::cout << c.narration().str() << "\n\n";
        return res;
    }

    CombatResultAggregate run_test( std::initializer_list<const char*> attackers, std::initializer_list<const char*> defenders ) {
        CombatResultAggregate res{};
        for(int i = 0; i < 10000; ++i) {
            switch(run_single(attackers, defenders)) {
                case ECombatResult::VICTORY:
                    ++res.wins;
                    break;
                case ECombatResult::DEFEAT:
                   ++res.defeats;
                    break;
                case ECombatResult::DRAW:
                    ++res.draws;
                    break;
            }
        }

        std::cout << "RESULT: " << res.wins << "/" << res.draws << "/" << res.defeats << "\n";
        return res;
    }

    void test_case(const char* a, const char* b, CombatResultAggregate min, CombatResultAggregate max) {
        auto result = run_test({a}, {b});
        // check validity
        if(result.wins < min.wins || result.wins > max.wins) {
            std::cerr << "FAIL!";
        }

        if(result.draws < min.draws || result.draws > max.draws) {
            std::cerr << "FAIL!";
        }

        if(result.defeats < min.defeats || result.defeats > max.defeats) {
            std::cerr << "FAIL!";
        }

        std::cout << "PASS\n";
    }

    std::map<std::string, std::function<std::unique_ptr<Combatant>()>> m_Rooster;
};



int main() {
    CombatTest test;

    test.run_single({"HLM"}, {"HLP"}, true);

    // general properties: As the combatants get more skilled, draws get less likely
    // physical combat
    /*test.test_case("HLP", "HLP", {3000, 2000, 3000}, {4000, 4000, 4000});
    test.test_case("HLP", "MLP", {9500, 25, 0}, {10000, 100, 10});
    test.test_case("HLP", "LLP", {9900, 5, 0}, {10000, 10, 1});

    test.test_case("MLP", "MLP", {2000, 4000, 2000}, {4000, 7000, 4000});
    test.test_case("MLP", "LLP", {9500, 500, 0}, {9900, 1000, 10});

    test.test_case("LLP", "LLP", {500, 5000, 500}, {2000, 8000, 2000});
    */

    // magic combat
    test.test_case("HLM", "HLP", {3000, 2000, 3000}, {4000, 4000, 4000});
    //test.test_case("HLM", "HLM", {3000, 2000, 3000}, {4000, 4000, 4000});
    /*test.test_case("HLM", "MLM", {9500, 25, 0}, {10000, 100, 10});
    test.test_case("HLM", "LLM", {9900, 5, 0}, {10000, 10, 1});

    test.test_case("MLM", "MLM", {2000, 4000, 2000}, {4000, 7000, 4000});
    test.test_case("MLM", "LLM", {9500, 500, 0}, {9900, 1000, 10});

    test.test_case("LLM", "LLM", {500, 5000, 500}, {2000, 8000, 2000});
*/
    /*
    Combat c{ECombatObjective::KILL, ECombatObjective::KILL};
    auto attacker = std::make_unique<Combatant>("Attacker", 50, 50, 20, 40, 50, 100, 30, 50);
    auto defender = std::make_unique<Combatant>("Defender", 50, 10, 20, 40, 100, 20, 30, 100);
    // c.add_combatant(ECombatSide::DEFENDER, std::move(attacker));
    c.add_combatant(ECombatSide::DEFENDER, std::move(defender));

    sGang gang("test");
    gang.m_Num = 5;
    gang.constitution(10);
    c.add_combatants(ECombatSide::ATTACKER, gang);

    c.run();

    std::cout << c.narration().str() << "\n";
    std::cout << "\n\n";
     */
}
