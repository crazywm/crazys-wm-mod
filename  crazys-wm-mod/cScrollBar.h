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
#ifndef __cScrollBar_H
#define __cScrollBar_H

#include <string>
#include <memory>
using namespace std;
#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"

class CSurface;

class cScrollBar : public cUIWidget
{
	static void LoadInitial();
	static SDL_Surface* m_ImgBarBG;  // static class-wide shared images
	static SDL_Surface* m_ImgBarOn;  // ...
	static SDL_Surface* m_ImgBarOff;
	static SDL_Surface* m_ImgBarDisabled;
	static SDL_Surface* m_ImgNotches;
	static Uint8 m_NotchOffset;  // Y offset for drawing bar notches
	static SDL_Surface* m_ImgButtonUpOn;
	static SDL_Surface* m_ImgButtonUpOff;
	static SDL_Surface* m_ImgButtonUpDisabled;
	static SDL_Surface* m_ImgButtonDownOn;
	static SDL_Surface* m_ImgButtonDownOff;
	static SDL_Surface* m_ImgButtonDownDisabled;

	SDL_Surface* m_ImgBar = nullptr;  // points to the appropriate bar surface above (on, off, or disabled)
	SDL_Surface* m_ImgButtonUp = nullptr;  // same for "Up" button surface
	SDL_Surface* m_ImgButtonDown = nullptr;  // same for "Down" button surface
	std::unique_ptr<SDL_Rect> m_RectBGTop;  // rectangle for top half of background to be blitted
    std::unique_ptr<SDL_Rect> m_RectBGBottom;  // rectangle for bottom half of background to be blitted
    std::unique_ptr<SDL_Rect> m_RectTop;  // rectangle for top half of bar to be blitted
    std::unique_ptr<SDL_Rect> m_RectBottom;  // rectangle for bottom half of bar to be blitted

	void LogScrollBarError(const string& description);

public:
	cScrollBar(int ID, int x, int y, int width, int height, int visibleitems);
	~cScrollBar();

	void UpdateScrollBar();  // update size of draggable bar based on total items vs. visible items
	bool IsOver(int x, int y);
	bool MouseDown(int x, int y);
	void DragMove(int y);  // dragging of bar was initiated, so handle movement
	bool ButtonClicked(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);
	void SetTopValue(int itemnum);  // update bar position based on top visible item
	void SetDisabled(bool disable);

	void DrawWidget() override;

	int m_BarHeight = 0;  // height of draggable bar
	int m_SectionHeight = 0;  // height of entire section, not just draggable bar
	int m_BarTop = 0;  // top Y offset of draggable bar
	int m_ItemTop = 0;  // top item visible; correlates to m_BarTop
	int m_ItemsTotal = 0;  // total items
	int m_ItemsVisible = 0;  // visible items
	int m_ItemsTotalLast = 0;  // track last items total, to determine if bar size update is needed
	int m_DragInitYPos;  // track position drag was initiated from

	int m_ScrollAmount = 1;  // how much to change the position from mouse scroll up/down events
	int m_PageAmount = 3;  // how much to change the position from clicking in the scroll area above or below the draggable bar

	bool m_UpdateSelf = true;  // whether control should update own position when sending updates to parent

	bool m_Disabled = false;

	int *ParentPosition = nullptr;  // pointer to callback value of parent with updated position
};


#endif
