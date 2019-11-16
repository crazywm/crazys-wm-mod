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
#ifndef __cSlider_H
#define __cSlider_H

#include <string>
#include <memory>
#include <functional>
#include "interface/cSurface.h"
#include "interface/cInterfaceObject.h"


class cSlider : public cUIWidget
{
private:
	int m_MinVal;  // minumum value, left end of slider
	int m_MaxVal;  // maximum value, right end of slider
	int m_Value;  // current value of slider position
	int m_IncrementAmount;  // how much to increment/decrement the value when clicking to left or right of drag-button

	int m_Offset = 0;  // physical X offset for left edge of button (determined based on m_Value, of course)
	int m_MaxOffset = 0;  // maximum X offset (width of element - width of button)
	int m_LastOffset = 0;  // stored X offset when initiating drag
	int m_DragInitXPos;  // stored mouse position when initiating drag

	bool m_ShowMarker = false;  // whether to show the marker
	int m_MarkerOffset = 0;  // physical X offset to show the marker at

    // run by first slider control created, to load appropriate source images into memory
	static cSurface LoadAlphaImageFromFile(std::string file);  // utility function used by above for loading specific source image from file
	void ValueToOffset();  // set the Offset of the button based on the current Value
	void OffsetToValue();  // conversely, set the Value based on the current Offset of the button

	std::unique_ptr<SDL_Rect> BGLeft;  // rectangle sized for left half of displayed background rail
    std::unique_ptr<SDL_Rect> BGRight;  // rectangle sized and moved for right half of displayed background rail
	cSurface m_ImgButton;           // points to the appropriate button surface (off, on, or disabled)
	cSurface m_ImgRail;             // points to the appropriate background rail surface (normal or disabled)

	cSurface m_ImgRailDefault;      // default background surface (rail) image, shared by all slider controls
	cSurface m_ImgRailDisabled;     // etc.
	cSurface m_ImgButtonOff;
	cSurface m_ImgButtonOn;
	cSurface m_ImgButtonDisabled;
	cSurface m_ImgMarker;

    int CalcHeight(float height);

	void OnValueUpdate();

	std::function<void(int)> m_Callback;

	bool m_IsBeingDragged = false;
public:
	cSlider(cInterfaceWindow* parent, int ID, int x, int y, int width, int min = 0, int max = 100, int increment = 5, int value = 0, float height = 1.0);
    ~cSlider();

	bool IsOver(int x, int y) const override;  // determine if mouse is over control, switch button image accordingly
	void DragMove(int x);  // slider button is being dragged and has moved, so update offset and value
	void EndDrag();  // slider button is no longer being dragged
	void DrawWidget(const CGraphics& gfx) override;  // blit background rail and button to screen

	int SetRange(int min, int max, int value, int increment);  // change min value, max value, current value, and increment amount
	int Value()	{ return m_Value; }  // get value
	int Value(int NewValue, bool TriggerEvent = false);  // set value, return value afterwards (might be different due to out-of-bounds)

	void SetMarker(int value);  // set value for a visual "marker" to be shown at, to denote a default or target value or similar
	void RemoveMarker();  // stop displaying marker

	void SetDisabled(bool disable) override;  // disable/enable this control
	void LiveUpdate(bool live_update) { m_LiveUpdate = live_update; }  // set LiveUpdate on or off

	void SetCallback(std::function<void(int)>);
	void SetHotKeys(SDLKey increase, SDLKey decrease);

	bool m_LiveUpdate = true;  // send update events while user is dragging? default enabled; if disabled, only sends update event on MouseUp

    bool HandleClick(int x, int y, bool press) override;
    void HandleMouseMove(bool over, int x, int y) override;
    bool HandleMouseWheel(bool down) override;
    bool HandleKeyPress(SDL_keysym key) override;
    bool HandleSetFocus(bool focus) override;

private:
    SDLKey m_IncreaseHotKey = SDLK_UNKNOWN;
    SDLKey m_DecreaseHotKey = SDLK_UNKNOWN;
};


#endif
