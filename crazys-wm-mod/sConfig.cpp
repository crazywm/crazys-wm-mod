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
#include "Revision.h"
#include "Globals.h"


extern CLog g_LogFile;
static CLog &l = g_LogFile;
static cColor ColorConvert;
sConfigData *cConfig::data;

cConfig::cConfig()
{
	if (!data)
	{
		l.ss() << "Whore Master v" << g_MajorVersion << "." << g_MinorVersionA << g_MinorVersionB << "." << g_StableVersion << " BETA" << " Svn: " << svn_revision
			<< "\n------------------------------------------------------------------------------------------------------------------------\nLoading Default configuration variables"; 
		l.ssend();
		data = new sConfigData();
	}
}

void cConfig::reload(const char *filename)
{
	sConfigData *newd = new sConfigData(filename);
	*data = *newd;
}

/*
* changed this to take a filename so we can pass config files on the command line
* default is config.xml
*/
sConfigData::sConfigData(const char *a_filename)
	: fonts()
{

	DirPath dpnew = DirPath() << ".." << "config.xml";	// `J` Added to load user's config file first
	DirPath dpdef = DirPath() << a_filename;	// `J` moved to root directory
	DirPath dpold = DirPath() << "Resources" << "Data" << a_filename;
	string filenamenew = dpnew.c_str();
	string filenamedef = dpdef.c_str();
	string filenameold = dpold.c_str();
	l.write("Attempting to load config.xml file.");
	/*
	*	make sure we have something playable,
	*	even if the file doesn't load
	*/
	set_defaults();
	/*
	*	open the file - moan most eloqently in its absence
	*/
	TiXmlDocument docnew(filenamenew);
	TiXmlDocument doc(filenamedef);
	TiXmlDocument docold(filenameold);

	if (docnew.LoadFile())
	{
		l.ss() << "Loading Config file :  " << filenamenew << "  : " << endl; l.ssend();
		doc = docnew;
	}
	else if (doc.LoadFile())
	{
		l.ss() << "Loading Config file :  " << filenamedef << "  : " << endl; l.ssend();
	}
	else if (docold.LoadFile())
	{
		l.ss() << "Loading Config file :  " << filenameold << "  : " << endl; l.ssend();
		doc = docold;
	}
	else
	{
		l.ss() << "Could not load any config.xml files, using defaults." << endl; l.ssend();
		l.ss() << "*** Game will run with default pricing factors.\n*** This may seem a little easy. To fix this\n*** get a config.xml file from pinkpetal.org\n*** or make one with Whore Master Editor"; l.ssend();
		return;
	}

	/*
	*	get the docuement root
	*/
	TiXmlElement *el, *root_el = doc.RootElement();
	/*
	*	loop over the elements attached to the root
	*/
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		string tag = el->ValueStr();		//	now, depending on the tag name...
		if (el->ValueStr() == "Folders")		{ get_folders_data(el);		continue; }
		if (el->ValueStr() == "Resolution")		{ get_resolution_data(el);	continue; }
		if (el->ValueStr() == "Initial")		{ get_initial_values(el);	continue; }
		if (el->ValueStr() == "Income")			{ get_income_factors(el);	continue; }
		if (el->ValueStr() == "Expenses")		{ get_expense_factors(el);	continue; }
		if (el->ValueStr() == "Gambling")		{ get_gambling_factors(el);	continue; }
		if (el->ValueStr() == "Prostitution")	{ get_pros_factors(el);		continue; }
		if (el->ValueStr() == "Catacombs")		{ get_catacombs_data(el);	continue; }
		if (el->ValueStr() == "SlaveMarket")	{ get_slave_market_data(el);continue; }
		if (el->ValueStr() == "Pregnancy")		{ get_preg_factors(el);		continue; }
		if (el->ValueStr() == "Tax")			{ get_tax_factors(el);		continue; }
		if (el->ValueStr() == "Gangs")			{ get_gang_factors(el);		continue; }
		if (el->ValueStr() == "Items")			{ get_item_data(el);		continue; }
		if (el->ValueStr() == "Fonts")			{ get_font_data(el);		continue; }
		if (el->ValueStr() == "Debug")			{ get_debug_flags(el);		continue; }

		l.ss() << "\nWarning: config.xml: tag: '" << tag << "' unexpected"; l.ssend();
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
				/* */if (name == "ItemRarity0") ColorConvert.RGBToSDLColor(items.rarity_color[0], r, g, b);
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
	TiXmlDocument doct(dpt.c_str());
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
	if (!pt) {
		l.ss() << "Error: in config: No attribute named '" << name << "' found."; l.ssend();
		return;
	}
	/*
	*	convert to a string, and then squash that to lower case
	*/
	string s = pt;
	for (u_int i = 0; i < s.length(); i++) { s[i] = std::tolower(s[i]); }
	/*
	*	now we expect either "true" or "false"
	*	we can take "1" and "0" as well
	*/
	if (s == "true" || s == "1")		{ bval = true;	return; }
	if (s == "false" || s == "0")	{ bval = false;	return; }

	l.ss() << "Error in config: Binary attribute '" << name << "': unexpected value '" << s << "'"; l.ssend();
}


