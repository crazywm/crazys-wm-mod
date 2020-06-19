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
#include <ostream>
#include <cctype>
#include "sConfig.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include "utils/FileList.h"
#include <tinyxml2.h>
#include "interface/cColor.h"
#include "Revision.h"
#include "SDL.h"
#include "xml/util.h"
// TODO(refactor) get rid of this dependence!
#include "../game/Constants.h"

static cColor ColorConvert;
sConfigData *cConfig::data;

cConfig::cConfig()
{
    if (!data)
    {
        data = new sConfigData();
    }
}

/*
* changed this to take a filename so we can pass config files on the command line
* default is config.xml
*/
sConfigData::sConfigData(const char *a_filename)
    : fonts()
{

    DirPath dpdef = DirPath() << a_filename;    // `J` moved to root directory
    std::string filenamedef = dpdef.c_str();
    pXMLDocument doc;
    g_LogFile.info( "engine", "Attempting to load config file: ",filenamedef );
    try {
        doc = LoadXMLDocument(filenamedef);
    } catch( std::runtime_error& error ){
        g_LogFile.error("engine", "Could not load any config.xml files, using defaults.");
        g_LogFile.warning("engine", "*** Game will run with default pricing factors.\n*** This may seem a little easy. To fix this\n*** get a config.xml file from pinkpetal.org\n*** or make one with Whore Master Editor");
        set_defaults();
        return;
    }

    /*
    *    loop over the elements attached to the root
    */
    for (auto& el : IterateChildElements(*doc->RootElement()))
    {
        std::string tag = el.Value();        //    now, depending on the tag name...
        if (tag == "Folders")        { get_folders_data(&el);    continue; }
        if (tag == "Resolution")    { get_resolution_data(&el);    continue; }
        if (tag == "Initial")        { get_initial_values(&el);    continue; }
        if (tag == "Expenses")        { get_expense_factors(&el);    continue; }
        if (tag == "Prostitution")    { get_pros_factors(&el);    continue; }
        if (tag == "Catacombs")        { get_catacombs_data(&el);    continue; }
        if (tag == "Fonts")            { get_font_data(&el);        continue; }
        if (tag == "Debug")            { get_debug_flags(&el);        continue; }

        g_LogFile.warning("engine", "config.xml: tag: '", tag, "' unexpected");
    }
    ReadItemData();
    /// TODO add a way to specify this
    fonts.detailfontsize = 9;    // default to 9
}


void sConfigData::get_att(XMLElement *el, const char *name, bool &bval)
{
    const char *pt;

    pt = el->Attribute(name);
    /*
    *    if we didn't get a value, but it wasn't mandatory
    *    we can go
    */
    if (!pt) {
        g_LogFile.error("engine", "config.xml: No attribute named '", name, "' found.");
        return;
    }
    /*
    *    convert to a string, and then squash that to lower case
    */
    std::string s = pt;
    for (u_int i = 0; i < s.length(); i++) { s[i] = std::tolower(s[i]); }
    /*
    *    now we expect either "true" or "false"
    *    we can take "1" and "0" as well
    */
    if (s == "true" || s == "1")        { bval = true;    return; }
    if (s == "false" || s == "0")    { bval = false;    return; }

    g_LogFile.error("engine", "config.xml: : Binary attribute '", name, "': unexpected value '", s, "'");
}


void sConfigData::get_att(XMLElement *el, const char *name, double *dpt)
{
    if (el->QueryAttribute(name, dpt) == tinyxml2::XML_SUCCESS) { return; }
    g_LogFile.warning("engine", "config.xml: No '", name, "' attribute: defaulting to ", *dpt);
}

void sConfigData::get_att(XMLElement *el, const char *name, std::string &s)
{
    const char *pt;
    pt = el->Attribute(name);
    if (pt) { s = pt;    return; }
    g_LogFile.warning("engine", "config.xml: No '", name, "' attribute: defaulting to ", s);
}

