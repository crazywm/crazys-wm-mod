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
#include "character/traits/ITraitsCollection.h"

class cScreenGirlDetails : public cGameWindow
{
private:
    int girlname_id;        // Girl Name text
    int girldesc_id;        // Girl Description text
    int girlimage_id;        // Girl image
    int antipreg_id;        // Use Anti-preg checkbox
    int senddungeon_id;        // Send To Dungeon button
    int reldungeon_id;        // Release From Dungeon button
    int interact_id;        // Interact button
    int interactc_id;        // Interact Count
    int takegold_id;        // Take Gold button
    int accom_id;            // Accom slider
    int accomval_id;        // Accom value text
    int houseperc_id;        // House Percentage slider
    int housepercval_id;    // House Percentage value text
    int jobtypelist_id;        // Job Types listbox
    int joblist_id;            // Job listbox
    int day_id;                // Day button
    int night_id;            // Night button
    int traitlist_id;        // Trait listbox
    int traitdesc_id;        // Trait Description text

    void set_ids() override;
    void RefreshJobList();
    void PrevGirl();
    void NextGirl();

    void take_gold(sGirl& girl);
    void set_shift(int shift);

    std::shared_ptr<sGirl> m_SelectedGirl = nullptr;

    void OnKeyPress(SDL_Keysym keysym) override;

    bool m_Refresh = false;
    bool m_EditNightShift = false;

    std::vector<sTraitInfo> m_TraitInfoCache;
public:
    cScreenGirlDetails();

    void init(bool back) override;
    void process() override;

    void do_interaction();

    void send_to_dungeon();

    void release_from_dungeon();

    void on_select_job(int selection, bool fulltime);

    void set_house_percentage(int value);
    void set_accomodation(int value);

    void UpdateImage(int imagetype) override;
};