void sConfigData::get_att(TiXmlElement *el, const char *name, double *dpt)
{
	if (el->Attribute(name, dpt)) { return; }
	l.ss() << "\nWarning: config.xml: No '" << name << "' attribute: defaulting to " << *dpt; l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, string &s)
{
	const char *pt;
	pt = el->Attribute(name);
	if (pt) { s = pt;	return; }
	l.ss() << "\nWarning: config.xml: No '" << name << "' attribute: defaulting to " << s; l.ssend();
}

void sConfigData::get_att(TiXmlElement *el, const char *name, int *ipt)
{
	int def_val = *ipt;
	if (el->Attribute(name, ipt)) { return; }
	l.ss() << "\nWarning: config.xml: No '" << name << "' attribute: defaulting to " << def_val; l.ssend();
	*ipt = def_val;
}

void sConfigData::get_folders_data(TiXmlElement *el)
{
	const char *pt;
	folders.configXMLch = false;
	folders.configXMLsa = false; 
	folders.configXMLdi = false;
	folders.configXMLil = false;

	folders.backupsaves = false;
	folders.preferdefault = false;
	folders.characters		= (DirPath() << "Resources" << "Characters").c_str();
	folders.saves			= (DirPath() << "Saves").c_str();
	folders.items			= (DirPath() << "Resources" << "Items").c_str();
	folders.defaultimageloc	= (DirPath() << "Resources" << "DefaultImages").c_str();

	string testch = "", testsa = "", testdi = "", testil = "";
	if (pt = el->Attribute("Characters"))		get_att(el, "Characters", testch);
	if (pt = el->Attribute("Saves"))			get_att(el, "Saves", testsa);
	if (pt = el->Attribute("Items"))			get_att(el, "Items", testil);
	if (pt = el->Attribute("BackupSaves"))		get_att(el, "BackupSaves", folders.backupsaves);
	if (pt = el->Attribute("DefaultImages"))	get_att(el, "DefaultImages", testdi);
	if (pt = el->Attribute("PreferDefault"))	get_att(el, "PreferDefault", folders.preferdefault);

	if (testch != "")
	{
		DirPath abs_ch = DirPath(testch.c_str());
		DirPath rel_ch = DirPath() << testch;
		FileList abstest(abs_ch, "*.*girlsx");
		FileList reltest(rel_ch, "*.*girlsx");
		if (abstest.size() > 0)
		{
			folders.characters = abs_ch.c_str();
			folders.configXMLch = true;
			l.ss() << "Success: config.xml: Loading Characters from absolute location: " << folders.characters; l.ssend();
		}
		else if (reltest.size() > 0)
		{
			folders.characters = rel_ch.c_str();
			folders.configXMLch = true;
			l.ss() << "Success: config.xml: Loading Characters from relative location: " << folders.characters; l.ssend();
		}
		else
		{
			l.ss() << "\nWarning: config.xml: Characters folder '" << testch << "' does not exist or has no girls in it.\n\tDefaulting to ./Resources/Characters"; l.ssend();
		}
	}
	if (testsa != "")
	{
		DirPath abs_sa = DirPath(testsa.c_str());
		DirPath rel_sa = DirPath() << testsa;
		FILE *fp;
		DirPath testloc = DirPath(abs_sa) << ".Whore Master Save Games folder";
		if ((fp = fopen(testloc, "w")) != 0) fclose(fp);
		DirPath testlocrel = DirPath(rel_sa) << ".Whore Master Save Games folder";
		if ((fp = fopen(testlocrel, "w")) != 0) fclose(fp);
		FileList abstest(abs_sa, "*.*");
		FileList reltest(rel_sa, "*.*");

		if (abstest.size() > 0)
		{
			folders.saves = abs_sa.c_str();
			folders.configXMLsa = true;
			l.ss() << "Success: config.xml: Loading Saves from absolute location: " << folders.saves; l.ssend();
		}
		else if (reltest.size() > 0)
		{
			folders.saves = rel_sa.c_str();
			folders.configXMLsa = true;
			l.ss() << "Success: config.xml: Loading Saves from relative location: " << folders.saves; l.ssend();
		}
		else
		{
			l.ss() << "\nWarning: config.xml: Save game folder '" << testsa << "' does not exist.\n\tDefaulting to ./Saves"; l.ssend();
		}
	}
	if (testil != "")
	{
		DirPath abs_il = DirPath(testil.c_str());
		DirPath rel_il = DirPath() << testil;
		FileList abstest(abs_il, "*.itemsx");
		FileList reltest(rel_il, "*.itemsx");
		if (abstest.size() > 0)
		{
			folders.items = abs_il.c_str();
			folders.configXMLil = true;
			l.ss() << "Success: config.xml: Loading Items from absolute location: " << folders.items; l.ssend();
		}
		else if (reltest.size() > 0)
		{
			folders.items = rel_il.c_str();
			folders.configXMLil = true;
			l.ss() << "Success: config.xml: Loading Items from relative location: " << folders.items; l.ssend();
		}
		else
		{
			l.ss() << "\nWarning: config.xml: Items folder '" << testil << "' does not exist or has no Items in it.\n\tDefaulting to ./Resources/Items"; l.ssend();
		}
	}
	if (testdi != "")
	{
		DirPath abs_di = DirPath(testdi.c_str());
		DirPath rel_di = DirPath() << testdi;
		FileList abstest(abs_di, "*.*g"); abstest.add("*.ani"); abstest.add("*.gif");
		FileList reltest(rel_di, "*.*g"); reltest.add("*.ani"); reltest.add("*.gif");

		if (abstest.size() > 0)
		{
			folders.defaultimageloc = abs_di.c_str();
			folders.configXMLdi = true;
			l.ss() << "Success: config.xml: Loading Default Images from absolute location: " << folders.defaultimageloc; l.ssend();
		}
		else if (reltest.size() > 0)
		{
			folders.defaultimageloc = rel_di.c_str();
			folders.configXMLdi = true;
			l.ss() << "Success: config.xml: Loading Default Images from relative location: " << folders.defaultimageloc; l.ssend();
		}
		else
		{
			l.ss() << "\nWarning: config.xml: Default Images folder '" << testdi << "' does not exist or has no images in it."; l.ssend();
		}
	}
}

