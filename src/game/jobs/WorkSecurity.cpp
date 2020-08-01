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
#include "cRng.h"
#include "buildings/cBuildingManager.h"
#include <sstream>
#include "cJobManager.h"


// `J` Job Brothel - General
bool WorkSecurity(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKSECURITY;
    stringstream ss;
    if (girl.disobey_check(actiontype, JOB_SECURITY))
    {
        ss << "${name} refused to work security in your brothel " << (Day0Night1 ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked Security in the brothel.\n \n";




    int enjoy = 0;
    int wages = 0;
    int tips = 0;
    int roll_a = rng.d100();
    int imagetype = IMGTYPE_COMBAT;

    cGirls::EquipCombat(girl);    // ready armor and weapons!

    const sGirl* stripperonduty = random_girl_on_job(*brothel, JOB_BARSTRIPPER, Day0Night1);
    string strippername = (stripperonduty ? "Stripper " + stripperonduty->FullName() + "" : "the Stripper");

    const sGirl* whoreonduty = random_girl_on_job(*brothel, JOB_WHOREBROTHEL, Day0Night1);
    string whorename = (whoreonduty ? "Whore " + whoreonduty->FullName() + "" : "the Whore");


    double SecLev = girl.job_performance(JOB_SECURITY, false);


    // Complications
    if (roll_a <= 25)
    {
        switch(rng%5)
        {
        case 2: //'Mute' Unrulely Customers rape her
        {
            enjoy-=rng%3+1;
            SecLev-=SecLev/10;
            ss<< "She tried to Fight off some unruly patrons, but they turned on her and raped her.";
            int custCount=rng%4+1;
            cJobManager::customer_rape(girl, custCount);
            break;
        }
        case 3:
        {
            enjoy -= rng % 3 + 1;
            double secLvlMod = SecLev / 10.0;
            ss << "She stumbled across some patrons trying to rape a female customer.\n";
            int combatMod = (girl.combat() + girl.magic() + girl.agility()) / 3;
            if (rng.percent(combatMod))
            {
                ss << "She succeeded in saving the girl from being raped."; //'Mute" TODO add posiblity of adding female customers to dungeon
                SecLev += secLvlMod;
            }
            else
            {
                SecLev -= secLvlMod;
                int rapers = rng % 4 + 1;
                ss << "She failed in saving her. They where both raped by " << rapers << " men.\n";
                cJobManager::customer_rape(girl, rapers);
            }
            break;
        }
        default:
        {
            enjoy -= rng % 3 + 1;
            SecLev -= SecLev / 10;
            ss << "She had to deal with some very unruly patrons that gave her a hard time.";
            break;
        }
        }

    }
    else if (roll_a >= 75)
    {
        enjoy += rng % 3 + 1;
        SecLev += SecLev / 10;
        ss << "She successfully handled unruly patrons.";
    }
    else
    {
        enjoy += rng % 2;
        ss << "She had an uneventful day watching over the brothel.";
    }
    ss << "\n \n";

    if (girl.libido() >= 70 && rng.percent(20))
    {
        int choice = rng % 2;
        ss << "Her libido caused her to get distracted while watching ";
        /*might could do more with this FIXME CRAZY*/
        if (girl.has_active_trait("Lesbian")) choice = 0;
        if (girl.has_active_trait("Straight")) choice = 1;
        switch (choice)
        {
        case 0:
            ss << (stripperonduty ? strippername : "one of the strippers") << " dance.\n";
            break;
        case 1:
        default:
            ss << (whoreonduty ? whorename : "one of the whores") << " with a client.\n";
            break;
        }
        SecLev -= 20;
        ss << "\n \n";

    }

    if ((girl.libido() > 50 && rng.percent(girl.libido() / 5)) || (girl.has_active_trait("Nymphomaniac") && rng.percent(20)))
    {
        ss <<"\nGave some bonus service to the well behaved patrons, ";
        int l = 0;
        switch (rng % 4)        // `J` just roll for the 4 sex options and flash only if sex is restricted
        {
        case 1:    if (brothel->is_sex_type_allowed(SKILL_ORALSEX))    { l = 10;    imagetype = IMGTYPE_ORAL;    ss << "She sucked them off";    break; }
        case 2:    if (brothel->is_sex_type_allowed(SKILL_TITTYSEX))    { l = 7;    imagetype = IMGTYPE_TITTY;    ss << "She used her tits to get them off";    break; }
        case 3:    if (brothel->is_sex_type_allowed(SKILL_HANDJOB))    { l = 6;    imagetype = IMGTYPE_HAND;    ss << "She jerked them off";    break; }
        case 4:    if (brothel->is_sex_type_allowed(SKILL_FOOTJOB))    { l = 4;    imagetype = IMGTYPE_FOOT;    ss << "She used her feet to get them off";    break; }
        default:/*                         */    { l = 2;    imagetype = IMGTYPE_STRIP;    ss << "She flashed them";    break; }
        }
        ss << ".\n \n";
        girl.upd_temp_stat(STAT_LIBIDO, -l, true);
    }

    if (SecLev < 10) SecLev = 10;
    brothel->m_SecurityLevel += int(SecLev);

    ss << "\nPatrolling the building, ${name} increased the security level by " << int(SecLev) << ".";
    girl.AddMessage(ss.str(), imagetype, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve girl
    int xp = 15, skill = 2;

    /* */if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 5; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 5; }

    wages += 70;
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    //g_Game->gold().staff_wages(70);  // wages come from
    // 'Mute' Updated
    girl.exp(xp);
    girl.combat((rng%skill)+1);
    girl.magic((rng%skill)+1);
    girl.agility((rng%skill)+1);

    girl.upd_Enjoyment(actiontype, enjoy);

    // Copy-pasta from WorkExploreCatacombs
    cGirls::PossiblyGainNewTrait(girl, "Tough", 15, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Adventurer", 45, actiontype, "She has been in enough tough spots to consider herself Adventurer.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
    return false;
}

double JP_Security(const sGirl& girl, bool estimate)    // Used inside the WorkSecurity
{
    /*    MYR: Modified security level calculation & added traits for it
    *    A gang of 1-10 customers attack girls now in function work_related_violence.
    *    It takes (# members x 5) security points to intercept them.
    *    See work_related_violence for details.
    */
    int SecLev = 0;
    // 'Mute' Updated
    if (estimate)    // for third detail string
    {

        SecLev = (girl.combat())
            + (girl.magic() / 2)
            + (girl.agility() / 2);
    }
    else            // for the actual check
    {

        SecLev = g_Dice % (girl.combat() / 2)
        /*  */ + g_Dice % (girl.magic() / 4)
        /*  */ + g_Dice % (girl.agility() / 4);
    }

    // Good traits
    if (girl.has_active_trait("Incorporeal")) SecLev += 100;    // I'm fucking Superman!
    if (girl.has_active_trait("Assassin")) SecLev += 50;    // I was born for this job. I know how you think. Is 50 too high? Assassin is a relatively rare trait, and there's no way to gain it (That I'm aware of) so it trades off(?)
    if (girl.has_active_trait("Zombie")) SecLev += 20;    // You have what guarding the building?!?! I'm out of here.
    if (girl.has_active_trait("Skeleton")) SecLev += 10;    // You have what guarding the building?!?! I'm out of here.
    if (girl.has_active_trait("Tough")) SecLev += 15;    // You hit like a girl
    if (girl.has_active_trait("Powerful Magic")) SecLev += 10;    //
    if (girl.has_active_trait("Psychic")) SecLev += 10;    // I sense danger
    if (girl.has_active_trait("Demon")) SecLev += 10;    // Even scarier
    if (girl.has_active_trait("Brawler")) SecLev += 10;    //
    if (girl.has_active_trait("Strong")) SecLev += 10;    //
    if (girl.has_active_trait("Fleet of Foot")) SecLev += 5;    // Moves around quickly
    if (girl.has_active_trait("Charming")) SecLev += 5;    // Gets more cooperation
    if (girl.has_active_trait("Cool Person")) SecLev += 5;    // Gets more cooperation redux
    if (girl.has_active_trait("Adventurer")) SecLev += 5;    // Has experience
    if (girl.has_active_trait("Aggressive")) SecLev += 5;    // Rawr! I kill you now!
    if (girl.has_active_trait("Yandere")) SecLev += 5;    // Rawr! I kill you now!
    if (girl.has_active_trait("Tsundere")) SecLev += 5;    // Rawr! I kill you now!
    if (girl.has_active_trait("Sadistic")) SecLev += 5;    // I kill you slowly with this dull knife!
    if (girl.has_active_trait("Merciless")) SecLev += 5;    // Your cries for clemency amuse me
    if (girl.has_active_trait("Fearless")) SecLev += 5;    // Chhhhaaaarrrrrggggeeeeee!!
    if (girl.has_active_trait("Iron Will")) SecLev += 5;    // Hold the line!
    if (girl.has_active_trait("Construct")) SecLev += 5;    // Scary
    if (girl.has_active_trait("Not Human")) SecLev += 5;    // Scary
    if (girl.has_active_trait("Agile")) SecLev += 5;    //
    if (girl.has_active_trait("Dominatrix")) SecLev += 5;    // likes to be in charge
    if (girl.has_active_trait("Giant")) SecLev += 5;    // Scary
    if (girl.has_active_trait("Goddess")) SecLev += 5;    // people might behave better around a goddess
    if (girl.has_active_trait("Heroine")) SecLev += 5;    // likes to protect others
    if (girl.has_active_trait("Muscular")) SecLev += 5;    // hit harder
    if (girl.has_active_trait("Strong Magic")) SecLev += 5;    //
    if (girl.has_active_trait("Strange Eyes")) SecLev += 2;    // I'm watching you


    // Bad traits
    if (girl.has_active_trait("Blind")) SecLev -= 50;    // can't see what people are doing
    if (girl.has_active_trait("No Arms")) SecLev -= 40;    // catch me if you... use your feet?
    if (girl.has_active_trait("No Legs")) SecLev -= 50;    // run? whats that?
    if (girl.has_active_trait("Deaf")) SecLev -= 25;    // can't hear girls scream for help
    if (girl.has_active_trait("No Hands")) SecLev -= 20;    //
    if (girl.has_active_trait("No Feet")) SecLev -= 20;    //
    if (girl.has_active_trait("One Arm")) SecLev -= 10;    //
    if (girl.has_active_trait("One Foot")) SecLev -= 10;    //
    if (girl.has_active_trait("One Leg")) SecLev -= 10;    //
    if (girl.has_active_trait("One Hand")) SecLev -= 5;    //
    if (girl.has_active_trait("One Eye")) SecLev -= 5;    //

    if (girl.has_active_trait("Broken Will")) SecLev -= 50;    // I'm too tired to patrol
    if (girl.has_active_trait("Mind Fucked")) SecLev -= 50;    // duurrrrrr..... secu.... sec... what? (Mind fucked can be cured btw.)
    if (girl.has_active_trait("Alcoholic")) SecLev -= 30;    //
    if (girl.has_active_trait("Cum Addict")) SecLev -= 30;    // Be looking for cum instead of doing job
    if (girl.has_active_trait("Retarded")) SecLev -= 20;    // duurrrrrr..... secu.... sec... what?
    if (girl.has_active_trait("Nymphomaniac")) SecLev -= 20;    // Wait! The security officer is a nymphomaniac in a brothel?
    if (girl.has_active_trait("Shroud Addict")) SecLev -= 15;    // high at work never good
    if (girl.has_active_trait("Fairy Dust Addict")) SecLev -= 15;    // high at work never good
    if (girl.has_active_trait("Viras Blood Addict")) SecLev -= 15;    // high at work never good
    if (girl.has_active_trait("Cum Addict")) SecLev -= 5;
    if (girl.has_active_trait("Smoker")) SecLev -= 10;    // less lung power = less running
    if (girl.has_active_trait("Dependant")) SecLev -= 10;    // I can't do this alone
    if (girl.has_active_trait("Weak Magic")) SecLev -= 5;    //
    if (girl.has_active_trait("Lolita")) SecLev -= 5;    // Hi there kiddo.  Lost your mommy?
    if (girl.has_active_trait("Nerd")) SecLev -= 5;    // Gets no respect
    if (girl.has_active_trait("Bimbo")) SecLev -= 5;    // Gets no respect
    if (girl.has_active_trait("Nervous")) SecLev -= 5;    // Gets no respect
    if (girl.has_active_trait("Twisted")) SecLev -= 5;    // Wierd ideas about security rarely work
    if (girl.has_active_trait("Meek")) SecLev -= 5;    // Wait... bad person... come back
    if (girl.has_active_trait("Clumsy")) SecLev -= 5;    // "Stop thief!" ..... "Ahhhhh! I fell again!"
    if (girl.has_active_trait("Delicate")) SecLev -= 5;    // Awww, I broke a nail :(
    if (girl.has_active_trait("Old")) SecLev -= 5;    // Gets no respect
    if (girl.has_active_trait("Plump")) SecLev -= 5;    // Chubby... chaser?

    if (!estimate)
    {
        int t = girl.tiredness() - 70;
        if (t > 0) SecLev -= t * 2;

        int h = girl.health();
        if (h < 10) SecLev -= (20 - h) * 5;
        else if (h < 20) SecLev -= (20 - h) * 2;
        else if (h < 30) SecLev -= 30 - h;

        int y = girl.happiness();
        if (y < 20) SecLev -= 20 - y;

        if (SecLev < 0)    SecLev = 0;
    }


    return SecLev;
}
