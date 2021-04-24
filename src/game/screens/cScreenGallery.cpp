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
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "utils/FileList.h"
#include "character/sGirl.h"
#include <sstream>

extern    std::string                    pic_types[];
extern    std::string                    galtxt[];

extern    std::string    numeric;

static int Mode = 0;
static int Img = 0;    // what image currently drawing
static int numimages[NUM_IMGTYPES];

cScreenGallery::cScreenGallery() : cGameWindow("gallery_screen.xml")
{
}

void cScreenGallery::set_ids()
{
    prev_id        = get_id("PrevButton","Prev");
    next_id        = get_id("NextButton","Next");
    image_id       = get_id("GirlImage");
    imagename_id   = get_id("ImageName");
    imagelist_id   = get_id("ImageList");

    std::vector<std::string> ILColumns{ "ILName", "ILTotal" };
    SortColumns(imagelist_id, ILColumns);

    SetButtonCallback(prev_id, [this]() {
        Img--;
        if (Img < 0) Img = numimages[Mode] - 1;
        change_image(Mode);
    });
    SetButtonHotKey(prev_id, SDLK_LEFT);

    SetButtonCallback(next_id, [this](){
        Img++;
        if (Img == numimages[Mode]) Img = 0;
        change_image(Mode);
    });
    SetButtonHotKey(next_id, SDLK_RIGHT);

    SetListBoxSelectionCallback(imagelist_id, [this](int selection) {
        if(selection >= 0) {
            Mode = selection;
        }
        if (Img > numimages[Mode]) Img = 0;
        change_image(Mode);
    });

    SetListBoxHotKeys(imagelist_id, SDLK_UP, SDLK_DOWN);
}

void cScreenGallery::change_image(int mode)
{
    PrepareImage(image_id, m_SelectedGirl, mode, false, Img, true);
    cImageItem* image_ui = GetImage(image_id);
    if (image_ui->m_Image)
        EditTextItem(image_ui->m_Image.GetFileName(), imagename_id);
    else if(image_ui->m_AnimatedImage)
        EditTextItem(image_ui->m_AnimatedImage.GetFileName(), imagename_id);

    SetSelectedItemInList(imagelist_id, mode, false);
}

void cScreenGallery::init(bool back)
{
    m_SelectedGirl = &active_girl();

    Focused();
    ClearListBox(imagelist_id);

    int startmode = -1;
    // start with what the config has set
    DirPath imagedir = m_SelectedGirl->GetImageFolder();
    FileList testall(imagedir, "*.*");
    if (testall.size() == 0)
    {
        pop_window();
        push_message(m_SelectedGirl->FullName() + " ( " + m_SelectedGirl->m_Name + " ) has no images.", COLOR_RED);
    }
    FileList readall(imagedir, "*.*");

    for (int i = 0; i < NUM_IMGTYPES; i++)
    {
        FileList testmode1(imagedir, (pic_types[i] + "*").c_str());    numimages[i] = testmode1.size();
        if (numimages[i] > 0)
        {
            if (startmode == -1) startmode = i;
            std::vector<FormattedCellData> dataP{ mk_text(galtxt[i]), mk_num(numimages[i]) };
            AddToListBox(imagelist_id, i, std::move(dataP));
        }
    }
    Mode = startmode;
    Img = 0;
    if(Mode != -1)
        change_image(Mode);
}
