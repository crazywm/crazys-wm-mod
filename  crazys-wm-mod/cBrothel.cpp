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

#include "cBrothel.h"
#include "cGangs.h"
#include "DirPath.h"
#include "cMessageBox.h"
#include "strnatcmp.h"
#include "XmlMisc.h"
#include "libintl.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cHouse.h"

//#include "cDungeon.h"
//#include "cJobManager.h"
//#include "cCustomers.h"
//#include "cGirls.h"
//#include "GameFlags.h"
//#include "CGraphics.h"
//#include "sConfig.h"
//#include "cTariff.h"

extern cMessageQue      g_MessageQue;
extern cCustomers       g_Customers;
extern cGirls           g_Girls;
extern cInventory       g_InvManager;
extern cBrothelManager  g_Brothels;
extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;
extern cRng             g_Dice;
extern cGold            g_Gold;
extern cGangManager     g_Gangs;
extern char             buffer[1000];
extern cMovieStudioManager  g_Studios;
extern cArenaManager g_Arena;
extern cClinicManager g_Clinic;
extern cCentreManager g_Centre;
extern cHouseManager g_House;


//extern CGraphics		g_Graphics;

/*
 * mod - this is a bit big for an inline func
 * and we don't create so many of them that we
 * we need the speed - so taken out of the header
 */

// Prototypes
sGirl* girl_sort(sGirl* girl, sGirl** lastgirl);
int sGirlcmp(sGirl *a, sGirl *b);

// // ----- Strut sBrothel Create / destroy
sBrothel::sBrothel()	:	m_Finance(0)	// constructor
{
	m_MiscCustomers			= 0;
	m_TotalCustomers		= 0;
	m_Filthiness			= 0;
	m_HasGambStaff			= 0;
	m_HasBarStaff			= 0;
	m_AdvertisingBudget		= 0;
	m_Next					= 0;
	m_Girls					= 0;
	m_LastGirl				= 0;
	m_Fame					= 0;
	m_Happiness				= 0;
	// end mod
	m_NumGirls				= 0;
	m_ShowTime				= 0;
	m_ShowQuality			= 0;
	m_SecurityLevel			= 0;
	m_KeepPotionsStocked	= 0;
	m_AntiPregPotions		= 0;
	m_RestrictAnal			= false;
	m_RestrictBDSM			= false;
	m_RestrictOral			= false;
	m_RestrictTitty			= false;
	m_RestrictBeast			= false;
	m_RestrictGroup			= false;
	m_RestrictNormal		= false;
	m_RestrictLesbian		= false;
	for(u_int i=0; i<NUMJOBTYPES; i++)
		m_BuildingQuality[i] = 0;

	//movie
	
	m_CurrFilm				= 0;
	m_NumMovies				= 0;
	m_LastMovies			= 0;
	m_Movies				= 0;
	m_MovieRunTime			= 0;
}

sBrothel::~sBrothel()			// destructor
{
	m_ShowTime				= 0;
	m_ShowQuality			= 0;

	if(m_Next)
		delete m_Next;
	m_Next					= 0;
	if(m_Girls)
		delete m_Girls;
	m_LastGirl				= 0;
	m_Girls					= 0;
	//movie
	if(m_CurrFilm)
		delete m_CurrFilm;
	m_NumMovies				= 0;
	if(m_Movies)
		delete m_Movies;
	m_Movies				= 0;
	m_LastMovies			= 0;
}

// ----- Matron  // `J` added building checks
//bool sBrothel::matron_on_shift(int shift)
bool sBrothel::matron_on_shift(int shift, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, int BrothelID)
{
	if (isArena)
	{
		if (g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, shift == SHIFT_DAY) > 0)
		{
			return true;
		}
	}
	else if (isStudio)
	{
		if (g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, shift == SHIFT_DAY) > 0)
		{
			return true;
		}
	}
	else if (isClinic)
	{
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, shift == SHIFT_DAY) > 0)
		{
			return true;
		}
	}
	else if (isCentre)
	{
		if (g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, shift == SHIFT_DAY) > 0)
		{
			return true;
		}
	}
	else if (isHouse)
	{
		if (g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, shift == SHIFT_DAY) > 0)
		{
			return true;
		}
	}
	else if (g_Brothels.GetNumGirlsOnJob(BrothelID, JOB_MATRON, shift == SHIFT_DAY) > 0)
	{
		return true;
	}
	return false;
}
//int sBrothel::matron_count
int sBrothel::matron_count(bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, int BrothelID)
{
	int i, sum = 0;

	for (i = 0; i < 2; i++)
	{
		if (isArena)
		{
			if (g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, (i == 0)) > 0)
			{
				sum++;
			}
		}
		else if (isStudio)
		{
			if (g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, (i == 0)) > 0)
			{
				sum++;
			}
		}
		else if (isClinic)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, (i == 0)) > 0)
			{
				sum++;
			}
		}
		else if (isCentre)
		{
			if (g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, (i == 0)) > 0)
			{
				sum++;
			}
		}
		else if (isHouse)
		{
			if (g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, (i == 0)) > 0)
			{
				sum++;
			}
		}
		else if (g_Brothels.GetNumGirlsOnJob(BrothelID, JOB_MATRON, (i == 0)) > 0)
		{
			sum++;
		}
	}
	return sum;
}

bool sBrothel::has_matron()
{
	int i;

	for(i = 0; i < 2; i++) {
		if (g_Brothels.GetNumGirlsOnJob(m_id, JOB_MATRON, (i == 0)) > 0) {
			return true;
		}
	}
	return false;
}



// ----- Class cBrothelManager Create / destroy
cBrothelManager::cBrothelManager()			// constructor
{
	cConfig cfg;
	m_NumInventory		= 0;
	for(int i=0; i<MAXNUM_INVENTORY; i++)
	{
		m_Inventory[i]	= 0;
		m_EquipedItems[i] = 0;
		m_NumItem[i]	= 0;
	}
	m_Parent			= 0;
	m_Last				= 0;
	m_NumBrothels		= 0;
//	m_AntiPregPotions	= 0;
	m_SupplyShedLevel	= 1;
	m_BribeRate			= 0;
	m_Influence			= 0;
	m_Bank				= 0;

	m_Objective			= 0;

	m_Prison			= 0;
	m_NumPrison			= 0;
	m_LastPrison		= 0;

	m_NumRunaways		= 0;
	m_Runaways			= 0;
	m_LastRunaway		= 0;

	m_HandmadeGoods		= 0; 
	m_Beasts			= 0; 
	m_AlchemyIngredients = 0;
//	m_KeepPotionsStocked = false;

	m_TortureDoneFlag	= false;
	m_Processing_Shift	= -1;

	m_JobManager.Setup();
}

cBrothelManager::~cBrothelManager()			// destructor
{
	Free();
}

void cBrothelManager::Free()
{
	cConfig cfg;
	if(m_Prison)
		delete m_Prison;
	m_Prison			= 0;
	m_NumPrison			= 0;
	m_LastPrison		= 0;

	m_NumRunaways		= 0;
	if(m_Runaways)
		delete m_Runaways;
	m_Runaways			= 0;
	m_LastRunaway		= 0;

	m_NumInventory		= 0;
	for(int i=0; i<MAXNUM_INVENTORY; i++)
	{
		m_Inventory[i]	= 0;
		m_EquipedItems[i] = 0;
		m_NumItem[i]	= 0;
	}
	m_NumBrothels		= 0;
//	m_AntiPregPotions	= 0;
	m_SupplyShedLevel	= 1;
	m_BribeRate			= 0;
	m_Influence			= 0;
	m_Bank				= 0;

//	m_KeepPotionsStocked = false;
	m_HandmadeGoods		= 0;
	m_Beasts			= 0;
	m_AlchemyIngredients = 0;


	if(m_Objective)
		delete m_Objective;
	m_Objective			= 0;

	m_Dungeon.Free();

	m_Rivals.Free();
	if(m_Parent)
		delete m_Parent;
	m_Parent			= 0;
	m_Last				= 0;
}

// ----- Misc
void cBrothelManager::check_raid()
{
	cRival *rival = 0;
	cRivalManager *rival_mgr = GetRivalManager();
/*
 *	If the player's influence can shield him
 *	it only follows that the influence of his rivals
 *	can act to stitch him up
 *
 *	see if there exists a rival with infulence
 */
 	if(rival_mgr->player_safe() == false) {
		rival = rival_mgr->get_influential_rival();
	}
/*
 *	chance is based on how much suspicion is leveled at
 *	the player, less his influence at city hall.
 *
 *	And then modified back upwards by rival influence
 */
	int pc = m_Player.suspicion() - m_Influence;
	if(rival) {
		pc += rival->m_Influence / 4;
	}
/*
 *	pc gives us the % chance of a raid
 *	let's do the "not raided" case first
 */
	if(g_Dice.percent(pc) == false) {
/*
 *		you are clearly a model citizen, sir
 *		and are free to go
 */
 		return;
	}
/*
 *	OK, the raid is on. Start formatting a message
 */
	stringstream ss;
	ss << gettext("The local authorities perform a bust on your operations: ");
/*
 *	if we make our influence check, the guard captain will be under
 *	orders from the mayor to let you off.
 *
 *	Let's make sure the player can tell
 */
	if(g_Dice.percent(m_Influence)) {
		ss << gettext("the guard captain lectures you on the importance of ")
		 << gettext("crime prevention, whilst also passing on the Mayor's ")
		 << gettext("heartfelt best wishes.")
		 ;
		m_Player.suspicion(-5);
		g_MessageQue.AddToQue(ss.str(), 1);
		return;
	}
/*
 *	if we have a rival influencing things, it might not matter
 *	if the player is squeaky clean
 */
	if(m_Player.disposition() > 0 && g_Dice.percent(rival->m_Influence/2)) {
		int fine = (g_Dice%1000)+150;
		g_Gold.fines(fine);
		ss << gettext("the guard captain condemns your operation as a ")
		 << gettext("hotbed of criminal activity and fines you ")
		 << fine
		 << gettext(" gold for 'living without due care and attention'.")
		 ;
		
/*
 *		see if there's a girl using drugs he can nab
 */
		check_druggy_girl(ss);
/*
 *		make sure the player knows why the captain is 
 *		being so blatantly unfair
 */
		ss << gettext("On his way out the captain smiles and says that the ")
		 << rival->m_Name
		 << gettext(" send their regards.")
		 ;
		g_MessageQue.AddToQue(ss.str(), 1);
		return;
	}
/*
 *	if the player is basically a goody-goody type
 *	he's unlikely to have anything incriminating on
 *	the premises. 20 disposition should see him 
 */
	if(g_Dice.percent(m_Player.disposition() * 5)) {
		ss << gettext("they pronounce your operation to be ")
		 << gettext("entirely in accordance with the law.")
		;
		
		m_Player.suspicion(-5);
		g_MessageQue.AddToQue(ss.str(), 1);
		return;
	}

	int nPlayer_Disposition = m_Player.disposition();
	if(nPlayer_Disposition > -10)
	{
		int fine = (g_Dice%100)+20;
		g_Gold.fines(fine);
		ss << gettext("they find in technical violation of some health ")
		 << gettext("and safety ordinances, and they fine you ")
		 << fine 
		 << gettext(" gold.")
		;
	}
	else if(nPlayer_Disposition > -30)
	{
		int fine = (g_Dice%300)+40;
		g_Gold.fines(fine);
		ss << gettext("they find some minor criminalities and fine you ")
		 << fine
		 << gettext(" gold.")
		 ;
	}
	else if(nPlayer_Disposition > -50)
	{
		int fine = (g_Dice%600)+100;
		g_Gold.fines(fine);
		ss << gettext("they find evidence of dodgy dealings and fine you ")
		 << fine 
		 << gettext(" gold.")
		 ;
		
	}
	else if(nPlayer_Disposition > -70)
	{
		int fine = (g_Dice%1000)+150;
		int bribe = (g_Dice%300)+100;
		g_Gold.fines(fine + bribe);
		ss << gettext("they find a lot of illegal activities and fine you ")
		 << fine
		 << gettext(" gold, it also costs you an extra ")
		 << bribe
		 << gettext(" to pay them off from arresting you.")
		 ;
		
	}
	else if(nPlayer_Disposition > -90)
	{
		int fine = (g_Dice%1500)+200;
		int bribe = (g_Dice%600)+100;
		g_Gold.fines(fine + bribe);
		ss << gettext("they find enough dirt to put you behind bars for life. ")
		   << gettext("It costs you ") << bribe << gettext(" to stay out of prison, ")
		   << gettext(" plus another ")
		   << fine
		   << gettext(" in fines on top of that")
		;
	}
	else
	{
		int fine = (g_Dice%2000)+400;
		int bribe = (g_Dice%800)+150;
		g_Gold.fines(fine + bribe);
		ss << gettext("the captain declares your premises to be a sinkhole ")
		   << gettext("of the utmost vice and depravity, and it is only ")
		   << gettext("with difficulty that you dissuade him from seizing ")
		   << gettext("all your property on the spot. You pay ")
		   << fine
		   << gettext(" gold in fines, but only after slipping the captain ")
		   << bribe
		   << gettext(" not to drag you off to prison.")
		;
	}
/*
 *	check for a drug-using girl they can arrest
 */
	check_druggy_girl(ss);
	g_MessageQue.AddToQue(ss.str(), 1);
}

int cBrothelManager::TotalFame(sBrothel * brothel)
{
	int total_fame=0;
	sGirl * current=brothel->m_Girls;
	while (current)
	{
		total_fame+=g_Girls.GetStat(current,STAT_FAME);
		current = current->m_Next;
	}
	return total_fame;
}

bool cBrothelManager::CheckScripts()
{
	sBrothel* current = m_Parent;
	DirPath base = DirPath() << "Resources" << "Characters" << "";
	while(current)
	{
		sGirl* girl;

		for(girl = current->m_Girls; girl; girl = girl->m_Next) {
/*
 *			if no trigger for this girl, skip to the next one
 */
			if(!girl->m_Triggers.GetNextQueItem()) {
				continue;
			}
			string fileloc = base.c_str();
			fileloc += girl->m_Name;
			girl->m_Triggers.ProcessNextQueItem(fileloc);
			return true;
		}
		current = current->m_Next;
	}

	return false;
}

//bool cBrothelManager::UseAntiPreg(bool use)
bool UseAntiPreg(bool use, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, int whereisshe)
{
	if (!use)
		return false;
	/*
	*	anti-preg potions, we probably should allow
	*	on-the-fly restocks. You can imagine someone
	*	noticing things are running low and
	*	sending a girl running to the shops to get
	*	a restock
	*
	*	that said, there's a good argument here for
	*	making this the matron's job, and giving it a
	*	chance dependent on skill level. Could have a
	*	comedy event where the matron forgets, or the
	*	girl forgets (or disobeys) and half a dozen
	*	girls get knocked up.
	*
	*	'course, we could do that anyway.. :)
	*/
	if (isClinic)
	{
		if (g_Clinic.GetBrothel(0)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_Clinic.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Clinic.GetBrothel(0)->m_AntiPregPotions--;
			return true;
		}
	}
	else if (isStudio)
	{
		if (g_Studios.GetBrothel(0)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_Studios.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Studios.GetBrothel(0)->m_AntiPregPotions--;
			return true;
		}
	}
	else if (isArena)
	{
		if (g_Arena.GetBrothel(0)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_Arena.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Arena.GetBrothel(0)->m_AntiPregPotions--;
			return true;
		}
	}
	else if (isCentre)
	{
		if (g_Centre.GetBrothel(0)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_Centre.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Centre.GetBrothel(0)->m_AntiPregPotions--;
			return true;
		}
	}
	else if (isHouse)
	{
		if (g_House.GetBrothel(0)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_House.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_House.GetBrothel(0)->m_AntiPregPotions--;
			return true;
		}
	}
	else
	{
		if (g_Brothels.GetBrothel(whereisshe)->m_KeepPotionsStocked)
		{
			g_Gold.consumable_cost(10);
			return true;
		}
		if (g_Brothels.GetBrothel(whereisshe)->m_AntiPregPotions > 0)
		{
			g_Brothels.GetBrothel(whereisshe)->m_AntiPregPotions--;
			return true;
		}
	}
	return false;
}

/*	`J` replaced with building check
bool cBrothelManager::UseAntiPreg(bool use)
{
	if (!use)
		return false;
	/*
	*	anti-preg potions, we probably should allow
	*	on-the-fly restocks. You can imagine someone
	*	noticing things are running low and
	*	sending a girl running to the shops to get
	*	a restock
	*
	*	that said, there's a good argument here for
	*	making this the matron's job, and giving it a
	*	chance dependent on skill level. Could have a
	*	comedy event where the matron forgets, or the
	*	girl forgets (or disobeys) and half a dozen
	*	girls get knocked up.
	*
	*	'course, we could do that anyway.. :)
	* /
	if (m_KeepPotionsStocked)
	{
		g_Gold.consumable_cost(10);
		return true;
	}

	if (m_AntiPregPotions > 0)
	{
		m_AntiPregPotions--;
		return true;
	}

	return false;
}
bool cBrothelManager::UseAntiPreg(bool use, int BrothelID)
{
	if (!use)
		return false;
	if (m_KeepPotionsStocked)
	{
		g_Gold.consumable_cost(10);
		return true;
	}

	if (m_AntiPregPotions > 0)
	{
		m_AntiPregPotions--;
		return true;
	}

	return false;
}
*/

//void cBrothelManager::AddAntiPreg(int amount)
void sBrothel::AddAntiPreg(int amount) // unused
{
	m_AntiPregPotions += amount;
	if(m_AntiPregPotions > 700)
		m_AntiPregPotions=700;
}


// ----- Add / remove
void cBrothelManager::AddGirl(int brothelID, sGirl* girl)
{
	if(girl == 0)
		return;
	if(girl->m_InMovieStudio)
		girl->m_DayJob = girl->m_NightJob = JOB_FILMFREETIME;
		//girl->m_DayJob = girl->m_NightJob = JOB_RESTING;
	else if(girl->m_InArena)
		girl->m_DayJob = girl->m_NightJob = JOB_ARENAREST;
	else if(girl->m_InCentre)
		girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
	else if(girl->m_InClinic)
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
	else if(girl->m_InHouse)
		girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
	else
		girl->m_DayJob = girl->m_NightJob = JOB_RESTING;
		//girl->m_DayJob = girl->m_NightJob = JOB_FILMFREETIME;

	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}
	g_Girls.RemoveGirl(girl, false);
	girl->where_is_she = brothelID;

	girl->m_Prev = girl->m_Next = 0;
	if(current->m_Girls)
	{
		girl->m_Prev = current->m_LastGirl;
		current->m_LastGirl->m_Next = girl;
		current->m_LastGirl = girl;
	}
	else
		current->m_LastGirl = current->m_Girls = girl;

	current->m_NumGirls++;
	//sort(current);
}

void cBrothelManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	if(girl == 0)
		return;

	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	sGirl* t = current->m_Girls;
	bool found = false;
	while(t)
	{
		if(t == girl)
		{
			found = true;
			break;
		}
		t = t->m_Next;
	}
	t = 0; // MYR

	if(found == true)
	{
		if(girl->m_Next)
			girl->m_Next->m_Prev = girl->m_Prev;
		if(girl->m_Prev)
			girl->m_Prev->m_Next = girl->m_Next;
		if(girl == current->m_Girls)
			current->m_Girls = girl->m_Next;
		if(girl == current->m_LastGirl)
			current->m_LastGirl = girl->m_Prev;

		girl->m_Next = girl->m_Prev = 0;

		if(deleteGirl)
		{
			delete girl;
			girl = 0;
		}
		current->m_NumGirls--;
	}
}

