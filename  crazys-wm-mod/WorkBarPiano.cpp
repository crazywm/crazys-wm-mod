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
#pragma region //	Includes and Externs			//
#include "cJobManager.h"
#include "cBrothel.h"
#include "cRng.h"
#include "sConfig.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern cBrothelManager g_Brothels;
extern cMessageQue g_MessageQue;

#pragma endregion

// `J` Job Brothel - Bar
bool cJobManager::WorkBarPiano(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMUSIC;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to play piano in your bar " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (brothel->m_TotalCustomers < 1)
	{
		ss.str("");
		ss << "There were no customers in the bar on the " << (Day0Night1 ? "night" : "day") << " shift so " << girlName << " just cleaned up a bit.";
		brothel->m_Filthiness -= 20 + girl->service() * 2;
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	ss << " played the piano in the bar.";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	sGirl* singeronduty = g_Brothels.GetRandomGirlOnJob(brothel->m_id, JOB_SINGER, Day0Night1);
	string singername = (singeronduty ? "Singer " + singeronduty->m_Realname + "" : "the Singer");

	int wages = 20, tips = 0;
	int enjoy = 0, fame = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_BarPiano(girl, false);
	tips = (int)((jobperformance / 8.0) * ((g_Dice % (girl->beauty() + girl->charisma()) / 20.0) + (girl->performance() / 5.0)));

	if (jobperformance >= 245)
	{
		ss << " She plays with the grace of an angel. Customers come from miles around to listen to her play.\n";
		roll_b += g_Dice % 23 + 8;						// +8 to +30 enjoy check
		if (roll_a <= 20)
		{
			ss << girlName << "'s playing brought many patrons to tears as she played a song full of sadness.";
			brothel->m_Happiness += g_Dice % 6 + 5;		// +5 to +10
		}
		else if (roll_a <= 40)
		{
			ss << "Nice melody fills the room when " << girlName << " is behind the piano.";
			brothel->m_Happiness += g_Dice % 11 + 5;	// +5 to +15
		}
		else if (roll_a <= 60)
		{
			ss << "Knowing that she is good, " << girlName << " played all the tunes blindfolded.";
			brothel->m_Fame += g_Dice % 10 + 1;			// +1 to +10
		}
		else if (roll_a <= 80)
		{
			ss << "Being confident in her skill, " << girlName << " played today using only one hand.";
			brothel->m_Fame += g_Dice % 10 + 1;			// +1 to +10
		}
		else
		{
			ss << girlName << "'s soothing playing seems to glide over the noise and bustling of the bar.";
			brothel->m_Happiness += g_Dice % 11 + 5;	// +5 to +15
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her playing skills.\n";
		roll_b += g_Dice % 22 + 4;						// +4 to +25 enjoy check
		if (roll_a <= 20)
		{
			ss << girlName << " began to acquire her own following - a small crowd of people came in just to listen to her and buy drinks";
			brothel->m_Fame += g_Dice % 5 + 1;			// +1 to +5
			tips += g_Dice % 101 + 10;					// +10 to +110
		}
		else if (roll_a <= 40)
		{
			ss << "Her playing fills the room. Some customers hum the melody under their noses.";
			brothel->m_Happiness += g_Dice % 8 + 3;		// +3 to +10
		}
		else if (roll_a <= 60)
		{
			ss << "After making a mistake she improvised a passage to the next song.";
		}
		else if (roll_a <= 80)
		{
			ss << "She plays without music sheets having all the songs memorized.";
			brothel->m_Fame += g_Dice % 5 + 1;			// +1 to +5
		}
		else
		{
			ss << girlName << "'s soothing playing seems to glide over the noise and bustling of the bar.";
			brothel->m_Happiness += g_Dice % 8 + 3;		// +3 to +10
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " Her playing is really good and gets praised by the customers often.\n";
		roll_b += g_Dice % 17 - 1;						//	-1 to +15 enjoy check
		if (roll_a <= 20)
		{
			ss << "Her playing was pleasing, if bland.  Her rythem was nice, if slightly untrained.";
		}
		else if (roll_a <= 40)
		{
			ss << girlName << " doesn't have any trouble playing the piano.";
		}
		else if (roll_a <= 60)
		{
			ss << "Give " << girlName << " any kind of music sheet and she will play it. She is really good at this.";
			brothel->m_Happiness += g_Dice % 5 + 1;		// +1 to +5
		}
		else if (roll_a <= 80)
		{
			ss << "When asked to play one of the more complicated tunes she gave her all.";
			brothel->m_Happiness += g_Dice % 7 - 1;		// -1 to +5
		}
		else
		{
			ss << "The slow song " << girlName << " played at the end of shift really had her full emotion and heart.";
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She hits a few right notes but she still has room to improve.\n";
		roll_b += g_Dice % 14 - 3;						// -3 to +10 enjoy check
		if (roll_a <= 20)
		{
			ss << "While she won't win any contests, " << girlName << " isn't a terrible pianist.";
		}
		else if (roll_a <= 40)
		{
			ss << girlName << "'s performance today was good. She is a promising pianist.";
		}
		else if (roll_a <= 60)
		{
			ss << "She gets the key order right most of the time.";
		}
		else if (roll_a <= 80)
		{
			ss << "You could tell that there was something like a melody, but " << girlName << " still needs a lot of practice.";
		}
		else
		{
			ss << "The slow song " << girlName << " played at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She almost never hits a right note. Lucky for you most of the customers are too drunk and horny to care.\n";
		roll_b += g_Dice % 16 - 10;						// -10 to +5 enjoy check
		if (roll_a <= 20)
		{
			ss << "Her playing is barely acceptable, but fortunately the bustling of the bar drowns " << girlName << " out for the most part.";
		}
		else if (roll_a <= 40)
		{
			ss << "She is terrible at this. Some customers left after she started to play.";
			brothel->m_Happiness -= g_Dice % 5 + 1;
		}
		else if (roll_a <= 60)
		{
			ss << "You could count on the fingers of one hand the part in her performance that was clean.";
			brothel->m_Happiness -= g_Dice % 5 + 1;
		}
		else if (roll_a <= 80)
		{
			ss << "She is bad at playing the piano.";
		}
		else
		{
			ss << girlName << " knows a note.  Too bad it's the only one she knows and plays it over and over.";
		}
	}
	else
	{
		ss << " She didn't play the piano so much as banged on it.\n";
		roll_b += g_Dice % 19 - 15;						// -15 to +3 enjoy check
		if (roll_a <= 20)
		{
			ss << "Her audience seems paralyzed, as if they couldn't believe that a piano was capable of making such noise.";
			brothel->m_Happiness -= g_Dice % 8 + 3;		// -3 to -10
			brothel->m_Fame -= g_Dice % 4;			// 0 to -3
		}
		else if (roll_a <= 40)
		{
			ss << "After ten seconds you wanted to grab an axe and end the instrument's misery under " << girlName << "'s attempt to play.";
			brothel->m_Happiness -= g_Dice % 8 + 3;		// -3 to -10
		}
		else if (roll_a <= 60)
		{
			ss << "Noone else would call this random key-mashing 'playing', but " << girlName << " thinks otherwise.";
			brothel->m_Happiness -= g_Dice % 8 + 3;		// -3 to -10
		}
		else if (roll_a <= 80)
		{
			ss << "When " << girlName << " started to play, the bar emptied almost instantly. This could be useful in a fire.";
			brothel->m_Fame -= g_Dice % 5 + 1;			// -1 to -5
		}
		else
		{
			ss << girlName << " banged on the piano clearly having no clue which note was which.";
			brothel->m_Happiness -= g_Dice % 8 + 3;		// -3 to -10
		}
		//SIN - bit of randomness.
		if (g_Dice.percent(brothel->m_Filthiness / 50))
		{
			ss << "Soon after she started her set, some rats jumped out of the piano and fled the building. Patrons could be heard laughing.";
			brothel->m_Fame -= g_Dice % 2;			// 0 to -1
		}
	}
	ss << "\n \n";


	//try and add randomness here
	if (girl->beauty() > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n";
		tips += 15 + g_Dice % 26;					// +15 to +40
	}

	if (girl->has_trait( "Clumsy") && g_Dice.percent(5))
	{
		ss << "Her clumsy nature caused her to close the lid on her fingers making her have to stop playing for a few hours.\n";
		wages -= 10;
		tips /= 2;
	}

	if (girl->has_trait( "Pessimist") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n";
			tips = int(tips * 0.9);
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
			tips = int(tips * 1.1);
		}
	}
	else if (girl->has_trait( "Optimist") && g_Dice.percent(10))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her on her playing.\n";
			tips = int(tips *0.9);
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			tips = int(tips * 1.1);
		}
	}

	if (girl->has_trait( "Psychic") && g_Dice.percent(20))
	{
		ss << "She used her Psychic skills to know exactly what the patrons wanted to hear her play.\n";
		tips = int(tips * 1.1);
	}

	if (girl->has_trait( "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			ss << "A patron bumped into the piano causing her to miss a note. This pissed her off and using her Assassin skills she killed him before even thinking about it, resulting in patrons fleeing the building.\n";
			wages = 0;
			tips = int(tips * 0.5);
		}
		else
		{
			ss << "A patron bumped into the piano, but with her skill she didn't miss a note. The patron was lucky to leave with his life.\n";
		}
	}

	if (girl->has_trait( "Horrific Scars") && g_Dice.percent(15))
	{
		if (jobperformance < 150)
		{
			ss << "A patron gasped at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
		}
		else
		{
			ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she played so well, they left a good tip.\n";
			tips = int(tips * 1.1);
		}
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_SINGER, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " played poorly with " << singername << " making people leave.\n";
			tips = int(tips * 0.8);
		}
		else
		{
			ss << girlName << " played well with " << singername << " increasing tips.\n";
			tips = int(tips * 1.1);
		}
	}

#pragma endregion
#pragma region	//	Enjoyment				//

	//enjoyed the work or not
	if (roll_b <= 10)
	{
		ss << "Some of the patrons abused her during the shift.";
		enjoy -= g_Dice % 3 + 1;
		tips = int(tips * 0.9);
	}
	else if (roll_b >= 90)
	{
		ss << "She had a pleasant time working.";
		enjoy += g_Dice % 3 + 1;
		tips = int(tips * 1.1);
	}
	else
	{
		ss << "Otherwise, the shift passed uneventfully.";
		enjoy += g_Dice % 2;
	}

#pragma endregion
#pragma region	//	Money					//

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		wages = 0;
		tips = int(tips * 0.9);
	}
	else
	{
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	int I_xp = (g_Dice % xp) + 1;							girl->exp(I_xp);
	int I_libido = (g_Dice % libido) + 1;					girl->upd_temp_stat(STAT_LIBIDO, I_libido);

	// primary improvement (+2 for single or +1 for multiple)
	int I_performance = (g_Dice % skill) + 2;			 	girl->performance(I_performance);
	// secondary improvement (-1 for one then -2 for others)
	int I_confidence = max(0, (g_Dice % skill) - 1);		girl->confidence(I_confidence);
	int I_intelligence = max(0, (g_Dice % skill) - 2);		girl->intelligence(I_intelligence);
	int I_fame = fame;										girl->fame(I_fame);

	// Update Enjoyment
	if (jobperformance < 50) enjoy -= 1; if (jobperformance < 0) enjoy -= 1;	// if she doesn't do well at the job, she enjoys it less
	if (jobperformance > 200) enjoy *= 2;		// if she is really good at the job, her enjoyment (positive or negative) is doubled
	girl->upd_Enjoyment(actiontype, enjoy);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Elegant", 75, ACTION_WORKMUSIC, "Playing the piano has given " + girlName + " an Elegant nature.", Day0Night1);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKMUSIC, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);

	if (cfg.debug.log_show_numbers())
	{
		ss << "\n \nNumbers:"
			<< "\n Job Performance = " << (int)jobperformance
			<< "\n Wages = " << (int)wages
			<< "\n Tips = " << (int)tips
			<< "\n Xp = " << I_xp
			<< "\n Libido = " << I_libido
			<< "\n Fame = " << I_fame
			<< "\n Confidence = " << I_confidence
			<< "\n Intelligence = " << I_intelligence
			<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy
			;
	}

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}
double cJobManager::JP_BarPiano(sGirl* girl, bool estimate)// Used in cJobManager::WorkBarPiano
{
	double jobperformance =
		// primary - first 100
		girl->performance() +
		// secondary - second 100
		((girl->intelligence() + girl->confidence() + girl->agility()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Elegant"))				jobperformance += 15;	// Elegant people usally know how to play the piano lol
	if (girl->has_trait( "Psychic"))				jobperformance += 10;	// knows what people want to hear
	if (girl->has_trait( "Singer"))				jobperformance += 10;	// Would understand rythem
	if (girl->has_trait( "Charismatic"))			jobperformance += 10;
	if (girl->has_trait( "Princess"))				jobperformance += 10;	// usally taught to play
	if (girl->has_trait( "Queen"))				jobperformance += 10;	// usally taught to play
	if (girl->has_trait( "Noble"))				jobperformance += 10;	// usally taught to play
	if (girl->has_trait( "Idol"))					jobperformance += 10;
	if (girl->has_trait( "Sexy Air"))				jobperformance += 5;
	if (girl->has_trait( "Cool Person"))			jobperformance += 5;	// people love to be around her
	if (girl->has_trait( "Cute"))					jobperformance += 5;
	if (girl->has_trait( "Charming"))				jobperformance += 5;	// people like charming people
	if (girl->has_trait( "Quick Learner"))		jobperformance += 5;	// people like charming people
	if (girl->has_trait( "Long Legs"))			jobperformance += 5;
	if (girl->has_trait( "Nerd"))					jobperformance += 5;
	if (girl->has_trait( "Playful Tail"))			jobperformance += 5;	// use it to help her play lol

	//bad traits
	if (girl->has_trait( "Tone Deaf"))			jobperformance -= 75;	// should never get good at this job
	if (girl->has_trait( "Deaf"))					jobperformance -= 30;
	if (girl->has_trait( "Retarded"))				jobperformance -= 30;
	if (girl->has_trait( "Broken Will"))			jobperformance -= 30;
	if (girl->has_trait( "Nervous"))				jobperformance -= 30;	// don't like to be around people
	if (girl->has_trait( "Dependant"))			jobperformance -= 30;	// needs others to do the job
	if (girl->has_trait( "Aggressive"))			jobperformance -= 20;	// gets mad easy and may attack people
	if (girl->has_trait( "Meek"))					jobperformance -= 20;
	if (girl->has_trait( "Slow Learner"))			jobperformance -= 10;

	if (girl->has_trait( "No Arms"))				jobperformance -= 200;
	if (girl->has_trait( "No Hands"))				jobperformance -= 190;
	if (girl->has_trait( "One Arm"))				jobperformance -= 100;
	if (girl->has_trait( "One Hand"))				jobperformance -= 90;
	if (girl->has_trait( "Missing Fingers"))		jobperformance -= 50;
	if (girl->has_trait( "Missing Finger"))		jobperformance -= 20;
	if (girl->has_trait( "No Legs"))				jobperformance -= 20;
	if (girl->has_trait( "No Feet"))				jobperformance -= 10;
	if (girl->has_trait( "Blind"))				jobperformance -= 5;

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Smoker"))				jobperformance -= 5;	// would need smoke breaks
	if (girl->has_trait( "Cum Addict"))			jobperformance -= 5;

	return jobperformance;
}