void sConfigData::get_att(XMLElement *el, const char *name, int *ipt)
{
    int def_val = *ipt;
    if (el->QueryAttribute(name, ipt) == tinyxml2::XML_SUCCESS) { return; }
    g_LogFile.warning("engine", "config.xml: No '", name, "' attribute: defaulting to ", def_val);
    *ipt = def_val;
}

void sConfigData::get_folders_data(XMLElement *el)
{
    const char *pt;

    folders.backupsaves = false;
    folders.preferdefault = false;
    folders.characters        = (DirPath() << "Resources" << "Characters").c_str();
    folders.saves            = (DirPath() << "Saves").c_str();
    folders.items            = (DirPath() << "Resources" << "Items").c_str();
    folders.defaultimageloc    = (DirPath() << "Resources" << "DefaultImages").c_str();

    std::string testch, testsa, testdi, testil;
    if (pt = el->Attribute("Characters"))        get_att(el, "Characters", testch);
    if (pt = el->Attribute("Saves"))            get_att(el, "Saves", testsa);
    if (pt = el->Attribute("Items"))            get_att(el, "Items", testil);
    if (pt = el->Attribute("BackupSaves"))        get_att(el, "BackupSaves", folders.backupsaves);
    if (pt = el->Attribute("DefaultImages"))    get_att(el, "DefaultImages", testdi);
    if (pt = el->Attribute("PreferDefault"))    get_att(el, "PreferDefault", folders.preferdefault);

    if (!testch.empty())
    {
        DirPath abs_ch = DirPath(testch.c_str());
        DirPath rel_ch = DirPath() << testch;
        FileList abstest(abs_ch, "*.*girlsx");
        FileList reltest(rel_ch, "*.*girlsx");
        if (abstest.size() > 0)
        {
            folders.characters = abs_ch.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Characters from absolute location: ", folders.characters);
        }
        else if (reltest.size() > 0)
        {
            folders.characters = rel_ch.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Characters from relative location: ", folders.characters);
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: Characters folder '", testch, "' does not exist or has no girls in it.\n\tDefaulting to ./Resources/Characters");
        }
    }

    if (!testsa.empty())
    {
        DirPath abs_sa = DirPath(testsa.c_str());
        DirPath rel_sa = DirPath() << testsa;
        FILE *fp;
        DirPath testloc = DirPath(abs_sa) << ".Whore Master Save Games folder";
        if ((fp = fopen(testloc, "w")) != nullptr) fclose(fp);
        DirPath testlocrel = DirPath(rel_sa) << ".Whore Master Save Games folder";
        if ((fp = fopen(testlocrel, "w")) != nullptr) fclose(fp);
        FileList abstest(abs_sa, "*.*");
        FileList reltest(rel_sa, "*.*");

        if (abstest.size() > 0)
        {
            folders.saves = abs_sa.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Saves from absolute location: ", folders.saves);
        }
        else if (reltest.size() > 0)
        {
            folders.saves = rel_sa.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Saves from relative location: ", folders.saves);
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: Save game folder '", testsa, "' does not exist.\n\tDefaulting to ./Saves");
        }
    }
    if (!testil.empty())
    {
        DirPath abs_il = DirPath(testil.c_str());
        DirPath rel_il = DirPath() << testil;
        FileList abstest(abs_il, "*.itemsx");
        FileList reltest(rel_il, "*.itemsx");
        if (abstest.size() > 0)
        {
            folders.items = abs_il.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Items from absolute location: ", folders.items);
        }
        else if (reltest.size() > 0)
        {
            folders.items = rel_il.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Items from relative location: ", folders.items);
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: Items folder '", testil, "' does not exist or has no Items in it.\n\tDefaulting to ./Resources/Items");
        }
    }
    if (!testdi.empty())
    {
        DirPath abs_di = DirPath(testdi.c_str());
        DirPath rel_di = DirPath() << testdi;
        FileList abstest(abs_di, "*.*g"); abstest.add("*.ani"); abstest.add("*.gif");
        FileList reltest(rel_di, "*.*g"); reltest.add("*.ani"); reltest.add("*.gif");

        if (abstest.size() > 0)
        {
            folders.defaultimageloc = abs_di.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Default Images from absolute location: ", folders.defaultimageloc);
        }
        else if (reltest.size() > 0)
        {
            folders.defaultimageloc = rel_di.c_str();
            g_LogFile.info("engine", "Success: config.xml: Loading Default Images from relative location: ", folders.defaultimageloc);
        }
        else
        {
            g_LogFile.warning("engine", "Default Images folder '", testdi, "' does not exist or has no images in it.");
        }
    }
}

