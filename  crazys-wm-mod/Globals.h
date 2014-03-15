#pragma once

class Globals
{
public:
	
	int g_ScreenWidth, g_ScreenHeight;
	bool g_Fullscreen;

	Globals();
	~Globals();
};

extern Globals _G;
