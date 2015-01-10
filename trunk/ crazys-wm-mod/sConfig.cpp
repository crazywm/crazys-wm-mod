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
#include <iostream>
#include <ostream>
#include <fstream>
#include <cctype>
#include "sConfig.h"
#include "DirPath.h"
#include "CLog.h"
#include "FileList.h"
#include "tinyxml.h"
#include "cColor.h"
#include "Constants.h"
#include "XmlUtil.h"


extern CLog g_LogFile;
static CLog &l = g_LogFile;
static cColor ColorConvert;
sConfigData *cConfig::data ;

/*
 * changed this to take a filename so we can pass config files on the command line
 * default is config.xml
 */
sConfigData::sConfigData(const char *a_filename)
: fonts()
{
	DirPath dp = DirPath() << a_filename;	// `J` moved to root directory
	DirPath dpold = DirPath() << "Resources" << "Data" << a_filename;
	string filename = dp.c_str();
	string filenameold = dpold.c_str();
	l.ss() << "Loading configration variables from '" << filename << "'";
	l.ssend();
/*
 *	make sure we have something playable,
 *	even if the file doesn't load
 */
	set_defaults();
/*
 *	open the file - moan most eloqently in its absence
 */
	TiXmlDocument doc(filename);
	TiXmlDocument docold(filenameold);
	if (!doc.LoadFile())
	{
		l.ss() << "Can't load " << filename << " from root directory." << endl;
		l.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		l.ss() << "Attempting to load old file " << filenameold << "." << endl;
		l.ssend();
		doc = docold;
	}
	if (!doc.LoadFile())
	{
		l.ss() << "can't load " << filename << endl;
		l.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc();
		l.ssend();
/*
 *		a bit of narrative for the players: makes it easier to tell
 *		if the config isn't being found
 */
		l.ss() << "*** Game will run with default pricing factors." << endl
			<< "*** This may seem a little easy. To fix this" << endl
			<< "*** get a config.xml file from pinkpetal.org" << endl
			<< "*** or make one with Whore Master Editor";
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
	for(el = root_el->FirstChildElement() ; el ; el = el->NextSiblingElement() ) 
	{
		string tag = el->ValueStr();		//	now, depending on the tag name...
		if (el->ValueStr() == "Resolution")		{ get_resolution_data(el);	continue; }
		if (el->ValueStr() == "Initial")		{ get_initial_values(el);	continue; }
		if (el->ValueStr() == "Income")			{ get_income_factors(el);	continue; }
		if (el->ValueStr() == "Expenses")		{ get_expense_factors(el);	continue; }
		if (el->ValueStr() == "Gambling")		{ get_gambling_factors(el);	continue; }
		if (el->ValueStr() == "Prostitution")	{ get_pros_factors(el);		continue; }
		if (el->ValueStr() == "Pregnancy")		{ get_preg_factors(el);		continue; }
		if (el->ValueStr() == "Tax")			{ get_tax_factors(el);		continue; }
		if (el->ValueStr() == "Gangs")			{ get_gang_factors(el);		continue; }
		if (el->ValueStr() == "Items")			{ get_item_data(el);		continue; }
		if (el->ValueStr() == "Fonts")			{ get_font_data(el);		continue; }
		if (el->ValueStr() == "Debug")			{ get_debug_flags(el);		continue; }

		l.ss() << "Warning: config.xml: tag: '" << tag << "' unexpected";
		l.ssend();
	}
	// check interface for colors
	DirPath dpi = DirPath() << "Resources" << "Interface" << resolution.resolution << "InterfaceColors.xml";
	TiXmlDocument doci(dpi.c_str());
	if (doci.LoadFile())
	{
		string m_filename = dpi.c_str();
		TiXmlElement *el, *root_el = doci.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			string tag = el->ValueStr();
			if (tag == "Color")
			{
				XmlUtil xu(m_filename); string name; int r, g, b;
				xu.get_att(el, "Name", name); xu.get_att(el, "R", r); xu.get_att(el, "G", g); xu.get_att(el, "B", b);
				 if (name == "ItemRarity0") ColorConvert.RGBToSDLColor(items.rarity_color[0], r, g, b);
			else if (name == "ItemRarity1") ColorConvert.RGBToSDLColor(items.rarity_color[1], r, g, b);
			else if (name == "ItemRarity2") ColorConvert.RGBToSDLColor(items.rarity_color[2], r, g, b);
			else if (name == "ItemRarity3") ColorConvert.RGBToSDLColor(items.rarity_color[3], r, g, b);
			else if (name == "ItemRarity4") ColorConvert.RGBToSDLColor(items.rarity_color[4], r, g, b);
			else if (name == "ItemRarity5") ColorConvert.RGBToSDLColor(items.rarity_color[5], r, g, b);
			else if (name == "ItemRarity6") ColorConvert.RGBToSDLColor(items.rarity_color[6], r, g, b);
			else if (name == "ItemRarity7") ColorConvert.RGBToSDLColor(items.rarity_color[7], r, g, b);
			else if (name == "ItemRarity8") ColorConvert.RGBToSDLColor(items.rarity_color[8], r, g, b);

			}
		}
	}
	fonts.detailfontsize = 9;	// default to 9 then check if it is set in girl_details_screen.xml
	DirPath dpt = DirPath() << "Resources" << "Interface" << resolution.resolution << "girl_details_screen.xml";
	TiXmlDocument doct(dp.c_str());
	if (doct.LoadFile())
	{
		string m_filename = dpt.c_str();
		TiXmlElement *el, *root_el = doct.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			string tag = el->ValueStr();
			if (tag == "Text")
			{
				XmlUtil xu(m_filename); string name; int f = 9;
				xu.get_att(el, "Name", name); xu.get_att(el, "FontSize", f, true);
				if (name == "GirlDescription" && f > 0)
				{
					fonts.detailfontsize = f; break;
				}
			}
		}
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
		g_LogFile.ss() << "Error: in config: No attribute named '" << name << "' found.";
		g_LogFile.ssend();
		return;
	}