// ----- Load save create destroy
void cBrothelManager::LoadDataLegacy(ifstream& ifs)
{
	Free();
	int temp;
	string message = "";

	// load the player
	//         ...................................................
	message = "***************** loading Player ******************";
	g_LogFile.write(message);
	m_Player.LoadPlayerLegacy(ifs);

	// load the dungeon
	//         ...................................................
	message = "***************** loading dungeon *****************";
	g_LogFile.write(message);
	m_Dungeon.LoadDungeonDataLegacy(ifs);

	// load preg potions, supply shed level, other goodies
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
//	ifs >> m_AntiPregPotions >> m_SupplyShedLevel >> m_HandmadeGoods >> m_Beasts >> m_AlchemyIngredients;
	ifs >> m_SupplyShedLevel >> m_HandmadeGoods >> m_Beasts >> m_AlchemyIngredients;

	// load runaways
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	int numGirlsToLoad = 0;
	ifs>>numGirlsToLoad;
	m_NumPrison = 0;
	for(int i=0; i<numGirlsToLoad; i++)
	{
		sGirl* rgirl = new sGirl();
		g_Girls.LoadGirlLegacy(rgirl,ifs);

		AddGirlToRunaways(rgirl);
	}

	// load prison
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	numGirlsToLoad = 0;
	ifs>>numGirlsToLoad;
	m_NumPrison = 0;
	for(int i=0; i<numGirlsToLoad; i++)
	{
		sGirl* pgirl = new sGirl();
		g_Girls.LoadGirlLegacy(pgirl,ifs);

		AddGirlToPrison(pgirl);
	}

	// load bribe rate and bank
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_BribeRate>>m_Bank;

	// load objective
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	if(temp == 1)
	{
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		m_Objective = new sObjective();
		ifs>>m_Objective->m_Difficulty>>m_Objective->m_Limit>>m_Objective->m_Objective>>m_Objective->m_Reward>>m_Objective->m_SoFar>>m_Objective->m_Target;
	}

	// load rivals
	//         ...................................................
	message = "***************** Loading rivals *****************";
	g_LogFile.write(message);
	m_Rivals.LoadRivalsLegacy(ifs);

	// Load inventory
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	m_NumInventory = temp;
	//         ...................................................
	message = "************ Loading players inventory ************";
	g_LogFile.write(message);
	sInventoryItem * tempitem=0;
	int total_num=0;
	for(int i=0; i<m_NumInventory;i++)
	{
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		tempitem = g_InvManager.GetItem(buffer);

		message = "Loading item: ";
		message += buffer;
		g_LogFile.write(message);

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if(tempitem)
		m_EquipedItems[total_num]= temp;
		ifs>>temp;
		if(tempitem)
		{
			m_NumItem[total_num] = temp;
		
			m_Inventory[total_num]=tempitem;
			total_num++;
		}
	}
	m_NumInventory=total_num;

/*	`J` moved potion control to individual buildings
	// load potions restock
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_KeepPotionsStocked;
*/

	// load alcohol restock
//	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
//	ifs>>m_KeepAlcStocked;

	// Number of brothels
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	int numBrothelsToLoad = 0;
	ifs>>numBrothelsToLoad;
	m_NumBrothels = 0;
	//         ...................................................
	message = "***************** Loading brothels ****************";
	g_LogFile.write(message);
	for(int j=0; j<numBrothelsToLoad; j++)
	{
		sBrothel* current = new sBrothel();
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		current->m_Name = buffer;

		message = "Loading brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		// load variables for sex restrictions
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictAnal = true;
		else
			current->m_RestrictAnal = false;
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictBDSM = true;
		else
			current->m_RestrictBDSM = false;
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictOral = true;
		else
			current->m_RestrictOral = false;
		if(temp == 1)
			current->m_RestrictTitty = true;
		else
			current->m_RestrictTitty = false;
		if(temp == 1)
			current->m_RestrictBeast = true;
		else
			current->m_RestrictBeast = false;
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictGroup = true;
		else
			current->m_RestrictGroup = false;
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictNormal = true;
		else
			current->m_RestrictNormal = false;
		ifs>>temp;
		if(temp == 1)
			current->m_RestrictLesbian = true;
		else
			current->m_RestrictLesbian = false;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		current->m_AdvertisingBudget = temp;
		ifs>>temp;
		current->m_Bar = temp;
		ifs>>temp;
		current->m_Fame = temp;
		ifs>>temp;
		current->m_GamblingHall = temp;
		ifs>>temp;
		current->m_Happiness = temp;
		ifs>>temp;
		current->m_HasBarStaff = temp;
		ifs>>current->m_id;
		ifs>>temp;
		current->m_HasGambStaff = temp;
		//ifs>>current->m_MovieRunTime;
		//ifs>>temp;
		current->m_NumGirls = temp;
		ifs>>temp;
		current->m_NumRooms = temp;
		ifs>>current->m_ShowQuality;
		ifs>>current->m_ShowTime;
//		ifs>>current->m_Upkeep;
		ifs>>current->m_Filthiness;
		ifs>>current->m_SecurityLevel;

		//ifs>>current->m_Finance;
		current->m_Finance.loadGoldLegacy(ifs);
		// load building qualities
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		for(u_int i=0; i<NUMJOBTYPES; i++)
			ifs>>current->m_BuildingQuality[i];

		/*
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		int count = 0;
		ifs>>count;
		for(int i=0; i<count; i++)
		{
			if (ifs.peek()=='\n') ifs.ignore(1,'\n');
			ifs>>temp;
			//when you create a new movie, you set m_Quality to quality*0.5
			//but you directly save m_Quality, so this undoes the division
			temp *= 2;
			NewMovie(current, temp);
		}
		*/

		// Load girls
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		numGirlsToLoad = current->m_NumGirls;
		current->m_NumGirls = 0;
		for(int i=0; i<numGirlsToLoad; i++)
		{
			sGirl* girl = new sGirl();
			g_Girls.LoadGirlLegacy(girl, ifs);

			message = "Loading girl: ";
			message += girl->m_Realname;
			g_LogFile.write(message);

			current->AddGirl(girl);
		}

		//current->building.load(ifs);

		AddBrothel(current);
	} // load a brothel
}

bool cBrothelManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too

	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0)
	{
		return false;
	}

	string message = "";

	// load the player
	//         ...................................................
	message = "***************** loading Player ******************";
	g_LogFile.write(message);
	m_Player.LoadPlayerXML(hBrothelManager.FirstChild("Player"));

	// load the dungeon
	//         ...................................................
	message = "***************** loading dungeon *****************";
	g_LogFile.write(message);
	m_Dungeon.LoadDungeonDataXML(hBrothelManager.FirstChild("Dungeon"));

	// load preg potions, supply shed level, other goodies
	/* `J` moved to individual buildings
	pBrothelManager->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	*/
	pBrothelManager->QueryIntAttribute("SupplyShedLevel", &m_SupplyShedLevel);
	pBrothelManager->QueryIntAttribute("HandmadeGoods", &m_HandmadeGoods);
	pBrothelManager->QueryIntAttribute("Beasts", &m_Beasts);
	pBrothelManager->QueryIntAttribute("AlchemyIngredients", &m_AlchemyIngredients);

	// load runaways
	m_NumRunaways = 0;
	TiXmlElement* pRunaways = pBrothelManager->FirstChildElement("Runaways");
	if (pRunaways)
	{
		for(TiXmlElement* pGirl = pRunaways->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* rgirl = new sGirl();
			bool success = rgirl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				AddGirlToRunaways(rgirl);
			}
			else
			{
				delete rgirl;
				continue;
			}
		}
	}

	// load prison
	m_NumPrison = 0;
	TiXmlElement* pPrisonGirls = pBrothelManager->FirstChildElement("PrisonGirls");
	if (pPrisonGirls)
	{
		for(TiXmlElement* pGirl = pPrisonGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* pgirl = new sGirl();
			bool success = pgirl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true)
			{
				AddGirlToPrison(pgirl);
			}
			else
			{
				delete pgirl;
				continue;
			}
		}
	}

	// load bribe rate and bank
	pBrothelManager->QueryValueAttribute<long>("BribeRate", &m_BribeRate);
	pBrothelManager->QueryValueAttribute<long>("Bank", &m_Bank);

	// load objective
	TiXmlElement* pObjective = pBrothelManager->FirstChildElement("Objective");
	if (pObjective)
	{
		m_Objective = new sObjective();
		pObjective->QueryIntAttribute("Difficulty", &m_Objective->m_Difficulty);
		pObjective->QueryIntAttribute("Limit", &m_Objective->m_Limit);
		pObjective->QueryValueAttribute<u_int>("Objective", &m_Objective->m_Objective);
		pObjective->QueryIntAttribute("Reward", &m_Objective->m_Reward);
		pObjective->QueryIntAttribute("SoFar", &m_Objective->m_SoFar);
		pObjective->QueryIntAttribute("Target", &m_Objective->m_Target);
	}

	// load rivals
	//         ...................................................
	message = "***************** Loading rivals *****************";
	g_LogFile.write(message);
	m_Rivals.LoadRivalsXML(hBrothelManager.FirstChild("Rival_Manager"));

	// Load inventory
	//         ...................................................
	message = "************ Loading players inventory ************";
	g_LogFile.write(message);
	//now would be a great time to move this to cPlayer
	LoadInventoryXML(hBrothelManager.FirstChild("Inventory"),
		m_Inventory, m_NumInventory, m_EquipedItems, m_NumItem);

	/* `J` moved to individual buildings
	// load potions restock
	pBrothelManager->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);
	*/

	// load alcohol restock
//	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
//	ifs>>m_KeepAlcStocked;

	//         ...................................................
	message = "***************** Loading brothels ****************";
	g_LogFile.write(message);
	m_NumBrothels = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Brothels");
	if (pBrothels)
	{
		for(TiXmlElement* pBrothel = pBrothels->FirstChildElement("Brothel");
			pBrothel != 0;
			pBrothel = pBrothel->NextSiblingElement("Brothel"))
		{
			sBrothel* current = new sBrothel();
			bool success = current->LoadBrothelXML(TiXmlHandle(pBrothel));
			if (success == true)
			{
				AddBrothel(current);
			}
			else
			{
				delete current;
				continue;
			}

		} // load a brothel
	}
	return true;
}

bool sBrothel::LoadBrothelXML(TiXmlHandle hBrothel)
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

	std::string message = "Loading brothel: ";
	message += m_Name;
	g_LogFile.write(message);

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
	// `J` Added to save potion stuff in individual buildings
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);
	if (m_AntiPregPotions < 0){ m_AntiPregPotions = 0; }
	if (m_KeepPotionsStocked != 0 && m_KeepPotionsStocked != 1){ m_KeepPotionsStocked = 0; }

	pBrothel->QueryIntAttribute("Bar", &tempInt); m_Bar = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute(gettext("Fame"), &tempInt); m_Fame = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("GamblingHall", &tempInt); m_GamblingHall = tempInt; tempInt = 0;
	pBrothel->QueryValueAttribute<unsigned short>("Happiness", &m_Happiness);
	pBrothel->QueryIntAttribute("HasBarStaff", &tempInt); m_HasBarStaff = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("id", &m_id);
	pBrothel->QueryIntAttribute("HasGambStaff", &tempInt); m_HasGambStaff = tempInt; tempInt = 0;
	//pBrothel->QueryIntAttribute("MovieRunTime", &m_MovieRunTime);
	pBrothel->QueryIntAttribute("NumRooms", &tempInt); m_NumRooms = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("ShowQuality", &m_ShowQuality);
	pBrothel->QueryIntAttribute("ShowTime", &m_ShowTime);
	pBrothel->QueryIntAttribute("Filthiness", &m_Filthiness);
	pBrothel->QueryIntAttribute("SecurityLevel", &m_SecurityLevel);

	m_Finance.loadGoldXML(hBrothel.FirstChild("Gold"));

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

void sBrothel::AddGirl(sGirl* pGirl)
{
	if(m_Girls)
	{
		pGirl->m_Prev = m_LastGirl;
		m_LastGirl->m_Next = pGirl;
		m_LastGirl = pGirl;
	}
	else
	{
		m_LastGirl = m_Girls = pGirl;
	}
	++m_NumGirls;
}

TiXmlElement* cBrothelManager::SaveDataXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothelManager = new TiXmlElement("Brothel_Manager");
	pRoot->LinkEndChild(pBrothelManager);
	string message;

	// save the Player
	//         ...................................................
	message = "************* saving Player data ******************";
	g_LogFile.write(message);
	m_Player.SavePlayerXML(pBrothelManager);

	// save the dungeon
	//         ...................................................
	message = "************* saving dungeon data *****************";
	g_LogFile.write(message);
	m_Dungeon.SaveDungeonDataXML(pBrothelManager);

	// save preg potions, supply shed level, other goodies
//	pBrothelManager->SetAttribute("AntiPregPotions", m_AntiPregPotions); // `J` moved antipreg to individual buildings
	pBrothelManager->SetAttribute("SupplyShedLevel", m_SupplyShedLevel);
	pBrothelManager->SetAttribute("HandmadeGoods", m_HandmadeGoods);
	pBrothelManager->SetAttribute("Beasts", m_Beasts);
	pBrothelManager->SetAttribute("AlchemyIngredients", m_AlchemyIngredients);

	// save runaways
	TiXmlElement* pRunaways = new TiXmlElement("Runaways");
	pBrothelManager->LinkEndChild(pRunaways);
	sGirl* rgirl = m_Runaways;
	while(rgirl)
	{
		rgirl->SaveGirlXML(pRunaways);
		rgirl = rgirl->m_Next;
	}

	// save prison
	TiXmlElement* pPrison = new TiXmlElement("PrisonGirls");
	pBrothelManager->LinkEndChild(pPrison);
	sGirl* pgirl = m_Prison;
	while(pgirl)
	{
		pgirl->SaveGirlXML(pPrison);
		pgirl = pgirl->m_Next;
	}

	// save bribe rate and bank
	pBrothelManager->SetAttribute("BribeRate", m_BribeRate);
	pBrothelManager->SetAttribute("Bank", m_Bank);

	// save objective
	if(m_Objective)
	{
		TiXmlElement* pObjective = new TiXmlElement("Objective");
		pBrothelManager->LinkEndChild(pObjective);
		pObjective->SetAttribute("Difficulty", m_Objective->m_Difficulty);
		pObjective->SetAttribute("Limit", m_Objective->m_Limit);
		pObjective->SetAttribute("Objective", m_Objective->m_Objective);
		pObjective->SetAttribute("Reward", m_Objective->m_Reward);
		pObjective->SetAttribute("SoFar", m_Objective->m_SoFar);
		pObjective->SetAttribute("Target", m_Objective->m_Target);
	}

	// save rivals
	//         ...................................................
	message = "***************** Saving rivals *******************";
	g_LogFile.write(message);
	m_Rivals.SaveRivalsXML(pBrothelManager);

	// save inventory
	//         ...................................................
	message = "************** Saving players inventory ***********";
	g_LogFile.write(message);
	TiXmlElement* pInventory = new TiXmlElement("Inventory");
	pBrothelManager->LinkEndChild(pInventory);
	SaveInventoryXML(pInventory, m_Inventory, MAXNUM_INVENTORY, m_EquipedItems, m_NumItem);

	/* `J` moved to individual buildings
	// save potions restock 
	pBrothelManager->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);
	*/

	// save alcohol restock
//	ofs<<m_KeepAlcStocked<<endl;

	// save brothels
	TiXmlElement* pBrothels = new TiXmlElement("Brothels");
	pBrothelManager->LinkEndChild(pBrothels);
	sBrothel* current = m_Parent;
	//         ...................................................
	message = "***************** Saving brothels *****************";
	g_LogFile.write(message);
	while(current)
	{
		message = "Saving brothel: ";
		message += current->m_Name;
		g_LogFile.write(message);

		current->SaveBrothelXML(pBrothels);
		current = current->m_Next;
	}
	return pBrothelManager;
}

TiXmlElement* sBrothel::SaveBrothelXML(TiXmlElement* pRoot)
{
	TiXmlElement* pBrothel = new TiXmlElement("Brothel");
	pRoot->LinkEndChild(pBrothel);
	pBrothel->SetAttribute("Name", m_Name);

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
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);

	pBrothel->SetAttribute("Bar", m_Bar);
	pBrothel->SetAttribute("Fame", m_Fame);
	pBrothel->SetAttribute("GamblingHall", m_GamblingHall);
	pBrothel->SetAttribute("Happiness", m_Happiness);
	pBrothel->SetAttribute("HasBarStaff", m_HasBarStaff);
	pBrothel->SetAttribute("id", m_id);
	pBrothel->SetAttribute("HasGambStaff", m_HasGambStaff);
	//pBrothel->SetAttribute("MovieRunTime", m_MovieRunTime);
	pBrothel->SetAttribute("NumRooms", m_NumRooms);
	pBrothel->SetAttribute("ShowQuality", m_ShowQuality);
	pBrothel->SetAttribute("ShowTime", m_ShowTime);
	pBrothel->SetAttribute("Filthiness", m_Filthiness);
	pBrothel->SetAttribute("SecurityLevel", m_SecurityLevel);

	m_Finance.saveGoldXML(pBrothel);

	// save building qualities
#if 0
	//building qualities appears to be unimplemented, why waste space
	TiXmlElement* pBuildingQualities = new TiXmlElement("BuildingQualities");
	pBrothel->LinkEndChild(pBuildingQualities);
	SaveJobsXML(pBuildingQualities, m_BuildingQuality);
#endif

	// Save Girls
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pBrothel->LinkEndChild(pGirls);
	sGirl* girl = m_Girls;
	while(girl)
	{
		girl->SaveGirlXML(pGirls);
		girl = girl->m_Next;
	}
/*
*		save the building setup
*/
	//this was commented out before the conversion to XML
	//current->building.save(ofs, current->m_Name);
	return pBrothel;
}

void cBrothelManager::NewBrothel(int NumRooms)
{
	sBrothel* newBroth = new sBrothel();
	newBroth->m_NumRooms = NumRooms;
	newBroth->m_Next = 0;

	AddBrothel(newBroth);
}

void cBrothelManager::AddBrothel(sBrothel* newBroth)
{
	if(m_Parent)
	{
		m_Last->m_Next = newBroth;
		newBroth->m_id = m_Last->m_id+1;
		m_Last = newBroth;
	}
	else
	{
		m_Parent = m_Last =  newBroth;
		newBroth->m_id = 0;
	}
	m_NumBrothels++;
}

void cBrothelManager::DestroyBrothel(int ID)
{
	sBrothel* current = m_Parent;

	if(current->m_id == ID)
	{
		m_Parent = current->m_Next;
		current->m_Next = 0;
		delete current;
		return;
	}

	while(current->m_Next)
	{
		if(current->m_Next->m_id == ID)
			break;
		current = current->m_Next;
	}

	if(current)
	{
		sBrothel* temp = current->m_Next;

		current->m_Next = temp->m_Next;

		temp->m_Next = 0;
		delete temp;

		m_NumBrothels--;
	}
}

// ----- Rival
void cBrothelManager::check_rivals()
{
	int num_rivals = m_Rivals.GetNumRivals();
	static bool peace = false;
/*
 *	a full set of rivals = nothing to do
 */
	if(num_rivals > 5) {
		return;
	}
/*
 *	if there are no rivals, and we were not
 *	at peace last turn, peace has broken out
 */
	if(num_rivals == 0 && !peace) {
		peace = true;
		peace_breaks_out();
	}
/*
 *	we only create new rivals after the game has
 *	been won
 */
	if(m_Player.m_WinGame == false) {
		return;
	}
/*
 *	create new random rival
 */
	if(g_Dice.percent(70)) {
		return;		// or not!
	}
/*
 *	flag the war as on again, (should be a field somewhere)
 *	create a new rival and tell the player the good news
 */
	peace = false;
	m_Rivals.CreateRandomRival();
	g_MessageQue.AddToQue(new_rival_text(), 2);
}

