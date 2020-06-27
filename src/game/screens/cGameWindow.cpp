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

#include "cInventory.h"
#include "cGameWindow.h"
#include "interface/cWindowManager.h"
#include "Game.hpp"
#include "buildings/cBrothel.h"
#include "CLog.h"
#include "widgets/cImageItem.h"
#include "interface/CGraphics.h"
#include "utils/DirPath.h"
#include "utils/FileList.h"
#include "utils/string.hpp"
#include "sConfig.h"

extern cConfig cfg;


IBuilding& cGameWindow::active_building() const {
    auto bld = window_manager().GetActiveBuilding();
    if(bld)
        return *bld;
    throw std::logic_error("No active building");
}

void cGameWindow::set_active_building(IBuilding* building)
{
    window_manager().SetActiveBuilding(building);
}

IBuilding& cGameWindow::cycle_building(int direction)
{
    auto& buildings = g_Game->buildings();
    auto next = (buildings.find(&active_building()) + direction) % buildings.num_buildings();
    auto& active = buildings.get_building(next);
    set_active_building(&active);
    return active;
}

sGirl& cGameWindow::active_girl() const
{
    auto girl = window_manager().GetActiveGirl();
    if(girl)
        return *girl;
    throw std::logic_error("No girl selected");
}

void cGameWindow::set_active_girl(sGirl * girl)
{
    window_manager().SetActiveGirl(girl);
}

sGirl* cGameWindow::selected_girl() const
{
    return window_manager().GetActiveGirl();
}


std::string numeric = "0123456789 ().,[]-";

std::string pic_types[] =    // `J` moved this out to global and removed file extensions
                    {
                            // `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem > pic_types
                            "anal*.", "bdsm*.", "sex*.", "beast*.", "group*.", "les*.", "torture*.",
                            "death*.", "profile*.", "combat*.", "oral*.", "ecchi*.", "strip*.", "maid*.", "sing*.",
                            "wait*.", "card*.", "bunny*.", "nude*.", "mast*.", "titty*.", "milk*.", "hand*.",
                            "foot*.", "bed*.", "farm*.", "herd*.", "cook*.", "craft*.", "swim*.", "bath*.",
                            "nurse*.", "formal*.", "shop*.", "magic*.", "sign*.", "presented*.", "dom*.",
                            "deep*.", "eatout*.", "dildo*.", "sub*.", "strapon*.", "les69ing*.", "lick*.", "balls*.",
                            "cowgirl*.", "revcowgirl*.", "sexdoggy*.", "jail*.", "puppygirl*.", "ponygirl*.", "catgirl*.",
                            "brand*.", "rape*.", "beastrape*.", "birthhuman*.", "birthmultiplehuman*.", "birthbeast*.",                    // `J` new .06.03.01 for DarkArk
                            "impregsex*.", "impreggroup*.", "impregbdsm*.", "impregbeast*.",                                             // `J` new .06.03.01 for DarkArk
                            "virginsex*.", "virgingroup*.", "virginbdsm*.", "virginbeast*.",                                            // `J` new .06.03.01 for DarkArk
                            "escort*.", "sport*.", "study*.", "teacher*.",                                                                // `J` new .06.03.02 for grishnak
                            "preg*.",    // pregnant varients
                            "preganal*.", "pregbdsm*.", "pregsex*.", "pregbeast*.", "preggroup*.", "pregles*.",
                            "pregtorture*.", "pregdeath*.", "pregprofile*.", "pregcombat*.", "pregoral*.", "pregecchi*.",
                            "pregstrip*.", "pregmaid*.", "pregsing*.", "pregwait*.", "pregcard*.", "pregbunny*.", "pregnude*.",
                            "pregmast*.", "pregtitty*.", "pregmilk*.", "preghand*.", "pregfoot*.", "pregbed*.", "pregfarm*.",
                            "pregherd*.", "pregcook*.", "pregcraft*.", "pregswim*.", "pregbath*.", "pregnurse*.", "pregformal*.",
                            "pregshop*.", "pregmagic*.", "pregsign*.", "pregpresented*.", "pregdom*.", "pregdeep*.",
                            "pregeatout*.", "pregdildo*.", "pregsub*.", "pregstrapon*.", "pregles69ing*.", "preglick*.",
                            "pregballs*.", "pregcowgirl*.", "pregrevcowgirl*.", "pregsexdoggy*.", "pregjail*.", "pregpuppygirl*.",
                            "pregponygirl*.", "pregcatgirl*.",
                            "pregbrand*.", "pregrape*.", "pregbeastrape*.",                                                                // `J` new .06.03.01 for DarkArk
                            "pregbirthhuman*.", "pregbirthmultiplehuman*.", "pregbirthbeast*.",                                         // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "pregimpregsex*.", "pregimpreggroup*.", "pregimpregbdsm*.", "pregimpregbeast*.",                             // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "pregvirginsex*.", "pregvirgingroup*.", "pregvirginbdsm*.", "pregvirginbeast*.",                            // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "pregescort*.", "pregsport*.", "pregstudy*.", "pregteacher*."                                                // `J` new .06.03.02 for grishnak

                    };
