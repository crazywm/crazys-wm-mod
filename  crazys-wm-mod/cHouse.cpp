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

#include "cHouse.h"
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

// // ----- Strut sHouse Create / destroy
sHouse::sHouse() :	m_Finance(0)	// constructor
{
	m_var			= 0;
	m_Name			= "House";
	m_Filthiness	= 0;
	m_Next			= 0;
	m_Girls			= 0;
	m_LastGirl		= 0;
	m_NumGirls		= 0;
	m_SecurityLevel	= 0;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;
}

sHouse::~sHouse()			// destructor
{
	m_var			= 0;
	if (m_Next)		delete m_Next;
	m_Next			= 0;
	if (m_Girls)	delete m_Girls;
	m_LastGirl		= 0;
	m_Girls			= 0;
}

void cHouseManager::AddGirl(int brothelID, sGirl* girl)
{
	girl->where_is_she		= 0;
	girl->m_InMovieStudio	= false;
	girl->m_InArena			= false;
	girl->m_InCentre		= false;
	girl->m_InClinic		= false;
	girl->m_InFarm			= false;
	girl->m_InHouse			= true;
	cBrothelManager::AddGirl(brothelID, girl);
}

void cHouseManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	girl->m_InHouse = false;
	cBrothelManager::RemoveGirl(brothelID, girl, deleteGirl);
}

// ----- Class cHouseManager Create / destroy
cHouseManager::cHouseManager()			// constructor
{
	m_JobManager.Setup();
}

cHouseManager::~cHouseManager()			// destructor
{
	cHouseManager::Free();
}

void cHouseManager::Free()
{
	if (m_Parent)	delete m_Parent;
	m_Parent		= 0;
	m_Last			= 0;
	m_NumBrothels	= 0;
}

// ----- Update & end of turn
void cHouseManager::UpdateHouse()
{
	sBrothel* current = (sBrothel*) m_Parent;
	u_int restjob = JOB_HOUSEREST;
	u_int matronjob = JOB_HEADGIRL;
	u_int firstjob = JOB_HOUSEREST;
	u_int lastjob = JOB_CLEANHOUSE;

	current->m_Finance.zero();
	current->m_AntiPregUsed = 0;

	// Clear the girls' events from the last turn and do start of day stuff
	sGirl* cgirl = current->m_Girls;
	while (cgirl)
	{
		// Remove any dead bodies from last week
		if (cgirl->health() <= 0)
		{
			current->m_Filthiness++; // `J` Death is messy
			sGirl* DeadGirl = 0;
			string girlName = cgirl->m_Realname;
			DeadGirl = cgirl;
			// If there are more girls to process
			cgirl = (cgirl->m_Next) ? cgirl->m_Next : 0;
			// increase all the girls fear and hate of the player for letting her die (weather his fault or not)
			UpdateAllGirlsStat(current, STAT_PCFEAR, 2);
			UpdateAllGirlsStat(current, STAT_PCHATE, 1);

			// Two messages go into the girl queue...
			string msg = girlName + " has died from her injuries, the other girls all fear and hate you a little more.";
			string summary = girlName + " has died from her injuries.  Her body will be removed by the end of the week.";
			DeadGirl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			DeadGirl->m_Events.AddMessage(summary, IMGTYPE_DEATH, EVENT_SUMMARY);
			// There is also one global message
			g_MessageQue.AddToQue(msg, COLOR_RED);

			RemoveGirl(0, DeadGirl);
			DeadGirl = 0; msg = ""; summary = "";	// cleanup
		}
		else
		{
			cgirl->where_is_she = 0;
			cgirl->m_InMovieStudio = false;
			cgirl->m_InArena = false;
			cgirl->m_InCentre = false;
			cgirl->m_InClinic = false;
			cgirl->m_InFarm = false;
			cgirl->m_InHouse = true;

			cgirl->m_Events.Clear();

			cgirl->m_Pay = 0;
			cgirl->m_Tips = 0;

			// `J` Check for out of building jobs and set yesterday jobs for everyone first
			if (cgirl->m_DayJob	  < firstjob && cgirl->m_DayJob   > lastjob)	cgirl->m_DayJob = restjob;
			if (cgirl->m_NightJob < firstjob && cgirl->m_NightJob > lastjob)	cgirl->m_NightJob = restjob;

			cgirl->m_YesterDayJob = cgirl->m_DayJob;		// `J` set what she did yesterday
			cgirl->m_YesterNightJob = cgirl->m_NightJob;	// `J` set what she did yesternight
			cgirl->m_Refused_To_Work = false;


			string summary = "";
			do_food_and_digs(current, cgirl);			// Brothel only update for girls accommodation level
			g_Girls.CalculateGirlType(cgirl);			// update the fetish traits
			g_Girls.updateGirlAge(cgirl, true);		// update birthday counter and age the girl
			g_Girls.updateTempStats(cgirl);			// update temp stats
			g_Girls.updateTempSkills(cgirl);			// update temp skills
			g_Girls.updateTempTraits(cgirl);			// update temp traits
			g_Girls.HandleChildren(cgirl, summary);	// handle pregnancy and children growing up
			g_Girls.updateSTD(cgirl);					// health loss to STD's				NOTE: Girl can die
			g_Girls.updateHappyTraits(cgirl);			// Update happiness due to Traits	NOTE: Girl can die
			updateGirlTurnBrothelStats(cgirl);		// Update daily stats				Now only runs once per day
			g_Girls.updateGirlTurnStats(cgirl);		// Stat Code common to Dugeon and Brothel

			if (cgirl->m_JustGaveBirth)		// if she gave birth, let her rest this week
			{
				if (cgirl->m_DayJob != restjob)		cgirl->m_PrevDayJob = cgirl->m_DayJob;
				if (cgirl->m_NightJob != restjob)	cgirl->m_PrevNightJob = cgirl->m_NightJob;
				cgirl->m_DayJob = cgirl->m_NightJob = restjob;
			}

			cgirl = cgirl->m_Next;
		}
	}

	if (current->m_Filthiness < 0) current->m_Filthiness = 0;

	// Generate customers for the brothel for the day shift and update girls
	UpdateGirls(current, 0);

	// update the girls and satisfy the customers for this brothel during the night
	UpdateGirls(current, 1);
	g_Gold.brothel_accounts(current->m_Finance, current->m_id);
}