/*
 *	convert to a string, and then squash that to lower case
 */
	string s = pt;
	for(u_int i = 0; i < s.length(); i++) {	s[i] = std::tolower(s[i]);	}
/*
 *	now we expect either "true" or "false"
 *	we can take "1" and "0" as well
 */
	if(s == "true" || s == "1")		{	bval = true;	return;	}
	if(s == "false" || s == "0")	{	bval = false;	return;	}

	g_LogFile.ss() << "Error in config: Binary attribute '" << name << "': unexpected value '" << s << "'";
	g_LogFile.ssend();
}


void sConfigData::get_att(TiXmlElement *el, const char *name, double *dpt)
{
	if (el->Attribute(name, dpt)) { return; }
	l.ss() << "Warning: config.xml: No '" << name << "' attribute: defaulting to " << *dpt;
	l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, string &s)
{
	const char *pt;
	pt = el->Attribute(name);
	if (pt) { s = pt;	return; }
	l.ss() << "Warning: config.xml: No '" << name << "' attribute: defaulting to " << s;
	l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, int *ipt)
{
	int def_val = *ipt;
	if(el->Attribute(name, ipt)) {	return;	}
	l.ss() << "Warning: config.xml: No '" << name << "' attribute: defaulting to " << def_val;
	l.ssend();
	*ipt = def_val;
}

void sConfigData::get_resolution_data(TiXmlElement *el)
{
	resolution.configXML = false;
	const char *pt;
	string testa = "", resfound = "";
	if (pt = el->Attribute("Resolution"))		get_att(el, "Resolution", testa);
	if (testa != "")
	{
		DirPath location = DirPath() << "Resources" << "Interface" << testa;
		XMLFileList test(location, "*.xml");
		if (test.size() > 0)
		{
			resolution.resolution = testa;
			resfound = location.c_str();
		}
	}
	if (resfound == "")
	{
		l.ss() << "\n\nWarning: config.xml:\n'Resolution' attribute points to an invalid interface folder:\ndefaulting to 'J_1024x768'\n\n";
		l.ssend();
	}
	else
	{
		l.ss() << "Success: config.xml: Loading Interface: " << resfound;
		l.ssend();

	}
	if (pt = el->Attribute("Width"))			{	get_att(el, "Width",		&resolution.width);		resolution.configXML = true; }
	if (pt = el->Attribute("Height"))			{	get_att(el, "Height",		&resolution.height);	resolution.configXML = true; }
	if (pt = el->Attribute("ScaleWidth"))		{	get_att(el, "ScaleWidth",	&resolution.scalewidth); }
	if (pt = el->Attribute("ScaleHeight"))		{	get_att(el, "ScaleHeight",	&resolution.scaleheight); }
	if (pt = el->Attribute("FullScreen"))		{	get_att(el, "FullScreen",	resolution.fullscreen); }
	if (pt = el->Attribute("ListScrollAmount"))	{	get_att(el, "ListScrollAmount", &resolution.list_scroll); }	else { resolution.list_scroll = 3; }
	if (pt = el->Attribute("TextScrollAmount"))	{	get_att(el, "TextScrollAmount", &resolution.text_scroll); }	else { resolution.text_scroll = 3; }
}

