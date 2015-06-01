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
#include "cFarm.h"

//#include "cDungeon.h"
//#include "cJobManager.h"
//#include "cCustomers.h"
//#include "cGirls.h"
//#include "GameFlags.h"
//#include "CGraphics.h"
//#include "sConfig.h"
//#include "cTariff.h"

extern cMessageQue			g_MessageQue;
extern cCustomers			g_Customers;
extern cGirls				g_Girls;
extern cInventory			g_InvManager;
extern cBrothelManager		g_Brothels;
extern unsigned long		g_Year;
extern unsigned long		g_Month;
extern unsigned long		g_Day;
extern cRng					g_Dice;
extern cGold				g_Gold;
extern cGangManager			g_Gangs;
extern cMovieStudioManager  g_Studios;
extern cArenaManager		g_Arena;
extern cClinicManager		g_Clinic;
extern cCentreManager		g_Centre;
extern cHouseManager		g_House;
extern cFarmManager			g_Farm;

// `J` the one and only time GetPlayer() should be used (hopefully)
// `J` This will be moved to cPlayer.(h/cpp) eventually, just cleaning it up for now
cPlayer* The_Player = g_Brothels.GetPlayer();	



//extern CGraphics			g_Graphics;

/*
* mod - this is a bit big for an inline func
* and we don't create so many of them that we
* we need the speed - so taken out of the header
*/

// Prototypes
sGirl* girl_sort(sGirl* girl, sGirl** lastgirl);
int sGirlcmp(sGirl *a, sGirl *b);

// // ----- Strut sBrothel Create / destroy
sBrothel::sBrothel() : m_Finance(0)	// constructor
{
	m_MiscCustomers = 0;
	m_TotalCustomers = 0;
	m_Filthiness = 0;
	m_HasGambStaff = 0;
	m_HasBarStaff = 0;
	m_AdvertisingBudget = 0;
	m_Next = 0;
	m_Girls = 0;
	m_LastGirl = 0;
	m_Fame = 0;
	m_Happiness = 0;
	// end mod
	m_NumGirls = 0;
	m_ShowTime = 0;
	m_ShowQuality = 0;
	m_SecurityLevel = 0;
	m_KeepPotionsStocked = 0;
	m_AntiPregPotions = 0;
	m_RestrictAnal = false;
	m_RestrictBDSM = false;
	m_RestrictBeast = false;
	m_RestrictFoot = false;
	m_RestrictGroup = false;
	m_RestrictHand = false;
	m_RestrictLesbian = false;
	m_RestrictNormal = false;
	m_RestrictOral = false;
	m_RestrictStrip = false;
	m_RestrictTitty = false;
	for (u_int i = 0; i < NUMJOBTYPES; i++) m_BuildingQuality[i] = 0;

	//movie

	m_CurrFilm = 0;
	m_NumMovies = 0;
	m_LastMovies = 0;
	m_Movies = 0;
	m_MovieRunTime = 0;
}

sBrothel::~sBrothel()			// destructor
{
	m_ShowTime = 0;
	m_ShowQuality = 0;

	if (m_Next) delete m_Next;
	m_Next = 0;
	if (m_Girls) delete m_Girls;
	m_LastGirl = 0;
	m_Girls = 0;
	//movie
	if (m_CurrFilm) delete m_CurrFilm;
	m_NumMovies = 0;
	if (m_Movies) delete m_Movies;
	m_Movies = 0;
	m_LastMovies = 0;
}

// ----- Matron  // `J` added building checks
bool sBrothel::matron_on_shift(int shift, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, bool isFarm, int BrothelID)
{
	/* */if (isArena)	{ if (g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, shift) > 0)				return true; }
	else if (isStudio)	{ if (g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, shift) > 0)			return true; }
	else if (isClinic)	{ if (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, shift) > 0)			return true; }
	else if (isCentre)	{ if (g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, shift) > 0)		return true; }
	else if (isHouse)	{ if (g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, shift) > 0)				return true; }
	else if (isFarm)	{ if (g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, shift) > 0)			return true; }
	else /*         */	{ if (g_Brothels.GetNumGirlsOnJob(BrothelID, JOB_MATRON, shift) > 0)	return true; }
	return false;
}


//int sBrothel::matron_count
int sBrothel::matron_count(bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, bool isFarm, int BrothelID)
{
	int i, sum = 0;
	for (i = 0; i < 2; i++)
	{
		/* */if (isArena)	{ if (g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, i) > 0)				sum++; }
		else if (isStudio)	{ if (g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, i) > 0)			sum++; }
		else if (isClinic)	{ if (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, i) > 0)			sum++; }
		else if (isCentre)	{ if (g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, i) > 0)		sum++; }
		else if (isHouse)	{ if (g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, i) > 0)				sum++; }
		else if (isFarm)	{ if (g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, i) > 0)			sum++; }
		else /*        */	{ if (g_Brothels.GetNumGirlsOnJob(BrothelID, JOB_MATRON, i) > 0)	sum++; }
	}
	return sum;
}

// ----- Class cBrothelManager Create / destroy
cBrothelManager::cBrothelManager()			// constructor
{
	for (int i = 0; i < MAXNUM_INVENTORY; i++)
	{
		m_Inventory[i] = 0;
		m_EquipedItems[i] = 0;
		m_NumItem[i] = 0;
	}
	/* sBrothel */	m_Parent = m_Last = 0;
	/* int */		m_NumBrothels = m_Influence = m_NumInventory = m_NumPrison = m_NumRunaways = m_HandmadeGoods = m_Beasts = m_Alchemy = 0;
	/* int */		m_SupplyShedLevel = 1;
	/* int */		m_HandmadeGoodsReserves = m_AlchemyReserves = 0;
	/* int */		m_BeastsReserves = 100;
	/* int */		m_FoodReserves = m_DrinksReserves = 1000;
	/* int */		m_Processing_Shift = -1;
	/* long */		m_BribeRate = m_Bank = 0;
	/* sObjective */m_Objective = 0;
	/* sGirl */		m_Prison = m_LastPrison = m_Runaways = m_LastRunaway = 0;
	/* bool */		m_TortureDoneFlag = false;
	m_JobManager.Setup();
}

cBrothelManager::~cBrothelManager()			// destructor
{
	Free();
}

