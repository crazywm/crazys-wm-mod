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
#include "cBrothel.h"
#include "cScreenBuildingManagement.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "libintl.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_EnterKey;
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	int		g_CurrentScreen;

extern void GetString();
extern cInterfaceWindow g_GetString;

void cBuildingManagement::IDBlock::hide(cBuildingManagement *mgr)
{
	mgr->HideImage(shade);
	mgr->HideText(desc);
	mgr->HideText(type);
	cerr << "hiding rename button : " << rename << endl;
	mgr->HideButton(rename);
	mgr->HideText(unit);
	mgr->HideText(space);
	mgr->HideText(cost);
	mgr->HideImage(bevel_t);
	mgr->HideImage(bevel_b);
	mgr->HideImage(bevel_r);
	mgr->HideImage(bevel_l);
	mgr->HideImage(newsplash);
	mgr->HideButton(delete_btn);
/*
 *	should really be able to hide and unhide things
 *	at the widget level rather than addressing each component
 *	individually
 */
	mgr->HideText(glitz_cap);
	mgr->HideText(glitz_level);
	mgr->HideButton(glitz_down);
	mgr->HideButton(glitz_up);

	mgr->HideText(provides_cap);
	mgr->HideText(provides_level);
	mgr->HideButton(provides_down);
	mgr->HideButton(provides_up);

	mgr->HideText(secure_cap);
	mgr->HideText(secure_level);
	mgr->HideButton(secure_down);
	mgr->HideButton(secure_up);

	mgr->HideText(stealth_cap);
	mgr->HideText(stealth_level);
	mgr->HideButton(stealth_down);
	mgr->HideButton(stealth_up);

}

void cBuildingManagement::IDBlock::display(
	cBuildingManagement *mgr,
	int num,
	cBuilding *building
)
{
	stringstream ss;
	sFacility *facility = building->item_at(num);

	mgr->UnhideImage(shade);
	mgr->UnhideText(desc);
	mgr->UnhideText(type);
	mgr->UnhideButton(rename);
	mgr->UnhideText(unit);
	mgr->UnhideText(space);
	mgr->UnhideText(cost);
	mgr->UnhideImage(bevel_t);
	mgr->UnhideImage(bevel_l);
	mgr->UnhideImage(bevel_b);
	mgr->UnhideImage(bevel_r);
	mgr->HideImage(newsplash, !facility->new_flag);
	mgr->UnhideButton(delete_btn);

	mgr->UnhideText(glitz_cap);
	mgr->UnhideText(glitz_level);
	mgr->UnhideButton(glitz_down);
	mgr->UnhideButton(glitz_up);

	mgr->UnhideText(provides_cap);
	mgr->UnhideText(provides_level);
	mgr->UnhideButton(provides_down);
	mgr->UnhideButton(provides_up);

	mgr->UnhideText(secure_cap);
	mgr->UnhideText(secure_level);
	mgr->UnhideButton(secure_down);
	mgr->UnhideButton(secure_up);

	mgr->UnhideText(stealth_cap);
	mgr->UnhideText(stealth_level);
	mgr->UnhideButton(stealth_down);
	mgr->UnhideButton(stealth_up);

	ss << num << ": ";
	mgr->EditTextItem(ss.str(), unit);
	ss.str("");

	ss << facility->name() << ".";
	mgr->EditTextItem(ss.str(), desc);
	ss.str("");

	ss << facility->type() << ".";
	mgr->EditTextItem(ss.str(), type);
	ss.str("");

	ss << ("Space: ") << facility->space_taken() << ".";
	mgr->EditTextItem(ss.str(), space);
	ss.str("");

	ss << facility->slots();
	mgr->EditTextItem(ss.str(), provides_level);
	ss.str("");

	mgr->EditTextItem(("Glitz"), glitz_cap);
	ss << facility->glitz();
	mgr->EditTextItem(ss.str(), glitz_level);
	cerr	<< ("setting glitz_level (") << glitz_level << (") to ")
		<< ss.str()
		<< endl;
	ss.str("");

	mgr->EditTextItem(("Secure"), secure_cap);
	ss << facility->secure();
	mgr->EditTextItem(ss.str(), secure_level);
	ss.str("");

	mgr->EditTextItem(("Stealth"), stealth_cap);
	ss << facility->stealth();
	mgr->EditTextItem(ss.str(), stealth_level);
	ss.str("");

	ss << ("Cost: ") << facility->price() << ".";
	mgr->EditTextItem(ss.str(), cost);
	ss.str("");
}

