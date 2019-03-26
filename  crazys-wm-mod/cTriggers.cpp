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
#include "cTriggers.h"
#include <fstream>
#include "cScriptManager.h"
#include "GameFlags.h"
#include "cGirls.h"
#include "cGold.h"
using namespace std;

extern cGirls g_Girls;
extern cGold g_Gold;
extern cRng g_Dice;
static map<string, int> trigger_types;

void cTriggerList::AddTrigger(cTrigger* trigger)
{
	trigger->m_Next = m_Triggers;
	m_Triggers = trigger;
	m_NumTriggers++;
}

void cTriggerList::LoadTriggersLegacy(ifstream& ifs)
{
	int temp;
	int numTriggersToLoad = 0;
	char buffer[1000];
	Free();

	if (ifs.peek() == '\n') ifs.ignore(1, '\n');
	ifs >> numTriggersToLoad;
	if (numTriggersToLoad > 10) // Myr: For some reason I can't determine my save game is suddenly
		numTriggersToLoad = 10; //      generating a value of 16842851 here. Sanity check.
	for (int i = 0; i<numTriggersToLoad; i++)
	{
		cTrigger* current = new cTrigger();

		// load the type
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		ifs >> temp;
		current->m_Type = temp;

		// load additional data
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		if (current->m_Type == TRIGGER_SKILL || current->m_Type == TRIGGER_STAT)
		{
			// skill/stat id
			ifs >> temp;
			current->m_Values[0] = temp;

			// what value it must reach
			ifs >> temp;
			current->m_Values[1] = temp;
		}
		else if (current->m_Type == TRIGGER_STATUS)
		{
			// status number id
			ifs >> temp;
			current->m_Values[0] = temp;

			// 0 means has status, 1 means doesn't have status
			ifs >> temp;
			current->m_Values[1] = temp;
		}
		else if (current->m_Type == TRIGGER_MONEY)
		{
			// 0 means has less than, 1 means greater than
			ifs >> temp;
			current->m_Values[0] = temp;

			// The amount of money to have
			ifs >> temp;
			current->m_Values[1] = temp;
		}
		else if (current->m_Type == TRIGGER_PLAYERMONEY)
		{
			// 0 means has less than, 1 means greater than
			ifs >> temp;
			current->m_Values[0] = temp;

			// The amount of money to have
			ifs >> temp;
			current->m_Values[1] = temp;
		}
		else if (current->m_Type == TRIGGER_MEET)
		{
			// 0 means meet in town, 1 in catacombs, 2 means at slave market
			ifs >> temp;
			current->m_Values[0] = temp;
		}
		else if (current->m_Type == TRIGGER_TALK)
		{
			// 0 means meet in dungeon, 1 in details screen
			ifs >> temp;
			current->m_Values[0] = temp;
		}
		else if (current->m_Type == TRIGGER_WEEKSPAST)
		{
			// number of weeks that must pass
			ifs >> temp;
			current->m_Values[0] = temp;
		}
		else if (current->m_Type == TRIGGER_GLOBALFLAG)
		{
			// what global flag must be set
			ifs >> temp;
			current->m_Values[0] = temp;
		}
		else if (current->m_Type == TRIGGER_SCRIPTRUN)
		{
			// Get the 1st req script to have run
			ifs >> temp;
			current->m_Values[0] = temp;

			// Get the 2nd req script to have run
			ifs >> temp;
			current->m_Values[1] = temp;
		}

		// load the amount of times it has already been triggered
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		ifs >> temp;
		current->m_Triggered = temp;

		// load chance of triggering
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		ifs >> temp;
		current->m_Chance = temp;

		// load whether it triggers once only
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		ifs >> temp;
		current->m_Once = temp;

		// load the script name
		if (ifs.peek() == '\n') ifs.ignore(1, '\n');
		ifs.getline(buffer, 1000, '\n');
		current->m_Script = buffer;

		AddTrigger(current);
	}
}

