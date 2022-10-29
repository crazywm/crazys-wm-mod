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

#include <utils/algorithms.hpp>
#include "cInventory.h"
#include "cGameWindow.h"
#include "interface/cWindowManager.h"
#include "IGame.h"
#include "buildings/cBuildingManager.h"
#include "CLog.h"
#include "widgets/cImageItem.h"
#include "interface/CGraphics.h"
#include "utils/DirPath.h"
#include "utils/FileList.h"
#include "utils/string.hpp"
#include "sConfig.h"
#include "images/cImageLookup.h"

extern cConfig cfg;


IBuilding& cGameWindow::active_building() const {
    auto bld = window_manager().GetActiveBuilding();
    if(bld)
        return *bld;
    throw std::logic_error("No active building");
}

void cGameWindow::set_active_building(IBuilding* building)
{
    window_manager().SetActiveBuilding(building);
}

IBuilding& cGameWindow::cycle_building(int direction)
{
    auto& buildings = g_Game->buildings();
    int current = buildings.find(&active_building());
    int next = (current + direction) % buildings.num_buildings();
    if(next < 0) next = buildings.num_buildings() - 1;
    auto& active = buildings.get_building(next);
    set_active_building(&active);
    return active;
}

sGirl& cGameWindow::active_girl() const
{
    auto girl = window_manager().GetActiveGirl();
    if(girl)
        return *girl;
    throw std::logic_error("No girl selected");
}

void cGameWindow::set_active_girl(std::shared_ptr<sGirl> girl)
{
    window_manager().ResetCycleList();
    if(girl)
        window_manager().AddToCycleList(std::move(girl));
}

std::shared_ptr<sGirl> cGameWindow::selected_girl() const
{
    return window_manager().GetActiveGirl();
}

void cGameWindow::add_to_cycle_list(std::shared_ptr<sGirl> girl) {
    window_manager().AddToCycleList(std::move(girl));
}

void cGameWindow::cycle_girl_forward() {
    window_manager().CycleGirlsForward();
}

void cGameWindow::cycle_girl_backward() {
    window_manager().CycleGirlsBackward();
}

bool cGameWindow::remove_from_cycle() {
    return window_manager().RemoveActiveGirlFromCycle();
}

void cGameWindow::reset_cycle_list() {
    window_manager().ResetCycleList();
}

bool cGameWindow::cycle_to(const sGirl* target) {
    auto girl = window_manager().GetActiveGirl().get();
    if(!girl)
        return false;

    do {
        if(window_manager().GetActiveGirl().get() == target) {
            return true;
        }
        cycle_girl_forward();
    } while(window_manager().GetActiveGirl().get() != girl);
    return false;
}

void cGameWindow::PrepareImage(int id, const sGirl& girl, const sImageSpec& spec, std::uint64_t seed)
{
    if(seed == 0) {
        seed = g_Game->get_weeks_played() + 1000 + girl.GetID();
    }
    auto found = g_Game->image_lookup().find_image(girl.GetImageFolder().str(), spec, seed);
    PrepareImage(id, found);
}

void cGameWindow::PrepareImage(int id, const sGirl& girl, sImagePreset img_type, std::uint64_t seed) {
    auto spec = girl.MakeImageSpec(img_type);
    PrepareImage(id, girl, spec, seed);
}


void cGameWindow::PrepareImage(int id, const std::string& file_name) {
    cImageItem* image = GetImage(id);
    if(!image) return;

    std::string ext = tolower(file_name.substr(file_name.find_last_of('.') + 1));
    // this is the list of supported formats found on SDL_image's website
    // BMP, PNM (PPM/PGM/PBM), XPM, LBM, PCX, GIF, JPEG, PNG, TGA, and TIFF
    if(is_in(ext, {"jpg", "jpeg", "png", "bmp", "tga", "tiff"})) {
        if(image->SetImage(file_name, true)) {
            return;
        }
    }
    image->SetAnimation(file_name);
}

void cGameWindow::UpdateImage(sImagePreset imagetype, std::uint64_t seed) {
    UpdateImage(get_image_girl()->MakeImageSpec(imagetype), seed);
}

void cGameWindow::UpdateImage(const sImageSpec& spec, std::uint64_t seed) {
    if(get_image_girl()) {
        PrepareImage(m_MainImageId, *get_image_girl(), spec, seed);
        HideWidget(m_MainImageId, false);
    } else {
        HideWidget(m_MainImageId, true);
    }
}

void cGameWindow::UpdateImage(const std::string& file_name) {
    PrepareImage(m_MainImageId, file_name);
}
