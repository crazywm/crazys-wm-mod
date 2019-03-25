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
bool cJobManager::WorkJeweler(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	ss << " worked as a Jeweler at the arena.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 40, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//


	double jobperformance = JP_Jeweler(girl, false);
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

			if (fire > 5) g_MessageQue.AddToQue(girlName + " accidently started a large fire while working as a Jeweler at the Arena.", COLOR_RED);
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
				g_MessageQue.AddToQue(girlName + " was killed in an accident while working as a Jeweler at the Arena.", COLOR_RED);
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
		ss << g_InvManager.CraftItem(girl, JOB_JEWELER, int(craftpoints));
#else
		int numitems = 0, tries = 0, random = 0;
		sInventoryItem* item = NULL;

		while (craftpoints > 0 && numitems < (1 + girl->crafting() / 15) && tries < 20)
		{
			string itemmade = "";
			int Magic = 0, Cost = 0, NumMade = 1;

			if (craftpoints < 20)						// Simple items
			{
				random = g_Dice % 10;
				switch (random)
				{
				case 0:		Cost = 15;	itemmade = "Nipple Barbells";			break;	// Desc = "Barbell ring nipple piercings. (+2 BDSM, +2 Confidence, +1 Titty, +1 Libido, adds Pierced Nipples)"
				case 1:		Cost = 15;	itemmade = "Spiked Nipple Barbells";	break;	// Desc = "This set of black spiked barbell rings look dangerous and sexy at the same time. (+5 BDSM, +3 Confidence, -1 Titty, +1 Libido, +3 Spirit, adds Pierced Nipples)"
				case 2:		Cost = 10;	itemmade = "Hooker Hoops";				break;	// Desc = "Huge hoop earrings. They are the opposite of classy but also sexy in their own special way. (+3 Cha, -1 Hap, -15 Refinement, removes Elegant)"
				case 3:		Cost = 10;	itemmade = "Bauble Necklace";			break;	// Desc = "This necklace is basically some semi-precious stones on a string. It's pretty, but it's not the kind of jewelry you write home about. (D) (+5 Cha)"
				case 4:		Cost = 10;	itemmade = "Belly Button Ring";			break;	// Desc = "Barbell ring usually worn by exotic dancers. (+3 Strip, +2 Charisma, +1 Beauty, Adds Pierced Navel)"
				case 5:		Cost = 10;	itemmade = "Clit Ring";					break;	// Desc = "Clit piercing that keeps the girl in a permanent state of arousal. (+2 Normal, +2 BDSM, +7 Hate and Fear, -5 Happiness, +10 Libido, adds Pierced Clit)"
				case 6:		Cost = 10;	itemmade = "Nipple Rings";				break;	// Desc = "Ring shaped nipple piercings. (+2 BDSM, +3 Libido and Titty, adds Pierced Nipples)"
				case 7:		Cost = 10;	itemmade = "Nose Ring";					break;	// Desc = "A thin, silver ring that pierces the septum and hangs in between the nostrils. (+5 Best, +2 BDSM, -7 Refinement, adds Pierced nose, removes Elegant)"
				case 8:		Cost = 10;	itemmade = "Pink Bangle";				break;	// Desc = "Pink bangle made of plastic, frequently worn by women with no class. (+3 Charisma, -6 Refinement)"
				default:	Cost = 10;	itemmade = "Tongue Stud";				break;	// Desc = "Tongue piercing that will drive her customers crazy. (+2 BDSM, -5 Refinement,  -Elegant, adds Pierced Tongue)"
				}
			}
			else if (craftpoints < 50)					// small common items
			{
				random = g_Dice % 10;
				if (girl->magic() >= 30 && girl->mana() >= 30 && g_Dice.percent(girl->magic() / 3)) random = (g_Dice % 2)*2;
				switch (random)
				{
				case 0:	if (girl->magic() >= 30 && girl->mana() >= 30)
				{
					Magic = 30;	Cost = 90;	itemmade = "Rainbow Ring";	break;	// Desc = "This magical stone ring allows its wearer to change the color of their hair at will, and even to slightly alter their skin tone to fit their whims or their partners'. (+15 Beau, +5 Happ)"
				}
				case 1:		Cost = 45;	itemmade = "Silver Necklace";	break;	// Desc = "This is a slender silver neck chain. It's subtle, but definitely elegant. (C) (+10 Cha)"
				case 2:	if (girl->magic() >= 10 && girl->mana() >= 10)
				{
					Magic = 10;	Cost = 40;	itemmade = "Blonde Ring";	break;	// Desc = "A ring given by the cult of blondes. It can shoot lasers. (Combat +5, Beauty +5)"
				}
				case 3:		Cost = 35;	itemmade = "Cross Necklace";	break;	// Desc = "Necklace with a gold crucifix. One can't help but want to be good when they wear it. Unfortunately, not all sex acts are considered 'good' by most people. (+10 Spirit, +7 Charisma, +5 Morality, -20 Lesbian, -15 Group, -10 Anal/Oral)"
				case 4:		Cost = 35;	itemmade = "Silver Bracelet";	break;	// Desc = "This handcrafted bracelet is a good complement to any outfit. (+3 Charisma, +10 Refinement)"
				case 5:		Cost = 30;	itemmade = "Nipple Chain";		break;	// Desc = "Set of nipple rings with a metallic chain that connects them. (+3 BDSM, +3 Titty, +2 Charisma, +3 Libido, adds Pierced Nipples)"
				case 6:		Cost = 30;	itemmade = "Belly Chain";		break;	// Desc = "Metallic chain usually worn by exotic dancers around their waist. (+5 Strip, +3 Performance, +3 Charisma, +1 Beauty)"
				case 7:		Cost = 30;	itemmade = "Cowbell Collar";	break;	// Desc = "Moo. I hope you don't find the sound of a cowbell annoying. (+10 Best, -2 Cha, +7 Obed, -5 Happ, -15 Spi, -5 Refinement, removes Elegant)"
				case 8:		Cost = 25;	itemmade = "Cross Earrings";	break;	// Desc = "Earrings with a dangling crucifix made of silver. Popular with all types of girls - except vampires. (+5 Spirit, +3 Charisma)"
				default:	Cost = 25;	itemmade = "Silver Ring";		break;	// Desc = "A plain silver ring. Despite its price it doesn't have any magical properties. (+10 Refinement)"
				}
			}
			else if (craftpoints < 100)					// medium items
			{
				random = g_Dice % 3;
				switch (random)
				{
				case 0:		Cost = 90;	itemmade = "Gold Pendant";		break;	// Desc = "A finely crafted heart pendant hangs on this pair of twisting gold chains. (A) (+20 Cha)"
				case 1:		Cost = 80;	itemmade = "Gold Bangle";		break;	// Desc = "Handcrafted gold bangle with a beautiful decoration carved around it. (+5 Charisma, +12 Refinement)"
				default:	Cost = 70;	itemmade = "Gold Ring";			break;	// Desc = "A plain gold ring. Despite its price it doesn't have any magical properties. (+15 Refinement)"
				}
			}
			else if (craftpoints < 150)					// large items
			{
				random = g_Dice % 7;
				if (girl->magic() >= 50 && girl->mana() >= 50 && g_Dice.percent(girl->magic() / 3)) random = (g_Dice % 3) * 2;
				switch (random)
				{
				case 0:	if (girl->magic() >= 50 && girl->mana() >= 50)
				{
					Magic = 50;	Cost = 130;	itemmade = "Slave Band";	break;	// Desc = "This thick steel armband affects the wearer's mind and makes it difficult for her to refuse what is asked of her. It has a frightening aura about it, and it is almost impossible to conceal the object's true nature. (+50 Obed, adds Controlled status)"
				}
				case 1:		Cost = 140;	itemmade = "Diamond Hairpin";	break;	// Desc = "A Diamond Hairpin. She'll feel like a million gold pieces with this, though it fortunately doesn't actually cost that much. (+5 Refinement, +10 Cha)"
				case 2:	if (girl->magic() >= 50 && girl->mana() >= 50)
				{
					Magic = 30;	Cost = 140;	itemmade = "Star Pendant";	break;	// Desc = "Star-shaped pendant with a jewel at its center. (Prevents poison and heavy poison)"
				}
				case 3:		Cost = 140;	itemmade = "Obsidian Choker";	break;	// Desc = "This black silk choker boasts a polished obsidian medallion. Simple, but sophisticated. (B) (+10 Lib, +5 Cha, adds Elegant)"
				case 4:	if (girl->magic() >= 50 && girl->mana() >= 50)
				{
					Magic = 20;	Cost = 140;	itemmade = "Elven Tiara";	break;	// Desc = "This mithril tiara is a proof of the mastery of elven blacksmiths and jewelcrafters. (+10 Charisma, +15 Refinement, +5 Magic)"
				}
				case 5:		Cost = 120;	itemmade = "Diamond Necklace";	break;	// Desc = "Platinum necklace with inlaid diamonds, elegant and expensive. (+15 Cha/Refine, +5 Bea, +20 Love, -10 Hate)"
				default:	Cost = 135;	itemmade = "Engagement Ring";	break;	// Desc = "A gold ring with a big diamond in the middle. If you give a girl this, she pretty much has to say yes. (Love +50, Beauty +10)"
				}
			}
			else if (craftpoints < 200)					// difficult to make items
			{
				random = g_Dice % 3;
				switch (random)
				{
				case 0:	if (girl->magic() >= 20 && girl->mana() >= 20)
				{
					Magic = 20;	Cost = 180;	itemmade = "Exotic Amulet";	break;	// Desc = "This amulet gives the wearer a slight otherworldly quality. Their eyes appear unnaturally striking, and they seem more beautiful and compelling. (+10 Beauty, +Strange Eyes, +Exotic)"
				}
				case 1:		Cost = 160;	itemmade = "Pearl Necklace";	break;	// Desc = "A simple string of pearls, short enough to remain classy but long enough to be worth a lot of money. (B) (+15 Cha, +5 Lib)"
				default:	Cost = 190;	itemmade = "Jeweled Poignard";	break;	// Desc = "A finely crafted dagger, inlaid with silver and bearing a precious stone on its hilt. It's almost jewelry. (C) (+5 Cha, +10 Com, -10 Fear, -5 Hate, +5 Conf)"
				}
			}
			else if (craftpoints < 250)					// rare and difficult to make items
			{
				Cost = 240;	itemmade = "Eridium Jewelry";	break;	// Desc = "A bracelet made of the rare element Eridium. Very rare, very valuabe, and slightly magic. (Magic +5, Love +25, Hate and Fear -15)"
			}
			else if (craftpoints >= 250)				// unique items
			{
			}
			else										// unsorted items
			{
				Magic = 0;		Cost = 0;		itemmade = "Amulet of Fleshy Softness";//                  Desc = "Turn your girl completely human again. She'll need some time adjusting to the newer, fragile human body of hers, though. (Removes Construct, Incorporeal, Sterile. Adds Fragile)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of Incest";//                           Desc = "A magical amulet that makes the girl biologically related to whoever gives it them. (Adds Your Daughter.)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of Invigoration";//                     Desc = "This bright, simple pendant is composed of some unknown metal. It always trembles slightly, as if some energy inside it simply can't remain stagnant. When put on, this energy extends to the wearer, ensuring that they never feel tired. The strong, stimulating effect can cause some nervousness, and the lack of any sleep may reduce physical resilience. (-100 Tiredness, -10 Constitution, +Nervous)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of Life";//                             Desc = "Contender for the Most Heroes Killed award. (B) (+20 Cons, +10 Hea, +10 Conf, Int -15)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of Pinocchio";//                        Desc = "This fey-enchanted amulet has the unique ability to make any construct into a real, living, breathing, flesh and bone human being! (+50 Hap, -10 Com, -25 Cons, removes Construct)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of the Cunning Linguist";//             Desc = "This tongue-shaped amulet instantly gives its wearer the ability to speak and understand all known languages perfectly, magnifying the effects of learning both mundane and arcane. It also has the curious side effect of making one intensely desire putting one's tongue on a woman's genitals. Odd. Strange. (+10 Mag, +10 Les, +10 Lib, gives Lesbian and Quick Learner)"
				Magic = 0;		Cost = 0;		itemmade = "Amulet of the Sex Elemental";//                Desc = "This beautiful ruby amulet, hanging from a gold chain, holds a terrifying power. Any woman wearing it becomes an insatiable thing of pure lust, mentally and physically. Her breasts grow to an enormous size, an immense, permanently-erect penis appears between her legs so she can pleasure (and be pleasured by) anyone regardless of gender, and every moment of every day sees her mind obsessed with intense thoughts of sex. She also gains supernatural endurance, allowing her body to withstand the exertion of her new appetites. (+100 all sex skills, +100 Lib, Cons, Obed, removes Big Boobs, Lolita, Small Boobs, adds Fast Orgasms, Not Human, Nymphomaniac, Great Figure, Great Arse, Fearless, Sterile, Tough, Futanari, Mind Fucked, Long Legs, Puffy Nipples, Perky Nipples, Abnormally Large Boobs)"
				Magic = 0;		Cost = 0;		itemmade = "Asexual Armlet";//                             Desc = "This inconspicuous bracelet locks onto a girl's wrist, affecting her with powerful magic. She completely loses all sexual drive and ability but retains any natural beauty. It bestows a level of intelligence and enlightenment that allows the wearer to see great joy in virtually everything. She'll also be so attuned to the world around her that the feelings of others become easily realized. (-100 All Sex Skills, -100 Lib, +10 Int, +50 Hap, +Sterile, +Optimist, +Psychic)"
				Magic = 0;		Cost = 0;		itemmade = "Bestial Ring";//                               Desc = "This rough bronze ring awakens its wearer's primal instincts, suppressing their inhibitions and intellect as it draws their base passions to the surface. (+20 Lib/Str, +50 Best, -50 Strip and Serv, +10 all other sex skills, -50 Int, removes Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Bimbo Ring";//                                 Desc = "This gold-plated tin ring considerably increases its wearer's physical beauty... at the cost of making her vapid, crude, and generally not as much fun to be around. (+50 Bea, -50 Cha, -20 Serv, removes Cool Person)"
				Magic = 0;		Cost = 0;		itemmade = "Bracer of Power";//                            Desc = "This magical bracer boosts the strength of whoever wears it. (+5 Combat, +15 Strength)"
				Magic = 0;		Cost = 0;		itemmade = "Cock Ring of Removal";//                       Desc = "A magic ring that is designed to be worn around a penis. When worn, the penis disappears and the ring along with it. (Removes Futanari)"
				Magic = 0;		Cost = 0;		itemmade = "Collar of Empathy";//                          Desc = "These ornate, locking collars were originally used voluntarily by priests committed to non-violence. It causes the wearer to shun aggression and lets them psychically share the emotions of those around them. The increased empathy also leads to greater acceptance towards the will of others. (-30 Combat, +30 Obed, +Psychic, +20 Cha, +50 Spirit, -30 Hate, -Aggressive, -Sadistic, -Merciless, -Iron Will, -Tsundere/Yandere, -Assassin, -Fearless, -Twisted)"
				Magic = 0;		Cost = 0;		itemmade = "Cowbell Nipple Rings of Lactation";//          Desc = "Nipple rings with dangling magical cowbells that increase the girl's lactation to impressive levels. They'll also increase breast size, but only if the wearer is of below-average size. (+10 Beast, +2 BDSM, +3 Libido, +5 Titty, -5 Spirit, adds Cow Tits and Pierced Nipples, Increases breast size to Average)"
				Magic = 0;		Cost = 0;		itemmade = "Daddy's Girl Necklace";//                      Desc = "A small necklace with the word's Daddy's Girl on it. (Obedience +5, Adds Dependant)"
				Magic = 0;		Cost = 0;		itemmade = "Disguised Slave Band";//                       Desc = "It looks like a simple piece of jewelry, but the subtle interlocking geometrical designs on this beautiful silver armband barely hint at the object's true power: to suppress the wearer's ego and put her entirely under your will. (+50 Obed, adds Controlled status, -Iron Will)"
				Magic = 0;		Cost = 0;		itemmade = "Doll's Ring (Greater)";//                      Desc = "This ring is somehow a tight fit on any girl's finger, regardless of her actual size. It causes the wearer to become permanently youthful, as well as making her body like smooth, delicate, flawless porcelain. In addition, her breasts will become smaller and more perky, but her sexual ability decreases. (Age never above minimum, +10 Beauty, +Lolita, +Small Boobs, +Puffy Nipples, +Fragile, -25 Constitution, -25 Sex Skills, -50 Combat)"
				Magic = 0;		Cost = 0;		itemmade = "Doll's Ring (Lesser)";//                       Desc = "This ring makes the wearer become significantly younger, as well as giving her rather doll-like features. She becomes more fair and beautiful, but she'll also be more fragile. (-8 Age, +5 Beauty, +Lolita, +Small Boobs, -Tough, -15 Cons/Str, -30 Combat)"
				Magic = 0;		Cost = 0;		itemmade = "Epic Necklace of Babeosity";//                 Desc = "A serious refinement of the initial design. (Removes Lolita, Small Boobs, Slow Orgasms. Adds Fast Orgasms, Nymphomaniac, Great Figure, Great Arse, Long Legs, Puffy Nipples, Perky Nipples, Abnormally Large Boobs, +50 cha, beauty, libido, confidence, constitution)"
				Magic = 0;		Cost = 0;		itemmade = "Hime Amulet";//                                Desc = "A magical amulet that makes your girl into a perfect little princess. (Adds Lolita, Cute, and Princess.)"
				Magic = 0;		Cost = 0;		itemmade = "Hime Ring";//                                  Desc = "Removes all blemishes - physical and mental. A princess should be perfect, after all! (+10 Beauty. Removes Scars, Mind Fucked, Malformed, Retarded, Slow Learner, Twisted)"
				Magic = 0;		Cost = 0;		itemmade = "Hime Tiara";//                                 Desc = "A cute little crown to complete a princess's ensemble. Teaches her to act like a perfect little girl. (+10 Beauty. Removes Yandere, Iron Will, Assassin. Adds Meek, Charming.)"
				Magic = 0;		Cost = 0;		itemmade = "Invincible Armlet ";//                         Desc = "Fist or flame, sword or spell, the wearer of this adamantine armlet needs fear harm no longer. Time alone will take its toll. (S) (-100 Fear, adds Incorporeal)"
				Magic = 0;		Cost = 0;		itemmade = "Invincible Armlet";//                          Desc = "Fist or flame, sword or spell, the wearer of this adamantine armlet needs fear harm no longer. Time alone will take its toll. (S) (-100 Fear, adds Incorporeal)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Belly-Button Piercing";//                Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  The piercing and the jewelry's magic make the girl's figure seem more attractive, especially when scantily-dressed or naked. (+Great Figure and Pierced Navel, +10 Strip, +5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Clit Hood Piercing";//                   Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This piercing and the jewelry's magic keep the girl more aroused and compliant. (+Fast Orgasms, +15 Lib/Obed, +10 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Clit Piercing";//                        Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This unpleasant piercing goes straight through the clitoris, and the jewelry's magic keeps the girl too sensitive and aroused to ever be fully satisfied.  (+Fast Orgasms, +Nympho, +40 Lib, +12 Fear/Hate)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Ear-Series Piercings";//                 Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This makes a series of several piercings around each of a girl's earlobes and cartilidge.  Many find it sexy, but it serves little other purpose. (+Sexy Air, +3 Beauty +5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Facial Piercings";//                     Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This versatile item pierces a girl's lips, eyebrows, or nose.  While not especially useful, many find it cool and kind of sexy. (+Cool Person, +5 Cha. +5 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Labia Piercings";//                      Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This rare set of labia piercings create a magical desire for pain, while the jewelry contains many small, sharp barbs to help satisfy that desire.  (+Masochist, +Fast Orgasms, -20 Norm, +20 Libedo, +30 BDSM, +10 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Nipple Piercings";//                     Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  These piercings make the girl's nipples seem perky and cute, even under clothes. The jewelry's magic also keeps her slightly aroused and anxious. (+Perky Nips, +Cute, +15 Lib, +10 BDSM, +10 Fear, -15 Conf)"
				Magic = 0;		Cost = 0;		itemmade = "Magic Tongue Piercing";//                      Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  The piercing and the jewelry's magic will make her tongue feel more pleasing to others, but the magic makes her less able to articulate and speak elegantly. (+Good Kisser, -Elegant, +15 Norm/Les Sex, +25 Oral, +8 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Minor Ring of Beauty";//                       Desc = "This copper band smoothes your rough edges, hides small imperfections and makes you look good. (+30 Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Minor Ring of Charisma";//                     Desc = "A solid crystal band who telepathically whispers to you the right things to say and do. (+30 Charisma)"
				Magic = 0;		Cost = 0;		itemmade = "Minor Ring of the Horndog";//                  Desc = "All night, every night and they'll never know. (+30 Libido)"
				Magic = 0;		Cost = 0;		itemmade = "Minor Ring of the Schwarzenegger";//           Desc = "Personally endorsed by the Governator of California. (+30 Str, +15 Cons)"
				Magic = 0;		Cost = 0;		itemmade = "Necklace of Control";//                        Desc = "Worn about the neck, this discreet iron chain affects its wearer's mind, suppressing her willpower and putting her under your control. (+100 Obed, -Iron Will,  adds Controlled status)"
				Magic = 0;		Cost = 0;		itemmade = "Necklace of Humbling";//                       Desc = "This unattractive necklace requires a magical key to be removed. Designed to teach humility, it makes anyone quite plain and dull. It's magic will cause even the most self-absorbed girl to get over herself and be think more of others. (+100 Serv, +25 Obed, -10 Beauty/Cha -50 Spirit/Conf, +10 Hate/Fear, Removes: Yandere, Tsundere, Iron Will, Aggressive, Merciless, Assassin, Fearless, Sadistic, Cute, Elegant, Sexy Air, Great Figure/Arse, Charismatic, Charming, Long Legs, Puffy/Perky Nipples, Big/Huge Breasts)"
				Magic = 0;		Cost = 0;		itemmade = "Necklace of Pain Reversal";//                  Desc = "This sinister steel necklace transforms the wearer's pain into pleasure. It sounds good at first, but a tendency to seek bodily harm may come with its own set of problems... (-15 Comb, -15 Cons, adds Masochist and Fearless)"
				Magic = 0;		Cost = 0;		itemmade = "Nipple Rings of Breast Expansion";//           Desc = "Set with crystals refined from oil of breast growth, this pair of silver rings cause a woman's chest to an impressive (if still plausible) size when placed around the nipples. Naturally well-endowed woman won't see much of a change, though. (adds Big Boobs)"
				Magic = 0;		Cost = 0;		itemmade = "Nipple Rings of Lactation";//                  Desc = "This pair of nipple rings increase the girl lactation to impressive levels. (+1 BDSM, +1 Libido, adds Abundant Lactation and Pierced Nipples)"
				Magic = 0;		Cost = 0;		itemmade = "Nipple Rings of Perkiness";//                  Desc = "Special rings designed to keep the nipples of her wearer erect. (+1 BDSM, +1 Libido, adds Perky Nipples and Pierced Nipples)"
				Magic = 0;		Cost = 0;		itemmade = "Nipple Rings of Pillowy Softness";//           Desc = "When placed on a woman's nipples, this pair of gold rings causes her breasts to expand to a prodigious size. Fortunately, they are trivial to remove, allowing their wearer to go about their daily life without repercussions. (adds Abormally Large Boobs)"
				Magic = 0;		Cost = 0;		itemmade = "Nipple Rings of Puffiness";//                  Desc = "This set of nipple rings causes a women's areolas to puff up. (+1 BDSM, +1 Libido, adds Puffy Nipples and Pierced Nipples)"
				Magic = 0;		Cost = 0;		itemmade = "Pacifist's Armlet";//                          Desc = "The enchantment placed on this pretty blue armband slows down the wearer's movements whenever they are attempting to harm another person for any reason. It's the bane of warriors everywhere, but also an unpleasant object for anyone to wear. (+5 Cha, -60 Comb, -40 Str, -30 BDSM, -50 Hap, -10 Conf, +60 Fear, +30 Hate)"
				Magic = 0;		Cost = 0;		itemmade = "Prophylactic Ring";//                          Desc = "A common enchantment on this strange flexible ring makes it impossible for its wearer to become pregnant by any means. It can't stop non-human creatures from implanting autonomous seed in the body, however. (-10 Fear, -10 Hate, adds Sterile)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Beauty";//                             Desc = "Made of electrum, this band alters your body to make you stunningly attractive. (+50 Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Breast Reduction";//                   Desc = "Prized by the likes of female acrobats and warriors whose assets are occasionally a liability, this simple copper ring causes a woman's chest to shrink and flatten. (adds Small Boobs)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Charisma";//                           Desc = "This blood red band is more knowledgeable and aggressive than the minor ring in making you the life of the party. (+50 Charisma)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Enlightenment";//                      Desc = "This delicate silver ring opens the eyes of its wearer's mind, increasing their capacity for learning. (adds Quick Learner)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Limited Freedom (Jade)";//             Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure the slave remains subservient to their master. The effects only go away if the master is the one to remove the ring. This variation causes a slave to feel increasingly unbearable anxiety if away from their master for any length of time. (-Slave, +10 Obedience, +Nervous, +Dependent)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Limited Freedom (Onyx)";//             Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure the former slave remains subservient to their master. The effects only go away if the master is the one to remove the ring. This variation causes the wearer to grow ridiculously large breasts that both impede her bodily movement and objectify her to everyone. (-Slave, +10 Obedience, +Abnormally Large Boobs, -20 Agility)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Limited Freedom (Ruby)";//             Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure a former slave remains subservient to their master. This variation gives the wearer greater sexual desire, especially towards men, but also makes it extremely difficult to achieve release or orgasm. (-Slave, +10 Obedience, +60 Libido, +10 Normal/Group. -10 Lesbian. +Slow Orgasms)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Major Intellect";//                    Desc = "This ring is beautifully crafted using precious materials, and it greatly enhances the wearer's intelligence. Such enchanted items are uncommon, but can usually be found by those who can afford them. Anyone can appreciate both the beauty and practicality of such an item. (+30 Int, +10 Love)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Minor Intellect";//                    Desc = "This ring is enchanted to moderately increase the wearer's intelligence. Magic users often learn this as one of their first enchantments, which makes rings like this one readily available. (+8 Int)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Pleasure";//                           Desc = "This magical ring affects the wearer's nervous system, heightening the effects of sexual stimulation and allowing her to reach orgasm much faster than most. (adds Fast Orgasms)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of Reliance";//                           Desc = "While worn, this ring causes the wearer to feel very needy towards other, especially the person that gave it to them. The effect also extends to the ring itself, ensuring that anyone who wears it will always want to have it on. Any girl will develop a significant sense of love and connection, but she will also feel very dependent and helpless without you. (+Dependent, +30 Love, -100 Confidence)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of the Hivemind";//                       Desc = "This topaz ring magically links its wearer's mind with the minds of others around her, making her feel their surface emotions. The effect is too faint to work one-on-one, but it will encourage her to seek the pleasure of others in group situations. (+20 Group, +5 Strip)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of the Horndog";//                        Desc = "All day and night for the rest of time and they'll never know. (+50 Libido)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of the MILF";//                           Desc = "A mysterious ring that makes a woman slightly older and more mature. (Age +5. Adds Big Boobs and MILF. Removes Lolita.)"
				Magic = 0;		Cost = 0;		itemmade = "Ring of the Schwarzenegger";//                 Desc = "Only the best for the Californian Governator, This ring terminates flabby muscles!. (+50 Str, +25 Cons)"
			}

			if (itemmade == "")	{ Magic = 0;  Cost = 10; itemmade = "Bauble Necklace"; }//                 Desc = "This necklace is basically some semi-precious stones on a string. It's pretty, but it's not the kind of jewelry you write home about. (D) (+5 Cha)"
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


	// `J` - Finish the shift - Jeweler

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
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary improvement (+2 for single or +1 for multiple)
	girl->upd_skill(SKILL_CRAFTING,	(g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	girl->agility((g_Dice % skill) - 1);
	girl->confidence(max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Sharp-Eyed", 50, actiontype, "Working on such small items has made " + girlName + " rather Sharp-Eyed.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Jeweler(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->crafting() +
		// secondary - second 100
		((girl->agility() + girl->confidence() + girl->intelligence() + girl->magic()) / 4) + 
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
