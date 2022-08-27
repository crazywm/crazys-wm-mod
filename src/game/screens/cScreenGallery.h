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
#pragma once

#include "cGameWindow.h"
#include "images/cImageList.h"
#include <thread>
#include <atomic>
#include <mutex>

class cScreenGallery : public cGameWindow
{
private:
    int next_id;           // next button
    int prev_id;           // prev button
    int image_id;          // the image
    int imagename_id;      // image name
    int imagelist_id;      // image type list
    int pregnant_id;       // pregnant check box
    int futa_id;           // futa check box
    int tied_id;           // tied-up check box
    int participants_id;   // participants input
    int source_id;         // source text field
    int info_id;           // info text field

    std::vector<std::vector<std::pair<sImageRecord, int>>> m_ImageFiles;
    int m_CurrentImageID = 0;
    EImageBaseType m_CurrentType = EImageBaseType::PROFILE;

    void set_ids() override;
    void determine_images();

    sGirl* m_SelectedGirl;

    std::mutex m_UpdateMutex;
    std::deque<std::function<void()>> m_ScheduledUpdates;
    std::thread m_AsyncLoad;
    std::atomic<bool> m_Cancel;
public:
    cScreenGallery();
    ~cScreenGallery();

    void init(bool back) override;
    void process() override;

    void update_image();
};