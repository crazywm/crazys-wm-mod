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

#ifndef CRAZYS_WM_MOD_SGIRL_HPP
#define CRAZYS_WM_MOD_SGIRL_HPP

#include <string>
#include <map>
#include <vector>
#include <set>
#include <tinyxml.h>

#include "cTriggers.h"
#include "cEvents.h"
#include "Constants.h"

class TraitSpec;
class sInventoryItem;
class sChild;
class IBuilding;
class cPlayer;
class sCustomer;

typedef struct sChild
{
    int m_MultiBirth;
    string multibirth_str()
    {
        if (m_MultiBirth == 2) return "Twins";
        if (m_MultiBirth == 3) return "Triplets";
        if (m_MultiBirth == 4) return "Quads";
        if (m_MultiBirth == 5) return "Quints";
        // `J` anything else is single
        m_MultiBirth = 1;
        return "Single";
    };
    enum Gender {
        None = -1,
        Girl = 0,
        Boy = 1
    };
    Gender m_Sex;
    int m_GirlsBorn;			// if multiple births, how many are girls
    string boy_girl_str()
    {
        if (m_MultiBirth == 2)	return "twins";
        if (m_MultiBirth == 3)	return "triplets";
        if (m_MultiBirth > 3)	return "a litter";
        if (m_Sex == Boy)		return "a baby boy";
        if (m_Sex == Girl)		return "a baby girl";
        return "a baby";
    }
    bool is_boy()	{ return m_Sex == Boy; }
    bool is_girl()	{ return m_Sex == Girl; }

    int m_Age;	// grows up at 60 weeks
    bool m_IsPlayers;	// 1 when players
    unsigned char m_Unborn;	// 1 when child is unborn (for when stats are inherited from customers)

    // skills and stats from the father
    int m_Stats[NUM_STATS];
    int m_Skills[NUM_SKILLS];

    sChild* m_Next;
    sChild* m_Prev;

    sChild(bool is_players = false, Gender gender = None, int MultiBirth = 1);
    ~sChild(){ m_Prev = nullptr; if (m_Next)delete m_Next; m_Next = nullptr; }

    TiXmlElement* SaveChildXML(TiXmlElement* pRoot);
    bool LoadChildXML(TiXmlHandle hChild);

}sChild;
/*
a class to handle all the child related code to prevent errors.
*/
class cChildList
{
public:

    sChild* m_FirstChild;
    sChild* m_LastChild;
    int m_NumChildren;
    cChildList(){ m_FirstChild = nullptr; m_LastChild = nullptr; m_NumChildren = 0; }
    ~cChildList(){ if (m_FirstChild) delete m_FirstChild; }
    void add_child(sChild*);
    sChild* remove_child(sChild*, sGirl*);
    //void handle_childs();
    //void save_data(ofstream);
    //void write_data(ofstream);
    //sChild* GenerateBornChild();// need to figure out what the player/customer base class is and if needed create one
    //sChild* GenerateUnbornChild();

};


// Represents a single girl
struct sGirl
{
    sGirl();
    ~sGirl();

    int m_newRandomFixed;

    std::string m_Name;								// The girls name
    std::string m_Realname;							// this is the name displayed in text
    /*	`J` adding first and surnames for future use.
    *	m_Realname will be used for girl tracking until first and surnames are fully integrated
    *	a girl id number system may be added in the future to allow for absolute tracking
    */
    std::string m_FirstName;							// this is the girl's first name
    std::string m_MiddleName;						// this is the girl's middle name
    std::string m_Surname;							// this is the girl's surname
    std::string m_MotherName;						//	`J` added mother and father names
    std::string m_FatherName;						//	`J` added mother and father names
    /*
    *	MOD: changed from char* -- easier to change from lua -- doc
    */
    std::string m_Desc;								// Short story about the girl

