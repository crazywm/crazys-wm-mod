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

#pragma once

#ifndef __CBROTHEL_H
#define __CBROTHEL_H

#include <string>
#include <vector>

#include "cGirls.h"
#include "cRival.h"
#include "cGold.h"
#include "cJobManager.h"
#include "cBuilding.h"
#include "cDungeon.h"		// WD:	cBrothel clean up move class to own file	
#include "cPlayer.h"		// WD:	cBrothel clean up move class to own file

using namespace std;

extern  cGirls  g_Girls;
extern  CLog    g_LogFile;
extern cConfig cfg;

// holds an objective and its data
typedef struct sObjective
{
	u_int m_Objective;	// the objective type
	int m_Reward;		// the reward type
	int m_Target;		// the x variable for the objective
	int m_SoFar;		// how much of the x variable has been achieved
	int m_Limit;		// the number of weeks must be done by
	int m_Difficulty;	// a number representing how hard it is
	string m_Text;		// save the text for pass objective report.
	string m_FailText;	// save the text for fail objective report.
	string m_PassText;	// save the text for fail objective report.

}sObjective;

bool UseAntiPreg(sGirl *girl);

// holds data for movies
typedef struct sMovie
{
	string m_Name;
	string m_Director;
	string m_Cast;
	string m_Crew;
	long m_Init_Quality;
	long m_Promo_Quality;
	long m_Quality;
	long m_Money_Made;
	long m_RunWeeks;
	sMovie* m_Next;
	sMovie()		{ m_Next = 0; }
	~sMovie()		{ if (m_Next) delete m_Next; m_Next = 0; }
}sMovie;

// defines a single brothel
struct sBrothel
{
	sBrothel();							// constructor
	~sBrothel();						// destructor

	string m_Name;

	int             m_id;
	unsigned short	m_Happiness;				// av. % happy customers last week
	int				m_TotalCustomers;			// the total number of customers for the last week
	int				m_RejectCustomersRestrict;	// How many customers were turned away by your sex restrictions.
	int				m_RejectCustomersDisease;	// How many customers were turned away because of disease.
	int				m_MiscCustomers;			// customers used for temp purposes but must still be taken into account
	unsigned char	m_Fame;						// How famous this brothel is
	int				m_NumRooms;					// How many rooms it has
	int				m_MaxNumRooms;				// How many rooms it can have
	int				m_NumGirls;					// How many girls are here
	unsigned char	m_Bar;						// level of bar: 0 => none
	unsigned char	m_GamblingHall;				// as above
	unsigned short	m_AdvertisingBudget;		// Budget player has set for weekly advertising
	double			m_AdvertisingLevel;			// multiplier for how far budget goes, based on girls working in advertising
	int				m_AntiPregPotions;			// `J` added so all buildings save their own number of potions
	int				m_AntiPregUsed;				// `J` number of potions used last turn
	bool			m_KeepPotionsStocked;		// `J` and if they get restocked
	void AddAntiPreg(int amount);
	int  GetNumPotions()					{ return m_AntiPregPotions; }
	void KeepPotionsStocked(bool stocked)	{ m_KeepPotionsStocked = stocked; }
	bool GetPotionRestock()					{ return m_KeepPotionsStocked; }




	bool Control_Girls()	{ return m_Control_Girls; }
	int Girl_Gets_Girls()	{ return m_Girl_Gets_Girls; }
	int Girl_Gets_Items()	{ return m_Girl_Gets_Items; }
	int Girl_Gets_Beast()	{ return m_Girl_Gets_Beast; }
	bool Control_Girls(bool cg)	{ return m_Control_Girls = cg; }
	int Girl_Gets_Girls(int g)	{ return m_Girl_Gets_Girls = g; }
	int Girl_Gets_Items(int g)	{ return m_Girl_Gets_Items = g; }
	int Girl_Gets_Beast(int g)	{ return m_Girl_Gets_Beast = g; }

	bool m_Control_Girls;
	int m_Girl_Gets_Girls;
	int m_Girl_Gets_Items;
	int m_Girl_Gets_Beast;




	int				m_MovieRunTime;		// see above, counter for the 7 week effect
	int				m_NumMovies;

