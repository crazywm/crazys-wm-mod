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
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"
#include "FileList.h"

extern cWindowManager g_WinManager;
extern bool g_InitWin;
extern int g_CurrentScreen;

extern bool g_EscapeKey;

static bool revert = true;
bool cScreenSettings::ids_set = false;

void cScreenSettings::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenSettings");

	back_id					/**/ = get_id("BackButton", "Back");
	ok_id					/**/ = get_id("OkButton");
	revert_id				/**/ = get_id("RevertButton");

	// Folders
	characters_id			/**/ = get_id("CharactersFol");
	saves_id				/**/ = get_id("SavesFol");
	defaultimages_id		/**/ = get_id("DefaultImagesFol");
	items_id				/**/ = get_id("ItemsFol");
	backupsaves_id			/**/ = get_id("BackupSavesFol");
	preferdefault_id		/**/ = get_id("PreferDefaultFol");

	// Catacombs
	uniquecatacombs_id		/**/ = get_id("UniqueCatacombsCat");
	uniquecatacombslabel_id	/**/ = get_id("UniqueCatacombsLabelCat");
	controlgirls_id			/**/ = get_id("ControlGirlsCat");
	controlgangs_id			/**/ = get_id("ControlGangsCat");
	girlgetsgirls_id		/**/ = get_id("GirlGetsGirlsCat");
	girlgetsitems_id		/**/ = get_id("GirlGetsItemsCat");
	girlgetsbeast_id		/**/ = get_id("GirlGetsBeastCat");
	girlsgirlspercslider_id	/**/ = get_id("GirlsGirlsPercSliderCat");
	girlsitemspercslider_id	/**/ = get_id("GirlsItemsPercSliderCat");
	ganggetsgirls_id		/**/ = get_id("GangGetsGirlsCat");
	ganggetsitems_id		/**/ = get_id("GangGetsItemsCat");
	ganggetsbeast_id		/**/ = get_id("GangGetsBeastCat");
	gangsgirlspercslider_id	/**/ = get_id("GangsGirlsPercSliderCat");
	gangsitemspercslider_id	/**/ = get_id("GangsItemsPercSliderCat");
}

cScreenSettings::cScreenSettings()
{
	cConfig cfg;
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "settings.xml";
	m_filename = dp.c_str();
	revert = true;
}

void cScreenSettings::init()
{
	if (g_CurrentScreen != SCREEN_SETTINGS) revert = true;
	g_CurrentScreen = SCREEN_SETTINGS;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		if (revert)
		{
			stringstream ss;
			m_EditBoxes[characters_id]->m_Text->SetText(cfg.folders.characters());
			m_EditBoxes[saves_id]->m_Text->SetText(cfg.folders.saves());
			m_EditBoxes[defaultimages_id]->m_Text->SetText(cfg.folders.defaultimageloc());
			m_EditBoxes[items_id]->m_Text->SetText(cfg.folders.items());
			m_CheckBoxes[backupsaves_id]->SetState(cfg.folders.backupsaves());
			m_CheckBoxes[preferdefault_id]->SetState(cfg.folders.preferdefault());

			ss.str("");	ss << "Chance of Unique Girls from Catacombs: "<< cfg.catacombs.unique_catacombs() <<"%";
			m_TextItems[uniquecatacombslabel_id]->SetText(ss.str());
			m_Sliders[uniquecatacombs_id]->Value(cfg.catacombs.unique_catacombs());
			m_CheckBoxes[controlgirls_id]->SetState(cfg.catacombs.control_girls());
			m_CheckBoxes[controlgangs_id]->SetState(cfg.catacombs.control_gangs());
			ss.str("");	ss << "Girls: " << cfg.catacombs.girl_gets_girls() << "%";	m_TextItems[girlgetsgirls_id]->SetText(ss.str());
			ss.str("");	ss << "Items: " << cfg.catacombs.girl_gets_items() << "%";	m_TextItems[girlgetsitems_id]->SetText(ss.str());
			ss.str("");	ss << "Beast: " << cfg.catacombs.girl_gets_beast() << "%";	m_TextItems[girlgetsbeast_id]->SetText(ss.str());
			m_Sliders[girlsgirlspercslider_id]->Value(cfg.catacombs.girl_gets_girls());
			m_Sliders[girlsitemspercslider_id]->Value(cfg.catacombs.girl_gets_girls() + cfg.catacombs.girl_gets_items());
			ss.str("");	ss << "Girls: " << cfg.catacombs.gang_gets_girls() << "%";	m_TextItems[ganggetsgirls_id]->SetText(ss.str());
			ss.str("");	ss << "Items: " << cfg.catacombs.gang_gets_items() << "%";	m_TextItems[ganggetsitems_id]->SetText(ss.str());
			ss.str("");	ss << "Beast: " << cfg.catacombs.gang_gets_beast() << "%";	m_TextItems[ganggetsbeast_id]->SetText(ss.str());
			m_Sliders[gangsgirlspercslider_id]->Value(cfg.catacombs.gang_gets_girls());
			m_Sliders[gangsitemspercslider_id]->Value(cfg.catacombs.gang_gets_girls() + cfg.catacombs.gang_gets_items());










			revert = false;
		}
//	backupsaves_id
//	preferdefault_id
	}

}

