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
#include <sstream>
#include "cJobManager.h"
#include "buildings/cBuildingManager.h"
#include "Game.hpp"
#include "cJobManager.h"
#include "character/predicates.h"
#include "character/cPlayer.h"

// `J` Job House - General - Learning_Job
bool WorkPersonalTraining(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_SEX;
    stringstream ss;
    if (girl.disobey_check(actiontype, JOB_PERSONALTRAINING))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "You oversee ${name}'s training.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, not needed for sex training



    // first set sex restrictions
    bool Allow_BEAST = brothel->is_sex_type_allowed(SKILL_BEASTIALITY);
    bool Allow_BDSM = brothel->is_sex_type_allowed(SKILL_BDSM);
    bool Allow_GROUP = brothel->is_sex_type_allowed(SKILL_GROUP);
    bool Allow_NORMALSEX = brothel->is_sex_type_allowed(SKILL_NORMALSEX);
    bool Allow_ANAL = brothel->is_sex_type_allowed(SKILL_ANAL);
    bool Allow_LESBIAN = brothel->is_sex_type_allowed(SKILL_LESBIAN);
    bool Allow_FOOTJOB = brothel->is_sex_type_allowed(SKILL_FOOTJOB);
    bool Allow_HANDJOB = brothel->is_sex_type_allowed(SKILL_HANDJOB);
    bool Allow_ORALSEX = brothel->is_sex_type_allowed(SKILL_ORALSEX);
    bool Allow_TITTYSEX = brothel->is_sex_type_allowed(SKILL_TITTYSEX);
    bool Allow_STRIP = brothel->is_sex_type_allowed(SKILL_STRIP);

    // if everything is banned do nothing
    if (!Allow_BEAST && !Allow_BDSM && !Allow_GROUP && !Allow_NORMALSEX && !Allow_ANAL && !Allow_LESBIAN &&
        !Allow_FOOTJOB && !Allow_HANDJOB && !Allow_ORALSEX && !Allow_TITTYSEX && !Allow_STRIP)
    {
        ss << "All sex is banned in this building so you just talk to her.";
        girl.pcfear(-(rng % 4 - 1));
        girl.pclove(+(rng % 4 - 1));
        girl.pchate(-(rng % 4 - 1));
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;
    }

    enum {
        PT_BEAST,
        PT_BDSM,
        PT_GROUP,
        PT_NORMALSEX,
        PT_ANAL,
        PT_LESBIAN,
        PT_FOOTJOB,
        PT_HANDJOB,
        PT_ORALSEX,
        PT_TITTYSEX,
        PT_STRIP
    };



    double roll_a = rng.d100();
    double roll_b = rng.d100();

    int Disp = g_Game->player().disposition();

    int PT_Fear = 0;
    int PT_Love = 0;
    int PT_Hate = 0;








    int xp = rng % 8 + 4;
    if (girl.has_active_trait("Quick Learner"))        { xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { xp -= 3; }

    int skill = 0;
    /* */if (roll_a <= 1)    skill = 14;
    else if (roll_a <= 3)    skill = 13;
    else if (roll_a <= 6)    skill = 12;
    else if (roll_a <= 10)    skill = 11;
    else if (roll_a <= 15)    skill = 10;
    else if (roll_a <= 21)    skill = 9;
    else if (roll_a <= 28)    skill = 8;
    else if (roll_a <= 37)    skill = 7;
    else if (roll_a <= 47)    skill = 6;
    else if (roll_a <= 58)    skill = 5;
    else if (roll_a <= 70)    skill = 4;
    else if (roll_a <= 83)    skill = 3;
    else /*             */    skill = 2;

    /* */if (girl.has_active_trait("Quick Learner")) skill += 1;
    else if (girl.has_active_trait("Slow Learner")) skill -= 1;


#if 0        // `J` zzzzzz - Work in Progress

    int numtypesallowed = Allow_BEAST + Allow_BDSM + Allow_GROUP + Allow_NORMALSEX + Allow_ANAL + Allow_LESBIAN + Allow_FOOTJOB + Allow_HANDJOB + Allow_ORALSEX + Allow_TITTYSEX + Allow_STRIP;

    if (numtypesallowed == 1)
    {
        ss << "You only allow one type of sex in this building so ";
        /* */if (Allow_BEAST)        { choice = PT_BEAST; }
        else if (Allow_BDSM)        { choice = PT_BDSM; }
        else if (Allow_GROUP)        { choice = PT_GROUP; }
        else if (Allow_NORMALSEX)    { choice = PT_NORMALSEX; }
        else if (Allow_ANAL)        { choice = PT_ANAL; }
        else if (Allow_LESBIAN)        { choice = PT_LESBIAN; }
        else if (Allow_FOOTJOB)        { choice = PT_FOOTJOB; }
        else if (Allow_HANDJOB)        { choice = PT_HANDJOB; }
        else if (Allow_ORALSEX)        { choice = PT_ORALSEX; }
        else if (Allow_TITTYSEX)    { choice = PT_TITTYSEX; }
        else if (Allow_STRIP)        { choice = PT_STRIP; }
    }



    if (is_virgin(girl))
    {
        // if she loves you and she is still a virgin, she will ask you to be her first.
        if (girl.pclove() > 50
            && (Allow_NORMALSEX || Allow_BDSM)    // remove this check when it gets more options and is fully independent
            )
        {
            ss << girl.name() << " tells you she loves you and she wants you to be her first. ";

            // `J` zzzzzz - these need texts and other variables.

            if (Allow_NORMALSEX)
            {
                PT_Fear -= 5;
                PT_Love += 5;
                PT_Hate -= 5;
                choice = PT_NORMALSEX;
            }
            else if (Allow_BDSM)
            {
                PT_Fear += 2;
                PT_Love += 2;
                PT_Hate += 2;
                choice = PT_BDSM;
            }


        }    // end love option

        // if all virginity-keeping options are banned but at least 1 sex option is allowed, no choice on loss of virginity
        if (choice == -1 && (Allow_BEAST || Allow_BDSM || Allow_GROUP || Allow_NORMALSEX)
            && !Allow_ANAL && !Allow_LESBIAN && !Allow_FOOTJOB && !Allow_HANDJOB && !Allow_ORALSEX && !Allow_TITTYSEX && !Allow_STRIP)
        {

            if (Allow_BEAST)        choice = PT_BEAST;
            if (Allow_BDSM)            choice = PT_BDSM;
            if (Allow_GROUP)        choice = PT_GROUP;
            if (Allow_NORMALSEX)    choice = PT_NORMALSEX;






        }



        if (choice == -1 && Disp >= -10 && Allow_NORMALSEX)
        {
            bool declines = false;
            if (Disp >= 50)                // Nice or better
            {
                ss << "She is a virgin so you ask her if she wants to let you be her first.\nShe ";
                if (rng.percent(min(Disp, 95)))    // always at least 5% decline
                {
                    PT_Fear -= 5;
                    PT_Love += 5;
                    PT_Hate -= 5;
                    choice = PT_NORMALSEX;
                    ss << "agrees so ";
                }
                else
                {
                    PT_Fear -= 3;
                    PT_Love += 3;
                    PT_Hate -= 3;
                    declines = true;
                    ss << "declines so ";
                }
            }
            else if (Disp > 10)            // Pleasant
            {
                ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
                if (roll_b <= 50)        { ss << "She declines so "; declines = true; }
                else if (roll_b <= 80)    { ss << "She agrees so ";    choice = PT_NORMALSEX; }
                else                    { ss << "She declines so ";    choice = PT_ANAL; }
            }
            else if (Disp >= -10)        // Neutral
            {
                ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
                if (roll_b <= 50)        { ss << "She declines so "; declines = true; }
                else if (roll_b <= 70)    { ss << "She agrees so ";    choice = PT_NORMALSEX; }
                else if (roll_b <= 90)    { ss << "She declines so ";    choice = PT_ANAL; }
                else                    { ss << "She refuses so instead ";    choice = PT_BDSM; }
            }
            if (declines)
            {
                if ((Allow_LESBIAN    &&    girl.m_Skills[SKILL_LESBIAN] < 100) ||
                    (Allow_FOOTJOB    &&    girl.m_Skills[SKILL_FOOTJOB] < 100) ||
                    (Allow_HANDJOB    &&    girl.m_Skills[SKILL_HANDJOB] < 100) ||
                    (Allow_ORALSEX    &&    girl.m_Skills[SKILL_ORALSEX] < 100) ||
                    (Allow_TITTYSEX    &&    girl.m_Skills[SKILL_TITTYSEX] < 100) ||
                    (Allow_STRIP    &&    girl.m_Skills[SKILL_STRIP] < 100)
                    )
                {
                    while (choice == -1)
                    {
                        switch (rng % 6)
                        {
                        case 0: if (Allow_LESBIAN    && girl.m_Skills[SKILL_LESBIAN] < 100)    { choice = PT_LESBIAN; }    break;
                        case 1: if (Allow_FOOTJOB    && girl.m_Skills[SKILL_FOOTJOB] < 100)    { choice = PT_FOOTJOB; }    break;
                        case 2:    if (Allow_HANDJOB    && girl.m_Skills[SKILL_HANDJOB] < 100)    { choice = PT_HANDJOB; }    break;
                        case 3: if (Allow_ORALSEX    && girl.m_Skills[SKILL_ORALSEX] < 100)    { choice = PT_ORALSEX; }    break;
                        case 4: if (Allow_TITTYSEX    && girl.m_Skills[SKILL_TITTYSEX] < 100){ choice = PT_TITTYSEX; }    break;
                        case 5: if (Allow_STRIP        && girl.m_Skills[SKILL_STRIP] < 100)    { choice = PT_STRIP; }        break;
                        default:
                            break;
                        }
                    };
                }
                else // she declined sex but you don't allow non sex or she has maxed non sex
                {

                }

            }





        }
        //        Allow_BEAST
        //        Allow_BDSM
        //        Allow_GROUP
        //        Allow_NORMALSEX
        //        Allow_ANAL
        //        Allow_LESBIAN
        //        Allow_FOOTJOB
        //        Allow_HANDJOB
        //        Allow_ORALSEX
        //        Allow_TITTYSEX
        //        Allow_STRIP




        else if (Disp > -50)            // not nice
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 70)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 85)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else if (Disp > -80)            //Mean
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 60)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 80)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else                                            //Evil
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 50)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 70)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else if (roll_b <= 90)    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
            else                    { ss << "Wanting her to know she is just an animal to you, ";    roll_b = 105; } // beast
        }
    }


    if (choice == -1)    // if a choice has not been made yet...
    {
        // remove maxed skills
        if (girl.m_Skills[SKILL_BEASTIALITY] > 99)    Allow_BEAST = false;
        if (girl.m_Skills[SKILL_BDSM] > 99)        Allow_BDSM = false;
        if (girl.m_Skills[SKILL_GROUP] > 99)        Allow_GROUP = false;
        if (girl.m_Skills[SKILL_NORMALSEX] > 99)    Allow_NORMALSEX = false;
        if (girl.m_Skills[SKILL_ANAL] > 99)        Allow_ANAL = false;
        if (girl.m_Skills[SKILL_LESBIAN] > 99)        Allow_LESBIAN = false;
        if (girl.m_Skills[SKILL_FOOTJOB] > 99)        Allow_FOOTJOB = false;
        if (girl.m_Skills[SKILL_HANDJOB] > 99)        Allow_HANDJOB = false;
        if (girl.m_Skills[SKILL_ORALSEX] > 99)        Allow_ORALSEX = false;
        if (girl.m_Skills[SKILL_TITTYSEX] > 99)    Allow_TITTYSEX = false;
        if (girl.m_Skills[SKILL_STRIP] > 99)        Allow_STRIP = false;




    }


