	sMovie*			m_Movies;			// the movies currently selling
	sMovie*			m_LastMovies;
	sFilm *			m_CurrFilm;

	cBuilding		building;
	cGold			m_Finance;          // for keeping track of how well the place is doing (for the last week)

	// For keeping track of any shows currently being produced here
	int				m_ShowTime;			// when reaches 0 then the show is ready
	int				m_ShowQuality;		// Determined by the average fame and skill of the girls in the show
	unsigned char	m_HasGambStaff;		// gambling hall or
	unsigned char	m_HasBarStaff;		// Bar staff. Not as good as girls but consistent

	bool	m_RestrictAnal;
	bool	m_RestrictBDSM;
	bool	m_RestrictOral;
	bool	m_RestrictTitty;
	bool	m_RestrictHand;
	bool	m_RestrictBeast;
	bool	m_RestrictGroup;
	bool	m_RestrictNormal;
	bool	m_RestrictLesbian;
	bool	m_RestrictFoot;
	bool	m_RestrictStrip;

	int		m_Filthiness;

	cEvents m_Events;

	int		m_BuildingQuality[NUMJOBTYPES];

	sGirl*		m_Girls;				// A list of all the girls this place has
	sGirl*		m_LastGirl;
	sBrothel*	m_Next;

	int		m_SecurityLevel;

	TiXmlElement* SaveBrothelXML(TiXmlElement* pRoot);
	bool LoadBrothelXML(TiXmlHandle hBrothel);
	int free_rooms() { return m_NumRooms - m_NumGirls; }
	bool matron_on_shift(int shift, bool isClinic = false, bool isStudio = false, bool isArena = false, bool isCentre = false, bool isHouse = false, bool isFarm = false, int BrothelID = 0); // `J` added building checks
	int matron_count(bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, bool isFarm, int BrothelID);
	void AddGirl(sGirl* pGirl);
};


/*
 * Manages all brothels
 *
 * Anyone else think this class tries to do too much?
 * Yes it does, I am working on reducing it-Delta
 */
class cBrothelManager
{
public:
	cBrothelManager();					// constructor
	~cBrothelManager();					// destructor

	void Free();

	sGirl* GetDrugPossessor();

	void AddGirlToPrison(sGirl* girl);
	void RemoveGirlFromPrison(sGirl* girl);
	int  GetNumInPrison()			{ return m_NumPrison; }

	void AddGirlToRunaways(sGirl* girl);
	void RemoveGirlFromRunaways(sGirl* girl);
	int  GetNumRunaways()			{ return m_NumRunaways; }


	void NewBrothel(int NumRooms, int MaxNumRooms = 200);
	void DestroyBrothel(int ID);
	void UpdateBrothels();
	void UpdateGirls(sBrothel* brothel, bool Day0Night1);

	void UpdateCustomers(sBrothel* brothel, bool Day0Night1);

	// MYR: Start of my automation functions
	void UsePlayersItems(sGirl* cur);
	bool AutomaticItemUse(sGirl * girl, int InvNum, string message);
	bool AutomaticSlotlessItemUse(sGirl * girl, int InvNum, string message);
	bool AutomaticFoodItemUse(sGirl * girl, int InvNum, string message);
	bool RemoveItemFromInventoryByNumber(int Pos); // support fn
	// End of automation functions

	void UpdateAllGirlsStat(sBrothel* brothel, int stat, int amount);
	void SetGirlStat(sGirl* girl, int stat, int amount);

	sGirl* GetPrison()				{ return m_Prison; }
	int  &stat_lookup(string stat_name, int brothel_id = -1);

	int GetGirlsCurrentBrothel(sGirl* girl); // Used by new security guard code
	vector<sGirl*> GirlsOnJob(int BrothelID, int JobID, bool Day0Night1);	// Also used by new security code
	sGirl* GetRandomGirl(int BrothelID);									// `J` - added
	sGirl* GetRandomGirlOnJob(int BrothelID, int JobID, bool Day0Night1);	// `J` - added
	sGirl* GetFirstGirlOnJob(int BrothelID, int JobID, bool Day0Night1);	// `J` - added

	int GetTotalNumGirls(bool monster = false);
	int GetFreeRooms(sBrothel* brothel);
	int GetFreeRooms(int brothelnum = 0);

