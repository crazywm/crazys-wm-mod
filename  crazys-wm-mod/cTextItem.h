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
#ifndef __CTEXTITEM_H
#define __CTEXTITEM_H

#include "cFont.h"
#include "cInterfaceObject.h"
#include "sConfig.h"
#include "cScrollBar.h"

extern cConfig cfg;

class cTextItem : public cInterfaceObject
{
	bool m_Hide;
	bool m_AutoScrollBar;	// automatically use scrollbar if text is too tall?
	bool m_ForceScrollBar;	// force scrollbar display even if text fits?

public:
	cTextItem();
	~cTextItem();

	void CreateTextItem(int ID, int x, int y, int width, int height, string text, int size, bool auto_scrollbar = true,
		bool force_scrollbar = false, bool leftorright = false, int red = 0, int green = 0, int blue = 0);
	void DisableAutoScroll(bool disable);
	void ForceScrollBar(bool force);

	// does scrollbar exist, but current text fits, and scrollbar isn't being forced?
	bool NeedScrollBarHidden();
	// does scrollbar exist but is hidden, and current text doesn't fit?
	bool NeedScrollBarShown();
	// does a scrollbar need to be added?
	bool NeedScrollBar();
	int HeightTotal();
	void MouseScrollWheel(int x, int y, bool ScrollDown = true);

	bool IsOver(int x, int y);

	void ChangeFontSize(int FontSize, int red = 0, int green = 0, int blue = 0);

	void SetText(string text);
	void Draw();

	void hide();
	void unhide();

	//int m_CharsPerLine, m_LinesPerBox, m_CharHeight;
	string m_Text;
	int m_ID;
	cTextItem* m_Next;
	cFont m_Font;
	int m_FontHeight;		// height of the font

	cScrollBar* m_ScrollBar;  // pointer to the associated scrollbar, if any
	int m_ScrollChange;  // scrollbar changes will update this value; translates to skipped lines of text
};

#endif
