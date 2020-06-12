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

class cScreenGallery : public cGameWindow
{
private:
	int		back_id;		// Back button
	int		next_id;		// next button
	int		prev_id;		// prev button
	int		image_id;		// the image
	int		imagename_id;	// image name
	int		imagelist_id;	// image type list

	void set_ids() override;

	sGirl* m_SelectedGirl;

    void OnKeyPress(SDL_Keysym keysym) override;
public:
	cScreenGallery();

	void init(bool back) override;
	void process() override {};

    void change_image();
};