std::string galtxt[] =
                    {
                            // `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem.cpp > galtxt
                            "Anal", "BDSM", "Sex", "Beast", "Group", "Lesbian", "Torture", "Death", "Profile", "Combat",
                            "Oral", "Ecchi", "Strip", "Maid", "Sing", "Wait", "Card", "Bunny", "Nude", "Mast", "Titty", "Milk", "Hand",
                            "Foot", "Bed", "Farm", "Herd", "Cook", "Craft", "Swim", "Bath", "Nurse", "Formal", "Shop", "Magic", "Sign",
                            "Presented", "Dominatrix", "Deepthroat", "Eatout", "Dildo", "Sub", "Strapon", "Les69ing", "Lick",
                            "Balls", "Cowgirl", "Revcowgirl", "Sexdoggy", "Jail", "Puppygirl", "Ponygirl", "Catgirl",
                            "Branding", "Rape", "Beast Rape", "Human Birth", "Human Birth Multiple", "Monster Birth",                    // `J` new .06.03.01 for DarkArk
                            "Impregnate Sex", "Impregnate Group", "Impregnate Bondage", "Impregnate Beast",                             // `J` new .06.03.01 for DarkArk
                            "Virgin Sex", "Virgin Group", "Virgin Bondage", "Virgin Beast",                                                // `J` new .06.03.01 for DarkArk
                            "Escort", "Sport", "Study", "Teacher",                                                                        // `J` new .06.03.02 for grishnak

                            "Pregnant",    // pregnant varients
                            "Pregnant Anal", "Pregnant BDSM", "Pregnant Sex", "Pregnant Beast", "Pregnant Group",
                            "Pregnant Lesbian", "Pregnant Torture", "Pregnant Death", "Pregnant Profile", "Pregnant Combat",
                            "Pregnant Oral", "Pregnant Ecchi", "Pregnant Strip", "Pregnant Maid", "Pregnant Sing", "Pregnant Wait",
                            "Pregnant Card", "Pregnant Bunny", "Pregnant Nude", "Pregnant Mast", "Pregnant Titty", "Pregnant Milk",
                            "Pregnant Hand", "Pregnant Foot", "Pregnant Bed", "Pregnant Farm", "Pregnant Herd", "Pregnant Cook",
                            "Pregnant Craft", "Pregnant Swim", "Pregnant Bath", "Pregnant Nurse", "Pregnant Formal", "Pregnant Shop",
                            "Pregnant Magic", "Pregnant Sign", "Pregnant Presented", "Pregnant Dominatrix", "Pregnant Deepthroat",
                            "Pregnant Eatout", "Pregnant Dildo", "Pregnant Sub", "Pregnant Strapon", "Pregnant Les69ing", "Pregnant Lick",
                            "Pregnant Balls", "Pregnant Cowgirl", "Pregnant Revcowgirl", "Pregnant Sexdoggy", "Pregnant Jail",
                            "Pregnant Puppygirl", "Pregnant Ponygirl", "Pregnant Catgirl",
                            "Pregnant Branding", "Pregnant Rape", "Pregnant Beast Rape",                                                // `J` new .06.03.01 for DarkArk
                            "P Human Birth", "P Human Multi-Birth", "P Monster Birth",                                                    // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "P Impreg Sex", "P Impreg Group", "P Impreg Bondage", "P Impreg Beast",                                        // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "P Virgin Sex", "P Virgin Group", "P Virgin Bondage", "P Virgin Beast",                                        // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                            "Pregnant Escort", "Pregnant Sport", "Pregnant Study", "Pregnant Teacher"                                    // `J` new .06.03.02 for grishnak

                    };