void sConfigData::get_resolution_data(XMLElement *el)
{
    resolution.configXML = false;
    const char *pt;
    std::string testa;
    if (pt = el->Attribute("Resolution"))        get_att(el, "Resolution", testa);
    if (!testa.empty())
    {
        DirPath location = DirPath() << "Resources" << "Interface" << testa;
        FileList test(location, "*.xml");
        if (test.size() > 0)
        {
            resolution.resolution = testa;
            g_LogFile.info("engine", "Success: config.xml: Loading Interface: ", location.c_str());
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: 'Resolution' attribute points to an invalid interface folder:\n\tDefaulting to 'J_1024x768'");
        }
    }
    else
    {
        g_LogFile.warning("engine", "config.xml: No Resolution specified, using defaults.");
    }
    if (pt = el->Attribute("Width"))            { get_att(el, "Width", &resolution.width);        resolution.configXML = true; }
    if (pt = el->Attribute("Height"))            { get_att(el, "Height", &resolution.height);    resolution.configXML = true; }
    if (pt = el->Attribute("FullScreen"))        { get_att(el, "FullScreen", resolution.fullscreen); }
    if (pt = el->Attribute("ListScrollAmount"))    { get_att(el, "ListScrollAmount", &resolution.list_scroll); }
    if (pt = el->Attribute("TextScrollAmount"))    { get_att(el, "TextScrollAmount", &resolution.text_scroll); }
    if (pt = el->Attribute("NextTurnEnter"))    { get_att(el, "NextTurnEnter", resolution.next_turn_enter); }

}

void sConfigData::get_initial_values(XMLElement *el)
{
    const char *pt;
    /// TODO redo loading of superceded settings
    // if (pt = el->Attribute("Gold"))                    get_att(el, "Gold", &initial.gold);
    if (pt = el->Attribute("GirlMeet"))                get_att(el, "GirlMeet", &initial.girl_meet);
    if (pt = el->Attribute("TortureTraitWeekMod"))    get_att(el, "TortureTraitWeekMod", &initial.torture_mod);
}

void sConfigData::get_expense_factors(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("Training"))                get_att(el, "Training", &out_fact.training);
    if (pt = el->Attribute("MovieCost"))            get_att(el, "MovieCost", &out_fact.movie_cost);
    if (pt = el->Attribute("ActressWages"))            get_att(el, "ActressWages", &out_fact.actress_wages);        // `J` ?not used?
    if (pt = el->Attribute("GoonWages"))            get_att(el, "GoonWages", &out_fact.goon_wages);
    if (pt = el->Attribute("MatronWages"))            get_att(el, "MatronWages", &out_fact.matron_wages);
    if (pt = el->Attribute("StaffWages"))            get_att(el, "StaffWages", &out_fact.staff_wages);            // `J` ?not used?
    if (pt = el->Attribute("GirlSupport"))            get_att(el, "GirlSupport", &out_fact.girl_support);            // `J` ?not used?
    if (pt = el->Attribute("Consumables"))            get_att(el, "Consumables", &out_fact.consumables);
    if (pt = el->Attribute("Items"))                get_att(el, "Items", &out_fact.item_cost);
    if (pt = el->Attribute("SlavesBought"))            get_att(el, "SlavesBought", &out_fact.slave_cost);
    if (pt = el->Attribute("BuyBrothel"))            get_att(el, "BuyBrothel", &out_fact.brothel_cost);
    if (pt = el->Attribute("BrothelSupport"))        get_att(el, "BrothelSupport", &out_fact.brothel_support);
    if (pt = el->Attribute("Bribes"))                get_att(el, "Bribes", &out_fact.bribes);                    // `J` ?not used?
    if (pt = el->Attribute("Fines"))                get_att(el, "Fines", &out_fact.fines);                        // `J` ?not used?
    if (pt = el->Attribute("Advertising"))            get_att(el, "Advertising", &out_fact.advertising);
}

