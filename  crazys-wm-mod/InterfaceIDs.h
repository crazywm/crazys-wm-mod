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
#ifndef __INTERFACEIDS_H
#define __INTERFACEIDS_H
struct sInterfaceIDs
{
// Misc (Stuff that isn't interactive with so can all use the same ID)
  int STATIC_STATIC;  

// Main menu
  int BUTTON_NEWGAME ; 
  int BUTTON_LOADGAME ; 
  int BUTTON_EXITGAME ; 

// Settings Screen
  int BUTTON_CONTINUE;

// Get string dialog
  int BUTTON_OK  ;
  int BUTTON_CANCEL  ;
  int EDITBOX_NAME  ;
  int TEXT_TEXT1  ;

// Brothel Management
  int BUTTON_MANAGEGIRLS  ;
  int BUTTON_MANAGESTAFF  ;
  int BUTTON_TURNSUMMARY  ;
  int BUTTON_MANAGEBUILDING  ;
  int BUTTON_VISITDUNGEON  ;
  int BUTTON_VISITTOWN ; 
  int BUTTON_SAVEGAME  ;
  int BUTTON_NEWWEEK  ;
  int BUTTON_QUIT  ;
  int BUTTON_NEXTBROTHEL  ;
  int BUTTON_PREVBROTHEL ; 
  int TEXT_BROTHELNAME;
  int TEXT_BMDETAILS  ;
  int IMAGE_BIMAGE  ;

// Girl management screen
  int BUTTON_GMVIEWGIRL;  
  int BUTTON_GMBACK  ;
  int LIST_GMGIRLS;  
  int TEXT_GMGIRLDESCRIPTION  ;
  int BUTTON_GMFIREGIRL ; 
  int BUTTON_GMFREESLAVE  ;
  int BUTTON_GMSELLSLAVE  ;
  int BUTTON_GMTRANSFERGIRLS;
  int IMAGE_GMIMAGE  ;
  int TEXT_GMFILTERDESCRIPTION ; 
  int TEXT_GMJOBDESCRIPTION;
  int LIST_GMJOBFILTER  ;
  int LIST_GMJOBS  ;
  int TEXT_GMJOBTITLE  ;
  int BUTTON_GMDAY  ;
  int BUTTON_GMNIGHT  ;

// Girl Details screen
  int BUTTON_GDMOVEGIRL  ;
  int BUTTON_GDSENDDUNGEON  ;
  int BUTTON_GDTALKTO  ;
  int BUTTON_GDSENDQUEST  ;
  int BUTTON_GDSENDRIVAL  ;
  int BUTTON_GDBACK;
  int TEXT_GDDETAILS  ;
  int TEXT_GDGIRLNAME  ;
  int BUTTON_GDACOMUP  ;
  int BUTTON_GDACOMDOWN ; 
  int BUTTON_GDNEXT  ;
  int BUTTON_GDPREV ; 
  int BUTTON_GDVIEWGALLERY  ;
  int BUTTON_GDTAKEGOLD  ;
  int BUTTON_GDGIVEGIFT  ;
  int BUTTON_GDRELEASE  ;
  int LIST_GDJOBFILTER  ;
  int LIST_GDJOBS  ;
  int LIST_GDTRAITS  ;
  int TEXT_GDTRAITDESC;  
  int CHECK_GDUSEANTIPREG;  
  int BUTTON_GDCHANGEHOUSEPERCENT  ;
  int IMAGE_GDGIRLIMAGE ; 
  int BUTTON_GDSHOWMORE  ;
  int TEXT_GDJOB  ;
  int BUTTON_GDDAY;
  int BUTTON_GDNIGHT  ;

// Change Jobs Screen
  int BUTTON_CJOK ;
  int BUTTON_CJCANCEL  ;
  int TEXT_CJDESC  ;
  int LIST_CJDAYTIME ; 
  int LIST_CJNIGHTTIME ; 

// STAFF MANAGEMENT SCREEN
  int BUTTON_SMHIREGOON;
  int BUTTON_SMFIREGOON;
  int BUTTON_SMHIREBAR;
  int BUTTON_SMFIREBAR  ;
  int BUTTON_SMHIREGAMB ; 
  int BUTTON_SMFIREGAMB ; 
  int BUTTON_SMBACK  ;
  int BUTTON_SMSENDMISSION  ;
  int LIST_SMMISSIONS  ;
  int BUTTON_SMRECALLMISSION  ;
  int TEXT_SMTOTALCOST ; 
  int LIST_SMMISSLIST  ;
  int TEXT_SMMISSDESC  ;
  int TEXT_SMGANGDESC;
  int TEXT_SMWEAPONDESC ;  
  int BUTTON_SMUPWEP  ;
  int TEXT_SMNETDESC  ;
  int BUTTON_SMBUYNETS ; 
  int TEXT_SMHEALPOTDESC;  
  int BUTTON_SMBUYHEALPOT;  
  int CHECK_KEEPNETSINSTOCK;  
  int CHECK_KEEPHEALPOTINSTOCK  ;

// TURN summary SCREEN
  int LIST_TSCATEGORY  ;
  int LIST_TSEVENTS  ;
  int LIST_TSITEM  ;
  int BUTTON_TSCLOSE;  
  int TEXT_TSEVENTDESC  ;
  int BUTTON_TSNEWWEEK  ;
  int BUTTON_TSGOTO  ;
  int BUTTON_TSPREVBROTHEL  ;
  int BUTTON_TSNEXTBROTHEL  ;
  int IMAGE_TSIMAGE  ;

// DUNGEON SCREEN
  int TEXT_DTITLE  ;
  int LIST_DPEOPLE  ;
  int BUTTON_DRELEASE;  
  int BUTTON_DFEED  ;
  int BUTTON_DNOFEED ; 
  int BUTTON_DTALK  ;
  int BUTTON_DBACK  ;
  int BUTTON_DFORCESLAVE;  
  int BUTTON_DTORTURE  ;
  int BUTTON_DRELEASEALLGIRLS  ;
  int BUTTON_DRELEASEALLCUSTS  ;
  int BUTTON_DVIEWGIRL  ;
  int BUTTON_DSELLSLAVE  ;

// shop screen
  int TEXT_SHOPDETAILS  ;
  int LIST_SHOPITEMS  ;
  int BUTTON_SHOPBUYITEMS  ;
  int BUTTON_SHOPSELLITEMS  ;
  int BUTTON_SHOPBUY  ;
  int BUTTON_SHOPSELL  ;
  int BUTTON_SHOPBACK  ;
  int LIST_SHOPFILTER  ;

// SLAVE MARKET SCREEN
  int TEXT_SLDETAILS  ;
  int LIST_SLGIRLS  ;
  int BUTTON_SLBUYGIRL  ;
  int BUTTON_SLBACK  ;
  int LIST_SLTRAITS  ;
  int TEXT_SLTRAITDESC  ;
  int BUTTON_SLSHOWMORE  ;
  int IMAGE_SLIMAGE  ;

// TOWN SCREEN
  int BUTTON_TSBACK  ;
  int BUTTON_TSSLAVEMARKET  ;
  int BUTTON_TSBROTHEL0  ;
  int BUTTON_TSSHOP  ;
  int BUTTON_TSMAYORSOFFICE  ;
  int BUTTON_TSBANK  ;
  int BUTTON_TSWALKAROUND  ;
  int BUTTON_TSBROTHEL1  ;
  int BUTTON_TSBROTHEL2  ;
  int BUTTON_TSBROTHEL3  ;
  int BUTTON_TSBROTHEL4  ;
  int BUTTON_TSBROTHEL5  ;
  int BUTTON_TSBROTHEL6  ;
  int BUTTON_TSPHOUSE  ;
  int BUTTON_TSPRISON  ;
// VIEW INVENTORY OR TRAITS SCREEN
  int BUTTON_VIEWINV  ;
  int BUTTON_VIEWBACK  ;
  int BUTTON_VIEWTRAITS ; 
  int BUTTON_VIEWEQUIP  ;
  int BUTTON_VIEWUNEQUIP ; 
  int TEXT_VIEWDETAILS  ;
  int LIST_VIEWITEMS  ;
  int BUTTON_VIEWTAKE  ;
// Gallery screen
  int BUTTON_GALLERYANAL  ;
  int BUTTON_GALLERYBDSM  ;
  int BUTTON_GALLERYSEX  ;
  int BUTTON_GALLERYBEAST ; 
  int BUTTON_GALLERYGROUP  ;
  int BUTTON_GALLERYLESBIAN ; 
  int BUTTON_GALLERYPREGNANT ; 
  int BUTTON_GALLERYDEATH  ;
  int BUTTON_GALLERYPROFILE ; 
  int BUTTON_GALLERYCOMBAT ; 
  int BUTTON_GALLERYORAL ; 
  int BUTTON_GALLERYECCHI ; 
  int BUTTON_GALLERYSTRIP ; 
  int BUTTON_GALLERYMAID ; 
  int BUTTON_GALLERYSING ; 
  int BUTTON_GALLERYWAIT ; 
  int BUTTON_GALLERYCARD ;
  int BUTTON_GALLERYBUNNY ;
  int BUTTON_GALLERYNUDE ;
  int BUTTON_GALLERYMAST ;
  int BUTTON_GALLERYTITTY ;
  int BUTTON_GALLERYBACK  ;
  int BUTTON_GALLERYNEXT  ;
  int BUTTON_GALLERYPREV  ;
  int IMAGE_GALLERYIMAGE  ;
  int TEXT_GALLERYTYPE	;
  int BUTTON_NEXTGALLERY  ;
  int BUTTON_WHAT  ;
// BROTHEL UPGRADE SCREEN
  int BUTTON_UPGRADEBUYTENPOT  ;
  int BUTTON_UPGRADEBUYTWENTYPOT;  
  int BUTTON_UPGRADEBACK  ;
  int TEXT_UPGRADEPOTNUM  ;
  int CHECK_KEEPPOTINSTOCK ; 
  int CHECK_KEEPALCINSTOCK  ;
  int BUTTON_UPGRADEADDMOREROOMS  ;
  int CHECK_UPGRADEANAL  ;
  int CHECK_UPGRADEBDSM  ;
  int CHECK_UPGRADENORMAL ; 
  int CHECK_UPGRADEBEAST  ;
  int CHECK_UPGRADEGROUP  ;
  int CHECK_UPGRADELES  ;
  int CHECK_UPGRADEORAL  ;

// Mayors office screen
  int BUTTON_MAYOROBACK  ;
  int BUTTON_MAYOROBRIBE  ;
  int TEXT_MAYORODETAILS  ;
// town bank screen
  int TEXT_BANKDETAILS  ;
  int BUTTON_BANKBACK  ;
  int BUTTON_BANKDEPOSIT;  
  int BUTTON_BANKWITHDRAW;  
  int BUTTON_BANKDEPOSITALL;  

// GIVE GIFT SCREEN
  int BUTTON_GGCANCEL  ;
  int BUTTON_GGOK  ;
  int TEXT_GGDETAILS;  
  int LIST_GGLIST  ;

// Load Game Screen
  int IMAGE_BGIMAGE;
  int LIST_LOADGSAVES;
  int BUTTON_LOADGLOAD;  
  int BUTTON_LOADGBACK ; 

// players house
  int TEXT_PHOUSETEXT  ;
  int BUTTON_PHOUSEBACK ; 

// Transfer girls
  int BUTTON_TRANSGBACK  ;
  int BUTTON_TRANSGSHIFTR ; 
  int BUTTON_TRANSGSHIFTL  ;
  int LIST_TRANSGLEFTGIRLS  ;
  int LIST_TRANSGRIGHTGIRLS  ;
  int LIST_TRANSGLEFTBROTHEL  ;
  int LIST_TRANSGRIGHTBROTHEL  ;
// Item Management screen
  int BUTTON_ITMBACK  ;
  int BUTTON_ITMSHIFTL ; 
  int BUTTON_ITMSHIFTR  ;
  int LIST_ITMOWNERSL  ;
  int LIST_ITMOWNERSR  ;
  int LIST_ITMITEMSL  ;
  int LIST_ITMITEMSR  ;
  int TEXT_ITMITEMDESC ; 
  int TEXT_ITMPLAYERGOLD;  
  int BUTTON_ITMEQUIP1  ;
  int BUTTON_ITMUNEQUIP1 ; 
  int LIST_ITMFILTER  ;
  int BUTTON_ITMEQUIP2 ; 
  int BUTTON_ITMUNEQUIP2;  

// prison screen
  int BUTTON_PRISBACK  ;
  int BUTTON_PRISSHOWMORE;  
  int BUTTON_PRISRELEASE  ;
  int LIST_PRISGIRLS  ;
  int TEXT_PRISGIRLDESC ; 

// misc
  int TEXT_CURRENTBROTHEL;  
  int TEXT_LABELITEM;
};
#endif
