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
#include "character/cPlayer.h"

void FilmThroat::DoScene(sGirl& girl) {
    int hate = 0, tired = 0;
    int OPTION = uniform(0, 3);
    if (result.performance >= 350)
    {
        switch (OPTION)
        {
            case 0:
                ss << ("${name} knelt in front of him, opened wide, and swallowed down his whole length, bouncing back and forward until he came ");
                break;
            case 1:
                ss << ("${name} lay on her back on the bed with her head hanging back over the edge. Gripping her breasts, he rammed his cock down her throat until cum exploded ");
                break;
            case 2:
                ss << ("The actor sat on a chair, with ${name} knelt before him. Gripping her head, he pulled her onto his cock, rocking her back and forward on his cock until he came ");
                break;
            default:
                ss << ("${name} did some kind of throatfuck until he came ");
                break;
        }

        ss << rng().select_text({"deep down her throat", "in her mouth", "deep in her stomach"}) << ".\n";
        result.bonus = 12;
        hate = 1;
        tired = 1;
    }
    else if (result.performance >= 245)
    {
        switch (OPTION)
        {
            case 0:
                ss << ("${name} knelt in front of him and sucked his cock down, bouncing back and forward until he came ");
                break;
            case 1:
                ss <<  ("${name} lay back on the bed with her head over the edge. He fucked her throat until he came ");
                break;
            case 2:
                ss << ("The actor sat on a chair, with ${name} before him. Gripping her head, he pulled her onto his cock, back and forth until he came ");
                break;
            default:
                ss << "${name} did some kind of throatfuck until he came ";
                break;
        }

        ss << rng().select_text({"all over her", "in her mouth", "deep down her throat"});
        hate = 2;
        tired = 4;
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        switch (OPTION)
        {
            case 0:
                ss << ("${name} knelt down and let him facefuck her until he came ");
                break;
            case 1:
                ss << ("${name} lay on the bed and let him fuck her throat. He carefully facefucked her until he came ");
                break;
            case 2:
                ss << ("The actor pulled ${name}'s head onto his cock, rubbing his cock down her throat until he came ");
                break;
            default:
                ss << ("${name} did some kind of throatfuck until he came ");
                break;
        }
        ss << rng().select_text({"all over her", "in her mouth", "deep down her throat"});
        result.bonus = 4;
        hate = 3;
        tired = 8;
    }
    else if (result.performance >= 145)
    {
        switch (OPTION)
        {
            case 0:
                ss << ("${name} knelt down and get his cock down her throat. Eventually he came ");
                break;
            case 1:
                ss << ("${name} lay on the bed and tried to take his meat in her throat. He facefucked her until he finally came ");
                break;
            case 2:
                ss << ("The actor pulled ${name}'s head onto his cock, making her gag, until he came ");
                break;
            default:
                ss << ("${name} did some kind of throatfuck until he came ");
                break;
        }

        ss << rng().select_text({"in her face", "in her mouth", "over her face"});
        result.bonus = 2;
        hate = 5;
        tired = 10;
    }
    else if (result.performance >= 100)
    {
        ss << ("It was a pretty awkward scene, with the actor not getting much pleasure trying to fuck her throat. ");
        ss << ("In the end he gave up and plunged his cock down her throat, making her throw up through her nose, as he came in her head.");
        result.bonus = 1;
        hate = 8;
        tired = 12;
    }
    else
    {
        ss << ("With her continual gagging, retching and vomittig the actor couldn't get any pleasure, and ended up wanking over her face.");
        hate = 8;
        tired = 12;
    }
    ss << ("\n");

    //Enjoyed?! No, but some will hate it less.
    PerformanceToEnjoyment("She won't say it, but you suspect she secretly gets off on the degradation.",
                           "She's pretty raw and her makeup is everywhere.",
                           "From the way she's coughing and gagging and has bright red eyes, you suspect this wasn't much fun for her."
                           );
    result.bonus += result.enjoy;


    int impact = uniform(0, 10);
    if (girl.has_active_trait("Strong Gag Reflex"))
    {
        ss << "She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterward.\n \n";
        girl.health((10 + impact));
        girl.tiredness((10 + impact + tired));
        girl.pchate((2 + hate));
        girl.pcfear((2 + hate));
    }
    else if (girl.has_active_trait("Gag Reflex"))
    {
        ss << "She gagged and retched a lot. It was exhausting and left her feeling sick.\n \n";
        girl.health((2 + impact));
        girl.tiredness((5 + impact + tired));
        girl.pchate(hate);
        girl.pcfear(hate);
    }
}

