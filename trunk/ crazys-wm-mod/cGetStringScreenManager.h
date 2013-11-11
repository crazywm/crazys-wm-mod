/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include <string>

using namespace std;

extern bool g_EnterKey;

typedef void (*Funker)();

class cGetStringTransport_Base
{
public:
	virtual	void	assign(string s) =0;
};

class cGetStringTransport : public cGetStringTransport_Base
{
	string&	m_dest;
public:
	cGetStringTransport(string &dest) : m_dest(dest) { }
	void	assign(string s)  {
		m_dest = s;
	}
	cGetStringTransport& operator=(const cGetStringTransport& other)
	{
		if (this != &other)
		{
			m_dest = other.m_dest;
		}
		return *this;
	}
};

class cGetStringTransport_Func : public cGetStringTransport_Base
{
	Funker funky;
public:
	cGetStringTransport_Func(Funker munky) : funky(munky) { }
virtual	void	assign(string s)  {
		funky();
	}
};

class cGetStringScreenManager_Inner
{
	cGetStringTransport_Base *trans;
	bool	m_empty_ok;
public:
	cGetStringScreenManager_Inner() {
		trans = 0;
		m_empty_ok = true;
	}
	void process();
	void submit();
	void set_dest(string& dest) {
		trans = new cGetStringTransport(dest);
	}
	void set_handler(Funker funk) {
		trans = new cGetStringTransport_Func(funk);
	}
	void empty_allowed(bool bval) {
		m_empty_ok = bval;
	}
	bool empty_allowed() {
		return m_empty_ok;
	}
};

class cGetStringScreenManager
{
	static	cGetStringScreenManager_Inner *instance;
public:
	cGetStringScreenManager() {
		if(instance) {
			return;
		}
		g_EnterKey = false;
		instance = new cGetStringScreenManager_Inner();
	}
	void process() {
		instance->process();
	}
	void set_dest(string& dest) {
		instance->set_dest(dest);
	}
	void set_handler(Funker funk) {
		instance->set_handler(funk);
	}
	void empty_allowed(bool bval) {
		instance->empty_allowed(bval);
	}
	bool empty_allowed() {
		return instance->empty_allowed();;
	}
};