void sConfigData::get_pros_factors(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("RapeStreet"))            get_att(el, "RapeStreet", &prostitution.rape_streets);
    if (pt = el->Attribute("RapeBrothel"))            get_att(el, "RapeBrothel", &prostitution.rape_brothel);
}

void sConfigData::get_catacombs_data(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("UniqueCatacombs"))        get_att(el, "UniqueCatacombs", &catacombs.unique_catacombs);
    // load them
    if (pt = el->Attribute("GirlGetsGirls"))        get_att(el, "GirlGetsGirls", &catacombs.girl_gets_girls);
    if (pt = el->Attribute("GirlGetsItems"))        get_att(el, "GirlGetsItems", &catacombs.girl_gets_items);
    if (pt = el->Attribute("GirlGetsBeast"))        get_att(el, "GirlGetsBeast", &catacombs.girl_gets_beast);
    if (pt = el->Attribute("GangGetsGirls"))        get_att(el, "GangGetsGirls", &catacombs.gang_gets_girls);
    if (pt = el->Attribute("GangGetsItems"))        get_att(el, "GangGetsItems", &catacombs.gang_gets_items);
    if (pt = el->Attribute("GangGetsBeast"))        get_att(el, "GangGetsBeast", &catacombs.gang_gets_beast);
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

void sConfigData::get_font_data(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("Normal"))            get_att(el, "Normal", fonts.normal);
    if (pt = el->Attribute("Fixed"))            get_att(el, "Fixed", fonts.fixed);
    if (pt = el->Attribute("Antialias"))        get_att(el, "Antialias", fonts.antialias);
    if (pt = el->Attribute("ShowPercent"))        get_att(el, "ShowPercent", fonts.showpercent);
}

void sConfigData::ReadItemData()
{
    // check interface for colors
    DirPath dpi = DirPath() << "Resources" << "Interface" << resolution.resolution << "InterfaceColors.xml";
    tinyxml2::XMLDocument doci;
    for (int i = 0; i<NUM_ITEM_RARITY; i++)
    {
        items.rarity_color[i] = new SDL_Color();
    }
    if (doci.LoadFile(dpi.c_str()) == tinyxml2::XML_SUCCESS)
    {
        for (auto& el : IterateChildElements(*doci.RootElement()))
        {
            std::string tag = el.Value();
            /// TODO move this to all the other interface stuff
            if (tag == "Color")
            {
                int r, g, b;
                if(el.QueryAttribute("R", &r) == tinyxml2::XML_SUCCESS && el.QueryAttribute("G", &g) == tinyxml2::XML_SUCCESS &&
                    el.QueryAttribute("B", &b) == tinyxml2::XML_SUCCESS) {
                    std::string name = el.Attribute("Name");
                    /* */if (name == "ItemRarity0") ColorConvert.RGBToSDLColor(items.rarity_color[0], r, g, b);
                    else if (name == "ItemRarity1") ColorConvert.RGBToSDLColor(items.rarity_color[1], r, g, b);
                    else if (name == "ItemRarity2") ColorConvert.RGBToSDLColor(items.rarity_color[2], r, g, b);
                    else if (name == "ItemRarity3") ColorConvert.RGBToSDLColor(items.rarity_color[3], r, g, b);
                    else if (name == "ItemRarity4") ColorConvert.RGBToSDLColor(items.rarity_color[4], r, g, b);
                    else if (name == "ItemRarity5") ColorConvert.RGBToSDLColor(items.rarity_color[5], r, g, b);
                    else if (name == "ItemRarity6") ColorConvert.RGBToSDLColor(items.rarity_color[6], r, g, b);
                    else if (name == "ItemRarity7") ColorConvert.RGBToSDLColor(items.rarity_color[7], r, g, b);
                    else if (name == "ItemRarity8") ColorConvert.RGBToSDLColor(items.rarity_color[8], r, g, b);
                } else {
                    g_LogFile.error("engine", "Error reading Color definition from '", dpi.c_str(), "': ",  el.GetLineNum());
                }

            }
        }
    }
}

