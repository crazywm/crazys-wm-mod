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

#include "cBrothel.h"
#include "cGold.h"

using namespace std;

// holds data for movies
typedef struct sMovieScene
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
	long m_Init_Quality;
	long m_Quality;
	long m_Promo_Quality;
	long m_Money_Made;
	long m_RunWeeks;
	sMovieScene()		{  }
	~sMovieScene()		{  }
	void OutputSceneRow(string* Data, const vector<string>& columnNames);
	void OutputSceneDetailString(string& Data, const string& detailName);
}sMovieScene;

// defines a single studio
typedef struct sMovieStudio : sBrothel
{
	sMovieStudio();							// constructor
	~sMovieStudio();						// destructor
	unsigned short	m_var;	// customers used for temp purposes but must still be taken into account
	cGold			m_Finance;
	
	TiXmlElement* SaveMovieStudioXML(TiXmlElement* pRoot);
	bool LoadMovieStudioXML(TiXmlHandle hBrothel);

}sMovieStudio;


/*
 * manages the studio
 *
 * extend cBrothelManager
 */
class cMovieStudioManager : public cBrothelManager
{
public:
	cMovieStudioManager();					// constructor
	~cMovieStudioManager();					// destructor

	int				m_MovieRunTime;			// see above, counter for the 7 week effect
	int				m_NumMovies;
	long			RunWeeks;
	sMovie*			m_Movies;				// the movies currently selling
	sMovie*			m_LastMovies;
	sFilm *			m_CurrFilm;
	// added the following so movie crew could effect quality of each scene. --PP
	int				m_FlufferQuality;		// Bonus to film quality based on performance of Fluffers this shift.
	int				m_CameraQuality;		// Bonus to film quality based on performance of Cameramages this shift.
	int				m_PurifierQaulity;		// Bonus to film quality based on performance of CrystalPurifiers this shift.
	int				m_DirectorQuality;		// Bonus to film quality based on performance of  the Director this shift.
	string			m_DirectorName;			// The Director's name.
	int				m_StagehandQuality;		// Bonus to film quality based on performance of stagehands this shift.
	double			m_PromoterBonus;		// Bonus added directly to film sales by promoter.
	int				m_TotalScenesFilmed;	// `J` added for .06.02.55

	void StartMovie(int brothelID, int Time);
	int  GetTimeToMovie(int brothelID);
	void NewMovie(sMovieStudio* brothel, string Name, string Director, string Cast, string Crew, int Init_Quality, int Quality, int Promo_Quality, int Money_Made, int RunWeeks);
	void EndMovie(sBrothel* brothel);
	bool CheckMovieGirls(sBrothel* brothel);	// checks if any girls are working on the movie
	long calc_movie_quality(bool autoreleased = false);
	void ReleaseCurrentMovie(bool autoreleased = false, bool save = false);
	string BuildDirectorList(bool autoreleased = false, bool save = false);
	string BuildCastList(bool autoreleased = false, bool save = false);
	string BuildCrewList(bool autoreleased = false, bool save = false);

	sMovieScene* GetScene(int num);
	sMovieScene* GetMovieScene(int num);
	int GetNumScenes();
	int GetNumMovieScenes();
	vector<int> AddSceneToMovie(int num);
	vector<int> RemoveSceneFromMovie(int num);
	int MovieSceneUp(int num);
	int MovieSceneDown(int num);
	int DeleteScene(int num);
	void SortMovieScenes();

	vector<sMovieScene*> m_availableScenes;
	vector<sMovieScene*> m_movieScenes;
	int AddScene(sGirl* girl, int Job, int Bonus = 0, sGirl* Director = NULL, sGirl* CM = NULL, sGirl* CP = NULL);	// Added job parameter so different types of sex could effect film quality. --PP
	void LoadScene(int m_SceneNum, string m_Name, string m_Actress, string m_Director, int m_Job, long m_Init_Quality, long m_Quality, long m_Promo_Quality, long m_Money_Made, long m_RunWeeks, int m_MovieSceneNum, string m_CM = "", string m_CP = "");
	void AddGirl(int brothelID, sGirl* girl, bool keepjob = false);
	void RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = false);
	void UpdateMovieStudio();
	void UpdateGirls(sBrothel* brothel);
	//void	AddBrothel(sBrothel* newBroth);
	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);
	void Free();
	int m_NumMovieStudios;
	cJobManager m_JobManager;
	
	int Num_Actress(int brothel);
	bool is_Actress_Job(int testjob);
	bool CrewNeeded();
};


#endif  /* __CMOVIESTUDIO_H */
