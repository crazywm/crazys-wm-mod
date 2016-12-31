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
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cHouse.h"
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
extern cHouseManager g_House;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;
extern cPlayer* The_Player;
#if 1   //BSIN - used to synchronise group actions across girls
extern unsigned long g_Day;
#endif

// `J` Job House - General
bool cJobManager::WorkPersonalBedWarmer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	// `J` moving multiple bed warmers to a group function
	int haremcount = g_House.GetNumGirlsOnJob(0, JOB_PERSONALBEDWARMER, Day0Night1);
//	if (haremcount > 1)	return g_Girls.DisobeyCheck(girl, ACTION_WORKHAREM, brothel);

	int actiontype = ACTION_SEX;
	stringstream ss; string girlName = girl->m_Realname;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100(), roll_d = g_Dice.d100(), roll;
	//cTariff tariff;
	//g_Building = BUILDING_HOUSE;
	g_Girls.UnequipCombat(girl);	// put that shit away, not needed for sex training
	int imagetype = IMGTYPE_MAID;
	int msgtype = Day0Night1;

	ss << gettext("You tell ") << girlName << gettext(" she is going to warm your bed tonight");

	//initiate things I need
	int wages = 0, tips = 0;
	int HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);


	// Headgirl can influence against refusal
	sGirl* headGirl = g_House.GetFirstGirlOnJob(0, JOB_HEADGIRL, Day0Night1);
	bool headOnDuty = (headGirl ? true : false);
	string headName = (headOnDuty ? headGirl->m_Realname : "");

	//	refusal check
	if (g_Girls.DisobeyCheck(girl, ACTION_WORKHAREM, brothel))
	{
#if 1
		ss << gettext(" but she refuses to lay with you.");
		int effectiveness = g_Dice.d100();

		if (headOnDuty)
		{
			ss << gettext("\n\nHead Girl ") << headName << gettext(" intervenes");
			if (girl->is_slave())
			{
				//BSIN
				//slave - so cannot refuse.
				//planned to just make easier to persuade slaves, but as this job runs separately on each girl, this meant
				//a girl refused in her own description, but showed up in someone else's harem group.
				//Cannot see an easy way around - short of some kind of global tracking - so for now Slave = compulsory participation.
				//And group harem will only involve slaves. Simple.
				//Headgirl will persuade somehow

				ss << gettext(" reminding ") << girlName << gettext(" that she is your property and must do as you command. ");

				// If headgirl likes girls... Because this will be a rare combo this is at the top, so it has some chance of being seen
				if (g_Girls.HasTrait(headGirl, "Lesbian") && girl->has_trait( "Lesbian") && effectiveness > 10)
				{
					ss << gettext("As ") << girlName << gettext(" continues to refuse ") << headName << gettext(" suddenly kisses her, putting a hand under her underwear and pulling back to expose her ");
					if (girl->has_trait( "Pierced Nipples")) ss << gettext("pierced nipples");
					else if (girl->has_trait( "Pierced Clit")) ss << gettext("pierced clit");
					else if (girl->has_trait( "Perky Nipples")) ss << gettext("perky nipples");
					else if (girl->has_trait( "Puffy Nipples")) ss << gettext("puffy nipples");
					else if (girl->has_trait( "Abundant Lactation") || girl->has_trait( "Cow Tits")) ss << gettext("moist, milky breasts");
					else if (girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts") || girl->has_trait( "Small Boobs")) ss << gettext("small breasts");
					else if (girl->has_trait( "Busty Boobs") || girl->has_trait( "Big Boobs") || girl->has_trait( "Giant Juggs")) ss << gettext("large breasts");
					else if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Titanic Tits")) ss << gettext("extremely large breasts");
					else ss << gettext("breasts");
					ss << gettext(", which she eagerly licks. ") << girlName << gettext(" is initially shocked, but is quickly aroused by ")
						<< headName << gettext("'s expert tongue.\nThey're soon putting on a hell of a show together. ")
						<< headName << gettext(" quickly drives her wild, bringing her to the brink of orgasm several times, but never quite letting her come.\n\n")
						<< headName << gettext(" suddenly stops, leaving ") << girlName << gettext(" on your bed, wet, frustrated, horny as hell and begging to be fucked.");

					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, +10, true);
					g_Girls.UpdateSkill(headGirl, SKILL_LESBIAN, +4);
				}

				//If headgirl has some charm and turns it on a little...
				else if ((g_Girls.HasTrait(headGirl, "Cool Person") || g_Girls.HasTrait(headGirl, "Charismatic") || g_Girls.HasTrait(headGirl, "Charming") || g_Girls.GetStat(headGirl, STAT_CHARISMA) > 70) && effectiveness > 20)
				{
					ss << gettext("Because ") << headName << gettext(" is so cool with her and makes it all seem exciting, ") << girlName << gettext(" soon agrees.");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}

				//if slave is an addict
				else if (girl->is_addict(true) && effectiveness > 10)
				{
					ss << gettext("Noticing the girl's shakes, and the look in her eye, ") << headName << gettext(" suggests that tonight could help ") << girlName
						<< gettext(" raise a little \"spending money.\"\nShe agrees to stay for ") << effectiveness / 2 << gettext(" extra drug money.");
					tips += (effectiveness / 2);
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
				}

				//If the reminder of her status changes her mind
				else if (effectiveness > 70)
				{
					ss << gettext("Remembering she is a slave, ") << girlName << gettext(" reluctantly submits.\n");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, -2);
				}

				//if head is a bully
				else if (g_Girls.HasTrait(headGirl, "Aggressive") && effectiveness > 20)
				{
					ss << gettext("When she still refuses, ");
					if (effectiveness > 70)
					{
						ss << headName << gettext(" loses it and rips off all the slave's clothes. Grabbing her violently by the hair, ") << headName << gettext(" drags ") << girlName
							<< gettext(" naked into the streets and chains her over a large crate. She smiles, slapping and spreading ") << girlName << gettext("'s exposed ass.\n\"Guess I'll leave you out for the ")
							<< gettext("city's dogs and homeless. Have a great night!\"\nAs she starts to walk away, ") << girlName << gettext(" finally breaks and screams to be let back in. She'll do whatever you want.\n")
							<< headName << gettext(" reluctantly agrees.");
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -15);
						g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 5);
						g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
						g_Girls.UpdateStat(girl, STAT_PCFEAR, 5);
						g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 1);  //relief
					}
					else
					{
						ss << headName << gettext(" grabs her hair and threatens her. She sees no choice but to stay.");
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
						g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
						g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, -1);
					}
				}

				// if nothing else can persuade her
				else
				{
					ss << gettext("After some argument, your Head Girl makes clear there is no choice for a slave in this, and offers ")
						<< girlName << gettext(" something to 'make it all feel like a nice warm dream.'\n") << girlName << gettext(" finally nods.");
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
					g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
					g_Girls.UpdateStat(girl, STAT_HEALTH, -1);
					g_Gold.misc_debit(20); //drug/spell money
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 2, true);
					g_Girls.UpdateStat(girl, STAT_PCHATE, 5); //she'll hate you later
					HateLove = 50;  //probably best fit for next bit...
				}
			}
			else // not slave
			{
				// If headgirl likes girls. Again because rare combo, is at the top, so it has some chance of being seen - this time can fail
				if (g_Girls.HasTrait(headGirl, "Lesbian") && girl->has_trait( "Lesbian") && effectiveness > 10)
				{
					ss << gettext(" suddenly kissing ") << girlName << gettext(" and putting a hand under her underwear, pulling back to expose her ");
					if (girl->has_trait( "Pierced Nipples")) ss << gettext("pierced nipples");
					else if (girl->has_trait( "Pierced Clit")) ss << gettext("pierced clit");
					else if (girl->has_trait( "Perky Nipples")) ss << gettext("perky nipples");
					else if (girl->has_trait( "Puffy Nipples")) ss << gettext("puffy nipples");
					else if (girl->has_trait( "Abundant Lactation") || girl->has_trait( "Cow Tits")) ss << gettext("moist, milky breasts");
					else if (girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts") || girl->has_trait( "Small Boobs")) ss << gettext("small breasts");
					else if (girl->has_trait( "Busty Boobs") || girl->has_trait( "Big Boobs") || girl->has_trait( "Giant Juggs")) ss << gettext("large breasts");
					else if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Titanic Tits")) ss << gettext("extremely large breasts");
					else ss << gettext("breasts");
					ss << gettext(", which she eagerly licks. ") << girlName << gettext(" is shocked, but is quickly aroused by ")
						<< headName << gettext("'s expert tongue.\n\nThey're soon putting on a hell of a show together");

					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
					if (effectiveness < 60)
					{
						ss << gettext(", licking, rubbing, and bringing each other to orgasm after orgasm there on your bed.\n\n")
							<< gettext("You're a bit annoyed when ") << girlName << gettext(" leaves, exhausted, to spend the night in ") << headName
							<< gettext("'s room.\nStill they did put on quite a show, and she probably learned something tonight.\n");
						g_Girls.UpdateSkill(headGirl, SKILL_LESBIAN, +2);
						g_Girls.UpdateSkill(girl, SKILL_LESBIAN, +2);
						girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, EVENT_NOWORK);
						return true;
					}
					else
					{
						ss << gettext(". ") << headName << gettext(" quickly drives her wild, bringing her right to the brink of orgasm, but never quite letting her come. ");
						ss << headName << gettext(" suddenly stops, leaving ") << girlName << gettext(" on your bed, wet, frustrated, horny as hell and begging to be fucked.\n");
						g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, +10, true);
						g_Girls.UpdateSkill(headGirl, SKILL_LESBIAN, +4);
					}
				}

				//if headgirl is VERY persuasive, or much smarter, or persuasive and charming...
				else if (effectiveness >= 80 || g_Girls.GetStat(headGirl, STAT_INTELLIGENCE) >= g_Girls.GetStat(girl, STAT_INTELLIGENCE) + 20
					|| (effectiveness > 30 && (g_Girls.HasTrait(headGirl, "Charismatic") || g_Girls.HasTrait(headGirl, "Charming") || g_Girls.GetStat(girl, STAT_CHARISMA) > 80)))
				{
					ss << gettext(" and talks her into taking part.\n");
				}

				//if girl is an addict
				else if (girl->is_addict(true) && effectiveness > 20)
				{
					ss << gettext(" and offers her some drug money if she'll stay. ") << girlName << gettext(" agrees to stay for ") << effectiveness << gettext(" extra gold.\n");
					tips += effectiveness;
				}

				//if headgirl is a bully
				else if (g_Girls.HasTrait(headGirl, "Aggressive") && effectiveness > 50)
				{
					ss << gettext(" grabbing her hair and threatening her");
					// IF she refuses to go along, and she's tough enough to physically resist
					if ((g_Girls.GetStat(girl, STAT_SPIRIT) > 60 || girl->has_trait( "Aggressive") || girl->has_trait( "Fearless") || girl->has_trait( "Iron Will"))
						&& g_Girls.GetSkill(girl, SKILL_COMBAT) > (g_Girls.GetStat(headGirl, SKILL_COMBAT) - 30))
					{
						ss << gettext(". ") << girlName << gettext(" snaps, grabbing ") << headName << gettext(" by the throat and telling her to go fuck herself.\n");
						girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
						return true;
					}
					else
					{
						ss << gettext(" leaving her no choice but to stay.\n");
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -20);
						g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, -5);
					}
				}

				//if she's worth more
				else if (g_Girls.GetStat(girl, STAT_BEAUTY) > 95 || girl->has_trait( "Princess") || girl->has_trait( "Queen") || girl->has_trait( "Goddess"))
				{
					ss << gettext(" stopping her. Because she's rather special you offer extra money. ");
					//
					if (g_Dice.percent(50))
					{
						int pay = 2 * girl->askprice();
						ss << girlName << gettext(" agrees to stay for ") << pay << gettext(" extra gold.\n");
						tips += pay;
					}

					else
					{
						ss << gettext("She refuses.\n");
						girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
						return true;
					}
				}

				//if nothing can change her mind
				else
				{
					ss << gettext(" but is unable to change her mind.\n");
					girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
					return true;
				}
			}
		}
		else  // no headgirl
		{
			if (girl->is_slave())
			{
				//slave - so cannot refuse.
				//planned to just make easier to persuade slaves, but as this job runs separately on each girl, this meant
				//a girl refused in her own description, but showed up in someone else's harem group.
				//Cannot see an easy way around - short of some kind of global tracking - so for now Slave = compulsory participation.
				//And group harem will only involve slaves. Simple.
				//You will persuade somehow

				ss << gettext("You remind ") << girlName << gettext(" that slaves are property and should do as their master commands");

				//does she like you
				if (HateLove > 80)
				{
					ss << gettext(". Thinking again, she agrees as you have been an excellent master.");
				}
				//do you have anything she wants?
				else if (girl->has_trait( "Cum Addict") && effectiveness > 30)
				{
					ss << gettext(". Blushing, she says she'll do it... as long as she gets to finish you off with her mouth.")
						<< gettext("\nYou do not have a problem with this.");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}
				else if (girl->has_trait( "Nymphomaniac") && effectiveness > 30)
				{
					ss << gettext(". She raises her eyebrows and drops her clothes to the floor.\n")
						<< gettext("\"Then get on and fuck me already... 'Master.'\"");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}
				else if (girl->is_addict(true) && effectiveness > 10)
				{
					ss << gettext(". Noticing her shakes and the look in her eye, you suggest tonight could help her raise a little \"spending money.\"\n")
						<< gettext("She agrees to stay for ") << effectiveness / 2 << gettext(" extra drug money.");
					tips += (effectiveness / 2);
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
				}
				//Are you psychopathic
				else if ((The_Player->disposition() < -80) && effectiveness > 60)
				{
					ss << gettext(", but add that you are a kind master and she has nothing to fear from you. Over a drink, you explain how you respect your slaves and their choices, ")
						<< gettext("and that her choice to defy you, led to your choice to drug her.\nShe smiles dreamily, rubbing her breasts against you as you strip away her clothes.")
						<< gettext("\nYour drug delivers the perfect blend of horniness and suggestibility. ") << girlName
						<< gettext(" will participate. Fully.\n");
					g_Gold.misc_debit(100); //drug money
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 10, true);
					g_Girls.UpdateStat(girl, STAT_PCHATE, +10); //she'll hate you later
					HateLove = 50;  //probably best fit for next bit...
				}
				//Lucky?
				else if (effectiveness > 75)
				{
					ss << gettext(". You are very persuasive and recognizing the truth of your words, ")
						<< girlName << gettext(" apologizes, promising to do better in future.\n");
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 4);
					g_Girls.UpdateStat(girl, STAT_SPIRIT, -4);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}
				//does she like you
				else if (HateLove > 40)
				{
					ss << gettext(". She agrees to stay because you've mostly been good to her.");
				}
				//are you nice
				else if (The_Player->disposition() > 40)
				{
					ss << gettext(", adding that you are a kind master she should trust that you would only ever act in her best interests.\nWhile you respect that she is uncomfortable, ") << girlName
						<< gettext(" must broaden her mind, adjust to this life and learn the skills.\nShe WILL share your bed tonight, and she will learn something too.\n")
						<< gettext("\"One day you will thank me,\" you tell her.\nShe looks up at you, closes her eyes and nods.\n");
					g_Girls.UpdateStat(girl, STAT_PCFEAR, -2);
					g_Girls.UpdateStat(girl, STAT_PCLOVE, +1);
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, -1);
				}
				//or just mean?
				else if (The_Player->disposition() < -50)
				{
					ss << gettext(", adding that slaves who defy you here, get to enjoy a much more intimate and much less pleasant night down in the dungeon...\n");
					ss << gettext("\"Less pleasant for YOU, at least,\" you smile.\n\n");
					g_Girls.UpdateStat(girl, STAT_PCFEAR, 5);
					if (effectiveness > 35)
					{
						ss << gettext("Terrified, she agrees to spend the night.\n");
						g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 2);
						g_Girls.UpdateStat(girl, STAT_SPIRIT, -2);
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -10);
					}
					else
					{
						ss << gettext("She doesn't say a word. She doesn't leave either.\n");
						g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
						g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
						g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
					}
				}
				else
				{
					ss << gettext(". Finally, you explain that as a slave she doesn't have a choice in this. You offer ")
						<< girlName << gettext(" a little something to 'make it all feel like a nice warm dream.'\n") << girlName << gettext(" finally nods.");
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, 1);
					g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
					g_Girls.UpdateStat(girl, STAT_HEALTH, -1);
					g_Gold.misc_debit(20); //drug/spell money
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 2, true);
					g_Girls.UpdateStat(girl, STAT_PCHATE, 5); //she'll hate you later
					HateLove = 50;  //probably best fit for next bit...
				}
			}
			else // not a slave
			{

				ss << girlName << gettext(" is a free woman, so you cannot legally force her. ");

				//  Lucky
				if (effectiveness > 90 || ((g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 30) && effectiveness > 30))
				{
					ss << gettext("However, you successfully talk her into taking part.\n");
				}
				//if you have anything she wants?
				else if (girl->has_trait( "Cum Addict") && effectiveness > 50)
				{
					ss << gettext("Blushing, she says she might do it... if you'll finish in her mouth.")
						<< gettext("\nYou agree to this.\n");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}
				else if ((girl->has_trait( "Nymphomaniac") || girl->has_trait( "Slut")) && effectiveness > 40)
				{
					ss << gettext("Aware of her love for sex, you quickly snake your hand between her legs and start to rub on her panties. ")
						<< gettext("She doesn't try to stop you, instead just moving her panties aside and guiding your hand.\nShe is quickly naked, panting and aroused and is now happy to stay.\n");
					g_Girls.UpdateStat(girl, STAT_HAPPINESS, 2);
					g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
				}
				//if she's an addict
				else if (girl->is_addict(true) && effectiveness > 40)
				{
					ss << gettext("You offer her some drug money if she'll stay. ") << girlName << gettext(" agrees to stay for ") << 2 * effectiveness << gettext(" extra gold.\n");
					tips += (2 * effectiveness);
				}
				//if she's worth more
				else if (g_Girls.GetStat(girl, STAT_BEAUTY) > 95 || girl->has_trait( "Princess") || girl->has_trait( "Queen") || girl->has_trait( "Goddess"))
				{
					ss << gettext("Because she's rather special you offer some extra money.\n");
					//
					if (g_Dice.percent(65))
					{
						int pay = 2 * girl->askprice();
						ss << girlName << gettext(" agrees to stay for ") << pay << gettext(" extra gold.\n");
						tips += pay;
					}
					else
					{
						ss << gettext("She refuses.\n");
						girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
						return true;
					}
				}
				//if you are pure evil and very lucky
				else if ((The_Player->disposition() < -90) && effectiveness > 80)
				{
					ss << gettext("You persuade her to stay for a few minutes, for a casual chat over a drink. She agrees. Her words soon start to slur as the drug takes effect.")
						<< gettext("\nShe smiles dreamily, rubbing her nipples against you as you strip away her clothes.")
						<< gettext("\nYour drug delivers a perfect blend of horniness and suggestibility. ") << girlName
						<< gettext(" will now participate. Fully.\n");
					g_Gold.misc_debit(100); //drug money
					g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, 10, true);
					g_Girls.UpdateStat(girl, STAT_PCHATE, +20); //she'll hate you later
					HateLove = 50;  //probably best fit for next bit...
				}
				//if nothing can change her mind
				else
				{
					ss << gettext("She leaves.\n");
					girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
					return true;
				}
			}
		}