// End of turn stuff is here
// Same method than Brothel but different job
void cHouseManager::UpdateGirls(sBrothel* brothel, int DayNight)
{
	// `J` added to allow for easier copy/paste to other buildings
	u_int restjob = JOB_HOUSEREST;
	u_int matronjob = JOB_HEADGIRL;
	u_int firstjob = JOB_HOUSEREST;
	u_int lastjob = JOB_CLEANHOUSE;
	bool matron = (GetNumGirlsOnJob(brothel->m_id, matronjob, false) >= 1) ? true : false;
	string MatronMsg = "", MatronWarningMsg = "";
	stringstream ss;

	cConfig cfg;
	sGirl* current = brothel->m_Girls;
	string summary, msg, girlName;
	int totalPay = 0, totalTips = 0, totalGold = 0;

	int sum = EVENT_SUMMARY;
	u_int sw = 0;						//	Job type
	bool refused = false;
	m_Processing_Shift = DayNight;		// WD:	Set processing flag to shift type


	while (current)
	{
		totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY;
		// ONCE DAILY processing at start of Day Shift
		if (DayNight == SHIFT_DAY)
		{
			// Back to work
			if ((current->m_NightJob == restjob && current->m_DayJob == restjob) && current->m_PregCooldown < cfg.pregnancy.cool_down() &&
				g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20)
			{
				if ((matron || current->m_PrevDayJob == matronjob)					// do we have a director, or was she the director and made herself rest?
					&& current->m_PrevDayJob != 255 && current->m_PrevNightJob != 255)	// 255 = nothing, in other words no previous job stored
				{
					g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, true);
					if (current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
						current->m_NightJob = current->m_PrevNightJob;
					else
						current->m_DayJob = restjob;
					current->m_PrevDayJob = current->m_PrevNightJob = 255;
					MatronMsg += gettext("The Head Girl puts ") + girlName + gettext(" back to work.\n");
					current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, EVENT_BACKTOWORK);
					MatronMsg = "";
				}
				else
				{
					current->m_DayJob = current->m_NightJob = restjob;
					MatronWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
					current->m_Events.AddMessage(MatronWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
					MatronWarningMsg = "";
				}
			}

		}


/*
 *		EVERY SHIFT processing
 */

		// Sanity check! Don't process dead girls
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
		unsigned int restjob = JOB_HOUSEREST;	// `J` added this to allow for easier copy/paste to other buildings
		u_int sw = 0;							//	Job type

		if (current->m_JustGaveBirth)		// if she gave birth, let her rest this week
		{
			if (current->m_DayJob != restjob)	current->m_PrevDayJob = current->m_DayJob;
			if (current->m_NightJob != restjob)	current->m_PrevNightJob = current->m_NightJob;
			current->m_DayJob = restjob;
			current->m_NightJob = restjob;
		}
		sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;

		// `J` added check to force jobs into the Centre correcting a bug
		if (sw >= JOB_HOUSEREST && sw <= JOB_CLEANHOUSE)
		{
			refused = m_JobManager.JobFunc[sw](current, brothel, DayNight, summary);
		}
		else // Any job not in the House will be replaced with JOB_HOUSEREST
		{
			if (DayNight == SHIFT_DAY) current->m_DayJob = restjob;
			else current->m_NightJob = restjob;
			sw = restjob;
			refused = m_JobManager.JobFunc[restjob](current, brothel, DayNight, summary);
		}
		// if she refused she still gets tired
		if (refused) g_Girls.AddTiredness(current);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;

		// work out the pay between the house and the girl 
		g_Brothels.CalculatePay(brothel, current, sw);

		brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);


/*
 *		Summary Messages
 */
		ss.str("");
		if (refused) summary += girlName + gettext(" refused to work so made no money.");
		// `J` if a slave does a job that is normally paid by you but you don't pay your slaves...
		else if (current->is_slave() && !cfg.initial.slave_pay_outofpocket() &&
#if 0	// `J` until all jobs have this part added to them, use the individual job list instead of this
			m_JobManager.is_job_Paid_Player(sw))
#else
			(
			sw == JOB_CLEANHOUSE
			))