void cBrothelManager::Free()
{
	if (m_Prison)		delete m_Prison;
	if (m_Runaways)		delete m_Runaways;
	/* sGirls */	m_Prison = m_LastPrison = m_Runaways = m_LastRunaway = 0;
	/* int    */	m_NumPrison = m_NumRunaways = m_NumInventory = 0;
	for (int i = 0; i < MAXNUM_INVENTORY; i++)
	{
		m_Inventory[i] = 0;
		m_EquipedItems[i] = 0;
		m_NumItem[i] = 0;
	}
	/* long   */	m_BribeRate = m_Bank = 0;
	/* int    */	m_NumBrothels = m_Influence = m_HandmadeGoods = m_Beasts = m_Alchemy = 0;
	/* int    */	m_SupplyShedLevel = 1;
	if (m_Objective)	delete m_Objective;
	m_Objective = 0;
	m_Dungeon.Free();
	m_Rivals.Free();
	if (m_Parent)		delete m_Parent;
	m_Parent = m_Last = 0;
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
	if (rival_mgr->player_safe() == false)
	{
		rival = rival_mgr->get_influential_rival();
	}
	/*
	*	chance is based on how much suspicion is leveled at
	*	the player, less his influence at city hall.
	*
	*	And then modified back upwards by rival influence
	*/
	int pc = The_Player->suspicion() - m_Influence;
	if (rival)
	{
		pc += rival->m_Influence / 4;
	}
	/*
	*	pc gives us the % chance of a raid
	*	let's do the "not raided" case first
	*/
	if (g_Dice.percent(pc) == false) {
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
	ss << "The local authorities perform a bust on your operations: ";
	/*
	*	if we make our influence check, the guard captain will be under
	*	orders from the mayor to let you off.
	*
	*	Let's make sure the player can tell
	*/
	if (g_Dice.percent(m_Influence))
	{
		ss << "the guard captain lectures you on the importance of crime prevention, whilst also passing on the Mayor's heartfelt best wishes.";
		The_Player->suspicion(-5);
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
		return;
	}
	/*
	*	if we have a rival influencing things, it might not matter
	*	if the player is squeaky clean
	*/

	if (rival && The_Player->disposition() > 0 && g_Dice.percent(rival->m_Influence / 2))
	{
		int fine = (g_Dice % 1000) + 150;
		g_Gold.fines(fine);
		ss << "the guard captain condemns your operation as a hotbed of criminal activity and fines you " << fine << " gold for 'living without due care and attention'.";
		/*
		*		see if there's a girl using drugs he can nab
		*/
		check_druggy_girl(ss);
		/*
		*		make sure the player knows why the captain is
		*		being so blatantly unfair
		*/
		ss << "On his way out the captain smiles and says that the " << rival->m_Name << " send their regards.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		return;
	}
	/*
	*	if the player is basically a goody-goody type
	*	he's unlikely to have anything incriminating on
	*	the premises. 20 disposition should see him
	*/
	if (g_Dice.percent(The_Player->disposition() * 5))
	{
		ss << "they pronounce your operation to be entirely in accordance with the law.";
		The_Player->suspicion(-5);
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
		return;
	}
	int nPlayer_Disposition = The_Player->disposition();
	if (nPlayer_Disposition > -10)
	{
		int fine = (g_Dice % 100) + 20;
		g_Gold.fines(fine);
		ss << "they find you in technical violation of some health and safety ordinances, and they fine you " << fine << " gold.";
	}
	else if (nPlayer_Disposition > -30)
	{
		int fine = (g_Dice % 300) + 40;
		g_Gold.fines(fine);
		ss << "they find some minor criminalities and fine you " << fine << " gold.";
	}
	else if (nPlayer_Disposition > -50)
	{
		int fine = (g_Dice % 600) + 100;
		g_Gold.fines(fine);
		ss << "they find evidence of dodgy dealings and fine you " << fine << " gold.";
	}
	else if (nPlayer_Disposition > -70)
	{
		int fine = (g_Dice % 1000) + 150;
		int bribe = (g_Dice % 300) + 100;
		g_Gold.fines(fine + bribe);
		ss << "they find a lot of illegal activities and fine you " << fine << " gold, it also costs you an extra " << bribe << " to pay them off from arresting you.";
	}
	else if (nPlayer_Disposition > -90)
	{
		int fine = (g_Dice % 1500) + 200;
		int bribe = (g_Dice % 600) + 100;
		g_Gold.fines(fine + bribe);
		ss << "they find enough dirt to put you behind bars for life. " << "It costs you " << bribe << " to stay out of prison, plus another " << fine << " in fines on top of that";
	}
	else
	{
		int fine = (g_Dice % 2000) + 400;
		int bribe = (g_Dice % 800) + 150;
		g_Gold.fines(fine + bribe);
		ss << "the captain declares your premises to be a sinkhole of the utmost vice and depravity, and it is only with difficulty that you dissuade him from seizing all your property on the spot. You pay " << fine << " gold in fines, but only after slipping the captain " << bribe << " not to drag you off to prison.";
	}
	/*
	*	check for a drug-using girl they can arrest
	*/
	check_druggy_girl(ss);
	g_MessageQue.AddToQue(ss.str(), COLOR_RED);
}

int cBrothelManager::TotalFame(sBrothel * brothel)
{
	int total_fame = 0;
	sGirl* current = brothel->m_Girls;
	while (current)
	{
		total_fame += g_Girls.GetStat(current, STAT_FAME);
		current = current->m_Next;
	}
	return total_fame;
}

bool cBrothelManager::CheckScripts()
{
	sBrothel* current = m_Parent;
	DirPath base;
	if (cfg.folders.configXMLch())
		base = DirPath() << cfg.folders.characters() << "";
	else
		base = DirPath() << "Resources" << "Characters" << "";
	while (current)
	{
		sGirl* girl;
		for (girl = current->m_Girls; girl; girl = girl->m_Next)
		{
			// if no trigger for this girl, skip to the next one
			if (!girl->m_Triggers.GetNextQueItem()) continue;
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
bool UseAntiPreg(bool use, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, bool isFarm, int whereisshe)
{
	if (!use) return false;
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
	*
	*	`J` adjusted it so it uses your existing stock first
	*	before it buys extras at a higher cost as emergency stock
	*
	*/
	cTariff tariff;
	int cost = tariff.anti_preg_price(1);
	if (isClinic)
	{
		if (g_Clinic.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_Clinic.GetBrothel(0)->m_AntiPregPotions < g_Clinic.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Clinic.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Clinic.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Clinic.GetBrothel(0)->m_AntiPregUsed++;
			g_Clinic.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else if (isStudio)
	{
		if (g_Studios.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_Studios.GetBrothel(0)->m_AntiPregPotions < g_Studios.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Studios.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Studios.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Studios.GetBrothel(0)->m_AntiPregUsed++;
			g_Studios.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else if (isArena)
	{
		if (g_Arena.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_Arena.GetBrothel(0)->m_AntiPregPotions < g_Arena.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Arena.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Arena.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Arena.GetBrothel(0)->m_AntiPregUsed++;
			g_Arena.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else if (isCentre)
	{
		if (g_Centre.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_Centre.GetBrothel(0)->m_AntiPregPotions < g_Centre.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Centre.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Centre.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Centre.GetBrothel(0)->m_AntiPregUsed++;
			g_Centre.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else if (isHouse)
	{
		if (g_House.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_House.GetBrothel(0)->m_AntiPregPotions < g_House.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_House.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_House.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_House.GetBrothel(0)->m_AntiPregUsed++;
			g_House.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else if (isFarm)
	{
		if (g_Farm.GetBrothel(0)->m_KeepPotionsStocked)
		{
			if (g_Farm.GetBrothel(0)->m_AntiPregPotions < g_Farm.GetBrothel(0)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Farm.GetBrothel(0)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Farm.GetBrothel(0)->m_AntiPregPotions > 0)
		{
			g_Farm.GetBrothel(0)->m_AntiPregUsed++;
			g_Farm.GetBrothel(0)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	else
	{
		if (g_Brothels.GetBrothel(whereisshe)->m_KeepPotionsStocked)
		{
			if (g_Brothels.GetBrothel(whereisshe)->m_AntiPregPotions < g_Brothels.GetBrothel(whereisshe)->m_AntiPregUsed) cost *= 5;
			g_Gold.consumable_cost(cost);
			g_Brothels.GetBrothel(whereisshe)->m_AntiPregUsed++;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
		if (g_Brothels.GetBrothel(whereisshe)->m_AntiPregPotions > 0)
		{
			g_Brothels.GetBrothel(whereisshe)->m_AntiPregUsed++;
			g_Brothels.GetBrothel(whereisshe)->m_AntiPregPotions--;
			if (g_Dice.percent(cfg.pregnancy.anti_preg_failure()))	return false;
			return true;
		}
	}
	return false;
}

//void cBrothelManager::AddAntiPreg(int amount)
void sBrothel::AddAntiPreg(int amount) // unused
{
	m_AntiPregPotions += amount;
	if (m_AntiPregPotions > 700)
		m_AntiPregPotions = 700;
}


// ----- Add / remove
void cBrothelManager::AddGirl(int brothelID, sGirl* girl)
{
	if (girl == 0)	return;
	if (girl->m_InStudio)	girl->m_DayJob = girl->m_NightJob = JOB_FILMFREETIME;
	else if (girl->m_InArena)	girl->m_DayJob = girl->m_NightJob = JOB_ARENAREST;
	else if (girl->m_InCentre)	girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
	else if (girl->m_InClinic)	girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
	else if (girl->m_InHouse)	girl->m_DayJob = girl->m_NightJob = JOB_HOUSEREST;
	else if (girl->m_InFarm)	girl->m_DayJob = girl->m_NightJob = JOB_FARMREST;
	else						girl->m_DayJob = girl->m_NightJob = JOB_RESTING;

	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	g_Girls.RemoveGirl(girl, false);
	girl->where_is_she = brothelID;

	girl->m_Prev = girl->m_Next = 0;
	if (current->m_Girls)
	{
		girl->m_Prev = current->m_LastGirl;
		current->m_LastGirl->m_Next = girl;
		current->m_LastGirl = girl;
	}
	else
		current->m_LastGirl = current->m_Girls = girl;

	current->m_NumGirls++;
}

void cBrothelManager::RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl)
{
	if (girl == 0) return;
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	sGirl* t = current->m_Girls;
	bool found = false;
	while (t)
	{
		if (t == girl)
		{
			found = true;
			break;
		}
		t = t->m_Next;
	}
	t = 0; // MYR

	if (found == true)
	{
		if (girl->m_Next)					girl->m_Next->m_Prev = girl->m_Prev;
		if (girl->m_Prev)					girl->m_Prev->m_Next = girl->m_Next;
		if (girl == current->m_Girls)		current->m_Girls = girl->m_Next;
		if (girl == current->m_LastGirl)	current->m_LastGirl = girl->m_Prev;
		girl->m_Next = girl->m_Prev = 0;
		if (deleteGirl)
		{
			delete girl;
			girl = 0;
		}
		current->m_NumGirls--;
	}
}

// ----- Load save create destroy
bool cBrothelManager::LoadDataXML(TiXmlHandle hBrothelManager)
{
	Free();//everything should be init even if we failed to load an XML element
	//watch out, this frees dungeon and rivals too

	TiXmlElement* pBrothelManager = hBrothelManager.ToElement();
	if (pBrothelManager == 0) return false;

	stringstream ss;

	// load the player
	//         ...................................................
	g_LogFile.write("***************** loading Player ******************");
	m_Player.LoadPlayerXML(hBrothelManager.FirstChild("Player"));

	// load the dungeon
	//         ...................................................
	g_LogFile.write("***************** loading dungeon *****************");
	m_Dungeon.LoadDungeonDataXML(hBrothelManager.FirstChild("Dungeon"));

	// load supply shed level, other goodies
	pBrothelManager->QueryIntAttribute("SupplyShedLevel", &m_SupplyShedLevel);
	pBrothelManager->QueryIntAttribute("HandmadeGoods", &m_HandmadeGoods);
	pBrothelManager->QueryIntAttribute("HandmadeGoodsReserves", &m_HandmadeGoodsReserves);
	pBrothelManager->QueryIntAttribute("Beasts", &m_Beasts);
	pBrothelManager->QueryIntAttribute("BeastsReserves", &m_BeastsReserves);
	pBrothelManager->QueryIntAttribute("AlchemyIngredients", &m_Alchemy);
	pBrothelManager->QueryIntAttribute("AlchemyIngredientsReserves", &m_AlchemyReserves);
	pBrothelManager->QueryIntAttribute("Food", &m_Food);
	pBrothelManager->QueryIntAttribute("FoodReserves", &m_FoodReserves);
	pBrothelManager->QueryIntAttribute("Drinks", &m_Drinks);
	pBrothelManager->QueryIntAttribute("DrinksReserves", &m_DrinksReserves);


	// load runaways
	m_NumRunaways = 0;
	TiXmlElement* pRunaways = pBrothelManager->FirstChildElement("Runaways");
	if (pRunaways)
	{
		for (TiXmlElement* pGirl = pRunaways->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
		{	// load each girl and add her
			sGirl* rgirl = new sGirl();
			bool success = rgirl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true) { AddGirlToRunaways(rgirl); }
			else { delete rgirl; continue; }
		}
	}

	// load prison
	m_NumPrison = 0;
	TiXmlElement* pPrisonGirls = pBrothelManager->FirstChildElement("PrisonGirls");
	if (pPrisonGirls)
	{
		for (TiXmlElement* pGirl = pPrisonGirls->FirstChildElement("Girl");
			pGirl != 0;
			pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
		{
			sGirl* pgirl = new sGirl();
			bool success = pgirl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true) { AddGirlToPrison(pgirl); }
			else { delete pgirl; continue; }
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
		
		if (pObjective->Attribute("Text"))	// `J` added
		{
			m_Objective->m_Text = pObjective->Attribute("Text");
		}
		else m_Objective->m_Text = "";
	}

	// load rivals
	//         ...................................................
	g_LogFile.write("***************** Loading rivals *****************");
	m_Rivals.LoadRivalsXML(hBrothelManager.FirstChild("Rival_Manager"));

	// Load inventory
	//         ...................................................
	g_LogFile.write("************ Loading players inventory ************");
	//now would be a great time to move this to cPlayer
	LoadInventoryXML(hBrothelManager.FirstChild("Inventory"), m_Inventory, m_NumInventory, m_EquipedItems, m_NumItem);

	// load alcohol restock
	//	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	//	ifs>>m_KeepAlcStocked;

	//         ...................................................
	g_LogFile.write("***************** Loading brothels ****************");
	m_NumBrothels = 0;
	TiXmlElement* pBrothels = pBrothelManager->FirstChildElement("Brothels");
	if (pBrothels)
	{
		for (TiXmlElement* pBrothel = pBrothels->FirstChildElement("Brothel"); pBrothel != 0; pBrothel = pBrothel->NextSiblingElement("Brothel"))
		{
			sBrothel* current = new sBrothel();
			bool success = current->LoadBrothelXML(TiXmlHandle(pBrothel));
			if (success == true) { AddBrothel(current); }
			else { delete current; continue; }
		} // load a brothel
	}
	return true;
}

bool sBrothel::LoadBrothelXML(TiXmlHandle hBrothel)
{
	//no need to init this, we just created it
	TiXmlElement* pBrothel = hBrothel.ToElement();
	if (pBrothel == 0) return false;
	if (pBrothel->Attribute("Name")) m_Name = pBrothel->Attribute("Name");
	int tempInt = 0;
	g_LogFile.write("Loading brothel: " + m_Name);

	// load variables for sex restrictions
	pBrothel->QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
	pBrothel->QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
	pBrothel->QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
	pBrothel->QueryValueAttribute<bool>("RestrictFoot", &m_RestrictFoot);
	pBrothel->QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
	pBrothel->QueryValueAttribute<bool>("RestrictHand", &m_RestrictHand);
	pBrothel->QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);
	pBrothel->QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
	pBrothel->QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
	pBrothel->QueryValueAttribute<bool>("RestrictStrip", &m_RestrictStrip);
	pBrothel->QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);

	pBrothel->QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
	pBrothel->QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
	pBrothel->QueryIntAttribute("AntiPregUsed", &m_AntiPregUsed);
	pBrothel->QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);

	pBrothel->QueryIntAttribute("Bar", &tempInt); m_Bar = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute(gettext("Fame"), &tempInt); m_Fame = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("GamblingHall", &tempInt); m_GamblingHall = tempInt; tempInt = 0;
	pBrothel->QueryValueAttribute<unsigned short>("Happiness", &m_Happiness);
	pBrothel->QueryIntAttribute("HasBarStaff", &tempInt); m_HasBarStaff = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("id", &m_id);
	pBrothel->QueryIntAttribute("HasGambStaff", &tempInt); m_HasGambStaff = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("NumRooms", &tempInt); m_NumRooms = tempInt; tempInt = 0;
	pBrothel->QueryIntAttribute("MaxNumRooms", &tempInt); m_MaxNumRooms = tempInt; tempInt = 0;
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
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
		for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
		{	// load each girl and add her
			sGirl* girl = new sGirl();
			bool success = girl->LoadGirlXML(TiXmlHandle(pGirl));
			if (success == true) { girl->where_is_she = m_id; AddGirl(girl); }
			else { delete girl; continue; }
		}
	}
	return true;
}

void sBrothel::AddGirl(sGirl* pGirl)
{
	if (m_Girls)
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

	g_LogFile.write("************* saving Player data ******************");
	m_Player.SavePlayerXML(pBrothelManager);

	g_LogFile.write("************* saving dungeon data *****************");
	m_Dungeon.SaveDungeonDataXML(pBrothelManager);

	// save preg potions, supply shed level, other goodies
	pBrothelManager->SetAttribute("SupplyShedLevel", m_SupplyShedLevel);
	pBrothelManager->SetAttribute("HandmadeGoods", m_HandmadeGoods);
	pBrothelManager->SetAttribute("HandmadeGoodsReserves", m_HandmadeGoodsReserves);
	pBrothelManager->SetAttribute("Beasts", m_Beasts);
	pBrothelManager->SetAttribute("BeastsReserves", m_BeastsReserves);
	pBrothelManager->SetAttribute("AlchemyIngredients", m_Alchemy);
	pBrothelManager->SetAttribute("AlchemyIngredientsReserves", m_AlchemyReserves);
	pBrothelManager->SetAttribute("Food", m_Food);
	pBrothelManager->SetAttribute("FoodReserves", m_FoodReserves);
	pBrothelManager->SetAttribute("Drinks", m_Drinks);
	pBrothelManager->SetAttribute("DrinksReserves", m_DrinksReserves);

	// save runaways
	TiXmlElement* pRunaways = new TiXmlElement("Runaways");
	pBrothelManager->LinkEndChild(pRunaways);
	sGirl* rgirl = m_Runaways;
	while (rgirl)
	{
		rgirl->SaveGirlXML(pRunaways);
		rgirl = rgirl->m_Next;
	}

	// save prison
	TiXmlElement* pPrison = new TiXmlElement("PrisonGirls");
	pBrothelManager->LinkEndChild(pPrison);
	sGirl* pgirl = m_Prison;
	while (pgirl)
	{
		pgirl->SaveGirlXML(pPrison);
		pgirl = pgirl->m_Next;
	}

	// save bribe rate and bank
	pBrothelManager->SetAttribute("BribeRate", m_BribeRate);
	pBrothelManager->SetAttribute("Bank", m_Bank);

	// save objective
	if (m_Objective)
	{
		TiXmlElement* pObjective = new TiXmlElement("Objective");
		pBrothelManager->LinkEndChild(pObjective);
		pObjective->SetAttribute("Difficulty", m_Objective->m_Difficulty);
		pObjective->SetAttribute("Limit", m_Objective->m_Limit);
		pObjective->SetAttribute("Objective", m_Objective->m_Objective);
		pObjective->SetAttribute("Reward", m_Objective->m_Reward);
		pObjective->SetAttribute("SoFar", m_Objective->m_SoFar);
		pObjective->SetAttribute("Target", m_Objective->m_Target);
		pObjective->SetAttribute("Text", m_Objective->m_Text);
	}

	g_LogFile.write("***************** Saving rivals *******************");
	m_Rivals.SaveRivalsXML(pBrothelManager);

	g_LogFile.write("************** Saving players inventory ***********");
	TiXmlElement* pInventory = new TiXmlElement("Inventory");
	pBrothelManager->LinkEndChild(pInventory);
	SaveInventoryXML(pInventory, m_Inventory, MAXNUM_INVENTORY, m_EquipedItems, m_NumItem);

	// save alcohol restock
	//	ofs<<m_KeepAlcStocked<<endl;

	g_LogFile.write("***************** Saving brothels *****************");
	TiXmlElement* pBrothels = new TiXmlElement("Brothels");
	pBrothelManager->LinkEndChild(pBrothels);
	sBrothel* current = m_Parent;

	while (current)
	{
		g_LogFile.write("Saving brothel: " + current->m_Name);
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
	pBrothel->SetAttribute("RestrictBeast", m_RestrictBeast);
	pBrothel->SetAttribute("RestrictFoot", m_RestrictFoot);
	pBrothel->SetAttribute("RestrictGroup", m_RestrictGroup);
	pBrothel->SetAttribute("RestrictHand", m_RestrictHand);
	pBrothel->SetAttribute("RestrictLesbian", m_RestrictLesbian);
	pBrothel->SetAttribute("RestrictNormal", m_RestrictNormal);
	pBrothel->SetAttribute("RestrictOral", m_RestrictOral);
	pBrothel->SetAttribute("RestrictStrip", m_RestrictStrip);
	pBrothel->SetAttribute("RestrictTitty", m_RestrictTitty);

	pBrothel->SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
	pBrothel->SetAttribute("AntiPregPotions", m_AntiPregPotions);
	pBrothel->SetAttribute("AntiPregUsed", m_AntiPregUsed);
	pBrothel->SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);

	pBrothel->SetAttribute("Bar", m_Bar);
	pBrothel->SetAttribute("Fame", m_Fame);
	pBrothel->SetAttribute("GamblingHall", m_GamblingHall);
	pBrothel->SetAttribute("Happiness", m_Happiness);
	pBrothel->SetAttribute("HasBarStaff", m_HasBarStaff);
	pBrothel->SetAttribute("id", m_id);
	pBrothel->SetAttribute("HasGambStaff", m_HasGambStaff);
	pBrothel->SetAttribute("NumRooms", m_NumRooms);
	if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
	else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
	pBrothel->SetAttribute("MaxNumRooms", m_MaxNumRooms);
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
	while (girl)
	{
		girl->SaveGirlXML(pGirls);
		girl = girl->m_Next;
	}
	/*
	*		save the building setup
	*/
	return pBrothel;
}

void cBrothelManager::NewBrothel(int NumRooms, int MaxNumRooms)
{
	sBrothel* newBroth = new sBrothel();
	newBroth->m_NumRooms = NumRooms;
	newBroth->m_MaxNumRooms = MaxNumRooms;
	newBroth->m_Next = 0;

	AddBrothel(newBroth);
}

void cBrothelManager::AddBrothel(sBrothel* newBroth)
{
	if (m_Parent)
	{
		m_Last->m_Next = newBroth;
		newBroth->m_id = m_Last->m_id + 1;
		m_Last = newBroth;
	}
	else
	{
		m_Parent = m_Last = newBroth;
		newBroth->m_id = 0;
	}
	m_NumBrothels++;
}

void cBrothelManager::DestroyBrothel(int ID)
{
	sBrothel* current = m_Parent;

	if (current->m_id == ID)
	{
		m_Parent = current->m_Next;
		current->m_Next = 0;
		delete current;
		return;
	}

	while (current->m_Next)
	{
		if (current->m_Next->m_id == ID) break;
		current = current->m_Next;
	}

	if (current)
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
	if (num_rivals > 5) return;					// a full set of rivals = nothing to do
	if (num_rivals == 0 && !peace)				// if there are no rivals, and we were not at peace last turn, peace has broken out
	{
		peace = true;
		peace_breaks_out();
	}
	if (The_Player->m_WinGame == false) return;	// we only create new rivals after the game has been won
	if (g_Dice.percent(70)) return;				// create new random rival or not!
	peace = false;								// flag the war as on again, (should be a field somewhere)
	m_Rivals.CreateRandomRival();				// create a new rival and tell the player the good news
	g_MessageQue.AddToQue(new_rival_text(), COLOR_RED);
}

string cBrothelManager::new_rival_text()
{
	stringstream ss;

	enum {
		Slaver = 0,
		Gladiator = 1,
		Goon = 2,
		Slave = 3,
		Mage = 4,
		Demon = 5,
		Priest = 6,
		Noble = 7,
		Technologist = 8,
		Patriarch = 9,	// or Matriarch
		MaxChallengers = 10
	};

	bool male = g_Dice.percent(75);
	/*
	*	let's put the gender specific terms in
	*	variables. Might make the code cleaner
	*/
	string man, boy, He, he, him, his, sorcerer, gladiator, fellow, patriarch;
	if (male)
	{
		He = "He";
		he = "he";
		him = "him";
		his = "his";
		man = "man";
		boy = "boy";
		sorcerer = "sorcerer";
		gladiator = "gladiator";
		fellow = "fellow";
		patriarch = "patriarch ";
	}
	else
	{
		He = "She";
		he = "she";
		him = "her";
		his = "her";
		man = "woman";
		boy = "girl";
		sorcerer = "sorceress";
		gladiator = "gladiatrix";
		fellow = "wench";			// not sure what the feminine of "fellow" is I did wonder about "fellatrix"...
		patriarch = "matriarch ";
	}

	switch (g_Dice.random(MaxChallengers))
	{
	case Slaver:
		ss << "A lieutenant reports that one of the professional slavers, finding customers be scarce, has taken to whoring out " << his << " slavegirls to make ends meet. Your men arranged a meet with " << him << " in order to explain your position on the subject, but the discussion did not go well, ending with bared steel and threats of blood.\n\nIt would seem you have a challenger.";
		break;
	case Gladiator:
		ss << "Ask any Crossgate sports fan who rules the Arenas of the city. Almost always, the answer will be the same. For five long years one " << gladiator << " has stood " << his << " ground on the bloody sands and defied all who came before " << him << ".\n\nLast week, the " << gladiator << " bought " << his << " freedom from the arena, and chose to celebrate the occasion at one of your brothels. Sadly, an overindulgence in wine led to harsh words and a rash vow to show you how a whorehouse SHOULD be run.\n\nWith anyone else, the matter would have ended when the morning brought sobriety. But this is a " << man << " who has never turned " << his << " back on any sort of challenge. With wealthy admirers supplying premises and finance, and with a handful of arena veterans to provide the core of " << his << " enforcers, this is a challenger you would be foolish to ignore.";
		break;
	case Goon:
		ss << "The " << boy << " was just skin and bones; a dull eyed waif from gutters of Sleaze Street, a dozen like " << him << " on any street corner. But put a knife in " << his << " hands and the " << boy << " became an artist, painting effortless masterpieces in blood and greased lightning.\n\nQuickly recruited into one of the goon squads, it soon became apparent that behind that flat unblinking stare, there lurked a mind almost as keen as " << his << " blades. The " << boy << " rose quickly, coming to head " << his << " own squad before becoming one of your trusted lieutenants. If only " << his << " ambition had stopped there...\n\n" << "" << ((male) ? "His" : "Her") << " challenge was almost over before it began; for you that is. That you still live says more about the skill of your healers than any talent you might lay claim to. Your newest rival is not only a deadly fighter and a clever strategist, but one who knows your operation, inside and out.\n\nThis will not be easy.";
		break;
	case Slave:
		ss << "There are ways to beat a slaver tattoo. It wouldn't do were that to become widely known, of course. Nevertheless there are ways around it. One such is to find an area of unstable spacetime. Do it right, and you can overload the tracking spell, and the enchantment just falls apart. This is, of course wildly dangerous, but many escapees nevertheless head straight for the Crossgate sewers, which on a bad day can give the catacombs a run for their money.\n\nOver time, a community of ecapees has grown up in the sewers, survivor types, grown hardy in the most hostile environment. And as long as they stay down there, no one much minds. If nothing else they keep the monster population down. But now they seem to be organising a crusade. Against slavery. Against exploitation. Against you.\n\nRumour has it that their leader is one of your offspring, conceived of rape, born into slavery. True or not, this new factions seems determined to bring about your downfall.\n\nThis time, as the bards would say, it is personal.";
		break;
	case Mage:
		ss << "The " << sorcerer << " blew into town with a travelling entertainer show, promising exotic pleasures and the taste of forbidden fruit. But behind the showman's patter and the coloured smoke, the pleasures on offer were of a distinctly carnal nature, and no more exotic than those you yourself could offer.\n\nFor a travelling show, this need not be a problem. For a week, or even two, you can stand to see a little competition. However, the newcomer has been here a month now and shows no sign of moving on. On the contrary, he appears to be shopping for permanent premises.\n\nWith this in mind, you send some men to explain the situation. To everyone's surprise, it turns out that behind the glib charlatanry, there lies genuine magecraft, most likely tantric in nature.\n\nIn your organisation you have no shortage of mages. Any fighting force in Crossgate needs a battle mage or two. This newcomer however operates on a level far beyond what you are used to. And he seems determined to stay, and challenge you for control of the city.";
		break;
	case Priest:
		break;
	case Noble:
		ss << "They say " << he << " is a noble, an exile from " << his << " native land. Certainly, " << he << " has the manners of a courtier and the amused weariness of the jaded dilettante.\n\nAnd yet it seems there is steel behind the foppery, as many a Crossgate duelist has learned. And a wit to match the blade as well. An admirable " << fellow << " this, one you would be pleased to call 'friend', if only ...\n\nEarlier this week, your men were explaining to a handful of freelance scrubbers how prostitution worked in this city. If only " << he << " had not chosen to take the women's side against your men. If only " << his << " rash defiance had not caught the imagination of the city's duellists.\n\nAlas, such was not to be.\n\nEn Garde!";
		break;
	case Technologist:
		ss << "From the distant city of Abby's Crossing comes a new rival to challenge for your throne, wielding some strange non-magic " << he << " calls 'technology', an alien art of smoke and steam and noise and lighting; one they say functions strangely in Mundiga, when it chooses to work at all.\n\nBut the hollow metal men that make up " << his << " enforcers would seem to work with deadly efficicency and the strange collapsible maze " << he << " calls a 'tesseract' seems to share many properties with the catacombs under your headquarters. Then there are rumours of strange procedures that can break a slavegirl's will, far faster than the most skilled of Crossgate's torturers.\n\nIn short, far from unreliable, " << his << " arts seem deadly efficient to you. You have no idea what other surprises this otherworldly artisan may have up " << his << " sleeve, but one thing is for certain: this challenge may not go unanswered.";
		break;
	case Patriarch:
		ss << "Outside the walls of Crossgate, there is a shanty-town maze of tumbledown hovels, teeming with the poorest and most desperate of the City's inhabitants. Polygamy and incest are rife here, and extended families can run into the hundreds\n\nOne such family is ruled by the iron will of a dreadful old " << patriarch << " with a well earned reputation for utter ruthlessness. For years " << he << " has sent " << his << " progeny to the city markets, to trade, to steal, to bring back money for the clan in any way they can.\n\nNow it seems they are expanding their operation to include organised prostitution. Bad move.\n\nSomething about the " << patriarch << "'s operation disturbs you. There is a coldness in the way " << he << " sends sons and grandsons out to die for " << him << "; the way " << he << "casually rapes and enslaves " << his << " own daughters and granddaughters before sending them off to whore for " << him << ". This " << man << " holds up a mirror to what you are - or perhaps to what you could easily become. The image it presents is far from flattering.\n\nPersonal feelings aside, this is a situation that can only get worse. The time to end this, is now.";
		break;
	case Demon:
		ss << "Somewhere in Crossgate, a hand trembled inscribing a pentagram; a tongue stumbled over the nine syllables of the charm of binding. A magical being slipped his arcane bonds and slaughtered those mages foolish enough to dream they might command it.\n\nA demon lord now stalks the streets of the city.\n\nWhich, in itself, is not so big a deal. It is not of unheard that the aristocracy of Hell should find themselves stumbling dazed and confused through Crossgate market. They just tend to recover quickly and promptly open a portal home.\n\nBut not this one. This one chooses to briefly linger, to partake of Crossgate society and seek such amusements as the city can offer. Unfortunately, it seems the demon finds amusement trafficking in human misery and human sex. As do you, in the eyes of many.\n\nFor a demon, 'briefly' may be anything from a day to a thousand years. You cannot afford to wait until it grows bored. A demon lord is a formidable opponent, but to ignore this challenge will send entirely the wrong signal to the other would be whore-masters in the city.\n\nLike it or not, this means war.";
		break;
	}
	return ss.str();
}

void cBrothelManager::peace_breaks_out()
{
	stringstream ss;
	// if the PC already won, this is just an minor outbreak of peace in the day-to-day feuding in crossgate
	if (The_Player->m_WinGame)
	{
		ss << "The last of your challengers has been overthrown. Your domination of Crossgate is absolute.\n\nUntil the next time that is...";
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
		return;
	}
	// otherwise, the player has just won flag it as such
	The_Player->m_WinGame = true;
	// let's have a bit of chat to mark the event
	ss.str("");
	ss << "The last of your father's killers has been brought before you for judgement. None remain who would dare to oppose you. For all intents and purposes, the city is yours.\n\nWhether or not your father will rest easier for your efforts, you cannot say, but now, with the city at your feet, you feel sure he would be proud of you at this moment.\n\nBut pride comes before a fall, and in Crossgate, complacency kills. The city's slums and slave markets and the fighting pits are full of hungry young bloods burning to make their mark on the world, and any one of them could rise to challenge you at any time.\n\nYou may have seized the city, but holding on to it is never going to be easy.";
	g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	return;
}

// ----- Update & end of turn
void cBrothelManager::UpdateBrothels()	// Start_Building_Process_A
{
	cTariff tariff;
	stringstream ss;
	sBrothel* current = (sBrothel*)m_Parent;
	u_int restjob = JOB_RESTING;
	u_int matronjob = JOB_MATRON;
	u_int firstjob = JOB_RESTING;
	u_int lastjob = JOB_WHORESTREETS;

	m_TortureDoneFlag = false;							//WD: Reset flag each day is set in WorkTorture()

	UpdateBribeInfluence();

	while (current)
	{
		// reset the data
		current->m_Happiness = current->m_MiscCustomers = current->m_TotalCustomers = 0;
		current->m_Finance.zero();
		current->m_Events.Clear();
		current->m_AntiPregUsed = 0;
		current->m_RejectCustomersRestrict = current->m_RejectCustomersDisease = 0;

		bool matron = (GetNumGirlsOnJob(current->m_id, matronjob, false) >= 1) ? true : false;

		// `J` do all the things that the girls do at the start of the turn
		sGirl* cgirl = current->m_Girls;
		while (cgirl)
		{
			string girlName = cgirl->m_Realname;

			// Remove any dead bodies from last week
			if (cgirl->health() <= 0)
			{
				sGirl* DeadGirl = cgirl;
				cgirl = (cgirl->m_Next) ? cgirl->m_Next : 0;
				UpdateAllGirlsStat(current, STAT_PCFEAR, 2);	// increase all the girls fear of the player for letting her die (weather his fault or not)
				UpdateAllGirlsStat(current, STAT_PCHATE, 1);	// increase all the girls hate of the player for letting her die (weather his fault or not)
				// Two messages go into the girl queue...
				ss.str("");
				ss << girlName << " has died from her injuries, the other girls all fear and hate you a little more.";
				DeadGirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
				g_MessageQue.AddToQue(ss.str(), COLOR_RED);
				ss.str("");
				ss << girlName << " has died from her injuries.  Her body will be removed by the end of the week.";
				DeadGirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_SUMMARY);
				RemoveGirl(current->m_id, DeadGirl);
				DeadGirl = 0;
				ss.str("");
			}
			else
			{
				cgirl->where_is_she = current->m_id;
				cgirl->m_InStudio = cgirl->m_InArena = cgirl->m_InCentre = cgirl->m_InClinic = cgirl->m_InFarm = cgirl->m_InHouse = false;
				cgirl->m_Events.Clear();
				cgirl->m_Pay = cgirl->m_Tips = 0;
				cgirl->m_Tort = false;

				// `J` Check for out of building jobs
				if (cgirl->m_DayJob	  < firstjob || cgirl->m_DayJob   > lastjob)	cgirl->m_DayJob = restjob;
				if (cgirl->m_NightJob < firstjob || cgirl->m_NightJob > lastjob)	cgirl->m_NightJob = restjob;
				if (cgirl->m_PrevDayJob != 255 && (cgirl->m_PrevDayJob	 < firstjob || cgirl->m_PrevDayJob   > lastjob))	cgirl->m_PrevDayJob = 255;
				if (cgirl->m_PrevNightJob != 255 && (cgirl->m_PrevNightJob < firstjob || cgirl->m_PrevNightJob > lastjob))	cgirl->m_PrevNightJob = 255;
				// set yesterday jobs for everyone
				cgirl->m_YesterDayJob = cgirl->m_DayJob;
				cgirl->m_YesterNightJob = cgirl->m_NightJob;
				cgirl->m_Refused_To_Work_Day = false;
				cgirl->m_Refused_To_Work_Night = false;
				string summary = "";

				g_Girls.AddTiredness(cgirl);			// `J` moved all girls add tiredness to one place
				do_food_and_digs(current, cgirl);		// Brothel only update for girls accommodation level
				g_Girls.CalculateGirlType(cgirl);		// update the fetish traits
				g_Girls.updateGirlAge(cgirl, true);		// update birthday counter and age the girl
				g_Girls.HandleChildren(cgirl, summary);	// handle pregnancy and children growing up
				g_Girls.updateSTD(cgirl);				// health loss to STD's				NOTE: Girl can die
				g_Girls.updateHappyTraits(cgirl);		// Update happiness due to Traits	NOTE: Girl can die
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

		// Moved to here so Security drops once per day instead of everytime a girl works security -PP
		current->m_SecurityLevel -= 10;
		current->m_SecurityLevel -= current->m_NumGirls;	//`J` m_SecurityLevel is extremely over powered. Reducing it's power a lot.
		if (current->m_SecurityLevel <= 0) current->m_SecurityLevel = 0;	 // crazy added


		// Generate customers for the brothel for the day shift and update girls
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

		ss.str("");
		ss << current->m_TotalCustomers << " customers visited the building.";
		if (current->m_RejectCustomersRestrict > 0) ss << "\n\n" << current->m_RejectCustomersRestrict << " were turned away because of your sex restrictions.";
		if (current->m_RejectCustomersDisease > 0) ss << "\n\n" << current->m_RejectCustomersDisease << " were turned away because they had an STD.";
		current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BROTHEL);

		// empty rooms cost 2 gold to maintain
		current->m_Finance.building_upkeep(tariff.empty_room_cost(current));

		// update brothel stats
		if (current->m_NumGirls > 0)
			current->m_Fame = (TotalFame(current) / current->m_NumGirls);
		if (current->m_Happiness > 0 && g_Customers.GetNumCustomers())
			current->m_Happiness = min(100, current->m_Happiness / current->m_TotalCustomers);


		// advertising costs are set independently for each brothel
		current->m_Finance.advertising_costs(tariff.advertising_costs(current->m_AdvertisingBudget));

		ss.str("");
		ss << "Your advertising budget for this brothel is " << current->m_AdvertisingBudget << " gold.";
		if (tariff.advertising_costs(current->m_AdvertisingBudget) != current->m_AdvertisingBudget)
		{
			ss << " However, due to your configuration, you instead had to pay " <<
				tariff.advertising_costs(current->m_AdvertisingBudget) << " gold.";
		}
		current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BROTHEL);

		// `J` include antipreg potions in summary
		ss.str("");
		if (current->m_AntiPregPotions > 0 || current->m_AntiPregUsed > 0)
		{
			int num = current->m_AntiPregPotions;
			int used = current->m_AntiPregUsed;
			bool stocked = current->m_KeepPotionsStocked;
			bool matron = (GetNumGirlsOnJob(current->m_id, matronjob, false) >= 1);
			bool skip = false;	// to allow easy skipping of unneeded lines
			bool error = false;	// in case there is an error this makes for easier debugging

			// first line: previous stock
			if (stocked && num > 0)		ss << "You keep a regular stock of " << num << " Anti-Pregnancy potions in this brothel.\n\n";
			else if (num + used > 0)	ss << "You " << (used > 0 ? "had" : "have") << " a stock of " << (num + used) << " Anti-Pregnancy potions in this brothel.\n\n";
			else { skip = true;			ss << "You have no Anti-Pregnancy potions in this brothel."; }

			// second line: number used
			/* */if (skip){}	// skip the rest of the lines
			else if (used == 0)			{ skip = true;	ss << "None were used.\n\n"; }
			else if (num == 0)			{ skip = true;	ss << "All have been used.\n\n"; }
			else if (used > 0 && stocked)				ss << used << " were " << (used > num ? "needed" : "used") << " this week.\n\n";
			else if (used > 0 && num > 0 && !stocked)	ss << used << " were used this week leaving " << num << " in stock.\n\n";
			else
			{	// `J` put this in just in case I missed something
				ss << "error code::  BAP02|" << current->m_AntiPregPotions << "|" << current->m_AntiPregUsed << "|" << current->m_KeepPotionsStocked << "  :: Please report it to pinkpetal.org so it can be fixed";
				error = true;
			}

			// third line: budget
			if (!skip && stocked)
			{
				int cost = 0;
				if (used > num)
				{
					ss << used - num << " more than were in stock were needed so an emergency restock had to be made.\n";
					ss << "Normally they cost " << tariff.anti_preg_price(1) << " gold, but our supplier charges five times the normal price for unscheduled deliveries.\n\n";
					cost += tariff.anti_preg_price(num);
					cost += tariff.anti_preg_price(used - num) * 5;
				}
				else
				{
					cost += tariff.anti_preg_price(used);
				}

				ss << "Your budget for Anti-Pregnancy potions for this brothel is " << cost << " gold.";

				if (matron && used > num)
				{
					int newnum = (((used / 10) + 1) * 10) + 10;

					current->AddAntiPreg(newnum - num);
					ss << "\n\nThe Matron of this brothel has increased the quantity of Anti-Pregnancy potions for further orders to " << current->m_AntiPregPotions << ".";
				}
			}
			if (error) g_LogFile.write("\n\n" + ss.str() + "\n\n");
			current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BROTHEL);
		}



		// update the global cash
		g_Gold.brothel_accounts(current->m_Finance, current->m_id);

		if (current->m_Filthiness < 0) current->m_Filthiness = 0;
		if (current->m_SecurityLevel < 0) current->m_SecurityLevel = 0;

		cgirl = current->m_Girls;
		while (cgirl)
		{
			g_Girls.updateTemp(cgirl);			// update temp stuff
			g_Girls.EndDayGirls(current, cgirl);
			cgirl = cgirl->m_Next;
		}

		current = current->m_Next;		// goto the next brothel
	}

	// Update the bribe rate
	g_Gold.bribes(m_BribeRate);

	if (m_Runaways)
	{
		sGirl* rgirl = m_Runaways;
		while (rgirl)
		{
			if (rgirl->m_RunAway > 0)
			{
				// there is a chance the authorities will catch her if she is branded a slave
				if (rgirl->is_slave() && g_Dice.percent(5))
				{
					// girl is recaptured and returned to you
					sGirl* temp = rgirl;
					rgirl = temp->m_Next;
					RemoveGirlFromRunaways(temp);
					m_Dungeon.AddGirl(temp, DUNGEON_GIRLRUNAWAY);
					g_MessageQue.AddToQue(gettext("A runnaway slave has been recaptured by the authorities and returned to you."), COLOR_GREEN);
					continue;
				}
				rgirl->m_RunAway--;
			}
			else	// add her back to girls available to reacquire
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

	if (m_Prison)
	{
		if (g_Dice.percent(10))	// 10% chance of someone being released
		{
			sGirl* girl = m_Prison;
			RemoveGirlFromPrison(girl);
			g_Girls.AddGirl(girl);
		}
	}

	// keep gravitating player suspicion to 0
	/* */if (The_Player->suspicion() > 0)	The_Player->suspicion(-1);
	else if (The_Player->suspicion() < 0)	The_Player->suspicion(1);
	if (The_Player->suspicion() > 20) check_raid();	// is the player under suspision by the authorities

	if (m_Bank > 0)									// incraese the bank gold by 02%
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
	ss.str("");
	if (g_Gangs.GetNumBusinessExtorted() > 0)
	{
		if (g_Dice.percent(6.7))
		{
			sGirl* girl = g_Girls.CreateRandomGirl(17, false);
			ss << "A man cannot pay so he sells you his daughter " << girl->m_Realname << " to clear his debt to you.\n";
			stringstream ssg;
			ssg << girl->m_Realname << "'s father could not pay his debt to you so he gave her to you as payment.";
			girl->m_Events.AddMessage(ssg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
			m_Dungeon.AddGirl(girl, DUNGEON_NEWGIRL);
			g_Gangs.NumBusinessExtorted(-1);
		}
		long gold = g_Gangs.GetNumBusinessExtorted()*INCOME_BUSINESS;
		ss << "You gain " << gold << " gold from the " << g_Gangs.GetNumBusinessExtorted() << " businesses under your control.\n";
		g_Gold.extortion(gold);
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	}

	do_tax();
	check_rivals();

	ss.str("");
	long totalProfit = g_Gold.total_profit();
	if (totalProfit < 0)
	{
		ss << "Your brothel had an overall deficit of " << -totalProfit << " gold.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
	}
	else if (totalProfit > 0)
	{
		ss << "You made a overall profit of " << totalProfit << " gold.";
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	}
	else
	{
		ss << "You are breaking even (made as much money as you spent)";
		g_MessageQue.AddToQue(ss.str(), COLOR_DARKBLUE);
	}

	// MYR: I'm really curious about what goes in these if statements

	// DustyDan, 04/08/2013:  This is for future to include inside these ifs, 
	// the actions to take when not enough businesses controlled to support the 
	// number of brothels currently owned (according to formula that allowed original purchase).

	// Suggest future something like not allowing any net profit from the brothels 
	// that are unsupported by enough businesses. 
	// Forcing sale of a brothel would be too drastic; maybe allowing sale if player 
	// wants to would be an option to present.

	// `J` added loss of security if not enough businesses held.

	if (g_Gangs.GetNumBusinessExtorted() < 40 && GetNumBrothels() >= 2)
	{
		g_Brothels.GetBrothel(1)->m_SecurityLevel -= (40 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

	if (g_Gangs.GetNumBusinessExtorted() < 70 && GetNumBrothels() >= 3)
	{
		g_Brothels.GetBrothel(2)->m_SecurityLevel -= (70 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

	if (g_Gangs.GetNumBusinessExtorted() < 100 && GetNumBrothels() >= 4)
	{
		g_Brothels.GetBrothel(3)->m_SecurityLevel -= (100 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

	if (g_Gangs.GetNumBusinessExtorted() < 140 && GetNumBrothels() >= 5)
	{
		g_Brothels.GetBrothel(4)->m_SecurityLevel -= (140 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

	if (g_Gangs.GetNumBusinessExtorted() < 170 && GetNumBrothels() >= 6)
	{
		g_Brothels.GetBrothel(5)->m_SecurityLevel -= (170 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

	if (g_Gangs.GetNumBusinessExtorted() < 220 && GetNumBrothels() >= 7)
	{
		g_Brothels.GetBrothel(6)->m_SecurityLevel -= (220 - g_Gangs.GetNumBusinessExtorted()) * 2;
	}

}

// End of turn stuff is here
void cBrothelManager::UpdateGirls(sBrothel* brothel, bool Day0Night1)	// Start_Building_Process_B
{
	// `J` added to allow for easier copy/paste to other buildings
	u_int firstjob = JOB_RESTING;
	u_int lastjob = JOB_WHORESTREETS;
	u_int restjob = JOB_RESTING;
	u_int matronjob = JOB_MATRON;
	bool matron = (GetNumGirlsOnJob(brothel->m_id, matronjob, false) >= 1) ? true : false;
	string MatronMsg = "", MatronWarningMsg = "";
	stringstream ss;

	sGirl* current = brothel->m_Girls;
	sGirl* DeadGirl = 0;
	string summary, msg, girlName;
	int totalPay = 0, totalTips = 0, totalGold = 0;

	int sum = EVENT_SUMMARY;
	u_int sw = 0, psw = 0;
	bool refused = false;
	m_Processing_Shift = Day0Night1;		// WD:	Set processing flag to shift type


	/*
	*	handle any girls training during this shift
	*/
	m_JobManager.do_training(brothel, Day0Night1);
	/*
	*	as for the rest of them...
	*/
	while (current)
	{
		totalPay = totalTips = totalGold = 0;
		refused = false;
		girlName = current->m_Realname;
		sum = EVENT_SUMMARY;

		/*
		*		ONCE DAILY processing
		*		at start of Day Shift
		*/
		if (Day0Night1 == SHIFT_DAY)
		{
			// Back to work
			if (current->m_NightJob == restjob && current->m_DayJob == restjob && current->m_PregCooldown < cfg.pregnancy.cool_down() &&
				g_Girls.GetStat(current, STAT_HEALTH) >= 80 && g_Girls.GetStat(current, STAT_TIREDNESS) <= 20)
			{
				if ((matron || current->m_PrevDayJob == matronjob)					// do we have a director, or was she the director and made herself rest?
					&& current->m_PrevDayJob != 255 && current->m_PrevNightJob != 255)	// 255 = nothing, in other words no previous job stored
				{
					g_Brothels.m_JobManager.HandleSpecialJobs(brothel->m_id, current, current->m_PrevDayJob, current->m_DayJob, false);
					if (current->m_DayJob == current->m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
						current->m_NightJob = current->m_PrevNightJob;
					else
						current->m_NightJob = restjob;
					current->m_PrevDayJob = current->m_PrevNightJob = 255;
					MatronMsg += gettext("The Matron puts ") + girlName + gettext(" back to work.\n");
					current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, EVENT_BACKTOWORK);
					MatronMsg = "";
				}
				else
				{
					MatronWarningMsg += gettext("WARNING ") + girlName + gettext(" is doing nothing!\n");
					current->m_Events.AddMessage(MatronWarningMsg, IMGTYPE_PROFILE, EVENT_WARNING);
					MatronWarningMsg = "";
				}
			}



		}


		/*
		*		EVERY SHIFT processing
		*/

		// Sanity check! Don't process dead girls and check that m_Next points to something
		if (current->health() <= 0)
		{
			if (current->m_Next) { current = current->m_Next; continue; }
			else { current = 0; break; }
		}
		g_Girls.UseItems(current);						// Girl uses items she has
		g_Girls.CalculateAskPrice(current, true);		// Calculate the girls asking price

		/*
		*		JOB PROCESSING
		*/
		u_int sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);

		// do their job
		//	if((sw != JOB_ADVERTISING) && (sw != JOB_WHOREGAMBHALL) && (sw != JOB_WHOREBROTHEL) && (sw != JOB_BARWHORE))		// advertising and whoring are handled earlier.
		// Was not testing for some jobs which were already handled, changed to a switch case statement just for ease of reading, and expansion -PP
		if (sw == JOB_ADVERTISING || sw == JOB_WHOREGAMBHALL || sw == JOB_WHOREBROTHEL ||
			sw == JOB_BARWHORE || sw == JOB_BARMAID || sw == JOB_WAITRESS ||
			sw == JOB_SINGER || sw == JOB_PIANO || sw == JOB_DEALER || sw == JOB_ENTERTAINMENT ||
			sw == JOB_XXXENTERTAINMENT || sw == JOB_SLEAZYBARMAID || sw == JOB_SLEAZYWAITRESS ||
			sw == JOB_BARSTRIPPER || sw == JOB_MASSEUSE || sw == JOB_BROTHELSTRIPPER || sw == JOB_PIANO || sw == JOB_PEEP)
		{
			// these jobs are already done so we skip them
		}
		else refused = m_JobManager.JobFunc[sw](current, brothel, Day0Night1, summary);


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
		/* */if (sw == JOB_RESTING)			summary += girlName + gettext(" was resting so made no money.");
		else if (sw == JOB_MATRON && Day0Night1 == SHIFT_NIGHT)
			summary += girlName + gettext(" continued to help the other girls throughout the night.");

		// `J` temporary -1 until I reflow brothel jobs
		else if (sw == JOB_TRAINING || sw == JOB_ADVERTISING)	sum = -1;
		// WD:	No night shift summary message needed for Torturer job
		else if (sw == JOB_TORTURER && Day0Night1 == SHIFT_NIGHT)	sum = -1;

		// `J` if a slave does a job that is normally paid by you but you don't pay your slaves...
		else if (current->is_slave() && !cfg.initial.slave_pay_outofpocket() &&
#if 0	// `J` until all jobs have this part added to them, use the individual job list instead of this
			m_JobManager.is_job_Paid_Player(sw))
#else
			(
			sw == JOB_BEASTCARER ||
			sw == JOB_CLEANING
			))
#endif
		{
			summary += "\nYou own her and you don't pay your slaves.";
		}
		// WD:	Bad girl did not work. Moved from cJobManager::Preprocessing()
		else if (refused) summary += girlName + gettext(" refused to work so made no money.");
		else if (totalGold > 0)
		{
			ss << girlName << " earned a total of " << totalGold << " gold";
			u_int job = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
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
			ss.str("");
			ss << "ERROR: She has a loss of " << totalGold << " gold\n\n Please report this to the Pink Petal Devloment Team at http://pinkpetal.org";
			summary += ss.str();
			sum = EVENT_DEBUG;
		}
		if (sum >= 0)	// `J` temporary -1 not to show until I reflow brothel jobs
			current->m_Events.AddMessage(summary, IMGTYPE_PROFILE, sum);

		summary = "";






		// Runaway, Depression & Drug checking
		if (runaway_check(brothel, current))
		{
			sGirl* temp = current;
			current = current->m_Next;
			g_Brothels.RemoveGirl(brothel->m_id, temp, false);
			
			temp->run_away();
			continue;
		}

		/*
		*		MATRON CODE START
		*/

		// Lets try to compact multiple messages into one.
		MatronMsg = "";
		MatronWarningMsg = "";

		bool matron = (GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, true) >= 1 || GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, false) >= 1);

		if (g_Girls.GetStat(current, STAT_TIREDNESS) > 80)
		{
			if (matron)
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_RESTING;
					MatronWarningMsg += gettext("Your matron takes ") + girlName + gettext(" off duty to rest due to her tiredness.\n");
				}
				else
				{
					if (g_Dice.percent(70))
					{
						MatronMsg += gettext("Your matron helps ") + girlName + gettext(" to relax.\n");
						g_Girls.UpdateStat(current, STAT_TIREDNESS, -5);
					}
				}
			}
			else
				MatronWarningMsg += gettext("CAUTION! This girl desparatly need rest. Give her some free time\n");
		}

		if (g_Girls.GetStat(current, STAT_HAPPINESS) < 40 && matron && g_Dice.percent(70))
		{
			MatronMsg = gettext("Your matron helps cheer up ") + girlName + gettext(" after she feels sad.\n");
			g_Girls.UpdateStat(current, STAT_HAPPINESS, 5);
		}

		if (g_Girls.GetStat(current, STAT_HEALTH) < 40)
		{
			if (matron)
			{
				if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255)
				{
					current->m_PrevDayJob = current->m_DayJob;
					current->m_PrevNightJob = current->m_NightJob;
					current->m_DayJob = current->m_NightJob = JOB_RESTING;
					MatronWarningMsg += girlName + gettext(" is taken off duty by your matron to rest due to her low health.\n");
				}
				else
				{
					MatronMsg = gettext("Your matron helps heal ") + girlName + gettext(".\n");
					g_Girls.UpdateStat(current, STAT_HEALTH, 5);
				}
			}
			else
			{
				MatronWarningMsg = gettext("DANGER ") + girlName + gettext("'s health is very low!\nShe must rest or she will die!\n");
			}
		}

		if (strcmp(MatronMsg.c_str(), "") != 0)
		{
			current->m_Events.AddMessage(MatronMsg, IMGTYPE_PROFILE, Day0Night1);
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



		// Do item check at the end of the day
		if (Day0Night1 == SHIFT_NIGHT)
		{
			// Myr: Automate the use of a number of different items. See the function itself for more comments.
			//      Enabled or disabled based on config option.
			if (cfg.initial.auto_use_items()) UsePlayersItems(current);

			// update for girls items that are not used up
			do_daily_items(brothel, current);					// `J` added

			// Natural healing, 2% health and 2% tiredness per day
			g_Girls.UpdateStat(current, STAT_HEALTH, 2, false);
			g_Girls.UpdateStat(current, STAT_TIREDNESS, -2, false);
		}

		// Level the girl up if nessessary
		g_Girls.LevelUp(current);



		// Process next girl
		current = current->m_Next;
	}

	// WD: Finished Processing Shift set flag
	m_Processing_Shift = -1;
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
	2. Cure diseases like aids and syphilis.
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
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("Used a mana crystal to restore 25 mana."));
	}
	has = g_Brothels.HasItem("Eldritch Cookie");
	if (g_Girls.GetStat(cur, STAT_MANA) < 20 && has != -1)
	{
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("Used an eldritch cookie to restore 30 mana."));
	}
	has = g_Brothels.HasItem("Mana Potion");
	if (g_Girls.GetStat(cur, STAT_MANA) < 20 && has != -1)
	{
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("Used a mana potion to restore 100 mana."));
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
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && has != -1)
		AutomaticItemUse(cur, has, gettext("She put on a silken dress to better slide with her customers. (Libido up.)"));

	// Minor Ring of the Horndog [30 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Minor Ring of the Horndog");
	if (g_Girls.GetStat(cur, STAT_LIBIDO) <= 10 && g_Girls.HasItem(cur, "Minor Ring of the Horndog") == -1
		&& g_Girls.HasItem(cur, "Ring of the Horndog") == -1 && g_Girls.HasItem(cur, "Organic Lingerie") == -1 && has != -1)
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
	if (g_Girls.GetStat(cur, STAT_BEAUTY) <= 70 && g_Girls.HasItem(cur, "Minor Ring of Beauty") == -1 && has != -1)
		AutomaticItemUse(cur, has, gettext("She put on a minor ring of beauty to cover some flaws."));

	// Rainbow Ring [15 pts] (Piece of equipment)
	has = g_Brothels.HasItem("Rainbow Ring");
	if (g_Girls.GetStat(cur, STAT_BEAUTY) <= 85 && g_Girls.HasItem(cur, "Rainbow Ring") == -1 && has != -1)
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

	// `J` xp can now be above 255 so removing restriction
	has = g_Brothels.HasItem("Nut of Knowledge"); 
	if (has != -1)
	{
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("Used a small nut of knowledge."));
	}
	has = g_Brothels.HasItem("Mango of Knowledge");
	if (has != -1)
	{
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("She ate a mango of knowledge."));
	}
	has = g_Brothels.HasItem("Watermelon of Knowledge");
	if (has != -1)
	{
		if (g_Dice.percent(5))	AutomaticFoodItemUse(cur, has, gettext("She had a watermelon of knowledge for lunch."));
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
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 90 && g_Girls.GetStat(cur, STAT_STRENGTH) <= 90 && g_Girls.HasItem(cur, "Manual of Health") == -1 && has != -1)
		AutomaticSlotlessItemUse(cur, has, gettext("You gave her a manual of health to read."));

	// Free Weights [10 pts] (Piece of equipment, but slotless)
	has = g_Brothels.HasItem("Free Weights");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 90 && g_Girls.GetStat(cur, STAT_STRENGTH) <= 90 && g_Girls.HasItem(cur, "Free Weights") == -1 && has != -1)
		AutomaticSlotlessItemUse(cur, has, gettext("You gave her free weights to work with."));

	// Stripper Pole [5 pts] (Piece of equipment, but slotless)
	has = g_Brothels.HasItem("Stripper Pole");
	if (g_Girls.GetStat(cur, STAT_CONSTITUTION) <= 95 && g_Girls.GetStat(cur, STAT_STRENGTH) <= 95 && g_Girls.HasItem(cur, "Stripper Pole") == -1 && has != -1)
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
	if (!g_Girls.HasTrait(cur, "Fleet of Foot") && g_Girls.HasItem(cur, "Glass Shoes") == -1 && has != -1)
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

//	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
//	if (g_Girls.HasTrait(cur, "Malformed") && has != -1)
//		AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to cure her malformities."));

	// Tsundere & yandere

	has = g_Brothels.HasItem("Attitude Reajustor");
	if ((g_Girls.HasTrait(cur, "Yandere") || g_Girls.HasTrait(cur, "Tsundere")) && has != -1)
		AutomaticFoodItemUse(cur, has, gettext("You had her take an attitude reajustor pill."));

	// Eyes

	has = g_Brothels.HasItem("Eye Replacement Candy");
	if ((g_Girls.HasTrait(cur, "One Eye") || g_Girls.HasTrait(cur, "Eye Patch")) && has != -1)
		AutomaticFoodItemUse(cur, has, gettext("Used an eye replacement candy to restore her eye."));

	// Last ditch eye check.  Use the big guns if you don't have anything else.
//	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
//	if ((g_Girls.HasTrait(cur, "One Eye") || g_Girls.HasTrait(cur, "Eye Patch")) && has != -1)
//		AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to restore her eye."));

	// Scars - start with the least powerful cures and work up
	has = g_Brothels.HasItem("Oil of Lesser Scar Removing");
	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars")) && has != -1)
		AutomaticFoodItemUse(cur, has, gettext("Used an oil of lesser scar removal to remove work-related damage."));

	has = g_Brothels.HasItem("Oil of Greater Scar Removing");
	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars") || g_Girls.HasTrait(cur, "Horrific Scars")) && has != -1)
		AutomaticFoodItemUse(cur, has, gettext("Used an oil of greater scar removal to remove her scars."));

//	has = g_Brothels.HasItem("Elixir of Ultimate Regeneration");
//	if ((g_Girls.HasTrait(cur, "Small Scars") || g_Girls.HasTrait(cur, "Cool Scars") || g_Girls.HasTrait(cur, "Horrific Scars")) && has != -1)
//		AutomaticFoodItemUse(cur, has, gettext("Used an elixir of ultimate regeneration to remove her scars."));

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
		if (!g_Girls.HasTrait(cur, "Good Kisser"))			PolishCount++;
		if (!g_Girls.HasTrait(cur, "Great Figure"))			PolishCount++;
		if (!g_Girls.HasTrait(cur, "Great Arse"))			PolishCount++;
		if (!g_Girls.HasTrait(cur, "Long Legs"))			PolishCount++;
		if (!g_Girls.HasTrait(cur, "Puffy Nipples"))		PolishCount++;

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
	if (rival)
	{
		long total = m_BribeRate;
		total += TOWN_OFFICIALSWAGES;	// this is the amount the government controls

		while (rival)	// get the total for all bribes
		{
			total += rival->m_BribeRate;
			rival = rival->m_Next;
		}

		rival = GetRivals();
		while (rival)	// get the total for all bribes
		{
			if (rival->m_BribeRate > 0 && total != 0)
				rival->m_Influence = (int)(((float)rival->m_BribeRate / (float)total)*100.0f);
			else
				rival->m_Influence = 0;
			rival = rival->m_Next;
		}

		if (m_BribeRate != 0 && total != 0)
			m_Influence = (int)(((float)m_BribeRate / (float)total)*100.0f);
		else
			m_Influence = 0;
	}
	else
	{
		if (m_BribeRate <= 0)
			m_Influence = 0;
		else
			m_Influence = (int)(((float)m_BribeRate / (float)((float)TOWN_OFFICIALSWAGES + (float)m_BribeRate))*100.0f);
	}
}

void cBrothelManager::do_tax()
{
	double taxRate = cfg.tax.rate();	// normal tax rate is 6%
	if (m_Influence > 0)	// can you influence it lower
	{
		int lowerBy = m_Influence / 20;
		float amount = (float)(lowerBy / 100);
		taxRate -= amount;
		if (taxRate < cfg.tax.minimum())
			taxRate = cfg.tax.minimum();
	}
	// check for money laundering and apply tax
	int earnings = g_Gold.total_earned();

	if (earnings <= 0)
	{
		g_MessageQue.AddToQue(gettext("You didn't earn any money so didn't get taxed."), COLOR_BLUE);
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
	if (tax <= 0)
	{
		g_MessageQue.AddToQue(gettext("Thanks to a clever accountant, none of your income turns out to be taxable"), COLOR_BLUE);
		return;
	}
	g_Gold.tax(tax);
	stringstream ss;
	/*
	*	Let's report the laundering, at least.
	*	Otherwise, it just makes the tax rate wobble a bit
	*/
	ss << "You were taxed " << tax << " gold. You managed to launder " << laundry << " through various local businesses.";
	g_MessageQue.AddToQue(ss.str(), COLOR_BLUE);
}

bool is_she_cleaning(sGirl *girl)
{
	if (girl->m_DayJob == JOB_CLEANING || girl->m_NightJob == JOB_CLEANING ||
		girl->m_DayJob == JOB_CLEANARENA || girl->m_NightJob == JOB_CLEANARENA ||
		girl->m_DayJob == JOB_STAGEHAND || girl->m_NightJob == JOB_STAGEHAND ||
		girl->m_DayJob == JOB_JANITOR || girl->m_NightJob == JOB_JANITOR ||
		girl->m_DayJob == JOB_CLEANCENTRE || girl->m_NightJob == JOB_CLEANCENTRE ||
		girl->m_DayJob == JOB_FARMHAND || girl->m_NightJob == JOB_FARMHAND ||
		girl->m_DayJob == JOB_CLEANHOUSE || girl->m_NightJob == JOB_CLEANHOUSE)
	{
		return true;
	}
	return false;
}
bool is_she_resting(sGirl *girl)
{
	if ((girl->m_DayJob == JOB_FILMFREETIME	&& girl->m_NightJob == JOB_FILMFREETIME) ||
		(girl->m_DayJob == JOB_ARENAREST	&& girl->m_NightJob == JOB_ARENAREST) ||
		(girl->m_DayJob == JOB_CENTREREST	&& girl->m_NightJob == JOB_CENTREREST) ||
		(girl->m_DayJob == JOB_CLINICREST	&& girl->m_NightJob == JOB_CLINICREST) ||
		(girl->m_DayJob == JOB_HOUSEREST	&& girl->m_NightJob == JOB_HOUSEREST) ||
		(girl->m_DayJob == JOB_FARMREST		&& girl->m_NightJob == JOB_FARMREST) ||
		(girl->m_DayJob == JOB_RESTING		&& girl->m_NightJob == JOB_RESTING))
	{
		return true;
	}
	return false;
}
bool is_she_stripping(sGirl *girl)
{
	if (girl->m_DayJob == JOB_BARSTRIPPER || girl->m_NightJob == JOB_BARSTRIPPER ||
		girl->m_DayJob == JOB_BROTHELSTRIPPER || girl->m_NightJob == JOB_BROTHELSTRIPPER ||
		girl->m_DayJob == JOB_PEEP || girl->m_NightJob == JOB_PEEP)
	{
		return true;
	}
	return false;
}


void cBrothelManager::do_daily_items(sBrothel *brothel, sGirl *girl) // `J` added
{
	if (girl->m_NumInventory < 1) return;	// no items so skip it

	stringstream ss;
	string girlName = girl->m_Realname;
	int mast = false;
	int strip = false;
	int combat = false;
	int formal = false;
	int swim = false;
	int cook = false;
	int maid = false;

	// `J` zzzzzz - This list needs to be sorted into groups

	// unrestricted use items
#if 1
	if (g_Girls.HasItemJ(girl, "Android, Assistance") != -1 && g_Dice.percent(50))
	{
		ss << "Her Assistance Android swept up and took out the trash for her.\n\n";
		brothel->m_Filthiness -= 5;
	}
	if (g_Girls.HasItemJ(girl, "Room Decorations") != -1 && g_Dice.percent(3))
	{
		ss << "She looks around at her Room Decorations and smiles, she really likes that her room is a little better then most the other girls.\n\n";
		girl->happiness(5);
	}
	if (g_Girls.HasItemJ(girl, "Journal") != -1 && g_Dice.percent(15))
	{
		if (g_Girls.HasTrait(girl, "Nerd") && g_Dice.percent(50))
		{
			ss << "She decide to write on her novel some today.\n\n";
			girl->happiness(g_Dice % 2);
			girl->intelligence(g_Dice % 2);
		}
		else if (g_Girls.HasTrait(girl, "Bimbo") && g_Dice.percent(50))
		{
			ss << "She doodled silly pictures in her journal.\n\n";
			girl->happiness(g_Dice % 3);
		}
		else
		{
			string thoughts = "";
			switch (g_Dice % 20)
			{
			case 0:		girl->happiness(1);					thoughts = " happy";		break;
			case 1:		girl->happiness(-1);				thoughts = " sad";			break;
			case 2:		girl->happiness(1 + g_Dice % 3);	thoughts = " fun";			break;
			case 3:		girl->intelligence(1);				thoughts = " interesting";	break;
			case 4:		girl->spirit(1);					thoughts = " positive";		break;
			case 5:		girl->spirit(-1);					thoughts = " negative";		break;
			case 6:		girl->obedience(1);					thoughts = " helpful";		break;
			case 7:		girl->obedience(-1);				thoughts = " anoying";		break;
			case 8:		girl->pclove(1);					thoughts = " loving";		break;
			case 9:		girl->pclove(-1);					thoughts = " unloving";		break;
			case 10:	girl->pchate(1);					thoughts = " hateful";		break;
			case 11:	girl->pchate(-1);					thoughts = " carefree";		break;
			case 12:	girl->pcfear(1);					thoughts = " fearful";		break;
			case 13:	girl->pcfear(-1);					thoughts = " fearless";		break;
			case 14:	girl->dignity(1);					thoughts = " proper";		break;
			case 15:	girl->dignity(-1);					thoughts = " slutty";		break;
			case 16:	girl->libido(-1);					thoughts = " tame";			break;
			case 17:	girl->libido(1);					thoughts = " sexy";			break;
			default:	break;
			}
			ss << "She used her Journal to write some of her" << thoughts << " thoughts down today.\n\n";

		}
	}

	// Dream Orbs
#if 1
	if (g_Girls.HasItemJ(girl, "Nightmare Orb") != -1 && g_Dice.percent(50))
	{
		if (girl->pclove() > girl->pcfear() && g_Dice.percent(girl->pclove()))
		{
			if (g_Dice.percent(50))	ss << girlName << " comes to you and tells you she had a scary dream about you.\n\n";
			girl->pcfear(g_Dice % 2);
			girl->happiness(-1);
			girl->tiredness(1);
		}
		else if (girl->has_trait("Masochist") || girl->has_trait("Twisted"))	// she liked it
		{
			girl->pcfear(g_Dice % 2);
			girl->happiness(g_Dice % 3);
			girl->tiredness(g_Dice % 2);
		}
		else	// everyone else
		{
			girl->pcfear(g_Dice % 3);
			girl->happiness(-(g_Dice % 3));
			girl->tiredness(g_Dice % 4);
		}
	}
	if (g_Girls.HasItemJ(girl, "Lovers Orb") != -1 && g_Dice.percent(50))
	{
		if (girl->pclove() > girl->pchate() && g_Dice.percent(girl->pclove()))
		{
			girl->pclove(1 + g_Dice % 3);
			girl->pcfear(-(g_Dice % 3));
			girl->pchate(-(g_Dice % 3));
			girl->happiness(3 + g_Dice % 3);
			girl->tiredness(1 + g_Dice % 3);
			girl->npclove(-(1 + g_Dice % 3));
			if (g_Dice.percent(50))
			{
				ss << girlName << " comes to you and tells you she had a sexy dream about you.";
				if (girl->has_trait("Lesbian") && The_Player->Gender() >= GENDER_HERMFULL && g_Dice.percent(girl->pclove() / 10))
				{
					g_Girls.RemoveTrait(girl, "Lesbian");
					g_Girls.AddTrait(girl, "Bisexual");
					ss << "  \"Normally I don't like men but for you I'll make an exception.\"";
				}
				if (girl->has_trait("Straight") && The_Player->Gender() <= GENDER_FUTAFULL && g_Dice.percent(girl->pclove() / 10))
				{
					g_Girls.RemoveTrait(girl, "Straight");
					g_Girls.AddTrait(girl, "Bisexual");
					ss << "  \"Normally I don't like women but for you I'll make an exception.\"";
				}
				ss << "\n\n";
			}
		}
		else	// everyone else
		{
			girl->pclove(g_Dice % 3);
			girl->pcfear(-(g_Dice % 2));
			girl->pchate(-(g_Dice % 2));
			girl->happiness(1 + g_Dice % 3);
			girl->tiredness(1 + g_Dice % 3);
		}
	}
	if (g_Girls.HasItemJ(girl, "Happy Orb") != -1 && g_Dice.percent(50))
	{
		if (girl->pclove() > girl->pcfear() && g_Dice.percent(girl->pclove()))
		{
			if (g_Dice.percent(50))
				ss << girlName << " comes to you and tells you she had a happy dream about you.\n\n";
			girl->happiness(4 + g_Dice % 5);
			girl->pclove(g_Dice % 2);
			girl->pcfear(-(g_Dice % 2));
			girl->pchate(-(g_Dice % 2));
		}
		else	// everyone else
		{
			girl->happiness(3 + g_Dice % 3);
		}
	}
#endif		


#endif

	// Items ONLY useable if resting
#if 1
	if (is_she_resting(girl))
	{
		if (g_Girls.HasItemJ(girl, "Free Weights") != -1 && g_Dice.percent(15))
		{
			ss << girl->m_Realname << " decide to spend her time working out with her Free Weights.\n\n";
			if (g_Dice.percent(5))	g_Girls.UpdateStat(girl, STAT_BEAUTY, 1);		// working out will help her look better
			if (g_Dice.percent(10))	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 1);	// working out will make her healthier
			if (g_Dice.percent(50))	g_Girls.UpdateStat(girl, STAT_STRENGTH, 1);		// working out will make her stronger
		}

		// Books and reading materials
#if 1
		// first list all books to see if she has any
		if (g_Girls.HasItemJ(girl, "Manual of Sex") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of Bondage") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of Two Roses") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of Arms") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of the Dancer") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of Magic") != -1 ||
			g_Girls.HasItemJ(girl, "Manual of Health") != -1 ||

			g_Girls.HasItemJ(girl, "Library Card") != -1)
		{
			int numbooks = girl->intelligence() / 30;	// how many books can she read?
			if (g_Girls.HasTrait(girl, "Blind"))				numbooks = 1;
			else
			{
				if (g_Girls.HasTrait(girl, "Nerd"))				numbooks += 1;
				if (g_Girls.HasTrait(girl, "Quick Learner"))	numbooks += 1;
				if (g_Girls.HasTrait(girl, "Slow Learner"))		numbooks -= 2;
				if (g_Girls.HasTrait(girl, "Bimbo"))			numbooks -= 1;
			}
			if (numbooks < 1)				numbooks = 1;

			// then see if she wants to read any
			if (g_Girls.HasItemJ(girl, "Manual of Sex") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Sex.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of Bondage") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Bondage.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of Two Roses") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Two Roses.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of Arms") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Arms.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_COMBAT, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of the Dancer") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of the Dacer.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_STRIP, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of Magic") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Magic.\n\n";
				g_Girls.UpdateSkill(girl, SKILL_MAGIC, 2);
				numbooks--;
			}
			if (g_Girls.HasItemJ(girl, "Manual of Health") != -1 && g_Dice.percent(5) && numbooks > 0)
			{
				ss << "Spent her time off reading her Manual of Health.\n\n";
				g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 1);
				numbooks--;
			}

			// She may go to the library if she runs out of books to read
			if (g_Girls.HasItemJ(girl, "Library Card") != -1 && g_Dice.percent(15) && numbooks > 0)
			{
				if (g_Girls.HasTrait(girl, "Nymphomaniac"))
				{
					ss << "She spent the day at the Library looking at porn making her become horny.\n\n";
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 15);
				}
				else
				{
					ss << "She spent her free time at the library reading.";
					if (g_Dice.percent(5))		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
					if (g_Dice.percent(5))
					{
						int upskill = g_Dice%NUM_SKILLS;
						int upskillg = g_Dice % 4 - 1;
						ss << " She found a book on " << sGirl::skill_names[upskill];
						if (upskillg > 0)
						{
							ss << " and gained " << upskillg << " points in it.";
							g_Girls.UpdateSkill(girl, upskill, upskillg);
						}
						else ss << "but didn't find it very useful.";
					}
					ss << "\n\n";
				}
			}
		}

















#endif	// End Books and reading materials

	}
#endif

	////////////////////
	// Unsorted items //
	////////////////////


	if (g_Girls.HasItemJ(girl, "Television Set") != -1)
	{
		if (is_she_resting(girl))
		{
			ss << girl->m_Realname << " spent most of her day lounging in front of her Television Set.\n\n";
			girl->tiredness(-5);
			if (g_Dice.percent(5))		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
		}
		else
		{
			ss << "At the end of her long day, " << girlName << " flopped down in front of her Television Set and relaxed.\n\n";
			girl->tiredness(-3);
		}
	}
	if (g_Girls.HasItemJ(girl, "Appreciation Trophy") != -1 && is_she_cleaning(girl) && g_Dice.percent(5) && girl->pclove() > girl->pchate() - 10)
	{
		ss << "While cleaning, " << girlName << " came across her Appreciation Trophy and smiled.\n\n";
		girl->pclove(1);
	}
	if (g_Girls.HasItemJ(girl, "Art Easel") != -1 && g_Dice.percent(2))
	{
		int sale = g_Dice % 30 + 1;
		ss << girl->m_Realname << " managed to sell one of her paintings for " << sale << " gold.\n\n";
		girl->m_Money += sale;
		girl->happiness(sale / 5);
		girl->fame(1);
	}
	if (g_Girls.HasItemJ(girl, "Compelling Dildo") != -1)
	{
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 65 && is_she_resting(girl))
		{
			ss << girlName << "'s lust got the better of her and she spent the day using her Compelling Dildo.\n\n";
			g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
			mast = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Pet Spider") != -1 && g_Dice.percent(15))
	{
		if (g_Girls.HasTrait(girl, "Meek"))
		{
			ss << girlName << "'s Meek nature makes her cover her Pet Spiders cage so it doesn't scare her.\n\n";
		}
		else if (g_Girls.HasTrait(girl, "Aggressive"))
		{
			ss << girlName << " throws in some food to her Pet Spider and smiles while she watchs it kill its prey.\n\n";
		}
		else
		{
			ss << girlName << " plays with her pet spider.\n\n";
		}
	}
	if (g_Girls.HasItemJ(girl, "Chrono Bed") != -1)
	{
		ss << "Thanks to her Chrono Bed she got a great nights sleep and woke up feeling wonderful.\n\n";
		girl->health(50);
		girl->tiredness(-50);
	}
	else if (g_Girls.HasItemJ(girl, "Rejuvenation Bed") != -1)
	{
		ss << "Thanks to her Rejuvenation Bed she got a great nights sleep and woke up feeling better.\n\n";
		girl->health(5);
		girl->tiredness(-5);
	}
	if (g_Girls.HasItemJ(girl, "Claptrap") != -1 && g_Dice.percent(10))
	{
		ss << "Thanks to Claptrap's sense of humor she is a better mood.\n\n";
		girl->happiness(5);
	}
	if (g_Girls.HasItemJ(girl, "The Realm of Darthon") != -1 && g_Dice.percent(2))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time playing The Realm of Darthon with some of the other girls.\n\n";
			girl->happiness(5);
		}
	}
	if (g_Girls.HasItemJ(girl, "Stripper Pole") != -1 && g_Dice.percent(10))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off practicing on her Stripper Pole.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_STRIP, 2);
			strip = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Android, Combat MK I") != -1 && g_Dice.percent(5))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off training with her Android, Combat MK I.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, 1);
			combat = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Android, Combat MK II") != -1 && g_Dice.percent(10))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off training with her Android, Combat MK II.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, 2);
			combat = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Compelling Buttplug") != -1 && g_Dice.percent(10))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off with her Compelling Buttplug in.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
		}
	}
	if (g_Girls.HasItemJ(girl, "Computer") != -1 && g_Dice.percent(15) && is_she_resting(girl))
	{
		if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		{
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 65)
			{
				ss << girlName << "'s lust got the better of her while she was on the her Computer looking at porn.\n\n";
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
				mast = true;
			}
			else
			{
				ss << "She spent the day on her Computer looking at porn making her become horny.\n\n";
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 15);
			}
		}
		else
		{
			ss << "She spent her free time playing on her Computer.\n\n";
			if (g_Dice.percent(5))		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
		}
	}
	if ((g_Girls.HasItemJ(girl, "Cat") != -1 || g_Girls.HasItemJ(girl, "Black Cat") != -1) && g_Dice.percent(10))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off with her pet Cat.\n\n";
			girl->happiness(5);
		}
	}
	if (g_Girls.HasItemJ(girl, "Guard Dog") != -1 && g_Dice.percent(15))
	{
		if (g_Girls.HasTrait(girl, "Meek"))
		{
			ss << girlName << "'s Meek nature makes her scared of her pet Guard Dog.\n\n";
		}
		else if (g_Girls.HasTrait(girl, "Aggressive"))
		{
			ss << girlName << " seeks her Guard Dog on some random patrons and laughs while they run scared.\n\n";
		}
		else
		{
			ss << girlName << " plays with her pet Guard Dog.\n\n";
		}
	}
	if (g_Girls.HasItemJ(girl, "Noble Gown") != -1)
	{
		ss << girlName << " went around wearing her Noble Gown today making her look quite formal.\n\n";
		formal = true;
	}
	if (g_Girls.HasItemJ(girl, "Fishnet Stocking") != -1)
	{
		ss << girlName << " went around wearing her Fishnet Stocking today making her sexy even if it did make her feel a litte trashy.\n\n";
	}
	if (g_Dice.percent(15) && (g_Girls.HasItemJ(girl, "White String Bikini") != -1 || g_Girls.HasItemJ(girl, "Black String Bikini") != -1))
	{
		int num = 0;
		swim = true;
		if (g_Girls.HasItemJ(girl, "White String Bikini") != -1 && g_Girls.HasItemJ(girl, "Black String Bikini") != -1)
		{
			num = g_Dice % 2;
			if (num == 1) num = 2;
		}
		else if (g_Girls.HasItemJ(girl, "White String Bikini") != -1) num = 0;
		else	num = 2;
		if (g_Dice.percent(50) && is_she_resting(girl))
		{
			num++;
			girl->happiness(5);
		}

		switch (num)
		{
		case 0:
			ss << girlName << " went around wearing her White String Bikini today.\n\n";
			break;
		case 1:
			ss << "Spent her time off at the local pool in her White String Bikini.\n\n";
			break;
		case 2:
			ss << girlName << " went around wearing her Black String Bikini today.\n\n";
			break;
		case 3:
			ss << "Spent her time off at the local pool in her Black String Bikini.\n\n";
			break;
		default: break;
		}
	}
	if (g_Girls.HasItemJ(girl, "Apron") != -1 && g_Dice.percent(10))
	{
		if (is_she_resting(girl))
		{
			ss << "She put on her Apron and cooked a meal for some of the girls.\n\n";
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKCOOKING, 1);
			girl->happiness(5);
			cook = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Maid Uniform") != -1 && g_Dice.percent(5))
	{
		if (is_she_resting(girl))
		{
			ss << "She put on her Maid Uniform and cleaned up.\n\n";
			brothel->m_Filthiness -= 5;
			maid = true;
		}
	}
	if (g_Girls.HasItemJ(girl, "Disguised Slave Band") != -1)
	{
		ss << girlName << " went around wearing her Disguised Slave Band having no idea of what it really does to her.\n\n";
	}
	if (g_Girls.HasItemJ(girl, "Anger Management Tapes") != -1 && g_Dice.percent(2))
	{
		if (is_she_resting(girl))
		{
			ss << "Spent her time off listen to her Anger Management Tapes.\n\n";
			g_Girls.UpdateStat(girl, STAT_SPIRIT, -2);
		}
	}
	if (g_Girls.HasItemJ(girl, "Rainbow Underwear") != -1)
	{
		if (is_she_stripping(girl))  //not sure this will work like i want it to might be that i need to added them to the jobs
		{
			ss << girlName << " stripped down to reveal her Rainbow Underwear to the approval of the patrons watching her.\n\n";
			brothel->m_Happiness += 5;
		}
	}
	if (g_Girls.HasItemJ(girl, "Short Sword") != -1 && g_Dice.percent(5))
	{
		if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) > 65 && is_she_resting(girl))
		{
			ss << girlName << " sharpened her Short Sword making it more ready for combat.\n\n";
			g_Girls.UpdateSkillTemp(girl, SKILL_COMBAT, 2);
		}
		else
		{
			ss << girlName << " tried to sharpen her Short Sword but doesn't have the brains to do it right.\n\n";
			g_Girls.UpdateSkillTemp(girl, SKILL_COMBAT, -2);
		}
	}

	if (ss.str().length()>0)		// only pass the summary if she has any of the items listed
	{
		int imagetype = IMGTYPE_PROFILE;
		/* */if (mast)		imagetype = IMGTYPE_MAST;
		else if (strip)		imagetype = IMGTYPE_STRIP;
		else if (combat)	imagetype = IMGTYPE_COMBAT;
		else if (formal)	imagetype = IMGTYPE_FORMAL;
		else if (swim)		imagetype = IMGTYPE_SWIM;
		else if (cook)		imagetype = IMGTYPE_COOK;
		else if (maid)		imagetype = IMGTYPE_MAID;

		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);

	}


}

