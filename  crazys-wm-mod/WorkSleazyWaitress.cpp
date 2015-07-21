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

#pragma endregion

// `J` Job Brothel - Sleazy Bar
bool cJobManager::WorkSleazyWaitress(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCLUB;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a waitress in the strip club.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	double wages = 25, tips = 0;
	int enjoy = 0, anal = 0, health = 0, happy = 0, oral = 0, hand = 0, fame = 0;
	int imagetype = IMGTYPE_ECCHI;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_SleazyWaitress(girl, false);

	sGirl* barmaidonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_SLEAZYBARMAID, Day0Night1);
	string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->m_Realname + "" : "the Barmaid");

	// `CRAZY`
	/*default*/	int dick_type = 1;    string dick_type_text = "normal sized";
	/* */if (roll_c <= 10)	{ dick_type = 2; dick_type_text = "huge"; }
	else if (roll_c >= 90)	{ dick_type = 0; dick_type_text = "small"; }


	if (jobperformance >= 245)
	{
		ss << " She must be the perfect waitress customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
	}
	else if (jobperformance >= 135)
	{
		ss << " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
	}
	else if (jobperformance >= 85)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
	}


	//base tips, aprox 10-20% of base wages
	tips += (((10 + jobperformance / 22) * wages) / 100);
	
	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; tips += 25;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature cause her to spill food on a customer resulting in them storming off without paying.\n"; wages -= 25;
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
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; tips -= 10;
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Great Arse") && g_Dice.percent(15))
	{
		if (jobperformance >= 185) //great
		{
			ss << "A patron reached out to grab her ass. But she skillfully avoided it";
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70 && !g_Girls.HasTrait(girl, "Lesbian"))
			{
				ss << " and said that's only on the menu if your willing to pay up. He jumped at the chance to get to try her ass out and bent her over the table and whiping out his " << dick_type_text << " dick.";
				wages += g_Girls.GetStat(girl, STAT_ASKPRICE) + 50;
				imagetype = IMGTYPE_ANAL;
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
				if (roll_c >= 90)//small
				{
					if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
					{
						ss << " It slide right in her well trained ass with no problems."; anal += 1;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " Thankfully she is fast to orgasms or she wouldn't have got much out of this.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " She got nothing out of this as his dick was to small and its hard to get her off anyway.\n";
						}
						else
						{
							ss << " She slightly enjoyed herself.\n";
						}
					}
					else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
					{
						ss << " It slide into her ass with little trouble as she is slight trained in the anal arts."; anal += 2;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " She was able to get off on his small cock a few times thanks to her fast orgasm ability.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " Didn't get much out of his small cock as she is so slow to orgasm.\n";
						}
						else
						{
							ss << " Enjoyed his small cock even if she didn't get off.\n";
						}
					}
					else
					{
						ss << " Despite the fact that it was small it was still a tight fit in her inexperienced ass."; anal += 3;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " Her lack of skill at anal and the fact that she is fast to orgasm she had a great time even with the small cock.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " Her tight ass help her get off on the small cock even though it is hard for her to get off.\n";
						}
						else
						{
							ss << " Her tight ass help her get off on his small cock.\n";
						}
					}
				}
				else if (roll_c <= 10)//huge
				{
					if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
					{
						ss << " Her well trained ass was able to take the huge dick with little trouble."; anal += 3;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " She orgasmed over and over on his huge cock and when he finally finished she was left a gasping for air in a state of ecstasy.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " Despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n";
						}
						else
						{
							ss << " She orgasmed many times and loved every inch of his huge dick.\n";
						}
					}
					else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
					{
						if ((g_Girls.HasItemJ(girl, "Booty Lube") != -1 || g_Girls.HasItemJ(girl, "Deluxe Booty Lube") != -1) && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
						{
							ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in easier."; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " With the help of her Booty Lube she was able to enjoy every inch of his huge dick and orgasmed many times. When he was done she was left shacking with pleasure.\n";
							}
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{
								ss << " With the help of her Booty Lube and despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n";
							}
							else
							{
								ss << " With the help of her Booty Lube she was able to orgasm many times and loved every inch of his huge dick.\n";
							}
						}
						else
						{
							ss << " Her slighted trained ass was able to take the huge dick with only a little pain at the start."; anal += 2;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " After a few minutes of letting her ass get used to his big cock she was finally able to enjoy it and orgasmed many times screaming in pleasure.\n";
							}
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{
								ss << " After a few minutes of letting her ass get used to his big cock she was able to orgasm.\n";
							}
							else
							{
								ss << " After a few minutes of letting her ass get used to his big cock she was able to take the whole thing and orgasmed a few times.\n";
							}
						}
					}
					else
					{
						if ((g_Girls.HasItemJ(girl, "Booty Lube") != -1 || g_Girls.HasItemJ(girl, "Deluxe Booty Lube") != -1) && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
						{
							ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed many times.\n";
							}
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{
								ss << " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed one time.\n";
							}
							else
							{
								ss << " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed a few times.\n";
							}
						}
						else
						{
							ss << " She screamed in pain as he stuffed his huge dick in her tight ass.\n"; anal += 1;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " She was able to get some joy out of it in the end as she is fast to orgasm.\n";
							}
							else
							{
								ss << " It was nothing but a painful experience for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n";
							}
						}
					}
				}
				else//normal
				{
					if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 70)
					{
						ss << " It slide right in her well trained ass."; anal += 2;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " She was able to get off a few times as she is fast to orgasm.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " She was getting close to done when he pulled out and shot his wade all over her back. Its to bad she is slow to orgasm.\n";
						}
						else
						{
							ss << " She was able to get off by the end.\n";
						}
					}
					else if (g_Girls.GetSkill(girl, SKILL_ANAL) >= 40)
					{
						ss << " It was a good fit for her slightly trained ass."; anal += 3;
						if (g_Girls.HasTrait(girl, "Fast Orgasms"))
						{
							ss << " His cock being a good fit for her ass she was able to orgasm many times and was screaming in pleasure before to long.\n";
						}
						else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
						{
							ss << " His cock being a good fit for her ass he was able to bring her to orgasm if a bit slowly.\n";
						}
						else
						{
							ss << " His cock being a good fit for her ass she orgasmed a few times. When he was done she was left with a smile on her face.\n";
						}
					}
					else
					{
						if ((g_Girls.HasItemJ(girl, "Booty Lube") != -1 || g_Girls.HasItemJ(girl, "Deluxe Booty Lube") != -1) && g_Girls.GetStat(girl, STAT_INTELLIGENCE) >= 60)
						{
							ss << " Upon seeing his dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier."; anal += 3;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n";
							}
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{
								ss << " Thanks to her lube she was able to enjoy it much faster and was able to orgasm.\n";
							}
							else
							{
								ss << " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n";
							}
						}
						else
						{
							ss << " Despite the fact that it was normal sized it was still a very tight fit in her inexperienced ass."; anal += 2;
							if (g_Girls.HasTrait(girl, "Fast Orgasms"))
							{
								ss << " It was slightly painful at first but after a few minutes it wasn't a problem and she was able to orgasm.\n";
							}
							else if (g_Girls.HasTrait(girl, "Slow Orgasms"))
							{
								ss << " It was slightly painful at first but after a few minutes it wasn't a problem. But she wasn't able to orgasm in the end.\n";
							}
							else
							{
								ss << " It was slightly painful at first but after a few minutes it wasn't a problem she enjoyed it in the end.\n";
							}
						}
					}
				}
			}
			else
			{
				ss << " and with a laugh and told him that her ass wasn't on the menu. He laughed so hard he increased her tip!\n"; tips += 25;
			}
		}
		else if (jobperformance >= 135) //decent or good
		{
			ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything.\n";
		}
		else if (jobperformance >= 85) //bad
		{
			ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n"; wages -= 10;
		}
		else  //very bad
		{
			ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n"; wages -= 15;
		}
	}

	if (g_Girls.HasTrait(girl, "Great Figure") && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << "'s amazing figure wasn't enough to keep the patrons happy when her service was so bad.\n"; tips -= 10;
		}
		else
		{
			ss << "Not only does she have an amazing figure but she is also an amazing waitress the patrons really love her and her tips prove it.\n"; tips += 20;
		}
	}

	if ((g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Shy")) && g_Dice.percent(5))
	{
		ss << girlName << " was taking an order from a rather rude patron when he decide to grope her. She isn't the kind of girl to resist this and had a bad day at work because of this.\n";
		enjoy -= 5;
	}

	if (g_Dice.percent(25) && (g_Girls.HasTrait(girl, "Nymphomaniac")     || g_Girls.HasTrait(girl, "Succubus")) && (g_Girls.GetSkill(girl, SKILL_ORALSEX) > 80 || g_Girls.HasTrait(girl, "Cum Addict")) && !g_Girls.HasTrait(girl, "Lesbian")  && g_Girls.GetStat(girl, STAT_LIBIDO) > 90 )
	{
		ss << girlName << " thought she deserved a short break and disappeared under one of the tables when nobody was looking, in order to give one of the clients a blowjob. Kneeling under the table, she devoured his cock with ease and deepthroated him as he came to make sure she didn't make a mess. The client himself was wasted out of his mind and didn't catch as much as a glimpse of her, but he left the locale with a big tip on the table.\n";
		tips += 50; 
		imagetype = IMGTYPE_ORAL;
		oral += 2;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
	}

	if (g_Dice.percent(25) && !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) > 90  && (g_Girls.HasTrait(girl, "Nymphomaniac") || g_Girls.HasTrait(girl, "Succubus")) && (g_Girls.GetSkill(girl, SKILL_HANDJOB) > 80 || g_Girls.HasTrait(girl, "Cum Addict")))
	{
		ss << "During her shift, " << girlName << " unnoticeably dived under the table belonging to a lonely-looking fellow, quickly unzipped his pants and started jacking him off enthusiastically. She skillfully wiped herself when he came all over her face. The whole event took no longer than two minutes, but was well worth the time spent on it, since the patron left with a heavy tip.\n";
		tips += 50;
		imagetype = IMGTYPE_HAND;
		hand += 2;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
	}

	if (g_Dice.percent(20) && (g_Girls.HasTrait(girl, "Exhibitionist") || g_Girls.GetStat(girl, STAT_DIGNITY) <= -20))
	{
		if (roll_b <= 50)
		{
			ss << "During her shift, " << girlName << " deliberately dropped the pen she uses to write down orders in front of one of the customers. Exploiting her skimpy outfit, she made sure to bend over to pick it up in a way that allowed her to directly flash her butt on the sitting client's eye level. This earned her an extra tip.\n"; tips += 20;
		}
		else
		{
			ss << "An inebriated patron said half-jokingly to " << girlName << " that he'll leave a heavy tip if she takes his order while sitting on his lap. Much to his surprise, " << girlName << " was almost too eager to comply, sitting directly on his crotch instead, making sure to grind her butt into it. The customer lived up to his word and " << girlName << " left the table with some extra cash.\n"; tips += 30;
		}
	}

	if (g_Girls.GetStat(girl, STAT_DIGNITY) <= -20 && g_Dice.percent(20) && (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Giant Juggs") || g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits"))) //updated for the new breast traits
	{
		ss << "A drunk patron \"accidentally\" fell onto " << girlName << " and buried his face between her breasts. To his joy and surprise, " << girlName << " flirtatiously encouraged him to motorboat them for awhile, which he gladly did, before slipping some cash between the titties and staggering out on his way.\n"; tips += 40;
	}

	if (g_Girls.HasTrait(girl, "Futanari") && g_Girls.GetStat(girl, STAT_LIBIDO) > 80 && g_Dice.percent(5))
	{
		if (g_Girls.HasTrait(girl, "Open Minded") || g_Girls.HasTrait(girl, "Exhibitionist") || g_Girls.HasTrait(girl, "Slut") || g_Girls.HasTrait(girl, "Succubus") || (g_Girls.GetStat(girl, STAT_CONFIDENCE) > 35 && g_Girls.GetStat(girl, STAT_DIGNITY) < 35))
		{
			ss << "Noticing the bulge under her skirt one of the customers asked for a very special service: He wanted some \"cream\" in his drink. " << girlName << " took her already hard cock out and sprinkled the drink with some of her jizz. The customer thanked her and slipped a good tip under her panties.\n";
			g_Girls.UpdateSkill(girl, SKILL_SERVICE, 2);
			g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -30);
			tips += 30 + (g_Girls.GetSkill(girl, SKILL_SERVICE) / 5); //Not sure if this will work fine
			imagetype = IMGTYPE_MAST;
		}
		else
		{
			ss << "Noticing the bulge under her skirt one of the customers asked " << girlName << " to spill some of her \"cream\" in his drink, but she refused, blushing.\n";
		}
	}
	
	if ((g_Girls.HasTrait(girl, "Busty Boobs")              &&g_Dice.percent(5) ) ||
		(g_Girls.HasTrait(girl, "Big Boobs")                &&g_Dice.percent(10)) ||
		(g_Girls.HasTrait(girl, "Giant Juggs")              &&g_Dice.percent(15)) ||
		(g_Girls.HasTrait(girl, "Massive Melons")           &&g_Dice.percent(20)) ||
		(g_Girls.HasTrait(girl, "Abnormally Large Boobs")   &&g_Dice.percent(25)) ||
		(g_Girls.HasTrait(girl, "Titanic Tits")             &&g_Dice.percent(30)))
	{
		if (g_Girls.HasTrait(girl, "Exhibitionist") || g_Girls.HasTrait(girl, "Bimbo"))
		{
			ss << "A patron was staring obviously at her large breasts, so she took off her top to show him her tits, which earned her a small tip.\n";
			tips += 15;
			if (g_Girls.HasTrait(girl, "Missing Nipple")) {tips -= 5;}
			if (g_Girls.HasTrait(girl, "No Nipples")) {tips -= 5;}
			if (g_Girls.HasTrait(girl, "Big Boobs")) {tips += 1;}
			if (g_Girls.HasTrait(girl, "Giant Juggs")) {tips += 2;}
			if (g_Girls.HasTrait(girl, "Massive Melons")) {tips += 3;}
			if (g_Girls.HasTrait(girl, "Abnormally Large Boobs")) {tips += 4;}
			if (g_Girls.HasTrait(girl, "Titanic Tits")) {tips += 5;}
			if (g_Girls.HasTrait(girl, "Puffy Nipples")) {tips += 1;}
			if (g_Girls.HasTrait(girl, "Perky Nipples")) {tips += 1;}
			if (g_Girls.HasTrait(girl, "Pierced Nipples")) {tips += 2;}
			if (g_Girls.GetStat(girl, STAT_DIGNITY) > 60)
			{
				g_Girls.UpdateStat(girl, STAT_DIGNITY, -1);
			}
		}
		else if (!g_Girls.HasTrait(girl, "Lesbian") && (g_Girls.HasTrait(girl, "Slut") || g_Girls.HasTrait(girl, "Succubus")))
		{
			g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 2);
			tips += 15;
			if (g_Girls.GetStat(girl, STAT_DIGNITY) > 30)
			{
				g_Girls.UpdateStat(girl, STAT_DIGNITY, -1);
			} 
			if (g_Girls.HasTrait(girl, "Pierced Nipples"))
			{
				tips += 3;
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 1);
				ss << "A patron was staring obviously at her large breasts, so she grabbed his hand, slipped it under her clothes and let him play with her boobs. Her nipple piercings were a pleasant surprise to him, and he slipped a small tip between her tits afterwards.\n";
			}
			else
			{
				ss << "A patron was staring obviously at her large breasts, so she grabbed his hand, slipped it under her clothes and let him play with her boobs. He slipped a small tip between her tits afterwards.\n";
			}
		}
	}
	
	if (girl->is_pregnant() && g_Dice.percent(15))
	{
		if (g_Girls.GetStat(girl, STAT_LACTATION) > 50 && 
			(g_Girls.HasTrait(girl, "Open Minded") || g_Girls.HasTrait(girl, "Exhibitionist") || g_Girls.HasTrait(girl, "Slut") 
			|| g_Girls.HasTrait(girl, "Succubus") || 
			
			(g_Girls.GetStat(girl, STAT_CONFIDENCE) > 35 && g_Girls.GetStat(girl, STAT_DIGNITY) < 50)))
		{
			ss << "Noticing her belly one of the customers asked for some breast milk in his coffee. " << girlName << " took one of her breasts out and put some of her milk in the drink. The customer thanked her and slipped a good tip under her skirt.\n";
			g_Girls.UpdateSkill(girl, SKILL_SERVICE, 2);
			g_Girls.UpdateStat(girl, STAT_LACTATION, -30);
			tips += 30 + (g_Girls.GetSkill(girl, SKILL_SERVICE) / 5);
		}
		else if (g_Girls.GetStat(girl, STAT_LACTATION) < 50)
		{
			ss << "Noticing her belly one of the customers asked for some breast milk in his drink, but " << girlName << " said that she didn't have enough.\n";
		}
		else
		{
			ss << "Noticing her belly one of the customers asked for some breast milk in his drink, but she refused, blushing.\n";
		}
	}
	
	if (g_Girls.HasTrait(girl, "Alcoholic") && g_Dice.percent(10))
	{
		ss << girlName << " couldn't resist the offer of some patrons who invited her for a drink. And another one. And another one... When she came back to her senses she was lying in the floor half naked and covered in cum...\n";
		int NumCust = g_Dice % 6 + 1;
		tips -= 10;
		wages -= 50;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
		g_Girls.UpdateSkill(girl, SKILL_ANAL, max(0, g_Dice % 4 + 1));
		g_Girls.UpdateSkill(girl, SKILL_BDSM, max(0, g_Dice % 4 - 1));
		g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, max(0, g_Dice % 4 + 1));
		g_Girls.UpdateSkill(girl, SKILL_GROUP, max(2, g_Dice % 4 + 2));
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, max(0, g_Dice % 4 + 0));
		g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, max(0, g_Dice % 4 - 1));
		g_Girls.UpdateSkill(girl, SKILL_HANDJOB, max(0, g_Dice % 4 + 0));
		g_Girls.UpdateSkill(girl, SKILL_FOOTJOB, max(0, g_Dice % 4 - 1));
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
		g_Girls.UpdateStat(girl, STAT_HEALTH, -1);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, -2);
		g_Girls.UpdateStat(girl, STAT_SPIRIT, -2);
		imagetype = IMGTYPE_GROUP;
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_GROUP, EVENT_DANGER);
		if (g_Girls.CheckVirginity(girl))
		{
			g_Girls.LoseVirginity(girl);
			ss << "\nShe is no longer a virgin.\n";
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -10);
			g_Girls.UpdateEnjoyment(girl, ACTION_SEX, -2);
			g_Girls.UpdateStat(girl, STAT_HEALTH, -1);
			g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
		}
		sCustomer* Cust = new sCustomer; g_Customers.GetCustomer(Cust, brothel); Cust->m_Amount = NumCust;
		if (!girl->calc_group_pregnancy(Cust, false, 1.0))
		{
			g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant.", 0);
		}
		delete Cust;
	}
	
	if (g_Brothels.GetNumGirlsOnJob(0, JOB_SLEAZYBARMAID, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance > 100)
		{
			ss << "\nWith the help from " << barmaidname << " " << girlName << " provided a better service to the customers, increasing her tips.\n";
			tips *= 1.2;
		}
	}
	else if (g_Brothels.GetNumGirlsOnJob(0, JOB_SLEAZYBARMAID, false) == 0 && jobperformance <= 100)
	{
		ss << "\n" << girlName << " had a hard time attending all the customers without the help of a barmaid.\n";
		tips *= 0.9;
	}


	if (wages < 0)
		wages = 0;
	if (tips < 0)
		tips = 0;

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	if (roll_a <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; enjoy -= 1;
	}
	else if (roll_a <= 25)
	{
		ss << "\nShe had a pleasant time working."; enjoy += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; enjoy += 1;
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//



	g_Girls.UpdateSkill(girl, SKILL_ORALSEX, oral);
	g_Girls.UpdateSkill(girl, SKILL_HANDJOB, hand);
	g_Girls.UpdateSkill(girl, SKILL_ANAL, anal);
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	// Money
	if (wages < 0)	wages = 0;	girl->m_Pay = (int)wages;
	if (tips < 0)	tips = 0;	girl->m_Tips = (int)tips;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }
	if (!g_Girls.HasTrait(girl, "Straight"))	{ libido += min(3, g_Brothels.GetNumGirlsOnJob(0, JOB_BARSTRIPPER, false)); }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 60 && jobperformance >= 185)		{ fame += 1; }

	g_Girls.UpdateStat(girl, STAT_FAME, fame);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gained traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, actiontype, girlName + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", Day0Night1);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONSTITUTION) > 65) { g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 60, actiontype, girlName + " has been dodging between tables and avoiding running into customers for so long she has become Fleet Of Foot.", Day0Night1); }
	if (g_Dice.percent(25) && g_Girls.GetStat(girl, STAT_DIGNITY) < 0 && (anal > 0 || oral > 0 || hand > 0))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1);
	}

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her breaking hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", Day0Night1);


