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
#include <sstream>

#ifdef LINUX
#include "linux.h"
#endif

#include "cDungeon.h"
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cGangs.h"
#include "strnatcmp.h"
#include "cGirlTorture.h"
#include "libintl.h"

extern cMessageQue		g_MessageQue;
extern cGirls			g_Girls;
extern cBrothelManager	g_Brothels;
extern cRng				g_Dice;
extern cGangManager		g_Gangs;


// strut sDungeonCust
sDungeonCust::sDungeonCust()		// constructor
{
	m_Prev = m_Next = 0;
	m_Weeks = 0;
	m_Tort = false;
	m_Feeding = true;
	m_Health = 100;
}

sDungeonCust::~sDungeonCust()		// destructor
{
	if (m_Next) delete m_Next;
	m_Next = 0;
}

// strut sDungeonGirl
sDungeonGirl::sDungeonGirl()		// constructor
{
	m_Girl = 0;
	m_Prev = m_Next = 0;
	m_Weeks = 0;
	m_Feeding = true;
}

sDungeonGirl::~sDungeonGirl() 		// destructor
{
	if (m_Girl) delete m_Girl;
	m_Girl = 0;
	if (m_Next) delete m_Next;
	m_Next = 0;
}

// class cDungeon
cDungeon::cDungeon()		// constructor
{
	m_LastDGirl = m_Girls = 0;
	m_LastDCusts = m_Custs = 0;
	m_NumberDied = 0;
	m_NumGirls = m_NumCusts = 0;
}

cDungeon::~cDungeon()		// destructor
{
	Free();
}

void cDungeon::Free()
{
	if (m_Girls) delete m_Girls;
	m_LastDGirl = m_Girls = 0;
	if (m_Custs) delete m_Custs;
	m_LastDCusts = m_Custs = 0;
	m_NumberDied = 0;
	m_NumGirls = m_NumCusts = 0;
}

TiXmlElement* cDungeon::SaveDungeonDataXML(TiXmlElement* pRoot)// saves all the people (they are stored with the dungeon)
{
	TiXmlElement* pDungeon = new TiXmlElement("Dungeon");
	pRoot->LinkEndChild(pDungeon);

	// save number died
	pDungeon->SetAttribute("NumberDied", m_NumberDied);

	// save girls
	TiXmlElement* pDungeonGirls = new TiXmlElement("Dungeon_Girls");
	pDungeon->LinkEndChild(pDungeonGirls);
	sDungeonGirl* girl = m_Girls;
	stringstream ss;
	while (girl)
	{
		ss << "Saving Dungeon Girl: " << girl->m_Girl->m_Realname;
		g_LogFile.write(ss.str()); ss.str("");

		girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_INDUNGEON;
		TiXmlElement* pGirl = girl->m_Girl->SaveGirlXML(pDungeonGirls);

		TiXmlElement* pDungeonData = new TiXmlElement("Dungeon_Data");
		pGirl->LinkEndChild(pDungeonData);
		pDungeonData->SetAttribute("Feeding", girl->m_Feeding);
		pDungeonData->SetAttribute("Reason", girl->m_Reason);
		pDungeonData->SetAttribute("Weeks", girl->m_Weeks);

		girl = girl->m_Next;
	}

	ss << "Saving Customers";
	g_LogFile.write(ss.str()); ss.str("");
	// save customers
	TiXmlElement* pDungeonCustomers = new TiXmlElement("Dungeon_Customers");
	pDungeon->LinkEndChild(pDungeonCustomers);
	sDungeonCust* cust = m_Custs;
	while (cust)
	{
		TiXmlElement* pCustomer = new TiXmlElement("Customer");
		pDungeonCustomers->LinkEndChild(pCustomer);
		pCustomer->SetAttribute("Feeding", cust->m_Feeding);
		pCustomer->SetAttribute("Tort", cust->m_Tort);
		pCustomer->SetAttribute("HasWife", cust->m_HasWife);
		pCustomer->SetAttribute("NumDaughters", cust->m_NumDaughters);
		pCustomer->SetAttribute("Reason", cust->m_Reason);
		pCustomer->SetAttribute("Weeks", cust->m_Weeks);
		pCustomer->SetAttribute("Health", cust->m_Health);

		cust = cust->m_Next;
	}
	return pDungeon;
}