	void UpgradeSupplySheds()				{ m_SupplyShedLevel++; }
	int  GetSupplyShedLevel()				{ return m_SupplyShedLevel; }

	void	AddGirl(int brothelID, sGirl* girl, bool keepjob = false);
	void	RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = true);
	sGirl*	GetFirstRunaway();
	void	sort(sBrothel* brothel);		// sorts the list of girls
	void	SortInventory();

	void	SetName(int brothelID, string name);
	string	GetName(int brothelID);

	bool CheckBarStaff(sBrothel* brothel, int numGirls);	// returns true if the bar is staffed 
	bool CheckGambStaff(sBrothel* brothel, int numGirls);	// as above but for gambling hall

	bool	FightsBack(sGirl* girl);
	int		GetNumGirls(int brothelID);
	string	GetGirlString(int brothelID, int girlNum);
	int		GetNumGirlsOnJob(int brothelID, int jobID, int day = 0);

	string GetBrothelString(int brothelID);

	sGirl* GetGirl(int brothelID, int num);
	int    GetGirlPos(int brothelID, sGirl* girl);
	sGirl* GetGirlByName(int brothelID, string name); // MYR: Used by new end of turn code in InterfaceProcesses::TurnSummary

	sBrothel*	GetBrothel(int brothelID);
	int			GetNumBrothels()			{ return m_NumBrothels; }
	int			GetNumBrothelsWithVacancies();
	int			GetFirstBrothelWithVacancies();
	int			GetRandomBrothelWithVacancies();
	sBrothel*	GetRandomBrothel();

	void CalculatePay(sBrothel* brothel, sGirl* girl, u_int Job);

	bool PlayerCombat(sGirl* girl);	// returns true if the girl wins

	cPlayer*	GetPlayer()				{ return &m_Player; }
	cDungeon*	GetDungeon()			{ return &m_Dungeon; }

	int HasItem(string name, int countFrom = -1);

	// Some public members for ease of use
	int m_NumInventory;								// current amount of inventory the brothel has
	sInventoryItem* m_Inventory[MAXNUM_INVENTORY];	// List of inventory items they have (3000 max)
	unsigned char m_EquipedItems[MAXNUM_INVENTORY];	// value of > 0 means equipped (wearing) the item
	int m_NumItem[MAXNUM_INVENTORY];		// the number of items there are stacked
	cJobManager m_JobManager;						// manages all the jobs

	int GetNumberOfItemsOfType(int type, bool splitsubtype=false);



	long GetBribeRate()					{ return m_BribeRate; }
	void SetBribeRate(long rate)		{ m_BribeRate = rate; }
	void UpdateBribeInfluence();
	int  GetInfluence()					{ return m_Influence; }

	cRival* GetRivals()					{ return m_Rivals.GetRivals(); }
	cRivalManager* GetRivalManager()	{ return &m_Rivals; }

	void WithdrawFromBank(long amount);
	void DepositInBank(long amount);
	long GetBankMoney()					{ return m_Bank; }
	int  GetNumFood()					{ return m_Food; }
	int  GetNumDrinks()					{ return m_Drinks; }
	int  GetNumBeasts()					{ return m_Beasts; }
	int  GetNumGoods()					{ return m_HandmadeGoods; }
	int  GetNumAlchemy()				{ return m_Alchemy; }
	void add_to_food(int i)				{ m_Food += i; if (m_Food < 0) m_Food = 0; }
	void add_to_drinks(int i)			{ m_Drinks += i; if (m_Drinks < 0) m_Drinks = 0; }
	void add_to_beasts(int i)			{ m_Beasts += i; if (m_Beasts < 0) m_Beasts = 0; }
	void add_to_goods(int i)			{ m_HandmadeGoods += i; if (m_HandmadeGoods < 0) m_HandmadeGoods = 0; }
	void add_to_alchemy(int i)			{ m_Alchemy += i; if (m_Alchemy < 0) m_Alchemy = 0; }

	bool CheckScripts();

	void UpdateObjective();				// updates an objective and checks for compleation
	sObjective* GetObjective();			// returns the objective
	void CreateNewObjective();			// Creates a new objective
	void PassObjective();				// Gives a reward
	void AddCustomObjective(int limit, int diff, int objective, int reward, int sofar, int target, string text="");

	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);

	bool NameExists(string name);
	bool SurnameExists(string name);

	bool AddItemToInventory(sInventoryItem* item);

	void check_druggy_girl(stringstream& ss);
	void check_raid();
	void do_tax();
	void do_daily_items(sBrothel* brothel, sGirl* girl);
	void do_food_and_digs(sBrothel* brothel, sGirl* girl);	
	string disposition_text();
	string fame_text(sBrothel* brothel);
	string suss_text();
	string happiness_text(sBrothel* brothel);
	double calc_pilfering(sGirl *girl);

	bool runaway_check(sBrothel *brothel, sGirl *girl);


	// WD: JOB_TORTURER stuff
	void	TortureDone(bool flag)		{ m_TortureDoneFlag = flag; return; }
	bool	TortureDone()				{ return m_TortureDoneFlag; }
	sGirl* WhoHasTorturerJob();

	// WD: test to check if doing turn processing.  Used to ingnore HOUSE_STAT value in GetRebelValue() if girl gets to keep all her income.
	bool is_Dayshift_Processing()		{ return m_Processing_Shift == SHIFT_DAY; };
	bool is_Nightshift_Processing()		{ return m_Processing_Shift == SHIFT_NIGHT; };

	// WD:	Update code of girls stats
	void updateGirlTurnBrothelStats(sGirl* girl);



	//private:
	int TotalFame(sBrothel *);
	cPlayer m_Player;				// the stats for the player owning these brothels
	cDungeon m_Dungeon;				// the dungeon

	int m_NumBrothels;
	sBrothel* m_Parent;
	sBrothel* m_Last;

	// brothel supplies