    unsigned char m_NumTraits;					// current number of traits they have
    TraitSpec* m_Traits[MAXNUM_TRAITS];			// List of traits they have
    int m_TempTrait[MAXNUM_TRAITS];	// a temp trait if not 0. Trait removed when == 0. traits last for 20 weeks.

    unsigned char m_NumRememTraits;				// number of traits that are apart of the girls starting traits
    TraitSpec* m_RememTraits[MAXNUM_TRAITS * 2];		// List of traits they have inbuilt

    unsigned int m_DayJob;						// id for what job the girl is currently doing
    unsigned int m_NightJob;					// id for what job the girl is currently doing
    unsigned int m_PrevDayJob;					// id for what job the girl was doing
    unsigned int m_PrevNightJob;				// id for what job the girl was doing
    unsigned int m_YesterDayJob;				// id for what job the girl did yesterday
    unsigned int m_YesterNightJob;				// id for what job the girl did yesternight

    //ADB needs to be int because player might have more than 256
    int m_NumInventory;							// current amount of inventory they have
    sInventoryItem* m_Inventory[MAXNUM_GIRL_INVENTORY];		// List of inventory items they have (40 max)
    unsigned char m_EquipedItems[MAXNUM_GIRL_INVENTORY];	// value of > 0 means equipped (wearing) the item

    long m_States;								// Holds the states the girl has
    long m_BaseStates;							// `J` Holds base states the girl has for use with equipable items

    // Abstract stats (not shown as numbers but as a raiting)
    int m_Stats[NUM_STATS];
    int m_StatTr[NUM_STATS];					// Trait modifiers to stats
    int m_StatMods[NUM_STATS];					// perminant modifiers to stats
    int m_StatTemps[NUM_STATS];					// these go down (or up) by 30% each week until they reach 0

    int m_Enjoyment[NUM_ACTIONTYPES];			// these values determine how much a girl likes an action
    int m_EnjoymentTR[NUM_ACTIONTYPES];			// `J` added for traits to affect enjoyment
    int m_EnjoymentMods[NUM_ACTIONTYPES];		// `J` added perminant modifiers to stats
    int m_EnjoymentTemps[NUM_ACTIONTYPES];		// `J` added these go down (or up) by 30% each week until they reach 0
    // (-100 is hate, +100 is loves)
    int m_Virgin;								// is she a virgin, 0=false, 1=true, -1=not checked

    bool m_UseAntiPreg;							// if true she will use anit preg measures

    unsigned char m_Withdrawals;				// if she is addicted to something this counts how many weeks she has been off

    int m_Money;

    int m_AccLevel;					// how good her Accommodation is, 0 is slave like and non-slaves will really hate it

    int m_Skills[NUM_SKILLS];
    int m_SkillTr[NUM_SKILLS];
    int m_SkillMods[NUM_SKILLS];
    int m_SkillTemps[NUM_SKILLS];				// these go down (or up) by 1 each week until they reach 0

    int m_Training[NUM_TRAININGTYPES];			// these values determine how far a girl is into her training CRAZY
    int m_TrainingMods[NUM_TRAININGTYPES];		// 
    int m_TrainingTemps[NUM_TRAININGTYPES];		// 
    // (starts at 0, 100 if fully trained)

    int m_RunAway;					// if 0 then off, if 1 then girl is removed from list,
    // otherwise will count down each week
    unsigned char m_Spotted;					// if 1 then she has been seen stealing but not punished yet

    unsigned long m_WeeksPast;					// number of weeks in your service
    unsigned int m_BDay;						// number of weeks in your service since last aging

    int BirthMonth;
    int BirthDay;


    unsigned long m_NumCusts;					// number of customers this girl has slept with
    unsigned long m_NumCusts_old;				// number of customers this girl has slept with before this week

    bool m_Tort;								// if true then have already tortured today
    bool m_JustGaveBirth;						// did she give birth this current week?

