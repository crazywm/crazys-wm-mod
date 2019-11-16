/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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
#include "Films.h"
#include "Game.hpp"
#include "character/cCustomers.h"
#include "character/predicates.h"

void FilmSex::DoScene(sGirl& girl) {
    int roll = d100();
    if (roll <= 10) { result.enjoy -= uniform(1, 4);	    ss << "She didn't want sex today, but she did it anyway.\n \n"; }
    else if (roll >= 90) { result.enjoy += uniform(1, 4);	ss << "She loved having a man in her pussy today.\n \n"; }
    else /*            */{ result.enjoy += uniform(0, 2);	ss << "She had a pleasant day fucking her co-star.\n \n"; }
    result.performance += result.enjoy * 2;

    if (girl.lose_trait("Virgin"))
    {
        result.performance += 50;
        ss << "She is no longer a virgin.\n";
    }
}

void FilmSex::GainTraits(sGirl& girl, int performance) const {
    auto brothel = girl.m_Building;
    sCustomer Cust = g_Game->GetCustomer(*brothel);
    Cust.m_Amount = 1;
    Cust.m_IsWoman = false;
    if (!girl.calc_pregnancy(Cust, 1.0))
    {
        g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
    }
}

FilmSex::FilmSex() : GenericFilmJob(JOB_FILMSEX, {
        IMGTYPE_SEX, ACTION_SEX, SKILL_NORMALSEX, 50, 0,
        FilmJobData::NORMAL, SKILL_NORMALSEX, "Sex",
        " worked as an actress filming sex scences.",
        " refused to fuck on film today.", {GainPornStar, GainSlut, GainFaker}
}) {}

double FilmSex::GetPerformance(const sGirl& girl, bool estimate) const
{
	return 0;
}