string cBrothelManager::new_rival_text()
{
	stringstream ss;

	enum { 
		Slaver		= 0,
		Gladiator	= 1,
		Goon		= 2,
		Slave		= 3,
		Mage		= 4,
		Demon		= 5,
		Priest		= 6,
		Noble		= 7,
		Technologist	= 8,
		Patriarch	= 9,	// or Matriarch
		MaxChallengers	= 10
	};

	bool male = g_Dice.percent(75);
/*
 *	let's put the gender specific terms in
 *	variables. Might make the code cleaner
 */
	string man, boy, He, he, him, his, sorcerer, gladiator;
	string fellow, patriarch;
	if(male) {
		He  = gettext("He");
		he  = gettext("he");
		him = gettext("him");
		his = gettext("his");
		man = gettext("man");
		boy = gettext("boy");
		sorcerer = gettext("sorcerer");
		gladiator = gettext("gladiator");
		fellow = gettext("fellow");
		patriarch = gettext("patriarch ");
	}
	else {
		He  = gettext("She");
		he  = gettext("she");
		him = gettext("her");
		his = gettext("her");
		man = gettext("woman");
		boy = gettext("girl");
		sorcerer = gettext("sorceress");
		gladiator = gettext("gladiatrix");
/*
 *		not sure what the feminine of "fellow" is
 *		I did wonder about "fellatrix"...
 */
		fellow = gettext("wench");
		patriarch = gettext("matriarch ");
	}

	switch(g_Dice.random(MaxChallengers)) {
	case Slaver:
		ss << gettext("A lieutenant reports that one of the ")
		   << gettext("professional slavers, finding customers ")
		   << gettext("be scarce, has taken to whoring out ")
		   << his << gettext(" slavegirls to make ends meet.")
		   << gettext("Your men arranged a meet with ") << him
		   << gettext(" in order to explain your position ")
		   << gettext("on the subject, but the discussion ")
		   << gettext("did not go well, ending with bared ")
		   << gettext("steel and threats of blood.")
		   << gettext("\n\n")
		   << gettext("It would seem you have a challenger.")
		;
		break;
	case Gladiator:
		ss << gettext("Ask any Crossgate sports fan who rules the Arenas ");
		ss << gettext("of the city.  Almost always, the answer will be ");
		ss << gettext("the same. For five long years one ")<<gladiator<<gettext(" has ");
		ss << gettext("stood ")<<his<<gettext(" ground on the bloody sands and defied ");
		ss << gettext("all who came before ") << him << gettext(".");
		ss << gettext("\n\n");
		ss << gettext("Last week, the ") << gladiator << gettext(" bought ") << his ;
		ss << gettext(" freedom from ");
		ss << gettext("the arena, and chose to celebrate the occasion at ");
		ss << gettext("one of your brothels.  Sadly, an overindulgence ");
		ss << gettext("in wine led to harsh words and a rash vow to ");
		ss << gettext("show you how a whorehouse SHOULD be run. ");
		ss << gettext("\n\n");
		ss << gettext("With anyone else, the matter would have ended ");
		ss << gettext("when the morning brought sobriety. But this is ");
		ss << gettext("a ")<<man<<gettext(" who has never turned ")<<his<<gettext(" back ");
		ss << gettext("on any sort of challenge. With wealthy admirers ");
		ss << gettext("supplying premises and finance, and with a handful ");
		ss << gettext("of arena veterans to provide the core of ") << his;
		ss << gettext(" enforcers, this ");
		ss << gettext("is a challenger you would be foolish to ignore.");
		break;
	case Goon:
		ss <<	gettext("The ") <<boy<< gettext(" was just skin and bones; a dull eyed ")
			<< gettext("waif from gutters of Sleaze Street, a dozen like ")
			<< him << gettext(" on any street corner. But put a knife in ")
			<< his << gettext(" hands and the ")<<boy<<gettext(" became an artist, ")
			<< gettext("painting effortless masterpieces in blood and ")
			<< gettext("greased lightning. ")
			<< gettext("\n\n")
			<< gettext("Quickly recruited into one of the goon squads, ")
			<< gettext("it soon became apparent that behind that flat ")
			<< gettext("unblinking stare, there lurked a mind almost ")
			<< gettext("as keen as ")<< his<<gettext(" blades. The ")<<boy<<gettext(" rose quickly, ")
			<< gettext("coming to head ")<<his<<gettext(" own squad before becoming ")
			<< gettext("one of your trusted lieutenants. If only ")<<his<<gettext(" ")
			<< gettext("ambition had stopped there... ")
			<< gettext("\n\n")
			<< "" << (male? gettext("His") : gettext("Her")) << gettext(" challenge ")
			<< gettext("was almost over before it began; ")
			<< gettext("for you that is. That you still live says more ")
			<< gettext("about the skill of your healers than any talent ")
			<< gettext("you might lay claim to.  Your newest rival is not ")
			<< gettext("only a deadly fighter and a clever strategist, ")
			<< gettext("but one who knows your operation, inside and out. ")
			<< gettext("\n\n")
			<< gettext("This will not be easy.")
		;
		break;
	case Slave:
		ss << 	gettext("There are ways to beat a slaver tattoo. It wouldn't ")
			<< gettext("do were that to become widely known, of course. ")
			<< gettext("Nevertheless there are ways around it. ")
			<< gettext("One such is to find an area of unstable spacetime. ")
			<< gettext("Do it right, and you can overload the tracking ")
			<< gettext("spell, and the enchantment just falls apart")
			<< gettext("This is, of course wildly dangerous, but many ")
			<< gettext("escapees nevertheless head straight for the ")
			<< gettext("Crossgate sewers, which on a bad day can give ")
			<< gettext("the catacombs a run for their money.")
			<< gettext("\n\n")
			<< gettext("Over time, a community of ecapees has grown up ")
			<< gettext("in the sewers, survivor types, grown hardy in ")
			<< gettext("the most hostile environment. And as long as they ")
			<< gettext("stay down there, no one much minds. If nothing else ")
			<< gettext("they keep the monster population down. But now ")
			<< gettext("they seem to be organising a crusade. Against ")
			<< gettext("slavery. Against exploitation. Against you.")
			<< gettext("\n\n")
			<< gettext("Rumour has it that their leader is one of your ")
			<< gettext("offspring, conceived of rape, born into slavery. ")
			<< gettext("True or not, this new factions seems determined ")
			<< gettext("to bring about your downfall.")
			<< gettext("\n\n")
			<< gettext("This time, as the bards would say, it is personal.")
		;
		break;
	case Mage:
		ss <<	gettext("The ")<<sorcerer<<gettext(" blew into town with a travelling ");
		ss <<	gettext("entertainer show, promising exotic pleasures ");
		ss <<	gettext("and the taste of forbidden fruit. But behind the ");
		ss <<	gettext("showman's patter and the coloured smoke, the pleasures ");
		ss <<	gettext("on offer were of a distinctly carnal nature, and no ");
		ss <<	gettext("more exotic than those you yourself could offer.");
		ss <<	gettext("\n\n");
		ss <<	gettext("For a travelling show, this need not be a ");
		ss <<	gettext("problem. For a week, or even two, you can ");
		ss <<	gettext("stand to see a little competition. However, ");
		ss <<	gettext("the newcomer has been here a month now and ");
		ss <<	gettext("shows no sign of moving on. On the contrary, ");
		ss <<	gettext("he appears to be shopping for permanent premises.");
		ss <<	gettext("\n\n");
		ss <<	gettext("With this in mind, you send some men ");
		ss <<	gettext("to explain the situation. ");
		ss <<	gettext("To everyone's surprise, it turns out ");
		ss <<	gettext("that behind the glib charlatanry, there lies ");
		ss <<	gettext("genuine magecraft, most likely tantric in nature.");
		ss <<	gettext("\n\n");
		ss <<	gettext("In your organisation you have no shortage of ");
		ss <<	gettext("mages. Any fighting force in Crossgate needs ");
		ss <<	gettext("a battle mage or two. This newcomer however ");
		ss <<	gettext("operates on a level far beyond what you are ");
		ss <<	gettext("used to. And he seems determined to stay, and ");
		ss <<	gettext("challenge you for control of the city.");


		break;
	case Priest:
		break;
	case Noble:
		ss << gettext("They say ")<<he<<gettext(" is a noble, an exile from ")<<his<<gettext(" ");
		ss << gettext("native land. Certainly, ")<<he<<gettext(" has the manners of a courtier ");
		ss << gettext("and the amused weariness of the jaded dilettante.");
		ss << gettext("\n\n");
		ss << gettext("And yet it seems there is steel behind the foppery, as ");
		ss << gettext("many a Crossgate duelist has learned. And a wit to ");
		ss << gettext("match the blade as well. An admirable ")<<fellow<<gettext(" this, ");
		ss << gettext("one you would be pleased to call 'friend', if only ...");
		ss << gettext("\n\n");
		ss << gettext("Earlier this week, your men were explaining to a handful of ");
		ss << gettext("freelance scrubbers how ");
		ss << gettext("prostitution worked in this city. If only ")<<he<<gettext(" had not chosen ");
		ss << gettext("to take the women's side against your men. If only ");
		ss << his<<gettext(" rash defiance had not caught the imagination of ");
		ss << gettext("the city's duellists.");
		ss << gettext("\n\n");
		ss << gettext("Alas, such was not to be.");
		ss << gettext("\n\n");
		ss << gettext("En Garde!");
		break;
	case Technologist:
		ss << gettext("From the distant city of Abby's Crossing comes a new ");
		ss << gettext("rival to challenge for your throne, wielding some ");
		ss << gettext("strange non-magic ")<<he<<gettext(" calls 'technology', ");
		ss << gettext("an alien art of smoke and steam and noise and ");
		ss << gettext("lighting; one they say functions strangely in Mundiga, ");
		ss << gettext("when it chooses to work at all.");
		ss << gettext("\n\n");
		ss << gettext("But the hollow metal men that make up ") << his;
		ss << gettext(" enforcers would seem to work with deadly efficicency ");
		ss << gettext("and the strange collapsible maze ")<<he<<gettext(" calls a ");
		ss << gettext("'tesseract' seems to share many properties with the ");
		ss << gettext("catacombs under your headquarters. Then there are ");
		ss << gettext("rumours of strange procedures that can break a ");
		ss << gettext("slavegirl's will, far faster than the most skilled of ");
		ss << gettext("Crossgate's torturers.");
		ss << gettext("\n\n");
		ss << gettext("In short, far from unreliable, ")<<his<<gettext(" arts seem deadly ");
		ss << gettext("efficient to you. You have no idea what other surprises ");
		ss << gettext("this otherworldly artisan may have up ")<<his<<gettext(" sleeve, but ");
		ss << gettext("one thing is for certain: this challenge may not go ");
		ss << gettext("unanswered.");
		break;
	case Patriarch:
		ss << gettext("Outside the walls of Crossgate, there is a shanty-town ");
		ss << gettext("maze of tumbledown hovels, teeming with the poorest ");
		ss << gettext("and most desperate of the City's inhabitants. Polygamy ");
		ss << gettext("and incest are rife here, and extended families can ");
		ss << gettext("run into the hundreds");
		ss << gettext("\n\n");
		ss << gettext("One such family is ruled by the iron will of a ");
		ss << gettext("dreadful old ") << patriarch << gettext(" with a well earned ");
		ss << gettext("reputation for utter ruthlessness. For years ") << he << gettext(" ");
		ss << gettext("has sent ")<<his<<gettext(" progeny to the city markets, to trade, to ");
		ss << gettext("steal, to bring back money for the clan in any way ");
		ss << gettext("they can.");
		ss << gettext("\n\n");
		ss << gettext("Now it seems they are expanding their operation to ");
		ss << gettext("include organised prostitution. Bad move.");
		ss << gettext("\n\n");
		ss << gettext("Something about the ")<<patriarch<<gettext("'s operation ");
		ss << gettext("disturbs you. There is a coldness in the way ")<<he<<gettext(" ");
		ss << gettext("sends sons and grandsons out to die for ")<<him<<gettext("; the way ");
		ss << he<< gettext("casually rapes and enslaves ")<<his<<gettext(" own daughters and ");
		ss << gettext("granddaughters before sending them off to whore for ");
		ss << him<<gettext(". This ")<<man<<gettext(" holds up a mirror to what you are ");
		ss << gettext("- or perhaps to what you could easily become. The ");
		ss << gettext("image it presents is far from flattering.");
		ss << gettext("\n\n");
		ss << gettext("Personal feelings aside, this is a situation that ");
		ss << gettext("can only get worse. The time to end this, is now.");
		break;
	case Demon:
		ss << gettext("Somewhere in Crossgate, a hand trembled ");
		ss << gettext("inscribing a pentagram; a tongue stumbled over ");
		ss << gettext("the nine syllables of the charm of binding. ");
		ss << gettext("A magical being slipped his arcane bonds ");
		ss << gettext("and slaughtered those mages foolish enough to dream ");
		ss << gettext("they might command it.");
		ss << gettext("\n\n");
		ss << gettext("A demon lord now stalks the streets of the city.");
		ss << gettext("\n\n");
		ss << gettext("Which, in itself, is not so big a deal. It is not of ");
		ss << gettext("unheard that the aristocracy of Hell should find ");
		ss << gettext("themselves stumbling dazed and confused through ");
		ss << gettext("Crossgate market.  They just tend to recover quickly ");
		ss << gettext("and promptly open a portal home.");
		ss << gettext("\n\n");
		ss << gettext("But not this one. This one chooses to briefly linger, ");
		ss << gettext("to partake of Crossgate society and seek such ");
		ss << gettext("amusements as the city can offer. Unfortunately, it ");
		ss << gettext("seems the demon finds amusement trafficking in human ");
		ss << gettext("misery and human sex. As do you, in the eyes of many.");
		ss << gettext("\n\n");
		ss << gettext("For a demon, 'briefly' may be anything from a ");
		ss << gettext("day to a thousand years. You cannot afford to wait ");
		ss << gettext("until it grows bored. A demon lord is a formidable ");
		ss << gettext("opponent, but to ignore this challenge will send ");
		ss << gettext("entirely the wrong signal to the other would be ");
		ss << gettext("whore-masters in the city.");
		ss << gettext("\n\n");
		ss << gettext("Like it or not, this means war.");
		break;
	}
	return ss.str();
}

void cBrothelManager::peace_breaks_out()
{
/*
 *	if the PC already won, this is just an minor outbreak
 *	of peace in the day-to-day feuding in crossgate
 */
	if(m_Player.m_WinGame) {
		string s = gettext("The last of your challengers has been overthrown. ");
		s += gettext("Your domination of Crossgate is absolute.\n\n");
		s += gettext("Until the next time that is...");
		g_MessageQue.AddToQue(
			s, 1
		);
		return;
	}
/*
 *	otherwise, the player has just won
 *	flag it as such
 */
	m_Player.m_WinGame = true;
/*
 *	let's have a bit of chat to mark the event
 */
	string s = gettext("The last of your father's killers has been ");
	s += gettext("brought before you for judgement. None ");
	s += gettext("remain who would dare to oppose you. For all intents ");
	s += gettext("and purposes, the city is yours.");
	s += gettext("\n\n");
	s += gettext("Whether or not your father will rest easier for ");
	s += gettext("your efforts, you cannot say, but now, with the ");
	s += gettext("city at your feet, you feel sure he would be proud ");
	s += gettext("of you at this moment.");
	s += gettext("\n\n");
	s += gettext("But pride comes before a fall, and in Crossgate, ");
	s += gettext("complacency kills. The city's slums and slave markets ");
	s += gettext("and the fighting pits are full of hungry young bloods ");
	s += gettext("burning to make their mark on the world, and any one of ");
	s += gettext("them could rise to challenge you at any time.");
	s += gettext("\n\n");
	s += gettext("You may have seized the city, but holding on to it ");
	s += gettext("is never going to be easy.");
	;
	g_MessageQue.AddToQue(s, 1);
	return;
}

// ----- Update & end of turn
void cBrothelManager::UpdateBrothels()
{
	cTariff tariff;
	stringstream ss;
	sBrothel* current = m_Parent;

	m_TortureDoneFlag = false;							//WD: Reset flag each day is set in WorkTorture()

	UpdateBribeInfluence();

	while(current)
	{
		// reset the data
		current->m_Happiness = current->m_MiscCustomers = current->m_TotalCustomers = 0;
		current->m_Finance.zero();
		current->m_Events.Clear();

		// Clear the girls' events from the last turn
		sGirl* cgirl = current->m_Girls;
		while(cgirl)
		{
			cgirl->where_is_she = current->m_id;
			cgirl->m_Events.Clear();
			cgirl->m_Pay = 0;
			cgirl = cgirl->m_Next;
		}

		// handle advertising jobs to determine advertising multiplier
		//m_JobManager.do_advertising(current);
		/* Night advertising girls aren't going to affect day business. Splitting
		 * this to happen twice, once for day shift and once for night shift, and
		 * before customers are generated in that particular shift */

		// Generate customers for the brothel for the day shift and update girls
		current->m_SecurityLevel -= 30; // Moved to here so Security drops once per day instead of everytime a girl works security -PP
		if (current->m_SecurityLevel <= 0) // crazy added
			current->m_SecurityLevel = 0;
		m_JobManager.do_advertising(current, 0);
		g_Customers.GenerateCustomers(current, 0);
		current->m_TotalCustomers += g_Customers.GetNumCustomers();
		m_JobManager.do_whorejobs(current, 0);
		m_JobManager.do_custjobs(current, 0);
		UpdateGirls(current, 0);

		// update the girls and satisfy the customers for this brothel during the night
		m_JobManager.do_advertising(current, 1);
		g_Customers.GenerateCustomers(current, 1);
		current->m_TotalCustomers += g_Customers.GetNumCustomers();
		m_JobManager.do_whorejobs(current, 1);
		m_JobManager.do_custjobs(current, 1);
		UpdateGirls(current, 1);

		// get the misc customers
		current->m_TotalCustomers += current->m_MiscCustomers;

		string data = "";
		_itoa(current->m_TotalCustomers+current->m_MiscCustomers, buffer, 10);
		data += buffer;
		data += gettext(" customers visited the building.");
		
		current->m_Events.AddMessage(data, IMGTYPE_PROFILE, EVENT_BROTHEL);

		// empty rooms cost 2 gold to maintain
		current->m_Finance.building_upkeep(tariff.empty_room_cost(current));

		// update brothel stats
		if(current->m_NumGirls > 0)
			current->m_Fame = (TotalFame(current)/ current->m_NumGirls);
		if(current->m_Happiness > 0 && g_Customers.GetNumCustomers())
			current->m_Happiness = current->m_Happiness/current->m_TotalCustomers;
		if(current->m_Happiness>100)
			current->m_Happiness=100;

		// advertising costs are set independently for each brothel
		current->m_Finance.advertising_costs(tariff.advertising_costs(current->m_AdvertisingBudget));

		string data2 = "";
		data2 += gettext("Your advertising budget for this brothel is ");
		_itoa(current->m_AdvertisingBudget, buffer, 10);
		data2 += buffer;
		data2 += gettext(" gold.");
		if(tariff.advertising_costs(current->m_AdvertisingBudget) != current->m_AdvertisingBudget)
		{
			data2 += gettext(" However, due to your configuration, you instead had to pay ");
			_itoa(tariff.advertising_costs(current->m_AdvertisingBudget), buffer, 10);
			data2 += buffer;
			data2 += gettext(" gold.");
		}
		current->m_Events.AddMessage(data2, IMGTYPE_PROFILE, EVENT_BROTHEL);

		// update the global cash
		g_Gold.brothel_accounts(current->m_Finance, current->m_id);

		/*if(current->m_Filthiness > 100)   // MYR: Lets make this a little harsher
			current->m_Filthiness=100;
		else*/ if(current->m_Filthiness < 0)
			current->m_Filthiness = 0;

		// goto the next brothel
		current = current->m_Next;
	}

	// Update the bribe rate
	g_Gold.bribes(m_BribeRate);

	if(m_Runaways)
	{
		sGirl* rgirl = m_Runaways;
		while(rgirl)
		{
			if(rgirl->m_RunAway > 0)
			{
				// there is a chance the authorities will catch her if she is branded a slave
				if(rgirl->m_States&(1<<STATUS_SLAVE))
				{
					if((g_Dice%100)+1 < 5)
					{
						// girl is recaptured and returned to you
						sGirl* temp = rgirl;
						rgirl = temp->m_Next;
						RemoveGirlFromRunaways(temp);
						m_Dungeon.AddGirl(temp,DUNGEON_GIRLRUNAWAY);
						g_MessageQue.AddToQue(gettext("A runnaway slave has been recaptured by the authorities and returned to you."), 1);
						continue;
					}
				}
				rgirl->m_RunAway--;
			}
			else	// add her back to girls
			{
				sGirl* temp = rgirl;
				rgirl = temp->m_Next;
				temp->m_NightJob = temp->m_DayJob = JOB_RESTING;
				RemoveGirlFromRunaways(temp);
				g_Girls.AddGirl(temp);
				continue;
			}
			rgirl = rgirl->m_Next;
		}
	}

	if(m_Prison)
	{
		if((g_Dice%100)+1 < 10)	// 10% chance of someone being released
		{
			sGirl* girl = m_Prison;
			RemoveGirlFromPrison(girl);
			g_Girls.AddGirl(girl);
		}
	}

	// keep gravitating player suspicion to 0
	if(m_Player.suspicion() > 0)
		m_Player.suspicion(-1);
	else if(m_Player.suspicion() < 0)
		m_Player.suspicion(1);

	// is the player under suspision by the authorities
	if(m_Player.suspicion() > 20)
	{
		check_raid();
	}

	// incraese the bank gold by 02%
	if(m_Bank > 0)
	{
		int amount = (int)(m_Bank*0.002f);
		m_Bank += amount;
/*
 *		bank iterest isn't added to the gold value
 *		but it can be recorded for reporting purposes
 */
		g_Gold.bank_interest(amount);
	}

	// get money from currently extorted businesses
	if(g_Gangs.GetNumBusinessExtorted() > 0)
	{
		long gold = g_Gangs.GetNumBusinessExtorted()*INCOME_BUSINESS;
		int num = g_Dice%15;
		if(num == 1)
			gold -= INCOME_BUSINESS;
		string message = gettext("You gain ");
		_ltoa(gold, buffer, 10);
		message += buffer;
		message += gettext(" gold from the ");
		_itoa(g_Gangs.GetNumBusinessExtorted(), buffer, 10);
		message += buffer;
		message += gettext(" businesses under your control.\n");
		g_Gold.extortion(gold);
		if(num == 1)
		{
			message += gettext("A man cannot pay so he sells you his daughter.");
			m_Dungeon.AddGirl(g_Girls.CreateRandomGirl(17, false), DUNGEON_GIRLKIDNAPPED);
		}
		g_MessageQue.AddToQue(message, 3);
	}

	do_tax();

	// update the people in the dungeon
	m_Dungeon.Update();

	check_rivals();

	long totalProfit = g_Gold.total_profit();
	if(totalProfit == 0)
	{
		g_MessageQue.AddToQue(gettext("You are breaking even (made as much money as you spent)"), 3);
	}
	else
	{
		stringstream ss;

		ss << gettext("you made a overall profit of ")
		   << totalProfit
		   << gettext(" gold.");
		g_MessageQue.AddToQue(ss.str(), 2);
	}

	// MYR: I'm really curious about what goes in these if statements

	// DustyDan, 04/08/2013:  This is for future to include inside these ifs, 
	// the actions to take when not enough businesses controlled to support the 
	// number of brothels currently owned (according to formula that allowed original purchase).

	// Suggest future something like not allowing any net profit from the brothels 
	// that are unsupported by enough businesses. 
	// Forcing sale of a brothel would be too drastic; maybe allowing sale if player 
	// wants to would be an option to present.
	
	if(g_Gangs.GetNumBusinessExtorted() < 40 && GetNumBrothels() >= 2)
	{
	}

	if(g_Gangs.GetNumBusinessExtorted() < 70 && GetNumBrothels() >= 3)
	{
	}

	if(g_Gangs.GetNumBusinessExtorted() < 100 && GetNumBrothels() >= 4)
	{
	}

	if(g_Gangs.GetNumBusinessExtorted() < 140 && GetNumBrothels() >= 5)
	{
	}

	if(g_Gangs.GetNumBusinessExtorted() < 170 && GetNumBrothels() >= 6)
	{
	}

	if(g_Gangs.GetNumBusinessExtorted() < 220 && GetNumBrothels() >= 7)
	{
	}

	// update objectives or maybe create a new one
	if(GetObjective())
		UpdateObjective();
	else
	{
		if((g_Dice%100)+1 < 45)
			CreateNewObjective();
	}
}

