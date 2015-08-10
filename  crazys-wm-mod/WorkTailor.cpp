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
#include "cFarm.h"


extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;
extern cInventory g_InvManager;

#pragma endregion

// `J` Job farm - Staff
bool cJobManager::WorkTailor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMAKEITEMS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked making and mending clothes at the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	double wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Tailor(girl, false);
	double craftpoints = jobperformance;

	int dirtyloss = brothel->m_Filthiness / 10;		// craftpoints lost due to repairing equipment
	if (dirtyloss > 0)
	{
		craftpoints -= dirtyloss * 2;
		brothel->m_Filthiness -= dirtyloss;
		ss << "She spent some of her time repairing the Farm's equipment instead of making new stuff.\n";
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
		wages -= 15; craftpoints *= 0.4; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n\n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	int tired = (300 - (int)jobperformance);	// this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
	if (roll_a <= 10)
	{
		tired /= 8;
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
				ss << "She stabed herself while working";
			if (girl->health() <= 0)
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident while working as a Tailor at the Farm.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}

		else	// unhappy
		{
			ss << "She did not like working as a tailor today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 12;
		craftpoints *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace with her neddle work by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 10;
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";

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


	// `J` Farm Bookmark - adding in items that can be created in the Farm
	if (craftpoints > 0)
	{
		// `J` Incomplete Craftable code - commenting out
#if 0
		ss << g_InvManager.CraftItem(girl, JOB_TAILOR, int(craftpoints));
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
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints < 50)					// small common items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints < 100)					// medium items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints < 150)					// large items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints < 200)					// difficult to make items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints < 250)					// rare and difficult to make items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else if (craftpoints >= 250)				// unique items
			{
				switch (g_Dice % 6)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else										// unsorted items
			{
				Magic = 0;		Cost = 10;		itemmade = "Eye patch";//                       Desc = "Who says that you have to actually lose an eye to be able to wear this? (Adds Eye Patch)"
				Magic = 0;		Cost = 0;		itemmade = "Adorable Underwear";//              Desc = "Teddy bears and other childlike images cover this vibrantly pink underwear. The cute patterns and a minor magical effect make the wearer seem slightly younger and very adorable. (+Cute, -1 Age, +4 Cha, +3 Beauty, +5 Happiness)"
				Magic = 0;		Cost = 0;		itemmade = "Apron";//                           Desc = "A simple apron, used for cooking. (+15 Service/Cooking, +5 Obedience)"
				Magic = 0;		Cost = 0;		itemmade = "Bad Nurse Costume";//               Desc = "Similar to the nurse costume, but it's skimpier and black instead of white - which should give you the idea that something went wrong. (+7 Cha, +7 Lib, +5 BDSM, +5 Hand, +5 Foot, +8 Strip, +15 Med, -7 Morality, adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Black Knight Uniform Coat";//       Desc = "The uniform coat for a rebel soldier - Defeated, though there are said to be a few surviving remnants, still waiting to bring Justice to Crossgate... (+15 Charisma, + 10 Combat, + 10 Cons, +45 Spirit, + 5 Hate, - 20 Fear, - 5 Obediance, + Cool Person, + Fearless) "
				Magic = 0;		Cost = 0;		itemmade = "Black Leather Underwear";//         Desc = "The thick leather that comprises this bra and panties makes them somewhat uncomfortable to wear. The interior is intentionally rough and slightly abrasive. While quite unpleasant, they don't cause any actual harm; however, they make the wearer more capable of tolerating discomfort. They also serve as a reminder of the girl's status. (+20 BDSM, +10 Obedience, +5 Libedo)"
				Magic = 0;		Cost = 0;		itemmade = "Black Stole";//                     Desc = "It's the exact same thing as the pink version, but the manufacturer figured he'd make it more limited and more expensive, since black supposedly looks less trashy. (BDSM +5, Strip +10, Perf +3, Cha +4)"
				Magic = 0;		Cost = 0;		itemmade = "Black String Bikini";//             Desc = "Water doesn't affect this bikini as it does white swimwear. (+Great Figure, Great Arse, +30 agil +10 cha +10 happy +5 love/conf/int -5 fear)"
				Magic = 0;		Cost = 0;		itemmade = "Bloomers";//                        Desc = "Part of a school's gym uniform. Makes girl's derriere's look tasty, and encourages exercise! (Adds Great Arse, +5 Con)"
				Magic = 0;		Cost = 0;		itemmade = "Bodice";//                          Desc = "A medieval era corset made of cloth and worn over clothing that laces in the front. (Adds Big Boobs)"
				Magic = 0;		Cost = 0;		itemmade = "Bourgeoise Gown";//                 Desc = "Stylish, yet affordable, this type of gown is commonly worn by merchant's wives and other such well-to-do middle class ladies. (+10 Cha, +10 Hap, +5 Conf, +8 Love)"
				Magic = 0;		Cost = 0;		itemmade = "Brothel Gown";//                    Desc = "This respectable-looking gown holds an enticing secret: parts of it are separately detachable, allowing unrestricted access to the wearer's breasts, ass and genitals. It's perfect for the girl who needs to go from presentable to slutty in the blink of an eye. (C) (+10 Cha, +5 Hap, +5 Conf, +10 Lib, +15 Service, +5 Strip)"
				Magic = 0;		Cost = 0;		itemmade = "Bunny Ears";//                      Desc = "Headband with black bunny ears. Goes perfectly with the Bunny Outift and Tail Plug, but can also work well on its own. (+8 Charisma, +5 Service)"
				Magic = 0;		Cost = 0;		itemmade = "Bunny Outfit";//                    Desc = "A shiny leotard with fishnet stockings, cuff links, and a seperate collar and bow tie. An absolute must for bunny girls. (+12 Cha, +2 Bea, +8 Lib, +12 Serv, adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Cat Ears Headband";//               Desc = "Headband with cute cat ears. The moe will be hard to resist. (+7 Cha, +7 Beast, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Cat Maid Headband";//               Desc = "This headband doubles as both a maid's headband and a pair of cat ears. (+7 Cha, +7 Beast, +7 Service, +7 Obed, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Cat Paw Glove";//                   Desc = "Awww, what could be more cute than a pair of big fluffy cat-paw gloves? Of course, it does make it rather difficult for her to perform handjobs. (+5 Charisma, +3 Beast, -3 Service, -3 Handjob, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Childish Heart Backpack";//         Desc = "This may not be edible, but it sure does add a bit of childish nature to said girl.....no literally, it make them younger and have younger traits. (Love +10, Spirit +15, Age -5) (Fear and Hate -10) (Adds Cute, Lolita and a fresh new Va-jay-jay!)"
				Magic = 0;		Cost = 0;		itemmade = "Chinese Dress";//                   Desc = "A brilliantly decorated, colorful, and supremely shape-hugging Quipao from an exotic land. The dress is slit all the way up the side to allow for plenty of leg to show. (+7 Cha, +7 Refinement, +2 Spi, +5 Agi, +7 Service, Adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Classy Underwear";//                Desc = "The height of fashion, this underwear is made of excellent quality materials. Simple enchantments also enhance the physical appearance of the wearer. (+6 Cha, +3 Beauty, +5 Happiness, +Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Cloche Hat";//                      Desc = "Seeing this hat, you just know you'll never understand fashion.. (Cha/Bea/Hap +5, adds Cute, Dojikko)"
				Magic = 0;		Cost = 0;		itemmade = "Comfortable Underwear";//           Desc = "Made of a silky material, this underwear is comfortable and has a nice look to it. (+5 Happiness, +1 Cha, +1 Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Corset";//                          Desc = "Make sure to pull it so tight that she can't breathe to give its full effect! (-5 constitution. Adds Big Boobs and Great Figure.)"
				Magic = 0;		Cost = 0;		itemmade = "Cotton Panties";//                  Desc = "It's still very basic underwear, but 100 percent cotton, making it a little more comfortable to wear. (+4 Happiness, +1 Charisma)"
				Magic = 0;		Cost = 0;		itemmade = "Cow Ears Headband";//               Desc = "Headband with cow ears and horns. (+1 Charisma, +10 Beastiality, -5 Spirit)"
				Magic = 0;		Cost = 0;		itemmade = "Cow-Print Glove";//                 Desc = "Elbow length cow-print glove. Sympathetic magic related to the design allows girls to give better Titty Sex. (+2 Cha, +2 Con, +2 Obed, +10 Beast, +5 Titty, +2 BDSM, +2 Service, -5 Spirit)"
				Magic = 0;		Cost = 0;		itemmade = "Cow-Print Lingerie";//              Desc = "This skimpy set of bra and panties is decorated with cow prints. Sympathetic magic related to the design allows girls to give better Titty Sex. (+15 Best, +12 Titty, +7 Service, +5 all other sex skills, +3 Cha, +10 Cons, +5 Lib, -15 Spirit)"
				Magic = 0;		Cost = 0;		itemmade = "Cowgirl Hat";//                     Desc = "A wide-brimmed hat that yells 'Country Gal' for whoever wears it. Not literally though, that would be wierd. (+2 Charisma, +10 Spirit, +7 Farming, +5 Animal Handling)"
				Magic = 0;		Cost = 0;		itemmade = "Cutoff Shorts";//                   Desc = "Tiny little shorts made from getting a pair of jeans and cutting them so small that they look like panties. (Adds Great Arse, Long Legs.)"
				Magic = 0;		Cost = 0;		itemmade = "Dancer Costume";//                  Desc = "This colorful and flashy dress is made of long, interlocking strips of light fabric, festooned with sequins and small polished stones. It is intended to flow gracefully around the wearer's body while she dances, and while it wasn't designed primarily as a stripping aid, it is skimpy and light enough to perform admirably in that regard. (C) (+20 Performance, +15 Strip, +10 Cha, +5 Lib)"
				Magic = 0;		Cost = 0;		itemmade = "Designer Lingerie";//               Desc = "This unbelievably skimpy underwear was designed by a famous tailor. There's so little actual fabric that you have to wonder what justifies its exorbitant price... but it does look incredibly sexy, and every girl loves being at the height of fashion. (A) (+15 Cha, +20 Lib, +5 Hap, +5 Refinement, +20 all sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Eggscellent Hat";//                 Desc = "A trucker cap that says I'm Eggscellent. You can't just buy this with money. (Happiness, Charisma, Constitution, Fame, Beauty +10.)"
				Magic = 0;		Cost = 0;		itemmade = "Fancy Stockings";//                 Desc = "Thigh high sheer stockings that add a nice, classy look to a girl. (Adds Sexy Air and Elegant, +10 Charisma, +5 Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Fishnet Stockings";//               Desc = "Thigh high fishnet stockings that are more trashy than classy. (+5 Charisma, adds Sexy Air and Long Legs, removes Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Fox Stole";//                       Desc = "It's a dead fox! But, for some reason, girls love wrapping this around their neck. (Cha/Bea/Conf/Lib +5, HAP +10)"
				Magic = 0;		Cost = 0;		itemmade = "Frilled Cotton Panties";//          Desc = "A girls first attempt at lingerie for her first sexy date. Inexpensive, but only meant to send a message to her boyfriend. (+4 Happiness, +1 Charisma, +3 Libido, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Fur Stole";//                       Desc = "A genuine fur stole. The quality of the material sets it totally apart from the cheaper fabricated variations, but this also means girls will not want to use these as mere tools for rough play. Now just keep PETA activists away from your girl. (Strip +10, Perf +3, Cha/Bea +10, Conf +5, HAP +10, Love +20, Hate/Fear -20, adds Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Fuzzy Dice";//                      Desc = "The dots are actually diamonds. (-50 Tiredness, +50 Happiness)"
				Magic = 0;		Cost = 0;		itemmade = "Garter";//                          Desc = "A simple garter worn around a girl's thigh. (+2 Charisma, +3 Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Gemstone Dress";//                  Desc = "This unbelievably opulent silk dress is covered in dozens upon dozens of precious gems, making its wearer look like a sexy walking treasure hoard. It's still light enough to run in, so make sure you only give it to a girl you trust not to skip town! (A) (+24 Cha, +42 Lib, +5 Mag, +5 Conf)"
				Magic = 0;		Cost = 0;		itemmade = "Hat of Beauty";//                   Desc = "This hat will give its wearer the looks to get by. Although the hat looks so wonderfully nice, as long as people around her doesn't mind her getting cocky, she can wear it anywhere. (Serv -10, Int -10, Beau 20, Cha 10) (Adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Hime Dress";//                      Desc = "A poofy, elegant dress worn by a princess. (+10 Beauty. Adds Elegant, Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Huge Sun Hat";//                    Desc = "It shields her eyes from the sun. For some reason, she made sure to pick the one matching her most expensive dress, and wears the combo in the evening as well. (Cha +10, Spi +5, adds Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Jungle Camo Leotard";//             Desc = "Thong Leotard with a green and brown camouflage pattern. (+3 Charisma, +5 Combat, +5 Agility)"
				Magic = 0;		Cost = 0;		itemmade = "Lace Lingerie";//                   Desc = "Your basic set of lace underwear, with matching hose and garters. It looks dainty and delicate but it's robust enough to withstand being torn off in a fit of passion. (C) (+10 Lib, +4 Cha, +10 all sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Leopard Lingerie";//                Desc = "When normal lingerie just isn't wild enough for you, try lingerie with leopard print on it! Hopefully it's just print anyway. It could be made from actual leopard. Try not to think about it too much. (+15 Best, +5 all other sex skills, +5 Cons, +7 Lib, +5 Cha, +7 Spi, +7 Conf, +5 Agi, +5 Str, -2 Refinement, adds Sexy Air.)"
				Magic = 0;		Cost = 0;		itemmade = "Linen Dress";//                     Desc = "This light, inexpensive dress cut of common fabric was designed with talent and an expert eye for enhancing the female form. It's not fancy, but it's enough to make a poor girl feel pretty. (D) (+6 Cha, +18 Lib)"
				Magic = 0;		Cost = 0;		itemmade = "Long Nurse Glove";//                Desc = "Elbow length white latex glove. No nurse outfit is complete without a pair. (+3 Charisma, +7 Medicine, +2 Service, +3 Strip, +2 Handjob, +2 Animal Handling)"
				Magic = 0;		Cost = 0;		itemmade = "Lounge Dress";//                    Desc = "A long, strapless dress with a slit running down one side to expose some leg. (Adds Long Legs, Elegant, Sexy Air.)"
				Magic = 0;		Cost = 0;		itemmade = "Mage Cloak";//                      Desc = "This simple cloak, commonly worn by apprentice mages, is woven with minor enchantments that protect magic-users from their own spells, allowing them to put in just a little more juice without putting themselves in danger. It also slightly increases their endurance, in case they do bite off more than they can chew. (D) (+10 Mag, +5 Cons, -5 Agi, -2 Cha, -5 Fear, -5 Hate, -5 Comb)"
				Magic = 0;		Cost = 0;		itemmade = "Maid Glove";//                      Desc = "Excellent at cleaning, cooking and stroking hard objects. (+3 Charisma, +7 Service, +7 Cooking, +5 Handjob)"
				Magic = 0;		Cost = 0;		itemmade = "Maid Headband";//                   Desc = "A frilly, white headband that no maid's ensemble would be complete without. (+5 Charisma, +7 Service, +7 Obedience)"
				Magic = 0;		Cost = 0;		itemmade = "Maid Uniform";//                    Desc = "This outfit consists of a short-skirted clean black cotton dress, a frilly white apron and matching bonnet, and white thigh-high stockings, all of the highest quality. It has an irresistible air of quiet, willing submission about it. (B) (+12 Cha, +18 Lib, +20 Obed, +30 Serv/Cook, adds Elegant, removes Aggressive)"
				Magic = 0;		Cost = 0;		itemmade = "Ninja Garb";//                      Desc = "Want to blend in to the shadows like a ninja? With this classic set of ninja clothes, you still won't be able to. Come on, it takes years of training to be able to do that. Regardless, it is quite easy to move in and carries a certain intimidation factor. (+5 Comb, +2 Conf, +2 Con, +10 Agi, -5 Fear, -5 Hate)"
				Magic = 0;		Cost = 0;		itemmade = "Noble Gown";//                      Desc = "This type of magnificent, elaborate gown is exclusively worn by ladies of the upper class, because they're the only ones who can afford it. It's made for showing off and looking pretty, but not so much for agility or ease of motion. (B) (+18 Cha, +10 Hap, +10 Conf, -20 Agi, -10 Com, removes Fleet of Foot)"
				Magic = 0;		Cost = 0;		itemmade = "Nurse Cap";//                       Desc = "White and red nurse cap made of cotton. (+2 Charisma, +7 Medicine, +2 Animal Handling)"
				Magic = 0;		Cost = 0;		itemmade = "Panda Fur Stole";//                 Desc = "A stole made from genuine Panda Fur. Whether the source was a Panda or a Panda Girl can no longer be determined. But either way, a girl would need a rather sick mind to be wanted seen wearing this, no matter how great it looks (Strip +10, Perf +3, Cha -15, Bea +30, Conf +5, HAP +10, Love +20, Hate/Fear -20, adds Elegant, Twisted)"
				Magic = 0;		Cost = 0;		itemmade = "Paw-Print Teddy";//                 Desc = "This skimpy piece of lingerie is decorated with little puppy paw prints. It's adorable and sexy, guaranteed to bring a smile to the lips of any animal lover! (B) (+25 Best, +10 all other sex skills, +4 Cha, +5 Cons, +10 Lib, adds Cute)"
				Magic = 0;		Cost = 0;		itemmade = "Peaked Cap";//                      Desc = "A military hat. If you like your girls dangerous-looking, then this is the hat you need to get them. Looks especially good when paired with an eyepatch or cool scars. (+2 Charisma, +5 BDSM, +7 Confidence +3 Spirit)"
				Magic = 0;		Cost = 0;		itemmade = "Peasant Gown";//                    Desc = "This simple, colorful gown worn by peasant girls at the harvest festivals has an air of celebration about it. It's fancier than their usual outfits... but not by much. (D) (+6 Cha, +5 Hap)"
				Magic = 0;		Cost = 0;		itemmade = "Pillbox Hat";//                     Desc = "It does look a bit like a gun placement. Without the guns. In an elegant manner. (+5 Cha, adds Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Pink Stole";//                      Desc = "This garment has little to do with the more classy variations, but is quite popular with strippers to extend their show with an additional scene. Its material is strong enough for use in light bondage as well. (BDSM +5, Strip +10, Perf +3, Cha +2, Love -5, Hate +5, removes Elegant.)"
				Magic = 0;		Cost = 0;		itemmade = "Pitch Black Underwear";//           Desc = "This underwear radiates with dark magic. It clings to a girl's breasts and genitals, and only her owner or employer can remove them. Fortunately, the crotch has openings that allow for penetration. The inside surfaces have hundreds of small, ghostly barbs that lengthen and recede at random. When piercing the skin, they inject a venom that causes great pain and an intense rush of arousal. Over time, the wearer's mind begins to equate the pain with strong desire. (+Masochist, +Nymphomaniac, +Twisted, +15 Fear)"
				Magic = 0;		Cost = 0;		itemmade = "Plain Lingerie";//                  Desc = "This simple set of cloth bra and panties is definitely erotic, but it's nothing fancy or special. It's only as pretty as the girl wearing it. (D) (+5 Lib, +2 Cha, +5 all sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Plain Underwear";//                 Desc = "This basic underwear is perfectly functional, although it doesn't do much beyond that. (+3 Happiness)"
				Magic = 0;		Cost = 0;		itemmade = "Rainbow Underwear";//               Desc = "The design of this underwear constantly shifts into complex, extremely colorful patterns. Any girl who wears them will have a much greater outlook on life. As an unusual side effect, the wearer will also begin to find member of the same sex more attractive. (+Optimist, +4 Cha, +20 Happiness, +15 Lesbian)"
				Magic = 0;		Cost = 0;		itemmade = "Royal Gown";//                      Desc = "This opulent gown was crafted by some of the most talented seamstresses in the land. Destined to clad queens and princesses, it bears several precious gemstones of multiple colors and has enough gold thread woven in to feed an entire village for a month. (A) (+24 Cha, +15 Hap, +15 Conf, -20 Agi, -10 Com, removes Fleet of Foot)"
				Magic = 0;		Cost = 0;		itemmade = "School Uniform";//                  Desc = "An elementary or middle school uniform. (+10 Intelligence, Charisma. Adds Quick Learner, Lolita, Cute.)"
				Magic = 0;		Cost = 0;		itemmade = "Sequin Lingerie";//                 Desc = "This skimpy and colorful set of bra and panties is decorated with shimmering sequins. (+15 Strip, +12 Perf, +7 Service, +5 all other sex skills, +7 Cha, +5 Lib)"
				Magic = 0;		Cost = 0;		itemmade = "Sexy Nurse Costume";//              Desc = "Much the same as a regular nurse costume, only not not sexy. Just wearing it makes you feel better equipped to handle medical emergencies. It also makes you want to take it off for some reason. (+7 Cha, +7 Lib, +5 Serv, +7 Strip, +15 Med, +5 Animal Handling, adds Sexy Air)"
				Magic = 0;		Cost = 0;		itemmade = "Sexy Underwear";//                  Desc = "This skimpy underwear has a lace border and is made of quality materials. Overall, it looks quite attractive. (+4 cha, +2 Beauty, +5 Happiness)"
				Magic = 0;		Cost = 0;		itemmade = "Sheer Lingerie";//                  Desc = "A skimpy sheer nightgown and matching underwear. It hides absolutely nothing, but the see-through fabric has that peculiar, classy way of being almost better than naked. (B) (+6 Cha, +10 Lib, +15 all sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Silk Lingerie";//                   Desc = "The cool cloth of this undergarment makes a girl feel sexy. (C) (+5 Lib, +4 Bea, +6 all sex skills)"
				Magic = 0;		Cost = 0;		itemmade = "Silken Dress";//                    Desc = "This gorgeous silk dress is proof that less is sometimes more. It is simple in design, but its light fabric accentuates all of a woman's charms, hiding and revealing exactly as much skin as is desired. (B) (+18 Cha, +34 Lib)"
				Magic = 0;		Cost = 0;		itemmade = "Silver Cape";//                     Desc = "This elegant cape is lined with threads of enchanted silver. It purifies the ambient arcane energy flowing around its wearer, making it easier to cast spells. (C) (+15 Mag, +15 Int, -5 Agi, -5 Fear, -5 Hate, -5 Comb)"
				Magic = 0;		Cost = 0;		itemmade = "Slave Rags";//                      Desc = "Worn rags and crude iron shackles, more for show than actual restraint. Wearing this is an unpleasant and humbling experience, and it certainly does nothing for one's looks. (E) (+5 BDSM, +10 Obed, -10 Conf, -10 Spi, -5 Cha, -40 Refinement, -20 Love, -10 Hap, removes Optimist)"
				Magic = 0;		Cost = 0;		itemmade = "Smarty Pants";//                    Desc = "A small stuffed animal that comes with its own pencil and notepad so he can do his homework. (Happiness +5, Adds Nerd.)"
				Magic = 0;		Cost = 0;		itemmade = "Sombrero";//                        Desc = "She hates you for giving her that, but for some reason, she still always wears it to beach parties, even if it keeps falling in front of her eyes. (Spi +10, Cha +5, adds Clumsy)"
				Magic = 0;		Cost = 0;		itemmade = "Starlit Robes";//                   Desc = "These heavy robes are embroidered with images of various constellations and generate a matrix of astral energy around their wearer. This energy can be used to empower spells and form magical shields. (B) (+20 Mag, +10 Cons, -10 Agi, -2 Cha, -10 Fear, -10 Hate, -25 Comb)"
				Magic = 0;		Cost = 0;		itemmade = "Stockings";//                       Desc = "A simple pair of stockings that can be knee length or thigh length. (+5 Charisma and Beauty)"
				Magic = 0;		Cost = 0;		itemmade = "Strap-on Corset";//                 Desc = "This black leather corset comes with a strap-on for your lesbian slaves. (+1 Charisma, +7 Libido, +10 Confidence, +15 Lesbian, +12 BDSM, + 8 Bestiality, Anal, Handjob, Footjob, and Group)"
				Magic = 0;		Cost = 0;		itemmade = "Striped Socks";//                   Desc = "A pair of cute, striped socks. They come in ankle length, knee length, and thigh length varieties. (Adds Cute.)"
				Magic = 0;		Cost = 0;		itemmade = "Teddy Doll";//                      Desc = "Lingerie that looks more or less like a see through tank top. (Beauty +5. Adds Sexy Air.)"
				Magic = 0;		Cost = 0;		itemmade = "Thong";//                           Desc = "A tiny little thong. (Removes Elegant. Adds Great Arse.)"
				Magic = 0;		Cost = 0;		itemmade = "Tiger Leotard";//                   Desc = "This sexy striped leotard holds a bound tiger spirit, and can give any girl a sleek, feline demeanor. It even enhances her fighting instincts! (A) (+30 Best, +10 all other sex skills, +15 Comb/Lib, +20 Cons, +6 Cha, +5 Spi/Agi/Str, adds Sexy Air and Strange Eyes)"
				Magic = 0;		Cost = 0;		itemmade = "Top Hat";//                         Desc = "With some practice, people can do marvelous tricks with these things, giving more conservative visitors an excuse to watch the shows of your otherwise scantily dressed girls. (Perf +10, Cha/Bea/Agi +5)"
				Magic = 0;		Cost = 0;		itemmade = "Trashy Dress";//                    Desc = "This cheap, gaudy dress shows off too much skin and demonstrates a total lack of class. It's only worn by women who don't know better, and it's only appreciated by men with no taste. (E) (+10 Cha, +10 Lib, +2 Hate, -5 Int, -20 Refinement, removes Elegant)"
				Magic = 0;		Cost = 0;		itemmade = "Velvet Dress";//                    Desc = "This delightfully elegant dress hugs the body like none other. It highlights every curve and its thick, warm fabric hides all but the most severe imperfections. (C) (+12 Cha, +26 Lib, +6 Bea)"
				Magic = 0;		Cost = 0;		itemmade = "Velvet Opera Glove";//              Desc = "A beautiful, elbow-length velvet glove often worn by opera singers and other ladies of class and sophistication. (+10 Cha, +7 Refinement, +1 Bea, +5 Strip, +5 Perf)"
				Magic = 0;		Cost = 0;		itemmade = "White Fedora";//                    Desc = "You wonder how it's possible that the hat which makes you look scruffy, makes her look like a classy photo model. (Int/Bea +5)"
				Magic = 0;		Cost = 0;		itemmade = "White Stole";//                     Desc = "The white stole is less popular than either the black or the pink stole. However, some performers still favor the White Stole because with proper backlighting it gives a nice halo. (BDSM +5, Strip +5, Perf +10, Cha/Bea +3)"
				Magic = 0;		Cost = 0;		itemmade = "White String Bikini";//             Desc = "A classic amongst the fetish set. (+ Sexy air/great figure &amp; arse +30 agil +15 cha +10 happy +5 love -5 fear)"
				Magic = 10;		Cost = 0;		itemmade = "Arcane Glove";//                    Desc = "Gloves like this are created to give mages an easy defence without the inconvenience of traditional weapons. The glove can emit an electric shock designed to travel through clothes and armor, stunning an opponent briefly and giving time for escape. (+8 Magic, +8 Combat, -10 Fear)"
				Magic = 10;		Cost = 0;		itemmade = "Red Swimsuit";//                    Desc = "It's actually a little bit too tight for her, making her boobs almost spill out of the top, while pulling tightly into her nether regions. But, it was a bargain, and it came with that neat little floater she can carry with her while jogging down the beach. (Serv/Med +5, Cha/Bea/Hap +5, adds Big Boobs, Great Figure)"
				Magic = 10;		Cost = 0;		itemmade = "Witch Hat";//                       Desc = "Black pointed hat which is said to have magical properties. (+2 Charisma, +5 Magic)"
				Magic = 10;		Cost = 0;		itemmade = "Witch's Garter";//                  Desc = "Everyone knows witches channel their magic through garters. (+10 magic, +5 Beauty, -5 combat)"
				Magic = 20;		Cost = 0;		itemmade = "Sexy X-Mas Lingerie";//             Desc = "Wow...with this lingerie your girl/lover/mistress will totally look so festive when they go to bed with someone! They also bring out a girl's natural curves. (+20 Mag/Strip/Anal, +30 Serv/Spi, +25 Love/Beauty/Cha/Hap/Lib/Conf, -25 Fear/Hate, +Sexy Air/Great Figure/Optimist, -Pessimist)"
				Magic = 30;		Cost = 0;		itemmade = "Magic Underwear";//                 Desc = "Arcane symbols cover this underwear. It enhances magical abilities and intelligence, and it also looks good on the wearer. (+5 Magic, +4 Cha, +3 Int, +5 Happiness)"
				Magic = 30;		Cost = 0;		itemmade = "Miko Outfit";//                     Desc = "Holy robes of a temple priestess. The white top and poofy red hakama pants are instantly recognizable. (+10 Morality, +6 Cha, -5 Lib, +5 Confidence, +10 Spirit, removes Controlled)"
				Magic = 30;		Cost = 0;		itemmade = "Miko Sleeves";//                    Desc = "A pair of detachable sleeves that usually accompany the traditional outfit of a shrine maiden. (+3 Charisma, +5 Spirit)"
				Magic = 30;		Cost = 0;		itemmade = "Mimi Headband";//                   Desc = "A magical headband that turns your girl into a cat girl. (Adds Cat Girl)"
				Magic = 30;		Cost = 0;		itemmade = "Stockings of the Hooker";//         Desc = "An enchanted pair of stockings once worn by the town's bicycle. (+50 Libido, Confidence, +15 all sex skills. Adds Fast Orgasms, Nymphomaniac. Removes Elegant)"
				Magic = 30;		Cost = 0;		itemmade = "Vexing blindfold";//                Desc = "Confuses and disorients the wearer as well as taking their sight. (+30 obedience/libido +20 bdsm  -lots group/service/magic/combat)"
				Magic = 40;		Cost = 0;		itemmade = "Empress' New Clothes";//            Desc = "This dress is invisible, but no one has the guts to point it out. Handy! (S) (+50 Cha, +75 Lib, +25 Conf, adds Sexy Air)"
				Magic = 40;		Cost = 0;		itemmade = "Enchanted Dress";//                 Desc = "This intricate, gorgeous piece of attire instantly adapts to the form of any girl who puts it on. It's extremely comfortable, and it enhances the appearance of the wearer's body. The color, pattern, and length all change with a simple thought, making it even more desirable. (+24 Charisma, +8 Beauty, +10 Agility, +20 Happiness, +20 Love, +Great Figure)"
				Magic = 40;		Cost = 0;		itemmade = "Faerie Gown";//                     Desc = "This incredibly rare garment bears an otherworldly beauty; its magical origin is visible even to the untrained eye. Woven from the silk of unknown insects and shimmering with ever-shifting rainbow hues, its effect is almost hypnotic. It carries a powerful enchantment that transforms the wearer's body, enhancing their natural looks, and even though it seems heavy and difficult to move in it is actually much lighter than it appears. (S) (+30 Cha, +20 Hap, +15 Conf, +5 Mag, +10 Bea, +10 Lib, -10 Agi, -5 Com)"
				Magic = 50;		Cost = 0;		itemmade = "Apparel of the Automaton";//        Desc = "This looks like a form-fitting, silky outfit with a metallic shine. While attractive, it doesn't change much regarding outward appearance. Internally, it makes some very dramatic changes -- the outfit turns a girl into an emotionless, physically enhanced machine. Normal behavior will be simulated, but complex actions may be difficult. (+Construct, +Tough, +Strong, +Controlled, +Fearless, -Pessimist, -Optimist. -Yandere/Tsundere, -Iron Will, -Broken Will, -Meek, -Nervous, -Clumsy,  +100 Obed, -25 Cha, -40 Int, -Quick Learner, -100 Spirit, -100 Fear/Love/Hate)"
				Magic = 50;		Cost = 0;		itemmade = "Cloak of the Sefirot";//            Desc = "The intricate mystic symbols embroidered in the front of this full-body cloak represent the divine forces of creation. More than mere decoration, they assist the wearer in applying their will to reshape the world as they see fit. (A) (+25 Mag, -10 Agi, -15 Cha, -30 Fear, -15 Hate, -40 Comb)"
				Magic = 50;		Cost = 0;		itemmade = "Corset of Illusion";//              Desc = "When placed around a girl's body, this corset constricts very tightly, accentuating her form. In addition, it creates the illusion of a tight, attractive dress that the wearer can customize at will. The tightness can be rather uncomfortable and makes normal movement more difficult, though. (+15 Cha, +6 Beauty, -25 Agi, +5 Fear, +Great Figure, +Great Arse)"
				Magic = 50;		Cost = 0;		itemmade = "Death Mullet";//                    Desc = "This hairstyle is required to practice Death Kwon Do, unattractive as it may be. (+20 Cmb, +10 Con/Str. Removes Cool Person.)"
				Magic = 50;		Cost = 0;		itemmade = "Death Shorts";//                    Desc = "Cutoff jean shorts that are both stylish and functional. Required attire for practicioners of Death Kwon Do. (Combat +20, Beauty +10. Adds Fleet of Foot.)"
				Magic = 50;		Cost = 0;		itemmade = "Gantz Suit";//                      Desc = "A skintight black suit that enhances both the combat skills and the assets of women who wear it!  Beware of religious statuary, though!  (Combat+40, Strip+20, Cha+25, Libedo+10, Cons+30, Conf+10, Agi+60, Bea+50, Str+20, Fear-25,  Adds Big Boobs, Strong, Adventurer, Great Figure, Great Arse, Fleet of Foot)"
				Magic = 50;		Cost = 0;		itemmade = "Hat of the Mind";//                 Desc = "This hat will give its wearer the smarts to get by. Although the hat looks so goofy, as long as the girl doesn't mind looking weird, she can wear it anywhere. (Serv 10, Int 20, Beau -20, Cha -10) "
				Magic = 60;		Cost = 0;		itemmade = "Undead One Piece";//                Desc = "This swimsuit would look good on a dead body ( or at least someone that looks dead)."
				Magic = 60;		Cost = 0;		itemmade = "Underwear of Arousal";//            Desc = "In order to create this underwear, a very experienced mage has to work for several hours. This causes them to be quite expensive and somewhat uncommon. While worn, they cause a slow, steady magical resonance within the wearers nipples and clitoris. The effect begins to wane after they've been removed for a few hours, but otherwise the wearer will be in a constant state of arousal with a strong desire for sex. (+60 Libido, +Nymphomaniac, +Fast Orgasms, +4 Cha)"
				Magic = 60;		Cost = 0;		itemmade = "Underwear of Endowment";//          Desc = "This set of underwear includes both attractive panties and a large bra. No matter what size breasts a girl has, imbued magic will cause them to grow to a large enough size for the bra to fit. Once removed, the effect will start to dissipate in about 24 hours.(+Big Boobs, +6 Cha, +5 Happiness)"
				Magic = 60;		Cost = 0;		itemmade = "Underwear of Immensity";//          Desc = "This set of underwear is quite similar to underwear of endowment, except the included bra is absurdly large. No matter what size breasts a girl has, imbued magic will cause them to grow to an immense size that will fit appropriately. Once removed, the effect will dissipate in a couple of days. (+Abnormally Large Boobs, +8 Cha, +5 Happiness)"
				Magic = 80;		Cost = 0;		itemmade = "Goron Blessed Swimsuit";//          Desc = "With this baby on, you can swim in lava! ( Why anyone other than a goron would do it, I have no clue....) Plus it gives a strength bonus."
				Magic = 80;		Cost = 0;		itemmade = "Kokiri Blessed Swimsuit";//         Desc = "The power of nature baby! Plus it makes you look a bit younger when you put it on."
				Magic = 80;		Cost = 0;		itemmade = "Succubus' Wardress";//              Desc = "Mostly composed of thin strips of leather, gold and blackened steel, this otherworldly elegant suit of &quot;armor&quot; worn to battle by infernal temptresses seems to be more lingerie than protection. It covers very little of the body, entirely revealing all of the wearer's physical charms, but it carries a powerful warding enchantment that makes it stronger than any mundane plate mail. (S) (+40 Cmb/Cns, +25 Lib/Str, +10 Conf, -10 Fear, +10 to all sex skills, adds Nymphomaniac)"
				Magic = 80;		Cost = 0;		itemmade = "Zora Blessed Swimsuit";//           Desc = "It's blue and allows you to swim underwater for a long time (might as well live in the water). Plus it gives you a slight speed boost."

			}

			if (itemmade == "")	{ Magic = 0; Cost = 10; itemmade = "Eye patch"; }//             Desc = "Who says that you have to actually lose an eye to be able to wear this? (Adds Eye Patch)"
			item = g_InvManager.GetItem(itemmade);
			if (item)
			{
				craftpoints -= Cost;
				girl->mana(-Magic);
				msgtype = EVENT_GOODNEWS;
				if (numitems == 0)	ss << "\n\n" << girlName << " made:";
				ss << "\n\t" << itemmade;
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


	// `J` - Finish the shift - Tailor

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);
	if (tired > 0) girl->tiredness(tired);

	// Money
	if (wages < 0)	wages = 0;	girl->m_Pay = (int)wages;
	if (tips < 0)	tips = 0;	girl->m_Tips = (int)tips;

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary improvement (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, (g_Dice % skill) + 1);
	g_Girls.UpdateStat(girl, STAT_STRENGTH, (g_Dice % skill) + 1);
	// secondary improvement (-1 for one then -2 for others)
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_Tailor(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->crafting() +
		// secondary - second 100
		((girl->service() + girl->intelligence() + girl->magic()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
	// traits modifiers


	return jobperformance;
}
