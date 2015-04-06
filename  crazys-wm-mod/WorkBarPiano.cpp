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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Brothel - Bar
bool cJobManager::WorkBarPiano(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMUSIC;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " played the piano in the bar.";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	sGirl* singeronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_SINGER, Day0Night1);
	string singername = (singeronduty ? "Singer " + singeronduty->m_Realname + "" : "the Singer");

	int wages = 20, work = 0;

	int roll = g_Dice.d100();

	double jobperformance = JP_BarPiano(girl, false);

	if (jobperformance >= 245)
	{
		ss << " She plays with the grace of an angel. Customers come from miles around to listen to her play.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			ss << girlName << "'s playing brought many patrons to tears as she played a song full of sadness.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll <= 40)
		{
			ss << "Nice melody fills the room when " << girlName << " is behind the piano.\n";
			brothel->m_Happiness += 10;
		}
		else if (roll <= 60)
		{
			ss << "Knowing that she is good, " << girlName << " played all the tunes blindfolded.\n";
			brothel->m_Fame += 10;
		}
		else if (roll <= 80)
		{
			ss << "Being confident in her skill, " << girlName << " played today using only one hand.\n";
			brothel->m_Fame += 10;
		}
		else
		{
			ss << girlName << "'s soothing playing seems to glide over the noise and bustling of the bar.\n";
			brothel->m_Happiness += 10;
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her playing skills.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			ss << girlName << " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
			brothel->m_Fame += 5;
			wages += 10;
		}
		else if (roll <= 40)
		{
			ss << "Her playing fills the room. Some customers hum the melody under their noses.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll <= 60)
		{
			ss << "After making a mistake she improvised a passage to the next song.\n";
		}
		else if (roll <= 80)
		{
			ss << "She plays without music sheets having all the songs memorized.\n";
			brothel->m_Fame += 5;
		}
		else
		{
			ss << girlName << "'s soothing playing seems to glide over the noise and bustling of the bar.\n";
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " Her playing is really good and gets praised by the customers often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << "Her playing was pleasing, if bland.  Her rythem was nice, if slightly untrained.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName << " doesn't have any trouble playing the piano.\n";
		}
		else if (roll <= 60)
		{
			ss << "Give " << girlName << " any kind of music sheet and she will play it. She is really good at this.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll <= 80)
		{
			ss << "When asked to play one of the more complicated tunes she gave her all.\n";
		}
		else
		{
			ss << "The slow song " << girlName << " played at the end of shift really had her full emotion and heart.\n";
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She hits a few right notes but she still has room to improve.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			ss << "While she won't win any contests, " << girlName << " isn't a terrible pianist.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName << "'s performance today was good. She is a promising pianist.\n";
		}
		else if (roll <= 60)
		{
			ss << "She gets the key order right most of the time.\n";
		}
		else if (roll <= 80)
		{
			ss << "You could tell that there was something like a melody, but " << girlName << " still needs a lot of practice.\n";
		}
		else
		{
			ss << "The slow song " << girlName << " played at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She almost never hits a right note. Lucky for you most of the customers are too drunk and horny to care.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			ss << "Her playing is barely acceptable, but fortunately the bustling of the bar drowns " << girlName << " out for the most part.\n";
		}
		else if (roll <= 40)
		{
			ss << "She is terrible at this. Some customers left after she started to play.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll <= 60)
		{
			ss << "You could count on the fingers of one hand the part in her play that was clean.\n";
		}
		else if (roll <= 80)
		{
			ss << "She is bad at playing the piano.\n";
		}
		else
		{
			ss << girlName << " knows a note.  To bad its the only one she knows and plays it over and over.\n";
		}
	}
	else
	{
		ss << " She didn't play the piano so much as banged on it.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			ss << "Her audience seems paralyzed, as if they couldn't believe that a piano was capable of making such noise.\n";
			brothel->m_Happiness -= 10;
		}
		else if (roll <= 40)
		{
			ss << "After ten seconds you wanted to grab an axe and end the instrument's misery under " << girlName << "'s attempt to play.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll <= 60)
		{
			ss << "Noone else would call this random key-mashing 'playing', but " << girlName << " thinks otherwise.\n";
		}
		else if (roll <= 80)
		{
			ss << "When " << girlName << " started to play, the bar emptied almost instantly. This could be useful in a fire.\n";
		}
		else
		{
			ss << girlName << " banged on the piano clearly having no clue which note was which.\n";
		}
		//SIN - bit of randomness.
		if (g_Dice.percent(brothel->m_Filthiness / 50)) ss << "Soon after she started her set, some rats jumped out of the piano and fled the building. Patrons could be heard laughing.\n\n";
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 15;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(5))
	{
		ss << "Her clumsy nature caused her to close the lid on her fingers making her have to stop playing for a few hours.\n\n"; wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her on her playing.\n\n"; wages -= 10;
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Psychic") && g_Dice.percent(20))
	{
		ss << "She used her Psychic skills to know exactly what the patrons wanted to hear her play.\n"; wages += 15;
	}

	if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			ss << "A patron bumped into the piano causing her to miss a note. This pissed her off and using her Assassin skills she killed him before even thinking about it, resulting in patrons fleeing the building.\n"; wages -= 50;
		}
		else
		{
			ss << "A patron bumped into the piano, but with her skill she didn't miss a note. The patron was lucky to leave with his life.\n";
		}
	}

	if (g_Girls.HasTrait(girl, "Horrific Scars") && g_Dice.percent(15))
	{
		if (jobperformance < 150)
		{
			ss << "A patron gasped at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
		}
		else
		{
			ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she played so well, they left a good tip.\n"; wages += 15;
		}
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_SINGER, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " played poorly with " << singername << " making people leave.\n\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " played well with " << singername << " increasing tips.\n\n"; wages += 40;
		}
	}


	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMUSIC, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill);
	else
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Elegant", 75, ACTION_WORKMUSIC, "Playing the piano has given " + girlName + " an Elegant nature.", Day0Night1);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKMUSIC, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);

	return false;
}
double cJobManager::JP_BarPiano(sGirl* girl, bool estimate)// not used
{
	double jobperformance = (g_Girls.GetStat(girl, STAT_CONFIDENCE) / 2 +
		g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))    jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))    jobperformance += 5;   //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))			  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		  jobperformance += 5;   //people like charming people
	if (g_Girls.HasTrait(girl, "Elegant"))		  jobperformance += 15;  //Elegant people usally know how to play the piano lol
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;   //people like charming people
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;  //knows what people want to hear
	if (g_Girls.HasTrait(girl, "Idol"))			  jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Long Legs"))	  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Nerd"))			  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Playful Tail"))	  jobperformance += 5;	// use it to help her play lol
	if (g_Girls.HasTrait(girl, "Princess"))		  jobperformance += 10;	//usally taught to play
	if (g_Girls.HasTrait(girl, "Queen"))		  jobperformance += 10;	//usally taught to play
	if (g_Girls.HasTrait(girl, "Noble"))		  jobperformance += 10;	//usally taught to play
	if (g_Girls.HasTrait(girl, "Singer"))		  jobperformance += 10;	//Would understand rythem


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50;  //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30;  //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))  jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Slow Learner")) jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Tone Deaf"))	jobperformance -= 75; //should never get good at this job

	if (g_Girls.HasTrait(girl, "One Arm"))		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "One Hand"))		jobperformance -= 15; 
	if (g_Girls.HasTrait(girl, "No Arms"))		jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "No Feet"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "No Hands"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "No Legs"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Missing Finger"))jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Missing Fingers"))jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Blind"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Deaf"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 10;//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Cum Addict"))			jobperformance -= 5;



	return jobperformance;
}