void cBuildingManagement::init()
{
	stringstream ss;
	g_CurrentScreen = SCREEN_BUILDINGMANAGEMENT;
	if(!g_InitWin) {
		return;
	}
	Focused();
	g_InitWin = false;
/*
 *	buttons enable/disable
 */
	DisableButton(new_id, (selection == -1));
/*
 *	screen header
 */
	ss << ("Building Management: ") << brothel->m_Name << "." ;
	EditTextItem(ss.str(), header_id);
/*
 *	capacity / free / used
 */
	ss.str("");
	ss << ("Capacity: ") << building->capacity()
	   << (". Free: ")  << building->free_space()
	   << (". Used: ")  << building->used_space()
	   << "."
	;
	EditTextItem(ss.str(), capacity_id);
/*
 *	player gold for reference
 */
	ss.str("");
	ss << ("Gold: ") << g_Gold.ival();
	EditTextItem(ss.str(), gold_id);

	int unum;
	total_cost = 0;
	for(int i = 0; i < 6; i++) {
		unum = i + 6 * screen;

		if(unum >= building->size()) {
			blocks[unum].hide(this);
			continue;
		}
		blocks[unum].display(this, unum, building);
		total_cost += building->item_at(unum)->price();
	}
/*
 *	list box of elements to add
 */
	sFacilityList list;
	ClearListBox(listbox_id);
	for(u_int i = 0; i < list.size(); i++) {
		AddToListBox(listbox_id, i, list[i]->name());
	}

	ss.str("");
	ss << ("Cost: ") << total_cost;
	EditTextItem(ss.str(), total_cost_id);

	DisableButton(buy_button_id, total_cost == 0);
	DisableButton(revert_button_id, total_cost == 0);

	SetSelectedItemInList(listbox_id, selection, true);
}

bool cBuildingManagement::ids_set = false;

void cBuildingManagement::set_ids()
{
	stringstream ss;

	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cBuildingManagement");

	header_id = get_id("ScreenHeader");
	back_id				= get_id("BackButton","Back");
	capacity_id			= get_id("Capacity");
	narrative_id		= get_id("Narrative");
	new_id				= get_id("NewButton");
	gold_id				= get_id("Gold");
	total_cost_id		= get_id("TotalCost");
	buy_button_id		= get_id("BuyButton");
	revert_button_id	= get_id("RevertButton");
	for(int i = 0; i < 6; i++) {
		ss.str("");
		ss << i;
		string istr	= ss.str();
		blocks[i].unit	= get_id(string("Unit")	+ istr);
		blocks[i].shade	= get_id(string("Shader") + istr);
		blocks[i].desc	= get_id(string("Name") + istr);
		blocks[i].type	= get_id(string("Type") + istr);
		blocks[i].rename= get_id(string("RenameButton") + istr);
		blocks[i].space	= get_id(string("Spaces") + istr);
		blocks[i].cost	= get_id(string("Cost") + istr);
		blocks[i].delete_btn	= get_id(string("DeleteButton") + istr);

		blocks[i].glitz_cap = get_id(string("CaptionGlitz") + istr);
		blocks[i].glitz_level = get_id(string("LevelGlitz") + istr);
		blocks[i].glitz_down = get_id(string("DownGlitz") + istr);
		blocks[i].glitz_up = get_id(string("UpGlitz") + istr);

		blocks[i].provides_cap = get_id(string("CaptionProvides") + istr);
		blocks[i].provides_level = get_id(string("LevelProvides") + istr);
		blocks[i].provides_down = get_id(string("DownProvides") + istr);
		blocks[i].provides_up = get_id(string("UpProvides") + istr);

		blocks[i].secure_cap = get_id(string("CaptionSecure") + istr);
		blocks[i].secure_level = get_id(string("LevelSecure") + istr);
		blocks[i].secure_down = get_id(string("DownSecure") + istr);
		blocks[i].secure_up = get_id(string("UpSecure") + istr);

		blocks[i].stealth_cap = get_id(string("CaptionStealth") + istr);
		blocks[i].stealth_level = get_id(string("LevelStealth") + istr);
		blocks[i].stealth_down = get_id(string("DownStealth") + istr);
		blocks[i].stealth_up = get_id(string("UpStealth") + istr);

		blocks[i].bevel_t = get_id(
			string("BevelTop") + istr, "*Unused*"
		);
		blocks[i].bevel_b = get_id(
			string("BevelBottom") + istr, "*Unused*"
		);
		blocks[i].bevel_r = get_id(
			string("BevelRight") + istr, "*Unused*"
		);
		blocks[i].bevel_l = get_id(
			string("BevelLeft") + istr, "*Unused*"
		);
		blocks[i].newsplash	= get_id(string("NewSplash") + istr);
	}
}

