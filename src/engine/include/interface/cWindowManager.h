/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#include "interface/fwd.hpp"

class cModalWindow;
class IBuilding;
struct SDL_Keysym;
class sGirl;

/*!
 * \brief Manages the game's ui by handling the windows that are show.
 */
class cWindowManager
{
public:
    cWindowManager(CGraphics*, std::unique_ptr<cTheme> theme);
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
    // user interaction
    // this function is called when a key is pressed
    void OnKeyEvent(SDL_Keysym key, bool down);
    void UpdateMouseMovement(int x, int y);
    void OnMouseClick(int x, int y, bool down);
    void OnMouseWheel(int x, int y, bool mouseWheelDown = false);
    void OnTextInput(const char* text);
    // Enable and/or disable text input. Internally, this counts the number of enables, and only
    // disables text input if there is no UI element left that requests it.
    void EnableTextInput();
    void DisableTextInput();

    cInterfaceWindow* GetWindow(bool allow_model=true);
    void Draw();

    CGraphics& GetGraphics();
    const cTheme& GetTheme() const;

    IBuilding* GetActiveBuilding() const;
    void SetActiveBuilding(IBuilding*);

    // input handling
    void InputInteger(std::function<void(int)> callback, int def_value);
    void InputConfirm(std::function<void()> callback);
    void InputString(std::function<void(const std::string&)> callback, std::string def_value = {});
    void InputChoice(std::string question, std::vector<std::string> options, std::function<void(int)> callback);

    // message box
    void PushMessage(std::string text, int color, std::function<void()> cb = {});
    void PushError(std::string text);

    std::shared_ptr<cModalWindow> GetModalWindow();

    // girl selection
    std::shared_ptr<sGirl> GetActiveGirl() const;
    void AddToCycleList(std::shared_ptr<sGirl> girl);
    void ResetCycleList();
    void CycleGirlsForward();
    void CycleGirlsBackward();
    bool RemoveActiveGirlFromCycle();

    // key state functions
    bool IsCtrlHeld() const;
    bool IsShiftHeld() const;
private:
    // normal windows
    std::vector<std::shared_ptr<cInterfaceWindow>> m_WindowStack;

    // map of known window names
    std::map<std::string, std::shared_ptr<cInterfaceWindow>> windows;

    // the currently selected building
    IBuilding* m_ActiveBuilding = nullptr;

    // the active girl list.
    std::vector<std::shared_ptr<sGirl>> m_GirlCycleList;
    int m_SelectedGirlIndex;

    // pointer to the graphics engine
    CGraphics* m_GFX;

    // The theme folder. This is where all xml screens will be loaded from
    std::unique_ptr<cTheme> m_Theme;

    // counter to allow recursive enabling/disabling of text input
    int m_TextInputEnabled = 0;

    // keep track of the state of control keys
    bool m_IsCtrlHeld = false;
    bool m_IsShiftHeld = false;
};


#endif
