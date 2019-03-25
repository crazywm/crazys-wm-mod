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

// `J` Job Arena - Staff
bool cJobManager::WorkBlacksmith(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	ss << " worked as a blacksmith at the arena.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 40, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Blacksmith(girl, false);
	double craftpoints = jobperformance;

	int dirtyloss = brothel->m_Filthiness / 20;		// craftpoints lost due to repairing equipment
	if (dirtyloss > 0)
	{
		craftpoints -= dirtyloss * 2;
		brothel->m_Filthiness -= dirtyloss * 10;
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
		wages -= 10; craftpoints *= 0.6; roll_a -= 2; roll_b -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 25; craftpoints *= 0.4; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	int tired = (300 - (int)jobperformance);	// this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
	if (roll_a <= 10)
	{
		tired /= 8;
		enjoy -= g_Dice % 3;
		if (roll_b < 10)	// fire
		{
			int fire = max(0, g_Dice.bell(-2, 10));
			brothel->m_Filthiness += fire * 2;
			craftpoints -= (craftpoints * (fire * 0.1));
			if (girl->pcfear() > 20) girl->pcfear(fire / 2);	// she is afraid you will get mad at her
			ss << "She accidently started a fire";
			/* */if (fire < 3)	ss << " but it was quickly put out.";
			else if (fire < 6)	ss << " that destroyed several racks of equipment.";
			else if (fire < 10)	ss << " that destroyed most of the equipment she had made.";
			else /*          */	ss << " destroying everything she had made.";

			if (fire > 5) g_MessageQue.AddToQue(girlName + " accidently started a large fire while working as a Blacksmith at the Arena.", COLOR_RED);
		}
		else if (roll_b < 30)	// injury
		{
			girl->health(-(1 + g_Dice % 5));
			craftpoints *= 0.8;
			if (girl->magic() > 50 && girl->mana() > 20)
			{
				girl->mana(-10 - (g_Dice % 10));
				ss << "While trying to enchant an item, the magic rebounded on her";
			}
			else
				ss << "She burnt herself in the heat of the forge";
			if (girl->is_dead())
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident while working as a Blacksmith at the Arena.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}

		else	// unhappy
		{
			ss << "She did not like working in the arena today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 12;
		craftpoints *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 10;
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
		ss << g_InvManager.CraftItem(girl, JOB_BLACKSMITH, int(craftpoints));
#else
		sInventoryItem* item = NULL;
		int numitems = 0, tries = 0;

		while (craftpoints > 0 && numitems < (1 + girl->crafting() / 15) && tries < 20)
		{

			string itemmade = "";
			int Magic = 0, Cost = 0, NumMade = 1;

			if (craftpoints < 20)						// Simple items
			{
				switch (g_Dice % 6)
				{
				case 0:
					if (girl->magic() >= 10)
					{
						Magic = 10;		Cost = 5;		itemmade = "Lumps of Coal";//                 Desc = "These strange pieces of coal come from Mareth and when inhaled, brings people to heat or rut depending on their gender. Futanaris beware! (Max Lib, +Nymphomaniac/Aggressive)";
						break;
					}
				case 1:
					if (girl->magic() >= 5)
					{
						Magic = 5;		Cost = 10;		itemmade = "Bowl of Nails";//                 Desc = "Is your girl tough enough to eat this without any milk? (Health -10, Constitution +5)";
						break;
					}
				case 2:
					if (girl->combat() > 10)
					{
						Magic = 0;		Cost = 10;		itemmade = "Tonfa";//                         Desc = "A weapon from a far away land. It looks somewhat like a police baton with a handle sticking out of the side. (+2 Con, +5 Com, -10 Fear, -5 Hate, +2 Conf)";
						break;
					}
				case 3: case 4:
					Magic = 0;		Cost = 10;			itemmade = "Dagger";//                        Desc = "This sturdy iron dagger isn't fancy but it gets the job done. (C) (+5 Com, -10 Fear, -5 Hate, +5 Conf)";
					break;
				default:
					NumMade = 1 + (int)(craftpoints / 5);
					Magic = 0;		Cost = NumMade * 5;	itemmade = "Shiv";//                          Desc = "This sturdy iron dagger isn't fancy but it gets the job done. (C) (+5 Com, -10 Fear, -5 Hate, +5 Conf)";
					break;
				}
			}
			else if (craftpoints < 50)					// small common items
			{
				switch (g_Dice % 4)
				{
				case 0:
					if (girl->magic() > 30)
					{
						Magic = 30;		Cost = 45;		itemmade = "Lesser Bracer of the Phantom";//  Desc = "A small bracer that makes your girl less real. (Adds half-construct)";
						break;
					}
				case 1:
					if (girl->combat() > 10)
					{
						Magic = 0;		Cost = 40;		itemmade = "Iron Mace";//                     Desc = "One-handed weapon with an iron head on the end of a wooden shaft, commonly used by clerics or bishops. (+10 Com, -2 Agi, -20 Fear, -10 Hate, +5 Conf, -10 Cha)";
						break;
					}
				case 2:
					if (girl->combat() > 10)
					{
						Magic = 0;		Cost = 25;		itemmade = "Short Sword";//                   Desc = "This robust stabbing sword is quite effective for its size. It can be concealed to some degree, but it's too large to completely camouflage. (B) (+15 Com, -10 Fear, -5 Hate, +5 Conf, -5 Cha)";
						break;
					}
				default:
					Magic = 0;		Cost = 40;		itemmade = "Stripper Pole";//                 Desc = "Installed in a girl's room, this shiny pole allows her to practice dancing during her spare time. (+10 Strip, +5 Cns/Str, +5 Agi)";
					girl->strip(g_Dice % 2);		// she may try it out
					break;
				}
			}
			else if (craftpoints < 100)					// medium items
			{
				switch (g_Dice % 2)
				{
				case 0:
					Magic = 0;		Cost = 60;		itemmade = "Long Sword";//                    Desc = "A perennial favorite of adventurers everywhere, this quality steel sword is simple in design but sturdy and effective. It might be unsettling to see a woman you're paying to have sex carrying this sort of weapon, however. (A) (+20 Com, -20 Fear, -10 Hate, +10 Conf, -10 Cha)";
					break;
				default:
					Magic = 0;		Cost = 0;		itemmade = "Chain Mail";//                    Desc = "A suit of chainmail armour, hot and heavy it still offers good protection. (-20 Magic, +15 Cons, Conf, -10 Agl, Fear, Cha) (Removes Fleet of Foot)";
					break;
				}
			}
			else if (craftpoints < 150)					// large items
			{
				switch (g_Dice % 2)
				{
				case 0:
					if (girl->magic() > 50)
					{
						Magic = 50;		Cost = 130;		itemmade = "Bracer of the Phantom";//         Desc = "A small bracer that makes your girl a lot less real. (Adds construct)";
						break;
					}
				default:
					Magic = 0;		Cost = 0;		itemmade = "Full Plate Armor";//              Desc = "This heavy suit of steel plate armor is the best battlefield protection money can buy. It is engineered for maximum mobility but is still somewhat cumbersome. (A) (-40 Magic, +30 Cons, Conf, -30 Agi, -20 Cha, Fear) (Removes Fleet of Foot and Agile, Adds Tough)";
					break;
				}
			}
			else if (craftpoints < 200)					// difficult to make items
			{
				switch (g_Dice % 6)
				{
				case 0:
					Magic = 30;		Cost = 150;		itemmade = "Claptrap";//                      Desc = "An annoying little robot that still manages to help more often than not. (Tiredness -10, Service +10)";
					break;
				case 1:
					Magic = 30;		Cost = 180;		itemmade = "Android, Assistance";//           Desc = "A robot helper for your girl. (Tiredness -10, Happiness +10, Service +15, Love +5)";
					break;
				case 2:
					Magic = 30;		Cost = 160;		itemmade = "Cybernetic Leg";//                Desc = "Replace your girl's leg with this robotic one. (+5 Agl/Str. Adds Half-construct, removes One Leg, One Foot and Missing Toe)";
					break;
				case 3:
					Magic = 30;		Cost = 160;		itemmade = "Cybernetic Eyes";//               Desc = "Replace your girl's eyes with these robotic one. (Adds Strange Eyes. Removes One Eye and Eye Patch)";
					break;
				case 4:
					Magic = 30;		Cost = 160;		itemmade = "Cybernetic Arm";//                Desc = "Replace your girl's arm with this robotic one. (+10 Strength, +5 combat. Adds Half-construct, removes One Arm, One Hand, Missing Finger and Missing Fingers)";
					break;
				default:
					Magic = 10;		Cost = 160;		itemmade = "Elven Tiara";//                   Desc = "This mithril tiara is a proof of the mastery of elven blacksmiths and jewelcrafters. (+10 Charisma, +15 Refinement, +5 Magic)";
				}
			}
			else if (craftpoints < 250)					// rare and difficult to make items
			{
				switch (g_Dice % 7)
				{
				case 0:
					Magic = 50;		Cost = 240;		itemmade = "Android, Sex MK III";//           Desc = "An android that can train your girl for you. This one can switch between male and female, giving your girl a lot of experience! (Sex +10, Tiredness +10, Lesbian +10, Experience +10.)";
					break;
				case 1:
					Magic = 50;		Cost = 220;		itemmade = "Android, Sex MK II";//            Desc = "A female android that can train your girl for you. (Lesbian +10, Tiredness +10. Adds Lesbian)";
					break;
				case 2:
					Magic = 50;		Cost = 200;		itemmade = "Android, Sex MK I";//             Desc = "A male android that can train your girl for you. Unfortunately, this one can be very overzealous. (Sex +10, Tiredness +10, Constitution -5)";
					break;
				case 3:
					Magic = 80;		Cost = 240;		itemmade = "Android, Combat MK II";//         Desc = "A robot to help train your girl in her fighting ability. This one can help with magic training, too. (Combat +10, Experience +15, Magic +10 Tiredness +20)";
					break;
				case 4:
					Magic = 50;		Cost = 200;		itemmade = "Android, Combat MK I";//          Desc = "A robot to help train your girl in her fighting ability. (Combat +10, Experience +10, Tiredness +15)";
					break;
				case 5:
					Magic = 40;		Cost = 230;		itemmade = "Bracer of Toughness";//           Desc = "The potent enchantment placed on this discreet leather bracelet provides the wearer with vastly improved endurance, making them more resistant to physical effort and exertion. (+40 Cons, +10 BDSM, +5 Group, +5 Best)";
					break;
				default:
					Magic = 50;		Cost = 220;		itemmade = "Deathtrap";//                     Desc = "A large combat robot for security. (Combat +15, Fear -10. Adds Fearless.)";
				}
			}
			else if (craftpoints >= 250)				// unique items
			{
				switch (g_Dice % 2)
				{
				case 0:
					Magic = 10;		Cost = 250;		itemmade = "Epic Bracer of Seduction";//      Desc = "This rare silver bracer will make any girl into a magnificent woman who will take all you want to give her and then beg for more. (+50 Cons, Cha, Bea, Lib and Conf, removes Lolita, adds Fast Orgasms, Nymphomaniac, Great Figure, Great Arse, Long Legs, Puffy Nipples, Perky Nipples, Big Boobs, Tough)";
					break;
				default:
					Magic = 10;		Cost = 250;		itemmade = "Epic Bracer of Babeosity";//      Desc = "A serious refinement of the initial design. (Removes Lolita, Small Boobs, Slow Orgasms. Adds Fast Orgasms, Nymphomaniac, Great Figure, Great Arse, Long Legs, Puffy Nipples, Perky Nipples, Abnormally Large Boobs, +50 cha, beauty, libido, confidence, constitution)";
				}
			}
			else										// unsorted items
			{

				Magic = 10;		Cost = 30;		itemmade = "Power Limiter";//                 Desc = "Wrist cuffs that remove pretty much any strength or power a girl has. (Removes Strong, Strong Magic, Psychic, Aggressive, Assassin. -10 Happy/Love/Str, +5 fear/hate)";
				Magic = 10;		Cost = 30;		itemmade = "Chains of Bondage";//             Desc = "Enslave the body to liberate the mind (+50 cha +100 con/libido/obedience +30 sex skills -lots service/strip/magic/combat +many effects)";
				Magic = 10;		Cost = 30;		itemmade = "Bracer of Seduction";//           Desc = "This flashy brass armband can make any woman into a gorgeous babe, and it enhances her sex drive to boot. Created as a prank by freshman mages, it caught on in certain circles. (Removes Lolita, adds Fast Orgasms, Nymphomaniac, Great Figure, Great Arse, Long Legs, Puffy Nipples, Perky Nipples, Big Boobs)";
				Magic = 10;		Cost = 30;		itemmade = "Bracelet of Refreshing Desire";// Desc = "This subtle, metallic band can either be placed or locked around a girl's wrist, ankle, or even a tail. While it won't actually increase the strength of her desire, it significantly increases the frequency when desire is felt. The wearer will recover from sex very quickly and will find she wants it far more often. Unfortunately being so sexually-focused makes her unfocused and less resilient. (+Nymphomaniac, +40 Libedo, -20 Intelligence, -20 Constitution)";
				Magic = 10;		Cost = 20;		itemmade = "Shackles of the Mundane";//       Desc = "This pair of cold iron bracelets completely suppresses its wearer's magical ability. The experience is downright traumatic to mages, but even the dullest peasant will suffer from being cut off from the arcane forces of the universe. (-100 Mag, -10 Conf/Str, -40 Spi, -75 Hap, -5 Cha, -5 Lib, +75 Hate, adds Pessimist, removes Strong Magic and Optimist)";
				Magic = 10;		Cost = 20;		itemmade = "Magic Ankle Cuffs";//             Desc = "These adamantine cuffs lock around a girl's ankles to impede movement. When worn, they create magical chains and weights. The weights change in mass depending on how quickly the girl tries to move. If she moves slowly, they can be dragged along with only moderate effort. Walking normally requires significant work, and attempting to move quickly causes the weights to be too heavy to pull at all. (+60 Obed, +25 Con, -50 Agi, -20 Str, -Fleet of Foot, +15 Fear/Hate)";
				Magic = 10;		Cost = 20;		itemmade = "Gauntlet de Sade";//              Desc = "This long, black leather gauntlet locks tightly onto the wrist and hand. It creates sadistic desires in the wearer, and the gauntlet can actually absorb power from the physical suffering of other people -- especially women. Normal sexual desire will be reduced, but the wearer will feel intense pleasure and even orgasms from inflicting pain. However, most wearers will still maintain their sense of compassion and try to avoid causing permanent harm. (+Sadistic, +50 BDSM, -50 Normal Sex)";
				Magic = 10;		Cost = 10;		itemmade = "Feline Bracer";//                 Desc = "This furry bracer gives the wearer a somewhat feline mindset, increasing their agility and their natural curiosity and cleverness. It may also cause slightly animalistic desires at times. (+10 Agi, +5 Int, +10 Bestiality)";
				Magic = 0;		Cost = 20;		itemmade = "Plated Gauntlet";//               Desc = "This padded iron gauntlet is part of a suit of plate armor. It's warm and durable, but not exactly sexy. (+3 Combat, -5 Mag, +2 Cons, +2 Conf, -5 Fear, -5 Hate, -5 Cha)";


				Magic = 10;		Cost = 50;		itemmade = "Power Armor";//                   Desc = "A full suit of powered armor from the future. (+50 Combat, +30 Constitution, +40 Strength, -40 Magic, -10 Agility. Adds Manly.)";
				Magic = 10;		Cost = 30;		itemmade = "Dazzling Armor";//                Desc = "This armor is designed to augment the attractiveness of the female form for advantage in combat. Physically, it provides reasonable protection while allowing agile movement and unimpeded use of magic. Additionally, it makes the wearer seem more graceful and almost hypnotic in combat, regardless of the gender or species of the opponent. It imparts a slight improvement to tactical understanding, as well. (+10 Combat, +8 Int, +5 Cha/Agi, -10 Fear)";
				Magic = 0;		Cost = 50;		itemmade = "Plate Mail";//                    Desc = "A well made suit of steel Plate Mail, despite it's apperance it is suprisingly flexible and is excedingly tough, however it does heavily impede magical abilities. (-30 Magic, +20 Cons, Conf, -20 Agl, -15 Fear, Cha) (Removes Fleet of Foot, Adds Tough)";
				Magic = 0;		Cost = 0;		itemmade = "Hero Armor";//                    Desc = "The armor of a true hero! (Con, Int, Conf, Fame, Spirit, Health +10) (Adds Adventurer)";
				Magic = 0;		Cost = 0;		itemmade = "Armor of the Battlemaiden";//     Desc = "This suit of steel plate armor has been modified to expose small areas of a female wearer's anatomy, including cleavage, navel and thighs. The protection it offers is lessened, of course, but some say the distraction effect is worth it. (A) (+15 Comb/Cons/Conf/Str, +15 Strip, +8 BDSM, -10 Fear, -10 Hate, -15 Agi, -5 Cha, -20 Mag)";
				Magic = 0;		Cost = 0;		itemmade = "Chainmail Bikini";//              Desc = "This set of chainmail bra and panties doesn't offer much protection at all, but some would argue it is irrelevant to their appeal. (E) (+5 Comb, +5 Cons, +8 Cha, +5 BDSM, +10 Best, -5 Int, -5 Mag, removes Elegant)";



				Magic = 10;		Cost = 30;		itemmade = "Chaos Knife";//                   Desc = "A small blade used by mad-driven beings. Careful you don't try to hurt people with it....oh who am I kidding LLIK OG, HSELF TAE. (Com 15, Int -20, Con 10, Health 55, Mag 20, Perf 10, +Open Minded, Small Scars, Strong Magic, Twisted, -Mind Fucked, Broken Will)";
				Magic = 10;		Cost = 20;		itemmade = "Sword of Air";//                  Desc = "This sword is extraordinarily light and easy to handle. The material that composes it is both strong and translucent, providing an elegant look. When equipped or even holstered, the wielder becomes more graceful in their movements. (+15 Combat, +15 Agility, -10 Fear)";




				Magic = 0;		Cost = 20;		itemmade = "Ornate Gladius";//                Desc = "A preferred weapon of wealthy merchants, the blade of this short stabbing sword is engraved with exquisite depictions of charging animals and its hilt is of flawless ivory. (B) (+5 Cha, +15 Com, -10 Fear, -5 Hate, +5 Conf)";
				Magic = 0;		Cost = 20;		itemmade = "Ornamental Heater";//             Desc = "With your brothels logo painted on in bright colors, it's almost a shame if the girls get this damaged in battle. (Cha +15, Cons/Conf +5, adds Tough)";
				Magic = 0;		Cost = 20;		itemmade = "Kite Shield";//                   Desc = "Since your enemies cant see your girls Great Figure behind this shield, you hired an artist to paint her on the front in a lewd position, to distract her enemies. (Beau/Cons +5, adds Half-Construct)";
				Magic = 0;		Cost = 20;		itemmade = "Ironclad Buckler";//              Desc = "A rather small shield, but quite sturdy. (adds Tough)";

				Magic = 0;		Cost = 20;		itemmade = "Iron Helmet";//                   Desc = "Standard army issue helmet, most of the city guards wear one of these. (Cons +5, adds Tough.)";
				Magic = 0;		Cost = 0;		itemmade = "Steel Helmet";//                  Desc = "If steel wasn't in such short supply, this would be part of the regular outfit of any army. (Cons +15, adds Tough.)";
				Magic = 0;		Cost = 0;		itemmade = "Pot Helmet";//                    Desc = "There's some of last night's stew left inside. (Cha -10, Cons +10, adds Brawler.)";
				Magic = 0;		Cost = 0;		itemmade = "Plated Helmet";//                 Desc = "This padded iron helmet is part of a suit of plate armor. Good for protecting the brains. (+3 Combat, -5 Mag, +2 Cons, +2 Conf, -5 Fear, -5 Hate, -5 Cha)";
				Magic = 0;		Cost = 0;		itemmade = "Gold Helmet";//                   Desc = "It's more to present the wearers' wealth than to protect his head (Cha +10, Cons +5, adds Tough.)";
				Magic = 0;		Cost = 0;		itemmade = "Crescent Moon Helmet";//          Desc = "Samurai helmet with decoration in the shape of a crescent moon. (+5 Combat, -5 Mag, +3 Cons, +3 Conf, -5 Fear, -5 Hate, +3 Cha)";
				Magic = 0;		Cost = 0;		itemmade = "Adamantium Helmet";//             Desc = "Very rare and much sought after head protection. Even rarer and more expensive matching Adamantium Suit Of Armor not included! (Cha/Bea +15, Cons +25, adds Tough.)";


				Magic = 0;		Cost = 0;		itemmade = "Scutum";//                        Desc = "A large, but surprisingly light shield. However, it does take considerable skill to use properly in one on one combat. (Cons/Conf +10, Agi -15, adds Construct)";
				Magic = 0;		Cost = 0;		itemmade = "Safe by Marcus";//                Desc = "A safe endorsed by famous arms dealer Marcus. If he trusts it to keep your valuables safe, then anyone would. (Happiness +3, Fear -2)";
				Magic = 0;		Cost = 0;		itemmade = "Titanium handcuffs";//            Desc = "(+50 libido/obedience, +30 con, +40 BDSM/group/best, -10 Str, -lots service/strip/magic/combat)";

				Magic = 0;		Cost = 0;		itemmade = "Submachine Gun";//                Desc = "A fast and weak firearm. (+10 Combat, +10 Agility, -5 Charisma, adds Fleet of Foot)";
				Magic = 0;		Cost = 0;		itemmade = "SnS Combat Rifle";//              Desc = "A combat rifle lost in the catacombs made by the now defunct SnS Munitions. Its bullets contain a potent toxin. (Constitution +10, Combat +10, Magic +10)";
				Magic = 0;		Cost = 0;		itemmade = "Revolver";//                      Desc = "A small but powerful firearm (+6 Combat)";
				Magic = 0;		Cost = 0;		itemmade = "Bolt-action Rifle";//             Desc = "A slow and powerful firearm. (+15 Combat, -5 Agility -5 Charisma,)";
				Magic = 0;		Cost = 0;		itemmade = "Derringer";//                     Desc = "A small, disposable firearm. (+6 Combat, temporary)";
				Magic = 0;		Cost = 0;		itemmade = "Atlas Combat Rifle";//            Desc = "A combat rifle lost in the catacombs made by the now defunct Atlas Corporation. Its bullets heal the user. (Constitution +10, Combat +20)";
				Magic = 0;		Cost = 0;		itemmade = "Assault Rifle";//                 Desc = "A good all around firearm. (+15 Combat -5 Charisma,)";
				Magic = 0;		Cost = 0;		itemmade = "Engraved Revolver";//             Desc = "A beutifully decorated revolver filled with love from its previous owner. Found still on his corpse in the catacombs. (+40 Combat, +10 Charisma, Love, -10 Fear. Adds Fleet of Foot.)";

				Magic = 0;		Cost = 0;		itemmade = "Chain Leash";//                   Desc = "This metallic chain attached to a leather collar isn't just an effective physical restraint: it's an humiliating punishment. A woman wearing this is at the complete mercy of whoever is holding it. (+15 BDSM, +5 Best and Oral, +20 Obed, -25 Spi, -25 Conf, -30 Refinement, -20 Comb, -30 Agi, -35 Happ, +10 Hate, +15 Fear, removes Elegant)";
				Magic = 0;		Cost = 0;		itemmade = "Great Hammer";//                  Desc = "A large hammer. It might be unsettling to see a woman you're paying to have sex carrying this sort of weapon, however. (+15 Com, -20 Fear, -10 Hate, +5 Conf, -10 Cha, -10 Agi, removes Agile)";
				Magic = 0;		Cost = 0;		itemmade = "Replica Sword";//                 Desc = "This imitation of some legendary sword or other is basically no good in a fight and bears little resemblance to a weapon one would actually wield in battle. If you're into that sort of thing, it looks kind of cool. If you're not, it looks kind of silly. (E) (+2 Com, +10 Hap, -2 Cha, adds Nerd)";
				Magic = 0;		Cost = 0;		itemmade = "Chain Whip";//                    Desc = "This product is the result of an out-of-control BDSM enthusiast trying to combine the best aspects of his chosen hobby. (+2 BDSM, +7 Com, -10 Fear, -5 Hate, +2 Conf, -5 Cha)";
				Magic = 0;		Cost = 0;		itemmade = "Ceremonial Sabre";//              Desc = "More a work of art than a weapon, this magnificent sabre is heavy with gems and gold inlays. It could almost pass for jewelry, but it's still sharp and deadly. (A) (+5 Cha, +20 Com, -20 Fear, -10 Hate, +10 Conf)";
				Magic = 0;		Cost = 0;		itemmade = "Bodice Knife";//                  Desc = "A small knife designed to be hidden in between a woman's breasts. (-5 Fear, +5 Combat)";
				Magic = 0;		Cost = 0;		itemmade = "Blue Steel Fan";//                Desc = "Kitana's favourite. Some people discount the validity of fans in battle; the victims of Kitana's fatalities are not among them. (+2 Cha, +5 Com, +3 Perf, +5 Refinement, -10 Fear, -5 Hate, +2 Conf, adds Elegant)";
				Magic = 0;		Cost = 0;		itemmade = "Battle Axe";//                    Desc = "A large axe. It might be unsettling to see a stripper carrying this sort of weapon, however. (+12 Com, -20 Fear, -10 Hate, +5 Conf, -10 Cha, -7 Agi)";
				Magic = 0;		Cost = 0;		itemmade = "Adamantine leg fetter";//         Desc = "It really interfers with her ability to move around. (+20 con +20 libido/obedience +10 group +20 best/BDSM/anal -lots service/strip/magic/combat/strength -fleet of foot)";
				Magic = 0;		Cost = 0;		itemmade = "Adamantine arm fetter";//         Desc = "This device restrains the girls arms. (+10 con +30 libido/obedience +20 BDSM/group/best/anal -lots service/strip/magic/combat -elegant)";

			}

			if (itemmade == "")	{ Magic = 0;		Cost = 10;		itemmade == "Shiv"; }
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
	girl->upd_temp_stat(STAT_LIBIDO, libido, false);

	// primary improvement (+2 for single or +1 for multiple)
	girl->upd_skill(SKILL_CRAFTING,	(g_Dice % skill) + 1);
	girl->upd_stat(STAT_STRENGTH, (g_Dice % skill) + 1);
	// secondary improvement (-1 for one then -2 for others)
	girl->upd_stat(STAT_CONSTITUTION,	max(0, (g_Dice % skill) - 1));
	girl->upd_skill(SKILL_COMBAT,		max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 50, actiontype, "Working in the heat of the forge has made " + girlName + " rather Tough.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Blacksmith(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->crafting() + girl->strength()) / 2) +
		// secondary - second 100
		((girl->constitution() + girl->intelligence() + girl->magic() + girl->combat()) / 4) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits


	return jobperformance;
}