void sConfigData::get_resolution_data(TiXmlElement *el)
{
	resolution.configXML = false;
	const char *pt;
	string testa = "";
	if (pt = el->Attribute("Resolution"))		get_att(el, "Resolution", testa);
	if (testa != "")
	{
		DirPath location = DirPath() << "Resources" << "Interface" << testa;
		XMLFileList test(location, "*.xml");
		if (test.size() > 0)
		{
			resolution.resolution = testa;
			l.ss() << "Success: config.xml: Loading Interface: " << location.c_str(); l.ssend();
		}
		else
		{
			l.ss() << "\nWarning: config.xml:\n'Resolution' attribute points to an invalid interface folder:\n\tDefaulting to 'J_1024x768'\n \n"; l.ssend();
		}
	}
	else
	{
		l.ss() << "\nWarning: config.xml: No Resolution specified, using defaults."; l.ssend();
	}
	if (pt = el->Attribute("Width"))			{ get_att(el, "Width", &resolution.width);		resolution.configXML = true; }
	if (pt = el->Attribute("Height"))			{ get_att(el, "Height", &resolution.height);	resolution.configXML = true; }
	if (pt = el->Attribute("ScaleWidth"))		{ get_att(el, "ScaleWidth", &resolution.scalewidth); }
	if (pt = el->Attribute("ScaleHeight"))		{ get_att(el, "ScaleHeight", &resolution.scaleheight); }
	if (pt = el->Attribute("FullScreen"))		{ get_att(el, "FullScreen", resolution.fullscreen); }
	if (pt = el->Attribute("FixedScale"))		{ get_att(el, "FixedScale", resolution.fixedscale); }
	if (pt = el->Attribute("ListScrollAmount"))	{ get_att(el, "ListScrollAmount", &resolution.list_scroll); }
	if (pt = el->Attribute("TextScrollAmount"))	{ get_att(el, "TextScrollAmount", &resolution.text_scroll); }
	if (pt = el->Attribute("NextTurnEnter"))	{ get_att(el, "NextTurnEnter", resolution.next_turn_enter); }

}

