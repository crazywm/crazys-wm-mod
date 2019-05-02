/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include <string>
#include <vector>
using namespace std;

struct sGirl;
struct sBrothel;
struct sGang;

typedef unsigned int u_int;

struct sFilm
{
	//int total_girls;
	//int total_cost;
	float quality_multiplyer;
	vector<int> scene_quality;
	char time;
	int final_quality;
	bool sex_acts_flags[5];
	int total_customers;
};

struct sJobs
{
	string m_Name;
	bool m_FreeGirlAble;
	bool m_SlaveAble;
	short m_MinPositions;
	short m_MaxPositions;
	// this looked like a comment that lost its // -- doc
	//to much work atm
	char m_Upkeep;
	char m_GirlPay;
	int m_ActionID;
	string m_LikeWork;
	string m_HateWork;
	//string m_BrothelStatToUpdate;
	int m_DependantSkill;
	sJobs(void) = default;
	void setup(
		string name,
		int min=0, int max=-1,
		bool slave=true,
		bool free=true,
		char girlpay=0,
		char up=0,
		int id=0,
		string like="",
		string hate="",
		//string brothel_stat="",
		int skill=0
	)
	{
		m_Name=name;
		m_LikeWork=like;
		m_HateWork=hate;
		//m_BrothelStatToUpdate=brothel_stat;
		m_DependantSkill=skill;
		m_FreeGirlAble=free;
		m_SlaveAble=slave;
		m_MinPositions=min;
		m_MaxPositions=max;
		m_Upkeep=up;
		m_GirlPay=girlpay;
		m_ActionID=id;
	}
	~sJobs(void);
};
//mainly a list of functions 
class cJobManager
{
	vector<sFilm *> film_list;
public:
	static vector<sJobs> job_list;
	void setup_job_list();
	bool generic_job(sGirl * girl,int job_num,sBrothel *,string &,int);
	void WorkFreetime(sGirl* girl, sBrothel* brothel, int DayNight);
	void WorkTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkBar(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkHall(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkShow(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkCleaning(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkMatron(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkSecurity(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkAdvertising(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	bool WorkTorturer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	void customer_rape(sGirl*);
	bool gang_stops_rape(sGirl* girl, sGang *gang, int chance, int day_night);
	bool girl_fights_rape(sGirl*, int);
	int guard_coverage(vector<sGang*> *v=nullptr);
	bool work_related_violence(sGirl*, int, bool);
	bool explore_catacombs(sGirl *);
	bool beast_capture(sGirl * girl,sBrothel * brothel,string& summary,int);
	bool work_show(sGirl * girl,sBrothel * brothel,string& summary,char job_id,int DayNight);
	void update_film(sBrothel *);
	long make_money_films();

	//helpers
	string get_job_name(int id){return job_list[id].m_Name;}
	vector<sGirl*> girls_on_job(sBrothel *brothel, u_int job_wanted, int day_or_night);
	sGirl **get_training_set(vector<sGirl*> &v);
	void do_training(sBrothel* brothel, int DayNight);
	void do_training_set(sGirl **girls, int day_night);
	void do_solo_training(sGirl *girl, int day_night);
};