bool cTrigger::LoadTriggerXML(TiXmlHandle hTrigger)
{
	// no need to init this, we just created it
	TiXmlElement* pTrigger = hTrigger.ToElement();
	if (pTrigger == 0) return false;

	int tempInt = 0;

	// load the type, this is an important attribute
	int success = pTrigger->QueryIntAttribute("Type", &tempInt); m_Type = tempInt; tempInt = 0;
	if (success != TIXML_SUCCESS) return false;

	// load the script name, this is an important attribute
	if (pTrigger->Attribute("Script"))
	{
		m_Script = pTrigger->Attribute("Script");
	}
	else
	{
		return false;
	}

	// load additional data
	if (m_Type == TRIGGER_SKILL)
	{
		string skill = pTrigger->Attribute("Skill");				// Check if the trigger is sending a name
		if (skill.size() > 2)
		{
			m_Values[0] = sGirl::lookup_skill_code(skill);
		}
		else  pTrigger->QueryIntAttribute("Skill", &m_Values[0]);	// otherwise get the number
		pTrigger->QueryIntAttribute("Value", &m_Values[1]);			// what value it must reach
	}
	else if (m_Type == TRIGGER_STAT)
	{
		string stat = pTrigger->Attribute("Stat");				// Check if the trigger is sending a name
		if (stat.size() > 2)
		{
			m_Values[0] = sGirl::lookup_stat_code(stat);
		}
		else  pTrigger->QueryIntAttribute("Stat", &m_Values[0]);	// otherwise get the number
		pTrigger->QueryIntAttribute("Value", &m_Values[1]);			// what value it must reach
	}
	else if (m_Type == TRIGGER_STATUS)
	{
		string status = pTrigger->Attribute("Status");				// Check if the trigger is sending a name
		if (status.size() > 2)
		{
			m_Values[0] = sGirl::lookup_status_code(status);
		}
		else  pTrigger->QueryIntAttribute("Status", &m_Values[0]);	// otherwise get the number
		pTrigger->QueryIntAttribute("Value", &m_Values[1]);			// 0 means has status, 1 means doesn't have status
	}
	else if (m_Type == TRIGGER_MONEY)
	{
		pTrigger->QueryIntAttribute("Money", &m_Values[0]);			// 0 means has less than, 1 means greater than
		pTrigger->QueryIntAttribute("Value", &m_Values[1]);			// The amount of money to have
	}
	else if (m_Type == TRIGGER_PLAYERMONEY)
	{
		pTrigger->QueryIntAttribute("PlayerMoney", &m_Values[0]);	// 0 means has less than, 1 means greater than
		pTrigger->QueryIntAttribute("Value", &m_Values[1]);			// The amount of money to have
	}
	else if (m_Type == TRIGGER_MEET)
	{
		// 0 means meet in town, 1 in catacombs, 2 means at slave market, 3 in Arena
		pTrigger->QueryIntAttribute("Meet", &m_Values[0]);
	}
	else if (m_Type == TRIGGER_TALK)
	{
		pTrigger->QueryIntAttribute("Talk", &m_Values[0]);			// 0 means meet in dungeon, 1 in details screen
	}
	else if (m_Type == TRIGGER_WEEKSPAST)
	{
		pTrigger->QueryIntAttribute("WeeksPassed", &m_Values[0]);	// number of weeks that must pass
	}
	else if (m_Type == TRIGGER_GLOBALFLAG)
	{
		pTrigger->QueryIntAttribute("Flag", &m_Values[0]);			// what global flag must be set
	}
	else if (m_Type == TRIGGER_SCRIPTRUN)
	{
		pTrigger->QueryIntAttribute("Script1", &m_Values[0]);		// Get the 1st req script to have run
		pTrigger->QueryIntAttribute("Script2", &m_Values[1]);		// Get the 2nd req script to have run
	}

	// load the amount of times it has already been triggered
	pTrigger->QueryIntAttribute("Triggered", &tempInt); m_Triggered = tempInt; tempInt = 0;

	// load chance of triggering
	pTrigger->QueryIntAttribute("Chance", &tempInt); m_Chance = tempInt; tempInt = 0;

	// load whether it triggers once only
	pTrigger->QueryIntAttribute("Once", &tempInt); m_Once = tempInt; tempInt = 0;

	return true;
}

