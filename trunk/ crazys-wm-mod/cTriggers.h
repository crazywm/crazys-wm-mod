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
#ifndef __CTRIGGERS_H
#define __CTRIGGERS_H

#include <string>
#include <queue>
#include "GameFlags.h"
#include "tinyxml.h"
struct sGirl;
using namespace std;

// girl specific triggers
const int TRIGGER_RANDOM = 0;	// May trigger each week
const int TRIGGER_SHOPPING = 1;	// May trigger when shopping
const int TRIGGER_SKILL = 2;	// May trigger when a skill is greater or equal to a value
const int TRIGGER_STAT = 3;		// same as skill
const int TRIGGER_STATUS = 4;	// has a particular status, ie slave, pregnant etc
const int TRIGGER_MONEY = 5;	// same as skill or stat levels
const int TRIGGER_MEET = 6;		// Triggers when meeting girl
const int TRIGGER_TALK = 7;		// triggered when talking to girl in dungeon on details screen
const int TRIGGER_WEEKSPAST = 8;	// certain number of weeks pass while girl is in employment
const int TRIGGER_GLOBALFLAG = 9;	// triggered when a global flag is set
const int TRIGGER_SCRIPTRUN = 10;	// triggered when a specifed script has been run
const int TRIGGER_KIDNAPPED = 11;	// triggers when a girl is kidnaped
const int TRIGGER_PLAYERMONEY = 12;	// triggers when players money hits a value

class cTrigger
{
public:
	string m_Script;			// the scripts filename
	unsigned char m_Type;		// the type of trigger
	unsigned char m_Triggered;	// 1 means this trigger has triggered already
	unsigned char m_Chance;		// Percent chance of occuring
	unsigned char m_Once;		// if 1 then this trigger will only work once, from then on it doesn't work
	int m_Values[3];			// values used for the triggers

	cTrigger* m_Next;

	cTrigger() {m_Type=m_Triggered=m_Chance=m_Once=0;m_Next=0;}
	~cTrigger() {if(m_Next)delete m_Next;m_Next=0;}

	TiXmlElement* SaveTriggerXML(TiXmlElement* pRoot);
	bool LoadTriggerXML(TiXmlHandle hTrigger);

	bool	get_once_from_xml(TiXmlElement*);
	int	get_type_from_xml(TiXmlElement*);
	int	get_chance_from_xml(TiXmlElement*);
	int	load_skill_from_xml(TiXmlElement*);
	int	load_stat_from_xml(TiXmlElement*);
	int	load_status_from_xml(TiXmlElement*);
	int	load_from_xml(TiXmlElement *el);
	int	load_money_from_xml(TiXmlElement *el);
	void	load_meet_from_xml(TiXmlElement *el);
	void	load_talk_from_xml(TiXmlElement *el);
	int	load_weeks_from_xml(TiXmlElement *el);
	int	load_flag_from_xml(TiXmlElement *el);

/*
 *	some accessor funcs to make the meaning of the values 
 *	array elements a little less opaque
 */
	int global_flag()	{ return m_Values[0]; }
	int global_flag(int n)	{ return m_Values[0] = n; }
	int global_flag(string s) {
		if(s == "NoPay") {
			return m_Values[0] = FLAG_CUSTNOPAY;
		}
		if(s == "GirlDies") {
			return m_Values[0] = FLAG_DUNGEONGIRLDIE;
		}
		if(s == "CustomerDies") {
			return m_Values[0] = FLAG_DUNGEONCUSTDIE;
		}
		if(s == "GamblingCheat") {
			return m_Values[0] = FLAG_CUSTGAMBCHEAT;
		}
		if(s == "RivalLose") {
			return m_Values[0] = FLAG_RIVALLOSE;
		}
		return -1;
	}
	int where()		{ return m_Values[0]; }
	int where(int n)	{
		return m_Values[0] = n;
	}
	int where(string s) {
		if(s == "Town" || s == "Dungeon") {
			return where(0);
		}
		if(s == "Catacombs" || s == "Brothel") {
			return where(1);
		}
		if(s == "SlaveMarket") {
			return where(2);
		}
		if(s == "Arena") {
			return where(3);
		}
		return -1;
	}
	int status()		{ return m_Values[0]; }
	int status(int n)	{
		return m_Values[0] = n;
	}
	int stat()		{ return m_Values[0]; }
	int stat(int n)	{
		return m_Values[0] = n;
	}
	int skill()		{ return m_Values[0]; }
	int skill(int n)	{
		return m_Values[0] = n;
	}
	int has()		{ return m_Values[1]; }
	int has(int n)	{
		return m_Values[1] = n;
	}
	int threshold()		{ return m_Values[1]; }
	int threshold(int n)	{
		return m_Values[1] = n;
	}
};

class cTriggerQue
{
public:
	cTrigger* m_Trigger;	// the trigger that needs to be triggered
	cTriggerQue* m_Next;	// the next one in the que
	cTriggerQue* m_Prev;	// the previous one in the que

	cTriggerQue() {m_Trigger=0;m_Next=m_Prev=0;}
	~cTriggerQue() {if(m_Next)delete m_Next;m_Prev=m_Next=0;m_Trigger=0;}
};

class cTriggerList
{
public:
	cTriggerList() {m_Triggers=0;m_CurrTrigger=0;m_Last=0;/*m_StartQue=m_EndQue=0;m_NumQued=0;*/m_NumTriggers=0;m_GirlTarget=0;}
	~cTriggerList() {Free();}

	void Free();
	void LoadList(string filename);
	TiXmlElement* SaveTriggersXML(TiXmlElement* pRoot);
	bool LoadTriggersXML(TiXmlHandle hTriggers);
	void LoadTriggersLegacy(ifstream& ifs);

	void AddTrigger(cTrigger* trigger);

	void AddToQue(cTrigger* trigger);
	void RemoveFromQue(cTrigger* trigger);
	cTriggerQue* GetNextQueItem();

	cTrigger* CheckForScript(int Type, bool trigger, int values[2]);

	void ProcessTriggers();	// function that process the triggers in the list and adds them to the que if the conditions are met
	void ProcessNextQueItem(string fileloc);

	// set script targets
	void SetGirlTarget(sGirl* girl){m_GirlTarget = girl;}

	bool HasRun(int num);

private:
	cTrigger* m_CurrTrigger;
	cTrigger* m_Triggers;
	cTrigger* m_Last;
	int m_NumTriggers;

	//int m_NumQued;
	//cTriggerQue* m_StartQue;
	//cTriggerQue* m_EndQue;
	queue<cTriggerQue *> m_TriggerQueue;//mod

	
	// script targets (things that the script will affect with certain commands)
	sGirl* m_GirlTarget;	// if not 0 then the script is affecting a girl
};

#endif