void sConfigData::get_initial_values(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Gold"))					get_att(el, "Gold",					&initial.gold);
	if (pt = el->Attribute("GirlMeet"))				get_att(el, "GirlMeet",				&initial.girl_meet);
	if (pt = el->Attribute("GirlsHousePerc"))		get_att(el, "GirlsHousePerc",		&initial.girls_house_perc);	// `J` added
	if (pt = el->Attribute("GirlsKeepTips"))		get_att(el, "GirlsKeepTips",		initial.girls_keep_tips);	// `J` added
	if (pt = el->Attribute("SlavePayOutOfPocket"))	get_att(el, "SlavePayOutOfPocket",	initial.slave_pay_outofpocket);	// `J` added
	if (pt = el->Attribute("SlaveHousePerc"))		get_att(el, "SlaveHousePerc",		&initial.slave_house_perc);
	if (pt = el->Attribute("SlaveKeepTips"))		get_att(el, "SlaveKeepTips",		initial.slave_keep_tips);	// `J` added
	if (pt = el->Attribute("AutoUseItems"))			get_att(el, "AutoUseItems",			initial.auto_use_items);
	if (pt = el->Attribute("AutoCombatEquip"))		get_att(el, "AutoCombatEquip",		initial.auto_combat_equip);	// `J` moved from items
	if (pt = el->Attribute("TortureTraitWeekMod"))	get_att(el, "TortureTraitWeekMod",	&initial.torture_mod);
}

void sConfigData::get_income_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("ExtortionIncome"))		get_att(el, "ExtortionIncome",		&in_fact.extortion);
	if (pt = el->Attribute("GirlsWorkBrothel"))		get_att(el, "GirlsWorkBrothel",		&in_fact.brothel_work);
	if (pt = el->Attribute("GirlsWorkStreet"))		get_att(el, "GirlsWorkStreet",		&in_fact.street_work);
	if (pt = el->Attribute("MovieIncome"))			get_att(el, "MovieIncome",			&in_fact.movie_income);
	if (pt = el->Attribute("StripperIncome"))		get_att(el, "StripperIncome",		&in_fact.stripper_work);
	if (pt = el->Attribute("BarmaidIncome"))		get_att(el, "BarmaidIncome",		&in_fact.barmaid_work);
	if (pt = el->Attribute("SlaveSales"))			get_att(el, "SlaveSales",			&in_fact.slave_sales);
	if (pt = el->Attribute("ItemSales"))			get_att(el, "ItemSales",			&in_fact.item_sales);
	if (pt = el->Attribute("ClinicIncome"))			get_att(el, "ClinicIncome",			&in_fact.clinic_income);
	if (pt = el->Attribute("ArenaIncome"))			get_att(el, "ArenaIncome",			&in_fact.arena_income);
}

void sConfigData::get_expense_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Training"))				get_att(el, "Training",				&out_fact.training);
	if (pt = el->Attribute("MovieCost"))			get_att(el, "MovieCost",			&out_fact.actress_wages);
	if (pt = el->Attribute("ActressWages"))			get_att(el, "ActressWages",			&out_fact.movie_cost);
	if (pt = el->Attribute("GoonWages"))			get_att(el, "GoonWages",			&out_fact.goon_wages);
	if (pt = el->Attribute("MatronWages"))			get_att(el, "MatronWages",			&out_fact.matron_wages);
	if (pt = el->Attribute("StaffWages"))			get_att(el, "StaffWages",			&out_fact.staff_wages);		// `J` ?not used?
	if (pt = el->Attribute("GirlSupport"))			get_att(el, "GirlSupport",			&out_fact.girl_support);
	if (pt = el->Attribute("Consumables"))			get_att(el, "Consumables",			&out_fact.consumables);
	if (pt = el->Attribute("Items"))				get_att(el, "Items",				&out_fact.item_cost);
	if (pt = el->Attribute("SlavesBought"))			get_att(el, "SlavesBought",			&out_fact.slave_cost);
	if (pt = el->Attribute("BuyBrothel"))			get_att(el, "BuyBrothel",			&out_fact.brothel_cost);
	if (pt = el->Attribute("BrothelSupport"))		get_att(el, "BrothelSupport",		&out_fact.brothel_support);	// `J` ?not used?
	if (pt = el->Attribute("BarSupport"))			get_att(el, "BarSupport",			&out_fact.bar_cost);		// `J` ?not used?
	if (pt = el->Attribute("CasinoSupport"))		get_att(el, "CasinoSupport",		&out_fact.casino_cost);		// `J` ?not used?
	if (pt = el->Attribute("Bribes"))				get_att(el, "Bribes",				&out_fact.bribes);
	if (pt = el->Attribute("Fines"))				get_att(el, "Fines",				&out_fact.fines);
	if (pt = el->Attribute("Advertising"))			get_att(el, "Advertising",			&out_fact.advertising);
}

