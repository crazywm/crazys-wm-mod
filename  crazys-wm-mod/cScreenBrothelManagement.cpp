#include "cBrothel.h"
#include "cWindowManager.h"
#include "cMessageBox.h"
#include "InterfaceProcesses.h"
#include "InterfaceGlobals.h"
#include "cScreenBrothelManagement.h"
#include "libintl.h"

extern CSurface *g_BrothelImages[];

extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cMessageQue g_MessageQue;

extern int g_CurrentScreen;
extern int g_CurrBrothel;
extern unsigned long g_Day, g_Month, g_Year;
extern bool g_InitWin;

extern void confirm_exit();
extern void Turnsummary();
extern void SaveGameXML();

bool cScreenBrothelManagement::id_set = false;

void cScreenBrothelManagement::set_ids()
{
	id_set = true;

	id_header = get_id("Header");
	id_details = get_id("Details");
	id_image = get_id("Image");
	id_girls = get_id("Girl Management");
	id_staff = get_id("Staff Management");
	id_setup = get_id("Setup");
	id_dungeon = get_id("Dungeon");
	id_town = get_id("Visit Town");
	id_week = get_id("Next Week");
	id_save = get_id("Save");
	id_turn = get_id("Turn Summary");
	id_quit = get_id("Quit");
	id_prev = get_id("Prev");
	id_next = get_id("Next");
}

cScreenBrothelManagement::cScreenBrothelManagement()
{
	cConfig cfg;
	DirPath dp = DirPath()
		<< "Resources"
		<< "Interface"
		<< cfg.resolution.resolution()
		<< "brothel_management.xml";
	m_filename = dp.c_str();
}


cScreenBrothelManagement::~cScreenBrothelManagement()
{
}

void cScreenBrothelManagement::init()
{
	if (g_InitWin)
	{
		Focused();

		stringstream ss;
		ss << gettext("Day: ") << g_Day << gettext(" Month: ") << g_Month << gettext(" Year: ") << g_Year << gettext(" -- Brothel: ") << g_Brothels.GetName(g_CurrBrothel);
		EditTextItem(ss.str(), id_header);
		EditTextItem(g_Brothels.GetBrothelString(g_CurrBrothel), id_details);
		g_InitWin = false;
		// selected_girl = 0;

		SetImage(id_image, g_BrothelImages[g_CurrBrothel]);
	}
}

void cScreenBrothelManagement::process()
{
	g_CurrentScreen = SCREEN_BROTHEL;

	if (!id_set)
		set_ids();

	if (check_keys())
		return;

	init();

	check_events();
}

void cScreenBrothelManagement::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() != 0)
	{
		if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_girls))
		{
			g_InitWin = true;
			g_WinManager.push("Girl Management");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_staff))
		{
			g_InitWin = true;
			g_WinManager.push("Gangs");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_setup))
		{
			g_InitWin = true;
			g_WinManager.push("Building Setup");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_dungeon))
		{
			g_InitWin = true;
			g_WinManager.push("Dungeon");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_town))
		{
			g_InitWin = true;
			g_WinManager.push("Town");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_save))
		{
			g_MessageQue.AddToQue("Game Saved", 2);
			SaveGameXML(DirPath()
			<< "Saves"
			<< (g_Brothels.GetBrothel(0)->m_Name + ".gam").c_str()
			);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_week))
		{
			g_InitWin = true;
			SaveGameXML(DirPath()
			<< "Saves"
			<< "autosave.gam");
			NextWeek();
			g_WinManager.Push(Turnsummary, &g_Turnsummary);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_turn))
		{
			g_InitWin = true;
			g_CurrentScreen = SCREEN_TURNSUMMARY;
			g_WinManager.Push(Turnsummary, &g_Turnsummary);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_quit))
		{
			g_InitWin = true;
			g_WinManager.Push(confirm_exit, &g_GetString);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_next))
		{
			g_CurrBrothel++;
			if (g_CurrBrothel >= g_Brothels.GetNumBrothels())
				g_CurrBrothel = 0;
			g_InitWin = true;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_prev))
		{
			g_CurrBrothel--;
			if (g_CurrBrothel < 0)
				g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
			g_InitWin = true;
			return;
		}
	}
}

bool cScreenBrothelManagement::check_keys()
{
	/*if (g_LeftArrow)
	{
	g_CurrBrothel--;
	if (g_CurrBrothel < 0)
	g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
	g_InitWin = true;
	g_LeftArrow = false;
	return;
	}
	else if (g_RightArrow)
	{
	g_CurrBrothel++;
	if (g_CurrBrothel >= g_Brothels.GetNumBrothels())
	g_CurrBrothel = 0;
	g_InitWin = true;
	g_RightArrow = false;
	return;
	}*/

	return false;
}