// End of turn stuff is here
void cBrothelManager::UpdateGirls(sBrothel* brothel, int DayNight)
{
	sGirl* current = brothel->m_Girls;
	string summary, msg, girlName, MatronMsg, MatronWarningMsg;
	int totalGold;
	bool refused;
	sGirl* DeadGirl = 0;

	// WD:	Set processing flag to shift type
	m_Processing_Shift = DayNight;							

/*
 *	handle any girls training during this shift
 */
	m_JobManager.do_training(brothel, DayNight);
/*
 *	as for the rest of them...
 */
	while(current)
	{
		//srand(g_Graphics.GetTicks()+(g_Dice%5432));
		totalGold = 0;
		//summary = "";
		refused = false;
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

				RemoveGirl(brothel->m_id, DeadGirl);
				DeadGirl = 0;

				if (current)  // If there are more girls to process
					continue;
				else
					break;
			}

			current->m_Tort = false;

			current->m_YesterDayJob = current->m_DayJob;		// `J` set what she did yesterday
			current->m_YesterNightJob = current->m_NightJob;	// `J` set what she did yesternight

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

			// Check for dead girls
			// MYR: Moved this code in with the rest, above
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
			sw = JOB_RESTING;
		else
			sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;

		// do their job
		//	if((sw != JOB_ADVERTISING) && (sw != JOB_WHOREGAMBHALL) && (sw != JOB_WHOREBROTHEL) && (sw != JOB_BARWHORE))		// advertising and whoring are handled earlier.
		// Was not testing for some jobs which were already handled, changed to a switch case statement just for ease of reading, and expansion -PP
		if (sw == JOB_ADVERTISING || sw == JOB_WHOREGAMBHALL || sw == JOB_WHOREBROTHEL || 
			sw == JOB_BARWHORE || sw == JOB_BARMAID || sw == JOB_WAITRESS || 
			sw == JOB_SINGER || sw == JOB_DEALER || sw == JOB_ENTERTAINMENT || 
			sw == JOB_XXXENTERTAINMENT || sw == JOB_SLEAZYBARMAID || sw == JOB_SLEAZYWAITRESS || 
			sw == JOB_BARSTRIPPER || sw == JOB_MASSEUSE || sw == JOB_BROTHELSTRIPPER)
		{

		}
		else if (sw != JOB_RESTING && sw >= JOB_RESTING && sw <= JOB_MILK)
		{
			refused = m_JobManager.JobFunctions[sw](current, brothel, DayNight, summary);
		}
		else // Any job not in the House will be replaced with JOB_HOUSEREST
		{
			if (DayNight == SHIFT_DAY)current->m_DayJob = JOB_RESTING;
			else current->m_NightJob = JOB_RESTING;
			sw = JOB_RESTING;
			refused = m_JobManager.JobFunctions[JOB_RESTING](current, brothel, DayNight, summary);
		}

		if(refused)						// if she refused she still gets tired
			g_Girls.AddTiredness(current);

		totalGold += current->m_Pay;

		// work out the pay between the house and the girl
		g_Brothels.CalculatePay(brothel, current, sw);

		brothel->m_Fame += g_Girls.GetStat(current, STAT_FAME);

		// Check for dead girls
		// Avoids all the Warning Messages you get for a dead girl
		// MYR: This has already been done. Commenting it out.
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


		// Runaway, Depression & Drug checking
		if(runaway_check(brothel, current) == true) {
			sGirl* temp = current;
			current = current->m_Next;
			g_Brothels.RemoveGirl(brothel->m_id, temp, false);
			g_Brothels.AddGirlToRunaways(temp);
			continue;
		}

/*
 *		MATRON CODE START
 */

		// Lets try to compact multiple messages into one.
		MatronMsg = "";
		MatronWarningMsg = "";

		bool matron = false;
		if(GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, false) >= 1)
			matron = true;

		if(g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (matron)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_RESTING;
					MatronWarningMsg += gettext("Your matron takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
					//current->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_WARNING);
				}
				else
				{
					if((g_Dice%100)+1 < 70)
					{
						MatronMsg += gettext("Your matron helps ") + girlName + gettext(" to relax.\n");
						//current->m_Events.AddMessage(msg, IMGTYPE_PROFILE, DayNight);
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				MatronWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
				//current->m_Events.AddMessage("CAUTION! This girl desparatly need rest. Give her some free time", IMGTYPE_DEATH, EVENT_WARNING);
		}

		if(g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && matron && (g_Dice%100) +1 < 70)
		{
			MatronMsg = gettext("Your matron helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			//current->m_Events.AddMessage(msg, IMGTYPE_PROFILE, DayNight);
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if(g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if(matron)
			{
				if(current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_RESTING;
					MatronWarningMsg += girlName + gettext(" is taken off duty by your matron to rest due to her low health.\n");
					//current->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_WARNING);
				}
				else
				{
					MatronMsg = gettext("Your matron helps heal ") + girlName + gettext(".\n");
					//current->m_Events.AddMessage(msg, IMGTYPE_PROFILE, DayNight);
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				MatronWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
				//current->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			}
		}


		// MOVED Check for dead girls to before matron code to avoid all the unneeded health warning messages

		// Back to work
		if((current->m_NightJob == JOB_RESTING && current->m_DayJob == JOB_RESTING) && (g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20))
		{
			if(
				(matron || current->m_PrevDayJob == JOB_MATRON)  // do we have a Matron, or was she the Matron and made herself rest?
				&& current->m_PrevDayJob != 255  // 255 = nothing, in other words no previous job stored
				&& current->m_PrevNightJob != 255
				)
			{
				g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, true);
				//current->m_DayJob = current->m_PrevDayJob;  // covered by HandleSpecialJobs above
				if(current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
					current->m_NightJob = current->m_PrevNightJob;
				else
					current->m_DayJob = JOB_RESTING;
				current->m_PrevDayJob = current->m_PrevNightJob = 255;
				MatronMsg += gettext("Matron puts ") + girlName + gettext(" back to work.\n");
				//current->m_Events.AddMessage(msg, IMGTYPE_PROFILE, DayNight);
			}
			else
			{
				current->m_DayJob = JOB_RESTING;
				MatronWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
				//current->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_WARNING);
			}
		}

		// Now print out the consolodated message
		if (strcmp(MatronMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, DayNight);
			MatronMsg = "";
		}

        if (strcmp(MatronWarningMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(MatronWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
			MatronWarningMsg = "";
		}
/*
 *		MATRON CODE END
 */

		// update girl triggers
		current->m_Triggers.ProcessTriggers();

/*
 *		Summary Messages
 */
		bool sum = true;

		if(sw == JOB_RESTING)
			summary += girlName + gettext(" was resting so made no money.");

		else if(sw == JOB_TRAINING)
			sum = false;

		else if(sw == JOB_MATRON && DayNight == SHIFT_NIGHT)
			summary += girlName + gettext(" continued to help the other girls throughout the night.");

		else if(sw == JOB_ADVERTISING)
			sum = false;

		// WD:	No night shift sunnary message needed for Torturer job
		else if(sw == JOB_TORTURER && DayNight == SHIFT_NIGHT)		
			sum = false;

		// WD:	Bad girl did not work. Moved from cJobManager::Preprocessing()
		else if(refused)											
			summary += girlName + gettext(" refused to work so made no money.");

		// WD:	Only do summary messages if there is income to report
		else if(totalGold > 0)										
		{
			summary += girlName + gettext(" earned a total of ");
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += " gold";
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
				summary += "%. ";
			}
		}

		// WD:	No Income today
		else if(totalGold == 0)										
			summary += girlName + gettext(" made no money.");

#if 1																// WD: Income Loss Sanity Checking
		else if(totalGold < 0)										
		{
			summary += gettext("ERROR: She has a loss of ");
			_itoa(totalGold, buffer, 10);
			summary += buffer;
			summary += gettext(" gold");
			summary += gettext("\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org");
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_DEBUG);
			sum = false;
		}
#endif

		if(sum)
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, EVENT_SUMMARY);

		summary = "";

		// Do item check at the end of the day
		if (DayNight == SHIFT_NIGHT)
		{
			// update for girls items that are not used up
			do_daily_items(brothel, current);					// `J` added
		}

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
		// `J` corrected it
		int value = current->m_Stats[STAT_TIREDNESS] - 2;
		if (value > 100)value = 100;
		else if (value < 0)value = 0;
		current->m_Stats[STAT_TIREDNESS] = value;

		// Myr: Automate the use of a number of different items. See the function itself for more comments.
		//      Enabled or disabled based on config option.
		cConfig cfg;
		if( cfg.initial.auto_use_items() )
			UsePlayersItems(current);

		// Process next girl
		current = current->m_Next;
	}

	// WD: Finished Processing Shift set flag
	m_Processing_Shift= -1;				
}

// MYR: My automation is here
void cBrothelManager::UsePlayersItems(sGirl* cur)
{
	int has = 0, has2 = 0, Die = 0, PolishCount = 0;

	/* Automatic item use - to stop the monotonous work.
	   (I started writing this for my test game where I had 6 brothels with
	   125+ girls in each. 16 of them were full time catacombs explorers.)
	
	   Food type items are forced. Actual pieces of equipment are not.
	   The players equipment choices should always be respected.

	   There are a number of things this function specifically DOES NOT do:
	   1. Use skill raising items. 
	   2. Cure diseases like aids and syphilus.
	   3. Cure addictions like shroud and fairy dust.
	   4. Use temporary items.
	   5. Use items related to pregnancy, insemenation or children

	   I should qualify this by saying, "It doesn't directly raise stats, cure
	   diseases and addictions." They can happen indirectly as a piece of equipment
	   equipped for a stat boost or trait may also raise skills. Similarily a
	   item used to cure some condition (like an Elixir of Ultimate Regeneration
	   curing one-eye or scars)  may also cure a disease or addiction as well.

	   The way this is currently written it shouldn't be released as part
	   of the game. It makes too many choices for the player. Perhaps we can
	   make it into a useful game function somehow. Regardless, this can be
	   disabled by commenting out a single line in UpdateGirls.
    */

	// ------------ Part 1: Stats -------------

# pragma region automation_stats

	// Health

	// Healing items are wasted on constructs as the max. 4% applies to both damage and
	// healing
	has = g_Brothels.HasItem("Healing Salve (L)");
	if (g_Girls.GetStat(cur, STAT_HEALTH) <= 25 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used a large healing salve to stay healthy."));

	has = g_Brothels.HasItem("Healing Salve (M)");
	if (g_Girls.GetStat(cur, STAT_HEALTH) <= 50 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used a medium healing salve to stay healthy."));

	has = g_Brothels.HasItem("Healing Salve (S)");
	if (g_Girls.GetStat(cur, STAT_HEALTH) <= 75 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used a small healing salve to stay healthy."));

	// Tiredness/fatigue
	has = g_Brothels.HasItem("Incense of Serenity (L)");
	if (g_Girls.GetStat(cur, STAT_TIREDNESS) >= 75 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used a large incense of serenity to stay awake."));

	has = g_Brothels.HasItem("Incense of Serenity (M)");
	if (g_Girls.GetStat(cur, STAT_TIREDNESS) >= 50 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
	  AutomaticFoodItemUse(cur, has, gettext("Used a medium incense of serenity to stay awake."));

	has = g_Brothels.HasItem("Incense of Serenity (S)");
	if (g_Girls.GetStat(cur, STAT_TIREDNESS) >= 25 && !g_Girls.HasTrait(cur, "Construct") && has != -1)
	  AutomaticFoodItemUse(cur, has, gettext("Used a small incense of serenity to stay awake."));

	// Mana

	// Set threshold at 20 as that is what is required to charm a customer to sleep with a girl
	has = g_Brothels.HasItem("Mana Crystal");
	if (g_Girls.GetStat(cur, STAT_MANA) < 20 && has != -1)
	{
	  Die = g_Dice%20 + 1;
	  if (Die == 12) // Share them around
        AutomaticFoodItemUse(cur, has, gettext("Used a mana crystal to restore 25 mana."));
	}
	has = g_Brothels.HasItem("Eldritch Cookie");
	if (g_Girls.GetStat(cur, STAT_MANA) < 20 && has != -1)
	{
	  Die = g_Dice%20 + 1;
	  if (Die == 13) // Share them around
        AutomaticFoodItemUse(cur, has, gettext("Used an eldritch cookie to restore 30 mana."));
	}
	has = g_Brothels.HasItem("Mana Potion");
	if (g_Girls.GetStat(cur, STAT_MANA) < 20 && has != -1)
	{
	  Die = g_Dice%20 + 1;
	  if (Die == 15) // Share them around
        AutomaticFoodItemUse(cur, has, gettext("Used a mana potion to restore 100 mana."));
	}

	// Libido - ordered big to small

	// Succubus Milk [100 pts]
	has = g_Brothels.HasItem("Succubus Milk");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) < 5 && has != -1)  // Lower threshold
      AutomaticFoodItemUse(cur, has, gettext("Used succubus milk to restore 100 libido."));

	// Sinspice [75 pts]
	has = g_Brothels.HasItem("Sinspice");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) < 10 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used sinspice to restore 75 libido."));

	//Empress' New Clothes [50 pts] (Piece of equipment)  (This is a tossup between charisma & libido)
	has = g_Brothels.HasItem("Empress' New Clothes");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
      AutomaticItemUse(cur, has, gettext("You had her put on the empress' new clothes to get her libido up."));

	// Red Rose Extravaganza [50 pts?]
	has = g_Brothels.HasItem("Red Rose Extravaganza");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) < 10 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Gave her a red rose extravaganza to get her libido going again."));

	// Ring of the Horndog [50 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Ring of the Horndog");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && g_Girls.HasItem(cur, gettext("Minor Ring of the Horndog")) == -1
	  && g_Girls.HasItem(cur, "Ring of the Horndog") == -1 && g_Girls.HasItem(cur, "Organic Lingerie") == -1 && has != -1)
	  AutomaticItemUse(cur, has, gettext("You had her equip a ring of the horndog to better serve her customers. (Libido up.)"));

	// Gemstone Dress [42 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Gemstone Dress");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
      AutomaticItemUse(cur, has, gettext("She put on a gemstone dress for that million-dollar feeling. (Libido up.)"));

	// Silken Dress [34 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Silken Dress");
	if(g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
      AutomaticItemUse(cur, has, gettext("She put on a silken dress to better slide with her customers. (Libido up.)"));

	// Minor Ring of the Horndog [30 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Minor Ring of the Horndog");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && g_Girls.HasItem(cur, "Minor Ring of the Horndog") == -1
	  && g_Girls.HasItem(cur, "Ring of the Horndog") == -1  && g_Girls.HasItem(cur, "Organic Lingerie") == -1 && has != -1)
	  AutomaticItemUse(cur, has, gettext("She was lookin a little listless so you had her equip a minor ring of the horndog. (Libido up.)"));

	// Velvet Dress [34 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Velvet Dress");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
	  AutomaticItemUse(cur, has, gettext("She put on a velvet dress to feel even more sexy. (Libido up.)"));

	// Designer Lingerie [20 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Designer Lingerie");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
	  AutomaticItemUse(cur, has, gettext("She put on designer lingerie to feel more at home. (Libido up.)"));

	// Charisma 
	
	//Ring of Charisma [50 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Ring of Charisma");
	if (g_Girls.GetStat(cur, STAT_CHARISMA) <= 50 && g_Girls.HasItem(cur, "Ring of Charisma") == -1 && has != -1)
      AutomaticItemUse(cur, has, gettext("You had her put on a ring of charisma to overcome her speaking difficulties."));

	// Minor Ring of Charisma [30 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Minor Ring of Charisma");
	if (g_Girls.GetStat(cur, STAT_CHARISMA) <= 70 && g_Girls.HasItem(cur, "Minor Ring of Charisma") == -1 && has != -1)
	  AutomaticItemUse(cur, has, gettext("Put on a minor ring of charisma."));

	// Beauty
	
	// Ring of Beauty [50 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Ring of Beauty");
	if (g_Girls.GetStat(cur, STAT_BEAUTY) <= 50 && g_Girls.HasItem(cur, "Ring of Beauty") == -1 && has != -1)
	  AutomaticItemUse(cur, has, gettext("You had her put on a ring of beauty to overcome her ugly-stick disadvantage."));

	// Minor Ring of Beauty [30 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Minor Ring of Beauty");
	if (g_Girls.GetStat(cur, STAT_BEAUTY) <= 70 && g_Girls.HasItem(cur, "Minor Ring of Beauty") == -1&& has != -1 )
	  AutomaticItemUse(cur, has, gettext("She put on a minor ring of beauty to cover some flaws."));

	// Rainbow Ring [15 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Rainbow Ring");
	if(g_Girls.GetStat(cur, STAT_BEAUTY) <= 85 && g_Girls.HasItem(cur, "Rainbow Ring") == -1 && has != -1)
      AutomaticItemUse(cur, has, gettext("She put on a rainbow ring, to match her rainbow personality."));

	// Happiness - ordered from big values to small

	// Heaven-and-Earth Cake [100 pts]
	has = g_Brothels.HasItem("Heaven-and-Earth Cake");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 10 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had a heaven-and-earth cake to stave off serious depression."));

	// Eldritch cookie [70 pts]
	has = g_Brothels.HasItem("Eldritch Cookie");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 30 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had an eldritch cookie to improve her mood."));

	// Expensive Chocolates [50 pts]
	has = g_Brothels.HasItem("Expensive Chocolates");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 50 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had some expensive chocolates to improve her mood."));

	// Apple Tart [30 pts]
	has = g_Brothels.HasItem("Apple Tart");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 70 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had an apple tart to improve her mood."));

	// Honeypuff Scones [30 pts]
	has = g_Brothels.HasItem("Honeypuff Scones");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 70 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had a honeypuff scone for lunch."));

	// Fancy breath mints [10 pts]
	has = g_Brothels.HasItem("Fancy Breath Mints");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 90 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had fancy breath mints. (Why not? They were lying around.)"));

	// Exotic Bouquet [10 pts]
	has = g_Brothels.HasItem("Exotic Bouquet");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 90 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("You gave her an exotic bouquet for work well done."));

	// Wild Flowers [5 pts]
	has = g_Brothels.HasItem("Wild Flowers");
	if (g_Girls.GetStat(cur, STAT_HAPPINESS) <= 95 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("You gave her some wild flowers."));

	// Age
	
	//Do this before boobs b/c lolly wand makes them small
	// My arbitrary rule is, once they hit 30, make 'em young again.

	// To prevent using an elixir, then a wand, set an arbitrary upper age limit of 35 for elixirs
	has = g_Brothels.HasItem("Elixir of Youth");
	if ((g_Girls.GetStat(cur, STAT_AGE) >= 30) && (g_Girls.GetStat(cur, STAT_AGE) <= 35) && (has != -1))
      AutomaticFoodItemUse(cur, has, gettext("Used a elixir of youth to remove ten years of age."));

	has = g_Brothels.HasItem("Lolita Wand"); 
	if (g_Girls.GetStat(cur, STAT_AGE) >= 30 && g_Girls.GetStat(cur, STAT_AGE) <= 80 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used a lolita wand to become seventeen again."));

	// XP: Nuts & tomes & mangos of knowledge, etc...

	// 25 xp
	has = g_Brothels.HasItem("Nut of Knowledge");
	if (g_Girls.GetStat(cur, STAT_EXP) <= 230 && g_Girls.GetStat(cur, STAT_LEVEL) < 255 && has != -1)
	{
		Die = g_Dice % 50;   // Spread them around
		if (Die == 5)
          AutomaticFoodItemUse(cur, has, gettext("Used a small nut of knowledge."));
	}

	// 100 xp
	has = g_Brothels.HasItem("Mango of Knowledge");
	if (g_Girls.GetStat(cur, STAT_EXP) <= 155 && g_Girls.GetStat(cur, STAT_LEVEL) < 255 && has != -1)
	{
		Die = g_Dice % 30;
		if (Die == 9)
          AutomaticFoodItemUse(cur, has, gettext("She ate a mango of knowledge."));
	}

	// 200 xp
	has = g_Brothels.HasItem("Watermelon of Knowledge");
	if (g_Girls.GetStat(cur, STAT_EXP) <= 55 && g_Girls.GetStat(cur, STAT_LEVEL) < 255 && has != -1)
	{
		Die = g_Dice % 30;
		if (Die == 9)
          AutomaticFoodItemUse(cur, has, gettext("She had a watermelon of knowledge for lunch."));
	}

	// Constitution (Items in reverse order. That is, the items offering the largest increases are first)

	// Ring of the Schwarzenegger [50 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Ring of the Schwarzenegger");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 50 && has != -1)
      AutomaticItemUse(cur, has, gettext("She put on a Ring of the Schwarzenegger for the constitution boost."));

	// Bracer of Toughness [40 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Bracer of Toughness");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 60 && has != -1)
      AutomaticItemUse(cur, has, gettext("She put on a bracer of toughness for the constitution boost."));

	// Minor Ring of the Schwarzenegger [30 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Minor Ring of the Schwarzenegger");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 70 && has != -1)
	  AutomaticItemUse(cur, has, gettext("She put on a Minor Ring of the Schwarzenegger for the constitution boost."));

	// Necklace of Pain Reversal [25 pts net: +40 for masochist -15 on necklace] (Piece of equipment)
	has = g_Brothels.HasItem("Necklace of Pain Reversal");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 75 && !g_Girls.HasTrait(cur, "Masochist") && has != -1)
	  AutomaticItemUse(cur, has, gettext("You had her put on a necklace of pain reversal for the constitution boost."));

	// Tiger Leotard [20 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Tiger Leotard");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 80 && has != -1)
	  AutomaticItemUse(cur, has, gettext("She put on a tiger leotard to feel it's strength and power."));

	// Manual of health [10 pts] (Piece of equipment, but slotless)
	// Lets be reasonable and only allow only one of each slotless item to be given to a girl.
	// (Having 8 stripper poles in a girl's inventory looks silly IMO.)
	has = g_Brothels.HasItem("Manual of Health");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 90 && g_Girls.HasItem(cur, "Manual of Health") == -1 && has != -1)
	  AutomaticSlotlessItemUse(cur, has, gettext("You gave her a manual of health to read.")); 

	// Free Weights [10 pts] (Piece of equipment, but slotless)
	has = g_Brothels.HasItem("Free Weights");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 90 && g_Girls.HasItem(cur, "Free Weights") == -1 && has != -1)
	  AutomaticSlotlessItemUse(cur, has, gettext("You gave her free weights to work with."));

	// Stripper Pole [5 pts] (Piece of equipment, but slotless)
	has = g_Brothels.HasItem("Stripper Pole");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 95 && g_Girls.HasItem(cur, "Stripper Pole") == -1 && has != -1)
	  AutomaticSlotlessItemUse(cur, has, gettext("You gave her a stripper pole to practice with."));

	// Obedience

	// Necklace of Control (piece of equipment)
	has = g_Brothels.HasItem("Necklace of Control");
	if (g_Girls.GetStat(cur, STAT_OBEDIENCE) <= 10 && has != -1)
	  AutomaticItemUse(cur, has, gettext("Her obedience is a problem so you had her put on a necklace of control."));

	has = g_Brothels.HasItem("Disguised Slave Band");
	if (g_Girls.GetStat(cur, STAT_OBEDIENCE) <= 50 && has != -1)
      AutomaticItemUse(cur, has, gettext("You had her put on a disguised slave band, claiming it was something else."));

	has = g_Brothels.HasItem("Slave Band");
	if (g_Girls.GetStat(cur, STAT_OBEDIENCE) <= 50 && has != -1)
      AutomaticItemUse(cur, has, gettext("You dealth with her obedience problems by forcing her to wear a slave band."));

	has = g_Brothels.HasItem("Willbreaker Spice");
	if (g_Girls.GetStat(cur, STAT_OBEDIENCE) <= 90 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("You slipped some willbreaker spice in to her food."));

