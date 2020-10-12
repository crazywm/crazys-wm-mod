#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "CLog.h"
#include "sConfig.h"

class cScreenGetInput;

// define the global variables that are currently expected by the engine code
CLog g_LogFile;
cConfig cfg;
cScreenGetInput* g_GetInput = nullptr;