#pragma endregion
	return false;
}


double cJobManager::JP_SleazyWaitress(sGirl* girl, bool estimate)// not used
{
	double jobperformance = ((g_Girls.GetStat(girl, STAT_CHARISMA) +
		g_Girls.GetStat(girl, STAT_BEAUTY) +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE)) / 3 +
		g_Girls.GetSkill(girl, SKILL_SERVICE));
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

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
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))	jobperformance += 5;  //faster at taking orders and droping them off
	if (g_Girls.HasTrait(girl, "Waitress"))			jobperformance += 25;
	if (g_Girls.HasTrait(girl, "Natural Pheromones"))jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Agile"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Flexible"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Flat Ass"))		jobperformance += 5;	//Ass wont get in the way


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 20; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))  jobperformance -= 10;  //boobs are to big and get in the way
	if (g_Girls.HasTrait(girl, "Titanic Tits"))	jobperformance -= 15; //boobs are to big and get in the way
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Eye"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Shy"))			jobperformance -= 10;
	
	if (g_Girls.HasTrait(girl, "One Arm"))		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "One Foot"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "One Hand"))		jobperformance -= 15; 
	if (g_Girls.HasTrait(girl, "One Leg"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "No Arms"))		jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "No Feet"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "No Hands"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "No Legs"))		jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "Blind"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Deaf"))			jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 40; //might drink the drinks instead of taking to people
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
