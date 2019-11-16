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
#include "cScreenSettings.h"
#include "FileList.h"
#include "sConfig.h"
#include "CLog.h"

extern cConfig cfg;

void cScreenSettings::set_ids()
{
	back_id					 = get_id("BackButton", "Back");
	ok_id					 = get_id("OkButton");
	revert_id				 = get_id("RevertButton");

	// Folders
	characters_id			 = get_id("CharactersFol");
	saves_id				 = get_id("SavesFol");
	defaultimages_id		 = get_id("DefaultImagesFol");
	items_id				 = get_id("ItemsFol");
	backupsaves_id			 = get_id("BackupSavesFol");
	preferdefault_id		 = get_id("PreferDefaultFol");

	// Catacombs
	uniquecatacombs_id		 = get_id("UniqueCatacombsCat");
	uniquecatacombslabel_id	 = get_id("UniqueCatacombsLabelCat");
	controlgirls_id			 = get_id("ControlGirlsCat");
	controlgangs_id			 = get_id("ControlGangsCat");
	girlgetsgirls_id		 = get_id("GirlGetsGirlsCat");
	girlgetsitems_id		 = get_id("GirlGetsItemsCat");
	girlgetsbeast_id		 = get_id("GirlGetsBeastCat");
	girlsgirlspercslider_id	 = get_id("GirlsGirlsPercSliderCat");
	girlsitemspercslider_id	 = get_id("GirlsItemsPercSliderCat");
	ganggetsgirls_id		 = get_id("GangGetsGirlsCat");
	ganggetsitems_id		 = get_id("GangGetsItemsCat");
	ganggetsbeast_id		 = get_id("GangGetsBeastCat");
	gangsgirlspercslider_id	 = get_id("GangsGirlsPercSliderCat");
	gangsitemspercslider_id	 = get_id("GangsItemsPercSliderCat");

	SetButtonNavigation(back_id, "Main Menu");
	SetButtonCallback(revert_id, [this]() { init(false); });
    SetButtonCallback(ok_id, [this]() {
        update_settings();
        init(false);
	});

    SetSliderCallback(girlsgirlspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(girlsitemspercslider_id);
        if (s2 < s1)
        {
            s2 = s1;
            SliderRange(girlsitemspercslider_id, 0, 100, s2, 1);
        }
        update_girl_sliders();
    });

    SetSliderCallback(girlsitemspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(girlsgirlspercslider_id);
        if (s1 < s2)
        {
            s2 = s1;
            SliderRange(girlsgirlspercslider_id, 0, 100, s2, 1);
        }
        update_girl_sliders();
    });

    SetSliderCallback(gangsgirlspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(gangsitemspercslider_id);
        if (s2 < s1)
        {
            s2 = s1;
            SliderRange(gangsitemspercslider_id, 0, 100, s2, 1);
        }
        update_gang_sliders();
    });

    SetSliderCallback(gangsitemspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(gangsgirlspercslider_id);
        if (s1 < s2)
        {
            s2 = s1;
            SliderRange(gangsgirlspercslider_id, 0, 100, s2, 1);
        }
        update_gang_sliders();
    });

    SetSliderCallback(uniquecatacombs_id, [this](int value) {
        std::stringstream ss;
        cfg.catacombs.unique_catacombs() = value;
        ss.str("");	ss << "Chance of Unique Girls from Catacombs: " << cfg.catacombs.unique_catacombs() << "%";
        EditTextItem(ss.str(), uniquecatacombslabel_id);
    });
}

cScreenSettings::cScreenSettings() : cInterfaceWindowXML("settings.xml")
{
}