bool FilmThroat::CheckRefuseWork(sGirl& girl) {
    if (girl.health() < 20)
    {
        ss << ("The crew refused to film a throatjob scene because ${name} is not healthy enough.\n\"We are NOT filming snuff\".");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    else if (girl.has_active_trait("Mind Fucked"))
    {
        ss << ("Mind fucked ${name} was pleased to be paraded naked in public and tortured and degraded. It is natural.\n");
        result.bonus += 10;
    }
    else if (girl.has_active_trait("Masochist"))
    {
        ss << ("Masochist ${name} was pleased to be tortured, used and degraded. It is her place.\n");
        result.bonus += 6;
    }
    else if (girl.disobey_check(ACTION_WORKMOVIE, JOB_FILMFACEFUCK))
    {
        ss << ("${name} angrily refused to be throat-fucked on film today.");
        if (girl.is_slave())
        {
            if (g_Game->player().disposition() > 30)  // nice
            {
                ss << ("\nThough she is a slave, she was upset so you allowed her the day off.\n");
                girl.pclove(2);
                girl.spirit(1);
                girl.pchate(-1);
                girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
                return true;
            }
            else if (g_Game->player().disposition() > -30) //pragmatic
            {
                ss << (" Amused, you over-ruled her, and gave owner's consent for her. She glared at you as they dragged her away.\n");
                girl.pclove(-1);
                girl.pchate(2);
                girl.pcfear(2);
                g_Game->player().disposition(-1);
                result.enjoy -= 2;
            }
            else if (g_Game->player().disposition() > -30)
            {
                ss << (" Amused, you over-ruled her, and gave owner's consent.\nShe made a hell of a fuss, but you knew just the thing to shut her up.");
                girl.pclove(-4);
                girl.pchate(+5);
                girl.pcfear(+5);
                g_Game->player().disposition(-2);
                result.enjoy -= 6;
            }
        }
        else // not a slave
        {
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
    }
    else ss << "${name} was filmed in facefucking scenes.\n \n";
    return false;
}

void FilmThroat::GainTraits(sGirl& girl, int performance) const {
    if (result.performance > 200) cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", false);
    if (chance(5) && (girl.happiness() > 80) && (girl.get_enjoyment(ACTION_WORKMOVIE) > 75))
        cGirls::AdjustTraitGroupGagReflex(girl, 1, true);

    //lose
    if (chance(5)) cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, "Somewhere between having a dick in her throat, balls slapping her eyes and a camera watching her retch, ${name} has lost her iron will.", false);
}

FilmThroat::FilmThroat() : GenericFilmJob(JOB_FILMFACEFUCK, {
      IMGTYPE_ORAL, ACTION_SEX, SKILL_ORALSEX, 50, 5,
      FilmJobData::EVIL, SKILL_ORALSEX, "Facefucking",
      " worked filming teaser scenes to arouse viewers.",
      " refused to make a teaser clip today.",
}) {
    set_performance_data("work.film.throat", {STAT_CHARISMA, STAT_BEAUTY}, {SKILL_ORALSEX});
    add_trait_chance(GainMasochist);
    add_trait_chance({true, "Mind Fucked", 90, ACTION_WORKMOVIE, "She has been abused so much she is now completely Mind Fucked."});
}