/*	// `J` moved to individual buildings
	bool m_KeepPotionsStocked;
	int  m_AntiPregPotions;			// the number of pregnancy/insimination preventive potions in stock
*/
	int  m_SupplyShedLevel;			// the level of the supply sheds. the higher the level, the more alcohol and antipreg potions can hold

	// brothel resources
	int m_HandmadeGoods;			// used with the community centre
	int m_Beasts;					// used for beastiality scenes
	int m_Food;						// food produced at the farm
	int m_Drinks;					// drinks produced at the farm
	int m_Alchemy;

	// brothel resource Reserves - How much will NOT be sold so it can be used by the Brothels 
	int m_HandmadeGoodsReserves;
	int m_BeastsReserves;
	int m_FoodReserves;
	int m_DrinksReserves;
	int m_AlchemyReserves;


	int		m_NumPrison;
	sGirl*  m_Prison;				// a list of girls kept in prision
	sGirl*  m_LastPrison;

	int		m_NumRunaways;          // a list of runaways
	sGirl*	m_Runaways;
	sGirl*	m_LastRunaway;

	long m_BribeRate;				// the amount of money spent bribing officials per week
	int  m_Influence;				// based on the bribe rate this is the percentage of influence you have
	int  m_Dummy;					//a dummy variable
	long m_Bank;					// how much is stored in the bank

	sObjective* m_Objective;

	cRivalManager m_Rivals;			// all of the players compedators

	bool m_TortureDoneFlag;			// WD:	Have we got a torturer working today
	int  m_Processing_Shift;		// WD:	Store Day0Night1 value when processing girls

	void AddBrothel(sBrothel* newBroth);

/*
int FoodAnimal[6] = { 0, 0, 0, 0, 0, 0 };
string FoodAnimalName[6] = { "Egg", "Chicken", "Goat", "Sheep", "Ostrich", "Cow" };
int FoodAnimalFoodValue[6] = { 1, 2, 3, 3, 6, 10 };

int FoodPlant[6] = { 0, 0, 0, 0, 0, 0 };
string FoodPlantName[6] = { "Wheat", "Corn", "Potato", "Tomato", "Lettuce", "Hops" };

int GardenPlant[10] = { 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
string GardenPlantName[10] = { "Weeds", "Easy", "Simple", "Common", "Uncommon", "Special", "Very Special", "Rare", "Very Rare", "Unique" };
*/

void
EndOfDay(sBrothel * brothel, const string& matron_title, bool Day0Night1, u_int restjob, u_int matronjob,
         bool has_matron);
};


#endif  /* __CBROTHEL_H */
