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

#include "cArena.h"
#include "cGangs.h"
#include "DirPath.h"
#include "cMessageBox.h"
#include "strnatcmp.h"
#include "XmlMisc.h"
#include "libintl.h"

extern cMessageQue      g_MessageQue;
extern cGirls           g_Girls;
extern cBrothelManager  g_Brothels;
extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;
extern cRng             g_Dice;
extern cGold            g_Gold;
extern char             buffer[1000];

// // ----- Strut sArena Create / destroy
sArena::sArena() :	m_Finance(0)	// constructor
{
	m_var	= 0;
	m_Name = "arena";
	m_Filthiness			= 0;
	m_Next					= 0;
	m_Girls					= 0;
	m_LastGirl				= 0;
	// end mod
	m_NumGirls				= 0;
	m_SecurityLevel			= 0;
	for(u_int i=0; i<NUMJOBTYPES; i++)
		m_BuildingQuality[i] = 0;
}

sArena::~sArena()			// destructor
{
	m_var	= 0;
	if(m_Next)
		delete m_Next;
	m_Next					= 0;
	if(m_Girls)
		delete m_Girls;
	m_LastGirl				= 0;
	m_Girls					= 0;
}

void cArenaManager::AddGirl(int brothelID, sGirl* girl)
{
	girl->m_InArena = true;
	cBrothelManager::AddGirl(brothelID, girl);
}

void cArenaManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InArena = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}


// ----- Class cArenaManager Create / destroy
cArenaManager::cArenaManager()			// constructor
{
	m_JobManager.Setup();
}

cArenaManager::~cArenaManager()			// destructor
{
	cArenaManager::Free();
}

void cArenaManager::Free()
{
	if(m_Parent)
		delete m_Parent;
	m_Parent			= 0;
	m_Last				= 0;
}

// ----- Update & end of turn
void cArenaManager::UpdateArena()
{
	sBrothel* current = (sBrothel*) m_Parent;
	current->m_Finance.zero();
	// Clear the girls' events from the last turn
	sGirl* cgirl = current->m_Girls;
	while(cgirl)
	{
		cgirl->m_Events.Clear();
		cgirl->m_Pay = 0;
		cgirl = cgirl->m_Next;
	}

	if(current->m_Filthiness < 0)
			current->m_Filthiness = 0;

	// Generate customers for the brothel for the day shift and update girls
	UpdateGirls(current, 0);

	// update the girls and satisfy the customers for this brothel during the night
	UpdateGirls(current, 1);
	g_Gold.brothel_accounts(current->m_Finance, current->m_id);
}