bool cTriggerList::LoadTriggersXML(TiXmlHandle hTriggers)
{
	Free();//everything should be init even if we failed to load an XML element
	TiXmlElement* pTriggers = hTriggers.ToElement();
	if (pTriggers == 0)
	{
		return false;
	}

	m_NumTriggers = 0;
	for (TiXmlElement* pTrigger = pTriggers->FirstChildElement("Trigger"); pTrigger != 0; pTrigger = pTrigger->NextSiblingElement("Trigger"))
	{
		cTrigger* current = new cTrigger();
		bool success = current->LoadTriggerXML(TiXmlHandle(pTrigger));
		if (success == true)
		{
			AddTrigger(current);
		}
		else
		{
			delete current;
			continue;
		}
	}
	return true;
}

TiXmlElement* cTrigger::SaveTriggerXML(TiXmlElement* pRoot)
{
	TiXmlElement* pTrigger = new TiXmlElement("Trigger");
	pRoot->LinkEndChild(pTrigger);
	// save the type
	pTrigger->SetAttribute("Type", m_Type);

	// save additional data
	if (m_Type == TRIGGER_SKILL)
	{
		// skill id
		pTrigger->SetAttribute("Skill", m_Values[0]);

		// what value it must reach
		pTrigger->SetAttribute("Value", m_Values[1]);
	}
	else if (m_Type == TRIGGER_STAT)
	{
		// stat id
		pTrigger->SetAttribute("Stat", m_Values[0]);

		// what value it must reach
		pTrigger->SetAttribute("Value", m_Values[1]);
	}
	else if (m_Type == TRIGGER_STATUS)
	{
		// status number id
		pTrigger->SetAttribute("Status", m_Values[0]);

		// 0 means has status, 1 means doesn't have status
		pTrigger->SetAttribute("Value", m_Values[1]);
	}
	else if (m_Type == TRIGGER_MONEY)
	{
		// 0 means has less than, 1 means greater than
		pTrigger->SetAttribute("Money", m_Values[0]);

		// The amount of money to have
		pTrigger->SetAttribute("Value", m_Values[1]);
	}
	else if (m_Type == TRIGGER_PLAYERMONEY)
	{
		// 0 means has less than, 1 means greater than
		pTrigger->SetAttribute("PlayerMoney", m_Values[0]);

		// The amount of money to have
		pTrigger->SetAttribute("Value", m_Values[1]);
	}
	else if (m_Type == TRIGGER_MEET)
	{
		// 0 means meet in town, 1 in catacombs, 2 means at slave market
		pTrigger->SetAttribute("Meet", m_Values[0]);
	}
	else if (m_Type == TRIGGER_TALK)
	{
		// 0 means meet in dungeon, 1 in details screen
		pTrigger->SetAttribute("Talk", m_Values[0]);
	}
	else if (m_Type == TRIGGER_WEEKSPAST)
	{
		// number of weeks that must pass
		pTrigger->SetAttribute("WeeksPassed", m_Values[0]);
	}
	else if (m_Type == TRIGGER_GLOBALFLAG)
	{
		// what global flag must be set
		pTrigger->SetAttribute("Flag", m_Values[0]);
	}
	else if (m_Type == TRIGGER_SCRIPTRUN)
	{
		// Get the 1st req script to have run
		pTrigger->SetAttribute("Script1", m_Values[0]);

		// Get the 2nd req script to have run
		pTrigger->SetAttribute("Script2", m_Values[1]);
	}

	// save the amount of times it has already been triggered
	pTrigger->SetAttribute("Triggered", m_Triggered);

	// save chance of triggering
	pTrigger->SetAttribute("Chance", m_Chance);

	// save whether it triggers once only
	pTrigger->SetAttribute("Once", m_Once);

	// save the script name
	pTrigger->SetAttribute("Script", m_Script);
	return pTrigger;
}

TiXmlElement* cTriggerList::SaveTriggersXML(TiXmlElement* pRoot)
{
	TiXmlElement* pTriggers = new TiXmlElement("Triggers");
	pRoot->LinkEndChild(pTriggers);
	cTrigger* current = m_Triggers;
	while (current && m_NumTriggers != 0)
	{
		current->SaveTriggerXML(pTriggers);
		current = current->m_Next;
	}
	return pTriggers;
}