bool cDungeon::LoadDungeonDataXML(TiXmlHandle hDungeon)	// loads all the people (they are stored with the dungeon)
{
	Free();//everything should be init even if we failed to load an XML element
	TiXmlElement* pDungeon = hDungeon.ToElement();
	if (pDungeon == 0) { return false; }

	// load number died
	pDungeon->QueryValueAttribute<unsigned long>("NumberDied", &m_NumberDied);

	// load girls
	m_NumGirls = 0;
	stringstream ss;
	TiXmlElement* pDungeonGirls = pDungeon->FirstChildElement("Dungeon_Girls");
	if (pDungeonGirls)
	{
		// load each girl and add her
		for (TiXmlElement* pGirl = pDungeonGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
		{
			sDungeonGirl* girl = new sDungeonGirl();
			girl->m_Girl = new sGirl();
			bool success = girl->m_Girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				ss << "Loading Dungeon Girl: " << girl->m_Girl->m_Realname;
				g_LogFile.write(ss.str()); ss.str("");
			//	girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_INDUNGEON;

				TiXmlElement* pDungeonData = pGirl->FirstChildElement("Dungeon_Data");
				if (pDungeonData)
				{
					pDungeonData->QueryValueAttribute<bool>("Feeding", &girl->m_Feeding);
					pDungeonData->QueryIntAttribute("Reason", &girl->m_Reason);
					pDungeonData->QueryIntAttribute("Weeks", &girl->m_Weeks);
				}

				PlaceDungeonGirl(girl);
			}
			else
			{
				delete girl;
				continue;
			}
		}
	}

	// load customers
	m_NumCusts = 0;
	ss.str("");
	ss << "Loading customers";
	g_LogFile.write(ss.str());
	TiXmlElement* pDungeonCustomers = pDungeon->FirstChildElement("Dungeon_Customers");
	if (pDungeonCustomers)
	{
		for (TiXmlElement* pCustomer = pDungeonCustomers->FirstChildElement("Customer"); pCustomer != 0; pCustomer = pCustomer->NextSiblingElement("Customer"))
		{
			sDungeonCust* customer = new sDungeonCust();

			pCustomer->QueryValueAttribute<bool>("Feeding", &customer->m_Feeding);
			pCustomer->QueryValueAttribute<bool>("Tort", &customer->m_Tort);
			pCustomer->QueryValueAttribute<bool>("HasWife", &customer->m_HasWife);
			pCustomer->QueryIntAttribute("Reason", &customer->m_Reason);
			pCustomer->QueryIntAttribute("NumDaughters", &customer->m_NumDaughters);
			pCustomer->QueryIntAttribute("Weeks", &customer->m_Weeks);
			pCustomer->QueryIntAttribute("Health", &customer->m_Health);

			PlaceDungeonCustomer(customer);
		}
	}
	return true;
}

void cDungeon::AddGirl(sGirl* girl, int reason)
{
	if (reason == DUNGEON_GIRLKIDNAPPED)
	{
		if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
		{
			g_Brothels.GetObjective()->m_SoFar++;
		}
	}

	girl->m_InStudio = girl->m_InCentre = girl->m_InClinic = girl->m_InHouse = girl->m_InArena = false;	// `J`
	girl->where_is_she = 0;	// `J`
	girl->m_DayJob = girl->m_NightJob = JOB_INDUNGEON;

	// by this stage they should no longer be a part of any other lists of girls
	sDungeonGirl* newPerson = new sDungeonGirl();
	newPerson->m_Reason = reason;
	newPerson->m_Girl = girl;

	// remove from girl manager if she is there
	g_Girls.RemoveGirl(girl);

	// remove girl from brothels if she is there
	for (int i = 0; i < g_Brothels.GetNumBrothels(); i++)
	{
		g_Brothels.RemoveGirl(i, girl, false);
	}

	PlaceDungeonGirl(newPerson);
}

void cDungeon::PlaceDungeonGirl(sDungeonGirl* newGirl)
{
	if (m_Girls)
	{
		m_LastDGirl->m_Next = newGirl;
		newGirl->m_Prev = m_LastDGirl;
		m_LastDGirl = newGirl;
	}
	else
	{
		m_LastDGirl = m_Girls = newGirl;
	}

	m_NumGirls++;
}

void cDungeon::AddCust(int reason, int numDaughters, bool hasWife)
{
	sDungeonCust* newPerson = new sDungeonCust();
	newPerson->m_NumDaughters = numDaughters;
	newPerson->m_HasWife = hasWife;
	newPerson->m_Reason = reason;

	PlaceDungeonCustomer(newPerson);
}

