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
bool cJobManager::WorkBarSinger(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMUSIC;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a singer in the bar.\n\n";

	g_Girls.UnequipCombat(girl); // put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0, work = 0, happy = 0, fame = 0;
	int roll = g_Dice.d100(), roll_a = g_Dice.d100();
	double jobperformance = JP_BarSinger(girl, false);
	
	sGirl* pianoonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_PIANO, Day0Night1);
	string pianoname = (pianoonduty ? "Pianist " + pianoonduty->m_Realname + "" : "the Pianist");

	//dont effect things but what she sings
	if (g_Dice.percent(60) && (g_Girls.HasTrait(girl, "Country Gal") || g_Girls.HasTrait(girl, "Farmers Daughter")))
		roll_a = 60;	// country
	else if (g_Dice.percent(60) && (g_Girls.HasTrait(girl, "Elegant")))
		roll_a = 30;	// classical
	else if (g_Dice.percent(60) && (g_Girls.HasTrait(girl, "Aggressive")))
		roll_a = 20;	// death metal

	// `CRAZY` The type of music she sings
	/*default*/	int song_type = 1;    string song_type_text = "Various types of music";
	/* */if (roll_a <= 10)	{ song_type = 7; song_type_text = "Goth Rock songs"; }
	else if (roll_a <= 20)	{ song_type = 6; song_type_text = "Death Metal songs"; }
	else if (roll_a <= 30)	{ song_type = 5; song_type_text = "Classical songs"; }
	else if (roll_a <= 40)	{ song_type = 4; song_type_text = "Metal songs"; }
	else if (roll_a <= 50)	{ song_type = 3; song_type_text = "Rock songs"; }
	else if (roll_a <= 60)	{ song_type = 2; song_type_text = "Country songs"; }
	else if (roll_a >= 90)	{ song_type = 0; song_type_text = "Pop songs"; }

	// `CRAZY` How well she sings
	/*default*/	int sing_pre = 0;	string sing_pre_text = "";
	/* */if (jobperformance >= 245)	{ sing_pre = 6; sing_pre_text = " perfectly"; }
	else if (jobperformance >= 185)	{ sing_pre = 5; sing_pre_text = " great"; }
	else if (jobperformance >= 145)	{ sing_pre = 4; sing_pre_text = " good"; }
	else if (jobperformance >= 100)	{ sing_pre = 3; sing_pre_text = " decent"; }
	else if (jobperformance >= 70)	{ sing_pre = 2; sing_pre_text = " poorly"; }
	else							{ sing_pre = 1; sing_pre_text = " very poorly"; }

	ss << "She sang " << song_type_text << sing_pre_text << ".\n";


	if (jobperformance >= 245)
	{
		ss << "She must have the voice of an angel, the customers go on and on about her and always come to listen when she sings.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			ss << girlName << "'s voice brought many patrons to tears as she sang a song full of sadness.\n";
		}
		else if (roll <= 40)
		{
			ss << "Wanting to have some fun she encouraged some listeners to sing-along with her.\n";
			happy += 10;
		}
		else if (roll <= 60)
		{
			ss << "Feeling a little blue she only sang sad ballads today. You could swear that some customers were crying from the emotion.\n";
		}
		else if (roll <= 80)
		{
			ss << "The whole room was quiet when " << girlName << " was singing. After she finished, she gathered listeners applauded for minutes.\n";
			fame += 5;
		}
		else
		{
			ss << girlName << "'s soothing voice seems to glide over the noise and bustling of the bar.\n";
		}
	}
	else if (jobperformance >= 185)
	{
		ss << "She's unbelievable at this and is always getting praised by the customers for her voice.\n\n";
		wages += 95;
		if (roll <= 25)
		{
			ss << girlName << " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
			fame += 10; wages += 10;
		}
		else if (roll <= 50)
		{
			ss << "Her performance was really great, giving the listeners a pleasant time.\n";
			happy += 5;
		}
		else if (roll <= 75)
		{
			ss << "When " << girlName << " got on stage the crowd went wild. She didn't disappoint her fans giving one of the best performances in her life.\n";
			happy += 5;
		}
		else
		{
			ss << girlName << "'s soothing voice seems to glide over the noise and bustling of the bar.\n";
		}
	}
	else if (jobperformance >= 145)
	{
		ss << "Her voice is really good and gets praised by the customers often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
		}
		else if (roll <= 40)
		{
			ss << "She sang every part of the song clearly. " << girlName << " is a really good singer.\n";
		}
		else if (roll <= 60)
		{
			ss << "This wasn't the best performance of her life time, but in general she did well.\n";
		}
		else if (roll <= 80)
		{
			ss << "She is good at this. With some work she could be a star.\n";
			fame += 5;
		}
		else
		{
			ss << "The slow song " << girlName << " sang at the end of shift really had her full emotion and heart.\n";
		}
	}
	else if (jobperformance >= 100)
	{
		ss << "She hits a few right notes but she still has room to improve.\n\n";
		wages += 15;
		if (roll <= 25)
		{
			ss << "While she won't win any contests, " << girlName << " isn't a terrible singer.\n";
		}
		else if (roll <= 50)
		{
			ss << "She didn't sing every part clearly but overall she was good.\n";
		}
		else if (roll <= 75)
		{
			ss << "Maybe she isn't the best but at least she doesn't scare away the customers.\n";
		}
		else
		{
			ss << "The slow song " << girlName << " sang at the end of shift really had her full emotion and heart.  A pity she was bored and tired.\n";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << "She almost never hits a right note. Lucky for you most of your customers are drunks.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			ss << "Her singing is barely acceptable, but fortunately the bustling of the bar drowns " << girlName << " out for the most part.\n";
		}
		else if (roll <= 40)
		{
			ss << "After hearing today's performance, you order your guards to gag her for a week.\n";
		}
		else if (roll <= 60)
		{
			ss << "Some customers left immediately after " << girlName << " started to sing.\n";
			wages -= 10;
		}
		else if (roll <= 80)
		{
			ss << girlName << " singing was awful. Not a single line was sung clearly.\n";
			happy -= 5;
		}
		else
		{
			ss << girlName << "'s voice is all over the place as she sings.\n";
		}
	}
	else
	{
		ss << "Her voice sounds like nails on a chalk board.  She could be the worst singer ever.\n\n";
		wages -= 15;
		if (roll <= 14)
		{
			ss << "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
			happy -= 5;
		}
		else if (roll <= 28)
		{
			ss << "It's tragic, " << girlName << " really can't sing.\n";
		}
		else if (roll <= 42)
		{
			ss << "She is just terrible. You could swear that your singing under the shower is far better.\n";
		}
		else if (roll <= 56)
		{
			ss << girlName << " was the first to get on stage before today's star performance. Seeing the gathered crowd, she froze up being able to let out a single word.\n";
		}
		else if (roll <= 70)
		{
			ss << girlName << " singing was awful. Angry listeners throw random objects at her.\n";
			happy -= 5;
		}
		else if (roll <= 84)
		{
			ss << "Hearing " << girlName << "'s singing gave you a thought to use it as a new torture method.\n";
		}
		else
		{
			ss << girlName << " bellowed out a melody that caused the bar to go into a panic clearing it quickly.\n";
			happy -= 10;
		}
	}

	//base tips, aprox 5-30% of base wages
	tips += (((5 + jobperformance / 8) * wages) / 100);

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; tips += 15;
	}

	if (g_Girls.GetStat(girl, STAT_CHARISMA) > 80 && g_Dice.percent(15))
	{
		ss << "Her charisma shone through as she chatted to customers between songs.\n\n"; tips += 15; happy += 5;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(5))
	{
		ss << "Her clumsy nature caused her to trip coming on stage causing the crowed to go wild with laughter. She went to the back and hide refusing to take the stage for her song set.\n"; wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n"; tips -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; tips += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her on her singing.\n"; tips -= 10;
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Psychic") && g_Dice.percent(20))
	{
		ss << "She knew just what songs to sing to get better tips by using her Psychic powers.\n"; tips += 15;
	}

	if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			ss << "A patron booed her. She saw red and using her Assassin skills killed him instantly. Other patrons fled out without paying.\n"; wages -= 50;
		}
		else
		{
			ss << "A patron booed her. But was drunk and started crying a moment later so she ignored them.\n";
		}
	}

	if (g_Girls.HasTrait(girl, "Horrific Scars") && g_Dice.percent(15))
	{
		if (jobperformance < 150)
		{
			ss << "A patron gasped and pointed at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
		}
		else
		{
			ss << "A patron gasped and pointed at her Horrific Scars making her sad. Her singing was so wonderful that at the end of the performance they personally apologized and thanked her, leaving her a good tip.\n"; tips += 15;
		}
	}

	if (g_Girls.HasTrait(girl, "Idol") && g_Girls.GetStat(girl, STAT_FAME) > 75 && g_Dice.percent(25))
	{
		ss << "Today a large group of " << girlName << "'s followers came to listen to her sing, leaving very generous tips behind.\n";
		wages += 15;
		tips += 25 + g_Girls.GetStat(girl, STAT_FAME) / 4;
	}
	else if (g_Girls.HasTrait(girl, "Idol") && g_Dice.percent(25))
	{
		ss << "A group of " << girlName << "'s fans came to listen to her sing, leaving good tips behind.\n";
		wages += 10;
		tips += 20 + g_Girls.GetStat(girl, STAT_FAME) / 5;
	}
	else if (!g_Girls.HasTrait(girl, "Idol") && g_Girls.GetStat(girl, STAT_FAME) > 75 && g_Dice.percent(15))
	{
		ss << girlName << " is quite popular in Crossgate so a small crowd of people came in just to listen to her.\n";
		wages += 5;
		tips += 15;
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_PIANO, Day0Night1) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << "'s singing was out of tune with " << pianoname << " causing customers to leave with their fingers in their ears.\n"; tips -= 10;
		}
		else
		{
			ss << pianoname << " took her singing to the next level causing the tips to flood in.\n"; tips += 40;
		}
	}

	if (wages < 0)
		wages = 0;
	if (tips < 0)
		tips = 0;


	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3; g_Girls.UpdateStat(girl, STAT_CONFIDENCE, 1);
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}

	brothel->m_Fame += fame;
	brothel->m_Happiness += happy;

	g_Girls.UpdateEnjoyment(girl, actiontype, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_SING, Day0Night1);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	if (wages < 0) wages = 0;
	if (tips < 0) tips = 0;
	girl->m_Pay = wages;
	girl->m_Tips = tips;

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 70, actiontype, "Singing on a daily basis has made " + girlName + " more Charismatic.", Day0Night1);
	if (g_Girls.GetStat(girl, STAT_FAME) >= 80 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Idol", 50, actiontype, "Her fame and singing skills has made " + girlName + " an Idol in Crossgate.", Day0Night1);
	}

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Meek", 50, actiontype, girlName + "'s having to sing every day has forced her to get over her meekness.", Day0Night1);

	return false;
}
double cJobManager::JP_BarSinger(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_CONFIDENCE) + g_Girls.GetSkill(girl, SKILL_PERFORMANCE));
	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 5;   //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 5;   //people like charming people	
	if (g_Girls.HasTrait(girl, "Elegant"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))    jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10;  //knows what people want to hear
	if (g_Girls.HasTrait(girl, "Fearless"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Singer"))			jobperformance += 50;
	if (g_Girls.HasTrait(girl, "Idol"))				jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Natural Pheromones"))jobperformance += 15;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 10; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))	jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 10;//might trip on stage	
	if (g_Girls.HasTrait(girl, "Slow Learner")) jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Shy"))			jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Construct"))	jobperformance -= 20; //voice would sound funny
	if (g_Girls.HasTrait(girl, "Tone Deaf"))	jobperformance -= 150; //should never get good at this job

	if (g_Girls.HasTrait(girl, "Deaf"))			jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 20;	//would need smoke breaks and get out of breath quicker
	if (g_Girls.HasTrait(girl, "Mute"))			jobperformance -= 1000; //cant sing
	if (g_Girls.HasTrait(girl, "Canine"))		jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Cat Girl"))		jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Demon Possessed"))jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Reptilian"))	jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Slitherer"))	jobperformance -= 20;	//Voice would sound funny
	if (g_Girls.HasTrait(girl, "Spirit Possessed"))jobperformance -= 20;	//Voice would sound funny

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Cum Addict"))			jobperformance -= 5;


	return jobperformance;
}