void cScreenSettings::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

void cScreenSettings::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_WinManager.Pop();
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(revert_id))
	{
		revert = true;
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(ok_id))
	{
		cfg.folders.characters() = m_EditBoxes[characters_id]->m_Text->GetText();
		cfg.folders.saves() = m_EditBoxes[saves_id]->m_Text->GetText();
		cfg.folders.defaultimageloc() = m_EditBoxes[defaultimages_id]->m_Text->GetText();
		cfg.folders.items() = m_EditBoxes[items_id]->m_Text->GetText();
		cfg.folders.backupsaves() = m_CheckBoxes[backupsaves_id]->GetState();
		cfg.folders.preferdefault() = m_CheckBoxes[preferdefault_id]->GetState();

		cfg.catacombs.unique_catacombs() = m_Sliders[uniquecatacombs_id]->Value();
		cfg.catacombs.control_girls() = m_CheckBoxes[controlgirls_id]->GetState();
		cfg.catacombs.control_gangs() = m_CheckBoxes[controlgangs_id]->GetState();
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







		revert = true;
		g_InitWin = true;
		return;
	}



	bool dogirlsliders = false;
	bool dogangsliders = false;
	stringstream ss;

	//	girlsgirlspercslider_id
	//	girlsitemspercslider_id
	//	gangsgirlspercslider_id
	//	gangsitemspercslider_id

	if (g_InterfaceEvents.CheckSlider(girlsgirlspercslider_id))
	{
		int s1 = SliderValue(girlsgirlspercslider_id);
		int s2 = SliderValue(girlsitemspercslider_id);
		if (s2 < s1)
		{
			s2 = s1;
			SliderRange(girlsitemspercslider_id, 0, 100, s2, 1);
		}
		dogirlsliders = true;
	}
	if (g_InterfaceEvents.CheckSlider(girlsitemspercslider_id))
	{
		int s1 = SliderValue(girlsitemspercslider_id);
		int s2 = SliderValue(girlsgirlspercslider_id);
		if (s1 < s2)
		{
			s2 = s1;
			SliderRange(girlsgirlspercslider_id, 0, 100, s2, 1);
		}
		dogirlsliders = true;
	}
	if (dogirlsliders)
	{
		int s1 = SliderValue(girlsgirlspercslider_id);
		int s2 = SliderValue(girlsitemspercslider_id);
		cfg.catacombs.girl_gets_girls() = s1;
		cfg.catacombs.girl_gets_items() = s2 - s1;
		cfg.catacombs.girl_gets_beast() = 100 - s2;
		ss.str("");	ss << "Girls: " << cfg.catacombs.girl_gets_girls() << "%";		EditTextItem(ss.str(), girlgetsgirls_id);
		ss.str("");	ss << "Items: " << cfg.catacombs.girl_gets_items() << "%";		EditTextItem(ss.str(), girlgetsitems_id);
		ss.str("");	ss << "Beast: " << cfg.catacombs.girl_gets_beast() << "%";	EditTextItem(ss.str(), girlgetsbeast_id);
		return;
	}

	if (g_InterfaceEvents.CheckSlider(gangsgirlspercslider_id))
	{
		int s1 = SliderValue(gangsgirlspercslider_id);
		int s2 = SliderValue(gangsitemspercslider_id);
		if (s2 < s1)
		{
			s2 = s1;
			SliderRange(gangsitemspercslider_id, 0, 100, s2, 1);
		}
		dogangsliders = true;
	}
	if (g_InterfaceEvents.CheckSlider(gangsitemspercslider_id))
	{
		int s1 = SliderValue(gangsitemspercslider_id);
		int s2 = SliderValue(gangsgirlspercslider_id);
		if (s1 < s2)
		{
			s2 = s1;
			SliderRange(gangsgirlspercslider_id, 0, 100, s2, 1);
		}
		dogangsliders = true;
	}
	if (dogangsliders)
	{
		int s1 = SliderValue(gangsgirlspercslider_id);
		int s2 = SliderValue(gangsitemspercslider_id);
		cfg.catacombs.gang_gets_girls() = s1;
		cfg.catacombs.gang_gets_items() = s2 - s1;
		cfg.catacombs.gang_gets_beast() = 100 - s2;
		ss.str("");	ss << "Girls: " << cfg.catacombs.gang_gets_girls() << "%";		EditTextItem(ss.str(), ganggetsgirls_id);
		ss.str("");	ss << "Items: " << cfg.catacombs.gang_gets_items() << "%";		EditTextItem(ss.str(), ganggetsitems_id);
		ss.str("");	ss << "Beast: " << cfg.catacombs.gang_gets_beast() << "%";	EditTextItem(ss.str(), ganggetsbeast_id);
		return;
	}

	if (g_InterfaceEvents.CheckSlider(uniquecatacombs_id))
	{
		cfg.catacombs.unique_catacombs() = m_Sliders[uniquecatacombs_id]->Value();
		ss.str("");	ss << "Chance of Unique Girls from Catacombs: " << cfg.catacombs.unique_catacombs() << "%";
		m_TextItems[uniquecatacombslabel_id]->SetText(ss.str());
		return;
	}





}

bool cScreenSettings::check_keys()
{
	return false;
}