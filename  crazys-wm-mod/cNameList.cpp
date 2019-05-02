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
#include <fstream>
#include "Constants.h"
#include "cNameList.h"
#include "CLog.h"
#include "cRng.h"

using namespace std;

extern cRng g_Dice;
extern CLog g_LogFile;

cNameList::cNameList(string file)
{
	load(file);
}

void cNameList::load(string file)
{
	ifstream in;
	char buff[1024];
	bool first = true;
	/*
	*	open the file
	*/
	in.open(file.c_str());
	if (!in)
	{
		g_LogFile.ss() << "Error: unable to open file '" << file << "'";
		g_LogFile.ssend();
		return;
	}
	names.clear();
	/*
	*	loop until the stream breaks
	*/
	for (;;)
	{
		in.getline(buff, sizeof(buff) - 1);
		/*
		*		before we do anythign with the string
		*		test to see if there's an error flag
		*/
		if (!in.good())
		{
			break;		// ok, we're done
		}
		/*
		*		the first line is the number of names
		*		we don't need it any more, so skip over it
		*/
		if (first)
		{
			first = false;
			continue;
		}
		string s = buff;
		/*
		*		just for linux (although will do no harm on windows)
		*		we need to check for a carriage return char at the
		*		end of the line and remove it if found
		*/
		int last = s.length() - 1;
		if (s[last] == '\r')
		{
			s.erase(last);
		}
		/*
		*		finally, add it to the vector
		*/
		names.push_back(s);
	}
	/*
	*	quick sanity check
	*/
	if (names.empty())
	{
		g_LogFile.ss() << "Error: zero names found in file '" << file << "'";
		g_LogFile.ssend();
		return;
	}
}

string cNameList::random()
{
	u_int size = names.size();
	/*
	*	subscripting an empty vector causes a crash
	*	let's make sure that doesn't happen
	*/
	if (size == 0) 
	{
		g_LogFile.ss() << "Error: no names in cNameList: " << "Returning name 'error'";
		g_LogFile.ssend();
		/*
		*		make sure we see the bug reports ...
		*/
		return "";
	}
	/*
	*	otherwise, pick a random name
	*/
	return names[g_Dice.random(size - 1)];  // edited from size to size-1 since I got an OOB vector crash once
}


cSurnameList::cSurnameList(string file)
{
	load(file);
}

void cSurnameList::load(string file)
{
	ifstream in;
	char buff[1024];
	bool first = true;
	/*
	*	open the file
	*/
	in.open(file.c_str());
	if (!in)
	{
		g_LogFile.ss() << "Error: unable to open file '" << file << "'";
		g_LogFile.ssend();
		return;
	}
	names.clear();
	/*
	*	loop until the stream breaks
	*/
	for (;;)
	{
		in.getline(buff, sizeof(buff) - 1);
		/*
		*		before we do anythign with the string
		*		test to see if there's an error flag
		*/
		if (!in.good()) { break; }		// ok, we're done
		/*
		*		the first line is the number of names
		*		we don't need it any more, so skip over it
		*/
		if (first) { first = false; continue; }
		string s = buff;
		/*
		*		just for linux (although will do no harm on windows)
		*		we need to check for a carriage return char at the
		*		end of the line and remove it if found
		*/
		int last = s.length() - 1;
		if (s[last] == '\r') { s.erase(last); }
		/*
		*		finally, add it to the vector
		*/
		names.push_back(s);
	}
	/*
	*	quick sanity check
	*/
	if (names.empty())
	{
		g_LogFile.ss() << "Error: zero names found in file '" << file << "'";
		g_LogFile.ssend();
		return;
	}
}

string cSurnameList::random()
{
	u_int size = names.size();
	/*
	*	subscripting an empty vector causes a crash
	*	let's make sure that doesn't happen
	*/
	if (size == 0)
	{
		g_LogFile.ss() << "Error: no names in cSurnameList: " << "Returning name 'error'";
		g_LogFile.ssend();
		/*
		*		make sure we see the bug reports ...
		*/
		return "";
	}
	/*
	*	otherwise, pick a random name
	*/
	return names[g_Dice.random(size - 1)];  // edited from size to size-1 since I got an OOB vector crash once
}