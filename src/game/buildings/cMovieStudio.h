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

#include "IBuilding.h"
#include "cJobManager.h"
#include "movies/fwd.h"
#include <vector>

// defines a single studio
struct sMovieStudio : public IBuilding
{
    sMovieStudio();                            // constructor
    ~sMovieStudio();                        // destructor

    void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) override;
    void UpdateGirls(bool is_night) override;

    void Update() override;

    std::string meet_no_luck() const override;

    int GetNumScenes() const;

    // added the following so movie crew could effect quality of each scene. --PP
    int m_FluffPoints;           // How much fluffer work there was this shif
    int m_StageHandPoints;       // How much work the stage hands provide this shift

    struct WorkerData {
        sGirl* Worker;
        int ScenesFilmed = 0;
    };
    std::vector<WorkerData> m_CameraMages;
    std::vector<WorkerData> m_CrystalPurifiers;
    std::vector<WorkerData> m_Directors;

private:
    void auto_create_movies();

    void check_camera_mages_overuse();
    void check_purifier_overuse();
    void check_director_overuse();
};

#endif  /* __CMOVIESTUDIO_H */
