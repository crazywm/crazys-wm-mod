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
#ifndef __CLISTBOX_H
#define __CLISTBOX_H

#include "cInterfaceObject.h"
#include "cScrollBar.h"
#include "cFont.h"
#include "DirPath.h"
#include "Constants.h"

using namespace std;

#if 0
// Listbox Constants moved to Constants.h
const unsigned int COLOR_BLUE = 0;
const unsigned int COLOR_RED = 1;
//const unsigned int COLOR_DARKBLUE = 2;

//how tall (in pixels) each list item is
const int LISTBOX_ITEMHEIGHT = 20;

//how many columns are allowed
const unsigned int LISTBOX_COLUMNS = 15;
#endif

class cScrollBar;

class cListItem
{
public:
	cListItem() {m_Next = 0;m_Color=0;m_TextColor=0;m_Selected = false;}
	~cListItem() {if(m_Next) delete m_Next; m_Next = 0;}

	int m_Color;
	bool m_Selected;
	string m_Data[LISTBOX_COLUMNS+1];	// the text to display, up to LISTBOX_COLUMNS number of columns (+1 is used for "original sort" slot)
	int m_ID;	// the id for the item
	SDL_Color* m_TextColor;
	cListItem* m_Next;	// pointer to the next object in the list
};

class cListBox : public cInterfaceObject
{
public:
	cListBox();
	~cListBox();

	bool IsOver(int x, int y);
	void OnClicked(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);

	void CreateListbox(int ID, int x, int y, int width, int height, int BorderSize, bool MultiSelect, bool ShowHeaders = false, bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = LISTBOX_ITEMHEIGHT);

	void Draw();

	void AddElement(int ID, string data, int color = COLOR_BLUE);
	void SetElementText(int ID, string data);
//	void RemoveElement(int ID);

	void ClearList();

	int ArrowDownList();
	int ArrowUpList();

	bool HasMultiSelected();

	void ScrollDown(int amount = 0, bool updatebar = true);
	void ScrollUp(int amount = 0, bool updatebar = true);
	int m_ScrollChange;  // scrollbar changes will update this value

	int GetSelected();	// returns the id for the first selected element
	string GetSelectedText();
	int GetAfterSelected();	// returns the id for the element after the last selected element
	int GetNextSelected(int from, int& pos);	// returns the id for the next selected element and sets pos to its position
	int GetLastSelected();	// gets the last item selected
	bool IsSelected();	// returns true if an element is selected
	void SetSelected(int ID, bool ev = true, bool deselect_others = true);	// sets the selected item
	int GetSize() {return m_NumElements;};
	bool IsMultiSelect() {return m_MultiSelect;}
	void GetSortedIDList(vector<int> *id_vec, int *vec_pos);  // fills up a vector with the list of item IDs, sorted
	void GetColumnNames(vector<string>& columnNames);

	int DayJobColumn();				// `J` returns the column number of "DayJob"
	int NightJobColumn();			// `J` returns the column number of "NightJob"
	int JobColumn(bool Day0Night1);	// `J` returns the column number of either 0="DayJob" 1="NightJob"


	int m_ID;
	cListItem* m_Items;	// The list of elements in the list
	cListItem* m_LastItem;	// The last elements of the list
	cListItem* m_LastSelected;
	int m_Position;	// What element is at position 0 on the list
	int m_NumElements;	// number of elements in the list
	int m_NumDrawnElements;	// how many elements can be rendered at a time

	int m_eHeight, m_eWidth;	// the height and width of element images
	int m_ScrollDragID;	// the id for the scroll bar


	// Multi-Column Support
	int m_ColumnCount;						// how many columns to display
	int m_ColumnOffset[LISTBOX_COLUMNS];	// x offset for each column
	bool m_ShowHeaders;						// whether to show column headers
	bool m_HeaderDividers;					// whether to show dividers between column headers
	string m_Header[LISTBOX_COLUMNS];		// text of column headers
	string m_ColumnName[LISTBOX_COLUMNS];	// reference names of columns, for sorting
	int m_ColumnSort[LISTBOX_COLUMNS];		// reference table used for custom sort of columns
	bool m_SkipColumn[LISTBOX_COLUMNS];		// used to skip a column which would normally be shown

	string m_HeaderClicked;					// set to m_ColumnName value of a header that has just been clicked; otherwise empty

	void DefineColumns(string name[], string header[], int offset[], bool skip[], int columns);  // define column layout
	void SetColumnSort(string column_name[], int columns);	// Update column sorting based on expected default order
	void AddElement(int ID, string data[], int columns, int color = COLOR_BLUE);
	void SetElementText(int ID, string data[], int columns);
	void SetElementColumnText(int ID, string data, int column);
	void SetElementTextColor(int ID, SDL_Color* text_color);

	SDL_Surface* m_HeaderBackground;		// the background and border for the multi-column header box
	static SDL_Surface* m_HeaderSortAsc;	// image used on a column header when sorting "ascending" on that column
	static SDL_Surface* m_HeaderSortDesc;	// image used on a column header when sorting "descending" on that column
	SDL_Surface* m_HeaderSortBack;			// the above two images are copied resized and stored here for actual use
	static SDL_Surface* m_HeaderUnSort;		// image used for the extra "un-sort" header which removes any custom sort

	SDL_Rect m_Divider;

	//sorting of list, normally based on header clicks
	bool m_HeaderClicksSort;				// whether clicks on column headers should sort data accordingly
	string m_SortedColumn;					// m_ColumnName of column which sort is currently based on
	bool m_SortedDescending;				// descending or ascending sort
	void SortByColumn(string ColumnName, bool Descending = false);  // re-sort list items based on specified column
	void ReSortList();						// re-sort list again, if needed
	void UnSortList();						// un-sort list back to the order the elements were originally added in
	cListItem* BubbleSortList(cListItem *head, int count, int col_id, bool Descending);
	bool StrCmpIn(const string &left, const string &right);

	// Double-click detection
	bool DoubleClicked();
	Uint32 m_CurrentClickTime;
	int m_CurrentClickX;
	int m_CurrentClickY;
	Uint32 m_LastClickTime;
	int m_LastClickX;
	int m_LastClickY;


	cFont m_Font;

	int m_BorderSize;
	int m_RowHeight = LISTBOX_ITEMHEIGHT;
	SDL_Surface* m_Background;	// the background and border for the list item
	SDL_Surface* m_RedBackground;	// the background used for important things
	SDL_Surface* m_GreenBackground;	// the background used for good news
	SDL_Surface* m_DarkBlueBackground;	// the background used for important things
	SDL_Surface* m_YellowBackground;	// the background used for level up
	SDL_Surface* m_Border;

	SDL_Surface* m_SelectedRedBackground;	// the background used for selected important things
	SDL_Surface* m_SelectedDarkBlueBackground;	// the background used for selected important things
	SDL_Surface* m_SelectedGreenBackground;	// `J` added green background used for good news
	SDL_Surface* m_SelectedYellowBackground;	// `J` added green background used for good news

	SDL_Surface* m_ElementBackground;	// the background and border for the list elements
	SDL_Surface* m_ElementSelectedBackground;	// the background and border for the list elements
	SDL_Surface* m_ElementBorder;
	bool m_EnableEvents;	// are events enabled
	bool m_MultiSelect;
	bool m_HasMultiSelect;

	cScrollBar* m_ScrollBar; // pointer to the associated scrollbar

	cListBox* m_Next;
};

#endif
