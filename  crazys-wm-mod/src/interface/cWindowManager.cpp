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
#include <algorithm>
#include "src/screens/cScreenGetInput.h"
#include "cWindowManager.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cChoiceMessage.h"
#include <cassert>
#include <utility>

extern cScreenGetInput*	g_GetInput;
extern CGraphics g_Graphics;


cWindowManager::cWindowManager() = default;
cWindowManager::~cWindowManager() = default;

void cWindowManager::push(const std::string& window_name)
{
	if (windows.find(window_name) == windows.end())						//check the screen exists
	{
		g_LogFile.ss() << "cWindowManager::Push: can't find window named '" << window_name << "'" ; g_LogFile.ssend();
		return;
	}

    m_WindowStack.push_back(windows[window_name]);
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
    if(!m_WindowStack.empty())
        m_WindowStack.back()->init(true);
}

void cWindowManager::PopToWindow(const std::string& window_name)
{
    if (windows.find(window_name) == windows.end())						//check the screen exists
    {
        g_LogFile.ss() << "cWindowManager::Push: can't find window named '" << window_name << "'" ; g_LogFile.ssend();
        return;
    }

    while (!m_WindowStack.empty() && m_WindowStack.back() != windows[window_name]) {
        m_WindowStack.pop_back();
    }

    if(m_WindowStack.empty()) {
        m_WindowStack.push_back(windows[window_name]);
    }

    m_WindowStack.back()->init(true);
}

void cWindowManager::UpdateCurrent()
{
    if (m_MessageBox->IsActive()) {
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

void cWindowManager::OnMouseClick(int x, int y, bool down)
{
    if (m_MessageBox->IsActive()) {
        if(!down) m_MessageBox->Advance();
    } else if(!m_WindowStack.empty())
        m_WindowStack.back()->MouseClick(x, y, down);
}

void cWindowManager::OnMouseWheel(int x, int y, bool mouseWheelDown)
{
    if (m_MessageBox->IsActive())
        m_MessageBox->Advance();
    else if(!m_WindowStack.empty())
        m_WindowStack.back()->MouseWheel(x, y, mouseWheelDown);
}

cInterfaceWindow* cWindowManager::GetWindow()
{
    if(m_WindowStack.empty())
        return nullptr;
	return m_WindowStack.back().get();
}

void cWindowManager::Draw()
{
    // figure out which windows to draw
    if(!m_WindowStack.empty()) {
        auto found = std::find_if_not(m_WindowStack.rbegin(), m_WindowStack.rend(),
                                      [](auto &w) { return w->IsTransparent(); });
        if(found == m_WindowStack.rend())
            --found;

        while(true) {
            (*found)->Draw(g_Graphics);
            if(found == m_WindowStack.rbegin())
                break;
            --found;
        }
    }

    if (m_MessageBox->IsActive()) {
        m_MessageBox->Draw(g_Graphics);
    }
}

void cWindowManager::FreeAllWindows()
{
    windows.clear();
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

void cWindowManager::add_window(string name, shared_ptr<cInterfaceWindow> win)
{
    win->load(this);
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
    return m_MessageBox->IsActive() || dynamic_cast<cModalWindow*>(m_WindowStack.back().get()) != nullptr;
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

void cWindowManager::InputChoice(std::string question, std::vector<std::string> options, std::function<void(int)> callback)
{
    auto choice_window = std::make_shared<cModalWindow>();
    int length = std::max_element(begin(options), end(options),
                                  [](const std::string& a, const std::string& b){ return a.size() < b.size(); })->size();
    auto cb = std::make_unique<cChoice>(224, 112, 352, 384, 0, options.size(), 32, length, 16, choice_window.get());
    cb->Question(std::move(question));
    for(std::size_t i = 0; i < options.size(); ++i) {
        cb->AddChoice(std::move(options[i]), i);
    }
    cb->SetCallback( std::move(callback) );
    choice_window->AddWidget(std::move(cb));
    choice_window->load(this);
    m_WindowStack.push_back(std::move(choice_window));
    try {
        m_WindowStack.back()->init(false);
    } catch(...) {
        m_WindowStack.pop_back();
        throw;
    }
}

void cWindowManager::PopAll() {
    m_WindowStack.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<cWindowManager> WindowManager;

cWindowManager& window_manager()
{
    assert(WindowManager);
    return *WindowManager;
}

void InitInterface() {
    assert(!WindowManager);
    WindowManager = std::make_unique<cWindowManager>();
}

void ShutdownInterface()
{
    g_LogFile.write("Releasing Interface");
    WindowManager.reset();
}

CGraphics& cInterfaceWindow::GetGraphics() {
    return g_Graphics;
}

// TODO this does not belong here
CGraphics& cUIWidget::GetGraphics()
{
    return m_Parent->GetGraphics();
}
