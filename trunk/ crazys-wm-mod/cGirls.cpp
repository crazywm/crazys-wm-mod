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
#include "cTariff.h"
#include "cGirls.h"
#include "cEvents.h"
#include "math.h"
#include <fstream>
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "tinyxml.h"
#include "XmlMisc.h"
#include "cGangs.h"
#include "CGraphics.h"
#include <algorithm>
#include "cMovieStudio.h"

#ifdef LINUX
#include "linux.h"
#endif
#include "DirPath.h"
#include "FileList.h"
#include "libintl.h"

using namespace std;

extern cMessageQue g_MessageQue;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager  g_Studios;
extern cTraits g_Traits;
extern cEvents g_Events;
extern cInventory g_InvManager;
extern sGirl* MarketSlaveGirls[8];
extern char buffer[1000];
extern CGraphics g_Graphics;
extern cRng g_Dice;
extern bool g_GenGirls;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern int g_Building;
extern cGirls g_Girls;

/*
 * MOD: DocClox: Stuff for the XML loader code
 *
 * first: static members need declaring
 */
bool sGirl::m_maps_setup = false;
map<string,unsigned int> sGirl::stat_lookup;
map<string,unsigned int> sGirl::skill_lookup;
map<string,unsigned int> sGirl::status_lookup;

const char *sGirl::stat_names[] = 
{

	"Charisma",
	"Happiness",
	"Libido",
	"Constitution",
	"Intelligence",
	"Confidence",
	"Mana",
	"Agility",
	"Fame",
	"Level",
	"AskPrice",
	"House",
	"Exp",
	"Age",
	"Obedience",
	"Spirit",
	"Beauty",
	"Tiredness",
	"Health",
	"PCFear",
	"PCLove",
	"PCHate"
};
/*
 * calculate the max like this, and it's self-maintaining
 */
const unsigned int sGirl::max_stats = (
	sizeof(sGirl::stat_names) / sizeof(sGirl::stat_names[0])
);

/*
 * same again for skill names
 */
const char *sGirl::skill_names[] = 
{
	"Anal",
	"Magic",
	"BDSM",
	"NormalSex",
	"Beastiality",
	"Group",
	"Lesbian",
	"Service",
	"Strip",
	"Combat",
	"OralSex",
	"TittySex"
};
const unsigned int sGirl::max_skills = (
	sizeof(sGirl::skill_names) / sizeof(sGirl::skill_names[0])
);

const char *sGirl::status_names[] =
{
	"None",
	"Poisoned",
	"Badly Poisoned",
	"Pregnant",
	"Pregnant By Player",
	"Slave",
	"Has Daughter",
	"Has Son",
	"Inseminated",
	"Controlled",
	"Catacombs",
	"Arena"
};
const unsigned int sGirl::max_statuses = (
	sizeof(sGirl::status_names) / sizeof(sGirl::status_names[0])
);

// ----- Lookups
void sGirl::setup_maps()
{

		//if(m_maps_setup) 
		//	return;	// only need to do this once

		g_LogFile.os() << "[sGirl::setup_maps] Setting up Stats, Skills and Status codes."<< endl;

		m_maps_setup = true;
		stat_lookup["Charisma"]		= STAT_CHARISMA;
		stat_lookup["Happiness"]	= STAT_HAPPINESS;
		stat_lookup["Libido"]		= STAT_LIBIDO;
		stat_lookup["Constitution"]	= STAT_CONSTITUTION;
		stat_lookup["Intelligence"]	= STAT_INTELLIGENCE;
		stat_lookup["Confidence"]	= STAT_CONFIDENCE;
		stat_lookup["Mana"]			= STAT_MANA;
		stat_lookup["Agility"]		= STAT_AGILITY;
		stat_lookup["Fame"]			= STAT_FAME;
		stat_lookup["Level"]		= STAT_LEVEL;
		stat_lookup["AskPrice"]		= STAT_ASKPRICE;
		stat_lookup["House"]		= STAT_HOUSE;
		stat_lookup["Exp"]			= STAT_EXP;
		stat_lookup["Age"]			= STAT_AGE;
		stat_lookup["Obedience"]	= STAT_OBEDIENCE;
		stat_lookup["Spirit"]		= STAT_SPIRIT;
		stat_lookup["Beauty"]		= STAT_BEAUTY;
		stat_lookup["Tiredness"]	= STAT_TIREDNESS;
		stat_lookup["Health"]		= STAT_HEALTH;
		stat_lookup["PCFear"]		= STAT_PCFEAR;
		stat_lookup["PCLove"]		= STAT_PCLOVE;
		stat_lookup["PCHate"]		= STAT_PCHATE;

		skill_lookup["Anal"]		= SKILL_ANAL;
		skill_lookup["Magic"]		= SKILL_MAGIC;
		skill_lookup["BDSM"]		= SKILL_BDSM;
		skill_lookup["NormalSex"]	= SKILL_NORMALSEX;
		skill_lookup["OralSex"]		= SKILL_ORALSEX;
		skill_lookup["TittySex"]	= SKILL_TITTYSEX;
		skill_lookup["Beastiality"]	= SKILL_BEASTIALITY;
		skill_lookup["Group"]		= SKILL_GROUP;
		skill_lookup["Lesbian"]		= SKILL_LESBIAN;
		skill_lookup["Service"]		= SKILL_SERVICE;
		skill_lookup["Strip"]		= SKILL_STRIP;
		skill_lookup["Combat"]		= SKILL_COMBAT;

		//	WD: Missing mapping for status
		status_lookup["None"]				= STATUS_NONE; 
		status_lookup["Poisoned"]			= STATUS_POISONED;
		status_lookup["Badly Poisoned"]		= STATUS_BADLY_POISONED;
		status_lookup["Pregnant"]			= STATUS_PREGNANT;
		status_lookup["Pregnant By Player"]	= STATUS_PREGNANT_BY_PLAYER;
		status_lookup["Slave"]				= STATUS_SLAVE;
		status_lookup["Has Daughter"]		= STATUS_HAS_DAUGHTER;
		status_lookup["Has Son"]			= STATUS_HAS_SON;
		status_lookup["Inseminated"]		= STATUS_INSEMINATED;
		status_lookup["Controlled"]			= STATUS_CONTROLLED;
		status_lookup["Catacombs"]			= STATUS_CATACOMBS;
		status_lookup["Arena"]			    = STATUS_ARENA;

}

int sGirl::lookup_skill_code(string s)
{
/*
 *	be useful to be able to log unrecognised
 *	type names here
 */
	if(skill_lookup.find(s) == skill_lookup.end()) {
		g_LogFile.os() << "[sGirl::lookup_skill_code] Error: unknown Skill: " <<
			s << endl;
		return -1;
	}
	return skill_lookup[s];
}

int sGirl::lookup_status_code(string s)
{
/*
 *	be useful to be able to log unrecognised
 *	type names here
 */
	if(status_lookup.find(s) == status_lookup.end()) {
		g_LogFile.os() << "[sGirl::lookup_status_code] Error: unknown Status: " <<
			s << endl;
		return -1;
	}
	return status_lookup[s];
}

int sGirl::lookup_stat_code(string s)
{
/*
 *	be useful to be able to log unrecognised
 *	type names here
 */
	if(stat_lookup.find(s) == stat_lookup.end()) {
		g_LogFile.os() << "[sGirl::lookup_stat_code] Error: unknown Stat: " <<
			s << endl;
		return -1;
	}
	return stat_lookup[s];
}

/*
 * END MOD
 */

class GirlPredicate_GRG : public GirlPredicate {
	bool m_slave;
	bool m_catacomb;
	bool m_arena;
public:
	GirlPredicate_GRG(bool slave, bool catacomb, bool arena) {
		m_slave = slave;
		m_catacomb = catacomb;
		m_arena = arena;
	}
		virtual bool test(sGirl *girl) {
		return  girl->is_slave() == m_slave 
		&&	girl->is_monster() == m_catacomb
		&&	girl->is_arena() == m_arena;
	}
};


// ----- Create / destroy

cGirls::cGirls()
{
	m_DefImages = 0;
	m_Parent = 0;
	m_Last = 0;
	m_NumRandomGirls = m_NumGirls = 0;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
	names.load(DirPath() << "Resources" << "Data" << "RandomGirlNames.txt");
}

cGirls::~cGirls()
{
	Free();
	m_ImgListManager.Free();
}

void cGirls::Free()
{
	if(m_Parent)
		delete m_Parent;
	m_Parent = m_Last = 0;
	m_NumGirls = 0;
	g_GenGirls = false;
	if(m_RandomGirls)
		delete m_RandomGirls;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
	m_NumRandomGirls=0;

	m_DefImages = 0;
}

// ----- Utility

static char *n_strdup(const char *s)
{
	return strcpy(new char[strlen(s)+1], s);
}

sGirl *sRandomGirl::lookup = new sGirl();  // used to look up stat and skill IDs

// ----- Misc

/*
 * if this returns true, the girl will disobey
 */
bool cGirls::DisobeyCheck(sGirl* girl, int action, sBrothel* brothel)
{
	int diff;
	int chance_to_obey = 0;		// high value - more likely to obey
/*
 *	let's start out with the basic rebelliousness
 */
	chance_to_obey = -GetRebelValue(girl, false);
/*
 *	let's normalise that:
 *	multiply by -1 to make high values more obedient;
 *	add 100 to make it range from 0 to 200
 *	and then divide by 2 to get a conventional percentage value
 */
	//chance_to_obey *= -1;
	chance_to_obey += 100;
	chance_to_obey /= 2;
/*
 *	OK, let's factor in having a matron: normally this is done in GetRebelValue
 *	but matrons have shifts now, so really we want twice the effect for a matron
 *	on each shift as we'd get from just one. Either that, or we need to make this
 *	check shift dependent.
 *
 *	Anyway, the old version added 15 for a matron either shift. Let's add
 *	8 for each shift. Full coverage gets you 16 points
 */
	if(brothel) {
		if(brothel->matron_on_shift(SHIFT_DAY)) chance_to_obey += 10;
		if(brothel->matron_on_shift(SHIFT_NIGHT)) chance_to_obey += 10;
	}
/*
 *	This is still confusing - at least it still confuses me
 *	why not normalise the rebellion -100 to 100 value so it runs
 *	0 to 100, and invert it so it's basically an obedience check
 */

	switch(action) {
	case ACTION_COMBAT:
/*
 *		I thought I did this before - must have been lost in the merge
 *		somewhere.
 *
 *		anyway, separate combat checks into two - half
 *		for magic and half for combat. If a girl is 
 *		an archmage in her spare time, but doesn't know which
 *		end of a rapier to hold - that ought to cancel out
 *
 *		Also, let's make this a sliding scale: 60% is the break-even
 *		point (whores *should* require some training before they'll work
 *		as soldiers) and for every 5 points above or below that
 *		there's a + or -1 modifier
 */

#if 1	// WD use best stat as many girls have only one stat high	
		
		diff = max(girl->combat(), girl->magic()) - 50;
		diff /= 3;
#else
		diff = girl->combat() - 50;
		diff /= 5;
		chance_to_obey += diff;
		diff = girl->magic() - 50;
		diff /= 5;
#endif
		chance_to_obey += diff;
		break;
	case ACTION_SEX:
/*
 *		Let's do the same thing here
 *
 *		Just noticed that high libido was lowering the chances
 *		of obedience...
 */
		diff = girl->libido() /*- 40*/;  // MYR
		diff /= 5;
		chance_to_obey += diff;
		break;

	default:
		break;
	}
/*
 *	add in her enjoyment level
 */
	chance_to_obey += girl->m_Enjoyment[action];
/*
 *	let's add in some mods for love, fear and hate
 */
	chance_to_obey += girl->pclove() / 10;
	chance_to_obey += girl->pcfear() / 10;
	chance_to_obey -= girl->pchate() / 10;
/*
 *	Let's add a blanket 30% to all of that
 */
	chance_to_obey += 30;
/*
 *	let's get a percentage roll
 */
	int roll = g_Dice.d100();
	diff = chance_to_obey - roll;
	bool girl_obeys = (diff >= 0);
/*
 *	there's a price to be paid for relying on love or fear
 *
 *	if the only reason she obeys is love it wears away that love
 *	just a little bit. And if she's only doing it out of fear
 */
	if(girl_obeys) {
		if(diff < (girl->pclove() / 10)) girl->pclove(-1);
		if(diff < (girl->pcfear() / 10)) girl->pcfear(-1);
	}
/*
 *	do we need any more than this, really?
 *	we can add in some shaping factors if desired
 */

// This was supposed to be overriden by previous calculation... It was basically rolling for disobedience twice -PP
//	return g_Dice.percent(100 - chance_to_obey);  
		return !girl_obeys;
}

void cGirls::CalculateGirlType(sGirl* girl)
{
	int BigBoobs = 0;
	int CuteGirl = 0;
	int Dangerous = 0;
	int Cool = 0;
	int Nerd = 0;
	int NonHuman = 0;
	int Lolita = 0;
	int Elegant = 0;
	int Sexy = 0;
	int NiceFigure = 0;
	int NiceArse = 0;
	int SmallBoobs = 0;
	int Freak = 0;

	// init the types to 0
	girl->m_FetishTypes = 0;

	if(HasTrait(girl, "Big Boobs"))
	{
		BigBoobs += 60;
		CuteGirl -= 5;
		Sexy += 10;
		NiceFigure = 5;
		SmallBoobs -= 50;
	}
	if(HasTrait(girl, "Abnormally Large Boobs"))
	{
		BigBoobs += 85;
		CuteGirl -= 15;
		NonHuman += 5;
		Freak += 20;
		SmallBoobs -= 50;
	}
	if(HasTrait(girl, "Small Scars"))
	{
		CuteGirl -= 5;
		Dangerous += 5;
		Cool += 2;
		Freak += 2;
	}
	if(HasTrait(girl, "Cool Scars"))
	{
		CuteGirl -= 10;
		Dangerous += 20;
		Cool += 30;
		Freak += 5;
	}
	if(HasTrait(girl, "Horrific Scars"))
	{
		CuteGirl -= 15;
		Dangerous += 30;
		Freak += 20;
	}
	if(HasTrait(girl, "Cool Person"))
	{
		Dangerous += 5;
		Cool += 60;
		Nerd -= 10;
	}
	if(HasTrait(girl, "Nerd"))
	{
		CuteGirl += 10;
		Dangerous -= 30;
		Cool -= 30;
		Nerd += 60;
		SmallBoobs += 5;
	}
	if(HasTrait(girl, "Clumsy"))
	{
		CuteGirl += 10;
		Dangerous -= 20;
		Cool -= 10;
		Nerd += 20;
		Freak += 5;
	}
	if(HasTrait(girl, "Fast orgasms"))
	{
		Cool += 10;
		Sexy += 30;
	}
	if(HasTrait(girl, "Slow orgasms"))
	{
		CuteGirl -= 5;
		Cool -= 5;
		Elegant += 5;
		Sexy -= 10;
		Freak += 5;
	}
	if(HasTrait(girl, "Quick Learner"))
	{
		Cool -= 20;
		Nerd += 30;
	}
	if(HasTrait(girl, "Slow Learner"))
	{
		CuteGirl += 10;
		Cool += 10;
		Nerd -= 20;
	}
	if(HasTrait(girl, "Cute"))
	{
		CuteGirl += 60;
 		Lolita += 20;//Really makes no sense to me CRAZY
		SmallBoobs += 5;
	}
	if(HasTrait(girl, "Strong"))
	{
		Dangerous += 20;
		Cool += 20;
		Nerd -= 30;
		NiceFigure += 20;
		NiceArse += 20;
	}
	if(HasTrait(girl, "Psychic"))
	{
		Dangerous += 10;
		Nerd += 10;
		NonHuman += 10;
		Freak += 10;
	}
	if(HasTrait(girl, "Strong Magic"))
	{
		Dangerous += 20;
		Nerd += 5;
		NonHuman += 5;
		Freak += 20;
	}
	if(HasTrait(girl, "Shroud Addict"))
	{
		Dangerous += 5;
		Cool += 15;
		Nerd -= 10;
		Elegant -= 20;
		Sexy -= 20;
		Freak += 10;
	}
	if(HasTrait(girl, "Fairy Dust Addict"))
	{
		Dangerous += 10;
		Cool += 20;
		Nerd -= 15;
		Elegant -= 25;
		Sexy -= 25;
		Freak += 15;
	}
	if(HasTrait(girl, "Viras Blood Addict"))
	{
		Dangerous += 15;
		Cool += 25;
		Nerd -= 20;
		Elegant -= 30;
		Sexy -= 30;
		Freak += 20;
	}
	if(HasTrait(girl, "Aggressive"))
	{
		CuteGirl -= 15;
		Dangerous += 20;
		Lolita -= 5;
		Elegant -= 10;
		Freak += 10;
	}
	if(HasTrait(girl, "Not Human"))
	{
		NonHuman += 60;
		Freak += 10;
	}
	if(HasTrait(girl, "Adventurer"))
	{
		Dangerous += 20;
		Cool += 10;
		Nerd -= 20;
		Elegant -= 5;
	}
	if(HasTrait(girl, "Assassin"))
	{
		Dangerous += 25;
		Cool += 15;
		Nerd -= 25;
		Freak += 10;
	}
	if(HasTrait(girl, "Lolita"))
	{
		BigBoobs -= 30;
		CuteGirl += 30;
		Dangerous -= 5;
		Lolita += 60;
		SmallBoobs += 15;
	}
	if(HasTrait(girl, "Nervous"))
	{
		CuteGirl += 10;
		Nerd += 15;
	}
	if(HasTrait(girl, "Good Kisser"))
	{
		Cool += 10;
		Sexy += 20;
	}
	if(HasTrait(girl, "Nymphomaniac"))
	{
		Sexy += 15;
		Freak += 20;
	}
	if(HasTrait(girl, "Elegant"))
	{
		Dangerous -= 30;
		Nerd -= 20;
		NonHuman -= 20;
		Elegant += 60;
		Freak -= 30;
	}
	if(HasTrait(girl, "Fake orgasm expert"))
	{
		Sexy += 5;
	}
	if(HasTrait(girl, "Sexy Air"))
	{
		Cool += 5;
		Elegant -= 5;
		Sexy += 10;
	}
	if(HasTrait(girl, "Great Figure"))
	{
		BigBoobs += 10;
		Sexy += 10;
		NiceFigure += 60;
	}
	if(HasTrait(girl, "Great Arse"))
	{
		Sexy += 10;
 		NiceArse += 60;
	}
	if(HasTrait(girl, "Small Boobs"))
	{
		BigBoobs -= 60;
		CuteGirl += 25;
		Lolita += 15;
		SmallBoobs += 60;
	}
	if(HasTrait(girl, "Broken Will"))
	{
		Cool -= 40;
		Nerd -= 40;
		Elegant -= 40;
		Sexy -= 40;
		Freak += 40;
	}
	if(HasTrait(girl, "Masochist"))
	{
		CuteGirl -= 10;
		Nerd -= 10;
		CuteGirl -= 15;
		Dangerous += 10;
		Elegant -= 10;
		Freak += 30;
	}
	if(HasTrait(girl, "Sadistic"))
	{
		CuteGirl -= 20;
		Dangerous += 15;
		Nerd -= 10;
		Elegant -= 30;
		Sexy -= 10;
		Freak += 30;
	}
	if(HasTrait(girl, "Tsundere") || HasTrait(girl, "Yandere"))
	{
		Dangerous += 5;
		Cool += 5;
		Nerd -= 5;
		Elegant -= 20;
		Freak += 10;
	}
	if(HasTrait(girl, "Meek"))
	{
		CuteGirl += 15;
		Dangerous -= 30;
		Cool -= 30;
		Nerd += 30;
	}
	if(HasTrait(girl, "Manly"))
	{
		CuteGirl -= 15;
		Dangerous += 5;
		Elegant -= 20;
		Sexy -= 20;
		NiceFigure -= 20;
		SmallBoobs += 10;
		Freak += 5;
	}
	if(HasTrait(girl, "Merciless"))
	{
		CuteGirl -= 20;
		Dangerous += 20;
		Nerd -= 10;
		Elegant -= 5;
	}
	if(HasTrait(girl, "Fearless"))
	{
		Dangerous += 20;
		Cool += 15;
		Nerd -= 10;
		Elegant -= 10;
	}
	if(HasTrait(girl, "Iron Will"))
	{
		Dangerous += 10;
		Cool += 10;
		Nerd -= 5;
		Elegant -= 10;
	}
	if(HasTrait(girl, "Twisted"))
	{
		CuteGirl -= 40;
		Dangerous += 30;
		Elegant -= 30;
		Sexy -= 20;
		Freak += 40;
	}
	if(HasTrait(girl, "Optimist"))
	{
		Elegant += 5;
	}
	if(HasTrait(girl, "Pessimist"))
	{
		Elegant -= 5;
	}
	if(HasTrait(girl, "Dependant"))
	{
		CuteGirl += 5;
		Dangerous -= 20;
		Cool -= 5;
		Nerd += 5;
		Elegant -= 20;
		Sexy -= 20;
		Freak += 10;
	}
	if(HasTrait(girl, "Sterile"))
	{
		Freak += 20;
	}
	if(HasTrait(girl, "Fleet of Foot"))
	{
		Dangerous += 10;
		Sexy += 20;
	}
	if(HasTrait(girl, "Tough"))
	{
		CuteGirl -= 5;
		Dangerous += 10;
		Cool += 10;
		Nerd -= 5;
		Elegant -= 5;
	}
	if(HasTrait(girl, "One Eye"))
	{
		CuteGirl -= 20;
		Cool += 5;
		Dangerous += 10;
		Sexy -= 20;
		NiceFigure -= 10;
		Freak += 20;
	}
	if(HasTrait(girl, "Eye Patch"))
	{
		CuteGirl -= 5;
		Dangerous += 5;
		Cool += 20;
		Sexy -= 5;
		Freak += 20;
	}
	if(HasTrait(girl, "Futanari"))
	{
		CuteGirl -= 15;
		NonHuman += 10;
		Freak += 30;
	}
	if(HasTrait(girl, "Construct"))
	{
		Dangerous += 10;
 		NonHuman += 60;
		Freak += 20;
	}
	if(HasTrait(girl, "Half-Construct"))
	{
		Dangerous += 5;
		NonHuman += 20;
		Freak += 20;
	}
	if(HasTrait(girl, "Fragile"))
	{
		CuteGirl += 10;
		Nerd += 5;
		Freak += 10;
	}
	if(HasTrait(girl, "Mind Fucked"))
	{
		CuteGirl -= 60;
		Dangerous -= 60;
		Cool -= 60;
		Nerd -= 60;
		Elegant -= 60;
		Sexy -= 60;
		Freak += 40;
	}
	if(HasTrait(girl, "Charismatic"))
	{
		Elegant += 30;
		Sexy += 30;
		Freak -= 20;
	}
	if(HasTrait(girl, "Charming"))
	{
		Elegant += 20;
		Sexy += 20;
		Freak -= 15;
	}
	if(HasTrait(girl, "Long Legs"))
	{
		Sexy += 20;
		NiceFigure += 20;
	}
	if(HasTrait(girl, "Puffy Nipples"))
	{
		BigBoobs += 10;
		CuteGirl += 5;
		SmallBoobs += 10;
	}
	if(HasTrait(girl, "Perky Nipples"))
	{
		BigBoobs += 10;
		CuteGirl += 5;
		SmallBoobs += 10;
	}
	if(HasTrait(girl, "Different Colored Eyes"))
	{
 		NonHuman += 20;//again makes no sense to me CRAZY
		Freak += 10;
	}
	if(HasTrait(girl, "Strange Eyes"))
	{
 		NonHuman += 20;//ditto CRAZY
		Freak += 15;
	}
	if(HasTrait(girl, "Incorporeal"))
	{
		NonHuman += 60;
		Freak += 40;
	}
	if(HasTrait(girl, "MILF"))
	{
		Freak += 15;
	}
	if(HasTrait(girl, "Cat Girl"))
	{
		CuteGirl += 20;
		NonHuman += 60;
		Freak += 5;
	}
	if(HasTrait(girl, "Demon"))
	{
		Dangerous += 10;
		NonHuman += 60;
		Freak += 5;
	}
	if(HasTrait(girl, "Malformed"))
	{
		NonHuman += 10;
		Freak += 50;
	}
	if(HasTrait(girl, "Retarded"))
	{
		NonHuman += 2;
		Freak += 45;
	}
	if(HasTrait(girl, "Shape Shifter"))  //CRAZY added this trait
	{
		NonHuman += 35;
		Sexy += 20;
		NiceFigure += 40;
		Freak += 40;
	}
 	if(HasTrait(girl, "Queen"))
 	{
 		Elegant += 40;
 		Sexy += 20;
 		Freak -= 15;
 	}
 	if(HasTrait(girl, "Princess"))
 	{
 		Elegant += 40;
 		Sexy += 20;
 		Freak -= 15;
 	}
	if(HasTrait(girl, "Pierced Nipples"))
 	{
 		Elegant -= 20;
 		Sexy += 20;
 		Freak += 15;
 	}
	if(HasTrait(girl, "Pierced Tongue"))
 	{
 		Elegant -= 20;
 		Sexy += 20;
 		Freak += 15;
 	}
	if(HasTrait(girl, "Pierced Clit"))
 	{
 		Elegant += 20;
 		Sexy += 20;
 		Freak += 15;
 	}
	if(HasTrait(girl, "Gag Reflex"))
 	{
 		Elegant += 10;
 		Nerd += 20;
 		Freak -= 15;
 	}
	if(HasTrait(girl, "No Gag Reflex"))
 	{
 		Elegant -= 10;
 		Sexy += 10;
 		Freak += 15;
 	}

	if(BigBoobs > SmallBoobs)
	{
		if(BigBoobs > 50)
			girl->m_FetishTypes|=(1<<FETISH_BIGBOOBS);
	}
	else
	{
		if(SmallBoobs > 50)
			girl->m_FetishTypes|=(1<<FETISH_SMALLBOOBS);
	}

	if(CuteGirl > 50)
		girl->m_FetishTypes|=(1<<FETISH_CUTEGIRLS);

	if(Dangerous > 50)
		girl->m_FetishTypes|=(1<<FETISH_DANGEROUSGIRLS);

	if(Cool > 50)
		girl->m_FetishTypes|=(1<<FETISH_COOLGIRLS);

	if(Nerd > 50)
		girl->m_FetishTypes|=(1<<FETISH_NERDYGIRLS);

	if(NonHuman > 50)
		girl->m_FetishTypes|=(1<<FETISH_NONHUMAN);

	if(Lolita > 50)
		girl->m_FetishTypes|=(1<<FETISH_LOLITA);

	if(Elegant > 50)
		girl->m_FetishTypes|=(1<<FETISH_ELEGANT);

	if(Sexy > 50)
		girl->m_FetishTypes|=(1<<FETISH_SEXY);

	if(NiceFigure > 50)
		girl->m_FetishTypes|=(1<<FETISH_FIGURE);

	if(NiceArse > 50)
		girl->m_FetishTypes|=(1<<FETISH_ARSE);

	if(Freak > 50)
		girl->m_FetishTypes|=(1<<FETISH_FREAKYGIRLS);
}

bool cGirls::CheckGirlType(sGirl* girl, int type)
{
	if(type == FETISH_TRYANYTHING)
		return true;
	else if(girl->m_FetishTypes&(1<<type))
		return true;

	return false;
}

sGirl *sGirl::run_away()
{
	m_RunAway = 6;		// player has 6 weeks to retreive
	m_NightJob = m_DayJob = JOB_RUNAWAY;
	g_Brothels.AddGirlToRunaways(this);
	return 0;
}

void cGirls::CalculateAskPrice(sGirl* girl, bool vari)
{
	girl->m_Stats[STAT_ASKPRICE] = 0;
	SetStat(girl, STAT_ASKPRICE, 0);
	int askPrice = (int)(((GetStat(girl, STAT_BEAUTY)+GetStat(girl, STAT_CHARISMA))/2)*0.6f);	// Initial price
	askPrice += GetStat(girl, STAT_CONFIDENCE)/10;	// their confidence will make them think they are worth more
	askPrice += GetStat(girl, STAT_INTELLIGENCE)/10;	// if they are smart they know they can get away with a little more
	askPrice += GetStat(girl, STAT_FAME)/2;	// And lastly their fame can be quite useful too
	if(GetStat(girl, STAT_LEVEL) > 0)
		askPrice += GetStat(girl, STAT_LEVEL) * 10;  // MYR: Was * 1

/*
 *	I can't see the sense in reducing a slave's price
 *	if you can't sell slaves
 *
 *	if(girl->m_States&(1<<STATUS_SLAVE))
 *	{
 *		SlaveNeg = (int)((float)askPrice*0.45f);
 *		askPrice -= SlaveNeg;
 *	}
 */

	if(vari)
	{
		int minVariance = 0 - (g_Dice%10)+1;
		int maxVariance = (g_Dice%10)+1;
		int variance = ((g_Dice%10)+maxVariance)-minVariance;
		askPrice += variance;
	}

	if(askPrice > 100)
		askPrice = 100;

	UpdateStat(girl, STAT_ASKPRICE, askPrice);
}

sRandomGirl* cGirls::random_girl_at(u_int n)
{
	u_int i;
	sRandomGirl* current = m_RandomGirls;
/*
 *	if we try and get a random girl template
 *	that's numbered higher than the number of 
 *	templates... well we're not going to find it.
 *
 *	So let's cut to the chase
 */
	if(n >= m_NumRandomGirls) {
		return 0;
	}
/*
 *	loop through the linked list n times
 *	
 */
	for(i = 0; i < n; i++) {
		current = current->m_Next;
/*
 *		current should only be null at the end
 *		of the chain. Which we shouldn't get to
 *		since we know n < m_NumRandomGirls from 
 *		the above.
 *
 *		so if it IS null, we have an integrity error
 *		in the pointer chain.
 *
 *		is it too late to rewrite this using vector?
 */
		if(current == 0) {
			g_LogFile.os() << "broken chain in cGirls::random_girl_at"<< endl;
			return 0;
		}
	}
	return current;		// and there we (hopefully) are
}

sGirl* cGirls::CreateRandomGirl(int age, bool addToGGirls, bool slave, bool undead, bool NonHuman, bool childnaped, bool arena)
{
	cConfig cfg;
	sRandomGirl* current;
	int random_girl_index = 0;

	// Mod - removed s - should only need to call it once - docclox

	if(m_NumRandomGirls == 0) {
		return 0;
	}
/*
 *	pick a number between 0 and m_NumRandomGirls
 */
	random_girl_index = g_Dice%m_NumRandomGirls;
/*
 *	loop until we find a human/non-human template as required
 */
	while(true)
	{
		current = random_girl_at(random_girl_index);
/*
 *		if we couldn't find the girl (which should be impossible_
 *		we pick another number at random and see if that one works
 */
		if(current == 0) {
			random_girl_index = g_Dice%m_NumRandomGirls;
			continue;
		}
/*
 *		test for humanity - or lack of it as the case may be
 */
		if(NonHuman == (current->m_Human == 0))
			break;
/*
 *		She's either human when we wanted non-human
 *		or non-human when we wanted human
 *
 *		Either way, try again...
 */
		random_girl_index = g_Dice%m_NumRandomGirls;
	}

	if(!current)
	{
		return 0;
	}

	sGirl* newGirl = new sGirl();
	newGirl->m_AccLevel = 1;
	newGirl->m_States = 0;
	newGirl->m_NumTraits = 0;

	newGirl->m_Desc = current->m_Desc;		// Bugfix.. was populating description with name.

	newGirl->m_Name = new char[current->m_Name.length()+1];	// name
	strcpy(newGirl->m_Name, current->m_Name.c_str());

	g_LogFile.os() << gettext("getting money for ") << newGirl->m_Name << endl;
	g_LogFile.os() << gettext("template is ") << current->m_Name << endl;
	g_LogFile.os() << gettext("min money ") << current->m_MinMoney << endl;
	g_LogFile.os() << gettext("max money ") << current->m_MaxMoney << endl;

	newGirl->m_Money = (g_Dice%(current->m_MaxMoney-current->m_MinMoney))+current->m_MinMoney;	// money
	
	// skills
	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		if((int)current->m_MaxSkills[i] == (int)current->m_MinSkills[i])
			newGirl->m_Skills[i] = (int)current->m_MaxSkills[i];
		else if((int)current->m_MaxSkills[i] < (int)current->m_MinSkills[i])
			newGirl->m_Skills[i] = g_Dice%101;
		else
			newGirl->m_Skills[i] = (int)(g_Dice%((int)current->m_MaxSkills[i]-(int)current->m_MinSkills[i]))+(int)current->m_MinSkills[i];
	}

	// stats
	for(int i=0; i<NUM_STATS; i++)
	{
		if((int)current->m_MaxStats[i] == (int)current->m_MinStats[i])
			newGirl->m_Stats[i] = (int)current->m_MaxStats[i];
		else if((int)current->m_MaxStats[i] < (int)current->m_MinStats[i])
			newGirl->m_Stats[i] = g_Dice%101;
		else
			newGirl->m_Stats[i] = (int)(g_Dice%((int)current->m_MaxStats[i]-(int)current->m_MinStats[i]))+(int)current->m_MinStats[i];
	}

	for(int i=0; i<current->m_NumTraits; i++)	// add the traits
	{
		int chance = g_Dice%100+1;
		if(g_Traits.GetTrait(current->m_Traits[i]->m_Name))
		{
			if(chance <= (int)current->m_TraitChance[i])
			{
				if(!HasTrait(newGirl, current->m_Traits[i]->m_Name))
					AddTrait(newGirl, current->m_Traits[i]->m_Name);
			}
		}
		else
		{
			string message = "cGirls::CreateRandomGirl: ERROR: Trait '";
			message += current->m_Traits[i]->m_Name;
			message += "' from girl template ";
			message += current->m_Name;
			message += " doesn't exist or is spelled incorrectly.";
			g_MessageQue.AddToQue(message, 2);
		}
	}

	if(current->m_Human == 0)
		AddTrait(newGirl, "Not Human");
	
	newGirl->m_DayJob = JOB_RESTING;
	newGirl->m_NightJob = JOB_RESTING;

	if(!slave)
		newGirl->m_Stats[STAT_HOUSE] = 60;	// 60% is the norm
	else
		newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();	// 100% is the norm
	newGirl->m_Stats[STAT_FAME] = 0;
	if(age != 0)
		newGirl->m_Stats[STAT_AGE] = age;
	newGirl->m_Stats[STAT_HEALTH] = 100;
	newGirl->m_Stats[STAT_HAPPINESS] = 100;

#if 0  //crazy this seems to break slaves from beening set to 100 house by defualt
	if(!arena)
		newGirl->m_Stats[STAT_HOUSE] = 60;	// 60% is the norm
	else
		newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();	// 100% is the norm
	newGirl->m_Stats[STAT_FAME] = 0;
#endif
	
	if(age != 0)				// Repeat from above ??
		newGirl->m_Stats[STAT_AGE] = age;
	newGirl->m_Stats[STAT_HEALTH] = 100;
	newGirl->m_Stats[STAT_HAPPINESS] = 100;

	if(slave)
	{
		newGirl->m_AccLevel = 0;
		newGirl->m_States |= (1<<STATUS_SLAVE);
		newGirl->m_Money = 0;
	}

	if(arena)
	{
		newGirl->m_AccLevel = 0;
		newGirl->m_States |= (1<<STATUS_ARENA);
		newGirl->m_Money = 0;
	}

	if(age < 17)
		AddTrait(newGirl, "Lolita");

	if(g_Dice%100 <= 3)
		AddTrait(newGirl, "Shroud Addict");

	if(g_Dice%100 <= 2)
		AddTrait(newGirl, "Fairy Dust Addict");

	if(g_Dice%100 == 1)
		AddTrait(newGirl, "Viras Blood Addict");

	if(childnaped)	// this girl has been taken against her will so make her rebelious
	{
		newGirl->m_Stats[STAT_SPIRIT] = 100;
		newGirl->m_Stats[STAT_CONFIDENCE] = 100;
		newGirl->m_Stats[STAT_OBEDIENCE] = 0;
		newGirl->m_Stats[STAT_PCHATE] = 50;
	}

	newGirl->m_Next = 0;

	// If the girl is a slave or arena.. then make her more obedient.
	if(newGirl->m_States&(1<<STATUS_SLAVE))
	{
		newGirl->m_AccLevel = 0;
		if(((int)newGirl->m_Stats[STAT_OBEDIENCE] + 20) > 100)
			newGirl->m_Stats[STAT_OBEDIENCE] = 100;
		else
			newGirl->m_Stats[STAT_OBEDIENCE] += 20;
	}

	if(newGirl->m_States&(1<<STATUS_ARENA))
	{
		newGirl->m_AccLevel = 0;
		if(((int)newGirl->m_Stats[STAT_OBEDIENCE] + 20) > 100)
			newGirl->m_Stats[STAT_OBEDIENCE] = 100;
		else
			newGirl->m_Stats[STAT_OBEDIENCE] += 20;
	}

	// Load any girl images if available
	LoadGirlImages(newGirl);

	if(current->m_newRandom && (newGirl->m_GirlImages->m_Images[8].m_NumImages > 0))
	{
		if(current->m_newRandomTable == 0)
		{
			current->m_newRandomTable = new bool[newGirl->m_GirlImages->m_Images[8].m_NumImages];
			for(int i = 0;i < newGirl->m_GirlImages->m_Images[8].m_NumImages;i++)
				current->m_newRandomTable[i] = false;
		}
		int j = 3;
		do 
		{
			newGirl->m_newRandomFixed = g_Dice % newGirl->m_GirlImages->m_Images[8].m_NumImages;
			j--;
		} while((j > 0) && current->m_newRandomTable[newGirl->m_newRandomFixed]);
		current->m_newRandomTable[newGirl->m_newRandomFixed] = true;
	}
	else
		newGirl->m_newRandomFixed = -1;

/*		apply trait bonuses
 *		WD:	Duplicated stat bonuses of traits as
 * 			allready applied with addtrait() calls
 */

	//ApplyTraits(newGirl);

/*
 *		WD: remove any rembered traits created
 *			from trait incompatibilities
 */
		RemoveAllRememberedTraits(newGirl);

/*
 *	Now that everything is in there, time to give her a random name
 *
 *	we'll try five times for a unique name
 *	if that fails, we'll give her the last one found
 *	this should be ok - assuming that names don't have to be unique
 *
 *	If they do need to be unique, the game will slow drastically as
 *	the number of girls approaches the limit, and hang once it is surpassed.
 *
 *	So I'm assuming non-unique names are ok
 */
	string name;
	for(int i = 0; i < 5; i++) {
		name = names.random();
		if(NameExists(buffer)) {
			continue;
		}
		break;
	}
	newGirl->m_Realname = name;

	newGirl->m_Virgin = false;
	if(newGirl->m_Stats[STAT_AGE] <= 18)
	{
		newGirl->m_Stats[STAT_AGE] = 18;
		newGirl->m_Virgin = true;
	}
	else if(newGirl->m_Stats[STAT_AGE] == 18)
	{
		if(g_Dice%3 == 1)
			newGirl->m_Virgin = true;
	}
	else if(newGirl->m_Stats[STAT_AGE] <= 25)
	{
		int avg = 0;
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			if(i != SKILL_SERVICE)
				avg += (int)newGirl->m_Skills[i];
		}
		avg = avg/(NUM_SKILLS-1);

		if(avg < 30)
			newGirl->m_Virgin = true;
	}

	if(newGirl->m_Stats[STAT_AGE] > 20 && HasTrait(newGirl, "Lolita"))
		RemoveTrait(newGirl, "Lolita");

	if(newGirl->m_Stats[STAT_AGE] >= 26)
	{
		if(g_Dice%100 <= 20)
			AddTrait(newGirl, "MILF");
	}

	DirPath dp = DirPath()
		<< "Resources"
		<< "Characters"
		<< newGirl->m_Name
		<< "triggers.xml"
		;
	newGirl->m_Triggers.LoadList(dp);
	newGirl->m_Triggers.SetGirlTarget(newGirl);

	if(addToGGirls)
		AddGirl(newGirl);

	CalculateGirlType(newGirl);

	return newGirl;
}

bool cGirls::NameExists(string name)
{
	sGirl* current = m_Parent;

	while(current)
	{
		if(current->m_Realname == name)
			return true;
		current = current->m_Next;
	}

	if(g_Brothels.NameExists(name))
		return true;

	for(int i=0; i<8; i++)
	{
		if(MarketSlaveGirls[i])
		{
			if(MarketSlaveGirls[i]->m_Realname == name)
				return true;
		}
	}

	return false;
}

void cGirls::LevelUp(sGirl* girl)
{
	stringstream ss;

	// MYR: Changed from 20.  Nothing appears to be level-dependant (beyond sell price) so
	//      this shouldn't affect anything (I hope. Feel free to revert if this breaks
	//      something.)
	//      Fixed the rest of the fn to be consistent with this change.
	if(GetStat(girl, STAT_LEVEL) >= 255)    // Unsigned char's max value is 255
		return;

	SetStat(girl, STAT_EXP, 0);

	if (GetStat(girl, STAT_LEVEL) <= 20)
		LevelUpStats(girl);

	UpdateStat(girl, STAT_LEVEL, 1);

	ss << girl->m_Realname << gettext(" levelled up to ") << girl->level() << ".";  

	// add traits
	// MYR: One chance to get a new trait every five levels.
	if(GetStat(girl, STAT_LEVEL)%5 == 0)
	{
		int chance = g_Dice%100;
		if(chance <= 10 && !HasTrait(girl, "Good Kisser"))
		{
			AddTrait(girl, "Good Kisser");
			ss << gettext(" She has gained the Good Kisser trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 20 && !HasTrait(girl, "Nymphomaniac"))
		{
			AddTrait(girl, "Nymphomaniac");
			ss << gettext(" She has gained the Nymphomaniac trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 30 && !HasTrait(girl, "Fake orgasm expert"))
		{
			AddTrait(girl, gettext("Fake orgasm expert"));
			ss << gettext(" She has gained the Fake Orgasm Expert trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 40 && !HasTrait(girl, "Sexy Air"))
		{
			AddTrait(girl, "Sexy Air");
			ss << gettext(" She has gained the Sexy Air trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 50 && !HasTrait(girl, "Fleet of Foot"))
		{
			AddTrait(girl, "Fleet of Foot");
			ss << gettext(" She has gained the Fleet of Foot trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 60 && !HasTrait(girl, "Charismatic"))
		{
			AddTrait(girl, "Charismatic");
			ss << gettext(" She has gained the Charismatic trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}
		else if(chance <= 70 && !HasTrait(girl, "Charming"))
		{
			AddTrait(girl, "Charming");
			ss << gettext(" She has gained the Charming trait.");
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);                
		}

		// A bug appeared to supress this message. Instead of making it global it now appears
		// on the girl's regular message list.
		//g_MessageQue.AddToQue(ss.str(), 0);
	}
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
}

void cGirls::LevelUpStats(sGirl* girl)
{
	int DiceSize = 0;

	if(HasTrait(girl,"Quick Learner"))
		DiceSize = 4;
	else if(HasTrait(girl,"Slow Learner"))
		DiceSize = 2;
	else
		DiceSize = 3;

	// level up stats (only first 8 advance in levelups)
	for(int i=0; i<8; i++)
		UpdateStat(girl,i,(g_Dice%4)+1); 

	// level up skills
	for(u_int i=0; i<NUM_SKILLS; i++)
		UpdateSkill(girl,i,(g_Dice%4)+1);
}

// ----- Add remove

void cGirls::AddRandomGirl(sRandomGirl* girl)
{
	girl->m_Next = 0;
	if(m_RandomGirls)
		m_LastRandomGirls->m_Next = girl;
	else
		m_RandomGirls = girl;
	m_LastRandomGirls = girl;
	m_NumRandomGirls++;
}

void cGirls::AddGirl(sGirl* girl)
{
	girl->m_Prev = girl->m_Next = 0;
	if(m_Parent)
	{
		girl->m_Prev = m_Last;
		m_Last->m_Next = girl;
		m_Last = girl;
	}
	else
		m_Last = m_Parent = girl;
	m_NumGirls++;
}

void cGirls::RemoveGirl(sGirl* girl, bool deleteGirl)
{
	if(m_Parent == 0)
		return;

	bool match = false;

	sGirl* currGirl = m_Parent;
	while(currGirl)
	{
		if(currGirl == girl)
		{
			match = true;
			break;
		}
		currGirl = currGirl->m_Next;
	}

	if(match)
	{
		if(deleteGirl)
		{
			if(girl->m_Prev)
				girl->m_Prev->m_Next = girl->m_Next;
			if(girl->m_Next)
				girl->m_Next->m_Prev = girl->m_Prev;

			if(girl == m_Parent)
				m_Parent = girl->m_Next;
			if(girl == m_Last)
				m_Last = girl->m_Prev;

			girl->m_Next = girl->m_Prev = 0;
			delete girl;
			girl = 0;
		}
		else
		{
			if(girl->m_Prev)
				girl->m_Prev->m_Next = girl->m_Next;
			if(girl->m_Next)
				girl->m_Next->m_Prev = girl->m_Prev;

			if(girl == m_Parent)
				m_Parent = girl->m_Next;
			if(girl == m_Last)
				m_Last = girl->m_Prev;

			girl->m_Next = girl->m_Prev = 0;
		}
		m_NumGirls--;
	}
}

void cGirls::AddTiredness(sGirl* girl)
{
	if (g_Girls.HasTrait(girl, "Incorporeal")) // Sanity check
	{
		g_Girls.SetStat(girl, STAT_TIREDNESS, 0);
		return;
	}

	int tiredness = 10;
	if(g_Girls.GetStat(girl, STAT_CONSTITUTION) > 0)
		tiredness -= (g_Girls.GetStat(girl, STAT_CONSTITUTION))/10;
	if(tiredness <= 0)
		tiredness = 0;
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, tiredness);
	if(g_Girls.GetStat(girl, STAT_TIREDNESS) == 100)
	{
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -1);
		g_Girls.UpdateStat(girl, STAT_HEALTH, -1);
	}
}

// ----- Get

int cGirls::GetSlaveGirl(int from)
{
	sGirl* current = m_Parent;
	int num = 0;
	int girlnum=0;
	bool found = false;

	// count to the girl at the number from
	//while(current)
	//{
	//	if(num == from)
	//		break;
	//	num++;
	//	current = current->m_Next;
	//}

	while(current)
	{
		if(current->m_States&(1<<STATUS_SLAVE))
		{
			if(num==from)
			{
			found = true;
			break;
			}
			else
				num++;

		}
		girlnum++;
		current = current->m_Next;
	}

	if(found == false)
		return -1;

	return girlnum;
}

vector<sGirl *>  cGirls::get_girls(GirlPredicate* pred)
{
	sGirl *girl;
	vector<sGirl *> v;

	for(girl = m_Parent; girl; girl = girl->m_Next) {
		if(pred->test(girl)) {
			v.push_back(girl);
		}
	}
	return v;
}

string cGirls::GetGirlMood(sGirl* girl)
{
	string ret = gettext("Her Feelings: ");
	int variable = 0;

	int HateLove = GetStat(girl, STAT_PCLOVE) - GetStat(girl, STAT_PCHATE);
	ret += gettext("Feels the player ");
	if(HateLove < 0)
	{
		if(HateLove > -20)
			ret += gettext("is annoying ");
		else if(HateLove > -40)
			ret += gettext("isn't nice ");
		else if(HateLove > -60)
			ret += gettext("is mean ");
		else if(HateLove > -80)
			ret += gettext("is better off dead ");
		else
			ret += gettext("should die ");
	}
	else
	{
		if(HateLove < 20)
			ret += gettext("is ok ");
		else if(HateLove < 40)
			ret += gettext("is easy going ");
		else if(HateLove < 60)
			ret += gettext("is good ");
		else if(HateLove < 80)
			ret += gettext("is attractive ");
		else
			ret += gettext("is her true love ");
	}

	if(GetStat(girl, STAT_PCFEAR) > 20)
	{
		if(HateLove > 0)
			ret += gettext("but she is also ");
		else
			ret += gettext("and she is ");
		if(GetStat(girl, STAT_PCFEAR) < 40)
			ret += gettext("afraid of him.");
		else if(GetStat(girl, STAT_PCFEAR) < 60)
			ret += gettext("fearful of him.");
		else if(GetStat(girl, STAT_PCFEAR) < 80)
			ret += gettext("afraid he will hurt her.");
		else
			ret += gettext("afraid he will kill her.");
	}
	else
		ret += gettext("and he isn't scary.\n");

	variable = GetStat(girl, STAT_HAPPINESS);
	ret += gettext("She is ");
	if(variable > 90)
		ret += gettext("happy.\n");
	else if(variable > 80)
		ret += gettext("joyful.\n");
	else if(variable > 60)
		ret += gettext("reasonably happy.\n");
	else if(variable > 40)
		ret += gettext("unhappy.\n");
	else
		ret += gettext("showing signs of depression.\n");

	return ret;
}

string cGirls::GetMoreDetailsString(sGirl* girl)
{
	if(girl == 0)
		return string("");
	string data = gettext("Fetish Categories: ");

	if(CheckGirlType(girl, FETISH_BIGBOOBS))
		data += gettext(" |Big Boobs| ");
	if(CheckGirlType(girl, FETISH_CUTEGIRLS))
		data += gettext(" |Cute Girl| ");
	if(CheckGirlType(girl, FETISH_DANGEROUSGIRLS))
		data += gettext(" |Dangerous| ");
	if(CheckGirlType(girl, FETISH_COOLGIRLS))
		data += gettext(" |Cool| ");
	if(CheckGirlType(girl, FETISH_NERDYGIRLS))
		data += gettext(" |Nerd| ");
	if(CheckGirlType(girl, FETISH_NONHUMAN))
		data += gettext(" |Non or part human| ");
	if(CheckGirlType(girl, FETISH_LOLITA))
		data += gettext(" |Lolita| ");
	if(CheckGirlType(girl, FETISH_ELEGANT))
		data += gettext(" |Elegant| ");
	if(CheckGirlType(girl, FETISH_SEXY))
		data += gettext(" |Sexy| ");
	if(CheckGirlType(girl, FETISH_FIGURE))
		data += gettext(" |Nice Figure| ");
	if(CheckGirlType(girl, FETISH_ARSE))
		data += gettext(" |Nice Arse| ");
	if(CheckGirlType(girl, FETISH_SMALLBOOBS))
		data += gettext(" |Small Boobs| ");
	if(CheckGirlType(girl, FETISH_FREAKYGIRLS))
		data += gettext(" |Freaky| ");
	data += gettext("\n\n");

	string jobs[]={
		gettext("combat"),
		gettext("working as a whore"),
		gettext("doing miscellaneous tasks"),  // general
		gettext("cleaning"),
		gettext("acting as a matron"),
		gettext("working in the bar"), 
		gettext("working in the gambling hall"),
		gettext("producing movies"),
		gettext("providing security"),
		gettext("doing advertising"),
		gettext("torturing people"),
		gettext("caring for beasts"),
		gettext("working as a doctor"),
		gettext("producing movies"),
		gettext("providing customer service"),
		gettext("working in the centre"),
		gettext("working in the club"),
		gettext("being in your harem"),
		gettext("being a recruiter")
	};
	string base=gettext("She");
	string text;
	unsigned char count = 0;
	for(int i=0;i<NUM_ACTIONTYPES;++i)
	{
		if(jobs[i] == "")
			continue;
		if(girl->m_Enjoyment[i]<-70)
		{
			text=gettext(" hates ");
		}
		else if(girl->m_Enjoyment[i]<-50)
		{
			text=gettext(" really dislikes ");
		}
		else if(girl->m_Enjoyment[i]<-30)
		{
			text=gettext(" dislikes ");
		}
		else if(girl->m_Enjoyment[i]<-20)
		{
			text=gettext(" doesn't particularly enjoy ");
		}
		else if(girl->m_Enjoyment[i]<15)
		{
			//text=" is indifferent to ";
			continue;  // if she's indifferent, why specify it? Let's instead skip it.
		}
		else if(girl->m_Enjoyment[i]<30)
		{
			text=gettext(" is happy enough with ");
		}
		else if(girl->m_Enjoyment[i]<50)
		{
			text=gettext(" likes ");
		}
		else if(girl->m_Enjoyment[i]<70)
		{
			text=gettext(" really enjoys ");
		}
		else
		{
			text= gettext(" loves ");
		}
		data+=base;
		data+=text;
		data+=jobs[i]+gettext(".\n");
		count++;
	}
	if(count > 0)
		data+=gettext("\nShe is indifferent to all other tasks.\n\n");
	else
		data+=gettext("At the moment, she is indifferent to all tasks.\n\n");

	data += gettext("\nOther Stats\n\n");
	stringstream ss;
	ss << gettext("Charisma: ") << GetStat(girl, STAT_CHARISMA) << gettext("\n");
	ss << gettext("Beauty: ") << GetStat(girl, STAT_BEAUTY) << gettext("\n");
	ss << gettext("Libido: ") << GetStat(girl, STAT_LIBIDO) << gettext("\n");
	ss << gettext("Mana: ") << GetStat(girl, STAT_MANA) << gettext("\n");
	ss << gettext("Intelligence: ") << GetStat(girl, STAT_INTELLIGENCE) << gettext("\n");
	ss << gettext("Confidence: ") << GetStat(girl, STAT_CONFIDENCE) << gettext("\n");
	ss << gettext("Obedience: ") << GetStat(girl, STAT_OBEDIENCE) << gettext("\n");
	ss << gettext("Spirit: ") << GetStat(girl, STAT_SPIRIT) << gettext("\n");
	ss << gettext("Agility: ") << GetStat(girl, STAT_AGILITY) << gettext("\n");
	ss << gettext("Fame: ") << GetStat(girl, STAT_FAME) << gettext("\n");
	data += ss.str();

	//Job rating system  ///CRAZY
	int barmaid = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int barwait = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int cards = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetStat(girl, STAT_AGILITY))/2;  //intel makes her smart enough to know when to cheat agility makes her fast enough to cheat
	int dealer = (cards + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int sing = (g_Girls.GetStat(girl, STAT_CONFIDENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int piano = (g_Girls.GetStat(girl, STAT_CONFIDENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int entertainer = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int xxx = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int clubwait = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int clubbar = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int strip = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));
	int peep = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));
	int brothelstrip = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));
	int massusse = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
	{
		barmaid += 15;
		barwait += 15;
		sing += 15;
		piano += 10;
		dealer += 5;
		entertainer += 15;
		xxx += 10;
		clubwait += 10;
		clubbar += 20;
		strip += 10;
		peep += 10;
		brothelstrip += 20;
		massusse += 10;
	}
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
	{
		barmaid += 5;
		barwait += 10;
		sing += 5;
		piano += 5;
		dealer += 5;
		entertainer += 5;
		xxx += 10;
		clubwait += 10;
		clubbar += 10;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;
	}
	if (g_Girls.HasTrait(girl, "Cool Person"))
	{
		barmaid += 5;
		barwait += 10;
		sing += 5;
		piano += 5;
		dealer += 5;
		entertainer += 5;
		xxx += 5;
		clubwait += 10;
		clubbar += 10;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;
	}
	if (g_Girls.HasTrait(girl, "Cute"))  
	{
		barmaid += 5;
		barwait += 5;
		sing += 5;
		piano += 5;
		dealer += 5;
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;
	}
	if (g_Girls.HasTrait(girl, "Charming"))  
	{
		barmaid += 15;
		barwait += 20;
		sing += 5;
		piano += 5;
		dealer += 10;
		entertainer += 15;
		xxx += 5;
		clubwait += 15;
		clubbar += 15;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;
	}
	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		barmaid += 5;
		barwait += 5;
		sing += 5;
		piano += 5;
		dealer += 5;
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;
	}
	if (g_Girls.HasTrait(girl, "Psychic"))
	{
		barmaid += 10;
		barwait += 10;
		sing += 10;
		piano += 10;
		dealer += 15;
		entertainer += 15;
		xxx += 10;
		clubwait += 10;
		clubbar += 10;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;
	}
	if (g_Girls.HasTrait(girl, "Fleet of Foot")) 
	{
		//barmaid += 10;
		barwait += 5;
		/*sing += 10;
		piano += 10;
		dealer += 15;
		entertainer += 15;
		xxx += 10;*/
		clubwait += 5;
		/*clubbar += 10;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;*/
	}
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		{
		//barmaid += 10;
		barwait -= 20;
		/*sing += 10;
		piano += 10;
		dealer += 15;
		entertainer += 15;
		xxx += 10;*/
		clubwait -= 20;
		/*clubbar += 10;
		strip += 10;
		peep += 10;
		brothelstrip += 10;
		massusse += 10;*/
	}
	if (g_Girls.HasTrait(girl, "Great Figure"))
	{
		/*barmaid += 10;
		barwait -= 20;
		sing += 10;
		piano += 10;
		dealer += 15;*/
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;
	}
	if (g_Girls.HasTrait(girl, "Great Arse")) 
	{
		/*barmaid += 10;
		barwait -= 20;
		sing += 10;
		piano += 10;
		dealer += 15;*/
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;
	}
	if (g_Girls.HasTrait(girl, "Elegant"))
		{
		/*barmaid += 10;
		barwait -= 20;*/
		sing += 5;
		piano += 15;
		/*dealer += 15;
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;*/
	}
	if (g_Girls.HasTrait(girl, "Fearless"))
		{
		/*barmaid += 10;
		barwait -= 20;*/
		sing += 5;
		//piano += 15;
		/*dealer += 15;
		entertainer += 5;
		xxx += 5;
		clubwait += 5;
		clubbar += 5;
		strip += 5;
		peep += 5;
		brothelstrip += 5;
		massusse += 5;*/
	}

		//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
	{
		barmaid -= 50;
		barwait -= 50;
		sing -= 50;
		piano -= 50;
		dealer -= 50;
		entertainer -= 50;
		xxx -= 50;
		clubwait -= 50;
		clubbar -= 50;
		strip -= 50;
		peep -= 50;
		brothelstrip -= 50;
		massusse -= 50;
	}
	if (g_Girls.HasTrait(girl, "Clumsy"))  
	{
		barmaid -= 20;
		barwait -= 20;
		sing -= 10;
		//piano -= 20;
		dealer -= 10;
		entertainer -= 10;
		xxx -= 10;
		clubwait -= 20;
		clubbar -= 20;
		strip -= 20;
		peep -= 20;
		brothelstrip -= 20;
		massusse -= 20;
	}
	if (g_Girls.HasTrait(girl, "Aggressive"))
	{
		barmaid -= 20;
		barwait -= 20;
		sing -= 20;
		piano -= 20;
		dealer -= 20;
		entertainer -= 20;
		xxx -= 20;
		clubwait -= 20;
		clubbar -= 20;
		strip -= 20;
		peep -= 20;
		brothelstrip -= 20;
		massusse -= 20;
	}
	if (g_Girls.HasTrait(girl, "Nervous"))
	{
		barmaid -= 30;
		barwait -= 30;
		sing -= 30;
		piano -= 30;
		dealer -= 30;
		entertainer -= 30;
		xxx -= 30;
		clubwait -= 30;
		clubbar -= 30;
		strip -= 30;
		peep -= 30;
		brothelstrip -= 30;
		massusse -= 30;
	}
	if (g_Girls.HasTrait(girl, "Meek"))
	{
		barmaid -= 20;
		barwait -= 20;
		sing -= 20;
		piano -= 20;
		dealer -= 20;
		entertainer -= 20;
		xxx -= 20;
		clubwait -= 20;
		clubbar -= 20;
		strip -= 20;
		peep -= 20;
		brothelstrip -= 20;
		massusse -= 20;
	}
	if (g_Girls.HasTrait(girl, "Broken Will"))
	{
		//barmaid -= 20;
		//barwait -= 20;
		sing -= 50;
		piano -= 50;
		//dealer -= 20;
		entertainer -= 50;
		//xxx -= 20;
		//clubwait -= 20;
		//clubbar -= 20;
		//strip -= 20;
		//peep -= 20;
		//brothelstrip -= 20;
		//massusse -= 20;
	}
	if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		barmaid -= 10;
		barwait -= 10;
		sing -= 10;
		piano -= 10;
		dealer -= 15;
		entertainer -= 15;
		xxx -= 10;
		clubwait -= 10;
		clubbar -= 10;
		strip -= 10;
		peep -= 10;
		brothelstrip -= 10;
		massusse -= 10;
	}
	if (g_Girls.HasTrait(girl, "Horrific Scars"))
	{
		//barmaid -= 20;
		//barwait -= 20;
		//sing -= 50;
		//piano -= 50;
		//dealer -= 20;
		//entertainer -= 50;
		//xxx -= 20;
		//clubwait -= 20;
		//clubbar -= 20;
		strip -= 20;
		peep -= 20;
		brothelstrip -= 20;
		//massusse -= 20;
	}

		/*stringstream dd;
	dd << gettext("barmaid: ") << (barmaid) << gettext("\n");
	dd << gettext("barwait: ") << (barwait) << gettext("\n");
	dd << gettext("sing: ") << (sing) << gettext("\n");
	dd << gettext("piano: ") << (piano) << gettext("\n");
	data += dd.str();*/

	data += gettext("\nJob Ratings\n");

	data+=gettext("\nBarmaid- ");
	{
	if (barmaid >= 245)
		data+=gettext ("S");
	else if (barmaid >= 185)
		data+=gettext ("A");
	else if (barmaid >= 145)
		data+=gettext ("B");
	else if(barmaid >= 100)
		data+=gettext ("C");
	else if (barmaid >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nBar Waitress- ");
	{
	if (barwait >= 245)
		data+=gettext ("S");
	else if (barwait >= 185)
		data+=gettext ("A");
	else if (barwait >= 145)
		data+=gettext ("B");
	else if(barwait >= 100)
		data+=gettext ("C");
	else if (barwait >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nSinger- ");
	{
	if (sing >= 245)
		data+=gettext ("S");
	else if (sing >= 185)
		data+=gettext ("A");
	else if (sing >= 145)
		data+=gettext ("B");
	else if(sing >= 100)
		data+=gettext ("C");
	else if (sing >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nPiano- ");
	{
	if (piano >= 245)
		data+=gettext ("S");
	else if (piano >= 185)
		data+=gettext ("A");
	else if (piano >= 145)
		data+=gettext ("B");
	else if(piano >= 100)
		data+=gettext ("C");
	else if (piano >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nDealer- ");
	{
	if (dealer >= 245)
		data+=gettext ("S");
	else if (dealer >= 185)
		data+=gettext ("A");
	else if (dealer >= 145)
		data+=gettext ("B");
	else if(dealer >= 100)
		data+=gettext ("C");
	else if (dealer >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nEntertainer- ");
	{
	if (entertainer >= 245)
		data+=gettext ("S");
	else if (entertainer >= 185)
		data+=gettext ("A");
	else if (entertainer >= 145)
		data+=gettext ("B");
	else if(entertainer >= 100)
		data+=gettext ("C");
	else if (entertainer >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nXXX Entertainer- ");
	{
	if (xxx >= 245)
		data+=gettext ("S");
	else if (xxx >= 185)
		data+=gettext ("A");
	else if (xxx >= 145)
		data+=gettext ("B");
	else if(xxx >= 100)
		data+=gettext ("C");
	else if (xxx >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nClub Barmaid- ");
	{
	if (clubbar >= 245)
		data+=gettext ("S");
	else if (clubbar >= 185)
		data+=gettext ("A");
	else if (clubbar >= 145)
		data+=gettext ("B");
	else if(clubbar >= 100)
		data+=gettext ("C");
	else if (clubbar >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nClub Waitress- ");
	{
	if (clubwait >= 245)
		data+=gettext ("S");
	else if (clubwait >= 185)
		data+=gettext ("A");
	else if (clubwait >= 145)
		data+=gettext ("B");
	else if(clubwait >= 100)
		data+=gettext ("C");
	else if (clubwait >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nStripper- ");
	{
	if (strip >= 245)
		data+=gettext ("S");
	else if (strip >= 185)
		data+=gettext ("A");
	else if (strip >= 145)
		data+=gettext ("B");
	else if(strip >= 100)
		data+=gettext ("C");
	else if (strip >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nMassusse- ");
	{
	if (massusse >= 245)
		data+=gettext ("S");
	else if (massusse >= 185)
		data+=gettext ("A");
	else if (massusse >= 145)
		data+=gettext ("B");
	else if(massusse >= 100)
		data+=gettext ("C");
	else if (massusse >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nBrothel Stripper- ");
	{
	if (brothelstrip >= 245)
		data+=gettext ("S");
	else if (brothelstrip >= 185)
		data+=gettext ("A");
	else if (brothelstrip >= 145)
		data+=gettext ("B");
	else if(brothelstrip >= 100)
		data+=gettext ("C");
	else if (brothelstrip >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}
	data+=gettext("\nPeep Show- ");
	{
	if (peep >= 245)
		data+=gettext ("S");
	else if (peep >= 185)
		data+=gettext ("A");
	else if (peep >= 145)
		data+=gettext ("B");
	else if(peep >= 100)
		data+=gettext ("C");
	else if (peep >= 70)
		data+=gettext ("D");
	else
		data+=gettext ("E");
	}

	return data;
}

string cGirls::GetDetailsString(sGirl* girl, bool purchase)
{
	cConfig cfg;
	cTariff tariff;
	stringstream ss;
	char buffer[100];

	if(girl == 0)
		return string("");
	string data = gettext("Looks: ");
	int variable = ((GetStat(girl, STAT_BEAUTY)+GetStat(girl, STAT_CHARISMA))/2);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Age: ");
	if(GetStat(girl, STAT_AGE) == 100)
	{
		data += gettext("unknown");
	}
	else
	{
		_itoa(GetStat(girl, STAT_AGE), buffer, 10);
		data += buffer;
	}
	data += gettext("\n");

	data += gettext("Level: ");
	_itoa(GetStat(girl, STAT_LEVEL), buffer, 10);
	data += buffer;
	data += gettext(" | ");
	data += gettext("Exp: ");
	_itoa(GetStat(girl, STAT_EXP), buffer, 10);
	data += buffer;
	data += gettext("\n");

	if(girl->m_Virgin)
		data += gettext("She is a virgin\n");

	if(girl->m_Virgin)
	{
			AddTrait(girl, "Virgin");
			//UpdateEnjoyment(girl, ACTION_SEX, -20, true);
	}
	else
	{
		RemoveTrait(girl, "Virgin");
	}

	data += gettext("Rebelliousness: ");
	_itoa(GetRebelValue(girl, false), buffer, 10);
	data += buffer;
	data += gettext("\n");

	if(girl->m_States&(1<<STATUS_POISONED))
		data += gettext("Is poisoned\n");
	if(girl->m_States&(1<<STATUS_BADLY_POISONED))
		data += gettext("Is badly poisoned\n");

	int to_go = cfg.pregnancy.weeks_pregnant() - girl->m_WeeksPreg;
	if(girl->m_States&(1<<STATUS_PREGNANT))
	{
		//cerr << "config.weeks_preg: " << cfg.pregnancy.weeks_pregnant() << endl;
		//cerr << "to go            : " << to_go << endl;

		_itoa(to_go, buffer, 10);
		data += gettext("Is pregnant, due: ");
		data += buffer;
		data += gettext(" weeks\n");
	}
	if(girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
	{
		//cerr << "config.weeks_preg: " << cfg.pregnancy.weeks_pregnant() << endl;
		//cerr << "to go (player's) : " << to_go << endl;

		_itoa(to_go, buffer, 10);
		data += gettext("Is pregnant with your child, due: ");
		data += buffer;
		data += gettext(" weeks\n");
	}
	if(girl->m_States&(1<<STATUS_SLAVE))
		data += gettext("Is branded a slave\n");
	if(girl->m_States&(1<<STATUS_HAS_DAUGHTER))
		data += gettext("Has daughter\n");
	if(girl->m_States&(1<<STATUS_HAS_SON))
		data += gettext("Has Son\n");
	if(girl->m_States&(1<<STATUS_INSEMINATED))
	{
		_itoa(to_go, buffer, 10);
		data += gettext("Is inseminated, due: ");
		data += buffer;
		data += gettext(" weeks\n");
	}
	if(girl->m_PregCooldown != 0)
	{
		_itoa(((int)girl->m_PregCooldown), buffer, 10);
		data += gettext("Cannot get pregnant for: ");
		data += buffer;
		data += gettext(" weeks\n");
	}

	if(girl->is_addict())
		data += gettext("Has addiciton\n");

	variable = GetStat(girl, STAT_HAPPINESS);
	data += gettext("Happiness: ");
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Constitution: ");
	variable = (int)GetStat(girl, STAT_CONSTITUTION);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	if(!purchase)
	{
		data += gettext("Health: ");
		_itoa((int)GetStat(girl, STAT_HEALTH), buffer, 10);
		data += buffer;
		data += gettext("%\n");

		data += gettext("Tiredness: ");
		variable = (int)GetStat(girl, STAT_TIREDNESS);
		_itoa(variable, buffer, 10);
		data += buffer;
		data += gettext("%\n");

		data += gettext("Accomodation: ");
		if(girl->m_AccLevel == 0)
			data += gettext("Very Poor\n");
		if(girl->m_AccLevel == 1)
			data += gettext("Adequate\n");
		if(girl->m_AccLevel == 2)
			data += gettext("Nice\n");
		if(girl->m_AccLevel == 3)
			data += gettext("Good\n");
		if(girl->m_AccLevel == 4)
			data += gettext("Wonderful\n");
		if(girl->m_AccLevel == 5)
			data += gettext("High Class\n");

		if(g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			data += gettext("Gold: ");
			int money = girl->m_Money;
			_itoa(money,buffer,10);
			data += buffer;
			data += gettext("\n");
		}
		else
		{
			data += gettext("Gold: Unknown\n");
		}
	}


	int cost = int(tariff.slave_price(girl, purchase));
	g_LogFile.ss() << gettext("slave ")
		       << (purchase ? gettext("buy") : gettext("sell"))
		       << gettext("price = ")
		       << cost
	;
	g_LogFile.ssend();
	ss << gettext("Worth: ") << cost << gettext(" Gold\n") ;
	data += ss.str();
	ss.str("");

	data += gettext("Avg Pay per customer: ");
	CalculateAskPrice(girl, false);
	cost = g_Girls.GetStat(girl, STAT_ASKPRICE);
	_itoa(cost,buffer,10);
	data += buffer;
	data += gettext(" gold\n");

/*  // shown elsewhere now
	data += "House Percentage: ";
	cost = g_Girls.GetStat(girl, STAT_HOUSE);
	_itoa(cost,buffer,10);
	data += buffer;
	data += "%\n";
*/

	data += gettext("\nSKILLS\n");

	data += gettext("Magic Ability: ");
	variable = GetSkill(girl, SKILL_MAGIC);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Combat Ability: ");
	variable = GetSkill(girl, SKILL_COMBAT);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Anal Sex: ");
	variable = GetSkill(girl, SKILL_ANAL);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("BDSM Sex: ");
	variable = GetSkill(girl, SKILL_BDSM);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Normal Sex: ");
	variable = GetSkill(girl, SKILL_NORMALSEX);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Bestiality Sex: ");
	variable = GetSkill(girl, SKILL_BEASTIALITY);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Group Sex: ");
	variable = GetSkill(girl, SKILL_GROUP);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Lesbian Sex: ");
	variable = GetSkill(girl, SKILL_LESBIAN);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += "Oral Sex: ";
	variable = GetSkill(girl, SKILL_ORALSEX);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += "%\n";

	data += "Titty Sex: ";
	variable = GetSkill(girl, SKILL_TITTYSEX);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += "%\n";

	data += gettext("Service Skills: ");
	variable = GetSkill(girl, SKILL_SERVICE);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	data += gettext("Stripping Sex: ");
	variable = GetSkill(girl, SKILL_STRIP);
	_itoa(variable, buffer, 10);
	data += buffer;
	data += gettext("%\n");

	return data;
}

sGirl* cGirls::GetRandomGirl(bool slave, bool catacomb, bool arena)
{
	int num_girls = m_NumGirls;
	if((num_girls == GetNumSlaveGirls()+GetNumCatacombGirls()+GetNumArenaGirls()) || num_girls == 0)
	{
		int r = 3;
		while(r)
		{
			CreateRandomGirl(0, true);
			r--;
		}
	}

	GirlPredicate_GRG pred(slave, catacomb, arena);
	vector<sGirl *> girls = get_girls(&pred);

	if(girls.size() == 0) {
		return 0;
	}

	return girls[
		g_Dice.random(girls.size())
	];
}

sGirl* cGirls::GetGirl(int girl)
{
	int count = 0;
	sGirl* current = m_Parent;

	if(girl < 0 || (unsigned int)girl >= m_NumGirls)
		return 0;

	while(current)
	{
		if(count == girl)
			return current;
		count++;
		current = current->m_Next;
	}

	return 0;
}

int cGirls::GetRebelValue(sGirl* girl, bool matron)
{
/*
 *	WD:	Added test to ingnore STAT_HOUSE value
 *	if doing a job that the palyer is paying
 *	only when processing Day or Night Shift
 *
 *	This is to make it so that the jobs that
 *	cost the player support where the hosue take
 *	has no effect has no impact on the chance of 
 *	refusal.
 */

	if(HasTrait(girl, "Broken Will"))
		return -100;

	int chanceNo		= 0;

	int houseStat		= GetStat(girl, STAT_HOUSE);
	int happyStat		= GetStat(girl, STAT_HAPPINESS);
	bool girlIsSlave	= girl->is_slave();

	if (!girlIsSlave)								// House Take has no effect on slaves
	{
		// WD	House take of gold has no affect on rebellion if
		//		job is paid by player. eg Matron / cleaner
		if (g_Brothels.is_Dayshift_Processing())
		{
			if(g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_DayJob))
			{
				houseStat = 0;
			}
		}
		else if (g_Brothels.is_Nightshift_Processing())
		{
			if(g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_NightJob))
			{
				houseStat = 0;
			}
		}
	}

	if(matron)	// a matron will help convince a girl to obey (this always applies in dungeon)
		chanceNo -= 15;

	if(HasTrait(girl, "Retarded"))
		chanceNo -= 30;

	chanceNo -= GetStat(girl, STAT_PCLOVE) / 5;
	chanceNo += GetStat(girl, STAT_SPIRIT) / 2;
	chanceNo -= GetStat(girl, STAT_OBEDIENCE) / 5;

	// having a guarding gang will enforce order
	sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
	if(gang)
		chanceNo -= 10;

	chanceNo += GetStat(girl, STAT_TIREDNESS) / 10;	// Tired girls less Rebel

	if(happyStat < 50)								// Unhappy girls increase Rebel
	{
		chanceNo += (50 - happyStat) / 5;
		if(happyStat < 10)							// WD:	Fixed missing case Happiness < 10
			chanceNo += 10 - happyStat;				// WD:	Rebel increases by an additional point if happy < 10
	}
	else											// happy girls are less cranky, less Rebel
		chanceNo -= (happyStat - 50) / 10;

	if(girlIsSlave)									// Slave Girl lowers rebelinous of course
		chanceNo -= 15;								// House Take has no impact on slaves

	else
	{
		chanceNo += 15;								// Free girls are a little more rebelious

													// Free Girls Income
		if(houseStat < 60)							// Take less money than normal lower Rebel 
			chanceNo -= (60 - houseStat) / 2;

		else // if(houseStat > 60)
		{
			chanceNo += (houseStat - 60) / 2;		// Take more money than normal, more Rebel
		
			if(houseStat >= 100)					// Take all the money, more Rebel
				chanceNo += 10;
		}
	}

													// guarantee certain rebelliousness values for specific traits
	if(HasTrait(girl, "Mind Fucked") && chanceNo > -50)
		return -50;

	if(HasTrait(girl, "Dependant") && chanceNo > -40)
		return -40;

	if(HasTrait(girl, "Meek" ) && chanceNo > 20)
		return 20;

	if(chanceNo < -100)								// Normalise
		chanceNo = -100;
	else if(chanceNo > 100)
		chanceNo = 100;

	return chanceNo;
}

int cGirls::GetNumCatacombGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while(current)
	{
		if(current->m_States&(1<<STATUS_CATACOMBS))
			number++;
		current = current->m_Next;
	}

	return number;
}

int cGirls::GetNumSlaveGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while(current)
	{
		if(current->m_States&(1<<STATUS_SLAVE))
			number++;
		current = current->m_Next;
	}

	return number;
}
int cGirls::GetNumArenaGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while(current)
	{
		if(current->m_States&(1<<STATUS_ARENA))
			number++;
		current = current->m_Next;
	}

	return number;
}
// ----- Stat

int cGirls::GetStat(sGirl* girl, int a_stat)
{
	u_int stat = a_stat;
	int value = 0;

	if (stat == STAT_AGE) // Matches SetStat
		return (int) girl->m_Stats[stat];

	else if (stat == STAT_EXP)
	{
		value = girl->m_Stats[stat];
		if(value < 0)
			value = 0;
		else if(value > 255)
			value = 255;
		return value;
	}

	// Health and tiredness require the incorporeal sanity check
	else if(stat == STAT_HEALTH)
	{
		value = girl->m_Stats[stat];

		if (g_Girls.HasTrait(girl, "Incorporeal"))
			value = 100;
		else if(value < 0)
			value = 0;
		else if(value > 100)
			value = 100;
		return value;
	}

	else if(stat == STAT_TIREDNESS)
	{
		value = girl->m_Stats[stat];

		if (g_Girls.HasTrait(girl, "Incorporeal"))
			value = 0;
		else if(value < 0)
			value = 0;
		else if(value > 100)
			value = 100;
		return value;
	}

	// Generic calculation
	value = ((int)girl->m_Stats[stat]) + girl->m_StatMods[stat] + girl->m_TempStats[stat];

	if (value < 0)
		value = 0;
	else if (value > 100)
		value = 100;

	return value;
}

void cGirls::SetStat(sGirl* girl, int a_stat, int amount)
{
	u_int stat = a_stat;
	int amt = amount;       // Modifying amount directly isn't a good idea

	if(stat == STAT_HOUSE || stat == STAT_HAPPINESS)
	{
		if(amt > 100)
			amt = 100;
		else if(amt < 0)
			amt = 0;
		girl->m_Stats[stat] = amt;
	}
	// Health and tiredness need the incorporeal sanity check
	else if(stat == STAT_HEALTH)
	{
		if (g_Girls.HasTrait(girl, "Incorporeal"))
			amt = 100;
		else if(amt > 100)
			amt = 100;
		else if(amt < 0)
			amt = 0;
		girl->m_Stats[stat] = amt;
	}
	else if(stat == STAT_TIREDNESS)
	{
		if (g_Girls.HasTrait(girl, "Incorporeal"))
			amt = 0;
		else if(amt > 100)
			amt = 100;
		else if(amt < 0)
			amt = 0;
		girl->m_Stats[stat] = amt;
	}
	else if(stat == STAT_EXP)
	{
		if(amt > 255)
			amt = 255;
		else if(amt < 0)
			amt = 0;
		girl->m_Stats[stat] = amt;
	}
	else if(stat == STAT_AGE)
	{
		/*
		 *	WD: Allow age to be SET to 100
		 *		for imortals
		 */

		if (girl->m_Stats[STAT_AGE] > 99)			
			girl->m_Stats[stat] = 100;
		else if(girl->m_Stats[stat] > 80)
			girl->m_Stats[stat] = 80;
		else if(girl->m_Stats[stat] < 18)
			girl->m_Stats[stat] = 18;
		else
			girl->m_Stats[stat] = amt;
	}
	else
	{
		if(amt > 100 /*&& stat != STAT_ASKPRICE*/)   // STAT_ASKPRICE is the price per sex act and is capped at 100
			girl->m_StatMods[stat] = 100;
		else if(amt < -100)
			girl->m_StatMods[stat] = -100;
		else
			girl->m_StatMods[stat] = amt;
	}
}

void cGirls::UpdateStat(sGirl* girl, int a_stat, int amount)
{
	u_int stat = a_stat;
	if(stat == STAT_HEALTH || stat == STAT_HAPPINESS)
	{
		if(HasTrait(girl, "Fragile") &&  stat != STAT_HAPPINESS)
			amount -= 3;
		else if(HasTrait(girl, "Tough") &&  stat != STAT_HAPPINESS)
			amount+=2;
		if(HasTrait(girl, "Construct") &&  stat != STAT_HAPPINESS)
		{
			if(amount < -4)
				amount = -4;
			else if(amount > 4)
				amount = 4;
		}
		if(HasTrait(girl, "Incorporeal") &&  stat != STAT_HAPPINESS)
		{
			//amount = 0;
			girl->m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
			return;
		}

		else
		{
			if (amount != 0)
			{
				int value = girl->m_Stats[stat] + amount;
				if(value > 100)
					value = 100;
				else if(value < 0)
					value = 0;
				girl->m_Stats[stat] = value;
			}
		}
	}


	else if(stat == STAT_EXP || stat == STAT_LEVEL)
	{
		int value = girl->m_Stats[stat] + amount;
		if(stat == STAT_LEVEL)
		{
			if(value > 30)
				value = 30;
			else if(value < 0)
				value = 0;
		}
		else
		{
			if(value > 255)
				value = 255;
			else if(value < 0)
				value = 0;
		}
		girl->m_Stats[stat] = value;
	}

	else if(stat == STAT_TIREDNESS)
	{
		if(HasTrait(girl, "Tough"))
			amount-=2;
		else if(HasTrait(girl, "Fragile"))
			amount += 3;
		if(HasTrait(girl, "Construct"))
		{
			 /*
            if(amount < -4)
            amount = -4;
            else if(amount > 4)
            amount = 4;
            `J` Reworked construct damage/healing to 10% instead of maximum of 4 up or down
            */
            amount = (int)ceil(amount*0.1);
		}

		if(HasTrait(girl, "Incorporeal"))
		{
//			amount = 0;
			girl->m_Stats[stat] = 0;								// WD: Sanity - Incorporeal Tirdness should allways be at 0%
		}

		else
		{
			if (amount != 0)
			{
				int value = girl->m_Stats[stat] + amount;
				if(value > 100)
					value = 100;
				else if(value < 0)
					value = 0;
				girl->m_Stats[stat] = value;
			}
		}
	}

	else if(stat == STAT_AGE)
	{
		if (girl->m_Stats[STAT_AGE] != 100 && amount != 0)			// WD: Dont change imortal age = 100
		{
			int value = girl->m_Stats[stat]+amount;
			//if(value > 80 && value != 100)
			if(value > 80)
				value = 80;
			else if(value < 18)
				value = 18;
			girl->m_Stats[stat] = value;
		}
	}

	else if(stat == STAT_HOUSE)
	{
		int value = girl->m_Stats[stat]+amount;
		if(value > 100)
			value = 100;
		else if(value < 0)
			value = 0;
		girl->m_Stats[stat] = value;
	}

	else
	{
		if (amount != 0)
		{
			if(amount > 0)
			{
				if(amount+girl->m_StatMods[stat] > 100 && stat != STAT_ASKPRICE)
					girl->m_StatMods[stat] = 100;
				else
					girl->m_StatMods[stat] += amount;
			}
			else
			{
				if(amount+girl->m_StatMods[stat] < -100)
					girl->m_StatMods[stat] = -100;
				else
					girl->m_StatMods[stat] += amount;
			}
		}
	}
}

void cGirls::updateTempStats(sGirl* girl)
{
/*
 *	 Normalise to zero by 30%
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	for(int i=0; i<NUM_STATS; i++)
	{
		if(girl->m_TempStats[i] != 0)				// normalize towards 0 by 30% each week
		{ 
			int newStat = (int)(float(girl->m_TempStats[i]) * 0.7);
			if(newStat != girl->m_TempStats[i])
				girl->m_TempStats[i] = newStat;
			else
			{										// if 30% did nothing, go with 1 instead
				if(girl->m_TempStats[i] > 0)
					girl->m_TempStats[i]--;
				else if(girl->m_TempStats[i] < 0)
					girl->m_TempStats[i]++;
			}
		}
	}
}

void cGirls::UpdateTempStat(sGirl* girl, int stat, int amount)
{
	if(amount >= 0)
	{
		if(amount+girl->m_TempStats[stat] > INT_MAX)
			girl->m_TempStats[stat] = INT_MAX;
		else
			girl->m_TempStats[stat] += amount;
	}
	else
	{
		if(amount+girl->m_TempStats[stat] < INT_MIN)
			girl->m_TempStats[stat] = INT_MIN;
		else
			girl->m_TempStats[stat] += amount;
	}
}

// ----- Skill

int cGirls::GetSkill(sGirl* girl, int skill)
{
	int value = ((int)girl->m_Skills[skill]) + girl->m_SkillMods[skill] + girl->m_TempSkills[skill];
	if(value < 0)
		value = 0;
	else if(value > 100)
		value = 100;
	return value;
}

void cGirls::SetSkill(sGirl* girl, int skill, int amount)
{
	girl->m_SkillMods[skill] = amount;
}

int cGirls::GetSkillWorth(sGirl* girl)
{
	int num = 0;
	for(u_int i=0; i<NUM_SKILLS; i++)
		num += (int)girl->m_Skills[i];
	return num;
}

void cGirls::UpdateSkill(sGirl* girl, int skill, int amount)
{
	if(amount >= 0)
	{
		if(amount+girl->m_SkillMods[skill] > INT_MAX)
			girl->m_SkillMods[skill] = INT_MAX;
		else
			girl->m_SkillMods[skill] += amount;
	}
	else
	{
		if(amount+girl->m_SkillMods[skill] < INT_MIN)
			girl->m_SkillMods[skill] = INT_MIN;
		else
			girl->m_SkillMods[skill] += amount;
	}
}

void cGirls::updateTempSkills(sGirl* girl)
{
/*
 *	 Normalise to zero by 30%
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		if(girl->m_TempSkills[i] != 0)
		{											// normalize towards 0 by 30% each week
			int newSkill = (int)(float(girl->m_TempSkills[i]) * 0.7);
			if(newSkill != girl->m_TempSkills[i])
				girl->m_TempSkills[i] = newSkill;
			else
			{										// if 30% did nothing, go with 1 instead
				if(girl->m_TempSkills[i] > 0)
					girl->m_TempSkills[i]--;
				else if(girl->m_TempSkills[i] < 0)
					girl->m_TempSkills[i]++;
			}
		}
	}
}

void cGirls::UpdateTempSkill(sGirl* girl, int skill, int amount)
{
	if(amount >= 0)
	{
		if(amount+girl->m_TempSkills[skill] > INT_MAX)
			girl->m_TempSkills[skill] = INT_MAX;
		else
			girl->m_TempSkills[skill] += amount;
	}
	else
	{
		if(amount+girl->m_TempSkills[skill] < INT_MIN)
			girl->m_TempSkills[skill] = INT_MIN;
		else
			girl->m_TempSkills[skill] += amount;
	}
}

// ----- Load save

// This load

void cGirls::LoadGirlLegacy(sGirl* current, ifstream& ifs)
{
	cConfig cfg;
	int temp = 0;
	stringstream ss;
	char buffer[4000];

	// load the name
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs.getline(buffer, sizeof(buffer), '\n');		// get the name
	current->m_Name = new char[strlen(buffer)+1];
	strcpy(current->m_Name, buffer);

	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs.getline(buffer, sizeof(buffer), '\n');
	current->m_Realname = buffer;

	if(cfg.debug.log_girls()) {
		ss << "Loading girl: '" << current->m_Realname;
		g_LogFile.write(ss.str());
	}

	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs.getline(buffer, sizeof(buffer), '\n');		// get the description
	current->m_Desc = buffer;

	// load the amount of days they are unhappy in a row
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_DaysUnhappy = temp;

	// Load their traits
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_NumTraits = temp;
	if(current->m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("--- ERROR - Loaded more traits then girls can have??");
	for(int i=0; i<current->m_NumTraits; i++)	// load the trait names
	{
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		current->m_Traits[i] = g_Traits.GetTrait(buffer);

		// get the temp trait variable
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		current->m_TempTrait[i] = temp;
	}

	// Load their remembered traits
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_NumRememTraits = temp;
	if(current->m_NumRememTraits > MAXNUM_TRAITS*2)
		g_LogFile.write("--- ERROR - Loaded more remembered traits then girls can have??");
	for(int i=0; i<current->m_NumRememTraits; i++)	// load the trait names
	{
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		current->m_RememTraits[i] = g_Traits.GetTrait(buffer);
	}

	// Load inventory items
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;   // # of items they have
	current->m_NumInventory = temp;
	sInventoryItem *tempitem;
	int total_num=0;
	for(int i=0; i<current->m_NumInventory; i++)	// save in the items
	{
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		tempitem = g_InvManager.GetItem(buffer);

		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;  // Equipped or not (0 = no)
		if(tempitem)
		{
			current->m_EquipedItems[total_num] = temp;  // Is the item equipped?
			current->m_Inventory[total_num]=tempitem;

			total_num++;
		}
	}

	// load their states
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>current->m_States;

	// load their stats
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(int i=0; i<NUM_STATS; i++)
	{
		temp = 0;
		ifs>>temp>>current->m_StatMods[i]>>current->m_TempStats[i];
		current->m_Stats[i] = temp;
	}

	// load their skills
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		temp = 0;
		ifs>>temp>>current->m_SkillMods[i]>>current->m_TempSkills[i];
		current->m_Skills[i] = temp;
	}

	// load virginity
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	if(temp == 1)
		current->m_Virgin = true;
	else
		current->m_Virgin = false;

	// load using antipreg
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	if(temp == 1)
		current->m_UseAntiPreg = true;
	else
		current->m_UseAntiPreg = false;

	// load withdrawals
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_Withdrawals = temp;

	// load money
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>current->m_Money;

	// load working day count
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>current->m_WorkingDay;

	// load acom level
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_AccLevel = temp;

	// load day/night jobs
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_DayJob = temp;
	ifs>>temp;
	current->m_NightJob = temp;

	// load prev day/night jobs
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_PrevDayJob = temp;
	ifs>>temp;
	current->m_PrevNightJob = temp;

	// load runnayway value
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_RunAway = temp;

	// load spotted
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	current->m_Spotted = temp;

	// load weeks past, birth day, and pregant time
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>current->m_WeeksPast>>current->m_BDay;
	ifs>>temp;
	current->m_WeeksPreg = temp;

	// load number of customers slept with
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>current->m_NumCusts;

	// load girl flags
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(int i=0; i<NUM_GIRLFLAGS; i++)
	{
		temp = 0;
		ifs>>temp;
		current->m_Flags[i] = temp;
	}

	// load their torture value
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	if(temp == 1)
		current->m_Tort = true;
	else
		current->m_Tort = false;

	// Load their children
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');ifs>>temp;current->m_PregCooldown = temp;	// load preg cooldown
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');ifs>>temp;	
	// load number of children
	int temp2=temp;
	if(temp2 > 0)
	{
		for(int i=0; i<temp2; i++)
		{
			sChild* child = new sChild();
			int a,b,c,d;
			if(ifs.peek()=='\n') ifs.ignore(1,'\n');
			ifs>>a>>b>>c>>d;

			child->m_Age = a;
			child->m_IsPlayers = b;
			child->m_Sex = sChild::Gender(c);
			child->m_Unborn = d;

			// load their stats
			if(ifs.peek()=='\n') ifs.ignore(1,'\n');
			for(int i=0; i<NUM_STATS; i++)
			{
				temp = 0;
				ifs>>temp;
				child->m_Stats[i] = temp;
			}

			// load their skills
			if(ifs.peek()=='\n') ifs.ignore(1,'\n');
			for(u_int i=0; i<NUM_SKILLS; i++)
			{
				temp = 0;
				ifs>>temp;
				child->m_Skills[i] = temp;
			}

			current->m_Children.add_child(child);
		}
	}

	// load enjoyment values
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(int i=0; i<NUM_ACTIONTYPES; i++)
		ifs>>current->m_Enjoyment[i];

	// load their triggers
	current->m_Triggers.LoadTriggersLegacy(ifs);
	current->m_Triggers.SetGirlTarget(current);

	// load their images
	LoadGirlImages(current);

	if(current->m_Stats[STAT_AGE] < 18)
		current->m_Stats[STAT_AGE] = 18;

	CalculateGirlType(current);
}

bool sGirl::LoadGirlXML(TiXmlHandle hGirl)
{
	//this is always called after creating a new girl, so let's not init sGirl again
	TiXmlElement* pGirl = hGirl.ToElement();
	if (pGirl == 0)
	{
		return false;
	}

	int tempInt = 0;

	// load the name
	const char* pTempString = pGirl->Attribute("Name");
	if (pTempString)
	{
		m_Name = new char[strlen(pTempString)+1];
		strcpy(m_Name, pTempString);
	}
	else
	{
		m_Name = "";
	}

	if (pGirl->Attribute("Realname"))
	{
		m_Realname = pGirl->Attribute("Realname");
	}

	cConfig cfg;
	if(cfg.debug.log_girls()) {
		std::stringstream ss;
		ss << "Loading girl: '" << m_Realname;
		g_LogFile.write(ss.str());
	}

	// get the description
	if (pGirl->Attribute("Desc"))
	{
		m_Desc = pGirl->Attribute("Desc");
	}

	// load the amount of days they are unhappy in a row
	pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

	// Load their traits
	LoadTraitsXML(hGirl.FirstChild("Traits"), m_NumTraits, m_Traits, m_TempTrait);
	if(m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("--- ERROR - Loaded more traits than girls can have??");

	// Load their remembered traits
	LoadTraitsXML(hGirl.FirstChild("Remembered_Traits"), m_NumRememTraits, m_RememTraits);
	if(m_NumRememTraits > MAXNUM_TRAITS*2)
		g_LogFile.write("--- ERROR - Loaded more remembered traits than girls can have??");

	// Load inventory items
	LoadInventoryXML(hGirl.FirstChild("Inventory"), m_Inventory, m_NumInventory, m_EquipedItems);

	// load their states
	pGirl->QueryValueAttribute<long>("States", &m_States);

	// load their stats
	LoadStatsXML(hGirl.FirstChild("Stats"), m_Stats, m_StatMods, m_TempStats);

	// load their skills
	LoadSkillsXML(hGirl.FirstChild("Skills"), m_Skills, m_SkillMods, m_TempSkills);

	// load virginity
	pGirl->QueryValueAttribute<bool>("Virgin", &m_Virgin);

	// load using antipreg
	pGirl->QueryValueAttribute<bool>("UseAntiPreg", &m_UseAntiPreg);

	// load withdrawals
	pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

	// load money
	pGirl->QueryIntAttribute("Money", &m_Money);

	// load working day counter
	pGirl->QueryIntAttribute("WorkingDay", &m_WorkingDay);

	// load acom level
	pGirl->QueryIntAttribute("AccLevel", &tempInt); m_AccLevel = tempInt; tempInt = 0;
	// load day/night jobs
	pGirl->QueryIntAttribute("DayJob", &tempInt); m_DayJob = tempInt; tempInt = 0;
	pGirl->QueryIntAttribute("NightJob", &tempInt); m_NightJob = tempInt; tempInt = 0;

	// load prev day/night jobs
	pGirl->QueryIntAttribute("PrevDayJob", &tempInt); m_PrevDayJob = tempInt; tempInt = 0;
	pGirl->QueryIntAttribute("PrevNightJob", &tempInt); m_PrevNightJob = tempInt; tempInt = 0;

	// load runnayway value
	pGirl->QueryIntAttribute("RunAway", &tempInt); m_RunAway = tempInt; tempInt = 0;

	// load spotted
	pGirl->QueryIntAttribute("Spotted", &tempInt); m_Spotted = tempInt; tempInt = 0;

	// load newRandomFixed
	tempInt = -1;
	pGirl->QueryIntAttribute("NewRandomFixed", &tempInt); m_newRandomFixed = tempInt; tempInt = 0;

	// load weeks past, birth day, and pregant time
	pGirl->QueryValueAttribute<unsigned long>("WeeksPast", &m_WeeksPast);
	pGirl->QueryValueAttribute<unsigned int>("BDay", &m_BDay);
	pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

	// load number of customers slept with
	pGirl->QueryValueAttribute<unsigned long>("NumCusts", &m_NumCusts);

	// load girl flags
	TiXmlElement* pFlags = pGirl->FirstChildElement("Flags");
	if (pFlags)
	{
		std::string flagNumber;
		for(int i=0; i<NUM_GIRLFLAGS; i++)
		{
			flagNumber = "Flag_";
			std::stringstream stream;
			stream << i;
			flagNumber.append(stream.str());
			pFlags->QueryIntAttribute(flagNumber, &tempInt); m_Flags[i] = tempInt; tempInt = 0;
		}
	}

	// load their torture value
	pGirl->QueryValueAttribute<bool>("Tort", &m_Tort);

	// Load their children
	pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;


	// load number of children
	TiXmlElement* pChildren = pGirl->FirstChildElement("Children");
	if (pChildren)
	{
		for(TiXmlElement* pChild = pChildren->FirstChildElement("Child");
			pChild != 0;
			pChild = pChild->NextSiblingElement("Child"))
		{
			sChild* child = new sChild();
			bool success = child->LoadChildXML(TiXmlHandle(pChild));
			if (success == true)
			{
				m_Children.add_child(child);
			}
			else
			{
				delete child;
				continue;
			}
		}
	}

	// load enjoyment values
	LoadActionsXML(hGirl.FirstChildElement("Actions"), m_Enjoyment);

	// load their triggers
	m_Triggers.LoadTriggersXML(hGirl.FirstChildElement("Triggers"));
	m_Triggers.SetGirlTarget(this);

	if(m_Stats[STAT_AGE] < 18)
		m_Stats[STAT_AGE] = 18;

	// load their images
	g_Girls.LoadGirlImages(this);
	g_Girls.CalculateGirlType(this);

	return true;
}

// This save

TiXmlElement* sGirl::SaveGirlXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirl = new TiXmlElement("Girl");
	pRoot->LinkEndChild(pGirl);
	// save the name
	pGirl->SetAttribute("Name", m_Name);

	// save the real name
	pGirl->SetAttribute("Realname", m_Realname);

	// save the description
	pGirl->SetAttribute("Desc", m_Desc);

	// save the amount of days they are unhappy
	pGirl->SetAttribute("DaysUnhappy", m_DaysUnhappy);

	// Save their traits
	if(m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("---- ERROR - Saved more traits then girls can have");
	SaveTraitsXML(pGirl, "Traits", MAXNUM_TRAITS, m_Traits, m_TempTrait);

	// Save their remembered traits
	if(m_NumRememTraits > MAXNUM_TRAITS*2)
		g_LogFile.write("---- ERROR - Saved more remembered traits then girls can have");
	SaveTraitsXML(pGirl, "Remembered_Traits", MAXNUM_TRAITS*2, m_RememTraits, 0);

	// Save inventory items
	TiXmlElement* pInventory = new TiXmlElement("Inventory");
	pGirl->LinkEndChild(pInventory);
	SaveInventoryXML(pInventory, m_Inventory, 40, m_EquipedItems);

	// save their states
	pGirl->SetAttribute("States", m_States);

	// Save their stats
	SaveStatsXML(pGirl, m_Stats, m_StatMods, m_TempStats);

	// save their skills
	SaveSkillsXML(pGirl, m_Skills, m_SkillMods, m_TempSkills);

	// save virginity
	pGirl->SetAttribute("Virgin", m_Virgin);

	// save using antipreg
	pGirl->SetAttribute("UseAntiPreg", m_UseAntiPreg);

	// save withdrawals
	pGirl->SetAttribute("Withdrawals", m_Withdrawals);

	// save money
	pGirl->SetAttribute("Money", m_Money);

	// save working day counter
	pGirl->SetAttribute("WorkingDay", m_WorkingDay);

	// save acom level
	pGirl->SetAttribute("AccLevel", m_AccLevel);

	// save day/night jobs
	pGirl->SetAttribute("DayJob", m_DayJob);
	pGirl->SetAttribute("NightJob", m_NightJob);

	// save prev day/night jobs
	pGirl->SetAttribute("PrevDayJob", m_PrevDayJob);
	pGirl->SetAttribute("PrevNightJob", m_PrevNightJob);

	// save runnayway vale
	pGirl->SetAttribute("RunAway", m_RunAway);

	// save spotted
	pGirl->SetAttribute("Spotted", m_Spotted);

	if(m_newRandomFixed >=0)
		pGirl->SetAttribute("NewRandomFixed", m_newRandomFixed);

	// save weeks past, birth day, and pregant time
	pGirl->SetAttribute("WeeksPast", m_WeeksPast);
	pGirl->SetAttribute("BDay", m_BDay);
	pGirl->SetAttribute("WeeksPreg", m_WeeksPreg);

	// number of customers slept with
	pGirl->SetAttribute("NumCusts", m_NumCusts);

	// girl flags
	TiXmlElement* pFlags = new TiXmlElement("Flags");
	pGirl->LinkEndChild(pFlags);
	std::string flagNumber;
	for(int i=0; i<NUM_GIRLFLAGS; i++)
	{
		flagNumber = "Flag_";
		std::stringstream stream;
		stream << i;
		flagNumber.append(stream.str());
		pFlags->SetAttribute(flagNumber, m_Flags[i]);
	}

	// save their torture value
	pGirl->SetAttribute("Tort", m_Tort);

	// save their children
	pGirl->SetAttribute("PregCooldown", m_PregCooldown);
	TiXmlElement* pChildren = new TiXmlElement("Children");
	pGirl->LinkEndChild(pChildren);
	sChild* child = m_Children.m_FirstChild;
	while(child)
	{
		child->SaveChildXML(pChildren);
		child = child->m_Next;
	}

	// save their enjoyment values
	SaveActionsXML(pGirl, m_Enjoyment);

	// save their triggers
	m_Triggers.SaveTriggersXML(pGirl);
	return pGirl;
}

bool sChild::LoadChildXML(TiXmlHandle hChild)
{
	TiXmlElement* pChild = hChild.ToElement();
	if (pChild == 0)
	{
		return false;
	}

	int tempInt = 0;
	pChild->QueryIntAttribute(gettext("Age"), &tempInt); m_Age = tempInt; tempInt = 0;
	pChild->QueryIntAttribute("IsPlayers", &tempInt); m_IsPlayers = tempInt; tempInt = 0;
	pChild->QueryIntAttribute("Sex", &tempInt);
	m_Sex = sChild::Gender(tempInt); tempInt = 0;
	pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;

	// load their stats
	LoadStatsXML(hChild.FirstChild("Stats"), m_Stats);

	// load their skills
	LoadSkillsXML(hChild.FirstChild("Skills"), m_Skills);
	return true;
}

TiXmlElement* sChild::SaveChildXML(TiXmlElement* pRoot)
{
	TiXmlElement* pChild = new TiXmlElement("Child");
	pRoot->LinkEndChild(pChild);
	pChild->SetAttribute("Age", m_Age);
	pChild->SetAttribute("IsPlayers", m_IsPlayers);
	pChild->SetAttribute("Sex", m_Sex);
	pChild->SetAttribute("Unborn", m_Unborn);
	// Save their stats
	SaveStatsXML(pChild, m_Stats);

	// save their skills
	SaveSkillsXML(pChild, m_Skills);
	return pChild;
}


/*
 * and an xml loader for sGirl
 */
void sGirl::load_from_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
/*
 *	get the simple fields
 */
	if((pt = el->Attribute("Name"))) {
		m_Name = n_strdup(pt);
		m_Realname = pt; 
	}
	else {
		g_LogFile.os() << "Error: can't find name when loading girl."
		     << "XML = " << (*el) << endl;
		return;
	}

	m_newRandomFixed = -1;

	if((pt = el->Attribute("Desc"))) 
		m_Desc = n_strdup(pt);
	
	if((pt = el->Attribute("Gold", &ival))) 
		m_Money = ival;
	
/*
 *	loop through stats
 */
	for(int i = 0; i < NUM_STATS; i++) {
		int ival;
		const char *stat_name = sGirl::stat_names[i];
		pt = el->Attribute(stat_name, &ival);

		ostream& os = g_LogFile.os();
		if(pt == 0) {
			os << "Error: Can't find stat '" << stat_name << "' for girl '" << m_Realname << "'" << endl;
			continue;
		}
		m_Stats[i] = ival;
		os << "Debug: Girl='" << m_Realname << "'; Stat='" << stat_name << "'; Value='" << pt << "'; Ival = " 
		   << int(m_Stats[i]) << "'" << endl;
	}

	// "fix" underage girls, determine virgin status
	m_Virgin = false;
	if(m_Stats[STAT_AGE] <= 18)
	{
		m_Stats[STAT_AGE] = 18;
		m_Virgin = true;
	}
	else if(m_Stats[STAT_AGE] == 18)
	{
		if(g_Dice%3 == 1)
			m_Virgin = true;
	}
	else if(m_Stats[STAT_AGE] <= 25)
	{
		int avg = 0;
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			if(
				i != SKILL_SERVICE
				&& i != SKILL_COMBAT
				&& i != SKILL_MAGIC
			)
				avg += (int)m_Skills[i];
		}
		avg = avg/(NUM_SKILLS-1);

		if(avg < 30)
			m_Virgin = true;
	}

/*
 *	loop through skills
 */
	for(u_int i = 0; i < NUM_SKILLS; i++) {
		int ival;
		if((pt = el->Attribute(sGirl::skill_names[i], &ival))) 
			m_Skills[i] = ival;
		}

	if((pt = el->Attribute("Status"))) {
		if(strcmp(pt, gettext("Catacombs")) == 0) 
			m_States |=(1<<STATUS_CATACOMBS);
		else if(strcmp(pt,gettext("Slave"))==0)
			m_States |=(1<<STATUS_SLAVE);
		else if(strcmp(pt,gettext("Arena"))==0)
			m_States |=(1<<STATUS_ARENA);
		else 
			m_States=0;
	}

	TiXmlElement * child;
	for(child=el->FirstChildElement();child;child=child->NextSiblingElement())
	{
/*
 *		get the trait name 
 */
		if(child->ValueStr()=="Trait") {
			pt=child->Attribute("Name");
			m_Traits[m_NumTraits] =g_Traits.GetTrait(n_strdup(pt));
			m_NumTraits++;
		}
	}
	m_AccLevel=1;
}

void sRandomGirl::load_from_xml(TiXmlElement *el)
{
	const char *pt;
	m_NumTraits = 0;
/*
 *	name and description are easy
 */
	pt = el->Attribute("Name");
	if(pt) 
		m_Name = pt;

	g_LogFile.os() << "Loading " << pt << endl;
	if((pt = el->Attribute("Desc")))
		m_Desc = pt;

/*
 *	DQ - new random type ...
 */
	m_newRandom = false;
	m_newRandomTable = 0;
	if((pt = el->Attribute("NewRandom"))) 
	{
		if(strcmp(pt, "Yes") == 0) 
			m_newRandom = true;
	}
/*
 *	logic is back-to-front on this one...
 */
	if((pt = el->Attribute("Human"))) 
	{
		if(strcmp(pt, "Yes") == 0) 
			m_Human = 1;
		else 
			m_Human = 0;
		}
/*
 *	catacomb dweller?
 */
	if((pt = el->Attribute("Catacomb")))
	{
		if(strcmp(pt, "Yes") == 0)
			m_Catacomb = 1;
		else 
			m_Catacomb = 0;
		}

	if((pt = el->Attribute("Arena")))
	{
		if(strcmp(pt, "Yes") == 0)
			m_Arena = 1;
		else 
			m_Arena = 0;
		}
/*
 *	loop through children
 */
	TiXmlElement *child;
	for(child = el->FirstChildElement(); child ; child = child->NextSiblingElement())
	{
/*
 *		now: what we do depends on the tag string
 *		which we can get from the ValueStr() method
 *
 *		Let's process each tag type in its own method.
 *		Keep things cleaner that way.
 */
 		if(child->ValueStr() == "Gold") {
			process_cash_xml(child);
			continue;
		}
/*
 *		OK: is it a stat?
 */
		if(child->ValueStr() == "Stat") 
		{
			process_stat_xml(child);
			continue;
		}
/*
 *		How about a skill?
 */
		if(child->ValueStr() == "Skill") 
		{
			process_skill_xml(child);
			continue;
		}
/*
 *		surely a trait then?
 */
		if(child->ValueStr() == "Trait") 
		{
			process_trait_xml(child);
			continue;
		}
/*
 *		None of the above? Better ask for help then.
 */
		g_LogFile.os() << "Unexpected tag: " << child->ValueStr() << endl;
		g_LogFile.os() << "	don't know what do to, ignoring" << endl;
	}
}

void cGirls::LoadRandomGirl(string filename)
{
/*
 *	before we go any further: files that end in "x" are
 *	in XML format. Get the last char of the filename.
 */
	char c = filename.at(filename.length()-1);
/*
 *	now decide how we want to really load the file
 */
	if(c == 'x') {
		cerr << "loading " << filename << " as XML" << endl;
		LoadRandomGirlXML(filename);
	}
	else {
		cerr << "loading " << filename << " as Legacy" << endl;
		LoadRandomGirlLegacy(filename);
	}
}

void cGirls::LoadRandomGirlXML(string filename)
{
	TiXmlDocument doc(filename);

	if(!doc.LoadFile()) {
		g_LogFile.os() << "can't load random XML girls " << filename << endl;
		g_LogFile.os()	<< "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol()
			<< ": " << doc.ErrorDesc() << endl;
		return;
	}
/*
 *	get the docuement root
 */
	TiXmlElement *el, *root_el = doc.RootElement();
/*
 *	loop over the elements attached to the root
 */
	for(el = root_el->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sRandomGirl* girl = new sRandomGirl;
/*
 *		walk the XML DOM to get the girl data
 */
		girl->load_from_xml(el);
/*
 *		add the girl to the list
 */
		AddRandomGirl(girl);
	}
}

void cGirls::LoadRandomGirlLegacy(string filename)
{
	ifstream in;
	in.open(filename.c_str());
	char buffer[500];
	sRandomGirl* newGirl = 0;
	int tempData;

	while(in.good())
	{
		newGirl = new sRandomGirl();

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the name
		newGirl->m_Name = buffer;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		newGirl->m_Desc = buffer;

		// Get their min stats
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(int i=0; i<NUM_STATS; i++)
		{
			in>>tempData;
			newGirl->m_MinStats[i] = tempData;
		}
		// Get their max stats
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(int i=0; i<NUM_STATS; i++)
		{
			in>>tempData;
			newGirl->m_MaxStats[i] = tempData;
		}

		// get their min skills
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			in>>tempData;
			newGirl->m_MinSkills[i] = tempData;
		}
		// get their max skills
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			in>>tempData;
			newGirl->m_MaxSkills[i] = tempData;
		}

		// Get their min starting cash
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_MinMoney = tempData;
		// Get their max starting cash
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_MaxMoney = tempData;

		// Get if they are human
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_Human = !(unsigned char)tempData;

		// Get if they are found in the catacombs
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_Catacomb = (unsigned char)tempData;

		// Get if they are found in the arena
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_Arena = (unsigned char)tempData;

		// Get how many traits they can have
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_NumTraits = tempData;
		// clear out the rest of the newline
		if (in.peek()=='\n') in.ignore(1,'\n');
		if (in.peek()=='\r') in.ignore(1,'\r');
/*
 *		load in the traits and percentages
 */
		for(int i=0; i<newGirl->m_NumTraits; i++)	// load in the traits and percentages
		{
			if (in.peek()=='\n') in.ignore(1,'\n');
			in.getline(buffer, sizeof(buffer), '\n');
			if(g_Traits.GetTrait(buffer) == 0)	// test a trait exists
			{
				string message = "ERROR: Trait ";
				message += buffer;
				message += " from girl template ";
				message += newGirl->m_Name;
				message += " doesn't exist or is spelled incorrectly.";
				g_MessageQue.AddToQue(message, 2);
			}
			else
			{
				newGirl->m_Traits[i] = g_Traits.GetTrait(buffer);	// get the trait name

				if (in.peek()=='\n') in.ignore(1,'\n');
				in>>tempData;
				newGirl->m_TraitChance[i] = (unsigned char)tempData;	// get the percent chance of getting the trait
				if (in.peek()=='\n') in.ignore(1,'\n');
				if (in.peek()=='\r') in.ignore(1,'\r');
			}
		}

		AddRandomGirl(newGirl);
	}
	in.close();
}

void cGirls::LoadGirlsDecider(string filename)
{
/*
 *	before we go any further: files that end in "x" are
 *	in XML format. Get the last char of the filename.
 */
	char c = filename.at(filename.length()-1);
/*
 *	now decide how we want to really load the file
 */
	if(c == 'x') {
		cerr << "loading " << filename << " as XML" << endl;
		LoadGirlsXML(filename);
	}
	else {
		cerr << "loading " << filename << " as legacy" << endl;
		LoadGirlsLegacy(filename);
	}
}

void cGirls::LoadGirlsXML(string filename)
{
	cConfig cfg;
	if(cfg.debug.log_girls()) {
		g_LogFile.ss() << "loading " << filename;
		g_LogFile.ssend();
	}

	TiXmlDocument doc(filename);
	if(!doc.LoadFile()) {
		g_LogFile.ss() << "can't load XML girls " << filename << endl;
		g_LogFile.ss()	<< "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol()
			<< ": " << doc.ErrorDesc() << endl;
		g_LogFile.ssend();
		return;
	}
/*
 *	get the docuement root
 */
	TiXmlElement *el, *root_el = doc.RootElement();
/*
 *	loop over the elements attached to the root
 */
	for(el = root_el->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sGirl* girl = new sGirl;
/*
 *		walk the XML DOM to get the girl data
 */
		girl->load_from_xml(el);
		if(cfg.debug.log_girls()) {
			g_LogFile.ss() << *girl << endl;
			g_LogFile.ssend();
		}
/*
 *		Mod: need to do this if you do not want a crash
 */
		g_Girls.LoadGirlImages(girl);
/*
 *		make sure all the trait effects are applied
 */
		ApplyTraits(girl);
/*
 *		WD: For new girlsr emove any remembered traits from 
 *			trait incompatibilities
 */
		RemoveAllRememberedTraits(girl);
/*
 *		load triggers if the girl has any
 */
		DirPath dp = DirPath()
			<< "Resources"
			<< "Characters"
			<< girl->m_Name
			<< "triggers.xml"
			;
		girl->m_Triggers.LoadList(dp);
		girl->m_Triggers.SetGirlTarget(girl);
/*
 *		add the girl to the list
 */
		AddGirl(girl);
/*
 *		not sure what this does, but the legacy version does it
 *		WD: Fetish list for customer happiniess caculation on the 
 *			more button of the girl details screen
 */
		CalculateGirlType(girl);
	}
}

void cGirls::LoadGirlsLegacy(string filename)
{
	ifstream in;
	in.open(filename.c_str());
	char buffer[500];
	sGirl* newGirl = 0;
	int tempData;
	cConfig cfg;

	while(in.good())
	{
		newGirl = new sGirl;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the name
		newGirl->m_Name = new char[strlen(buffer)+1];
		strcpy(newGirl->m_Name, buffer);

		newGirl->m_Realname = buffer;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		newGirl->m_Desc = buffer;

		// Get how many traits they have
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		newGirl->m_NumTraits = tempData;
		for(int i=0; i<tempData; i++)	// load in the traits
		{
			if (in.peek()=='\n') in.ignore(1,'\n');
			if (in.peek()=='\r') in.ignore(1,'\r');
			in.getline(buffer, sizeof(buffer), '\n');
			if(g_Traits.GetTrait(buffer) == 0)	// test a trait exists
			{
				string message = "ERROR in cGirls::LoadGirls: Trait '";
				message += buffer;
				message += "' from girl template ";
				message += newGirl->m_Name;
				message += " doesn't exist or is spelled incorrectly.";
				g_MessageQue.AddToQue(message, 2);
			}
			else
				newGirl->m_Traits[i] = g_Traits.GetTrait(buffer);
		}

		// Get how many inventory items they have
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_NumInventory = tempData;
		for(int i=0; i<tempData; i++)	// load in the items
		{
			if (in.peek()=='\n') in.ignore(1,'\n');
			in.getline(buffer, sizeof(buffer), '\n');
			//newGirl->m_Inventory[i] = g_InventoryItems.GetItem(buffer);
		}

		// Get their stats
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(int i=0; i<NUM_STATS; i++)
		{
			in>>tempData;
			newGirl->m_Stats[i] = tempData;
		}

		// get their skills
		if (in.peek()=='\n') in.ignore(1,'\n');
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			in>>tempData;
			newGirl->m_Skills[i] = tempData;
		}

		// Get their starting cash
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newGirl->m_Money = tempData;

		// Are they a slave (the only default state)
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		if(tempData == 1)
		{
			newGirl->m_AccLevel = 0;
			newGirl->m_States |= (1<<STATUS_SLAVE);
			newGirl->m_Stats[STAT_HOUSE] =  cfg.initial.slave_house_perc();
		}
		else if(tempData == 2)
		{
			newGirl->m_AccLevel = 0;
			newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();
			newGirl->m_States |= (1<<STATUS_CATACOMBS);
		}
		else if(tempData == 3)
		{
			newGirl->m_AccLevel = 0;
			newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();
			newGirl->m_States |= (1<<STATUS_ARENA);
		}
		else	// default stats
		{
			newGirl->m_AccLevel = 1;
			newGirl->m_States = 0;
			newGirl->m_Stats[STAT_HOUSE] = 60;
		}

		LoadGirlImages(newGirl);
		ApplyTraits(newGirl);

/*
 *		WD: remove any rembered traits created
 *			from trait incompatibilities
 */
		RemoveAllRememberedTraits(newGirl);

		DirPath dp = DirPath()
			<< "Resources"
			<< "Characters"
			<< newGirl->m_Name
			<< "triggers.txt"
			;
		newGirl->m_Triggers.LoadList(dp);
		newGirl->m_Triggers.SetGirlTarget(newGirl);

		if(newGirl->m_Stats[STAT_AGE] <= 19)
			newGirl->m_Virgin = true;
		else if(newGirl->m_Stats[STAT_AGE] <= 20)
		{
			if(g_Dice%3 == 1)
				newGirl->m_Virgin = true;
		}
		else
		{
			int avg = 0;
			for(u_int i=0; i<NUM_SKILLS; i++)
			{
				if(i != SKILL_SERVICE)
					avg += (int)newGirl->m_Skills[i];
			}
			avg = avg/(NUM_SKILLS-1);

			if((g_Dice%100)+1 > avg)
			{
				newGirl->m_Virgin = true;
			}
		}

		if(newGirl->m_Stats[STAT_AGE] < 18)
			newGirl->m_Stats[STAT_AGE] = 18;

		AddGirl(newGirl);
		CalculateGirlType(newGirl);
		newGirl = 0;
	}

	in.close();
}

bool cGirls::LoadGirlsXML(TiXmlHandle hGirls)
{
	TiXmlElement* pGirls = hGirls.ToElement();
	if (pGirls == 0)
	{
		return false;
	}

	// load the number of girls
	sGirl* current = 0;
	for(TiXmlElement* pGirl = pGirls->FirstChildElement("Girl");
		pGirl != 0;
		pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
	{
		current = new sGirl();
		bool success = current->LoadGirlXML(TiXmlHandle(pGirl));
		if (success == true)
		{
			AddGirl(current);
		}
		else
		{
			delete current;
			continue;
		}
	}
	return true;
}

void cGirls::LoadGirlsLegacy(ifstream& ifs)
{
	sGirl* current = 0;
	int temp;

#if 0  // should already be cleared before now, and doing again here wipes out any newly loaded girl packs (from LoadMasterFile)
	if(m_Parent)
		delete m_Parent;
	m_Parent = m_Last = 0;
	m_NumGirls = 0;
#endif

	// load the number of girls
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>temp;
	for(int i=0; i<temp; i++)	// load each girl and add her
	{
		current = new sGirl();
		LoadGirlLegacy(current, ifs);
		AddGirl(current);
		current = 0;
	}
}

TiXmlElement* cGirls::SaveGirlsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pRoot->LinkEndChild(pGirls);
	sGirl* current = m_Parent;
	// save the number of girls
	while(current)
	{
		current->SaveGirlXML(pGirls);
		current = current->m_Next;
	}
	return pGirls;
}

// ----- Tag processing

void sRandomGirl::process_trait_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
/*
 *	we need to allocate a new sTrait scruct,
 */
	sTrait *trait = new sTrait();
/*
 *	get the trait name 
 */
	if((pt = el->Attribute("Name"))) {
		trait->m_Name = n_strdup(pt);
	}
/*
 *	store that in the next free index slot
 */
	m_Traits[m_NumTraits] = trait;
/*
 *	get the percentage chance
 */
	if((pt = el->Attribute("Percent", &ival))) {
		m_TraitChance[m_NumTraits] = ival;
	}
/*
 *	and whack up the trait count.
 */
	m_NumTraits ++;
}

void sRandomGirl::process_stat_xml(TiXmlElement *el)
{
	int ival, index;
	const char *pt;

	if((pt = el->Attribute("Name"))) {
		index = lookup->stat_lookup[pt];
	}
	else 
	{
		g_LogFile.os() << "can't find 'Name' attribute - can't process stat"
			 << endl;
		return;		// do as much as we can without crashing
	}
	if((pt = el->Attribute("Min", &ival))) {
		m_MinStats[index] = ival;
	}
	if((pt = el->Attribute("Max", &ival))) {
		m_MaxStats[index] = ival;
	}
}

void sRandomGirl::process_skill_xml(TiXmlElement *el)
{
	int ival, index;
	const char *pt;
/*
 *	Strictly, I should use something that lets me
 *	test for absence. This won't catch typos in the
 *	XML file
 */
	if((pt = el->Attribute("Name"))) {
		index = lookup->skill_lookup[pt];
	}
	else {
		g_LogFile.os() << "can't find 'Name' attribute - can't process skill"
			 << endl;
		return;		// do as much as we can without crashing
	}

	if((pt = el->Attribute("Min", &ival))) {
		m_MinSkills[index] = ival;
	}
	if((pt = el->Attribute("Max", &ival))) {
		m_MaxSkills[index] = ival;
	}
}

void sRandomGirl::process_cash_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;

	if((pt = el->Attribute("Min", &ival))) {
		g_LogFile.os() << "	min money = " << ival << endl;
		m_MinMoney = ival;
	}
	if((pt = el->Attribute("Max", &ival))) {
		g_LogFile.os() << "	max money = " << ival << endl;
		m_MaxMoney = ival;
	}
}

// ----- Equipment & inventory

int cGirls::HasItem(sGirl* girl, string name)
{
	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i])
		{
			if(girl->m_Inventory[i]->m_Name == name)
				return i;
		}
	}

	return -1;
}

void cGirls::EquipCombat(sGirl* girl)
{  // girl makes sure best armor and weapons are equipped, ready for combat
	cConfig cfg;
	if(!cfg.items.auto_combat_equip()) // this feature disabled in config?
		return;
	// if she's retarded, she might refuse or forget
	int refusal = 0;
	if(girl->has_trait("Retarded"))
		refusal += 30;
	if(g_Dice.percent(refusal))
		return;

	int Armor = -1, Weap1 = -1, Weap2 = -1;
	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i] != 0)
		{
			if(girl->m_Inventory[i]->m_Type == INVWEAPON)
			{
				g_InvManager.Unequip(girl, i);
				if(Weap1 == -1)
					Weap1 = i;
				else if(Weap2 == -1)
					Weap2 = i;
				else if(girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap1]->m_Cost)
				{
					Weap2 = Weap1;
					Weap1 = i;
				}
				else if(girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap2]->m_Cost)
					Weap2 = i;
			}
			if(girl->m_Inventory[i]->m_Type == INVARMOR)
			{
				g_InvManager.Unequip(girl, i);
				if(Armor == -1)
					Armor = i;
				else if(girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Armor]->m_Cost)
					Armor = i;
			}
		}
	}
	if(Armor > -1)
		g_InvManager.Equip(girl, Armor, false);
	if(Weap1 > -1)
		g_InvManager.Equip(girl, Weap1, false);
	if(Weap2 > -1)
		g_InvManager.Equip(girl, Weap2, false);
}

void cGirls::UnequipCombat(sGirl* girl)
{  // girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs	
	cConfig cfg;
	if(!cfg.items.auto_combat_equip()) // this feature disabled in config?
		return;
	// if she's a really rough or crazy bitch, she might just keep combat gear equipped
	int refusal = 0;
	if(girl->has_trait("Aggressive"))
		refusal += 30;
	if(girl->has_trait("Yandere"))
		refusal += 30;
	if(girl->has_trait("Twisted"))
		refusal += 30;
	if(girl->has_trait("Retarded"))
		refusal += 30;
	if(g_Dice.percent(refusal))
		return;

	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i] != 0)
		{
			sInventoryItem* curItem = girl->m_Inventory[i];
			if(curItem->m_Type == INVWEAPON)
				g_InvManager.Unequip(girl, i);
			else if(curItem->m_Type == INVARMOR)
			{
#if 0  // code to only unequip armor if it has bad effects... unfinished and disabled for the time being; not sure it should be used anyway
				bool badEffects = false;
				for(u_int j = 0; j < curItem->m_Effects.size(); j++)
				{
					sEffect* curEffect = &curItem->m_Effects[j];
					if (curEffect->m_Affects == sEffect::Stat && curEffect->m_Amount < 0)
					{
					}
					else if (curEffect->m_Affects == sEffect::Skill && curEffect->m_Amount < 0)
					{
					}
				}
				if(badEffects)
#endif
					g_InvManager.Unequip(girl, i);
			}
		}
	}
}

void cGirls::UseItems(sGirl* girl)
{
	bool withdraw = false;

	// uses drugs first
	if(HasTrait(girl, "Viras Blood Addict"))
	{
		int temp = HasItem(girl, "Vira Blood");
		
		if(temp == -1)	// withdrawals for a week
		{
			if(girl->m_Withdrawals >= 30)
				RemoveTrait(girl, "Viras Blood Addict");
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if(!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 10);
			g_InvManager.Equip(girl, temp, false);
		}
	}

	if(HasTrait(girl, "Fairy Dust Addict"))
	{
		int temp = HasItem(girl, "Fairy Dust");
		
		if(temp == -1)	// withdrawals for a week
		{
			if(girl->m_Withdrawals >= 20)
				RemoveTrait(girl, "Fairy Dust Addict");
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if(!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 5);
			g_InvManager.Equip(girl, temp, false);
		}
	}

	if(HasTrait(girl, "Shroud Addict"))
	{
		int temp = HasItem(girl, "Shroud Mushroom");
		
		if(temp == -1)	// withdrawals for a week
		{
			if(girl->m_Withdrawals >= 20)
				RemoveTrait(girl, "Shroud Addict");
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if(!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 2);
			g_InvManager.Equip(girl, temp, false);
		}
	}

	// sell crapy items
	for(int i=0; i<40; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		if(girl->m_Inventory[i] != 0)
		{
			int max = 0;
			switch((int)girl->m_Inventory[i]->m_Type)
			{
			case INVRING:
				if(max == 0)
					max = 8;
			case INVDRESS:
				if(max == 0)
					max = 1;
			case INVUNDERWEAR:
				if(max == 0)
					max = 1;
			case INVSHOES:
				if(max == 0)
					max = 1;
			case INVNECKLACE:
				if(max == 0)
					max = 1;
			case INVWEAPON:
				if(max == 0)
					max = 2;
			case INVSMWEAPON:
				if(max == 0)
					max = 1;
			case INVARMOR:
				if(max == 0)
					max = 1;
			case INVARMBAND:
				if(max == 0)
					max = 2;
				if(g_Girls.GetNumItemType(girl, girl->m_Inventory[i]->m_Type) > max) // MYR: Bug fix, was >=
				{
					int nicerThan = g_Girls.GetWorseItem(girl, girl->m_Inventory[i]->m_Type, girl->m_Inventory[i]->m_Cost);	// find a worse item of the same type
					if(nicerThan != -1)
						g_Girls.SellInvItem(girl, nicerThan);
				}
				break;
			}
		}
	}

	int usedFood = (g_Dice%3)+1;
	int usedFoodCount = 0;
	for(int i=0; i<40; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		sInventoryItem* curItem = girl->m_Inventory[i];
		if(curItem != 0)
		{
			if((curItem->m_Type == INVFOOD || curItem->m_Type == INVMAKEUP) && usedFoodCount < usedFood)
			{
				if(!g_Dice.percent(curItem->m_GirlBuyChance))  // make sure she'd want it herself
					continue;

				// make sure there's some reason for her to use it
				bool useful = false;
				for(u_int j = 0; j < curItem->m_Effects.size(); j++)
				{
					sEffect* curEffect = &curItem->m_Effects[j];
					if(curEffect->m_Affects == sEffect::Nothing)
					{  // really? it does nothing? sure, just for the hell of it
						useful = true;
						break;
					}
					else if (curEffect->m_Affects == sEffect::GirlStatus)
					{
						if( (curEffect->m_Amount == 0) && girl->m_States&(1<<curEffect->m_EffectID) )
						{  // hmm... I don't think she would really want any of the available states, so...
							useful = true;
							break;
						}
					}
					else if (curEffect->m_Affects == sEffect::Trait)
					{
						if( (curEffect->m_Amount >= 1) != girl->has_trait(curEffect->m_Trait) )
						{  // girl has trait and item removes it, or doesn't have trait and item adds it
							useful = true;
							break;
						}
					}
					else if (curEffect->m_Affects == sEffect::Stat)
					{
						unsigned int Stat = curEffect->m_EffectID;

						if( (curEffect->m_Amount > 0) &&
							(
								   Stat == STAT_CHARISMA
								|| Stat == STAT_HAPPINESS
								|| Stat == STAT_FAME
								|| Stat == STAT_LEVEL
								|| Stat == STAT_ASKPRICE
								|| Stat == STAT_EXP
								|| Stat == STAT_BEAUTY
							)
						)
						{  // even if this stat can't be increased further, she still wants it (call it vanity, greed, whatever)
							useful = true;
							break;
						}
						if( (curEffect->m_Amount > 0) && (girl->m_Stats[Stat] < 100) &&
							(
								   Stat == STAT_LIBIDO
								|| Stat == STAT_CONSTITUTION
								|| Stat == STAT_INTELLIGENCE
								|| Stat == STAT_CONFIDENCE
								|| Stat == STAT_MANA
								|| Stat == STAT_AGILITY
								|| Stat == STAT_SPIRIT
								|| Stat == STAT_HEALTH
							)
						)
						{  // this stat increase would be good
							useful = true;
							break;
						}
						if( (curEffect->m_Amount < 0) && (girl->m_Stats[Stat] > 0) &&
							(
								   Stat == STAT_AGE
								|| Stat == STAT_TIREDNESS
							)
						)
						{  // decreasing this stat would actually be good
							useful = true;
							break;
						}
					}
					else if (curEffect->m_Affects == sEffect::Skill)
					{
						if( (curEffect->m_Amount > 0) && (girl->m_Stats[curEffect->m_EffectID] < 100) )
						{  // skill would actually increase (wouldn't want to lose any skills)
							useful = true;
							break;
						}
					}
				}

				if(useful)
				{  // hey, this consumable item might actually be useful... gobble gobble gobble
					g_InvManager.Equip(girl, i, false);
					usedFoodCount++;
				}
			}

			// MYR: Girls shouldn't be able (IMHO) to take off things like control bracelets
			//else if(curItem->m_Badness > 20 && DisobeyCheck(girl, ACTION_GENERAL) && girl->m_EquipedItems[i] == 1)
			//{
			//	g_InvManager.Unequip(girl, i);
			//}
		}
	}

	// add the selling of items that are no longer needed here
}

bool cGirls::CanEquip(sGirl* girl, int num, bool force)
{
	if(force)
		return true;

	switch(girl->m_Inventory[num]->m_Type)
	{
	case INVRING:	// worn on fingers (max 8)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 8)
			return false;
		break;
	case INVDRESS:	// Worn on body, (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVUNDERWEAR:	// Worn on body, (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVSHOES:	// worn on feet, (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVFOOD:	// Eaten, single use
		return true;
		break;
	case INVNECKLACE:	// worn on neck, (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVWEAPON:	// equiped on body, (max 2)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2)
			return false;
		break;
	case INVSMWEAPON: // hidden on body, (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVMAKEUP:	// worn on face, single use
		return true;
		break;
	case INVARMOR:	// worn on body over dresses (max 1)
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1)
			return false;
		break;
	case INVMISC:		// these items don't usually do anythinig just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
		return true;
		break;
	case INVARMBAND:	// (max 2), worn around arms
		if(GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2)
			return false;
		break;
	}

	return true;
}

bool cGirls::IsItemEquipable(sGirl* girl, int num)
{
	switch(girl->m_Inventory[num]->m_Type) {
	case sInventoryItem::Ring:
	case sInventoryItem::Dress:
	case sInventoryItem::UnderWear:
	case sInventoryItem::Shoes:
	case sInventoryItem::Necklace:
	case sInventoryItem::Weapon:
	case sInventoryItem::Armor:
	case sInventoryItem::Armband:
		return true;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
	// adding this in explicitly - it was absent and returning false
	// this way gcc won't complain about the omission
	case sInventoryItem::SmWeapon:
		return false;
	}
	return false;
}

bool cGirls::EquipItem(sGirl* girl, int num, bool force)
{
	if(CanEquip(girl, num, force))
	{
		g_InvManager.Equip(girl, num, force);
		return true;
	}

	return false;
}

bool cGirls::IsInvFull(sGirl* girl)
{
	bool full = true;
	if(girl)
	{
	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i] == 0)
		{
			full = false;
			break;
		}
	}
	}

	return full;
}

int cGirls::AddInv(sGirl* girl, sInventoryItem* item)
{
	int i;
	for(i=0; i<40; i++)
	{
		if(girl->m_Inventory[i] == 0)
		{
			girl->m_Inventory[i] = item;
			girl->m_NumInventory++;
			if(item->m_Type == INVMISC)
				EquipItem(girl, i, true);
			return i;  // MYR: return i for success, -1 for failure
		}
	}

	return -1;
}

bool cGirls::RemoveInvByNumber(sGirl* girl, int Pos)
{
	// Girl inventories don't stack items
	if (girl->m_Inventory[Pos] != 0)
	{
		g_InvManager.Unequip(girl, Pos);
		girl->m_Inventory[Pos] = 0;
		girl->m_NumInventory--;
		return true;
	}
	return false;
}

void cGirls::SellInvItem(sGirl* girl, int num)
{
	girl->m_Money += (int)((float)girl->m_Inventory[num]->m_Cost*0.5f);
	girl->m_NumInventory--;
	g_InvManager.Unequip(girl, num);
	girl->m_Inventory[num] = 0;
}

int cGirls::GetWorseItem(sGirl* girl, int type, int cost)
{
	int ret = -1;
	if(girl->m_NumInventory == 0)
		return -1;

	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i])
		{
			if(girl->m_Inventory[i]->m_Type == type && girl->m_Inventory[i]->m_Cost < cost)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}

int cGirls::GetNumItemType(sGirl* girl, int Type)
{
	if(girl->m_NumInventory == 0)
		return 0;

	int num = 0;
	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i])
		{
			if(girl->m_Inventory[i]->m_Type == Type)
				num++;
		}
	}

	return num;
}

int cGirls::GetNumItemEquiped(sGirl* girl, int Type)
{
	if(girl->m_NumInventory == 0)
		return 0;

	int num = 0;
	for(int i=0; i<40; i++)
	{
		if(girl->m_Inventory[i])
		{
			if(girl->m_Inventory[i]->m_Type == Type && girl->m_EquipedItems[i] == 1)
				num++;
		}
	}

	return num;
}

// ----- Trait

//int cGirls::UnapplyTraits(sGirl* girl, sTrait* trait)	//	WD:	no idea as nothing uses the return value
void cGirls::UnapplyTraits(sGirl* girl, sTrait* trait)
{
	/* WD:
	*	Added doOnce = false; to end of fn
	*	else the fn will allways abort
	*/

	//	WD:	don't know why it has to be static
	//static bool doOnce = false;

	//static int num=0;
	//if(doOnce)
	//{
	//	doOnce=false;
	//	return num;
	//}

	bool doOnce = false;
	if(trait)
		doOnce = true;

	for(int i=0; i<girl->m_NumTraits || doOnce; i++)
	{
		sTrait* tr = 0;
		if(doOnce)
			tr = trait;
		else
			tr = girl->m_Traits[i];
		if(tr == 0)
			continue;


		if(strcmp(tr->m_Name, "Big Boobs") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Abnormally Large Boobs", false, false, true))
				AddTrait(girl, "Small Boobs", false, false, true);

			UpdateStat(girl,STAT_BEAUTY,-10);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateStat(girl,STAT_AGILITY,5);
			UpdateStat(girl,STAT_CHARISMA,-2);
			UpdateSkill(girl,SKILL_TITTYSEX,-15);
		}

		else if(strcmp(tr->m_Name, "Abnormally Large Boobs") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Big Boobs", false, false, true))
				AddTrait(girl, "Small Boobs", false, false, true);

			UpdateStat(girl,STAT_BEAUTY,-10);
			UpdateStat(girl,STAT_CONSTITUTION,-10);
			UpdateStat(girl,STAT_AGILITY,10);
			UpdateSkill(girl,SKILL_TITTYSEX,-20);
		}

		else if(strcmp(tr->m_Name, "Small Boobs") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Big Boobs", false, false, true))
				AddTrait(girl, "Abnormally Large Boobs", false, false, true);

			UpdateStat(girl,STAT_AGILITY,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,10);
		}

		else if(strcmp(tr->m_Name, "Fast orgasms") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Fake orgasm expert", false, false, true))
				AddTrait(girl, "Slow orgasms", false, false, true);

			UpdateStat(girl,STAT_LIBIDO,-10);
			UpdateSkill(girl,SKILL_ANAL,-10);
			UpdateSkill(girl,SKILL_BDSM,-10);
			UpdateSkill(girl,SKILL_NORMALSEX,-10);
			UpdateSkill(girl,SKILL_ORALSEX, -10);
			UpdateSkill(girl,SKILL_BEASTIALITY,-10);
			UpdateSkill(girl,SKILL_GROUP,-10);
			UpdateSkill(girl,SKILL_LESBIAN,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if(strcmp(tr->m_Name, "Fake orgasm expert") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Fast orgasms", false, false, true))
				AddTrait(girl, "Slow orgasms", false, false, true);

			UpdateSkill(girl,SKILL_ANAL,-2);
			UpdateSkill(girl,SKILL_BDSM,-2);
			UpdateSkill(girl,SKILL_NORMALSEX,-2);
			UpdateSkill(girl,SKILL_BEASTIALITY,-2);
			UpdateSkill(girl,SKILL_ORALSEX,-2);
			UpdateSkill(girl,SKILL_GROUP,-2);
			UpdateSkill(girl,SKILL_LESBIAN,-2);
		}

		else if(strcmp(tr->m_Name, "Slow orgasms") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Fast orgasms", false, false, true))
				AddTrait(girl, "Fake orgasm expert", false, false, true);

			UpdateSkill(girl,SKILL_ANAL,2);
			UpdateSkill(girl,SKILL_BDSM,2);
			UpdateSkill(girl,SKILL_NORMALSEX,2);
			UpdateSkill(girl,SKILL_BEASTIALITY,2);
			UpdateSkill(girl,SKILL_ORALSEX,2);
			UpdateSkill(girl,SKILL_GROUP,2);
			UpdateSkill(girl,SKILL_LESBIAN,2);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}

		else if(strcmp(tr->m_Name, "Different Colored Eyes") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Eye Patch", false, false, true))
				AddTrait(girl, "One Eye", false, false, true);

		}

		else if(strcmp(tr->m_Name, "Eye Patch") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Different Colored Eyes", false, false, true))
				AddTrait(girl, "One Eye", false, false, true);

			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
		}

		else if(strcmp(tr->m_Name, "One Eye") == 0)
		{
			// Can only have one trait added
			if (!AddTrait(girl, "Different Colored Eyes", false, false, true))
				AddTrait(girl, "Eye Patch", false, false, true);

			UpdateStat(girl,STAT_BEAUTY,20);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateStat(girl,STAT_SPIRIT,10);
		}

		else if(strcmp(tr->m_Name, "Fearless") == 0)
		{
			// OK Not incompatible traits
			AddTrait(girl, "Meek", false, false, true);
			AddTrait(girl, "Dependant", false, false, true);
			AddTrait(girl, "Nervous", false, false, true);

			UpdateStat(girl,STAT_PCFEAR,-200);
			UpdateStat(girl, STAT_SPIRIT, 30);
		}

		else if(strcmp(tr->m_Name, "Meek") == 0)
		{
			// OK Not incompatible traits
			AddTrait(girl, "Aggressive", false, false, true);
			AddTrait(girl, "Fearless", false, false, true);

			UpdateStat(girl, STAT_CONFIDENCE, 20);
			UpdateStat(girl, STAT_OBEDIENCE, -20);
			UpdateStat(girl, STAT_SPIRIT, 20);
		}

		/////////////////////////////////////////////////////////////
		else if(strcmp(tr->m_Name, "Good Kisser") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,-2);
			UpdateStat(girl,STAT_CONFIDENCE,-2);
			UpdateStat(girl,STAT_CHARISMA,-2);
			UpdateSkill(girl,SKILL_SERVICE,-5);
		}

		else if(strcmp(tr->m_Name, "Long Legs") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-10);
		}

		else if(strcmp(tr->m_Name, "Perky Nipples") == 0)
		{
			AddTrait(girl, "Puffy Nipples", false, false, true);
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,-5);
		}

		else if(strcmp(tr->m_Name, "Retarded") == 0)
		{
// WD:		UpdateStat(girl,STAT_CONFIDENCE,20);
			UpdateStat(girl,STAT_SPIRIT,20);
			UpdateStat(girl,STAT_INTELLIGENCE,50);
// WD:		UpdateStat(girl,STAT_CONFIDENCE,40);
			UpdateStat(girl,STAT_CONFIDENCE,60);
		}

		else if(strcmp(tr->m_Name, "Malformed") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,20);
			UpdateStat(girl,STAT_SPIRIT,20);
			UpdateStat(girl,STAT_INTELLIGENCE,10);
			UpdateStat(girl,STAT_BEAUTY,20);
		}

		else if(strcmp(tr->m_Name, "Puffy Nipples") == 0)
		{
			AddTrait(girl, "Perky Nipples", false, false, true);
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,-5);
		}

		else if(strcmp(tr->m_Name, "Charming") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-30);
			UpdateStat(girl,STAT_BEAUTY,-15);
		}

		else if(strcmp(tr->m_Name, "Charismatic") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-50);
			UpdateStat(girl,STAT_BEAUTY,-10);
		}

		else if(strcmp(tr->m_Name, "Construct") == 0)
		{
			AddTrait(girl, "Half-Construct", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Half-Construct") == 0)
		{
			AddTrait(girl, "Construct", false, false, true);
			UpdateStat(girl, STAT_CONSTITUTION, -20);
			UpdateSkill(girl, SKILL_COMBAT, -20);
		}

		else if(strcmp(tr->m_Name, "Fragile") == 0)
		{
			AddTrait(girl, "Tough", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Tough") == 0)
		{
			AddTrait(girl, "Fragile", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Mind Fucked") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, 50);
			UpdateStat(girl, STAT_OBEDIENCE, -100);
			UpdateStat(girl, STAT_SPIRIT, 50);
		}

		else if(strcmp(tr->m_Name, "Fleet of Foot") == 0)
		{
			UpdateStat(girl, STAT_AGILITY, -50);
		}

		else if(strcmp(tr->m_Name, "Dependant") == 0)
		{
			AddTrait(girl, "Fearless", false, false, true);
			UpdateStat(girl, STAT_SPIRIT, 30);
			UpdateStat(girl, STAT_OBEDIENCE, -50);
		}

		else if(strcmp(tr->m_Name, "Pessimist") == 0)
		{
			AddTrait(girl, "Optimist", false, false, true);
			UpdateStat(girl, STAT_SPIRIT, 60);
		}

		else if(strcmp(tr->m_Name, "Optimist") == 0)
		{
			AddTrait(girl, "Pessimist", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Iron Will") == 0)
		{
			AddTrait(girl, "Broken Will", false, false, true);
			UpdateStat(girl, STAT_PCFEAR, 60);
			UpdateStat(girl, STAT_SPIRIT, -60);
			UpdateStat(girl, STAT_OBEDIENCE, 90);
		}

		else if(strcmp(tr->m_Name, "Merciless") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -20);
		}

		else if(strcmp(tr->m_Name, "Manly") == 0)
		{
			AddTrait(girl, "Elegant", false, false, true);
			UpdateStat(girl, STAT_CONSTITUTION, -10);
			UpdateStat(girl, STAT_CONFIDENCE, -10);
			UpdateStat(girl, STAT_OBEDIENCE, 10);
			UpdateStat(girl, STAT_SPIRIT, -10);
		}

		else if(strcmp(tr->m_Name, "Tsundere") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, -20);
			UpdateStat(girl, STAT_OBEDIENCE, 20);
		}

		else if(strcmp(tr->m_Name, "Yandere") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, -20);
		}

		else if(strcmp(tr->m_Name, gettext("Lesbian")) == 0)
		{
			UpdateSkill(girl, SKILL_LESBIAN, -40);
		}

		else if(strcmp(tr->m_Name, "Masochist") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, -50);
			UpdateStat(girl,STAT_CONSTITUTION, -50);
			UpdateStat(girl,STAT_OBEDIENCE,-30);
		}

		else if(strcmp(tr->m_Name, "Sadistic") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, -20);
		}

		else if(strcmp(tr->m_Name, "Broken Will") == 0)
		{
			AddTrait(girl, "Iron Will", false, false, true);
			UpdateStat(girl,STAT_SPIRIT, 20);
			UpdateStat(girl,STAT_OBEDIENCE, 10);
		}

		else if(strcmp(tr->m_Name, "Nymphomaniac") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,-20);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateEnjoyment(girl, ACTION_SEX, -21, true);
		}

		else if(strcmp(tr->m_Name, "Elegant") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
		}

		else if(strcmp(tr->m_Name, "Sexy Air") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}

		else if(strcmp(tr->m_Name, "Great Arse") == 0)
		{
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateSkill(girl,SKILL_ANAL,-10);
		}

		else if(strcmp(tr->m_Name, "Small Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,-2);
			UpdateStat(girl,STAT_SPIRIT,2);
		}

		else if(strcmp(tr->m_Name, "Cool Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,5);
			UpdateStat(girl,STAT_CHARISMA,2);
		}

		else if(strcmp(tr->m_Name, "Horrific Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CONSTITUTION,-10);
			UpdateStat(girl,STAT_SPIRIT,10);
		}

		else if(strcmp(tr->m_Name, "Cool Person") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_SPIRIT,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if(strcmp(tr->m_Name, "Nerd") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Clumsy") == 0)
		{
			UpdateSkill(girl,SKILL_ANAL,10);
			UpdateSkill(girl,SKILL_BDSM,10);
			UpdateSkill(girl,SKILL_NORMALSEX,10);
			UpdateSkill(girl,SKILL_ORALSEX,10);
			UpdateSkill(girl,SKILL_BEASTIALITY,10);
			UpdateSkill(girl,SKILL_GROUP,10);
			UpdateSkill(girl,SKILL_LESBIAN,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Cute") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-5);
		}

		else if(strcmp(tr->m_Name, "Strong") == 0)
		{
			UpdateSkill(girl,SKILL_COMBAT,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if(strcmp(tr->m_Name, "Strong Magic") == 0)
		{
			UpdateSkill(girl,SKILL_MAGIC,-20);
			UpdateStat(girl,STAT_MANA,-20);
		}

		else if(strcmp(tr->m_Name, "Shroud Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
		}

		else if(strcmp(tr->m_Name, "Fairy Dust Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,10);
			UpdateStat(girl,STAT_OBEDIENCE,-5);
		}

		else if(strcmp(tr->m_Name, "Viras Blood Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,15);
			UpdateStat(girl,STAT_OBEDIENCE,-20);
			UpdateStat(girl,STAT_CHARISMA,10);
		}

		else if(strcmp(tr->m_Name, "Aggressive") == 0)
		{
			AddTrait(girl, "Meek", false, false, true);
			UpdateSkill(girl, SKILL_COMBAT, -10);
			UpdateStat(girl, STAT_SPIRIT, -10);
			UpdateStat(girl, STAT_CONFIDENCE, -5);
		}

		else if(strcmp(tr->m_Name, "Adventurer") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -10);
		}

		else if(strcmp(tr->m_Name, "Assassin") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -15);
		}

		else if(strcmp(tr->m_Name, "Lolita") == 0)
		{
			AddTrait(girl, "MILF", false, false, true);
			UpdateStat(girl,STAT_OBEDIENCE,-5);
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

		else if(strcmp(tr->m_Name, "MILF") == 0)
		{
			AddTrait(girl, "Lolita", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Nervous") == 0)
		{
			AddTrait(girl, "Fearless", false, false, true);
			UpdateStat(girl,STAT_OBEDIENCE,-10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Great Figure") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

		else if(strcmp(tr->m_Name, "Incorporeal") == 0)
		{
			RemoveTrait(girl, "Sterile");
		}

		else if(strcmp(tr->m_Name, "Quick Learner") == 0)
		{
			AddTrait(girl, "Slow Learner", false, false, true);
		}
		else if(strcmp(tr->m_Name, "Slow Learner") == 0)
		{
			AddTrait(girl, "Quick Learner", false, false, true);
		}

		else if(strcmp(tr->m_Name, "Shape Shifter") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-20);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

 		else if(strcmp(tr->m_Name, "Queen") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,-20);
 			UpdateStat(girl,STAT_BEAUTY,-20);
 			UpdateStat(girl,STAT_CONFIDENCE,-10);
 			UpdateStat(girl,STAT_OBEDIENCE,15);
 		}
 
 		else if(strcmp(tr->m_Name, "Princess") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,-10);
 			UpdateStat(girl,STAT_BEAUTY,-10);
 			UpdateStat(girl,STAT_CONFIDENCE,-10);
 			UpdateStat(girl,STAT_OBEDIENCE,5);
 		}

		else if(strcmp(tr->m_Name, "Pierced Nipples") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,-10);
			UpdateSkill(girl, SKILL_TITTYSEX, -10);
 		}
 
 		else if(strcmp(tr->m_Name, "Pierced Tongue") == 0)
 		{
 			UpdateSkill(girl, SKILL_ORALSEX, -10);
 		}

		else if(strcmp(tr->m_Name, "Pierced Clit") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,-20);
			UpdateSkill(girl, SKILL_NORMALSEX, -10);
 		}
		else if(strcmp(tr->m_Name, "Gag Reflex") == 0)
		{
			AddTrait(girl, "No Gag Relex", false, false, true);
			UpdateSkill(girl, SKILL_ORALSEX, 50);
		}
		else if(strcmp(tr->m_Name, "No Gag Reflex") == 0)
		{
			AddTrait(girl, "Gag Relex", false, false, true);
			UpdateSkill(girl, SKILL_ORALSEX, -30);
		}
 

		if(doOnce)
		{
			// WD: 	Added to stop fn from aborting
			//doOnce = false;
			//return i;
			break;
		}
	}
//	return -1;
}

// If a girl enjoys a job enough, she has a chance of gaining traits associated with it
// (Made a FN out of code appearing in WorkExploreCatacombs etc...)

bool cGirls::PossiblyGainNewTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool DayNight)
{
	if(girl->m_Enjoyment[ActionType] > Threshold && !girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if(g_Dice.percent(chance))
		{
			girl->add_trait(Trait, false);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, EVENT_WARNING);
			return true;
		}
	}
	return false;
}

bool cGirls::PossiblyLoseExistingTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool DayNight)
{
	if(girl->m_Enjoyment[ActionType] > Threshold && girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if(g_Dice.percent(chance))
		{
			girl->remove_trait(Trait);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, EVENT_WARNING);
			return true;
		}
	}
	return false;
}

void cGirls::ApplyTraits(sGirl* girl, sTrait* trait, bool rememberflag)
{
	/* WD:
	*	Added doOnce = false; to end of fn
	*	else the fn will allways abort
	*/

	//	WD:	don't know why it has to be static
	//static bool doOnce = false;

	//if(doOnce)
	//	return;

	bool doOnce = false;
	if(trait)
		doOnce = true;

	for(int i=0; i<girl->m_NumTraits || doOnce; i++)
	{
		sTrait* tr = 0;
		if(doOnce)
			tr = trait;
		else
			tr = girl->m_Traits[i];
		if(tr == 0)
			continue;
			
		if(strcmp(tr->m_Name, "Big Boobs") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Abnormally Large Boobs", true, true))
			//	RemoveTrait(girl, "Small Boobs", false, true);
			//else
			//	RemoveTrait(girl, "Small Boobs", true, true);

			RemoveTrait(girl, "Abnormally Large Boobs", rememberflag, true);
			RemoveTrait(girl, "Small Boobs", rememberflag, true);

			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_AGILITY,-5);
			UpdateStat(girl,STAT_CHARISMA,2);
			UpdateSkill(girl,SKILL_TITTYSEX,15);
		}

		else if(strcmp(tr->m_Name, "Abnormally Large Boobs") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Big Boobs", true, true))
			//	RemoveTrait(girl, "Small Boobs", false, true);
			//else
			//	RemoveTrait(girl, "Small Boobs", true, true);

			RemoveTrait(girl, "Big Boobs", rememberflag, true);
			RemoveTrait(girl, "Small Boobs", rememberflag, true);

			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONSTITUTION,10);
			UpdateStat(girl,STAT_AGILITY,-10);
			UpdateSkill(girl,SKILL_TITTYSEX,20);
		}
		
		else if(strcmp(tr->m_Name, "Small Boobs") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Big Boobs", true, true))
			//	RemoveTrait(girl, "Abnormally Large Boobs", false, true);
			//else
			//	RemoveTrait(girl, "Abnormally Large Boobs", true, true);

			RemoveTrait(girl, "Big Boobs", rememberflag, true);
			RemoveTrait(girl, "Abnormally Large Boobs", rememberflag, true);
			
			UpdateStat(girl,STAT_AGILITY,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateSkill(girl,SKILL_TITTYSEX,-15);
		}

		else if(strcmp(tr->m_Name, "Fast orgasms") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Fake orgasm expert", true, true))
			//	RemoveTrait(girl, "Slow orgasms", false, true);
			//else
			//	RemoveTrait(girl, "Slow orgasms", true, true);
				
			RemoveTrait(girl, "Fake orgasm expert", rememberflag, true);
			RemoveTrait(girl, "Slow orgasms", rememberflag, true);

			UpdateStat(girl,STAT_LIBIDO,10);
			UpdateSkill(girl,SKILL_ANAL,10);
			UpdateSkill(girl,SKILL_BDSM,10);
			UpdateSkill(girl,SKILL_NORMALSEX,10);
			UpdateSkill(girl,SKILL_ORALSEX,10);
			UpdateSkill(girl,SKILL_BEASTIALITY,10);
			UpdateSkill(girl,SKILL_GROUP,10);
			UpdateSkill(girl,SKILL_LESBIAN,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
			UpdateEnjoyment(girl, ACTION_SEX, +10, true);
		}
		
		else if(strcmp(tr->m_Name, "Fake orgasm expert") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Fast orgasms", true, true))
			//	RemoveTrait(girl, "Slow orgasms", false, true);
			//else
			//	RemoveTrait(girl, "Slow orgasms", true, true);
				
			RemoveTrait(girl, "Slow orgasms", rememberflag, true);
			RemoveTrait(girl, "Fast orgasms", rememberflag, true);

			UpdateSkill(girl,SKILL_ANAL,2);
			UpdateSkill(girl,SKILL_BDSM,2);
			UpdateSkill(girl,SKILL_NORMALSEX,2);
			UpdateSkill(girl,SKILL_BEASTIALITY,2);
			UpdateSkill(girl,SKILL_ORALSEX,2);
			UpdateSkill(girl,SKILL_GROUP,2);
			UpdateSkill(girl,SKILL_LESBIAN,2);
		}


		else if(strcmp(tr->m_Name, "Slow orgasms") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Fast orgasms", true, true))
			//	RemoveTrait(girl, "Fake orgasm expert", false, true);
			//else
			//	RemoveTrait(girl, "Fake orgasm expert", true, true);	

			RemoveTrait(girl, "Fake orgasm expert", rememberflag, true);
			RemoveTrait(girl, "Fast orgasms", rememberflag, true);

			UpdateSkill(girl,SKILL_ANAL,-2);
			UpdateSkill(girl,SKILL_BDSM,-2);
			UpdateSkill(girl,SKILL_NORMALSEX,-2);
			UpdateSkill(girl,SKILL_BEASTIALITY,-2);
			UpdateSkill(girl,SKILL_ORALSEX,-2);
			UpdateSkill(girl,SKILL_GROUP,-2);
			UpdateSkill(girl,SKILL_LESBIAN,-2);
			UpdateStat(girl,STAT_CONFIDENCE,-2);
			UpdateEnjoyment(girl, ACTION_SEX, -10, true);
		}

		else if(strcmp(tr->m_Name, "Different Colored Eyes") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Eye Patch", true, true))
			//	RemoveTrait(girl, "One Eye", false, true);
			//else
			//	RemoveTrait(girl, "One Eye", true, true);	

			RemoveTrait(girl, "Eye Patch", rememberflag, true);	
			RemoveTrait(girl, "One Eye", rememberflag, true);	
		}

		else if(strcmp(tr->m_Name, "Eye Patch") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "One Eye", true, true))
			//	RemoveTrait(girl, "Different Colored Eyes", false, true);
			//else
			//	RemoveTrait(girl, "Different Colored Eyes", true, true);	

			RemoveTrait(girl, "Different Colored Eyes", rememberflag, true);	
			RemoveTrait(girl, "One Eye", rememberflag, true);	

			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
		}

		else if(strcmp(tr->m_Name, "One Eye") == 0)
		{
			// should only have one trait but lets make sure
			//if (RemoveTrait(girl, "Different Colored Eyes", true, true))
			//	RemoveTrait(girl, "Eye Patch", false, true);
			//else
			//	RemoveTrait(girl, "Eye Patch", true, true);	

			RemoveTrait(girl, "Eye Patch", rememberflag, true);	
			RemoveTrait(girl, "Different Colored Eyes", rememberflag, true);	

			UpdateStat(girl,STAT_BEAUTY,-20);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,-10);
		}

		else if(strcmp(tr->m_Name, "Fearless") == 0)
		{
			// OK Not incompatible traits
			RemoveTrait(girl, "Meek", rememberflag, true);
			RemoveTrait(girl, "Dependant", rememberflag, true);
			RemoveTrait(girl, "Nervous", rememberflag, true);
			
			UpdateStat(girl,STAT_PCFEAR,-200);
			UpdateStat(girl, STAT_SPIRIT, 30);
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}
	
		else if(strcmp(tr->m_Name, "Meek") == 0)
		{
			// OK Not incompatible traits
			RemoveTrait(girl, "Aggressive", rememberflag, true);
			RemoveTrait(girl, "Fearless", rememberflag, true);
			
			UpdateStat(girl, STAT_CONFIDENCE, -20);
			UpdateStat(girl, STAT_OBEDIENCE, 20);
			UpdateStat(girl, STAT_SPIRIT, -20);
			UpdateEnjoyment(girl, ACTION_COMBAT, -20, true);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
		}
	
			
		/////////////////////////////////////////////////////////////
		else if(strcmp(tr->m_Name, "Good Kisser") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,2);
			UpdateStat(girl,STAT_CONFIDENCE,2);
			UpdateStat(girl,STAT_CHARISMA,2);
			UpdateSkill(girl,SKILL_SERVICE,5);
			UpdateEnjoyment(girl, ACTION_SEX, +5, true);
		}

		else if(strcmp(tr->m_Name, "Retarded") == 0)
		{
//	WD:		UpdateStat(girl,STAT_CONFIDENCE,-20);
			UpdateStat(girl,STAT_SPIRIT,-20);
			UpdateStat(girl,STAT_INTELLIGENCE,-50);
//	WD:		UpdateStat(girl,STAT_CONFIDENCE,-40);
			UpdateStat(girl,STAT_CONFIDENCE,-60);
		}

		else if(strcmp(tr->m_Name, "Malformed") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-20);
			UpdateStat(girl,STAT_SPIRIT,-20);
			UpdateStat(girl,STAT_INTELLIGENCE,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

		else if(strcmp(tr->m_Name, "Long Legs") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,10);
		}

		else if(strcmp(tr->m_Name, "Perky Nipples") == 0)
		{
			RemoveTrait(girl, "Puffy Nipples", rememberflag, true);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateSkill(girl,SKILL_TITTYSEX,5);
		}

		else if(strcmp(tr->m_Name, "Puffy Nipples") == 0)
		{
			RemoveTrait(girl, "Perky Nipples", rememberflag, true);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateSkill(girl,SKILL_TITTYSEX,5);
		}

		else if(strcmp(tr->m_Name, "Charismatic") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,50);
			UpdateStat(girl,STAT_BEAUTY,10);
		}

		else if(strcmp(tr->m_Name, "Charming") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,30);
			UpdateStat(girl,STAT_BEAUTY,15);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +20, true);
		}

		else if(strcmp(tr->m_Name, "Construct") == 0)
		{
			RemoveTrait(girl, "Half-Construct", rememberflag, true);
		}

		else if(strcmp(tr->m_Name, "Half-Construct") == 0)
		{
			RemoveTrait(girl, "Construct", rememberflag, true);
			UpdateStat(girl, STAT_CONSTITUTION, 20);
			UpdateSkill(girl, SKILL_COMBAT, 20);
		}

		else if(strcmp(tr->m_Name, "Fragile") == 0)
		{
			RemoveTrait(girl, "Tough", rememberflag, true);
		}

		else if(strcmp(tr->m_Name, "Tough") == 0)
		{
			RemoveTrait(girl, "Fragile", rememberflag, true);
		}

		else if(strcmp(tr->m_Name, "Fleet of Foot") == 0)
		{
			UpdateStat(girl, STAT_AGILITY, 50);
		}

		else if(strcmp(tr->m_Name, "Dependant") == 0)
		{
			RemoveTrait(girl, "Fearless", rememberflag, true);
			UpdateStat(girl, STAT_SPIRIT, -30);
			UpdateStat(girl, STAT_OBEDIENCE, 50);
			UpdateEnjoyment(girl, ACTION_COMBAT, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, -20, true);
		}

		else if(strcmp(tr->m_Name, "Pessimist") == 0)
		{
			RemoveTrait(girl, "Optimist", rememberflag, true);
			UpdateStat(girl, STAT_SPIRIT, -60);
			UpdateEnjoyment(girl, ACTION_WORKADVERTISING, -20, true);
		}

		else if(strcmp(tr->m_Name, "Optimist") == 0)
		{
			RemoveTrait(girl, "Pessimist", rememberflag, true);
			UpdateEnjoyment(girl, ACTION_WORKADVERTISING, +20, true);
		}

		else if(strcmp(tr->m_Name, "Iron Will") == 0)
		{
			RemoveTrait(girl, "Broken Will", rememberflag, true);
			UpdateStat(girl, STAT_PCFEAR, -60);
			UpdateStat(girl, STAT_SPIRIT, 60);
			UpdateStat(girl, STAT_OBEDIENCE, -90);
		}

		else if(strcmp(tr->m_Name, "Merciless") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 20);
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}

		else if(strcmp(tr->m_Name, "Manly") == 0)
		{
			RemoveTrait(girl, "Elegant", rememberflag, true);
			UpdateStat(girl, STAT_CONSTITUTION, 10);
			UpdateStat(girl, STAT_CONFIDENCE, 10);
			UpdateStat(girl, STAT_OBEDIENCE, -10);
			UpdateStat(girl, STAT_SPIRIT, 10);
		}

		else if(strcmp(tr->m_Name, "Mind Fucked") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, -50);
			UpdateStat(girl, STAT_OBEDIENCE, 100);
			UpdateStat(girl, STAT_SPIRIT, -50);
		}

		else if(strcmp(tr->m_Name, "Tsundere") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, 20);
			UpdateStat(girl, STAT_OBEDIENCE, -20);
		}

		else if(strcmp(tr->m_Name, "Yandere") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, 20);
		}

		else if(strcmp(tr->m_Name, "Lesbian") == 0)
		{
			UpdateSkill(girl, SKILL_LESBIAN, 40);
		}

		else if(strcmp(tr->m_Name, "Masochist") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, 50);
			UpdateStat(girl,STAT_CONSTITUTION, 50);
			UpdateStat(girl,STAT_OBEDIENCE,30);
		}

		else if(strcmp(tr->m_Name, "Sadistic") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, 20);
		}

		else if(strcmp(tr->m_Name, "Broken Will") == 0)
		{
			RemoveTrait(girl, "Iron Will", rememberflag, true);
			UpdateStat(girl,STAT_SPIRIT,-100);
			UpdateStat(girl,STAT_OBEDIENCE,100);
		}

		else if(strcmp(tr->m_Name, "Nymphomaniac") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,20);
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateEnjoyment(girl, ACTION_SEX, +25, true);
		}

		else if(strcmp(tr->m_Name, "Elegant") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKMATRON, +20, true);
		}

		else if(strcmp(tr->m_Name, "Sexy Air") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}

		else if(strcmp(tr->m_Name, "Great Arse") == 0)
		{
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateSkill(girl,SKILL_ANAL,10);
		}

		else if(strcmp(tr->m_Name, "Small Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-2);
			UpdateStat(girl,STAT_CONSTITUTION,2);
			UpdateStat(girl,STAT_SPIRIT,-2);
		}

		else if(strcmp(tr->m_Name, "Cool Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,5);
			UpdateStat(girl,STAT_CHARISMA,2);
		}

		else if(strcmp(tr->m_Name, "Horrific Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CONSTITUTION,10);
			UpdateStat(girl,STAT_SPIRIT,-10);
		}

		else if(strcmp(tr->m_Name, "Cool Person") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_SPIRIT,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Nerd") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateStat(girl,STAT_INTELLIGENCE,10);
		}

		else if(strcmp(tr->m_Name, "Clumsy") == 0)
		{
			UpdateSkill(girl,SKILL_ANAL,-10);
			UpdateSkill(girl,SKILL_BDSM,-10);
			UpdateSkill(girl,SKILL_NORMALSEX,-10);
			UpdateSkill(girl,SKILL_BEASTIALITY,-10);
			UpdateSkill(girl,SKILL_ORALSEX,-10);
			UpdateSkill(girl,SKILL_GROUP,-10);
			UpdateSkill(girl,SKILL_LESBIAN,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateEnjoyment(girl, ACTION_WORKCLEANING, -20, true);
		}

		else if(strcmp(tr->m_Name, "Cute") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, +10, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +15, true);
		}

		else if(strcmp(tr->m_Name, "Strong") == 0)
		{
			UpdateSkill(girl,SKILL_COMBAT,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Strong Magic") == 0)
		{
			UpdateSkill(girl,SKILL_MAGIC,20);
			UpdateStat(girl,STAT_MANA,20);
		}

		else if(strcmp(tr->m_Name, "Shroud Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-5);
			UpdateStat(girl,STAT_OBEDIENCE,5);
		}

		else if(strcmp(tr->m_Name, "Fairy Dust Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateStat(girl,STAT_OBEDIENCE,5);
		}

		else if(strcmp(tr->m_Name, "Viras Blood Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-15);
			UpdateStat(girl,STAT_OBEDIENCE,20);
			UpdateStat(girl,STAT_CHARISMA,-10);
		}

		else if(strcmp(tr->m_Name, "Aggressive") == 0)
		{
			RemoveTrait(girl, "Meek", rememberflag, true);
			UpdateSkill(girl, SKILL_COMBAT, 10);
			UpdateStat(girl, STAT_SPIRIT, 10);
			UpdateStat(girl, STAT_CONFIDENCE, 5);
			UpdateEnjoyment(girl, ACTION_COMBAT, +10, true);
			UpdateEnjoyment(girl, ACTION_WORKSECURITY, +20, true);
		}

		else if(strcmp(tr->m_Name, "Adventurer") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 10);
			UpdateEnjoyment(girl, ACTION_COMBAT, +10, true);
		}

		else if(strcmp(tr->m_Name, "Assassin") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 15);
			UpdateEnjoyment(girl, ACTION_COMBAT, +15, true);
			UpdateEnjoyment(girl, ACTION_WORKSECURITY, +20, true);
		}

		else if(strcmp(tr->m_Name, "Lolita") == 0)
		{
			RemoveTrait(girl, "MILF", rememberflag, true);
			UpdateStat(girl,STAT_OBEDIENCE,5);
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,20);
		}

		else if(strcmp(tr->m_Name, "MILF") == 0)
		{
			RemoveTrait(girl, "Lolita", rememberflag, true);
		}

		else if(strcmp(tr->m_Name, "Nervous") == 0)
		{
			RemoveTrait(girl, "Fearless", rememberflag, true);
			UpdateStat(girl,STAT_OBEDIENCE,10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, -10, true);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, -5, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, -5, true);
		}

		else if(strcmp(tr->m_Name, "Great Figure") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,20);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +20, true);
		}

		else if(strcmp(tr->m_Name, "Incorporeal") == 0)
		{
			AddTrait(girl, "Sterile");
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}

		else if(strcmp(tr->m_Name, "Quick Learner") == 0)
		{
			RemoveTrait(girl, "Slow Learner", rememberflag, true);
		}

		else if(strcmp(tr->m_Name, "Slow Learner") == 0)
		{
			RemoveTrait(girl, "Quick Learner", rememberflag, true);
		}	

		else if(strcmp(tr->m_Name, "Shape Shifter") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,20);
			UpdateStat(girl,STAT_BEAUTY,20);
		}

 		else if(strcmp(tr->m_Name, "Queen") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,20);
 			UpdateStat(girl,STAT_BEAUTY,20);
 			UpdateStat(girl,STAT_CONFIDENCE,10);
 			UpdateStat(girl,STAT_OBEDIENCE,-15);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
 		}
 
 		else if(strcmp(tr->m_Name, "Princess") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,10);
 			UpdateStat(girl,STAT_BEAUTY,10);
 			UpdateStat(girl,STAT_CONFIDENCE,10);
 			UpdateStat(girl,STAT_OBEDIENCE,-5);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
 		}

		else if(strcmp(tr->m_Name, "Pierced Nipples") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,10);
			UpdateSkill(girl, SKILL_TITTYSEX, 10);
		}

 		else if(strcmp(tr->m_Name, "Pierced Tongue") == 0)
 		{
 			UpdateSkill(girl, SKILL_ORALSEX, 10);
 		}
 
 		else if(strcmp(tr->m_Name, "Pierced Clit") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,20);
			UpdateSkill(girl, SKILL_NORMALSEX, 10);
			UpdateEnjoyment(girl, ACTION_SEX, 10, true);
 		}
		else if(strcmp(tr->m_Name, "Gag Reflex") == 0)
		{
			RemoveTrait(girl, "No Gag Reflex", rememberflag, true);
			UpdateSkill(girl, SKILL_ORALSEX, -50);
		}
		else if(strcmp(tr->m_Name, "No Gag Reflex") == 0)
		{
			RemoveTrait(girl, "Gag Reflex", rememberflag, true);
			UpdateSkill(girl, SKILL_ORALSEX, 30);
		}
 
		if(doOnce)
		{
			// WD: 	Added to stop fn from aborting
			// doOnce = false;
			break;
		}
	}
}

bool cGirls::HasRememberedTrait(sGirl* girl, string trait)
{
	for(int i=0; i<MAXNUM_TRAITS*2; i++)
	{
		if(girl->m_RememTraits[i])
		{
			if(trait.compare(girl->m_RememTraits[i]->m_Name) == 0)
				return true;
		}
	}
	return false;
}

bool cGirls::HasTrait(sGirl* girl, string trait)
{
	for(int i=0; i<MAXNUM_TRAITS; i++)
	{
		if(girl->m_Traits[i])
		{
			if(trait.compare(girl->m_Traits[i]->m_Name) == 0)
				return true;
		}
	}
	return false;
}

void cGirls::RemoveRememberedTrait(sGirl* girl, string name)
{
	sTrait* trait = g_Traits.GetTrait(name);
	for(int i=0; i<MAXNUM_TRAITS*2; i++)	// remove the traits
	{
		if(girl->m_RememTraits[i])
		{
			if(girl->m_RememTraits[i] == trait)
			{
				girl->m_NumRememTraits--;
				girl->m_RememTraits[i] = 0;
				return;
			}
		}
	}
}

void cGirls::RemoveAllRememberedTraits(sGirl* girl)
{
	/*
	 *	WD: Remove all traits to for new girl creation
	 *		is required after using ApplyTraits() & UnApplyTraits()
	 *		can create remembered traits especially random girls
	 */

	for(int i=0; i<MAXNUM_TRAITS*2; i++)
	{
		// WD:	This should be faster 121 writes instead of 120 reads and (120 + numTraits) writes
		girl->m_RememTraits[i] = 0;

		//if(girl->m_RememTraits[i])
		//{
		//	girl->m_NumRememTraits--;
		//	girl->m_RememTraits[i] = 0;
		//}
	}
	girl->m_NumRememTraits = 0;
}

bool cGirls::RemoveTrait(sGirl* girl, string name, bool addrememberlist, bool force)
{	
	/*
	 *	WD: Added logic for remembered trait
	 *
	 *		addrememberlist = true when trait is  
	 *		to be added to the remember list
	 *
	 *		force = true when you must force
	 *		removal of the trait if active even if 
	 *		it is on the remember list. 
	 *
	 *		Returns true if there was a active trait
	 *		or remember list entry removed
	 *
	 *		This should fix items with duplicate 
	 *		traits and loss of original trait if 
	 *		overwritten by a trait from an item
	 *		that is later removed
	 */

	 
	
	bool hasRemTrait = HasRememberedTrait(girl, name);
	
	if(!HasTrait(girl, name))							// WD:	no active trait to remove
#if 0
		if (!force && hasRemTrait)						// WD:	try remembered trait
		{
			RemoveRememberedTrait(girl, name);
			return true;
		}
		else
			return false;		
#else
	{
		if (hasRemTrait)								// WD:	try remembered trait
		{
			RemoveRememberedTrait(girl, name);
			return true;
		}
		else
			return false;
	}
#endif

	//	WD:	has remembered trait so don't touch active trait 
	//		unless we are forcing removal of active trait
	if (!force && hasRemTrait)				
	{
		RemoveRememberedTrait(girl, name);
		return true;
	}

	//	WD:	save trait to remember list before removing
	if (addrememberlist)
		AddRememberedTrait(girl, name);

	//	WD: Remove trait
	sTrait* trait = g_Traits.GetTrait(name);
	for(int i=0; i<MAXNUM_TRAITS; i++)			// remove the traits
	{
		if(girl->m_Traits[i])
		{
			if(girl->m_Traits[i] == trait)
			{
				girl->m_NumTraits--;
				UnapplyTraits(girl, girl->m_Traits[i]);
				if(girl->m_TempTrait[i] > 0)
					girl->m_TempTrait[i] = 0;
				girl->m_Traits[i] = 0;
				return true;
			}
		}
	}
	return false;
}

bool cGirls::LoseVirginity(sGirl* girl, bool addrememberlist, bool force)
{	
	//		Usually called as just g_Girls.LoseVirginity(girl) with implied no-remember, force=true

	/*  Very similar to (and uses) RemoveTrait(). Added since trait "Virgin" created 04/14/2013.
	 *	This includes capability for items, magic or other processes 
	 *	to have a "remove but remember" effect, like a "Belt of False Defloration"
	 *	that provides a magical substitute vagina, preserving the original while worn.
	 *	Well, why not?		DustyDan
	 */

	string traitName = "Virgin";
	bool traitOpSuccess = false;

	girl->m_Virgin = false; 

	//	Let's avoid re-inventing the wheel

	traitOpSuccess = RemoveTrait(girl, traitName, addrememberlist, force);
	return traitOpSuccess;
}

bool cGirls::RegainVirginity(sGirl* girl, bool temp, bool removeitem, bool inrememberlist)
{	
	//	Usually called as just g_Girls.RegainVirginity(girl) with
	//		implied temp=false, removeitem=false, inrememberlist=falsee

	/*  Very similar to (and uses) AddTrait(). Added since trait "Virgin" created 04/14/2013.
	 *	This includes capability for items, magic or other processes 
	 *	to have a "remove but remember" effect, like a "Belt of False Defloration"
	 *	that provides a magical substitute vagina, preserving the original while worn.
	 *	Well, why not?		DustyDan
	 */


	string traitName = "Virgin";
	bool traitOpSuccess = false;

	girl->m_Virgin = true; 

	//	Let's avoid re-inventing the wheel

	traitOpSuccess = AddTrait(girl, traitName, temp, removeitem, inrememberlist);
	return traitOpSuccess;
}

void cGirls::AddRememberedTrait(sGirl* girl, string name)
{
	for(int i=0; i<MAXNUM_TRAITS*2; i++)	// add the traits
	{
		if(girl->m_RememTraits[i] == 0)
		{
			girl->m_NumRememTraits++;
			girl->m_RememTraits[i] = g_Traits.GetTrait(name);
			return;
		}
	}
}

bool cGirls::AddTrait(sGirl* girl, string name, bool temp, bool removeitem, bool inrememberlist)
{
	/*
	 *	WD: Added logic for remembered trait
	 *		
	 *		removeitem = true Will add to Remember
	 *		trait list if the trait is allready active
	 *		Used with items / efects may be removed 
	 *		later eg items - rings
	 *
	 *		inrememberlist = true only add trait if
	 *		exists in the remember list and remove  
	 *		from the list. Use mainly with unequiping 
	 *		items and Trait overiding in ApplyTraits()
	 *
	 *		Returns true if trait mad active or added
	 *		trait remember list.
	 *
	 *		This should fix items with duplicate 
	 *		traits and loss of original trait if 
	 *		overwritten by a trait from an item
	 *		that is later removed
	 *
	 */


	if(HasTrait(girl, name))
	{
		if (removeitem)								//	WD: Overwriting existing trait with removable item / effect
			AddRememberedTrait(girl, name);			//	WD:	Save trait for when item is removed

		return true;
	}

	if(inrememberlist)								// WD: Add trait only if it is in the Remember List
	{
		if(HasRememberedTrait(girl, name))
		{
			RemoveRememberedTrait(girl, name);		
		}

		else
			return false;							//	WD:	No trait to add
	}
	
	for(int i=0; i<MAXNUM_TRAITS; i++)				// add the trait
	{
		if(girl->m_Traits[i] == 0)
		{
			if(temp)
				girl->m_TempTrait[i] = 20;
			girl->m_NumTraits++;
			girl->m_Traits[i] = g_Traits.GetTrait(name);
			ApplyTraits(girl, girl->m_Traits[i], removeitem);
			return true;
		}
	}
	return false;
}
#if 0
	/*
	 * WD: Sanity checks not allways working as apply traits is called
	 *	direclty by the load coad bypassing these checks
	 *	also some of the checks in applytraits() and unapplytraits() are not here
	 */
	// Sanity checks
	if (HasTrait(girl, "Manly") && HasTrait(girl, "Elegant"))
		RemoveTrait(girl, "Elegant");
	if (HasTrait(girl, "MILF") && HasTrait(girl, "Lolita"))
		RemoveTrait(girl, "Lolita");
	if (HasTrait(girl, "Quick Learner") && HasTrait(girl, "Slow Learner"))
		RemoveTrait(girl, "Slow Learner");
	if (HasTrait(girl, "Small Boobs") && (HasTrait(girl, "Big Boobs") || HasTrait(girl, "Abnormally Large Boobs")))
		RemoveTrait(girl, "Small Boobs");
	if (HasTrait(girl, "Big Boobs") && HasTrait(girl, "Abnormally Large Boobs"))
		RemoveTrait(girl, "Big Boobs");
	if (HasTrait(girl, "Iron Will") && HasTrait(girl, "Broken Will"))
		RemoveTrait(girl, "Iron Will");
	if (HasTrait(girl, "Fast Orgasms") && HasTrait(girl, "Slow Orgasms"))
		RemoveTrait(girl, "Slow Orgasms");
	if (HasTrait(girl, "Aggressive") && HasTrait(girl, "Meek"))
		RemoveTrait(girl, "Meek");
	if (HasTrait(girl, "Fearless") && (HasTrait(girl, "Dependant") || HasTrait(girl, "Nervous") || HasTrait(girl, "Meek")))
	{
		if (HasTrait(girl, "Dependant"))
			RemoveTrait(girl, "Dependant");
		if (HasTrait(girl, "Nervous"))
			RemoveTrait(girl, "Nervous");
		if (HasTrait(girl, "Meek"))
			RemoveTrait(girl, "Meek");
	}
	if (HasTrait(girl, "Optimist") && HasTrait(girl, "Pessimist"))
		RemoveTrait(girl, "Pessimist");
	if (HasTrait(girl, "Tough") && HasTrait(girl, "Fragile"))
		RemoveTrait(girl, "Fragile");
	if (HasTrait(girl, "Different Colored Eyes") && (HasTrait(girl, "One Eye") || HasTrait(girl, "Eye Patch") ))
	{
		if (HasTrait(girl, "One Eye"))
			RemoveTrait(girl, "One Eye");
		if (HasTrait(girl, "Eye Patch"))
			RemoveTrait(girl, "Eye Patch");
	}
}
#endif

void cGirls::updateTempTraits(sGirl* girl)
{
/*
 *	 Update temp traits and remove expired traits
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	for(int i=0; i<MAXNUM_TRAITS; i++)
	{
		if(girl->m_Traits[i] && girl->m_TempTrait[i] > 0)
		{
			girl->m_TempTrait[i]--;
			if(girl->m_TempTrait[i] == 0)
				g_Girls.RemoveTrait(girl, girl->m_Traits[i]->m_Name);
		}
	}
}

void cGirls::updateHappyTraits(sGirl* girl)
{
/*
 *	 Update happiness for trait affects
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	if(girl->has_trait("Optimist"))
		girl->happiness(5);

	if(girl->has_trait("Pessimist"))
	{
		girl->happiness(-5);
		if(girl->happiness() <= 0)
		{
			string msg = girl->m_Realname + gettext(" has killed herself since she was unhappy and depressed.");
			girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
			g_MessageQue.AddToQue(msg, 1);
			//g_Girls.SetStat(girl, STAT_HEALTH, 0);
		}
	}
}

// ----- Sex

void cGirls::GirlFucks(sGirl* girl, int DayNight, sCustomer* customer, bool group, string& message, u_int& SexType)
{
	bool good = false;
	bool contraception = false;
	
	// Start the customers unhappiness/happiness bad sex decreases, good sex inceases
	if(HasTrait(girl,"Fast orgasms"))	// has priority
		customer->m_Stats[STAT_HAPPINESS] += 15;
	else if(HasTrait(girl,"Slow orgasms"))
		customer->m_Stats[STAT_HAPPINESS] -= 10;
	
	if(HasTrait(girl,"Psychic"))
		customer->m_Stats[STAT_HAPPINESS] += 10;

	if(HasTrait(girl,"Fake orgasm expert"))  // CRAZY fixed was fake orgasms should be what it is now
		customer->m_Stats[STAT_HAPPINESS] += 15;

	if(HasTrait(girl,"Abnormally Large Boobs"))		// WD: added
		customer->m_Stats[STAT_HAPPINESS] += 15;
	else if(HasTrait(girl,"Big Boobs"))				// WD: Fixed Spelling
		customer->m_Stats[STAT_HAPPINESS] += 10;


	girl->m_NumCusts += (int)customer->m_Amount;
	
	if(group)
	{
		// the customer will be an average in all skills for the customers involved in the sex act
		SexType = SKILL_GROUP;
	}
	else	// Any other sex acts
		SexType = customer->m_SexPref;

	// If the girls skill < 50 then it will be unsatisfying otherwise it will be satisfying
	if(GetSkill(girl, SexType) < 50)
		customer->m_Stats[STAT_HAPPINESS] -= (100-GetSkill(girl, SexType))/5;
	else
		customer->m_Stats[STAT_HAPPINESS] += GetSkill(girl, SexType)/5;

	// If the girl is famous then he will be slightly happier
	customer->m_Stats[STAT_HAPPINESS] += GetStat(girl, STAT_FAME)/5;

	// her service ability will also make him happier (I.e. does she help clean him well)
	customer->m_Stats[STAT_HAPPINESS] += GetSkill(girl, SKILL_SERVICE)/10;

	// her magic ability can make him think he enjoyed it more if she has mana
	// WD: only if Customer is not Max Happy else Whores never have any mana
	if(customer->m_Stats[STAT_HAPPINESS] < 100 && GetStat(girl, STAT_MANA) > 20 && GetSkill(girl, SKILL_MAGIC) > 9)	// Added check for magic skill to prevent waste of Mana. -PP
	{
		customer->m_Stats[STAT_HAPPINESS] += GetSkill(girl, SKILL_MAGIC)/10;
		UpdateStat(girl, STAT_MANA, -20);
	}

	message += girl->m_Realname;
	switch(SexType)
	{
	case SKILL_ANAL:
		if(GetSkill(girl, SexType) < 20)
			message += gettext(" found it difficult to get it in but painfully allowed the customer to fuck her in her tight ass.");
		else if(GetSkill(girl, SexType) < 40)
			message += gettext(" had to relax somewhat but had the customer fucking her in her ass.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext(" found it easier going with the customer fucking her in her ass.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext(" had the customer's cock go in easy. She found his cock in her ass a very pleasurable experience.");
		else
			message += gettext(" came hard as the customer fucked her ass.");
		break;

	case SKILL_BDSM:
		if(GetSkill(girl, SexType) < 40)
			message += gettext(" was frightened by being tied up and having pain inflicted on her.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext(" was a little turned on by being tied up and having the customer hurting her.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext(" was highly aroused by the pain and bondage, even more so when fucking at the same time.");
		else
			message += GetRandomBDSMString();
			//			" her screams of pain mixed with pleasure could be heard throughout the building as she came over and over during the bondage session.";
		break;

	case SKILL_NORMALSEX:
		if(GetSkill(girl, SexType) < 20)
			message += gettext(" didn't do much as she allowed the customer to fuck her pussy.");
		else if(GetSkill(girl, SexType) < 40)
			message += gettext(" fucked the customer back while their cock was embedded in her cunt.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext(" liked the feeling of having a cock buried in her cunt and fucked back as much as she got.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext(" fucked like a wild animal, cumming several times and ending with her and the customer covered in sweat.");
		else
			message += GetRandomSexString();
			//" fucked hard and came many times, so much so that the bed was soaked in their sweat and juices.";
		break;

	case SKILL_ORALSEX:
		if(GetSkill(girl, SexType) < 20)
			message += " awkwardly licked the customer's cock, and recoiled when he came.";
		else if(GetSkill(girl, SexType) < 60)
			message += " licked and sucked the customer's cock.";
		else if(GetSkill(girl, SexType) < 80)
			message += " loved sucking the customer's cock, and let him cum all over her.";
		else
			message += " wouldn't stop licking and sucking the customer's cock until she had swallowed his entire load.";
		break;

	case SKILL_TITTYSEX:
		if(GetSkill(girl, SexType) < 20)
			message += " awkwardly let the customer's cock fuck her tits, and recoiled when he came.";
		else if(GetSkill(girl, SexType) < 60)
			message += " used her breasts on the customer's cock.";
		else if(GetSkill(girl, SexType) < 80)
			message += " loved using her breasts on the customer's cock, and let him cum all over her.";
		else
			message += " wouldn't stop using her breasts to massage the customer's cock until she had made him spill his entire load.";
		break;

	case SKILL_BEASTIALITY:
		if(g_Brothels.GetNumBeasts() == 0)
		{
			message += gettext(" found that there were no beasts available, so some fake ones were used. This disapointed the customer somewhat.");
			customer->m_Stats[STAT_HAPPINESS] -= 10;
		}
		else
		{
			if(GetSkill(girl, SexType) < 50)
			{  // the less skilled she is, the more chance of hurting a beast accidentally
				int harmchance = -(GetSkill(girl, SexType) - 50);  // 50% chance at 0 skill, 1% chance at 49 skill
				if(g_Dice.percent(harmchance))
				{
					message += gettext(" accidentally harmed some beasts during the act and she");
					g_Brothels.add_to_beasts(-((g_Dice%3)+1));
				}
			}
			if(GetSkill(girl, SexType) < 20)
				message += gettext(" was disgusted by the idea but still allowed the customer to watch as she was fucked by some animals.");
			else if(GetSkill(girl, SexType) < 40)
				message += gettext(" was a only little put off by the idea but still allowed the customer to watch and help as she was fucked by animals.");
			else if(GetSkill(girl, SexType) < 60)
				message += gettext(" took a large animal's cock deep inside her and enjoyed being fucked by it, her cries of pleasure being muffled by the customer's cock in her mouth.");
			else if(GetSkill(girl, SexType) < 80)
				message += gettext(" fucked some exotic beasts covered with massive cocks and tentacles, she came over and over alongside with the customer.");
			else
				message += GetRandomBeastString();
				//" came many times as she enjoyed all the pleasures of sex with animals and monsters, covered in cum she lay with the customer exhausted a long time.";
		}
		break;

	case SKILL_GROUP:
		if(GetSkill(girl, SexType) < 20)
			message += gettext(" struggled to service everyone in the group that came to fuck her.");
		else if(GetSkill(girl, SexType) < 40)
			message += gettext(" managed to keep the group of customers fucking her satisfied.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext(" serviced all of the group of customers that fucked her.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext(" fucked and came many times with everyone in the group of customers.");
		else
			message += GetRandomGroupString();
			//"'s orgasms could be heard through the building, along with all the customers in the group.";
		break;

	case SKILL_LESBIAN:
		if(GetSkill(girl, SexType) < 20)
			message += gettext(" licked her female customer's cunt until she came. She didn't want any herself.");
		else if(GetSkill(girl, SexType) < 40)
			message += gettext(" was aroused as she made her female customer cum.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext(" fucked and was fucked by her female customer.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext(" and her female customer's cumming could be heard thoughout the building.");
		else
			message += GetRandomLesString();
			//" came many times with her female customer, soaking the room in their juices.";
		break;
	}

	// WD:	customer HAPPINESS changes complete now cap the stat to 100
	customer->m_Stats[STAT_HAPPINESS] = min(100, (int)customer->m_Stats[STAT_HAPPINESS]);

	if(SexType == SKILL_GROUP)
		message += gettext("\nThe customers ");
	else
		message += gettext("\nThe customer ");
	if(customer->m_Stats[STAT_HAPPINESS] > 80)
		message += gettext("swore they would come back.");
	else if(customer->m_Stats[STAT_HAPPINESS] > 50)
		message += gettext("enjoyed the experience.");
	else if(customer->m_Stats[STAT_HAPPINESS] > 30)
		message += gettext("didn't enjoy it.");
	else
		message += gettext("thought it was crap.");

#if 1
	// WD: update Fame based on Customer HAPPINESS
		UpdateStat(girl, STAT_FAME, (customer->m_Stats[STAT_HAPPINESS]-1) / 33);
#else
	// If he is happy then increase her fame
	if(customer->m_Stats[STAT_HAPPINESS] > 50)
		UpdateStat(girl, STAT_FAME, ((((int)customer->m_Stats[STAT_HAPPINESS])/10)+1));
#endif

	// The girls STAT_CONSTITUTION and STAT_AGILITY modify how tired she gets
	AddTiredness(girl);

	// WD:  Allready being done in AddTiredness() Fn

	//if(g_Girls.GetStat(girl, STAT_TIREDNESS) == 100)
	//{
	//	UpdateStat(girl, STAT_HAPPINESS, -5);
	//	UpdateStat(girl, STAT_HEALTH, -5);
	//}

	// if the girl likes sex and the sex type then increase her happiness otherwise decrease it
	if(GetStat(girl, STAT_LIBIDO) > 5)
	{
		if(GetSkill(girl, SexType) < 20)
			message += gettext("\nThough she had a tough time with it, she was horny and still managed to gain some little enjoyment.");
		else if(GetSkill(girl, SexType) < 40)
			message += gettext("\nShe considered it a learning experience and enjoyed it a bit.");
		else if(GetSkill(girl, SexType) < 60)
			message += gettext("\nShe enjoyed it a lot and wanted more.");
		else if(GetSkill(girl, SexType) < 80)
			message += gettext("\nIt was nothing new for her, but she really does appreciate such work.");
		else
			message += gettext("\nIt seems that she lives for this sort of thing.");
		UpdateStat(girl, STAT_HAPPINESS, GetStat(girl, STAT_LIBIDO)/5);
	}
	else
	{
		message += gettext("\nShe wasn't really in the mood.");
 		UpdateStat(girl, STAT_HAPPINESS, -1);
	}

	// special cases for certain sex types
	switch(SexType)
	{
	case SKILL_ANAL:
		if(GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_HEALTH, -3);
		}

		UpdateStat(girl, STAT_SPIRIT, -1);

		break;

	case SKILL_BDSM:
		if(GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		contraception = girl->calc_pregnancy(customer, false, 0.75);
		UpdateStat(girl, STAT_SPIRIT, -1);
		break;

	case SKILL_NORMALSEX:
		if(GetSkill(girl, SexType) < 10)
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
/*
 *		if they're both happy afterward, it's good sex
 *		which modifies the chance of pregnancy
 */
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		contraception = girl->calc_pregnancy(customer, good);
		break;

	case SKILL_ORALSEX:
		if(GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
		//	UpdateStat(girl, STAT_HEALTH, -3);				// Removed... oral doesn't hurt unless you get herpes or something. --PP
		}

/*
 *		if they're both happy afterward, it's good sex
 *		which modifies the chance of pregnancy
 */
		break;

	case SKILL_TITTYSEX:
		if(GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
		//	UpdateStat(girl, STAT_HEALTH, -3);				// Removed... oral doesn't hurt unless you get herpes or something. --PP
		}

/*
 *		if they're both happy afterward, it's good sex
 *		which modifies the chance of pregnancy
 */
		break;

	case SKILL_BEASTIALITY:
		if(GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}

		UpdateStat(girl, STAT_SPIRIT, -1);	// is pretty degrading
/*
 *		if they're both happy afterward, it's good sex
 *		which modifies the chance of pregnancy
 */
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		// mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
		if(g_Brothels.GetNumBeasts() > 0)
			contraception = girl->calc_insemination(customer, good);
		break;

	case SKILL_GROUP:
		if(GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
/*
 *		if they're both happy afterward, it's good sex
 *		which modifies the chance of pregnancy
 */
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
/*
 *		adding a 50% bonus to the chance of pregnancy
 *		since there's more than one partner involved
 */
		contraception = girl->calc_pregnancy(customer, good, 1.5);
	}

	// lose virginity unless it was anal sex -- or lesbian, or Oral also customer is happy no matter what. -PP
	if(girl->m_Virgin)
	{
		message += gettext(" The customer was overjoyed that she was a virgin.");
//		girl->m_Virgin = false;
		customer->m_Stats[STAT_HAPPINESS] = 100;
		if(SexType != SKILL_ANAL && SexType != SKILL_LESBIAN && SexType != SKILL_ORALSEX && SexType != SKILL_TITTYSEX)
			girl->m_Virgin = false;
	}

	// Now calculate other skill increases
	if(HasTrait(girl,"Quick Learner"))
	{
		if(SexType == SKILL_GROUP)
		{
			for(u_int i=0; i<NUM_SKILLS; i++)
				UpdateSkill(girl,i,g_Dice%3);
		}
		else
			UpdateSkill(girl,SexType,g_Dice%5);
		UpdateSkill(girl,SKILL_SERVICE,g_Dice%5);
		UpdateStat(girl,STAT_EXP,((g_Dice%7) * 3));  // MYR: More xp, so levelling happens more
	}
	else if(HasTrait(girl,"Slow Learner"))
	{
		if(SexType == SKILL_GROUP)
		{
			for(u_int i=0; i<NUM_SKILLS; i++)
				UpdateSkill(girl,i,g_Dice%2);
		}
		else
			UpdateSkill(girl,SexType,g_Dice%2);
		UpdateSkill(girl,SKILL_SERVICE,g_Dice%2);
		UpdateStat(girl,STAT_EXP,((g_Dice%3) * 3)); 
	}
	else
	{
		if(SexType == SKILL_GROUP)
		{
			for(u_int i=0; i<NUM_SKILLS; i++)
				UpdateSkill(girl,i,g_Dice%2);
		}
		else
			UpdateSkill(girl,SexType,g_Dice%3);
		UpdateSkill(girl,SKILL_SERVICE,g_Dice%3);
		UpdateStat(girl,STAT_EXP,((g_Dice%5) * 3));
	}

	if(GetStat(girl, STAT_HAPPINESS) > 50 && !HasTrait(girl,"Nymphomaniac"))
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +2, true);
		//UpdateStat(girl,STAT_LIBIDO,-3);	// libido is satisfied after sex  // Libido decremented in JobManager
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
	}

	if(GetStat(girl, STAT_HAPPINESS) <= 5)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, -1, true);
	}
	//UpdateStat(girl,STAT_LIBIDO,-1);  // Libido decremented in JobManager

	if(HasTrait(girl, "AIDS"))
		customer->m_Stats[STAT_HAPPINESS] -= 10;
	else if(GetStat(girl, STAT_HEALTH) <= 10)
	{
		if((g_Dice%100)+1 == 1 && !contraception)
		{
			string mess = girl->m_Realname;
			mess += gettext(" has caught the disease AIDS! She will likely die, but a rare cure can sometimes be found in the shop.");
			girl->m_Events.AddMessage(mess, IMGTYPE_PROFILE, EVENT_DANGER);
			AddTrait(girl, "AIDS");
		}
	}

	if(HasTrait(girl, "Chlamydia"))
		customer->m_Stats[STAT_HAPPINESS] -= 20;
	else if(GetStat(girl, STAT_HEALTH) <= 10)
	{
		if((g_Dice%100)+1 <= 1 && !contraception)
		{
			string mess = "";
			mess += girl->m_Realname;
			mess += gettext(" has caught the disease Chlamydia! A cure can sometimes be found in the shop.");
			girl->m_Events.AddMessage(mess, IMGTYPE_PROFILE, EVENT_DANGER);
			AddTrait(girl, "Chlamydia");
		}
	}

	if(HasTrait(girl, "Syphilis"))
		customer->m_Stats[STAT_HAPPINESS] -= 10;
	else if(GetStat(girl, STAT_HEALTH) <= 10)
	{
		if((g_Dice%100)+1 <= 1 && !contraception)
		{
			string mess = "";
			mess += girl->m_Realname;
			mess += gettext(" has caught the disease Syphilis! This can be deadly, but a cure can sometimes be found in the shop.");
			girl->m_Events.AddMessage(mess, IMGTYPE_PROFILE, EVENT_DANGER);
			AddTrait(girl, "Syphilis");
		}
	}
}

string cGirls::GetRandomSexString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	string OutStr;


	// MYR: Can't resist a little cheeky chaos
	random = g_Dice%500;
	if (random == 345)
	{
		OutStr += " (phrase 1). (phrase 2) (phrase 3).";
		return OutStr;
	}

	OutStr += " ";  // Consistency

	// Roll #1
# pragma region sex1
	roll1 = g_Dice % 8 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("straddled");
		else if (random <= 4)
			OutStr += gettext("cow-girled");
		else if (random <= 6)
			OutStr += gettext("wrapped her legs around");
		else if (random <= 8)
			OutStr += gettext("contorted her legs behind her head for");
		else
			OutStr += gettext("scissored");

		OutStr += gettext(" the client, because it ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("turned him on");
		else if (random <= 4)
			OutStr += gettext("made him crazy");
		else if (random <= 6)
			OutStr += gettext("gave him a massive boner");
		else if (random <= 8)
			OutStr += gettext("was more fun than talking");
		else
			OutStr += gettext("made him turn red");

		break;
	case 2: 
		OutStr += gettext("was told to grab ");
				
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("her ankles");
		else if (random <= 4)
			OutStr += gettext("the chair");
		else if (random <= 6)
			OutStr += gettext("her knees");
		else if (random <= 8)
			OutStr += gettext("the table");
		else
			OutStr += gettext("the railing");

		OutStr += gettext(" and ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("shook her hips");
		else if (random <= 4)
			OutStr += gettext("spread her legs");
		else if (random <= 6)
			OutStr += gettext("close her eyes");
		else if (random <= 8)
			OutStr += gettext("look away");
		else
			OutStr += gettext("bend waaaaayyy over");

		break;
	case 3: 
		OutStr += gettext("had fun with his ");
		
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("foot");
		else if (random <= 4)
			OutStr += gettext("stocking");
		else if (random <= 6)
			OutStr += gettext("hair");
		else if (random <= 8)
			OutStr += gettext("lace");
		else if (random <= 10)
			OutStr += gettext("butt");
		else
			OutStr += gettext("food");

		OutStr += gettext(" fetish and gave him an extended ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("foot");
		else if (random <= 4)
			OutStr += gettext("hand");
		else
			OutStr += gettext("oral");

		OutStr += gettext(" surprise"); 

		break;
	case 4: 
		OutStr += gettext("dressed as ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("a school girl");
		else if (random <= 4)
			OutStr += gettext("a nurse");
		else if (random <= 6)
			OutStr += gettext("a nun");
		else if (random <= 8)
			OutStr += gettext("an adventurer");
		else
			OutStr += gettext("a dominatrix");
			
		OutStr += gettext(" to grease "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("the little man");
		else if (random <= 4)
			OutStr += gettext("his pole");
		else if (random <= 6)
			OutStr += gettext("his tool");
		else if (random <= 8)
			OutStr += gettext("his fingers");
		else
			OutStr += gettext("his toes");

		break;
	case 5: 
		OutStr += gettext("decided to skip "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("the bed");
		else if (random <= 4)
			OutStr += gettext("foreplay");
		else if (random <= 6)
			OutStr += gettext("niceties");
		else
			OutStr += gettext("greetings");

		OutStr += gettext(" and assumed position "); 
		
		random = g_Dice%9999 + 1;
		char buffer[10];
		_itoa(random, buffer, 10);
		OutStr += buffer;

		break;
	case 6: 
		OutStr += gettext("gazed in awe at ");
				 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("how well hung he was");
		else if (random <= 4)
			OutStr += gettext("the time");
		else if (random <= 6)
			OutStr += gettext("his muscles");
		else if (random <= 8)
			OutStr += gettext("his handsome face");					 
		else
			OutStr += gettext("his collection of sexual magic items");

		OutStr += gettext(" and ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("felt inspired");
		else if (random <= 4)
			OutStr += gettext("played hard to get");
		else if (random <= 6)
			OutStr += gettext("squealed like a little girl");
		else
			OutStr += gettext("prepared for action");

		break;
	case 7: OutStr += gettext("bent into ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("a delightful");
		else if (random <= 4)
			OutStr += gettext("an awkward");
		else if (random <= 6)
			OutStr += gettext("a difficult");
		else
			OutStr += gettext("a crazy");	

		OutStr += gettext(" position and "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("squealed");
		else if (random <= 4)
			OutStr += gettext("moaned");
		else 
			OutStr += gettext("grew hot");

		OutStr += gettext(" as he ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("touched");
		else if (random <= 4)
			OutStr += gettext("caressed");
		else
			OutStr += gettext("probed");

		OutStr += gettext(" her defenseless body");
		break;
	case 8: 
		OutStr += gettext("lay on the "); 
		
		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("floor");
		else if (random <= 4)
			OutStr += gettext("bed");
		else
			OutStr += gettext("couch");
	
		OutStr += gettext(" and ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("had him take off all her clothes");
		else if (random <= 4)
			OutStr += gettext("told him exactly what turned her on");
		else
			OutStr += gettext("encouraged him to take off her bra and panties with his teeth");

		break;
	}
# pragma endregion sex1

	// Roll #2
# pragma region sex2
	OutStr += ". ";

	roll2 = g_Dice % 11 + 1;
	switch (roll2)
	{
	case 1: 
		OutStr += gettext("She ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("rode him all the way to the next town");
		else if (random <= 4)
			OutStr += gettext("massaged his balls and sucked him dry");
		else if (random <=6 )
			OutStr += gettext("titty fucked and sucked the well dry");
		else
			OutStr += gettext("fucked him blind");

		OutStr += gettext(". He was a trooper though and rallied: She ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("was deeply penetrated");
		else if (random <= 4)
			OutStr += gettext("was paralyzed with stunning sensations");
		else if (random <=6 )
			OutStr += gettext("bucked like a bronko");
		else
			OutStr += gettext("shook with pleasure");

		OutStr += gettext(" and ");

		random = g_Dice%4 + 1;
		if (random <= 2)
			OutStr += gettext("came like a fire hose from");
		else
			OutStr += gettext("repeatedly shook in orgasm with");
	
		break;
	case 2: 
		OutStr += gettext("It took a lot of effort to stay ");
				
		random = g_Dice%10 + 1;
		if (random <= 3)
			OutStr += gettext("interested in");
		else if (random <= 7)
			OutStr += gettext("awake for");
		else 
			OutStr += gettext("conscious for");

		break;

	case 3: 
		OutStr += gettext("She was fucked ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("blind");
		else if (random <= 4)
			OutStr += gettext("silly twice over");
		else if (random <= 6)
			OutStr += gettext("all crazy like");
		else if (random <= 8)
			OutStr += gettext("for hours");
		else
			OutStr += gettext("for minutes");

		OutStr += gettext(" by"); break;

	case 4: 
		OutStr += gettext("She performed ");
		
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("uninspired ");
		else
			OutStr += gettext("inspired ");
				
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("missionary ");
		else if (random <= 4)
			OutStr += gettext("oral ");
		else if (random <= 6)
			OutStr += gettext("foot ");
		else
			OutStr += gettext("hand ");
				
		OutStr += gettext("sex for"); break;

	case 5: 
		//OutStr += ""; 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Semen");
		else if (random <= 4)
			OutStr += gettext("Praise");
		else if (random <= 6)
			OutStr += gettext("Flesh");
		else if (random <= 8)
			OutStr += gettext("Drool");
		else
			OutStr += gettext("Chocolate sauce");

		OutStr += gettext(" rained down on her from"); 
		break;

	case 6: 
		OutStr += gettext("She couldn't "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("stand");
		else if (random <= 4)
			OutStr += gettext("walk");
		else if (random <= 6)
			OutStr += gettext("swallow");
		else if (random <= 8)
			OutStr += gettext("feel her legs");
		else
			OutStr += gettext("move");

		OutStr += gettext(" after screwing"); 
		break;
	case 7: 
		OutStr += gettext("It took a great deal of effort to look ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("interested in");
		else if (random <= 4)
			OutStr += gettext("awake for");
		else if (random <= 6)
			OutStr += gettext("alive for");
		else if (random <= 8)
			OutStr += gettext("enthusiastic for");
		else
			OutStr += gettext("hurt for");
				 
		break;
	case 8: 
		OutStr += gettext("She played 'clean up the ");
				
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("tools");
		else if (random <= 4)
			OutStr += gettext("customer");
		else if (random <= 6)
			OutStr += gettext("sword");
		else 
			OutStr += gettext("sugar frosting");

		OutStr += gettext("' with"); 
		break;
	case 9: 
		OutStr += gettext("Hopefully her ");
			
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("cervix");
		else if (random <= 4)
			OutStr += gettext("pride");
		else if (random <= 6)
			OutStr += gettext("reputation");
		else if (random <= 8)
			OutStr += gettext("ego");
		else
			OutStr += gettext("stomach");

		OutStr += gettext(" wasn't bruised by"); 
		break;
	case 10: 
		OutStr += gettext("She called in "); 
		
		random = g_Dice%3 + 2;
		char buffer[10];
		_itoa(random, buffer, 10);
		OutStr += buffer;
		
		OutStr += gettext(" reinforcements to tame"); 
		break;
	case 11: 
		OutStr += gettext("She orgasmed "); 
		
		random = g_Dice%100 + 30;
		_itoa(random, buffer, 10);
		OutStr += buffer;

		OutStr += gettext(" times with"); break;
	}
# pragma endregion sex2

	// Roll #3
# pragma region sex3
	OutStr += " ";	// Consistency

	roll3 = g_Dice % 20 + 1;
	switch (roll3)
	{
	case 1:
		OutStr += gettext("the guy "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("wearing three amulets of the sex elemental.");
		else if (random <= 4)
			OutStr += gettext("wearing eight rings of the horndog.");
		else if (random <= 6)
			OutStr += gettext("wearing a band of invulnerability.");
		else if (random <= 8)
			OutStr += gettext("carrying a waffle iron.");
		else
			OutStr += gettext("carrying a body probe of irresistable sensations.");

		break;
	case 2: OutStr += gettext("Thor, God of Thunderfucking!!!!"); break;
	case 3: 
		OutStr += gettext("the frustrated "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("astronomer.");
		else if (random <= 4)
			OutStr += gettext("physicist.");
		else if (random <= 6)
			OutStr += gettext("chemist.");
		else if (random <= 8)
			OutStr += gettext("biologist.");
		else
			OutStr += gettext("engineer.");

		break;
	case 4: OutStr += gettext("the invisible something or other????"); break;
	case 5: OutStr += gettext("the butler. (He always did it.)"); break;
	case 6: 
		OutStr += gettext("the "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += "sentient apple tree.";
		else if (random <= 4)
			OutStr += gettext("sentient sex toy.");
		else if (random <= 6)
			OutStr += gettext("pan-dimensional toothbrush.");
		else if (random <= 8)
			OutStr += gettext("magic motorcycle.");
		else
			OutStr += gettext("regular bloke.");

		break;
	case 7: 
		OutStr += gettext("the unbelievably well behaved ");
				
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("Pink Petal forum member.");
		else if (random <= 4)
			OutStr += gettext("tentacle.");
		else if (random <= 6)
			OutStr += gettext("pirate.");
		else 
			OutStr += gettext("sentient bottle.");

		break;
	case 8: 
		random = g_Dice%20 + 1;
		if (random <= 2)
			OutStr += gettext("Cousin");
		else if (random <= 4)
			OutStr += gettext("Brother");
		else if (random <= 6)
			OutStr += gettext("Saint");
		else if (random <= 8)
			OutStr += gettext("Lieutenant");
		else if (random <= 10)
			OutStr += gettext("Master");
		else if (random <= 12)
			OutStr += gettext("Doctor");
		else if (random <= 14)
			OutStr += gettext("Mr.");
		else if (random <= 16)
			OutStr += gettext("Smith");
		else if (random <= 18)
			OutStr += gettext("DockMaster");
		else
			OutStr += gettext("Perfect");
			
		OutStr += gettext(" Parkins from down the street."); 
		break;
	case 9: OutStr += gettext("the master of the hidden dick technique. (Where is it? Nobody knows.)"); break;
	case 10: OutStr += gettext("cake. It isn't a lie!"); break;
	case 11: 
		OutStr += gettext("the really, really macho ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Titan.");
		else if (random <= 4)
			OutStr += gettext("Storm Giant.");
		else if (random <= 6)
			OutStr += gettext("small moon.");
		else if (random <= 8)
			OutStr += gettext("kobold.");
		else
			OutStr += gettext("madness.");
		
		break;
	case 12: 
		OutStr += gettext("the clockwork man!");
			
		OutStr += gettext(" (With no sensation in his clockwork ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tool");
		else if (random <= 4)
			OutStr += gettext("head");
		else if (random <= 6)
			OutStr += gettext("fingers");
		else if (random <= 8)
			OutStr += gettext("attachment");
		else
			OutStr += gettext("clock");
		
		OutStr += gettext(" and no sense to ");
			
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("stop");
		else if (random <= 4)
			OutStr += gettext("slow down");
		else if (random <= 6)
			OutStr += gettext("moderate");
		else if (random <= 8)
			OutStr += gettext("be gentle");
		else
		{
			OutStr += gettext("stop at ");

			random = g_Dice%50 + 30;
			_itoa(random, buffer, 10);
			OutStr += buffer;
		
			OutStr += gettext(" orgasms");
		}

		OutStr += gettext(".)"); 
		break;
	case 13: 
		// MYR: This one gives useful advice to the players.  A gift from us to them.
		OutStr += gettext("the Brothel Master developer. ");
		
		random = g_Dice%20 + 1;
		if (random <= 2)
			OutStr += gettext("(Quick learner is a great talent to have.)");
		else if (random <= 4)
			OutStr += gettext("(Don't ignore the practice skills option for your girls.)");
		else if (random <= 6)
			OutStr += gettext("(Train your gangs.)");
		else if (random <= 8)
			OutStr += gettext("(Every time you restart the game, the shop inventory is reset.)");
		else if (random <= 10)
			OutStr += gettext("(Invulnerable (insubstantial) characters should be exploring the catacombs.)");
		else if (random <= 12)
			OutStr += gettext("(High dodge gear is great for characters exploring the catacombs.)");
		else if (random <= 14)
			OutStr += gettext("(For a character with a high constitution, experiment with working on both shifts.)");
		else if (random <= 16)
			OutStr += gettext("(Matrons need high service skills.)");
		else if (random <= 18)
			OutStr += gettext("(Girls see a max of 3 people for high reputations, 3 for high appearance and 3 for high skills.)");
		else
			OutStr += gettext("(Don't overlook the bribery option in the town hall and the bank.)");

		break;
	case 14: OutStr += gettext("grandmaster piledriver the 17th."); break;
	case 15:
		OutStr += gettext("the evolved sexual entity from ");
				 
		random = g_Dice%8 + 1;
		if (random <= 2)
		{
			random = g_Dice%200000 + 100000;
			_itoa(random, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" years in the future.");
		}
		else if (random <= 4)
			OutStr += gettext("the closet.");
		else if (random <= 6)
			OutStr += gettext("the suburbs.");
		else 
			OutStr += gettext("somewhere in deep space.");
				 
		break;
	case 16:
		OutStr += gettext("the ");
				 
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("mayor");
		else if (random <= 4)
			OutStr += gettext("bishop");
		else if (random <= 6)
			OutStr += gettext("town treasurer");
		else 
			OutStr += gettext("school principle");

		 OutStr += gettext(", on one of his regular health checkups."); 
		 break;
	case 17: OutStr += gettext("the letter H."); break;
	case 18: OutStr += gettext("a completely regular and unspectacular guy."); break;
	case 19: 
		OutStr += gettext("the ");
			
		random = g_Dice%20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" dick, ");

		random = g_Dice%20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext("-armed ");

		OutStr += gettext("(Each wearing ");
		
		random = g_Dice%2 + 4;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("rings of the Schwarzenegger");
		else if (random <= 4)
			OutStr += gettext("rings of the horndog");
		else if (random <= 6)
			OutStr += gettext("rings of beauty");
		else 
			OutStr += gettext("rings of potent sexual stamina");

		OutStr += gettext(") ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("neighbor");
		else if (random <= 4)
			OutStr += gettext("yugoloth");
		else if (random <= 6)
			OutStr += gettext("abberation");
		else 
			OutStr += gettext("ancient one");

		OutStr += gettext(".");
		break;
	case 20: 
		OutStr += gettext("the number 69."); break;
	}
# pragma endregion sex3

	OutStr += gettext("\n");
	return OutStr;
}

string cGirls::GetRandomGroupString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	string OutStr;
	char buffer[10];

	// Part 1
# pragma region group1
	OutStr += gettext(" ");

	roll1 = g_Dice % 4 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		OutStr += gettext("counted the number of customers: ");

		random = g_Dice%20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += ". ";

		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("This was going to be rough");
		else if (random <= 4)
			OutStr += gettext("Sweet");
		else if (random <= 6)
			OutStr += gettext("It could be worse");
		else if (random <= 8)
			OutStr += gettext("A smile formed on her lips. This was going to be fun");
		else if (random <= 10)
			OutStr += gettext("Boring");
		else if (random <= 12)
			OutStr += gettext("Not enough");
		else
			OutStr += gettext("'Could you get more?' she wondered");

		break;
	case 2: OutStr += gettext("was lost in ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("a sea");
		else if (random <= 4)
			OutStr += gettext("a storm");
		else if (random <= 6)
			OutStr += gettext("an ocean");
		else
			OutStr += gettext("a jungle");		

		OutStr += gettext(" of hot bodies");
		break;
	case 3: 
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("sat");
		else if (random <= 4)
			OutStr += gettext("lay");
		else if (random <= 6)
			OutStr += gettext("stood");
		else
			OutStr += gettext("crouched");
		
		OutStr += " ";

		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("blindfolded and ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("handcuffed");
		else if (random <= 4)
			OutStr += gettext("tied up");
		else if (random <= 6)
			OutStr += gettext("wrists bound in rope");
		else
			OutStr += gettext("wrists in chains hanging from the ceiling");

		OutStr += gettext(" in the middle of a ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("circle");
		else if (random <= 4)
			OutStr += gettext("smouldering pile");
		else if (random <= 6)
			OutStr += gettext("phalanx");
		else
			OutStr += gettext("wall");

		OutStr += gettext(" of flesh");
		break;
	case 4: 	
		OutStr += gettext("was ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("amazed by");
		else if (random <= 4)
			OutStr += gettext("disappointed by");
		else if (random <= 6)
			OutStr += gettext("overjoyed with");
		else
			OutStr += gettext("ecstatically happy with"); 
		
		OutStr += gettext(" the ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("brigade");
		else if (random <= 4)
			OutStr += gettext("army group");
		else if (random <= 6)
			OutStr += gettext("squad");
		else
			OutStr += gettext("batallion"); 
		
		OutStr += gettext(" of "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("man meat");
		else if (random <= 4)
			OutStr += gettext("cock");
		else if (random <= 6)
			OutStr += gettext("muscle");
		else
			OutStr += gettext("horny, brainless thugs");
		
		OutStr += gettext(" around her");
		break;
	}
# pragma endregion group1

	// Part 2
# pragma region group2
	OutStr += gettext(". ");

	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1: 
		OutStr += gettext("She was thoroughly ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("banged");
		else if (random <= 4)
			OutStr += gettext("fucked");
		else if (random <= 6)
			OutStr += gettext("disappointed");
		else
			OutStr += gettext("penetrated");
		
		OutStr += gettext(" by");
		break;
	case 2: 
		OutStr += gettext("They handled her like ");

		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("an expensive");
		else
			OutStr += gettext("a cheap");

		OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("hooker");
		else if (random <= 4)
			OutStr += gettext("street worker");
		else if (random <= 6)
			OutStr += gettext("violin");
		else
			OutStr += gettext("wine");

		OutStr += gettext(" for");
		break;
	case 3: 
		OutStr += gettext("Her ");
		
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("holes were");
		else
			OutStr += gettext("love canal was");

		OutStr += gettext(" plugged by"); 
		break;
	case 4: 
		OutStr += gettext("She ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("was bukkaked by");
		else if (random <= 4)
			OutStr += gettext("was given pearl necklaces by");
		else 
			OutStr += gettext("received a thorough face/hair job from");
		
		break;
	case 5: 
		OutStr += gettext("They demanded simultaneous hand, foot and mouth ");
				
		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("jobs");
		else if (random <= 4)
			OutStr += gettext("action");
		else 
			OutStr += gettext("combat");

		OutStr += gettext(" for");
		break;
	case 6: 
		OutStr += gettext("There was a positive side: 'So much ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("meat");
		else if (random <= 4)
			OutStr += gettext("cock");
		else if (random <= 6)
			OutStr += gettext("testosterone");
		else
			OutStr += gettext("to do");
			
		OutStr += gettext(", so little time' she said to");
		break;
	case 7: 
		OutStr += gettext("They made sure she had a nutritious meal of ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("man meat");
		else if (random <= 4)
			OutStr += gettext("cock");
		else if (random <= 6)
			OutStr += gettext("penis");
		else
			OutStr += gettext("meat rods");
			
		OutStr += gettext(" and drinks of delicious ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("semen");
		else if (random <= 4)
			OutStr += gettext("man mucus");
		else if (random <= 6)
			OutStr += gettext("man-love");
		else
			OutStr += gettext("man-cream");
			
		OutStr += gettext(" from"); 
		break;
	case 8: 
		OutStr += gettext("She was ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("fucked");
		else if (random <= 4)
			OutStr += gettext("banged");
		else if (random <= 6)
			OutStr += gettext("humped");
		else
			OutStr += gettext("sucked");		
		
		OutStr += gettext(" silly ");
		
		if (random <= 2)
			OutStr += gettext("twice over");
		else if (random <= 4)
			OutStr += gettext("three times over");
		else 
			OutStr += gettext("so many times");

		OutStr += gettext(" by"); 
		break;
	}
# pragma endregion group2

	// Part 3
# pragma region group3
	OutStr += gettext(" ");

	roll3 = g_Dice % 11 + 1;
	switch (roll3)
	{
	case 1: 
		OutStr += gettext("every member of the Crossgate ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("roads crew.");
		else if (random <= 4)
			OutStr += gettext("administrative staff.");
		else if (random <= 6)
			OutStr += gettext("interleague volleyball team.");
		else
			OutStr += gettext("short persons defense league.");			
			
		; break;
	case 2:
		OutStr += gettext("all the ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("lieutenants");
		else if (random <= 4)
			OutStr += gettext("sergeants");
		else if (random <= 6)
			OutStr += gettext("captains");
		else
			OutStr += gettext("junior officers");	
		
		OutStr += gettext(" in the Mundigan ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("army.");
		else if (random <= 4)
			OutStr += gettext("navy.");
		else if (random <= 6)
			OutStr += gettext("elite forces.");
		else
			OutStr += gettext("foreign legion.");

		break;
	case 3: 
		OutStr += gettext("the visiting ");
				
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("half-giants. (Ouch!)");
		else if (random <= 4)
			OutStr += gettext("storm giants.");
		else if (random <= 6)
			OutStr += gettext("titans.");
		else
			OutStr += gettext("ogres.");

				break;
	case 4: 
		OutStr += gettext("the ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("Hentai Research");
		else if (random <= 4)
			OutStr += gettext("Women's Rights");
		else if (random <= 6)
			OutStr += gettext("Prostitution Studies");
		else
			OutStr += gettext("Celibacy");

		 OutStr += gettext(" club of the University of Cunning Linguists."); 
		 break;
	case 5: 
		OutStr += gettext("the squad of ");
		
		random = g_Dice%10 + 1;
		if (random <= 5)
		  OutStr += gettext("hard-to-find ninjas."); 
		else
		  OutStr += gettext("racous pirates.");
		
		break;
	case 6: OutStr += gettext("a group of people from some place called the 'Pink Petal forums'."); break;
	case 7: 
		OutStr += gettext("the seemingly endless ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("army");
		else if (random <= 4)
			OutStr += gettext("horde");
		else if (random <= 6)
			OutStr += gettext("number");
		else
			OutStr += gettext("group");		
		
		OutStr += gettext(" of really");
			
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext(", really ");
		else
			OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("fired up");
		else if (random <= 4)
			OutStr += gettext("horny");
		else if (random <= 6)
			OutStr += gettext("randy");
		else
			OutStr += gettext("backed up");

		OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("gnomes.");
		else if (random <= 4)
			OutStr += gettext("halflings.");
		else if (random <= 6)
			OutStr += gettext("kobolds.");
		else
			OutStr += gettext("office workers.");

			break;
	case 8: 
		OutStr += gettext("CSI ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("New York");
		else if (random <= 4)
			OutStr += gettext("Miami");
		else if (random <= 6)
			OutStr += gettext("Mundigan");
		else
			OutStr += gettext("Tokyo"); 
			
		OutStr += gettext(" branch."); 
		break;
	case 9: 
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("frosh");
		else if (random <= 4)
			OutStr += gettext("seniors");
		else if (random <= 6)
			OutStr += gettext("young adults");
		else
			OutStr += gettext("women");

		OutStr += gettext(" on a "); 
		
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("serious ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("bender");
		else if (random <= 4)
			OutStr += gettext("road trip");
		else if (random <= 6)
			OutStr += gettext("medical study");
		else
			OutStr += gettext("lark");

		OutStr += gettext(".");
		break;
	case 10: 
		OutStr += gettext("all the ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("second stringers");
		else if (random <= 4)
			OutStr += gettext("has-beens");
		else if (random <= 6)
			OutStr += gettext("never-weres");
		else
			OutStr += gettext("victims"); 
		
		OutStr += gettext(" from the ");
		
		random = g_Dice%20 + 1991;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		
		OutStr += gettext(" H anime season."); 
		break;
	case 11:
		OutStr += gettext("Grandpa Parkins and his extended family of "); 
		
		random = g_Dice%200 + 100;
		_itoa(random, buffer, 10);
		OutStr += buffer;

		OutStr += gettext(".");

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" (And ");

			random = g_Dice%100 + 50;
			_itoa(random, buffer, 10);
			OutStr += buffer;

			OutStr += gettext(" guests.)");
		}

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" (And ");

			random = g_Dice%100 + 50;
			_itoa(random, buffer, 10);
			OutStr += buffer;

			OutStr += gettext(" more from the extended extended family.)");
		}
		break;
	}
# pragma endregion group3

	OutStr += gettext("\n");
	return OutStr;
}

string cGirls::GetRandomBDSMString()
{
	int roll2 = 0, roll3 = 0, random = 0;
	string OutStr;
	char buffer[10];

	OutStr += gettext(" was ");

	// Part 1:
# pragma region bdsm1
	// MYR: Was on a roll so I completely redid the first part

	random = g_Dice%12 + 1;
	if (random <= 2)
		OutStr += gettext("dressed as a dominatrix");
	else if (random <= 4)
		OutStr += gettext("stripped naked");
	else if (random <= 6)
		OutStr += gettext("dressed as a (strictly legal age) school girl");
	else if (random <= 8)
		OutStr += gettext("dressed as a nurse");
	else if (random <= 10)
		OutStr += gettext("put in heels");
	else
		OutStr += gettext("covered in oil");

	random = g_Dice%4 + 1;
	if (random == 3)
	{
		random = g_Dice%4 + 1;
		if (random <= 2)
			OutStr += gettext(", rendered helpless by drugs");
		else 
			OutStr += gettext(", restrained by magic");
	}

	random = g_Dice%4 + 1;
	if (random == 2)
		OutStr += gettext(", blindfolded");

	random = g_Dice%4 + 1;
	if (random == 2)
		OutStr += gettext(", gagged");

	OutStr += gettext(", and ");

	random = g_Dice%12 + 1;
	if (random <= 2)
		OutStr += gettext("chained");
	else if (random <= 4)
		OutStr += gettext("lashed");
	else if (random <= 6)
		OutStr += gettext("tied");
	else if (random <= 8)
		OutStr += gettext("bound");
	else if (random <= 10)
		OutStr += gettext("cuffed");
	else
		OutStr += gettext("leashed");

	random = g_Dice%4 + 1;
	if (random == 3)
		OutStr += gettext(", arms behind her back");

	random = g_Dice%4 + 1;
	if (random == 2)
		OutStr += gettext(", fettered");

	random = g_Dice%4 + 1;
	if (random == 2)
		OutStr += gettext(", spread eagle");

	random = g_Dice%4 + 1;
	if (random == 2)
		OutStr += gettext(", upside down");

	OutStr += gettext(" ");

	random = g_Dice%16 + 1;
	if (random <= 2)
		OutStr += gettext("to a bed");
	else if (random <= 4)
		OutStr += gettext("to a post");
	else if (random <= 6)
		OutStr += gettext("to a wall");
	else if (random <= 8)
		OutStr += gettext("to vertical stocks");
	else if (random <= 10)
		OutStr += gettext("to a table");	
	else if (random <= 12)
		OutStr += gettext("on a wooden horse");
	else if (random <= 14)
		OutStr += gettext("in stocks");
	else
		OutStr += gettext("at the dog house");
# pragma endregion bdsm1

	// Part 2
# pragma region bdsm2
	OutStr += gettext(". ");

	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1: 
		OutStr += gettext("She was fucked "); 

		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("with a rake");
		else if (random <= 4)
			OutStr += gettext("with a giant dildo");
		else if (random <= 6)
			OutStr += gettext("and flogged");
		else if (random <= 8)
			OutStr += gettext("and lashed");
		else if (random <= 10)
			OutStr += gettext("tenderly");
		else
			OutStr += gettext("like a dog");	

		OutStr += gettext(" by");
		break;
	case 2: 
		OutStr += gettext("Explanations were necessary before she was "); 
		
		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("screwed");
		else if (random <= 4)
			OutStr += gettext("penetrated");
		else if (random <= 6)
			OutStr += gettext("abused");
		else if (random <= 8)
			OutStr += gettext("whipped");
		else if (random <= 10)
			OutStr += gettext("yelled at");
		else if (random <= 12)
			OutStr += gettext("banged repeatedly");
		else
			OutStr += gettext("smacked around");

		OutStr += gettext(" by");
		break;
	case 3: 
		OutStr += gettext("Her holes were filled "); 
	
		random = g_Dice%16 + 1;
		if (random <= 2)
			OutStr += gettext("with wiggly things");
		else if (random <= 4)
			OutStr += gettext("with vibrating things");
		else if (random <= 6)
			OutStr += gettext("with sex toys");
		else if (random <= 8)
			OutStr += gettext("by things with uncomfortable edges");
		else if (random <= 10)
			OutStr += gettext("with marbles");
		else if (random <= 12)
			OutStr += gettext("with foreign objects");
		else if (random <= 14)
			OutStr += gettext("with hopes and dreams");
		else
			OutStr += gettext("with semen");

		OutStr += gettext(" by");
		break;
	case 4: 
		OutStr += gettext("A massive aphrodisiac was administered before she was "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("teased");
		else if (random <= 4)
			OutStr += gettext("fucked");
		else if (random <= 6)
			OutStr += gettext("left alone");
		else if (random <= 8)
			OutStr += gettext("repeatedly brought to the edge of orgasm, but not over");
		else 
			OutStr += gettext("mercilessly tickled by a feather wielded");

		OutStr += gettext(" by");
		break;
	case 5: 
		OutStr += gettext("Entertainment was demanded before she was "); 
		
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("humped");
		else if (random <= 4)
			OutStr += gettext("rough-housed");
		else if (random <= 6)
			OutStr += gettext("pinched over and over");
		else if (random <= 8)
			OutStr += gettext("probed by instruments");
		else if (random <= 10)
			OutStr += gettext("fondled roughly");
		else
			OutStr += gettext("sent away");

		OutStr += gettext(" by");
		break;
	case 6: 
		OutStr += gettext("She was pierced repeatedly by ");
		
		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("needles");
		else if (random <= 4)
			OutStr += gettext("magic missiles");
		else
			OutStr += gettext("evil thoughts");
			
		OutStr += gettext(" from"); 
		break;
	case 7: 
		//OutStr += "She had ";
			
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Weights");
		else if (random <= 4)
			OutStr += gettext("Christmas ornaments");
		else if (random <= 6)
			OutStr += gettext("Lewd signs");
		else if (random <= 6)
			OutStr += gettext("Trinkets");
		else
			OutStr += gettext("Abstract symbols");
			
		OutStr += gettext(" were hung from her unmentionables by"); 
		break;
	case 8: 
		OutStr += gettext("She was ordered to "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("flail herself");
		else if (random <= 4)
			OutStr += gettext("perform fellatio");
		else if (random <= 6)
			OutStr += gettext("masturbate");
		else
			OutStr += gettext("beg for it");

		OutStr += gettext(" by");
		break;
	}
# pragma endregion bdsm2

	// Part 3
# pragma region bdsm3
	OutStr += gettext(" ");

	roll3 = g_Dice % 18 + 1;
	switch (roll3)
	{
	case 1: OutStr += gettext("Iron Man."); break;
	case 2: 
		OutStr += gettext("the ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("wizard"); 
		else if (random <= 4)
			OutStr += gettext("sorceress");
		else if (random <= 6)
			OutStr += gettext("archmage");
		else
			OutStr += gettext("warlock");

		OutStr += gettext("'s ");

		random = g_Dice%8;
		if (random <= 2)
			OutStr += gettext("golem.");
		else if (random <= 4)
			OutStr += gettext("familiar.");
		else if (random <= 6)
			OutStr += gettext("homoculous.");
		else
			OutStr += gettext("summoned monster.");

		break;
	case 3: 
		OutStr += gettext("the amazingly hung "); 
	
		random = g_Dice%8;
		if (random <= 2)
			OutStr += gettext("goblin.");
		else if (random <= 4)
			OutStr += gettext("civic worker.");
		else if (random <= 6)
			OutStr += gettext("geletanious cube.");
		else
			OutStr += gettext("sentient shirt.");   // MYR: I love this one.

		break;
	case 4: OutStr += gettext("the pirate dressed as a ninja. (Cool things are cool.)"); break;
	case 5: OutStr += gettext("Hannibal Lecter."); break;
	case 6: 
		OutStr += gettext("the stoned "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("dark elf.");
		else if (random <= 4)
			OutStr += gettext("gargoyle.");
		else if (random <= 6)
			OutStr += gettext("earth elemental.");
		else if (random <= 8)
			OutStr += gettext("astral deva.");
		else
			OutStr += gettext("college kid.");		
		
		break;
	case 7: 
		OutStr += gettext("your hyperactive ");
				
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("cousin.");
		else if (random <= 4)
			OutStr += gettext("grandmother.");
		else if (random <= 6)
			OutStr += gettext("grandfather.");
		else if (random <= 8)
			OutStr += gettext("brother.");
		else
			OutStr += gettext("sister.");

			break;
	case 8: OutStr += gettext("someone who looks exactly like you!"); break;
	case 9: 
		OutStr += gettext("the horny "); 
				 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("genie.");
		else if (random <= 4)
			OutStr += gettext("fishmonger.");
		else if (random <= 6)
			OutStr += gettext("chauffeur.");
		else if (random <= 8)
			OutStr += gettext("Autobot.");
		else
			OutStr += gettext("thought.");

		break;
	case 10: 
		OutStr += gettext("the rampaging "); 
				 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("english professor.");
		else if (random <= 4)
			OutStr += gettext("peace activist.");
		else if (random <= 6)
			OutStr += gettext("color red.");
		else if (random <= 8)
			OutStr += gettext("special forces agent.");
		else
			OutStr += gettext("chef.");

		break;
	case 11: 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("disloyal thugs");
		else if (random <= 4)
			OutStr += gettext("girls");
		else if (random <= 6)
			OutStr += gettext("dissatisfied customers");
		else if (random <= 8)
			OutStr += gettext("workers");
		else
			OutStr += gettext("malicious agents");		
		
		OutStr += gettext(" from a competing brothel."); break;
	case 12: OutStr += gettext("a cruel "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("Cyberman.");
		else if (random <= 4)
			OutStr += gettext("Dalek.");
		else if (random <= 6)
			OutStr += gettext("Newtype.");
		else
			OutStr += gettext("Gundam.");

		break;
	case 13: OutStr += gettext("Sexbot Mk-"); 
		
		random = g_Dice%200+50;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(".");

		break;
	case 14: 
		OutStr += gettext("underage kids ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("(Who claim to be of age.)");
		else if (random <= 4)
			OutStr += gettext("(Who snuck in.)");
		else if (random <= 6)
			OutStr += gettext("(Who are somehow related to the Brothel Master, so its ok.)");
		else if (random <= 8)
			OutStr += gettext("(They paid, so who cares?)");
		else
			OutStr += gettext("(We must corrupt them while they're still young.)");
			
		break;
	case 15: OutStr += gettext("Grandpa Parkins from down the street."); break;
	case 16: 
		OutStr += gettext("the ... thing living "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("in the underwear drawer");
		else if (random <= 4)
			OutStr += gettext("in the sex-toy box");
		else if (random <= 6)
			OutStr += gettext("under the bed");
		else if (random <= 8)
			OutStr += gettext("in her shadow");
		else
			OutStr += gettext("in her psyche");

		OutStr += gettext(".");
		break;
	case 17: OutStr += gettext("the senior member of the cult of ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tentacles.");
		else if (random <= 4)
			OutStr += gettext("unending pain.");
		else if (random <= 6)
			OutStr += gettext("joy and happiness.");
		else if (random <= 8)
			OutStr += gettext("Whore Master developers.");
		else
			OutStr += gettext("eunuchs.");

		break;
	case 18: 
		OutStr += gettext("this wierdo who appeared out of this blue box called a ");
			
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("TARDIS."); 
		else
			OutStr += gettext("TURDIS"); // How many people will say I made a spelling mistake?

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += " ";
			random = g_Dice%10 + 1;
			if (random <= 2)
				OutStr += gettext("His female companion was in on the action too.");
			else if (random <= 4)
				OutStr += gettext("His mechanical dog was involved as well.");
			else if (random <= 6)
				OutStr += gettext("His female companion and mechanical dog did lewd things to each other and watched.");
		}
		break;
	}
# pragma endregion bdsm3

	OutStr += "\n";
	return OutStr;
}

string cGirls::GetRandomBeastString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	char buffer[10];
	string OutStr;
	bool NeedAnd = false;

	OutStr += gettext(" was ");

# pragma region beast1
	roll1 = g_Dice % 7 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OutStr += gettext("filled with ");
		
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("vibrating");
		else if (random <= 4)
			OutStr += gettext("wiggling");
		else if (random <= 6)
			OutStr += gettext("living");
		else if (random <= 8)
			OutStr += gettext("energetic");
		else if (random <= 10)
			OutStr += gettext("big");
		else 
			OutStr += gettext("pokey");
		
		OutStr += gettext(" things that "); 
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("tickled");
		else if (random <= 4)
			OutStr += gettext("pleasured");
		else if (random <= 6)
			OutStr += gettext("massaged");
		else
			OutStr += gettext("scraped");

		OutStr += gettext(" her insides");
		break;
	case 2: 
		OutStr += gettext("forced against ");
			
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("a wall");
		else if (random <= 4)
			OutStr += gettext("a window");
		else if (random <= 6)
			OutStr += gettext("another client");
		else
			OutStr += gettext("another girl");

		OutStr += gettext(" and told to ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("spread her legs");
		else if (random <= 4)
			OutStr += gettext("give up hope");
		else if (random <= 6)
			OutStr += gettext("hold on tight");
		else
			OutStr += gettext("smile through it");

		break;
	case 3: 
		OutStr += gettext("worried by the ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("huge size");
		else if (random <= 4)
			OutStr += gettext("skill");
		else if (random <= 6)
			OutStr += gettext("reputation");
		else
			OutStr += gettext("aggressiveness"); 
		
		OutStr += gettext(" of the client");
		break;
	case 4: 
		OutStr += gettext("stripped down to her ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("g-string");
		else if (random <= 4)
			OutStr += gettext("panties");
		else if (random <= 6)
			OutStr += gettext("bra and panties");
		else if (random <= 8)
			OutStr += gettext("teddy");
		else
			OutStr += gettext("skin");

		OutStr += gettext(" and covered in ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("compliments");
		else if (random <= 4)
			OutStr += gettext("abuse");
		else if (random <= 6)
			OutStr += gettext("peanut butter");
		else if (random <= 8)
			OutStr += gettext("honey");
		else
			OutStr += gettext("motor oil");			
		
		break;
	case 5: 
		OutStr += gettext("chained up in the ");
				
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("dungeon");
		else if (random <= 4)
			OutStr += gettext("den");
		else if (random <= 6)
			OutStr += gettext("kitchen");
		else if (random <= 8)
			OutStr += gettext("most public of places");
		else
			OutStr += gettext("backyard");			
		
		OutStr += gettext(" and her ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("arms");
		else if (random <= 4)
			OutStr += gettext("legs");
		else
			OutStr += gettext("arms and legs");

		OutStr += gettext(" were lashed to posts");
		break;
	case 6: 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tied up");
		else if (random <= 4)
			OutStr += gettext("wrapped up");
		else if (random <= 6)
			OutStr += gettext("trapped");
		else if (random <= 8)
			OutStr += gettext("bound");
		else
			OutStr += gettext("covered");	

		OutStr += gettext(" in ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("flesh");
		else if (random <= 4)
			OutStr += gettext("tentacles");
		else if (random <= 6)
			OutStr += gettext("cellophane");
		else if (random <= 8)
			OutStr += gettext("tape");
		else
			OutStr += gettext("false promises");			
		
		OutStr += gettext(" and ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("groped");
		else if (random <= 4)
			OutStr += gettext("tweaked");
		else if (random <= 6)
			OutStr += gettext("licked");
		else if (random <= 8)
			OutStr += gettext("spanked");
		else
			OutStr += gettext("left alone");

		OutStr += gettext(" for hours");
		break;				
	case 7: 
		OutStr += gettext("pushed to the limits of ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("flexibility");
		else if (random <= 4)
			OutStr += gettext("endurance");
		else if (random <= 6)
			OutStr += gettext("patience");
		else if (random <= 8)
			OutStr += gettext("consciousness");
		else
			OutStr += gettext("sanity");			
		
		OutStr += gettext(" and ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("cried out");
		else if (random <= 4)
			OutStr += gettext("swooned");
		else
			OutStr += gettext("spasmed");
		break;
	}
# pragma endregion beast1

	// Part 2
# pragma region beast2
	OutStr += ". ";

	roll2 = g_Dice % 9 + 1;
	switch (roll2)
	{
	case 1: 
		OutStr += gettext("She ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("smoothly");
		else if (random <= 4)
			OutStr += gettext("roughly");
		else if (random <= 6)
			OutStr += gettext("lustily");
		else if (random <= 8)
			OutStr += gettext("repeatedly");
		else
			OutStr += gettext("orgasmically");

		OutStr += gettext(" ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("fucked");
		else if (random <= 4)
			OutStr += gettext("railed");
		else if (random <= 6)
			OutStr += gettext("banged");
		else if (random <= 8)
			OutStr += gettext("screwed");
		else
			OutStr += gettext("pleasured");
		
		break;
	case 2: 
		OutStr += gettext("She was ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("teased");
		else if (random <= 4)
			OutStr += gettext("taunted");       
		else 
			OutStr += gettext("roughed up");
	
		OutStr += gettext(" and ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("orally");
		else if (random <= 4)
			OutStr += "";         // MYR: This isn't a bug.  'physically violated' is redundant, so this just prints 'violated'
		else if (random <= 6)
			OutStr += gettext("mentally");
		else if (random <= 8)
			OutStr += gettext("repeatedly");
		else
			OutStr += gettext("haughtily");			
		
		OutStr += gettext(" violated by"); 
		break;
	case 3:
		OutStr += gettext("She was drenched in "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("cum");
		else if (random <= 4)
			OutStr += gettext("sweat");
		else if (random <= 6) 
			OutStr += gettext("broken hopes and dreams");
		else if (random <= 8)
			OutStr += gettext("Koolaid");
		else
			OutStr += gettext("sticky secretions");

		OutStr += gettext(" by");
		break;
	case 4: 
		OutStr += gettext("She ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("moaned");
		else if (random <= 4)
			OutStr += gettext("winced");
		else if (random <= 6) 
			OutStr += gettext("swooned");
		else if (random <= 8) 
			OutStr += gettext("orgasmed");	
		else
			OutStr += gettext("begged for more");
		
		OutStr += gettext(" as her stomach repeatedly poked out from ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("thrusts");
		else if (random <= 4)
			OutStr += gettext("strokes");
		else if (random <= 6) 
			OutStr += gettext("fistings");
		else
			OutStr += gettext("a powerful fucking");
			
		OutStr += gettext(" by"); 
		break;
	case 5:
		OutStr += gettext("She used her ");

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext("hands, ");
			NeedAnd = true;
		}

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext("feet, ");
			NeedAnd = true;
		}

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext("mouth, ");
			NeedAnd = true;
		}

		if (NeedAnd == true)
		{
			OutStr += gettext("and ");
			NeedAnd = false;   // Just in case it's used again here
		}

		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("pussy");
		else
			OutStr += gettext("holes");

		OutStr += gettext(" to ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("please");
		else if (random <= 4)
			OutStr += gettext("pleasure");
		else
			OutStr += gettext("tame");

		break;
	case 6: 
		OutStr += gettext("She shook with "); 
				
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("orgasmic joy");
		else if (random <= 4)
			OutStr += gettext("searing pain");
		else if (random <= 6)
			OutStr += gettext("frustration");
		else if (random <= 8)
			OutStr += gettext("agony");
		else
			OutStr += gettext("frustrated boredom");

		OutStr += gettext(" when fondled by"); 
		break;
	case 7: 
		OutStr += gettext("It felt like she was ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("ripping apart");
		else if (random <= 4)
			OutStr += gettext("exploding");
		else if (random <= 6)
			OutStr += gettext("imploding");
		else if (random <= 8)
			OutStr += gettext("nothing");
		else
			OutStr += gettext("absent");

		OutStr += gettext(" when handled by"); 
		break;
	case 8: 
		OutStr += gettext("She passed out from ");
				 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("pleasure");
		else if (random <= 4)
			OutStr += gettext("pain");
		else if (random <= 6)
			OutStr += gettext("boredom");
		else if (random <= 8)
			OutStr += gettext("rough sex");
		else
			OutStr += gettext("inactivity");

		OutStr += gettext(" from");
		break;
	case 9: 
		OutStr += gettext("She screamed as ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("wrenching pain");
		else if (random <= 4)
			OutStr += gettext("powerful orgasms");
		else if (random <= 6)
			OutStr += gettext("incredible sensations");
		else if (random <= 8)
			OutStr += gettext("freight trains");
		else
			OutStr += gettext("lots and lots of nothing");

		OutStr += gettext(" thundered through her from"); 
		break;
	}
# pragma endregion beast2

	// Part 3
# pragma region beast3
	OutStr += " ";

	roll3 = g_Dice % 12 + 1;
	switch (roll3)
	{
	case 1: 
		OutStr += gettext("the ravenous ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tentacles.");
		else if (random <= 4)
			OutStr += gettext(", sex-starved essences of lust.");
		else if (random <= 6)
			OutStr += gettext("Balhannoth. (Monster Manual 4, pg. 15.)");
		else if (random <= 8)
			OutStr += gettext("priest.");
		else
			OutStr += gettext("Yugoloth.");

		break;
	case 2: 
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("an evil");
		else
			OutStr += gettext("a misunderstood");

		OutStr += gettext(" ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tengu.");
		else if (random <= 4)
			OutStr += gettext("Whore Master developer.");
		else if (random <= 6)
			OutStr += gettext("school girl.");
		else if (random <= 8)
			OutStr += gettext("garden hose.");
		else
			OutStr += gettext("thought.");				
				
				break;
	case 3: 
		OutStr += gettext("a major "); 
		
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("demon");
		else if (random <= 4)
			OutStr += gettext("devil");
		else if (random <= 6)
			OutStr += gettext("oni");
		else if (random <= 8)
			OutStr += gettext("fire elemental");
		else if (random <= 10)
			OutStr += gettext("god");
		else
			OutStr += gettext("Mr. Coffee");

		OutStr += gettext(" from the outer planes."); 
		break;
	case 4: 
		OutStr += gettext("the angel.");
				
		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" ('");
			random = g_Dice%8 + 1;
			if (random <= 2)
				OutStr += gettext("You're very pretty");
			else if (random <= 4)
				OutStr += gettext("I was never here");
			else if (random <= 6)
				OutStr += gettext("I had a great time");
			else 
				OutStr += gettext("I didn't know my body could do that");

			OutStr += gettext("' he said.)");
		}
			
		break;
	case 5:
		OutStr += gettext("the ");

		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("demon");
		else if (random <= 4)
			OutStr += gettext("major devil");
		else if (random <= 6)
			OutStr += gettext("oni");
		else if (random <= 8)
			OutStr += gettext("earth elemental");
		else if (random <= 10)
			OutStr += gettext("raging hormome beast");
		else
			OutStr += gettext("Happy Fun Ball");

		OutStr += gettext(" with an urge to exercise his ");
		
		random = g_Dice%30 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" cocks and ");
		
		random = g_Dice%30 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" claws.");

		break;
	case 6: OutStr += gettext("the swamp thing with (wait for it) swamp tentacles!"); break;
	case 7: OutStr += gettext("the pirnja gestalt. (The revolution is coming.)"); break;
	case 8: 
		OutStr += gettext("the color ");
			
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("purple");
		else if (random <= 4)
			OutStr += gettext("seven");  // MYR: Not a mistake. I meant to write 'seven'.
		else if (random <= 6)
			OutStr += gettext("mauve");
		else if (random <= 8)
			OutStr += gettext("silver");
		else if (random <= 10)
			OutStr += gettext("ochre");
		else
			OutStr += gettext("pale yellow");
			
		OutStr += gettext(".");
		break;
	case 9: 
		random = g_Dice%10 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		
		OutStr += gettext(" werewolves wearing ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("true");
		else if (random <= 4)
			OutStr += gettext("minor artifact"); 
		else if (random <= 6)
			OutStr += gettext("greater artifact");
		else if (random <= 10)
			OutStr += gettext("godly");
		else
			OutStr += gettext("near omnipitent");

		OutStr += gettext(" rings of the ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("eternal");
		else if (random <= 4)
			OutStr += gettext("body breaking"); 
		else if (random <= 6)
			OutStr += gettext("vorporal");
		else if (random <= 10)
			OutStr += gettext("transcendent");
		else
			OutStr += gettext("incorporeal"); 
			
		OutStr += gettext(" hard-on."); 
		break;
	case 10: 
		random = g_Dice%10 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;		
		
		OutStr += gettext(" Elder Gods.");
		
		random = g_Dice%10 + 1;
		if (random <= 4)
		{
			OutStr += gettext(" (She thought ");

			random = g_Dice%12 + 1;
			if (random <= 2)
				OutStr += gettext("Cthulhu");
			else if (random <= 4)
				OutStr += gettext("Hastur");
			else if (random <= 6)
				OutStr += gettext("an Old One");
			else if (random <= 8)
				OutStr += gettext("Shub-Niggurath");
			else if (random <= 10)
				OutStr += gettext("Nyarlathotep");
			else
				OutStr += gettext("Yog-Sothoth");

			OutStr += gettext(" was amongst them, but blacked out after a minute or so.)"); 
		}
		break;
	case 11: 
		OutStr += gettext("the level ");
		
		random = g_Dice%20 + 25;
		_itoa(random, buffer, 10);
		OutStr += buffer;

		OutStr += gettext(" epic paragon ");
			
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("troll");
		else if (random <= 4)
			OutStr += gettext("beholder");
		else if (random <= 6)
			OutStr += gettext("displacer beast");
		else if (random <= 8)
			OutStr += gettext("ettin");
		else if (random <= 10)
			OutStr += gettext("gargoyle");
		else
			OutStr += gettext("fire extinguisher");
			
		OutStr += gettext(" with ");
		
		random = g_Dice%20 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" strength and ");
		
		random = g_Dice%20 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" constitution.");
		
		break;
	case 12: 
		OutStr += gettext("the phalanx of ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("horny orcs.");
		else if (random <= 4)
			OutStr += gettext("goblins.");
		else if (random <= 6)
			OutStr += gettext("sentient marbles.");
		else if (random <= 8)
			OutStr += gettext("living garden gnomes.");
		else
			OutStr += gettext("bugbears.");

		break;
	}
# pragma endregion beast3

	OutStr += gettext("\n");
	return OutStr;
}

string cGirls::GetRandomLesString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0, plus = 0;
	string OutStr;
	char buffer[10];

	OutStr += gettext(" ");

	// Part1
# pragma region les1
	roll1 = g_Dice % 6 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: 
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("aggressively");
		else if (random <= 4)
			OutStr += gettext("tenderly");
		else if (random <= 6)
			OutStr += gettext("slowly");
		else if (random <= 8) 
			OutStr += gettext("authoratively");
		else
			OutStr += gettext("violently");

		OutStr += gettext(" ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("straddled");
		else if (random <= 4)
			OutStr += gettext("scissored");
		else if (random <= 6)
			OutStr += gettext("symmetrically docked with");
		else if (random <= 8) 
			OutStr += gettext("cowgirled");	
		else
			OutStr += gettext("69ed");

		OutStr += gettext(" the woman");
		break;
	case 2: 
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("shaved her");
		else
			OutStr += gettext("was shaved");
			
		OutStr += gettext(" with a +"); 
		
		plus = g_Dice%7 + 4;
		_itoa(plus, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" ");

		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("vorporal broadsword");
		else if (random <= 4)
		{
			OutStr += gettext("dagger, +");
			plus = plus + g_Dice%5 + 2;
			_itoa(plus, buffer, 10);
			OutStr += buffer;				
			OutStr += gettext(" vs pubic hair");
		}
		else if (random <= 6)
			OutStr += gettext("flaming sickle");
		else if (random <= 8) 
			OutStr += gettext("lightning burst bo-staff");	
		else if (random <= 10)
			OutStr += gettext("human bane greatsword");
		else if (random <= 12)
			OutStr += gettext("acid burst warhammer");
		else
			OutStr += gettext("feral halfling");

		break;
	case 3: 
		OutStr += gettext("had a "); 
		
		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("pleasant");
		else if (random <= 4)
			OutStr += gettext("long");
		else if (random <= 6)
			OutStr += gettext("heartfelt");
		else if (random <= 8)
			OutStr += gettext("deeply personal");	
		else if (random <= 10)
			OutStr += gettext("emotional");
		else if (random <= 12)
			OutStr += gettext("angry");	
		else
			OutStr += gettext("violent");
		
		OutStr += gettext(" conversation with her lady-client about ");

		random = g_Dice%16 + 1;
		if (random <= 2)
			OutStr += gettext("sadism");
		else if (random <= 4)
			OutStr += gettext("particle physics");
		else if (random <= 6)
			OutStr += gettext("domination");
		else if (random <= 8) 
			OutStr += gettext("submission");
		else if (random <= 10)
			OutStr += gettext("brewing poisons");
		else if (random <= 12) 
			OutStr += gettext("flower arranging");
		else if (random <= 14)
			OutStr += gettext("the Brothel Master");
		else
			OutStr += gettext("assassination techniques");

		break;
	case 4: 
		OutStr += gettext("massaged the woman with "); 
		
		// MYR: Ok, I know I'm being super-silly
		random = g_Dice%20 + 1;
		if (random <= 2)
			OutStr += gettext("bath oil");
		else if (random <= 4)
			OutStr += gettext("aloe vera");
		else if (random <= 6)
			OutStr += gettext("the tears of Chuck Norris's many victims");
		else if (random <= 8)
			OutStr += gettext("the blood of innocent angels");
		else if (random <= 10)
			OutStr += gettext("Unicorn blood");
		else if (random <= 12)
			OutStr += gettext("Unicorn's tears");
		else if (random <= 14)
			OutStr += gettext("a strong aphrodisiac");
		else if (random <= 16)
			OutStr += gettext("oil of greater breast growth");
		else if (random <= 18)
			OutStr += gettext("potent oil of massive breast growth");
		else
			OutStr += gettext("oil of camel-toe growth");

		break;
	case 5: 
		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("put a ball gag and blindfolded on");
		else
			OutStr += gettext("put a sensory deprivation hood on"); 
		
		OutStr += gettext(", was ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("lashed");
		else if (random <= 4)
			OutStr += gettext("cuffed");
		else if (random <= 6)
			OutStr += gettext("tied");
		else 
			OutStr += gettext("chained");

		OutStr += gettext(" to a ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("bed");
		else if (random <= 4)
			OutStr += gettext("bench");
		else if (random <= 6)
			OutStr += gettext("table");
		else 
			OutStr += gettext("post");

		OutStr += gettext(" and ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("symmetrically docked");
		else if (random <= 4)
			OutStr += gettext("69ed");
		else if (random <= 6)
			OutStr += gettext("straddled");
		else 
			OutStr += gettext("scissored");
		
		break;
	case 6: 
		// MYR: This is like a friggin movie! The epic story of the whore and her customer.
		OutStr += gettext("looked at the woman across from her. ");
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Position");
		else if (random <= 4)
			OutStr += gettext("Toy");
		else if (random <= 6)
			OutStr += gettext("Oil");
		else if (random <= 8)
			OutStr += gettext("Bed sheet color");
		else
			OutStr += gettext("Price");

		OutStr += gettext(" was to be ");
		
		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("a trial");
		else if (random <= 4)
			OutStr += gettext("decided");
		else if (random <= 6)
			OutStr += gettext("resolved");
		else 
			OutStr += gettext("dictated");

		OutStr += gettext(" by combat. Both had changed into "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("string bikinis");
		else if (random <= 4)
			OutStr += gettext("lingerie");
		else if (random <= 6)
			OutStr += gettext("body stockings");
		else if (random <= 8)
			OutStr += gettext("their old school uniforms");
		else
			OutStr += gettext("dominatrix outfits");

		OutStr += gettext(" and wielded ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("whips");
		else if (random <= 4)
			OutStr += gettext("staves");
		else if (random <= 6)
			OutStr += gettext("boxing gloves");
		else 
			OutStr += gettext("cat-o-nine tails");

		OutStr += gettext(" of ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("explosive orgasms");
		else if (random <= 4)
			OutStr += gettext("clothes shredding");
		else if (random <= 6)
			OutStr += gettext("humiliation");
		else if (random <= 8) 
			OutStr += gettext("subjugation");
		else
			OutStr += gettext("brutal stunning");

		OutStr += gettext(". ");

		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			OutStr += gettext("They stared at each other across the ");
			random = g_Dice%8 + 1;
			if (random <= 2)
				OutStr += gettext("mud");
			else if (random <= 4)
				OutStr += gettext("jello");
			else if (random <= 6)
				OutStr += gettext("whip cream");
			else 
				OutStr += gettext("clothes-eating slime");
			OutStr += gettext(" pit.");
		}

		OutStr += gettext(" A bell sounded! They charged and ");
		
		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("dueled");
		else if (random <= 4)
			OutStr += gettext("fought it out");
		else if (random <= 6)
			OutStr += gettext("battled");
			
		OutStr += gettext("!\n");

		random = g_Dice%10 + 1;
		if (random <= 6)  // MYR: small bias for the customer
			OutStr += gettext("The customer won");
		else
			OutStr += gettext("The customer was vanquished");

		break;
	}
# pragma endregion les1

	OutStr += gettext(". ");

	// Part 2
# pragma region les2
	roll2 = g_Dice%8 + 1;
	switch (roll2)
	{
	case 1: 
		OutStr += gettext("She was "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("tormented");
		else if (random <= 4)
			OutStr += gettext("teased");
		else if (random <= 6)
			OutStr += gettext("massaged");
		else if (random <= 8) 
			OutStr += gettext("frustrated");
		else
			OutStr += gettext("satisfied");

		OutStr += gettext(" with ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("great care");
		else if (random <= 4)
			OutStr += gettext("deva feathers");
		else if (random <= 6)
			OutStr += gettext("drug-soaked sex toys");
		else if (random <= 8) 
			OutStr += gettext("extreme skill");
		else
			OutStr += gettext("wild abandon");

		OutStr += gettext(" by");
		break;
	case 2: 
		// Case 1 reversed and reworded
		OutStr += gettext("She used ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("phoenix down");
		else if (random <= 4)
			OutStr += gettext("deva feathers");
		else if (random <= 6)
			OutStr += gettext("drug-soaked sex toys");
		else if (random <= 8) 
			OutStr += gettext("restraints");
		else
			OutStr += gettext("her wiles");

		OutStr += gettext(" to "); 
		
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("torment");
		else if (random <= 4)
			OutStr += gettext("tease");
		else if (random <= 6)
			OutStr += gettext("massage");
		else if (random <= 8) 
			OutStr += gettext("frustrate");
		else
			OutStr += gettext("satisfy");

		break;
	case 3: 
		OutStr += gettext("She ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("fingered");
		else if (random <= 4)
			OutStr += gettext("teased");
		else if (random <= 6)
			OutStr += gettext("caressed");
		else if (random <= 8) 
			OutStr += gettext("fondled");
		else
			OutStr += gettext("pinched");
			
		OutStr += gettext(" the client's "); 

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("clit");
		else if (random <= 4)
			OutStr += gettext("clitorus");
		else
			OutStr += gettext("love bud");
	
		OutStr += gettext(" and expertly elicited orgasm after orgasm from");
		break;
	case 4: 
		OutStr += gettext("Her "); 

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("clit");
		else if (random <= 4)
			OutStr += gettext("clitorus");
		else
			OutStr += gettext("love bud");

		OutStr += gettext(" was ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("fingered");
		else if (random <= 4)
			OutStr += gettext("teased");
		else if (random <= 6)
			OutStr += gettext("caressed");
		else if (random <= 8) 
			OutStr += gettext("fondled");
		else
			OutStr += gettext("pinched");

		OutStr += gettext(" and she orgasmed repeatedly under the expert touch of");
		break;
	case 5:
		OutStr += gettext("She ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("clam wrestled");
		else if (random <= 4)
			OutStr += gettext("rubbed");
		else if (random <= 6)
			OutStr += gettext("attacked");
		else
			OutStr += gettext("hammered");

		OutStr += gettext(" the client's "); 

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("clit");
		else if (random <= 4)
			OutStr += gettext("clitorus");
		else if (random <= 6)
			OutStr += gettext("love bud");
		else
			OutStr += gettext("vagina");

		OutStr += gettext(" causing waves of orgasms to thunder through");
		break;
	case 6: 
		OutStr += gettext("She ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("single mindedly");
		else if (random <= 4)
			OutStr += gettext("repeatedly");
		else
			OutStr += gettext("roughly");

		OutStr += gettext(" ");

		random = g_Dice%2 + 1;
		if (random <= 2)
			OutStr += gettext("rubbed");
		else if (random <= 4)
			OutStr += gettext("fondled");
		else if (random <= 6)
			OutStr += gettext("prodded");
		else if (random <= 8)
			OutStr += gettext("attacked");
		else if (random <= 10)
			OutStr += gettext("tongued");
		else
			OutStr += gettext("licked");

		OutStr += gettext(" the client's g-spot. Wave after wave of "); 

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("orgasms");
		else if (random <= 4)
			OutStr += gettext("pleasure");
		else if (random <= 6)
			OutStr += gettext("powerful sensations");
		else
			OutStr += gettext("indescribable joy");

		OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("rushed");
		else if (random <= 4)
			OutStr += gettext("thundered");
		else if (random <= 6)
			OutStr += gettext("cracked");
		else
			OutStr += gettext("pounded");

		OutStr += gettext(" through");
		break;
	case 7: 
		OutStr += gettext("Wave after wave of "); 

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("orgasms");
		else if (random <= 4)
			OutStr += gettext("back-stretching joy");
		else if (random <= 6)
			OutStr += gettext("madness");
		else
			OutStr += gettext("incredible feeling");

		OutStr += gettext(" ");

		random = g_Dice%8 + 1;
		if (random <= 2)
			OutStr += gettext("throbbed");
		else if (random <= 4)
			OutStr += gettext("shook");
		else if (random <= 6)
			OutStr += gettext("arced");
		else
			OutStr += gettext("stabbed");

		OutStr += gettext(" through her as she was ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("single mindedly");
		else if (random <= 4)
			OutStr += gettext("repeatedly");
		else
			OutStr += gettext("roughly");

		OutStr += gettext(" ");

		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("rubbed");
		else if (random <= 4)
			OutStr += gettext("fondled");
		else if (random <= 6)
			OutStr += gettext("prodded");
		else if (random <= 8)
			OutStr += gettext("attacked");
		else if (random <= 10)
			OutStr += gettext("tongued");
		else
			OutStr += gettext("licked");

		OutStr += gettext(" by");	
		break;
	case 8:
		// MYR: I just remembered about \n
		OutStr += gettext("Work stopped ");

		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("in the brothel");
		else if (random <= 4)
			OutStr += gettext("on the street");
		else if (random <= 6)
			OutStr += gettext("all over the block");
		else if (random <= 8)
			OutStr += gettext("in the town");
		else if (random <= 10)
			OutStr += gettext("within the country");
		else  if (random <= 12)
			OutStr += gettext("over the whole planet");
		else  if (random <= 12)
			OutStr += gettext("within the solar system");
		else
			OutStr += gettext("all over the galactic sector");


		OutStr += gettext(". Everything was drowned out by:\n\n");
		OutStr += gettext("Ahhhhh!\n\n");


		random = g_Dice%10 + 1;
		if (random <= 5)
		{
			random = g_Dice%6 + 1;
			if (random <= 2)
				OutStr += gettext("For the love... of aaaaahhhhh mercy.  No nnnnnnnnh more!\n\n");
			else if (random <= 4)
				OutStr += gettext("oooooOOOOOO YES! ahhhhhhHHHH!\n\n");
			else
				OutStr += gettext("nnnnnhhh nnnnnhhhh NNNHHHHHH!!!!\n\n");
		}

		OutStr += gettext("Annnnnhhhhhaaa!\n\n");
		OutStr += gettext("AHHHHHHHH! I'm going to ");

		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("CCCUUUUUUMMMMMMMM!!!!!");
		else if (random <= 4)
			OutStr += gettext("EEEXXXXXPLLLOOODDDDEEEE!!!");
		else if (random <= 6)
			OutStr += gettext("DIEEEEEE!");
		else if (random <= 8)
			OutStr += gettext("AHHHHHHHHHHH!!!!");
		else if (random <= 10)
			OutStr += gettext("BRRRREEEEAAAAKKKKKK!!!!");
		else
			OutStr += gettext("WAAAAHHHHHOOOOOOO!!!");

		OutStr += gettext("\nfrom ");
		break;
	}
# pragma endregion les2

	OutStr += " ";

	// Part 3
# pragma region les3

	// For case 2
	int BrothelNo = -1, NumGirlsInBroth = -1;
	sGirl * TempGPtr = 0;

	roll3 = g_Dice %6 + 1;
	switch (roll3)
	{
	case 1: 
		OutStr += gettext("the ");
			
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("prostitute");
		else if (random <= 4)
			OutStr += gettext("street worker");
		else if (random <= 6)
			OutStr += gettext("hooker");
		else if (random <= 8)
			OutStr += gettext("street walker");
		else
			OutStr += gettext("working girl");
			
		OutStr += gettext(" from a ");
			
		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("friendly");
		else if (random <= 4)
			OutStr += gettext("rival");
		else if (random <= 6)
			OutStr += gettext("hostile");
		else 
			OutStr += gettext("feeder");
			
		OutStr += gettext(" brothel.");
		break;
	case 2: 
		BrothelNo = g_Dice%g_Brothels.GetNumBrothels();
		NumGirlsInBroth = g_Brothels.GetNumGirls(BrothelNo);
		random = g_Dice%NumGirlsInBroth;
		TempGPtr = g_Brothels.GetGirl(BrothelNo, random);

		if(TempGPtr == 0)
			OutStr += "a girl";
		else
			OutStr += TempGPtr->m_Realname;
		OutStr += gettext(" from ");
		OutStr += g_Brothels.GetName(BrothelNo);
		OutStr += gettext(" brothel.");
		
		BrothelNo = -1;        // MYR: Paranoia
		NumGirlsInBroth = -1;
		TempGPtr = 0;
		break;
	case 3: 
		OutStr += gettext("the ");
		
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("sexy");
		else if (random <= 4)
			OutStr += gettext("rock hard");
		else if (random <= 6)
			OutStr += gettext("hot");
		else if (random <= 8)
			OutStr += gettext("androgonous");
		else if (random <= 10)
			OutStr += gettext("spirited");
		else
			OutStr += gettext("exuberant"); 
		
		OutStr += gettext(" MILF."); 
		break;
	case 4:
		OutStr += gettext("the ");

		random = g_Dice%10 + 1;
		if (random <= 5)
			OutStr += gettext("senior");
		else
			OutStr += gettext("junior");

		OutStr += gettext(" ");

		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("Sorceress");
		else if (random <= 4)
			OutStr += gettext("Warrioress");
		else if (random <= 6)
			OutStr += gettext("Priestess");
		else if (random <= 8)
			OutStr += gettext("Huntress");
		else if (random <= 10)
			OutStr += gettext("Amazon");
		else
			OutStr += gettext("Druidess"); 

		OutStr += gettext(" of the ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Hidden");
		else if (random <= 4)
			OutStr += gettext("Silent");
		else if (random <= 6)
			OutStr += gettext("Masters");
		else if (random <= 8)
			OutStr += gettext("Scarlet");
		else
			OutStr += gettext("Resolute");


		OutStr += gettext(" ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Hand");
		else if (random <= 4)
			OutStr += gettext("Dagger");
		else if (random <= 6)
			OutStr += gettext("Will");
		else if (random <= 8)
			OutStr += gettext("League");
		else
			OutStr += gettext("Hearts");

		OutStr += gettext(".");
		break;
	case 5:
		OutStr += gettext("the ");

		random = g_Dice%6 + 1;
		if (random <= 2)
			OutStr += gettext("high-ranking");
		else if (random <= 4)
			OutStr += gettext("mid-tier");
		else
			OutStr += gettext("low-ranking");

		OutStr += gettext(" ");

		random = g_Dice%14 + 1;
		if (random <= 2)
			OutStr += gettext("elf");
		else if (random <= 4)
			OutStr += gettext("woman");     // MYR: Human assumed
		else if (random <= 6)
			OutStr += gettext("dryad");
		else if (random <= 8)
			OutStr += gettext("succubus");
		else if (random <= 10)
			OutStr += gettext("nymph");
		else if (random <= 12)
			OutStr += gettext("eyrine"); 
		else
			OutStr += gettext("cat girl");

		OutStr += gettext(" from the ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("Nymphomania");
		else if (random <= 4)
			OutStr += gettext("Satyriasis");
		else if (random <= 6)
			OutStr += gettext("Women Who Love Sex");
		else if (random <= 8)
			OutStr += gettext("Real Women Don't Marry");
		else
			OutStr += gettext("Monster Sex is Best");

		OutStr += gettext(" ");

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("support group");
		else if (random <= 4)
			OutStr += gettext("league");
		else if (random <= 6)
			OutStr += gettext("club");
		else if (random <= 8)
			OutStr += gettext("faction");
		else
			OutStr += gettext("guild");

		OutStr += gettext(".");
		break;
	case 6: 
		OutStr += gettext("the "); 

		random = g_Dice%10 + 1;
		if (random <= 2)
			OutStr += gettext("disguised");
		else if (random <= 4)
			OutStr += gettext("hot");
		else if (random <= 6)
			OutStr += gettext("sexy");
		else if (random <= 8)
			OutStr += gettext("curvacious");
		else
			OutStr += gettext("sultry");

		OutStr += gettext(" ");

		// MYR: Covering the big fetishes/stereotpes
		random = g_Dice%12 + 1;
		if (random <= 2)
			OutStr += gettext("idol singer");
		else if (random <= 4)
			OutStr += gettext("princess");
		else if (random <= 6)
			OutStr += gettext("school girl");
		else if (random <= 8)
			OutStr += gettext("nurse");
		else if (random <= 10)
			OutStr += gettext("maid");
		else
			OutStr += gettext("waitress");

		OutStr += gettext(".");
		break;
	}
# pragma endregion les3
	OutStr += gettext("\n");
	return OutStr;
}

// MYR: Burned out before anal. Someone else feeling creative?

string cGirls::GetRandomAnalString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0;
	string OutStr;

	OutStr += gettext(" ");

	// Part 1
#pragma region anal1
	roll1 = g_Dice % 10 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal1

	OutStr += ". ";

	// Part 2
#pragma region anal2
	roll2 = g_Dice % 10 + 1;
	switch (roll2)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal2

	OutStr += " ";

	// Part 3
#pragma endregion anal3
	roll3 = g_Dice % 10 + 1;
	switch (roll3)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal3

	return OutStr;
}

// ----- Combat

Uint8 cGirls::girl_fights_girl(sGirl* a, sGirl* b)
{
	// NB: For girls exploring the catacombs, a is the character, b the monster
	// Return value of 1 means a (the girl) won
	//   "      "    " 2  "   b (the monster) won
	//   "      "    " 0  "   it was a draw

	CLog l;

	// MYR: Sanity checks on incorporeal. It is actually possible (but very rare) 
	//      for both girls to be incorporeal.
	if(a->has_trait("Incorporeal") && b->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: Both ") << a->m_Realname << gettext(" and ") << b->m_Realname 
			<< gettext(" are incorporeal, so the fight is a draw.\n");
		return 0;
	}
	else if(a->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: ") << a->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return 1;
	}
	else if(a->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: ") << b->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return 2;
	}

	int a_dodge = 0;
	int b_dodge = 0;
	u_int a_attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int b_attack = SKILL_COMBAT;

	if(a == 0 || b == 0)
		return 0;

	// first determine what skills they will fight with
	// girl a
	if(g_Girls.GetSkill(a, SKILL_COMBAT) >= g_Girls.GetSkill(a, SKILL_MAGIC))
		a_attack = SKILL_COMBAT;
	else
		a_attack = SKILL_MAGIC;

	// girl b
	if(g_Girls.GetSkill(b, SKILL_COMBAT) >= g_Girls.GetSkill(b, SKILL_MAGIC))
		b_attack = SKILL_COMBAT;
	else
		b_attack = SKILL_MAGIC;

	// determine dodge ability
	// girl a
	if((g_Girls.GetStat(a, STAT_AGILITY) - g_Girls.GetStat(a, STAT_TIREDNESS)) < 0)
		a_dodge = 0;
	else
		a_dodge = (g_Girls.GetStat(a, STAT_AGILITY) - g_Girls.GetStat(a, STAT_TIREDNESS));

	// girl b
	if((g_Girls.GetStat(b, STAT_AGILITY) - g_Girls.GetStat(b, STAT_TIREDNESS)) < 0)
		b_dodge = 0;
	else
		b_dodge = (g_Girls.GetStat(b, STAT_AGILITY) - g_Girls.GetStat(b, STAT_TIREDNESS));

	l.ss() << gettext("Girl vs. Girl: ") << a->m_Realname << gettext(" fights ") << b->m_Realname << gettext("\n");
	l.ss() << gettext("\t") << a->m_Realname << gettext(": Health ") << a->health() << gettext(", Dodge ") << a_dodge << gettext(", Mana ") << a->mana() << gettext("\n");
	l.ss() << gettext("\t") << b->m_Realname << gettext(": Health ") << b->health() << gettext(", Dodge ") << b_dodge << gettext(", Mana ") << b->mana() << gettext("\n");
	l.ssend();

	sGirl* Attacker = b;
	sGirl* Defender = a;
	unsigned int attack = 0;
	int dodge = a_dodge;
	int attack_count = 0;
	int winner = 0; // 1 for a, 2 for b
	while(1)
	{
		if(g_Girls.GetStat(a, STAT_HEALTH) <= 20)
		{
			g_Girls.UpdateEnjoyment(a, ACTION_COMBAT, -1, true);
			winner = 2;
			break;
		}
		else if(g_Girls.GetStat(b, STAT_HEALTH) <= 20)
		{
			g_Girls.UpdateEnjoyment(b, ACTION_COMBAT, -1, true);
			winner = 1;
			break;
		}

		if(attack_count > 1000)  // if the fight's not over after 1000 blows, call it a tie
		{
			l.ss()	<< gettext("The fight has gone on for too long, over 1000 (attempted) blows either way. Calling it a draw.");
			l.ssend();

			return 0;
		}
		attack_count++;

		if(Attacker == a)
		{
			Attacker = b;
			attack = b_attack;
			Defender = a;
			b_dodge = dodge;
			dodge = a_dodge;
		}
		else if(Attacker == b)
		{
			Attacker = a;
			attack = a_attack;
			Defender = b;
			a_dodge = dodge;
			dodge = b_dodge;
		}

		// Girl attacks
		l.ss() << gettext("\t\t") << Attacker->m_Realname << gettext(" attacks: ");

		if(attack == SKILL_MAGIC)
		{
			if(Attacker->mana() < 7)
				l.ss()	<< gettext("Insufficient mana: using combat");
			else
			{
				Attacker->mana(-7);
				l.ss()	<< gettext("Casts a spell (mana now ") << Attacker->mana() << gettext(")");
			}
		}
		else
			l.ss()	<< gettext("Using physical attack");

		l.ssend();

		int girl_attack_chance = g_Girls.GetSkill(Attacker, attack);
		int die_roll = g_Dice.d100();

		l.ss()	<< gettext("\t\t") << gettext("Attack chance: ") << girl_attack_chance << gettext(" Die roll: ") << die_roll;
		l.ssend();


		if(die_roll > girl_attack_chance)
			l.ss()	<< gettext("\t\t\t") << gettext("Miss!");
		else 
		{
			int damage = 5;
			l.ss()	<< gettext("\t\t\t") << gettext("Hit! base damage is ") << damage << gettext(". ");

	/*
	*		she has hit now calculate how much damage will be done
	*
	*		magic works better than conventional violence.
	*		Link this amount of mana remaining?
	*/
			int bonus = 0;
			if(attack == SKILL_MAGIC)
				bonus = g_Girls.GetSkill(Attacker, SKILL_MAGIC)/5 + 2;
			else 
				bonus = g_Girls.GetSkill(Attacker, SKILL_COMBAT)/10;

			l.ss()	<< gettext("Bonus damage is ") << bonus << gettext(".");
			l.ssend();
			damage += bonus;

			die_roll = g_Dice.d100();

			// Defender attempts Dodge
			l.ss()	<< gettext("\t\t\t\t") << Defender->m_Realname << gettext(" tries to dodge: needs ") << dodge << gettext(", gets ") << die_roll << gettext(": ");

			if(die_roll <= dodge)
			{
				l.ss()	<< gettext("Success!");
				l.ssend();
			}
			else
			{
				l.ss()	<< gettext("Failure! ");
				l.ssend();

				//int con_mod = Defender->m_Stats[STAT_CONSTITUTION] / 10;
				int con_mod = g_Girls.GetStat(Defender, STAT_CONSTITUTION)/10;
				int ActualDmg = damage - con_mod;
				if (ActualDmg <= 0)
					l.ss() << gettext("\t\t\t\t") << Defender->m_Realname << gettext(" shrugs it off.");
				else
				{
					g_Girls.UpdateStat(Defender,STAT_HEALTH,-ActualDmg);
					l.ss() << gettext("\t\t\t\t") << Defender->m_Realname << gettext(" takes ") << damage  << gettext(" damage, less ") << con_mod << gettext(" for CON\n"); 
					l.ss() << gettext("\t\t\t\t\t") << gettext("New health value = ") << Defender->health();
				}
			} // Didn't dodge
		}     // Hit
		l.ssend();

		// update girls dodge ability
		if((dodge - 1) < 0)
			dodge = 0;
		else
			dodge--;

		// Check if girl 2 (the Defender) has been defeated
		// Highest damage is 27 pts.  Checking for health between 20 and 30 is a bug as it can be stepped over.
		if(g_Girls.GetStat(Defender, STAT_HEALTH) <= 40 && g_Girls.GetStat(Defender, STAT_HEALTH) >= 10)
		{
			g_Girls.UpdateEnjoyment(Defender, ACTION_COMBAT, -1, true);
			break; 
		}  // if defeated
	}  // while (1)

	// Girls exploring catacombs: Girl is "a" - and thus wins
	if (Attacker == a)  
	{
		l.ss() << a->m_Realname << gettext(" WINS!");
		l.ssend();

		g_Girls.UpdateEnjoyment(a, ACTION_COMBAT, +1, true);

		return 1;
	}
	if (Attacker == b)  // Catacombs: Monster wins
	{
		l.ss()	<< b->m_Realname << gettext(" WINS!");
		l.ssend();

		g_Girls.UpdateEnjoyment(b, ACTION_COMBAT, +1, true);

		return 2;
	}

	l.ss()	<< gettext("ERROR: cGirls::girl_fights_girl - Shouldn't reach the function bottom.");
	l.ssend();

	return 0;
}

/*
 * passing the girl_wins boolean here to avoid confusion over
 * what returning TRUE actually means
 */

// MYR: How should incorporeal factor in to this?

void sGirl::fight_own_gang(bool &girl_wins)
{
	girl_wins = false;
	vector<sGang*> v = g_Gangs.gangs_on_mission(MISS_GUARDING);
/*
 *	we'll take goons from the top gang in the list
 */
	sGang *gang = v[0];
/*
 *	4 + 1 for each gang on guard duty
 *	that way there's a benefit to multiple gangs guarding
 */
	int max_goons = 4 + v.size();
/*
 *	to the maximum of the number in the gang
 */
	if(max_goons > gang->m_Num) {
		max_goons = gang->m_Num;
	}
/*
 *	now - sum the girl and gang stats
 *	we're not going to average the gangs.
 *	yes this gives them an unfair advantage
 *	that's the point of having 5:1 odds :)
 */
	int girl_stats = combat() + magic() + intelligence();
/*
 *	Now the gangs. I'm not factoring the girl's health
 *	because there's something dramatically satisfying
 *	about her breeaking out of the dungeon after being 
 *	tortured near unto death, and then still beating the
 *	thugs up. You'd buy into it in a Hollywood blockbuster...
 *
 *	Annnnyway....
 */
	int goon_stats = 0;
	for(int i = 0; i < max_goons; i++) {
		goon_stats +=	gang->combat() +
				gang->magic() +
				gang->intelligence()
		;
	}
/*
 *	the girl's base chance of winning is determined by the stat ratio
 */
	double odds = 1.0 * girl_stats / (goon_stats + girl_stats);
/*
 *	let's add some trait based bonuses
 *	I'm not going to do any that are already reflected in stat values
 *	(so no "Psychic" bonus, no "Tough" either)
 *	we can streamline this with the trait overhaul
 */
	if(has_trait("Clumsy"))		odds -= 0.05;
	if(has_trait("Broken Will"))	odds -= 0.10;
	if(has_trait("Meek"))		odds -= 0.05;
	if(has_trait("Dependant"))	odds -= 0.10;
	if(has_trait("Fearless"))	odds += 0.10;
	if(has_trait("Fleet of Foot"))	odds += 0.10;
/*
 *	get it back into the 0 <= N <= 1 range
 */
	if(odds < 0) odds = 0;
	if(odds > 1) odds = 1;
/*
 *	roll the dice! If it passes then the girl wins
 */
	if(g_Dice.percent(int(odds * 100))) {
		win_vs_own_gang(v, max_goons, girl_wins);
	}
	else {
		lose_vs_own_gang(v, max_goons,girl_stats,goon_stats,girl_wins);
	}
}

void sGirl::win_vs_own_gang(vector<sGang*> &v, int max_goons, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = true;
/*
 *	Give her some damage from the combat. She won, so don't kill her.
 *	should really modify this for traits. And agility.
 */
	int damage = g_Dice.random(10) * max_goons;
	health(-damage);
	if(health() == 0) health(1);
/*
 *	now - how many goons die today?
 *	I'm assuming the girl's priority is to escape
 *	rather than "clearing the level" so let's have a base of
 *	1-5
 */
	int casualties = g_Dice.in_range(1,6);
/*
 *	some more trait mods. Assasin adds an extra dice since
 *	she's been trained to deliver the coup-de-grace on a helpless
 *	opponent.
 */
	if(has_trait("Assassin")) {
		casualties += g_Dice.in_range(1,6);
	}
	if(has_trait("Adventurer")) {	// some level clearing instincts
		casualties += 2;
	}
	if(has_trait("Merciless")) casualties ++;
	if(has_trait("Yandere")) casualties ++;
	if(has_trait("Tsundere")) casualties ++;
	if(has_trait("Meek")) casualties --;
	if(has_trait("Dependant")) casualties -= 2;  //misspelled fixed by crazy
/*
 *	fleet of foot means she gets out by running away more than fighting
 *	so fewer fatalities
 */
	if(has_trait("Fleet of Foot")) casualties -= 2;
/*
 *	OK, apply the casualties and make sure it doesn't go negative
 */
	gang->m_Num -= casualties;
	if(gang->m_Num < 0) {
		gang->m_Num = 0;
		
	}
}

void sGirl::lose_vs_own_gang( vector<sGang*> &v, int max_goons, int girl_stats, int gang_stats, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = false;
/*
 *	She's going to get hurt some. Moderating this, we have the fact that
 *	the goons don't really want to damage her (lest the boss get all shouty)
 *	However, depending on how good she is, they may not have a choice
 *
 *	also, I don't want to kill her if she was a full health. I hate it when
 *	that happens. You can send a girl to the dungeons and she's dead before
 *	you've even had a chance to twirl your moustaches at her.
 *	So we need to know how much health she had in the first place
 */
	int start_health = health();
	int damage = g_Dice.random(12) * max_goons;
/*
 *	and if the gangs had the advantage, reduce the
 *	damage actually taken accordingly
 */
	if(gang_stats > girl_stats) {
		damage *= girl_stats;
		damage /= gang_stats;
	}
/*
 *	lastly, make sure this isn't going to kill her, if her health was
 *	> 90 in the first place
 */
	if(start_health >= 90 && damage >= start_health) {
		damage = start_health - 1;
	}
	health(-damage);
/*
 *	need a bit more detail here, really...
 */
	int casualties = g_Dice.in_range(1,6);
	casualties += 3;
	if(casualties < 0) casualties = 0;
	gang->m_Num -= casualties;
}

bool cGirls::GirlInjured(sGirl* girl, unsigned int unModifier)
{  // modifier: 5 = 5% chance, 10 = 10% chance
/*
 *	WD	Injury was only possible if girl is pregnant or
 *		hasn't got the required traits.
 *
 *		Now check for injury first
 *		Use usigned int so can't pass negative chance
 */
//	bool injured = false;
	string message;
	int nMod	= static_cast<int>(unModifier); 

	// Sanity check, Can't get injured
	if (girl->has_trait("Incorporeal"))
		return false;

	if (girl->has_trait("Fragile"))
		nMod += nMod;	// nMod *= 2;
	if (girl->has_trait("Tough"))
		nMod /= 2;

	// Did the girl get injured
	if (!g_Dice.percent(nMod))
		return false;
/*
 *	INJURY PROCESSING
 *	Only injured girls continue past here
 */

	// getting hurt badly could lead to scars
	if (
		g_Dice.percent(nMod*2) &&
		!girl->has_trait("Small Scars") &&
		!girl->has_trait("Cool Scars") &&
		!girl->has_trait("Horrific Scars")
	)
	{
		//injured = true;
		int chance = g_Dice%6;
		if (chance == 0)
		{
			girl->add_trait("Horrific Scars", false);
			message = gettext("She was badly injured, and now has to deal with Horrific Scars.");
		}
		else if (chance <= 2)
		{
			girl->add_trait("Small Scars", false);
			message = gettext("She was injured and now has a couple of Small Scars.");
		}
		else
		{
			girl->add_trait("Cool Scars", false);
			message = gettext("She was injured and scarred. As scars go however, at least they are pretty Cool Scars.");
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// in rare cases, she might even lose an eye
	if (
		g_Dice.percent((nMod/2)) &&
		!girl->has_trait("One Eye") &&
		!girl->has_trait("Eye Patch")
	)
	{
		//injured = true;
		int chance = g_Dice%3;
		if (chance == 0)
		{
			girl->add_trait("One Eye", false);
			message = gettext("Oh, no! She was badly injured, and now only has One Eye!");
		}
		else
		{
			girl->add_trait("Eye Patch", false);
			message = gettext("She was injured and lost an eye, but at least she has a cool Eye Patch to wear.");
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// or become fragile
	if (
		g_Dice.percent((nMod/2))
		&& !girl->has_trait("Fragile")
	)
	{
		//injured = true;
		girl->add_trait("Fragile", false);
		message = gettext("Her body has become rather Fragile due to the extent of her injuries.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// and if pregnant, she might lose the baby; I'll assume inseminations can't be aborted so easily
	if (girl->carrying_human() && g_Dice.percent((nMod*2)))
	{  // unintended abortion time
		//injured = true;
		girl->clear_pregnancy();
		girl->happiness(-20);
		girl->spirit(-5);
		message = gettext("Her unborn child has been lost due to the injuries she sustained, leaving her quite distraught.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// Lose between 5 - 14 hp
	girl->health(-5 - g_Dice%10);

	return true;
}

// MYR: Centralized the damage calculation and added in traits that should apply
//      This fn and TakeCombatDamage(..) are in the attack/defense/agility style of combat
//      and will probably be made redundanty by the new style

int cGirls::GetCombatDamage(sGirl *girl, int CombatType)
{
  int damage = 5;

  // Some traits help for both kinds of combat
  // There are a number of them so I set them at one point each
  // This also has the effect that some traits actually do something in the
  // game now

  if (girl->has_trait("Psychic"))
	  damage++;

  if (girl->has_trait("Adventurer"))
	  damage++;

  if (girl->has_trait("Aggressive"))
	  damage++;

  if (girl->has_trait("Fearless"))
	  damage++;

  if (girl->has_trait("Yandere"))
	  damage++;

  if (girl->has_trait("Merciless"))
	  damage++;

  if (girl->has_trait("Sadistic"))
	  damage++;

  if (girl->has_trait("Twisted"))
	  damage++;

  if(unsigned(CombatType) == SKILL_MAGIC)
  {
	damage += g_Girls.GetSkill(girl, SKILL_MAGIC)/5 + 2;

	// Depending on how you see magic, charisma can influence how it flows
	// (Think Dungeons and Dragons sorcerer)
	if (girl->has_trait("Charismatic"))
	  damage += 1;

	// Same idea as charismatic.
	// Note that I love using brainwashing oil, so this hurts me more than
	// it hurts you
	if (girl->has_trait("Iron Will"))
	  damage += 2;
	else if (girl->has_trait("Broken Will"))
	  damage -= 2;

	if (girl->has_trait("Strong Magic"))
	  damage += 2;

	// Can Mind Fucked people even work magic?
	if (girl->has_trait("Mind Fucked"))
	  damage -= 5;
  }
  else   // SKILL_COMBAT case
  {
	damage += g_Girls.GetSkill(girl, SKILL_COMBAT)/10;

	if (girl->has_trait("Manly"))
	  damage += 2;

	if (girl->has_trait("Strong"))
	  damage += 2;
  }

  return damage;
}

// MYR: Separated out taking combat damage from taking damage from other sources
//     Combat damage can be lowered a bit by certain traits, where other kinds of
//     damage cannot
//
// Returns the new health value

int cGirls::TakeCombatDamage(sGirl* girl, int amt)
{

	if(HasTrait(girl, "Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
		return 100;                         // MYR: Sanity is good. Moved to the top
	}
	if (amt == 0)
		return girl->m_Stats[STAT_HEALTH];

	int value;

	// This function works with negative numbers, but we'll be flexible and take
	// positive numbers as well
	if (amt > 0)
		value = amt * -1;
	else
		value = amt;

	// High con allows you to shrug off damage
	value = value + (GetStat(girl, STAT_CONSTITUTION) / 20);

	if(HasTrait(girl, "Fragile"))  // Takes more damage
		value -= 3;
	else if(HasTrait(girl, "Tough"))  // Takes less damage
		value += 2;

	if (girl->has_trait("Adventurer"))
		value++;

	if (girl->has_trait("Fleet of Foot"))
		value++;

	if (girl->has_trait("Optimist"))
		value++;
	else if (girl->has_trait("Pessimist"))
		value--; 

	if (girl->has_trait("Manly"))
		value++;

	if (girl->has_trait("Maschoist"))
		value += 2;

	if (value >= 0)  //  Can't heal when damage is dealed
		value = -1;  //  min 1 damage inflicted


	if(HasTrait(girl, "Construct"))
	{
		if(value < -4)
			value = -4;
	}

	girl->m_Stats[STAT_HEALTH] += value;

	if (girl->m_Stats[STAT_HEALTH] < 0)  // Consistency?
		girl->m_Stats[STAT_HEALTH] = 0;

	return girl->m_Stats[STAT_HEALTH];
}

// ----- Update

void cGirls::UpdateEnjoyment(sGirl* girl, int whatSheEnjoys, int amount, bool wrapTo100)
{
	girl->m_Enjoyment[whatSheEnjoys] += amount;
	if (wrapTo100 == true) {
		if (girl->m_Enjoyment[whatSheEnjoys] > 100) {
			girl->m_Enjoyment[whatSheEnjoys] = 100;
		}
		else if (girl->m_Enjoyment[whatSheEnjoys] < -100) {
			girl->m_Enjoyment[whatSheEnjoys] = -100;
		}
	}
}

void cGirls::updateGirlAge(sGirl* girl, bool inc_inService)
{
/*
 *	Increment birthday counter and update Girl's age if needed
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	if (inc_inService)
	{
		girl->m_WeeksPast++;
		girl->m_BDay++;
	}

	if(girl->m_BDay >= 52)					// Today is girl's birthday
	{
		girl->m_BDay = 0;
		girl->age(1);

		if(girl->age() > 20 && girl->has_trait("Lolita"))
			g_Girls.RemoveTrait(girl, "Lolita");
	}
}

void cGirls::updateSTD(sGirl* girl)
{
/*
 *	 Update health for any STD
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	// STDs
	if(girl->has_trait("AIDS"))
		girl->health(-15);
	if(girl->has_trait("Chlamydia"))
		girl->health(-2);
	if(girl->has_trait("Syphilis"))
		girl->health(-5);

	if(girl->health() <= 0)
	{
		string msg = girl->m_Realname + gettext(" has died from STD's.");
		girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
		//g_MessageQue.AddToQue(msg, 1);
	}
}

void cGirls::updateGirlTurnStats(sGirl* girl)
{
/*
 *	Stat update code that is to be run every turn
 */

	// Sanity check. Abort on dead girl
	if(girl->health() <= 0)
	{
		return;
	}

	// TIREDNESS Really tired girls get unhappy fast
	int bonus = girl->tiredness() - 90;
	if(bonus > 0)
	{
		girl->obedience(-1);				// Base loss for being tired
		girl->pclove(-1);

		bonus	= bonus / 3 + 1;			// bonus vs tiredness values 1: 91-92, 2: 93-95, 3: 96-98, 4: 99-100
		girl->happiness(-bonus);

		bonus	= bonus / 2 + 1;			// bonus vs tiredness values 1: 91-92, 2: 93-98, 3: 99-100
		if (girl->health() - bonus < 10)	// Don't kill the girl from tiredness
		{
			girl->health(10);				// Girl will hate player more if badly hurt from being tired
			girl->pclove(-1);				
			girl->pchate(1);
		}
		else
		{
			girl->health(-bonus);			// Really tired girls lose more health
		}

/*		These messages duplicate warning messages in the matron code
 *
 *		msg = girlName + " is so tired her health has been affected.";
 *		girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_WARNING);
 */
	}


	// HEALTH hurt girls get tired fast
	bonus = 40 - girl->health();
	if(bonus > 0)
	{
		girl->pchate(1);					// Base loss for being hurt
		girl->pclove(-1);
		girl->happiness(-1);

		bonus	= bonus / 8 + 1;			// bonus vs health values 1: 33-39, 2: 25-32, 3: 17-24, 4: 09-16 5: 01-08
		girl->tiredness(bonus);

		bonus	= bonus / 2 + 1;			// bonus vs health values 1: 33-39, 2: 17-32, 3: 01-16
		if (girl->health() - bonus < 1)		// Don't kill the girl from low health
		{
			girl->health(1);				// Girl will hate player more for very low health
			girl->pclove(-1);
			girl->pchate(1);
		}
		else
		{
			girl->health(-bonus);
		}

/*		These messages duplicate warning messages in the matron code
 *
 *		msg = "DANGER " + girlName + " health is low!";
 *		girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_DANGER);
 */
	}		


	// LOVE love is updated only if happiness is >= 100 or < 50
	if(girl->happiness() >= 100)
	{
		girl->pclove(2);					// Happy girls love player more
	}
	if (!girl->is_slave() && girl->happiness() < 50)
	{
		girl->pclove(-2);					// Unhappy FREE girls love player less	
	}

}


// ----- Stream operators

/*
 * While I'm here, let's defined the stream operator for the girls,
 * random and otherwise.
 *
 * This looks big and complex, but it's just printing out the random
 * girl data from the load. It's so I can say cout << *rgirl << endl;
 * and get a something sensible printed
 */

ostream& operator<<(ostream &os, sRandomGirl &g)
{
	os << g.m_Name << endl;
	os << g.m_Desc << endl;
	os << gettext("Human? ") << (g.m_Human == 0 ? gettext("Yes") : gettext("No")) << endl;
	os << gettext("Catacomb Dweller? ")
	   << (g.m_Catacomb == 0 ? gettext("No") : gettext("Yes"))
	   << endl;
	os << gettext("Money: Min = ") << g.m_MinMoney << gettext(". Max = ") << g.m_MaxMoney << endl;
/*
 *	loop through stats
 *	setw sets a field width for the next operation,
 *	left forces left alignment. Makes the columns line up.
 */
	for(unsigned int i = 0; i < sGirl::max_stats; i++) 
	{
		os << setw(14) << left << sGirl::stat_names[i]
		   << gettext(": Min = ") << int(g.m_MinStats[i])
		   << endl
		;
		os << setw(14) << ""
		   << gettext(": Max = ") << int(g.m_MaxStats[i])
		   << endl
		;
	}
/*
 *	loop through skills
 */
	for(unsigned int i = 0; i < sGirl::max_skills; i++) 
	{
		os << setw(14) << left << sGirl::skill_names[i]
		   << gettext(": Min = ") << int(g.m_MinSkills[i])
		   << endl
		;
		os << setw(14) << ""
		   << gettext(": Max = ") << int(g.m_MaxSkills[i])
		   << endl
		;
	}
/*
 *	loop through traits
 */
	for(int i = 0; i < g.m_NumTraits; i++) 
	{
		string name = g.m_Traits[i]->m_Name;
		int percent = int(g.m_TraitChance[i]);
		os << gettext("Trait: ")
		   << setw(14) << left << name
		   << ": " << percent
		   << gettext("%")
		   << endl
		;
	}
/*
 *	important to return the stream, so the next 
 *	thing in the << chain has something on which to operate 
 */
	return os;
}

/*
 * another stream operator, this time for sGirl
 */

ostream& operator<<(ostream& os, sGirl &g)
{
	os << g.m_Realname << endl;
	os << g.m_Desc << endl;
	os << endl;

	for(int i = 0; i < NUM_STATS; i++) {
		os.width(20);
		os.flags(ios::left);
		os << g.stat_names[i] 
		   << gettext("\t: ")
		   << int(g.m_Stats[i])
		   << endl
		;
	}
	os << endl;

	for(u_int i = 0; i < NUM_SKILLS; i++) {
		os.width(20);
		os.flags(ios::left);
		os << g.skill_names[i] 
		   << gettext("\t: ")
		   << int(g.m_Skills[i])
		   << endl
		;
	}
	os << endl;

	os << endl;
	return os;
}

// ----- Pregnancy, insemination & children

static bool has_contraception(sGirl *girl)
{
/*
 *	if contraception is TRUE, then she can't get pregnant
 *	Which makes sense
 *
 *	Two ways to get contraception: one of them is 
 *	if the brothel has a an anti-preg potion
 *	(and if she's willing to drink it!)
 *
 *	this tests both
 */
	if(g_Building == BUILDING_STUDIO && (g_Studios.UseAntiPreg(girl->m_UseAntiPreg)))
		{
		g_Building = BUILDING_BROTHEL;
		return true;
		}
	else if (g_Brothels.UseAntiPreg(girl->m_UseAntiPreg)) {
		return true;
	}
	g_Building = BUILDING_BROTHEL;
/*
 *	otherwise, sterile babes should be safe as well
 */
	if(girl->has_trait("Sterile")) {
		return true;
	}
/*
 *	Actually, I lied. If she's in her cooldown period after
 *	giving birth, that is effective contraception as well
 */
	if(girl->m_PregCooldown > 0) {
		return true;
	}
/*
 *	Oh, and if she's pregnant, she shouldn't get pregnant
 *	So I guess that's four
 */
	if(girl->is_pregnant()) {
		return true;
	}
	return false;
}

bool sGirl::calc_pregnancy(int chance, cPlayer *player)
{
	return g_GirlsPtr->CalcPregnancy(
		this,
		chance,
		STATUS_PREGNANT_BY_PLAYER,
		player->m_Stats,
		player->m_Skills
	);
}

sChild::sChild(bool is_players, Gender gender)
{
	m_Unborn	= 1;
	m_Age		= 0;
	m_IsPlayers	= is_players;
	m_Sex		= gender;
	m_Next		= m_Prev = 0;
	if(gender != None) {
		return;
	}
	m_Sex = (g_Dice.is_girl() ? Girl : Boy);
}

void cChildList::add_child(sChild * child)
{
	m_NumChildren++;
	if(m_LastChild)
	{
		m_LastChild->m_Next=child;
		child->m_Prev=m_LastChild;
		m_LastChild=child;
	}
	else
	{
		m_LastChild=child;
		m_FirstChild=child;
	}

}

sChild * cChildList::remove_child(sChild * child,sGirl * girl)
{
	m_NumChildren--;
	sChild* temp = child->m_Next;
	if(child->m_Next)
		child->m_Next->m_Prev = child->m_Prev;
	if(child->m_Prev)
		child->m_Prev->m_Next = child->m_Next;
	if(child == girl->m_Children.m_FirstChild)
		girl->m_Children.m_FirstChild = child->m_Next;
	if(child == girl->m_Children.m_LastChild)
		girl->m_Children.m_LastChild = child->m_Prev;
	child->m_Next = 0;
	delete child;
	return temp;
}

int sGirl::preg_chance(int base_pc, bool good, double factor)
{
	cConfig cfg;	
	g_LogFile.ss() << "sGirl::preg_chance: " << "base %chance = " << base_pc << ", " << "good flag = " << good << ", "
		<< "factor = " << factor;
	g_LogFile.ssend();
	
	double chance = base_pc;
/*
 *	factor is used to pass situational modifiers.
 *	For instance BDSM has a 25% reduction in chance
 */
	chance *= factor;
/*
 *	if the sex was good, the chance is modded, again from the
 *	config file
 */
	if(good) {
		chance *= cfg.pregnancy.good_sex_factor();
	}
	return int(chance);
}

bool sGirl::calc_pregnancy(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(
		cfg.pregnancy.player_chance(), good, factor
	);
/*
 *	now do the calculation
 */
	return g_GirlsPtr->CalcPregnancy(
		this, int(chance), STATUS_PREGNANT_BY_PLAYER,
		player->m_Stats,
		player->m_Skills
	);
}

bool sGirl::calc_group_pregnancy(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(
		cfg.pregnancy.player_chance(), good, factor
	);
/*
 *	player has 25% chance to be father (4 men in the group)
*/
	int father = STATUS_PREGNANT;
	if(g_Dice.percent(75)) {
		father = STATUS_PREGNANT_BY_PLAYER;
	}
/*
 *	now do the calculation
 */
	return g_GirlsPtr->CalcPregnancy(
		this, int(chance), father,
		player->m_Stats,
		player->m_Skills
	);
}

bool sGirl::calc_pregnancy(sCustomer *cust, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(
		cfg.pregnancy.customer_chance(), good, factor
	);
/*
 *	now do the calculation
 */
	return g_GirlsPtr->CalcPregnancy(
		this,
		int(chance),
		STATUS_PREGNANT,
		cust->m_Stats,
		cust->m_Skills
	);
}

bool sGirl::calc_insemination(sCustomer *cust, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(
		cfg.pregnancy.monster_chance(), good, factor
	);
/*
 *	now do the calculation
 */
	return g_GirlsPtr->CalcPregnancy(
		this,
		int(chance),
		STATUS_INSEMINATED,
		cust->m_Stats,
		cust->m_Skills
	);
}

bool sGirl::calc_insemination(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(
		cfg.pregnancy.monster_chance(), good, factor
	);
/*
 *	now do the calculation
 */
	return g_GirlsPtr->CalcPregnancy(
		this,
		int(chance),
		STATUS_INSEMINATED,
		player->m_Stats,
		player->m_Skills
	);
}

bool cGirls::CalcPregnancy(sGirl* girl, int chance, int type, unsigned char stats[NUM_STATS], unsigned char skills[NUM_SKILLS])
{
	string text=gettext("She has");
/*
 *	for reasons I do not understand, but nevertheless think
 *	are kind of cool, virgins have a +10 to their pregnancy
 *	chance
 */
	if(girl->m_Virgin && chance > 0) {
		chance += 10;
	}
/*
 *	If there's a condition that would stop her getting preggers
 *	then we get to go home early
 *
 *	return TRUE to indicate that pregnancy is FALSE
 *	(actually, supposed to mean that contraception is true,
 *	but it also applies for things like being pregnant,
 *	or just blowing the dice roll. That gets confusing too.
 */
	if(has_contraception(girl)) {
		return true;
	}
/*
 *	the other effective form of contraception, of course,
 *	is failing the dice roll. Let's check the chance of
 *	her NOT getting preggers here
 */
	if(g_Dice.percent(100 - chance)) {
		return true;
	}
/*
 *	set the pregnant status
 */
	girl->m_States |= (1<<type);
/*
 *	narrative depends on what it was that Did The Deed
 *	specifically, was it human or not?
 */

	switch (type)
	{
		case STATUS_INSEMINATED:
			text+=gettext(" been inseminated.");
			break;

		case STATUS_PREGNANT:
			text+=gettext(" gotten pregnant.");
			break;

		case STATUS_PREGNANT_BY_PLAYER:
			text+=gettext(" gotten pregnant with you.");
			break;

	}
	girl->m_Events.AddMessage(text, IMGTYPE_PREGNANT, EVENT_DANGER);

	sChild* child = new sChild(unsigned(type) == STATUS_PREGNANT_BY_PLAYER);

	for(int i=0; i<NUM_STATS; i++)
		child->m_Stats[i] = stats[i];
	for(u_int i=0; i<NUM_SKILLS; i++)
		child->m_Skills[i] = skills[i];

	// if there is somehow leftover pregnancy data, clear it
	girl->m_WeeksPreg = 0;
	sChild* leftover = girl->m_Children.m_FirstChild;
	while(leftover)
	{
		leftover = girl->next_child(leftover, (leftover->m_Unborn > 0));
	}

	girl->m_Children.add_child(child);
	return false;
}

int cGirls::calc_abnormal_pc(sGirl *mom, sGirl *sprog, bool is_players)
{
/*
 *	the non-pc-daughter case is simpler
 */
	if(is_players == false) {
/*
 *		if the mom is your daughter then any customer is
 *		a safe dad - genetically speaking, anyway
 */
		if(mom->has_trait("Your Daughter")) {
			return 0;
		}
/*
 *		so what are the odds that this customer 
 *		fathered both mom and sprog. Let's say 2%
 */
		if(g_Dice.percent(98)) {
			return 0;
		}
/*
 *		that's enough to give the sprog the incest trait
 *		but there's only a risk of abnormality if
 *		mom is herself incestuous
 */
		sprog->add_trait("Incest", false);
		if(mom->has_trait("Incest") == false) {
			return 0;
		}
/*
 *		If we get past all that lot, there's
 *		a 5% chance of abnormality
 */
		return 5;
	}
 /*
  *	OK. The sprog is the player's get
  */
	sprog->add_trait("Your Daughter", false);
/*
 *	if mom isn't the player's then there is no problem
 */
	if(mom->has_trait("Your Daughter") == false) {
		return 0;
	}
/*
 *	she IS, so we add the incest trait
 */
	sprog->add_trait("Incest", false);
/*
 *	if mom is also incestuous, that adds 5% to the odds
 */
	if(mom->has_trait("Incest")) {
		return 10;
	}
	return 5;
}

bool cGirls::child_is_grown(sGirl* mom, sChild *child, string& summary, bool PlayerControlled)
{
	cConfig cfg;
	cTariff tariff;
	stringstream ss;
/*
 *	bump the age - if it's still not grown, go home
 */
	child->m_Age++;
	if(child->m_Age < cfg.pregnancy.weeks_till_grown()) {
		return false;
	}
/*
 *	we need a coming of age ceremony
 *
 *	for boys, slap 'em in irons and sell 'em into slavery
 */
	if(child->is_boy()) {
		summary += gettext("A son grew of age. ");
/*
 *		get the going rate for a male slave
 *		and sell the poor sod
 */
		mom->m_States |= (1<<STATUS_HAS_SON);

		if(PlayerControlled)
		{
			int gold = tariff.male_slave_sales();
			g_Gold.slave_sales(gold);
/*
 *			format a message
 */
			ss << gettext("Her son has grown of age and has been sold into slavery.\n");
			ss << gettext("You make ") << gold << gettext(" gold selling the boy.\n");
/*
 *			and tell the player
 */
			mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
		}
		return true;
	}

	mom->m_States |= (1<<STATUS_HAS_DAUGHTER);
	bool slave = mom->is_slave();
	bool AllowNonHuman = mom->has_trait("Not Human");
/*
 *	create a new girl for the bairn
 *	
 *	WD: Bugfix as reported by chronos 
 *		http://pinkpetal.co.cc/index.php?topic=416.msg11968#msg11968
 */
	//sGirl* sprog = g_Girls.CreateRandomGirl(12, false, "", slave,AllowNonHuman);
	sGirl* sprog = g_Girls.CreateRandomGirl(12, false, slave, false, AllowNonHuman);
/*
 *	check for incest, get the odds on abnormality
 */
	int abnormal_pc = calc_abnormal_pc(mom, sprog, (child->m_IsPlayers !=0));
	if(g_Dice.percent(abnormal_pc)) {
		if(g_Dice.percent(50)) {
			g_Girls.AddTrait(sprog, "Malformed");
		}
		else {
			g_Girls.AddTrait(sprog, "Retarded");
		}
	}
/*
 *	loop throught the mom's traits, inheriting where appropriate
 */
// 	for(int i=0; i<30; i++)		// WD: wrong loop conditions might be reason for excessive traits on kids
	for(int i=0; i<MAXNUM_TRAITS; i++)
	{
		if(g_Girls.InheritTrait(mom->m_Traits[i]))
			g_Girls.AddTrait(sprog, mom->m_Traits[i]->m_Name);
	}

	// inherit stats
	for(int i=0; i<NUM_STATS; i++)
	{
		int min, max;
		if(mom->m_Stats[i] < child->m_Stats[i])
		{
			min = mom->m_Stats[i];
			max = child->m_Stats[i];
		}
		else {
			max = mom->m_Stats[i];
			min = child->m_Stats[i];
		}
		sprog->m_Stats[i] = (g_Dice%(max-min))+min;
	}

	//set age to 17 fix health
	sprog->m_Stats[STAT_AGE]=18;
	sprog->m_Stats[STAT_HEALTH]=100;
	sprog->m_Stats[STAT_LEVEL]=0;
	sprog->m_Stats[STAT_EXP]=0;

	// inherit skills
	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		int max;
		if(mom->m_Skills[i] < child->m_Skills[i])
		{
			max = child->m_Skills[i];
			if(max > 20)
				max = 20;
			sprog->m_Skills[i] = g_Dice%max;
		}
		else {
			max = mom->m_Skills[i];
			if(max > 20)
				max = 20;
			sprog->m_Skills[i] = g_Dice%max;
		}
	}

	// make sure slave daughters have house perc. set to 100, otherwise 60
	if(slave)
		sprog->m_Stats[STAT_HOUSE]=cfg.initial.slave_house_perc();
	else
		sprog->m_Stats[STAT_HOUSE]=60;

	if(PlayerControlled)
	{
		summary += gettext("A daughter grew of age. ");
		mom->m_Events.AddMessage(
			gettext("Her daughter has grown of age and has been placed in your dungeon."),
			IMGTYPE_PROFILE, EVENT_DANGER
		);
		g_Brothels.GetDungeon()->AddGirl(sprog, 13);
	}
	else
		g_Girls.AddGirl(sprog);
	return true;
}

void cGirls::UncontrolledPregnancies()
{
	sGirl* current = m_Parent;
	string summary;
	while(current)
	{
		HandleChildren(current, summary, false);
		current = current->m_Next;
	}
}

void cGirls::HandleChildren(sGirl* girl, string& summary, bool PlayerControlled)
{
	sChild* child;
	girl->m_JustGaveBirth = false;
/*
 *	start by advancing pregnancy cooldown time
 */
	if(girl->m_PregCooldown > 0) {
		girl->m_PregCooldown--;
	}
/*
 *	now: if the girl has no children
 *	we have nothing to do
 *
 *	logically this can precede the cooldown bump
 *	since if she's on cooldown she must have 
 *	given birth
 *
 *	but I guess this way offers better bugproofing
 */
	if(girl->m_Children.m_FirstChild == 0) {
		return;
	}
/*
 *	loop through the girl's children,
 *	and divide them into those growing up
 *	and those still to be born
 */
	bool remove_flag;
	child = girl->m_Children.m_FirstChild; 
	while(child) {
/*
 *		if the child is yet unborn
 *		see if it is due
 */
		if(child->m_Unborn) {
/*
 *			some births (monsters) we do not track to adulthood 
 *			these need removing from the list
 */
			remove_flag = child_is_due(girl, child, summary, PlayerControlled);
		}
		else {
/*
 *			the child has been born already
 *
 *			if it comes of age we remove it from the list
 */
			remove_flag = child_is_grown(girl, child, summary, PlayerControlled);
		}
		child = girl->next_child(child, remove_flag);
	}
}

bool cGirls::child_is_due(sGirl* girl, sChild *child, string& summary, bool PlayerControlled)
{
	cConfig cfg;
	cTariff tariff;
	stringstream ss;
/*
 *	clock on the count and see if she's due
 *	if not, return false (meaning "do not remove this child yet)
 */
	girl->m_WeeksPreg++;
	if(girl->m_WeeksPreg < cfg.pregnancy.weeks_pregnant()) {
		return false;
	}
/*
 *	OK, it's time to give birth
 *	start with some basic bookkeeping.
 */
	girl->m_WeeksPreg = 0;
	child->m_Unborn = 0;
	girl->m_PregCooldown = cfg.pregnancy.cool_down();

	//ADB low health is risky for pregnancy!
	//80 health will add 2 to percent chance of sterility and death, 10 health will add 9 percent!
	int healthFactor = (100 - g_Girls.GetStat(girl, STAT_HEALTH)) / 10;
/*
 *	the human-baby case is marginally easier than the
 *	tentacle-beast-monstrosity one, so we do that first
 */
	if(girl->carrying_human()) {
/*
 *		first things first - clear the pregnancy bit
 *		this is a human birth, so add the MILF trait
 */
		girl->clear_pregnancy();
		g_Girls.AddTrait(girl, "MILF");
/*
 *		format a message
 */
		summary += "Gave birth. ";
		ss << gettext("She has given birth to a baby ")
		   << child->boy_girl_str()
		   << gettext(". You grant her the week off for maternity leave.")
		;
/*
 *		check for sterility
 */
		if(g_Dice.percent(1+healthFactor)) 
		{
			ss << gettext(" It was a difficult birth and she has lost the ability to have children.");
			g_Girls.AddTrait(girl, "Sterile");
		}
/*
 *		queue the message and return false because we need to
 *		see this one grow up
 */
		if(PlayerControlled)
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
		girl->m_JustGaveBirth = true;
		return false;
	}
/*
 *	It's monster time!
 *
 *	How much does one of these suckers bring on the open market
 *	anyway?
 *
 *	might as well record the transaction and clear the preggo bit
 *	while we're at it
 */
	if(PlayerControlled)
	{
		long gold = tariff.creature_sales();
		g_Gold.creature_sales(gold);

		summary += gettext("Gave birth to a beast. ");
		ss << gettext("The creature within her has matured and emerged from her womb.\n");
		ss << gettext("You make ") << gold << gettext(" gold selling the creature.");
		ss << gettext(" You grant her the week off for her body to recover.");
	}
	girl->clear_pregnancy();
/*
 *	check for death
 */
	if(g_Dice.percent(1+healthFactor))
	{
		summary += gettext("And died from it. ");
		ss << gettext("\nSadly, the girl did not survive the experience.");
		girl->m_Stats[STAT_HEALTH] = 0;
	}
/*
 *	and sterility
 *	slight mod: 1% death, 5% sterility for monster births
 *	as opposed to other way around. Seems better this way.
 */
	else if(g_Dice.percent(5+healthFactor))
	{
		ss << gettext(" It was a difficult birth and she has lost the ability to have children.");
		g_Girls.AddTrait(girl, "Sterile");
	}
/*
 *	queue the message and return TRUE
 *	because we're not interested in watching
 *	little tentacles grow to adulthood
 */
	if(PlayerControlled)
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
	girl->m_JustGaveBirth = true;
	return true;
}

bool cGirls::InheritTrait(sTrait* trait)
{

	/*
	 * WD: tidy up and add default chance for unknown traits to inherit
	 *
	 */

	if(trait)
	{
		if( strcmp(trait->m_Name, "Fragile") == 0		|| 
			strcmp(trait->m_Name, "Fleet of Foot") == 0 || 
			strcmp(trait->m_Name, "Clumsy") == 0		|| 
			strcmp(trait->m_Name, "Strong") == 0		|| 
			strcmp(trait->m_Name, "Psychic") == 0		|| 
			strcmp(trait->m_Name, "Strong Magic") == 0
			)
		{
			if(g_Dice.percent(30))
				return true;
		}

		if(	strcmp(trait->m_Name, "Tough") == 0			|| 
			strcmp(trait->m_Name, "Fast orgasms") == 0	|| 
			strcmp(trait->m_Name, "Slow orgasms") == 0	|| 
			strcmp(trait->m_Name, "Quick Learner") == 0 || 
			strcmp(trait->m_Name, "Slow Learner") == 0
			)	
	{
			if(g_Dice.percent(50))
			return true;
	}
	
		if(	strcmp(trait->m_Name, "Perky Nipples") == 0 || 
			strcmp(trait->m_Name, "Puffy Nipples") == 0 || 
			strcmp(trait->m_Name, "Long Legs") == 0		|| 
			strcmp(trait->m_Name, "Big Boobs") == 0		|| 
			strcmp(trait->m_Name, "Abnormally Large Boobs") == 0 || 
			strcmp(trait->m_Name, "Small Boobs") == 0	|| 
			strcmp(trait->m_Name, "Great Arse") == 0	|| 
			strcmp(trait->m_Name, "Great Figure") == 0	|| 
			strcmp(trait->m_Name, "Cute") == 0
			)
		{
			if(g_Dice.percent(70))	
				return true;
		}

		if(	strcmp(trait->m_Name, "Demon") == 0			|| 
			strcmp(trait->m_Name, "Cat Girl") == 0		|| 
			strcmp(trait->m_Name, "Not Human") == 0		|| 
			strcmp(trait->m_Name, "Shroud Addict") == 0 || 
			strcmp(trait->m_Name, "Fairy Dust Addict") == 0 || 
			strcmp(trait->m_Name, "Viras Blood Addict") == 0
			)	
			return true;

		if(	strcmp(trait->m_Name, "Nymphomaniac") == 0)
		{
			if(g_Dice.percent(60))
				return true;
		}

		if(	strcmp(trait->m_Name, "Strange Eyes") == 0 ||
			strcmp(trait->m_Name, "Different Colored Eyes") == 0
			)
		{
			if(g_Dice.percent(70))
				return true;
		}

		// WD: traits that can't be inherited or are a special case
		if(	strcmp(trait->m_Name, "Construct") == 0		||
			strcmp(trait->m_Name, "Half-Construct") == 0 ||
			strcmp(trait->m_Name, "Cool Scars") == 0	||
			strcmp(trait->m_Name, "Small Scars") == 0	||
			strcmp(trait->m_Name, "Horrific Scars") == 0 ||
			strcmp(trait->m_Name, "MILF") == 0			||
			strcmp(trait->m_Name, "Your Daughter") == 0	||
			strcmp(trait->m_Name, "Incest") == 0		||
			strcmp(trait->m_Name, "One Eye") == 0		||
			strcmp(trait->m_Name, "Eye Patch") == 0		||
			strcmp(trait->m_Name, "Malformed") == 0		||
			strcmp(trait->m_Name, "Retarded") == 0		||
			strcmp(trait->m_Name, "AIDS") == 0			||
			strcmp(trait->m_Name, "Malformed") == 0		||
			strcmp(trait->m_Name, "Chlamydia") == 0		||
			strcmp(trait->m_Name, "Syphilis") == 0		||
			strcmp(trait->m_Name, "Assassin") == 0		||
			strcmp(trait->m_Name, "Adventurer") == 0	||
			strcmp(trait->m_Name, "Mind Fucked") == 0	||
			strcmp(trait->m_Name, "Broken Will") == 0	||
			strcmp(trait->m_Name, "Sterile") == 0	
			)
			return false;

		// WD: Any unlisted traits here
		if(g_Dice.percent(30))
			return true;
	}	
	return false;
}

#if 0
void cGirls::HandleChildren(sGirl* girl, string summary)
{
	cConfig cfg;
	if(girl->m_PregCooldown > 0)
		girl->m_PregCooldown--;
	if(girl->m_Children.m_FirstChild)
	{
		sChild* child = girl->m_Children.m_FirstChild;
		while(child)
		{
			if(child->m_Unborn == 0)
			else	// handle the pregnancy
			{
				girl->m_WeeksPreg++;
				if(girl->m_WeeksPreg >= cfg.pregnancy.weeks_pregnant())
				{
					girl->m_PregCooldown = cfg.pregnancy.cool_down();
					if(girl->m_States & (1<<STATUS_INSEMINATED))
						g_Girls.AddTrait(girl, "MILF");
					string message = "";
					if(girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
					{
						summary += gettext("Gave birth. ");
						message = gettext("She has given birth to a baby ");

						if(child->m_Sex == 1)
							message += gettext("boy.");
						else
							message += gettext("girl.");
						girl->m_States &= ~(1<<STATUS_PREGNANT);
						girl->m_States &= ~(1<<STATUS_PREGNANT_BY_PLAYER);
						if(g_Dice%100 == 0)
						{
							message += gettext(" It was a difficult birth and she has lost the ability to have children.");
							g_Girls.AddTrait(girl, "Sterile");
						}
					}
					else if(girl->m_States & (1<<STATUS_INSEMINATED))
					{
						summary += gettext("Gave birth to beast. ");
						message = gettext("The creature within her has matured and emerged from her womb.\n");
						message += gettext("You make ");
						long gold = (g_Dice%2000)+100;
						_ltoa(gold,buffer,10);
						message += buffer;
						message += gettext(" gold selling the creature.\n");
						g_Gold.creature_sales(gold);

						int death = g_Dice%101;
						if(death < 5)
						{
							summary += gettext("And died from it. ");
							message += gettext("Sadly the girl did not survive the experiance.";
							girl->m_Stats[STAT_HEALTH] = 0;
						}

						girl->m_States &= ~(1<<STATUS_INSEMINATED);
						if(g_Dice%100 == 0)
						{
							message += gettext(" It was a difficult birth and she has lost the ability to have children.");
							g_Girls.AddTrait(girl, "Sterile");
						}

						girl->m_Events.AddMessage(message, IMGTYPE_PREGNANT, 3);
						girl->m_WeeksPreg = 0;
						child->m_Unborn = 0;

						// remove from the list
						//child = girl->m_Children.remove_child(child,girl);
						continue;
					}

					girl->m_Events.AddMessage(message, IMGTYPE_PREGNANT, 3);
					girl->m_WeeksPreg = 0;
					child->m_Unborn = 0;
				}
			}
			child = child->m_Next;
		}
	}
}
#endif

// ----- Image

void cGirls::LoadGirlImages(sGirl* girl)
{
	girl->m_GirlImages = g_Girls.GetImgManager()->LoadList(girl->m_Name);
}

bool cImageList::AddImage(string filename, string path, string file)
{
	//ifstream in;
	//in.open(filename.c_str());
	//if(!in)
	//{
	//	in.close();
	//	return false;
	//}
	//in.close();

	// create image item
	cImage* newImage = new cImage();

	if(filename[filename.size()-1] == 'i')
	{
		string name = path;
		name += "\\ani\\";
		name += file;
		name.erase(name.size()-4, 4);
		name += ".jpg";
		newImage->m_Surface = new CSurface();
		newImage->m_Surface->LoadImage(name);
		newImage->m_AniSurface = new cAnimatedSurface();
		int numFrames, speed, aniwidth, aniheight;
		ifstream input;
		input.open(filename.c_str());
		if(!input)
		{
			CLog l;
			l.ss() << "Incorrect data file given for animation - " << filename;
			l.ssend();
			return false;
		}
		else
			input>>numFrames>>speed>>aniwidth>>aniheight;
		newImage->m_AniSurface->SetData(0,0,numFrames, speed, aniwidth, aniheight, newImage->m_Surface);
		input.close();
		//newImage->m_Surface->FreeResources();  //this was causing lockup in CResourceManager::CullOld
	}
	else
		newImage->m_Surface = new CSurface(filename);

	// Store image item
	if(m_Images)
	{
		m_LastImages->m_Next = newImage;
		m_LastImages = newImage;
	}
	else
		m_LastImages = m_Images = newImage;

	return true;
}

CSurface* cImageList::GetImageSurface(bool random, int& img)
{
	int count = 0;
	int ImageNum = -1;

	if(!random)
	{
		if(img == -1)
			return 0;

		ImageNum = img;
		cImage* current = m_Images;
		while(current)
		{
			if(count == ImageNum)
				break;
			count++;
			current = current->m_Next;
		}

		if(current)
		{
			img = ImageNum;
			return current->m_Surface;
		}
	}
	else
	{
		if(m_NumImages == 0)
			return 0;
		else if(m_NumImages == 1)
		{
			img = 0;
			return m_Images->m_Surface;
		}
		else
		{
			ImageNum = g_Dice%m_NumImages;
			cImage* current = m_Images;
			while(current)
			{
				if(count == ImageNum)
					break;
				count++;
				current = current->m_Next;
			}

			if(current)
			{
				img = ImageNum;
				return current->m_Surface;
			}
			else
			{
				img = ImageNum;
				return 0;
			}
		}
	}

	img = ImageNum;
	return 0;
}

cAnimatedSurface* cImageList::GetAnimatedSurface(int& img)
{
	int count = 0;
	int ImageNum = -1;

	if(img == -1)
		return 0;

	ImageNum = img;
	cImage* current = m_Images;
	while(current)
	{
		if(count == ImageNum)
			break;
		count++;
		current = current->m_Next;
	}

	if(current)
	{
		img = ImageNum;
		if(current->m_AniSurface)
			return current->m_AniSurface;
		else
			return 0;
	}

	return 0;
}

bool cImageList::IsAnimatedSurface(int& img)
{
	int count = 0;
	int ImageNum = -1;

	if(img == -1)
		return false;

	ImageNum = img;
	cImage* current = m_Images;
	while(current)
	{
		if(count == ImageNum)
			break;
		count++;
		current = current->m_Next;
	}

	if(current)
	{
		img = ImageNum;
		if(current->m_AniSurface)
			return true;
		else
			return false;
	}

	return false;
}

int cImageList::DrawImage(int x, int y, int width, int height, bool random, int img)
{
	int count = 0;
	SDL_Rect rect;
	int ImageNum = -1;

	rect.y = rect.x = 0;
	rect.w = width;
	rect.h = height;

	if(!random)
	{
		if(img == -1)
			return -1;

		if(img > m_NumImages)
		{
			if(m_NumImages == 1)
				ImageNum = 0;
			else
				ImageNum = g_Dice%m_NumImages;
		}
		else
			ImageNum = img;
		cImage* current = m_Images;
		while(current)
		{
			if(count == ImageNum)
				break;
			count++;
			current = current->m_Next;
		}

		if(current)
		{
			if(current->m_AniSurface)
				current->m_AniSurface->DrawFrame(x,y, rect.w, rect.h, g_Graphics.GetTicks());
			else
				current->m_Surface->DrawSurface(x,y,0,&rect, true);
		}
	}
	else
	{
		if(m_NumImages == 0)
			return -1;
		else if(m_NumImages == 1)
		{
			m_Images->m_Surface->DrawSurface(x,y,0,&rect, true);
			return 0;
		}
		else
		{
			ImageNum = g_Dice%m_NumImages;
			cImage* current = m_Images;
			while(current)
			{
				if(count == ImageNum)
					break;
				count++;
				current = current->m_Next;
			}

			if(current)
			{
				if(current->m_AniSurface)
					current->m_AniSurface->DrawFrame(x,y, rect.w, rect.h, g_Graphics.GetTicks());
				else
					current->m_Surface->DrawSurface(x,y,0,&rect, true);
			}
			else
				return -1;
		}
	}

	return ImageNum;
}

string cImageList::GetName(int i)
{
	int count = 0;
	cImage* current = m_Images;
	while(current)
	{
		if(count == i)
			break;
		count++;
		current = current->m_Next;
	}

	if(current)
		return current->m_Surface->GetFilename();

	return string("");
}

cAImgList* cImgageListManager::ListExists(string name)
{
	cAImgList* current = m_First;
	while(current)
	{
		if(current->m_Name == name)
			break;
		current = current->m_Next;
	}
	return current;
}

cAImgList* cImgageListManager::LoadList(string name)
{
	cAImgList* current = ListExists(name);
	if(current)
		return current;

	current = new cAImgList();
	current->m_Name = name;
	current->m_Next = 0;
	/* mod
	uses dir path and file list to construct the girl images
	*/

	DirPath imagedir;
	imagedir<<"Resources"<< "Characters"<<name;
	string numeric="123456789";
	string pic_types[]={"Anal*.*g","BDSM*.*g","Sex*.*g","Beast*.*g","Group*.*g","Les*.*g","Preg*.*g","Death*.*g","Profile*.*g","Combat*.*g","Oral*.*g","Ecchi*.*g","Strip*.*g","Maid*.*g","Sing*.*g","Wait*.*g","Card*.*g","Bunny*.*g","Nude*.*g","Mast*.*g","Titty*.*g","Milk*.*g","PregAnal*.*g","PregBDSM*.*g","PregSex*.*g","pregbeast*.*g","preggroup*.*g","pregles*.*g",};
	int i=0;

	do {
		bool to_add=true;
		FileList the_files(imagedir,pic_types[i].c_str());
		for(int k=0;k<the_files.size();k++)
		{
			bool test=false;

			if(i==6)		//  Check Preg*.*g filenames [leaf] and accept as non-subtypew ONLY those with number 1--9 in char 5
							//  (Allows filename like 'Preg22.jpg' BUT DOESN'T allow like 'Preg (2).jpg' or 'Preg09.jpg')
							//  MIGHT BE BETTER to just throw out sub-type filenames in this Preg*.*g section.
			{
				char c=the_files[k].leaf()[4];
				for(int j = 0; j < 9; j++) {
				
					if(c==numeric[j])
					{
						test=true;
						break;
					}
				}
				if(!test)
				{
					k=the_files.size();
					to_add=false;
				}
			}
			if(to_add)
			{
				current->m_Images[i].AddImage(the_files[k].full());
				current->m_Images[i].m_NumImages++;
			}
		}
		i++;
	}while(i<NUM_IMGTYPES);


	// Yes this is just a hack to load animations (my bad ;) - Necro
	string pic_types2[]={"Anal*.ani","BDSM*.ani","Sex*.ani","Beast*.ani","Group*.ani","Les*.ani","Preg*.ani",
			"Death*.ani","Profile*.ani","Combat*.ani","Oral*.ani","Ecchi*.ani","Strip*.ani","Maid*.ani",
			"Sing*.ani","Wait*.ani","Card*.ani","Bunny*.ani","Nude*.ani","Mast*.ani","Titty*.ani",
			"Milk*.ani","PregAnal*.ani","PregBDSM*.ani","PregSex*.ani","pregbeast*.ani","preggroup*.ani",
			"pregles*.ani"};
	i=0;
	do {
		bool to_add=true;
		FileList the_files(imagedir,pic_types2[i].c_str());
		for(int k=0;k<the_files.size();k++)
		{
			bool test=false;

			if(i==6)		//  Check Preg*.*g filenames [leaf] and accept as non-subtypew ONLY those with number 1--9 in char 5
							//  (Allows filename like 'Preg22.jpg' BUT DOESN'T allow like 'Preg (2).jpg' or 'Preg09.jpg')
							//  MIGHT BE BETTER to just throw out sub-type filenames in this Preg*.*g section.

			{
				char c=the_files[k].leaf()[4];
				for(int j = 0; j < 9; j++) {
				
					if(c==numeric[j])
					{
						test=true;
						break;
					}
				}
				if(!test)
				{
					k=the_files.size();
					to_add=false;
				}
			}
			if(to_add)
			{
				current->m_Images[i].AddImage(the_files[k].full(), the_files[k].path(), the_files[k].leaf());
				current->m_Images[i].m_NumImages++;
			}
		}
		i++;
	} while(i<NUM_IMGTYPES);

//		}while(i<15);  Wasn't checking up to curr 28 image types, 04/07/2013

	if(m_Last)
	{
		m_Last->m_Next = current;
		m_Last = current;
	}
	else
		m_First = m_Last = current;

	return current;
}

void cGirls::LoadDefaultImages()	// for now they are hard coded
{
	m_DefImages = m_ImgListManager.LoadList("Default");
}

bool cGirls::IsAnimatedSurface(sGirl* girl, int ImgType, int& img)
{
//               Loop thru case stmt(s) for image types; if current imagetype has no images, 
//               substitute for next loop an image type that has/may have images.
//               Only substitute more-general image types or those with > 0 image count, 
//               ending with girl profile or default images, avoiding endless loop. 
//				 (Simplified 'if' logic by using success/failure 'case" statements, 4-5-2013.)

//				Also added capability to handle passed ImgType of pregnant subtypes

	CLog l;

	while(1)
	{
/*
 *		if you sell a girl from the dungeon, and then hotkey back to girl management
 *		it crashes with girl->m_GirlImages == 0
 *
 *		so let's test for that here
 */
		if(!girl || !girl->m_GirlImages) {
			break;
		}

		switch(ImgType)
		{
//				kept all cases to test for invalid Image Type.
		case IMGTYPE_ANAL:
		case IMGTYPE_BDSM:
		case IMGTYPE_BEAST:
		case IMGTYPE_GROUP:
		case IMGTYPE_LESBIAN:
//				Similar pregnant/non-pregnant 'success' condition and action; uses 'pregnancy offset'
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages)
				return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].IsAnimatedSurface(img);
			else if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PROFILE:
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
			{
//				if(girl->m_newRandomFixed >= 0)  // A simiar process had these 2 lines
//					random = true;
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].IsAnimatedSurface(img);
			}
			else if(girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].IsAnimatedSurface(img);
			else
//							Use default images, avoid endless loop
				return m_DefImages->m_Images[IMGTYPE_PROFILE].IsAnimatedSurface(img);
			break;

		case IMGTYPE_PREGANAL:
		case IMGTYPE_PREGBDSM:
		case IMGTYPE_PREGBEAST:
		case IMGTYPE_PREGGROUP:
		case IMGTYPE_PREGLESBIAN:
		case IMGTYPE_PREGSEX:
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PREGNANT:
		case IMGTYPE_DEATH:
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				return m_DefImages->m_Images[ImgType].IsAnimatedSurface(img);
			break;


		case IMGTYPE_SEX:
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].m_NumImages)
				return m_DefImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			else
				return m_DefImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			break;

		case IMGTYPE_COMBAT:
		case IMGTYPE_MAID:
		case IMGTYPE_SING:
		case IMGTYPE_WAIT:
		case IMGTYPE_CARD:
		case IMGTYPE_BUNNY:
		case IMGTYPE_MILK:
//				Similar'success' condition and action; 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;

			break;

		case IMGTYPE_ORAL:
		case IMGTYPE_ECCHI:
		case IMGTYPE_STRIP:
		case IMGTYPE_NUDE:
		case IMGTYPE_MAST:
		case IMGTYPE_TITTY:
//				Similar'success' condition and action  (but no alternative ImgType set); 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			break;

		default:
			//error!
			l.ss()	<< "cGirls::IsAnimatedSurface: "
				<< "unexpected image type: "
				<< ImgType
				;
			l.ssend();
			break;

		//				And many conditions return early
		}

//		If not returned to calling module already, have failed to find ImgType images.
//              If have not already, test a substitute image type that has/may have images,
//					substitute ImgType and leave processing for nexp loop if simpler.
//			(First switch testing success, 2nd setting replacement ImgType replaces complicated 'if's.
		
		switch(ImgType)
		{
		case IMGTYPE_ORAL:
				ImgType = IMGTYPE_SEX;
			break;

		case IMGTYPE_ECCHI:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].IsAnimatedSurface(img);
 			else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].IsAnimatedSurface(img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_STRIP:
			if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].IsAnimatedSurface(img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].IsAnimatedSurface(img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_NUDE:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].IsAnimatedSurface(img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].IsAnimatedSurface(img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_MAST:
 				ImgType = IMGTYPE_NUDE;
			break;

		case IMGTYPE_TITTY:
				ImgType = IMGTYPE_ORAL;
			break;


		default:
			break;
		}
	}
	return 0;
}

CSurface* cGirls::GetImageSurface(sGirl* girl, int ImgType, bool random, int& img, bool gallery)
{
	while(1)
	{
//               Loop thru case stmt(s) for image types; if current imagetype has no images, 
//               substitute for next loop an image type that has/may have images.
//               Only substitute more-general image types or those with > 0 image count, 
//               ending with girl profile or default images, avoiding endless loop. 
//				 (Simplified 'if' logic by using success/failure 'case" statements, 4-5-2013.)

/*
 *		if you sell a girl from the dungeon, and then hotkey back to girl management
 *		it crashes with girl->m_GirlImages == 0
 *
 *		so let's test for that here
 */
		if(!girl || !girl->m_GirlImages) {
			break;
		}

		switch(ImgType)
		{
//				kept all cases to test for invalid Image Type.
		case IMGTYPE_ANAL:
		case IMGTYPE_BDSM:
		case IMGTYPE_BEAST:
		case IMGTYPE_GROUP:
		case IMGTYPE_LESBIAN:
//				Similar pregnant/non-pregnant 'success' condition and action; uses 'pregnancy offset'
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages && !gallery)
				return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].GetImageSurface(random, img);
			else if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PROFILE:
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages && !gallery)
			{
				if(girl->m_newRandomFixed >= 0)
					random = true;
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].GetImageSurface(random, img);
			}
			else if(girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].GetImageSurface(random, img);
			else
//							Use default images, avoid endless loop
				return m_DefImages->m_Images[IMGTYPE_PROFILE].GetImageSurface(random, img);
			break;

		case IMGTYPE_PREGANAL:
		case IMGTYPE_PREGBDSM:
		case IMGTYPE_PREGBEAST:
		case IMGTYPE_PREGGROUP:
		case IMGTYPE_PREGLESBIAN:
		case IMGTYPE_PREGSEX:
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PREGNANT:
		case IMGTYPE_DEATH:
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);
			else
//							Use default images, avoid endless loop
				return m_DefImages->m_Images[ImgType].GetImageSurface(random, img);
			break;

		case IMGTYPE_SEX:
			if(girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].m_NumImages && !gallery)
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].GetImageSurface(random, img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_SEX].GetImageSurface(random, img);
			else
				return m_DefImages->m_Images[IMGTYPE_SEX].GetImageSurface(random, img);
			break;

		case IMGTYPE_COMBAT:
		case IMGTYPE_MAID:
		case IMGTYPE_SING:
		case IMGTYPE_WAIT:
		case IMGTYPE_CARD:
		case IMGTYPE_BUNNY:
		case IMGTYPE_MILK:
//				Similar'success' condition and action; 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);
			else
				ImgType = IMGTYPE_PROFILE;

			break;

		case IMGTYPE_ORAL:
		case IMGTYPE_ECCHI:
		case IMGTYPE_STRIP:
		case IMGTYPE_NUDE:
		case IMGTYPE_MAST:
		case IMGTYPE_TITTY:
//				Similar'success' condition and action  (but no alternative ImgType set); 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);
			break;

		default:
			//error!
			break;

		//				And many conditions return early
		}


//		If not returned to calling module already, have failed to find ImgType images.
//              If have not already, test a substitute image type that has/may have images,
//					substitute ImgType and leave processing for nexp loop if simpler.
//			(First switch testing success, 2nd setting replacement ImgType replaces complicated 'if's.
		
		switch(ImgType)
		{
		case IMGTYPE_ORAL:
				ImgType = IMGTYPE_SEX;
			break;

		case IMGTYPE_ECCHI:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].GetImageSurface(random, img);
 				else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
					return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].GetImageSurface(random, img);
				else	
					ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_STRIP:
			if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].GetImageSurface(random, img);
 				else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
					return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].GetImageSurface(random, img);
				else	
					ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_NUDE:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].GetImageSurface(random, img);
 				else if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
					return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].GetImageSurface(random, img);
				else	
					ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_MAST:
 				ImgType = IMGTYPE_NUDE;
			break;

		case IMGTYPE_TITTY:
				ImgType = IMGTYPE_ORAL;
			break;


		default:
			//		Not an error here, just that next ImgType should be already set
			break;
		}
	}

	return 0;		// would be failure to find & put image on surface, but errcode not passed back
}

cAnimatedSurface* cGirls::GetAnimatedSurface(sGirl* girl, int ImgType, int& img)
{
	return girl->m_GirlImages->m_Images[ImgType].GetAnimatedSurface(img);
}

/*
 * takes a girl, and image type number, and the pregnant equivalent thereof.
 *
 * returns the pregnant number if A) girl is preggers and B) girl specific images
 * exist
 *
 * Otherwise returns the non-preggy number, if the girl has images for that action
 *
 * if not, returns -1 so the caller can sort it out
 */
int cGirls::get_modified_image_type(sGirl *girl, int image_type, int preg_type)
{
	bool preg = girl->is_pregnant();
/*
 *	if she's pregnant, and if there exist pregnant images for 
 *	whatever this action is - use them
 */
	if(preg && girl->m_GirlImages->m_Images[preg_type].m_NumImages > 0) {
		return preg_type;
	}
/*
 *	if not, pregnant or not, try and find a non pregnant image for this sex type
 *	the alternative would be to use a pregnant vanilla sex image - but 
 *	we're keeping the sex type in preference to the pregnancy
 */
	if(girl->m_GirlImages->m_Images[image_type].m_NumImages > 0) {
		return image_type;
	}
/*
 *	rather than try for pregnant straight sex and straight sex
 *	let's just return -1 here and let the caller re-try with
 *	normal sex arguments
 */
	return -1;
}

/*
 * Given an image type, this tries to draw the girls own version
 * of that image if available. If not, it takes one from the default
 * set
 */
int cGirls::draw_with_default(
	sGirl* girl,
	int x, int y,
	int width, int height,
	int ImgType,
	bool random,
	int img
)
{
	cImageList *images;
/*
 *	does the girl have her own pics for this image type
 *	or do we need to use the default ones?
 */
	if(girl->m_GirlImages->m_Images[ImgType].m_NumImages == 0) {
		images = m_DefImages->m_Images + ImgType;
	}
	else {
		images = girl->m_GirlImages->m_Images + ImgType;
	}
/*
 *	draw and return
 */
	return images->DrawImage(x, y, width, height, random, img);
}

int cGirls::DrawGirl(sGirl* girl, int x, int y, int width, int height, int ImgType, bool random, int img)
{
	bool preg = false;
	if(girl->is_pregnant())
		preg = true;
	while(1)
	{
//               Loop thru case stmt(s) for image types; if current imagetype has no images, 
//               substitute for next loop an image type that has/may have images.
//               Only substitute more-general image types or those with > 0 image count, 
//               ending with girl profile or default images, avoiding endless loop. 
//				 (Simplified 'if' logic by using success/failure 'case" statements, 4-5-2013.)

//				Also added capability to handle passed ImgType of pregnant subtypes

/*
 *		if you sell a girl from the dungeon, and then hotkey back to girl management
 *		it crashes with girl->m_GirlImages == 0
 *
 *		so let's test for that here
 */
		if(!girl || !girl->m_GirlImages) {
			break;
		}
	
			//			NOTE that this DrawGirl() section does not check if 'gallery' like 
			//			     GetImageSurface() does.

		switch(ImgType)
		{
//				kept all cases to test for invalid Image Type.
		case IMGTYPE_ANAL:
		case IMGTYPE_BDSM:
		case IMGTYPE_BEAST:
		case IMGTYPE_GROUP:
		case IMGTYPE_LESBIAN:
//				Similar pregnant/non-pregnant 'success' condition and action; uses 'pregnancy offset'
			if(preg && girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages > 0)
					return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].DrawImage(x,y,width,height, random, img);
			else if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x,y,width,height, random, img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PROFILE:
			if(preg && girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
			{
				if(girl->m_newRandomFixed >= 0)
					random = true;
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].DrawImage(x,y,width,height, random, img);
			}
			else if(girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].DrawImage(x,y,width,height, random, img);
			else
//							Use default images, avoid endless loop
				return m_DefImages->m_Images[IMGTYPE_PROFILE].DrawImage(x,y,width,height, random, img);
			break;


		case IMGTYPE_PREGNANT:
		case IMGTYPE_DEATH:
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages == 0)
				return m_DefImages->m_Images[ImgType].DrawImage(x,y,width,height, random, img);
			else
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x,y,width,height, random, img);
			break;

		case IMGTYPE_SEX:
			if(preg && girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].m_NumImages)
					return girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].DrawImage(x,y,width,height, random, img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_SEX].DrawImage(x,y,width,height, random, img);
			else
				return m_DefImages->m_Images[IMGTYPE_SEX].DrawImage(x,y,width,height, random, img);
			break;

		case IMGTYPE_COMBAT:
		case IMGTYPE_MAID:
		case IMGTYPE_SING:
		case IMGTYPE_WAIT:
		case IMGTYPE_CARD:
		case IMGTYPE_BUNNY:
		case IMGTYPE_MILK:
//				Similar'success' condition and action; 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x,y,width,height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_ORAL:
		case IMGTYPE_ECCHI:
		case IMGTYPE_STRIP:
		case IMGTYPE_NUDE:
		case IMGTYPE_MAST:
		case IMGTYPE_TITTY:
//				Similar'success' condition and action  (but no alternative ImgType set); 
			if(girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x,y,width,height, random, img);
			break;

		default:
			//error!
			break;

		//				And many conditions return early
		}


//		If not returned to calling module already, have failed to find ImgType images.
//              If have not already, test a substitute image type that has/may have images,
//					substitute ImgType and leave processing for nexp loop if simpler.
//			(First switch testing success, 2nd setting replacement ImgType replaces complicated 'if's.
		
		switch(ImgType)
		{
		case IMGTYPE_ORAL:
				ImgType = IMGTYPE_SEX;
			break;

		case IMGTYPE_ECCHI:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
					return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].DrawImage(x,y,width,height, random, img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].DrawImage(x,y,width,height, random, img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_STRIP:
			if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].DrawImage(x,y,width,height, random, img);
			else if(girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].DrawImage(x,y,width,height, random, img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_NUDE:
			if(girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].DrawImage(x,y,width,height, random, img);
			else 			if(girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].DrawImage(x,y,width,height, random, img);
			else	
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_MAST:
 				ImgType = IMGTYPE_NUDE;
			break;

		case IMGTYPE_TITTY:
				ImgType = IMGTYPE_ORAL;
			break;

		default:
			//		Not an error here, just that next ImgType should be already set
			break;
		}
	}

	return -1;		// failure to find & draw image
}

void sGirl::OutputGirlRow(string* Data, const vector<string>& columnNames)
{
	for (unsigned int x = 0; x < columnNames.size(); ++x)
	{
		//for each column, write out the statistic that goes in it
		OutputGirlDetailString(Data[x], columnNames[x]);
	}
}

void sGirl::OutputGirlDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");

	if (detailName == "Name")
	{
		ss << m_Realname;
	}
	else if (detailName == "Health")
	{
		if (get_stat(STAT_HEALTH) == 0)
			ss << gettext("DEAD");
		else
			ss << get_stat(STAT_HEALTH) << "%";
	}
	else if (detailName == "Libido")
	{
		ss << libido();
	}
	else if (detailName == "Rebel")
	{
		ss << rebel();
	}
	else if (detailName == "Looks")
	{
		ss << ((get_stat(STAT_BEAUTY)+get_stat(STAT_CHARISMA))/2);
		ss << "%";
	}
	else if (detailName == "Tiredness")
	{
		ss << get_stat(STAT_TIREDNESS) << "%";
	}
	else if (detailName == "Happiness")
	{
		ss << get_stat(STAT_HAPPINESS) << "%";
	}
	else if (detailName == "Age")
	{
		if(get_stat(STAT_AGE) == 100)
		{
			ss << gettext("???");
		}
		else
		{
			ss << get_stat(STAT_AGE);
		}
	}
	else if (detailName == "Virgin")
	{
		if(m_Virgin)
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else if (detailName == "Weeks_Due")
	{
		if (is_pregnant())
		{
			cConfig cfg;
			int to_go = cfg.pregnancy.weeks_pregnant() - m_WeeksPreg;
			ss << to_go;
		}
		else
		{
			ss << gettext("---");
		}
	}
	else if (detailName == "PregCooldown")
	{
		ss << (int)m_PregCooldown;
	}
	else if (detailName == "Accomodation")
	{
		if(m_AccLevel == 0)
			ss << gettext("Very Poor");
		else if(m_AccLevel == 1)
			ss << gettext("Adequate");
		else if(m_AccLevel == 2)
			ss << gettext("Nice");
		else if(m_AccLevel == 3)
			ss << gettext("Good");
		else if(m_AccLevel == 4)
			ss << gettext("Wonderful");
		else if(m_AccLevel == 5)
			ss << gettext("High Class");
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName == "Gold")
	{
		if(g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			ss << m_Money;
		}
		else
		{
			ss << gettext("???");
		}
	}
	else if (detailName == "Pay")
	{
			ss << m_Pay;
	}
	else if (detailName == "DayJob")
	{
		if (m_DayJob >= NUM_JOBS)
		{
			ss << gettext("None");
		}
		else
		{
			ss << g_Brothels.m_JobManager.JobName[m_DayJob];
		}
	}
	else if (detailName == "NightJob")
	{
		if (m_NightJob >= NUM_JOBS)
		{
			ss << gettext("None");
		}
		else
		{
			ss << g_Brothels.m_JobManager.JobName[m_NightJob];
		}
	}
	else if (detailName.find("STAT_") != string::npos)
	{
		string stat = detailName;
		stat.replace(0, 5, "");
		int code = sGirl::lookup_stat_code(stat);
		if (code != -1)
		{
			ss << get_stat(code);
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName.find("SKILL_") != string::npos)
	{
		string skill = detailName;
		skill.replace(0, 6, "");
		int code = sGirl::lookup_skill_code(skill);
		if (code != -1)
		{
			ss << get_skill(code);
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName.find("STATUS_") != string::npos)
	{
		string status = detailName;
		status.replace(0, 7, "");
		int code = lookup_status_code(status);
		if (code != -1)
		{
			if(m_States&(1<<code))
			{
				ss << gettext("Yes");
			}
			else
			{
				ss << gettext("No");
			}
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName == "is_slave")
	{
		if(is_slave())
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else if (detailName == "carrying_human")
	{
		if(carrying_human())
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else if (detailName == "is_pregnant")
	{
		if(is_pregnant())
		{
			cConfig cfg;
			int to_go = cfg.pregnancy.weeks_pregnant() - m_WeeksPreg;
			ss << gettext("Yes") << "(" << to_go << ")";
		}
		else
		{
			if (m_PregCooldown == 0)
				ss << gettext("No");
			else
				ss << gettext("No") << "(" << (int)m_PregCooldown << ")";
		}
	}
	else if (detailName == "is_addict")
	{
		if(is_addict())
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else if (detailName == "is_mother")
	{
		if(is_mother())
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else if (detailName == "is_poisoned")
	{
		if(is_poisoned())
		{
			ss << gettext("Yes");
		}
		else
		{
			ss << gettext("No");
		}
	}
	else
	{
		ss << gettext("Not found");
	}
	Data = ss.str();
}

int sGirl::rebel()
{
	return g_Girls.GetRebelValue(this, this->m_DayJob == JOB_MATRON);
}