// End of turn stuff is here
// Same method than Brothel but different job
void cArenaManager::UpdateGirls(sBrothel* brothel, int DayNight)
{
	sGirl* current = brothel->m_Girls;
	string summary, msg, girlName;
	int totalGold;
	bool refused;
	sGirl* DeadGirl = 0;

	// WD:	Set processing flag to shift type
	m_Processing_Shift = DayNight;							

	while(current)
	{
		//srand(g_Graphics.GetTicks()+(g_Dice%5432));
		totalGold = 0;
		//summary = "";
		refused = false;
		current->m_Pay = 0;
		girlName = current->m_Realname;

/*
 *		ONCE DAILY processing
 *		at start of Day Shift
 */
		if(DayNight == SHIFT_DAY)					
		{
			// Remove any dead bodies from last week
			if(current->health() <= 0)
			{
				DeadGirl = current;

				if (current->m_Next) // If there are more girls to process
				  current = current->m_Next;
				else
				  current = 0;

				UpdateAllGirlsStat(brothel, STAT_PCFEAR, 2);	// increase all the girls fear of the player for letting her die (weather his fault or not)
				UpdateAllGirlsStat(brothel, STAT_PCHATE, 1);	// increase all the girls hate of the player for letting her die (weather his fault or not)

				// Two messages go into the girl queue...
				msg += girlName + gettext(" has died from her injuries, the other girls all fear and hate you a little more.");
				DeadGirl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
				summary += girlName + gettext(" has died from her injuries.  Her body will be removed by the end of the week.");
				DeadGirl->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);

				// There is also one global message
				g_MessageQue.AddToQue(msg, 1);

				// I forgot to set msg and summary to empty. D'OH!
				msg = "";
				summary = "";

				RemoveGirl(0, DeadGirl);
				DeadGirl = 0;

				if (current)  // If there are more girls to process
					continue;
				else
					break;
			}

			// Brothel only update for girls accomadation level
			do_food_and_digs(brothel, current);

			// update the fetish traits
			g_Girls.CalculateGirlType(current);

			// update birthday counter and age the girl
			g_Girls.updateGirlAge(current, true);

			// update temp stats
			g_Girls.updateTempStats(current);

			// update temp skills
			g_Girls.updateTempSkills(current);

			// update temp traits
			g_Girls.updateTempTraits(current);

			// handle pregnancy and children growing up
			g_Girls.HandleChildren(current, summary);
			
			// health loss to STD's		NOTE: Girl can die
			g_Girls.updateSTD(current);

			// Update happiness due to Traits		NOTE: Girl can die
			g_Girls.updateHappyTraits(current);

			//	Update daily stats	Now only runs once per day
			updateGirlTurnBrothelStats(current);

			//	Stat Code common to Dugeon and Brothel
			g_Girls.updateGirlTurnStats(current);
		}


/*
 *		EVERY SHIFT processing
 */

		// Sanity check! Don't process dead girls
		// Sanity check part 2: Check that m_Next points to something
		if(current->health() <= 0)
		{
			if (current->m_Next) // If there are more girls to process
			{
			    current = current->m_Next;
				continue;
			}
			else
			{
				current = 0;
				break;
			}
		}

		// Girl uses items she has
		g_Girls.UseItems(current);

		// Calculate the girls asking price
		g_Girls.CalculateAskPrice(current, true);

/*
 *		JOB PROCESSING
 */
		u_int sw = 0;						//	Job type
		if(current->m_JustGaveBirth)		// if she gave birth, let her rest this week
			sw = JOB_ARENAREST;
		else
			sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;

		/*if(sw == JOB_DOCTORE)
		{
			if (current->m_Next)
			{
			    current = current->m_Next;
				continue;
			}
			else
			{
				current = 0;
				break;
			}
		}*/
		if (sw == JOB_ARENAREST)
			refused = m_JobManager.JobFunctions[JOB_RESTING](current,brothel,SHIFT_NIGHT,summary);

		// do their job
		//else if(sw != JOB_ADVERTISING)			// advertising is handled earlier, before customer generation
		//{
		//	refused = m_JobManager.JobFunctions[sw](current,brothel,DayNight,summary);

		//	if(refused)						// if she refused she still gets tired
		//		g_Girls.AddTiredness(current);
		//}
		else
			refused = m_JobManager.JobFunctions[sw](current,brothel,DayNight,summary);

		if(refused)						// if she refused she still gets tired
			g_Girls.AddTiredness(current);

		totalGold += current->m_Pay;

		// work out the pay between the house and the girl 
		// may be change this for clinic
		g_Brothels.CalculatePay(brothel, current, sw);

		brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);

		// Check for dead girls
		// Avoids all the Warning Messages you get for a dead girl
		// MYR: This has already been done. Commenting it out.
		// TODO death girl in summary
		/*if (current->health() <= 0)
		{
			UpdateAllGirlsStat(brothel, STAT_PCFEAR, 2);	// increase all the girls fear of the player for letting her die (weather his fault or not)
			UpdateAllGirlsStat(brothel, STAT_PCHATE, 1);	// increase all the girls hate of the player for letting her die (weather his fault or not)
			msg = girlName + " has died from her injuries, the other girls all fear and hate you a little more.";
			current->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			summary += girlName + " has died from her injuries.  Her body will be removed by the end of the week.";
			current->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);
			current = current->m_Next;
			continue;
		}*/

		// update girl triggers ??
		//current->m_Triggers.ProcessTriggers();


			/*
 *		doctore CODE START
 */

		// Lets try to compact multiple messages into one.
		string DoctoreMsg = "";
		string DoctoreWarningMsg = "";

		bool doctore = false;
		if(GetNumGirlsOnJob(brothel->m_id, JOB_DOCTORE, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_DOCTORE, false) >= 1)
			doctore = true;

		if(g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (doctore)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_ARENAREST;
					DoctoreWarningMsg += gettext("The Doctore takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
				}
				else
				{
					if((g_Dice%100)+1 < 70)
					{
						DoctoreMsg += gettext("The Doctore helps ") + girlName + gettext(" to relax.\n");
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				DoctoreWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
		}

		if(g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && doctore && (g_Dice%100) +1 < 70)
		{
			DoctoreMsg = gettext("The Doctore helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if(g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if(doctore)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_ARENAREST;
					DoctoreWarningMsg += girlName + gettext(" is taken off duty by the Doctore to rest due to her low health.\n");
				}
				else
				{
					DoctoreMsg = gettext("The Doctore helps heal ") + girlName + gettext(".\n");
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				DoctoreWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
			}
		}

		// Back to work
		if((current->m_NightJob == JOB_ARENAREST) && (g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20))
		{
			if(
				(doctore || current->m_PrevDayJob == JOB_DOCTORE)  // do we have a director, or was she the director and made herself rest?
				&& current->m_PrevDayJob != 255  // 255 = nothing, in other words no previous job stored
				&& current->m_PrevNightJob != 255
				)
			{
				g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, true);
				if(current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
					current->m_NightJob = current->m_PrevNightJob;
				else
					current->m_DayJob = JOB_ARENAREST;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				DoctoreMsg += gettext("The Doctore puts ") + girlName + gettext(" back to work.\n");
			}
			else
			{
				current->m_DayJob = JOB_ARENAREST;
				DoctoreWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
			}
		}

		// Now print out the consolodated message
		if (strcmp(DoctoreMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(DoctoreMsg, IMGTYPE_PROFILE, SHIFT_NIGHT);
			DoctoreMsg = "";
		}

        if (strcmp(DoctoreWarningMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(DoctoreWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
			DoctoreWarningMsg = "";
		}
/*
 *		Doctore CODE END
 */
/*
 *		Summary Messages
 */
		bool sum = true;

		if(refused)											
			summary += girlName + gettext(" refused to work so made no money.");

		// WD:	Only do summary messages if there is income to report
		else if(totalGold > 0)										
		{
			summary += girlName + gettext(" earned a total of ");
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += gettext(" gold");
//			if(sw == JOB_MATRON)

			// WD: Job Paid by player
			if(m_JobManager.is_job_Paid_Player(sw))					
				summary += gettext(" directly from you. She gets to keep it all.");
			else if(current->house() <= 0)
				summary += gettext(" and she gets to keep it all.");
			else
			{
				summary += gettext(", you keep ");
				_itoa((int)current->m_Stats[STAT_HOUSE], buffer, 10);
				summary += buffer;
				summary += gettext("%. ");
			}
		}

		// WD:	No Income today
		else if(totalGold == 0)										
			summary += girlName + gettext(" made no money.");

#if 1																// WD: Income Loss Sanity Checking
		else if(totalGold < 0)										
		{
			summary += "ERROR: She has a loss of ";
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += " gold";
			summary += "\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.co.cc";
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_DEBUG);
			sum = false;
		}
#endif

		if(sum)
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_SUMMARY);

		summary = "";

		// Level the girl up if nessessary
		if(g_Girls.GetStat(current, STAT_EXP) == 255)
			g_Girls.LevelUp(current);

		// Myr: Natural healing. This amounts to 2% health/tiredness per shift and is not designed in any
		//      way to replace any healing item or mechanism in the game.
		//      I'm specifically not using g_Girls.UpdateStat as I don't want the modifiers for toughness
		//      and fragile to change the base 2% (Fragile would change 2% health gain to 1% health damage,
		//      which makes no sense and tough would change 2% health gain to 4% (8% per turn) which is
		//      IMO too high.)
		current->m_Stats[STAT_HEALTH] += 2;
		if (current->m_Stats[STAT_HEALTH] > 100)
			current->m_Stats[STAT_HEALTH] = 100;
		// Wow, this tiredness code causes the game to go nuts! Commented out for now
		//current->m_Stats[STAT_TIREDNESS] = current->m_Stats[STAT_TIREDNESS] - 2;
		//if (current->m_Stats[STAT_TIREDNESS] < 0)
		//	current->m_Stats[STAT_TIREDNESS] = 0;

		// Process next girl
		current = current->m_Next;
	}

	// WD: Finished Processing Shift set flag
	m_Processing_Shift= -1;				
}

TiXmlElement* cArenaManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("Arena_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	string message;

	// save arena
	TiXmlElement* pBrothels = new TiXmlElement("Arenas");
	pBrothelManager->LinkEndChild(pBrothels);
	sArena* current = (sArena*) m_Parent;
	//         ...................................................
	message = "***************** Saving arenas *****************";
	g_LogFile.write(message);
	while(current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveArenaXML(pBrothels);
		current = (sArena*) current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sArena::SaveArenaXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("Arena");
	pRoot->LinkEndChild(pBrothel);
	pBrothel->SetAttribute("Name", m_Name);

	pBrothel->SetAttribute("id", m_id);
	pBrothel->SetAttribute("NumRooms", m_NumRooms);
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);
	// save variables for sex restrictions
	pBrothel->SetAttribute("RestrictAnal", m_RestrictAnal);
	pBrothel->SetAttribute("RestrictBDSM", m_RestrictBDSM);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);
	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	// Save Girls
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pBrothel->LinkEndChild(pGirls);
	sGirl* girl = m_Girls;
	while(girl)
	{
		girl->SaveGirlXML(pGirls);
		girl = girl->m_Next;
	}
	return pBrothel;
}

