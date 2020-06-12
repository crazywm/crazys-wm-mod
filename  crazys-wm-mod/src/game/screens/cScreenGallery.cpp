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
#include "cScreenGallery.h"
#include "interface/cWindowManager.h"
#include "widgets/cImageItem.h"
#include "interface/cSurface.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "FileList.h"
#include "character/sGirl.hpp"
#include "sConfig.h"
#include <sstream>

extern	string					pic_types[];
extern	string					galtxt[];
extern  cConfig                 cfg;

extern	string	numeric;

static int Mode = 0;
static int Img = 0;	// what image currently drawing
static int numimages[NUM_IMGTYPES][4];

cScreenGallery::cScreenGallery() : cGameWindow("gallery_screen.xml")
{
}

void cScreenGallery::set_ids()
{
	back_id			/**/ = get_id("BackButton", "Back");
	prev_id			/**/ = get_id("PrevButton","Prev");
	next_id			/**/ = get_id("NextButton","Next");
	image_id		/**/ = get_id("GirlImage");
	imagename_id	/**/ = get_id("ImageName");
	imagelist_id	/**/ = get_id("ImageList");

	std::vector<std::string> ILColumns{ "ILName", "ILTotal", "ILjpg", "ILAni", "ILGif" };
	SortColumns(imagelist_id, ILColumns);

	SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(prev_id, [this](){
        Img--;
        if (Img < 0) Img = numimages[Mode][0] - 1;
        change_image();
    });
    SetButtonHotKey(prev_id, SDLK_LEFT);

    SetButtonCallback(next_id, [this](){
        Img++;
        if (Img == numimages[Mode][0]) Img = 0;
        change_image();
    });
    SetButtonHotKey(next_id, SDLK_RIGHT);

    SetListBoxSelectionCallback(imagelist_id, [this](int selection) {
        Mode = selection;
        if (Img > numimages[Mode][0]) Img = 0;
        change_image();
    });
}

void cScreenGallery::change_image()
{
    PrepareImage(image_id, m_SelectedGirl, Mode, false, Img, true);
    string t = GetImage(image_id)->m_Message;
    if (t.empty()) t = GetImage(image_id)->m_Image.GetFileName();
    EditTextItem(t, imagename_id);
    SetSelectedItemInList(imagelist_id, Mode, false);
}

void cScreenGallery::init(bool back)
{
    m_SelectedGirl = &active_girl();

	Focused();
	ClearListBox(imagelist_id);

	int usefolder = 0;	// 0=none, 1=cfg, 2=original
	int startmode = -1;
	// start with what the config has set
	DirPath imagedir = DirPath(cfg.folders.characters().c_str()) << m_SelectedGirl->m_Name;
	FileList testall(imagedir, "*.*");
	if (testall.size() > 0)		usefolder = 1;
	else		// if config is not found, check for images in the original folder
	{
		imagedir = DirPath() << "Resources" << "Characters" << m_SelectedGirl->m_Name;
		FileList testall(imagedir, "*.*");
		if (testall.size() > 0) usefolder = 2;
	}
	if (usefolder == 0)
	{
	    throw std::runtime_error("ERROR: " + m_SelectedGirl->FullName() + " ( " + m_SelectedGirl->m_Name + " ) has no images.");
	}
	FileList readall(imagedir, "*.*");

    for (int i = 0; i < NUM_IMGTYPES; i++)
	{
		if (i == IMGTYPE_PREGNANT)
		{
			string ext[3] = { "*g", "ani", "gif" };
			for (u_int e = 0; e < 3; e++)
			{
				string t = ("preg." + ext[e]);
				FileList testmode(imagedir, t.c_str());
				for (unsigned j = 0; j < numeric.size(); j++)
				{
					t = ("preg" + numeric.substr(j, 1) + "*." + ext[e]);
					testmode.add(t.c_str());
				}
				numimages[i][e + 1] = testmode.size();
			}
		}
		else
		{
			FileList testmode1(imagedir, (pic_types[i] + "*g").c_str());	numimages[i][1] = testmode1.size();
			FileList testmode2(imagedir, (pic_types[i] + "ani").c_str());	numimages[i][2] = testmode2.size();
			FileList testmode3(imagedir, (pic_types[i] + "gif").c_str());	numimages[i][3] = testmode3.size();
		}
		numimages[i][0] = numimages[i][1] + numimages[i][2] + numimages[i][3];
		if (numimages[i][0] > 0)
		{
			if (startmode == -1) startmode = i;
			stringstream num0;	num0 << numimages[i][0];
			stringstream num1;	num1 << numimages[i][1];
			stringstream num2;	num2 << numimages[i][2];
			stringstream num3;	num3 << numimages[i][3];

			std::vector<std::string> dataP{ galtxt[i], num0.str(), num1.str(), num2.str(), num3.str() };
			AddToListBox(imagelist_id, i, std::move(dataP));
		}
	}
	Mode = startmode;
	Img = 0;
	change_image();
}

void cScreenGallery::OnKeyPress(SDL_Keysym keysym)
{
    auto key = keysym.sym;
    bool up = key == SDLK_w || key == SDLK_UP;
    if (up || key == SDLK_s || key == SDLK_DOWN)
    {
        while (true)
        {
            Mode += (up ? -1 : 1);
            if (Mode < 0) Mode = NUM_IMGTYPES - 1;
            if (Mode >= NUM_IMGTYPES) Mode = 0;
            Img = 0;
            if (numimages[Mode][0] > 0)
            {
                change_image();
                return;
            }
        }
    }
}