#endif
	}
	else ss << gettext(".\n\n");

	//sin
	bool diseased = false, risky = false;

	//Disease refusal.
	if (girl->has_disease())
	{
		if (HateLove > 40)  //if she doesn't want you dead...
		{
			ss << girlName << gettext(" is diseased and refuses to put you at risk.\n");
			g_Girls.UpdateStat(girl, STAT_MORALITY, 2);
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
			return true;
		}
		else if (HateLove > -40)  //if she doesn't care...
		{
			ss << girlName << gettext(" is diseased, but doesn't care if you don't.\n");
			diseased = true;
		}
		else  //if she actively wants you dead
		{
			ss << girlName << gettext(" is happy for the chance to share her disease with you.\n");
			g_Girls.UpdateStat(girl, STAT_MORALITY, -2);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, 2);
			diseased = true;
		}
	}

	if (girl->is_slave())
	{
		wages += 20;  //slaves just get pocket money for being here - more if they do more
		/* */if (HateLove < -80)	ss << gettext("She hates you more then anything but you own her so she does what she is told");
		else if (HateLove < -60)	ss << gettext("She hates you but knows she must listen");
		else if (HateLove < -40)	ss << gettext("She doesn't like you but she is your slave and does what she is told");
		else if (HateLove < -20)	ss << gettext("She finds you annoying but knows she must listen");
		else if (HateLove < 0)		ss << gettext("She finds you to be annoying but you own her");
		else if (HateLove < 20)		ss << gettext("She finds you be a decent master");
		else if (HateLove < 40)		ss << gettext("She finds you be a good master");
		else if (HateLove < 60)		ss << gettext("She finds you to be attractive");
		else if (HateLove < 80)		ss << gettext("She has really strong feelings for you");
		else						ss << gettext("She loves you more than anything");
	}
	else
	{
		wages += 100; // non slaves get paid - more later if they do more
		/* */if (HateLove < -80)	{ wages += 150;	ss << gettext("She can't stand the sight of you and demands way more money to lay with you at night"); }
		else if (HateLove < -60)	{ wages += 100;	ss << gettext("She don't like you at all and wants more money to lay with you at night"); }
		else if (HateLove < -40)	{ wages += 75;	ss << gettext("She doesn't like you so she wants extra for the job"); }
		else if (HateLove < -20)	{ wages += 50;	ss << gettext("She finds you annoying so she wants extra for the job"); }
		else if (HateLove < 0)		{ wages += 25;	ss << gettext("She finds you to be annoying so she wants extra for the job"); }
		else if (HateLove < 20)		{ ss << gettext("She finds you to be okay"); }
		else if (HateLove < 40)		{ wages -= 20;	ss << gettext("She finds you to be nice so she gives you a discount"); }
		else if (HateLove < 60)		{ wages -= 40;	ss << gettext("She finds you attractive so she gives you a discount"); }
		else if (HateLove < 80)		{ wages -= 60;	ss << gettext("Shes has really strong feelings for you so she lays with you for less money"); }
		else						{ wages -= 80;	ss << gettext("She is totally in love with you and doesn't want as much money"); }
	}
	ss << ".\n\n";

	if (roll_a <= 15)
	{
		ss << gettext("\nYou did something to piss her off.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, -1);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
	}
	else if (roll_a >= 90)
	{
		ss << gettext("\nShe had a pleasant time with you.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, +3);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, 2);
	}
	else
	{
		ss << gettext("\nIt was as she expected.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, +1);
	}











#if 1

	//*****************************************NEW
	//One by one ain't a harem!
	//If we have lots of slaves let's put them all together...

	//**********ONE: check for lots of bedwarmers
	vector<sGirl*> harem = girls_on_job(brothel, JOB_PERSONALBEDWARMER, Day0Night1);

	//total girls on job
	int warmers = harem.size();

	//Slaves are the only ones we can guarantee haven't refused
	//so this is just going to include slaves
	bool other_notslave = false, other_sick = false;
	for (u_int i = warmers; i-- > 0; other_notslave = other_sick = false)
		// need to walk backwards through vector, as working forward while erasing no good
	{

		if (harem[i]->is_slave())
		{
		}
		else
		{	//If the other girl is not slave, she will not join group action. Pop this girl off the list and check the next
			other_notslave = true;
		}
		if (harem[i]->has_disease())
		{
			other_sick = true;
		}

		if (other_sick || other_notslave)
		{
			harem.erase(harem.begin() + i);
		}
	}

	//slaves left for activity
	int sz = harem.size();

	//if there's still a group, and THIS girl is a healthy slave, follow this logic - otherwise use the old logic.
	//Diseased & hatefilled girl can still do you one-to-one, but won't join group (and risk other girls' health)
	if (!diseased && sz > 1 && girl->is_slave())
	{
		//to simplify the upcoming code
		//Scenarios are options for the first Case struct, actions are for the 2nd
		int NUMSCENARIOS = 9;
		int NUMACTIONS = 11;

		wages += 50;

		//SCENARIOS
		//Was gonna use a random number for this, but since script runs seperately for each girl
		//resulted in one member of group reporting one kind of orgy, another reporting completely different one
		//using date as a common field. should result in same psuedo-random option for all
		if (Day0Night1)	roll = g_Day % NUMSCENARIOS;
		else roll = (g_Day + 1) % NUMSCENARIOS;

		ss << gettext("\nYou order the ") << sz << gettext(" slaves warming your bed to put on a show.\n");
		switch (roll)
		{
		case 0:
			ss << gettext("You put some toys on the bed and ask your girls to warm each other up. You watch for a while as around you ");
			break;
		case 1:
			ss << gettext("You get them to bathe together. Then you watch as ");
			break;
		case 2:
			if (headOnDuty) ss << gettext("Your Head Girl sits with you to watch as ");
			else ss << gettext("There are squeals, yelps, shouts and groans as ");
			break;
		case 3:
			ss << gettext("You ply them with drinks and watch as ");
			break;
		case 4:
			ss << gettext("You have work to take care of. Over on the bed ");
			break;
		case 5:
			ss << gettext("One of your gangs turns up to give you an update. The newest member openly stares at your bed where ");
			break;
		case 6:
			ss << gettext("A police chief comes to visit. You negotiate an excellent deal while he is clearly distracted by the scene in the corner, where ");
			The_Player->suspicion(-10);
			break;
		case 7:
			ss << gettext("You are tired. You sit on the sofa with a drink, barely bothering to watch as ");
			break;
		case 8:
			if ((warmers - sz) > 1) ss << gettext("The other girls in your house also watch as ");
			else if ((warmers - sz) == 1) ss << gettext("The other girl in your house watches as ");
			else ss << gettext("You notice a neighbour staring through your window as ");
			break;
		default:
			ss << gettext("Something impossible happens while ");
			break;
		}

		//Grammar fork: if just two girls
		if (sz == 2) ss << harem[0]->m_Realname << gettext(" and ") << harem[1]->m_Realname;
		else
		{
			//if more than two
			ss << harem[0]->m_Realname;
			for (u_int i = 1; i < (harem.size() - 1); i++)
			{
				ss << gettext(", ") << harem[i]->m_Realname;
			}
			ss << gettext(" and ") << harem[harem.size() - 1]->m_Realname;
		}

		//ACTIONS
		//Was gonna use a random number for this, but since script runs seperately for each girl
		//resulted in one member of group reporting one kind of orgy, another reporting completely different one
		//using date as a common field. should result in same psuedo-random option for all
		if (Day0Night1)	roll = g_Day % NUMACTIONS;
		else roll = (g_Day + 1) % NUMACTIONS;
		switch (roll)
		{
		case 0:
			ss << gettext(" share some impressively large sex toys. ");
			if (girl->has_trait( "Fast Orgasms"))
			{
				ss << girlName << gettext(" orgasms loudly and repeatedly.\n\n");
				g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 1);
				g_Girls.UpdateStat(girl, STAT_HAPPINESS, 1);
			}
			else ss << gettext("\n\n");
			break;
		case 1:
			ss << gettext(" play with anal beads and magical growth oils.\n\n");
			break;
		case 2:
			ss << gettext(" lick, touch and tease one another.\n\n");
			break;
		case 3:
			ss << gettext(" compete to whip each other across the bed. ");
			if (girl->has_trait( "Sadistic"))
			{
				ss << girlName << gettext(" loves this and loves the squeal every time she manages to crack her whip right on another girl's clit or nipples.\n\n");
				g_Girls.UpdateEnjoyment(girl, ACTION_WORKTORTURER, 2);
			}
			else if (girl->has_trait( "Masochist"))
			{
				ss << girlName << gettext(" is awful at this, with a stance like she wants to be hit. She seems to be enjoying it though.\n\n");
				g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 1);
			}
			else ss << gettext("\n\n");
			break;
		case 4:
			ss << gettext(" oil up and do some rough naked wrestling.\n\n");
			break;
		case 5:
			ss << gettext(" intimately 'clean' each other.\n\n");
			break;
		case 6:
			ss << gettext(" finger-fuck and fist each other.\n\n");
			break;
		case 7:
			ss << gettext(" crawl across your bed mouth-to-pussy in a writhing human-cunterpillar.\n\n");
			break;
		case 8:
			ss << gettext(" strip off and dance intimately together.\n\n");
			break;
		case 9:
			ss << gettext(" make excellent use of your 12-ended tentacle-beast dildo.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, +1);
			break;
		case 10:
			ss << gettext(" train together, inserting and then 'squeezing out' eggs.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, +1);
			break;
		default:
			ss << gettext(" do something unimaginable. (error)\n\n");
			break;
		}

		//Benefits of group
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, +(harem.size() / 2));
		g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, +(harem.size() / 4));
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, +(harem.size() / 2));
		int libido = (girl->has_trait( "Nymphomaniac")) ? 4 : 2;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, libido);

		// now you get involved...
		if (g_Girls.CheckVirginity(girl))
		{
			ss << girlName << gettext(" is a virgin");
			wages += 20;

			if (roll_d <= 20)
			{
				ss << gettext(" so you fuck her gently and with extra care.\n");
				g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
				g_Girls.UpdateStat(girl, STAT_PCLOVE, 1);
				g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, +1);
				g_Girls.LoseVirginity(girl);
				ss << gettext("She is no longer a virgin.\n");
				imagetype = IMGTYPE_SEX;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);

				if (!girl->calc_pregnancy(The_Player, false, 1.0))
				{
					stringstream ssm;
					ssm << girlName << " has gotten pregnant.\n";
					ss << ssm.str();
					g_MessageQue.AddToQue(ssm.str(), 0);
				}

			}
			else if (roll_d <= 35)
			{
				ss << gettext(" with a tight pussy you can't resist. You break her in hard.\n");
				g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, 2);
				g_Girls.LoseVirginity(girl);
				ss << gettext("She is no longer a virgin.\n");
				imagetype = IMGTYPE_SEX;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);

				if (!girl->calc_pregnancy(The_Player, false, 1.0))
				{
					stringstream ssm;
					ssm << girlName << " has gotten pregnant.\n";
					ss << ssm.str();
					g_MessageQue.AddToQue(ssm.str(), 0);
				}
			}
			else
			{
				ss << gettext(". You decide to keep her virginity intact for now");
				if (roll_c < 30 || girl->has_trait( "Lesbian"))
				{
					ss << gettext(", and instead have her play around with another girl in your harem.");
					g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 2);
					imagetype = IMGTYPE_LESBIAN;
					//girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, Day0Night1);
				}
				else if (roll_c < 60)
				{
					ss << gettext(", and instead fuck her ass.");
					g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
					imagetype = IMGTYPE_ANAL;
					//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
				}
				else
				{
					ss << gettext(", and instead have her 'clean it off' as you enjoy the other girls.");
					g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
					imagetype = IMGTYPE_ORAL;
					//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
				}
			}
		}
		else   // not virgin
		{
			if ((roll_d <= 20) || (girl->has_trait( "Cum Addict") && roll_d <= 50))
			{
				ss << gettext("While you enjoy another girl, ") << girlName << gettext(" grabs your cock and finishes you off with her mouth.\n");
				g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
				imagetype = IMGTYPE_ORAL;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
			}
			else if (roll_d <= 40)
			{
				ss << gettext("You watch ") << girlName << gettext(" play with another girl in the harem.");
				g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 2);
				imagetype = IMGTYPE_LESBIAN;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, Day0Night1););
			}
			else if (roll_d <= 60)
			{
				ss << gettext("You enjoy the pleasures of your harem, including fucking ") << girlName << gettext(".\n");
				g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
				g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
				imagetype = IMGTYPE_SEX;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);

				if (!girl->calc_pregnancy(The_Player, false, 1.0))
				{
					stringstream ssm;
					ssm << girlName << " has gotten pregnant.\n";
					ss << ssm.str();
					g_MessageQue.AddToQue(ssm.str(), 0);
				}
			}
			else if (roll_d <= 80)
			{
				ss << gettext("You tie up ") << girlName << gettext(" and get another girl to spank her as you fuck her.\n");
				g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
				imagetype = IMGTYPE_BDSM;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);

				if (!girl->calc_pregnancy(The_Player, false, 1.0))
				{
					stringstream ssm;
					ssm << girlName << " has gotten pregnant.\n";
					ss << ssm.str();
					g_MessageQue.AddToQue(ssm.str(), 0);
				}
			}
			else if (roll_d <= 100)
			{
				ss << gettext("You spin ") << girlName << gettext(" around, push her down and fuck her ass.");
				g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
				imagetype = IMGTYPE_ANAL;
				//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
			}
		}
		//Some group action randomness...
		if (g_Dice.percent(20) && girl->has_trait( "Shy"))
		{
			ss << gettext("\n") << girlName << gettext(" doesn't seem so shy tonight!\n");
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 1);
		}
		if (g_Dice.percent(20) && girl->has_trait( "Cum Addict"))
		{
			roll = g_Dice % 6;
			switch (roll)
			{
			case 0:
				ss << gettext("\n") << girlName << gettext(" gets upset when she sees you cum inside another girl. ")
					<< gettext("She acts quickly, spreading the girl's pussy and getting her tongue deep inside to lick and suck all the cum out.\n");
				g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
				break;
			case 1:
				ss << gettext("\n") << girlName << gettext(" gets upset when she sees you cum in another girl's mouth. She aggressively kisses the girl, trying to lick the cum right out of her mouth.\n");
				break;
			case 2:
				ss << gettext("\n") << girlName << gettext(" gets upset when she sees you cum inside another girl's ass. She quickly spreads the girl's ass and tries to suck the cum out.\n");
				g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
				g_Girls.UpdateStat(girl, STAT_DIGNITY, -1);
				break;
			case 3:
				ss << gettext("\n") << girlName << gettext(" gets upset when she sees you cum on a girl's face. She immediately pounces on the girl, licking the cum off the girl's eyes, cheek and nose.\n");
				break;
			case 4:
				ss << gettext("\n") << girlName << gettext(" gets upset when she sees you cum on a girl's breasts. She immediately pounces on the girl, licking the cum off her tits.\n");
				break;
			case 5:
				ss << gettext("\n") << girlName << gettext(" tries to kiss you afterwards, cum dripping from her mouth.\nYou decline.\n");
				break;
			default:
				ss << gettext("\n") << girlName << gettext(" is a Cum Addict doing something totally wild and unexpected (error).");
			}
			g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 2);
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
		}
		else if (g_Dice.percent(30) && HateLove > 0 && girl->has_trait( "Good Kisser"))
		{
			ss << gettext("\n") << girlName << gettext(" gives you a mindblowing kiss afterwards.\n");
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 1);
			g_Girls.UpdateStat(girl, STAT_PCLOVE, 1);
		}
		if (g_Dice.percent(20) && (girl->has_trait( "Masochist")))
		{
			ss << gettext("\n\"Next time spank me please");
			if (girl->is_slave()) ss << gettext(" master");
			ss << gettext(",\" she whispers.\n");
		}
		else if (g_Dice.percent(5) && (girl->has_trait( "Sadist") || girl->has_trait( "Aggressive")))
		{
			ss << gettext("\n\"If you made me Head Girl, I'd keep these bitches in line for you,");
			if (girl->is_slave()) ss << gettext(" master");
			ss << gettext(",\" she tells you.\n");
		}
	}
	else // if no harem - follow existing logic, copy-paste from prev version
	{
		if (HateLove >= 80) //loves you
		{
			if (g_Girls.CheckVirginity(girl))		// 25% decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe loves you greatly ");
				if (roll_d <= 25)	{ ss << gettext("but says she isn't ready so instead she "); roll_d *= 2; }
				else				{ ss << gettext("and agrees so she ");	roll_d = 77; } // normal sex
			}
			else
			{
				ss << gettext("She loves you greatly so she ");
			}
		}
		else if (HateLove >= 60)			//find u attractive
		{
			if (g_Girls.CheckVirginity(girl))		// 50 % decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be attractive ");
				if (roll_d <= 50)		{ ss << gettext("but declines so instead she ");	roll_d *= 2; }
				else					{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
			}
			else
			{
				ss << gettext("She finds you to be attractive so she ");
			}
		}
		else if (HateLove > 20)			//your okay
		{
			if (g_Girls.CheckVirginity(girl))		// 70% decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be an okay guy ");
				if (roll_d <= 70)	{ ss << gettext("but declines anyway. So she "); roll_d = 100; }
				else				{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
			}
			else
			{
				ss << gettext("She finds you to be okay so she ");
			}
		}
		else if (HateLove >= -20)			//annoying
		{
			if (g_Girls.CheckVirginity(girl))		// 80% decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be rather annoying ");
				if (roll_d <= 80)	{ ss << gettext("so she declines and she "); roll_d = 100; }
				else				{ ss << gettext("but agrees none the less. She ");	roll_d = 77; } // normal sex
			}
			else
			{
				ss << gettext("She finds you to be annoying so she ");
			}
		}
		else if (HateLove > -60)			//dont like u at all
		{
			if (g_Girls.CheckVirginity(girl))		// 95% decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe doesn't really like you ");
				if (roll_d <= 95)	{ ss << gettext("and declines so she "); roll_d = 100; }
				else				{ ss << gettext("but for whatever reason she agrees so she ");	roll_d = 77; } // normal sex
			}
			else
				ss << gettext("She don't like you at all so she ");
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
			{
				roll_d = 88;
			}
			else
			{
				roll_d = 100;
			}
		}
		else											//cant stand the site of u
		{
			if (g_Girls.CheckVirginity(girl))		// 100% decline
			{
				ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe ");
				ss << gettext("laughs hard at the thought of you touching her.\nShe ");
				if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
				{
					roll_d = 88;
				}
				else
				{
					roll_d = 100;
				}
			}
			else
				ss << gettext("She can't stand the sight of you so she ");
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
			{
				roll_d = 88;
			}
			else
			{
				roll_d = 100;
			}
		}


		if (roll_d <= 10)
		{
			g_Girls.UpdateSkill(girl, SKILL_STRIP, 2);
			ss << gettext("does a little strip show for you.\n\n");
			imagetype = IMGTYPE_STRIP;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, Day0Night1);
		}
		else if (roll_d <= 20 && is_sex_type_allowed(SKILL_TITTYSEX, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, 2);
			ss << gettext("uses her tits on you.\n\n");
			imagetype = IMGTYPE_TITTY;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_TITTY, Day0Night1);
		}
		else if (roll_d <= 30 && is_sex_type_allowed(SKILL_HANDJOB, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_HANDJOB, 2);
			ss << gettext("gives you a hand job.\n\n");
			imagetype = IMGTYPE_HAND;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_HAND, Day0Night1);
		}
		else if (roll_d <= 40 && is_sex_type_allowed(SKILL_FOOTJOB, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_FOOTJOB, 2);
			ss << gettext("gives you a foot job.\n\n");
			imagetype = IMGTYPE_FOOT;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_FOOT, Day0Night1);
		}
		else if (roll_d <= 50 && is_sex_type_allowed(SKILL_ORALSEX, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
			ss << gettext("decided to suck your cock.\n\n");
			if (girl->has_trait( "Herpes")) risky = true;
			imagetype = IMGTYPE_ORAL;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);

		}
		else if (roll_d <= 60 && is_sex_type_allowed(SKILL_ANAL, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
			ss << gettext("lets you use her ass.\n\n");
			risky = true;
			imagetype = IMGTYPE_ANAL;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
		}
		else if (roll_d <= 70 && is_sex_type_allowed(SKILL_BDSM, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
			ss << gettext("lets you tie her up.\n\n");
			risky = true;
			if (g_Girls.CheckVirginity(girl))
			{
				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
				ss << gettext("She is no longer a virgin.\n");
			}
			imagetype = IMGTYPE_BDSM;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);
			if (!girl->calc_pregnancy(The_Player, false, 1.0))
			{
				stringstream ssm;
				ssm << girlName << " has gotten pregnant.\n";
				ss << ssm.str();
				g_MessageQue.AddToQue(ssm.str(), 0);
			}
		}
		else if (roll_d <= 80 && is_sex_type_allowed(SKILL_NORMALSEX, brothel))
		{
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
			ss << gettext("has sex with you.\n\n");
			risky = true;
			if (g_Girls.CheckVirginity(girl))
			{
				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
				ss << gettext("She is no longer a virgin.\n");
			}
			imagetype = IMGTYPE_SEX;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);
			if (!girl->calc_pregnancy(The_Player, false, 1.0))
			{
				stringstream ssm;
				ssm << girlName << " has gotten pregnant.\n";
				ss << ssm.str();
				g_MessageQue.AddToQue(ssm.str(), 0);
			}
		}
		else if (roll_d <= 90)
		{
			if (HateLove > 20)
			{
				ss << gettext("let you watch her Masturbate.\n\n");
			}
			else
			{
				ss << gettext("was horny so she just Masturbated.\n\n");
			}
			g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1);
			g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -15, true);
			imagetype = IMGTYPE_MAST;
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAST, Day0Night1);
		}
		else
		{
			if (HateLove > 20)
			{
				ss << gettext("is to tired to fool around so she went to sleep.\n\n");
			}
			else
			{
				ss << gettext("just went to sleep.\n\n");
			}
			//girl->m_Events.AddMessage(ss.str(), IMGTYPE_ECCHI, Day0Night1);
			imagetype = IMGTYPE_ECCHI;
		}
	}