void sConfigData::get_gambling_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Odds"))					get_att(el, "Odds", &gamble.odds);
	if (pt = el->Attribute("Base"))					get_att(el, "Base", &gamble.base);
	if (pt = el->Attribute("Spread"))				get_att(el, "Spread", &gamble.spread);
	/*
	*	strictly, we only need one of these:
	*	a customer factor OR a house factor
	*	but I thought it might be easier to explain
	*	if each had its own multiplier
	*/
	if (pt = el->Attribute("CustomerFactor"))		get_att(el, "CustomerFactor", &gamble.customer_factor);
	if (pt = el->Attribute("HouseFactor"))			get_att(el, "HouseFactor", &gamble.house_factor);
}

void sConfigData::get_tax_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Rate"))					get_att(el, "Rate",					&tax.rate);
	if (pt = el->Attribute("Minimum"))				get_att(el, "Minimum",				&tax.minimum);
	if (pt = el->Attribute("Laundry"))				get_att(el, "Laundry",				&tax.laundry);
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

void sConfigData::get_preg_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("PlayerChance"))			get_att(el, "PlayerChance",			&pregnancy.player_chance);
	if (pt = el->Attribute("CustomerChance"))		get_att(el, "CustomerChance",		&pregnancy.customer_chance);
	if (pt = el->Attribute("MonsterChance"))		get_att(el, "MonsterChance",		&pregnancy.monster_chance);
	if (pt = el->Attribute("GoodSexFactor"))		get_att(el, "GoodSexFactor",		&pregnancy.good_sex_factor);
	if (pt = el->Attribute("ChanceOfGirl"))			get_att(el, "ChanceOfGirl",			&pregnancy.chance_of_girl);
	if (pt = el->Attribute("WeeksPregnant"))		get_att(el, "WeeksPregnant",		&pregnancy.weeks_pregnant);
	if (pt = el->Attribute("WeeksMonsterP"))		get_att(el, "WeeksMonsterP",		&pregnancy.weeks_monster_p);	// `J` added
	if (pt = el->Attribute("MiscarriageChance"))	get_att(el, "MiscarriageChance",	&pregnancy.miscarriage_chance);	// `J` added
	if (pt = el->Attribute("MiscarriageMonster"))	get_att(el, "MiscarriageMonster",	&pregnancy.miscarriage_monster);	// `J` added
	if (pt = el->Attribute("WeeksTillGrown"))		get_att(el, "WeeksTillGrown",		&pregnancy.weeks_till_grown);
	if (pt = el->Attribute("CoolDown"))				get_att(el, "CoolDown",				&pregnancy.cool_down);
}

void sConfigData::get_pros_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("RapeStreet"))		get_att(el, "RapeStreet",	&prostitution.rape_streets);
	if (pt = el->Attribute("RapeBrothel"))		get_att(el, "RapeBrothel",	&prostitution.rape_brothel);
}