#pragma endregion automation_stats

	// ---------- Part 2: Traits ----------------

# pragma region automation_traits

	// Perfection. This is an uber-valuable I put in. Ideally it should be Catacombs01, not Catacombs15.
	// It changes so many traits that it's hard to decide on a rule. In the end I kept it simple.
	// (Players will justifiably hate me if I made this decision for them.)
	// Do this first as it covers/replaces 90% of what follows
	has = g_Brothels.HasItem("Perfection");
	if (cur->m_NumTraits <= 8 && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used perfection to become a near perfect being."));

	// Tough

	// Aoshima beef
	has = g_Brothels.HasItem("Aoshima BEEF!!");
	if (!g_Girls.HasTrait(cur, "Tough") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Bulked up on Aoshima Beef to get the tough trait."));

	// Oiran Dress (Piece of equipment)
	has = g_Brothels.HasItem("Oiran Dress");
	if (!g_Girls.HasTrait(cur, "Tough") && has != -1)
	  AutomaticItemUse(cur, has, gettext("Put on an Oiran Dress."));

	// Nymphomaniac

	// Do this before quick learner b/c taking the shroud cola gives the girl the slow learner trait
	has = g_Brothels.HasItem("Shroud Cola");
	has2 = g_Brothels.HasItem("Cure for Shroud Addiction");
	if (!g_Girls.HasTrait(cur, "Nymphomaniac") && (has != -1 && has2 != -1))
	{
      // If one succeeds, the other should too
	  // Note the order is important here: Shroud cola has to be first
      AutomaticFoodItemUse(cur, has, 
		  gettext("You had her down a shround cola for the nymphomaniac side-effect.\n\n Unfortunately she also gains the slow-learner trait."));
	  AutomaticFoodItemUse(cur, has2, gettext("You had her take the shroud addiction cure."));
	}

	// Quick learner

	// Scroll of transcendance
	has = g_Brothels.HasItem("Scrolls of Transcendance");
	if (!g_Girls.HasTrait(cur, "Quick Learner") && !g_Girls.HasTrait(cur, "Optimist") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Read a Scroll of Transcendence to gain the quick learner and optimist traits."));

	// Book of enlightenment
	has = g_Brothels.HasItem("Book of Enlightenment");
	if (!g_Girls.HasTrait(cur, "Quick Learner") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Read a book of enlightenment for the quick learner trait."));

	// Ring of Enlightenment
	has = g_Brothels.HasItem("Ring of Enlightenment");
	if (!g_Girls.HasTrait(cur, "Quick Learner") && has != -1)
      AutomaticItemUse(cur, has, gettext("Put on a ring of enlightenment for the quick learner trait."));

	// Amulet of the Cunning Linguist
	has = g_Brothels.HasItem("Amulet of the Cunning Linguist");
	if (!g_Girls.HasTrait(cur, "Quick Learner") && has != -1)
      AutomaticItemUse(cur, has, gettext("Put on an amulet of the cunning linguist for the quick learner trait."));

	// Optimist: Good fortune, leprechaun biscuit, chatty flowers, etc...

	// Good Fortune
	has = g_Brothels.HasItem("Good Fortune");
	if (!g_Girls.HasTrait(cur, "Optimist") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Read a good fortune and feels more optimistic for it."));

	// Leprechaun Biscuit
	has = g_Brothels.HasItem("Leprechaun Biscuit");
	if (!g_Girls.HasTrait(cur, "Optimist") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Had a leprechaun biscuit and feels more optimistic for it."));

	// Chatty Flowers
	has = g_Brothels.HasItem("Chatty Flowers");
	if (!g_Girls.HasTrait(cur, "Optimist") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Talked with the chatty flowers and feels more optimistic for it."));

	// Glass shoes (piece of equipment)
	has = g_Brothels.HasItem("Glass Shoes");
	if (!g_Girls.HasTrait(cur, "Optimist") && g_Girls.HasItem(cur, "Sandals of Mercury") == -1 && has != -1)
      AutomaticItemUse(cur, has, gettext("Slipped on glass shoes for the optimist trait."));

	// Elegant (Obsidian Choker, piece of equipment)

	has = g_Brothels.HasItem("Obsidian Choker");
	if (!g_Girls.HasTrait(cur, "Elegant") && has != -1)
      AutomaticItemUse(cur, has, gettext("Put on an obsidian choker for the elegant trait."));

	// Fleet of foot (Sandals of Mercury, piece of equipment)

	has = g_Brothels.HasItem("Sandals of Mercury");
	if (!g_Girls.HasTrait(cur, "Fleet of Foot") && !g_Girls.HasTrait(cur, "Fleet Of Foot") && g_Girls.HasItem(cur, "Glass Shoes") == -1 && has != -1)
      AutomaticItemUse(cur, has, gettext("Put on Sandals of Mercury for the fleet of foot trait."));

	// Fast Orgasms & Nymphomaniac (Organic Lingerie, piece of equipment)

	has = g_Brothels.HasItem("Organic Lingerie");
	if (!g_Girls.HasTrait(cur, "Fast orgasms") && !g_Girls.HasTrait(cur, "Fast Orgasms") && !g_Girls.HasTrait(cur, "Nymphomaniac") && has != -1)
       AutomaticItemUse(cur, has, gettext("You had her wear organic lingerie."));

	// Fast Orgasms (Ring of Pleasure, piece of equipment)

	has = g_Brothels.HasItem("Ring of Pleasure");
	if (!g_Girls.HasTrait(cur, "Fast orgasms") && !g_Girls.HasTrait(cur, "Fast Orgasms") && has != -1)
       AutomaticItemUse(cur, has, gettext("You had her put on a ring of pleasure for the fast orgasms trait."));

	// Lets try and cure mind fucked & retarted
	// The amulet of the sex elemental gives you the mind fucked trait. It can be "cured" until the amulet is taken off and put on again.
	// Regardless, we'll not try to cure the amulet case.
	has = g_Brothels.HasItem("Refined Mandragora Extract");
	if (((g_Girls.HasTrait(cur, "Mind Fucked") && g_Girls.HasItem(cur, "Amulet of the Sex Elemental") == -1)
		|| g_Girls.HasTrait(cur, "Retarded")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("You had her use refined mandragora extract to remove mental damage."));

	// Malformed

	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
	if (g_Girls.HasTrait(cur, "Malformed") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to cure her malformities."));

	// Tsundere & yandere

	has = g_Brothels.HasItem("Attitude Reajustor");
	if ((g_Girls.HasTrait(cur, "Yandere") || g_Girls.HasTrait(cur, "Tsundere")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("You had her take an attitude reajustor pill."));

	// Eyes

	has = g_Brothels.HasItem("Eye Replacement Candy");
	if ((g_Girls.HasTrait(cur, "One Eye") || g_Girls.HasTrait(cur, "Eye Patch")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an eye replacement candy to restore her eye."));

	// Last ditch eye check.  Use the big guns if you don't have anything else.
	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
	if ((g_Girls.HasTrait(cur, "One Eye") || g_Girls.HasTrait(cur, "Eye Patch")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to restore her eye."));

	// Scars - start with the least powerful cures and work up
	has = g_Brothels.HasItem("Oil of Lesser Scar Removing");
	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an oil of lesser scar removal to remove work-related damage."));

	has = g_Brothels.HasItem("Oil of Greater Scar Removing");
	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars") || g_Girls.HasTrait(cur, "Horrific Scars")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an oil of greater scar removal to remove her scars."));

	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars") || g_Girls.HasTrait(cur, "Horrific Scars")) && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to remove her scars."));

	// Big boobs

	has = g_Brothels.HasItem("Oil of Extreme Breast Growth");
	if (!g_Girls.HasTrait(cur, "Big Boobs") && !g_Girls.HasTrait(cur, "Abnormally Large Boobs") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("She uses an oil of extreme breast growth to gain the abnormally large boobs trait."));

	has = g_Brothels.HasItem("Oil of Greater Breast Growth");
	if (!g_Girls.HasTrait(cur, "Big Boobs") && !g_Girls.HasTrait(cur, "Abnormally Large Boobs") && has != -1)
      AutomaticFoodItemUse(cur, has, gettext("She uses an oil of greater breast growth to gain the big boobs trait."));

	// Nipple Rings of Pillowy Softness (piece of [ring slot] equipment)
	has = g_Brothels.HasItem("Nipple Rings of Pillowy Softness");
	if (!g_Girls.HasTrait(cur, "Big Boobs") && !g_Girls.HasTrait(cur, "Abnormally Large Boobs") && has != -1)
      AutomaticSlotlessItemUse(cur, has, gettext("You had her put on a nipple rings of pillowy softness."));

	// Nipple Rings of Breast Expansion, (piece of [ring slot] equipment)
	has = g_Brothels.HasItem("Nipple Rings of Breast Expansion");
	if (!g_Girls.HasTrait(cur, "Big Boobs") && !g_Girls.HasTrait(cur, "Abnormally Large Boobs") && has != -1)
      AutomaticItemUse(cur, has, gettext("You had her put on nipple rings of breast expansion for the big boobs trait."));

	// Polish
	has = g_Brothels.HasItem("Polish");
	if (has != -1)
	{
		// If the girl doesn't have 4 of these 5 traits she will use polish
		if(!g_Girls.HasTrait(cur, "Good Kisser"))
			PolishCount++;
		if(!g_Girls.HasTrait(cur, "Great Figure"))
			PolishCount++;
		if(!g_Girls.HasTrait(cur, "Great Arse"))
			PolishCount++;
		if(!g_Girls.HasTrait(cur, "Long Legs"))
			PolishCount++;
		if(!g_Girls.HasTrait(cur, "Puffy Nipples"))
			PolishCount++;

		if (PolishCount >= 4)
          AutomaticFoodItemUse(cur, has, gettext("Used polish to make herself more attractive to clients."));
	}

	// Masochist

	// Put this at the bottom as there are better neck slot items that could be equipped above
	// Unlike the case of raising the constitution score in part one, we're only concerned with the trait here
	has = g_Brothels.HasItem("Necklace of Pain Reversal");
	if (!g_Girls.HasTrait(cur, "Masochist") && has != -1)
	  AutomaticItemUse(cur, has, gettext("You have this thing for masochism, so you had her put on a necklace of pain reversal."));

	// Iron Will

	// Disguised Slave band (piece of equipment)
	// (Statuses like 'controlled' on the Disguised Slave Band (amongst others) don't appear to do anything.)
	has = g_Brothels.HasItem("Disguised Slave Band");
	if (g_Girls.HasTrait(cur, "Iron Will") && has != -1)
	  AutomaticItemUse(cur, has, gettext("Her iron will is a problem so you had her put on a disguised slave band, claiming it was something else."));

	has = g_Brothels.HasItem("Slave Band");
	if (g_Girls.HasTrait(cur, "Iron Will") && has != -1)
      AutomaticItemUse(cur, has, gettext("You dealth with her iron will by forcing her to wear a slave band."));

	// Necklace of Control (piece of equipment)
	has = g_Brothels.HasItem("Necklace of Control");
	if (g_Girls.HasTrait(cur, "Iron Will") && has != -1)
	  AutomaticItemUse(cur, has, gettext("Her iron will is a problem so you had her put on a necklace of control."));

# pragma endregion automation_traits

}

bool cBrothelManager::AutomaticItemUse(sGirl * girl, int InvNum, string message)
{
	int EquipSlot = -1;

	EquipSlot = g_Girls.AddInv(girl, m_Inventory[InvNum]);
	if (EquipSlot != -1)
	{
		if (g_InvManager.equip_singleton_ok(girl, EquipSlot, false))  // Don't force equipment
		{
			RemoveItemFromInventoryByNumber(InvNum);  // Remove from general inventory
			g_InvManager.Equip(girl, EquipSlot, false); 
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
			return true;

		}	
		else
		{
			g_Girls.RemoveInvByNumber(girl, EquipSlot);	// Remove it from the girl's inventory if they can't equip		
			return false;
		}
	}
	else
		return false;
}

bool cBrothelManager::AutomaticSlotlessItemUse(sGirl * girl, int InvNum, string message)
{
	// Slotless items include manuals, stripper poles, free weights, etc...
	int EquipSlot = -1;

	EquipSlot = g_Girls.AddInv(girl, m_Inventory[InvNum]);
	if (EquipSlot != -1)
	{
		RemoveItemFromInventoryByNumber(InvNum);  // Remove from general inventory
		g_InvManager.Equip(girl, EquipSlot, false); 
		girl->m_Events.AddMessage(message, IMGTYPE_DEATH, EVENT_WARNING);
		return true;
	}
	else
		return false;
}

bool cBrothelManager::AutomaticFoodItemUse(sGirl * girl, int InvNum, string message)
{
	int EquipSlot = -1;

	EquipSlot = g_Girls.AddInv(girl, m_Inventory[InvNum]);
	if (EquipSlot != -1)
	{
	  RemoveItemFromInventoryByNumber(InvNum);
	  g_InvManager.Equip(girl, EquipSlot, true);
	  girl->m_Events.AddMessage(message, IMGTYPE_DEATH, EVENT_WARNING);
      return true;
	}	
	else
	  return false;
}

void cBrothelManager::UpdateBribeInfluence()
{
	m_Influence = GetBribeRate();
	cRival* rival = GetRivals();
	if(rival)
	{
		long total = m_BribeRate;
		total += TOWN_OFFICIALSWAGES;	// this is the amount the government controls

		while(rival)	// get the total for all bribes
		{
			total += rival->m_BribeRate;
			rival = rival->m_Next;
		}

		rival = GetRivals();
		while(rival)	// get the total for all bribes
		{
			if(rival->m_BribeRate > 0 && total != 0)
				rival->m_Influence = (int)(((float)rival->m_BribeRate/(float)total)*100.0f);
			else
				rival->m_Influence = 0;
			rival = rival->m_Next;
		}

		if(m_BribeRate != 0 && total != 0)
			m_Influence = (int)(((float)m_BribeRate/(float)total)*100.0f);
		else
			m_Influence = 0;
	}
	else
	{
		if(m_BribeRate <= 0)
			m_Influence = 0;
		else
			m_Influence = (int)(((float)m_BribeRate/(float)((float)TOWN_OFFICIALSWAGES+(float)m_BribeRate))*100.0f);
	}
}

void cBrothelManager::do_tax()
{
	cConfig cfg;
	double taxRate = cfg.tax.rate();	// normal tax rate is 6%
	if(m_Influence > 0)	// can you influence it lower
	{
		int lowerBy = m_Influence/20;
		float amount = (float)(lowerBy/100);
		taxRate -= amount;
		if(taxRate < cfg.tax.minimum())
			taxRate = cfg.tax.minimum();
	}
	// check for money laundering and apply tax
	int earnings = g_Gold.total_earned();

	if(earnings <= 0) {
		g_MessageQue.AddToQue(
			gettext("You didn't earn any money so didn't get taxed."), 0
		);
		return;
	}
/*
 *	money laundering: nice idea - I had no idea it was
 *	in the game.
 *
 *	Probably we should make the player work for this.
 *	invest a little in businesses to launder through.
 */
	int laundry = g_Dice.random(int(earnings * cfg.tax.laundry()));
	long tax = long((earnings - laundry) * taxRate);
/*
 *	this should not logically happen unless we 
 *	do something very clever with the money laundering
 */
	if(tax <= 0) {
		g_MessageQue.AddToQue(
			gettext("Thanks to a clever accountant, none of your income turns out to be taxable"), 0
		);
		return;
	}
	g_Gold.tax(tax);
	stringstream ss;
/*
 *	Let's report the laundering, at least.
 *	Otherwise, it just makes the tax rate wobble a bit
 */
	ss << gettext("You were taxed ") << tax << gettext(" gold. ")
	   << gettext("You managed to launder ") << laundry
	   << gettext(" through various local businesses.")
	;
	g_MessageQue.AddToQue(ss.str(), 0);
}

bool is_she_cleaning(sGirl *girl)
{
	if (girl->m_DayJob == JOB_CLEANING || girl->m_NightJob == JOB_CLEANING ||
		girl->m_DayJob == JOB_CLEANARENA || girl->m_NightJob == JOB_CLEANARENA ||
		girl->m_DayJob == JOB_STAGEHAND || girl->m_NightJob == JOB_STAGEHAND ||
		girl->m_DayJob == JOB_JANITOR || girl->m_NightJob == JOB_JANITOR ||
		girl->m_DayJob == JOB_CLEANCENTRE || girl->m_NightJob == JOB_CLEANCENTRE ||
		girl->m_DayJob == JOB_CLEANHOUSE || girl->m_NightJob == JOB_CLEANHOUSE)
	{
		return true;
	}
	return false;
}
bool is_she_resting(sGirl *girl)
{
	if (
		girl->m_DayJob == JOB_FILMFREETIME || girl->m_NightJob == JOB_FILMFREETIME ||
		girl->m_DayJob == JOB_ARENAREST || girl->m_NightJob == JOB_ARENAREST ||
		girl->m_DayJob == JOB_CENTREREST || girl->m_NightJob == JOB_CENTREREST ||
		girl->m_DayJob == JOB_CLINICREST || girl->m_NightJob == JOB_CLINICREST ||
		girl->m_DayJob == JOB_HOUSEREST || girl->m_NightJob == JOB_HOUSEREST ||
		girl->m_DayJob == JOB_RESTING || girl->m_NightJob == JOB_RESTING)
	{
		return true;
	}
	return false;
}


void cBrothelManager::do_daily_items(sBrothel *brothel, sGirl *girl) // `J` added
{
	string message = "";


	if (g_Girls.HasItemJ(girl, gettext("Android, Assistance")) != -1)
	{
		message += "Her Assistance Android swept up and took out the trash for her.\n\n";
		brothel->m_Filthiness -= 5;
	}
	if (g_Girls.HasItemJ(girl, gettext("Television Set")) != -1)
	{
		if (is_she_resting(girl))
		{
			message += girl->m_Realname + " spent most of her day lounging in front of her Television Set.\n\n";
			girl->tiredness(-5);
			if (g_Dice % 100<5)		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
		}
		else
		{
			message += "At the end of her long day, " + girl->m_Realname + " flopped down in front of her Television Set and relaxed.\n\n";
			girl->tiredness(-3);
		}
	}
	if (g_Girls.HasItemJ(girl, gettext("Appreciation Trophy")) != -1 && is_she_cleaning(girl) && g_Dice%100 < 5 && girl->pclove() > girl->pchate()-10)
	{
		message += "While cleaning, " + girl->m_Realname + " came across her Appreciation Trophy and smiled.\n\n";
		girl->pclove(1);
	}
	if (g_Girls.HasItemJ(girl, gettext("Art Easel")) != -1 && g_Dice%100 < 2)
	{
		int sale = g_Dice%30+1;
		message += girl->m_Realname + " managed to sell one of her paintings for ";
		_itoa(sale, buffer, 10);
		message += buffer;
		message += " gold.\n\n";
		girl->m_Money += sale;
		girl->happiness(sale / 5);
		girl->fame(1);
	}

	



	if (message != "")		// only pass the summary if she has any of the items listed
	{
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_SUMMARY);
	}
}

void cBrothelManager::do_food_and_digs(sBrothel *brothel, sGirl *girl)
{
/*
 *	add the girls accomodation and food costs to the upkeep
 */
	if(girl->is_slave()) {
/*
 *		For a slavegirl, 5 gold per accomodation level
 */
		brothel->m_Finance.girl_support(5 * (girl->m_AccLevel+1));	
/* 
 *		accomodation zero is all they expect
 */
		if(girl->m_AccLevel == 0) {
			return;
		}
/* 
 *		accomodation over 0 means happier,
 *		and maybe liking the PC more
 *
 *		mod: docclox - made happiness gains greater 
 *		for nicer digs
 */
		girl->happiness(4 + girl->m_AccLevel / 2);
		// end mod
/*
 *		mod - doc - make love and hate change faster
 *		with better digs - but not at the same rate as
 *		the happiness bonus, so there's a point to the
 *		intermediate levels
 */
		int excess = girl->happiness() - 100;
		if(excess >= 0) {
			int mod = 1 + excess / 3;
			girl->pchate(-mod);
			girl->pclove(mod);
		}
		// end mod
		return;
	}
/*
 *	For a freegirl, 20 gold per accomodation level
 *	mod - doc - simplified the calculation a bit
 */
	brothel->m_Finance.girl_support(20 * (girl->m_AccLevel+1));	
/*
 *	let's do the simple case
 *	if her accomodation is greater then her level 
 *	she'll get happier. That's a mod: it was >=
 *	before, but this way 0 level girls want level 1 accom
 *	and it goes up level for level thereafter
 */
	if(girl->m_AccLevel > girl->level()) {
		girl->happiness(2 + girl->m_AccLevel / 2);
		int excess = girl->happiness() - 100;
		if(excess >= 0) {
			int mod = 1 + excess / 3;
			girl->pchate(-mod);
			girl->pclove(mod);
		}
		return;
	}
/*
 *	If we get here, the accomodation level is less
 *	than a girl of her accomplisments would expect
 *	However, level 6 and greater and her sense of
 *	professionalism means she doesn't let it affect her
 *	state of mind
 */
	if(girl->level() >= 6) {
		return;
	}
/*
 *	Failing that, she will be less happy
 */
	int mod, diff =  girl->level() - girl->m_AccLevel;
	mod = diff / 2;	// half the difference, round down
	mod ++;		// and add one
	girl->happiness(-mod);
/*
 *	and if she gets completely miserable,
 *	she'll grow to hate the PC
 */
	if(girl->happiness() <= 0) {
		girl->pchate(1 + diff / 3);
	}
}

// ----- Inventory
void cBrothelManager::SortInventory()
{
//	qu_sort(0,299,m_Inventory);
}

int cBrothelManager::HasItem(string name, int countFrom)
{
	// We look for an item in the range of countFrom to MAXNUM_INVENTORY.
	// Either the index of the item or -1 is returned.

	if(countFrom == -1)
		countFrom = 0;

	if(countFrom>=MAXNUM_INVENTORY)
		return -1;

	for(int i=countFrom; i<MAXNUM_INVENTORY; i++)
	{
		if(m_Inventory[i])
		{
			if (m_Inventory[i]->m_Name == name)
				return i;
		}
	}

	return -1;
}

bool cBrothelManager::AddItemToInventory(sInventoryItem* item)
{
	bool added = false;
	int curI = g_Brothels.HasItem(item->m_Name, -1);

	bool loop = true;
	while(loop)
	{
		if(curI != -1)
		{
			if(g_Brothels.m_NumItem[curI] >= 999)
				curI = g_Brothels.HasItem(item->m_Name, curI+1);
			else
				loop = false;
		}
		else
			loop = false;
	}

	if(g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
	{
		if(curI != -1)
		{
			added = true;
			g_Brothels.m_NumItem[curI]++;

		}
		else
		{
			for(int j=0; j<MAXNUM_INVENTORY; j++)
			{
				if(g_Brothels.m_Inventory[j] == 0)
				{
					added = true;
					g_Brothels.m_Inventory[j] = item;
					g_Brothels.m_EquipedItems[j] = 0;
					g_Brothels.m_NumInventory++;
					g_Brothels.m_NumItem[j]++;
					break;
				}
			}
		}
	}

	SortInventory();
	return added;
}

bool cBrothelManager::RemoveItemFromInventoryByNumber(int Pos)
{
  bool removed = false;

  if (g_Brothels.m_Inventory[Pos] != 0)
  {
    if (g_Brothels.m_NumItem[Pos] > 0)
	{
	  removed = true;
	  g_Brothels.m_NumItem[Pos]--;

	  // We may reduce the stack size to zero
	  if (g_Brothels.m_NumItem[Pos] == 0)
	  {
	    g_Brothels.m_Inventory[Pos] = 0;
	    g_Brothels.m_EquipedItems[Pos] = 0;
        g_Brothels.m_NumInventory--;
	  }
	} // if num > 0
  }      // Inventory type not null

  SortInventory();
  return removed;
}

// ----- Bank & money
void cBrothelManager::WithdrawFromBank(long amount)
{
	if(m_Bank-amount >= 0)
		m_Bank -= amount;
}

void cBrothelManager::DepositInBank(long amount)
{
	if(amount > 0)
		m_Bank += amount;
}

double cBrothelManager::calc_pilfering(sGirl *girl)
{
	double factor = 0.0;
/*
 *	on top of all other factors, 
 *	an addict will steal to feed her habit
 */
	if(girl->is_addict() && girl->m_Money < 50) // `J` increased theft chances for addicts
	{
		factor += 0.2;
	}
/*
 *	let's work out what if she is going steal anything
 *
 *	love or obedience will keep her honest
 */
	if(girl->pclove() >= 50 || girl->obedience() >= 50) 
	{
		return factor;
	}
/*
 *	if her fear is greater than her hate, she won't dare steal
 */
	if(girl->pcfear() > girl->pchate()) {
		return factor;
	}
/*
 *	and apparently, slaves don't steal
 */
	if(girl->is_slave()) {
		return factor;
	}
/*
 *	given all the above, if she hates him enough, she'll steal
 */
	if(girl->pchate() > 40) {
		return factor + 0.15;
	}
/*
 *	if she's not motivated by hatred, she needs to be pretty confident
 *
 */
	if(girl->confidence() > 70 && girl->spirit() > 50) {
		return factor + 0.15;
	}
/*
 *  otherwise, she stays honest (aside from zero-money addict factored-in earlier)
 */
	return factor;

}

void cBrothelManager::CalculatePay(sBrothel* brothel, sGirl* girl, u_int Job)
{
	if(
		   girl->m_Stats[STAT_HOUSE] == 0 
//		|| girl->m_DayJob == JOB_MATRON  // Matrons get full pay
		|| m_JobManager.is_job_Paid_Player(Job)					// WD:	added test for paid jobs	
		)
	{
		girl->m_Money += girl->m_Pay;
		girl->m_Pay=0;
		return;
	}

	if(girl->m_Pay <= 0) 
	{
		if(girl->m_Pay < 0)
			girl->m_Pay = 0;
		return;
	}

	double steal_factor = calc_pilfering(girl);
	float house_factor = float(girl->m_Stats[STAT_HOUSE]) / 100.0f;
/*
 *	work out how much gold (if any) she steals
 */
	int stolen = int(steal_factor * girl->m_Pay);
	girl->m_Pay -= stolen;
/*
 *	the house takes its cut of whatever's left
 */
	int house = int(house_factor * girl->m_Pay);
/*
 *	this shouldn't happen. That said...			  [moved here 04/08/2013]
 */
	if(house > girl->m_Pay) house = girl->m_Pay;

/*
 *	add the various portions of the take to the 
 *	girl and the brothel finances
 */
	brothel->m_Finance.brothel_work(house);
	girl->m_Money += girl->m_Pay-house;
	girl->m_Money += stolen;	// she secrets away some money

/*
 *	Not sure how this could happen - suspect it's just
 *	a sanity check
 */
	if(girl->m_Money < 0) girl->m_Money = 0;
/*
 *	If she didn't steal anything, we're done
 */
 	if(!stolen)
		return;
/*
 *	if no-one is watching for theft, we're done
 */
	sGang* gang = g_Gangs.GetGangOnMission(MISS_SPYGIRLS);
	if(!gang)
		return;
/*
 *	work out the % chance that the girl gets caught
 */
 	int catch_pc = g_Gangs.chance_to_catch(girl);
/*
 *	if they don't catch her, we're done
 */
	if(!g_Dice.percent(catch_pc))
		return;
/*
 *	OK: she got caught. Tell the player
 */
	string gmess = gettext("Your Goons spotted ");
	gmess += girl->m_Realname;
	gmess += gettext(" taking more gold then she reported.");
	gang->m_Events.AddMessage(gmess, IMGTYPE_PROFILE,  EVENT_GANG);

	girl->m_Pay = 0;
}

// ----- Objective
void cBrothelManager::UpdateObjective()
{
	if(m_Objective)
	{
		if(m_Objective->m_Limit != -1)
			m_Objective->m_Limit--;
		if(m_Objective->m_Limit == 0)
		{
			g_MessageQue.AddToQue(gettext("You have failed an objective."), 2);
			delete m_Objective;
			m_Objective = 0;
			return;
		}

		switch(m_Objective->m_Objective)
		{
		case OBJECTIVE_REACHGOLDTARGET:
			if(g_Gold.ival() >= m_Objective->m_Target)
				PassObjective();
			break;
		case OBJECTIVE_HAVEXGOONS:
			if(g_Gangs.GetNumGangs() >= m_Objective->m_Target)
				PassObjective();
			break;
		case OBJECTIVE_STEALXAMOUNTOFGOLD:
		case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
		case OBJECTIVE_KIDNAPXGIRLS:
		case OBJECTIVE_EXTORTXNEWBUSINESS:
			if(m_Objective->m_SoFar >= m_Objective->m_Target)
				PassObjective();
			break;
		case OBJECTIVE_HAVEXMONSTERGIRLS:
			if(GetTotalNumGirls(true) >= m_Objective->m_Target)
				PassObjective();
			break;
		case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
			if(GetTotalNumGirls() >= m_Objective->m_Target)
				PassObjective();
			break;

		// note that OBJECTIVE_GETNEXTBROTHEL has PassObjective() call in cScreenTown when passed.
		}
	}
}

sObjective* cBrothelManager::GetObjective(){return m_Objective;}

void cBrothelManager::CreateNewObjective()
{
	if(m_Objective)
		delete m_Objective;
	m_Objective = 0;

	m_Objective = new sObjective();
	if(m_Objective)
	{
		string message = gettext("You have a new objective, you must ");
		bool done = false;
		m_Objective->m_Difficulty = g_Year-1209;
		m_Objective->m_SoFar = 0;
		m_Objective->m_Reward = g_Dice%NUM_REWARDS;
		m_Objective->m_Limit = -1;
		m_Objective->m_Target = 0;

		while(!done)
		{
			m_Objective->m_Objective = g_Dice%NUM_OBJECTIVES;
			switch(m_Objective->m_Objective)
			{
			case OBJECTIVE_REACHGOLDTARGET:
				message += gettext("Acquire ");
				if(m_Objective->m_Difficulty >= 3)
				{
					m_Objective->m_Limit = (g_Dice%20)+10;
					m_Objective->m_Target = m_Objective->m_Limit*1000;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" gold within ");
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = ((g_Dice%20)+1)*200;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" gold.");
				}
				done = true;
				break;

			case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
				message += gettext("Launch a successful attack mission within ");
				if(m_Objective->m_Difficulty >= 3)
					m_Objective->m_Limit = (g_Dice%5)+3;
				else
					m_Objective->m_Limit = (g_Dice%10)+10;
				_itoa(m_Objective->m_Limit, buffer, 10);
				message += buffer;
				message += gettext(" weeks.");
				done = true;
				break;

			case OBJECTIVE_HAVEXGOONS:
				if(g_Gangs.GetNumGangs() < 8)
				{
					message += gettext("Have ");
					m_Objective->m_Target = g_Gangs.GetNumGangs() + ((g_Dice%3) + 1);
					if( m_Objective->m_Target > 8 )
						m_Objective->m_Target = 8;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" gangs within ");
					if(m_Objective->m_Difficulty >= 3)
						m_Objective->m_Limit = (g_Dice%4)+3;
					else
						m_Objective->m_Limit = (g_Dice%7)+6;
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
					done = true;
				}
				break;

			case OBJECTIVE_STEALXAMOUNTOFGOLD:
				if(m_Objective->m_Difficulty >= 2)
				{
					message += gettext("Steal ");
					m_Objective->m_Limit = (g_Dice%20)+13;
					m_Objective->m_Target = m_Objective->m_Limit*1300;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" gold within ");
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = ((g_Dice%20)+1)*200;
					message += gettext("Steal ");
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" gold.");
				}
				done = true;
				break;

			case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
				message += gettext("Capture ");
				if(m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice%5)+1;
					m_Objective->m_Target = (g_Dice%(m_Objective->m_Limit-1))+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" girls from the catacombs within ");
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = (g_Dice%5)+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" girls from the catacombs.");
				}
				done = true;
				break;

			case OBJECTIVE_HAVEXMONSTERGIRLS:
				message += gettext("Have a total of ");
				if(m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice%8)+3;
					m_Objective->m_Target = GetTotalNumGirls(true)+(g_Dice%(m_Objective->m_Limit-1))+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" monster (non-human) girls within ");
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = GetTotalNumGirls(true)+(g_Dice%8)+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" monster (non-human) girls.");
				}
				done = true;
				break;

			case OBJECTIVE_KIDNAPXGIRLS:
				{
					message += gettext("Kidnap ");
					if(m_Objective->m_Difficulty >= 2)
					{
						m_Objective->m_Limit = (g_Dice%5)+1;
						m_Objective->m_Target = (g_Dice%(m_Objective->m_Limit-1))+1;
						_itoa(m_Objective->m_Target, buffer, 10);
						message += buffer;
						message += gettext(" girls from the streets within ");
						_itoa(m_Objective->m_Limit, buffer, 10);
						message += buffer;
						message += gettext(" weeks.");
					}
					else
					{
						m_Objective->m_Target = (g_Dice%5)+1;
						_itoa(m_Objective->m_Target, buffer, 10);
						message += buffer;
						message += gettext(" girls from the streets.");
					}
					done = true;
				}break;

			case OBJECTIVE_EXTORTXNEWBUSINESS:
				{
					message += gettext("Gain control of ");
					if(m_Objective->m_Difficulty >= 2)
					{
						m_Objective->m_Limit = (g_Dice%5)+1;
						m_Objective->m_Target = (g_Dice%(m_Objective->m_Limit-1))+1;
						_itoa(m_Objective->m_Target, buffer, 10);
						message += buffer;
						message += gettext(" new businesses within ");
						_itoa(m_Objective->m_Limit, buffer, 10);
						message += buffer;
						message += gettext(" weeks.");
					}
					else
					{
						m_Objective->m_Target = (g_Dice%5)+1;
						_itoa(m_Objective->m_Target, buffer, 10);
						message += buffer;
						message += gettext(" new businesses.");
					}
					done = true;
				}break;

			case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
				message += gettext("Have a total of ");
				if(m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice%8)+3;
					m_Objective->m_Target = GetTotalNumGirls()+(g_Dice%(m_Objective->m_Limit-1))+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" girls within ");
					_itoa(m_Objective->m_Limit, buffer, 10);
					message += buffer;
					message += gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = GetTotalNumGirls()+(g_Dice%8)+1;
					_itoa(m_Objective->m_Target, buffer, 10);
					message += buffer;
					message += gettext(" girls.");
				}
				done = true;
				break;
				
			case OBJECTIVE_GETNEXTBROTHEL:
				if(GetNumBrothels() < 6)
				{
					message += gettext("Purchase a new brothel");
					if(m_Objective->m_Difficulty >= 2)
					{
						m_Objective->m_Limit = (g_Dice%10)+10;
						message += gettext(" within ");
						_itoa(m_Objective->m_Limit, buffer, 10);
						message += buffer;
						message += gettext(" weeks.");
					}
					else
						message += gettext(".");
					done = true;
				}
				break;
			}
		}

		g_MessageQue.AddToQue(message, 2);
	}
}

