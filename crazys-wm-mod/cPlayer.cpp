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

#ifdef LINUX
#include "linux.h"
#endif

// #include <fstream>
#include <cmath>
#include "cPlayer.h"
#include "XmlMisc.h"

extern cConfig cfg;


cPlayer::cPlayer()			// constructor
{
	m_RealName = "";
	m_FirstName = "";
	m_Surname = "";
	m_BirthYear = 1190;
	m_BirthMonth = 0;
	m_BirthDay = 0;
	m_PlayerGender = GENDER_MALE;

	for (int i = 0; i < NUM_SKILLS; i++)	m_Skills[i] = 0;
	for (int i = 0; i < NUM_STATS; i++)		m_Stats[i] = 0;
	m_Stats[STAT_HEALTH] = 100;
	m_Stats[STAT_HAPPINESS] = 100;
	SetToZero();
}

void cPlayer::SetToZero()
{
	m_CustomerFear = m_Disposition = m_Suspicion = 0;
	m_WinGame = false;
}

TiXmlElement* cPlayer::SavePlayerXML(TiXmlElement* pRoot)
{
	TiXmlElement* pPlayer = new TiXmlElement("Player");
	pRoot->LinkEndChild(pPlayer);
	// save the player
	pPlayer->SetAttribute("RealName", m_RealName);
	pPlayer->SetAttribute("FirstName", m_FirstName);
	pPlayer->SetAttribute("Surname", m_Surname);
	pPlayer->SetAttribute("BirthMonth", m_BirthMonth);
	pPlayer->SetAttribute("BirthDay", m_BirthDay);

	// Save their stats
	SaveStatsXML(pPlayer, m_Stats);
	// skills
	SaveSkillsXML(pPlayer, m_Skills);
	// save other player stuff
	pPlayer->SetAttribute("Suspicion", m_Suspicion);
	pPlayer->SetAttribute("Disposition", m_Disposition);
	pPlayer->SetAttribute("CustomerFear", m_CustomerFear);
	pPlayer->SetAttribute("WinGame", m_WinGame);// have they won the game
	return pPlayer;
}


bool cPlayer::LoadPlayerXML(TiXmlHandle hPlayer)
{
	SetToZero();//init to 0
	TiXmlElement* pPlayer = hPlayer.ToElement();
	if (pPlayer == 0) return false;
	if (pPlayer->Attribute("RealName"))		m_RealName		= pPlayer->Attribute("RealName");
	if (pPlayer->Attribute("FirstName"))	m_FirstName		= pPlayer->Attribute("FirstName");
	if (pPlayer->Attribute("Surname"))		m_Surname		= pPlayer->Attribute("Surname");
	if (pPlayer->Attribute("BirthMonth"))	pPlayer->QueryIntAttribute("BirthMonth", &m_BirthMonth);
	if (pPlayer->Attribute("BirthDay"))		pPlayer->QueryIntAttribute("BirthDay", &m_BirthDay);

	// stats
	LoadStatsXML(hPlayer.FirstChild("Stats"), m_Stats);
	// skills
	LoadSkillsXML(hPlayer.FirstChild("Skills"), m_Skills);
	// load other player stuff
	pPlayer->QueryIntAttribute("Suspicion", &m_Suspicion);
	pPlayer->QueryIntAttribute("Disposition", &m_Disposition);
	pPlayer->QueryIntAttribute("CustomerFear", &m_CustomerFear);
	pPlayer->QueryValueAttribute<bool>("WinGame", &m_WinGame);// have they won the game
	return true;
}

inline int cPlayer::Limit100(int nStat)
{
	if (nStat > 100) nStat = 100;
	else if (nStat < -100) nStat = -100;
	return nStat;
}

