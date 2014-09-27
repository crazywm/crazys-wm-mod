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

bool cJobManager::WorkSleazyWaitress(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKCLUB, girl, brothel, DayNight, summary, message))			return true;

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int roll = g_Dice%100, roll_d = g_Dice%100;
	int jobperformance = ( (g_Girls.GetStat(girl, STAT_CHARISMA) +
							g_Girls.GetStat(girl, STAT_BEAUTY) +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE)) / 3 +
							g_Girls.GetSkill(girl, SKILL_SERVICE));

	int wages = 25, work = 0, anal = 0, health = 0, happy = 0, oral = 0, hand =0;
	int imagetype = IMGTYPE_ECCHI;
	message += "She worked as a waitress in the strip club.";


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10; //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10; //knows what people want to hear
	if (g_Girls.HasTrait(girl, "Fleet of Foot")) jobperformance += 5;  //faster at taking orders and droping them off


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 20; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))  jobperformance -= 20;  //boobs are to big and get in the way
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Eye"))		jobperformance -= 10;


	// `CRAZY`
	/*default*/	int dick_type = 1;    string dick_type_text = "normal sized";
	/* */if (roll_d <= 10)	{ dick_type = 2; dick_type_text = "huge"; }
	else if (roll_d >= 90)	{ dick_type = 0; dick_type_text = "small"; } 


	if (jobperformance >= 245)
		{
			message += " She must be the perfect waitress customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		}
	else if (jobperformance >= 135)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		}
	else if (jobperformance >= 85)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{ message += "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25; }

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
		{ message += "Her clumsy nature cause her to spill food on a custmoer resulting in them storming off without paying.\n"; wages -= 25; }

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += "Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10; }
		else
			{ message += girlName + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; wages -= 10; }
		else
			{ message += "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Great Arse") && g_Dice.percent(15))
	{
		if (jobperformance >= 185) //great
			{ message += "A patron reached out to grab her ass. But she skillfully avoided it" ;
				if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
				{
					message += " and said that's only on the menu if your willing to pay up. He jumped at the chance to get to try her ass out and bent her over the table and whiping out his " + dick_type_text + " dick."; 
					wages += g_Girls.GetStat(girl, STAT_ASKPRICE) + 50; imagetype = IMGTYPE_ANAL; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
					if (roll_d >= 90)//small
					{
						if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
						{
						  message += " It slide right in her well trained ass with no problems."; anal += 1; 
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{ message += " Thankfully she is fast to orgasms or she wouldn't have got much out of this.\n"; }
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{ message += " She got nothing out of this as his dick was to small and its hard to get her off anyway.\n"; }
							else
							{ message += " She slightly enjoyed herself.\n"; }
						}
						else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
						{
							message += " It slide into her ass with little trouble as she is slight trained in the anal arts."; anal += 2;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{ message += " She was able to get off on his small cock a few times thanks to her fast orgasm abilty.\n"; }
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{ message += " Didn't get much out of his small cock as she is so slow to orgasm.\n"; }
							else
							{ message += " Enjoyed his small cock even if she didn't get off.\n"; }
						}
						else
						{
							message += " Despite the fact that it was small it was still a tight fit in her inexperienced ass." ; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{ message += " Her lack of skill at anal and the fact that she is fast to orgasm she had a great time even with the small cock.\n"; }
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{ message += " Her tight ass help her get off on the small cock even though it is hard for her to get off.\n"; }
							else
							{ message += " Her tight ass help her get off on his small cock.\n"; }
						}
					}
					else if (roll_d <= 10)//huge
					{
						if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
						{
							message += " Her well trained ass was able to take the huge dick with little trouble."; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{ message += " She orgasmed over and over on his huge cock and when he finally finished she was left a gasping for air in a state of ecstasy.\n"; }
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{ message += " Despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n"; }
							else
							{ message += " She orgasmed many times and loved every inch of his huge dick.\n"; }
						}
						else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
						{
							if (g_Girls.HasItemJ(girl, "Booty Lube") != -1 && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
							{
								message += " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in easier."; anal += 3;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " With the help of her Booty Lube she was able to enjoy every inch of his huge dick and orgasmed many times. When he was done she was left shacking with pleasure.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " With the help of her Booty Lube and despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n"; }
								else
								{ message += " With the help of her Booty Lube she was able to orgasm many times and loved every inch of his huge dick.\n"; }
							}
							else
							{
								message += " Her slighted trained ass was able to take the huge dick with only a little pain at the start."; anal += 2;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " After a few minutes of letting her ass get used to his big cock she was finally able to enjoy it and orgasmed many times screaming in pleasure.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " After a few minutes of letting her ass get used to his big cock she was able to orgasm.\n"; }
								else
								{ message += " After a few minutes of letting her ass get used to his big cock she was able to take the whole thing and orgasmed a few times.\n"; }
							}
						}
						else
						{
							if (g_Girls.HasItemJ(girl, "Booty Lube") != -1 && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
							{
								message += " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed many times.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed one time.\n"; }
								else
								{ message += " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed a few times.\n"; }
							}
							else
							{
								message += " She screamed in pain as he stuffed his huge dick in her tight ass.\n" ; anal += 1;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " She was able to get some joy out of it in the end as she is fast to orgasm.\n"; }
								else
								{ message += " It was nothing but a painful experince for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n"; }
							}
						}
					}
					else//normal
					{
						if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
						{
							message += " It slide right in her well trained ass."; anal += 2;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " She was able to get off a few times as she is fast to orgasm.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " She was getting close to done when he pulled out and shot his wade all over her back. Its to bad she is slow to orgasm.\n"; }
								else
								{ message += " She was able to get off by the end.\n"; }
						}
						else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
						{
							message += " It was a good fit for her slightly trained ass."; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " His cock beign a good fit for her ass she was able to orgasm many times and was screaming in pleasure before to long.\n"; }
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " His cock beign a good fit for her ass he was able to bring her to orgasm if a bit slowly.\n"; }
							else
								{ message += " His cock beign a good fit for her ass she orgasmed a few times. When he was done she was left with a smile on her face.\n"; }
						}
						else
						{
							if (g_Girls.HasItemJ(girl, "Booty Lube") != -1 && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
							{
							message += " Upon seeing his dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
									{ message += " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
									{ message += " Thanks to her lube she was able to enjoy it much faster and was able to orgasm.\n"; }
								else
									{ message += " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n"; }
							}
							else
							{
							message += " Despite the fact that it was normal sized it was still a very tight fit in her inexperienced ass."; anal += 2;
								if (g_Girls.HasTrait(girl, "Fast Orgasms"))
								{ message += " It was slightly painful at first but after a few minutes it wasn't a problem and she was able to orgasm.\n"; }
								else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
								{ message += " It was slightly painful at first but after a few minutes it wasn't a problem. But she wasn't able to orgasm in the end.\n"; }
								else
								{ message += " It was slightly painful at first but after a few minutes it wasn't a problem she enjoyed it in the end.\n"; }
							}
						}
					}
				}
				else
				{ message += " and with a laugh and told him that her ass wasn't on the menu. He laughed so hard he increased her tip!\n"; wages += 25; }
			}
		else if (jobperformance >= 135) //decent or good
			{ message += "A patron reached out and grabed her ass. She's use to this and skilled enough so she didn't drop anything\n"; }
		else if (jobperformance >= 85) //bad
			{ message += "A patron reached out and grabed her ass. She was startled and ended up dropping half an order.\n"; wages -= 10; }
		else  //very bad
			{ message += "A patron reached out and grabed her ass. She was startled and ended up dropping a whole order\n"; wages -= 15; }
	}

	if (g_Girls.HasTrait(girl, "Great Figure")&& g_Dice.percent(25))
	{
		if (jobperformance < 125)
			{ message += girlName + "'s amazing figure wasn't enough to keep the patrons happy when her servies was so bad.\n"; wages -= 10; }
		else
			{ message += "Not only does she have an amazing figure but she is also an amazing waitress the patrons really love her and her tips prove it.\n"; wages += 20; }
	}

	if (g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Shy") && g_Dice.percent(5))
		{
			message += girlName + " was taking an order from a rather rude patron when he decide to grope her. She ins't the kind of girl to resist this and had a bad day at work because of this.\n";
			work -=5;
		}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) > 90 && g_Girls.GetSkill(girl, SKILL_ORALSEX) > 80 && g_Dice.percent(25))
	{
		message += girlName + " thought she deserved a short break and disappeared under one of the tables when nobody was looking, in order to give one of the clients a blowjob. Kneeling under the table, she devoured his cock with ease and deepthroated him as he came to make sure she didn't make a mess. The client himself was wasted out of his mind and didn't catch as much as a glimpse of her, but he left the locale with a big tip on the table.\n";
		wages += 50; imagetype = IMGTYPE_ORAL; oral +=2; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -10);
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) > 90 && g_Girls.GetSkill(girl, SKILL_HANDJOB) > 80 && g_Dice.percent(25))
	{
		message += "During her shift, " + girlName + " unnoticeably dived under the table belonging to a lonely-looking fellow, quickly unzipped his pants and started jacking him off enthusiastically. She skillfully wiped herself when he came all over her face. The whole event took no longer than two minutes, but was well worth the time spent on, since the patron left with a heavy tip.\n";
		wages += 50; imagetype = IMGTYPE_HAND; hand += 2; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -10);
	}

	/*if (g_Girls.GetStat(girl, STAT_DIGNITY) && <= -20 g_Dice.percent(20))
	{
		if (roll <-50)
		{ message += "During her shift, " + girlName + " deliberately dropped the pen she uses to write down orders in front of one of the customers. Exploiting her skimpy outfit, she made sure to bend over to pick it up in a way that allowed her to directly flash her butt on the sitting client's eye level. This earned her an extra tip.\n"; wages += 20; }
		else 
		{ message += "An inebriated patron said half-jokingly to " + girlName + " that he'll leave a heavy tip if she takes his order while sitting on his lap. Much to his surprise, " + girlName + " was almost too eager to comply, sitting directly on his crotch instead, making sure to grind her butt into it. The customer lived up to his word and " + girlName + " left the table with some extra cash.\n"; wages += 30; }
	}*/

	/*if (g_Girls.GetStat(girl, STAT_DIGNITY) && <= -20 g_Dice.percent(20) && g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
	{
		 message += "A drunk patron \"accidentally\" fell onto " + girlName + " and buried his face between her breasts. To his joy and surprise, " + girlName + " flirtatiously encouraged him to motorboat them for a while, which he gladly did, before slipping some cash between the titties and staggering out on his way.\n"; wages += 40;
	}*/


		if(wages < 0)
		 wages = 0;


	//enjoyed the work or not
	if (roll <= 5)
	{ message += "\nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ message += "\nShe had a pleasant time working."; work += 3; }
	else
	{ message += "\nOtherwise, the shift passed uneventfully."; work += 1; }


	g_Girls.UpdateSkill(girl, SKILL_ORALSEX, oral);
	g_Girls.UpdateSkill(girl, SKILL_HANDJOB, hand);
	g_Girls.UpdateSkill(girl, SKILL_ANAL, anal);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, work , true);
	girl->m_Events.AddMessage(message, imagetype, DayNight);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay += wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, ACTION_WORKCLUB, girlName + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", DayNight != 0);
	if (jobperformance > 150 || g_Girls.GetStat(girl, STAT_CONSTITUTION) > 65) { g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 60, ACTION_WORKBAR, girlName + " has been doding bewteen tables and avoiding running into customers for so long she has become Fleet Of Foot.", DayNight != 0); }

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLUB, "It took her break hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", DayNight != 0);

	return false;
}