/* `J` image tree for each image type
*    tries starts at 40 and counts down until an image is found
*    if preferdefault is true default images will be checked for type returned
*    if preferdefault is false, when tries is less than 10, it will allow default images
*/
int TryImageType(int imagetype, int tries)
{
    if (imagetype == IMGTYPE_PROFILE)
    {
        if (tries > 15) tries = 15;
        return IMGTYPE_PROFILE;
    }
    int trytype = imagetype;
    /*    the first 10 checks will always try for the asked for type
    *    when tries == 10, it will try one last time for the asked for type
    *    when tries goes below 10 it will start looking for defaults
    *    9 will always look for the asked for type
    *    when it gets down to the last checks it will look for profile
    */
    if (tries > 30 || tries == 9 || tries == 10 || tries == 0)    return trytype;
    if (tries == 1)    return IMGTYPE_PROFILE;

    int t = g_Dice.d100();
    // for 11 through 30 and 2 trhough 8, some choices need to be made for each type
    switch (trytype)
    {
        // `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem.cpp > TryImageType

    case IMGTYPE_ANAL:
    case IMGTYPE_BDSM:
    case IMGTYPE_BEAST:
    case IMGTYPE_GROUP:
    {
        if (tries > 6 && tries < 10)    return trytype;

        if (t < 5)    return IMGTYPE_NUDE;
        if (t < 10)    return IMGTYPE_STRIP;
        if (t < 15)    return IMGTYPE_ECCHI;
        return IMGTYPE_SEX;
    }break;
    case    IMGTYPE_PREGANAL:
    case    IMGTYPE_PREGBDSM:
    case    IMGTYPE_PREGBEAST:
    case    IMGTYPE_PREGGROUP:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 5)    return IMGTYPE_PREGECCHI;
            if (t < 10)    return IMGTYPE_PREGSTRIP;
            if (t < 15)    return IMGTYPE_PREGNUDE;
            if (t < 30)    return IMGTYPE_PREGBDSM;
            if (t < 50)    return IMGTYPE_PREGANAL;
            return IMGTYPE_PREGSEX;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4 || tries == 5)    // try nonpreg vars
        {
            if (t < 5)    return IMGTYPE_ECCHI;
            if (t < 10)    return IMGTYPE_STRIP;
            if (t < 15)    return IMGTYPE_NUDE;
            if (t < 30)    return IMGTYPE_BDSM;
            if (t < 50)    return IMGTYPE_ANAL;
            return IMGTYPE_SEX;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_SEX:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 15)    return IMGTYPE_COWGIRL;
        if (t < 30)    return IMGTYPE_REVCOWGIRL;
        if (t < 45)    return IMGTYPE_SEXDOGGY;
        if (t < 55)    return IMGTYPE_ANAL;
        if (t < 64)    return IMGTYPE_ORAL;
        if (t < 73)    return IMGTYPE_TITTY;
        if (t < 80)    return IMGTYPE_HAND;
        if (t < 85)    return IMGTYPE_FOOT;
        if (t < 90)    return IMGTYPE_STRIP;
        if (t < 95)    return IMGTYPE_ECCHI;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGSEX:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 15)    return IMGTYPE_PREGCOWGIRL;
            if (t < 30)    return IMGTYPE_PREGREVCOWGIRL;
            if (t < 45)    return IMGTYPE_PREGSEXDOGGY;
            if (t < 55)    return IMGTYPE_PREGANAL;
            if (t < 64)    return IMGTYPE_PREGORAL;
            if (t < 73)    return IMGTYPE_PREGTITTY;
            if (t < 80)    return IMGTYPE_PREGHAND;
            if (t < 85)    return IMGTYPE_PREGFOOT;
            if (t < 90)    return IMGTYPE_PREGSTRIP;
            if (t < 95)    return IMGTYPE_PREGECCHI;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 15)    return IMGTYPE_COWGIRL;
            if (t < 30)    return IMGTYPE_REVCOWGIRL;
            if (t < 45)    return IMGTYPE_SEXDOGGY;
            if (t < 55)    return IMGTYPE_ANAL;
            if (t < 64)    return IMGTYPE_ORAL;
            if (t < 73)    return IMGTYPE_TITTY;
            if (t < 80)    return IMGTYPE_HAND;
            if (t < 85)    return IMGTYPE_FOOT;
            if (t < 90)    return IMGTYPE_STRIP;
            if (t < 95)    return IMGTYPE_ECCHI;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_COWGIRL:
    case    IMGTYPE_REVCOWGIRL:
    case    IMGTYPE_SEXDOGGY:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 30)    return IMGTYPE_SEX;
        if (t < 45)    return IMGTYPE_COWGIRL;
        if (t < 60)    return IMGTYPE_REVCOWGIRL;
        if (t < 75)    return IMGTYPE_SEXDOGGY;
        if (t < 85)    return IMGTYPE_ANAL;
        if (t < 90)    return IMGTYPE_STRIP;
        if (t < 95)    return IMGTYPE_ECCHI;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGCOWGIRL:
    case    IMGTYPE_PREGREVCOWGIRL:
    case    IMGTYPE_PREGSEXDOGGY:

    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 30)    return IMGTYPE_PREGSEX;
            if (t < 45)    return IMGTYPE_PREGCOWGIRL;
            if (t < 60)    return IMGTYPE_PREGREVCOWGIRL;
            if (t < 75)    return IMGTYPE_PREGSEXDOGGY;
            if (t < 85)    return IMGTYPE_PREGANAL;
            if (t < 90)    return IMGTYPE_PREGSTRIP;
            if (t < 95)    return IMGTYPE_PREGECCHI;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 30)    return IMGTYPE_SEX;
            if (t < 45)    return IMGTYPE_COWGIRL;
            if (t < 60)    return IMGTYPE_REVCOWGIRL;
            if (t < 75)    return IMGTYPE_SEXDOGGY;
            if (t < 85)    return IMGTYPE_ANAL;
            if (t < 90)    return IMGTYPE_STRIP;
            if (t < 95)    return IMGTYPE_ECCHI;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;




    case    IMGTYPE_LESBIAN:
    case    IMGTYPE_EATOUT:
    case    IMGTYPE_LES69ING:
    case    IMGTYPE_STRAPON:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 10)    return IMGTYPE_STRIP;
        if (t < 20)    return IMGTYPE_ECCHI;
        if (t < 30)    return IMGTYPE_NUDE;
        if (t < 45)    return IMGTYPE_EATOUT;
        if (t < 60)    return IMGTYPE_STRAPON;
        if (t < 75)    return IMGTYPE_LES69ING;
        return IMGTYPE_LESBIAN;
    }break;
    case    IMGTYPE_PREGLESBIAN:
    case    IMGTYPE_PREGEATOUT:
    case    IMGTYPE_PREGLES69ING:
    case    IMGTYPE_PREGSTRAPON:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 10)    return IMGTYPE_PREGSTRIP;
            if (t < 20)    return IMGTYPE_PREGECCHI;
            if (t < 30)    return IMGTYPE_PREGNUDE;
            if (t < 45)    return IMGTYPE_PREGEATOUT;
            if (t < 60)    return IMGTYPE_PREGSTRAPON;
            if (t < 75)    return IMGTYPE_PREGLES69ING;
            return IMGTYPE_PREGLESBIAN;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 10)    return IMGTYPE_STRIP;

            if (t < 20)    return IMGTYPE_ECCHI;
            if (t < 30)    return IMGTYPE_NUDE;
            if (t < 45)    return IMGTYPE_EATOUT;
            if (t < 60)    return IMGTYPE_STRAPON;
            if (t < 75)    return IMGTYPE_LES69ING;
            return IMGTYPE_LESBIAN;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;


    case    IMGTYPE_ORAL:
    case    IMGTYPE_DEEPTHROAT:
    case    IMGTYPE_LICK:
    case    IMGTYPE_SUCKBALLS:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 20)    return IMGTYPE_DEEPTHROAT;
        if (t < 40)    return IMGTYPE_LICK;
        if (t < 60)    return IMGTYPE_SUCKBALLS;
        if (t < 70)    return IMGTYPE_TITTY;
        return IMGTYPE_ORAL;
    }break;

    case    IMGTYPE_PREGORAL:
    case    IMGTYPE_PREGDEEPTHROAT:
    case    IMGTYPE_PREGLICK:
    case    IMGTYPE_PREGSUCKBALLS:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 20)    return IMGTYPE_PREGDEEPTHROAT;
            if (t < 40)    return IMGTYPE_PREGLICK;
            if (t < 60)    return IMGTYPE_PREGSUCKBALLS;
            if (t < 70)    return IMGTYPE_PREGTITTY;
            return IMGTYPE_PREGORAL;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 20)    return IMGTYPE_DEEPTHROAT;
            if (t < 40)    return IMGTYPE_LICK;
            if (t < 60)    return IMGTYPE_SUCKBALLS;
            if (t < 70)    return IMGTYPE_TITTY;
            return IMGTYPE_ORAL;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_TITTY:
    case    IMGTYPE_HAND:
    case    IMGTYPE_FOOT:
    {
        if (tries > 6 && tries < 10)    return trytype;

        if (t < 20)    return IMGTYPE_TITTY;
        if (t < 35)    return IMGTYPE_HAND;
        if (t < 50)    return IMGTYPE_FOOT;
        if (t < 65)    return IMGTYPE_STRIP;
        if (t < 80)    return IMGTYPE_ECCHI;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGTITTY:

    case    IMGTYPE_PREGHAND:
    case    IMGTYPE_PREGFOOT:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {


            if (t < 20)    return IMGTYPE_PREGTITTY;
            if (t < 35)    return IMGTYPE_PREGHAND;
            if (t < 50)    return IMGTYPE_PREGFOOT;
            if (t < 65)    return IMGTYPE_PREGSTRIP;
            if (t < 80)    return IMGTYPE_PREGECCHI;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {


            if (t < 20)    return IMGTYPE_TITTY;
            if (t < 35)    return IMGTYPE_HAND;
            if (t < 50)    return IMGTYPE_FOOT;
            if (t < 65)    return IMGTYPE_STRIP;
            if (t < 80)    return IMGTYPE_ECCHI;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_MAST:
    case    IMGTYPE_DILDO:
    {
        if (tries > 7 && tries < 10)    return trytype;
        if (t < 30)    return IMGTYPE_MAST;
        if (t < 45)    return IMGTYPE_STRIP;
        if (t < 60)    return IMGTYPE_ECCHI;
        if (t < 80)    return IMGTYPE_DILDO;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGMAST:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 30)    return IMGTYPE_PREGMAST;
            if (t < 45)    return IMGTYPE_PREGSTRIP;

            if (t < 60)    return IMGTYPE_PREGECCHI;
            if (t < 80)    return IMGTYPE_PREGDILDO;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 30)    return IMGTYPE_MAST;
            if (t < 45)    return IMGTYPE_STRIP;
            if (t < 60)    return IMGTYPE_ECCHI;
            if (t < 80)    return IMGTYPE_DILDO;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_DOM:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_COMBAT;
    }break;
    case    IMGTYPE_PREGDOM:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 20 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGCOMBAT;
        }
        if (tries > 14 || tries == 4 || tries == 5)    // try nonpreg vars
        {
            return IMGTYPE_COMBAT;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_SUB:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 30)    return IMGTYPE_STRIP;
        if (t < 60)    return IMGTYPE_ECCHI;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGSUB:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 20 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 30)    return IMGTYPE_PREGSTRIP;
            if (t < 60)    return IMGTYPE_PREGECCHI;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 14 || tries == 4 || tries == 5)    // try nonpreg vars
        {
            if (t < 30)    return IMGTYPE_STRIP;
            if (t < 60)    return IMGTYPE_ECCHI;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;



    case    IMGTYPE_ECCHI:
    case    IMGTYPE_STRIP:
    case    IMGTYPE_MILK:
    case    IMGTYPE_BATH:
    case    IMGTYPE_BED:
    case    IMGTYPE_NUDE:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 20)    return IMGTYPE_ECCHI;
        if (t < 50)    return IMGTYPE_STRIP;
        return IMGTYPE_NUDE;
    }break;
    case    IMGTYPE_PREGBED:
    case    IMGTYPE_PREGECCHI:
    case    IMGTYPE_PREGSTRIP:
    case    IMGTYPE_PREGBATH:
    case    IMGTYPE_PREGNUDE:
    case    IMGTYPE_PREGMILK:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 20)    return IMGTYPE_PREGECCHI;
            if (t < 50)    return IMGTYPE_PREGSTRIP;
            return IMGTYPE_PREGNUDE;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 20)    return IMGTYPE_ECCHI;
            if (t < 50)    return IMGTYPE_STRIP;
            return IMGTYPE_NUDE;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_TORTURE:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 20)    return IMGTYPE_DEATH;
        return IMGTYPE_BDSM;
    }break;
    case    IMGTYPE_PREGTORTURE:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 20)    return IMGTYPE_PREGDEATH;
            return IMGTYPE_PREGBDSM;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 20)    return IMGTYPE_DEATH;
            return IMGTYPE_BDSM;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_COMBAT:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_MAGIC;
    }break;
    case    IMGTYPE_PREGCOMBAT:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGMAGIC;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            return IMGTYPE_MAGIC;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_SWIM:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_ECCHI;
    }break;
    case    IMGTYPE_PREGSWIM:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGECCHI;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            return IMGTYPE_ECCHI;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;



    case    IMGTYPE_SING:
    case    IMGTYPE_CARD:
    case    IMGTYPE_BUNNY:
    case    IMGTYPE_ESCORT:
    case    IMGTYPE_TEACHER:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_FORMAL;
    }break;
    case    IMGTYPE_PREGSING:
    case    IMGTYPE_PREGCARD:
    case    IMGTYPE_PREGBUNNY:
    case    IMGTYPE_PREGESCORT:
    case    IMGTYPE_PREGTEACHER:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGFORMAL;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            return IMGTYPE_FORMAL;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_MAID:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_BUNNY;
    }break;
    case    IMGTYPE_PREGMAID:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGBUNNY;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            return IMGTYPE_BUNNY;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_WAIT:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 50)    return IMGTYPE_BUNNY;
        return IMGTYPE_FORMAL;
    }break;
    case    IMGTYPE_PREGWAIT:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 50)    return IMGTYPE_PREGBUNNY;
            return IMGTYPE_PREGFORMAL;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 50)    return IMGTYPE_BUNNY;
            return IMGTYPE_FORMAL;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_COOK:
    {
        if (tries > 6 && tries < 10)    return trytype;
        if (t < 70)    return IMGTYPE_WAIT;
        return IMGTYPE_MAID;
    }break;
    case    IMGTYPE_PREGCOOK:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            if (t < 20)    return IMGTYPE_PREGWAIT;
            return IMGTYPE_PREGMAID;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            if (t < 20)    return IMGTYPE_WAIT;
            return IMGTYPE_MAID;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;

    case    IMGTYPE_HERD:
    case    IMGTYPE_CRAFT:
    {
        if (tries > 6 && tries < 10)    return trytype;
        return IMGTYPE_FARM;
    }break;
    case    IMGTYPE_PREGHERD:
    case    IMGTYPE_PREGCRAFT:
    {
        if (tries == 8)    return trytype;                    // try with defaults
        if (tries > 22 || tries == 6 || tries == 7)        // try preg vars
        {
            return IMGTYPE_PREGFARM;
        }
        if (tries > 18 || tries == 5)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 14 || tries == 4)    // try nonpreg vars
        {
            return IMGTYPE_FARM;
        }
        // try profile
        if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
        if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
    }break;



    case IMGTYPE_BRAND:
    case IMGTYPE_PREGBRAND:
    {
        if (t < 10)    return IMGTYPE_TORTURE;
        return IMGTYPE_BRAND;
    }break;

    case IMGTYPE_RAPE:
    case IMGTYPE_PREGRAPE:
    {
        if (t < 5)    return IMGTYPE_DEATH;
        if (t < 10)    return IMGTYPE_GROUP;
        if (t < 15)    return IMGTYPE_TORTURE;
        return IMGTYPE_RAPE;
    }break;

    case IMGTYPE_RAPEBEAST:
    case IMGTYPE_PREGRAPEBEAST:
    {
        if (t < 5)    return IMGTYPE_DEATH;
        if (t < 10)    return IMGTYPE_TORTURE;
        if (t < 15)    return IMGTYPE_BEAST;
        return IMGTYPE_RAPEBEAST;
    }break;


    case IMGTYPE_BIRTHHUMAN:
    case IMGTYPE_PREGBIRTHHUMAN:
    {
        if (t < 15)    return IMGTYPE_BIRTHBEAST;
        return IMGTYPE_BIRTHHUMAN;
    }break;
    case IMGTYPE_BIRTHHUMANMULTIPLE:
    case IMGTYPE_PREGBIRTHHUMANMULTIPLE:
    {
        if (t < 15)    return IMGTYPE_BIRTHBEAST;
        if (t < 20)    return IMGTYPE_BIRTHHUMAN;
        return IMGTYPE_BIRTHHUMANMULTIPLE;
    }break;
    case IMGTYPE_BIRTHBEAST:
    case IMGTYPE_PREGBIRTHBEAST:
    {
        if (t < 15)    return IMGTYPE_BIRTHHUMAN;
        return IMGTYPE_BIRTHBEAST;
    }break;
    case IMGTYPE_IMPREGSEX:
    case IMGTYPE_PREGIMPREGSEX:
    {
        if (t < 15)    return IMGTYPE_SEX;
        return IMGTYPE_IMPREGSEX;
    }break;
    case IMGTYPE_IMPREGGROUP:
    case IMGTYPE_PREGIMPREGGROUP:
    {
        if (t < 15)    return IMGTYPE_GROUP;
        return IMGTYPE_IMPREGGROUP;
    }break;
    case IMGTYPE_IMPREGBDSM:
    case IMGTYPE_PREGIMPREGBDSM:
    {
        if (t < 15)    return IMGTYPE_BDSM;
        return IMGTYPE_IMPREGBDSM;
    }break;
    case IMGTYPE_IMPREGBEAST:
    case IMGTYPE_PREGIMPREGBEAST:
    {
        if (t < 15)    return IMGTYPE_BEAST;
        return IMGTYPE_IMPREGBEAST;
    }break;
    case IMGTYPE_VIRGINSEX:
    case IMGTYPE_PREGVIRGINSEX:
    {
        if (t < 15)    return IMGTYPE_SEX;
        return IMGTYPE_VIRGINSEX;
    }break;
    case IMGTYPE_VIRGINGROUP:
    case IMGTYPE_PREGVIRGINGROUP:
    {
        if (t < 15)    return IMGTYPE_GROUP;
        return IMGTYPE_VIRGINGROUP;
    }break;
    case IMGTYPE_VIRGINBDSM:
    case IMGTYPE_PREGVIRGINBDSM:
    {
        if (t < 15)    return IMGTYPE_BDSM;
        return IMGTYPE_VIRGINBDSM;
    }break;
    case IMGTYPE_VIRGINBEAST:
    case IMGTYPE_PREGVIRGINBEAST:
    {
        if (t < 15)    return IMGTYPE_BEAST;
        return IMGTYPE_VIRGINBEAST;
    }break;





        // any preg varients not coded in yet just returns pregnant

    case IMGTYPE_PREGNANT:
    {
        if (tries > 15) tries = 15;
        if (tries < 5)    return IMGTYPE_PROFILE;
        else             return IMGTYPE_PREGPROFILE;
    }break;

    case IMGTYPE_PREGPROFILE:
    {
        return IMGTYPE_PREGNANT;
    }break;

        //    Pregnant versions of anything that only has profile as an alternative
    case IMGTYPE_PREGFARM:
    case IMGTYPE_PREGPRESENTED:
    case IMGTYPE_PREGNURSE:
    case IMGTYPE_PREGFORMAL:
    case IMGTYPE_PREGMAGIC:
    case IMGTYPE_PREGDEATH:
    case IMGTYPE_PREGSHOP:
    case IMGTYPE_PREGSIGN:
    case IMGTYPE_PREGSPORT:
    case IMGTYPE_PREGSTUDY:
    {
        if (tries > 25)    // try nonpreg base
        {
            return trytype - PREG_OFFSET;
        }
        if (tries > 21)    return IMGTYPE_PREGPROFILE;
        if (tries > 14)    return IMGTYPE_PREGNANT;
        if (tries > 8)    return trytype - PREG_OFFSET;    // the nonpregnant version
        if (tries > 6)    return IMGTYPE_PREGPROFILE;
        if (tries > 3)    return IMGTYPE_PREGNANT;
        return trytype - PREG_OFFSET;    // the nonpregnant version
    }break;

        //    anything that only has profile as an alternative
    case IMGTYPE_FARM:
    case IMGTYPE_PRESENTED:
    case IMGTYPE_NURSE:
    case IMGTYPE_FORMAL:
    case IMGTYPE_MAGIC:
    case IMGTYPE_DEATH:
    case IMGTYPE_SHOP:
    case IMGTYPE_SIGN:
    case IMGTYPE_JAIL:
    case IMGTYPE_PUPPYGIRL:
    case IMGTYPE_PONYGIRL:
    case IMGTYPE_CATGIRL:
    case IMGTYPE_SPORT:
    case IMGTYPE_STUDY:
    default:
        return IMGTYPE_PROFILE;
        break;

    }

    return IMGTYPE_PROFILE;    // if all else fails just try profile
}