int	cPlayer::Scale200(int nValue, int nStat)
{
	/*
	*	WD	Scale the value n so that if adjusting the value will have
	*		less effect as you approch the max or min values.
	*
	*	eg	if you are EVIL additional evil acts will only subtract 1 but
	*		if you are GOOD an evil act will subtract nVal from Disposition
	*
	*		This will slow down the changes in player stats as you near the
	*		end of the ranges.
	*/
	//printf("cPlayer::Scale200 nValue = %d, nStat = %d.\n", nValue, nStat);
	if (nValue == 0) return 0;					// Sanity check
	bool bSign = nValue >= 0;
	nStat += 100;						// set stat to value between 0 and 200
	if (bSign) nStat = 200 - nStat;							// Adjust for adding or subtraction
	double	fRatio = nStat / 200.0;
	int		nRetValue = static_cast <int> (nValue * fRatio);

	//printf("cPlayer::Scale200 nRetValue = %d, fRatio = %.2f.\n \n", nRetValue, fRatio);
	if (abs(nRetValue) > 1) return nRetValue;						// Value is larger than 1				
	return (bSign ? 1 : -1);
}

int cPlayer::disposition(int n)
{
	n = Scale200(n, m_Disposition);
	m_Disposition = Limit100(m_Disposition + n);
	return m_Disposition;
}
int cPlayer::evil(int n)
{
	// `J` add check for if harsher torture is set
	if (cfg.initial.torture_mod() < 0 && n > 0)
	{
		n += n;		// `J` double evil if increasing it BUT NOT IF LOWERING IT
	}	
	return disposition(-1 * n);
}

int cPlayer::suspicion(int n)
{
	n = Scale200(n, m_Suspicion);
	m_Suspicion = Limit100(m_Suspicion + n);
	return m_Suspicion;
}

int cPlayer::customerfear(int n)
{
	n = Scale200(n, m_CustomerFear);
	m_CustomerFear = Limit100(m_CustomerFear + n);
	return m_CustomerFear;
}


string cPlayer::SetTitle(string title)
{
	m_Title = title;
	return m_Title;
}
string cPlayer::SetFirstName(string firstname)
{
	m_FirstName = firstname;
	return m_FirstName;
}
string cPlayer::SetSurname(string surname)
{
	m_Surname = surname;
	return 	m_Surname;
}
string cPlayer::SetRealName(string realname)
{
	m_RealName = realname;
	return 	m_RealName;
}

int cPlayer::BirthYear(int n)
{
	m_BirthYear = m_BirthYear + n;
	return m_BirthYear;
}
int cPlayer::BirthMonth(int n)
{
	m_BirthMonth = m_BirthMonth + n;
	if (m_BirthMonth > 12)		m_BirthMonth = 12;
	if (m_BirthMonth < 1)		m_BirthMonth = 1;
	return m_BirthMonth;
}
int cPlayer::BirthDay(int n)
{
	m_BirthDay = m_BirthDay + n;
	if (m_BirthDay > 30)	m_BirthDay = 30;
	if (m_BirthDay < 1)		m_BirthDay = 1;
	return m_BirthDay;
}
int cPlayer::SetBirthYear(int n)
{
	m_BirthYear = m_BirthYear + n;
	return m_BirthYear;
}
int cPlayer::SetBirthMonth(int n)
{
	m_BirthMonth = n;
	if (m_BirthMonth > 12)		m_BirthMonth = 12;
	if (m_BirthMonth < 1)		m_BirthMonth = 1;
	return m_BirthMonth;
}
int cPlayer::SetBirthDay(int n)
{
	m_BirthDay = n;
	if (m_BirthDay > 30)	m_BirthDay = 30;
	if (m_BirthDay < 1)		m_BirthDay = 1;
	return m_BirthDay;
}

bool cPlayer::CanImpregnateFemale()
{
	if (m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
		m_PlayerGender == GENDER_HERM || m_PlayerGender == GENDER_MALE)
		return true;
	return false;
}
bool cPlayer::CanCarryOwnBaby()
{
	if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FUTA ||
		m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL)
		return true;
	return false;
}
bool cPlayer::CanCarryNormalBaby()
{
	if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FEMALENEUT ||
		m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
		m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
		m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM)
		return true;
	return false;
}
bool cPlayer::HasPenis()
{
	if (m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
		m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
		m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM ||
		m_PlayerGender == GENDER_MALENEUT || m_PlayerGender == GENDER_MALE)
		return true;
	return false;
}
bool cPlayer::HasVagina()
{
	if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FEMALENEUT ||
		m_PlayerGender == GENDER_FUTA || m_PlayerGender == GENDER_FUTANEUT ||
		m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
		m_PlayerGender == GENDER_HERMNEUT || m_PlayerGender == GENDER_HERM)
		return true;
	return false;
}
bool cPlayer::HasTestes()
{
	if (m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL ||
		m_PlayerGender == GENDER_HERM || m_PlayerGender == GENDER_MALE)
		return true;
	return false;
}
bool cPlayer::HasOvaries()
{
	if (m_PlayerGender == GENDER_FEMALE || m_PlayerGender == GENDER_FUTA ||
		m_PlayerGender == GENDER_FUTAFULL || m_PlayerGender == GENDER_HERMFULL)
		return true;
	return false;
}


