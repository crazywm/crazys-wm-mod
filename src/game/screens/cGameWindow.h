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

#ifndef WM_CGAMEWINDOW_H
#define WM_CGAMEWINDOW_H

#include "interface/cInterfaceWindowXML.h"

class IBuilding;
class sGirl;

class cGameWindow : public cInterfaceWindowXML {
public:
    using cInterfaceWindowXML::cInterfaceWindowXML;

    /// This function should be overwriden by screens that expect scripts to change an image.
    /// This is not a perfect solution, but will do for now.
    virtual void UpdateImage(int imagetype) {};
    void PrepareImage(int girlimage_id, sGirl* selected_girl, int imagetype, bool rand = true, int ImageNum = -1, bool gallery = false, std::string ImageName = "");
protected:
    // active building
    IBuilding& active_building() const;
    void set_active_building(IBuilding* building);
    IBuilding& cycle_building(int direction);

    // active girls
    sGirl& active_girl() const;
    std::shared_ptr<sGirl> selected_girl() const;
    void set_active_girl(std::shared_ptr<sGirl> girl);

    void reset_cycle_list();
    void cycle_girl_forward();
    void cycle_girl_backward();
    bool cycle_to(const sGirl* target);
    bool remove_from_cycle();

    void add_to_cycle_list(std::shared_ptr<sGirl> girl);
private:

};

#endif //WM_CGAMEWINDOW_H