bool cBuildingManagement::check_keys()
{
	if(g_UpArrow) {
		selection = ArrowUpListBox(listbox_id);
		g_UpArrow = false;
		//g_InitWin = true;
		SetSelectedItemInList(listbox_id, selection);
		return true;
	}
	if(g_DownArrow) {
		selection = ArrowDownListBox(listbox_id);
		g_DownArrow = false;
		//g_InitWin = true;
		SetSelectedItemInList(listbox_id, selection);
		return true;
	}
	if(g_AltKeys)
	{
	if(g_A_Key) {
		selection = ArrowUpListBox(listbox_id);
		g_A_Key = false;
		//g_InitWin = true;
		SetSelectedItemInList(listbox_id, selection);
		return true;
	}
	if(g_D_Key) {
		selection = ArrowDownListBox(listbox_id);
		g_D_Key = false;
		//g_InitWin = true;
		SetSelectedItemInList(listbox_id, selection);
		return true;
	}
	}
	return false;
}

void cBuildingManagement::new_facility()
{
	sFacilityList fax;
	int index = selection;
/*
 *	if there's nothing selected, we don't do nothing
 *	There should always be soemthing selected mind,
 *	Else the button should be greyed out
 */
 	if(index == -1) {
		return;
	}
/*
 *	also check that the index isn't bigger than the fax list
 *	Again, this is a "can not happen" check
 */
 	if(index >= int(fax.size())) {
		return;
	}
/*
 *	is this the first new modification?
 *	if it is, take a copy of the list
 *
 *	we need to be able to revert it
 *
 */
 	if(building->list_is_clean()) {
		building->make_reversion_list();
	}
	sFacility *fac = fax[index];
	sFacility *copy = fac->clone();
	copy->new_flag = true;
/*
 *	add it to the building
 */
	building->add(copy);
}

/*
 * click this to commit the changes on the screen
 */
void cBuildingManagement::buy_button()
{
/*
 *	This should be easy: deduct the gold
 *	total from the player's account
 */
 	g_Gold.brothel_cost( total_cost );
	total_cost = 0;
/*
 *	commit the changes: clear the new flag, set the cost
 *	to zero...
 */
 	building->commit();
	// and that should be it...
}

/*
 * click this to roll back the changes on the screen
 */
void cBuildingManagement::revert_button()
{
	building->revert();
}


void cBuildingManagement::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) {
		set_ids();
	}
/*
 *	handle arrow keys
 */
 	if(check_keys()) {
		return;
	}
/*
 *	get a pointer to the current brothel
 */
	brothel = g_Brothels.GetBrothel(g_CurrBrothel);
	building = &brothel->building;

	init();
/*
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}
/*
 *	otherwise, compare event IDs
 *
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
/*
 *	the "New" button takes the selected listbox entry
 *	and adds it to the building's build list
 */
	if(g_InterfaceEvents.CheckButton(new_id)) {
		new_facility();
		g_InitWin = true;
		return;
	}
/*
 *	The buy button commits the changes in the build list
 */
	if(g_InterfaceEvents.CheckButton(buy_button_id)) {
		buy_button();
		g_InitWin = true;
		return;
	}
/*
 *	the revert button reverts the screen back to how it was
 *	when the building was on entering the screen
 *	(or at the time of the last commit, of course)
 */
	if(g_InterfaceEvents.CheckButton(revert_button_id)) {
		revert_button();
		g_InitWin = true;
		return;
	}
/*
 *	catch a selection change event for the listbox
 */
	if(g_InterfaceEvents.CheckListbox(listbox_id)) {
		string s;
		sFacilityList fax;
/*
 *		update the selection variable
 */
		selection = GetLastSelectedItemFromList(listbox_id);
/*
 *		enable/disable new button if needed
 */
		bool new_disabled = (
			selection == -1
		||
			building->free_space() == 0
		);
		DisableButton(new_id, new_disabled);
/*
 *		set narrative text if a list element is selected
 */
		if(selection == -1) {
			s = "";
		}
		else {
			s = fax[selection]->desc();
		}
		EditTextItem(s, narrative_id);
		return;
	}
