#include "cScreenMainMenu.h"
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"

extern void NewGame();
extern void LoadGameScreen();
extern void GetString();

extern cWindowManager g_WinManager;

extern bool g_InitWin;
extern int g_CurrentScreen;

bool cScreenMainMenu::ids_set = false;

void cScreenMainMenu::set_ids()
{
	ids_set = true;
	new_id = get_id("New Game");
	load_id = get_id("Load Game");
	quit_id = get_id("Quit Game");
}

void cScreenMainMenu::init()
{
	g_CurrentScreen = SCREEN_MAINMENU;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		g_Girls.GetImgManager()->LoadList("Default");
	}
}

void cScreenMainMenu::process()
{
	// we need to make sure the ID variables are set
	if (!ids_set)
		set_ids();

	// handle arrow keys
	if (check_keys())
		return;

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}

void cScreenMainMenu::check_events()
{
	// no events means we can go home
	if (g_InterfaceEvents.GetNumEvents() == 0)
		return;

	if (g_InterfaceEvents.CheckButton(new_id))
	{
		cGetStringScreenManager gssm;
		gssm.empty_allowed(false);
		gssm.set_handler(NewGame);
		g_WinManager.Push(GetString, &g_GetString);
		g_MessageQue.AddToQue("Enter a name for your first brothel.", 0);
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(load_id))
	{
		g_WinManager.Push(LoadGameScreen, &g_LoadGame);
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(quit_id))
	{
		SDL_Event evn;
		evn.type = SDL_QUIT;
		SDL_PushEvent(&evn);
	}
}

bool cScreenMainMenu::check_keys()
{
	return false;
}