bool cArenaManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too

	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0)
	{
		return false;
	}

	string message = "";
	//         ...................................................
	message = "***************** Loading arena ****************";
	g_LogFile.write(message);
	m_NumArenas = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Arenas");
	if (pBrothels)
	{
		for(TiXmlElement* pBrothel = pBrothels->FirstChildElement("Arena");
			pBrothel != 0;
			pBrothel = pBrothel->NextSiblingElement("Arena"))
		{
			sArena* current = new sArena();
			bool success = current->LoadArenaXML(TiXmlHandle(pBrothel));
			if (success == true)
			{
				AddBrothel(current);
			}
			else
			{
				delete current;
				continue;
			}

		} // load a arena
	}
	return true;
}

bool sArena::LoadArenaXML(TiXmlHandle hBrothel)
{
	//no need to init this, we just created it
	TiXmlElement* pBrothel = hBrothel.ToElement();
	if (pBrothel == 0)
	{
		return false;
	}

	if (pBrothel->Attribute("Name"))
	{
		m_Name = pBrothel->Attribute("Name");
	}

	int tempInt = 0;

	std::string message = "Loading arena: ";
	message += m_Name;
	g_LogFile.write(message);

	pBrothel->QueryIntAttribute("id", &m_id);
	pBrothel->QueryIntAttribute("NumRooms", &tempInt); m_NumRooms = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);
	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);

	// Load girls
	m_NumGirls = 0;
	TiXmlElement* pGirls = pBrothel->FirstChildElement("Girls");
	if (pGirls)
	{
		for(TiXmlElement* pGirl = pGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				AddGirl(girl);
			}
			else
			{
				delete girl;
				continue;
			}
		}
	}

	//commented out before the conversion to XML
	//building.load(ifs);
	return true;
}