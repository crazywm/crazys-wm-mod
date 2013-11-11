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

#ifdef LINUX
#include "linux.h"
#endif

// #include <fstream>
#include <cmath>
#include "cPlayer.h"
#include "XmlMisc.h"


cPlayer::cPlayer()			// constructor
{
	SetToZero();
}

void cPlayer::SetToZero()
{
	m_CustomerFear = m_Disposition = m_Suspicion = 0;
	m_WinGame=false;
}


TiXmlElement* cPlayer::SavePlayerXML(TiXmlElement* pRoot)
{
	TiXmlElement* pPlayer = new TiXmlElement("Player");
	pRoot->LinkEndChild(pPlayer);
	// save the player
	pPlayer->SetAttribute("WinGame", m_WinGame);// have they won the game

	// Save their stats
	SaveStatsXML(pPlayer, m_Stats);

	// skills
	SaveSkillsXML(pPlayer, m_Skills);

	// save other player stuff
	pPlayer->SetAttribute("Suspicion", m_Suspicion);
	pPlayer->SetAttribute("Disposition", m_Disposition);
	pPlayer->SetAttribute("CustomerFear", m_CustomerFear);
	return pPlayer;
}

void cPlayer::LoadPlayerLegacy(ifstream& ifs)
{
	int nTemp;

	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>nTemp;
	if(nTemp == 1)	// have they won the game
		m_WinGame = true;
	else
		m_WinGame = false;

	// stats
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(int i=0; i<NUM_STATS; i++)
	{
		ifs>>nTemp;
		m_Stats[i] = nTemp;
	}

	// skills
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		ifs>>nTemp;
		m_Skills[i] = nTemp;
	}

	// load other player stuff
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_Suspicion>>m_Disposition>>m_CustomerFear;
}

bool cPlayer::LoadPlayerXML(TiXmlHandle hPlayer)
{
	SetToZero();//init to 0
	TiXmlElement* pPlayer = hPlayer.ToElement();
	if (pPlayer == 0)
	{
		return false;
	}
	pPlayer->QueryValueAttribute<bool>("WinGame", &m_WinGame);// have they won the game

	// stats
	LoadStatsXML(hPlayer.FirstChild("Stats"), m_Stats);

	// skills
	LoadSkillsXML(hPlayer.FirstChild("Skills"), m_Skills);

	// load other player stuff
	pPlayer->QueryIntAttribute("Suspicion", &m_Suspicion);
	pPlayer->QueryIntAttribute("Disposition", &m_Disposition);
	pPlayer->QueryIntAttribute("CustomerFear", &m_CustomerFear);
	return true;
}

inline int cPlayer::Limit100(int nStat)
{
	if(nStat > 100)
		nStat = 100;
	else if(nStat < -100)
		nStat = -100;
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
	if (nValue == 0)						// Sanity check
		return 0;

	bool bSign	= nValue >= 0;
	nStat		+= 100;						// set stat to value between 0 and 200

	if (bSign)								// Adjust for adding or subtraction
		nStat = 200 -  nStat;					

	double	fRatio		=	nStat / 200.0;
	int		nRetValue	=	static_cast <int> (nValue * fRatio);

	//printf("cPlayer::Scale200 nRetValue = %d, fRatio = %.2f.\n\n", nRetValue, fRatio);

	if (abs(nRetValue) > 1)
		return nRetValue;						// Value is larger than 1				
		
	if (bSign)	
		return 1;
	else
		return -1;

}

int cPlayer::disposition(int n)
{
	n = Scale200( n, m_Disposition);
	m_Disposition = Limit100(m_Disposition + n);
	return m_Disposition;
}

int cPlayer::suspicion(int n)
{
	n = Scale200( n, m_Suspicion);
	m_Suspicion = Limit100(m_Suspicion + n);
	return m_Suspicion;
}

int cPlayer::customerfear(int n)
{
	n = Scale200( n, m_CustomerFear);
	m_CustomerFear = Limit100(m_CustomerFear + n);
	return m_CustomerFear;
}
