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

#include "cColor.h"
#include <SDL2/SDL.h>
#include "CLog.h"

struct SDL_Color;

std::uint8_t cColor::convertFromHex(std::string hex)
{
	std::uint8_t value = 0;
    
	int a = 0;
	int b = hex.length() - 1;
	for (; b >= 0; a++, b--)
	{
		if (hex[b] >= '0' && hex[b] <= '9')
		{
			value += (hex[b] - '0') * (1 << (a * 4));
		}
		else
		{
			switch (hex[b])
			{
				case 'A':
				case 'a':
					value += 10 * (1 << (a * 4));
					break;
                    
				case 'B':
				case 'b':
					value += 11 * (1 << (a * 4));
					break;
                    
				case 'C':
				case 'c':
					value += 12 * (1 << (a * 4));
					break;
                    
				case 'D':
				case 'd':
					value += 13 * (1 << (a * 4));
					break;
                    
				case 'E':
				case 'e':
					value += 14 * (1 << (a * 4));
					break;
                    
				case 'F':
				case 'f':
					value += 15 * (1 << (a * 4));
					break;
                    
				default:
				    g_LogFile.error("interface", "Invalid character '", hex[a], "' in hex number");
				    break;
			}
		}
	}
    
	return value;
}

void cColor::HexToSDLColor(std::string HexColor, SDL_Color* SDLColor)
{
	int offset = (HexColor.substr(0, 1) == "#") ? 1 : 0;

	//int x = HexColor.length();
	if(HexColor.length() < 6 || HexColor.length() > 7)
	{  // not a valid RGB hex value
		SDLColor->r = 0;
		SDLColor->g = 0;
		SDLColor->b = 0;
        g_LogFile.error("interface", "Provided string '", HexColor, "' is not a valid RGB hex value");
		return;
	}

	std::string redString = HexColor.substr(0+offset, 2);
	std::string greenString = HexColor.substr(2+offset, 2);
	std::string blueString = HexColor.substr(4+offset, 2);

	SDLColor->r = convertFromHex(redString);
	SDLColor->g = convertFromHex(greenString);
	SDLColor->b = convertFromHex(blueString);
}

void cColor::RGBToSDLColor(SDL_Color* SDLColor, int r = 0, int g = 0, int b = 0)
{
	SDLColor->r = r;
	SDLColor->g = g;
	SDLColor->b = b;
}

void number_to_hex(char* target, std::uint8_t number) {
    std::uint8_t first_digit  = (number & 0xF0u) >> 4u;
    std::uint8_t second_digit = number & 0x0Fu;
    target[0] = first_digit + '0';
    target[1] = second_digit + '0';
}

std::string sColor::to_hex() const
{
    std::string result(6, ' ');
    number_to_hex(&result.front(), r);
    number_to_hex(&result.front() + 2, r);
    number_to_hex(&result.front() + 4, r);
    return std::move(result);
}