void sConfigData::get_initial_values(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Gold"))					get_att(el, "Gold", &initial.gold);
	if (pt = el->Attribute("GirlMeet"))				get_att(el, "GirlMeet", &initial.girl_meet);
	if (pt = el->Attribute("GirlsHousePerc"))		get_att(el, "GirlsHousePerc", &initial.girls_house_perc);	// `J` added
	if (pt = el->Attribute("GirlsKeepTips"))		get_att(el, "GirlsKeepTips", initial.girls_keep_tips);	// `J` added
	if (pt = el->Attribute("SlavePayOutOfPocket"))	get_att(el, "SlavePayOutOfPocket", initial.slave_pay_outofpocket);	// `J` added
	if (pt = el->Attribute("SlaveHousePerc"))		get_att(el, "SlaveHousePerc", &initial.slave_house_perc);
	if (pt = el->Attribute("SlaveKeepTips"))		get_att(el, "SlaveKeepTips", initial.slave_keep_tips);	// `J` added
	if (pt = el->Attribute("GirlsAccom"))			get_att(el, "GirlsAccom", &initial.girls_accom);
	if (pt = el->Attribute("SlaveAccom"))			get_att(el, "SlaveAccom", &initial.slave_accom);
	if (pt = el->Attribute("AutoUseItems"))			get_att(el, "AutoUseItems", initial.auto_use_items);
	if (pt = el->Attribute("AutoCombatEquip"))		get_att(el, "AutoCombatEquip", initial.auto_combat_equip);	// `J` moved from items
	if (pt = el->Attribute("TortureTraitWeekMod"))	get_att(el, "TortureTraitWeekMod", &initial.torture_mod);
	if (pt = el->Attribute("HoroscopeType"))		get_att(el, "HoroscopeType", &initial.horoscopetype);
	if (pt = el->Attribute("AutoCreateMovies"))		get_att(el, "AutoCreateMovies", initial.autocreatemovies);

}

void sConfigData::get_income_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("ExtortionIncome"))		get_att(el, "ExtortionIncome", &in_fact.extortion);			// `J` ?not used?
	if (pt = el->Attribute("GirlsWorkBrothel"))		get_att(el, "GirlsWorkBrothel", &in_fact.brothel_work);		// `J` ?not used?
	if (pt = el->Attribute("GirlsWorkStreet"))		get_att(el, "GirlsWorkStreet", &in_fact.street_work);		// `J` ?not used?
	if (pt = el->Attribute("MovieIncome"))			get_att(el, "MovieIncome", &in_fact.movie_income);			// `J` ?not used?
	if (pt = el->Attribute("StripperIncome"))		get_att(el, "StripperIncome", &in_fact.stripper_work);		// `J` ?not used?
	if (pt = el->Attribute("BarmaidIncome"))		get_att(el, "BarmaidIncome", &in_fact.barmaid_work);		// `J` ?not used?
	if (pt = el->Attribute("SlaveSales"))			get_att(el, "SlaveSales", &in_fact.slave_sales);
	if (pt = el->Attribute("ItemSales"))			get_att(el, "ItemSales", &in_fact.item_sales);				// `J` ?not used?
	if (pt = el->Attribute("ClinicIncome"))			get_att(el, "ClinicIncome", &in_fact.clinic_income);		// `J` ?not used?
	if (pt = el->Attribute("ArenaIncome"))			get_att(el, "ArenaIncome", &in_fact.arena_income);			// `J` ?not used?
	if (pt = el->Attribute("FarmIncome"))			get_att(el, "FarmIncome", &in_fact.farm_income);			// `J` ?not used?
}

