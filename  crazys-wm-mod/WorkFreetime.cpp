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

// `J` Brothel Job - General
bool cJobManager::WorkFreetime(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string girlName = girl->m_Realname;
	int roll = g_Dice%100;
	int roll_a = g_Dice%100; int roll_b = g_Dice%100; int roll_c = g_Dice%100; int roll_d = g_Dice%100;
	int general = false;
	int quest = false, nympo = false, fight = false;
	int happy = 0, health = 0;
	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int imagetype = IMGTYPE_PROFILE;
	vector<sGirl*> barmaid = girls_on_job(brothel, JOB_BARMAID, DayNight);
	vector<sGirl*> clubbar = girls_on_job(brothel, JOB_SLEAZYBARMAID, DayNight);
	//brothel->m_Filthiness++;
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, -20);
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, 15);
	g_Girls.UpdateStat(girl, STAT_HEALTH, 10);
	g_Girls.UpdateStat(girl, STAT_MANA, 10);
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	{ g_Girls.UpdateTempStat(girl, STAT_LIBIDO, 15); }
	else/*									  */{ g_Girls.UpdateTempStat(girl, STAT_LIBIDO, 5); }
	g_Girls.UpdateStat(girl, STAT_EXP, 1);   // Just because!

	string message = gettext("She rested and recovered some energy.\n");

	if(g_Dice%2 != 1)	// add inventory items since she is going shopping :D
		return false;

	int v[2] = {-1,-1};
	girl->m_Triggers.CheckForScript(TRIGGER_SHOPPING, true, v);	// check for and trigger shopping scripts

	if(girl->m_Money == 0 || girl->m_NumInventory == 40)
	{
	if(g_Girls.GetStat(girl, STAT_TIREDNESS) >= 80) { roll_b = 93; }
	if(g_Girls.GetStat(girl, STAT_HEALTH) <= 30)   { roll_b = 93; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	   { roll_a += 30; }
	if (g_Girls.HasTrait(girl, "Adventurer"))	   { roll_a += 30; }
	/* if (roll_a <= 75)	{ general = true; }
	else if (roll_a <= 90)	{ quest = true; }
	else if (roll_a <= 110)	{ nympo = true; }
	else if (roll_a <= 125)	{ fight = true; }*/

	/*May be best to handle this differently then this.  Could have it where one of the rolls is do hobby.
	Hobby would be like girl goes on adventurer or nympho looks for sex or nerd goes to libary or look for fight
	May make it much easier then all of this.
	
	Try and figure out how you could make the girls apper to talk to each other..  Figure just have it look for girl
	on certain jobs.  Have it like girl talks to the barmaid while she works... not sure how to do this yet.*/

		// `CRAZY` This is general things any girl can do
	/*default*/	int gen_type = 1;    string gen_type_text = "stayed in bed most of the day";
	/* */if (roll_b <= 6)	{ gen_type = 12; gen_type_text = "went to the local temple"; }
	else if (roll_b <= 14)	{ gen_type = 11; gen_type_text = "went to the clinic for a checkup"; }
	else if (roll_b <= 22)	{ gen_type = 10; gen_type_text = "went to the country side"; }
	else if (roll_b <= 30)	{ gen_type = 9; gen_type_text = "went to have a picnic"; }
	else if (roll_b <= 38)	{ gen_type = 8; gen_type_text = "went out for a stroll in the city"; }
	else if (roll_b <= 46)	{ gen_type = 7; gen_type_text = "went to the casino to gamble"; }
	else if (roll_b <= 54)	{ gen_type = 6; gen_type_text = "took a bath"; }
	else if (roll_b <= 62)	{ gen_type = 5; gen_type_text = "went to the local pool"; }
	else if (roll_b <= 70)	{ gen_type = 4; gen_type_text = "decided to cook a meal"; }
	else if (roll_b <= 78)	{ gen_type = 3; gen_type_text = "went to the movies"; }
	else if (roll_b <= 86)	{ gen_type = 2; gen_type_text = "went to the club"; }
	else if (roll_b >= 94)	{ gen_type = 0; gen_type_text = "went to the bar"; } 
	//add salons, concert, do hobby

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

	
	if (nympo) //maybe get merged into the do hobby ideal I have
	{
		if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		{
			message += girlName + " went out nympho.\n";
		}
		else
		{
			general = true;
		}
	}
	else if (quest) //this may just turn into a job.. hate to have a girl get hurt when on free time
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
	else if (fight)  //this may just be cut.. no point in letting the girls get hurt on free time
	{
		if (g_Girls.HasTrait(girl, "Aggressive"))
		{
			message += girlName + " went out on the town looking for a fight today.\n";
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
			if(g_Girls.GetStat(girl, STAT_MORALITY) >= 80)
				if (roll <= 33)
				{ message += girlName + " prayed for a couple of hours just to actively participate in the mass. Afterwards she helped cleaning the temple grounds.\n"; }
				else if (roll <=66)
				{ message += "She spent almost her entire free time praying! But from her line of work, she got used to being on her knees for long hours.\n"; }
				else
				{ message += "Being at the sanctuary for her whole free time, she could swear that she noticed a presents of some sort of holly being.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) >= 60)
				if (roll <= 33)
				{ message += girlName + " confessed her shameful actions that she did at work, asking for the God’s forgiveness.\n"; }
				else if (roll <=66)
				{ message += "She humbly sit down in the last row. Focused and with lot of interest, she listening to the whole mass.\n"; }
				else
				{ message += "Today she cleaned up a road side shrine and decorating it with fresh flowers.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) >= 40)
				if (roll <= 33)
				{ message += girlName + " starts to find this way of spending free time being interesting. Wanting to know more she listens to the preacher.\n"; }
				else if (roll <=66)
				{ message += "After participating in today’s ceremony she felt happier.\n"; happy += 5; }
				else
				{ message += "Being late, she tried to find a place to sit. Happily she noticed some free sits on the other side of the church. Unfortunately her high heels were knocking pretty loud while walking on the churches stone floor, disturbing that part of the mass with silence prays.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) >= 20)
				if (roll <= 33)
				{ message += girlName + " made a quick stop at the local temple for a short pray before heading further into town.\n"; }
				else if (roll <=66)
				{ message += "On her way back, she gave a short pray  in front of a road side shrine.\n"; }
				else
				{ message += "After eavesdropping on a couple of girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls was right about this place.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) <= -20)
				if (roll <= 33)
				{ message += girlName + " tried to listen to the preachers lecture, but she fell asleep halfway thru!\n"; }
				else if (roll <=66)
				{ message += "She was about to enter the sanctuary but she noticed a really cut kitten. She spent the time playing with the cat.\n"; }
				else
				{ message += "After eavesdropping couple girls at work talking about a nearby temple, she decided to visit this holy place. Listening to the preacher she felt that the girls was wrong about this place. Being bored she left in the middle of the mass.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) <= -40)
				if (roll <= 33)
				{ message += girlName + " threw some special weeds from your farm into a goblet of fire, standing in front of one of the side altars. Later on, most of praying at the altar swear that they saw angels!\n"; }
				else if (roll <=66)
				{ message += "At the church, she noticed a really young priest hearing to today’s confessions. Feeling mischievous she entered the confessional. After confessing her sins in high detail, she proposed to recreate them with him! The abashed youngster gave her a couple of prays as penance and left right after.\n"; }
				else
				{ message += "Getting bored at the mass, she started to whisper things to a man sitting next to her, not bothering with the fact that his wife was sitting next to him!\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) <= -60)
				if (roll <= 33)
				{ message += girlName + " entered the confessional. Whispering sensually of her job experiences, all in high details, she made the priest have a heart attack! When the man was squirming in pain on the floor, she left the temple laughing.\n"; }
				else if (roll <=66)
				{ message += "Dressed really slutty, she parade the mid lane of the church, wearing high hills that were heard all over the place, just to take a sit in the first row. The pose she had sat in, was to expose her pussy to all priests on the altar performing the holy mass.\n"; }
				else
				{ message += " In front of a temple she approached a young monk. After a brief chat about god, faith and salvation she gave him a proposal. Claiming that it was such waste for such young and handsome man to live in chastity, she proposed he could spend some quality, fun time with her. The man quickly run inside whispering some kind of manta, while the girl went her own way laughing.\n"; }
			else if(g_Girls.GetStat(girl, STAT_MORALITY) <= -80)
				if (roll <= 33)
				{ message += girlName + " came to the temple with one of girls working for you, but her companion flew right after seeing what kind place of worship this was. No human sacrifice provided by " + girlName + " today, sorry.\n"; }
				else if (roll <=66)
				{ message += "Scheming against the church, she came with a brilliant idea. Promising local, young tugs to repay a favor with her body, she made them to assault and beat up a group of priests. Not waiting for the outcome of the brawl, she disappear not holding to her end of the deal.\n"; }
				else
				{ message += "Not liking the architecture style of the temple she decided to do something about that. Unfortunately for her, the fire was extinguished fairly quickly.\n"; }
			else
				if (roll <= 33)
				{ message += girlName + " was on her way to pray in the local temple, but on the way there, she saw a beautiful bag on display. After entering the shop, she spent several hours, picking out the perfect bag for herself before returning.\n"; }
				else if (roll <=66)
				{ message += "Attending the mass she felt some how bored and not focused on the topic. She even yawn couple of times, fighting not to fell asleep.\n"; }
				else
				{ message += "She visited the local church feeling some how down. Listening to the preacher didn’t have much impact on improving her mood.\n"; }
		}
		else if (roll_b <= 14) //clinic
		{
			if (g_Girls.HasTrait(girl, "AIDS") || g_Girls.HasTrait(girl, "Chlamydia") || g_Girls.HasTrait(girl, "Herpes") || g_Girls.HasTrait(girl, "Syphilis"))
			{ message += "The doctor told her she has an STD.\n"; }
			if (g_Girls.GetStat(girl, STAT_HEALTH) >= 90)
			{ message += "Her check up went wonderful. She was told she was in near perfect health.\n"; happy += 5; }
			else if (g_Girls.GetStat(girl, STAT_HEALTH) < 90)
			{ message += "Her check up went good. She was told she was in very good health.\n"; }
			else if (g_Girls.GetStat(girl, STAT_HEALTH) <= 50)
			{ message += "Her check up decent. She was told she was in fair health.\n"; }
			else if (g_Girls.GetStat(girl, STAT_HEALTH) <= 20)
			{ message += "Her check up went poorly. She was told she was in bad health.\n"; happy -= 5; roll = 4; }//should hopefully get a booster shot to help her gain health faster
			if (roll <= 5)
			{ message += "The doctor decides to give her a booster shot."; g_Girls.UpdateStat(girl, STAT_HEALTH, 10); }
		}
		else if (roll_b <= 22) //country side
		{
		}
		else if (roll_b <= 30) //picnic
		{
		}
		else if (roll_b <= 38) //stroll in the city
		{
		}
		else if (roll_b <= 46) //casino might change this to something else..cant come up with much for it
		{
			/*Goes she might gamble might not.. maybe base it off traits.  Take in a show
			Needs more stuff.*/
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
			if(g_Girls.GetStat(girl, STAT_TIREDNESS) > 50)
			{
				message += "Begin on the tired side she just decided to lay around the pool and get some sun.  She is going to have a tan for a few days.\n";
				g_Girls.UpdateTempStat(girl, STAT_BEAUTY, 5);
				if (roll < 15)
				{
					message += "A rather brave man saw her laying there and decide to try and remove her top.";
					if (g_Girls.HasTrait(girl, "Psychic"))
					{ message += " But using her Psychic skills she stopped him before he could do it."; }
					else
					{
						if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")|| g_Girls.HasTrait(girl, "Titanic Tits"))
						{ message += " He succeed and was able to expose her gigantic boobs."; }
						else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs")|| g_Girls.HasTrait(girl, "Giant Juggs"))
						{ message += " He succeed and was able to expose her large breasts."; }
						else if (g_Girls.HasTrait(girl, "Small boobs") || g_Girls.HasTrait(girl, "Petite Breasts"))
						{ message += " He succeed and was able to expose her small and perky tits."; }
						else if (g_Girls.HasTrait(girl, "Flat Chest"))
						{ message += " He succeed and was able to expose her rather Flat Chest."; }
						else
						{ message += " He was able to do it exposing her breasts."; }
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
			message += "The meal she cooked was .\n";
			if (g_Girls.GetSkill(girl, SKILL_SERVICE) >= 85)//use service for now on how well she can cook.. if cooking skill ever gets added can be changed then
			{ message += "amazing.  She really knows how to cook.\n"; g_Girls.UpdateStat(girl, STAT_HEALTH, 5); }
			else if (g_Girls.GetSkill(girl, SKILL_SERVICE) < 85)
			{ message += "pretty good. She isn't the best but with some work she could be.\n"; }
			else if (g_Girls.GetSkill(girl, SKILL_SERVICE) < 50)
			{ message += "plain. She really needs some practice at this.\n"; }
			else 
			{ message += "awful. It can't really be called food it was so bad.\n"; }
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
						{ message += "Even though " + girlName + " loves you greatly this kind of movie always make her Pessiistic nature show up. She thinks the two of you will never get a happy ending like in this movie.\n"; }
						else
						{ message += "Being the Pessimist she is she hates sappy love movies as she don't believe she will ever find her true love.\n"; }
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);  roll = 4;
					}
					else if (g_Girls.HasTrait(girl, "Optimist"))
					{
						if (HateLove >= 80) //loves you
						{ message += girlName + " loves you greatly and her Optimistic nature makes her know that one day the two of you will have a happy ending just like in this movie.\n"; }
						else
						{ message += "Being the Optimist she is she loves this kind of movie. She knows one day she will find her true love.\n"; }
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);  roll = 96;
					}
					if (roll <= 5)
					{ message += girlName + " thought the movie was crap.\n"; }
					else if (roll >= 95)
					{ message += girlName + " thought the movie was amazing she had a really great time.\n"; }
					else 
					{ message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n"; }
				}
			else if (roll_c <= 40) //comedy
				{
					if (g_Girls.HasTrait(girl, "Aggressive"))
					{
						message += girlName + "'s aggressive nature makes her wish the movie would have been an action flick.\n";
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);  roll = 4;
					}
					if (roll <= 5)
					{ message += girlName + " thought the movie was crap.\n"; }
					else if (roll >= 95)
					{ message += girlName + " thought the movie was amazing she had a really great time.\n"; }
					else 
					{ message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n"; }
				}
			else if (roll_c <= 60) //scary
				{
					if (g_Girls.HasTrait(girl, "Meek"))
					{
						message += girlName + " Meekly ran from the theater crying. Seems she shouldn't have watched this kind of movie.\n";
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5); roll = 4;
					}
					if (roll <= 5)
					{ message += girlName + " thought the movie was crap.\n"; }
					else if (roll >= 95)
					{ message += girlName + " thought the movie was amazing she had a really great time.\n"; }
					else 
					{ message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n"; }
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
						g_Girls.UpdateStat(girl, STAT_LIBIDO, 5); roll = 96;
						if (g_Girls.GetStat(girl, STAT_LIBIDO) >= 70)
							{
								message += "The movie started to turn her on so she started to pleasure herself. ";
								if (roll_d <= 20)
								{
									message += "A man noticed and approched her asking if she wanted the real thing instead of her fingers.";
									if (g_Girls.HasTrait(girl, "Virgin"))
									{ message += " She informs him she is a Virgin and that she won't be having sex with him."; }
									else if (g_Girls.HasTrait(girl, "Lesbian"))
									{ message += " She informs him she is a Lesbian and that she doesn't have sex with guys."; }
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
								{ imagetype = IMGTYPE_MAST; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15); }
							}
					}
					if (roll <= 5)
					{ message += girlName + " thought the movie was crap.\n"; }
					else if (roll >= 95)
					{ message += girlName + " thought the movie was amazing she had a really great time.\n"; }
					else 
					{ message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n"; }
				}
			else //action
				{
					if (roll <= 5)
					{ message += girlName + " thought the movie was crap.\n"; }
					else if (roll >= 95)
					{ message += girlName + " thought the movie was amazing she had a really great time.\n"; }
					else 
					{ message += girlName + " enjoyed herself. The movie wasn't the best she ever seen but she had a good time.\n"; }
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
				{ message += " She ends up buying a lap dance from "; }*///have it pull up a stripper name here
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
			if(g_Girls.GetStat(girl, STAT_HAPPINESS) < 50)
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
		girl->m_Events.AddMessage(message, imagetype, DayNight);
		return false;
	}

	// 1. if she is addicted she will first attempt to purchase drugs until she has no money
	if(g_Girls.HasTrait(girl, "Viras Blood Addict"))
      AddictBuysDrugs("Viras Blood Addict", "Viras Blood", girl, brothel, DayNight);

	if(g_Girls.HasTrait(girl, "Fairy Dust Addict"))
      AddictBuysDrugs("Fairy Dust Addict", "Fairy Dust", girl, brothel, DayNight);

	if(g_Girls.HasTrait(girl, "Shroud Addict"))
      AddictBuysDrugs("Shroud Addict", "Shroud Mushroom", girl, brothel, DayNight);

	if(girl->m_Money == 0 || girl->m_NumInventory == 40)
	{
		girl->m_Events.AddMessage(message, imagetype, DayNight);
		return false;
	}

	// 2. buy any items that catch her fancy
	int numberToBuy = g_Dice%5;	// buy up to 10 things  MYR: Reduced to 5 to cut down on inventory clutter
	int itemsBought = 0;
	string buyList = "";

	for(int i=0; i < numberToBuy && girl->m_NumInventory < 40; i++)
	{
		int item = g_InvManager.GetRandomShopItem();
		int cost = g_InvManager.GetShopItem(item)->m_Cost;
		string itemName = g_InvManager.GetShopItem(item)->m_Name;

		if(g_Girls.HasItem(girl, itemName) > -1)
			continue;
		if(girl->m_Money - cost < 0)
			continue;

		if(g_Dice.percent(g_InvManager.GetShopItem(item)->m_GirlBuyChance))
		{
			int chance = (g_Dice%100+1);
			switch((int)g_InvManager.GetShopItem(item)->m_Type)
			{
				case INVRING:
				{
					// Third argument to GirlBuyITem is the max # of the item type (ring, dress, etc...) they 
					// should have (Max. 8 rings, max 1 dress, etc...)
					if(g_InvManager.GirlBuyItem(girl, item, 8, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVDRESS:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVUNDERWEAR:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSHOES:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVFOOD:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVNECKLACE:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVWEAPON:
				{
					// if she isn't a warrior type she probably won't buy it
					if(!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSMWEAPON:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMAKEUP:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMOR:
				{
					// if she isn't a warrior type she probably won't buy it
					if(!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMISC:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMBAND:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;
			} // Switch
		}     // if buy
	}         // for # buy chances

	if(itemsBought > 0)
	{
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5 + (itemsBought * 2));
		message += gettext(" She did some shopping, and bought: ") + buyList + ".";
	}

	girl->m_Events.AddMessage(message, IMGTYPE_SHOP, DayNight);
	return false;
}

bool cJobManager::AddictBuysDrugs(string Addiction, string Drug, sGirl* girl, sBrothel* brothel, int DayNight)
{
	int id = g_InvManager.CheckShopItem(Drug);
	if(id == -1)
		return false;

	int cost = g_InvManager.GetItem(Drug)->m_Cost;

	if(!g_Girls.CheckInvSpace(girl))	// quit if inventory is full
		return false;

	if((girl->m_Money - cost) < 0)	// if they can afford it, they will buy it
		return false;

	girl->m_Money -= cost;

	// If a matron is on shift, she may catch the girl buying drugs
	if((g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, true) >= 1 || g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, false) >= 1)
		&& (g_Dice%100)+1 < 70)
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
