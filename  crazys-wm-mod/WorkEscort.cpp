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
#include <algorithm>

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Brothel - Bar
bool cJobManager::WorkEscort(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKESCORT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " has been assigned to work as an Escort. She is informed that various men will ask for her to accompany them on dates, whether because they need a date for a social engagement of some kind or because of their own loneliness. Her skills in service, her beauty, her charisma, her intelligence, and her refinement may all be tested to provide the ideal date that each client requests. And, of course, should she decide to spend some \"extra\" time with the client, she will need to perform well with each of their sexual proclivities. This is her choice, however.\n\n";

	if (g_Girls.HasTrait(girl, "Deaf"))
	{
		ss << girlName << " is deaf, meaning she would be unable to hear the conversation that is so critical to being successful as an escort. As there is practically no chance that a client will want to have an entire date in sign language, assuming he even knows it, " << girlName << " is particularly unsuited to work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
		//end job
	}
	else if (g_Girls.HasTrait(girl, "Mute"))
	{
		ss << girlName << " is mute, and while some men enjoy a woman who stays silent, these men are not paying escorts to engage them in conversation. As it is severely unlikely that a client will want to spend the entire date deciphering sign language, even if they do know it, " << girlName << " is particularly unsuited for work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
		//end job
	}

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int jobperformance = 0, wages = 0, tips = 0, escort =0, fame = 0;
	int imagetype = IMGTYPE_FORMAL;

	int agl = (g_Girls.GetStat(girl, STAT_AGILITY));
	int roll_a = g_Dice.d100();							// customer type
	int roll_b = g_Dice.d100();							// customer wealth
	int roll_c = g_Dice.d100() + agl;					// agility adjustment
	int roll_d = g_Dice.d100();
	int roll_sex = g_Dice.d100();
	int prepare = (g_Girls.GetStat(girl, STAT_AGILITY) + g_Girls.GetStat(girl, SKILL_SERVICE)/2);

	int sex = false;
	int anal = false;
	int oral = false;

	int titty_lover = false;
	int ass_lover = false;
	int sex_event = false;
	int sex_offer = false;
	int group_offer = false;

	//CRAZY
	/*Escort plans
	General idea is to have her arrive to an appoiment with a client.  If she is to late they may turn her away
	resulting in no money if she is on time it goes to the next part.  Next part is they look her over seeing if
	she looks good enough for them or not and looks at what she is wearing.  Fancy dresses help while something like
	slave rags would hurt and may get her sent away again.  Then they are off to do something like go to dinner, a bar,
	maybe a fancy party what have u.  From there its can she hold his intrest and not embarres him.  Traits play the biggest
	part in this.  An elegant girl would do well for a rich person where a aggressive girl might not. On the other hand
	a deadbeat might be turned off by a elegant girl and prefer something more sleazy or such.  If they pass this part
	it goes to a will they have sex roll.  Sometimes they might be okay with not having sex with the girl and be more then
	happy with just her company other times sex is a must.  Do a roll for the sex type while taking into account what
	types of sex are allowed.  Her skill will determine if the client is happy or not.  Sex would be an extra cost.  The
	further a girl makes it and the happier the client the more she makes.  Deadbeat would be super easy to please where
	a king might be almost impossable to make fully happy without the perfect girl.  Sending a virgin should have different
	things happen if it comes to sex where the girl could accept or not.  Maybe have special things happen if the escort is
	your daughter.*/

	//a little pre-game randomness
	if (girl->is_fighter(true))
	{
		ss << girlName << "has some particularly aggressive tendencies that make her a poor choice for patrons seeking polite conversation. Even if she carries a conversation well, the undercurrent of restrained hostility will detract from her performance.\n";
		escort -= 2;
	}
	if (g_Girls.HasTrait(girl, "Bimbo"))
	{
		ss << girlName << " is a complete ditz. Her repertoire of conversation is limited to her favorite dresses and her many powerful feelings on makeup brands. This will stymy her abilities as an escort, no matter how well she does otherwise.\n";
		escort -= 1;
	}
	if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Shy"))
	{
		ss << girlName << " is very shy with new people. For a job that requires meeting with new people constantly, this is a bad trait to possess. Her nervousness will translate into some awkward conversational moments, no matter how great her charisma is.\n";
		escort -= 2;
	}
	if (g_Girls.HasTrait(girl, "Retarded"))
	{
		ss << girlName << " is retarded, and this is a natural obstacle to most conversations. For those men that do not want a prolonged conversation on her favorite colors or a description of her last \"poopy,\" which is the vast majority of men, she may have some trouble as an escort.\n";
		escort -= 3;
	}
	//good
	if (g_Girls.HasTrait(girl, "Psychic"))
	{
		ss << girlName << " has some psychic abilities, meaning she has a natural advantage in conversation. She knows what the client likes and can avoid the subjects that he dislikes. This may come in very handy.\n";
		escort += 2;
	}
	if (g_Girls.HasTrait(girl, "Charismatic") || g_Girls.HasTrait(girl, "Charming"))
	{
		ss << girlName << " is very personable and charming. Conversations with her will be easy, and this gives her an advantage as an escort.\n";
		escort += 2;
	}


	// `J` The type of customer She Escorts
	/*default*/	int cust_type = 1;    string cust_type_text = "Commoner";
	/* */if (roll_a <= 1)	{ cust_type = 9; cust_type_text = "King"; }
	else if (roll_a <= 3)	{ cust_type = 8; cust_type_text = "Prince"; }
	else if (roll_a <= 6)	{ cust_type = 7; cust_type_text = "Noble"; }
	else if (roll_a <= 10)	{ cust_type = 6; cust_type_text = "Judge"; }
	else if (roll_a <= 15)	{ cust_type = 5; cust_type_text = "Mayor"; }
	else if (roll_a <= 21)	{ cust_type = 4; cust_type_text = "Sheriff"; }
	else if (roll_a <= 45)	{ cust_type = 3; cust_type_text = "Bureaucrat"; }
	else if (roll_a <= 65)	{ cust_type = 2; cust_type_text = "Regular"; }
	else if (roll_a >= 98)	{ cust_type = 0; cust_type_text = "Deadbeat"; }

	// `J` The wealth of customer She Escorts
	/*default*/	int cust_wealth = 2;	string cust_wealth_text = "";
	/* */if (roll_b <= 20)	{ cust_wealth = 3; cust_wealth_text = "rich "; }
	else if (roll_b <= 40)	{ cust_wealth = 1; cust_wealth_text = "poor "; }
	else if (roll_b >= 98)	{ cust_wealth = 0; cust_wealth_text = "broke "; }

	// `J` do job performance
	ss << "She ";
	/* */if (roll_c >= 150)	{ jobperformance += 20;	ss << " arrived early"; }
	else if (roll_c >= 100)	{ jobperformance += 10;	ss << " was on time"; }
	else if (roll_c >= 80)	{ jobperformance += 0;	ss << " was a few minutes late"; }
	else if (roll_c >= 50)	{ jobperformance -= 5;	ss << " was late"; }
	else /*             */	{ jobperformance -= 10;	ss << " was very late"; }
	ss << " to her appointment with a " << cust_wealth_text << cust_type_text << ".\n";

	//// Where do they go?
	//*default*/	int loc_type = 1;    string loc_type_text = "a Restaurant";
	///* */if (roll_d <= 1)	{ loc_type = 8; loc_type_text = "Vacation"; }
	//else if (roll_d <= 3)	{ loc_type = 7; loc_type_text = "a Wedding"; }
	//else if (roll_d <= 6)	{ loc_type = 6; loc_type_text = "a Party"; }
	//else if (roll_d <= 10)	{ loc_type = 5; loc_type_text = "an Arena Match"; }
	//else if (roll_d <= 15)	{ loc_type = 4; loc_type_text = "the Movies"; }
	//else if (roll_d <= 45)	{ loc_type = 3; loc_type_text = "the Strip Club"; }
	//else if (roll_d <= 65)	{ loc_type = 2; loc_type_text = "the Bar"; }
	//else if (roll_d >= 98)	{ loc_type = 0; loc_type_text = "the Park"; }
	//ss << "They went to " << loc_type_text << " together.\n";


	// `J` do wages and tips
	if (cust_type * cust_wealth <= 0 || g_Dice.percent(2))	// the customer can not or will not pay
	{
		wages = tips = 0;
		if (g_Dice.percent(25))	// Runner
		{
			if (g_Gangs.GetGangOnMission(MISS_GUARDING))
			{
				sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
				if (g_Dice.percent(gang->m_Stats[STAT_AGILITY]))
				{
					ss << " The customer tried to run off without paying. Your men caught him before he got away.";
					SetGameFlag(FLAG_CUSTNOPAY);
					wages = max(g_Dice%girl->askprice(), girl->askprice() * cust_type * cust_wealth);	// Take what customer has
				}
				else	ss << " The customer couldn't pay and managed to elude your guards.";
			}
			else	ss << " The customer couldn't pay and ran off. There were no guards!";
		}
		else
		{
			// offers to pay the girl what he has
			if (g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))
			{
				// she turns him over to the goons
				ss << " The customer couldn't pay the full amount, so your girl turned them over to your men.";
				SetGameFlag(FLAG_CUSTNOPAY);
			}
			else	ss << " The customer couldn't pay the full amount.";
			wages = max(g_Dice%girl->askprice(), g_Dice % (girl->askprice() * cust_type * cust_wealth));	// Take what customer has
		}
	}
	else
	{
		wages = girl->askprice() * cust_type * cust_wealth;
		tips = (jobperformance > 0) ? (g_Dice%jobperformance) * cust_type * cust_wealth : 0;
	}


	// work out the pay between the house and the girl
	girl->m_Tips = max(tips, 0);
	girl->m_Pay = wages;

	ss << "\n\n" << girlName << " receives " << wages << " in payment for her work as an Escort for a " << cust_type_text << " client. Her fame as an Escort has changed by " << fame << ".";


	// Improve stats
	int xp = 20, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill + 1);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill + 1);
	g_Girls.UpdateStat(girl, STAT_FAME, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with customers and talking with them about their problems has made " + girlName + " more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Elegant", 40, actiontype, "Playing the doting girlfriend has given " + girlName + " an Elegant nature.", Day0Night1);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 40, actiontype, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Aggressive", 70, actiontype, "Controlling her temper has greatly reduced " + girlName + "'s Aggressive tendencies.", Day0Night1);

	return false;
}

double cJobManager::JP_Escort(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance += g_Girls.GetAverageOfSexSkills(girl) + (girl->charisma() + girl->beauty()) / 2;

	}
	else// for the actual check
	{
	}
	return jobperformance;
}