void sConfigData::get_expense_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("Training"))				get_att(el, "Training", &out_fact.training);
	if (pt = el->Attribute("MovieCost"))			get_att(el, "MovieCost", &out_fact.movie_cost);
	if (pt = el->Attribute("ActressWages"))			get_att(el, "ActressWages", &out_fact.actress_wages);		// `J` ?not used?
	if (pt = el->Attribute("GoonWages"))			get_att(el, "GoonWages", &out_fact.goon_wages);
	if (pt = el->Attribute("MatronWages"))			get_att(el, "MatronWages", &out_fact.matron_wages);
	if (pt = el->Attribute("StaffWages"))			get_att(el, "StaffWages", &out_fact.staff_wages);			// `J` ?not used?
	if (pt = el->Attribute("GirlSupport"))			get_att(el, "GirlSupport", &out_fact.girl_support);			// `J` ?not used?
	if (pt = el->Attribute("Consumables"))			get_att(el, "Consumables", &out_fact.consumables);
	if (pt = el->Attribute("Items"))				get_att(el, "Items", &out_fact.item_cost);
	if (pt = el->Attribute("SlavesBought"))			get_att(el, "SlavesBought", &out_fact.slave_cost);
	if (pt = el->Attribute("BuyBrothel"))			get_att(el, "BuyBrothel", &out_fact.brothel_cost);
	if (pt = el->Attribute("BrothelSupport"))		get_att(el, "BrothelSupport", &out_fact.brothel_support);
	if (pt = el->Attribute("BarSupport"))			get_att(el, "BarSupport", &out_fact.bar_cost);				// `J` ?not used?
	if (pt = el->Attribute("CasinoSupport"))		get_att(el, "CasinoSupport", &out_fact.casino_cost);		// `J` ?not used?
	if (pt = el->Attribute("Bribes"))				get_att(el, "Bribes", &out_fact.bribes);					// `J` ?not used?
	if (pt = el->Attribute("Fines"))				get_att(el, "Fines", &out_fact.fines);						// `J` ?not used?
	if (pt = el->Attribute("Advertising"))			get_att(el, "Advertising", &out_fact.advertising);
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
	/*
	*	we expect these in the format "25%" with the "%" being optional
	*	The trailing % will be chopped off by tinyxml,
	*	but we still need to divide by 100 to turn the numbers
	*	from a percentage to a floating point factor
	*/
	if (pt = el->Attribute("Rate"))					{ get_att(el, "Rate", &tax.rate);		tax.rate /= 100.0; }
	if (pt = el->Attribute("Minimum"))				{ get_att(el, "Minimum", &tax.minimum);	tax.minimum /= 100.0; }
	if (pt = el->Attribute("Laundry"))				{ get_att(el, "Laundry", &tax.laundry);	tax.laundry /= 100.0; }
}

void sConfigData::get_preg_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("PlayerChance"))			get_att(el, "PlayerChance", &pregnancy.player_chance);
	if (pt = el->Attribute("CustomerChance"))		get_att(el, "CustomerChance", &pregnancy.customer_chance);
	if (pt = el->Attribute("MonsterChance"))		get_att(el, "MonsterChance", &pregnancy.monster_chance);
	if (pt = el->Attribute("GoodSexFactor"))		get_att(el, "GoodSexFactor", &pregnancy.good_sex_factor);
	if (pt = el->Attribute("ChanceOfGirl"))			get_att(el, "ChanceOfGirl", &pregnancy.chance_of_girl);
	if (pt = el->Attribute("WeeksPregnant"))		get_att(el, "WeeksPregnant", &pregnancy.weeks_pregnant);
	if (pt = el->Attribute("WeeksMonsterP"))		get_att(el, "WeeksMonsterP", &pregnancy.weeks_monster_p);			// `J` added
	if (pt = el->Attribute("MiscarriageChance"))	get_att(el, "MiscarriageChance", &pregnancy.miscarriage_chance);	// `J` added
	if (pt = el->Attribute("MiscarriageMonster"))	get_att(el, "MiscarriageMonster", &pregnancy.miscarriage_monster);	// `J` added
	if (pt = el->Attribute("WeeksTillGrown"))		get_att(el, "WeeksTillGrown", &pregnancy.weeks_till_grown);
	if (pt = el->Attribute("CoolDown"))				get_att(el, "CoolDown", &pregnancy.cool_down);
	if (pt = el->Attribute("AntiPregFailure"))		get_att(el, "AntiPregFailure", &pregnancy.anti_preg_failure);
	if (pt = el->Attribute("MultiBirthChance"))		get_att(el, "MultiBirthChance", &pregnancy.multi_birth_chance);		// `J` added
	if (pregnancy.multi_birth_chance > 50)			pregnancy.multi_birth_chance = 50;									// `J` limited
}

