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
#ifndef __CINTERFACEEVENT_H
#define __CINTERFACEEVENT_H

const int EVENT_BUTTONCLICKED = 1;
const int EVENT_SELECTIONCHANGE = 2;
const int EVENT_CHECKBOXCLICKED = 3;
const int EVENT_SLIDERCHANGE = 4;
// need to think about this
class cInterfaceEvent
{
public:
	cInterfaceEvent() {m_Next=nullptr;}
	~cInterfaceEvent() {if(m_Next)delete m_Next;m_Next=nullptr;}

	int m_EventID;
	int m_ObjectID;

	cInterfaceEvent* m_Next;
};

class cInterfaceEventManager
{
public:
	cInterfaceEventManager(){m_NumEvents=0;m_Events=nullptr;}
	~cInterfaceEventManager(){if(m_Events)delete m_Events; m_Events=nullptr;}
/*
 *	a bit of shorthand - helps keep the code cleaner
 */
	bool CheckButton(int ObjectID) {
		return CheckEvent(EVENT_BUTTONCLICKED, ObjectID);
	}
	bool CheckListbox(int ObjectID) {
		return CheckEvent(EVENT_SELECTIONCHANGE, ObjectID);
	}
	bool CheckCheckbox(int ObjectID) {
		return CheckEvent(EVENT_CHECKBOXCLICKED, ObjectID);
	}
	bool CheckSlider(int ObjectID) {
		return CheckEvent(EVENT_SLIDERCHANGE, ObjectID);
	}

	bool CheckEvent(int EventID, int ObjectID)
	{
		if (!m_Events)
		{
			m_NumEvents = 0;
			return false;
		}

		cInterfaceEvent* current = m_Events;
		cInterfaceEvent* last = nullptr;
		while(current)
		{
			if(current->m_EventID == EventID && current->m_ObjectID == ObjectID)
			{
				if(last)
				{
					last->m_Next = current->m_Next;
					current->m_Next = nullptr;
					delete current;
					m_NumEvents--;
					return true;
				}
				else
				{
					if(current->m_Next)
						m_Events = current->m_Next;
					else
						m_Events = nullptr;

					current->m_Next = nullptr;
					delete current;
					m_NumEvents--;
					return true;
				}
			}
			last = current;
			current = current->m_Next;
		}
		return false;
	}

	int GetNumEvents() {return m_NumEvents;}

	void AddEvent(int ID, int Object)
	{
		cInterfaceEvent* newEvent = new cInterfaceEvent();
		newEvent->m_EventID = ID;
		newEvent->m_ObjectID = Object;
		newEvent->m_Next = nullptr;

		if (!m_Events)
		{
			m_NumEvents = 0;
			m_Events = newEvent;
		}
		else
		{
			cInterfaceEvent* current = m_Events;
			while(current->m_Next)
				current = current->m_Next;
			current->m_Next = newEvent;
		}
		m_NumEvents++;
	}

	void ClearEvents() { if(m_Events) delete m_Events; m_Events = nullptr;}

private:
	int m_NumEvents;
	cInterfaceEvent* m_Events;
};

#endif
