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
#include "cSlider.h"
#include "interface/CGraphics.h"
#include "CLog.h"
#include "DirPath.h"
#include <SDL_video.h>

//these static vars defined in the header file need to be specified here

cSlider::cSlider(cInterfaceWindow* parent, int ID, int x, int y, int width, int min, int max, int increment, int value, float height):
    cUIWidget(ID, x ,y, width, 1, parent), m_MinVal(min), m_MaxVal(max), m_Value(value), m_IncrementAmount(increment),
    BGLeft(new SDL_Rect), BGRight(new SDL_Rect)
{
    auto load = [this](const char* file){
        return GetGraphics().LoadImage(ImagePath("Slider").str() + file,
                -1, -1, true); };

    m_ImgButtonDisabled = load("ButtonDisabled.png");
    m_ImgButtonOff = load("ButtonOff.png");
    m_ImgButtonOn = load("ButtonOn.png");
    m_ImgRailDefault = load("Rail.png");
    m_ImgRailDisabled = load("RailDisabled.png");
    m_ImgMarker = load("Marker.png");
    /// TODO what is the point of this?
    if (m_ImgMarker.RawSurface() == nullptr) m_ImgMarker = m_ImgButtonDisabled;

    // at this point we can update the height based on the image sizes
    SetPosition(x, y, width, CalcHeight(height));

	m_MaxOffset = m_Width - m_ImgButtonOff.GetWidth();

    SetDisabled(false);
    ValueToOffset();

    // set up SDL_Rects indicating left and right halves of displayed background from source background images
    BGLeft->x = BGLeft->y = BGRight->y = 0;
    BGLeft->h = BGRight->h = m_ImgRailDefault.GetHeight();
    BGLeft->w = (m_Width / 2);
    BGRight->w = m_Width - BGLeft->w;
    BGRight->x = m_ImgRailDefault.GetWidth() - BGRight->w;
}

cSlider::~cSlider() = default;

int cSlider::SetRange(int min, int max, int value, int increment)
{
	m_MinVal = min;
	m_MaxVal = max;
	m_IncrementAmount = increment;
	return Value(value);
}

int cSlider::Value(int NewValue, bool TriggerEvent)
{
	if(NewValue < m_MinVal)
		NewValue = m_MinVal;
	if(NewValue > m_MaxVal)
		NewValue = m_MaxVal;

	int oldVal = m_Value;
	m_Value = NewValue;
	ValueToOffset();

	// if value changed, trigger update event if requested
	if(TriggerEvent && oldVal != m_Value)
		OnValueUpdate();

	return m_Value;
}

void cSlider::SetMarker(int value)
{
	m_ShowMarker = true;
	// see notes on offset calculation below in ValueToOffset()
	double PercMult = double(value - m_MinVal) / double(m_MaxVal - m_MinVal);
	m_MarkerOffset = int((m_MaxOffset * PercMult) + 0.5);
}

void cSlider::RemoveMarker()
{
	m_ShowMarker = false;
	m_MarkerOffset = 0;
}

void cSlider::SetDisabled(bool disable)
{
    cUIWidget::SetDisabled(disable);
	if(disable)
	{
		m_ImgButton = m_ImgButtonDisabled;
		m_ImgRail = m_ImgRailDisabled;
	}
	else
	{
		m_ImgButton = m_ImgButtonOff;
		m_ImgRail = m_ImgRailDefault;
	}
}

void cSlider::ValueToOffset()
{  // determine m_Offset based on m_Value
	// get percentage multiplier... ex. Value = 50, MinVal = 0, MaxVal = 100: 50%, so PercMult = 0.5
	double PercMult = double(m_Value - m_MinVal) / double(m_MaxVal - m_MinVal);
	// simple enough; the extra 0.5 is used to round the value (instead of "floor")
	m_Offset = int((m_MaxOffset * PercMult) + 0.5);
}

void cSlider::OffsetToValue()
{  // determine m_Value based on m_Offset
	// get percentage multiplier... ex. Offset = 500, MaxOffset = 1000: 50%, so PercMult = 0.5
	double PercMult = double(m_Offset) / double(m_MaxOffset);
	// slightly more complicated, but same principle as used to get offset from value
	m_Value = int((double(m_MaxVal - m_MinVal) * PercMult) + m_MinVal + 0.5);
}