void sConfigData::get_debug_flags(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("LogAll"))                get_att(el, "LogAll", debug.log_all);

    if (debug.log_all) debug.log_extra_details = debug.log_show_numbers = debug.log_all;
    else
    {
        if (pt = el->Attribute("LogExtraDetails"))    get_att(el, "LogExtraDetails", debug.log_extra_details);
        if (pt = el->Attribute("LogShowNumbers"))    get_att(el, "LogShowNumbers", debug.log_show_numbers);
    }
}

/*
* these just make sure the game has a playable
* set of factors in the event that the config
* file can't be found
*/
void sConfigData::set_defaults()
{
    folders.characters = "";                // `J` where the characters folder is located
    folders.saves = "";                        // `J` where the saves folder is located
    folders.items = "";                        // `J` where the items folder is located
    folders.backupsaves = false;            // `J` backup saves in the version folder incase moving to the next version breaks the save
    folders.defaultimageloc = "";            // `J` where the default images folder is located
    folders.preferdefault = false;            // `J` default images will be preferred over the alttype tree

    resolution.resolution = "J_1024x768";    // `J` I set this to my interface because that is the one I edit myself
    resolution.width = 1024;                // `J` added - Will be moved to interfaces
    resolution.height = 768;                // `J` added - Will be moved to interfaces
    resolution.fullscreen = false;            // `J` added - Will be moved to interfaces
    resolution.configXML = false;            // `J` added - Will be changed to interfaces
    resolution.list_scroll = 3;                // `Dagoth` added
    resolution.text_scroll = 3;                // `Dagoth` added
    resolution.next_turn_enter = false;        // `J` added - for `mjsmagalhaes`

    initial.girl_meet = 30;
    initial.torture_mod = 1;                // `J` added

    out_fact.training = 0.0;
    out_fact.movie_cost = 1.0;
    out_fact.goon_wages = 1.0;
    out_fact.matron_wages = 1.0;
    out_fact.consumables = 1.0;
    out_fact.item_cost = 1.0;
    out_fact.slave_cost = 1.0;
    out_fact.brothel_cost = 1.0;
    out_fact.brothel_support = 1.0;
    out_fact.advertising = 1.0;

    prostitution.rape_brothel = 1;
    prostitution.rape_streets = 5;

    catacombs.unique_catacombs = 50;
    catacombs.girl_gets_girls = 34;
    catacombs.girl_gets_items = 33;
    catacombs.girl_gets_beast = 33;
    catacombs.gang_gets_girls = 34;
    catacombs.gang_gets_items = 33;
    catacombs.gang_gets_beast = 33;

    ReadItemData();

    /*
    *    not hugely sensible values
    *    but I want something I'm not using so I can test this
    */
    fonts.normal = "comic.ttf";
    fonts.fixed = "comic.ttf";
    fonts.antialias = true;
    fonts.showpercent = false;
    fonts.detailfontsize = 9;

    debug.log_all = false;
    debug.log_extra_details = false;
    debug.log_show_numbers = false;
}