void sConfigData::get_pros_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("RapeStreet"))			get_att(el, "RapeStreet", &prostitution.rape_streets);
	if (pt = el->Attribute("RapeBrothel"))			get_att(el, "RapeBrothel", &prostitution.rape_brothel);
}

void sConfigData::get_catacombs_data(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("UniqueCatacombs"))		get_att(el, "UniqueCatacombs", &catacombs.unique_catacombs);
	// load them
	if (pt = el->Attribute("ControlGirls"))			get_att(el, "ControlGirls", catacombs.control_girls);
	if (pt = el->Attribute("ControlGangs"))			get_att(el, "ControlGangs", catacombs.control_gangs);
	if (pt = el->Attribute("GirlGetsGirls"))		get_att(el, "GirlGetsGirls", &catacombs.girl_gets_girls);
	if (pt = el->Attribute("GirlGetsItems"))		get_att(el, "GirlGetsItems", &catacombs.girl_gets_items);
	if (pt = el->Attribute("GirlGetsBeast"))		get_att(el, "GirlGetsBeast", &catacombs.girl_gets_beast);
	if (pt = el->Attribute("GangGetsGirls"))		get_att(el, "GangGetsGirls", &catacombs.gang_gets_girls);
	if (pt = el->Attribute("GangGetsItems"))		get_att(el, "GangGetsItems", &catacombs.gang_gets_items);
	if (pt = el->Attribute("GangGetsBeast"))		get_att(el, "GangGetsBeast", &catacombs.gang_gets_beast);
	// make them positive
	if (catacombs.girl_gets_girls < 0) catacombs.girl_gets_girls = -catacombs.girl_gets_girls;
	if (catacombs.girl_gets_items < 0) catacombs.girl_gets_items = -catacombs.girl_gets_items;
	if (catacombs.girl_gets_beast < 0) catacombs.girl_gets_beast = -catacombs.girl_gets_beast;
	if (catacombs.gang_gets_girls < 0) catacombs.gang_gets_girls = -catacombs.gang_gets_girls;
	if (catacombs.gang_gets_items < 0) catacombs.gang_gets_items = -catacombs.gang_gets_items;
	if (catacombs.gang_gets_beast < 0) catacombs.gang_gets_beast = -catacombs.gang_gets_beast;
	// make them percents
	double checkggirl = catacombs.girl_gets_girls + catacombs.girl_gets_items + catacombs.girl_gets_beast;
	if (checkggirl == 0) catacombs.girl_gets_girls = catacombs.girl_gets_items = catacombs.girl_gets_beast = (100 / 3);
	else if (checkggirl != 100)
	{
		catacombs.girl_gets_girls = int((100.0 / checkggirl) * (double)catacombs.girl_gets_girls);
		catacombs.girl_gets_items = int((100.0 / checkggirl) * (double)catacombs.girl_gets_items);
		catacombs.girl_gets_beast = int(100.0 - (double)catacombs.girl_gets_girls - (double)catacombs.girl_gets_items);
	}
	double checkggang = catacombs.gang_gets_girls + catacombs.gang_gets_items + catacombs.gang_gets_beast;
	if (checkggang == 0) catacombs.gang_gets_girls = catacombs.gang_gets_items = catacombs.gang_gets_beast = (100 / 3);
	else if (checkggang != 100)
	{
		catacombs.gang_gets_girls = int((100.0 / checkggang) * (double)catacombs.gang_gets_girls);
		catacombs.gang_gets_items = int((100.0 / checkggang) * (double)catacombs.gang_gets_items);
		catacombs.gang_gets_beast = int(100.0 - (double)catacombs.gang_gets_girls - (double)catacombs.gang_gets_items);
	}									 
}