#endif
		{
			summary += "\nYou own her and you don't pay your slaves.";
		}
		else if (totalGold > 0)
		{
			ss << girlName << " earned a total of " << totalGold << " gold";
			u_int job = (DayNight) ? current->m_NightJob : current->m_DayJob;
			// if it is a player paid job and she is not a slave
			if ((m_JobManager.is_job_Paid_Player(job) && !current->is_slave()) ||
				// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
				(m_JobManager.is_job_Paid_Player(job) && current->is_slave() && cfg.initial.slave_pay_outofpocket()))
				ss << " directly from you. She gets to keep it all.";
			else if (current->house() <= 0)				ss << " and she gets to keep it all.";
			else if (totalTips>0 && ((cfg.initial.girls_keep_tips() && !current->is_slave()) || (cfg.initial.slave_keep_tips() && current->is_slave())))
			{
				int hpay = int(double(totalPay * double(current->m_Stats[STAT_HOUSE] * 0.01)));
				int gpay = totalPay - hpay;
				ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut (" << 100 - current->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay << " gold.\n\nYou got " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
			}
			else
			{
				int hpay = int(double(totalGold * double(current->m_Stats[STAT_HOUSE] * 0.01)));
				int gpay = totalGold - hpay;
				ss << ".\nShe keeps " << gpay << " gold. (" << 100 - current->m_Stats[STAT_HOUSE] << "%)\nYou keep " << hpay << " gold (" << current->m_Stats[STAT_HOUSE] << "%).";
			}
			summary += ss.str();
		}
		else if (totalGold == 0)		summary += girlName + gettext(" made no money.");
		else if (totalGold < 0)
		{
			ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
			summary += ss.str();
			sum = EVENT_DEBUG;
		}

		current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);


		summary = "";

		/*
		*		head girl CODE START
		*/

		// Lets try to compact multiple messages into one.
		string HeadMsg = "";
		string HeadWarningMsg = "";

		bool head = false;
		if (GetNumGirlsOnJob(brothel->m_id, JOB_HEADGIRL, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_HEADGIRL, false) >= 1)
			head = true;

		if (g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (head)
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_HOUSEREST;
					HeadWarningMsg += gettext("The head girl takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
				}
				else
				{
					if ((g_Dice % 100) + 1 < 70)
					{
						HeadMsg += gettext("The Head girl helps ") + girlName + gettext(" to relax.\n");
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				HeadWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
		}

		if (g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && head && (g_Dice % 100) + 1 < 70)
		{
			HeadMsg = gettext("The Head girl helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if (g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if (head)
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_HOUSEREST;
					HeadWarningMsg += girlName + gettext(" is taken off duty by the head girl to rest due to her low health.\n");
				}
				else
				{
					HeadMsg = gettext("The head girl helps heal ") + girlName + gettext(".\n");
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				HeadWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
			}
		}

		// Now print out the consolodated message
		if (strcmp(HeadMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(HeadMsg, IMGTYPE_PROFILE, SHIFT_NIGHT);
			HeadMsg = "";
		}

		if (strcmp(HeadWarningMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(HeadWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
			HeadWarningMsg = "";
		}
		/*
		*		Head girl CODE END
		*/
		// Do item check at the end of the day
		if (DayNight == SHIFT_NIGHT)
		{
			// update for girls items that are not used up
			do_daily_items(brothel, current);					// `J` added
		}


		// Level the girl up if nessessary
		g_Girls.LevelUp(current);

		// Natural healing, 2% health and 2% tiredness per day
		g_Girls.UpdateStat(current, STAT_HEALTH, 2, false);
		g_Girls.UpdateStat(current, STAT_TIREDNESS, -2, false);


		// Process next girl
		current = current->m_Next;
	}

	// WD: Finished Processing Shift set flag
	m_Processing_Shift= -1;				
}

TiXmlElement* cHouseManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("House_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	TiXmlElement* pBrothels = new TiXmlElement("Houses");
	pBrothelManager->LinkEndChild(pBrothels);
	sHouse* current = (sHouse*) m_Parent;
	g_LogFile.write("***************** Saving houses *****************");
	while(current)
	{
		g_LogFile.write("Saving brothel: " + current->m_Name);
		current->SaveHouseXML(pBrothels);
		current = (sHouse*) current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sHouse::SaveHouseXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("House");
	pRoot->LinkEndChild(pBrothel);
	pBrothel->SetAttribute("Name", m_Name);

	pBrothel->SetAttribute("id", m_id);
	pBrothel->SetAttribute("NumRooms", m_NumRooms);
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
	pBrothel->SetAttribute("MaxNumRooms", m_MaxNumRooms);
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);
	// save variables for sex restrictions
	pBrothel->SetAttribute("RestrictAnal", m_RestrictAnal);
	pBrothel->SetAttribute("RestrictBDSM", m_RestrictBDSM);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);
	pBrothel->SetAttribute("RestrictHand", m_RestrictHand);
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);

	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("AntiPregUsed", m_AntiPregUsed);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);

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

bool cHouseManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too
	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0) return false;
	g_LogFile.write("***************** Loading house ****************");
	m_NumHouses = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Houses");
	if (pBrothels)
	{
		for (TiXmlElement* pBrothel = pBrothels->FirstChildElement("House"); pBrothel != 0; pBrothel = pBrothel->NextSiblingElement("House"))
		{
			sHouse* current = new sHouse();
			bool success = current->LoadHouseXML(TiXmlHandle(pBrothel));
			if (success == true) { AddBrothel(current); } else { delete current; continue; }
		} // load a house
	}
	return true;
}

bool sHouse::LoadHouseXML(TiXmlHandle hBrothel)
{
	//no need to init this, we just created it
	TiXmlElement* pBrothel = hBrothel.ToElement();
	if (pBrothel == 0) return false;
	if (pBrothel->Attribute("Name")) m_Name = pBrothel->Attribute("Name");
	int tempInt = 0;

	std::string message = "Loading house: ";
	message += m_Name;
	g_LogFile.write(message);

	pBrothel->QueryIntAttribute("id", &m_id);
	pBrothel->QueryIntAttribute("NumRooms", &tempInt); m_NumRooms = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("MaxNumRooms", &tempInt); m_MaxNumRooms = tempInt; tempInt = 0;
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);
	pBrothel->QueryValueAttribute<bool>("RestrictHand", &m_RestrictHand);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);

	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	pBrothel->QueryIntAttribute("AntiPregUsed", &m_AntiPregUsed);
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);

	// Load girls
	m_NumGirls = 0;
	TiXmlElement* pGirls = pBrothel->FirstChildElement("Girls");
	if (pGirls)
	{
		for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
		{
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				girl->m_InHouse = true;
				AddGirl(girl);
			}
			else
			{
				delete girl;
				continue;
			}
		}
	}
	return true;
}