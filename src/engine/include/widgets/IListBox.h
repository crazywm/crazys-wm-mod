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

#ifndef WM_ILISTBOX_H
#define WM_ILISTBOX_H

#include <functional>
#include <vector>
#include "interface/cInterfaceObject.h"
#include <SDL_keyboard.h>

class FormattedCellData;

class IListBox : public cUIWidget
{
public:
    // callbacks / hotkeys
    virtual void SetSelectionCallback(std::function<void(int)>) = 0;
    virtual void SetDoubleClickCallback(std::function<void(int)>) = 0;
    virtual void SetArrowHotKeys(SDL_Keycode up, SDL_Keycode down) = 0;

    // Content
    virtual void Clear() = 0;

    /// Given a query function that maps column names to cell data, this function
    /// adds a new row to the table.
    virtual void AddRow(int id, const std::function<FormattedCellData(const std::string&)>& query, int color) = 0;

    /// Given a query function that maps column names to cell data, this function
    /// updates a row in the table.
    virtual void UpdateRow(int id, const std::function<FormattedCellData(const std::string&)>& query, int color) = 0;

    /// Add row for any T* that has a GetDetail function
    template<class T>
    void AddRow(int id, const T* source, int color) {
        return AddRow(id, [source](const std::string& column){ return source->GetDetail(column); }, color);
    };

    /// Update row for any T* that has a GetDetail function
    template<class T>
    void UpdateRow(int id, const T* source, int color) {
        return UpdateRow(id, [source](const std::string& column){ return source->GetDetail(column); }, color);
    };

    // Generic Queries
    /// Returns the total number of items in the list box
    virtual int NumItems() const = 0;

    /// Creates a vector that contains the names of all the columns
    virtual std::vector<std::string> GetColumnNames() const = 0;

    // Scrolling
    /// Gets the index of the item that is at the top of the list box.
    virtual int GetTopPosition() const = 0;
    /// Sets the index of the item that is at the top of the list box.
    virtual void SetTopPosition(int pos) = 0;

    // Selection
    /// This function can only be used for single-selection ListBox elements, and will throw a logic error otherwise.
    /// If no element is selected, -1 is returned.
    virtual int GetSelectedID() const = 0;

    /// This function can only be used for single-selection ListBox elements, and will throw a logic error otherwise.
    /// If no element is selected, -1 is returned.
    virtual int GetSelectedIndex() const = 0;

    /// Sets the selected element based on its index in the list box.
    /// If trigger is true, then a `selection_change` event is triggered.
    /// If deselect is true, then all other elements will be deselected.
    virtual void SetSelectedIndex(int index, bool trigger=true, bool deselect = true) = 0;

    /// Sets the selected element based on its index in the list box.
    /// If trigger is true, then a `selection_change` event is triggered.
    /// If deselect is true, then all other elements will be deselected.
    virtual void SetSelectedID(int id, bool trigger=true, bool deselect = true) = 0;

    /// Returns the number of selected elements.
    virtual int NumSelectedElements() const = 0;

    /// Calls handler for each index that is selected. Returns the number of selected elements.
    /// Don't add or remove any rows from the ListBox in the handler!
    virtual int HandleSelectedIndices(std::function<void(int)> handler) = 0;

protected:
    using cUIWidget::cUIWidget;
};

#endif //WM_ILISTBOX_H