void sConfigData::get_slave_market_data(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("UniqueMarket"))			get_att(el, "UniqueMarket", &slavemarket.unique_market);
	if (pt = el->Attribute("SlavesNewWeeklyMin"))	get_att(el, "SlavesNewWeeklyMin", &slavemarket.slavesnewweeklymin);
	if (pt = el->Attribute("SlavesNewWeeklyMax"))	get_att(el, "SlavesNewWeeklyMax", &slavemarket.slavesnewweeklymax);
}

void sConfigData::get_gang_factors(TiXmlElement *el)
{
	const char *pt;
	if (pt = el->Attribute("MaxRecruitList"))		get_att(el, "MaxRecruitList", &gangs.max_recruit_list);
	if (pt = el->Attribute("StartRandom"))			get_att(el, "StartRandom", &gangs.start_random);
	if (pt = el->Attribute("StartBoosted"))			get_att(el, "StartBoosted", &gangs.start_boosted);
	if (pt = el->Attribute("InitMemberMin"))		get_att(el, "InitMemberMin", &gangs.init_member_min);
	if (pt = el->Attribute("InitMemberMax"))		get_att(el, "InitMemberMax", &gangs.init_member_max);
	if (pt = el->Attribute("ChanceRemoveUnwanted"))	get_att(el, "ChanceRemoveUnwanted", &gangs.chance_remove_unwanted);
	if (pt = el->Attribute("AddNewWeeklyMin"))		get_att(el, "AddNewWeeklyMin", &gangs.add_new_weekly_min);
	if (pt = el->Attribute("AddNewWeeklyMax"))		get_att(el, "AddNewWeeklyMax", &gangs.add_new_weekly_max);
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
	if (pt = el->Attribute("LogAll"))				get_att(el, "LogAll", debug.log_all);

	if (debug.log_all) debug.log_girls = debug.log_rgirls = debug.log_girl_fights = debug.log_items = debug.log_fonts = debug.log_torture = debug.log_debug = debug.log_extra_details = debug.log_show_numbers = debug.log_all;
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
		if (pt = el->Attribute("LogShowNumbers"))	get_att(el, "LogShowNumbers", debug.log_show_numbers);
	}
}

