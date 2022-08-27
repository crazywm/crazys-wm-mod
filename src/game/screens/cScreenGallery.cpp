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
#include "cScreenGallery.h"
#include "interface/cWindowManager.h"
#include "widgets/cImageItem.h"
#include "widgets/cEditBox.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "utils/FileList.h"
#include "character/sGirl.h"
#include "IGame.h"
#include <sstream>
#include <future>
#include "images/cImageLookup.h"
#include "utils/algorithms.hpp"
#include "utils/string.hpp"
#include "CLog.h"
#include "character/predicates.h"

namespace  {
    constexpr const EnumRange<EImageBaseType, EImageBaseType(0), EImageBaseType::NUM_TYPES> ImageTypeRange = {};
}

cScreenGallery::cScreenGallery() : cGameWindow("gallery_screen.xml")
{
}


void cScreenGallery::set_ids()
{
    prev_id         = get_id("PrevButton","Prev");
    next_id         = get_id("NextButton","Next");
    image_id        = get_id("GirlImage");
    imagename_id    = get_id("ImageName");
    imagelist_id    = get_id("ImageList");
    pregnant_id     = get_id("Pregnant");
    futa_id         = get_id("Futa");
    tied_id         = get_id("TiedUp");
    participants_id = get_id("Participants");
    source_id       = get_id("ImageSource");
    info_id         = get_id("ImageData");
    SetEditBoxText(participants_id, "Any");

    std::vector<std::string> parts(begin(get_participant_names()), end(get_participant_names()));
    GetEditBox(participants_id)->SetOptions(std::move(parts));

    std::vector<std::string> ILColumns{ "ILName", "ILTotal" };
    SortColumns(imagelist_id, ILColumns);

    SetCheckBoxCallback(pregnant_id, [this](bool){ determine_images(); });
    SetCheckBoxCallback(futa_id, [this](bool){ determine_images(); });
    SetCheckBoxCallback(tied_id, [this](bool){ determine_images(); });

    SetButtonCallback(prev_id, [this]() {
        m_CurrentImageID--;
        if (m_CurrentImageID < 0) m_CurrentImageID = m_ImageFiles[(int)m_CurrentType].size() - 1;
        update_image();
    });
    SetButtonHotKey(prev_id, SDLK_LEFT);

    SetButtonCallback(next_id, [this](){
        m_CurrentImageID++;
        if (m_CurrentImageID == m_ImageFiles[(int)m_CurrentType].size()) m_CurrentImageID = 0;
        update_image();
    });
    SetButtonHotKey(next_id, SDLK_RIGHT);

    SetListBoxSelectionCallback(imagelist_id, [this](int selection) {
        if(selection >= 0) {
            m_CurrentType = (EImageBaseType)selection;
        }
        if (m_CurrentImageID >= m_ImageFiles[(int)m_CurrentType].size()) m_CurrentImageID = 0;
        update_image();
    });

    SetListBoxHotKeys(imagelist_id, SDLK_UP, SDLK_DOWN);

    SetEditBoxCallback(participants_id, [this](const std::string& value){
        try {
            auto id = get_participant_id(value);
        } catch (const std::out_of_range&) {
            SetEditBoxText(participants_id, "Any");
        }
        determine_images();
    });
}

void cScreenGallery::update_image()
{
    cImageItem* image_ui = GetImage(image_id);
    if(m_CurrentImageID >= m_ImageFiles[(int) m_CurrentType].size()) {
        image_ui->SetHidden(true);
        EditTextItem("", imagename_id);
        EditTextItem("", source_id);
        return;
    }

    auto found = m_ImageFiles[(int) m_CurrentType].at(m_CurrentImageID);
    std::string ext = tolower(found.first.FileName.substr(found.first.FileName.find_last_of('.') + 1));

    // this is the list of supported formats found on SDL_image's website
    // BMP, PNM (PPM/PGM/PBM), XPM, LBM, PCX, GIF, JPEG, PNG, TGA, and TIFF

    DirPath target = m_SelectedGirl->GetImageFolder();
    auto& info = found.first;
    std::stringstream info_txt;
    info_txt << "Cost: " << found.second << "\n";
    info_txt << "Type: " << get_image_name(info.Attributes.BasicImage) << "\n";
    info_txt << "Part:   " << get_participant_name(info.Attributes.Participants) << "\n";
    auto print_flag = [](std::ostream& target, ETriValue v, const char* val){
        switch (v) {
            case ETriValue::Yes:
                target << '[' << val << "] ";
                return;
            case ETriValue::Maybe:
                target << "[?" << val << "] ";
                return;
            case ETriValue::No:
                return;
        }
    };
    info_txt << "Flags:  ";
    print_flag(info_txt, info.Attributes.IsPregnant, "preg");
    print_flag(info_txt, info.Attributes.IsFuta, "futa");
    print_flag(info_txt, info.Attributes.IsTied, "tied");

    EditTextItem("Source: " + info.Source, source_id);
    EditTextItem(info_txt.str(), info_id);
    std::string file_name = (target << found.first.FileName).str();

    if(is_in(ext, {"jpg", "jpeg", "png", "bmp", "tga", "tiff"})) {
        if(!image_ui->SetImage(file_name, true)) {
            image_ui->SetAnimation(file_name);
        }
    } else {
        image_ui->SetAnimation(file_name);
    }

    image_ui->SetHidden(false);
    if (image_ui->m_Image)
        EditTextItem(image_ui->m_Image.GetFileName(), imagename_id);
    else if(image_ui->m_AnimatedImage)
        EditTextItem(image_ui->m_AnimatedImage.GetFileName(), imagename_id);

    SetSelectedItemInList(imagelist_id, (int)m_CurrentType, false);
}