void cPlayer::SetGender(int x)
{
	/* */if (x < GENDER_FEMALE)	m_PlayerGender = GENDER_FEMALE;
	else if (x > GENDER_MALE)	m_PlayerGender = GENDER_MALE;
	else /*                 */	m_PlayerGender = x;
}

void cPlayer::AdjustGender(int male, int female)
{
	if (male == 0 && female == 0)			return;								// no change

	// do the easy stuff first
	else if (male <= 0 && female >= 5)		{ m_PlayerGender = GENDER_FEMALE;		return; }	// force female
	else if (male >= 5 && female <= 0)		{ m_PlayerGender = GENDER_MALE;			return; }	// force male
	else if (male <= -3 && female <= -3)	{ m_PlayerGender = GENDER_NONE;			return; }	// force andro
	else if (male >= 2 && female >= 2)
	{
		if (m_PlayerGender < 6)/*       */	{ m_PlayerGender = GENDER_FUTAFULL;		return; }	// everything on base female
		else /*                         */	{ m_PlayerGender = GENDER_HERMFULL;		return; }	// everything on base male
	}
	else if (male <= -2 && female <= -2)
	{
		if (m_PlayerGender < 6)/*     */	{ m_PlayerGender = GENDER_NONEFEMALE;	return; }	// nothing on base female
		else /*                       */	{ m_PlayerGender = GENDER_NONEMALE;		return; }	// nothing on base male
	}

	/*
	GENDER_FEMALE      = 0;	 0 0 1 1
	GENDER_FEMALENEUT  = 1;	 0 0 1 0
	GENDER_FUTA        = 2;	 1 0 1 1
	GENDER_FUTANEUT    = 3;	 1 0 1 0
	GENDER_FUTAFULL    = 4;	 1 1 1 1
	GENDER_NONEFEMALE  = 5;	 0 0 0 0
	GENDER_NONE        = 6;	 0 0 0 0
	GENDER_NONEMALE    = 7;	 0 0 0 0
	GENDER_HERMFULL    = 8;	 1 1 1 1
	GENDER_HERMNEUT    = 9;	 1 0 1 0
	GENDER_HERM        = 10; 1 1 1 0
	GENDER_MALENEUT    = 11; 1 0 0 0
	GENDER_MALE        = 12; 1 1 0 0
	*/


#if 0 // `J` - this needs a ton more work before all outcomes will work so not using it for now
	switch (m_PlayerGender)
	{
	case GENDER_FEMALE:
	{
		/* */if (male <= 0 && female >= 0)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= 0 && female == -1)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male == 1	&& female >= 0)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male == 1	&& female == -1)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male == 2	&& female >= 0)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= 0	&& female == -2)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= -1	&& female <= -3)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male == 0	&& female <= -3)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male >= #	&& female == #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= #	&& female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= #	&& female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= #	&& female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= #	&& female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_FEMALENEUT:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_FUTA:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_FUTANEUT:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_FUTAFULL:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_NONEFEMALE:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_NONE:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_NONEMALE:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_HERMFULL:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_HERMNEUT:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_HERM:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_MALENEUT:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	case GENDER_MALE:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
	default:
	{
		/* */if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALE;			// 0 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FEMALENEUT;		// 0 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTA;			// 1 0 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTANEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_FUTAFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEFEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONE;			// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_NONEMALE;		// 0 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMFULL;		// 1 1 1 1
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERMNEUT;		// 1 0 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_HERM;			// 1 1 1 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALENEUT;		// 1 0 0 0
		else if (male <= # && female <= #)/*  */	m_PlayerGender = GENDER_MALE;			// 1 1 0 0
		return;
	}
#endif


	}

