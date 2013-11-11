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
#include <iostream>
#include <ostream>
#include <fstream>
#include <cctype>
#include "sConfig.h"
#include "DirPath.h"
#include "CLog.h"
#include "tinyxml.h"
#include "cColor.h"

extern CLog g_LogFile;
static CLog &l = g_LogFile;
static cColor ColorConvert;
sConfigData *cConfig::data ;

/*
 * changed this to take a filename so we can pass config files on the
 * command line
 */
sConfigData::sConfigData(const char *a_filename)
: fonts()
{
	DirPath dp = DirPath() << "Resources" << "Data" << a_filename;
	string filename = dp.c_str();
	l.ss() 
		<< "Loading configration variables from '"
		<< filename
		<< "'"
	;
	l.ssend();
/*
 *	make sure we have something playable,
 *	even if the file doesn't load
 */
	set_defaults();
/*
 *	open thje file - moan most eloqently in its absence
 */
	TiXmlDocument doc(filename);
	if(!doc.LoadFile()) {
		l.ss()	<< "can't load " << filename << endl;
		l.ss()	<< "Error: line "
			<< doc.ErrorRow()
			<< ", col "
			<< doc.ErrorCol()
			<< ": "
			<< doc.ErrorDesc()
		;
		l.ssend();
/*
 *		a bit of narrative for the players: makes it easier to tell
 *		if the config isn't being found
 */
		l.ss()	<< "*** Game will run with default pricing factors."
			<< endl
			<< "*** This may seem a little easy. To fix this"
			<< endl
			<< "*** get a config.xml file from pinkpetal.co.cc"
		;
		l.ssend();
		return;
	}
/*
 *	get the docuement root
 */
	TiXmlElement *el, *root_el = doc.RootElement();
/*
 *	loop over the elements attached to the root
 */
	for(	el = root_el->FirstChildElement();
		el ;
		el = el->NextSiblingElement()
	) {
		string tag = el->ValueStr();
		/*
		l.ss()	<< "processing tag: '"
		 	<< tag
			<< "' ..."
		;
		l.ssend();
		*/
/*
 *		now, depending on the tag name...
 */
 		if(el->ValueStr() == "Income") {
			get_income_factors(el);
			continue;
		}
/*
 *		how about expenses?
 */
 		if(el->ValueStr() == "Expenses") {
			get_expense_factors(el);
			continue;
		}
/*
 *		Initialisation values
 */
 		if(el->ValueStr() == "Initial") {
			get_initial_values(el);
			continue;
		}

		/*
 *		Initialisation values
 */
 		if(el->ValueStr() == "Resolution") {
			get_resolution_data(el);
			continue;
		}
/*
 *		Gambling values
 */
 		if(el->ValueStr() == "Gambling") {
			get_gambling_factors(el);
			continue;
		}
/*
 *		Tax values
 */
 		if(el->ValueStr() == "Tax") {
			get_tax_factors(el);
			continue;
		}
/*
 *		Pregnancy controls
 */
 		if(el->ValueStr() == "Pregnancy") {
			get_preg_factors(el);
			continue;
		}
/*
 *		Gang controls
 */
 		if(el->ValueStr() == "Gangs") {
			get_gang_factors(el);
			continue;
		}
/*
 *		Prostitution controls
 */
 		if(el->ValueStr() == "Prostitution") {
			get_pros_factors(el);
			continue;
		}

/*
 *		Items
 */
 		if(el->ValueStr() == "Items") {
			get_item_data(el);
			continue;
		}

/*
 *		Fonts
 */
 		if(el->ValueStr() == "Fonts") {
			get_font_data(el);
			continue;
		}

/*
 *		Debug
 */
 		if(el->ValueStr() == "Debug") {
			get_debug_flags(el);
			continue;
		}

		l.ss()	<< "Warning: config.xml: tag: '"
		 	<< tag
			<< "' unexpected"
		;
		l.ssend();
	}
}


