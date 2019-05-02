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
#include "CRenderQue.h"

void CRenderQue::DrawQue()
{
	CRenderObject* temp = m_Parent;
	while(temp)
	{
		temp->Draw();
		temp = temp->m_Next;
	}

	ClearQue();
}

void CRenderQue::ClearQue() {m_Parent = nullptr; m_Last = nullptr;}

void CRenderQue::AddObject(CRenderObject* object)
{
	object->m_Next = nullptr;
	if(!m_Parent)
	{
		m_Parent = object;
		m_Last = object;
	}
	else
	{
		m_Last->m_Next = object;
		m_Last = object;
	}
}