void cDungeon::PlaceDungeonCustomer(sDungeonCust *newCust)
{
	if (m_Custs)
	{
		newCust->m_Prev = m_LastDCusts;
		m_LastDCusts->m_Next = newCust;
		m_LastDCusts = newCust;
	}
	else
	{
		m_LastDCusts = m_Custs = newCust;
	}

	m_NumCusts++;
}

int cDungeon::GetGirlPos(sGirl* girl)
{
	if (girl == 0 || m_Girls == 0) return -1;

	sDungeonGirl* current = m_Girls;
	int count = 0;
	while (current)
	{
		if (current->m_Girl == girl) break;
		count++;
		current = current->m_Next;
	}
	if (current == 0) return -1;
	return count;
}

sGirl* cDungeon::RemoveGirl(sGirl* girl)	// this returns the girl, it must be placed somewhere or deleted
{
	sDungeonGirl* current = m_Girls;
	while (current)
	{
		if (current->m_Girl == girl) break;
		current = current->m_Next;
	}
	if (current) return RemoveGirl(current);
	return 0;
}

sGirl* cDungeon::RemoveGirl(sDungeonGirl* girl)	// this returns the girl, it must be placed somewhere or deleted
{
	girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_RESTING;
	if (girl->m_Next)			girl->m_Next->m_Prev = girl->m_Prev;
	if (girl->m_Prev)			girl->m_Prev->m_Next = girl->m_Next;
	if (girl == m_LastDGirl)	m_LastDGirl = girl->m_Prev;
	if (girl == m_Girls)		m_Girls = girl->m_Next;

	sGirl* girlData = girl->m_Girl;
	girl->m_Next = girl->m_Prev = 0;
	girl->m_Girl = 0;

	m_NumGirls--;

	delete girl;
	girl = 0;

	return girlData;
}

void cDungeon::RemoveCust(sDungeonCust* cust)
{
	if (cust == 0) return;
	if (cust->m_Prev)			cust->m_Prev->m_Next = cust->m_Next;
	if (cust->m_Next)			cust->m_Next->m_Prev = cust->m_Prev;
	if (cust == m_LastDCusts)	m_LastDCusts = cust->m_Prev;
	if (cust == m_Custs)		m_Custs = cust->m_Next;
	cust->m_Next = cust->m_Prev = 0;
	delete cust;
	cust = 0;
	m_NumCusts--;
}

void cDungeon::OutputGirlRow(int i, string* Data, const vector<string>& columnNames)
{
	sDungeonGirl* girl = m_Girls;
	int tmp = 0;
	while (girl)
	{
		if (tmp == i) break;
		tmp++;
		girl = girl->m_Next;
	}
	if (girl)
	{
		for (unsigned int x = 0; x < columnNames.size(); ++x)
		{
			//for each column, write out the statistic that goes in it
			girl->OutputGirlDetailString(Data[x], columnNames[x]);
		}
	}
}