void sConfigData::get_att(TiXmlElement *el, const char *name, bool &bval)
{
	const char *pt;

        pt = el->Attribute(name);
/*
 *	if we didn't get a value, but it wasn't mandatory
 *	we can go
 */
	if(!pt) {
		g_LogFile.ss()
			<< "Error: in config: No attribute named '"
			<< name
			<< "' found."
		;
		g_LogFile.ssend();
		return;
	}
/*
 *	convert to a string, and then squash that to lower case
 */
	string s = pt;
	for(u_int i = 0; i < s.length(); i++) {
		s[i] = std::tolower(s[i]);
	}
/*
 *	now we expect either "true" or "false"
 *	we can take "1" and "0" as well
 */
	if(s == "true" || s == "1") {
		bval = true;
		return;
	}
	if(s == "false" || s == "0") {
		bval = false;
		return;
	}

	g_LogFile.ss()
		<< "Error in config: Binary attribute '"
		<< name
		<< "': unexpected value '"
		<< s
		<< "'"
	;
	g_LogFile.ssend();
}


void sConfigData::get_att(TiXmlElement *el, const char *name, double *dpt)
{
        if(el->Attribute(name, dpt)) {
		return;
	}
	l.ss()	<< "Warning: config.xml: No '"
		<< name
		<< "' attribute: defaulting to "
		<< *dpt;
	l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, string &s)
{
	const char *pt;

        pt = el->Attribute(name);
        if(pt) {
		s = pt;
		return;
	}
	l.ss()	<< "Warning: config.xml: No '"
		<< name
		<< "' attribute: defaulting to "
		<< s;
	l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, int *ipt)
{
	int def_val = *ipt;
	if(el->Attribute(name, ipt)) {
		return;
	}
	l.ss()	<< "Warning: config.xml: No '"
		<< name
		<< "' attribute: defaulting to "
		<< def_val
	;
	l.ssend();
	*ipt = def_val;
}

void sConfigData::get_income_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "ExtortionIncome",	&in_fact.extortion);
	get_att(el, "GirlsWorkBrothel",	&in_fact.brothel_work);
	get_att(el, "GirlsWorkStreet",	&in_fact.street_work);
	get_att(el, "MovieIncome",		&in_fact.movie_income);
	get_att(el, "StripperIncome",	&in_fact.stripper_work);
	get_att(el, "BarmaidIncome",	&in_fact.barmaid_work);
	get_att(el, "SlaveSales",		&in_fact.slave_sales);
	get_att(el, "ItemSales",		&in_fact.item_sales);
}

void sConfigData::get_expense_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "Training",			&out_fact.training);
	get_att(el, "MovieCost",		&out_fact.actress_wages);
	get_att(el, "ActressWages",		&out_fact.movie_cost);
	get_att(el, "GoonWages",		&out_fact.goon_wages);
	get_att(el, "MatronWages",		&out_fact.matron_wages);
	get_att(el, "GirlSupport",		&out_fact.girl_support);
	get_att(el, "Consumables",		&out_fact.consumables);
	get_att(el, "Items",			&out_fact.item_cost);
	get_att(el, "SlavesBought",		&out_fact.slave_cost);
	get_att(el, "BuyBrothel",		&out_fact.brothel_cost);
	get_att(el, "BrothelSupport",	&out_fact.brothel_support);
	get_att(el, "BarSupport",		&out_fact.bar_cost);
	get_att(el, "CasinoSupport",	&out_fact.casino_cost);
	get_att(el, "Bribes",			&out_fact.bribes);
	get_att(el, "Fines",			&out_fact.fines);
	get_att(el, "Advertising",		&out_fact.advertising);
}

