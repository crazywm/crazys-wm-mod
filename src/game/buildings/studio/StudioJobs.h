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

#ifndef WM_STUDIOJOBS_H
#define WM_STUDIOJOBS_H

#include "jobs/BasicJob.h"
#include "data.h"

struct sFilmPleasureData {
    int Factor = 0;                             // Determines the influence of libido
    SKILLS Skill = SKILLS::NUM_SKILLS;          // Which sex skill is required
    int SkillMin = 0;                           // Minimum skill to have the filming be pleasurable
    int BaseValue = 0;                          // Base value if there is no libido at all
};

struct sFilmObedienceData {
    int total() const {
        return Base + Libido + Enjoy + LoveHate;
    }

    int Base;
    int Libido;
    int Enjoy;
    int LoveHate;

};

class cFilmSceneJob : public cBasicJob {
public:
    enum class SexAction {
        NONE,
        HUMAN,
        MONSTER
    };
    cFilmSceneJob(JOBS job, const char* xml, Image_Types event_image, SceneType scene, SexAction sex = SexAction::NONE);

    SKILLS GetSexType() const;
    sFilmObedienceData CalcChanceToObey(const sGirl& girl) const;
private:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    void InitWork() override;
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;

    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;

    // Helper function that is called if the girl refuses to film the scene,
    // but it is a scene that can be filmed with her tied up against her will.
    bool RefusedTieUp(sGirl& girl);

    // scene properties
    int m_MinimumHealth = -100;
    bool m_RefuseIfPregnant = false;
    bool m_CanBeForced = false;

    SexAction m_SexAction = SexAction::NONE;

    Image_Types m_EventImage;
    SceneType m_SceneType;

    // enjoyment
    sFilmPleasureData m_PleasureFactor;         // Determines the influence of libido
    Action_Types m_PrimaryAction;               // Actress or Pornstar
    Action_Types m_SecondaryAction = Action_Types::NUM_ACTIONTYPES;

protected:
    // Processing Data
    int m_Enjoyment;
    bool m_IsForced;


    virtual void PreFilmCallback(sGirl& girl);
    virtual void PostFilmCallback(sGirl& girl);

    virtual void Narrate(sGirl& girl);
    void PrintPerfSceneEval();
    void PrintForcedSceneEval();

    virtual bool CheckRefuseWork(sGirl& girl);
    virtual bool CheckCanWork(sGirl& girl);

    mutable std::stringstream m_Dbg_Msg;

    void update_enjoyment(sGirl& girl) const;

    void produce_debug_message(sGirl& girl) const;
};

class cCrewJob : public cBasicJob {
public:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    using cBasicJob::cBasicJob;
protected:
    Image_Types m_EventImage = IMGTYPE_STUDIO_CREW;
private:
    virtual void HandleUpdate(sGirl& girl, float performance) = 0;
};

#endif //WM_STUDIOJOBS_H
