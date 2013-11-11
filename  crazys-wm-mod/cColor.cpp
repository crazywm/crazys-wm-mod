/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
//#include <iostream>
//#include <string>
//#include <vector>
#include "cColor.h"

extern CLog g_LogFile;
struct SDL_Color;

Uint8 cColor::convertFromHex(string hex)
{
	Uint8 value = 0;
    
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
					g_LogFile.ss() << "Error, invalid character '" << hex[a] << "' in hex number" << endl;
					g_LogFile.ssend();
					break;
			}
		}
	}
    
	return value;
}

void cColor::HexToSDLColor(string HexColor, SDL_Color* SDLColor)
{
	int offset = (HexColor.substr(0, 1) == "#") ? 1 : 0;

	//int x = HexColor.length();
	if(HexColor.length() < 6 || HexColor.length() > 7)
	{  // not a valid RGB hex value
		SDLColor->r = 0;
		SDLColor->g = 0;
		SDLColor->b = 0;
		g_LogFile.ss() << "Error, provided string '" << HexColor << "' is not a valid RGB hex value" << endl;
		g_LogFile.ssend();
		return;
	}

	string redString = HexColor.substr(0+offset, 2);
	string greenString = HexColor.substr(2+offset, 2);
	string blueString = HexColor.substr(4+offset, 2);

	SDLColor->r = convertFromHex(redString);
	SDLColor->g = convertFromHex(greenString);
	SDLColor->b = convertFromHex(blueString);
}

