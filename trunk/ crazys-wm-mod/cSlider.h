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
#ifndef __cSlider_H
#define __cSlider_H

#include <string>
using namespace std;
#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"

extern cInterfaceEventManager g_InterfaceEvents;

class cSlider : public cInterfaceObject
{
private:
	int m_MinVal;  // minumum value, left end of slider
	int m_MaxVal;  // maximum value, right end of slider
	int m_Value;  // current value of slider position
	int m_IncrementAmount;  // how much to increment/decrement the value when clicking to left or right of drag-button

	int m_Offset;  // physical X offset for left edge of button (determined based on m_Value, of course)
	int m_MaxOffset;  // maximum X offset (width of element - width of button)
	int m_LastOffset;  // stored X offset when initiating drag
	int m_DragInitXPos;  // stored mouse position when initiating drag

	void LoadInitial();  // run by first slider control created, to load appropriate source images into memory
	void ValueToOffset();  // set the Offset of the button based on the current Value
	void OffsetToValue();  // conversely, set the Value based on the current Offset of the button

	SDL_Rect* BGLeft;  // rectangle sized for left half of displayed background rail
	SDL_Rect* BGRight;  // rectangle sized and moved for right half of displayed background rail
	SDL_Surface* m_ImgButton;  // points to the appropriate button surface (off, on, or disabled)
	SDL_Surface* m_ImgRail;  // points to the appropriate background rail surface (normal or disabled)

	static SDL_Surface* m_ImgRailDefault;  // default background surface (rail) image, shared by all slider controls
	static SDL_Surface* m_ImgRailDisabled;  // etc.
	static SDL_Surface* m_ImgButtonOff;
	static SDL_Surface* m_ImgButtonOn;
	static SDL_Surface* m_ImgButtonDisabled;

	void LogSliderError(string description);

public:
	cSlider();
	~cSlider();

	bool CreateSlider(int ID, int x, int y, int width, int min = 0, int max = 100, int increment = 5, int value = 0, float height = 1.0);

	bool IsOver(int x, int y);  // determine if mouse is over control, switch button image accordingly
	bool MouseDown(int x, int y);  // user clicked mouse down; determine if mouse is over button itself so that we need to initiate drag
	void DragMove(int x);  // slider button is being dragged and has moved, so update offset and value
	void EndDrag();  // slider button is no longer being dragged
	bool ButtonClicked(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);  // handle mouse-up and scroll wheel
	virtual void Draw();  // blit background rail and button to screen

	int SetRange(int min, int max, int value, int increment);  // change min value, max value, current value, and increment amount
	int Value()	{ return m_Value; }  // get value
	int Value(int NewValue, bool TriggerEvent = false);  // set value, return value afterwards (might be different due to out-of-bounds)

	void Disable(bool disable);  // disable/enable this control
	void Hide(bool hide) { m_Hidden = hide; }  // hide/unhide this control
	void LiveUpdate(bool live_update) { m_LiveUpdate = live_update; }  // set LiveUpdate on or off

	int m_ID;
	bool m_Hidden;  // don't show and don't respond?
	bool m_Disabled;  // don't respond, and show grayed-out controls?
	bool m_LiveUpdate;  // send update events while user is dragging? default enabled; if disabled, only sends update event on MouseUp
};


#endif
