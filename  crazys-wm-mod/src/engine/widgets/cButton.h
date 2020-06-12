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
#ifndef __CBUTTON_H
#define __CBUTTON_H

#include <string>
#include <memory>
#include <functional>

#include "interface/cInterfaceObject.h"
#include "interface/cSurface.h"

class cButton : public cUIWidget
{
public:
	cButton(cInterfaceWindow* parent, const std::string& OffImage, const std::string& DisabledImage, const std::string& OnImage, int ID,
	        int x, int y, int width, int height, bool transparency = false);
	~cButton();

    bool HandleClick(int x, int y, bool press) override;
    void HandleMouseMove(bool over, int x, int y) override;
    bool HandleKeyPress(SDL_Keysym key) override;

	void SetDisabled(bool disable) override;
    void DrawWidget(const CGraphics& gfx) override;

    void SetCallback(std::function<void()>);
    void SetHotKey(SDL_Keycode key);

private:
    cSurface m_OffImage;
    cSurface m_DisabledImage;
    cSurface m_OnImage;
    std::function<void()> m_Callback;
    SDL_Keycode m_HotKey = SDLK_UNKNOWN;

    cSurface* m_CurrImage;
};


#endif