std::vector<std::string> FindImage(sGirl* girl, int imagetype, bool gallery, std::string ImageName) {
    if (!girl || imagetype < 0 || ImageName == "blank")        // no girl, no images
    {
        if(!girl)
            g_LogFile.log(ELogLevel::ERROR, "Debug Alt Images || No Girl");

        if(ImageName == "blank")
            g_LogFile.log(ELogLevel::WARNING, "Debug Alt Images || Blank Image");
        if(imagetype < 0)
            g_LogFile.log(ELogLevel::ERROR, "Debug Alt Images || Imagetype < 0");
        return {};
    }

    std::string girlName = girl->m_Name;
    g_LogFile.log(ELogLevel::DEBUG, "Debug Alt Images || Getting image for: ", girl->FullName(), " (", girlName, ")");

    int dir = 0; DirPath usedir = "";
    DirPath imagedirCc = DirPath(cfg.folders.characters().c_str()) << girlName;    // usedir = 1
    DirPath imagedirCo = DirPath() << "Resources" << "Characters" << girlName;    // usedir = 2
    DirPath imagedirDc = DirPath(cfg.folders.defaultimageloc().c_str());        // usedir = -1
    DirPath imagedirDo = DirPath() << "Resources" << "DefaultImages";            // usedir = -2
    FileList tiCc(imagedirCc, "*.*");
    FileList tiCo(imagedirCo, "*.*");
    FileList tiDc(imagedirDc, "*.*");
    FileList tiDo(imagedirDo, "*.*");

    int totalimagesCc = tiCc.size();
    int totalimagesCo = tiCo.size();
    int totalimagesDc = tiDc.size();
    int totalimagesDo = tiDo.size();
    if (totalimagesCc + totalimagesCo + totalimagesDc + totalimagesDo < 1)    // no images at all so return a blank image
    {
        g_LogFile.log(ELogLevel::WARNING, "No Images found for: ", girlName, " and no Default images found");
        return {};
    }

    std::string file;
    std::string filename;
    std::string ext;
    bool imagechosen = false;

    bool armor = girl->get_num_item_equiped(sInventoryItem::Armor) >= 1;
    bool dress = girl->get_num_item_equiped(sInventoryItem::Dress) >= 1;
    bool swim = girl->get_num_item_equiped(sInventoryItem::Swimsuit) >= 1;
    bool lingerie = girl->get_num_item_equiped(sInventoryItem::Underwear) >= 1;

    int tries = 40;
    if (gallery) tries = 0;
    else    // try some corrections
    {
        if (cfg.folders.preferdefault() || totalimagesCc + totalimagesCo < 1)    tries = 10;
        if (imagetype < 0 || imagetype > NUM_IMGTYPES)        imagetype = IMGTYPE_PROFILE;

        if (imagetype == IMGTYPE_PROFILE && g_Dice.percent(10))
        {
            if (armor)    { imagetype = IMGTYPE_COMBAT; }
            else if (dress)
            {
                /* */if (girl->has_active_trait("Elegant")) imagetype = IMGTYPE_FORMAL;
                else if (girl->has_active_trait("Dominatrix")) imagetype = IMGTYPE_DOM;
                else if (girl->has_active_trait("Maid")) imagetype = IMGTYPE_MAID;
                else if (girl->has_active_trait("Teacher")) imagetype = IMGTYPE_TEACHER;
                else if (girl->has_active_trait("Doctor")) imagetype = IMGTYPE_NURSE;
            }
            else if (swim)            { imagetype = IMGTYPE_SWIM; }
            else if (lingerie)        { imagetype = IMGTYPE_ECCHI; }
        }

        /* */if (imagetype >= IMGTYPE_PREGBIRTHHUMAN && imagetype <= IMGTYPE_PREGVIRGINBEAST) imagetype -= PREG_OFFSET;        // `J` new .06.03.01 for DarkArk - These should not have preg varients
        else if (imagetype >= IMGTYPE_BIRTHHUMAN && imagetype <= IMGTYPE_VIRGINBEAST) {}                                    // `J` new .06.03.01 for DarkArk - These should not have preg varients
        else if (girl->is_pregnant())
        {
            if (imagetype < IMGTYPE_PREGNANT)    imagetype += PREG_OFFSET;
        }
        else
        {
            if (imagetype == IMGTYPE_PREGNANT)    imagetype = IMGTYPE_PROFILE;
            if (imagetype > IMGTYPE_PREGNANT)    imagetype -= PREG_OFFSET;
        }
    }

    do
    {
        int tryimagetype = TryImageType(imagetype, tries);
        g_LogFile.log(ELogLevel::DEBUG, "Debug Alt Images || Try: ", tries, " || In: ", pic_types[imagetype],
                " | Out: ", pic_types[tryimagetype]);

        // choose an image folder
        dir = 0; usedir = "";
        std::string checkfor = pic_types[tryimagetype] + "*";
        if (tryimagetype == IMGTYPE_PREGNANT) checkfor = "pregnant*.*";
        if (tryimagetype == IMGTYPE_SUCKBALLS) checkfor = "suckballs*.*";

        if (totalimagesCc > 0)
        {
            FileList testall(imagedirCc, checkfor.c_str());
            if (tryimagetype == IMGTYPE_PREGNANT)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("preg" + numeric.substr(i, 1) + "*.*");
                    testall.add(t.c_str());
                }
            }
            if (tryimagetype == IMGTYPE_SUCKBALLS)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("balls*.*");
                    testall.add(t.c_str());
                }
            }
            if (testall.size() > 0)
            {
                usedir = imagedirCc;
                dir = 1;
            }
        }
        if (dir == 0 && totalimagesCo > 0)    // if config is not found, check for images in the original folder
        {
            FileList testall(imagedirCo, checkfor.c_str());
            if (tryimagetype == IMGTYPE_PREGNANT)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("preg" + numeric.substr(i, 1) + "*.*");
                    testall.add(t.c_str());
                }
            }
            if (tryimagetype == IMGTYPE_SUCKBALLS)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("balls*.*");
                    testall.add(t.c_str());
                }
            }
            if (testall.size() > 0)
            {
                usedir = imagedirCo;
                dir = 2;
            }
        }
        if (dir == 0 && gallery)    // gallery stops here if there are no images
        {
            g_LogFile.log(ELogLevel::WARNING, "Debug Alt Images || No gallery images found for: ", girlName);
            return {};
        }

        // if neither character folder has what we are looking for try the defaults
        if (totalimagesDc > 0 && dir == 0 && tries < 10)
        {
            FileList testall(imagedirDc, checkfor.c_str());
            if (tryimagetype == IMGTYPE_PREGNANT)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("preg" + numeric.substr(i, 1) + "*.*");
                    testall.add(t.c_str());
                }
            }
            if (tryimagetype == IMGTYPE_SUCKBALLS)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("balls*.*");
                    testall.add(t.c_str());
                }
            }
            if (testall.size() > 0)
            {
                usedir = imagedirDc;
                dir = -1;
            }
        }
        if (totalimagesDo > 0 && dir == 0 && tries < 10)
        {
            FileList testall(imagedirDo, checkfor.c_str());
            if (tryimagetype == IMGTYPE_PREGNANT)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("preg" + numeric.substr(i, 1) + "*.*");
                    testall.add(t.c_str());
                }
            }
            if (tryimagetype == IMGTYPE_SUCKBALLS)
            {
                for (u_int i = 0; i < numeric.size(); i++)
                {
                    std::string t = ("balls*.*");
                    testall.add(t.c_str());
                }
            }
            if (testall.size() > 0)
            {
                usedir = imagedirDo;
                dir = -2;
            }
        }
        if (dir == 0)
        {
            continue;
        }

        FileList testall(usedir, checkfor.c_str());
        if (tryimagetype == IMGTYPE_PREGNANT)
        {
            for (u_int i = 0; i < numeric.size(); i++)
            {
                std::string t = ("preg" + numeric.substr(i, 1) + "*.*");
                testall.add(t.c_str());
            }
        }
        if (tryimagetype == IMGTYPE_SUCKBALLS)
        {
            for (u_int i = 0; i < numeric.size(); i++)
            {
                std::string t = ("balls*.*");
                testall.add(t.c_str());
            }
        }
        if (testall.size() <= 0) continue;
        std::vector<std::string> result;
        for(int i = 0; i < testall.size(); ++i) {
            result.emplace_back(testall[i].full());
        }
        return result;
    }    while (!imagechosen && --tries > 0);

    return {};
}


