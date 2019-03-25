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

// `J` Job Brothel - Bar
bool cJobManager::WorkBarWaitress(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKBAR;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN changed to informative message - hate not knowing what she's refused on the recap screen
		ss << " refused to wait the bar " << (Day0Night1 ? "tonight." : "today.");
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

	ss << " has been assigned to work as a waitress at your restaurant. She is informed that this is a genteel and conservative establishment, and she should focus on providing timely and efficient service.\n";
	if (girl->has_trait( "Mind Fucked"))
	{
		ss << girlName << " nods in understanding, but she also has a hand down her skirt, absent-mindedly rubbing her pussy as she listens. You are not entirely sure that she understands what \"genteel and conservative\" means here.. " << girlName << "'s mind fucked state may make this a more interesting shift than you anticipated.\n \n";
	}
	else
	{
		ss << girlName << " worked as a waitress in the bar.\n \n";
	}

	g_Girls.UnequipCombat(girl);  // put that shit away, you'll scare off the customers!


	sGirl* barmaidonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_BARMAID, Day0Night1);
	string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->m_Realname + "" : "the Barmaid");

	sGirl* cookonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_BARCOOK, Day0Night1);
	string cookname = (cookonduty ? "Cook " + cookonduty->m_Realname + "" : "the cook");

	int wages = 15, tips = 0;
	int enjoy = 0, fame = 0;

	int imagetype = IMGTYPE_WAIT;
	int msgtype = Day0Night1;
	int HateLove = girl->pclove() - girl->pchate();

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_BarWaitress(girl, false);

	//a little pre-game randomness
	if (girl->has_trait( "Mind Fucked") && g_Dice.percent(20))
	{
		ss << girlName << " unnerves the customers with her far-away stare while she takes their orders, and causes a few gentlemen to sweat with discomfort as she casually mentions that she is also on the menu, if they would like to use her for anything. They all cough, turn their heads, and try to ignore it.";
		jobperformance -= 5;
	}
	if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Sexy Air")
				|| girl->has_trait( "Titanic Tits") || girl->has_trait( "Big Boobs")
				|| girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs") && g_Dice.percent(20))
	{
		ss << "Customers are willing to forgive a lot of mistakes where " << girlName << " is concerned. Her sexy body distracts them when they order, and some find themselves ordering much more expensive wine than they had anticipated in an effort to impress her.";
		jobperformance += 5; tips += 10;
	}
	if (girl->has_trait( "Shy") || girl->has_trait( "Nervous") && g_Dice.percent(20))
	{
		ss << "Some customers are forced to flag " << girlName << " down to get her to come to their table, as her nerves and shyness get in the way of providing suitable service. She is not comfortable talking with all of these new people.";
		jobperformance -= 10;
	}


	if (jobperformance >= 245)
	{
		ss << " She's the perfect waitress. Customers go on about her and many seem to come more for her than for the drinks or entertainment.\n \n";
		wages += 155;

		if (roll_b <= 14)
		{
			ss << girlName << " danced around the bar dropping orders off as if she didn't even have to think about it.\n";
		}
		else if (roll_b <= 28)
		{
			ss << "Knowing how to speak with customers, " << girlName << " always gets the clients to order something extra or more pricey then they wanted at the beginning, making you a tidy profit.\n";
			wages += 10;
		}
		else if (roll_b <= 42)
		{
			ss << "Being a very popular waitress made " << girlName << " some fans, that come here only to be served by her. On the other hand they leave generous tips behind.\n";
			brothel->m_Fame += 10;
			tips += 10;
		}
		else if (roll_b <= 56)
		{
			ss << "Her shift past smoothly, earning her some really juicy tips.\n";
			tips += 15;
		}
		else if (roll_b <= 70)
		{
			ss << "Memorizing the whole menu and prices, taking out three or four trays at once, not making a single mistake for days. " << girlName << " sure doesn't seem human.\n";
		}
		else if (roll_b <= 84)
		{
			ss << "Today she saved your place from a major disturbance. When greeting clients, " << girlName << " noticed that the newly arrived group was part of a gang that was at war with another group of men which were already inside your place. She politely apologized and explained that your place was full and couldn't take such a large group. The men left unhappy but without giving her any trouble.\n";
		}
		else
		{
			ss << "People came in from everywhere to see " << girlName << " work.  She bounces all around the bar laughing and keeping the patrons happy without messing anything up.\n";
			brothel->m_Happiness += 10;
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
		wages += 95;

		if (roll_b <= 16)
		{
			ss << girlName << " bounced from table to table taking orders and recommending items to help you make more money.\n";
			wages += 10;
		}
		else if (roll_b <= 32)
		{
			ss << "She's very good at this. You saw her several times today carrying out two orders at a time.\n";
		}
		else if (roll_b <= 48)
		{
			ss << "When placing drink on the table she got slapped on the ass by one of the customers. " << girlName << " scolded them in a flirty way, saying that this kind of behavior could get them kicked out by the security and that she didn't want to lose her favorite customers. The client apologies, assuring her that he didn't mean any harm.\n";
		}
		else if (roll_b <= 64)
		{
			ss << "Knowing the menu paid off for " << girlName << ". After hearing the order she advised a more expensive option. Customers went for it and enjoyed their stay. Happy with the good advice they left her a great tip.\n";
			tips += 10;
		}
		else if (roll_b <= 83)
		{
			ss << "Memorizing the menu and prices has paid off for " << girlName << ".\n";
		}
		else
		{
			ss << girlName << " is a town favourite and knows most of the patrons by name and what they order.\n";
			brothel->m_Happiness += 5;
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by the customers often.\n \n";
		wages += 55;

		if (roll_b <= 14)
		{   //SIN - implement code for new boob traits
			if (girl->has_trait( "Big Boobs") || girl->has_trait( "Giant Juggs") || girl->has_trait( "Massive Melons"))
			{
				ss << "The patrons love being served by " << girlName << ".  Due to her skill at this job and the chance to stare at her Big Boobs.\n";
			}
			else if (girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Titanic Tits"))
			{
				ss << "The patrons love being served by " << girlName << ".  Due to her skill at this job and the chance to stare at her legendary boobs.\n";
			}
			else
			{
				ss << girlName << " didn't mess up any order this shift.  Keeping the patrons happy.\n";
				brothel->m_Happiness += 5;
			}
		}
		else if (roll_b <= 28)
		{
			ss << "One mishap today. A customer got away without paying!\n";
			wages -= 10;
		}
		else if (roll_b <= 42)
		{
			ss << "She spends her shift greeting customers and showing them to their tables.\n";
		}
		else if (roll_b <= 56)
		{
			ss << "Being confident in her skill, " << girlName << " didn't make a single mistake today. She also earned some tips from happy customers.\n";
			tips += 10;
		}
		else if (roll_b <= 70)
		{
			ss << "Having a slow shift, she mostly gossip with other staff members.\n";
		}
		else if (roll_b <= 84)
		{
			ss << "Ensuring that every table was served, tired " << girlName << " took a five minute breather.\n";
		}
		else
		{
			ss << girlName << " had some regulars come in. She knows there order by heart and put it in as soon as she seen them walk in making them happy.\n";
			brothel->m_Happiness += 5;
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
		wages += 15;

		if (roll_b <= 14)
		{
			ss << girlName << " forgot to take an order to a table for over an hour.  But they were in a forgiving mood and stuck around.\n";
		}
		else if (roll_b <= 28)
		{
			ss << "Trying her best, " << girlName << " made few mistakes but otherwise she had a pleasant day.\n";
			enjoy += 1;
		}
		else if (roll_b <= 42)
		{
			ss << "She tripped over her own shoelaces when carrying out an expensive order!\n";
			brothel->m_Filthiness += 5;
		}
		else if (roll_b <= 56)
		{
			ss << "Taking orders without mistakes and getting drinks to the tables not spilling a single drop from them. Today was a good day for " << girlName << ".\n";
			enjoy += 1;
		}
		else if (roll_b <= 70)
		{
			ss << "When placing drink on the table she got slapped on the ass by one of the customers. " << girlName << " didn't ignore that and called him names. Her behavior left the clients stunned for a moment.\n";
		}
		else if (roll_b <= 84)
		{
			ss << girlName << " spent her shift on cleaning duty. Mopping the floor, wiping tables and ensuring that the bathrooms were accessible. She did a decent job.\n";
			brothel->m_Filthiness -= 5;
		}
		else
		{
			ss << girlName << " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
			brothel->m_Happiness -= 5;
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		wages -= 5;
		if (roll_b <= 14)
		{
			ss << girlName << " wrote down the wrong orders for a few patrons resulting in them leaving.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll_b <= 28)
		{
			ss << "After being asked for the fourth time to repeat his order, the irritated customer left your facility. " << girlName << " clearly isn't very good at this job.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll_b <= 42)
		{
			ss << "Giving back change to a customer, " << girlName << " made an error calculating the amount in favor of the client. \n";
			wages -= 5;
		}
		else if (roll_b <= 56)
		{
			ss << "Holding the tray firmly in her hands, " << girlName << " successfully delivered the order to designated table. She was so nervous and focused on not failing this time, that she jumped scared when the customer thanked her.\n";
		}
		else if (roll_b <= 70)
		{
			ss << girlName << " really hates this job. She used every opportunity to take a break.\n";
		}
		else if (roll_b <= 84)
		{
			ss << "Still learning to do her job, " << girlName << " gets some orders wrong making a lot of people really angry.\n";
			brothel->m_Happiness -= 5;
		}
		else
		{
			ss << girlName << " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
			brothel->m_Happiness -= 5;
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		wages -= 15;
		if (roll_b <= 14)
		{
			ss << girlName << " was taking orders to the wrong tables and letting a lot of people walk out without paying their tab.\n";
		}
		else if (roll_b <= 28)
		{
			ss << girlName << " was presenting the ordered dish when she sneezed in it. The outraged customer demanded a new serving, that he got on the house.\n";
			brothel->m_Happiness -= 5;
			wages -= 15;
		}
		else if (roll_b <= 42)
		{
			ss << "The tray slipped from " << girlName << "'s hand right in front of the patron. Causing her to get yelled at for being bad at her job.\n";
			brothel->m_Filthiness += 5;
		}
		else if (roll_b <= 56)
		{
			ss << "Trying her best, " << girlName << " focused on not screwing up today. Surprisingly she managed not to fail at one of her appointed tasks today.\n";
		}
		else if (roll_b <= 70)
		{
			ss << "After picking up a tray full of drinks from the bar, " << girlName << " tried to bring it to the table. Her attempt failed when she slipped on wet floor that she mopped a minute ago.\n";
			brothel->m_Filthiness += 5;
		}
		else if (roll_b <= 84)
		{
			ss << girlName << " was slacking on the job spending most of her shift chatting with other staff members.\n";
		}
		else
		{
			ss << girlName << " spilled food all over the place and mixed orders up constantly.\n";
			brothel->m_Filthiness += 5;
		}
	}


	//base tips, aprox 10-20% of base wages
	tips += (int)(((10 + jobperformance / 22) * wages) / 100);

	//try and add randomness here
	if (girl->beauty() > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n \n";
		tips += 25;
	}

	if (girl->has_trait( "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature cause her to spill food on a customer resulting in them storming off without paying.\n";
		wages -= 25;
	}

	if (girl->has_trait( "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n";
			tips -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
			tips += 10;
		}
	}

	if (girl->has_trait( "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
			tips -= 10;
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			tips += 10;
		}
	}

	if (girl->has_trait( "Psychic") && g_Dice.percent(20))
	{
		if (g_Dice.percent(50))
		{
			ss << "She used her Psychic skills to know exactly what the patrons wanted to order making them happy and increasing her tips.\n";
		}
		else
		{
			ss << girlName << " uses her psychic abilities to her advantage. She knows when a customer is thinking about dessert or another bottle of wine, and shows up just in time with the perfect recommendation.\n";
		}
		tips += 20;
	}

	if (girl->has_trait( "Great Arse") && g_Dice.percent(15))
	{
		if (jobperformance >= 185) //great
		{
			ss << "A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n";
			tips += 15;
		}
		else if (jobperformance >= 135) //decent or good
		{
			ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything\n";
		}
		else if (jobperformance >= 85) //bad
		{
			ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n";
			wages -= 10;
		}
		else  //very bad
		{
			ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n";
			wages -= 15;
		}
	}

	if (girl->has_trait( "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			ss << "A patron pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
			wages -= 50;
		}
		else
		{
			ss << "A patron pissed her off but she just gave them a death stare and walked away.\n";
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
			ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she did a wonderful job they left a good tip.\n";
			tips += 25;
		}
	}

	if (girl->libido() > 90 && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus") || girl->has_trait( "Slut")))
	{
		ss << "During her shift, " << girlName << " couldn't help but instinctively and excessively rub her ass against the crotches of the clients whenever she got the chance. Her slutty behavior earned her some extra tips, as a couple of patrons noticed her intentional butt grinding.\n";
		tips += 30;
	}

	if (g_Dice.percent(5))
	{
		/*if (girl->medicine() >= 90)
		{ ss << "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n";
		wages += 15; }
		else if (girl->medicine() >= 60)
		{ ss << "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n";
		wages += 15; }
		else*/ if (girl->medicine() >= 30)
		{
			ss << "A customer started chocking on his food so " << girlName << " performed the heimlich maneuver on him. Grateful the man left her a better tip.\n";
			tips += 15;
		}
		else
		{
			ss << "A customer started chocking on his food so " << girlName << " not knowing what to do started screaming for help.\n";
		}
	}

	if (girl->herbalism() >= 40 && g_Dice.percent(5))
	{
		ss << "Added a little something extra to the patrons order to spice it up. They enjoyed it greatly and she received some nice tips.\n \n";
		tips += 25;
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_BARMAID, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " wasn't good enough at her job to use " << barmaidname << " to her advantage.\n";
		}
		else
		{
			ss << girlName << " used " << barmaidname << " to great effect speeding up her work and increasing her tips.\n";
			tips += 25;
		}
	}

	if (girl->has_trait( "Fleet Of Foot") && g_Dice.percent(30))
	{
		ss << girlName << " is fast on her feet, and makes great time navigating from table to table. She is able to serve almost twice as many customers in her shift.\n";
		tips += 50;
	}

	if (girl->has_trait( "Dojikko") && g_Dice.percent(35))
	{
		ss << girlName << " accidentally sends a tray of plates crashing to the floor, drawing the eyes of the entire restaurant to her. She smiles and sighs with such cuteness that everyone just laughs, and the customer whose dish splattered all over the tiles grins and says he would be happy to wait for a replacement, and that it could happen to anyone.\n";
		tips += 15;
		brothel->m_Happiness += 5;
	}

	//BIRTHDAY /**/
	if (girl->m_BDay >= 51)
	{
		if (girl->m_States&(1 << STATUS_SLAVE))
		{
		}
		else
		{
			if (HateLove >= 80)
			{
				if (girl->libido() >= 60)
				{
					ss << girlName << " has barely finished her shift before she is changed into her sexiest dress and standing before you. \"I have a little birthday wish,\" she whispers, drawing closer to you. \"I thought maybe, as your gift to me, you would allow me to serve you alone tonight. I asked " << cookname << " to cook all your favorite dishes, and I've prepared the upper dining area so it will just be the two of us.\" She leads you upstairs and seats you before disappearing for a moment and returning with the first course. " << girlName << " feeds you with her own hands, giggling with every few bites. \"We have a cake, of course,\" she smiles as you finish everything, \"but that isn't the only dessert.\"\n";
					if (roll_c >= 80)//ANAL
					{
						ss << girlName << " lifts up her skirt so you can see that she is not wearing underwear. \"I was hoping that you might put your birthday present in my ass,\" she whispers into your ear, deftly opening your pants and lowering herself onto your suddenly and ragingly erect cock. She whimpers briefly as your dick penetrates her, then she spits on her hand and rubs the lubricant onto your tip before impaling herself again. \"You have no idea how often I fantasize about this when dealing with those stodgy customers all day,\" she pants, reveling as you ream her ass. \"Use me like a dirty backstreet whore,\" she continues, wrapping her asshole around you and bouncing up and down. It does not take long to cum for both of you. " << girlName << " smiles";
						ss << " with fulfillment as she lifts herself off your cock, semen leaking onto the table. \"I guess I'll need to clean that up,\" she comments, before turning back to you. \"Happy birthday to me,\" she grins. \"Let's do it again sometime.\"";
						imagetype = IMGTYPE_ANAL;
						girl->anal(1);
					}
					else if (roll_c >= 50)//SEX
					{
						ss << girlName << " lies flat on her back on the cleared table, hiking up her dress so you have direct access to her wet pussy and placing the cake on her stomach. \"If you want dessert, I think you should come and get it,\" she purrs. You insert your hard-on into her and slowly fuck her as she moans, stopping only for a moment to take a piece of cake. You eat a bite and then feed her the rest as you pump with increasing speed, and as she takes the last bite, you spurt deep inside her. \"Happy birthday to me,\" she smiles.";
						imagetype = IMGTYPE_SEX;
						girl->normalsex(1);
					}
					else//ORAL
					{
						if (girl->oralsex() >= 50 && girl->has_trait( "Deep Throat"))
						{
							ss << girlName << " does not even wait for a reply before she moves her hand to your cock, deftly opening your pants and working you to a raging hard-on. She smiles mischievously at you and then dives down, swallowing your whole cock with one quick motion. She stays there, locked with her tongue on your balls and your shaft buried in her throat, massaging your cock with swallowing motions while staring with watering eyes into yours, until she begins to lose oxygen. You cum buckets straight down her throat as she begins to choke herself on you, and when she has secured every drop in her stomach, she pulls back, takes a deep breath, and smiles. \"Happy birthday to me,\" she says.";
						}
						else if (girl->oralsex() >= 50)
						{
							ss << girlName << " kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is excellent, and you relish every moment, taking an occasional bite of cake as she latches onto your dick.";
						}
						else
						{
							ss << girlName << " kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is not amazing, but it is delivered with such tenderness and love that you find yourself very satisfied regardless.";
						}
						imagetype = IMGTYPE_ORAL;
						girl->oralsex(1);
					}
				}
				else
				{
					ss << girlName << " finished her work and came to you with a shy grin. \"Did you know that it's my birthday?\" she asks, brushing against you gently and continuing without waiting for a response. \"I asked " << cookname << " to make a little something special, and I thought maybe we could share it together.\" The two of you enjoy a delicious light dinner, followed by an adorable little cake, as she giggles appreciably at your jokes and flirts with you. \"Maybe we should do this again without waiting a whole year,\" she suggests at the end of the evening, eyes flashing with unspoken promises. \"I'd love to thank you more fully for everything you have done for me.\"\n";
				}
			}
			else
			{
				ss << girlName << " finished her work as a waitress and returned home for a private birthday celebration with her friends.\n";
			}
		}
	}


#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	if (roll_a <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll_a <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		enjoy += 1;
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//


	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	int roll_max = (girl->beauty() + girl->charisma());
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 60 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	if (g_Dice % 2 == 1)
		girl->intelligence(1);
	else
		girl->agility(1);
	girl->service(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, actiontype, girlName + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", Day0Night1);
	if (jobperformance > 150 && girl->constitution() > 65)
	{
		g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 60, actiontype, girlName + " has been dodging between tables and avoiding running into customers for so long she has become Fleet of Foot.", Day0Night1);
	}

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her breaking hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", Day0Night1);

#pragma endregion
	return false;
}
double cJobManager::JP_BarWaitress(sGirl* girl, bool estimate)// not used
{
#if 1  //SIN - standardizing job performance calc per J's instructs
	double jobperformance =
		//main stats - first 100 - needs outstanding service skills
		girl->service() +
		//secondary stats - second 100 - charisma to charm customers, and agility and intelligence to get to them all
		((girl->charisma() + girl->intelligence() + girl->agility()) / 4) +
		//add level
		girl->level();

	// next up tiredness penalty
#else
	double jobperformance =
		(girl->intelligence() / 2 +
		girl->agility() / 2 +
		girl->service());
#endif
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Charismatic"))			jobperformance += 15;
	if (girl->has_trait( "Sexy Air"))				jobperformance += 10;
	if (girl->has_trait( "Cool Person"))			jobperformance += 10;	//people love to be around her
	if (girl->has_trait( "Cute"))					jobperformance += 5;
	if (girl->has_trait( "Charming"))				jobperformance += 20;	//people like charming people
	if (girl->has_trait( "Quick Learner"))		jobperformance += 5;
	if (girl->has_trait( "Psychic"))				jobperformance += 10;
	if (girl->has_trait( "Fleet of Foot"))		jobperformance += 5;	//faster at taking orders and droping them off
	if (girl->has_trait( "Waitress"))				jobperformance += 40;
	if (girl->has_trait( "Natural Pheromones"))	jobperformance += 15;
	if (girl->has_trait( "Agile"))				jobperformance += 5;
	if (girl->has_trait( "Flexible"))				jobperformance += 5;
	if (girl->has_trait( "Flat Ass"))				jobperformance += 10;	//Ass wont get in the way
	if (girl->has_trait( "Prehensile Tail"))		jobperformance += 10;	//for carrying that one extra drink!
	if (girl->has_trait( "Social Drinker"))		jobperformance += 5;	//customers like a girl that might have a drink with them
	if (girl->has_trait( "Bisexual"))				jobperformance += 5;	//can flirt with everyone



	//bad traits
	if (girl->has_trait( "Dependant"))			  jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 				  jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive"))			  jobperformance -= 20; //gets mad easy and may attack people
	if (girl->has_trait( "Nervous"))				  jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Abnormally Large Boobs")) jobperformance -= 20; //boobs are too big and get in the way
	if (girl->has_trait( "Titanic Tits"))			  jobperformance -= 25; //boobs are too big and get in the way
	if (girl->has_trait( "Meek"))					  jobperformance -= 20;
	if (girl->has_trait( "Slow Learner"))			  jobperformance -= 10;
	if (girl->has_trait( "One Eye"))				  jobperformance -= 10;

	if (girl->has_trait( "One Arm"))		jobperformance -= 30;
	if (girl->has_trait( "One Foot"))		jobperformance -= 20;
	if (girl->has_trait( "One Hand"))		jobperformance -= 15;
	if (girl->has_trait( "One Leg"))		jobperformance -= 40;
	if (girl->has_trait( "No Arms"))		jobperformance -= 100;
	if (girl->has_trait( "No Feet"))		jobperformance -= 30;
	if (girl->has_trait( "No Hands"))		jobperformance -= 15;
	if (girl->has_trait( "No Legs"))		jobperformance -= 100;
	if (girl->has_trait( "Blind"))		jobperformance -= 60;
	if (girl->has_trait( "Deaf"))			jobperformance -= 40;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks
	if (girl->has_trait( "Dominatrix"))	jobperformance -= 5;	//try and force the people to order what she wants them to not what they want

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 40; //might drink the drinks instead of taking to people
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Cum Addict"))			jobperformance -= 5;

	return jobperformance;
}
