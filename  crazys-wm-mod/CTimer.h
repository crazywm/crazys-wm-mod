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
#ifndef __CTIMER_H
#define __CTIMER_H

#include <SDL.h>

// Frame Rate for games
const int FRAMES_PER_SECOND = 25;

class CTimer
{
public:
	CTimer() {m_StartTicks = m_PausedTicks = 0; m_Paused = m_Started = false;}
	~CTimer() = default;

	void Start() {m_Paused = false; m_Started = true; m_StartTicks = SDL_GetTicks();}
	void Stop() {m_Paused = m_Started = false;}
	void Pause(bool pause)
	{
		if(pause)
		{
			if(m_Started && !m_Paused)
			{
				m_Paused = true;
				m_PausedTicks = SDL_GetTicks() - m_StartTicks;
			}
		}
		else
		{
			m_Paused = false;
			m_StartTicks = SDL_GetTicks() - m_PausedTicks;
			m_PausedTicks = 0;
		}
	}

	int GetTicks()
	{
		if(m_Started)
		{
			if(m_Paused)
				return m_PausedTicks;
			else
				return (SDL_GetTicks() - m_StartTicks);
		}
		return 0;
	}

	bool IsStarted() {return m_Started;}
	bool IsPaused() {return m_Paused;}

private:
	int m_StartTicks;
	int m_PausedTicks;

	bool m_Paused;
	bool m_Started;
};

#endif
