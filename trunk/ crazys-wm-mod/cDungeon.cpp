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
extern char				buffer[1000];

// strut sDungeonCust
sDungeonCust::sDungeonCust()		// constructor 
{ 
	m_Prev=m_Next = 0; 
	m_Weeks = 0;
	m_Tort = false;
	m_Feeding = true;
	m_Health = 100;
}

sDungeonCust::~sDungeonCust()		// destructor
{ 
	if (m_Next) 
		delete m_Next; 
	m_Next = 0;
}

// strut sDungeonGirl
sDungeonGirl::sDungeonGirl()		// constructor
{ 
	m_Girl = 0;
	m_Prev=m_Next = 0;
	m_Weeks = 0;
	m_Feeding = true;
}

sDungeonGirl::~sDungeonGirl() 		// destructor
{
    if (m_Girl)
        delete m_Girl;
    m_Girl = 0;
    if (m_Next)
        delete m_Next;
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
    if (m_Girls)
        delete m_Girls;
    m_LastDGirl = m_Girls = 0;
    if (m_Custs)
        delete m_Custs;
    m_LastDCusts = m_Custs = 0;
    m_NumberDied=0; 
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
	string message = "";
	while(girl)
	{
		message = "Saving Dungeon Girl: ";
		message += girl->m_Girl->m_Realname;
		g_LogFile.write(message);
			
		TiXmlElement* pGirl = girl->m_Girl->SaveGirlXML(pDungeonGirls);

		TiXmlElement* pDungeonData = new TiXmlElement("Dungeon_Data");
		pGirl->LinkEndChild(pDungeonData);
		pDungeonData->SetAttribute("Feeding", girl->m_Feeding);
		pDungeonData->SetAttribute("Reason", girl->m_Reason);
		pDungeonData->SetAttribute("Weeks", girl->m_Weeks);

		girl = girl->m_Next;
	}

	message = "Saving Customers";
	g_LogFile.write(message);
	// save customers
	TiXmlElement* pDungeonCustomers = new TiXmlElement("Dungeon_Customers");
	pDungeon->LinkEndChild(pDungeonCustomers);
	sDungeonCust* cust = m_Custs;
	while(cust)
	{
		TiXmlElement* pCustomer = new TiXmlElement("Customer");
		pDungeonCustomers->LinkEndChild(pCustomer);
		pCustomer->SetAttribute("Feeding", cust->m_Feeding);
		pCustomer->SetAttribute("Tort", cust->m_Tort);
		pCustomer->SetAttribute("HasWife", cust->m_HasWife);
		pCustomer->SetAttribute("NumDaughters", cust->m_NumDaughters);
		pCustomer->SetAttribute("Reason", cust->m_Reason);
		pCustomer->SetAttribute("Weeks", cust->m_Weeks);
		pCustomer->SetAttribute(gettext("Health"), cust->m_Health);

		cust = cust->m_Next;
	}
	return pDungeon;
}

void cDungeon::LoadDungeonDataLegacy(ifstream& ifs)	// loads all the people (they are stored with the dungeon)
{
	Free();
	int temp;
	// load number died
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_NumberDied>>m_NumGirls>>m_NumCusts;

	// load girls
	string message = "";
	for(int i=0; i<m_NumGirls; i++)
	{
		sDungeonGirl* girl = new sDungeonGirl();

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if (temp == 1)
			girl->m_Feeding = true;
		else
			girl->m_Feeding = false;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>girl->m_Reason>>girl->m_Weeks;

		girl->m_Girl = new sGirl();
		g_Girls.LoadGirlLegacy(girl->m_Girl, ifs);

		message = "Loading Dungeon Girl: ";
		message += girl->m_Girl->m_Realname;
		g_LogFile.write(message);

		if (m_Girls)
		{
			sDungeonGirl* current = m_Girls;
			while(current->m_Next)
				current = current->m_Next;

			current->m_Next = girl;
			girl->m_Prev = current;
		}
		else
			m_Girls = girl;
		m_LastDGirl = girl;
	}

	// load customers
	message = "Loading customers";
	g_LogFile.write(message);
	for(int i=0; i<m_NumCusts; i++)
	{
		sDungeonCust* cust = new sDungeonCust();

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if (temp == 1)
			cust->m_Feeding = true;
		else
			cust->m_Feeding = false;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if (temp == 1)
			cust->m_Tort = true;
		else
			cust->m_Tort = false;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if (temp == 1)
			cust->m_HasWife = true;
		else
			cust->m_HasWife = false;

		ifs>>cust->m_NumDaughters>>cust->m_Reason>>cust->m_Weeks>>cust->m_Health;

		if (m_Custs)
		{
			sDungeonCust* current = m_Custs;
			while(current->m_Next)
				current = current->m_Next;

			current->m_Next = cust;
			cust->m_Prev = current;
		}
		else
			m_Custs = cust;
		m_LastDCusts = cust;
	}
}

