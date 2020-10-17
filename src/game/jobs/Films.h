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


#ifndef WM_JOBS_FILMS_H
#define WM_JOBS_FILMS_H

#include "Film.h"
// common includes
#include "character/sGirl.h"
#include "cGirls.h"

struct FilmAction : public GenericFilmJob {
    FilmAction();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};


struct FilmBeast : public GenericFilmJob {
    FilmBeast();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    bool CheckRefuseWork(sGirl& girl) override;
    bool CheckCanWork(sGirl& girl) override;
    void Reset() override;

    int tied = false;
};

struct FilmBdsm : public GenericFilmJob {
    FilmBdsm();
    void DoScene(sGirl& girl) override;
    bool CheckRefuseWork(sGirl& girl) override;
};

struct FilmBuk : public GenericFilmJob {
    FilmBuk();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    bool CheckRefuseWork(sGirl& girl) override;
    void Reset() override;

    int tied = false;
};

struct FilmChef : public GenericFilmJob {
    FilmChef();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
};

struct FilmGroup : public GenericFilmJob {
    FilmGroup();
    void DoScene(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

struct FilmMusic : public GenericFilmJob {
    FilmMusic();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
};

struct FilmOral : public GenericFilmJob {
    FilmOral();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
};


struct FilmPubBDSM : public GenericFilmJob {
    FilmPubBDSM();;
    bool CheckRefuseWork(sGirl& girl) override;
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    void Reset() override;

    bool hard = false;
    bool throat = false;
    int impact = 0;
    enum { NONE, BYMAN = 1, BYBEAST }
            fucked = NONE;
};

struct FilmSex : public GenericFilmJob {
    FilmSex();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

struct FilmTease: public GenericFilmJob {
    FilmTease();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
};

struct FilmThroat: public GenericFilmJob {
    FilmThroat();
    void DoScene(sGirl& girl) override;
    void GainTraits(sGirl& girl, int performance) const override;
    bool CheckRefuseWork(sGirl& girl) override;
};

#endif //WM_JOBS_FILMS_H