    int m_Pay;									// used to keep track of pay this turn
    int m_Tips;									// used to keep track of tips this turn

    std::set<Fetishs> m_FetishTypes;            // the types of fetishes this girl has

    char m_Flags[NUM_GIRLFLAGS];				// flags used by scripts

    cEvents m_Events;							// Each girl keeps track of all her events that happened to her in the last turn


    cTriggerList m_Triggers;					// triggers for the girl

    unsigned char m_DaysUnhappy;				// used to track how many days they are really unhappy for

    int m_WeeksPreg;							// number of weeks pregnant or inseminated
    int m_PregCooldown;							// number of weeks until can get pregnant again
    cChildList m_Children;
    int m_ChildrenCount[CHILD_COUNT_TYPES];

    std::vector<std::string> m_Canonical_Daughters;

    IBuilding* m_Building = nullptr;
    int m_PrevWorkingDay;						// `J` save the last count of the number of working days
    int m_WorkingDay;							// count the number of working day
    int m_SpecialJobGoal;						// `J` Special Jobs like surgeries will have a specific goal
    bool m_Refused_To_Work_Day;					// `J` to track better if she refused to work her assigned job
    bool m_Refused_To_Work_Night;				// `J` to track better if she refused to work her assigned job


    void dump(std::ostream &os);

    /*
    *	MOD: docclox. attach the skill and stat names to the
    *	class that uses them. Plus an XML load method and
    *	an ostream << operator to pretty print the struct for
    *	debug purposes.
    *
    *	Sun Nov 15 05:58:55 GMT 2009
    */
    static const char	*stat_names[];
    static const char	*skill_names[];
    static const char	*status_names[];
    static const char	*enjoy_names[];
    static const char	*enjoy_jobs[];
    static const char	*training_names[];
    static const char	*training_jobs[];
    static const char	*children_type_names[];	// `J` added
    /*
    *	again, might as well make them part of the struct that uses them
    */
    static const unsigned int	max_stats;
    static const unsigned int	max_skills;
    static const unsigned int	max_statuses;
    static const unsigned int	max_enjoy;
    static const unsigned int	max_jobs;
    static const unsigned int	max_training;
    /*
    *	we need to be able to go the other way, too:
    *	from std::string to number. The maps map stat/skill names
    *	onto index numbers. The setup flag is so we can initialise
    * 	the maps the first time an sGirl is constructed
    */
    static bool		m_maps_setup;
    static std::map<std::string, unsigned int>	stat_lookup;
    static std::map<std::string, unsigned int>	skill_lookup;
    static std::map<std::string, unsigned int>	status_lookup;
    static std::map<std::string, unsigned int>	enjoy_lookup;
    static std::map<std::string, unsigned int>	fetish_lookup;
    static std::map<std::string, unsigned int>	jobs_lookup;
    static std::map<std::string, unsigned int>	training_lookup;
    static void		setup_maps();

    static int lookup_stat_code(std::string s);
    static int lookup_skill_code(std::string s);
    static int lookup_status_code(std::string s);
    static int lookup_enjoy_code(std::string s);
    static int lookup_fetish_code(std::string s);
    static int lookup_jobs_code(std::string s);
    static int lookup_training_code(std::string s);
    std::string lookup_where_she_is();
    /*
    *	Strictly speaking, methods don't belong in structs.
    *	I've always thought that more of a guideline than a hard and fast rule
    */
    void load_from_xml(TiXmlElement* el);	// uses sGirl::load_from_xml
    TiXmlElement* SaveGirlXML(TiXmlElement* pRoot);
    bool LoadGirlXML(TiXmlHandle hGirl);

    /*
    *	stream operator - used for debug
    */
    friend ostream& operator<<(ostream& os, sGirl &g);
    /*
    *	it's a bit daft that we have to go through the global g_Girls
    *	every time we want a stat.
    *
    *	I mean the sGirl type is the one we're primarily concerned with.
    *	that ought to be the base for the query.
    *
    *	Of course, I could just index into the stat array,
    *	but I'm not sure what else the cGirls method does.
    *	So this is safer, if a bit inefficient.
    */
    int get_stat(int stat_id) const;
    void set_stat(int stat, int amount);