bool cSlider::IsOver(int x, int y) const
{
	if(m_IsBeingDragged) {
	    return true;
	}
	return cUIWidget::IsOver(x, y);
}

void cSlider::DragMove(int x)
{
    // Slider is being dragged so update button position, and send out an update event if LiveUpdate is enabled
	// this function is called only by main.cpp mousemove when drag was initiated
	int NPos = x - m_XPos;  // new mouse position
	int NOffset = m_LastOffset + (NPos - m_DragInitXPos);  // new button offset

	if(NOffset < 0)  // make sure offset is within bounds
		NOffset = 0;
	if(NOffset > m_MaxOffset)
		NOffset = m_MaxOffset;

	m_Offset = NOffset;
	int oldVal = m_Value;
	OffsetToValue();  // set the value based on the new offset

	if(m_LiveUpdate && oldVal != m_Value)  // if doing live updates and value changed, trigger update event
		OnValueUpdate();
}

void cSlider::EndDrag()
{
    // user has stopped dragging the slider
	if(!m_LiveUpdate)
		OnValueUpdate();
	ValueToOffset();  // pop button position over to exact position for value
	m_IsBeingDragged = false;
}

void cSlider::DrawWidget(const CGraphics& gfx)
{
	// draw background rail
	m_ImgRail.DrawSurface(m_XPos, m_YPos, BGLeft.get());
	m_ImgRail.DrawSurface(m_XPos + BGLeft->w, m_YPos, BGRight.get());
	// draw marker if it's enabled
	if (m_ShowMarker)
	{
        m_ImgMarker.DrawSurface(m_XPos + m_MarkerOffset, m_YPos);
	}

	// draw slider drag button
	m_ImgButton.DrawSurface(m_XPos + m_Offset, m_YPos);
}

int cSlider::CalcHeight(float height) {
    return int(m_ImgRailDefault.GetHeight() * height);
}

void cSlider::SetCallback(std::function<void(int)> cb)
{
    m_Callback = std::move(cb);
}

void cSlider::OnValueUpdate()
{
    if(m_Callback)
        m_Callback(Value());
}

bool cSlider::HandleClick(int x, int y, bool press)
{
    if(!press && m_IsBeingDragged) {
        EndDrag();
        return true;
    }

    m_DragInitXPos = x - m_XPos;  // mouse X position within entire slider section

    // let's just assume that wherever they click in the slider, that's where they want it to go
    m_IsBeingDragged = true; // g_DragSlider is used in main.cpp to reference whichever slider is being dragged (if any)
    m_LastOffset = m_DragInitXPos - (m_ImgButtonOff.GetWidth()/2);  // store starting offset for drag handling
    DragMove(x);  // refresh button position to proper x value

    return true;
}

void cSlider::HandleMouseMove(bool over, int x, int y)
{
    m_ImgButton = (over || HasFocus()) ? m_ImgButtonOn : m_ImgButtonOff;
    if(m_IsBeingDragged) {
        DragMove(x);
    }
}

bool cSlider::HandleMouseWheel(bool down)
{
    Value(m_Value + (down ? -m_IncrementAmount : m_IncrementAmount), true);
    return true;
}

void cSlider::SetHotKeys(SDLKey increase, SDLKey decrease)
{
    m_IncreaseHotKey = increase;
    m_DecreaseHotKey = decrease;
}

bool cSlider::HandleKeyPress(SDL_keysym key)
{
    if(key.sym == m_IncreaseHotKey && m_IncreaseHotKey != SDLK_UNKNOWN) {
        Value(m_Value + m_IncrementAmount, true);
        return true;
    } else if(key.sym == m_DecreaseHotKey && m_DecreaseHotKey != SDLK_UNKNOWN) {
        Value(m_Value - m_IncrementAmount, true);
        return true;
    }
    if(HasFocus()) {
        if(key.sym == SDLK_LEFT) {
            Value(m_Value - m_IncrementAmount, true);
            return true;
        } else if (key.sym == SDLK_RIGHT) {
            Value(m_Value + m_IncrementAmount, true);
            return true;
        }
    }
    return false;
}

bool cSlider::HandleSetFocus(bool focus)
{
    m_ImgButton = focus ? m_ImgButtonOn : m_ImgButtonOff;
    return true;
}
