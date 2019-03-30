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
#include "src/screens/cScreenGetInput.h"
#include "cWindowManager.h"
#include "CLog.h"
#include "cMessageBox.h"

extern cScreenGetInput*	g_GetInput;
extern CGraphics g_Graphics;


cWindowManager::cWindowManager()
{

};

cWindowManager::~cWindowManager() = default;

void cWindowManager::push(const std::string& window_name)
{
	if (windows.find(window_name) == windows.end())						//check the screen exists
	{
		g_LogFile.ss() << "cWindowManager::Push: can't find window named '" << window_name << "'" ; g_LogFile.ssend();
		return;
	}

    m_WindowStack.push_back(windows[window_name].get());
	try {
        m_WindowStack.back()->init(false);
    } catch(...) {
        m_WindowStack.pop_back();
        throw;
	}
}

void cWindowManager::replace(const std::string& window_name)
{
    auto current = m_WindowStack.back();
    m_WindowStack.pop_back();
    try {
        push(window_name);
    } catch(...) {
        // in case of exception, roll back the pop
        m_WindowStack.push_back(current);
        throw;
    }
}


// remove function from the stack
void cWindowManager::Pop()
{
    m_WindowStack.pop_back();
    if(m_WindowStack.empty())
        m_WindowStack.back()->init(true);
}

void cWindowManager::PopToWindow(cInterfaceWindow* Interface)
{
    while (!m_WindowStack.empty() && m_WindowStack.back() != Interface) {
        m_WindowStack.pop_back();
    }

    if(m_WindowStack.empty()) {
        m_WindowStack.push_back(Interface);
    }

    m_WindowStack.back()->init(true);
}

void cWindowManager::PopToWindow(const std::string& window_name)
{
    if (windows.find(window_name) == windows.end())						//check the screen exists
    {
        g_LogFile.ss() << "cWindowManager::Push: can't find window named '" << window_name << "'" ; g_LogFile.ssend();
        return;
    }
    PopToWindow(windows[window_name].get());
}

void cWindowManager::UpdateCurrent()
{
    // Draw Any message boxes
    if (m_MessageBox->IsActive()) {
        m_MessageBox->Draw();
    } else if (!m_WindowStack.empty()) {
        m_WindowStack.back()->update();
    }
}

void cWindowManager::UpdateMouseMovement(int x, int y)
{
    if (m_MessageBox->IsActive())
        return;
    if(!m_WindowStack.empty())
        m_WindowStack.back()->UpdateWindow(x, y);
}

void cWindowManager::UpdateMouseDown(int x, int y)
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->MouseDown(x, y);
}

void cWindowManager::UpdateMouseClick(int x, int y, bool mouseWheelDown, bool mouseWheelUp)
{
    if (m_MessageBox->IsActive())
        m_MessageBox->Advance();
    else if(!m_WindowStack.empty())
        m_WindowStack.back()->Click(x, y, mouseWheelDown, mouseWheelUp);
}

void cWindowManager::UpdateKeyInput(char key, bool upper)
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->UpdateEditBoxes(key, upper);
}

bool cWindowManager::HasEditBox()
{
    if(m_WindowStack.empty())
        return false;
	return m_WindowStack.back()->HasEditBox();
}

cInterfaceWindow* cWindowManager::GetWindow()
{
    if(m_WindowStack.empty())
        return nullptr;
	return m_WindowStack.back();
}

void cWindowManager::Draw()
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->Draw(g_Graphics);

    if (m_MessageBox->IsActive()) {
        m_MessageBox->Draw();
    }
}

void cWindowManager::FreeAllWindows()
{
    for(auto& w : windows) {
        w.second->Free();
    }
}

void cWindowManager::ResetAllWindows()
{
    for(auto& w : windows) {
        w.second->Reset();
    }
}

void cWindowManager::OnKeyPress(SDL_keysym key)
{
    if (m_MessageBox->IsActive()) {
        m_MessageBox->Advance();
    } else {
        GetWindow()->OnKeyPress(key);
    }
}

void cWindowManager::add_window(string name, std::unique_ptr<cInterfaceWindow> win)
{
    windows[std::move(name)] = std::move(win);
}

IBuilding * cWindowManager::GetActiveBuilding() const
{
    return m_ActiveBuilding;
}

void cWindowManager::SetActiveBuilding(IBuilding * building)
{
    m_ActiveBuilding = building;
}

void cWindowManager::InputInteger(std::function<void(int)> callback)
{
    g_GetInput->ModeGetInt(std::move(callback));
    push("GetInput");
}

void cWindowManager::InputConfirm(std::function<void()> callback)
{
    g_GetInput->ModeConfirm(std::move(callback));
    push("GetInput");
}

void cWindowManager::InputString(std::function<void(const std::string&)> callback)
{
    g_GetInput->ModeGetString(std::move(callback));
    push("GetInput");
}

void cWindowManager::PushMessage(std::string text, int color)
{
    m_MessageBox->PushMessage(std::move(text), color);
}

bool cWindowManager::HasActiveModal() const
{
    return m_MessageBox->IsActive();
}

void cWindowManager::load()
{
    m_MessageBox = std::make_unique<cMessageBox>();
}

sGirl * cWindowManager::GetActiveGirl() const
{
    if(m_SelectedGirls.empty())
        return nullptr;
    return m_SelectedGirls.front();
}

void cWindowManager::SetActiveGirl(sGirl * girl)
{
    m_SelectedGirls.clear();
    m_SelectedGirls.push_back(girl);
}