    int upd_temp_stat(int stat_id, int amount, bool usetraits=false);
    int upd_stat(int stat_id, int amount, bool usetraits = true);

    int upd_temp_Enjoyment(int stat_id, int amount);
    int upd_Enjoyment(int stat_id, int amount, bool usetraits = true);
    int upd_Training(int stat_id, int amount, bool usetraits = true);


    /*
    *	Now then:
    */
    // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h
    int charisma() const							{ return get_stat(STAT_CHARISMA); }
    int charisma(int n, bool usetraits = true)		{ return upd_stat(STAT_CHARISMA, n, usetraits); }
    int happiness() const							{ return get_stat(STAT_HAPPINESS); }
    int happiness(int n, bool usetraits = true)		{ return upd_stat(STAT_HAPPINESS, n, usetraits); }
    int libido() const  							{ return get_stat(STAT_LIBIDO); }
    int libido(int n, bool usetraits = true)		{ return upd_stat(STAT_LIBIDO, n, usetraits); }
    int constitution() const						{ return get_stat(STAT_CONSTITUTION); }
    int constitution(int n, bool usetraits = true)	{ return upd_stat(STAT_CONSTITUTION, n, usetraits); }
    int intelligence() const						{ return get_stat(STAT_INTELLIGENCE); }
    int intelligence(int n, bool usetraits = true)	{ return upd_stat(STAT_INTELLIGENCE, n, usetraits); }
    int confidence() const							{ return get_stat(STAT_CONFIDENCE); }
    int confidence(int n, bool usetraits = true)	{ return upd_stat(STAT_CONFIDENCE, n, usetraits); }
    int mana() const								{ return get_stat(STAT_MANA); }
    int mana(int n, bool usetraits = true)			{ return upd_stat(STAT_MANA, n, usetraits); }
    int agility() const								{ return get_stat(STAT_AGILITY); }
    int agility(int n, bool usetraits = true)		{ return upd_stat(STAT_AGILITY, n, usetraits); }
    int strength() const							{ return get_stat(STAT_STRENGTH); }
    int strength(int n, bool usetraits = true)		{ return upd_stat(STAT_STRENGTH, n, usetraits); }
    int fame() const								{ return get_stat(STAT_FAME); }
    int fame(int n, bool usetraits = true)			{ return upd_stat(STAT_FAME, n, usetraits); }
    int level() const								{ return get_stat(STAT_LEVEL); }
    int level(int n, bool usetraits = true)			{ return upd_stat(STAT_LEVEL, n, usetraits); }
    int askprice() const							{ return get_stat(STAT_ASKPRICE); }
    int askprice(int n, bool usetraits = true)		{ return upd_stat(STAT_ASKPRICE, n, usetraits); }
    int house() const								{ return get_stat(STAT_HOUSE); }				/* It's NOT lupus! */
    int house(int n, bool usetraits = true)			{ return upd_stat(STAT_HOUSE, n, usetraits); }
    int exp() const									{ return get_stat(STAT_EXP); }
    int exp(int n, bool usetraits = true)			{ return upd_stat(STAT_EXP, n, usetraits); }
    int age() const									{ return get_stat(STAT_AGE); }
    int age(int n, bool usetraits = true)			{ return upd_stat(STAT_AGE, n, usetraits); }
    int obedience() const							{ return get_stat(STAT_OBEDIENCE); }
    int obedience(int n, bool usetraits = true)		{ return upd_stat(STAT_OBEDIENCE, n, usetraits); }
    int spirit() const								{ return get_stat(STAT_SPIRIT); }
    int spirit(int n, bool usetraits = true)		{ return upd_stat(STAT_SPIRIT, n, usetraits); }
    int beauty() const								{ return get_stat(STAT_BEAUTY); }
    int beauty(int n, bool usetraits = true)		{ return upd_stat(STAT_BEAUTY, n, usetraits); }
    int tiredness() const							{ return get_stat(STAT_TIREDNESS); }
    int tiredness(int n, bool usetraits = true)		{ return upd_stat(STAT_TIREDNESS, n, usetraits); }
    int health() const								{ return get_stat(STAT_HEALTH); }
    int health(int n, bool usetraits = true)		{ return upd_stat(STAT_HEALTH, n, usetraits); }
    int pcfear() const								{ return get_stat(STAT_PCFEAR); }
    int pcfear(int n, bool usetraits = true)		{ return upd_stat(STAT_PCFEAR, n, usetraits); }
    int pclove() const								{ return get_stat(STAT_PCLOVE); }
    int pclove(int n, bool usetraits = true)		{ return upd_stat(STAT_PCLOVE, n, usetraits); }
    int pchate() const								{ return get_stat(STAT_PCHATE); }
    int pchate(int n, bool usetraits = true)		{ return upd_stat(STAT_PCHATE, n, usetraits); }
    int morality() const							{ return get_stat(STAT_MORALITY); }
    int morality(int n, bool usetraits = true)		{ return upd_stat(STAT_MORALITY, n, usetraits); }
    int refinement() const							{ return get_stat(STAT_REFINEMENT); }
    int refinement(int n, bool usetraits = true)	{ return upd_stat(STAT_REFINEMENT, n, usetraits); }
    int dignity() const								{ return get_stat(STAT_DIGNITY); }
    int dignity(int n, bool usetraits = true)		{ return upd_stat(STAT_DIGNITY, n, usetraits); }
    int lactation() const							{ return get_stat(STAT_LACTATION); }
    int lactation(int n, bool usetraits = true)		{ return upd_stat(STAT_LACTATION, n, usetraits); }
    int npclove() const								{ return get_stat(STAT_NPCLOVE); }
    int npclove(int n, bool usetraits = true)		{ return upd_stat(STAT_NPCLOVE, n, usetraits); }
    int sanity() const								{ return get_stat(STAT_SANITY); }
    int sanity(int n, bool usetraits = true)		{ return upd_stat(STAT_SANITY, n, usetraits); }


