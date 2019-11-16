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

#pragma once

#ifndef __CMOVIESTUDIO_H
#define __CMOVIESTUDIO_H

#include "buildings/cBrothel.h"
#include "cGold.h"
#include "cJobManager.h"

using namespace std;

// holds data for movies
struct sMovie
{
    string m_Name;
    string m_Director;
    string m_Cast;
    string m_Crew;
    int m_Init_Quality;
    int m_Promo_Quality;
    int m_Quality;
    int m_Money_Made;
    int m_RunWeeks;
};


// holds data for movies
struct sMovieScene
{
	int m_SceneNum;
	int m_MovieSceneNum;
	int m_Row;
	int m_RowM;
	string m_Name;
	string m_Actress;
	string m_Director;
	string m_CM;
	string m_CP;
	int m_Job;
	int m_Init_Quality;
	int m_Quality;
	int m_Promo_Quality;
	int m_Money_Made;
	int m_RunWeeks;
	sMovieScene()		= default;
	~sMovieScene()		= default;
	void OutputSceneRow(vector<string>& Data, const vector<string>& columnNames);
	void OutputSceneDetailString(string& Data, const string& detailName);
};

// defines a single studio
struct sMovieStudio : public IBuilding
{
	sMovieStudio();							// constructor
	~sMovieStudio();						// destructor

    void load_xml(tinyxml2::XMLElement& root) override;
    void save_additional_xml(tinyxml2::XMLElement& root) const override;

    void auto_assign_job(sGirl * target, std::stringstream& message, bool is_night) override;
    void UpdateGirls(bool is_night) override;

    void Update() override;

    int AddScene(sGirl* girl, int Job, int Bonus, int jobType, const char* scene_name);
    void LoadScene(int m_SceneNum, string m_Name, string m_Actress, string m_Director, int m_Job, long m_Init_Quality, long m_Quality, long m_Promo_Quality, long m_Money_Made, long m_RunWeeks, int m_MovieSceneNum, string m_CM = "", string m_CP = "");
    void NewMovie(string Name, string Director, string Cast, string Crew, int Init_Quality, int Quality,
                  int Promo_Quality, int Money_Made, int RunWeeks);
    void EndMovie();
    long calc_movie_quality(bool autoreleased = false);
    void ReleaseCurrentMovie(bool autoreleased = false, bool save = false);
    string BuildDirectorList(bool autoreleased = false, bool save = false);
    string BuildCastList(bool autoreleased = false, bool save = false);
    string BuildCrewList(bool autoreleased = false, bool save = false);

    sMovieScene* GetScene(int num);
    sMovieScene* GetMovieScene(int num);
    int GetNumScenes() const;
    int GetNumMovieScenes() const;
    vector<int> AddSceneToMovie(int num);
    vector<int> RemoveSceneFromMovie(int num);
    int MovieSceneUp(int num);
    int MovieSceneDown(int num);
    int DeleteScene(int num);
    void SortMovieScenes();

    vector<sMovieScene*> m_availableScenes;
    vector<sMovieScene*> m_movieScenes;

    int				m_MovieRunTime = 0;		// see above, counter for the 7 week effect

    std::vector<sMovie> m_Movies;   // the movies currently selling
    sFilm *			m_CurrFilm    = nullptr;

    // added the following so movie crew could effect quality of each scene. --PP
    int				m_FlufferQuality;		// Bonus to film quality based on performance of Fluffers this shift.
    int				m_CameraQuality;		// Bonus to film quality based on performance of Cameramages this shift.
    int				m_PurifierQaulity;		// Bonus to film quality based on performance of CrystalPurifiers this shift.
    int				m_DirectorQuality;		// Bonus to film quality based on performance of  the Director this shift.
    string			m_DirectorName;			// The Director's name.
    int				m_StagehandQuality;		// Bonus to film quality based on performance of stagehands this shift.
    double			m_PromoterBonus;		// Bonus added directly to film sales by promoter.
    int				m_TotalScenesFilmed;	// `J` added for .06.02.55
};

#endif  /* __CMOVIESTUDIO_H */
