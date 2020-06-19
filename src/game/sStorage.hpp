/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#ifndef CRAZYS_WM_MOD_SSTORAGE_HPP
#define CRAZYS_WM_MOD_SSTORAGE_HPP

#include <string>
#include <map>
namespace tinyxml2 {
    class XMLElement;
}

class sStorage {
public:
    // get amount available
    int food() const     { return get_stored_amount(food_key); }
    int drinks() const   { return get_stored_amount(drink_key); }
    int beasts() const   { return get_stored_amount(beast_key); }
    int goods() const    { return get_stored_amount(goods_key); }
    int alchemy() const  { return get_stored_amount(alchemy_key); }

    int get_stored_amount(const std::string& key) const;

    // add to storage
    int add_to_food(int i)     { return add_to_storage(food_key, i); }
    int add_to_drinks(int i)   { return add_to_storage(drink_key, i); }
    int add_to_beasts(int i)   { return add_to_storage(beast_key, i); }
    int add_to_goods(int i)    { return add_to_storage(goods_key, i); }
    int add_to_alchemy(int i)  { return add_to_storage(alchemy_key, i); }

    int add_to_storage(const std::string& key, int amount);

    // take out of storage

    // get excess amount
    int get_excess_food() const    { return get_excess(food_key); }
    int get_excess_drinks() const  { return get_excess(drink_key); }
    int get_excess_beasts() const  { return get_excess(beast_key); }
    int get_excess_goods() const   { return get_excess(goods_key); }
    int get_excess_alchemy() const { return get_excess(alchemy_key); }
    int get_excess(const std::string& key) const;

    void load_from_xml(const tinyxml2::XMLElement& el);

private:
    struct StorageRecord {
        int amount  = 0;
        int reserve = 0;
    };

    std::map<std::string, StorageRecord> m_Storage;

    // keys
    static std::string food_key;
    static std::string drink_key;
    static std::string beast_key;
    static std::string goods_key;
    static std::string alchemy_key;
};

#endif //CRAZYS_WM_MOD_SSTORAGE_HPP