    int rebel();
    std::string JobRating(double value, std::string type = "", std::string name = "");
    std::string JobRatingLetter(double value);
    bool FixFreeTimeJobs();
    /*
    *	notice that if we do tweak get_stat to reference the stats array
    *	direct, the above still work.
    *
    *	similarly...
    */
    int get_skill(int skill_id) const;

    int upd_temp_skill(int skill_id, int amount)
    {
        m_SkillTemps[skill_id] += amount;
        return get_skill(skill_id);
    }

    int upd_skill(int skill_id, int amount, bool usetraits = true);

    int	anal()										{ return get_skill(SKILL_ANAL); }
    int	anal(int n, bool usetraits = true)			{ return upd_skill(SKILL_ANAL, n, usetraits); }
    int	bdsm()										{ return get_skill(SKILL_BDSM); }
    int	bdsm(int n, bool usetraits = true)			{ return upd_skill(SKILL_BDSM, n, usetraits); }
    int	beastiality()								{ return get_skill(SKILL_BEASTIALITY); }
    int	beastiality(int n, bool usetraits = true)	{ return upd_skill(SKILL_BEASTIALITY, n, usetraits); }
    int	combat()									{ return get_skill(SKILL_COMBAT); }
    int	combat(int n, bool usetraits = true)		{ return upd_skill(SKILL_COMBAT, n, usetraits); }
    int	group()										{ return get_skill(SKILL_GROUP); }
    int	group(int n, bool usetraits = true)			{ return upd_skill(SKILL_GROUP, n, usetraits); }
    int	lesbian()									{ return get_skill(SKILL_LESBIAN); }
    int	lesbian(int n, bool usetraits = true)		{ return upd_skill(SKILL_LESBIAN, n, usetraits); }
    int	magic()										{ return get_skill(SKILL_MAGIC); }
    int	magic(int n, bool usetraits = true)			{ return upd_skill(SKILL_MAGIC, n, usetraits); }
    int	normalsex()									{ return get_skill(SKILL_NORMALSEX); }
    int	normalsex(int n, bool usetraits = true)		{ return upd_skill(SKILL_NORMALSEX, n, usetraits); }
    int oralsex()									{ return get_skill(SKILL_ORALSEX); }
    int oralsex(int n, bool usetraits = true)		{ return upd_skill(SKILL_ORALSEX, n, usetraits); }
    int tittysex()									{ return get_skill(SKILL_TITTYSEX); }
    int tittysex(int n, bool usetraits = true)		{ return upd_skill(SKILL_TITTYSEX, n, usetraits); }
    int handjob()									{ return get_skill(SKILL_HANDJOB); }
    int handjob(int n, bool usetraits = true)		{ return upd_skill(SKILL_HANDJOB, n, usetraits); }
    int footjob()									{ return get_skill(SKILL_FOOTJOB); }
    int footjob(int n, bool usetraits = true)			{ return upd_skill(SKILL_FOOTJOB, n, usetraits); }
    int	service()									{ return get_skill(SKILL_SERVICE); }
    int	service(int n, bool usetraits = true)			{ return upd_skill(SKILL_SERVICE, n, usetraits); }
    int	strip()										{ return get_skill(SKILL_STRIP); }
    int	strip(int n, bool usetraits = true)			{ return upd_skill(SKILL_STRIP, n, usetraits); }
    int	medicine()									{ return get_skill(SKILL_MEDICINE); }
    int	medicine(int n, bool usetraits = true)			{ return upd_skill(SKILL_MEDICINE, n, usetraits); }
    int	performance()								{ return get_skill(SKILL_PERFORMANCE); }
    int	performance(int n, bool usetraits = true)		{ return upd_skill(SKILL_PERFORMANCE, n, usetraits); }
    int	crafting()									{ return get_skill(SKILL_CRAFTING); }
    int	crafting(int n, bool usetraits = true)			{ return upd_skill(SKILL_CRAFTING, n, usetraits); }
    int	herbalism()									{ return get_skill(SKILL_HERBALISM); }
    int	herbalism(int n, bool usetraits = true)		{ return upd_skill(SKILL_HERBALISM, n, usetraits); }
    int	farming()									{ return get_skill(SKILL_FARMING); }
    int	farming(int n, bool usetraits = true)			{ return upd_skill(SKILL_FARMING, n, usetraits); }
    int	brewing()									{ return get_skill(SKILL_BREWING); }
    int	brewing(int n, bool usetraits = true)			{ return upd_skill(SKILL_BREWING, n, usetraits); }
    int	animalhandling()							{ return get_skill(SKILL_ANIMALHANDLING); }
    int	animalhandling(int n, bool usetraits = true)	{ return upd_skill(SKILL_ANIMALHANDLING, n, usetraits); }
    int	cooking()									{ return get_skill(SKILL_COOKING); }
    int	cooking(int n, bool usetraits = true)			{ return upd_skill(SKILL_COOKING, n, usetraits); }

