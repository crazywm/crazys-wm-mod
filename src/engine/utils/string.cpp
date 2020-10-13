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

#include "utils/string.hpp"
#include "cRng.h"
#include <regex>

namespace {
    std::regex scan_for_replacements{R"(\$\{[^}]*\}|\$\[[^\]]*\])", std::regex_constants::optimize};
}

void interpolate_string(std::ostream& target, const std::string& text_template,
                        const std::function<std::string(const std::string&)>& lookup, cRng& random) {
    std::sregex_iterator match_iter(begin(text_template), end(text_template), scan_for_replacements);
    std::sregex_iterator end_iter;

    std::size_t current_position = 0;
    while(match_iter != end_iter) {
        // write prefix text
        target.write(&text_template[current_position], match_iter->position() - current_position);
        auto match_begin = text_template.begin() + match_iter->position();
        if(*(match_begin+1) == '{') {
            // handle lookup
            std::string match{text_template.begin() + (match_iter->position() + 2), text_template.begin() + (match_iter->position() + match_iter->length() - 1)};
            interpolate_string(target, lookup(match), lookup, random);
        } else {
            assert(*(match_begin+1) == '[');
            auto last_start = match_begin + 1;
            auto start = match_begin + 2;
            auto end = match_begin + 3;
            int options = 0;
            for(auto iter = start; *iter != ']'; ++iter) {
                if(*iter == '|') {
                    ++options;
                    if (random.percent(100.f / options)) {
                        start = last_start + 1;
                        end = iter;
                    }
                    last_start = iter;
                }
            }

            ++options;
            if (random.percent(100.f / options)) {
                start = last_start + 1;
                end = match_begin + (match_iter->length() - 1);
            }

            std::string selected = {start, end};
            interpolate_string(target, selected, lookup, random);
        }

        current_position = match_iter->position() + match_iter->length();
        ++match_iter;
    };

    // push the suffix text
    target.write(&text_template[current_position], text_template.size() - current_position);
}

std::string
interpolate_string(const std::string& text_template, const std::function<std::string(const std::string&)>& lookup,
                   cRng& random) {
    std::stringstream target;
    interpolate_string(target, text_template, lookup, random);
    return target.str();
}

std::string readline(std::istream& is) {
    std::string str;

    const auto eof = std::istream::traits_type::eof();

    while(true)
    {
        auto ch = is.get();
        if(ch == eof)
            return str;
        else if(ch == '\n')    // \n -- Unix style
            return str;
        else if(ch == '\r')    // \r -- Mac style
        {
            auto ch2 = is.get();
            if(ch2 == eof)
                return str;
            else if(ch2 == '\n') // \r\n -- Windows style
                return str;
            else
            {
                is.unget();      // unread next lines's 1st char
                return str;
            }
        }
        else
            str.push_back(ch);
    }
}

#include "doctest.h"
#include "utils/algorithms.hpp"
TEST_CASE("readline") {
    std::stringstream stream;
    SUBCASE("linux-style") {
        stream.str("this is a line\n");
    }
    SUBCASE("windows-style") {
        stream.str("this is a line\r\n");
    }
    SUBCASE("mac-style") {
        stream.str("this is a line\r");
    }
    CHECK(readline(stream) == "this is a line");
}


TEST_CASE("string interpolation") {
    std::stringstream target;
    cRng random;

    auto replacement = [](const std::string& pattern) -> std::string {
        if(pattern == "name") {
            return "THE NAME";
        } else if(pattern == "test") {
            return "put this long stuff here";
        } else if(pattern == "recurse") {
            return "the replacement contains ${name}";
        } else if(pattern == "start rep") {
            return "${na";
        } else if(pattern == "end rep") {
            return "me}";
        }
        CHECK(false);
        return "";
    };

    SUBCASE("no replacement") {
        interpolate_string(target, "This is a simple string.", replacement, random);
        CHECK(target.str() == "This is a simple string.");
    }

    SUBCASE("simple replacement") {
        interpolate_string(target, "Here is a ${name} followed by some text.", replacement, random);
        CHECK(target.str() == "Here is a THE NAME followed by some text.");
    }

    SUBCASE("multi replacement") {
        interpolate_string(target, "${test} and ${name} and ${name} again.", replacement, random);
        CHECK(target.str() == "put this long stuff here and THE NAME and THE NAME again.");
    }

    SUBCASE("recursive replacement") {
        interpolate_string(target, "${recurse}!", replacement, random);
        CHECK(target.str() == "the replacement contains THE NAME!");
    }

    SUBCASE("no dynamic replacement") {
        interpolate_string(target, "${start rep}${end rep}!", replacement, random);
        CHECK(target.str() == "${name}!");
    }

    SUBCASE("braces in text") {
        interpolate_string(target, "This { text has ${name} more braces}.", replacement, random);
        CHECK(target.str() == "This { text has THE NAME more braces}.");
    }

    SUBCASE("empty alternative") {
        interpolate_string(target, "This is empty $[].", replacement, random);
        CHECK(target.str() == "This is empty .");
    }

    SUBCASE("single alternative") {
        interpolate_string(target, "This is $[one].", replacement, random);
        CHECK(target.str() == "This is one.");
    }

    SUBCASE("two alternatives") {
        interpolate_string(target, "This is $[one|two].", replacement, random);
        CHECK(is_in(target.str(), {"This is one.", "This is two."}));
    }

    SUBCASE("replace in alternative") {
        interpolate_string(target, "This is $[${name}].", replacement, random);
                CHECK(target.str() == "This is THE NAME.");
    }

    // TODO a test that checks the uniform distribution
}