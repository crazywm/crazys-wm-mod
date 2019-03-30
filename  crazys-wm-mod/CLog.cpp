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
#include <iostream>
#include "CLog.h"

bool CLogInner::setup = false;

CLogInner::CLogInner()
{
	if(!setup) {
		init();
	}
}

void CLogInner::init()
{
	cerr << "CLogInner::init" << endl;
	if(setup) return;
	setup = true;
	m_ofile.open("gamelog.txt");
}

CLogInner::~CLogInner()
{
	m_ofile.close();
}

void CLogInner::write(string text)
{
	m_ofile<<text<<endl;
	m_ofile.flush();
#ifdef LINUX
	std::cerr << text << endl;
#endif
}

CLogInner *CLog::inner = nullptr;

CLog::~CLog()
{
    if(!m_glob) {
        return;
    }
    delete inner;
}