// `J` Totally new method for image handling for .06.02.00
void cGameWindow::PrepareImage(int id, sGirl* girl, int imagetype, bool rand, int ImageNum, bool gallery, std::string ImageName)
{
    // Clear the old images
    cImageItem* image = GetImage(id);
    if(!image) return;

    auto images = FindImage(girl, imagetype, gallery, ImageName);
    if(images.empty()) {
        image->SetImage(GetGraphics().LoadImage(ImagePath("blank.png"), image->GetWidth(), image->GetHeight(), true));
        image->m_Message = "No image found.";
        return;
    }

    while(!images.empty()) {
        int num = ImageNum;
        if (rand || ImageNum < 0 || ImageNum >= images.size()) num = g_Dice % images.size();

        std::string file     = images[num];

        std::string ext = tolower(file.substr(file.find_last_of('.') + 1));
        if (ext == "ani") {
#ifdef LINUX
            auto splitpoint = file.find_last_of('/');
#else
            auto splitpoint = file.find_last_of('\\');
#endif
            DirPath anidir = DirPath() << file.substr(0, splitpoint) << "ani";
            anidir << "ani";
            std::string name = file.substr(splitpoint+1);
            name.erase(name.size() - 4, 4);
            name += ".jpg";
            FileList testani(anidir, name.c_str());
            if (testani.size() <= 0) {
                if (gallery) {
                    image->SetImage(
                            GetGraphics().LoadImage(ImagePath("blank.png"), image->GetWidth(), image->GetHeight(),
                                                    true));
                    image->m_Message = "Bad ani file: Missing its matching jpg file: " + file;
                    return;
                }
                // try a different image if there is a loading error
                images.erase(images.begin() + num);
                continue;
            }
            anidir << name;

            try {
                image->SetImage(
                        GetGraphics().GetImageCache().LoadAni(anidir, file, image->GetWidth(), image->GetHeight()));
                image->m_Message = file;
            } catch (std::runtime_error&) {
                if (gallery) {
                    image->SetImage(
                            GetGraphics().LoadImage(ImagePath("blank.png"), image->GetWidth(), image->GetHeight(),
                                                    true));
                    image->m_Message = "Bad ani file: Incorrect data file given for animation: " + file;
                    return;
                }
                // try a different image if there is a loading error
                images.erase(images.begin() + num);
                continue;
            }
        } else if (ext == "gif") {
            auto gif = GetGraphics().GetImageCache().LoadGif(file, image->GetWidth(), image->GetHeight());
            if (gif) {
                image->SetImage(gif);
            } else    // if it does not read as a gif, just load it as a normal image
            {
                image->SetImage(GetGraphics().LoadImage(file, image->GetWidth(), image->GetHeight(), true));
            }
        } else if (ext == "jpg" || ext == "jpeg" || ext == "png") {
            image->SetImage(GetGraphics().LoadImage(file, image->GetWidth(), image->GetHeight(), true));
        } else    // any other extension gets cleared.
        {
            // try a different image if there is an unrecognized extension
            images.erase(images.begin() + num);
            continue;
        }

        if (image->m_Message.empty()) {
            if(image->m_Image)
                image->m_Message = image->m_Image.GetFileName();
        }

        g_LogFile.verbose("image", "Loading image: ", image->m_Message);
        return;
    }
}