#else

    // roll_b random from 1-100 then is modified by player disposition and if is less than:
    // 7 strip | 15 les | 30 tit | 50 oral | 70 normal | 80 anal | 90 group | 100 bdsm | +beast
    // the nicer the player, the lower the roll, meaner is higher, only evil will do beast
    // will also skip down the list if the girl has 100 in the skill
    if (Disp >= 80)                //Benevolent
    {
        if (is_virgin(girl))        // 25% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\nShe ";
            if (roll_b <= 25)    { ss << "declines so "; roll_b *= 2; }
            else                { ss << "agrees so "; roll_b = 60; } // normal sex
        }
        else if (roll_b > 75) roll_b -= 8;
    }
    else if (Disp >= 50)            // nice
    {
        if (is_virgin(girl))        // 50 % decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\nShe ";
            if (roll_b <= 50)        { ss << "declines so "; }
            else                    { ss << "agrees so ";    roll_b = 60; } // normal sex
        }
        else if (roll_b > 90) roll_b -= 3;
    }
    else if (Disp > 10)            //Pleasant
    {
        if (is_virgin(girl))        // 70% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
            if (roll_b <= 50)        { ss << "She declines so "; }
            else if (roll_b <= 80)    { ss << "She agrees so ";            roll_b = 60; } // normal sex
            else                    { ss << "She declines so ";            roll_b = 75; } // anal sex
        }
        else if (roll_b > 95) roll_b *= 0.98;
    }
    else if (Disp >= -10)            // neutral
    {
        if (is_virgin(girl))        // 80% decline
        {
            ss << "She is a virgin so you ask her if she wants to let you be her first.\n";
            if (roll_b <= 50)        { ss << "She declines so "; }
            else if (roll_b <= 70)    { ss << "She agrees so ";            roll_b = 60; } // normal sex
            else if (roll_b <= 90)    { ss << "She declines so ";            roll_b = 75; } // anal sex
            else                    { ss << "She refuses so instead ";    roll_b = 95; } // BDSM
        }
    }
    else if (Disp > -50)            // not nice
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 70)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 85)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else if (roll_b < 30) roll_b *= 2;
    }
    else if (Disp > -80)            //Mean
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 60)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 80)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else                    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
        }
        else if (roll_b < 50) roll_b *= 2;
    }
    else                                            //Evil
    {
        if (is_virgin(girl))
        {
            ss << "She is a virgin, but not for long.\n";
            if (roll_b <= 50)        { ss << "Wanting her for yourself, "; roll_b = 60; } // normal
            else if (roll_b <= 70)    { ss << "Wanting to break in all her holes, ";            roll_b = 85; } // group
            else if (roll_b <= 90)    { ss << "Wanting her to know who the boss is, ";        roll_b = 95; } // bdsm
            else                    { ss << "Wanting her to know she is just an animal to you, ";    roll_b = 105; } // beast
        }
        else if (roll_b < 70) roll_b *= 2;
    }


    if (roll_b <= 10 && girl.strip() < 100)
    {
        girl.strip(skill);
        ss << "You decide to have her strip for you.\n \n";
        ss << "She managed to gain " << skill << " Strip.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_STRIP, Day0Night1);
    }
    else if (roll_b <= 20 && girl.lesbian() < 100 && brothel->is_sex_type_allowed(SKILL_LESBIAN))
    {
        girl.lesbian(skill);
        ss << "You decide to bring in another girl for her.\n \n";
        ss << "She managed to gain " << skill << " Lesbian.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_LESBIAN, Day0Night1);
    }
    else if (roll_b <= 27 && girl.tittysex() < 100 && brothel->is_sex_type_allowed(SKILL_TITTYSEX))
    {
        girl.tittysex(skill);
        ss << "You decide to have her use her tits on you.\n \n";
        ss << "She managed to gain " << skill << " Titty.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_TITTY, Day0Night1);
    }
    else if (roll_b <= 34 && girl.handjob() < 100 && brothel->is_sex_type_allowed(SKILL_HANDJOB))
    {
        girl.handjob(skill);
        ss << "You decide to teach her the art of manual stimulation.\n \n";
        ss << "She managed to gain " << skill << " Hand Job.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_HAND, Day0Night1);
    }
    else if (roll_b <= 40 && girl.footjob() < 100 && brothel->is_sex_type_allowed(SKILL_FOOTJOB))
    {
        girl.footjob(skill);
        ss << "You decide to teach her the art of manual stimulation with her feet.\n \n";
        ss << "She managed to gain " << skill << " Foot Job.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_FOOT, Day0Night1);
    }
    else if (roll_b <= 50 && girl.oralsex() < 100 && brothel->is_sex_type_allowed(SKILL_ORALSEX))
    {
        girl.oralsex(skill);
        ss << "You decide to teach her the art of sucking a cock.\n \n";
        ss << "She managed to gain " << skill << " Oral.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
    }
    else if (roll_b <= 70 && girl.normalsex() < 100 && brothel->is_sex_type_allowed(SKILL_NORMALSEX))
    {
        girl.normalsex(skill);
        ss << "You decide to teach her how to ride a dick like a pro.\n \n";
        ss << "She managed to gain " << skill << " Normal Sex.\n \n";
        if (girl.lose_trait("Virgin"))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (roll_b <= 80 && girl.anal() < 100 && brothel->is_sex_type_allowed(SKILL_ANAL))
    {
        girl.anal(skill);
        ss << "You decide to teach her how to use her ass.\n \n";
        ss << "She managed to gain " << skill << " Anal Sex.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
    }
    else if (roll_b <= 90 && girl.group() < 100 && brothel->is_sex_type_allowed(SKILL_GROUP))
    {
        girl.group(skill);
        ss << "You decide to over see her skill in a gang bang.\n \n";
        ss << "She managed to gain " << skill << " Group Sex.\n \n";
        if (girl.lose_trait("Virgin"))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), IMGTYPE_GROUP, Day0Night1);
        // TODO chance to get pregnant by non-player!
        // TODO if we remove the virgin trait before the pregnancy calculation, it cannot affect preg chance!
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (roll_b <= 100 && girl.bdsm() < 100 && brothel->is_sex_type_allowed(SKILL_BDSM))
    {
        girl.bdsm(skill);
        ss << "You decide to teach her the fine art of BDSM.\n \n";
        ss << "She managed to gain " << skill << " BDSM.\n \n";
        if (girl.lose_trait("Virgin"))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);
        girl.calc_pregnancy(&g_Game->player(), 1.0);
    }
    else if (girl.beastiality() < 100 && brothel->is_sex_type_allowed(SKILL_BEASTIALITY))
    {
        girl.beastiality(skill);
        ss << "You decide to have her get acquainted with some animals.\n \n";
        ss << "She managed to gain " << skill << " Beastiality.\n \n";
        if (girl.lose_trait("Virgin"))
        {
            ss << "She is no longer a virgin.\n";
        }
        girl.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1);
    }
    else
    {
        girl.strip((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_HANDJOB))        girl.handjob((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_ORALSEX))        girl.oralsex((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_TITTYSEX))        girl.tittysex((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_LESBIAN))        girl.lesbian((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_NORMALSEX))        girl.normalsex((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_ANAL))            girl.anal((rng % 3));
        if (brothel->is_sex_type_allowed(SKILL_BDSM))            girl.bdsm((rng % 3));
        ss << "You couldn't decide what to teach her so you just fooled around with her.\n \n";
        girl.AddMessage(ss.str(), IMGTYPE_ECCHI, Day0Night1);
    }

    if (girl.is_slave())
    {
        ss << "\nYou own her so you don't have to pay her.";
    }
    else
    {
        girl.m_Pay += 50;
    }
#endif





    // Improve stats


    girl.exp(xp);

    if (PT_Fear != 0) girl.pcfear(PT_Fear);
    if (PT_Love != 0) girl.pclove(PT_Love);
    if (PT_Hate != 0) girl.pchate(PT_Hate);





    return false;
}

double JP_PersonalTraining(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance += (100 - cGirls::GetAverageOfSexSkills(girl)) * 3;
    }
    else// for the actual check
    {
    }
    return jobperformance;
}
