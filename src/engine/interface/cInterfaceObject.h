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
#ifndef __CINTERFACEOBJECT_H
#define __CINTERFACEOBJECT_H

#include <string>
#include <memory>
#include <functional>
#include <SDL_keyboard.h>
#include "fwd.hpp"

class cInterfaceObject
{
public:
    cInterfaceObject();
    virtual ~cInterfaceObject();
    void SetPosition(int x, int y, int width, int height) {m_XPos = x; m_YPos = y; m_Width = width; m_Height = height;}
    virtual void Draw(const CGraphics& gfx) {};
    int GetXPos() {return m_XPos;}
    int GetYPos() {return m_YPos;}
    int GetWidth() {return m_Width;}
    int GetHeight() {return m_Height;}

protected:
    int m_XPos, m_YPos;
    int m_Width = -1;
    int m_Height = -1;
};

/*
 * Common code for the different UI Widgets
 */
class cUIWidget: public cInterfaceObject
{
public:
    cUIWidget(int id, int x, int y, int width, int height, cInterfaceWindow* parent) : m_ID(id), m_Parent(parent) {
        SetPosition(x, y, width, height);
    }

    // Hiding
    void hide()        { SetHidden(true); }
    void unhide()    { SetHidden(false); }
    virtual void SetHidden(bool mode) { m_Hidden = mode; }
    bool IsHidden() const { return m_Hidden; }

    // Disabling
    virtual void SetDisabled(bool disabled) { m_Disabled = disabled; }
    bool IsDisabled() const { return m_Disabled; }

    // Focus
    bool SetFocus(bool focus) { if(HandleSetFocus(focus)) {m_HasFocus = focus; return true;} return false; }
    bool HasFocus() const { return m_HasFocus; }

    cInterfaceWindow* GetParent() const { return m_Parent; }
    CGraphics& GetGraphics();

    void Draw(const CGraphics& gfx) final {
        if(!m_Hidden)
            DrawWidget(gfx);
    }

    int get_id() const { return m_ID; }

    virtual void Reset() { };

    // interactions
    virtual bool IsOver(int x, int y) const {
        if(m_Hidden) return false;
        return x > m_XPos && y > m_YPos && x < m_XPos + m_Width && y < m_YPos + m_Height;
    }

    bool OnMouseClick(int x, int y, bool press) {
        if(m_Disabled || !IsOver(x, y)) return false;
        return HandleClick(x, y, press);
    }

    void OnMouseMove(int x, int y) {
        if(!m_Disabled) {
            HandleMouseMove(IsOver(x, y), x, y);
        }
    }

    void OnTextInput(const char* ip) {
        if(!m_Disabled) {
            HandleTextInput(ip);
        }
    }

    /// force: This is true if the even should be handled even if the mouse is not over the widget
    bool OnMouseWheel(bool down, int x, int y, bool force)
    {
        if(m_Disabled|| (!IsOver(x, y) && !force)) return false;
        return HandleMouseWheel(down);
    }

    bool OnKeyPress(SDL_Keysym keysym) {
        if(m_Disabled) return false;
        return HandleKeyPress(keysym);
    }

private:
    int m_ID;
    bool m_Hidden = false;
    bool m_Disabled = false;
    bool m_HasFocus = false;
    cInterfaceWindow* m_Parent = nullptr;

    virtual void DrawWidget(const CGraphics& gfx) = 0;
    virtual bool HandleClick(int x, int y, bool press) { return false; }
    virtual void HandleMouseMove(bool over, int x, int y) { };
    virtual bool HandleKeyPress(SDL_Keysym sym) { return false; };
    virtual bool HandleMouseWheel(bool down)    { return false; }
    virtual bool HandleSetFocus(bool focus) { return false; }
    virtual void HandleTextInput(const char* t) { }
};

#endif