void sConfigData::get_gang_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("MaxRecruitList"))		get_att(el, "MaxRecruitList",		&gangs.max_recruit_list);
	if (pt = el->Attribute("StartRandom"))			get_att(el, "StartRandom",			&gangs.start_random);
	if (pt = el->Attribute("StartBoosted"))			get_att(el, "StartBoosted",			&gangs.start_boosted);
	if (pt = el->Attribute("InitMemberMin"))		get_att(el, "InitMemberMin",		&gangs.init_member_min);
	if (pt = el->Attribute("InitMemberMax"))		get_att(el, "InitMemberMax",		&gangs.init_member_max);
	if (pt = el->Attribute("ChanceRemoveUnwanted"))	get_att(el, "ChanceRemoveUnwanted", &gangs.chance_remove_unwanted);
	if (pt = el->Attribute("AddNewWeeklyMin"))		get_att(el, "AddNewWeeklyMin",		&gangs.add_new_weekly_min);
	if (pt = el->Attribute("AddNewWeeklyMax"))		get_att(el, "AddNewWeeklyMax",		&gangs.add_new_weekly_max);
}

// `J` get_item_data will be obsolete for .06 after Rarity Colors are moved to interfaces.
void sConfigData::get_item_data(TiXmlElement *el)
{
	const char *pt;
	// `J` AutoCombatEquip has been moved to initial. Kept here for legacy.
	if (pt = el->Attribute("AutoCombatEquip"))	get_att(el, "AutoCombatEquip", initial.auto_combat_equip);

	// `J` Rarity Colors will be moved to interfaces. Kept here for legacy.
	for (int i = 0; i<NUM_ITEM_RARITY; i++)
	{
		string ColorIn;
		stringstream ss;
		ss.str("");
		ss << "RarityColor" << i;
		if (pt = el->Attribute(ss.str().c_str()))	get_att(el, ss.str().c_str(), ColorIn);
		{
			get_att(el, ss.str().c_str(), ColorIn);
			ColorConvert.HexToSDLColor(ColorIn, items.rarity_color[i]);
		}
	}
}

void sConfigData::get_font_data(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Normal"))			get_att(el, "Normal", fonts.normal);
	if (pt = el->Attribute("Fixed"))			get_att(el, "Fixed", fonts.fixed);
	if (pt = el->Attribute("Antialias"))		get_att(el, "Antialias", fonts.antialias);
	if (pt = el->Attribute("ShowPercent"))		get_att(el, "ShowPercent", fonts.showpercent);
}

void sConfigData::get_debug_flags(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("LogAll"))				get_att(el, "LogItems", debug.log_all);

	if (debug.log_all) debug.log_girls = debug.log_rgirls = debug.log_girl_fights = debug.log_items = debug.log_fonts = debug.log_torture = debug.log_debug = debug.log_extra_details = debug.log_all;
	else 
	{
		if (pt = el->Attribute("LogGirls"))			get_att(el, "LogGirls", debug.log_girls);
		if (pt = el->Attribute("LogRGirls"))		get_att(el, "LogRGirls", debug.log_rgirls);
		if (pt = el->Attribute("LogGirlFights"))	get_att(el, "LogGirlFights", debug.log_girl_fights);
		if (pt = el->Attribute("LogItems"))			get_att(el, "LogItems", debug.log_items);
		if (pt = el->Attribute("LogFonts"))			get_att(el, "LogFonts", debug.log_fonts);
		if (pt = el->Attribute("LogTorture"))		get_att(el, "LogTorture", debug.log_torture);
		if (pt = el->Attribute("LogDebug"))			get_att(el, "LogDebug", debug.log_debug);
		if (pt = el->Attribute("LogExtraDetails"))	get_att(el, "LogExtraDetails", debug.log_extra_details);
	}
}

/*
 * these just make sure the game has a playable
 * set of factors in the event that the config
 * file can't be found
 */