// add the girls accommodation and food costs to the upkeep
void cBrothelManager::do_food_and_digs(sBrothel *brothel, sGirl *girl)
{
	// `J` new code for .06.01.18
#if 1
	stringstream ss;

	// Gold per accommodation level
	int gold = (girl->is_slave() ? 5 : 20) * (girl->m_AccLevel + 1);
	brothel->m_Finance.girl_support(gold);

	int preferredaccom = g_Girls.PreferredAccom(girl);	// what she wants/expects
	int mod = girl->m_AccLevel - preferredaccom;

	/*   if (acc == 0)	return "Bare Bones";
	else if (acc == 1)	return "Very Poor";
	else if (acc == 2)	return "Poor";
	else if (acc == 3)	return "Adequate";
	else if (acc == 4)	return "Comfortable";
	else if (acc == 5)	return "Nice";
	else if (acc == 6)	return "Good";
	else if (acc == 7)	return "Great";
	else if (acc == 8)	return "Wonderful";
	else if (acc == 9)	return "High Class";
	*/

	CLog l;
	if (cfg.debug.log_extradetails())
	{
		string name = girl->m_Realname;
		while (name.length() < 30) name += " ";
		l.ss() << "" << name << " | P_" << preferredaccom << "-A_" << girl->m_AccLevel << "=M_" << mod << (mod > 0 ? " " : "");
	}

	int hapA, hapB, lovA, lovB, hatA, hatB, feaA, feaB;		// A should always be lower than B
	if (mod < -9) mod = -9;	if (mod > 9) mod = 9;
	switch (mod)	// happiness, love, hate, fear
	{
	case -9:	hapA = -24;	hapB = -7;	lovA = -14;	lovB = -3;	hatA = 6;	hatB = 22;	feaA = 5;	feaB = 12;	break;
	case -8:	hapA = -19;	hapB = -6;	lovA = -11;	lovB = -3;	hatA = 5;	hatB = 18;	feaA = 4;	feaB = 9;	break;
	case -7:	hapA = -16;	hapB = -5;	lovA = -9;	lovB = -3;	hatA = 4;	hatB = 14;	feaA = 3;	feaB = 7;	break;
	case -6:	hapA = -13;	hapB = -4;	lovA = -7;	lovB = -2;	hatA = 4;	hatB = 10;	feaA = 2;	feaB = 5;	break;
	case -5:	hapA = -10;	hapB = -3;	lovA = -6;	lovB = -2;	hatA = 3;	hatB = 7;	feaA = 1;	feaB = 4;	break;
	case -4:	hapA = -8;	hapB = -2;	lovA = -5;	lovB = -1;	hatA = 2;	hatB = 5;	feaA = 0;	feaB = 3;	break;
	case -3:	hapA = -6;	hapB = -1;	lovA = -4;	lovB = 0;	hatA = 1;	hatB = 4;	feaA = 0;	feaB = 2;	break;
	case -2:	hapA = -4;	hapB = 0;	lovA = -3;	lovB = 0;	hatA = 0;	hatB = 3;	feaA = 0;	feaB = 1;	break;
	case -1:	hapA = -2;	hapB = 1;	lovA = -2;	lovB = 1;	hatA = -1;	hatB = 2;	feaA = 0;	feaB = 0;	break;
	case 0:		hapA = -1;	hapB = 3;	lovA = -1;	lovB = 2;	hatA = -1;	hatB = 1;	feaA = 0;	feaB = 0;	break;
	case 1:		hapA = 0;	hapB = 5;	lovA = -1;	lovB = 3;	hatA = -1;	hatB = 0;	feaA = 0;	feaB = 0;	break;
	case 2:		hapA = 1;	hapB = 8;	lovA = 0;	lovB = 3;	hatA = -3;	hatB = 0;	feaA = 0;	feaB = 0;	break;
	case 3:		hapA = 2;	hapB = 11;	lovA = 0;	lovB = 4;	hatA = -5;	hatB = -1;	feaA = -1;	feaB = 0;	break;
	case 4:		hapA = 3;	hapB = 14;	lovA = 1;	lovB = 4;	hatA = -6;	hatB = -1;	feaA = -1;	feaB = 0;	break;
	case 5:		hapA = 4;	hapB = 16;	lovA = 1;	lovB = 5;	hatA = -7;	hatB = -1;	feaA = -1;	feaB = 0;	break;
	case 6:		hapA = 5;	hapB = 18;	lovA = 2;	lovB = 5;	hatA = -7;	hatB = -2;	feaA = -2;	feaB = 0;	break;
	case 7:		hapA = 5;	hapB = 19;	lovA = 2;	lovB = 6;	hatA = -8;	hatB = -2;	feaA = -2;	feaB = 0;	break;
	case 8:		hapA = 5;	hapB = 20;	lovA = 2;	lovB = 7;	hatA = -9;	hatB = -3;	feaA = -3;	feaB = 0;	break;
	case 9:		hapA = 5;	hapB = 21;	lovA = 2;	lovB = 8;	hatA = -10;	hatB = -3;	feaA = -3;	feaB = 0;	break;
	default: break;
	}
	if (cfg.debug.log_extradetails())
	{
		l.ss() << "\t|";
	}

	if (girl->happiness() < 20 - mod)			// if she is unhappy, her mood will go down
	{
		if (cfg.debug.log_extradetails())
		{
			l.ss() << "a";
		}
		/* */if (mod < -6){ hapA -= 7;	hapB -= 3;	lovA -= 4;	lovB -= 1;	hatA += 2;	hatB += 5;	feaA += 2;	feaB += 5; }
		else if (mod < -3){ hapA -= 5;	hapB -= 2;	lovA -= 2;	lovB -= 1;	hatA += 1;	hatB += 3;	feaA += 1;	feaB += 3; }
		else if (mod < 0){ hapA -= 3;	hapB -= 1;	lovA -= 1;	lovB -= 0;	hatA += 0;	hatB += 2;	feaA += 0;	feaB += 2; }
		else if (mod < 1){ hapA -= 2;	hapB -= 0;	lovA -= 1;	lovB -= 0;	hatA += 0;	hatB += 1;	feaA += 0;	feaB += 1; }
		else if (mod < 4){ hapA -= 2;	hapB -= 0;	lovA -= 1;	lovB -= 0;	hatA += 0;	hatB += 1;	feaA -= 1;	feaB += 1; }
		else if (mod < 7){ hapA -= 1;	hapB -= 0;	lovA -= 1;	lovB -= 0;	hatA += 0;	hatB += 0;	feaA -= 1;	feaB += 0; }
	}
	else if (!g_Dice.percent(girl->happiness()))	// if she is not happy, her mood may go up or down
	{
		if (cfg.debug.log_extradetails())
		{
			l.ss() << "b";
		}
		/* */if (mod < -6){ hapA -= 3;	hapB += 1;	lovA -= 3;	lovB += 0;	hatA -= 0;	hatB += 4;	feaA -= 2;	feaB += 3; }
		else if (mod < -3){ hapA -= 2;	hapB += 1;	lovA -= 2;	lovB += 0;	hatA -= 0;	hatB += 3;	feaA -= 1;	feaB += 2; }
		else if (mod < 0){ hapA -= 1;	hapB += 2;	lovA -= 1;	lovB += 1;	hatA -= 1;	hatB += 2;	feaA -= 1;	feaB += 2; }
		else if (mod < 1){ hapA -= 1;	hapB += 2;	lovA -= 1;	lovB += 1;	hatA -= 1;	hatB += 1;	feaA -= 1;	feaB += 1; }
		else if (mod < 4){ hapA += 0;	hapB += 2;	lovA -= 0;	lovB += 1;	hatA -= 1;	hatB += 1;	feaA -= 1;	feaB += 0; }
		else if (mod < 7){ hapA += 0;	hapB += 3;	lovA += 0;	lovB += 1;	hatA -= 1;	hatB -= 0;	feaA -= 0;	feaB += 0; }
	}
	else										// otherwise her mood can go up
	{
		if (cfg.debug.log_extradetails())
		{
			l.ss() << "c";
		}
		/* */if (mod < -6){ hapA -= 1;	hapB += 2;	lovA -= 1;	lovB += 1;	hatA -= 1;	hatB -= 1;	feaA -= 1;	feaB += 1; }
		else if (mod < -3){ hapA += 0;	hapB += 2;	lovA += 0;	lovB += 1;	hatA -= 2;	hatB -= 0;	feaA -= 2;	feaB -= 0; }
		else if (mod < 0){ hapA += 0;	hapB += 3;	lovA += 0;	lovB += 1;	hatA -= 2;	hatB -= 0;	feaA -= 2;	feaB -= 0; }
		else if (mod < 1){ hapA += 0;	hapB += 5;	lovA += 0;	lovB += 1;	hatA -= 2;	hatB -= 1;	feaA -= 2;	feaB -= 0; }
		else if (mod < 4){ hapA += 1;	hapB += 7;	lovA += 0;	lovB += 2;	hatA -= 3;	hatB -= 1;	feaA -= 3;	feaB -= 0; }
		else if (mod < 7){ hapA += 2;	hapB += 8;	lovA += 1;	lovB += 3;	hatA -= 4;	hatB -= 1;	feaA -= 3;	feaB -= 1; }
	}
	if (girl->health() < 25)					// if she is injured she may be scared because of her surroundings
	{
		if (cfg.debug.log_extradetails())
		{
			l.ss() << "d";
		}
		/* */if (mod < -6){ hapA -= 6;	hapB -= 2;	lovA -= 4;	lovB -= 1;	hatA += 3;	hatB += 4;	feaA += 2;	feaB += 4; }
		else if (mod < -3){ hapA -= 4;	hapB -= 1;	lovA -= 3;	lovB -= 1;	hatA += 2;	hatB += 3;	feaA += 1;	feaB += 3; }
		else if (mod < 0){	hapA -= 2;	hapB -= 1;	lovA -= 1;	lovB += 0;	hatA += 1;	hatB += 2;	feaA += 0;	feaB += 2; }
		else if (mod < 1){	hapA -= 1;	hapB += 1;	lovA -= 0;	lovB += 0;	hatA -= 0;	hatB += 1;	feaA -= 1;	feaB += 1; }
		else if (mod < 4){	hapA += 0;	hapB += 4;	lovA += 0;	lovB += 1;	hatA -= 1;	hatB += 0;	feaA -= 2;	feaB += 1; }
		else if (mod < 7){	hapA += 2;	hapB += 8;	lovA += 1;	lovB += 1;	hatA -= 1;	hatB += 0;	feaA -= 3;	feaB += 0; }
	}
	else if (cfg.debug.log_extradetails())
	{
		l.ss() << " ";
	}

	if (girl->is_slave())						// slaves get half as much from their mods
	{
		if (cfg.debug.log_extradetails())
		{
			l.ss() << "e";
		}
		hapA /= 2;	hapB /= 2;	lovA /= 2;	lovB /= 2;	hatA /= 2;	hatB /= 2;	feaA /= 2;	feaB /= 2;
	}
	else if (cfg.debug.log_extradetails())
	{
		l.ss() << " ";
	}

	int hap = g_Dice.bell(hapA, hapB);
	int lov = g_Dice.bell(lovA, lovB);
	int hat = g_Dice.bell(hatA, hatB);
	int fea = g_Dice.bell(feaA, feaB);

	if (cfg.debug.log_extradetails())
	{
		l.ss() << "\t| happy:\t" << hapA << "\t" << hapB << "\t=" << hap
			<< "\t| love :\t" << lovA << "\t" << lovB << "\t=" << lov
			<< "\t| hate :\t" << hatA << "\t" << hatB << "\t=" << hat
			<< "\t| fear :\t" << feaA << "\t" << feaB << "\t=" << fea
			;
		l.ssend();
	}


	girl->happiness(hap);
	girl->pclove(lov);
	girl->pchate(hat);
	girl->pcfear(fea);


	// after all the happy, love fear and hate are done, do some other checks.

#if 0
	if (girl->pchate() > girl->pcfear())		// if she hates you more than she fears you, she will disobey more
	{
		girl->obedience(g_Dice.bell(mod, 0));
		girl->spirit(g_Dice.bell(-1, 2));
	}
	else										// otherwise she will obey more in hopes of getting an upgrade
	{
		girl->obedience(g_Dice.bell(0, -mod));
		girl->spirit(g_Dice.bell(-2, 1));
	}
#endif





	int chance = 1 + (mod < 0 ? -mod : mod);
	if (!g_Dice.percent(chance)) return;
	// Only check if a trait gets modified if mod is far from 0

	bool b_health = g_Dice.percent(girl->health());
	bool b_happiness = g_Dice.percent(girl->happiness());
	bool b_tiredness = g_Dice.percent(girl->tiredness());
	bool b_intelligence = g_Dice.percent(girl->intelligence());
	bool b_confidence = g_Dice.percent(girl->confidence());
	bool b_libido = g_Dice.percent(girl->libido());
	bool b_obedience = g_Dice.percent(girl->obedience());
	bool b_spirit = g_Dice.percent(girl->spirit());
	bool b_pclove = g_Dice.percent(girl->pclove());
	bool b_pcfear = g_Dice.percent(girl->pcfear());
	bool b_pchate = g_Dice.percent(girl->pchate());
	bool b_morality = g_Dice.percent(girl->morality());
	bool b_refinement = g_Dice.percent(girl->refinement());
	bool b_dignity = g_Dice.percent(girl->dignity());

	if (girl->has_trait("Homeless") && b_refinement && b_dignity && b_confidence &&
		mod >= 0 && girl->m_AccLevel >= 5 && g_Dice.percent(girl->m_AccLevel))
	{
		g_Girls.RemoveTrait(girl, "Homeless", true);
		ss << girl->m_Realname << " has gotten used to better surroundings and has lost the \"Homeless\" trait.";
	}
	else if (girl->has_trait("Masochist") && b_intelligence && b_spirit && b_confidence &&
		mod >= 2 && g_Dice.percent(girl->m_AccLevel - 7))
	{
		g_Girls.RemoveTrait(girl, "Masochist", true);
		// `J` zzzzzz - needs better text
		ss << girl->m_Realname << " seems to be getting over her Masochistic tendencies.";
	}
	else if (!girl->has_trait("Masochist") && !b_dignity && !b_spirit && !b_confidence &&
		mod <= -1 && g_Dice.percent(3 - mod))
	{
		g_Girls.AddTrait(girl, "Masochist");
		// `J` zzzzzz - needs better text
		ss << girl->m_Realname << " seems to be getting a little Masochistic from not getting what she thinks she needs.";
	}




#if 1
	else if (g_Dice.percent(90)){}	// `J` - zzzzzz - The rest need work so for now they will be less common
#endif
	else if (girl->has_trait("Optimist") && g_Dice.percent(3))
	{
		g_Girls.RemoveTrait(girl, "Optimist", true);
		ss << girl->m_Realname << " has lost the \"Optimist\" trait. (someone write better text for this)";
	}
	else if (!girl->has_trait("Optimist") && g_Dice.percent(3))	// `J` - zzzzzz - needs work
	{
		g_Girls.AddTrait(girl, "Optimist");
		ss << girl->m_Realname << " has gained the \"Optimist\" trait. (someone write better text for this)";
	}
	else if (girl->has_trait("Pessimist") && g_Dice.percent(3))	// `J` - zzzzzz - needs work
	{
		g_Girls.RemoveTrait(girl, "Pessimist", true);
		ss << girl->m_Realname << " has lost the \"Pessimist\" trait. (someone write better text for this)";
	}
	else if (!girl->has_trait("Pessimist") && g_Dice.percent(3))	// `J` - zzzzzz - needs work
	{
		g_Girls.AddTrait(girl, "Pessimist");
		ss << girl->m_Realname << " has gained the \"Pessimist\" trait. (someone write better text for this)";
	}



	if (ss.str().length() > 0)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);

	// old code
