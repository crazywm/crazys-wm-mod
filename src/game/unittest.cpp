#include "CLog.h"
#include "sConfig.h"
#include "cRng.h"
#include "IGame.h"
#include "cNameList.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

class cScreenGetInput;

// define the global variables that are currently expected by the engine code
CLog g_LogFile;
cConfig cfg;
cRng g_Dice;
std::unique_ptr<IGame> g_Game;
cNameList g_GirlNameList;
cNameList g_BoysNameList;
cNameList g_SurnameList;
std::string monthnames[13]
{
        "No Month", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};
bool g_AltKeys = true;          // Toggles the alternate hotkeys --PP
bool playershopinventory = false;