void sConfigData::set_defaults()
{
	resolution.resolution = "J_1024x768";	// `J` I set this to my interface because that is the one I edit myself
	resolution.width				= 1024;	// `J` added - Will be moved to interfaces
	resolution.height				= 768;	// `J` added - Will be moved to interfaces
	resolution.scalewidth			= 800;	// `J` added - Will be moved to interfaces
	resolution.scaleheight			= 600;	// `J` added - Will be moved to interfaces
	resolution.fullscreen			= false;	// `J` added - Will be moved to interfaces
	resolution.configXML			= false;	// `J` added - Will be changed to interfaces

	initial.gold					= 4000;
	initial.girl_meet				= 30;
	initial.girls_house_perc		= 60;	// `J` added
	initial.slave_pay_outofpocket	= true;	// `J` added
	initial.girls_keep_tips			= true;	// `J` added
	initial.slave_house_perc		= 100;
	initial.slave_keep_tips			= false;	// `J` added
	initial.auto_use_items			= false;
	initial.auto_combat_equip		= true;

	initial.torture_mod				= 1;	// `J` added

	in_fact.extortion				= 1.0;
	in_fact.brothel_work			= 1.0;
	in_fact.street_work				= 1.0;
	in_fact.movie_income			= 1.0;
	in_fact.stripper_work			= 1.0;
	in_fact.barmaid_work			= 1.0;
	in_fact.slave_sales				= 1.0;
	in_fact.item_sales				= 1.0;
	in_fact.clinic_income			= 1.0;
	in_fact.arena_income			= 1.0;
	
    out_fact.training				= 0.0;
	out_fact.actress_wages			= 0.0;
	out_fact.movie_cost				= 1.0;
	out_fact.goon_wages				= 1.0;
	out_fact.matron_wages			= 1.0;
	out_fact.staff_wages			= 1.0;	// `J` ?not used?
	out_fact.girl_support			= 1.0;
	out_fact.consumables			= 1.0;
	out_fact.item_cost				= 1.0;
	out_fact.slave_cost				= 1.0;
	out_fact.brothel_cost			= 1.0;
	out_fact.brothel_support		= 1.0;	// `J` ?not used?
	out_fact.bar_cost				= 1.0;	// `J` ?not used?
	out_fact.casino_cost			= 1.0;	// `J` ?not used?
	out_fact.bribes					= 1.0;
	out_fact.fines					= 1.0;
	out_fact.advertising			= 1.0;
	
    tax.rate						= 0.06;
	tax.minimum						= 0.01;
	tax.laundry						= 0.25;
	
    gamble.odds						= 49;
	gamble.base						= 79;
	gamble.spread					= 400;
	gamble.house_factor				= 1.0;
	gamble.customer_factor			= 0.5;
	
    pregnancy.player_chance			= 8;
	pregnancy.customer_chance		= 8;
	pregnancy.monster_chance		= 8;
	pregnancy.good_sex_factor		= 2.0;	// `J` changed from 8 to 2
	pregnancy.chance_of_girl		= 50;
	pregnancy.weeks_pregnant		= 38;
	pregnancy.weeks_monster_p		= 20;	// `J` added
	pregnancy.miscarriage_chance	= 0.1;	// `J` added
	pregnancy.miscarriage_monster	= 1.0;	// `J` added
	pregnancy.weeks_till_grown		= 60;
	pregnancy.cool_down				= 4;	// `J` changed from 60 weeks to 4 weeks

	gangs.max_recruit_list			= 6;
	gangs.start_random				= 2;
	gangs.start_boosted				= 2;
	gangs.init_member_min			= 1;
	gangs.init_member_max			= 10;
	gangs.chance_remove_unwanted	= 25;
	gangs.add_new_weekly_min		= 0;
	gangs.add_new_weekly_max		= 2;

	prostitution.rape_brothel		= 1;
	prostitution.rape_streets		= 5;


	for(int i=0; i<9; i++)
	{
		items.rarity_color[i] = new SDL_Color();
	}
	ColorConvert.HexToSDLColor("000000", items.rarity_color[0]);	//000000
	ColorConvert.HexToSDLColor("000066", items.rarity_color[1]);	//000066
	ColorConvert.HexToSDLColor("0000cc", items.rarity_color[2]);	//0000cc
	ColorConvert.HexToSDLColor("0066ff", items.rarity_color[3]);	//0066ff
	ColorConvert.HexToSDLColor("8f0000", items.rarity_color[4]);	//8f0000
	ColorConvert.HexToSDLColor("00ff00", items.rarity_color[5]);	//00ff00
	ColorConvert.HexToSDLColor("008f00", items.rarity_color[6]);	//008f00
	ColorConvert.HexToSDLColor("a00000", items.rarity_color[7]);	//a00000
	ColorConvert.HexToSDLColor("e00000", items.rarity_color[8]);	//e00000
/*
 *	not hugely sensible values
 *	but I want something I'm not using so I can test this
 */
	fonts.normal			= "comic.ttf";
	fonts.fixed				= "comic.ttf";
	fonts.antialias			= true;
	fonts.showpercent		= false;
	fonts.detailfontsize	= 9;

	debug.log_all			= false;
	debug.log_girls			= false;
	debug.log_rgirls		= false;
	debug.log_girl_fights	= false;
	debug.log_items			= false;
	debug.log_fonts			= false;
	debug.log_torture		= false;
	debug.log_debug			= false;
	debug.log_extra_details = false;

}

