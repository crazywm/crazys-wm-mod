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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Brothel - Bar
bool cJobManager::WorkBarmaid(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKBAR;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a barmaid.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 15, work = 0;
	int imagetype = IMGTYPE_WAIT;

	int roll = g_Dice.d100();
	double jobperformance = JP_Barmaid(girl, false);

	//what is she wearing?
	if (g_Girls.HasItemJ(girl, "Bourgeoise Gown") != -1)
	{
		ss << girlName << "'s Bourgeoise Gown didn't really help or hurt her tips.\n\n";
	}
	else if (g_Girls.HasItemJ(girl, "Maid Uniform") != -1)
	{
		if (roll <= 50)
		{
			ss << girlName << "'s Maid Uniform didn't do much to help her.\n\n";
		}
		else
		{
			wages += 35; 
			brothel->m_Happiness += 5; 
			ss << girlName << "'s Maid Uniform didn't do much for most of the patrons, but a few of them seemed to really like it and tipped her extra.\n\n";
		}
	}


	//a little pre-game randomness
	if (g_Dice.percent(10))
	{
		if (g_Girls.HasTrait(girl, "Alcoholic"))
		{
			ss << girlName << "'s alcoholic nature caused her to drink serveral bottles of booze becoming drunk and her serving suffered cause of it.";
			jobperformance -= 50;
		}
	}

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
		if (roll <= 14)
		{
			ss << girlName << " was sliding drinks all over the bar without spilling a drop she put on quite a show for the patrons.\n";
			brothel->m_Fame += 10;
		}
		else if (roll <= 28)
		{
			ss << "She agree to play a bar game with a client. Knowing every type of bar game there is, " << girlName << " easily wins. The customer pays double for his drinks and leaves the bar saying that he will win next time.\n";
			wages += 25;
		}
		else if (roll <= 42)
		{
			ss << girlName << " made an 11 layer drink like it was nothing. The amazed customer left her a big tip!\n";
			brothel->m_Fame += 10;
			wages += 10;
		}
		else if (roll <= 56)
		{
			ss << "She pours eleven 100ml shots from a single, one liter bottle. Now there's a good barmaid!\n";
		}
		else if (roll <= 70)
		{
			ss << girlName << " noticed that a client was upset about something. After a pleasant conversation she managed to cheer him up. The client left full of willpower, leaving a generous tip behind.\n";
			brothel->m_Happiness += 10;
			wages += 10;
		}
		else if (roll <= 84)
		{
			ss << "Bottles fly high under the ceiling when " << girlName << " is pouring drinks for the customers. The amazed crowd loudly applauses every caught bottle and leave big tips for the girl.\n";
			brothel->m_Fame += 10;
			wages += 15;
		}
		else
		{
			ss << girlName << " mixed up what some patrons called the perfect drink.  It got them drunk faster then anything they had before.\n";
			brothel->m_Happiness += 10;
			brothel->m_Fame += 10;
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
		if (roll <= 14)
		{
			ss << girlName << " had the bar filled with happy drunks.  She didn't miss a beat all shift.\n";
			brothel->m_Happiness += 10;
		}
		else if (roll <= 28)
		{
			ss << "Today wasn't really exciting for " << girlName << ". From boredom she spent some time trying to make more complicated drinks from the menu.\n";
		}
		else if (roll <= 42)
		{
			ss << girlName << " propose to a client to play a drinking game with her. If she loses she will serve nude to the end of her shift, but if she wins he will be paying double. Some other patrons join the wager on the same terms. After a few hours the last of them drops drunk and " << girlName << " cleaned up on money.\n";
			wages += 20;
		}
		else if (roll <= 56)
		{
			ss << "When taking orders from customers, " << girlName << " talked them into buying more expensive drinks, that let you make a solid profit today.\n";
			wages += 10;
		}
		else if (roll <= 70)
		{
			ss << girlName << " is great at this job. At happy hour she was irreplaceable getting all the orders right. Later on she even prevented a fight between customers.\n";
			brothel->m_Fame += 5;
		}
		else if (roll <= 84)
		{
			ss << "Her shift was slow and hardly anyone was buying. " << girlName << " took the initiative, announcing a special promotion. Every third shot ordered by a client could be drunk from a shot-glass between her breasts. The promotion was such a success that you almost run out of booze.\n";
			wages += 10;
		}
		else
		{
			ss << "People love seeing " << girlName << " work and they pour into the bar during her shift.  She mixes wonderful drinks and doesn't mess orders up so they couldn't be happier.\n";
			brothel->m_Happiness += 10;
			brothel->m_Fame += 5;
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
		if (roll <= 14)
		{
			ss << girlName << " didn't mix up any orders and kept the patrons drunk and happy.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll <= 28)
		{
			ss << girlName << " certainly knows what she is doing behind the bar counter. She spends her shift without making any mistakes and earning a lot from tips.\n";
			wages += 5;
		}
		else if (roll <= 42)
		{
			ss << girlName << " didn't make any mistakes today. She even earned some tips from happy customers.\n";
			brothel->m_Happiness += 5;
			wages += 5;
		}
		else if (roll <= 56)
		{
			ss << "When mixing one of the more complicated cocktails, " << girlName << " noticed that she made a mistake and remakes the order. She wasted some alcohol, but the customer has happy with his drink.\n";
			brothel->m_Happiness += 5;
			wages -= 5;
		}
		else if (roll <= 70)
		{
			ss << girlName << " spent more time talking with customers than filling their glasses. She didn't earn much today.\n";
		}
		else if (roll <= 84)
		{
			ss << "Her shift as a barmaid goes smoothly. " << girlName << " feels more confident in her skills after today.\n";
		}
		else
		{
			ss << girlName << " had some regulars come in.  She knows just how to keep them happy and spending gold.\n";
			brothel->m_Happiness += 5;
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 14)
		{
			ss << girlName << " mixed up a few drink orders...  But they ordered the same drink so it didn't matter too much.\n";
		}
		else if (roll <= 28)
		{
			ss << girlName << " made few mistakes but none of them were lethal.\n";
		}
		else if (roll <= 42)
		{
			ss << "Trying her best she accidently knocks down a bottle containing one of the bars most expensive liquors.\n";
			brothel->m_Filthiness += 5;
		}
		else if (roll <= 56)
		{
			ss << "Maybe she isn't the best and fastest, but at least every drop of a drink that she pours stays in the glass.\n";
		}
		else if (roll <= 70)
		{
			ss << "She agreed to play a bar game with a client. " << girlName << " loses and the client spends the rest of his stay drinking on the house.\n";
		}
		else if (roll <= 84)
		{
			ss << girlName << " focused all her attention on taking orders and making drinks. Her attitude wasn't too appealing to clients. Some customers left feeling mistreated and unhappy.\n";
			brothel->m_Happiness -= 5;
		}
		else
		{
			ss << girlName << " wasted a few drinks by forgetting to put ice in them but it wasn't anything major.\n";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 14)
		{
			ss << girlName << " mixed up people's drink orders...  When she only had four patrons drinking.\n";
		}
		else if (roll <= 28)
		{
			ss << girlName << " is having a bad day and she isn't trying to hide it. Her bad attitude shows and rubs off on the customers, leaving a negative impression on them.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll <= 42)
		{
			ss << "Not being very good at this, she makes few mistakes. " << girlName << " feels that she didn't improve today.\n";
		}
		else if (roll <= 56)
		{
			ss << "Wanting to impress a client, she throws a bottle of an expensive liquor into the air. Trying to catch it behind her back, " << girlName << " fails.\n";
			brothel->m_Filthiness += 5;
		}
		else if (roll <= 70)
		{
			ss << "One patron, looking for encouragement or understanding from the barmaid, unfortunately approached " << girlName << ". After a short conversation, he left crying.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll <= 84)
		{
			ss << girlName << " tried to tap a new keg of beer; consequently she spends the rest of her shift mopping the floor.\n";
			brothel->m_Filthiness += 5;
		}
		else
		{
			ss << girlName << " gave someone a drink she mixed that made them sick.  It was nothing but coke and ice so who knows how she did it.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 14)
		{
			ss << girlName << " was giving orders to the wrong patrons and letting a lot people walk out without paying their tab.\n";
		}
		else if (roll <= 28)
		{
			ss << "She mixed the ordered cocktails in the wrong proportions, making the clients throw up from the intoxication after just one shot! Besides swearing at her and yelling that they will never come here again, they left without paying.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll <= 42)
		{
			ss << "You can see that standing behind the bar isn't her happy place. Being tense she made a lot of mistakes today.\n";
		}
		else if (roll <= 56)
		{
			ss << "Not having any experience at this kind of job, " << girlName << " tries her best.. Regrettably without results.\n";
		}
		else if (roll <= 70)
		{
			ss << "She gets in a drinking game with a customer. Being a total lightweight, she gets drunk quickly and passes out on the job.\n";
		}
		else if (roll <= 84)
		{
			ss << "She spends most of her shift flirting with one client and not paying much attention to the others. What's worse, the guy she was flirting with skips without paying the bill!\n";
		}
		else
		{
			ss << girlName << " spilled drinks all over the place and mixed the wrong stuff when trying to make drinks for people.\n";
			brothel->m_Filthiness += 5;
		}
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25;
	}

	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 99 && g_Dice.percent(5))
	{
		wages += 50; ss << girlName << " looked absolutely stunning during her shift and was unable to hide it. Instead of her ass or tits, the patrons couldn't take their eyes off her face, and spent a lot more than usual on tipping her.\n";
	}

	if (g_Girls.GetStat(girl, STAT_CHARISMA) > 85 && g_Dice.percent(20))
	{
		wages += 35; ss << girlName << " surprised a couple of gentlemen discussing some complicated issue by her insightful comments when she was taking her order. They decided her words were worth a heavy tip.\n";
	}

	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 30 && g_Dice.percent(20))
	{
		ss << girlName << " got confused when calculating the tabs, and seems to have damn near given away most of the alcohol.\n"; wages -= 35; brothel->m_Happiness += 5;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature caused her to spill a drink on a custmoer resulting in them storming off without paying.\n"; wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; wages -= 10;
		}
		else
		{
			ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10;
		}
	}
	// `J` slightly lower percent compared to sleazy barmaid, I would think regular barmaid's uniform is less revealing
	if ((g_Dice.percent(3) && g_Girls.HasTrait(girl, "Busty Boobs")) ||
		(g_Dice.percent(6) && g_Girls.HasTrait(girl, "Big Boobs")) ||
		(g_Dice.percent(9) && g_Girls.HasTrait(girl, "Giant Juggs")) ||
		(g_Dice.percent(12) && g_Girls.HasTrait(girl, "Massive Melons")) ||
		(g_Dice.percent(16) && g_Girls.HasTrait(girl, "Abnormally Large Boobs")) ||
		(g_Dice.percent(20) && g_Girls.HasTrait(girl, "Titanic Tits")))
	{
		ss << "A patron was obviously staring at her large breasts. ";
		if (jobperformance < 150)
		{
			ss << "But she had no idea how to take advantage of it.\n";
		}
		else
		{
			ss << "So she over-charged them for drinks while they were too busy drooling to notice the price.\n"; wages += 15;
		}
	}

	if (g_Girls.HasTrait(girl, "Psychic") && g_Dice.percent(20))
	{
		ss << "She used her Psychic skills to know exactly what the patrons wanted to order and when to refill their mugs, keeping them happy and increasing tips.\n"; wages += 15;
	}

	if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			wages -= 50; brothel->m_Happiness -= 2; ss << "A patron pissed her off and using her Assassin skills she killed him before she even realised. In the chaos that followed a number of patrons fled without paying.\n";
		}
		else
		{
			ss << "A patron pissed her off but she was able to keep her cool as she is getting used to this kinda thing.\n";
		}
	}

	if (g_Girls.HasTrait(girl, "Horrific Scars") && g_Dice.percent(15))
	{
		if (jobperformance < 150)
		{
			ss << "A patron gasped at her Horrific Scars making her sad. But they didn't feel sorry for her.\n";
		}
		else
		{
			ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she did a wonderful job they left a good tip.\n"; wages += 25;
		}
	}

	if (g_Girls.GetStat(girl, STAT_MORALITY) >= 80 && g_Dice.percent(20))
	{
		if (roll <= 50)
		{
			wages += 35; ss << "During her shift " << girlName << " spotted a depressed-looking lone man sinking his sorrows in alcohol. She spent a short while cheering him up. Surprised with her kindness, the client left her a generous tip.\n";
		}
		else
		{
			wages += 25; ss << "One of the patrons paid way too much for his order. When " << girlName << " quickly pointed out his mistake, he said not to worry about it and told her to keep the extra as a reward for her honesty.\n";
		}
	}

	if (g_Girls.GetStat(girl, STAT_MORALITY) <= -20 && g_Dice.percent(20))
	{
		if (roll <= 33)
		{
			wages += 35; ss << "During her shift, " << girlName << " spotted a lone fellow passed out from alcohol alone at a table in a corner, his wallet bulging out of his pocket. Without a second thought, she discreetly snatched it out and claimed for herself.\n";
		}
		else if (roll <= 66)
		{
			wages += 25; ss << "One of the patrons paid way too much for his order... and " << girlName << " didn't really feel like pointing it out, considering the extra money a generous tip.\n";
		}
		else
		{
			wages -= 15; ss << girlName << " responded to one of the vulgar remarks by a client in a much more vulgar way. Needless to say, this didn't earn her any favors with the patrons that shift, and her tips were a bit less than usual.\n";
		}
	}

	if (g_Girls.GetStat(girl, STAT_MORALITY) <= -20 && g_Girls.GetStat(girl, STAT_DIGNITY) <= -20 && g_Dice.percent(20))
	{
		wages += 40; ss << "A drunk patron suddenly walked up to " << girlName << " and just started groping her body. Instead of pushing him away immediately, " << girlName << " allowed him to take his time with her tits and butt while she helped herself to his pockets and all the money inside them. The rowdy client left with a dumb glee on his face, probably to find out his fondling was much, much overpriced.\n";
	}

	if (g_Girls.GetStat(girl, STAT_DIGNITY) <= -20 && g_Dice.percent(20))
	{
		if (roll <= 50)
		{
			wages += 15; ss << "When taking an order, " << girlName << " made sure to lean in really close for the client to get a full view of her cleavage. Giving him an eyefull of tits was promptly rewarded with some extra cash in tips.\n";
		}
		else
		{
			wages += 20; ss << "One of the rowdier clients gently slapped the butt of " << girlName << " when she was passing through. Her coy giggle only encouraged more clients to occasionally fondle her butt through the rest of her work, which earned her some extra tips.\n";
		}
	}

	if (g_Girls.GetStat(girl, STAT_DIGNITY) <= -20 && g_Dice.percent(20) && g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
	{
		wages += 25; ss << girlName << " got an odd request from a client to carry a small drink he ordered between her tits to his table. After pouring the drink in a thin glass, " << girlName << " handled the task with minimal difficulty and earned a bigger tip.\n";
	}

	if (g_Girls.GetStat(girl, STAT_MORALITY) <= -20 && g_Dice.percent(10))
	{
		ss << "A patron came up to her and said he wanted to order some milk but that he wanted it straight from the source. ";
		if (g_Girls.GetStat(girl, STAT_LACTATION) >= 20)
		{
			wages += 40; ss << "With a smile she said she was willing to do it for an extra charge. The patron quickly agreed and " << girlName << " proceed to take out one of her tits and let the patron suck out some milk.\n";
		}
		else
		{
			ss << "She was willing to do it but didn't have enough milk production.";
		}
	}

	if (girl->is_pregnant() && g_Dice.percent(10))
	{
		ss << "A customer tried to buy " << girlName << " a drink, but she refused for the sake of her unborn child.";
	}

	if ((g_Girls.HasTrait(girl, "Deep Throat") || g_Girls.HasTrait(girl, "No Gag Reflex")) && g_Dice.percent(5))
	{
		ss << "Some customers were having a speed drinking contest and challenged " << girlName << " to take part./n";
		if (girl->is_pregnant()) ss << "She refused for the sake of her unborn child.";
		else
		{
			ss << "Her talent at getting things down her throat meant she could pour the drink straight down. She won easily, earning quite a bit of gold.";
			wages += 30;
		}
	}

	if (g_Girls.HasItemJ(girl, "Golden Pendant") != -1 && g_Dice.percent(10))//zzzzz FIXME need more CRAZY
	{
		ss << "A patron complimented her gold necklace your not sure if it was an actual compliment or ";
		if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
			|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
			|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
		{
			ss << "an excuse to stare at her ampale cleavage.";
		}
		else
		{
			ss << "an attempt to get a discount on their bill.";
		}
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);//girls like compliments
	}

	if (wages < 0)
		wages = 0;


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

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, work);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_WAIT, Day0Night1);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else
		g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKBAR, "Dealing with customers at the bar and talking with them about their problems has made " + girlName + " more Charismatic.", Day0Night1);
	if (jobperformance < 100 && roll <= 2) { g_Girls.PossiblyGainNewTrait(girl, "Assassin", 10, ACTION_WORKBAR, girlName + "'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.", Day0Night1); }

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 40, ACTION_WORKBAR, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	return false;
}
double cJobManager::JP_Barmaid(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))			jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))			jobperformance += 10;  //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))					jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming")) 			jobperformance += 15;  //people like charming people
	if (g_Girls.HasTrait(girl, "Quick Learner"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))				jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Mixologist"))			jobperformance += 40;
	if (g_Girls.HasTrait(girl, "Dick-Sucking Lips"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Long Legs"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Natural Pheromones"))	jobperformance += 15;


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))			jobperformance -= 50;  //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy")) 				jobperformance -= 20;  //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))			jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))				jobperformance -= 30;  //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))			jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 40;  //bad idea let an alcoholic near booze
	if (g_Girls.HasTrait(girl, "Social Drinker"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Bimbo"))				jobperformance -= 5;

	if (g_Girls.HasTrait(girl, "One Arm"))				jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "One Foot"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "One Hand"))				jobperformance -= 15; 
	if (g_Girls.HasTrait(girl, "One Leg"))				jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "No Arms"))				jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "No Feet"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "No Hands"))				jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "No Legs"))				jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Blind"))				jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Deaf"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Retarded"))				jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))				jobperformance -= 10;//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Cum Addict"))			jobperformance -= 5;


	return jobperformance;
}
