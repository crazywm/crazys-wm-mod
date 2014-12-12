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

#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cHouse.h"
#include "cFarm.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

extern cMovieStudioManager  g_Studios;
extern cArenaManager		g_Arena;
extern cClinicManager		g_Clinic;
extern cCentreManager		g_Centre;
extern cHouseManager		g_House;
extern cFarmManager			g_Farm;

// `J` Brothel Job - General
bool cJobManager::WorkFreetime(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	// `J` NOTE: start with just the basic stuff
	stringstream ss;
	string girlName = girl->m_Realname;

	int U_Tiredness = -(10 + g_Dice % 21);
	int U_Health = 10 + (girl->constitution() / 10);
	int U_Happiness = 10 + g_Dice % 11;
	int U_Mana = 5 + girl->magic() / 5;
	int U_Libido = (g_Girls.HasTrait(girl, "Nymphomaniac") ? 15 : 5);
	int U_EXP = 1;	// Just because!
	int U_Beauty = 0;
	int U_Money = 0;
	int U_Morality = 0;

	// `J` NOTE: one message to tell she is resting - use a separate one to tell the anything else she does
	ss << girlName << gettext(" rested and recovered some energy.");
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	int imagetype = IMGTYPE_PROFILE;
	ss.str("");

	int roll = g_Dice.d100();
	int roll_a = g_Dice.d100();
	int roll_b = g_Dice.d100();
	int roll_c = g_Dice.d100();



	if (g_Dice % 2 != 1)	// half of the time she will just stay home and rest
	{
#if 1	// `J` try a different way of choosing what to do

		/*	First we give her all the possible choices in the freetimechoice enum
		*	start each name with "FT_"
		*	Don't assign anything to the names, the enum will do that for you.
		//*/
		enum freetimechoice
		{
			FT_Nothing,			// do nothing special, standard free time only
			FT_Shopping,		// if she has any money she will go shopping
			FT_WindowShopping,	// if she does not have any money or does spend any money
			FT_BuyDrugs,		// if she is an addict or unhappy, she may turn to drugs
			FT_Bath,			// take a bath
			FT_Bed,				// stay in bed
			FT_Church,			// go to church - possibly update to Morality
			FT_Salon,			// go to the salon - checks if the girl has enough money
			FT_Pool,			// go to the local public pool
			FT_Cook,			// cook herself dinner
			FT_ClinicCheckup,	// go to the Clinic
			FT_ClinicVisit,		// go to the Clinic to visit

			// Crazy started adding these but didn't finish them yet
			FT_WatchMovie,		// go to see a movie
			FT_Concert,			// go see a concert
			FT_Picnic,			// go on a picnic
			FT_VisitBar,		// go to the bar
			FT_Club,			// go to a dance club
			FT_Quest,			// go on a quest
			FT_Hobby,			// does she have a hobby?

			// Suggestions to be added
			FT_Counseling,		// she goes to counseling
			FT_WatchFights,		// she goes to the arena as a spectator



			FT_NumberOfFreeTimeChoices	// Leave this as the last thing on the list to allow for random choices.
		};								// When the choice gets handled, the switch will use the "FT_name" as the case.

		// `J` the test for if the girl can act on that choice is done next
		int choice = 0;	bool choicemade = false;

#if 0	// change this to 1 and add your choice to debug a choice
		
		choice = FT_ClinicVisit;
		choicemade=true;
#endif

		while (!choicemade)
		{
			// First check if there are some things she will more likely do
			if (girl->health() < 30 && g_Clinic.GetNumBrothels() > 0)	// if she is in bad health and you own a clinic
			{
				choice = FT_ClinicCheckup;
				choicemade = true;
			}
			else if (girl->is_addict() && girl->happiness() < 80)
			{
				choice = FT_BuyDrugs;
				choicemade = true;
			}


			else	// If none of the above take precedence, randomly choose
			{

				choice = g_Dice % FT_NumberOfFreeTimeChoices;	// randomly choose from all of the choices
				switch (choice)
				{
				// these don't need a test
				case FT_Bath:
				case FT_Bed:
				case FT_Church:
				case FT_Pool:
				case FT_Cook:
				case FT_WindowShopping:
				case FT_ClinicVisit:
					choicemade = true;	// ready so continue
					break;

					// if there are restrictions for a choice, check them here
				case FT_Shopping:
					if (girl->m_Money >= 10)
					{
						choicemade = true;	// She has enough money for it, so continue
					}
					else
					{
						choice = FT_WindowShopping;
						choicemade = true;	// She does not have enough money for it but looking is free
					}
					// if she can not afford it, reroll.
					break;
				case FT_Salon:
					if (girl->m_Money >= 10)
					{
						choicemade = true;	// She has enough money for it, so continue
					}
					// if she can not afford it, reroll.
					break;
				case FT_BuyDrugs:
					if (girl->is_addict() || (girl->happiness() < 20 && !g_Dice.percent(girl->intelligence())))
					{
						choicemade = true;	// if she is an addict or unhappy, she may turn to drugs
					}
					break;
				case FT_ClinicCheckup:
					if (g_Clinic.GetNumBrothels() > 0)
					{
						choicemade = true;	// if you own a Clinic, she will go
					}
					else if (girl->m_Money >= 25)
					{
						choicemade = true;	// if you don't own a Clinic, she will go if she has enough money to pay 25 for a checkup
					}
					else if (girl->morality() > 50)
					{
						choice = FT_ClinicVisit;
						choicemade = true;	// if she is a good girl, she will go to help out
					}
					break;


					// These are not ready so reroll.
				case FT_WatchMovie:
				case FT_Concert:
				case FT_Picnic:
				case FT_VisitBar:
				case FT_Club:
				case FT_Quest:
				case FT_Hobby:
				case FT_Counseling:
				case FT_WatchFights:
					break;


					// anything not in the list gets changed to FT_Nothing and continues
				case FT_Nothing:
				default:
					choice = FT_Nothing;
					choicemade = true;	// ready so continue
					break;
				}
			}
		};


		// `J` Finally, let her do what she chooses.
		switch (choice)
		{
		case FT_Bath:
		{
			ss << girlName << " took a bath.\n";
			imagetype = IMGTYPE_BATH;
			U_Happiness += 5;
			U_Health += 5;
			if (girl->libido() > 70 || (g_Girls.HasTrait(girl, "Nymphomaniac") && girl->libido() > 30))
			{
				ss << "While in the tub the mood hit her and she proceed to pleasure herself.\n";
				imagetype = IMGTYPE_MAST;
				U_Libido -= 15;
				U_Happiness += 5;
			}
			else if (girl->tiredness() > 80)
			{
				ss << "She fell asleep in the tub and woke up in the cold water.\n";
				U_Health -= 2;
			}
			else
			{
				ss << "She enjoyed a nice long soak.\n";
				U_Tiredness -= 5;
			}
		}
		break;	// end FT_Bath

		case FT_Bed:
		{
			ss << girlName << " stayed in bed most of the day.\n";
			imagetype = IMGTYPE_BED;
			U_Health += 10;
			U_Tiredness -= 10;
			if (girl->libido() > 70 || (g_Girls.HasTrait(girl, "Nymphomaniac") && girl->libido() > 30))
			{
				ss << "While in bed the mood hit her and she proceed to pleasure herself.\n";
				imagetype = IMGTYPE_MAST;
				U_Libido -= 15;
				U_Happiness += 5;
			}
		}
		break;	// end FT_Bed

		case FT_Salon:
		{
			// add more options for more money
			ss << girlName << " had her hair and nails done today. She is going look better for a few days.\n";
			U_Beauty += 5;
			U_Money -= 10;
		}
		break;	// end FT_Salon

		case FT_Church:
		{
			ss << girlName << " had some free time so she went to church.\n";
			/* `J` use U_Morality to increase or decrease Morality
			*	Try to make multiple options that either increase or decrease morality
			*	right now there is not much done with morality so anything that can be added would be good.
			*	for each morality span (<-80, <-60 ... >80) make atleast one of each "add", "no change" and "reduce" option.
			//*/
			if (g_Girls.GetStat(girl, STAT_MORALITY) >= 80)
			{
				if (roll <= 33)
				{
					ss << girlName << " prayed for a couple of hours just to actively participate in the mass. Afterwards she helped clean up the temple grounds.\n";
					U_Morality += 5;
				}
				else if (roll <= 66)
				{
					ss << "She spent almost her entire free time praying! But from her line of work, she got used to being on her knees for long hours.\n";
				}
				else
				{
					ss << "Being at the sanctuary for her whole free time, she could swear that she noticed a presence of some sort of holy being.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 60)
			{
				if (roll <= 33)
				{
					ss << girlName + " confessed her shameful actions that she did at work, asking for the God's forgiveness.\n";
				}
				else if (roll <= 66)
				{
					ss << "She humbly sat in the last row. Focused and with a lot of interest, she listened to the whole mass.\n";
				}
				else
				{
					ss << "Today she cleaned up a road side shrine and decorated it with fresh flowers.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 40)
			{
				if (roll <= 33)
				{
					ss << girlName + " starts to find this way of spending free time interesting. Wanting to know more, she listens intently to the preacher.\n";
				}
				else if (roll <= 66)
				{
					ss << "After participating in today's ceremony she felt happier.\n";
					U_Happiness += 5;
				}
				else
				{
					ss << "Being late, she tried to find a place to sit. Happily, she noticed some free seats on the other side of the church. Unfortunately her high heels were knocking pretty loudly while walking on the church's stone floor, disturbing the silent prayers of the congregation.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 20)
			{
				if (roll <= 33)
				{
					ss << girlName + " made a quick stop at the local temple for a short prayer before heading further into town.\n";
				}
				else if (roll <= 66)
				{
					ss << "On her way back, she gave a short prayer in front of a road side shrine.\n";
				}
				else
				{
					ss << "After eavesdropping on a couple of girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls were right about this place.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -20)
			{
				if (roll <= 33)
				{
					ss << girlName + " tried to listen to the preachers lecture, but she fell asleep halfway thru!\n";
				}
				else if (roll <= 66)
				{
					ss << "She was about to enter the sanctuary but she noticed a really cute kitten. She spent the time playing with the cat.\n";
				}
				else
				{
					ss << "After eavesdropping couple girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls were wrong about this place. Being bored, she left in the middle of the mass.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -40)
			{
				if (roll <= 33)
				{
					ss << girlName + " threw some special weeds from your farm into a goblet of fire, standing in front of one of the side altars. Later on, most of praying at the altar swear that they saw angels!\n";
				}
				else if (roll <= 66)
				{
					ss << "At the church, she noticed a really young priest hearing to today's confessions. Feeling mischievous she entered the confessional. After confessing her sins in great detail, she proposed to recreate them with him! The abashed youngster gave her a couple of prayers as penance and left right after.\n";
				}
				else
				{
					ss << "Getting bored at the mass, she started to whisper things to a man sitting next to her, not bothering with the fact that his wife was sitting next to him!\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -60)
			{
				if (roll <= 33)
				{
					ss << girlName + " entered the confessional. Whispering sensually of her job experiences, all in great detail, she made the priest have a heart attack! When the man was squirming in pain on the floor, she left the temple laughing.\n";
				}
				else if (roll <= 66)
				{
					ss << "Dressed really slutty, she strode down the center aisle of the church, her high heals echoing throughout the place. She took a seat in the first row and sat in a pose to best expose her pussy to all priests on the altar performing their Holy Mass.\n";
				}
				else
				{
					ss << "In front of a temple she approached a young monk. After a brief chat about god, faith and salvation she gave him a proposal. Claiming that it was such waste for such young and handsome man to live in chastity, she proposed he could spend some quality, fun time with her. The man quickly ran inside whispering some kind of mantra, while " << girlName << " went her own way laughing.\n";
				}
			}
			else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -80)
			{
				if (roll <= 33)
				{
					ss << girlName + " came to the temple with one of girls working for you, but her companion flew right after seeing what kind place of worship this was. No human sacrifice provided by " + girlName + " today, sorry.\n";
				}
				else if (roll <= 66)
				{
					ss << "Scheming against the church, she came with a brilliant idea. Promising local young thugs to repay a favor with her body, she made them to assault and beat up a group of priests. Not waiting for the outcome of the brawl, she disappeared not holding up her end of the deal.\n";
				}
				else
				{
					ss << "Not liking the architectural style of the temple she decided to do something about it. Unfortunately for her, the fire was extinguished fairly quickly.\n";
				}
			}
			else
			{
				if (roll <= 33)
				{
					ss << girlName + " was on her way to pray in the local temple, but on the way there, she saw a beautiful bag on display. After entering the shop, she spent several hours, picking out the perfect bag for herself before returning.\n";
				}
				else if (roll <= 66)
				{
					ss << "Attending the mass, she felt somewhat bored and not focused on the topic. She even yawned a couple of times, fighting not to fell asleep.\n";
				}
				else
				{
					ss << "She visited the local church feeling somehow down. Listening to the preacher didn't have much impact on improving her mood.\n";
				}
			}
		}
		break;	// end FT_Church

		case FT_Pool:
		{
			imagetype = IMGTYPE_SWIM;
			U_Happiness += 5;
			ss << girlName << " went to the local pool.\n";
			if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 35)
			{
				ss << "Being on the tired side, she just decided to lay around the pool and get some sun.  She is going to have a tan for a few days.\n";
				U_Beauty += 5;
				if (g_Dice.percent(10))
				{
					bool success = false;
					bool fight = false;
					ss << "A rather brave young man saw her laying there and decide to try and remove her top.";
					if (g_Girls.HasTrait(girl, "Psychic"))
					{
						ss << " But using her Psychic skills she stopped him before he could do it.";
					}
					else if (!g_Dice.percent(girl->agility()) && g_Dice.percent(40))
					{
						ss << " He succeed and was able to expose her ";
						if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits")) { ss << "gigantic boobs."; }
						else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) { ss << "large breasts."; }
						else if (g_Girls.HasTrait(girl, "Small boobs") || g_Girls.HasTrait(girl, "Petite Breasts")) { ss << "small tits."; }
						else if (g_Girls.HasTrait(girl, "Flat Chest")) { ss << "rather Flat Chest."; }
						else { ss << "breasts."; }
						imagetype = IMGTYPE_NUDE;
						success = true;
					}
					else
					{
						ss << " He missed his oppertunity and ran away with her towel.";
					}
					if (g_Girls.HasTrait(girl, "Aggressive") || g_Girls.HasTrait(girl, "Assassin") ||
						g_Girls.HasTrait(girl, "Brawler") || g_Girls.HasTrait(girl, "Fearless") ||
						g_Girls.HasTrait(girl, "Merciless") || g_Girls.HasTrait(girl, "Tough") ||
						g_Girls.HasTrait(girl, "Tsundere") || g_Girls.HasTrait(girl, "Yandere"))
					{
						fight = true;
						ss << "\n " << girlName << " chased him down and beat the crap out of him. She took back her ";
						if (g_Girls.HasTrait(girl, "Exhibitionist")) { ss << (success ? "top" : "towel") << " and"; }
						else { ss << (success ? "top, put it back on then " : "towel and "); }
						ss << " kicked him in the balls.";
					}
					if (success && g_Girls.HasTrait(girl, "Exhibitionist"))
					{
						ss << "\n " << girlName << " continused with her day as if nothing had happened, topless.";
					}
				}
			}
			else
			{
				ss << "She took a dive into the pool and swam for awhile enjoying herself.\n";
			}
		}
		break;	// end FT_Pool

		case FT_Cook:
		{
			imagetype = IMGTYPE_COOK;
			ss << girlName << " decided to cook a meal.\nThe meal she cooked was ";
			if (g_Girls.GetSkill(girl, SKILL_SERVICE) >= 85)//use service for now on how well she can cook.. if cooking skill ever gets added can be changed then
			{
				ss << "amazing. She really knows how to cook.\n";
				U_Health += 5;
			}
			else if (g_Girls.GetSkill(girl, SKILL_SERVICE) > 50)
			{
				ss << "pretty good. She isn't the best but with some work she could be.\n";
				U_Health += 2;
			}
			else if (g_Girls.GetSkill(girl, SKILL_SERVICE) > 10)
			{
				ss << "plain. She really needs some practice at this.\n";
			}
			else
			{
				ss << "awful. It can't really be called food it was so bad.\n";
				U_Health -= 2;
			}
			g_Girls.UpdateSkill(girl, SKILL_SERVICE, 1);
		}
		break;	// end FT_Cook

		case FT_BuyDrugs:	// `J` zzzzzz - this section needs work
		{
			// 1. if she is addicted she will first attempt to purchase drugs until she has no money
			if (g_Girls.HasTrait(girl, "Viras Blood Addict") ||
				(girl->happiness() < 20 && g_Dice.percent(10)))	// 10% chance an unhappy girl will try this
			{
				AddictBuysDrugs("Viras Blood Addict", "Viras Blood", girl, brothel, Day0Night1);
				break;
			}
			if (g_Girls.HasTrait(girl, "Fairy Dust Addict") ||
				(girl->happiness() < 20 && g_Dice.percent(50)))	// 50% chance an unhappy girl will try this
			{
				AddictBuysDrugs("Fairy Dust Addict", "Fairy Dust", girl, brothel, Day0Night1);
				break;
			}
			if (g_Girls.HasTrait(girl, "Shroud Addict") || (girl->happiness() < 20))
			{
				AddictBuysDrugs("Shroud Addict", "Shroud Mushroom", girl, brothel, Day0Night1);
				break;
			}
		}
		break;	// end FT_BuyDrugs

		case FT_WindowShopping:
		{
			int hap = g_Dice % 10 - 4;
			U_Happiness += hap;
			ss << girlName << " wandered around the shops trying things on that she knew she could not afford. Though she wasted the shop girl's time, ";
			if (hap > 0) ss << " she enjoyed herself a " << (hap > 3 ? "bit." : "lot.");
			else ss << " she was sad because she didn't find what she liked for a price she could afford.";
		}
		break;	// end FT_WindowShopping

		case FT_Shopping:
		{
			// 2. buy any items that catch her fancy
			int numberToBuy = g_Dice % 10;	// try to buy up to 10 things
			int itemsBought = 0;
			string buyList = "";

			for (int i = 0; i < numberToBuy && girl->m_NumInventory < 40; i++)
			{
				int item = g_InvManager.GetRandomShopItem();

				int cost = g_InvManager.GetShopItem(item)->m_Cost;
				string itemName = g_InvManager.GetShopItem(item)->m_Name;

				if (g_Girls.HasItem(girl, itemName) > -1	// if she already has one
					|| girl->m_Money - cost < 0) 			// or if she can't afford it
					continue;								// skip it

				if (g_Dice.percent(g_InvManager.GetShopItem(item)->m_GirlBuyChance))
				{
					int type = g_InvManager.GetShopItem(item)->m_Type;
					bool equip = true;
					int num = 1;
					if (type == INVARMBAND || type == INVSMWEAPON)
					{

						if (g_Girls.HasTrait(girl, "Adventurer") || g_Girls.HasTrait(girl, "Assassin") ||
							girl->combat() >= 50 || girl->magic() >= 50)
						{
							int magic = 0, combat = 0;
							num = 0;
							for (u_int j = 0; j < g_InvManager.GetShopItem(item)->m_Effects.size(); j++)
							{
								sEffect* curEffect = &g_InvManager.GetShopItem(item)->m_Effects[j];
								if (curEffect->m_EffectID == SKILL_COMBAT)	combat = curEffect->m_Amount;
								if (curEffect->m_EffectID == SKILL_MAGIC)	magic = curEffect->m_Amount;
							}
							if ((girl->combat() >= girl->magic() && combat >= magic) ||	// if the girl favors combat and the item favors combat
								(girl->magic() >= girl->combat() && magic >= combat))	// or if the girl favors magic and the item favors magic
							{
								num = 2;	// buy it
							}
						}
						else	// if she isn't a warrior type she probably won't buy weapons or armor
						{
							num = 0;
						}
					}
					if (type == INVRING) num = 8;
					if (type == INVFOOD || type == INVMAKEUP || type == INVMISC)
					{
						num = 15; equip = false;
					}

					if (num > 0 && g_InvManager.GirlBuyItem(girl, item, num, equip))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}     // if buy

				if (itemsBought >= 3) i += numberToBuy;	// if she already has 3 items stop shopping
			}         // for # buy chances

			if (itemsBought > 0)
			{
				U_Happiness += 5 + (itemsBought * 2);
				ss << " She did some shopping, and bought: " << buyList << ".";
				imagetype = IMGTYPE_SHOP;
			}
			else
			{
				int hap = g_Dice % 10 - 4;
				U_Happiness += hap;
				ss << girlName << " wandered around the shops trying things on. Though she wasted the shop girl's time, ";
				if (hap > 0) ss << " she enjoyed herself a " << (hap > 3 ? "bit." : "lot.");
				else ss << " she was sad because she didn't find what she liked for a price she could afford.";

			}
		}
		break;	//end FT_Shopping

		case FT_ClinicVisit:
		{
			ss <<girlName<< " goes to the Clinic to cheer up the patients.";
			U_Happiness += 5;
			U_Morality += 5;
			// needs more options
		}
		break;	// end FT_ClinicVisit

		case FT_ClinicCheckup:
		{
			bool playerclinic = g_Clinic.GetNumBrothels() > 0;
			sGirl* doctoronduty = NULL;
			sGirl* nurseonduty = NULL;
			string doctorname = "the Doctor";	// Who?
			string nursename = "the Nurse";
			if (playerclinic)
			{
				vector<sGirl *> doctors = g_Clinic.GirlsOnJob(0, JOB_DOCTOR, Day0Night1);
				if (doctors.size() > 0) doctoronduty = doctors[g_Dice%doctors.size()];
				if (doctoronduty)	doctorname = "Doctor " + doctoronduty->m_Realname + "";
				else doctorname = "";	// no doctor
				vector<sGirl *> nurses = g_Clinic.GirlsOnJob(0, JOB_NURSE, Day0Night1);
				if (nurses.size() > 0) nurseonduty = nurses[g_Dice%nurses.size()];
				if (nurseonduty) nursename = "Nurse " + nurseonduty->m_Realname + "";
				else nursename = "";
			}

			if ((playerclinic && (doctoronduty || nurseonduty)) || girl->m_Money >= 25)	// get a checkup
			{
				ss << girlName << " went to the Clinic for a checkup.\n";
				if (g_Girls.HasTrait(girl, "AIDS") || g_Girls.HasTrait(girl, "Chlamydia") || g_Girls.HasTrait(girl, "Herpes") || g_Girls.HasTrait(girl, "Syphilis"))
				{
					if (doctorname.size() > 0) ss << doctorname << " told her";
					else if (nursename.size() > 0) ss << nursename << " told her";
					else ss << "She was told";
					ss << " she has an STD." << (girl->is_pregnant() ? " Her unborn child will most likely be born with it if she can't find a way to cure it" : "") << ".\n";
					U_Happiness -= 5;
					ss << "Other than that, ";
				}
				if (g_Girls.GetStat(girl, STAT_HEALTH) >= 90)
				{
					ss << "Her check up went wonderful. She was told she was in near perfect health" << (girl->is_pregnant() ? " and her unborn child was growing as expected" : "") << ".\n";
					U_Happiness += 5;
				}
				else if (g_Girls.GetStat(girl, STAT_HEALTH) >= 70)
				{
					ss << "Her check up went well. She was told she was in good health" << (girl->is_pregnant() ? " and her unborn child was doing well" : "") << ".\n";
				}
				else if (g_Girls.GetStat(girl, STAT_HEALTH) >= 50)
				{
					ss << "Her check up was routine. She was told she was in fair health" << (girl->is_pregnant() ? " and she needs to start taking better care of herself for her babys sake" : "") << ".\n";
				}
				else if (g_Girls.GetStat(girl, STAT_HEALTH) >= 20)
				{
					ss << "Her check up went poorly. She was told she was in bad health" << (girl->is_pregnant() ? " and that they are concerned for her unborn childs saftey" : "") << ".\n";
					U_Happiness -= 5;
				}
				else
				{
					ss << "Her check up took a long time. She was in such bad health ";
					if (doctorname.size() > 0) ss << doctorname;
					else if (nursename.size() > 0) ss << nursename;
					else ss << "it was";
					ss << " recommended that she get checked into the Clinic for full time treatment.\n";
					if (girl->is_pregnant()) ss << "If she remains in such grave condition, her baby may not survive.\n";
				}

				if (!playerclinic)	// if she goes to a clinic not owned by you...
				{
					int cost = 25;		// base cost for the checkup
					if (girl->health() + U_Health <= 90)
					{
						ss << "The Doctor decides to give her a booster shot.\n";
						cost += 25;		// additional cost fot 10 healing
						U_Health += 10;
					}
					ss << "\n" << girlName << " was charged " << cost << " gold for her checkup";
					if (girl->m_Money - cost < 0)
					{
						ss << " but she did not have enough money. The Doctor unzipped his pants and allowed her to work in off";
						imagetype = IMGTYPE_ORAL;
					}
					else
					{
						U_Money -= cost;
					}
					ss << ".\n";
				}
				else if ((doctoronduty || nurseonduty) && girl->health() + U_Health <= 90)
				{
					ss << (doctoronduty ? doctorname : nursename) << " gives her a booster shot.";
					U_Health += 10;
				}
			}
		}
		break;	// end FT_ClinicCheckup


		case FT_WatchMovie:
			break;	// end FT_WatchMovie	
		case FT_Concert:
			break;	// end FT_Concert
		case FT_Picnic:
			break;	// end FT_Picnic		
		case FT_VisitBar:
			break;	// end FT_VisitBar
		case FT_Club:
			break;	// end FT_Club
		case FT_Quest:
			break;	// end FT_Quest
		case FT_Hobby:
			break;	// end FT_Hobby
		case FT_Counseling:
			break;	// end FT_Counseling
		case FT_WatchFights:
			break;	// end FT_WatchFights


		case FT_Nothing:
		default:
			// nothing is done so do nothing :p
			break;
		}

		// `J` end of the line














#else

		int v[2] = { -1, -1 };
		girl->m_Triggers.CheckForScript(TRIGGER_SHOPPING, true, v);	// check for and trigger shopping scripts

		if (girl->m_Money < 1 || girl->m_NumInventory >= 40)
		{
			if (g_Girls.GetStat(girl, STAT_TIREDNESS) >= 80) { roll_b = 90; }
			if (g_Girls.GetStat(girl, STAT_HEALTH) <= 30)   { roll_b = 90; }
			if (g_Girls.HasTrait(girl, "Adventurer"))	   { roll_a += 30; }
			/* if (roll_a <= 75)	{ general = true; }
			else if (roll_a <= 90)	{ quest = true; }*/

			/*May be best to handle this differently then this.  Could have it where one of the rolls is do hobby.
			Hobby would be like girl goes on adventurer or nympho looks for sex or nerd goes to libary or look for fight
			May make it much easier then all of this.

			Try and figure out how you could make the girls apper to talk to each other..  Figure just have it look for girl
			on certain jobs.  Have it like girl talks to the barmaid while she works... not sure how to do this yet.*/

			// `CRAZY` This is general things any girl can do
			/*default*/	int gen_type = 1;    string gen_type_text = "stayed in bed most of the day";
			/* */if (roll_b <= 6)	{ gen_type = 12; gen_type_text = "went to the local temple"; }
			else if (roll_b <= 14)	{ gen_type = 11; gen_type_text = "went to the clinic for a checkup"; }
			else if (roll_b <= 22)	{ gen_type = 10; gen_type_text = "went to a salon"; }
			else if (roll_b <= 30)	{ gen_type = 9; gen_type_text = "went to have a picnic"; }
			else if (roll_b <= 38)	{ gen_type = 8; gen_type_text = "decided to do one of her hobby's today"; }
			else if (roll_b <= 46)	{ gen_type = 7; gen_type_text = "went to a local concert"; }
			else if (roll_b <= 54)	{ gen_type = 6; gen_type_text = "took a bath"; }
			else if (roll_b <= 62)	{ gen_type = 5; gen_type_text = "went to the local pool"; }
			else if (roll_b <= 70)	{ gen_type = 4; gen_type_text = "decided to cook a meal"; }
			else if (roll_b <= 78)	{ gen_type = 3; gen_type_text = "went to the movies"; }
			else if (roll_b <= 86)	{ gen_type = 2; gen_type_text = "went to the club"; }
			else if (roll_b >= 94)	{ gen_type = 0; gen_type_text = "went to the bar"; }
			//add country side, casino, stroll in the city, go watch a fight at the arena
			//casino is gone till i figure out a way for them to actually loose or gain money

			// `CRAZY` This is things she can explore on a quest
			/*default*/	int adv_type = 1;    string adv_type_text = "a cave";
			/* */if (roll_b <= 20)	{ adv_type = 4; adv_type_text = "the local forest"; }
			else if (roll_b <= 40)	{ adv_type = 3; adv_type_text = "the country side"; }
			else if (roll_b <= 60)	{ adv_type = 2; adv_type_text = "an old temple"; }
			else if (roll_b >= 80)	{ adv_type = 0; adv_type_text = "a local mountain"; }

			// `CRAZY` This is movies she can watch
			/*default*/	int mov_type = 1;    string mov_type_text = "a porno";
			/* */if (roll_c <= 20)	{ mov_type = 4; mov_type_text = "a romance movie"; }
			else if (roll_c <= 40)	{ mov_type = 3; mov_type_text = "a comedy"; }
			else if (roll_c <= 60)	{ mov_type = 2; mov_type_text = "a scary movie"; }
			else if (roll_c >= 80)	{ mov_type = 0; mov_type_text = "an action film"; }

			// `CRAZY` The type of music at the concert
			/*default*/	int song_type = 1;    string song_type_text = "Death Metal";
			/* */if (roll_c <= 14)	{ song_type = 6; song_type_text = "Goth Rock"; }
			else if (roll_c <= 28)	{ song_type = 5; song_type_text = "Classical"; }
			else if (roll_c <= 42)	{ song_type = 4; song_type_text = "Metal"; }
			else if (roll_c <= 56)	{ song_type = 3; song_type_text = "Rock"; }
			else if (roll_c <= 70)	{ song_type = 2; song_type_text = "Country"; }
			else if (roll_c >= 88)	{ song_type = 0; song_type_text = "Pop"; }



















			if (quest) //this may just turn into a job.. hate to have a girl get hurt when on free time
			{
				if (g_Girls.HasTrait(girl, "Adventurer"))
				{
					message += girlName + " went out on a quest to explore " + adv_type_text + ".\n";
					if (roll_b <= 20) //forest
					{
					}
					else if (roll_b <= 40) //country side
					{
					}
					else if (roll_b <= 60) //old temple
					{
					}
					else if (roll_b <= 80) //cave
					{
					}
					else //mountain
					{
					}
				}
				else
				{
					general = true;
				}
			}
			else if (general)
			{
				message += girlName + " had some free time so she " + gen_type_text + ".\n";
				if (roll_b <= 6) //temple
				{
					if (g_Girls.GetStat(girl, STAT_MORALITY) >= 80)
						if (roll <= 33)
						{
							message += girlName + " prayed for a couple of hours just to actively participate in the mass. Afterwards she helped cleaning the temple grounds.\n";
						}
						else if (roll <= 66)
						{
							message += "She spent almost her entire free time praying! But from her line of work, she got used to being on her knees for long hours.\n";
						}
						else
						{
							message += "Being at the sanctuary for her whole free time, she could swear that she noticed a presents of some sort of holly being.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 60)
						if (roll <= 33)
						{
							message += girlName + " confessed her shameful actions that she did at work, asking for the God's forgiveness.\n";
						}
						else if (roll <= 66)
						{
							message += "She humbly sit down in the last row. Focused and with lot of interest, she listening to the whole mass.\n";
						}
						else
						{
							message += "Today she cleaned up a road side shrine and decorating it with fresh flowers.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 40)
						if (roll <= 33)
						{
							message += girlName + " starts to find this way of spending free time being interesting. Wanting to know more she listens to the preacher.\n";
						}
						else if (roll <= 66)
						{
							message += "After participating in today's ceremony she felt happier.\n"; happy += 5;
						}
						else
						{
							message += "Being late, she tried to find a place to sit. Happily she noticed some free sits on the other side of the church. Unfortunately her high heels were knocking pretty loud while walking on the churches stone floor, disturbing that part of the mass with silence prays.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) >= 20)
						if (roll <= 33)
						{
							message += girlName + " made a quick stop at the local temple for a short pray before heading further into town.\n";
						}
						else if (roll <= 66)
						{
							message += "On her way back, she gave a short pray  in front of a road side shrine.\n";
						}
						else
						{
							message += "After eavesdropping on a couple of girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls was right about this place.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -20)
						if (roll <= 33)
						{
							message += girlName + " tried to listen to the preachers lecture, but she fell asleep halfway thru!\n";
						}
						else if (roll <= 66)
						{
							message += "She was about to enter the sanctuary but she noticed a really cut kitten. She spent the time playing with the cat.\n";
						}
						else
						{
							message += "After eavesdropping couple girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls was wrong about this place. Being bored she left in the middle of the mass.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -40)
						if (roll <= 33)
						{
							message += girlName + " threw some special weeds from your farm into a goblet of fire, standing in front of one of the side altars. Later on, most of praying at the altar swear that they saw angels!\n";
						}
						else if (roll <= 66)
						{
							message += "At the church, she noticed a really young priest hearing to today's confessions. Feeling mischievous she entered the confessional. After confessing her sins in high detail, she proposed to recreate them with him! The abashed youngster gave her a couple of prays as penance and left right after.\n";
						}
						else
						{
							message += "Getting bored at the mass, she started to whisper things to a man sitting next to her, not bothering with the fact that his wife was sitting next to him!\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -60)
						if (roll <= 33)
						{
							message += girlName + " entered the confessional. Whispering sensually of her job experiences, all in high details, she made the priest have a heart attack! When the man was squirming in pain on the floor, she left the temple laughing.\n";
						}
						else if (roll <= 66)
						{
							message += "Dressed really slutty, she parade the mid lane of the church, wearing high hills that were heard all over the place, just to take a sit in the first row. The pose she had sat in, was to expose her pussy to all priests on the altar performing the holy mass.\n";
						}
						else
						{
							message += " In front of a temple she approached a young monk. After a brief chat about god, faith and salvation she gave him a proposal. Claiming that it was such waste for such young and handsome man to live in chastity, she proposed he could spend some quality, fun time with her. The man quickly run inside whispering some kind of manta, while the girl went her own way laughing.\n";
						}
					else if (g_Girls.GetStat(girl, STAT_MORALITY) <= -80)
						if (roll <= 33)
						{
							message += girlName + " came to the temple with one of girls working for you, but her companion flew right after seeing what kind place of worship this was. No human sacrifice provided by " + girlName + " today, sorry.\n";
						}
						else if (roll <= 66)
						{
							message += "Scheming against the church, she came with a brilliant idea. Promising local, young tugs to repay a favor with her body, she made them to assault and beat up a group of priests. Not waiting for the outcome of the brawl, she disappear not holding to her end of the deal.\n";
						}
						else
						{
							message += "Not liking the architecture style of the temple she decided to do something about that. Unfortunately for her, the fire was extinguished fairly quickly.\n";
						}
					else
						if (roll <= 33)
						{
							message += girlName + " was on her way to pray in the local temple, but on the way there, she saw a beautiful bag on display. After entering the shop, she spent several hours, picking out the perfect bag for herself before returning.\n";
						}
						else if (roll <= 66)
						{
							message += "Attending the mass she felt some how bored and not focused on the topic. She even yawn couple of times, fighting not to fell asleep.\n";
						}
						else
						{
							message += "She visited the local church feeling somehow down. Listening to the preacher didn't have much impact on improving her mood.\n";
						}
				}
				else if (roll_b <= 14) //clinic
				{
					if (g_Girls.HasTrait(girl, "AIDS") || g_Girls.HasTrait(girl, "Chlamydia") || g_Girls.HasTrait(girl, "Herpes") || g_Girls.HasTrait(girl, "Syphilis"))
					{
						message += "The doctor told her she has an STD.\n";
					}
					if (g_Girls.GetStat(girl, STAT_HEALTH) >= 90)
					{
						message += "Her check up went wonderful. She was told she was in near perfect health.\n"; happy += 5;
					}
					else if (g_Girls.GetStat(girl, STAT_HEALTH) < 90)
					{
						message += "Her check up went good. She was told she was in very good health.\n";
					}
					else if (g_Girls.GetStat(girl, STAT_HEALTH) <= 50)
					{
						message += "Her check up decent. She was told she was in fair health.\n";
					}
					else if (g_Girls.GetStat(girl, STAT_HEALTH) <= 20)
					{
						message += "Her check up went poorly. She was told she was in bad health.\n"; happy -= 5; roll = 4;
					}//should hopefully get a booster shot to help her gain health faster
					if (roll <= 5)
					{
						message += "The doctor decides to give her a booster shot."; g_Girls.UpdateStat(girl, STAT_HEALTH, 10);
					}
				}
				else if (roll_b <= 22) //salon
				{
					message += "Had her hair and nails done today. She is going look better for a few days.\n";
					g_Girls.UpdateTempStat(girl, STAT_BEAUTY, 5);
				}
				else if (roll_b <= 30) //picnic
				{
				}
				else if (roll_b <= 38) //hobby
				{
					if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Girls.GetStat(girl, STAT_LIBIDO) > 80)
					{
						message += girlName + " went out looking to get laid.\n";
					}
					else if (g_Girls.HasTrait(girl, "Nerd"))
					{
						message += girlName + " stayed inside and read a book.\n";
					}
				}
				else if (roll_b <= 46) //concert
				{
					message += "They were playing " + song_type_text + " music.\n";
					if (roll_c <= 14) //goth rock
					{
					}
					else if (roll_c <= 28) //classical
					{
					}
					else if (roll_c <= 42) //metal
					{
					}
					else if (roll_c <= 56) //rock
					{
					}
					else if (roll_c <= 70) //country
					{
					}
					else if (roll_c <= 87) //death metal
					{
					}
					else //pop
					{
					}
					if (roll <= 5)
					{
						message += girlName + " thought the conert was crap.\n";
					}
					else if (roll >= 95)
					{
						message += girlName + " thought the concert was amazing she had a really great time.\n";
					}
					else
					{
						message += girlName + " enjoyed herself. The concert wasn't the best she ever been to but she had a good time.\n";
					}
					if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85)
					{
						message += "Having seen her amazing beauty the band invites her to come backstage and meet them.\n";
						if (roll <= 5)
						{
							message += girlName + " declined as she thought they sucked.\n";
						}
						else if (roll >= 95)
						{
							message += girlName + " accepted with great joy.\n"; happy += 5;
						}
						else
						{
							if (roll <= 50)
							{
								message += girlName + " told them she had a good time but had to be going.\n";
							}
							else
							{
								message += girlName + " enjoyed herself so she accepted.\n";
							}
						}
					}
				}
				else if (roll_b <= 54) //bath
				{
					imagetype = IMGTYPE_BATH; happy += 5; health += 5;
					if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
					{
						message += "While in the tub the mood hit her and she proceed to pleasure herself.\n";
						imagetype = IMGTYPE_MAST; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
					}
					else
					{
						message += "She enjoyed a nice long soak.\n"; g_Girls.UpdateStat(girl, STAT_TIREDNESS, -5);
					}
				}
				else if (roll_b <= 62) //pool
				{
					imagetype = IMGTYPE_SWIM;;
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
					if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 35)
					{
						message += "Begin on the tired side she just decided to lay around the pool and get some sun.  She is going to have a tan for a few days.\n";
						g_Girls.UpdateTempStat(girl, STAT_BEAUTY, 5);
						if (roll < 15)
						{
							message += "A rather brave man saw her laying there and decide to try and remove her top.";
							if (g_Girls.HasTrait(girl, "Psychic"))
							{
								message += " But using her Psychic skills she stopped him before he could do it.";
							}
							else
							{
								if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits"))
								{
									message += " He succeed and was able to expose her gigantic boobs.";
								}
								else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
								{
									message += " He succeed and was able to expose her large breasts.";
								}
								else if (g_Girls.HasTrait(girl, "Small boobs") || g_Girls.HasTrait(girl, "Petite Breasts"))
								{
									message += " He succeed and was able to expose her small and perky tits.";
								}
								else if (g_Girls.HasTrait(girl, "Flat Chest"))
								{
									message += " He succeed and was able to expose her rather Flat Chest.";
								}
								else
								{
									message += " He was able to do it exposing her breasts.";
								}
								imagetype = IMGTYPE_NUDE;
							}
						}
					}
					else
					{
						message += "She took a dive into the pool and swam for awhile enjoying herself.\n";
					}
				}
				else if (roll_b <= 70) //cook
				{
					imagetype = IMGTYPE_COOK;;
					message += "The meal she cooked was \n";
					if (g_Girls.GetSkill(girl, SKILL_SERVICE) >= 85)//use service for now on how well she can cook.. if cooking skill ever gets added can be changed then
					{
						message += "amazing.  She really knows how to cook.\n"; g_Girls.UpdateStat(girl, STAT_HEALTH, 5);
					}
					else if (g_Girls.GetSkill(girl, SKILL_SERVICE) < 85)
					{
						message += "pretty good. She isn't the best but with some work she could be.\n";
					}
					else if (g_Girls.GetSkill(girl, SKILL_SERVICE) < 50)
					{
						message += "plain. She really needs some practice at this.\n";
					}
					else
					{
						message += "awful. It can't really be called food it was so bad.\n";
					}
					g_Girls.UpdateSkill(girl, SKILL_SERVICE, 1);
				}
				else if (roll_b <= 78) //movies
				{
					message += "They were playing " + mov_type_text + ".\n";
					if (roll_c <= 20) //romance
					{
						if (g_Girls.HasTrait(girl, "Pessimist"))
						{
							if (HateLove >= 80) //loves you
							{
								message += "Even though " + girlName + " loves you greatly this kind of movie always make her Pessiistic nature show up. She thinks the two of you will never get a happy ending like in this movie.\n";
							}
							else
							{
								message += "Being the Pessimist she is she hates sappy love movies as she don't believe she will ever find her true love.\n";
							}
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);  roll = 4;
						}
						else if (g_Girls.HasTrait(girl, "Optimist"))
						{
							if (HateLove >= 80) //loves you
							{
								message += girlName + " loves you greatly and her Optimistic nature makes her know that one day the two of you will have a happy ending just like in this movie.\n";
							}
							else
							{
								message += "Being the Optimist she is she loves this kind of movie. She knows one day she will find her true love.\n";
							}
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);  roll = 96;
						}
					}
					else if (roll_c <= 40) //comedy
					{
						if (g_Girls.HasTrait(girl, "Aggressive"))
						{
							message += girlName + "'s aggressive nature makes her wish the movie would have been an action flick.\n";
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);  roll = 4;
						}
					}
					else if (roll_c <= 60) //scary
					{
						if (g_Girls.HasTrait(girl, "Meek"))
						{
							message += girlName + " Meekly ran from the theater crying. Seems she shouldn't have watched this kind of movie.\n";
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5); roll = 4;
						}
					}
					else if (roll_c <= 80) //porno
					{
						if (g_Girls.HasTrait(girl, "Shy"))
						{
							message += girlName + " face turned blood red when the movie got going. She snuck out of the movie and ran home.\n";
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5); roll = 4;
						}
						else if (g_Girls.HasTrait(girl, "Nymphomaniac"))
						{
							message += girlName + " loves everything to do with sex so this is her type of movie.\n";
							g_Girls.UpdateTempStat(girl, STAT_LIBIDO, 5); roll = 96;
							if (g_Girls.GetStat(girl, STAT_LIBIDO) >= 70)
							{
								message += "The movie started to turn her on so she started to pleasure herself. ";
								if (roll_d <= 20)
								{
									message += "A man noticed and approched her asking if she wanted the real thing instead of her fingers.";
									if (g_Girls.HasTrait(girl, "Virgin"))
									{
										message += " She informs him she is a Virgin and that she won't be having sex with him.";
									}
									else if (g_Girls.HasTrait(girl, "Lesbian"))
									{
										message += " She informs him she is a Lesbian and that she doesn't have sex with guys.";
									}
									else if (HateLove >= 80 && g_Girls.GetStat(girl, STAT_LIBIDO) > 99)
									{
										message += " Despite the fact that she is in love with you she couldn't help herself her lust is to great and she agrees. ";
										imagetype = IMGTYPE_SEX; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
										g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
									}
									else if (HateLove >= 80 && g_Girls.GetStat(girl, STAT_LIBIDO) <= 99)
									{
										message += " She tells him she is in love and that he can't compare to her love. She finishes herself off then leaves with a smile on her face.";
										imagetype = IMGTYPE_MAST; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
									}
									else
									{
										message += " She takes him up on the offer as she prefers the real thing.";
										imagetype = IMGTYPE_SEX; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
										g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
									}
								}
								else
								{
									imagetype = IMGTYPE_MAST; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
								}
							}
						}
					}
					else //action
					{
						if (g_Girls.HasTrait(girl, "Aggressive"))
						{
							message += girlName + "'s loves this type of movie with all the action it gets her blood pumping.\n";
							g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);  roll = 96;
						}
					}
					if (roll <= 5)
					{
						message += girlName + " thought the movie was crap.\n";
					}
					else if (roll >= 95)
					{
						message += girlName + " thought the movie was amazing she had a really great time.\n";
					}
					else
					{
						message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n";
					}
				}
				else if (roll_b <= 86) //club
				{
					message += girlName + " puts on her best dress before leaving.";
					//message += girlName + " says hi to " + clubbar->m_Realname + ".\n";  zzzzz needs work FIXME just a test to see it worked it didnt
					if (g_Girls.HasTrait(girl, "Lesbian"))
					{
						message += " She takes in some exotic dancing from some of the strippers there.";
						/*if (roll <= 15)
						{ message += " She ends up buying a lap dance from "; }*///have it pull up a club stripper name here
					}
					imagetype = IMGTYPE_FORMAL;;
				}
				else if (roll_b <= 93) //bed
				{
					//need events for this
					imagetype = IMGTYPE_BED; health += 10; g_Girls.UpdateStat(girl, STAT_TIREDNESS, -10);
				}
				else //bar
				{
					if (g_Girls.HasTrait(girl, "Alcoholic"))
					{
						message += "As an Alcoholic she loves coming to the bar.\n"; happy += 15;
					}
					if (g_Girls.GetStat(girl, STAT_HAPPINESS) < 50)
					{
						message += girlName + " feeling a little down, decide to get drunk while she was at the bar.\n"; health -= 5;
					}
					else
					{
						message += girlName + " was in a good mood so she had a few drinks and talked to the people around her.\n"; happy += 5;
					}
				}
			}



			g_Girls.UpdateStat(girl, STAT_HEALTH, health);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, happy);
			girl->m_Events.AddMessage(message, imagetype, Day0Night1);
			return false;
		}

		// 1. if she is addicted she will first attempt to purchase drugs until she has no money
		if (g_Girls.HasTrait(girl, "Viras Blood Addict"))
			AddictBuysDrugs("Viras Blood Addict", "Viras Blood", girl, brothel, Day0Night1);

		if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))
			AddictBuysDrugs("Fairy Dust Addict", "Fairy Dust", girl, brothel, Day0Night1);

		if (g_Girls.HasTrait(girl, "Shroud Addict"))
			AddictBuysDrugs("Shroud Addict", "Shroud Mushroom", girl, brothel, Day0Night1);

		if (girl->m_Money == 0 || girl->m_NumInventory == 40)
		{
			girl->m_Events.AddMessage(message, imagetype, Day0Night1);
			return false;
		}

		// 2. buy any items that catch her fancy
		int numberToBuy = g_Dice % 5;	// buy up to 10 things  MYR: Reduced to 5 to cut down on inventory clutter
		int itemsBought = 0;
		string buyList = "";

		for (int i = 0; i < numberToBuy && girl->m_NumInventory < 40; i++)
		{
			int item = g_InvManager.GetRandomShopItem();
			int cost = g_InvManager.GetShopItem(item)->m_Cost;
			string itemName = g_InvManager.GetShopItem(item)->m_Name;

			if (g_Girls.HasItem(girl, itemName) > -1)
				continue;
			if (girl->m_Money - cost < 0)
				continue;

			if (g_Dice.percent(g_InvManager.GetShopItem(item)->m_GirlBuyChance))
			{
				int chance = (g_Dice.d100());
				switch ((int)g_InvManager.GetShopItem(item)->m_Type)
				{
				case INVRING:
				{
					// Third argument to GirlBuyITem is the max # of the item type (ring, dress, etc...) they 
					// should have (Max. 8 rings, max 1 dress, etc...)
					if (g_InvManager.GirlBuyItem(girl, item, 8, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVDRESS:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVUNDERWEAR:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSHOES:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVFOOD:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVNECKLACE:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVWEAPON:
				{
					// if she isn't a warrior type she probably won't buy it
					if (!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSMWEAPON:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMAKEUP:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMOR:
				{
					// if she isn't a warrior type she probably won't buy it
					if (!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if (g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMISC:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMBAND:
				{
					if (g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;
				} // Switch
			}     // if buy
		}         // for # buy chances

		if (itemsBought > 0)
		{
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5 + (itemsBought * 2));
			message += gettext(" She did some shopping, and bought: ") + buyList + ".";
			imagetype = IMGTYPE_SHOP;
		}

		ss << message;

#endif
	}
	// `J` only add a new message if something new was done.
	if (ss.str().length() > 0) girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);

	// update stats and skills
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, U_Tiredness, false);
	g_Girls.UpdateStat(girl, STAT_HEALTH, U_Health, false);
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, U_Happiness);
	g_Girls.UpdateStat(girl, STAT_MANA, U_Mana);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, U_Libido);
	g_Girls.UpdateStat(girl, STAT_EXP, U_EXP);
	g_Girls.UpdateTempStat(girl, STAT_BEAUTY, U_Beauty);
	g_Girls.UpdateStat(girl, STAT_MORALITY, U_Morality);



	// update money
	girl->m_Money += U_Money;

	return false;
}

bool cJobManager::AddictBuysDrugs(string Addiction, string Drug, sGirl* girl, sBrothel* brothel, bool Day0Night1)
{
	int id = g_InvManager.CheckShopItem(Drug);
	if (id == -1) return false;							// quit if the shop does not have the item
	if (!g_Girls.CheckInvSpace(girl)) return false;		// quit if inventory is full

	int cost = g_InvManager.GetItem(Drug)->m_Cost;
	if ((girl->m_Money - cost) < 0) return false;		// if they can afford it, they will buy it

	girl->m_Money -= cost;

	// If a matron is on shift, she may catch the girl buying drugs
	if ((g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, true) >= 1 || g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, false) >= 1)
		&& g_Dice.percent(70))
	{
		girl->m_Events.AddMessage("Matron confiscates drugs", IMGTYPE_PROFILE, EVENT_WARNING);
		return false;
	}
	else
	{
		g_Girls.AddInv(girl, g_InvManager.BuyShopItem(id));
		return true;
	}

}