bool cDungeon::LoadDungeonDataXML(TiXmlHandle hDungeon)	// loads all the people (they are stored with the dungeon)
{
	Free();//everything should be init even if we failed to load an XML element
	TiXmlElement* pDungeon = hDungeon.ToElement();
	if (pDungeon == 0)
	{
		return false;
	}

	// load number died
	pDungeon->QueryValueAttribute<unsigned long>("NumberDied", &m_NumberDied);

	// load girls
	m_NumGirls = 0;
	string message = "";
	TiXmlElement* pDungeonGirls = pDungeon->FirstChildElement("Dungeon_Girls");
	if (pDungeonGirls)
	{
		for(TiXmlElement* pGirl = pDungeonGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sDungeonGirl* girl = new sDungeonGirl();
			girl->m_Girl = new sGirl();
			bool success = girl->m_Girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				message = "Loading Dungeon Girl: ";
				message += girl->m_Girl->m_Realname;
				g_LogFile.write(message);

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
	message = "Loading customers";
	g_LogFile.write(message);
	TiXmlElement* pDungeonCustomers = pDungeon->FirstChildElement("Dungeon_Customers");
	if (pDungeonCustomers)
	{
		for(TiXmlElement* pCustomer = pDungeonCustomers->FirstChildElement("Customer");
			pCustomer != 0;
			pCustomer = pCustomer->NextSiblingElement("Customer"))
		{
			sDungeonCust* customer = new sDungeonCust();

			pCustomer->QueryValueAttribute<bool>("Feeding", &customer->m_Feeding);
			pCustomer->QueryValueAttribute<bool>("Tort", &customer->m_Tort);
			pCustomer->QueryValueAttribute<bool>("HasWife", &customer->m_HasWife);
			pCustomer->QueryIntAttribute("Reason", &customer->m_Reason);
			pCustomer->QueryIntAttribute("NumDaughters", &customer->m_NumDaughters);
			pCustomer->QueryIntAttribute("Weeks", &customer->m_Weeks);
			pCustomer->QueryIntAttribute(gettext("Health"), &customer->m_Health);

			PlaceDungeonCustomer(customer);
		}
	}
	return true;
}

void cDungeon::AddGirl(sGirl* girl, int reason)
{
	if (reason == DUNGEON_GIRLKIDNAPPED)
	{
		if (g_Brothels.GetObjective())
		{
			if (g_Brothels.GetObjective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
				g_Brothels.GetObjective()->m_SoFar++;
		}
	}

	// by this stage they should no longer be apart of any other lists of girls
	//	girl->m_Next = girl->m_Prev = 0;
	sDungeonGirl* newPerson = new sDungeonGirl();
	newPerson->m_Reason		= reason;
	newPerson->m_Girl		= girl;
	girl->m_DayJob = girl->m_NightJob = JOB_INDUNGEON;

	// remove from girl manager if she is there
	g_Girls.RemoveGirl(girl);

	// remove girl from brothels if she is there
	for(int i=0; i<g_Brothels.GetNumBrothels(); i++)
		g_Brothels.RemoveGirl(i, girl, false);

	PlaceDungeonGirl(newPerson);
}

void cDungeon::PlaceDungeonGirl(sDungeonGirl* newGirl)
{
	if (m_Girls)
	{
		m_LastDGirl->m_Next	= newGirl;
		newGirl->m_Prev	= m_LastDGirl;
		m_LastDGirl			= newGirl;
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
	newPerson->m_HasWife	= hasWife;
	newPerson->m_Reason		= reason;

	PlaceDungeonCustomer(newPerson);
}

void cDungeon::PlaceDungeonCustomer(sDungeonCust *newCust)
{
	if (m_Custs)
	{
		newCust->m_Prev	= m_LastDCusts;
		m_LastDCusts->m_Next = newCust;
		m_LastDCusts		= newCust;
	}
	else
	{
		m_LastDCusts = m_Custs = newCust;
	}

	m_NumCusts++;
}

int cDungeon::GetGirlPos(sGirl* girl)
{
	if (girl == 0 || m_Girls == 0)
		return -1;

	sDungeonGirl* current = m_Girls;
	int count = 0;
	while(current)
	{
		if (current->m_Girl == girl)
			break;
		count++;
		current = current->m_Next;
	}

	if (current == 0)
		return -1;

	return count;
}

sGirl* cDungeon::RemoveGirl(sGirl* girl)	// this returns the girl, it must be placed somewhere or deleted
{
	sDungeonGirl* current = m_Girls;
	while(current)
	{
		if (current->m_Girl == girl)
			break;
		current = current->m_Next;
	}

	if (current)
		return RemoveGirl(current);

	return 0;
}

sGirl* cDungeon::RemoveGirl(sDungeonGirl* girl)	// this returns the girl, it must be placed somewhere or deleted
{
	girl->m_Girl->m_DayJob = girl->m_Girl->m_NightJob = JOB_RESTING;
	if (girl->m_Next)
		girl->m_Next->m_Prev = girl->m_Prev;
	if (girl->m_Prev)
		girl->m_Prev->m_Next = girl->m_Next;
	if (girl == m_LastDGirl)
		m_LastDGirl = girl->m_Prev;
	if (girl == m_Girls)
		m_Girls = girl->m_Next;

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
	if (cust == 0)
		return;

	if (cust->m_Prev)
		cust->m_Prev->m_Next = cust->m_Next;
	if (cust->m_Next)
		cust->m_Next->m_Prev = cust->m_Prev;
	if (cust == m_LastDCusts)
		m_LastDCusts = cust->m_Prev;
	if (cust == m_Custs)
		m_Custs = cust->m_Next;
	cust->m_Next = cust->m_Prev = 0;
	delete cust;
	cust = 0;

	m_NumCusts--;
}

void cDungeon::OutputGirlRow(int i, string* Data, const vector<string>& columnNames)
{
	sDungeonGirl* girl = m_Girls;
	int tmp = 0;
	while(girl)
	{
		if (tmp == i)
			break;
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
	if (detailName == "Rebelliousness")
	{
		ss << g_Girls.GetRebelValue(m_Girl, false);
	}
	else if (detailName == "Reason")
	{
		switch(m_Reason)
		{
		case DUNGEON_GIRLCAPTURED:
			ss << gettext("Newly Captured.");
			break;
		case DUNGEON_GIRLKIDNAPPED:
			ss << gettext("Taken from her family.");
			break;
		case DUNGEON_GIRLWHIM:
			ss << gettext("Your whim.");
			break;
		case DUNGEON_GIRLSTEAL:
			ss << gettext("Not reporting true earnings.");
			break;
		case DUNGEON_GIRLRUNAWAY:
			ss << gettext("Ran away and re-captured.");
			break;
		case DUNGEON_NEWSLAVE:
			ss << gettext("This is a new slave.");
			break;
		case DUNGEON_NEWGIRL:
			ss << gettext("This is a new girl.");
			break;
		case DUNGEON_KID:
			ss << gettext("Child of one of your girls.");
			break;
		}
	}
	else if (detailName == "Duration")
	{
		ss << (int)m_Weeks;
	}
	else if (detailName == "Feeding")
	{
		ss << ((m_Feeding) ? gettext("Yes") : gettext("No"));
	}
	else if (detailName == "Tortured")
	{
		ss << ((m_Girl->m_Tort) ? gettext("Yes") : gettext("No"));
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
	while(cust)
	{
		if (tmp == i)
			break;
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
	if (detailName == "Name")
	{
		ss << gettext("Customer");
	}
	else if (detailName == gettext("Health"))
	{
		if (m_Health == 0)
			ss << gettext("DEAD");
		else
			ss << m_Health << gettext("%");
	}
	else if (detailName == "Reason")
	{
		switch(m_Reason)
		{
		case DUNGEON_CUSTNOPAY:
			ss << gettext("Not paying.");
			break;
		case DUNGEON_CUSTBEATGIRL:
			ss << gettext("Beating your girls.");
			break;
		case DUNGEON_CUSTSPY:
			ss << gettext("Being a rival's spy.");
			break;
		case DUNGEON_RIVAL:
			ss << gettext("Is a rival.");
			break;
		}
	}
	else if (detailName == "Duration")
	{
		ss << (int)m_Weeks;
	}
	else if (detailName == "Feeding")
	{
		ss << ((m_Feeding) ? gettext("Yes") : gettext("No"));
	}
	else if (detailName == "Tortured")
	{
		ss << ((m_Tort) ? gettext("Yes") : gettext("No"));
	}
	else
	{
		ss << gettext("---");
	}
	Data = ss.str();
}

sDungeonGirl* cDungeon::GetGirl(int i)
{
	if (i < 0)
	{
		i = i + m_NumGirls;
	}

	if (i >= m_NumGirls)
	{
		i = i - m_NumGirls;
	}

	sDungeonGirl* girl = m_Girls;
	int tmp = 0;
	while(girl)
	{
		if (tmp == i)
			break;
		tmp++;
		girl = girl->m_Next;
	}

	return girl;
}

sDungeonGirl* cDungeon::GetGirlByName(string name)
{
	sDungeonGirl* currentGirl = m_Girls;
	while(currentGirl)
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
	while(tgirl)
	{
		if (tgirl->m_Girl == girl)
			break;
		tmp++;
		tgirl = tgirl->m_Next;
	}

	return tmp;
}

sDungeonCust* cDungeon::GetCust(int i)
{
	sDungeonCust* cust = m_Custs;
	int tmp = 0;
	while(cust)
	{
		if (tmp == i)
			break;
		tmp++;
		cust = cust->m_Next;
	}

	return cust;
}

void cDungeon::Update()
{
/*
 *	WD: GetNumGirlsOnJob() not testing if the girl worked 
 *
 */
	sGirl* TorturerGirlref = 0;
	string msg, summary, girlName;

	// Reser counters
	m_NumGirlsTort = m_NumCustsTort = 0;

	// WD:	Did we torture the girls
	bool tort = g_Brothels.TortureDone();							

	if (tort)
	{
		// WD:	Who is the Torturer
		TorturerGirlref = g_Brothels.WhoHasTorturerJob();				
	}

/*********************************
 *	DO ALL DUNGEON GIRLS
 *********************************/
	if (m_Girls)
	{

		sDungeonGirl* current = m_Girls;
		while(current)
		{
			sGirl* girl = current->m_Girl;
			// Clear the girls' events from the last turn
			girl->m_Events.Clear();

//			girl->m_Tort = false;// WD: Move till after Girls have been tortured so that we dont torture twice week
			girlName	= girl->m_Realname;
			summary		= "";
			msg			= "";

#if 0		// original dead girls Code
			if (g_Girls.GetStat(girl, STAT_HEALTH) <= 0)
			{
				m_NumberDied++;
				current->m_Reason = DUNGEON_DEAD;
				SetGameFlag(FLAG_DUNGEONGIRLDIE);
			}

			if (current->m_Reason == DUNGEON_DEAD)
			{
				sDungeonGirl* temp = current;
				current = current->m_Next;
				delete RemoveGirl(temp);
				continue;
			}
#else
			// Check for dead girls
			if (girl->health() <= 0)
			{	
				// remove dead bodies from last week
				// doc: changed this from a single to double "="
				// since that appears to be the intention
				if (current->m_Reason == DUNGEON_DEAD)
				{
					sDungeonGirl* temp = current;
					current = current->m_Next;

					msg =  girlName + "'s body has been removed from the dungeon since she was dead.";
					g_MessageQue.AddToQue(msg, 1);

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
#endif
/*
 *			DAILY Processing
 */

			// the number of weeks they have been in the dungeon
			current->m_Weeks++;

			// update the fetish traits
			g_Girls.CalculateGirlType(girl);

			// update birthday counter and age the girl
			g_Girls.updateGirlAge(girl, true);

			// add the girls acc level costs to the brothen upkeep and adjust happiness
			//do_food_and_digs(brothel, girl);		// BROTHEL ONLY

			// update temp stats
			g_Girls.updateTempStats(girl);

			// update temp skills
			g_Girls.updateTempSkills(girl);

			// update temp traits
			g_Girls.updateTempTraits(girl);

			// handle pregnancy and children growing up
			g_Girls.HandleChildren(girl, summary);
			
			// health loss to STD's							NOTE: Girl can die
			g_Girls.updateSTD(girl);

			// Update happiness due to Traits				NOTE: Girl can die
			g_Girls.updateHappyTraits(girl);

			//	Update stats
			updateGirlTurnDungeonStats(current);

			//	Stat Code common to Dugeon and Brothel
			g_Girls.updateGirlTurnStats(girl);

			// Check for dead girls
			if (g_Girls.GetStat(girl, STAT_HEALTH) <= 0)
			{

				m_NumberDied++;
				current->m_Reason = DUNGEON_DEAD;
				SetGameFlag(FLAG_DUNGEONGIRLDIE);

				msg = girlName + gettext(" has died in the dungeon.");
				girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
				summary += girlName + gettext(" has died.  Her body will be removed by the end of the week.\n");
				girl->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);
			
				// if there is a torturer send her a message
				if (tort)						
				{
					msg	= girlName + gettext(" has died in the dungeon under her care!");
					TorturerGirlref->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
				}

				current = current->m_Next;
				continue;
			}

			// Have dungeon girls tortured by the Torturer
			if (tort)
			{
				// Code moved to class cGirlTorture
				//doTorturer(current, TorturerGirlref, summary);
				cGirlTorture gt(current, TorturerGirlref);
			}
			girl->m_Tort = false;// WD: Move till after Girls have been tortured so that we dont torture twice week
			

/*
 *			WARNING MESSAGES
 *			Allow girl sorting in turn summary
 */
			// Health
			int		nHealth	= girl->health();
			_itoa(nHealth, buffer, 10);

			if (nHealth < 20)
			{
				msg	= gettext("DANGER: ") + girlName + gettext(" is severely injured.\n\nHer health is ") + buffer + gettext(".");
				girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			}
			else if (nHealth < 40)
			{
				msg	= girlName + gettext(" is injured.\n\nHer health is ") + buffer + gettext(".");
				girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_WARNING);
			}
			
			// Tiredness
			int		nTired	= girl->tiredness();
			_itoa(nTired, buffer, 10);

			if (nTired > 80)
			{
				msg	= girlName + gettext(" is exhausted  and it may effect her health.\n\nHer tiredness is ") + buffer + gettext(".");
				girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_WARNING);
			}
			else if (nTired > 60)
			{
				msg	= girlName + gettext(" is tired.\n\nHer tiredness is ") + buffer + gettext(".");
				girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
			}


/*
 *			SUMMARY MESSAGES
 */

			if(!summary.empty())
				girl->m_Events.AddMessage( summary, IMGTYPE_PROFILE, EVENT_SUMMARY);

			// loop next dungeon girl	
			current = current->m_Next;
		}

/*
 *			WD:  Torturer Girl summary
 *				Processed after all dGirls
 */
		if (tort)
		{
			msg	= TorturerGirlref->m_Realname + gettext(" has tortured ");
			_itoa(m_NumGirlsTort, buffer, 10);
			msg += buffer;
			msg += gettext(" girls in the Dungeon.");
			TorturerGirlref->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}


	}


/*********************************
 *	DO ALL CUSTOMERS
 *********************************/
	if (m_Custs)
	{
		sDungeonCust* current = m_Custs;
		while(current)
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
			if (!current->m_Feeding)
				current->m_Health -= 5;

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
	string girlName	= girl->m_Realname;

	// Sanity check. Abort on dead girl
	if (girl->health() <= 0)
	{
		return;
	}

#ifdef WDTEST // debuging

	string sum = "Start\n";
	_itoa(girl->happiness(), buffer, 10);
	sum	+= "   h=";
	sum	+= buffer;
	_itoa(girl->obedience(), buffer, 10);
	sum	+= "   o=";
	sum	+= buffer;
	_itoa(girl->pclove(), buffer, 10);
	sum	+= "   l=";
	sum	+= buffer;
	_itoa(girl->pcfear(), buffer, 10);
	sum	+= "   f=";
	sum	+= buffer;
	_itoa(girl->pchate(), buffer, 10);
	sum	+= "   h=";
	sum	+= buffer;
	_itoa(girl->health(), buffer, 10);
	sum	+= "   HP=";
	sum	+= buffer;
	_itoa(girl->tiredness(), buffer, 10);
	sum	+= "  TD=";
	sum	+= buffer;

#endif

	if (d_girl->m_Feeding)
	{
		if (girl->is_slave())
		{	// Slave being fed
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

		else
		{	// Free girl being fed
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

	// feeding off
	else
	{
		if (girl->is_slave())
		{	// Slave being starved
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

		// Free Girl
		else
		{	// Free girl being starved
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

	sum += "\n\nFinal\n";
	_itoa(girl->happiness(), buffer, 10);
	sum	+= "   h=";
	sum	+= buffer;
	_itoa(girl->obedience(), buffer, 10);
	sum	+= "   o=";
	sum	+= buffer;
	_itoa(girl->pclove(), buffer, 10);
	sum	+= "   l=";
	sum	+= buffer;
	_itoa(girl->pcfear(), buffer, 10);
	sum	+= "   f=";
	sum	+= buffer;
	_itoa(girl->pchate(), buffer, 10);
	sum	+= "   h=";
	sum	+= buffer;
	_itoa(girl->health(), buffer, 10);
	sum	+= "   HP=";
	sum	+= buffer;
	_itoa(girl->tiredness(), buffer, 10);
	sum	+= "  TD=";
	sum	+= buffer;

	girl->m_Events.AddMessage(sum, IMGTYPE_PROFILE, EVENT_DEBUG);

#undef WDTEST
#endif
}

#if 0		// WD	Moved to cGirlTorture class
void cDungeon::doTorturer(sDungeonGirl* d_girl, sGirl* t_girl, string& summary)
{
/*
 *	WD:	Torturer tortures dungeon girl
 *
 *	Will be moved to cTorture class when Doclox merges hie shanges to branch.
 *
 *	d_girl	:	Dungeon Girl to be tortured
 *	t_girl	:	Girl assigned to be the Torturer
 *	summary :	Summary messages are appended
 */

	sGirl *girl		= d_girl->m_Girl;
	string girlName = girl->m_Realname;
	string msg		= "";
	int chance		= 0;
	cConfig cfg;
	int chance_div	=  cfg.initial.torture_mod();

	// Sanity check. Abort on dead girl
	if (girl->health() <= 0)
	{
		return;
	}

	// Don't tourture new mums
	if (girl->m_JustGaveBirth)
	{
		msg = girlName + gettext(" gave birth so was not tortured this week.");
		summary += gettext("Since she gave birth she was not tortured this week.\n");
		girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DUNGEON);
		t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		return;

	}
	
	// Number of girls tortured today
	m_NumGirlsTort++;					

	// do heavy torture
	if (girl->health() > 10)		
	{
		girl->health(-5);
		girl->happiness(-5);
		girl->constitution(1);
		girl->confidence(-5);
		girl->obedience(10);
		girl->spirit(-5);
		girl->tiredness(-5);
		girl->pchate(3);
		girl->pclove(-5);
		girl->pcfear(7);
		girl->bdsm(1);
	}

	// do safer torture
	else	//	(girl->health() <= 10)						
	{
		girl->happiness(-2);
		girl->confidence(-2);
		girl->obedience(4);
		girl->spirit(-2);
		girl->tiredness(-2);
		girl->pchate(1);
		girl->pclove(-2);
		girl->pcfear(3);
/*
*		Danger Messages starving / Low health
*/
		// if she is on this low health the tortuer will start feeding again
		if (!d_girl->m_Feeding)
		{
			d_girl->m_Feeding = true;
			msg	= girlName + gettext("  health is low from ongoing torture and starvation.\nFeeding has been permitted.");
			summary += t_girl->m_Realname + gettext(" allows food as her health is low.\n");
			girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}				
		else
		{
			msg	= girlName + gettext("  health is low from ongoing torture.");
			girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
	}

#if 1
/*
*	Random Trait Gain
*/

	if (!girl->has_trait("Broken Will")) 
	{
		chance	= d_girl->m_Weeks * 10;							// Number of weeks in dungeon  
		chance 	+= 200 - girl->spirit() - girl->health();
		chance	/= chance_div;
		if (girl->has_trait("Iron Will")) 
		{
			chance /= 2;
		}

		if (g_Dice.percent(chance)) 
		{
			girl->add_trait("Broken Will", false);

			msg	= girlName + gettext(" has gained the trait \"Broken Will\".");
			summary += msg + gettext("\n");
			girl->m_Events.AddMessage( msg, IMGTYPE_BDSM, EVENT_WARNING);
			t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
	}


	if (!girl->has_trait("Masochist"))							// Nearly twice as likley as broken will as 4 stats instead of 2					
	{
		chance	= d_girl->m_Weeks * 10;							// Number of weeks in dungeon  
		chance 	+= 200 - girl->spirit() - girl->health();
		chance 	+= girl->bdsm() + girl->libido();
		chance	/= chance_div;


		if (g_Dice.percent(chance)) 
		{
			girl->add_trait("Masochist", false);

			msg	= girlName + gettext(" has gained the trait \"Masochist\".");
			summary += msg + gettext("\n");
			girl->m_Events.AddMessage( msg, IMGTYPE_BDSM, EVENT_WARNING);
			t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
	}

	if (!girl->has_trait("Mind Fucked")) 
	{
		chance	= d_girl->m_Weeks * 10;							// Number of weeks in dungeon  
		chance 	+= 200 - girl->health() - girl->happiness();
		chance	/= chance_div;

		if (g_Dice.percent(chance)) 
		{
			girl->add_trait("Mind Fucked", false);

			msg	= girlName + gettext(" has gained the trait \"Mind Fucked\".");
			summary += msg + gettext("\n");
			girl->m_Events.AddMessage( msg, IMGTYPE_BDSM, EVENT_WARNING);
			t_girl->m_Events.AddMessage( msg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
	}
#endif
}
#endif
