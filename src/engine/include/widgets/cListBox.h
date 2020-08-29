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

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <SDL_keyboard.h>

#include "interface/cInterfaceObject.h"
#include "interface/cFont.h"
#include "interface/cSurface.h"
#include "interface/constants.h"

class cScrollBar;

struct cListItem
{
    int m_Color = 0;
    bool m_Selected = false;
    std::vector<std::string> m_Data;    // the text to display, up to LISTBOX_COLUMNS number of columns (+1 is used for "original sort" slot)
    int m_ID;    // the id for the item
    std::unique_ptr<SDL_Color> m_TextColor;
    int m_InsertionOrder = -1;      // tracks the order in which elements were put into the list box.
    std::vector<cSurface> m_PreRendered;
};

struct sColumnData {
    std::string name;           // internal name of the column
    std::string header;         // displayed header of the column
    int offset = -1;            // draw offset
    int width = -1;             // width of the column
    int sort;                   // sorting index, in case display order does not correspond to internal data order.
    bool skip = false;          // if true, this column will not be shown

    cSurface header_gfx;        // pre-rendered column header
};

class cListBox : public cUIWidget
{
public:
    cListBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, bool MultiSelect, bool ShowHeaders = false,
            bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = LISTBOX_ITEMHEIGHT);
    ~cListBox();

    bool IsOver(int x, int y) const override;
    bool HandleClick(int x, int y, bool press) override;
    bool HandleMouseWheel(bool down) override;
    bool HandleKeyPress(SDL_Keysym key) override;
    void Reset() override;
    void DrawWidget(const CGraphics& gfx) override;

    void SetSelectionCallback(std::function<void(int)>);
    void SetDoubleClickCallback(std::function<void(int)>);
    void SetArrowHotKeys(SDL_Keycode up, SDL_Keycode down);

    void SetElementText(int ID, std::string data);

    void ClearList();

    int ArrowDownList();
    int ArrowUpList();

    bool HasMultiSelected() const;

    void ScrollDown(int amount = 0, bool updatebar = true);
    void ScrollUp(int amount = 0, bool updatebar = true);
    int m_ScrollChange = -1;  // scrollbar changes will update this value

    int GetSelected();    // returns the id for the first selected element
    const std::string& GetSelectedText();
    int GetAfterSelected();    // returns the id for the element after the last selected element
    int GetNextSelected(int from, int& pos);    // returns the id for the next selected element and sets pos to its position
    int GetLastSelected();    // gets the last item selected
    bool IsSelected();    // returns true if an element is selected
    void SetSelected(int ID, bool ev = true, bool deselect_others = true);    // sets the selected item
    int GetSize() {return m_NumElements;};
    bool IsMultiSelect() {return m_MultiSelect;}
    void GetSortedIDList(std::vector<int> *id_vec, int *vec_pos);  // fills up a vector with the list of item IDs, sorted
    const std::vector<sColumnData>& GetColumnData() const { return m_Columns; }
    std::vector<std::string> GetColumnNames() const;

    int m_Position         = 0;    // What element is at position 0 on the list
    int m_NumElements      = 0;    // number of elements in the list
    int m_NumDrawnElements = 0;    // how many elements can be rendered at a time

    int m_eHeight, m_eWidth;    // the height and width of element images

    std::string m_HeaderClicked;                    // set to m_ColumnName value of a header that has just been clicked; otherwise empty

    void DefineColumns(std::vector<std::string> name, std::vector<std::string> header, std::vector<int> offset, std::vector<bool> skip);  // define column layout
    void SetColumnSort(const std::vector<std::string>& column_name);    // Update column sorting based on expected default order
    void AddElement(int ID, std::vector<std::string> data, int color);
    void SetElementText(int ID, std::string data[], int columns);
    void SetElementColumnText(int ID, std::string data, const std::string& column);
    void SetElementTextColor(int ID, SDL_Color text_color);

    SDL_Rect m_Divider;

    //sorting of list, normally based on header clicks
    bool m_HeaderClicksSort    = true;                // whether clicks on column headers should sort data accordingly
    std::string m_SortedColumn = {};                // m_ColumnName of column which sort is currently based on
    bool m_SortedDescending    = false;                // descending or ascending sort
    void SortByColumn(std::string ColumnName, bool Descending = false);  // re-sort list items based on specified column
    void ReSortList();                        // re-sort list again, if needed
    void UnSortList();                        // un-sort list back to the order the elements were originally added in

    // Double-click detection
    bool DoubleClicked();
    Uint32 m_CurrentClickTime = 0;
    int m_CurrentClickX = 0;
    int m_CurrentClickY = 0;
    Uint32 m_LastClickTime = 0;
    int m_LastClickX = 0;
    int m_LastClickY = 0;

    int m_BorderSize;
    int m_RowHeight = LISTBOX_ITEMHEIGHT;
    bool m_EnableEvents   = false;    // are events enabled
    bool m_MultiSelect    = false;
    bool m_HasMultiSelect = false;

    cScrollBar* m_ScrollBar; // pointer to the associated scrollbar
    using item_list_t = std::list<cListItem>;
    item_list_t::iterator FindItemAtPosition(int x, int y);
    item_list_t::iterator FindSelected(const item_list_t::iterator& start);

private:
    int m_FontSize = -1;
    cFont m_Font;

    item_list_t m_Items;
    item_list_t::iterator m_LastSelected;


    void handle_selection_change();
    std::function<void(int)> m_SelectionCallback;
    std::function<void(int)> m_DoubleClickCallback;

    SDL_Keycode m_UpArrowHotKey = SDLK_UNKNOWN;
    SDL_Keycode m_DownArrowHotKey = SDLK_UNKNOWN;

    bool m_ShowHeaders    = false;                    // whether to show column headers
    bool m_HeaderDividers = true;                    // whether to show dividers between column headers

    // columns data
    std::vector<sColumnData> m_Columns;       // column data

    // gfx
    cSurface              m_Border;
    cSurface              m_Background;                          // the background and border for the list item
    std::vector<cSurface> m_ElementBackgrounds;     // individual row backgrounds with different color
    std::vector<cSurface> m_SelectedElementBackgrounds;      // individual row backgrounds with different color

    cSurface m_HeaderBackground;                // the background and border for the multi-column header box
    std::string m_SortAscImage;                 // image used on a column header when sorting "ascending" on that column
    std::string m_SortDescImage;                // image used on a column header when sorting "descending" on that column
    cSurface m_HeaderSortBack;                    // the above two images are copied resized and stored here for actual use
    cSurface m_HeaderUnSort;                    // image used for the extra "un-sort" header which removes any custom sort
    cSurface m_ElementBorder;

    void HandleColumnHeaderClick(const sColumnData& column);

    void UpdatePositionsAfterSort();
    bool HandleSetFocus(bool focus) override { return true; }
};

#endif
