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

#include <vector>

struct sXmlWidgetPart {
public:
	int x, y, w, h, r, g, b;
	int fontsize, bordersize;
	bool stat, alpha, scale, hidden, events, multi, hide, cache, leftorright;
	string base, on, off, disabled, type, name, text, file, seq;
};

class cXmlWidget {
	vector<sXmlWidgetPart> list;
public:
	cXmlWidget() {}
	int size() {	return int(list.size()); }
	sXmlWidgetPart& operator[](int i) {
		return list[i];
	}
	void add(sXmlWidgetPart &part)
	{
		list.push_back(part);
	}
};

