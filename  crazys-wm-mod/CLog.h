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
/*
 * #pragma once does the same thing as #ifndef __FOO_H etc etc
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

struct CLogInner
{
public:
	CLogInner();
	~CLogInner();
	void init();

	void write(string text);
	ofstream	&os()	{ return m_ofile; }
	stringstream	&ss()	{ return m_ss; }
	void ssend() {
		write(m_ss.str());
		m_ss.str("");
	}
static	bool setup;
	ofstream m_ofile;
	stringstream m_ss;
};


class CLog
{
public:
	explicit CLog(bool a_glob = false) {
		m_glob = a_glob;
	}
	~CLog();
	void write(string text)	{
		if(!inner) inner = new CLogInner();
		inner->write(text);
	}
	ofstream	&os()	{
		if(!inner) inner = new CLogInner();
		return inner->m_ofile;
	}
	stringstream	&ss()	{
		if(!inner) inner = new CLogInner();
		return inner->m_ss;
	}
	void		ssend()	{
		if(!inner) inner = new CLogInner();
		inner->ssend();
	}
private:
	bool m_glob;
    static CLogInner *inner;
};

extern CLog g_LogFile;
