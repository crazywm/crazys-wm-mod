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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cArena.h"


extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cArenaManager g_Arena;
extern cInventory g_InvManager;

#pragma endregion

// `J` Job arena - Staff
bool cJobManager::WorkCobbler(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMAKEITEMS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked making shoes and other leather items at the arena.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Cobbler(girl, false);
	double craftpoints = jobperformance/10;

	int dirtyloss = brothel->m_Filthiness / 10;		// craftpoints lost due to repairing equipment
	if (dirtyloss > 0)
	{
		craftpoints -= dirtyloss * 2;
		brothel->m_Filthiness -= dirtyloss;
		ss << "She spent some of her time repairing the Arena's equipment instead of making new stuff.\n";
	}

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		craftpoints *= 1.2; roll_a += 10; roll_b += 25;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		craftpoints *= 1.1; roll_a += 5; roll_b += 18;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.";
		roll_a += 2; roll_b += 10;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.";
		craftpoints *= 0.8;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		craftpoints *= 0.6; roll_a -= 2; roll_b -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 10; craftpoints *= 0.4; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	int tired = (300 - (int)jobperformance);	// this gets divided in roll_a by (10, 12 or 14) so it will end up around 0-23 tired
	if (roll_a <= 10)
	{
		tired /= 10;
		enjoy -= g_Dice % 3;
		if (roll_b < 20)	// injury
		{
			girl->health(-(1 + g_Dice % 5));
			craftpoints *= 0.8;
			if (girl->magic() > 50 && girl->mana() > 20)
			{
				girl->mana(-10 - (g_Dice % 10));
				ss << "While trying to enchant an item, the magic rebounded on her";
			}
			else
				ss << "She hurt herself while making items";
			if (girl->is_dead())
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident while working as a Cobbler at the Arena.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}

		else	// unhappy
		{
			ss << "She did not like";
			if (g_Dice % 2)	ss << " making shoes today.";
			else ss << " working with animal hides today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 14;
		craftpoints *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace with her neddle work by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 12;
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Money					//


	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		craftpoints *= 0.9;
		wages = 0;
	}
	else
	{
		wages += int(craftpoints); // `J` Pay her based on how much she made
	}

#pragma endregion
#pragma region	//	Create Items				//


	// `J` Arena Bookmark - adding in items that can be created in the Arena
	if (craftpoints > 0)
	{
		// `J` Incomplete Craftable code - commenting out
#if 0
		ss << g_InvManager.CraftItem(girl, JOB_COBBLER, int(craftpoints));
#else
		int numitems = 0, tries = 0;
		sInventoryItem* item = NULL;

		while (craftpoints > 0 && numitems < (1 + girl->crafting() / 15) && tries < 20)
		{

			string itemmade = "";
			int Magic = 0, Cost = 0, NumMade = 1;

			if (craftpoints < 20)						// Simple items
			{
				switch (g_Dice % 6)
				{
				case 0:
					if (girl->mana() >= 10)
					{
						break;
					}
				case 1:
					if (girl->mana() >= 5)
					{
						break;
					}
				case 2:
					if (girl->combat() > 10)
					{
						break;
					}
				case 3:
					if (girl->combat() > 10)
					{

						break;
					}
				case 4:

					break;
				default:

					break;
				}
			}
			else if (craftpoints < 50)					// small common items
			{
			}
			else if (craftpoints < 100)					// medium items
			{
			}
			else if (craftpoints < 150)					// large items
			{
			}
			else if (craftpoints < 200)					// difficult to make items
			{
			}
			else if (craftpoints < 250)					// rare and difficult to make items
			{
			}
			else if (craftpoints >= 250)				// unique items
			{
			}
			else										// unsorted items
			{
				Magic = 0;		Cost = 0;		itemmade = "Barbarian Boots";//                 Desc = "Knee-high fur boots. Perfect for wandering around the northern lands, but not so good at social gatherings. (+1 Charisma, +3 Spirit, +3 Bestiality, +5 Con, -5 Refinement, removes Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Black Leather Underwear";//         Desc = "The thick leather that comprises this bra and panties makes them somewhat uncomfortable to wear. The interior is intentionally rough and slightly abrasive. While quite unpleasant, they don't cause any actual harm; however, they make the wearer more capable of tolerating discomfort. They also serve as a reminder of the girl's status. (+20 BDSM, +10 Obedience, +5 Libedo)"
				Magic = 0;		Cost = 0;		itemmade = "Bullwhip";//                        Desc = "This tough leather whip, originally used on oxen, is more than capable of causing severely painful wounds on humans, though it is awkward and difficult to use in actual combat. Some find it of more practical use in the bedroom... (C) (+5 Comb, +10 BDSM, +3 AnimalH, +2 Farm, +5 Conf, -5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Bunny Slippers";//                  Desc = "These pink bunny-ear slippers can give any girl a cute, if slightly silly, air. (D) (+10 Hap, -5 Tired, -10 Fear, -5 Cha, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Cat O' Nine Tails";//               Desc = "The nine lashes of this dreadful torture implement, while difficult to use on unwilling and unrestrained victims, are still capable of instilling fear at a single glance. (B) (+10 Combat, +15 BDSM, +5 Conf, -5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Chap Boots";//                      Desc = "Black chap boots made of leather. With these, you'll be just like a real ranch hand. Or just a person wearing ranch hand boots. (+3 Cha, +2 Bea, +7 BDSM, +5 Strip, -10 Agi, -10 Com, +7 Conf, +7 Spi, +2 Con, +5 Farming, +3 Animal Handling)"
				Magic = 0;		Cost = 0;		itemmade = "Cowgirl Boots";//                   Desc = "Knee-high cowgirl boots. (+3 Charisma, +8 Spirit, +2 Con, +10 Farming, +7 AnimalHandling)"
				Magic = 0;		Cost = 0;		itemmade = "Dancer Slippers";//                 Desc = "These custom-made silk slippers with supple leather soles are often worn by female dancers while performing. They come with several colorful ribbons that can be tied to the shins and lower legs. (B) (+15 Strip, +5 Cha, +5 Agi, +5 Happ, +Fleet of Foot)"
				Magic = 0;		Cost = 0;		itemmade = "Designer Pumps";//                  Desc = "These fancy shoes are the height of fashion... for some reason. Maybe women just enjoy knowing that they walk around in a small fortune. (A) (+15 Hap, +15 Cha, +8 Refinement, +5 Love)"
				Magic = 0;		Cost = 10;		itemmade = "FlipFlops";//                       Desc = "Beach slippers might not look too great, but they're only meant to keep your feet free from the hot sand and prevent cuts from the sharper sea shells. Throw-away quality made from recycled materials. (Cha -2, Bea -3, HAP +2, Spir +3)"
				Magic = 0;		Cost = 0;		itemmade = "Fur Slippers";//                    Desc = "These soft fur slippers are built to be sexy and arousing, but they're still pretty comfortable. (C) (+5 Hap, +6 Cha, +6 Lib)"
				Magic = 0;		Cost = 0;		itemmade = "Heelless Heels";//                  Desc = "The doo doo doo, the GaGaGa. (B) (Cha +2, Bea +3, Lib +25, Conf +10, Fam +10, Int -10, Comb -30, Perf +5, removes Agile, adds Clumsy, Retarded, Great Arse, Long Legs, Fallen Goddes)."
				Magic = 0;		Cost = 0;		itemmade = "High Heels";//                      Desc = "The classic feminine footwear. They're hard to walk in, awful to run in and even worse to fight in, but that doesn't seem to diminish their popularity. (B) (+10 Hap, +10 Cha, -10 Agi, -20 Com)"
				Magic = 0;		Cost = 0;		itemmade = "Hime Shoes";//                      Desc = "Beautiful high heels that no outfit is complete without. (Removes Clumsy and Aggressive. Adds Long Legs.)"
				Magic = 0;		Cost = 0;		itemmade = "Hot Stilettos";//                   Desc = "These colorful high-heeled platform shoes are, in their own special way, kind of hot. They are also, in their own special way, the opposite of classy. (E) (+5 Cha, -5 Hap, -5 Int, -15 Refinement, removes Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Knee Pads";//                       Desc = "These offer moderate protection for a girl's knees. This can come in handy on the battlefield or in the bedroom. (Removes Elegant, +10 Service, +5 Combat, Constitution, Oral and Bestiality)"
				Magic = 0;		Cost = 0;		itemmade = "Kung Fu Shoes";//                   Desc = "These slim, lightweight cloth shoes are ideal for practicing martial arts, and are pretty dang comfortable everyday wear too! (B) (+5 Cha, +5 Spi, +15 Agi, +4 Comb)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Armor";//                   Desc = "This suit of boiled leather plates isn't as resistant as metal armor, but it's light and inexpensive. A favorite of militiamen everywhere. (-10 Magic,  +5 Cons, Conf,  -5 Agl, Fear, Cha)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Cap";//                     Desc = "This might make a fighter look like a dork, but a glancing blow will no longer knock that fighter unconcious either. (adds Brawler, Nerd)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Catsuit";//                 Desc = "The favourite of spies and mistresses. (+5 Comb, +2 Cons, +5 Lib, +2 Spi, +7 Cha, +5 Agi +10 BDSM, adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Chaps";//                   Desc = "These black leather chaps are good for farm work and somewhat sexy, but not adequate for social gatherings. (+3 Charisma, -10 Refinement, +10 Farming, +10 AnimalHandling, +5 Con)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Collar";//                  Desc = "Basic collar for slaves. (+1 BDSM, +5 Obed)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Harness";//                 Desc = "Tight leather underclothing with a crotch rope. (+1 Cha, +5 Con, +15 Lib, -10 Ref, -15 Happiness, Conf and Spirit, +15 Obedience, Fear and Hate, +15 BDSM, +12 Best, Group, +8 other sex skills except Strip)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Lingerie";//                Desc = "This buckled black leather bustier and matching panties bristle with studs and spikes. It's the ideal outfit for any lover, whether you call her &quot;mistress&quot; or &quot;pet&quot;. (B) (+3 Cha, +10 Lib, +25 BDSM, +10 Anal, Best and Group, +5 other sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Opera Glove";//             Desc = "Elbow long leather glove. Does not turn her into an expert singer, no matter what she says. (+7 Charisma, +5 Refinement, +2 Strip, +3 BDSM, +2 Perf)"
				Magic = 0;		Cost = 0;		itemmade = "Leather Pads";//                    Desc = "These small pads of hardened hide strapped over one's clothes provide a small amount of protection without impeding movement. (-5 Magic,  +3 Cons, Conf,  -2 Agl, Fear, Cha)"
				Magic = 0;		Cost = 0;		itemmade = "Low Heeled Pump";//                 Desc = "For the women who want to look sexy in a more refined manner. (B) (+5 HAP, +10 Cha, +5 Bea, +5 Lib, -5 Agi, -10 Com, adds Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Mage Slippers";//                   Desc = "These shoes are designed to be a true dream for any magic user. They're quite comfortable, and they slightly enhance the magic ability of the wearer. Better still, they're enchanted to appear like whatever footwear the owner desires, ensuring they'll always look good with anything. (+10 Cha, +5 Magic, +5 Agi, -5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magical High Heels";//              Desc = "Red high heel shoes with some mild magical powers, though they'll probably inhibit your movements in battle. (+5 Conf, +7 Cha, +2 BDSM, +3 Mag, +5 Agi, -20 Com)"
				Magic = 0;		Cost = 0;		itemmade = "Mistress Boots";//                  Desc = "These thigh-high leather boots boast a tall and particularly sharp heel. For some, being stepped on by a woman wearing these would be a nightmare. But for others... (B) (+10 Cha, +15 BDSM, -10 Agi, -10 Com, +5 Conf, +10 Spi)"
				Magic = 0;		Cost = 0;		itemmade = "Nightleather Boots";//              Desc = "An unusual alternative to traditional combat footwear, these boots are made of leather from a dangerous, magical beast. The boots are designed to look sexy and unimposing, while still remaining quite functional. An innate magical effect makes them morph and conform to practically any feet. Even though they have tall heels, their magic makes it easier for the wearer to move comfortably. (+2 Charisma, +5 Combat, +8 Agility, -Clumsy, -5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Plate Boots";//                     Desc = "These padded iron boots are part of a suit of plate armor. They're warm and durable, but not exactly sexy. (+5 Combat, +5 Cons, +5 Conf, -5 Fear, -5 Hate, -5 Cha, -10 Agi)"
				Magic = 0;		Cost = 0;		itemmade = "Platform Heels";//                  Desc = "High heels that also have a small platform. Good luck learning how to fight in these! (Sex +10, Strip +15, Combat -10. Adds Long Legs, Great Arse and Clumsy. Removes Assassin, Adventurer, and Elegant.)"
				Magic = 0;		Cost = 0;		itemmade = "Pony Boots";//                      Desc = "Thigh-high leather boots with metallic hooves and no heels. (+4 Cha, +2 Bea, +5 Con, +7 BDSM, +10 Best, +6 Obed, -15 Agi, -15 Com, -7 Spi, +2 Hate, adds Long Legs)"
				Magic = 0;		Cost = 0;		itemmade = "Riding Crop";//                     Desc = "This short whip, commonly used to spur horses, causes a sharp slapping pain but rarely any actual damage. (D) (+5 BDSM, +5 AnimalH, +2 Conf, -2 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Sandals of Mercury";//              Desc = "These cute gilded sandals each bear a small wing at the heel. The rumors that they let you fly have proven false, but they do let you run exceptionally fast. (S) (+10 Cha, +10 Hap, +25 Agi, adds Fleet of Foot)"
				Magic = 0;		Cost = 0;		itemmade = "Shoes of Icarus";//                 Desc = "These winged shoes not only look cool, they also grant the wearer a nice set of boosts too. The girl is sure to love them a lot. (+fleet of foot, +25 agility, +15 confidence, -15 fear/hate/love, +25 happiness, +10 constitution, +25 spirit, -50 tiredness"
				Magic = 0;		Cost = 0;		itemmade = "Six Inch Heels";//                  Desc = "Very trashy, but also very sexy. Hard to walk in. (Adds Clumsy, Long Legs, Great Arse, Sexy Air. Removes Elegant and Fleet of Foot. +10 Strip, +5 Sex -10 Combat)"
				Magic = 0;		Cost = 0;		itemmade = "Stripper Boots";//                  Desc = "Thigh-high platform boots that boast extremely tall heels. They aren't exactly comfortable but girls look damn sexy on them. (+7 Cha, +2 Bea, +2 BDSM, +7 Conf, +10 Strip, +5 Perf, -15 Agi, -10 Com, removes Fleet of Foot and adds Long Legs)"
				Magic = 0;		Cost = 0;		itemmade = "Stripper Shoes";//                  Desc = "Platform shoes that boast extremely tall heels. (+5 Cha, +10 Strip, +3 Perf, +7 Conf, -15 Agi, -20 Com, removes Fleet of Foot and adds Long Legs)"
				Magic = 0;		Cost = 0;		itemmade = "Thigh High Boots";//                Desc = "Leather boots that go up to a girl's thigh. (+10 Charisma, +5 Beauty and Confidence)"
				Magic = 0;		Cost = 0;		itemmade = "Wool Slippers";//                   Desc = "Warm, comfortable woolen slippers. They're not much to look at, but as far as nice places to put your feet in go, they're hard to beat. (D) (+5 Hap, +5 Love, -5 Tired, -5 Fear, -5 Hate)"

			}

			if (itemmade == "")	{ Magic = 0; Cost = 10; itemmade == "FlipFlops"; }//            Desc = "Beach slippers might not look too great, but they're only meant to keep your feet free from the hot sand and prevent cuts from the sharper sea shells. Throw-away quality made from recycled materials. (Cha -2, Bea -3, HAP +2, Spir +3)"
			item = g_InvManager.GetItem(itemmade);
			if (item)
			{
				craftpoints -= Cost;
				girl->mana(-Magic);
				msgtype = EVENT_GOODNEWS;
				if (numitems == 0)	ss << "\n \n" << girlName << " made:";
				ss << "\n" << itemmade;
				g_Brothels.AddItemToInventory(item);
				numitems++;
			}
			else
			{
				craftpoints -= 10;
			}
			tries++;
		}
#endif
	}

#pragma endregion
#pragma region	//	Finish the shift			//


	// `J` - Finish the shift - Cobbler

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);
	if (tired > 0) girl->tiredness(tired);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido,false);

	// primary improvement (+2 for single or +1 for multiple)
	girl->upd_skill(SKILL_CRAFTING,	(g_Dice % skill) + 1);
	girl->upd_stat( STAT_STRENGTH, (g_Dice % skill) + 1);
	// secondary improvement (-1 for one then -2 for others)
	girl->upd_stat( STAT_CONSTITUTION,	max(0, (g_Dice % skill) - 1));
	girl->upd_skill(SKILL_COMBAT, max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_Cobbler(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->crafting() +
		// secondary - second 100
		((girl->service() + girl->intelligence() + girl->magic()) / 3) +
		// level bonus
		girl->level();
		// traits modifiers
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	return jobperformance;
}
