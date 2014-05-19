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

#include "cClinic.h"
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

// // ----- Strut sClinic Create / destroy
sClinic::sClinic() :	m_Finance(0)	// constructor
{
	m_var	= 0;
	m_Name = "clinic";
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

sClinic::~sClinic()			// destructor
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

void cClinicManager::AddGirl(int brothelID, sGirl* girl)
{
	girl->m_InClinic = true;
	cBrothelManager::AddGirl(brothelID, girl);
}

void cClinicManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InClinic = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}


// ----- Class cClinicManager Create / destroy
cClinicManager::cClinicManager()			// constructor
{
	//Aika edit
	//sClinic* current = m_Parent;
	m_JobManager.Setup();
}

cClinicManager::~cClinicManager()			// destructor
{
	cClinicManager::Free();
}

void cClinicManager::Free()
{
	if(m_Parent)
		delete m_Parent;
	m_Parent			= 0;
	m_Last				= 0;
}

// ----- Update & end of turn
void cClinicManager::UpdateClinic()
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
void cClinicManager::UpdateGirls(sBrothel* brothel, int DayNight)
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
			sw = JOB_CLINICREST;
		else
			sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;

		/*if(sw == JOB_CHAIRMAN)
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
		if (sw == JOB_CLINICREST)
			refused = m_JobManager.JobFunctions[JOB_RESTING](current,brothel,DayNight,summary);

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
 *		chair man CODE START
 */

		// Lets try to compact multiple messages into one.
		string ChairMsg = "";
		string ChairWarningMsg = "";

		bool chair = false;
		if(GetNumGirlsOnJob(brothel->m_id, JOB_CHAIRMAN, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_CHAIRMAN, false) >= 1)
			chair = true;

		if(g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (chair)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_CLINICREST;
					ChairWarningMsg += gettext("The Chairman takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
				}
				else
				{
					if((g_Dice%100)+1 < 70)
					{
						ChairMsg += gettext("The Chairman helps ") + girlName + gettext(" to relax.\n");
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				ChairWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
		}

		if(g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && chair && (g_Dice%100) +1 < 70)
		{
			ChairMsg = gettext("The Chairman helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if(g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if(chair)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_CLINICREST;
					ChairWarningMsg += girlName + gettext(" is taken off duty by the Chairman to rest due to her low health.\n");
				}
				else
				{
					ChairMsg = gettext("The head girl helps heal ") + girlName + gettext(".\n");
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				ChairWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
			}
		}

		// Back to work
		if((current->m_NightJob == JOB_CLINICREST && current->m_DayJob == JOB_CLINICREST) && (g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20))
		{
			if(
				(chair || current->m_PrevDayJob == JOB_CHAIRMAN)  // do we have a director, or was she the director and made herself rest?
				&& current->m_PrevDayJob != 255  // 255 = nothing, in other words no previous job stored
				&& current->m_PrevNightJob != 255
				)
			{
				g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, true);
				if(current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
					current->m_NightJob = current->m_PrevNightJob;
				else
					current->m_DayJob = JOB_CLINICREST;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				ChairMsg += gettext("The head girl puts ") + girlName + gettext(" back to work.\n");
			}
			else
			{
				current->m_DayJob = JOB_CLINICREST;
				ChairWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
			}
		}

		// Now print out the consolodated message
		if (strcmp(ChairMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(ChairMsg, IMGTYPE_PROFILE, SHIFT_NIGHT);
			ChairMsg = "";
		}

        if (strcmp(ChairWarningMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(ChairWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
			ChairWarningMsg = "";
		}
/*
 *		chair man CODE END
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
			summary += "\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
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
//Aika Edit
//sGirl* cClinicManager::GetGirl(int brothelID, int num)
//{
//	if(num < 0)
//		return 0;
//	sClinic* current = m_Parent;
//	if(current == 0)
//		return 0;
//
//	if(num > current->m_NumGirls)
//		return 0;
//
//	if(current)
//	{
//		int count = 0;
//		sGirl* currentGirl = current->m_Girls;
//		while(currentGirl)
//		{
//			if(count == num)
//				break;
//			count++;
//			currentGirl = currentGirl->m_Next;
//		}
//
//		return currentGirl;
//	}
//
//	return 0;
//}
//End Aika Edit
TiXmlElement* cClinicManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("Clinic_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	string message;

	// save clinic
	TiXmlElement* pBrothels = new TiXmlElement("Clinics");
	pBrothelManager->LinkEndChild(pBrothels);
	sClinic* current = (sClinic*) m_Parent;
	//         ...................................................
	message = "***************** Saving clinics *****************";
	g_LogFile.write(message);
	while(current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveClinicXML(pBrothels);
		current = (sClinic*) current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sClinic::SaveClinicXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("Clinic");
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

bool cClinicManager::LoadDataXML(TiXmlHandle hBrothelManager)
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
	message = "***************** Loading clinic ****************";
	g_LogFile.write(message);
	m_NumClinics = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Clinics");
	if (pBrothels)
	{
		for(TiXmlElement* pBrothel = pBrothels->FirstChildElement("Clinic");
			pBrothel != 0;
			pBrothel = pBrothel->NextSiblingElement("Clinic"))
		{
			sClinic* current = new sClinic();
			bool success = current->LoadClinicXML(TiXmlHandle(pBrothel));
			if (success == true)
			{
				AddBrothel(current);
			}
			else
			{
				delete current;
				continue;
			}

		} // load a clinic
	}
	return true;
}

bool sClinic::LoadClinicXML(TiXmlHandle hBrothel)
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

	std::string message = "Loading clinic: ";
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
				girl->m_InClinic = true;
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