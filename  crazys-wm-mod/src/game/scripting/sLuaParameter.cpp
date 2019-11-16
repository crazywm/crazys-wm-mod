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
#include "sLuaParameter.hpp"
#include "sLuaGirl.h"
#include "cLuaState.h"

void scripting::sLuaParameter::push(scripting::cLuaState& target) const
{
	switch (type) {
	case GIRL:
		sLuaGirl::create(target.get_state(), data.girl);
		return;
	case CUSTOMER:
		sLuaCustomer::create(target.get_state(), data.customer);
		return;
	}
}

scripting::sLuaParameter::sLuaParameter(sGirl* girl) : type(GIRL), data{.girl=girl}
{
}

scripting::sLuaParameter::sLuaParameter(sGirl& girl) : sLuaParameter(&girl)
{
}

scripting::sLuaParameter::sLuaParameter(sCustomer* cust) : type(CUSTOMER), data{.customer=cust}
{
}

scripting::sLuaParameter::sLuaParameter(sCustomer& cust) : sLuaParameter(&cust)
{

}