    int get_enjoyment(int actiontype)
    {
        if (actiontype < 0) return 0;
        // Generic calculation
        int value = m_Enjoyment[actiontype] + m_EnjoymentTR[actiontype] +
                    m_EnjoymentMods[actiontype] + m_EnjoymentTemps[actiontype];

        if (value < -100) value = -100;
        else if (value > 100) value = 100;
        return value;
    }
    int get_training(int actiontype)
    {
        if (actiontype < 0) return 0;
        // Generic calculation
        int value = m_Training[actiontype] + m_TrainingMods[actiontype] + m_TrainingTemps[actiontype];

        if (value < 0) value = 0;
        else if (value > 100) value = 100;
        return value;
    }

    /*
    *	convenience func. Also easier to read like this
    */
    bool carrying_monster();
    bool carrying_human();
    bool carrying_players_child();
    bool carrying_customer_child();
    bool is_pregnant();
    bool is_mother();
    bool is_poisoned();
    bool has_weapon();
    void clear_pregnancy();
    void clear_dating();

    int preg_chance(int base_pc, bool good = false, double factor = 1.0);
    bool calc_pregnancy(int chance, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]);

    bool calc_pregnancy(int, cPlayer*);
    bool calc_pregnancy(cPlayer* player, bool good = false, double factor = 1.0);
    bool calc_insemination(cPlayer* player, bool good = false, double factor = 1.0);
    bool calc_group_pregnancy(cPlayer* player, bool good = false, double factor = 1.0);

    bool calc_pregnancy(int, sCustomer*);
    bool calc_pregnancy(const sCustomer& cust, bool good = false, double factor = 1.0);
    bool calc_insemination(const sCustomer& cust, bool good = false, double factor = 1.0);
    bool calc_group_pregnancy(const sCustomer& cust, bool good = false, double factor = 1.0);
    /*
    *	let's overload that...
    *	should be able to do the same using sCustomer as well...
    */
    bool add_trait(std::string trait, int temptime = 0, bool removeitem = false, bool remember = false);
    bool remove_trait(std::string trait,  bool addrememberlist = false, bool force = false, bool keepinrememberlist = false);
    bool has_trait(std::string trait) const;
    bool has_temp_trait(std::string trait) const;

    bool check_virginity();
    bool lose_virginity();
    bool regain_virginity();
    int breast_size();
    bool is_dead(bool sendmessage = false) const;		// `J` replaces a few DeadGirl checks
    bool is_addict(bool onlyhard = false);	// `J` added bool onlyhard to allow only hard drugs to be checked for
    bool has_disease();
    bool is_fighter(bool canbehelped = false);
    sChild* next_child(sChild* child, bool remove = false);
    int preg_type(int image_type);
    sGirl* run_away();

    bool is_slave()			{ return (m_States & (1u << STATUS_SLAVE)) != 0; }
    bool is_free()			{ return !is_slave(); }
    void set_slave()		{ m_States |= (1 << STATUS_SLAVE); }
    bool is_monster()		{ return (m_States & (1 << STATUS_CATACOMBS)) != 0; }
    bool is_human()			{ return !is_monster(); }
    bool is_arena()			{ return (m_States & (1 << STATUS_ARENA)) != 0; }
    bool is_yourdaughter()	{ return (m_States & (1 << STATUS_YOURDAUGHTER)) != 0; }
    bool is_isdaughter()	{ return (m_States & (1 << STATUS_ISDAUGHTER)) != 0; }
    bool is_warrior()		{ return !is_arena(); }
    bool is_resting();
    bool is_havingsex();
    bool was_resting();

    void OutputGirlRow(std::string* Data, const std::vector<std::string>& columnNames);
    void OutputGirlDetailString(std::string& Data, const std::string& detailName);

    // END MOD

    // more useful functions
    int has_item(const std::string& item);
    int has_item_j(const std::string& item);
    int get_num_item_equiped(int Type);
    int add_inv(sInventoryItem* item);
    bool equip(int slot, bool force);
    bool can_equip(int num, bool force);
    bool remove_inv(int slot);

    bool disobey_check(int action, JOBS job=NUM_JOBS);

    void use_items();

    void add_tiredness();

    bool fights_back();
};

#endif //CRAZYS_WM_MOD_SGIRL_HPP