/*
 *	check for a rename button being pushed
 *	there's 6 of these, so we need a loop
 */
	int fac_no;
	for(int i = 0; i < 6; i++) {
		IDBlock *bpt = blocks + i;
/*
 *		get the facility number
 */
		fac_no = i + 6 * screen;
/*
 *		check the delete button
 */
		if(g_InterfaceEvents.CheckButton(bpt->delete_btn)) {
			delete_button(fac_no);
			g_InitWin = true;
			continue;
		}
/*
 *		see if the rename button for this block has an event waiting
 */
		if(g_InterfaceEvents.CheckButton(bpt->rename)) {
			rename_button(fac_no);
			g_InitWin = true;
			continue;
		}
/*
 *		check the glitz up and down buttons
 */
		if(g_InterfaceEvents.CheckButton(bpt->glitz_up)) {
			glitz_up(fac_no);
			g_InitWin = true;
			continue;
		}
		if(g_InterfaceEvents.CheckButton(bpt->glitz_down)) {
			glitz_down(fac_no);
			g_InitWin = true;
			continue;
		}
/*
 *		same for security adjuster
 */
		if(g_InterfaceEvents.CheckButton(bpt->secure_up)) {
			secure_up(fac_no);
			g_InitWin = true;
			continue;
		}
		if(g_InterfaceEvents.CheckButton(bpt->secure_down)) {
			secure_down(fac_no);
			g_InitWin = true;
			continue;
		}
/*
 *		now stealth...
 */
		if(g_InterfaceEvents.CheckButton(bpt->stealth_up)) {
			stealth_up(fac_no);
			g_InitWin = true;
			continue;
		}
		if(g_InterfaceEvents.CheckButton(bpt->stealth_down)) {
			stealth_down(fac_no);
			g_InitWin = true;
			continue;
		}
	}
}

/*
 * all this does is pop up the GetString screen to get
 * the new name for the facility
 */
void cBuildingManagement::rename_button(int facility_idx)
{
	cGetStringScreenManager gssm;
/*
 *	get the facility data
 */
 	sFacility *sfac = building->item_at(facility_idx);
/*
 *	tell the GetString screen where to store the
 *	string
 */
	gssm.set_dest( sfac->m_instance_name );
/*
 *	push the GetString screen onto the stack
 */
	g_WinManager.Push(GetString, &g_GetString);
}

/*
 * remove the building from the list
 *
 * need to think about cash a bit more closely here
 */
void cBuildingManagement::delete_button(int facility_idx)
{
	building->remove(facility_idx);
}

/*
 * tweak the glitz rating
 */
void cBuildingManagement::glitz_up(int facility_idx)
{
/*
 *	get the facility data
 */
 	sFacility *sfac = building->item_at(facility_idx);
/*
 *	tweak the value
 */
	sfac->glitz_up();
}

/*
 * tweak the glitz rating
 */
void cBuildingManagement::glitz_down(int facility_idx)
{
/*
 *	get the facility data
 */
 	sFacility *sfac = building->item_at(facility_idx);
/*
 *	tweak the value
 */
	sfac->glitz_down();
}


/*
 * tweak the glitz rating
 */
void cBuildingManagement::secure_up(int facility_idx)
{
/*
 *	get the facility data
 */
 	sFacility *sfac = building->item_at(facility_idx);
/*
 *	tweak the value
 */
	sfac->secure_up();
}

/*
 * tweak the glitz rating
 */
void cBuildingManagement::secure_down(int facility_idx)
{
/*
 *	get the facility data
 */
 	sFacility *sfac = building->item_at(facility_idx);
/*
 *	tweak the value
 */
	sfac->secure_down();
}

/*
 * stealth tweakers
 */
void cBuildingManagement::stealth_up(int facility_idx)
{
 	sFacility *sfac = building->item_at(facility_idx);
	sfac->stealth_up();
}

void cBuildingManagement::stealth_down(int facility_idx)
{
 	sFacility *sfac = building->item_at(facility_idx);
	sfac->stealth_down();
}