void sDungeonGirl::OutputGirlDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");
	if (detailName == "Rebelliousness")	// `J` Dungeon "Matron" can be a Torturer from any brothel
	{
		ss << g_Girls.GetRebelValue(m_Girl, (
			g_Brothels.GetNumGirlsOnJob(0, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(1, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(2, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(3, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(4, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(5, JOB_TORTURER, 0) > 0 ||
			g_Brothels.GetNumGirlsOnJob(6, JOB_TORTURER, 0) > 0));
	}
	else if (detailName == "Reason")
	{
		switch (m_Reason)
		{
		case DUNGEON_GIRLCAPTURED:				ss << "Newly Captured.";					break;
		case DUNGEON_GIRLKIDNAPPED:				ss << "Taken from her family.";			break;
		case DUNGEON_GIRLWHIM:					ss << "Your whim.";						break;
		case DUNGEON_GIRLSTEAL:					ss << "Not reporting true earnings.";		break;
		case DUNGEON_GIRLRUNAWAY:				ss << "Ran away and re-captured.";			break;
		case DUNGEON_NEWSLAVE:					ss << "This is a new slave.";				break;
		case DUNGEON_NEWGIRL:					ss << "This is a new girl.";				break;
		case DUNGEON_KID:						ss << "Child of one of your girls.";		break;
		case DUNGEON_NEWARENA:					ss << "This is a girl won in the arena.";	break;
		case DUNGEON_RECRUITED:					ss << "This girl was recruited for you.";	break;
		}
	}
	else if (detailName == "Duration")			{ ss << m_Weeks; }
	else if (detailName == "Feeding")			{ ss << ((m_Feeding) ? "Yes" : "No"); }
	else if (detailName == "Tortured")			{ ss << ((m_Girl->m_Tort) ? "Yes" : "No"); }
	else if (detailName == "Kidnapped")
	{
		int duration = g_Girls.HasTempTrait(m_Girl, "Kidnapped");
		if (duration > 0) ss << duration;
		else ss << "-";
	}
	else
	{
		m_Girl->OutputGirlDetailString(Data, detailName);
		return;
	}

	Data = ss.str();
}

void cDungeon::OutputCustRow(int i, string* Data, const vector<string>& columnNames)
{
	sDungeonCust* cust = m_Custs;
	int tmp = 0;
	while (cust)
	{
		if (tmp == i) break;
		tmp++;
		cust = cust->m_Next;
	}
	if (cust)
	{
		for (unsigned int x = 0; x < columnNames.size(); ++x)
		{
			//for each column, write out the statistic that goes in it
			cust->OutputCustDetailString(Data[x], columnNames[x]);
		}
	}
}

void sDungeonCust::OutputCustDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");
	if (detailName == "Name")					{ ss << "Customer"; }
	else if (detailName == "Health")	{ if (m_Health <= 0) ss << "DEAD"; else ss << m_Health << "%"; }
	else if (detailName == "Reason")
	{
		switch (m_Reason)
		{
		case DUNGEON_CUSTNOPAY:			ss << "Not paying.";			break;
		case DUNGEON_CUSTBEATGIRL:		ss << "Beating your girls.";	break;
		case DUNGEON_CUSTSPY:			ss << "Being a rival's spy.";	break;
		case DUNGEON_RIVAL:				ss << "Is a rival.";			break;
		}
	}
	else if (detailName == "Duration")			{ ss << (int)m_Weeks; }
	else if (detailName == "Feeding")			{ ss << ((m_Feeding) ? "Yes" : "No"); }
	else if (detailName == "Tortured")			{ ss << ((m_Tort) ? "Yes" : "No"); }
	else
	{
		ss << "---";
	}
	Data = ss.str();
}

sDungeonGirl* cDungeon::GetGirl(int i)
{
	if (i < 0)				{ i = i + m_NumGirls; }
	if (i >= m_NumGirls)	{ i = i - m_NumGirls; }

	sDungeonGirl* girl = m_Girls;
	int tmp = 0;
	while (girl)
	{
		if (tmp == i) break;
		tmp++;
		girl = girl->m_Next;
	}
	return girl;
}

sDungeonGirl* cDungeon::GetGirlByName(string name)
{
	if (name == "") return 0;
	sDungeonGirl* currentGirl = m_Girls;
	while (currentGirl)
	{
		if (strnatcmp(name.c_str(), currentGirl->m_Girl->m_Realname.c_str()) == 0)
			return currentGirl;
		currentGirl = currentGirl->m_Next;
	}
	return 0;
}

int cDungeon::GetDungeonPos(sGirl* girl)
{
	sDungeonGirl* tgirl = m_Girls;
	int tmp = 0;
	while (tgirl)
	{
		if (tgirl->m_Girl == girl) break;
		tmp++;
		tgirl = tgirl->m_Next;
	}
	return tmp;
}

sDungeonCust* cDungeon::GetCust(int i)
{
	sDungeonCust* cust = m_Custs;
	int tmp = 0;
	while (cust)
	{
		if (tmp == i) break;
		tmp++;
		cust = cust->m_Next;
	}
	return cust;
}

void cDungeon::ClearDungeonGirlEvents()
{
	if (m_Girls)
	{
		sDungeonGirl* current = m_Girls;
		while (current)
		{
			sGirl* girl = current->m_Girl;
			// Clear the girls' events from the last turn
			girl->m_Events.Clear();
			current = current->m_Next;
		}
	}
}


void cDungeon::Update()
{
	/*
	*	WD: GetNumGirlsOnJob() not testing if the girl worked
	*
	*/
	sGirl* TorturerGirlref = 0;
	string girlName;
	stringstream msg;
	stringstream ss;

	// Reser counters
	m_NumGirlsTort = m_NumCustsTort = 0;

	// WD:	Did we torture the girls
	bool tort = g_Brothels.TortureDone();
	// WD: If so, who is the Torturer
	if (tort) { TorturerGirlref = g_Brothels.WhoHasTorturerJob(); }

	/*********************************
	*	DO ALL DUNGEON GIRLS
	*********************************/
	if (m_Girls)
	{

		sDungeonGirl* current = m_Girls;
		while (current)
		{
			sGirl* girl = current->m_Girl;

			//			girl->m_Tort = false;// WD: Move till after Girls have been tortured so that we dont torture twice week
			girlName = girl->m_Realname;
			ss.str("");
			msg.str("");

			// Check for dead girls
			if (girl->is_dead())
			{
				// remove dead bodies from last week
				if (current->m_Reason == DUNGEON_DEAD)
				{
					sDungeonGirl* temp = current;
					current = current->m_Next;
					msg << girlName << "'s body has been removed from the dungeon since she was dead.";
					g_MessageQue.AddToQue(msg.str(), COLOR_RED);
					delete RemoveGirl(temp);
					continue;
				}
				// Mark as dead
				else
				{
					m_NumberDied++;
					current->m_Reason = DUNGEON_DEAD;
					SetGameFlag(FLAG_DUNGEONGIRLDIE);
					continue;
				}
			}
			/*
			*			DAILY Processing
			*/
			string summary = "";

			current->m_Weeks++;						// the number of weeks they have been in the dungeon
			g_Girls.CalculateGirlType(girl);		// update the fetish traits
			g_Girls.updateGirlAge(girl, true);		// update birthday counter and age the girl
			g_Girls.updateTemp(girl);			// update temp stuff
			g_Girls.EndDayGirls(g_Brothels.GetBrothel(0), girl);
			g_Girls.HandleChildren(girl, summary);	// handle pregnancy and children growing up
			g_Girls.updateSTD(girl);				// health loss to STD's - NOTE: Girl can die
			g_Girls.updateHappyTraits(girl);		// Update happiness due to Traits - NOTE: Girl can die
			updateGirlTurnDungeonStats(current);	// Update stats
			g_Girls.updateGirlTurnStats(girl);		// Stat Code common to Dugeon and Brothel

			// Check again for dead girls
			if (girl->health() <= 0)
			{
				m_NumberDied++;
				current->m_Reason = DUNGEON_DEAD;
				SetGameFlag(FLAG_DUNGEONGIRLDIE);

				msg.str(""); ss.str("");
				msg << girlName << " has died in the dungeon.";
				girl->m_Events.AddMessage(msg.str(), IMGTYPE_DEATH, EVENT_DANGER);
				ss << girlName << " has died.  Her body will be removed by the end of the week.\n";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_SUMMARY);

				// if there is a torturer send her a message
				if (tort)
				{
					msg.str("");
					msg << girlName << " has died in the dungeon under her care!";
					TorturerGirlref->m_Events.AddMessage(msg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
				}

				current = current->m_Next;
				continue;
			}

			// Have dungeon girls tortured by the Torturer
			if (tort)
			{
				cGirlTorture gt(current, TorturerGirlref);
			}

			/*		`J` merged WARNING MESSAGES and SUMMARY MESSAGES
			*			Allow girl sorting in turn summary
			*/
			//	`J` set the basics
			msg.str(""); ss.str("");
			msg << girlName << " is languishing in the dungeon.\n \n";
			int msgtype = EVENT_DUNGEON;
			int imgtype = IMGTYPE_JAIL;
			int	nHealth = girl->health();
			int	nTired = girl->tiredness();

			//	`J` check them for dangers or warnings
			if (nHealth < 20 || nTired > 80)
			{
				msg << "DANGER: " << girlName;
				msgtype = EVENT_DANGER;
			}
			else if (nHealth < 40 || nTired > 60)
			{
				msg << "WARNING: " << girlName;
				msgtype = EVENT_WARNING;
			}

			//	`J` did msgtype change?
			if (msgtype != EVENT_DUNGEON)
			{
				if (girl->m_Tort)
				{
					msg << " was tortured this week.";
					imgtype = IMGTYPE_TORTURE;
					if (nHealth < 40 || nTired > 60)	{ msg << "\nShe"; }
				}
				if (nHealth < 20)						{ msg << " is severely injured"; }
				else if (nHealth < 40)					{ msg << " is injured"; }
				if (nHealth < 40 && nTired > 60)		{ msg << " and"; }
				else if (nTired > 60)					{ msg << " is"; }
				else									{ msg << "."; }
				if (nTired > 80)						{ msg << " exhausted, it may effect her health."; }
				else if (nTired > 60)					{ msg << " tired."; }
				msg << "\n \nHer health is " << nHealth << ".\nHer tiredness is " << nTired << ".";
			}
			girl->m_Events.AddMessage(msg.str(), imgtype, msgtype);

			girl->m_Tort = false;

			// loop next dungeon girl
			current = current->m_Next;
		}

		/*
		*			WD:  Torturer Girl summary
		*				Processed after all dGirls
		*/
		if (tort)
		{
			msg.str("");
			msg << TorturerGirlref->m_Realname << " has tortured " << m_NumGirlsTort << " girls in the Dungeon.";
			TorturerGirlref->m_Events.AddMessage(msg.str(), IMGTYPE_DOM, EVENT_DUNGEON);

		}
	}


	/*********************************
	*	DO ALL CUSTOMERS
	*********************************/
	if (m_Custs)
	{
		sDungeonCust* current = m_Custs;
		while (current)
		{
			current->m_Tort = false;
			if (current->m_Health <= 0)
			{
				m_NumberDied++;
				current->m_Reason = DUNGEON_DEAD;
				SetGameFlag(FLAG_DUNGEONCUSTDIE);
			}
			if (current->m_Reason == DUNGEON_DEAD)
			{
				sDungeonCust* temp = current;
				current = current->m_Next;
				RemoveCust(temp);
				continue;
			}

			/*
			*			lose health if not feeding
			*
			*			Mod: removed "no-effect" branch to silence
			*			compiler
			*/
			if (!current->m_Feeding) current->m_Health -= 5;

			current->m_Weeks++;
			if (current->m_Health <= 0)
			{
				m_NumberDied++;
				current->m_Reason = DUNGEON_DEAD;
				SetGameFlag(FLAG_DUNGEONCUSTDIE);
			}
			current = current->m_Next;
		}
	}
}

void cDungeon::updateGirlTurnDungeonStats(sDungeonGirl* d_girl)
{
	/*
	*	WD: Update each turn the stats for girls in dudgeon
	*/
//#define WDTEST // debuging
#undef WDTEST

	sGirl* girl = d_girl->m_Girl;
	string msg;
	string girlName = girl->m_Realname;

	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;

#ifdef WDTEST // debuging
	stringstream sum;
	sum << "Start\n"
		<< "   h=" << girl->happiness()
		<< "   o=" << girl->obedience()
		<< "   l=" << girl->pclove()
		<< "   f=" << girl->pcfear()
		<< "   h=" << girl->pchate()
		<< "  HP=" << girl->health()
		<< "  TD=" << girl->tiredness();
#endif

	if (d_girl->m_Feeding)
	{
		if (girl->is_slave())	// Slave being fed
		{
			girl->confidence(-2);
			girl->obedience(2);
			girl->spirit(-2);
			girl->pchate(1);
			girl->pclove(-1);
			girl->pcfear(4);
			girl->tiredness(-10);
			girl->happiness(-1);
			girl->health(4);
			girl->mana(5);
			girl->bdsm(1);
		}
		else	// Free girl being fed
		{
			girl->confidence(-1);
			girl->obedience(1);
			girl->spirit(-1);
			girl->pchate(1);
			girl->pclove(-4);
			girl->pcfear(4);
			girl->tiredness(-10);
			girl->happiness(-5);
			girl->health(1);
			girl->mana(5);
			girl->bdsm(1);
		}
	}
	else	// feeding off
	{
		if (girl->is_slave())	// Slave being starved
		{
			girl->confidence(-2);
			girl->obedience(2);
			girl->spirit(-2);
			girl->pchate(1);
			girl->pclove(-2);
			girl->pcfear(4);
			girl->tiredness(1);
			girl->happiness(-3);
			girl->health(-5);
			girl->mana(1);
			girl->bdsm(2);
		}
		else	// Free girl being starved
		{
			girl->confidence(-2);
			girl->obedience(2);
			girl->spirit(-2);
			girl->pchate(4);
			girl->pclove(-5);
			girl->pcfear(6);
			girl->tiredness(2);
			girl->happiness(-5);
			girl->health(-5);
			girl->mana(1);
			girl->bdsm(2);
		}
	}

#ifdef WDTEST // debuging
	sum << "\n \nFinal\n"
		<< "   h=" << girl->happiness()
		<< "   o=" << girl->obedience()
		<< "   l=" << girl->pclove()
		<< "   f=" << girl->pcfear()
		<< "   h=" << girl->pchate()
		<< "  HP=" << girl->health()
		<< "  TD=" << girl->tiredness();
	girl->m_Events.AddMessage(sum.str(), IMGTYPE_PROFILE, EVENT_DEBUG);
#undef WDTEST
#endif
}