void sConfigData::get_initial_values(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "Gold",					&initial.gold);
	get_att(el, "GirlMeet",				&initial.girl_meet);
	get_att(el, "SlaveHousePerc",		&initial.slave_house_perc);
	get_att(el, "AutoUseItems",			initial.auto_use_items);
	get_att(el, "TortureTraitWeekMod",	&initial.torture_mod);
}

void sConfigData::get_tax_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "Rate",			&tax.rate);
	get_att(el, "Minimum",		&tax.minimum);
	get_att(el, "Laundry",		&tax.laundry);
/*
 *	we expect these in the format "25%" with the "%" being optional
 *	The trailing % will be chopped off by tinyxml,
 *	but we still need to divide by 100 to turn the numbers
 *	from a percentage to a floating point factor
 */
	tax.rate	/= 100.0;
	tax.minimum	/= 100.0;
	tax.laundry	/= 100.0;
}

void sConfigData::get_gambling_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "Odds",			&gamble.odds);
	get_att(el, "Base",			&gamble.base);
	get_att(el, "Spread",		&gamble.spread);
/*
 *	strictly, we only need one of these:
 *	a customer factor OR a house factor
 *	but I thought it might be easier to explain
 *	if each had its own multiplier
 */
	get_att(el, "CustomerFactor",	&gamble.customer_factor);
	get_att(el, "HouseFactor",		&gamble.house_factor);
}

void sConfigData::get_preg_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "PlayerChance",		&pregnancy.player_chance);
	get_att(el, "CustomerChance",	&pregnancy.customer_chance);
	get_att(el, "MonsterChance",	&pregnancy.monster_chance);
	get_att(el, "GoodSexFactor",	&pregnancy.good_sex_factor);
	get_att(el, "ChanceOfGirl",		&pregnancy.chance_of_girl);
	get_att(el, "WeeksPregnant",	&pregnancy.weeks_pregnant);
	get_att(el, "WeeksTillGrown",	&pregnancy.weeks_till_grown);
	get_att(el, "CoolDown",			&pregnancy.cool_down);
}

void sConfigData::get_gang_factors(TiXmlElement *el)
{
/*
 *	get the factors by XML attribute name
 */
	get_att(el, "MaxRecruitList",		&gangs.max_recruit_list);
	get_att(el, "StartRandom",			&gangs.start_random);
	get_att(el, "StartBoosted",			&gangs.start_boosted);
	get_att(el, "InitMemberMin",		&gangs.init_member_min);
	get_att(el, "InitMemberMax",		&gangs.init_member_max);
	get_att(el, "ChanceRemoveUnwanted",	&gangs.chance_remove_unwanted);
	get_att(el, "AddNewWeeklyMin",		&gangs.add_new_weekly_min);
	get_att(el, "AddNewWeeklyMax",		&gangs.add_new_weekly_max);
}

void sConfigData::get_pros_factors(TiXmlElement *el)
{
	get_att(el, "RapeStreet",			&prostitution.rape_streets);
	get_att(el, "RapeBrothel",			&prostitution.rape_brothel);
}

void sConfigData::get_debug_flags(TiXmlElement *el)
{
	get_att(el, "LogItems",		debug.log_items);
	get_att(el, "LogGirls",		debug.log_girls);
	get_att(el, "LogRGirls",	debug.log_rgirls);
	get_att(el, "LogFonts",		debug.log_fonts);
	get_att(el, "LogTorture",	debug.log_torture);
}

void sConfigData::get_font_data(TiXmlElement *el)
{
	get_att(el, "Normal",		fonts.normal);
	get_att(el, "Fixed",		fonts.fixed);
	get_att(el, "Antialias",	fonts.antialias);
}

void sConfigData::get_resolution_data(TiXmlElement *el)
{
	get_att(el, "Resolution",	resolution.resolution);
}