void cBrothelManager::PassObjective()
{
	if(m_Objective)
	{
		string message = gettext("You have completed your objective and you get ");
		switch(m_Objective->m_Reward)
		{
		case REWARD_GOLD:
			{
				long gold = (g_Dice%200)+33;
				if(m_Objective->m_Difficulty > 0)
					gold *= m_Objective->m_Difficulty;
				_ltoa(gold, buffer, 10);
				message += buffer;
				message += gettext(" gold.");
				g_Gold.objective_reward(gold);
			}break;

		case REWARD_GIRLS:
			{
				int girls = 1;
				if(m_Objective->m_Difficulty > 0)
					girls *= m_Objective->m_Difficulty;
				_itoa(girls, buffer, 10);
				message += buffer;
				message += gettext(" slave girls.");
				while(girls > 0)
				{
					m_Dungeon.AddGirl(g_Girls.CreateRandomGirl(0, true, "", true, true), DUNGEON_NEWGIRL);
					girls--;
				}
			}break;

		case REWARD_RIVALHINDER:
			{
				cRival* rival = m_Rivals.GetRandomRival();
				if(rival)
				{
					long gold = 0;
					message += gettext("to steal ");
					if(rival->m_Gold > 10)
						gold = (g_Dice%(rival->m_Gold/2))+1;
					else
						gold = 436;
					rival->m_Gold -= gold;
					_ltoa(gold, buffer, 10);
					message += buffer;
					message += gettext(" gold from the ");
					message += rival->m_Name;
					message += gettext(".");
					g_Gold.objective_reward(gold);
				}
				else
				{
					message = "";
					m_Objective->m_Reward  = REWARD_GOLD;
					PassObjective();
				}
			}break;

		case REWARD_ITEM:
			{
				int numItems = 1;
				if(m_Objective->m_Difficulty > 0)
					numItems *= m_Objective->m_Difficulty;
				while(numItems > 0)
				{
					//mod
					//purpose fix a crash
					sInventoryItem* item=0;
					do
					{
					item = g_InvManager.GetRandomItem();
					}while(!item);
					// end mod
					if(item->m_Rarity != 5)
					{
						int curI = g_Brothels.HasItem(item->m_Name, -1);
						bool loop = true;
						while(loop)
						{
							if(curI != -1)
							{
								if(g_Brothels.m_NumItem[curI] >= 999)
									curI = g_Brothels.HasItem(item->m_Name, curI+1);
								else
									loop = false;
							}
							else
								loop = false;
						}

						if(g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
						{
							if(curI != -1)
							{
								message += gettext("a ");
								message += item->m_Name;
								message += gettext(", ");
								g_Brothels.m_NumItem[curI]++;
							}
							else
							{
								for(int j=0; j<MAXNUM_INVENTORY; j++)
								{
									if(g_Brothels.m_Inventory[j] == 0)
									{
										message += gettext("a ");
										message += item->m_Name;
										message += gettext(", ");
										g_Brothels.m_Inventory[j] = item;
										g_Brothels.m_EquipedItems[j] = 0;
										g_Brothels.m_NumInventory++;
										g_Brothels.m_NumItem[j]++;
										break;
									}
								}
							}
							numItems--;
						}
						else
						{
							numItems=0;
							message += gettext(" Your inventory is full\n");
						}
					}
				}
			}break;
		}

		if(message != "")
			g_MessageQue.AddToQue(message, 2);
		delete m_Objective;
		m_Objective = 0;
	}
}

void cBrothelManager::AddCustomObjective(int limit, int diff, int objective, int reward, int sofar, int target)
{
	if(m_Objective)
		delete m_Objective;
	m_Objective = 0;

	m_Objective = new sObjective();
	
	m_Objective->m_Difficulty = diff;
	m_Objective->m_Limit = limit;
	m_Objective->m_Objective = objective;
	m_Objective->m_Reward = reward;
	m_Objective->m_SoFar = sofar;
	m_Objective->m_Target = target;
}

// ----- Stats

//mod, damn it, I am trying to do python code in c++
int &cBrothelManager::stat_lookup(string stat_name,int brothel_id)
{
	if(stat_name=="filth")
		return this->GetBrothel(brothel_id)->m_Filthiness;
	else if(stat_name=="advertising")
		return (int&)GetBrothel(brothel_id)->m_AdvertisingBudget;
	else if(stat_name=="security")
		return GetBrothel(brothel_id)->m_SecurityLevel;
	else if(stat_name=="beasts")
		return this->m_Beasts;
	return m_Dummy;
}

void cBrothelManager::SetGirlStat(sGirl* girl, int stat, int amount)
{
	girl->m_StatMods[stat] = amount;
}

void cBrothelManager::updateGirlTurnBrothelStats(sGirl* girl)
{
/*
 *	WD: Update each turn the stats for girl in brothel
 *
 *	Uses scaling formula in the form of 
 *		bonus = (60 - STAT_HOUSE) / div
 *
 *				div = 	
 *		STAT	30	20	15
 *		0		2	3	4
 *		1		1	2	3
 *		10		1	2	3
 *		11		1	2	3
 *		20		1	2	2
 *		21		1	1	2
 *		30		1	1	2
 *		31		0	1	1
 *		40		0	1	1
 *		41		0	0	1
 *		50		0	0	0
 *		51		0	0	0
 *		60		0	0	0
 *		61		-1	-1	-1
 *		70		-1	-1	-1
 *		71		-1	-1	-1
 *		80		-1	-1	-2
 *		81		-1	-2	-2
 *		90		-1	-2	-2
 *		91		-2	-2	-3
 *		100		-2	-2	-3
 *
 *
 */

//#define WDTEST // debuging
#undef WDTEST

	string msg;
	string girlName	= girl->m_Realname;
	int statHouse	= girl->house();
	int bonus		= (60 - statHouse) / 30;

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
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

	if (girl->is_slave())
	{
		if (bonus > 0)						// Slaves don't get penalties
		{
			girl->obedience(bonus);			// bonus vs house stat	0: 31-60, 1: 01-30, 2: 00
			girl->pcfear(-bonus);
			girl->pchate(-bonus);

			bonus = (60 - statHouse) / 15;
			girl->happiness(bonus);			// bonus vs house stat	0: 46-60, 1: 31-45, 2: 16-30, 3: 01-15, 4: 00
		}
	}
	else									// Free girls
	{
		girl->obedience(bonus);				// bonus vs house stat	-2: 91-100, -1: 61-90, 0: 31-60, 1: 01-30, 2: 00

		if (bonus > 0)						// no increase for hate or fear
		{
			girl->pcfear(-bonus);
			girl->pchate(-bonus);
		}

		bonus = (60 - statHouse) / 15;
		girl->happiness(bonus);				// bonus vs house stat	-3: 91-100, -2: 76-90, -1: 61-75, 0: 46-60, 1: 31-45, 2: 16-30, 3: 01-15, 4: 00
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

void cBrothelManager::UpdateAllGirlsStat(sBrothel* brothel, int stat, int amount)
{
	if(brothel)
	{
		sGirl* current = brothel->m_Girls;
		while(current)
		{
			g_Girls.UpdateStat(current, stat, amount);
			current = current->m_Next;
		}
	}
	else
	{
		sBrothel* curBroth = m_Parent;
		while(curBroth)
		{
			sGirl* current = curBroth->m_Girls;
			while(current)
			{
				g_Girls.UpdateStat(current, stat, amount);
				current = current->m_Next;
			}
			curBroth = curBroth->m_Next;
		}
	}
}

// ----- Get / Set
sGirl* cBrothelManager::GetGirl(int brothelID, int num)
{
	sBrothel* current = m_Parent;

	if(current == 0)
		return 0;

	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if (current->m_NumGirls == 0)
		return 0;

	// Makes num reset when it is >= m_NumGirls
	if (num >= current->m_NumGirls)
	{
		num = num % current->m_NumGirls;
	}

	if (num < 0)
	{
		num = current->m_NumGirls + (num % current->m_NumGirls);
	}

	if(current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;
		while(currentGirl)
		{
			if(count == num)
				break;
			count++;
			currentGirl = currentGirl->m_Next;
		}

		return currentGirl;
	}

	return 0;
}

int cBrothelManager::GetGirlPos(int brothelID, sGirl* girl)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if(current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;
		while(currentGirl)
		{
			if(currentGirl == girl)
				return count;
			count++;
			currentGirl = currentGirl->m_Next;
		}
	}

	return -1;
}

sGirl* cBrothelManager::GetGirlByName(int brothelID, string name)
{
	// Get the proper brothel
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	// Find the girl
	if(current)
	{
		sGirl* currentGirl = current->m_Girls;
		while(currentGirl)
		{
			if(strnatcmp(name.c_str(), currentGirl->m_Realname.c_str()) == 0)
				break;
			currentGirl = currentGirl->m_Next;
		}

		return currentGirl;
	}

	return 0;
}

int cBrothelManager::GetGirlsCurrentBrothel(sGirl* girl)
{
	// Used by new security guard code
	int BrothelNo = -1;

	for(int i = 0; i < m_NumBrothels; i++)
	{
		BrothelNo = GetGirlPos(i, girl);
		// MYR: Im an idiot. I wrote /= for 'not equals' (which is B syntax)
		// instead of !=
		if (BrothelNo != -1)
			return i;
	}

	return -1;
}

vector<sGirl*> cBrothelManager::GirlsOnJob(int BrothelID, int JobID, bool day)
{
	// Used by new security code
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == BrothelID)
			break;
		current = current->m_Next;
	}

	vector<sGirl*> GirlsOnJob;

	sGirl* curr = current->m_Girls;
	while(curr)
	{
		if(day)
		{
			if(curr->m_DayJob == JobID)
				GirlsOnJob.push_back(curr);
		}
		else
		{
			if(curr->m_NightJob == JobID)
				GirlsOnJob.push_back(curr);
		}
		curr = curr->m_Next;
	}

	return GirlsOnJob;
}