#else //BSIN - this was the prev version - have linked this up to 'no harem' action
	if (HateLove >= 80) //loves you
	{
		if (g_Girls.CheckVirginity(girl))		// 25% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe loves you greatly ");
			if (roll_d <= 25)	{ ss << gettext("but says she isn't ready so instead she "); roll_d *= 2; }
			else				{ ss << gettext("and agrees so she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She loves you greatly so she ");
		}
	}
	else if (HateLove >= 60)			//find u attractive
	{
		if (g_Girls.CheckVirginity(girl))		// 50 % decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be attractive ");
			if (roll_d <= 50)		{ ss << gettext("but declines so instead she ");	roll_d *= 2; }
			else					{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be attractive so she ");
		}
	}
	else if (HateLove > 20)			//your okay
	{
		if (g_Girls.CheckVirginity(girl))		// 70% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be an okay guy ");
			if (roll_d <= 70)	{ ss << gettext("but declines anyway. So she "); roll_d = 100; }
			else				{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be okay so she ");
		}
	}
	else if (HateLove >= -20)			//annoying
	{
		if (g_Girls.CheckVirginity(girl))		// 80% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be rather annoying ");
			if (roll_d <= 80)	{ ss << gettext("so she declines and she "); roll_d = 100; }
			else				{ ss << gettext("but agrees none the less. She ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be annoying so she ");
		}
	}
	else if (HateLove > -60)			//dont like u at all
	{
		if (g_Girls.CheckVirginity(girl))		// 95% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe doesn't really like you ");
			if (roll_d <= 95)	{ ss << gettext("and declines so she "); roll_d = 100; }
			else				{ ss << gettext("but for whatever reason she agrees so she ");	roll_d = 77; } // normal sex
		}
		else
			ss << gettext("She don't like you at all so she ");
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
		{
			roll_d = 88;
		}
		else
		{
			roll_d = 100;
		}
	}
	else											//cant stand the site of u
	{
		if (g_Girls.CheckVirginity(girl))		// 100% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe ");
			ss << gettext("laughs hard at the thought of you touching her.\nShe ");
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
			{
				roll_d = 88;
			}
			else
			{
				roll_d = 100;
			}
		}
		else
			ss << gettext("She can't stand the sight of you so she ");
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
		{
			roll_d = 88;
		}
		else
		{
			roll_d = 100;
		}
	}




	if (roll_d <= 10)
	{
		g_Girls.UpdateSkill(girl, SKILL_STRIP, 2);
		ss << gettext("does a little strip show for you.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, Day0Night1);
	}
	else if (roll_d <= 20 && is_sex_type_allowed(SKILL_TITTYSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, 2);
		ss << gettext("uses her tits on you.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_TITTY, Day0Night1);
	}
	else if (roll_d <= 30 && is_sex_type_allowed(SKILL_HANDJOB, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_HANDJOB, 2);
		ss << gettext("gives you a hand job.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_HAND, Day0Night1);
	}
	else if (roll_d <= 40 && is_sex_type_allowed(SKILL_FOOTJOB, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_FOOTJOB, 2);
		ss << gettext("gives you a foot job.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_FOOT, Day0Night1);
	}
	else if (roll_d <= 50 && is_sex_type_allowed(SKILL_ORALSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
		ss << gettext("decided to suck your cock.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
	}
	else if (roll_d <= 60 && is_sex_type_allowed(SKILL_ANAL, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
		ss << gettext("lets you use her ass.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
	}
	else if (roll_d <= 70 && is_sex_type_allowed(SKILL_BDSM, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
		ss << gettext("lets you tie her up.\n\n");
		if (g_Girls.CheckVirginity(girl))
		{
			g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
			ss << gettext("She is no longer a virgin.\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);
		if (!girl->calc_pregnancy(The_Player, false, 1.0))
		{
			stringstream ssm;
			ssm << girlName << " has gotten pregnant.\n";
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
	}
	else if (roll_d <= 80 && is_sex_type_allowed(SKILL_NORMALSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
		ss << gettext("has sex with you.\n\n");
		if (g_Girls.CheckVirginity(girl))
		{
			g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
			ss << gettext("She is no longer a virgin.\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);
		if (!girl->calc_pregnancy(The_Player, false, 1.0))
		{
			stringstream ssm;
			ssm << girlName << " has gotten pregnant.\n";
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
	}
	else if (roll_d <= 90)
	{
		if (HateLove > 20)
		{
			ss << gettext("let you watch her Masturbate.\n\n");
		}
		else
		{
			ss << gettext("was horney so she just Masturbated.\n\n");
		}
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1);
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -15, true);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAST, Day0Night1);
	}
	else
	{
		if (HateLove > 20)
		{
			ss << gettext("is to tried to fool around so she went to sleep.\n\n");
		}
		else
		{
			ss << gettext("just went to sleep.\n\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ECCHI, Day0Night1);
	}
#endif
#if 1
	//BSIN bit more randomness
	if (girl->is_addict(true) && g_Dice.percent(20))
	{
		int theft = g_Dice.in_range(5, 50);
		ss << gettext("\nWhile you're not looking, she steals ") << theft << gettext(" gold from your room to feed her addiction.\n");
		wages += theft;
	}
	if (g_Dice.percent(30) && girl->has_trait( "Natural Pheromones"))
	{
		int wowfactor = g_Dice.in_range(5, 55);
		ss << gettext("\nSomething about her drives you wild. You pay her ") << wowfactor << gettext(" gold extra.\n");
		wages += wowfactor;
	}
	if (g_Dice.percent(30) && (girl->has_trait( "Exotic") || girl->has_trait( "Furry")))
	{
		ss << gettext("\n") << girlName << gettext(" mentions that this reminds her of a 'coming of age' ritual back home.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 1);
	}
	else if (g_Dice.percent(20) && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Slut") || girl->has_trait( "Open Minded")))
	{
		ss << gettext("\n") << girlName << gettext(" actually seems suited to this.\n\"Can I do this again");
		if (girl->is_slave()) ss << gettext(", master?\"\n");
		else ss << gettext("?\"\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, 2);
	}

	//BSIN - disease risk, mostly a cost, but also an expensive way to cure a girl of serious disease
	if (diseased && risky && !girl->m_UseAntiPreg)
	{
		if (girl->has_trait( "Chlamydia") && g_Dice.percent(30))
		{
			g_Gold.misc_debit(500);
			g_Girls.RemoveTrait(girl, "Chlamydia");
			stringstream ssm;
			ssm << girlName << gettext(" gave you Chlamydia.\nYou spend 500 gold getting your shit cleaned up.\nYou use the medicine to clear her up too.\n");
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
		else if (girl->has_trait( "Herpes") && g_Dice.percent(30))
		{
			g_Gold.misc_debit(1000);
			g_Girls.RemoveTrait(girl, "Herpes");
			g_Girls.UpdateStat(girl, STAT_PCLOVE, 1);
			g_Girls.UpdateStat(girl, STAT_PCHATE, -1);
			stringstream ssm;
			ssm << girlName << gettext(" gave you herpes.\nMedicine for it costs you 1,000 gold.\nYou use the medicine to clear her up too.\n");
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
		else if (girl->has_trait( "Syphilis") && g_Dice.percent(30))
		{
			g_Gold.misc_debit(3500);
			g_Girls.RemoveTrait(girl, "Syphilis");
			g_Girls.UpdateStat(girl, STAT_PCLOVE, 3);
			g_Girls.UpdateStat(girl, STAT_PCHATE, -3);
			stringstream ssm;
			ssm << girlName << gettext(" gave you syphilis.\nMedicine for it is hard to track down, costing you 3,500 gold.\nYou share it with her.\n");
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
		else if (girl->has_trait( "AIDS") && g_Dice.percent(30))
		{
			g_Gold.misc_debit(8000);
			g_Girls.RemoveTrait(girl, "AIDS");
			g_Girls.UpdateStat(girl, STAT_PCLOVE, 6);
			g_Girls.UpdateStat(girl, STAT_PCHATE, -6);
			stringstream ssm;
			ssm << girlName << gettext(" gave you AIDS.\n8,000 gold later and the wizards' incantations have finally cleansed you both.\n");
			ss << ssm.str();
			g_MessageQue.AddToQue(ssm.str(), 0);
		}
	}

#endif
	if (wages < 0) wages = 0;
	g_Gold.girl_support(wages);  // wages come from you
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, libido = 3;

	if (girl->has_trait( "Quick Learner"))		{ xp += 3; }
	else if (girl->has_trait( "Idiot Savant"))	{ xp += 5; }
	else if (girl->has_trait( "Slow Learner"))	{ xp -= 3; }
	else if (girl->has_trait( "Retarded"))		{ xp -= 5; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);
	return false;
}


double cJobManager::JP_PersonalBedWarmer(sGirl* girl, bool estimate)// not used
{
	return 0;
}

bool cJobManager::WorkPersonalBedWarmers(sBrothel* brothel)
{

	return false;
}