void sConfigData::get_item_data(TiXmlElement *el)
{
	get_att(el, "AutoCombatEquip", items.auto_combat_equip);
	for(int i=0; i<7; i++)
	{
		string ColorIn;
		stringstream ss;
		ss.str("");
		ss << "RarityColor" << i;
		get_att(el, ss.str().c_str(),	ColorIn);
		ColorConvert.HexToSDLColor(ColorIn, items.rarity_color[i]);
	}
}

/*
 * these just make sure the game has a playable
 * set of factors in the event that the config
 * file can't be found
 */
void sConfigData::set_defaults()
{
	initial.gold		        = 4000;
	initial.girl_meet	        = 30;
	initial.slave_house_perc    = 100;
	initial.auto_use_items	    = false;
	initial.torture_mod			= 1;

	in_fact.extortion	        = 1.0;
	in_fact.brothel_work	    = 1.0;
	in_fact.street_work	        = 1.0;
	in_fact.movie_income	    = 1.0;
	in_fact.stripper_work	    = 1.0;
	in_fact.barmaid_work	    = 1.0;
	in_fact.slave_sales	        = 1.0;
	in_fact.item_sales	        = 1.0;
	
    out_fact.training	        = 0.0;
	out_fact.actress_wages	    = 0.0;
	out_fact.movie_cost	        = 1.0;
	out_fact.goon_wages	        = 1.0;
	out_fact.matron_wages	    = 1.0;
	out_fact.girl_support	    = 1.0;
	out_fact.consumables	    = 1.0;
	out_fact.item_cost	        = 1.0;
	out_fact.slave_cost	        = 1.0;
	out_fact.brothel_cost	    = 1.0;
	out_fact.bribes		        = 1.0;
	out_fact.fines		        = 1.0;
	out_fact.advertising	    = 1.0;
	
    tax.rate		            = 0.06;
	tax.minimum		            = 0.01;
	tax.laundry		            = 0.25;
	
    gamble.odds		            = 49;
	gamble.base		            = 79;
	gamble.spread		        = 400;
	gamble.house_factor	        = 1.0;
	gamble.customer_factor	    = 0.5;
	
    pregnancy.player_chance	    = 8;
	pregnancy.customer_chance   = 8;
	pregnancy.monster_chance    = 8;
	pregnancy.good_sex_factor   = 8;
	pregnancy.chance_of_girl    = 50;
	pregnancy.weeks_pregnant    = 38;
	pregnancy.weeks_till_grown  = 60;
	pregnancy.cool_down	        = 60;

	gangs.max_recruit_list      = 6;
	gangs.start_random          = 2;
	gangs.start_boosted         = 2;
	gangs.init_member_min       = 1;
	gangs.init_member_max       = 10;
	gangs.chance_remove_unwanted = 25;
	gangs.add_new_weekly_min    = 0;
	gangs.add_new_weekly_max    = 2;

	prostitution.rape_brothel   =1;
	prostitution.rape_streets   =5;

	items.auto_combat_equip = true;

	for(int i=0; i<7; i++)
	{
		items.rarity_color[i] = new SDL_Color();
	}
	ColorConvert.HexToSDLColor("000000", items.rarity_color[0]);
	ColorConvert.HexToSDLColor("000050", items.rarity_color[1]);
	ColorConvert.HexToSDLColor("0000A0", items.rarity_color[2]);
	ColorConvert.HexToSDLColor("0000F0", items.rarity_color[3]);
	ColorConvert.HexToSDLColor("004000", items.rarity_color[4]);
	ColorConvert.HexToSDLColor("006000", items.rarity_color[5]);
	ColorConvert.HexToSDLColor("006000", items.rarity_color[6]);
/*
 *	not hugely sensible values
 *	but I want something I'm not using so I can test this
 */

	fonts.normal		= "comic.ttf";
	fonts.fixed		    = "comic.ttf";
	fonts.antialias		= true;

	debug.log_items		= false;
	debug.log_girls		= false;
	debug.log_rgirls	= false;
	debug.log_fonts		= false;
	debug.log_torture	= false;	
}

