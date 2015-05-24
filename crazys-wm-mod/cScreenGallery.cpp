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
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cScreenGallery.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include <iostream>
#include <locale>
#include <sstream>
#include "cGangs.h"
#include "libintl.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	int g_CurrClinic;
extern	int g_CurrStudio;
extern	int g_CurrArena;
extern	int g_CurrCentre;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cClinicManager		g_Clinic;
extern	cMovieStudioManager		g_Studios;
extern	cArenaManager		g_Arena;
extern	cCentreManager		g_Centre;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
//extern  cInterfaceWindow g_Gallery;
extern bool g_Cheats;
extern	bool	eventrunning;
extern string g_ReturnText;
extern bool g_AllTogle;
extern	int		g_CurrentScreen;
extern sInterfaceIDs g_interfaceid;

extern void GetString();
extern cInterfaceWindow g_GetString;

bool cScreenGallery::ids_set = false;
extern sGirl *selected_girl;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;

void cScreenGallery::set_ids()
{
	ids_set = true;
	back_id =		get_id("BackButton");
	next_id =		get_id("NextButton");
	prev_id =		get_id("PrevButton");
	anal_id =		get_id("AnalButton");
	bdsm_id =		get_id("BDSMButton");
	sex_id =		get_id("SexButton");
	beast_id =		get_id("BeastButton");
	group_id =		get_id("GroupButton");
	les_id =		get_id("LesButton");
	preg_id =		get_id("PregButton");
	death_id =		get_id("DeathButton");
	pro_id =		get_id("ProfileButton");
	combat_id =		get_id("CombatButton");
	oral_id =		get_id("OralButton");
}

void cScreenGallery::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) {
		set_ids();
	}

	//init();
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

/*
 *	otherwise, compare event IDs 
 *
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	sGirl *girl = selected_girl;

	if(g_InterfaceEvents.CheckButton(back_id)) 
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if(g_InterfaceEvents.CheckButton(anal_id)) 
	{
		if(girl->m_GirlImages->m_Images[IMGTYPE_ANAL].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, false);
	}

	static int Mode = IMGTYPE_ANAL;
	static int Img = 0;	// what image currently drawing
	g_CurrentScreen = SCREEN_GALLERY;
	if(g_InitWin)
	{
		if(girl == 0)
		{
			g_InitWin = true;
			g_MessageQue.AddToQue("ERROR: No girl selected", 1);
			g_WinManager.Pop();
			return;
		}
		g_Gallery.Focused();

		if(girl->m_GirlImages->m_Images[IMGTYPE_ANAL].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYANAL, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_BDSM].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBDSM, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBDSM, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYSEX, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYSEX, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_BEAST].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBEAST, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYBEAST, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_GROUP].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYGROUP, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYGROUP, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_LESBIAN].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYLESBIAN, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYLESBIAN, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPREGNANT, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPREGNANT, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_DEATH].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYDEATH, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYDEATH, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPROFILE, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYPROFILE, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_COMBAT].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYCOMBAT, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYCOMBAT, false);
		if(girl->m_GirlImages->m_Images[IMGTYPE_ORAL].m_NumImages == 0)
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYORAL, true);
		else
			g_Gallery.DisableButton(g_interfaceid.BUTTON_GALLERYORAL, false);

		while(girl->m_GirlImages->m_Images[Mode].m_NumImages == 0 && Mode < NUM_IMGTYPES)
		{
			Mode++;
		}

		if(Img >= girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		else if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;

		g_InitWin = false;
	}

	if(g_InterfaceEvents.GetNumEvents() != 0)
	{
		if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBACK))
		{
			g_WinManager.Pop();
			g_InitWin = true;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYANAL))
		{
			Mode=0;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBDSM))
		{
			Mode=1;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYSEX))
		{
			Mode=2;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYBEAST))
		{
			Mode=3;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYGROUP))
		{
			Mode=4;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYLESBIAN))
		{
			Mode=5;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREGNANT))
		{
			Mode=6;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYDEATH))
		{
			Mode=7;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPROFILE))
		{
			Mode=8;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYCOMBAT))
		{
			Mode=9;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYORAL))
		{
			Mode=10;
			Img=0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYPREV))
		{
			Img--;
			if(Img < 0)
				Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_GALLERYNEXT))
		{
			Img++;
			if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
				Img = 0;
			return;
		}
		else if(g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, g_interfaceid.BUTTON_NEXTGALLERY))
		{
			g_InitWin = true;
		g_WinManager.Push(Gallery2, &g_Gallery2);
			return;
		}
	}

	if(g_LeftArrow)
	{
		g_LeftArrow = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_RightArrow)
	{
		g_RightArrow = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)
			Img = 0;
		return;
	}
	if(g_A_Key)
	{
		g_A_Key = false;
		Img--;
		if(Img < 0)
			Img = girl->m_GirlImages->m_Images[Mode].m_NumImages-1;
		return;
	}
	else if(g_D_Key)
	{
		g_D_Key = false;
		Img++;
		if(Img == girl->m_GirlImages->m_Images[Mode].m_NumImages)	
			Img = 0;
		return;
	}
	if(g_W_Key)
	{
		while(1)
		{
			g_W_Key = false;
			Mode --;
			if(Mode < 0)
				Mode = 10;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_S_Key)
	{
		while(1)
		{
			g_S_Key = false;
			Mode ++;
			if(Mode > 10)
				Mode = 0;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
		if(g_UpArrow)
	{
		while(1)
		{
			g_UpArrow = false;
			Mode --;
			if(Mode < 0)
				Mode = 10;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	else if(g_DownArrow)
	{
		while(1)
		{
			g_DownArrow = false;
			Mode ++;
			if(Mode > 10)
				Mode = 0;
			Img = 0;
			if(girl->m_GirlImages->m_Images[Mode].m_NumImages > 0)   // This hack will only work as long as the Mode numbers are the same as the IMG type.
				break;
		}
	}
	if (Mode == NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}
	// Set the text for gallery type
	string galtxt = "";
	switch(Mode)
	{
	case 0:
		galtxt = "Anal";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 1:
		galtxt = "BDSM";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 2:
		galtxt = "Sex";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 3:
		galtxt = "Beastiality";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 4:
		galtxt = "Group";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 5:
		galtxt = "Lesbian";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 6:
		galtxt = "Pregnant";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 7:
		galtxt = "Death";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 8:
		galtxt = "Profile";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 9:
		galtxt = "Combat";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	case 10:
		galtxt = "Oral";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	default:
		galtxt = "";
		g_Gallery.EditTextItem(galtxt,g_interfaceid.TEXT_GALLERYTYPE);
		break;
	}
	// Draw the image
	if(girl)
	{
		g_Gallery.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetImageSurface(girl, Mode, false, Img, true));
		if(g_Girls.IsAnimatedSurface(girl, Mode, Img))
			g_Gallery.SetImage(g_interfaceid.IMAGE_TSIMAGE, g_Girls.GetAnimatedSurface(girl, Mode, Img));
	}
}