int cBrothelManager::GetTotalNumGirls(bool monster)
{
	int total = 0;
	if(!monster)
	{
		for(int i=0; i<m_NumBrothels; i++)
			total += GetNumGirls(i);
		total += GetDungeon()->GetNumGirls();
	}
	else
	{
		sBrothel* current = m_Parent;
		while(current)
		{
			sGirl* girl = current->m_Girls;
			while(girl)
			{
				if(g_Girls.HasTrait(girl, "Not Human"))
					total++;
				girl = girl->m_Next;
			}
			current = current->m_Next;
		}

		for(int i=0; i<GetDungeon()->GetNumGirls(); i++)
		{
			sDungeonGirl* dgirl = GetDungeon()->GetGirl(i);
			if(g_Girls.HasTrait(dgirl->m_Girl, "Not Human"))
				total++;
		}
	}

	return total;
}

string cBrothelManager::GetGirlString(int brothelID, int girlNum)
{
	string data = "";
	sBrothel* current = m_Parent;

	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if(current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;

		while(currentGirl)
		{
			if(count == girlNum)
				break;
			count++;
			currentGirl = currentGirl->m_Next;
		}
//need to change into string lookup
		string dayjob = "Resting";
		switch((int)currentGirl->m_DayJob)
		{
		case JOB_WHORESTREETS:
			dayjob = gettext("Streets");
			break;

		case JOB_WHOREBROTHEL:
			dayjob = gettext("Indoors");
			break;

		case JOB_INDUNGEON:
			dayjob = gettext("Dungeon");
			break;

		case JOB_TRAINING:
			dayjob = gettext("Training");
			break;
		}

		string nightjob = gettext("Resting");
		switch((int)currentGirl->m_NightJob)
		{
		case JOB_WHORESTREETS:
			nightjob = gettext("Streets");
			break;

		case JOB_WHOREBROTHEL:
			nightjob = gettext("Indoors");
			break;

		case JOB_INDUNGEON:
			nightjob = gettext("Dungeon");
			break;

		case JOB_TRAINING:
			nightjob = gettext("Training");
			break;
		}

		if(g_Girls.GetStat(currentGirl, STAT_AGE) == 100)
			sprintf(buffer,gettext("%s | Age: unknown | Health: %i | Happiness %i | Day Job: %s | Night Job: %s"),currentGirl->m_Realname.c_str(), (int)g_Girls.GetStat(currentGirl, STAT_HEALTH), (int)g_Girls.GetStat(currentGirl, STAT_HAPPINESS), dayjob.c_str(), nightjob.c_str());
		else
			sprintf(buffer,gettext("%s | Age: %i | Health: %i | Happiness %i | Day Job: %s | Night Job: %s"),currentGirl->m_Realname.c_str(), g_Girls.GetStat(currentGirl, STAT_AGE), (int)g_Girls.GetStat(currentGirl, STAT_HEALTH), (int)g_Girls.GetStat(currentGirl, STAT_HAPPINESS), dayjob.c_str(), nightjob.c_str());
		data = buffer;
	}

	return data;
}

string cBrothelManager::GetName(int brothelID)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if(current)
		return current->m_Name;
	return "cBrothelManager::GetName - Something went wrong";
}

string cBrothelManager::GetBrothelString(int brothelID)
{
	stringstream ss;
	sBrothel* brothel = GetBrothel(brothelID);
/*
 *	if we can't find the brothel, go home
 *	the error is logged in GetBrothel,
 *	so just return an empty string
 */
	if(!brothel) {
		return "";
	}
/*
 *	some shorthand variables for the simpler descriptions
 *	Commented out since we may want to put these back in at some stage
 *
	const char *has_bar =  (
		brothel->m_Bar == 1 ? "Yes" : "No"
	);
	const char *has_hall =  (
		brothel->m_GamblingHall == 1 ? "Yes" : "No"
	);
 */
	long profit = brothel->m_Finance.total_profit();
/*
 *	format the summary into one big string, and return it
 */
	ss << gettext("Customer Happiness: ")	<< happiness_text(brothel) << endl;
	ss << gettext("Fame: ")			<< fame_text(brothel);
	ss << gettext("Rooms (available/total): ")
	   << int(brothel->m_NumRooms) - int(brothel->m_NumGirls)
	   << "/"
	   << int(brothel->m_NumRooms)
	   << endl;
	//ss << "Strip Bar: "		<< has_bar		<< endl;
	//ss << "Gambling Hall: "	<< has_hall		<< endl;
	ss << gettext("This brothel's Profit: ")	<< profit		<< endl;
	ss << gettext("Your Gold: ")		<< g_Gold.ival()	<< endl;
	ss << gettext("Security Level: ") << brothel->m_SecurityLevel << endl;
	//ss << "Gambling Pool: "		<< m_GamblingHallPool	<< endl;
	ss << gettext("Disposition: ")		<< disposition_text()	<< endl;
	ss << gettext("Suspicion: ")		<< suss_text()		<< endl	;
	ss << gettext("Filthiness: ") <<  brothel->m_Filthiness << endl;
	ss << gettext("Beasts Owned: ") <<  g_Brothels.GetNumBeasts() << endl;
	return ss.str();
//add cleanliness and check gh and bh
}

sBrothel* cBrothelManager::GetBrothel(int brothelID)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			return current;
		current = current->m_Next;
	}
/*
 *	brothel not found at least deserves a log message
 */
	stringstream ss;
	ss << gettext("Brothel ") << brothelID << gettext(" not found in list!");
	g_LogFile.write(ss.str());
	return 0;
}

int cBrothelManager::GetNumBrothelsWithVacancies()
{
	int number = 0;
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_NumGirls < current->m_NumRooms)
			number++;
		current = current->m_Next;
	}

	return number;
}

int cBrothelManager::GetNumGirls(int brothelID)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if(current)
		return current->m_NumGirls;

	return 0;
}

int cBrothelManager::GetNumGirlsOnJob(int brothelID, int jobID, bool day)
{
	int count = 0;
	sBrothel* current = m_Parent;
	if (brothelID != -1)
	{
		while(current)
		{
			if(current->m_id == brothelID)
				break;
			current = current->m_Next;
		}
	}
//		if(g_Brothels.GetNumGirlsOnJob(-1, JOB_TORTURER, DayOrNight) == 1)

	while(current)
	{
		sGirl* curr = current->m_Girls;
		while(curr)
		{
			if(day)
			{
				if(curr->m_DayJob == jobID)
					count++;
			}
			else
			{
				if(curr->m_NightJob == jobID)
					count++;
			}
			curr = curr->m_Next;
		}
		if (brothelID == -1)
			current = current->m_Next;
		else
			current = 0;
	}

	return count;
}

void cBrothelManager::SetName(int brothelID, string name)
{
	string data = "";
	sBrothel* current = m_Parent;
	while(current)
	{
		if(current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if(current)
		current->m_Name = name;
	else
	  current->m_Name =  "cBrothelManager::GetName - Something went wrong";
}

// ----- Status texts
string cBrothelManager::disposition_text()
{
	if(m_Player.disposition() >= 80)	return gettext("Benevolent");
	if(m_Player.disposition() >= 50)	return gettext("Nice");
	if(m_Player.disposition() > 10)		return gettext("Pleasant");
	if(m_Player.disposition() >= -10)	return gettext("Neutral");
	if(m_Player.disposition() > -50)	return gettext("Not nice");
	if(m_Player.disposition() > -80)	return gettext("Mean");
						return gettext("Evil");
}

string cBrothelManager::fame_text(sBrothel* brothel)
{
	if(brothel->m_Fame >= 90)	return	gettext("World Renowned\n");
	if(brothel->m_Fame >= 80)	return	gettext("Famous\n");
	if(brothel->m_Fame >= 70)	return	gettext("Well Known\n");
	if(brothel->m_Fame >= 60)	return	gettext("Talk of the town\n");
	if(brothel->m_Fame >= 50)	return	gettext("Somewhat known\n");
	if(brothel->m_Fame >= 30)	return	gettext("Mostly unknown\n");
					return	gettext("Unknown\n");
}

string cBrothelManager::suss_text()
{
	//WD:	Should be Susipicion not Disposition 
	if(m_Player.suspicion() >= 80)	return gettext("Town Scum");
	if(m_Player.suspicion() >= 50)	return gettext("Miscreant");
	if(m_Player.suspicion() >= 10)	return gettext("Suspect");
	if(m_Player.suspicion() >= -10)	return gettext("Unsuspected");
	if(m_Player.suspicion() >= -50)	return gettext("Lawful");
	if(m_Player.suspicion() >= -80) 	return gettext("Philanthropist");
						return gettext("Town Hero");

	//if(m_Player.m_Disposition >= 80)	return "Town Scum";
	//if(m_Player.m_Disposition >= 50)	return "Miscreant";
	//if(m_Player.m_Disposition >= 10)		return "Suspect";
	//if(m_Player.m_Disposition >= -10)	return "Unsuspected";
	//if(m_Player.m_Disposition >= -50)	return "Lawful";
	//if(m_Player.m_Disposition >= -80) 	return "Philanthropist";
	//					return "Town Hero";
}

string cBrothelManager::happiness_text(sBrothel* brothel)
{
	if(brothel->m_Happiness >= 80)		return gettext("High");
	if(brothel->m_Happiness < 40)		return gettext("Low");
	else					return gettext("Medium");
}

// ----- Combat

// True means the girl beat the brothel master
bool cBrothelManager::PlayerCombat(sGirl* girl)		//  **************************** for now doesn't count items
{
	CLog l;

	// MYR: Sanity check: Incorporeal is an auto-win.
	if(girl->has_trait("Incorporeal") || girl->has_trait("Incorporial"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss()	<< gettext("\nGirl vs. Brothel owner: ") << girl->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return true;
	}

	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int pattack = SKILL_COMBAT;
	int dodge = 0;
	int pdodge = m_Player.m_Stats[STAT_AGILITY];
	int pHealth = 100;
	int pMana = 100;

	// first determine what she will fight with
	if(g_Girls.GetSkill(girl, SKILL_COMBAT) >= g_Girls.GetSkill(girl, SKILL_MAGIC))
		attack = SKILL_COMBAT;
	else
		attack = SKILL_MAGIC;

	// determine what player will fight with
	if(m_Player.m_Skills[SKILL_COMBAT] >= m_Player.m_Skills[SKILL_MAGIC])
		pattack = SKILL_COMBAT;
	else
		pattack = SKILL_MAGIC;

	// calculate the girls dodge ability
	if((g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS)) < 0)
		dodge = 0;
	else
		dodge = (g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS));

	while(g_Girls.GetStat(girl, STAT_HEALTH) > 20 && pHealth > 0)
	{
		// Girl attacks
		if((g_Dice%100)+1 < g_Girls.GetSkill(girl, attack))
		{
			int damage = 0;
			if(attack == SKILL_MAGIC)
			{
				if(g_Girls.GetStat(girl, STAT_MANA) <= 0)
				{
					attack = SKILL_COMBAT;
					damage = 2;
				}
				else
				{
					damage = 2+(g_Girls.GetSkill(girl, attack)/5);
					g_Girls.UpdateStat(girl,STAT_MANA,-7);
				}
			}
			else
			{
				// she has hit now calculate how much damage will be done
				damage = 5+(g_Girls.GetSkill(girl, attack)/10);
			}

			g_Girls.UpdateSkill(girl, attack, g_Dice%2);	// she may improve a little

			// player attempts Dodge
			if((g_Dice%100)+1 > pdodge)
				pHealth -= damage;
			else
				m_Player.m_Stats[STAT_AGILITY] += g_Dice%2;	// player may improve a little
		}

		// Player Attacks
		if((g_Dice%100)+1 < m_Player.m_Skills[pattack])
		{
			int damage = 0;
			if(pattack == SKILL_MAGIC)
			{
				if(pMana <= 0)
				{
					pattack = SKILL_COMBAT;
					damage = 2;
				}
				else
				{
					damage = 2+(m_Player.m_Skills[pattack]/5);
					pMana -= 5;
				}
			}
			else
			{
				// he has hit now calculate how much damage will be done
				damage = 5+(m_Player.m_Skills[pattack]/10);
			}

			m_Player.m_Skills[pattack] += g_Dice%2;	// he may improve a little

			// girl attempts Dodge
			if((g_Dice%100)+1 > dodge)
				g_Girls.UpdateStat(girl,STAT_HEALTH,-damage);
			else
			{
				m_Player.m_Stats[STAT_AGILITY] += g_Dice%2;	// player may improve a little
				if(m_Player.m_Stats[STAT_AGILITY] > 100)
					m_Player.m_Stats[STAT_AGILITY] = 100;
			}
		}

			
		// update girls dodge ability
		if((dodge - 2) < 0)
			dodge = 0;
		else
			dodge -= 2;

		// update players dodge ability
		if((pdodge - 2) < 0)
			pdodge = 0;
		else
			pdodge -= 2;
	}

	if(g_Girls.GetStat(girl, STAT_HEALTH) < 20)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
		return false;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1, true);

	return true;
}