/*
* these just make sure the game has a playable
* set of factors in the event that the config
* file can't be found
*/
void sConfigData::set_defaults()
{
	folders.characters = "";				// `J` where the characters folder is located 
	folders.configXMLch = false;			// `J` if character's location is set in config.xml
	folders.saves = "";						// `J` where the saves folder is located 
	folders.configXMLsa = false;			// `J` if saves's location is set in config.xml
	folders.items = "";						// `J` where the items folder is located 
	folders.configXMLil = false;			// `J` if items's location is set in config.xml
	folders.backupsaves = false;			// `J` backup saves in the version folder incase moving to the next version breaks the save
	folders.defaultimageloc = "";			// `J` where the default images folder is located 
	folders.configXMLdi = false;			// `J` if default images location is set in config.xml
	folders.preferdefault = false;			// `J` default images will be preferred over the alttype tree

	resolution.resolution = "J_1024x768";	// `J` I set this to my interface because that is the one I edit myself
	resolution.width = 1024;				// `J` added - Will be moved to interfaces
	resolution.height = 768;				// `J` added - Will be moved to interfaces
	resolution.scalewidth = 800;			// `J` added - Will be moved to interfaces
	resolution.scaleheight = 600;			// `J` added - Will be moved to interfaces
	resolution.fullscreen = false;			// `J` added - Will be moved to interfaces
	resolution.fixedscale = false;			// `J` added for .06.02.39
	resolution.configXML = false;			// `J` added - Will be changed to interfaces
	resolution.list_scroll = 3;				// `Dagoth` added
	resolution.text_scroll = 3;				// `Dagoth` added
	resolution.next_turn_enter = false;		// `J` added - for `mjsmagalhaes`

	initial.gold = 4000;
	initial.girl_meet = 30;
	initial.girls_house_perc = 60;			// `J` added
	initial.slave_pay_outofpocket = true;	// `J` added
	initial.girls_keep_tips = true;			// `J` added
	initial.slave_house_perc = 100;
	initial.slave_keep_tips = false;		// `J` added
	initial.girls_accom = 5;
	initial.slave_accom = 1;
	initial.auto_use_items = false;
	initial.auto_combat_equip = true;
	initial.torture_mod = 1;				// `J` added
	initial.horoscopetype = 1;				// `J` added
	initial.autocreatemovies = true;		// `J` added

	in_fact.extortion = 1.0;				// `J` ?not used?
	in_fact.brothel_work = 1.0;				// `J` ?not used?
	in_fact.street_work = 1.0;				// `J` ?not used?
	in_fact.movie_income = 1.0;				// `J` ?not used?
	in_fact.stripper_work = 1.0;			// `J` ?not used?
	in_fact.barmaid_work = 1.0;				// `J` ?not used?
	in_fact.slave_sales = 1.0;
	in_fact.item_sales = 0.5;
	in_fact.clinic_income = 1.0;			// `J` ?not used?
	in_fact.arena_income = 1.0;				// `J` ?not used?
	in_fact.farm_income = 1.0;				// `J` ?not used?

	out_fact.training = 0.0;
	out_fact.movie_cost = 1.0;
	out_fact.actress_wages = 0.0;			// `J` ?not used?
	out_fact.goon_wages = 1.0;
	out_fact.matron_wages = 1.0;
	out_fact.staff_wages = 1.0;				// `J` ?not used?
	out_fact.girl_support = 1.0;			// `J` ?not used?
	out_fact.consumables = 1.0;
	out_fact.item_cost = 1.0;
	out_fact.slave_cost = 1.0;
	out_fact.brothel_cost = 1.0;
	out_fact.brothel_support = 1.0;
	out_fact.bar_cost = 1.0;				// `J` ?not used?
	out_fact.casino_cost = 1.0;				// `J` ?not used?
	out_fact.bribes = 1.0;					// `J` ?not used?
	out_fact.fines = 1.0;					// `J` ?not used?
	out_fact.advertising = 1.0;

	tax.rate = 0.06;
	tax.minimum = 0.01;
	tax.laundry = 0.25;

	gamble.odds = 49;
	gamble.base = 79;
	gamble.spread = 400;
	gamble.house_factor = 1.0;
	gamble.customer_factor = 0.5;

	pregnancy.player_chance = 8;
	pregnancy.customer_chance = 8;
	pregnancy.monster_chance = 8;
	pregnancy.good_sex_factor = 2.0;		// `J` changed from 8 to 2
	pregnancy.chance_of_girl = 50;
	pregnancy.weeks_pregnant = 38;
	pregnancy.weeks_monster_p = 20;			// `J` added
	pregnancy.miscarriage_chance = 0.1;		// `J` added
	pregnancy.miscarriage_monster = 1.0;	// `J` added
	pregnancy.weeks_till_grown = 60;
	pregnancy.cool_down = 4;				// `J` changed from 60 weeks to 4 weeks
	pregnancy.anti_preg_failure = 0.0;		// `J` added
	pregnancy.multi_birth_chance = 1.0;		// `J` added

	gangs.max_recruit_list = 6;
	gangs.start_random = 2;
	gangs.start_boosted = 2;
	gangs.init_member_min = 1;
	gangs.init_member_max = 10;
	gangs.chance_remove_unwanted = 25;
	gangs.add_new_weekly_min = 0;
	gangs.add_new_weekly_max = 2;

	prostitution.rape_brothel = 1;
	prostitution.rape_streets = 5;

	catacombs.unique_catacombs = 50;
	catacombs.control_girls = false;
	catacombs.control_gangs = false;
	catacombs.girl_gets_girls = 34;
	catacombs.girl_gets_items = 33;
	catacombs.girl_gets_beast = 33;
	catacombs.gang_gets_girls = 34;
	catacombs.gang_gets_items = 33;
	catacombs.gang_gets_beast = 33;

	slavemarket.unique_market = 35;
	slavemarket.slavesnewweeklymin = 5;
	slavemarket.slavesnewweeklymax = 12;

	for (int i = 0; i<9; i++)
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
	fonts.normal = "comic.ttf";
	fonts.fixed = "comic.ttf";
	fonts.antialias = true;
	fonts.showpercent = false;
	fonts.detailfontsize = 9;

	debug.log_all = false;
	debug.log_girls = false;
	debug.log_rgirls = false;
	debug.log_girl_fights = false;
	debug.log_items = false;
	debug.log_fonts = false;
	debug.log_torture = false;
	debug.log_debug = false;
	debug.log_extra_details = false;
	debug.log_show_numbers = false;
}
