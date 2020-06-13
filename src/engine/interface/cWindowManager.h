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
#ifndef __CWINDOWMANAGER_H
#define __CWINDOWMANAGER_H

#include <stack>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include "fwd.hpp"

class cModalWindow;
class IBuilding;
class SDL_Keysym;
class sGirl;
class CGraphics;

/*!
 * \brief Manages the game's ui by handling the windows that are show.
 */
class cWindowManager
{
public:
    explicit cWindowManager(CGraphics*);
    ~cWindowManager();

    void add_window(std::string name, std::shared_ptr<cInterfaceWindow> win);

    // navigation
    void push(const std::string& window_name);
    /// replaces the top window with a new window. Differs from push/pop in that the window in the lower layer will never
    /// be initialized.
    void replace(const std::string& window_name);

    // remove function from the stack
    void Pop();
    void PopAll();

    void PopToWindow(const std::string& window_name);

    void UpdateCurrent();
    void UpdateMouseMovement(int x, int y);
    void OnMouseClick(int x, int y, bool down);
    void OnMouseWheel(int x, int y, bool mouseWheelDown = false);
    void OnTextInput(const char* text);

    cInterfaceWindow* GetWindow(bool allow_model=true);
    void Draw();

    CGraphics& GetGraphics();

    // user interaction
    // this function is called when a key is pressed
    void OnKeyPress(SDL_Keysym key);

    IBuilding* GetActiveBuilding() const;
    void SetActiveBuilding(IBuilding*);

    // input handling
    void InputInteger(std::function<void(int)> callback, int def_value);
    void InputConfirm(std::function<void()> callback);
    void InputString(std::function<void(const std::string&)> callback);
    void InputChoice(std::string question, std::vector<std::string> options, std::function<void(int)> callback);

    // message box
    void PushMessage(std::string text, int color, std::function<void()> cb = {});

    std::shared_ptr<cModalWindow> GetModalWindow();

    // girl selection
    sGirl* GetActiveGirl() const;
    void SetActiveGirl(sGirl* girl);
private:
    // normal windows
    std::vector<std::shared_ptr<cInterfaceWindow>> m_WindowStack;

    // map of known window names
    std::map<std::string, std::shared_ptr<cInterfaceWindow>> windows;

    // the currently selected building
    IBuilding* m_ActiveBuilding = nullptr;

    // the active girl list.
    std::vector<sGirl*> m_SelectedGirls;

    // pointer to the graphics engine
    CGraphics* m_GFX;
};


#endif