bool cBrothelManager::FightsBack(sGirl* girl)
{
	if(g_Girls.GetStat(girl, STAT_HEALTH) < 10)	// she won't fight back in this state
		return false;

	if(g_Girls.HasTrait(girl, "Merciless") && ((g_Dice%100) <= 20))
		return true;

	if(g_Girls.DisobeyCheck(girl, ACTION_COMBAT))
		return true;

	return false;
}

// ----- Utility
sGirl* girl_sort(sGirl* girl, sGirl** lastgirl)	// sort using merge sort (cause works well with linked lists)
{
	sGirl *p, *q, *e, *tail;
    int insize, nmerges, psize, qsize, i;

	insize = 1;

	if(!girl)
	{
		*lastgirl=0;
		return 0;
	}
	while(1)
	{
		p = girl;
		girl = 0;
        tail = 0;
		nmerges = 0;
		while(p)
		{
			nmerges++;
			q = p;
			psize = 0;
			for(i = 0; i < insize; i++)
			{
				psize++;
				q = q->m_Next;
				if(!q)
					break;
            }

			qsize = insize;

			while(psize > 0 || (qsize > 0 && q))
			{
				if(psize == 0)
				{
					e = q;
					q = q->m_Next;
					qsize--;
				}
				else if(qsize == 0 || !q)
				{
					e = p;
					p = p->m_Next;
					psize--;
				}
				else if(sGirlcmp(p,q) <= 0)
				{
					e = p;
					p = p->m_Next;
					psize--;
				}
				else
				{
					e = q;
					q = q->m_Next;
					qsize--;
				}
				
				if(tail)
					tail->m_Next = e;
				else
					girl = e;
				e->m_Prev = tail;
				tail = e;
			}
			p = q;
		}
		
		tail->m_Next = 0;

		if(nmerges <= 1)
		{
			// find the last girl in the list
			//*lastgirl = tail;
			sGirl* tmp = girl;
			while(tmp->m_Next)
				tmp = tmp->m_Next;
			*lastgirl = tmp;
			return girl;
		}
		insize *= 2;
    }
}

int sGirlcmp(sGirl *a, sGirl *b)
{
	if(a == 0 || b == 0)
		return 1;
	return strcmp(a->m_Realname.c_str(), b->m_Realname.c_str());
}

void cBrothelManager::sort(sBrothel* brothel)
{
	brothel->m_Girls = girl_sort(brothel->m_Girls, &brothel->m_LastGirl);
}

bool cBrothelManager::NameExists(string name)
{
	sBrothel* current = m_Parent;
	while(current)
	{
		sGirl* currentGirl = current->m_Girls;
		while(currentGirl)
		{
			if(name == currentGirl->m_Realname)
				return true;
			currentGirl = currentGirl->m_Next;
		}
		current = current->m_Next;
	}
	return false;
}

// ----- Runaways
sGirl* cBrothelManager::GetFirstRunaway()
{
	return m_Runaways;
}

void cBrothelManager::RemoveGirlFromRunaways(sGirl* girl)
{
	if(girl->m_Next)
		girl->m_Next->m_Prev = girl->m_Prev;
	if(girl->m_Prev)
		girl->m_Prev->m_Next = girl->m_Next;
	if(girl == m_Runaways)
		m_Runaways = girl->m_Next;
	if(girl == m_LastRunaway)
		m_LastRunaway = girl->m_Prev;
	girl->m_Next = girl->m_Prev = 0;
	m_NumRunaways--;
}

void cBrothelManager::AddGirlToRunaways(sGirl* girl)
{
	girl->m_Prev = girl->m_Next = 0;
	if(m_Runaways)
	{
		girl->m_Prev = m_LastRunaway;
		m_LastRunaway->m_Next = girl;
		m_LastRunaway = girl;
	}
	else
		m_Runaways = m_LastRunaway = girl;

	m_NumRunaways++;
}

bool cBrothelManager::runaway_check(sBrothel *brothel, sGirl *girl)
{
/*
 *	nothing interesting happens here unless the girl is miserable
 *
 *	WD: added m_DaysUnhappy tracking
 */
 	if(girl->happiness() > 10) {
		girl->m_DaysUnhappy = 0;
		return false;
	}

	if(!girl->is_slave())
		girl->m_DaysUnhappy++;
/*
 *	now there's a matron on duty, she has a chance of fending off
 *	bad things.
 *
 *	previously, it was a 75% chance if a matron was employed
 *	so since we have two shifts, let's have a 35% chance per
 *	shift with a matron
 *
 *	with matrons being girls now, we have some opportunities
 *	for mischief here. For instance, is there still a matron skill?
 *	this should depend on that, if so. Also on how motivated the 
 *	matron is. An unhappy matron, or one who hates the PC
 *	may be inclined to turn a blind eye to runaway attempts
 */
//	int matron_chance = brothel->matron_count() * 35;
	int matron_chance = brothel->matron_count(girl->m_InClinic, girl->m_InMovieStudio, girl->m_InArena, girl->m_InCentre, girl->m_InHouse, girl->where_is_she) * 35;
	
	if(g_Dice.percent(matron_chance)) {
		return false;
	}

	if (girl->m_DayJob == JOB_REHAB && (g_Clinic.GetNumGirlsOnJob(0, JOB_DRUGCOUNSELOR, true) > 0) || (g_Clinic.GetNumGirlsOnJob(0, JOB_DRUGCOUNSELOR, false) > 0))
	{
		return false;
	}

/*
 *	mainly here, we're interested in the chance that she might run away
 */
	if(g_Girls.DisobeyCheck(girl, ACTION_GENERAL))	// check if the girl will run away
	{
		if(g_Dice.percent(m_JobManager.guard_coverage() - girl->m_DaysUnhappy)) {
			return false;
		}

		girl->m_Events.AddMessage(gettext("She ran away."), IMGTYPE_PROFILE, EVENT_SUMMARY);
		girl->m_DayJob = girl->m_NightJob = JOB_RUNAWAY;
		SetGirlStat(girl, STAT_TIREDNESS, 0);
		SetGirlStat(girl, STAT_HEALTH, 100);
		girl->m_RunAway = 6;
		string smess = "";
		smess += girl->m_Realname;
		smess += gettext(" has run away,");
		smess += gettext("  Send your goons after her to attempt recapture.\nShe will escape for good after 6 weeks");
		g_MessageQue.AddToQue(smess, 1);
		return true;
	}

	if(girl->m_Money <= 50) {
		return false;
	}

	if(g_Dice.percent(80- girl->m_DaysUnhappy)) {
		return false;
	}
/*
 *	if she is unhappy she may turn to drugs
 */
	bool starts_drugs = false;
	if(!g_Girls.HasTrait(girl, "Shroud Addict")) {
		g_Girls.AddTrait(girl, "Shroud Addict");
		starts_drugs = true;
	}
	else if(girl->happiness() <= 5 && !g_Girls.HasTrait(girl, "Fairy Dust Addict"))
	{
		g_Girls.AddTrait(girl, "Fairy Dust Addict");
		starts_drugs = true;
	}
	else if(girl->happiness() <= 2 && !g_Girls.HasTrait(girl, "Viras Blood Addict"))
	{
		g_Girls.AddTrait(girl, "Viras Blood Addict");
		starts_drugs = true;
	}
/*
 *	if she Just Said No then we're done
 */
	if(!starts_drugs) {
		return false;
	}
/*
 *	otherwise, report the sad occurrence
 */
	girl->m_Events.AddMessage(
		gettext("This girls unhappiness has turned her onto to drugs"),
		IMGTYPE_PROFILE, EVENT_WARNING
	);
	return false;
}

// ----- Drugs & addiction
void cBrothelManager::check_druggy_girl(stringstream& ss)
{
	if(g_Dice.percent(90)) 
		return;

	sGirl* girl = GetDrugPossessor();
	if(girl == 0)
		return;

	
	ss << gettext(" They also bust a girl named ")
	   << girl->m_Realname
	   << gettext(" for possession of drugs and send her to prison.")
	;
	if(g_Girls.HasTrait(girl, "Viras Blood Addict"))
		g_Girls.RemoveTrait(girl, "Viras Blood Addict");
	if(g_Girls.HasTrait(girl, "Fairy Dust Addict"))
		g_Girls.RemoveTrait(girl, "Fairy Dust Addict");
	if(g_Girls.HasTrait(girl, "Shroud Addict"))
		g_Girls.RemoveTrait(girl, "Shroud Addict");

	m_NumInventory = 0;
	for(int i=0; i<40; i++)
	{
		m_EquipedItems[i] = 0;
		m_Inventory[i] = 0;
	}

	AddGirlToPrison(girl);
}

sGirl* cBrothelManager::GetDrugPossessor()
{
	sBrothel* current = m_Parent;
	while(current)
	{
		sGirl* girl = current->m_Girls;
		while(girl)
		{
			if((g_Dice%100)+1 > g_Girls.GetStat(girl, STAT_INTELLIGENCE))	// girls will only be found out if low intelligence
			{
				if(g_Girls.HasItem(girl, "Shroud Mushroom") || g_Girls.HasItem(girl, "Fairy Dust") || g_Girls.HasItem(girl, "Vira Blood"))
					return girl;
				girl = girl->m_Next;
			}
		}
		current = current->m_Next;
	}

	return 0;
}

// ----- Dungeon & torture
void cBrothelManager::RemoveGirlFromPrison(sGirl* girl)
{
	if(girl->m_Next)
		girl->m_Next->m_Prev = girl->m_Prev;
	if(girl->m_Prev)
		girl->m_Prev->m_Next = girl->m_Next;
	if(girl == m_Prison)
		m_Prison = girl->m_Next;
	if(girl == m_LastPrison)
		m_LastPrison = girl->m_Prev;

	girl->m_Next = girl->m_Prev = 0;
	m_NumPrison--;
}

void cBrothelManager::AddGirlToPrison(sGirl* girl)
{
	// remove from girl manager if she is there
	g_Girls.RemoveGirl(girl);

	// remove girl from brothels if she is there
	for(int i=0; i<g_Brothels.GetNumBrothels(); i++)
		g_Brothels.RemoveGirl(i, girl, false);

	girl->m_Prev = girl->m_Next = 0;
	if(m_Prison)
	{
		girl->m_Prev = m_LastPrison;
		m_LastPrison->m_Next = girl;
		m_LastPrison = girl;
	}
	else
		m_Prison = m_LastPrison = girl;

	m_NumPrison++;
}

sGirl* cBrothelManager::WhoHasTorturerJob()
{
/*	WD:
 *	Loops through all brothels to find first
 *	girl with JOB_TORTURER
 *
 *	NOTE: assumes that only one girl, the first
 *	found is the torturer.
 *
 */

	sBrothel* curBrothel = m_Parent;
	sGirl* curGirl;

	while(curBrothel)								// WD: loop through all brothels
	{
		curGirl = curBrothel->m_Girls;

		while(curGirl)								// WD: loop through all girls in this brothels
		{
			if((curGirl->m_DayJob == JOB_TORTURER) || (curGirl->m_PrevDayJob == JOB_TORTURER))	// Should fix torturer crash thx to akia
			{
				//LastTortureGirl = curGirl;
				//return LastTortureGirl;
				return curGirl;
			}

			curGirl = curGirl->m_Next;
		}

		curBrothel = curBrothel->m_Next;
	}
	return 0;										// WD: Not Found
}

#if 0
/*
 * best way to comment out a large block of code is with
 * #if 0
 *
 * Doesn't complain about embedded comments, and you don't
 * need to change the code inside the ifdef
 */
/*
 * returns the number of customers the bar draws in
 */
int cBrothelManager::bar_update(sbrothel* brothel)
{
	ctariff tariff;

//	brothel->m_Finance.bar_upkeep(
//		tariff.empty_bar_cost()
//	);
//	if(GetAlcohol() == 0 && !m_KeepAlcStocked) {
//		if(!brothel->m_HasBarStaff) {
//			return 0;
//		}
		brothel->m_Finance.staff_wages(
			tariff.bar_staff_wages()
		);
//		return 0;
//	}
//*
// *	get the number of girls working day and night
// *
//	int day_girls = count_bar_girls(brothel, SHIFT_DAY);
//	int night_girls = count_bar_girls(brothel, SHIFT_NIGHT);
//*
// *	get the shifts for which the bar is staffed
// *
//	int shifts = 0;
//	if(brothel->m_HasBarStaff == 1) {
//		shifts = 2;
//	}
//	else {
//		if(day_girls > 2) shifts ++;
//		if(night_girls > 2) shifts ++;
//	}
//	if(shifts == 0) {
//		return 0;
//	}
//*
// *	work out the additional upkeep for running bar
// *	half price if you only run it one shift
// *
//	brothel->m_Finance.bar_upkeep(
//		tariff.active_bar_cost(brothel->m_Bar, shifts)
//	);
//
//	int maxCust = brothel->m_Bar*8;
//	int numCusts = g_Dice.random(maxCust) + day_girls + night_girls + 1;
//*
// *	loop through the customes, using up booze and adding income
// *
//	bool message_shown = false;
//	for(int i=0; i<numCusts; i++)
//	{
//		if(UseAlcohol())
//		{
//			// add the income
//			brothel->m_Finance.bar_income(g_Dice.random(20)+60);
//			brothel->m_Happiness += 100;
//			continue;
//		}
//		brothel->m_Happiness += 5;
//		if(!message_shown)
//		{
//			g_MessageQue.AddToQue(
//				"Your bars have run out of booze", 1
//			);
//			message_shown = true;
//		}
//	}
//*
// *	bar events - 90% of the time, nothing happens
// *
//	if(g_Dice.percent(90)) {
//		return numCusts;
//	}
//*
// *	bar room brawl - won't happen if guards on duty
// *
//	if(GetGangOnMission(MISS_GUARDING) > 0) {
//		return numCusts;
//	}
//*
// *	a brawl costs gold and booze
// *
//	long gold = (g_Dice%300)+1;
//	int barrels = (g_Dice%3)+1;
//*
// *	do the bookkeeping
// *
//	m_Alcohol -= barrels*5;
//	if(m_Alcohol < 0)
//		m_Alcohol = 0;
//	g_Gold.bar_upkeep(gold);
//*
// *	format the message
// *
//	stringstream ss;
//	ss << "A brawl breaks out in your bar located at "
//	   << brothel->m_Name
//	   << ". It costs you "				  << gold
//	   << " gold to repair the damages and you lost " << barrels
//	   << " barrels of alcohol."
//	;
//	g_MessageQue.AddToQue(ss.str(), 2);
//	return numCusts;
}

/*
 * returns the number of customers the bar draws in
 */
int cBrothelManager::casino_update(sBrothel* brothel)
{
	cTariff tariff;
/*
 *	if the casino is staffed, it costs money
 */
	if(brothel->m_HasGambStaff) {
		brothel->m_Finance.casino_upkeep( 
			tariff.empty_casino_cost(
				brothel->m_GamblingHall
			)
		);
	}

	int day_girls = count_casino_girls(brothel, SHIFT_DAY);
	int night_girls = count_casino_girls(brothel, SHIFT_NIGHT);
/*
 *	get the shifts for which the casino is staffed
 */
	int shifts = 0;
	if(brothel->m_HasGambStaff == 1) {
		shifts = 2;
	}
	else {
		if(day_girls > 4)	shifts ++;
		if(night_girls > 4)	shifts ++;
	}
/*
 *	if there's no-one on either shift, we can go home
 */
	if(shifts == 0) {
		return 0;
	}
/*
 *	if the casino is operational, there are overheads
 *	above and beyond basic long term maintenance
 */
	brothel->m_Finance.building_upkeep(
		tariff.active_casino_cost(brothel->m_GamblingHall, shifts)
	);


	int count = 0;
	int random_range = 0;
	int numCusts = 2;
	int odds = GetGamblingChances();

	if(odds > 0)
	{
		count = (odds / 5) + 1;		// the amount of customer attracted is proportional to the chance of winning
		random_range = brothel->m_GamblingHall*count;
		numCusts += g_Dice.random(random_range) + day_girls + night_girls;
	}

	bool message_shown = false;
	for(int i=0; i<numCusts; i++)
	{
		if(!message_shown && GetGamblingPool() == 0)
		{
			g_MessageQue.AddToQue(gettext("CAUTION: The gold pool for the gambling halls is empty."), 1);
			message_shown = true;
		}
/*
 *		calculate the size of the wager (won or lost)
 *		based on values in config.xml
 */
		int wager = g_Config.gamble.base; 
		wager += g_Dice.random( g_Config.gamble.spread);
		bool customer_wins = g_Dice.percent(g_Config.gamble.odds);

		if(customer_wins == false)		// customer lose
		{
			wager = int(wager * g_Config.gamble.house_factor);
/*
 *			if the customer loses, wee gain some money
 *			but customer happiness goes up a little anyway.
 *			Because he likes to gamble, presumably.
 *			Fair enough: no one would run gaming tables
 *			if it made the customers miserable.
 */
			brothel->m_Happiness += 10;
			AddGamblingPool(wager);
			continue;
		}
/*
 *		if the customer wins, customer happiness goes up by a lot
 */
		brothel->m_Happiness += 100;
		wager = int(wager * g_Config.gamble.customer_factor);
/*
 *		if there's enough in the pool to cover it,
 *		pay out from the pool
 */
		int pool = GetGamblingPool();
		if(pool >= wager) {
			TakeGamblingPool(wager);
			continue;		// next punter, please!
		}
/*
 *		Here, and the customer wins more than the pool contains
 *
 *		There should probably be a bit more fanfare
 *		if a customer wins more on the tables than the
 *		player can cover: "the man who broke the bank at Monte Carlo"
 *		and all that...
 *
 *		Possibly have him take a girl or two as payment
 *		(if there are slaves working in the casino)
 *		Or worst case, lose the brothel. That would be a game over
 *		but then so would going too far in the red, so it works out the
 *		same.
 *
 *		otherwise, you'd be able to buy back the brothel 
 *		(after a cooldown period) and the girls would
 *		eventually show up in the slave market again
 */
		g_Gold.misc_debit(wager - pool);
		TakeGamblingPool(pool);

	}
/*
 *	if the last run through emptied the pool
 *	tell the customer
 */
	if(!message_shown && GetGamblingPool() == 0) {
		g_MessageQue.AddToQue(gettext("CAUTION: The gold pool for the gambling halls is empty."), 1);
		message_shown = true;
	}
/*
 *	get excess from pool and place into players gold
 */
	if(GetGamblingPool() > m_GamblingHallPoolMax)
	{
		int transfer = GetGamblingPool()-m_GamblingHallPoolMax;
		TakeGamblingPool(transfer);
		brothel->m_Finance.gambling_profits(transfer);
	}
	return numCusts;
}

#endif