void cScreenSettings::init(bool back)
{
    Focused();
    if (!back)
    {
        std::stringstream ss;
        SetEditBoxText(characters_id, cfg.folders.characters());
        SetEditBoxText(saves_id, cfg.folders.saves());
        SetEditBoxText(defaultimages_id, cfg.folders.defaultimageloc());
        SetEditBoxText(items_id, cfg.folders.items());
        SetCheckBox(backupsaves_id, cfg.folders.backupsaves());
        SetCheckBox(preferdefault_id, cfg.folders.preferdefault());

        ss.str("");	ss << "Chance of Unique Girls from Catacombs: "<< cfg.catacombs.unique_catacombs() <<"%";
        EditTextItem(ss.str(), uniquecatacombslabel_id);
        SliderValue(uniquecatacombs_id, cfg.catacombs.unique_catacombs());
        ss.str("");	ss << "Girls: " << cfg.catacombs.girl_gets_girls() << "%";	EditTextItem(ss.str(), girlgetsgirls_id);
        ss.str("");	ss << "Items: " << cfg.catacombs.girl_gets_items() << "%";	EditTextItem(ss.str(), girlgetsitems_id);
        ss.str("");	ss << "Beast: " << cfg.catacombs.girl_gets_beast() << "%";	EditTextItem(ss.str(), girlgetsbeast_id);
        SliderValue(girlsgirlspercslider_id, cfg.catacombs.girl_gets_girls());
        SliderValue(girlsitemspercslider_id, cfg.catacombs.girl_gets_girls() + cfg.catacombs.girl_gets_items());
        ss.str("");	ss << "Girls: " << cfg.catacombs.gang_gets_girls() << "%";	EditTextItem(ss.str(), ganggetsgirls_id);
        ss.str("");	ss << "Items: " << cfg.catacombs.gang_gets_items() << "%";	EditTextItem(ss.str(), ganggetsitems_id);
        ss.str("");	ss << "Beast: " << cfg.catacombs.gang_gets_beast() << "%";  EditTextItem(ss.str(), ganggetsbeast_id);
        SliderValue(gangsgirlspercslider_id, cfg.catacombs.gang_gets_girls());
        SliderValue(gangsitemspercslider_id, cfg.catacombs.gang_gets_girls() + cfg.catacombs.gang_gets_items());
    }
//	backupsaves_id
//	preferdefault_id


}

void cScreenSettings::update_gang_sliders()
{
    int s1 = SliderValue(gangsgirlspercslider_id);
    int s2 = SliderValue(gangsitemspercslider_id);
    cfg.catacombs.gang_gets_girls() = s1;
    cfg.catacombs.gang_gets_items() = s2 - s1;
    cfg.catacombs.gang_gets_beast() = 100 - s2;

    std::stringstream ss;
    ss << "Girls: " << cfg.catacombs.gang_gets_girls() << "%";
    EditTextItem(ss.str(), ganggetsgirls_id);
    ss.str("");
    ss << "Items: " << cfg.catacombs.gang_gets_items() << "%";
    EditTextItem(ss.str(), ganggetsitems_id);
    ss.str("");
    ss << "Beast: " << cfg.catacombs.gang_gets_beast() << "%";
    EditTextItem(ss.str(), ganggetsbeast_id);
}

void cScreenSettings::update_girl_sliders()
{
    std::stringstream ss;
    int s1 = SliderValue(girlsgirlspercslider_id);
    int s2 = SliderValue(girlsitemspercslider_id);
    cfg.catacombs.girl_gets_girls() = s1;
    cfg.catacombs.girl_gets_items() = s2 - s1;
    cfg.catacombs.girl_gets_beast() = 100 - s2;
    ss.str("");
    ss << "Girls: " << cfg.catacombs.girl_gets_girls() << "%";
    EditTextItem(ss.str(), girlgetsgirls_id);
    ss.str("");
    ss << "Items: " << cfg.catacombs.girl_gets_items() << "%";
    EditTextItem(ss.str(), girlgetsitems_id);
    ss.str("");
    ss << "Beast: " << cfg.catacombs.girl_gets_beast() << "%";
    EditTextItem(ss.str(), girlgetsbeast_id);
}

void cScreenSettings::update_settings()
{
    cfg.folders.characters()      = GetEditBoxText(characters_id);
    cfg.folders.saves()           = GetEditBoxText(saves_id);
    cfg.folders.defaultimageloc() = GetEditBoxText(defaultimages_id);
    cfg.folders.items()           = GetEditBoxText(items_id);
    cfg.folders.backupsaves()     = GetCheckBox(backupsaves_id);
    cfg.folders.preferdefault()   = GetCheckBox(preferdefault_id);
    
    cfg.catacombs.unique_catacombs() = SliderValue(uniquecatacombs_id);
    int s1 = SliderValue(girlsgirlspercslider_id);
    int s2 = SliderValue(girlsitemspercslider_id);
    int s3 = SliderValue(gangsgirlspercslider_id);
    int s4 = SliderValue(gangsitemspercslider_id);
    cfg.catacombs.girl_gets_girls() = s1;
    cfg.catacombs.girl_gets_items() = s2 - s1;
    cfg.catacombs.girl_gets_beast() = 100 - s2;
    cfg.catacombs.gang_gets_girls() = s3;
    cfg.catacombs.gang_gets_items() = s4 - s3;
    cfg.catacombs.gang_gets_beast() = 100 - s4;
}