void cScreenGallery::init(bool back)
{
    m_SelectedGirl = &active_girl();

    if(!back) {
        Focused();
        SetCheckBox(pregnant_id, m_SelectedGirl->is_pregnant());
        SetCheckBox(futa_id, is_futa(*m_SelectedGirl));
    }

    determine_images();
}

void cScreenGallery::determine_images() {
    cImageItem* image_ui = GetImage(image_id);
    image_ui->SetImage("");
    m_ImageFiles.resize((int)EImageBaseType::NUM_TYPES + 1);
    sImageSpec spec {
        EImageBaseType::NUM_TYPES,
        get_participant_id(GetEditBoxText(participants_id)),
        IsCheckboxOn(pregnant_id) ? ETriValue::Yes : ETriValue::No,
        IsCheckboxOn(futa_id) ? ETriValue::Yes : ETriValue::No,
        IsCheckboxOn(tied_id) ? ETriValue::Yes : ETriValue::No
    };

    auto run_update_in_bg = [this, spec]() mutable {
        {
            std::lock_guard<std::mutex> lck(m_UpdateMutex);
            m_ScheduledUpdates.emplace_back([this]() { ClearListBox(imagelist_id); });
        }
        auto& all_files = g_Game->image_lookup().lookup_files(m_SelectedGirl->GetImageFolder().str());
        std::set<const sImageRecord*> unmatched;
        for(auto& img : all_files) {
            unmatched.insert(&img);
        }

        for (auto img: ImageTypeRange) {
            spec.BasicImage = img;
            auto result = g_Game->image_lookup().find_images(m_SelectedGirl->GetImageFolder().str(), spec,
                                                        1000);
            std::sort(begin(result), end(result), [](const auto& a, const auto& b){ return a.second < b.second; });
            for(auto& tgt: result) {
                unmatched.erase(tgt.first);
            }
            if (!result.empty()) {
                std::vector<FormattedCellData> dataP{mk_text(g_Game->image_lookup().get_display_name(img)), mk_num(result.size())};
                std::lock_guard<std::mutex> lck(m_UpdateMutex);
                m_ScheduledUpdates.emplace_back([this, img, data = std::move(dataP), res=std::move(result)]() {
                    AddToListBox(imagelist_id, (int) img, std::move(data));
                    m_ImageFiles[(int) img].clear();
                    std::transform(begin(res), end(res), std::back_inserter(m_ImageFiles[(int) img]), [](auto&& a){
                        return std::make_pair(*a.first, a.second);
                    });
                });
            } else {
                g_LogFile.info("gallery", "No matching images for ", get_image_name(img));
            }
            if(m_Cancel) {
                return;
            }
        }

        // update the entry for unmatched images
        m_ImageFiles[(int) EImageBaseType::NUM_TYPES].clear();
        if(!unmatched.empty()) {
            std::vector<FormattedCellData> dataP{mk_text("<UNMATCHED>"), mk_num(unmatched.size())};
            std::for_each(begin(unmatched), end(unmatched),
                          [&](const auto& match){
                              m_ImageFiles[(int) EImageBaseType::NUM_TYPES].push_back({*match, 0});
            });

            std::lock_guard<std::mutex> lck(m_UpdateMutex);
            m_ScheduledUpdates.emplace_back([this, data = std::move(dataP)]() {
                AddToListBox(imagelist_id, (int) EImageBaseType::NUM_TYPES, std::move(data));
            });
        }
    };

    // if we have stuff currently running, cancel that
    if(m_AsyncLoad.joinable()) {
        m_Cancel = true;
        m_AsyncLoad.join();
    }
    m_Cancel = false;
    m_AsyncLoad = std::thread(run_update_in_bg);

    m_CurrentImageID = 0;
    m_CurrentType = EImageBaseType::PROFILE;
}

void cScreenGallery::process() {
    std::lock_guard<std::mutex> lck(m_UpdateMutex);
    while (!m_ScheduledUpdates.empty()) {
        m_ScheduledUpdates.front()();
        m_ScheduledUpdates.pop_front();
    }
}

cScreenGallery::~cScreenGallery() {
    m_Cancel = true;
    if(m_AsyncLoad.joinable()) {
        m_AsyncLoad.join();
    }
}