#else
	/*
	*	add the girls accommodation and food costs to the upkeep
	*/
	if (girl->is_slave()) {
		/*
		*		For a slavegirl, 5 gold per accommodation level
		*/
		brothel->m_Finance.girl_support(5 * (girl->m_AccLevel + 1));
		/*
		*		accommodation zero is all they expect
		*/
		if (girl->m_AccLevel == 0) return;
		/*
		*		accommodation over 0 means happier,
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
		if (excess >= 0) {
			int mod = 1 + excess / 3;
			girl->pchate(-mod);
			girl->pclove(mod);
		}
		// end mod
		return;
	}
	/*
	*	For a freegirl, 20 gold per accommodation level
	*	mod - doc - simplified the calculation a bit
	*/
	brothel->m_Finance.girl_support(20 * (girl->m_AccLevel + 1));
	/*
	*	let's do the simple case
	*	if her accommodation is greater then her level
	*		divided by 2		// `J` added
	*	she'll get happier. That's a mod: it was >=
	*	before, but this way 0 level girls want level 1 accom
	*	and it goes up level for level thereafter
	*/
	if (girl->m_AccLevel > girl->level() / 2)
	{
		girl->happiness(2 + girl->m_AccLevel / 2);
		int excess = girl->happiness() - 100;
		if (excess >= 0)
		{
			int mod = 1 + excess / 3;
			girl->pchate(-mod);
			girl->pclove(mod);
		}
		return;
	}
	/*
	*	If we get here, the accommodation level is less
	*	than a girl of her accomplisments would expect
	*	However, level 11 (was 6) and greater and her sense of
	*	professionalism means she doesn't let it affect her
	*	state of mind
	*/
	if (girl->level() >= 11) {
		return;
	}
	/*
	*	Failing that, she will be less happy
	*/
	// `J` - she will be much less happy with lower accom now
	int mod, diff = girl->level() - girl->m_AccLevel;
	mod = diff / 2;	// half the difference, round down
	mod++;		// and add one
	girl->happiness(-mod);
	/*
	*	and if she gets completely miserable,
	*	she'll grow to hate the PC
	*/
	if (girl->happiness() <= 0) {
		girl->pchate(1 + diff / 3);
	}
#endif		// end old code
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

	if (countFrom == -1)
		countFrom = 0;

	if (countFrom >= MAXNUM_INVENTORY)
		return -1;

	for (int i = countFrom; i<MAXNUM_INVENTORY; i++)
	{
		if (m_Inventory[i])
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
	while (loop)
	{
		if (curI != -1)
		{
			if (g_Brothels.m_NumItem[curI] >= 999)
				curI = g_Brothels.HasItem(item->m_Name, curI + 1);
			else
				loop = false;
		}
		else
			loop = false;
	}

	if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
	{
		if (curI != -1)
		{
			added = true;
			g_Brothels.m_NumItem[curI]++;

		}
		else
		{
			for (int j = 0; j<MAXNUM_INVENTORY; j++)
			{
				if (g_Brothels.m_Inventory[j] == 0)
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

int cBrothelManager::GetNumberOfItemsOfType(int type, bool splitsubtype)
{
	if (m_NumInventory < 1) return 0;

	int num = 0;
	int found = 0;	// to reduce loops
	for (int i = 0; i < MAXNUM_INVENTORY && found < m_NumInventory; i++)
	{
		if (m_Inventory[i])
		{
			found++;
			if (m_Inventory[i]->m_Type == type)
				num++;
			// if we are looking for consumables (INVFOOD) but we are not splitting subtypes, accept INVMAKEUP as INVFOOD.
			if (type == INVFOOD && !splitsubtype && m_Inventory[i]->m_Type == INVMAKEUP)
				num++;
		}
	}
	return num;
}


// ----- Bank & money
void cBrothelManager::WithdrawFromBank(long amount)
{
	if (m_Bank - amount >= 0)
		m_Bank -= amount;
}

void cBrothelManager::DepositInBank(long amount)
{
	if (amount > 0)
		m_Bank += amount;
}

double cBrothelManager::calc_pilfering(sGirl *girl)
{
	double factor = 0.0;
	if (girl->is_addict() && girl->m_Money < 100)			// on top of all other factors, an addict will steal to feed her habit
		factor += (girl->is_addict(true) ? 0.5 : 0.1);		// hard drugs will make her steal more
	// let's work out what if she is going steal anything
	if (girl->pclove() >= 50 || girl->obedience() >= 50) return factor;			// love or obedience will keep her honest
	if (girl->pcfear() > girl->pchate()) return factor;							// if her fear is greater than her hate, she won't dare steal
	// `J` yes they do // if (girl->is_slave()) return factor;					// and apparently, slaves don't steal
	if (girl->pchate() > 40) return factor + 0.15;								// given all the above, if she hates him enough, she'll steal
	if (girl->confidence() > 70 && girl->spirit() > 50) return factor + 0.15;	// if she's not motivated by hatred, she needs to be pretty confident
	return factor;	// otherwise, she stays honest (aside from addict factored-in earlier)
}

void cBrothelManager::CalculatePay(sBrothel* brothel, sGirl* girl, u_int Job)
{
	// no pay or tips, no need to continue
	if (girl->m_Pay <= 0 && girl->m_Tips <= 0) { girl->m_Pay = girl->m_Tips = 0; return; }

	if (girl->m_Tips > 0)		// `J` check tips first
	{
		if ((cfg.initial.girls_keep_tips() && !girl->is_slave()) ||	// if free girls tips are counted sepreatly from pay
			(cfg.initial.slave_keep_tips() && girl->is_slave()))		// if slaves tips are counted sepreatly from pay
		{
			girl->m_Money += girl->m_Tips;	// give her the tips directly
		}
		else	// otherwise add tips into pay
		{
			girl->m_Pay += girl->m_Tips;
		}
	}
	girl->m_Tips = 0;
	// no pay, no need to continue
	if (girl->m_Pay <= 0) { girl->m_Pay = 0; return; }

	// if the house takes nothing		or if it is a player paid job and she is not a slave
	if (girl->m_Stats[STAT_HOUSE] == 0 || (m_JobManager.is_job_Paid_Player(Job) && !girl->is_slave()) ||
		// or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
		(m_JobManager.is_job_Paid_Player(Job) && girl->is_slave() && cfg.initial.slave_pay_outofpocket()))
	{
		girl->m_Money += girl->m_Pay;	// she gets it all
		girl->m_Pay = 0;
		return;
	}

	// so now we are to the house percent.
	float house_factor = float(girl->m_Stats[STAT_HOUSE]) / 100.0f;

	// work out how much gold (if any) she steals
	double steal_factor = calc_pilfering(girl);
	int stolen = int(steal_factor * girl->m_Pay);
	girl->m_Pay -= stolen;
	girl->m_Money += stolen;


	int house = int(house_factor * girl->m_Pay);			// the house takes its cut of whatever's left
	if (house > girl->m_Pay) house = girl->m_Pay;			// this shouldn't happen. That said...

	girl->m_Money += girl->m_Pay - house;					// The girl collects her part of the pay
	brothel->m_Finance.brothel_work(house);					// and add the rest to the brothel finances
	girl->m_Pay = 0;										// clear pay
	if (girl->m_Money < 0) girl->m_Money = 0;				// Not sure how this could happen - suspect it's just a sanity check

	if (!stolen) return;									// If she didn't steal anything, we're done
	sGang* gang = g_Gangs.GetGangOnMission(MISS_SPYGIRLS);	// if no-one is watching for theft, we're done
	if (!gang) return;
	int catch_pc = g_Gangs.chance_to_catch(girl);			// work out the % chance that the girl gets caught
	if (!g_Dice.percent(catch_pc)) return;					// if they don't catch her, we're done

	// OK: she got caught. Tell the player
	stringstream gmess; gmess << "Your Goons spotted " << girl->m_Realname << " taking more gold then she reported.";
	gang->m_Events.AddMessage(gmess.str(), IMGTYPE_PROFILE, EVENT_GANG);
}

// ----- Objective
void cBrothelManager::UpdateObjective()
{
	if (m_Objective)
	{
		if (m_Objective->m_Limit > -1) m_Objective->m_Limit--;

		switch (m_Objective->m_Objective)
		{
		case OBJECTIVE_REACHGOLDTARGET:
			if (g_Gold.ival() >= m_Objective->m_Target)				PassObjective();
			break;
		case OBJECTIVE_HAVEXGOONS:
			if (g_Gangs.GetNumGangs() >= m_Objective->m_Target)		PassObjective();
			break;
		case OBJECTIVE_STEALXAMOUNTOFGOLD:
		case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
		case OBJECTIVE_KIDNAPXGIRLS:
		case OBJECTIVE_EXTORTXNEWBUSINESS:
			if (m_Objective->m_SoFar >= m_Objective->m_Target)		PassObjective();
			break;
		case OBJECTIVE_HAVEXMONSTERGIRLS:
			if (GetTotalNumGirls(true) >= m_Objective->m_Target)	PassObjective();
			break;
		case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
			if (GetTotalNumGirls() >= m_Objective->m_Target)		PassObjective();
			break;

			// note that OBJECTIVE_GETNEXTBROTHEL has PassObjective() call in cScreenTown when passed.
		}
		
		// `J` moved to the end and fixed so if the objective is passed (thus deleted), failure is not returned
		if (m_Objective != 0 && m_Objective->m_Limit == 0)
		{
			stringstream ss;
			if (m_Objective->m_Text.size() < 1)	ss << "You have failed an objective.";
			else ss << "You have failed your objective to " << m_Objective->m_Text;
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);
			delete m_Objective;
			m_Objective = 0;
		}
	}
}

sObjective* cBrothelManager::GetObjective(){ return m_Objective; }

void cBrothelManager::CreateNewObjective()
{
	if (m_Objective) delete m_Objective;
	m_Objective = 0;

	m_Objective = new sObjective();
	if (m_Objective)
	{
		stringstream ss;
		stringstream sst;

		sst << gettext("You have a new objective, you must ");
		bool done = false;
		m_Objective->m_Difficulty = g_Year - 1209;
		m_Objective->m_SoFar = 0;
		m_Objective->m_Reward = g_Dice%NUM_REWARDS;
		m_Objective->m_Limit = -1;
		m_Objective->m_Target = 0;
		m_Objective->m_Text = "";

		while (!done)
		{
			m_Objective->m_Objective = g_Dice%NUM_OBJECTIVES;
			switch (m_Objective->m_Objective)
			{
			case OBJECTIVE_REACHGOLDTARGET:
			{
				ss << gettext("Acquire ");
				if (m_Objective->m_Difficulty >= 3)
				{
					m_Objective->m_Limit = (g_Dice % 20) + 10;
					m_Objective->m_Target = m_Objective->m_Limit * 1000;
					ss << m_Objective->m_Target << gettext(" gold within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = ((g_Dice % 20) + 1) * 200;
					ss << m_Objective->m_Target << gettext(" gold.");
				}
				done = true;
			}break;

			case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
			{
				cRivalManager r;
				if (r.GetNumRivals() > 0)
				{
					ss << gettext("Launch a successful attack mission within ");
					m_Objective->m_Limit = (m_Objective->m_Difficulty >= 3 ? (g_Dice % 5) + 3 : (g_Dice % 10) + 10);
					ss << m_Objective->m_Limit << gettext(" weeks.");
					done = true;
				}
			}break;

			case OBJECTIVE_HAVEXGOONS:
			{
				if (g_Gangs.GetNumGangs() < g_Gangs.GetMaxNumGangs())
				{
					m_Objective->m_Target = g_Gangs.GetNumGangs() + ((g_Dice % 3) + 1);
					if (m_Objective->m_Target > g_Gangs.GetMaxNumGangs()) m_Objective->m_Target = g_Gangs.GetMaxNumGangs();
					m_Objective->m_Limit = (m_Objective->m_Difficulty >= 3 ? (g_Dice % 4) + 3 : (g_Dice % 7) + 6);
					ss << gettext("Have ") << m_Objective->m_Target << gettext(" gangs within ") << m_Objective->m_Limit << gettext(" weeks.");
					done = true;
				}
			}break;

			case OBJECTIVE_STEALXAMOUNTOFGOLD:
			{
				if (m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice % 20) + 13;
					m_Objective->m_Target = m_Objective->m_Limit * 1300;
					ss << gettext("Steal ") << m_Objective->m_Target << gettext(" gold within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = ((g_Dice % 20) + 1) * 200;
					ss << gettext("Steal ") << m_Objective->m_Target << gettext(" gold.");
				}
				done = true;
			}break;

			case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
			{
				ss << gettext("Capture ");
				if (m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice % 5) + 1;
					m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
					ss << m_Objective->m_Target << gettext(" girls from the catacombs within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = (g_Dice % 5) + 1;
					ss << m_Objective->m_Target << gettext(" girls from the catacombs.");
				}
				done = true;
			}break;

			case OBJECTIVE_HAVEXMONSTERGIRLS:
			{
				ss << gettext("Have a total of ");
				if (m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice % 8) + 3;
					m_Objective->m_Target = GetTotalNumGirls(true) + (g_Dice % (m_Objective->m_Limit - 1)) + 1;
					ss << m_Objective->m_Target << gettext(" monster (non-human) girls within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = GetTotalNumGirls(true) + (g_Dice % 8) + 1;
					ss << m_Objective->m_Target << gettext(" monster (non-human) girls.");
				}
				done = true;
			}break;

			case OBJECTIVE_KIDNAPXGIRLS:
			{
				ss << gettext("Kidnap ");
				if (m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice % 5) + 1;
					m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
					ss << m_Objective->m_Target << gettext(" girls from the streets within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = (g_Dice % 5) + 1;
					ss << m_Objective->m_Target << gettext(" girls from the streets.");
				}
				done = true;
			}break;

			case OBJECTIVE_EXTORTXNEWBUSINESS: 
			{	// `J` if there are not enough available businesses, don't use this one
				if (TOWN_NUMBUSINESSES > g_Gangs.GetNumBusinessExtorted() + 5)	
				{
					ss << gettext("Gain control of ");
					if (m_Objective->m_Difficulty >= 2)
					{
						m_Objective->m_Limit = (g_Dice % 5) + 1;
						m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
						ss << m_Objective->m_Target << gettext(" new businesses within ") << m_Objective->m_Limit << gettext(" weeks.");
					}
					else
					{
						m_Objective->m_Target = (g_Dice % 5) + 1;
						ss << m_Objective->m_Target << gettext(" new businesses.");
					}
					done = true;
				}
			}break;

			case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
			{
				ss << gettext("Have a total of ");
				if (m_Objective->m_Difficulty >= 2)
				{
					m_Objective->m_Limit = (g_Dice % 8) + 3;
					m_Objective->m_Target = GetTotalNumGirls() + (g_Dice % (m_Objective->m_Limit - 1)) + 1;
					ss << m_Objective->m_Target << gettext(" girls within ") << m_Objective->m_Limit << gettext(" weeks.");
				}
				else
				{
					m_Objective->m_Target = GetTotalNumGirls() + (g_Dice % 8) + 1;
					ss << m_Objective->m_Target << gettext(" girls.");
				}
				done = true;
			}break;

			case OBJECTIVE_GETNEXTBROTHEL:
			{
				if (GetNumBrothels() < 6)
				{
					ss << gettext("Purchase a new brothel");
					if (m_Objective->m_Difficulty >= 2)
					{
						m_Objective->m_Limit = (g_Dice % 10) + 10;
						ss << gettext(" within ") << m_Objective->m_Limit << gettext(" weeks");
					}
					ss << gettext(".");
					done = true;
				}
			}break;
			}
		}

		sst << ss.str();
		m_Objective->m_Text = ss.str();

		if (sst.str().length() > 0)
		{
			g_MessageQue.AddToQue(sst.str(), COLOR_DARKBLUE);
			g_Brothels.GetBrothel(0)->m_Events.AddMessage(sst.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
		}
	}
}

void cBrothelManager::PassObjective()
{
	if (m_Objective)
	{
		// `J` fix for REWARD_RIVALHINDER so it does not have to recall PassObjective()
		cRival* rival = 0;
		if (m_Objective->m_Reward == REWARD_RIVALHINDER)
		{
			rival = m_Rivals.GetRandomRival();
			if (!rival) m_Objective->m_Reward = REWARD_GOLD;
		}

		stringstream ss;
		if (m_Objective->m_Text.size() < 1)	ss << "You have completed your objective and you get ";
		else ss << "You have completed your objective to " << m_Objective->m_Text <<"\nYou get ";


		switch (m_Objective->m_Reward)
		{
		case REWARD_GOLD:
		{
			long gold = (g_Dice % 200) + 33;
			if (m_Objective->m_Difficulty > 0) gold *= m_Objective->m_Difficulty;

			// `J` if you had a time limit you get extra gold for the unused time
			int mod = m_Objective->m_Target;
			if (m_Objective->m_Objective == OBJECTIVE_REACHGOLDTARGET || m_Objective->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
				mod = min(1, m_Objective->m_Target / 100);
			if (m_Objective->m_Limit > 0) gold += mod * m_Objective->m_Limit;

			ss << gold << gettext(" gold.");
			g_Gold.objective_reward(gold);
		}break;

		case REWARD_GIRLS:
		{
			int girls = 1;
			if (m_Objective->m_Difficulty > 0) girls *= m_Objective->m_Difficulty;
			
			// `J` throw in a few extra girls if your mission was to get more girls
			int div = 0;
			int bonus = min(5, m_Objective->m_Limit < 4 ? 1 : m_Objective->m_Limit / 2);
			if (m_Objective->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS || m_Objective->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
				div = 10;
			if (m_Objective->m_Objective == OBJECTIVE_HAVEXMONSTERGIRLS || m_Objective->m_Objective == OBJECTIVE_HAVEXAMOUNTOFGIRLS)
				div = 20;
			if (bonus > 0 && div > 0) girls += min(bonus, m_Objective->m_Target / div);


			ss << girls << " slave girl" << (girls > 1 ? "s" : "") << ":\n";
			while (girls > 0)
			{
				sGirl* girl = g_Girls.CreateRandomGirl(0, false, true, false, g_Dice % 3 == 1);
				stringstream ssg;
				ss << girl->m_Realname << "\n";
				ssg << girl->m_Realname << " was given to you as a reward for completing your objective.";
				girl->m_Events.AddMessage(ssg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
				m_Dungeon.AddGirl(girl, DUNGEON_NEWGIRL);
				girls--;
			}
		}break;

		case REWARD_RIVALHINDER:
		{
			long gold = (rival->m_Gold > 10 ? (g_Dice % (rival->m_Gold / 2)) + 1 : 436);
			rival->m_Gold -= gold;
			g_Gold.objective_reward(gold);
			ss << gettext("to steal ") << gold << gettext(" gold from the ") << rival->m_Name << gettext(".");

			// `J` added 
			bool building = false;
			if (rival->m_NumBrothels > 0 && g_Dice.percent(10))
			{
				ss << "\nOne of their Brothels ";
				building = true;
				rival->m_NumBrothels--;
			}
			else if (rival->m_NumGamblingHalls > 0 && g_Dice.percent(25))
			{
				ss << "\nOne of their Gambling Halls ";
				building = true;
				rival->m_NumGamblingHalls--;
			}
			else if (rival->m_NumBars > 0 && g_Dice.percent(50))
			{
				ss << "\nOne of their Bars ";
				building = true;
				rival->m_NumBars--;
			}
			if (building)
			{
				switch (g_Dice % 5)
				{
				case 0: ss << "is closed down by the health department."; break;
				case 1: ss << "is bombed by an unknown party."; break;
				case 2: ss << "vanishes."; break;
				case 3: ss << "falls into a sinkhole."; break;
				default: ss << "mysteriously burns to the ground."; break;
				}
			}

			if (rival->m_NumGirls > 0 && g_Dice.percent(30))
			{
				int num = 1;
				rival->m_NumGirls--;
				while (rival->m_NumGirls > 0 && g_Dice.percent(50))
				{
					num++;
					rival->m_NumGirls--;
				}
				ss << "\n" << num << " of their girls ";
				switch (g_Dice % 5)
				{
				case 0: ss << "were arrested for various crimes."; break;
				case 1: ss << "were killed."; break;
				case 2: ss << "vanished."; break;
				case 3: ss << "disappeared."; break;
				default: ss << "were kidnapped."; break;
				}
			}
		}break;

		case REWARD_ITEM:
		{
			int numItems = max(1, m_Objective->m_Difficulty);
			int tries = numItems * 10;
			while (numItems > 0 && tries > 0)
			{
				tries--;
				sInventoryItem* item = g_InvManager.GetRandomItem();
				if (item && item->m_Rarity < RARITYSCRIPTONLY)
				{
					int curI = g_Brothels.HasItem(item->m_Name, -1);
					bool loop = true;
					while (loop)
					{
						if (curI != -1)
						{
							if (g_Brothels.m_NumItem[curI] >= 999)
								curI = g_Brothels.HasItem(item->m_Name, curI + 1);
							else
								loop = false;
						}
						else
							loop = false;
					}

					if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
					{
						if (curI != -1)
						{
							ss << gettext("a ");
							ss << item->m_Name;
							ss << gettext(", ");
							g_Brothels.m_NumItem[curI]++;
						}
						else
						{
							for (int j = 0; j<MAXNUM_INVENTORY; j++)
							{
								if (g_Brothels.m_Inventory[j] == 0)
								{
									ss << gettext("a ");
									ss << item->m_Name;
									ss << gettext(", ");
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
						numItems = 0;
						ss << gettext(" Your inventory is full\n");
					}
				}
			}
		}break;
		}

		if (ss.str().length() > 0)
		{
			g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
			g_Brothels.GetBrothel(0)->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
		}
		delete m_Objective;
		m_Objective = 0;
	}
}

void cBrothelManager::AddCustomObjective(int limit, int diff, int objective, int reward, int sofar, int target, string text)
{
	if (m_Objective) delete m_Objective;
	m_Objective = 0;
	m_Objective = new sObjective();

	m_Objective->m_Difficulty = diff;
	m_Objective->m_Limit = limit;
	m_Objective->m_Objective = objective;
	m_Objective->m_Reward = reward;
	m_Objective->m_SoFar = sofar;
	m_Objective->m_Target = target;
	m_Objective->m_Text = text;

}

// ----- Stats

//mod, damn it, I am trying to do python code in c++
int &cBrothelManager::stat_lookup(string stat_name, int brothel_id)
{
	/* */if (stat_name == "filth")			return this->GetBrothel(brothel_id)->m_Filthiness;
	else if (stat_name == "advertising")	return (int&)GetBrothel(brothel_id)->m_AdvertisingBudget;
	else if (stat_name == "security")		return GetBrothel(brothel_id)->m_SecurityLevel;
	else if (stat_name == "beasts")			return this->m_Beasts;
	return m_Dummy;
}

void cBrothelManager::SetGirlStat(sGirl* girl, int stat, int amount)
{
	girl->m_Stats[stat] = amount;	// `J` changed from m_StatMods to m_Stats
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

	// Sanity check. Abort on dead girl
	if (girl->health() <= 0) { return; }

	stringstream ss;
	string girlName = girl->m_Realname;
	int statHouse = girl->house();
	int bonus = (60 - statHouse) / 30;

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

	sum << "\n\nFinal\n"
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

void cBrothelManager::UpdateAllGirlsStat(sBrothel* brothel, int stat, int amount)
{
	if (brothel)
	{
		sGirl* current = brothel->m_Girls;
		while (current)
		{
			g_Girls.UpdateStat(current, stat, amount);
			current = current->m_Next;
		}
	}
	else
	{
		sBrothel* curBroth = m_Parent;
		while (curBroth)
		{
			sGirl* current = curBroth->m_Girls;
			while (current)
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

	if (current == 0) return 0;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	if (current->m_NumGirls == 0) return 0;

	// Makes num reset when it is >= m_NumGirls
	if (num >= current->m_NumGirls) { num = num % current->m_NumGirls; }
	if (num < 0) { num = current->m_NumGirls + (num % current->m_NumGirls); }

	if (current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;
		while (currentGirl)
		{
			if (count == num) break;
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
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}

	if (current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;
		while (currentGirl)
		{
			if (currentGirl == girl)
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
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}

	// Find the girl
	if (current)
	{
		sGirl* currentGirl = current->m_Girls;
		while (currentGirl)
		{
			if (strnatcmp(name.c_str(), currentGirl->m_Realname.c_str()) == 0) break;
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

	for (int i = 0; i < m_NumBrothels; i++)
	{
		BrothelNo = GetGirlPos(i, girl);
		if (BrothelNo != -1) return i;
	}
	return -1;
}

vector<sGirl*> cBrothelManager::GirlsOnJob(int BrothelID, int JobID, bool Day0Night1)
{
	// Used by new security code
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == BrothelID) break;
		current = current->m_Next;
	}

	vector<sGirl*> GirlsOnJob;

	sGirl* curr = current->m_Girls;
	while (curr)
	{
		if (Day0Night1)
		{
			if (curr->m_DayJob == JobID) GirlsOnJob.push_back(curr);
		}
		else
		{
			if (curr->m_NightJob == JobID) GirlsOnJob.push_back(curr);
		}
		curr = curr->m_Next;
	}
	return GirlsOnJob;
}
sGirl* cBrothelManager::GetRandomGirlOnJob(int BrothelID, int JobID, bool Day0Night1)
{
	sGirl* girl = 0;
	vector<sGirl *> girls = GirlsOnJob(BrothelID, JobID, Day0Night1);
	if (girls.size() > 0) girl = girls[g_Dice%girls.size()];

	return girl;
}
sGirl* cBrothelManager::GetFirstGirlOnJob(int BrothelID, int JobID, bool Day0Night1)
{
	sGirl* girl = 0;
	vector<sGirl *> girls = GirlsOnJob(BrothelID, JobID, Day0Night1);
	for (u_int i = 0; i < girls.size(); i++)
	{
		girl = girls[i];
		if (girl) return girl;
	}
	return 0;
}

int cBrothelManager::GetTotalNumGirls(bool monster)
{
	int total = 0;
	if (!monster)
	{
		for (int i = 0; i<m_NumBrothels; i++)
			total += GetNumGirls(i);
		total += GetDungeon()->GetNumGirls();
	}
	else
	{
		sBrothel* current = m_Parent;
		while (current)
		{
			sGirl* girl = current->m_Girls;
			while (girl)
			{
				if (g_Girls.HasTrait(girl, "Not Human"))
					total++;
				girl = girl->m_Next;
			}
			current = current->m_Next;
		}

		for (int i = 0; i<GetDungeon()->GetNumGirls(); i++)
		{
			sDungeonGirl* dgirl = GetDungeon()->GetGirl(i);
			if (g_Girls.HasTrait(dgirl->m_Girl, "Not Human"))
				total++;
		}
	}

	return total;
}

int cBrothelManager::GetFreeRooms(sBrothel* brothel)
{
	return brothel->m_NumRooms - brothel->m_NumGirls;
}

int cBrothelManager::GetFreeRooms(int brothelnum)
{
	sBrothel* brothel = g_Brothels.GetBrothel(brothelnum);
	return brothel->m_NumRooms - brothel->m_NumGirls;
}

#if 0	// `J` no longer used so commenting out
string cBrothelManager::GetGirlString(int brothelID, int girlNum)
{
	string data = "";
	sBrothel* current = m_Parent;

	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}

	if (current)
	{
		int count = 0;
		sGirl* currentGirl = current->m_Girls;

		while (currentGirl)
		{
			if (count == girlNum) break;
			count++;
			currentGirl = currentGirl->m_Next;
		}
		//need to change into string lookup
		string dayjob = "Resting";
		switch ((int)currentGirl->m_DayJob)
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
		switch ((int)currentGirl->m_NightJob)
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

		if (g_Girls.GetStat(currentGirl, STAT_AGE) == 100)
			sprintf(buffer, gettext("%s | Age: unknown | Health: %i | Happiness %i | Day Job: %s | Night Job: %s"), currentGirl->m_Realname.c_str(), (int)g_Girls.GetStat(currentGirl, STAT_HEALTH), (int)g_Girls.GetStat(currentGirl, STAT_HAPPINESS), dayjob.c_str(), nightjob.c_str());
		else
			sprintf(buffer, gettext("%s | Age: %i | Health: %i | Happiness %i | Day Job: %s | Night Job: %s"), currentGirl->m_Realname.c_str(), g_Girls.GetStat(currentGirl, STAT_AGE), (int)g_Girls.GetStat(currentGirl, STAT_HEALTH), (int)g_Girls.GetStat(currentGirl, STAT_HAPPINESS), dayjob.c_str(), nightjob.c_str());
		data = buffer;
	}

	return data;
}
#endif

string cBrothelManager::GetName(int brothelID)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if (current)
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
	if (!brothel) {
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
	ss << "Customer Happiness: " << happiness_text(brothel) << endl;
	ss << "Fame: " << fame_text(brothel);
	ss << "Rooms (available/current): " << (brothel->m_NumRooms - brothel->m_NumGirls) << " / " << brothel->m_NumRooms << endl;
	//ss << "Strip Bar: "		<< has_bar		<< endl;
	//ss << "Gambling Hall: "	<< has_hall		<< endl;
	ss << "This brothel's Profit: " << profit << endl;
	ss << "Your Gold: " << g_Gold.ival() << endl;
	ss << "Security Level: " << brothel->m_SecurityLevel << endl;
	//ss << "Gambling Pool: "		<< m_GamblingHallPool	<< endl;
	ss << "Disposition: " << disposition_text() << endl;
	ss << "Suspicion: " << suss_text() << endl;
	ss << "Filthiness: " << brothel->m_Filthiness << endl;
	ss << "Beasts Housed Here: " << g_Brothels.GetNumBeasts() << endl;
	return ss.str();
	//add cleanliness and check gh and bh
}

sBrothel* cBrothelManager::GetBrothel(int brothelID)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID)
			return current;
		current = current->m_Next;
	}
	/*
	*	brothel not found at least deserves a log message
	*/
	stringstream ss;
	ss << "Brothel " << brothelID << " not found in list!";
	g_LogFile.write(ss.str());
	return 0;
}

int cBrothelManager::GetNumBrothelsWithVacancies()
{
	int number = 0;
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_NumGirls < current->m_NumRooms)
			number++;
		current = current->m_Next;
	}

	return number;
}
int cBrothelManager::GetFirstBrothelWithVacancies()
{
	if (GetNumBrothelsWithVacancies() < 1) return -1;

	int number = 0;
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_NumGirls < current->m_NumRooms)
			return number;
		current = current->m_Next;
	}

	return -1;
}
int cBrothelManager::GetRandomBrothelWithVacancies()
{
	int chance = GetNumBrothelsWithVacancies();
	if (chance < 1) return -1;
	int *choice = new int[chance];
	int j = 0;
	int number = 0;
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_NumGirls < current->m_NumRooms)
		{
			choice[j] = number;
			j++;
		}
		number++;
		current = current->m_Next;
	}

	return choice[g_Dice%chance];
}

int cBrothelManager::GetNumGirls(int brothelID)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID) break;
		current = current->m_Next;
	}
	if (current) return current->m_NumGirls;
	return 0;
}

int cBrothelManager::GetNumGirlsOnJob(int brothelID, int jobID, int day)
{
	int count = 0;
	sBrothel* current = m_Parent;
	if (brothelID != -1)
	{
		while (current)
		{
			if (current->m_id == brothelID) break;
			current = current->m_Next;
		}
	}
	while (current)
	{
		sGirl* curr = current->m_Girls;
		while (curr)
		{
			if (day == 0)
			{
				if (curr->m_DayJob == jobID)	count++;
			}
			else
			{
				if (curr->m_NightJob == jobID)	count++;
			}
			curr = curr->m_Next;
		}
		if (brothelID == -1)	current = current->m_Next;
		else					current = 0;
	}
	return count;
}

void cBrothelManager::SetName(int brothelID, string name)
{
	string data = "";
	sBrothel* current = m_Parent;
	while (current)
	{
		if (current->m_id == brothelID)
			break;
		current = current->m_Next;
	}

	if (current)
		current->m_Name = name;
	else
		current->m_Name = "cBrothelManager::GetName - Something went wrong";
}

// ----- Status texts
string cBrothelManager::disposition_text()
{
	if (The_Player->disposition() >= 100)	return gettext("Saint");
	if (The_Player->disposition() >= 80)	return gettext("Benevolent");
	if (The_Player->disposition() >= 50)	return gettext("Nice");
	if (The_Player->disposition() >= 10)	return gettext("Pleasant");
	if (The_Player->disposition() >= -10)	return gettext("Neutral");
	if (The_Player->disposition() >= -50)	return gettext("Not nice");
	if (The_Player->disposition() >= -80)	return gettext("Mean");
	return gettext("Evil");
}

string cBrothelManager::fame_text(sBrothel* brothel)
{
	if (brothel->m_Fame >= 90)	return	gettext("World Renowned\n");
	if (brothel->m_Fame >= 80)	return	gettext("Famous\n");
	if (brothel->m_Fame >= 70)	return	gettext("Well Known\n");
	if (brothel->m_Fame >= 60)	return	gettext("Talk of the town\n");
	if (brothel->m_Fame >= 50)	return	gettext("Somewhat known\n");
	if (brothel->m_Fame >= 30)	return	gettext("Mostly unknown\n");
	return	gettext("Unknown\n");
}

string cBrothelManager::suss_text()
{
	//WD:	Should be Susipicion not Disposition 
	if (The_Player->suspicion() >= 80)	return gettext("Town Scum");
	if (The_Player->suspicion() >= 50)	return gettext("Miscreant");
	if (The_Player->suspicion() >= 10)	return gettext("Suspect");
	if (The_Player->suspicion() >= -10)	return gettext("Unsuspected");
	if (The_Player->suspicion() >= -50)	return gettext("Lawful");
	if (The_Player->suspicion() >= -80) return gettext("Philanthropist");
	return gettext("Town Hero");
}

string cBrothelManager::happiness_text(sBrothel* brothel)
{
	if (brothel->m_Happiness >= 80)		return gettext("High");
	if (brothel->m_Happiness < 40)		return gettext("Low");
	else					return gettext("Medium");
}

// ----- Combat

// True means the girl beat the brothel master
bool cBrothelManager::PlayerCombat(sGirl* girl)		//  **************************** for now doesn't count items
{
	CLog l;

	// MYR: Sanity check: Incorporeal is an auto-win.
	if (girl->has_trait("Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss() << gettext("\nGirl vs. Brothel owner: ") << girl->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return true;
	}

	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int pattack = SKILL_COMBAT;
	int dodge = 0;
	int pdodge = The_Player->m_Stats[STAT_AGILITY];
	int pHealth = 100;
	int pMana = 100;

	// first determine what she will fight with
	if (g_Girls.GetSkill(girl, SKILL_COMBAT) >= g_Girls.GetSkill(girl, SKILL_MAGIC))
		attack = SKILL_COMBAT;
	else
		attack = SKILL_MAGIC;

	// determine what player will fight with
	if (The_Player->m_Skills[SKILL_COMBAT] >= The_Player->m_Skills[SKILL_MAGIC])
		pattack = SKILL_COMBAT;
	else
		pattack = SKILL_MAGIC;

	// calculate the girls dodge ability
	if ((g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS)) < 0)
		dodge = 0;
	else
		dodge = (g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS));

	int combatrounds = 0;
	while (g_Girls.GetStat(girl, STAT_HEALTH) > 20 && pHealth > 0 && combatrounds < 1000)
	{
		// Girl attacks
		if (g_Dice.percent(g_Girls.GetSkill(girl, attack)))
		{
			int damage = 0;
			if (attack == SKILL_MAGIC)
			{
				if (g_Girls.GetStat(girl, STAT_MANA) <= 0)
				{
					attack = SKILL_COMBAT;
					damage = 2;
				}
				else
				{
					damage = 2 + (g_Girls.GetSkill(girl, attack) / 5);
					g_Girls.UpdateStat(girl, STAT_MANA, -7);
				}
			}
			else
			{
				// she has hit now calculate how much damage will be done
				damage = 5 + (g_Girls.GetSkill(girl, attack) / 10);
			}

			g_Girls.UpdateSkill(girl, attack, g_Dice % 2);	// she may improve a little

			// player attempts Dodge
			if (!g_Dice.percent(pdodge))
				pHealth -= damage;
			else
				The_Player->m_Stats[STAT_AGILITY] += g_Dice % 2;	// player may improve a little
		}

		// Player Attacks
		if (g_Dice.percent(The_Player->m_Skills[pattack]))
		{
			int damage = 0;
			if (pattack == SKILL_MAGIC)
			{
				if (pMana <= 0)
				{
					pattack = SKILL_COMBAT;
					damage = 2;
				}
				else
				{
					damage = 2 + (The_Player->m_Skills[pattack] / 5);
					pMana -= 5;
				}
			}
			else
			{
				// he has hit now calculate how much damage will be done
				damage = 5 + (The_Player->m_Skills[pattack] / 10);
			}

			The_Player->m_Skills[pattack] += g_Dice % 2;	// he may improve a little

			// girl attempts Dodge
			if (!g_Dice.percent(dodge))
				g_Girls.UpdateStat(girl, STAT_HEALTH, -damage);
			else
			{
				The_Player->m_Stats[STAT_AGILITY] += g_Dice % 2;	// player may improve a little
				if (The_Player->m_Stats[STAT_AGILITY] > 100)
					The_Player->m_Stats[STAT_AGILITY] = 100;
			}
		}


		// update girls dodge ability
		if ((dodge - 2) < 0)
			dodge = 0;
		else
			dodge -= 2;

		// update players dodge ability
		if ((pdodge - 2) < 0)
			pdodge = 0;
		else
			pdodge -= 2;

		combatrounds++;
	}

	if (combatrounds > 999)	// a tie?
	{
		if (g_Girls.GetStat(girl, STAT_HEALTH) > pHealth) return true;	// the girl won
		return false;
	}

	if (g_Girls.GetStat(girl, STAT_HEALTH) < 20)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1);
		return false;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1);

	return true;
}

bool cBrothelManager::FightsBack(sGirl* girl)
{
	if (g_Girls.GetStat(girl, STAT_HEALTH) < 10)	// she won't fight back in this state
		return false;

	if (g_Girls.HasTrait(girl, "Merciless") && (g_Dice.percent(20)))
		return true;

	if (g_Girls.DisobeyCheck(girl, ACTION_COMBAT))
		return true;

	return false;
}

// ----- Utility
sGirl* girl_sort(sGirl* girl, sGirl** lastgirl)	// sort using merge sort (cause works well with linked lists)
{
	sGirl *p, *q, *e, *tail;
	int insize, nmerges, psize, qsize, i;

	insize = 1;

	if (!girl)
	{
		*lastgirl = 0;
		return 0;
	}
	while (1)
	{
		p = girl;
		girl = 0;
		tail = 0;
		nmerges = 0;
		while (p)
		{
			nmerges++;
			q = p;
			psize = 0;
			for (i = 0; i < insize; i++)
			{
				psize++;
				q = q->m_Next;
				if (!q)
					break;
			}

			qsize = insize;

			while (psize > 0 || (qsize > 0 && q))
			{
				if (psize == 0)
				{
					e = q;
					q = q->m_Next;
					qsize--;
				}
				else if (qsize == 0 || !q)
				{
					e = p;
					p = p->m_Next;
					psize--;
				}
				else if (sGirlcmp(p, q) <= 0)
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

				if (tail)
					tail->m_Next = e;
				else
					girl = e;
				e->m_Prev = tail;
				tail = e;
			}
			p = q;
		}

		tail->m_Next = 0;

		if (nmerges <= 1)
		{
			// find the last girl in the list
			//*lastgirl = tail;
			sGirl* tmp = girl;
			while (tmp->m_Next)
				tmp = tmp->m_Next;
			*lastgirl = tmp;
			return girl;
		}
		insize *= 2;
	}
}

int sGirlcmp(sGirl *a, sGirl *b)
{
	if (a == 0 || b == 0)
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
	while (current)
	{
		sGirl* currentGirl = current->m_Girls;
		while (currentGirl)
		{
			if (name == currentGirl->m_Realname)
				return true;
			currentGirl = currentGirl->m_Next;
		}
		current = current->m_Next;
	}
	return false;
}

bool cBrothelManager::SurnameExists(string name)
{
	sBrothel* current = m_Parent;
	while (current)
	{
		sGirl* currentGirl = current->m_Girls;
		while (currentGirl)
		{
			if (name == currentGirl->m_Surname)
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
	if (girl->m_Next)
		girl->m_Next->m_Prev = girl->m_Prev;
	if (girl->m_Prev)
		girl->m_Prev->m_Next = girl->m_Next;
	if (girl == m_Runaways)
		m_Runaways = girl->m_Next;
	if (girl == m_LastRunaway)
		m_LastRunaway = girl->m_Prev;
	girl->m_Next = girl->m_Prev = 0;
	m_NumRunaways--;
}

void cBrothelManager::AddGirlToRunaways(sGirl* girl)
{
	girl->m_Prev = girl->m_Next = 0;
	if (m_Runaways)
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

	bool flightrisk = (girl->has_trait("Kidnapped") || girl->has_trait("Emprisoned Customer"));

	if (flightrisk && girl->happiness() > 50)	// Girls here totally against their will are more likely to try to get away
	{
		girl->m_DaysUnhappy--;					// and they don't reset days to 0 but instead reduce day count
		if (girl->m_DaysUnhappy < 0)
			girl->m_DaysUnhappy = 0;			// until it gets to 0
		return false;
	}
	else if ((girl->has_trait("Homeless") || girl->has_trait("Adventurer")) && girl->happiness() > 10)
	{	// homeless girls and adventurers know they can survive on their own so are more likely to runaway
		if (girl->m_DaysUnhappy > 3)
			girl->m_DaysUnhappy /= 2;		// they don't reset days to 0 but instead divide day count in half
		else girl->m_DaysUnhappy--;			// or just lower by 1
		return false;
	}
	else if (girl->happiness() > 10)
	{
		girl->m_DaysUnhappy = 0;
		return false;
	}

	/*	
	*	`J` this was only adding up for free girls
	*	I made it add up for all girls
	*	and free girls become unhappy faster
	*/
	girl->m_DaysUnhappy++;		
	if (!girl->is_slave()) girl->m_DaysUnhappy++;
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
	int matron_chance = brothel->matron_count(girl->m_InClinic, girl->m_InStudio, girl->m_InArena, girl->m_InCentre, girl->m_InHouse, girl->m_InFarm, girl->where_is_she) * 35;

	if (g_Dice.percent(matron_chance)) return false;	// if there is a matron 70%

	if (girl->m_DayJob == JOB_REHAB && (g_Clinic.GetNumGirlsOnJob(0, JOB_COUNSELOR, true) > 0) || (g_Clinic.GetNumGirlsOnJob(0, JOB_COUNSELOR, false) > 0))
	{
		if (g_Dice.percent(70)) return false;
	}

	/*
	*	mainly here, we're interested in the chance that she might run away
	*/
	if (g_Girls.DisobeyCheck(girl, ACTION_GENERAL))	// check if the girl will run away
	{
		if (g_Dice.percent(m_JobManager.guard_coverage() - girl->m_DaysUnhappy)) return false;

		girl->m_Events.AddMessage(gettext("She ran away."), IMGTYPE_PROFILE, EVENT_DANGER);
		SetGirlStat(girl, STAT_TIREDNESS, 0);
		SetGirlStat(girl, STAT_HEALTH, 100);
		girl->m_RunAway = 6;
		stringstream smess;
		smess << girl->m_Realname << gettext(" has run away.\nSend your goons after her to attempt recapture.\nShe will escape for good after 6 weeks.\n");
		g_MessageQue.AddToQue(smess.str(), COLOR_RED);
		return true;
	}

	if (girl->m_Money <= 50) { return false; }
	if (g_Dice.percent(80 - girl->m_DaysUnhappy)) { return false; }
	/*
	*	if she is unhappy she may turn to drugs
	*/
	bool starts_drugs = false;
	//Crazy changed it to this might not be the best // `J` made it better :p
	string drug = "";
	int i = 0;
	if (girl->happiness() <= 20 && g_Girls.HasTrait(girl, "Former Addict"))
	{
		while (!starts_drugs && i<10)		// `J` She will try to find a drug she used to be addicted to
		{									// and if she can't find it in 10 tries she will take what is available
			int d = g_Dice % 8;				// with a slight advantage to alcohol and fairy dust
			switch (d)
			{
			case 1:			drug = "Shroud Addict";			break;	// 12.5%
			case 2: case 3:	drug = "Fairy Dust Addict";		break;	// 25%
			case 4:			drug = "Viras Blood Addict";	break;	// 12.5%
			default:		drug = "Alcoholic";				break;	// 50%
			}
			if (g_Girls.HasRememberedTrait(girl, drug))
			{
				starts_drugs = true;
			}
			i++;
		}
	}
	else if (girl->happiness() <= 3 && g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Viras Blood Addict"))
	{
		drug = "Viras Blood Addict";
	}
	else if (girl->happiness() <= 5 && g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Shroud Addict"))
	{
		drug = "Shroud Addict";
	}
	else if (girl->happiness() <= 8 && g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Fairy Dust Addict"))
	{
		drug = "Fairy Dust Addict";
	}
	else if (girl->happiness() <= 10 && !g_Girls.HasTrait(girl, "Alcoholic"))
	{
		drug = "Alcoholic";
	}

	/*
	*	if she Just Said No then we're done
	*/
	if (drug == "")
	{
		return false;
	}

	g_Girls.AddTrait(girl, drug);
	g_Girls.RemoveTrait(girl, "Former Addict");

	/*
	*	otherwise, report the sad occurrence
	*/
	stringstream ss;
	ss << "This girl's unhappiness has turned her into " << (drug == "Alcoholic" ? "an" : "a") << " " << drug << ".";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
	return false;
}

// ----- Drugs & addiction
void cBrothelManager::check_druggy_girl(stringstream& ss)
{
	if (g_Dice.percent(90)) return;
	sGirl* girl = GetDrugPossessor();
	if (girl == 0) return;
	ss << " They also bust a girl named " << girl->m_Realname << " for possession of drugs and send her to prison.";
	for (int i = 0; i<girl->m_NumInventory; i++) { m_EquipedItems[i] = 0; m_Inventory[i] = 0; }
	m_NumInventory = 0;
	AddGirlToPrison(girl);
}

sGirl* cBrothelManager::GetDrugPossessor()
{
	sBrothel* current = m_Parent;
	while (current)
	{
		sGirl* girl = current->m_Girls;
		while (girl)
		{
			if (!g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))	// girls will only be found out if low intelligence
			{
				if (g_Girls.HasItem(girl, "Shroud Mushroom") || g_Girls.HasItem(girl, "Fairy Dust") || g_Girls.HasItem(girl, "Vira Blood"))
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
	if (girl->m_Next)
		girl->m_Next->m_Prev = girl->m_Prev;
	if (girl->m_Prev)
		girl->m_Prev->m_Next = girl->m_Next;
	if (girl == m_Prison)
		m_Prison = girl->m_Next;
	if (girl == m_LastPrison)
		m_LastPrison = girl->m_Prev;

	girl->m_Next = girl->m_Prev = 0;
	m_NumPrison--;
}

void cBrothelManager::AddGirlToPrison(sGirl* girl)
{
	// remove from girl manager if she is there
	g_Girls.RemoveGirl(girl);

	// remove girl from brothels if she is there
	for (int i = 0; i<g_Brothels.GetNumBrothels(); i++)
		g_Brothels.RemoveGirl(i, girl, false);

	girl->m_Prev = girl->m_Next = 0;
	if (m_Prison)
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

	while (curBrothel)								// WD: loop through all brothels
	{
		curGirl = curBrothel->m_Girls;

		while (curGirl)								// WD: loop through all girls in this brothels
		{
			if ((curGirl->m_DayJob == JOB_TORTURER) || (curGirl->m_PrevDayJob == JOB_TORTURER))	// Should fix torturer crash thx to akia
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
	//			g_MessageQue.AddToQue("Your bars have run out of booze", 1);
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
	if (brothel->m_HasGambStaff) {
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
	if (brothel->m_HasGambStaff == 1) {
		shifts = 2;
	}
	else {
		if (day_girls > 4)	shifts++;
		if (night_girls > 4)	shifts++;
	}
	/*
	*	if there's no-one on either shift, we can go home
	*/
	if (shifts == 0) {
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

	if (odds > 0)
	{
		count = (odds / 5) + 1;		// the amount of customer attracted is proportional to the chance of winning
		random_range = brothel->m_GamblingHall*count;
		numCusts += g_Dice.random(random_range) + day_girls + night_girls;
	}

	bool message_shown = false;
	for (int i = 0; i<numCusts; i++)
	{
		if (!message_shown && GetGamblingPool() == 0)
		{
			g_MessageQue.AddToQue(gettext("CAUTION: The gold pool for the gambling halls is empty."), 1);
			message_shown = true;
		}
		/*
		*		calculate the size of the wager (won or lost)
		*		based on values in config.xml
		*/
		int wager = g_Config.gamble.base;
		wager += g_Dice.random(g_Config.gamble.spread);
		bool customer_wins = g_Dice.percent(g_Config.gamble.odds);

		if (customer_wins == false)		// customer lose
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
		if (pool >= wager) {
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
	if (!message_shown && GetGamblingPool() == 0) {
		g_MessageQue.AddToQue(gettext("CAUTION: The gold pool for the gambling halls is empty."), 1);
		message_shown = true;
	}
	/*
	*	get excess from pool and place into players gold
	*/
	if (GetGamblingPool() > m_GamblingHallPoolMax)
	{
		int transfer = GetGamblingPool() - m_GamblingHallPoolMax;
		TakeGamblingPool(transfer);
		brothel->m_Finance.gambling_profits(transfer);
	}
	return numCusts;
}

#endif
