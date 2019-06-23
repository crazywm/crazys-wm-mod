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
bool cJobManager::WorkBarCook(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKBAR;
	stringstream ss; string girlName = girl->m_Realname;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN - replaced with more informative mssg
		//ss << girlName << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		ss << girlName << " refused to cook food in the bar " << (Day0Night1 ? "tonight." : "today.");
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


	if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
	{
		ss << "You have assigned " << girlName << " to work as a cook in the kitchens this week. She stares at the kitchen, with all the pots and pans and utensils, and then back at you, and then down at her own body, as if trying to draw your attention to the fact that she has no hands. After silently waiting for a response that you never give, she turns her gaze back to the kitchen and contemplates how she will even begin with this impossible task.\n \n";
	}
	else if (girl->has_trait( "Retarded"))
	{
		ss << girlName << " beams like a small child when you tell her that she is cooking today. You witness as she throws all the nearby ingredients into the same bowl, places the bowl on the stove, and then starts punching the mixture as if that is an approved cooking technique. \"I cook food good!\" she exclaims. Whatever possessed you to make a retarded girl into a cook, now you know that you are committed to seeing this through until the end.\n \n";
	}
	else if (girl->has_trait( "Assassin"))// Impact unknown; randomly very bad, maybe?
	{
		ss << girlName << "'s skills as an assassin have left her with a slightly skewed view towards food, which she casually refers to as \"poison masking.\" You are not sure whether you should have the waitresses serve what she cooks, and you definitely are not going to allow her to re-cook a meal that an angry customer sends back. Actually, there are a lot of knives in kitchens! You had not noticed that until right now. So many knives.\n \n";
	}
	else
	{
		ss << girlName << " is assigned to work as a cook in the kitchen, preparing food for the customers.\n \n";
	}

	g_Girls.UnequipCombat(girl);  // put that shit away, you'll scare off the customers!


	sGirl* barmaidonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_BARMAID, Day0Night1);
	string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->m_Realname + "" : "the Barmaid");

	int wages = 15, tips = 0;
	int enjoy = 0, fame = 0;

	int imagetype = IMGTYPE_COOK;
	int msgtype = Day0Night1;


	#pragma endregion
	#pragma region //	Job Performance			//

	double jobperformance = JP_Barcook(girl, false);

	//a little pre-game randomness
	if (g_Dice.percent(10))
	{
		if (girl->has_trait( "Chef"))
		{
			ss << girlName << " understands the difference between a \"Cook\" and a \"Chef.\" She ventures away from the recipe book occasionally, spicing each dish to her own concept of perfection. It is usually a great improvement.\n";
			jobperformance += 15;
		}
		else if (girl->has_trait( "Agile"))
		{
			ss << girlName << " looks like she is handling a dozen pots and pans at the same time without any problems! She is certainly efficient.";
			jobperformance += 5;
		}
		else if (girl->has_trait( "Mind Fucked"))
		{
			ss << girlName << " works efficiently, but stares off into space with that vacant mindfucked expression of hers. You would think there was nobody inside her brain at all, but sometimes, when she finishes cooking a dish, she beams a maniacal grin and starts muttering about being a \"good girl.\" \"And good girls,\" she smiles as she jams the end of the spatula into her asshole, \"get dessert!\" She giggles, removes the spatula, and then starts using it for the next dish. That cannot be hygienic.";
			jobperformance -= 10;
		}
		else if (girl->has_trait( "One Hand"))
		{
			ss << girlName << " struggles with only having one hand in the kitchen, but makes the best of it.";
			jobperformance -= 15;
		}
	}


	if (jobperformance >= 245)
	{
		ss << "  She must be the perfect at this.\n \n";
		wages += 155;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << "The food is incredible! But how?! How in hell did she do it without any hands? It just.. it just boggles your mind. Maybe she carries the pots with her mouth? How does she plate the food? Honestly, this whole thing, while very impressive, has left you with far more questions than answers.\n";
		}
		else if (roll_b >= 50)
		{
			if (girl->has_trait( "Chef"))
			{
				ss << girlName << " circulates briefly among the tables, talking to the patrons and asking their preferences so she can customize the menu for each of them. They are amazed by her abilities, as she not only lovingly prepares each dish, but plates every morsel like an artist. She circulates again after they eat, asking how they enjoyed their courses, and taking notes to improve them the next time. Somehow, even with all this time out of the kitchen talking with the customers, she manages to ensure that each dish arrives just in time, perfectly prepared and at the ideal temperature.\n";
				//tips here
			}
			else
			{
				ss << "She is an absolute frenzy of culinary activity, diving from plate to plate and stove to stove with expert grace. The food always appears right on time, just the right temperature, and elegantly plated.\n";
			}
		}
		else
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "There is no telling how she did it, whether by blind luck or through hidden idiot savant abilities, but " << girlName << " actually made good food. Great food, really. Truly surprisingly great food. She does not seem to understand the science of cooking, or which ingredients are which, but somehow the final product is fit for any master chef. \"I did good cooking, yes?\" she exclaims to you before grabbing a turnip and repeatedly bashing it against the table to \"make softer.\" While you have never seen this particular method of vegetable preparation before, you cannot argue with the results. For reasons which should be obvious, you opt against introducing her to those customers that ask for the privilege of thanking the chef.\n";
				//tips here
			}
			else
			{
				ss << "She is a master chef, and she leaves the traditional menu far behind as she prepares individualized tasting plates for each patron. " << girlName << " carefully prepares an appetizer of delicate quail eggs for one customer while drizzling rich chocolate on a raspberry tart for another. You only receive high compliments from the patrons, who promise to come back for more.\n";
			}
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.\n \n";
		wages += 95;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << "You are at a loss as to how she made such satisfying food without the use of her hands, but hardly one to look a gift horse in the mouth, you accept it and move on.\n";
		}
		else if (roll_b >= 66)
		{
			ss << girlName << " does very well today, and receives a number of compliments on her cooking. She improves on the traditional menu in a number of small but appreciable ways, perhaps by taking a minute to sear the meat just a bit longer, or chopping the vegetables into attractive florets. All in all, there are certainly no complaints.\n";
		}
		else if (roll_b >= 33)
		{
			if (girl->has_trait( "Chef"))
			{
				ss << girlName << " circulates briefly among the tables, talking to the patrons and asking their preferences so she can customize the menu for each of them. She considers this personal touch to be important, and while her skills are not sufficiently masterful to amaze each customer, they are all satisfied with the results. She loses some time cooking while she makes the rounds, meaning that some dishes arrive a few minutes later than would be ideal, but everyone seems to enjoy the personal touch.\n";
				//tips here
			}
			else
			{
				ss << girlName << " attempts something completely new today. She places chicken meat into the cavity of a duck, which she then stuffs into the cavity of a turkey, and then roasts all three together in some kind of perverse poultry centipede. You have your doubts about this lunacy, but actually, it tastes delicious! You are not so sure about her name for it, \"Fowl Play,\" but the customers love it.\n";
			}
		}
		else
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "Despite her mental limitations, " << girlName << " manages to whip up some truly satisfying food. Nobody knows how she does it, as she seems to just randomly throw various ingredients into each dish, but it serendipitously just works. You have some reservations about her penchant to throw kale into everything, and her repeated exclamations that it helps \"make poopy come on time\" for the customers are not overly persuasive. Nevertheless, the food tastes really good, so why fight it?\n";
			}
			else
			{
				ss << girlName << " shows her skills today with a seven-course tasting menu for the patrons, replete with saffron trout, roasted beef joints, and bone-marrow soup. While you have enjoyed better food in your life, you have done so only rarely. " << girlName << " is well on her way to becoming a master chef.\n";
			}
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.\n \n";
		wages += 55;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << girlName << "'s final products are surprisingly acceptable, given her lack of hands, but you can say nothing more positive about the dishes. Merely satisfactory. Perhaps the patrons would be more appreciative if they knew the herculean efforts that went into their food.\n";
		}
		else if (roll_b >= 66)
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "For a retarded girl, " << girlName << " is actually not that bad. Not great, and only bordering on good, but at least not as bad as you would have anticipated. She tends to put meat in everything and has for no explicable reason forsworn all wheat products, claiming that it is \"better for strong person eat like caveman,\" but this does not generate as many complaints as you would have imagined.\n";
			}
			else
			{
				ss << girlName << " is an acceptable cook, and manages every item on the menu without disappointment. She is not as fast as she could be, and some of the greens are occasionally wilted and the chicken rubbery, but all in all, the customers are satisfied with her performance.\n";
			}
		}
		else if (roll_b >= 33)
		{
			ss << girlName << " is still experimenting with some more advanced culinary concepts. The frog-leg milkshake is a dubious creation, but surprisingly, the few customers with enough temerity to order it have largely positive reviews. She is not a great cook yet, but with more practice, she very well could become one.\n";
		}
		else
		{
			if (girl->has_trait( "Chef"))
			{
				ss << girlName << " considers herself a chef, and tries to deviate from the menu with some additional spices or plating styles. She clearly would like her efforts to be noticed, but with the final product being merely satisfactory, nobody asks to meet and thank the cook. She resolves to try harder next time.\n";
			}
			else
			{
				ss << girlName << " sticks to the menu today, showing no originality or culinary ambition. Nevertheless, the food is satisfactory, timely, and generally well-received. Perhaps " << girlName << " will show more skill and experimentation as she becomes more comfortable with cooking.\n";
			}
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
		wages += 15;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << girlName << " does about as well as you could hope for someone with no hands in a kitchen. The food is acceptable enough to be served, and honestly, you are not sure you can ever expect any better than that from her.\n";
		}
		else if (roll_b >= 66)
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "The results are better than you would expect, but only because the bar was so low. This food is edible, and at times even acceptable, but not good. Someone at some point must have told " << girlName << " that gluten is bad for people, and since she has no idea what gluten is, this leads to some interesting additions and omissions in her meals. The burger has a wheat bun, for instance, but she refused to use potatoes for the fries and opted instead for turnips. You contemplate explaining to her what gluten is, but conclude that it would be a pointless effort.\n";
			}
			else
			{
				ss << girlName << " is able to follow the routine recipe instructions, but only barely. She works step-by-step through each dish, without imagination or experimentation, and produces acceptable results. She is a long way from a quiche, but at least she can fry onion rings.\n";
			}
		}
		else if (roll_b >= 33)
		{
			ss << girlName << "'s cooking abilities are merely okay, and she knows it. She wisely avoids the more expensive ingredients, knowing that she does not have the experience to cook them without ruining the dish, and focuses on the basics. The customers enjoy some simple and basic fare. They may want something more refined, but if you can keep them sufficiently liquored up, maybe they will not even notice.\n";
		}
		else
		{
			if (girl->has_trait( "Chef"))
			{
				ss << "You are not sure what culinary academy decided to make " << girlName << " a chef, but you suspect that it was in a foreign country with radically different concepts of what constitutes edible food. She is at best an acceptable cook, and her efforts at floretting vegetables and drizzling sauce reductions onto her plates would be adorable if they were not so sad. Her efforts at difficult dishes are disasters, and most patrons wisely stick to simple pork chops.\n";
			}
			else
			{
				ss << girlName << " tried something new, which is not a great idea for someone still mastering the basics of cooking. The pork blood pudding she made was an ambitious undertaking, but when the customers saw the gelatinous mess, they all opted for a simple hamburger. " << girlName << " was disappointed, but resolved to try harder next time.\n";
			}
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		wages -= 5;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << girlName << " heroically lifts a pot from the burner with her mouth, setting it to cook on the counter. She bites down on a knife handle and bobs her head back and forth to cut the vegetables. The end result is barely edible, but who would have the heart to tell her that?\n";
		}
		else if (roll_b >= 66)
		{
			ss << "The results are not good. " << girlName << " may be able to boil water without setting the kitchen on fire, but those are the limits of her abilities. She is able to produce some of the most simple items on the menu, like assembling the hamburgers, and is able to satisfy a few customers. The remainder of her food is either sent back or thrown away.\n";
		}
		else if (roll_b >= 33)
		{
			if (girl->has_trait( "Chef"))
			{
				ss << "Somebody, at some point, convinced " << girlName << " that she was a chef. This person was a terrible liar. She pours \"wine reductions,\" which is basically just slightly-heated wine from your cellars, all over dishes that do not benefit from her efforts. Actually, they mostly just turn into a soupy mess. The patrons are not as upset as you would expect, though, since " << girlName << " did not heat the wine enough to burn away the alcohol, and they all essentially got a free drink.\n";
			}
			else
			{
				ss << girlName << " wants to experiment with some new dishes for the menu, but her cooking is still so bad that every time she cooks could be considered an experiment. Stewed pig's ears are a disaster. Nobody takes her up on her turnip dumplings. Maybe she will stumble onto an exciting new recipe one day, but today is not that day.\n";
			}
		}
		else
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "The results are bad. You had to know this when you assigned her. She is, after all, retarded, and combining ingredients based solely on their colors is not a prudent way of cooking. Not that she cares, of course. This is just amazing fun for her. You instruct the staff to keep the most expensive ingredients out of her reach, because that kobe beef will almost certainly be ground together with ketchup and bananas into a barely palatable milkshake.\n";
			}
			else
			{
				ss << girlName << " is not very good at this. She is terrified of half the items on the menu, which are well beyond her skills, and barely scrapes by with the other half. The fries are soggy, the onion rings are basically raw, and the pork chops are cooked black. She does not even attempt the more complicated dishes, either diverting those orders to other cooks or leaving those customers waiting.\n";
			}
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		wages -= 15;

		if (girl->has_trait( "No Arms") || girl->has_trait( "No Hands"))
		{
			ss << "The results are what you would expect for a girl who has to carry, cut, cook, and plate everything with her mouth. It is awful. You consider telling the staff to 'give her a hand,� but conclude that the remark would be considered in poor taste. Not as poor taste as " << girlName << "'s cooking, of course, but pretty close.\n";
		}
		else if (roll_b >= 66)
		{
			if (girl->has_trait( "Retarded"))
			{
				ss << "The food is terrible, of course. Seriously. Of course it is. Did you know that she was retarded? Raw eggs and pork rinds do not mix well with coconut milk. " << girlName << " had just a magical time throwing everything together, and she is smiling from ear to ear, but she is the only one. Honestly, what did you expect?\n";
			}
			else
			{
				ss << "Eyeing the aftermath, you doubt whether this could have gone much worse. The kitchen is a disaster area, and pots and pans are strewn across the countertops with aborted efforts rotting inside. The customers were repulsed by everything that she cooked, and even the dogs outside are turning up their noses at the massive piles you are forced to throw away. This is not cooking, this is mad science.\n";
			}
		}
		else if (roll_b >= 33)
		{
			if (girl->has_trait( "Chef"))
			{
				ss << "It seems that just about anybody can be called a \"chef\" these days. " << girlName << "'s food edible the way that roots and leaves might be considered edible, but nobody actually wants to order this trash. She painstakingly labored on floretting a baby tomato to place on top of a soupy, vomit-like substance that she is calling a bisque, but the tomato ends up looking like a little mashed ketchup stain. Today could certainly have gone better.\n";
			}
			else
			{
				ss << girlName << " is not a cook. She has no idea what she is doing. The stove baffles her, and the smoking blackened chunks of \"bread\" that come out baffle the customers. A pot still lies on a burner, the water long since burned away, glowing orange and smelling like death. She sheepishly turns off the burner and turns her head away from your gaze. This was an unmitigated disaster.\n";
			}
		}
		else
		{
			ss << "The kitchen did not burn down today. That is the only positive thing you can draw from the whole experience. Customers were disgusted by everything " << girlName << " cooked, if \"cooking\" is the right word for it, and not a few walked away shaking their heads. Any profits were destroyed by all of the wasted ingredients. At least the rats will eat well tonight, because " << girlName << " has to throw away practically everything she attempted.\n";
		}
	}

	//try and add randomness here
	if (girl->has_trait( "Clumsy") && g_Dice.percent(15))
	{
		ss << "Customers might wonder what that occasional cacophony of breaking glass coming from the kitchen is all about. Not you, though. You know that " << girlName << " is slowly destroying your supply of dishes with her clumsiness. At least it was not another grease fire.\n";
		wages -= 15;
		jobperformance -= 10;
	}

	if (girl->has_trait( "Homeless") && g_Dice.percent(15))
	{
		ss << girlName << " has lived on the streets for so long that certain habits become unbreakable. When she is surrounded by food, for example, she usually cannot help but fill her pockets with leftovers and morsels that she will hoard for later. This may explain why each dish seems to need more ingredients than usual to prepare.\n";
		wages -= 25;
		jobperformance -= 5;
	}

	if (girl->has_trait( "Blind"))
	{
		ss << "Some foodstuffs tend to feel the same way, and even smell the same, which is usually not a problem for a competent chef. " << girlName << " is blind, however, so she.. and also the customers.. may be surprised to find out what ends up in the food.\n";
		wages -= 10;
		jobperformance -= 10;
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
        girl->confidence(1);
	girl->cooking(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gain traits

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her breaking hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Barcook(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->cooking() +
		// secondary - second 100
		((girl->intelligence() + girl->confidence()) / 2) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Quick Learner"))  jobperformance += 5;
	if (girl->has_trait( "Psychic"))		  jobperformance += 10;
	if (girl->has_trait( "Chef"))			  jobperformance += 30;
	if (girl->has_trait( "Mixologist"))	  jobperformance += 20;	//Good with measures

	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))		jobperformance -= 20; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 10;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 20;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 125;
	if (girl->has_trait( "No Feet"))		jobperformance -= 50;
	if (girl->has_trait( "No Hands"))		jobperformance -= 75;
	if (girl->has_trait( "No Legs"))		jobperformance -= 40;
	if (girl->has_trait( "Blind"))		jobperformance -= 30;
	if (girl->has_trait( "Deaf"))			jobperformance -= 15;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
