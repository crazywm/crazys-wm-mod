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
#ifndef __CMESSAGEBOX_H
#define __CMESSAGEBOX_H

#include <string>
#include <queue>

class SDL_Surface;
class cFont;

const int NUM_MESSBOXCOLOR = 4;

struct sMessage
{
    std::string m_Text = "";
    int m_Color   = 0;
};

// separate to text boxes and edit boxes these boxes will display text and on a user click advance to the next box
// to continue displaying the message
class cMessageBox
{
public:
	cMessageBox(int x = 32, int y = 416, int width = 736, int height = 160, int BorderSize = 1, int FontSize = 16, bool scale = true);
	~cMessageBox();

	void ChangeFontSize(int FontSize = 16);
	void Draw();
	void Advance();
	bool IsActive() const;

	void PushMessage(std::string text, int color);

private:
	int m_XPos, m_YPos, m_Height, m_Width, m_BorderSize, m_FontHeight;

	bool m_Advance;

	SDL_Surface* m_Background[NUM_MESSBOXCOLOR];
	SDL_Surface* m_Border;
	
	cFont* m_Font;
	std::string m_Text;	// contains the entire text string
	int m_Position;	// where we are up too

	bool m_TextAdvance;
	int m_Color;	// used to determine which color to use

    std::deque<sMessage> m_Messages;

    void UpdateMessageText();
};

#endif
