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
#include <algorithm>
#include <math.h>
#include "cRng.h"
#include "CLog.h"
#include "cGirls.h"
#include "cTrainable.h"
#include "Constants.h"
#include "libintl.h"

extern	cRng	g_Dice;
extern	CLog	g_LogFile;
extern	cGirls	g_Girls;

int cTrainable::value()
{
	if(m_type == Stat) {
		return m_girl->get_stat(m_index);
	}
	return m_girl->get_skill(m_index);
}

void cTrainable::upd(int increment)
{
	m_gain += increment;
	if(m_type == Stat) {
		m_girl->upd_stat(m_index, increment);
	}
	m_girl->upd_skill(m_index, increment);
}

TrainableGirl::TrainableGirl(sGirl *girl)
{
	stats.push_back(TrainableStat(girl, "Charisma", STAT_CHARISMA));
	stats.push_back(TrainableStat(girl, "Constitution", STAT_CONSTITUTION));
	stats.push_back(TrainableStat(girl, "Libido", STAT_LIBIDO));
	stats.push_back(TrainableSkill(girl, "Anal", SKILL_ANAL));
	stats.push_back(TrainableSkill(girl, "Magic", SKILL_MAGIC));
	stats.push_back(TrainableSkill(girl, "BDSM", SKILL_BDSM));
	stats.push_back(TrainableSkill(girl, "Normal Sex", SKILL_NORMALSEX));
	stats.push_back(TrainableSkill(girl, "Beastiality", SKILL_BEASTIALITY));
	stats.push_back(TrainableSkill(girl, "Group", SKILL_GROUP));
	stats.push_back(TrainableSkill(girl, "Lesbian", SKILL_LESBIAN));
	stats.push_back(TrainableSkill(girl, "Service", SKILL_SERVICE));
	stats.push_back(TrainableSkill(girl, "Stripping", SKILL_STRIP));
	stats.push_back(TrainableSkill(girl, "Combat", SKILL_COMBAT));
	stats.push_back(TrainableSkill(girl, "Oral Sex", SKILL_ORALSEX));
	stats.push_back(TrainableSkill(girl, "Titty Sex", SKILL_TITTYSEX));
	stats.push_back(TrainableSkill(girl, "Hand Job", SKILL_HANDJOB));
	stats.push_back(TrainableSkill(girl, "Foot Job", SKILL_FOOTJOB));
	stats.push_back(TrainableSkill(girl, "Medicine", SKILL_MEDICINE));
	stats.push_back(TrainableSkill(girl, "Performance", SKILL_PERFORMANCE));

	// MYR
	m_girl = girl;
}

string TrainableGirl::update_random(int size)
{
	u_int index = g_Dice.random(stats.size());
	stats[index].upd(size);
	return stats[index].name();
}

IdealAttr::IdealAttr(vector<TrainableGirl> set, string name, int attr_idx)
: cTrainable(0, name, attr_idx, cTrainable::Stat)
{
	m_value		= 0;
	m_potential	= 0;
	m_attr_idx	=  attr_idx;
	m_rand		= g_Dice.random(100);
/*
 *	we're going to loop over the girls in the training set
 *
 *	and get the highest value for the attribute specified by
 *	attr_idx. That's TrainableGirl indices,
 *	not the SKILL_ and STAT_ constants, dig?
 */
	for(u_int i = 0; i < set.size(); i++) {
/*
 *		get the girl for this iteration
 */
		TrainableGirl &trainee = set[i];
/*
 *		get the current value of the attr in question
 */
		int candidate_value = trainee[attr_idx].value();
/*
 *		if that value is higher than the one we
 *		already have, we use that
 */
		if(candidate_value > m_value) {
			m_value = candidate_value;
		}
	}
/*
 *	Now, we want to see which chars have the highest 
 *	potential. We do that by summing the differences
 *	from the ideal
 */
	for(u_int i = 0; i < set.size(); i++) {
		TrainableGirl &trainee = set[i];
		int diff = m_value - trainee[attr_idx].value();
		m_potential += diff;
	}
}

IdealGirl::IdealGirl(vector<TrainableGirl> set)
{
	TrainableGirl &girl_zero = set[0];
/*
 *	just set up the attribues. use the first girl in the set
 */
	for(u_int i = 0; i < girl_zero.size(); i++) {
		stats.push_back(IdealAttr( set, girl_zero[i].name(), i));
	}
}

/*
 * this is a sort predicate
 */
static bool sort_IdealAttr(const IdealAttr& a1, const IdealAttr& a2)
{
	const int v1 = a1.potential();
	const int v2 = a2.potential();
/*
 *	if the potential gains are different, sort on them
 *	using "v2 < v1" because we want a descending sort
 */
	if(v1 != v2) {
		return v2 < v1;
	}
/*
 *	it would be nice if, when several attributes with the same potential
 *	exist, we pick one at random rather than use the same predictable
 *	top down order.
 *
 *	As it happens, all we need is a secondary sort field set
 *	to a random number. So this is it.
 */
	return a2.noise() < a1.noise();
}

/*
 * OK: we need to find the three attributes with the greatest
 * potential gains for the group, and return them wrapped in a vector
 *
 * so:	copy the vector;
 *	sort it
 *	take the top three indices
 *	put them in their own vector
 *	and return that.
 *
 * Easy!
 */
vector<int> IdealGirl::training_indices()
{
/*
 *	to start: copy the vector
 */
 	vector<IdealAttr> copyvec(stats.begin(), stats.end());
/*
 *	now: sort it
 */
 	sort(copyvec.begin(), copyvec.end(), sort_IdealAttr);
/*
 *	OK: now we need an integer vector
 *
 *	(I could use an array here - but I want to make this
 *	easy to change if I need to make the number of
 *	atrributes trained into a config variable
 */
 	vector<int> indices;
/*
 *	and we use it to store the attribute indices
 *	of the top three elements (or less if there are less)
 */
 	for(u_int i = 0; i < 3; i++) {
		//indices[i] = copyvec[i].attr_index();
		indices.push_back(copyvec[i].attr_index());
	}
	return indices;
}