cTrigger* cTriggerList::CheckForScript(int Type, bool trigger, int values[2])
{
	cTrigger* curr = m_Triggers;
	while (curr)
	{
		if (curr->m_Type == Type)
		{
			bool match = true;

			if (values[0] != -1)
			{
				if (values[0] == curr->m_Values[0])
					match = true;
				else
					match = false;
			}

			if (values[1] != -1)
			{
				if (values[1] == curr->m_Values[1])
					match = true;
				else
					match = false;
			}

			if (trigger)
			{
				if (match)
				{
					if (curr->m_Once > 0)
					{
						if (curr->m_Triggered == curr->m_Once)
							break;
					}

					if (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance))
						AddToQue(curr);
				}
			}

			if (match)
				return curr;
		}
		curr = curr->m_Next;
	}
	return 0;
}

bool cTriggerList::HasRun(int num)
{
	cTrigger* curr = m_Triggers;
	int count = 0;

	// special case -1 is a non-existant script considered to always have run
	if (num == -1)
		return true;

	while (curr)
	{
		if (count == num)
		{
			if (curr->m_Triggered > 0)
				return true;
		}
		count++;
		curr = curr->m_Next;
	}

	return false;
}

void cTriggerList::ProcessTriggers()
{
	cTrigger* curr = m_Triggers;
	while (curr)
	{
		if (curr->m_Once > 0 && curr->m_Triggered == curr->m_Once)
		{
			curr = curr->m_Next;
			continue;
		}
		switch (curr->m_Type)
		{
		case TRIGGER_RANDOM:
		{
			if (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance))
				AddToQue(curr);
		}break;

		case TRIGGER_SKILL:
		{
			if (m_GirlTarget
				&& m_GirlTarget->get_skill(curr->m_Values[0]) >= curr->m_Values[1]
				&& (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
				AddToQue(curr);
		}break;

		case TRIGGER_STAT:
		{
			if (m_GirlTarget
				&& m_GirlTarget->get_stat(curr->m_Values[0]) >= curr->m_Values[1]
				&& (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
				AddToQue(curr);
		}break;

		case TRIGGER_PLAYERMONEY:
		{
			bool doit;
			/*
			*				values[0] says to do it when gold is < value[1]
			*				or >= value[1], depending
			*
			*				so let's note the result ofthe > comparison
			*/
			bool lt = (g_Gold.ival() < curr->m_Values[1]);
			/*
			*				now we either use that directly, or inverted
			*				depending on values[1]
			*/
			if (curr->m_Values[0] == 0)
				doit = lt;
			else
				doit = !lt;
			/*
			*				if the doit flag is set, we're done
			*				check the % - if it fails we're also done
			*/
			if (doit && g_Dice.percent(curr->m_Chance))
				AddToQue(curr);
		}break;

		case TRIGGER_MONEY:
		{
			if (!m_GirlTarget) { break; }
			bool doit;
			if (curr->m_Values[0] == 0)
				doit = (m_GirlTarget->m_Money < curr->m_Values[1]);
			else
				doit = (m_GirlTarget->m_Money >= curr->m_Values[1]);
			if (doit && g_Dice.percent(curr->m_Chance)) AddToQue(curr);
		}break;

		case TRIGGER_WEEKSPAST:
		{
			if (m_GirlTarget
				&& m_GirlTarget->m_WeeksPast >= (unsigned long)curr->m_Values[0]
				&& (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
				AddToQue(curr);
		}break;

		case TRIGGER_SCRIPTRUN:
		{
			if (HasRun(curr->m_Values[0]) && HasRun(curr->m_Values[1])
				&& (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
				AddToQue(curr);
		}break;

		case TRIGGER_STATUS:
		{
			if (m_GirlTarget)
			{
				int t = 0; if (m_GirlTarget->m_States&(1 << curr->m_Values[0])) t = 1;
				if (curr->m_Values[1] == t && (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
					AddToQue(curr);
			}
		}break;

		case TRIGGER_GLOBALFLAG:
		{
			if (CheckGameFlag(curr->m_Values[0]) && (curr->m_Chance >= 100 || g_Dice.percent(curr->m_Chance)))
				AddToQue(curr);
		}break;
		}
		curr = curr->m_Next;
	}
}

void cTriggerList::AddToQue(cTrigger* trigger)
{

	cTriggerQue* newItem = new cTriggerQue();
	newItem->m_Trigger = trigger;
	newItem->m_Next = 0;
	newItem->m_Prev = 0;
	m_TriggerQueue.push(newItem);
	/*
	if(m_StartQue)
	{
	newItem->m_Prev = m_EndQue;
	m_EndQue->m_Next = newItem;
	m_EndQue = newItem;
	}
	else
	m_StartQue = m_EndQue = newItem;
	m_NumQued++;*/
}


void cTriggerList::RemoveFromQue(cTrigger* trigger)
{//mod
	if (!m_TriggerQueue.empty())
	{
		delete m_TriggerQueue.front();
		m_TriggerQueue.pop();
	}
	//	cTriggerQue* curr = m_StartQue;
	////	while(curr)
	//	//{
	//		if(curr->m_Trigger == trigger)
	//		{
	//			if(curr->m_Prev)
	//			{//here is where it is likey
	//				curr->m_Prev->m_Next = curr->m_Next;
	//				if(curr->m_Next)
	//				{
	//					curr->m_Next->m_Prev = curr->m_Prev;
	//				curr->m_Next = 0;
	//				}
	//				delete curr;
	//				curr = 0;
	//				m_NumQued--;
	//				//break;
	//			}
	//			else
	//			{
	//
	//				m_StartQue = curr->m_Next;
	//				if(curr->m_Next)
	//					curr->m_Next = 0;
	//				delete curr;
	//				curr = 0;
	//				m_NumQued--;
	//				//break;
	//			}
	//		}
	////		curr = curr->m_Next;
	//	//}
	//
	//	if(m_NumQued == 0)
	//		m_StartQue = m_EndQue = 0;
}
//end mod
cTriggerQue* cTriggerList::GetNextQueItem()
{	//mod
	if (!m_TriggerQueue.empty())	return m_TriggerQueue.front();
	else						return 0;
	//end mod
}

void cTriggerList::Free()
{
	//mod
	while (!m_TriggerQueue.empty())
	{
		delete m_TriggerQueue.front();
		m_TriggerQueue.pop();
	}

	if (m_Triggers)	delete m_Triggers;
	m_Last = m_CurrTrigger = m_Triggers = 0;
	m_NumTriggers = 0;
	while (!m_TriggerQueue.empty())
	{
		delete m_TriggerQueue.front();
		m_TriggerQueue.pop();
	}
	/*
	if(m_StartQue)
	delete m_StartQue;
	m_StartQue=m_EndQue=0;
	*/

	if (m_GirlTarget) m_GirlTarget = 0;
	//end mod
}

void cTriggerList::ProcessNextQueItem(string fileloc)
{
	//mod
	cTriggerQue * top = m_TriggerQueue.front();
	/*
	*	nothing to do - go home
	*/
	if (!top) { return; }

	if (top->m_Trigger->m_Script != "")
	{
		cScriptManager sm;

		// Missing a path separator
		//string file = fileloc;
		//file += top->m_Trigger->m_Script;

		DirPath script = DirPath(fileloc.c_str()) << top->m_Trigger->m_Script;
		sm.Load(script, m_GirlTarget);
	}
	top->m_Trigger->m_Triggered++;
	RemoveFromQue(top->m_Trigger);
	//end mod
}

static void init_trigger_types()
{
	trigger_types["Random"] = TRIGGER_RANDOM;
	trigger_types["Shopping"] = TRIGGER_SHOPPING;
	trigger_types["Skill"] = TRIGGER_SKILL;
	trigger_types["Stat"] = TRIGGER_STAT;
	trigger_types["Status"] = TRIGGER_STATUS;
	trigger_types["Money"] = TRIGGER_MONEY;
	trigger_types["Meet"] = TRIGGER_MEET;
	trigger_types["Talk"] = TRIGGER_TALK;
	trigger_types["WeeksPast"] = TRIGGER_WEEKSPAST;
	trigger_types["GlobalFlag"] = TRIGGER_GLOBALFLAG;
	trigger_types["ScriptRun"] = TRIGGER_SCRIPTRUN;
	trigger_types["Kidnapped"] = TRIGGER_KIDNAPPED;
	trigger_types["PlayerMoney"] = TRIGGER_PLAYERMONEY;
}

static int lookup_type_code(string s)
{
	/*
	*	be useful to be able to log unrecognised
	*	type names here
	*/
	if (trigger_types.find(s) == trigger_types.end()) { return -1; }
	return trigger_types[s];
}



int cTrigger::get_type_from_xml(TiXmlElement *el)
{
	const char *pt;
	stringstream ss;
	/*
	*	find the type attribute
	*	let's be nice and allow them to miss the leading captial
	*/
	pt = el->Attribute("Type");
	/*
	*	if we didn't find it, we can't process the trigger
	*/
	if (pt == 0)
	{
		ss << "Error: Trigger with no 'Type' attribute - skipping";
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	if the attribute value is misspelled (or just wrong)
	*	then we are similarly stymied
	*/
	int code = lookup_type_code(pt);
	if (code == -1)
	{
		ss << "Error: Trigger with unrecognised type: '" << pt << "'";
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	otherwise, return the code
	*/
	return code;
}

int cTrigger::get_chance_from_xml(TiXmlElement *el)
{
	stringstream ss;
	const char *pt = el->Attribute("Chance");
	/*
	*	"not found" is not an error here
	*	we just default to 100%
	*/
	if (pt == 0) { return 100; }
	/*
	*	if we do have a string, we may need to
	*	trim away a trailing "%"
	*/
	string s = pt;
	size_t siz = s.find("%");
	if (siz != string::npos) { s.erase(siz, 1); }
	/*
	*	now, we should have a string with a number in it
	*/
	int pc = 0;
	istringstream iss(s);
	if (iss >> pc) { return pc; }
	/*
	*	If we get here, whatever is left in the
	*	string didn't look like a number
	*
	*	Let's log an error and then assume 100%
	*/
	ss << "Error: malformed chance value '" << s << "' in Trigger: assuming 100%";
	g_LogFile.write(ss.str());
	return 100;
}

bool cTrigger::get_once_from_xml(TiXmlElement *el)
{
	stringstream ss;
	const char *pt = el->Attribute("OnceOnly");
	/*
	*	"not found" is not an error here
	*	default is false
	*/
	if (pt == 0) { return false; }
	/*
	*	we now expect "True" or "False"
	*/
	string s = pt;
	if (s == "True" || s == "true") { return true; }
	if (s == "False" || s == "false") { return false; }
	/*
	*	Error handling time. Assume false, but log
	*	an error
	*/
	ss << "Error: malformed OnceOnly value '" << s << "' in Trigger: assuming False";
	g_LogFile.write(ss.str());
	return false;
}

int cTrigger::load_skill_from_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
	stringstream ss;
	/*
	*	get the attribute value
	*/
	pt = el->Attribute("Skill");
	if (!pt)
	{
		g_LogFile.write("Error: No skill specified for skill trigger: Ignoring.");
		return -1;
	}
	/*
	*	get the skill code
	*/
	int code = sGirl::lookup_skill_code(pt);
	/*
	*	make sure what we have is actually a skill name
	*/
	if (code == -1)
	{
		ss << "Error: unknown skill name '" << pt << "' in trigger definition: " << "Ignoring.";
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	set up some wrapper funcs for the values array
	*	explicitly using "this" for clarity
	*/
	this->skill(code);
	/*
	*	now the threshold value
	*/
	pt = el->Attribute("Threshold", &ival);
	if (pt == 0)
	{
		g_LogFile.write("Error: no threshold value for skill trigger");
		return -1;
	}
	this->threshold(ival);
	return 0;
}

int cTrigger::load_stat_from_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
	stringstream ss;
	/*
	*	get the attribute value
	*/
	pt = el->Attribute("Stat");
	if (!pt)
	{
		g_LogFile.write("Error: No stat specified for stat trigger: Ignoring.");
		return -1;
	}
	/*
	*	get the stat code
	*/
	int code = sGirl::lookup_stat_code(pt);
	/*
	*	make sure what we have is actually a stat name
	*/
	if (code == -1)
	{
		ss << "Error: unknown stat name '" << pt << "' in trigger definition: " << "Ignoring.";
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	set up some wrapper funcs for the values array
	*	explicitly using "this" for clarity
	*/
	this->stat(code);
	/*
	*	now the threshold value
	*/
	pt = el->Attribute("Threshold", &ival);
	if (pt == 0)
	{
		g_LogFile.write("Error: no threshold value for stat trigger");
		return -1;
	}
	this->threshold(ival);
	return 0;
}

int cTrigger::load_status_from_xml(TiXmlElement *el)
{
	const char *pt;
	stringstream ss;
	/*
	*	get the attribute value
	*/
	pt = el->Attribute("Status");
	if (!pt)
	{
		g_LogFile.write("Error: No status specified for status trigger: Ignoring.");
		return -1;
	}
	/*
	*	get the status code
	*/
	int code = sGirl::lookup_status_code(pt);
	/*
	*	make sure what we have is actually a stat name
	*/
	if (code == -1)
	{
		ss << "Error: unknown status string '" << pt << "' in trigger definition: " << "Ignoring.";
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	set up some wrapper funcs for the values array
	*	explicitly using "this" for clarity
	*/
	this->status(code);
	/*
	*	now the threshold value
	*/
	pt = el->Attribute("Has");
	if (pt == 0) {
		g_LogFile.write("Error: no 'Has' value for status trigger");
		return -1;
	}
	string s = pt;
	if (s == "True" || s == "true") { this->has(true); return 0; }
	if (s == "False" || s == "false") { this->has(false); return 0; }
	ss << "Error: unrecognised 'Has' value for stat trigger: '" << s << "' - assuming 'True'";
	g_LogFile.write(ss.str());
	this->has(true);
	return 0;
}

int cTrigger::load_money_from_xml(TiXmlElement *el)
{
	string s;
	const char *pt;
	stringstream ss;
	/*
	*	get the "Who" attribute value
	*
	*	If who is "Girl" then all is well. If
	*	it's "Player" then the type needs to be "PlayerMoney"
	*
	*	everything else gets to stay the same, though
	*/
	pt = el->Attribute("Who");
	if (!pt) {
		g_LogFile.write(
			"Error: No 'Who' attribute specified for "
			"Money trigger: Assuming 'Girl'."
			);
	}
	else if (string(pt) == "Player") {
		m_Type = lookup_type_code("PlayerMoney");
	}
	else if (string(pt) != "Girl") {
		ss << "Error: Malformed 'Who' attribute '"
			<< pt
			<< "' specified for 'Money' trigger: "
			<< "Assuming 'Girl'."
			;
		g_LogFile.write(ss.str());
	}
	/*
	*	get the comparison operation
	*/
	pt = el->Attribute("Comparison");
	if (pt == 0) {
		g_LogFile.write(
			"Error: No comparison for Money trigger:"
			"assume 'MoreThan'"
			);
		pt = "MoreThan";
	}
	s = pt;
	/*
	*	set the comparison field
	*/
	if (s == "MoreThan") {
		status(1);
	}
	else if (s == "LessThan") {
		status(0);
	}
	else  {
		ss << "Error: unsupported comparison '"
			<< pt
			<< "' in money trigger: Ignoring."
			;
		g_LogFile.write(ss.str());
		return -1;
	}
	/*
	*	now the threshold value
	*/
	int ival;
	pt = el->Attribute("Threshold", &ival);
	if (pt == 0) {
		g_LogFile.write(
			"Error: no threshold value for Money trigger"
			);
		return -1;
	}
	this->threshold(ival);
	return 0;
}

void cTrigger::load_meet_from_xml(TiXmlElement *el)
{
	string s;
	const char *pt;
	stringstream ss;
	/*
	*	get the "Where" attribute value
	*/
	pt = el->Attribute("Where");
	if (!pt)
	{
		g_LogFile.write("Error: No 'Where' attribute specified for 'Meet' trigger: Assuming 'Town'.");
		pt = "Town";
	}
	/*
	*	set the where field
	*/
	int rc = where(s = pt);
	/*
	*	if that failed, log an error
	*	and then default to "Town"
	*/
	if (rc < 0)
	{
		ss << "Error: malformed 'Where' attribute '" << s << "' for 'Meet' trigger: Assuming 'Town'.";
		g_LogFile.write(ss.str());
		where("Town");
	}

	if (rc > 3)
	{
		ss << "Error: malformed 'Where' attribute '" << s << "' for 'Meet' trigger: Assuming 'Arena'.";
		g_LogFile.write(ss.str());
		where("Arena");
	}
}

void cTrigger::load_talk_from_xml(TiXmlElement *el)
{
	string s;
	const char *pt;
	stringstream ss;
	/*
	*	get the "Where" attribute value
	*/
	pt = el->Attribute("Where");
	if (!pt) {
		g_LogFile.write("Error: No 'Where' attribute specified for 'Talk' trigger: Assuming 'Brothel'.");
		pt = "Brothel";
	}
	/*
	*	set the where field
	*/
	int rc = where(s = pt);
	/*
	*	if that failed, log an error
	*	and then default to "Brothel"
	*/
	if (rc == -1) {
		ss << "Error: malformed 'Where' attribute '" << s << "' for 'Talk' trigger: Assuming 'Brothel'.";
		g_LogFile.write(ss.str());
		where("Brothel");
	}
}

int cTrigger::load_weeks_from_xml(TiXmlElement *el)
{
	int ival;
	string s;
	const char *pt;
	stringstream ss;
	/*
	*	get the "Threshold" attribute value
	*/
	pt = el->Attribute("Threshold", &ival);
	if (!pt) {
		g_LogFile.write("Error: No 'Threshold' attribute specified for 'WeeksPast' trigger: ignoring");
		return -1;
	}
	/*
	*	set the where field
	*/
	threshold(ival);
	return 0;
}

int cTrigger::load_flag_from_xml(TiXmlElement *el)
{
	string s;
	const char *pt;
	stringstream ss;
	/*
	*	get the "Flag" attribute value
	*/
	pt = el->Attribute("Flag");
	if (!pt) {
		g_LogFile.write("Error: No 'Flag' attribute specified for 'GlobalFlag' trigger: ignoring");
		return -1;
	}
	/*
	*	set the where field
	*/
	if (global_flag(pt) >= 0) { return 0; }
	/*
	*	I don't think there's a sensible default, here
	*/
	ss << "Error: Malformed GlobalFlag '" << pt << "'in Trigger: Ignoring";
	g_LogFile.write(ss.str());
	return -1;
}

int cTrigger::load_from_xml(TiXmlElement *el)
{
	const char *pt;
	stringstream ss;
	/*
	*	make sure the maps are intialised
	*/
	if (trigger_types.size() == 0) { init_trigger_types(); }
	/*
	*	get the trigger type
	*/
	int code = get_type_from_xml(el);
	if (code == -1) { return -1; }	// error already reported
	m_Type = code;
	/*
	*	OK, get the script file
	*/
	pt = el->Attribute("File");
	if (pt == 0)
	{
		g_LogFile.write("Error: can't find script file for trigger");
		return -1;
	}
	m_Script = pt;
	/*
	*	Chance: this is allowed to be missing, and defaults to 100
	*/
	m_Chance = get_chance_from_xml(el);
	m_Once = get_once_from_xml(el);
	/*
	*	there's scope to set this in the config file
	*	but I can't for the life of me see why this would be useful
	*/
	m_Triggered = 0;
	/*
	*	OK, that's the easy bits. Everything else depends on
	*	the type field
	*/
	switch (m_Type) {
	case TRIGGER_SKILL:
		if (load_skill_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_STAT:
		if (load_stat_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_STATUS:
		if (load_status_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_MONEY:
		if (load_money_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_MEET:
		load_meet_from_xml(el);		break;
	case TRIGGER_TALK:
		load_talk_from_xml(el);		break;
	case TRIGGER_WEEKSPAST:
		if (load_weeks_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_GLOBALFLAG:
		if (load_flag_from_xml(el) == -1) { return -1; }
		break;
	case TRIGGER_SCRIPTRUN:		return -1;
		break;
	}
	return 0;
}

void cTriggerList::LoadList(string filename)
{
	TiXmlDocument doc(filename);
	
	if (!doc.LoadFile())
	{
		if (cfg.debug.log_girls())
		{
			if (!doc.ErrorRow() && !doc.ErrorCol())
			{
				cerr << "Girl has no script trigger file: " << filename << endl;
				return;
			}
			cerr << "can't load script trigger list " << filename << endl;
			cerr << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		}
		return;
	}
	/*
	*	get the docuement root
	*/
	TiXmlElement *el, *root_el = doc.RootElement();
	/*
	*	loop over the elements attached to the root
	*/
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		cTrigger *trigger = new cTrigger();
		/*
		*		load the trigger data. If there was no error,
		*		add it to the list and move on to the next one
		*/
		if (trigger->load_from_xml(el) == 0)
		{
			AddTrigger(trigger);
			continue;
		}
		/*
		*		this is the error condition.
		*		We should free up the trigger memory
		*/
		delete trigger;
	}
}
