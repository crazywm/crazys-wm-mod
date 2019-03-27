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
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " has been assigned to work as an Escort. She is informed that various men will ask for her to accompany them on dates, whether because they need a date for a social engagement of some kind or because of their own loneliness. Her skills in service, her beauty, her charisma, her intelligence, and her refinement may all be tested to provide the ideal date that each client requests. And, of course, should she decide to spend some \"extra\" time with the client, she will need to perform well with each of their sexual proclivities. This is her choice, however.\n \n";

	if (girl->has_trait("Deaf") && g_Dice.percent(50))
	{
		ss << girlName << " is deaf, meaning she would be unable to hear the conversation that is so critical to being successful as an escort. As there is practically no chance that a client will want to have an entire date in sign language, assuming he even knows it, " << girlName << " is particularly unsuited to work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
		return false;
	}
	else if (girl->has_trait("Mute") && g_Dice.percent(50))
	{
		ss << girlName << " is mute, and while some men enjoy a woman who stays silent, these men are not paying escorts to engage them in conversation. As it is severely unlikely that a client will want to spend the entire date deciphering sign language, even if they do know it, " << girlName << " is particularly unsuited for work as an escort. You should consider alternate employment for her. Nobody chooses her this week.\n";
		return false;
	}

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 0, tips = 0;
	double jobperformance = 0, escort = 0, fame = 0;
	int imagetype = IMGTYPE_ESCORT;

	int agl = (girl->agility());
	int roll_a = g_Dice.d100();							// customer type
	int roll_b = g_Dice.d100();							// customer wealth
	int roll_c = g_Dice.d100() + agl;					// agility adjustment
	int roll_d = g_Dice.d100();
	int roll_sex = g_Dice.d100();
	int prepare = (girl->agility() + girl->service()/2);
	double cust_wealth = 1;
	int cust_type = 0;
	string cust_type_text = "";

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
	if (girl->has_trait( "Bimbo"))
	{
		ss << girlName << " is a complete ditz. Her repertoire of conversation is limited to her favorite dresses and her many powerful feelings on makeup brands. This will stymy her abilities as an escort, no matter how well she does otherwise.\n";
		escort -= 1;
	}
	if (girl->has_trait( "Nervous") || girl->has_trait( "Shy"))
	{
		ss << girlName << " is very shy with new people. For a job that requires meeting with new people constantly, this is a bad trait to possess. Her nervousness will translate into some awkward conversational moments, no matter how great her charisma is.\n";
		escort -= 2;
	}
	if (girl->has_trait( "Retarded"))
	{
		ss << girlName << " is retarded, and this is a natural obstacle to most conversations. For those men that do not want a prolonged conversation on her favorite colors or a description of her last \"poopy,\" which is the vast majority of men, she may have some trouble as an escort.\n";
		escort -= 3;
	}
	//good
	if (girl->has_trait( "Psychic"))
	{
		ss << girlName << " has some psychic abilities, meaning she has a natural advantage in conversation. She knows what the client likes and can avoid the subjects that he dislikes. This may come in very handy.\n";
		escort += 2;
	}
	if (girl->has_trait( "Charismatic") || girl->has_trait( "Charming"))
	{
		ss << girlName << " is very personable and charming. Conversations with her will be easy, and this gives her an advantage as an escort.\n";
		escort += 2;
	}

// The type of customer She Escorts
///*default*/	int cust_type = 2;    string cust_type_text = "Commoner";
//{ cust_type = 10; cust_type_text = "King"; }
//{ cust_type = 9; cust_type_text = "Prince"; }
//{ cust_type = 8; cust_type_text = "Noble"; }
//{ cust_type = 7; cust_type_text = "Judge"; }
//{ cust_type = 6; cust_type_text = "Mayor"; }
//{ cust_type = 5; cust_type_text = "Sheriff"; }
//{ cust_type = 4; cust_type_text = "Bureaucrat"; }
//{ cust_type = 3; cust_type_text = "Regular"; }
//{ cust_type = 1; cust_type_text = "Deadbeat"; }

enum escortchoice
		{
			/*Es_King,
			Es_Prince,
			Es_Noble,
			Es_Judge,
			Es_Mayor,
			Es_Sheriff,
			Es_Bureaucrat,*/
			Es_Regular,
			Es_Commoner,
			Es_DeadBeat,			//


			MD_NumberOfEscortChoices	// Leave this as the last thing on the list to allow for random choices.
		};								// When the choice gets handled, the switch will use the "MD_name" as the case.

	// the test for if the girl can act on that choice is done next
	int choice = 0;	bool choicemade = false;

	while (!choicemade)
	{
			if (girl->fame() >= 20 && roll_a <= 65)
			{
				choice = Es_Regular;
				choicemade = true;	// ready so continue
			}
			else if (girl->fame() >= 10 && roll_a <= 90)
			{
				choice = Es_Commoner;
				choicemade = true;	// ready so continue
			}
	else
	{
	choice = g_Dice % MD_NumberOfEscortChoices;	// randomly choose from all of the choices
	switch (choice)
		{
			// these don't need a test
			case Es_DeadBeat:
			default:
			choice = Es_DeadBeat;
			choicemade = true;	// ready so continue
			break;
		}
	}
};


switch (choice)
{
	//case Es_Bureaucrat:
	//		{
	//		}
	//		break;	// end Es_Bureaucrat

case Es_Regular:
{
	cust_type = 3; cust_type_text = "Regular";
	//Regular event
	//telling what she is meeting and where
	ss << girlName << " has been requested by a regular client, a man who has called on her before and now is willing to pay extra for a special assignation. His new boss has asked the client out for a double date with the client's girlfriend and his own wife so he can get to know his new employee better. The problem is that the client does not have a girlfriend. " << girlName << " is to arrive at a local restaurant, equipped with a suitable dress and pair of shoes, ready to chat amiably. The client sends a list of written details about their \"shared relationship\" for her to memorize.\n";

	//how prepared and when does she arrive...
	if (prepare > 99)//very early very prepared
	{
		ss << girlName << " immediately delves into the customer biography on the client and does some independent research on him. She memorizes the list in total detail, and even scopes out the places where they are supposed to have met and had their previous dates. She arrives at the door of the restaurant at the exact appointed time.\n";
		escort += 3;
	}
	else if (prepare >= 75)//early and prepared
	{
		ss << girlName << " arrives at the door of the bustling downtown restaurant just in time. She actually arrived half an hour ago, and has spent her time scoping out the restaurant and its patrons and contemplating conversation topics. She read the biography that your escort clients are obligated to provide, and knows enough details about her repeat client and their fictional relationship that she is ready to make it appear believable.\n";
		escort += 2;
	}
	else if (prepare >= 50)//on time and prepared
	{
		ss << girlName << " arrives just on time, with a relatively good understanding of the details of their shared \"relationship\" so she can appear as if she really is his girlfriend.\n";
		escort += 1;
	}
	else if (prepare >= 25)//kind prepard sort late
	{
		ss << girlName << " arrives ten minutes late, dressed suitably and mulling over conversation topics. She knows her lateness will probably upset him, but she has at least reviewed some of the list of details of their \"relationship\" so that she does not need to completely improvise.\n";
	}
	else//very late and uprepared
	{
		ss << girlName << " arrives at the restaurant half an hour late, barely groomed and with minimal efforts to make herself presentable. She has not memorized anything on the list and will just have to improvise the details of their \"relationship\".\n";
		escort -= 1;
	}

	//beauty check
	ss << girlName << " enters the restaurant and locates the client, his boss, and his boss's wife sitting at a table for four in a quiet corner. She approaches the table, ready to make her first impression.\n";
	if (girl->beauty() > 99)
	{
		ss << girlName << " is not merely beautiful, she is absolutely stunning. The client knows this already, and while he is still somewhat transfixed by her charms, he is able to focus on his boss, who is practically drooling himself. The wife moves uncomfortably in her seat as this moment draws out longer. \"My goodness,\" says the boss to the client. \"You certainly did well for yourself!\" What only " << girlName << " can see during their little exchange, however, is that the wife continues shifting in her chair uncomfortably, staring deep into her eyes while casually rubbing her lips with one finger. Interesting.\n";
		escort += 3;
	}
	else if (girl->beauty() >= 75)
	{
		ss << girlName << " is a very attractive woman, and while the boss's wife is a little threatened and perturbed by how long her husband stares at his employee's \"girlfriend,\" the client is pleased by the wink of congratulations that his boss shares with him a moment later. What neither the boss nor the client see, however, is the long interested stare that the wife gives her. There is an interest in that glance that goes beyond the professional kind.\n";
		escort += 2;
	}
	else if (girl->beauty() >= 50)
	{
		ss << girlName << " is attractive, but not enough so to be threatening to the boss's wife, and while the boss gives her an appraising and thorough review with his eyes, his wife simply smiles pleasantly at her and asks that she tell her all about where she gets her makeup.\n";
		escort += 1;
	}
	else if (girl->beauty() >= 25)
	{
		ss << "While " << girlName << " is not blessed with a lot of natural beauty, this certainly does no harm here. The client has already met her and been impressed by her other attributes, and while his boss does not pay much attention to her, his wife seems happy that she is the most attractive woman at the table.\n";
	}
	else
	{
		ss << "While the client knows " << girlName << " and is aware of her plainness, the boss and his wife have not seen her before, and she forces herself to swallow her pride when the wife immediately makes a sarcastic comment about how pleased she is that her husband continues to employ individuals with such \"high standards\".\n";
		escort -= 1;
	}


	//boob event
	if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs")
					|| girl->has_trait( "Titanic Tits") || girl->has_trait( "Big Boobs")
					|| girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs") && g_Dice.percent(75))
				{
					ss << girlName << " knows that this assignation is all about pleasing the boss. So when she sees his eyes lock onto her substantial tits, she gives a quick victorious smile to her \"boyfriend.\" The boss's wife glances down at her own modest breasts with discomfort, but soon breaks the silence with a comment about the lovely weather.\n";
					escort += 1;
					if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
						{
							ss << "Being a bit of an exhibitionist, " << girlName << " decides to give the boss a quick \"accidental\" show when his wife is distracted by the menu. She leans forward enticingly to study the menu, giving him a full view of her deep cleavage.\n";
							if (g_Dice.percent(75))
							{
								ss << "The boss winks at the client and makes a joke about the \"immense\" profits he can both expect to enjoy later.\n";
								escort += 1;
							}
							else
							{
								ss << "The wife turns back just in time to find her husband staring shamelessly into that cleavage, but while " << girlName << " expects everything to get a little rocky after that, the wife only turns to her with a knowing smile and a quick appreciative stare at her cleavage as well.\n";
								escort += 2;
							}
						}
				}


	if (girl->has_trait( "Sexy Air") || girl->has_trait( "Great Figure") || girl->has_trait( "Hourglass Figure"))
	{
		ss << "The boss immediately takes note of " << girlName << "'s sexy body with his eyes, and swallows hard, unable to form words for a few seconds. His wife does not seem to notice the effects on her husband as she is also staring, trying to figure out what it is that is so transfixing about her.\n";
		escort += 1;
	}


	if (girl->has_trait( "Bruises"))
	{
		ss << "The dress and makeup are not enough to hide the bruises covering " << girlName << "'s body, and both the boss and his wife share a significant look before shifting their gaze to the client, clearly expecting an answer. \"I fell down some stairs,\" offers " << girlName << " with embarrassment, though they do not seem fully convinced of her explanation.\n";
		escort -= 1;
	}


	if (girl->has_item_j("Bad Nurse Costume") != -1 || girl->has_item_j("Sexy Nurse Costume") != -1)
	{
		ss << "The three of them do spend a moment staring at the revealing \"nurse\" costume that " << girlName << " has decided to wear to this assignation. \"You didn't tell me that she was in the medical profession,\" says the boss, his gaze lingering on her very short skirt while his wife chokes on her drink.\n";
		escort -= 1;
	}
	else if (girl->has_item_j("Black Knight Uniform Coat") != -1)
	{
		ss << "\"My goodness, you didn't tell me that your lovely girlfriend was with the Royal Guards,\" states the boss as they all pass their eyes over " << girlName << "'s Black Knight Uniform Coat. \"We must be on good behavior this evening,\" he jokes.\n";
	}
	else if (girl->has_item_j("Chinese Dress") != -1 || girl->has_item_j("Enchanted Dress") != -1 || girl->has_item_j("Hime Dress") != -1 || girl->has_item_j("Linen Dress") != -1 || girl->has_item_j("Miko Dress") != -1 || girl->has_item_j("Bourgeoise Gown") != -1 || girl->has_item_j("Faerie Gown") != -1 || girl->has_item_j("Peasant Gown") != -1)
	{
		ss << "\"What a lovely dress, darling,\" the wife begins with a compliment. " << girlName << " is happy to see that the client is pleased with her choice of attire.";
		escort += 1;
	}
	else if (girl->has_item_j( "Brothel Gown") != -1 || girl->has_item_j("Trashy Dress") != -1)
	{
		ss << "\"What a.. lovely.. dress,\" says the wife, elbowing her husband, who is staring with an open mouth at " << girlName << "'s trashy brothel attire. The client winces slightly.";
		escort -= 1;
	}
	else if (girl->has_item_j("Bunny Outfit") != -1)
	{
		ss << "All three patrons stare with consternation at the revealing bunny outfit that " << girlName << " is wearing. The boss sweats a little while locking eyes on her cleavage, and his wife politely inquires if perhaps she just got off of work at a casino. The client hides his face and deep embarrassment in his hands.";
		escort -= 2;
	}
	else if (girl->has_item_j("Dancer Costume") != -1)
	{
		ss << "\"Oh, you're a dancer!\" cries the wife with pleasure, looking at " << girlName << "'s dancer costume. \"How wonderful! I always wanted to be a dancer when I was younger. You must tell me everything about it!\"\n";
		escort += 1;
	}
	else if (girl->has_item_j("Gantz Suit") != -1 || girl->has_item_j("Plug Suit") != -1)
	{
		ss << "Nobody says anything as " << girlName << " sits, but they are all staring speechless at the skin-tight leather/latex dress she is wearing. \"Oh my,\" whispers the boss, loosening his collar and turning red. His wife just continues staring as the client buries his face in his hands.\n";
		escort -= 3;
	}
	else if (girl->has_item_j("Gemstone Dress") != -1 || girl->has_item_j("Noble Gown") != -1 || girl->has_item_j("Royal Gown") != -1 || girl->has_item_j("Silken Dress") != -1 || girl->has_item_j("Velvet Dress") != -1)
	{
		ss << "\"My goodness, what a beautiful dress!\" cries the wife, gazing at " << girlName << "'s expensive attire. While she is clearly overdressed for the occasion, both the boss and his wife simply chalk it up as a charming attempt to support her boyfriend by being extra presentable.\n";
	}
	else if (girl->has_item_j("Maid Uniform") != -1)
	{
		ss << "\"Do you work at one of the nearby estates?\" inquires the wife, looking at " << girlName << "'s maid uniform. \"I'm so glad you were able to join us with your work schedule.\" The client is surprised, but not disappointed, with the choice of attire, and quickly invents a story about her job.\n";
	}
	else if (girl->has_item_j("Nurse Uniform") != -1)
	{
		ss << "\"Oh, you must be coming straight from the hospital,\" exclaims the wife, looking over " << girlName << "'s nurse uniform. \"I'm so glad you could join us. I know how demanding those poor nurses work over there.\" The client is surprised at the choice of attire, but quickly runs with it, inventing a story about her job.\n";
	}
	else if (girl->has_item_j("School Uniform") != -1)
	{
		ss << "\"Aren't you a pretty young thing,\" says the wife, looking over " << girlName << "'s school uniform, and then back at the client. \"You didn't tell me she was continuing her education! What school do you attend, darling?\" she inquires, and the client quickly invents a story to justify the outfit.\n";
	}
	else if (girl->has_item_j("Slave Rags") != -1)
	{
		ss << "\"Oh, you poor thing,\" says the wife softly, taking in the slave rags that " << girlName << " is wearing. \"Is everything.. all right.. with your, um, job?\" asks the boss, awkwardly trying to navigate around the fact that " << girlName << " is apparently a slave who does not possess anything more than these revealing rags. The client struggles valiantly to explain how he has come to fall in love with a poor slave and is trying to buy her freedom. Everyone sits in uncomfortable silence for a moment.\n";
		escort -= 1;
	}



	//CONVERSATION PHASE (CHARISMA CHECK)
	ss << "The four order a few bottles of sake to share amongst themselves and a veritable pile of sushi. For once in her work as an Escort, the focus of the attention tonight is not on " << girlName << " but on her client, and her only role is to appear supportive of her \"boyfriend\" in front of her boss, and to make their relationship believable.\n";
	if (girl->service() > 99)
	{
		ss << girlName << " knows everything about this fictitious relationship. She also knows about the details of the job that the client just got, and is able to talk with great specificity about the circumstances of their meeting, where they enjoy going on dates, and how proud she was when he came home�here, with perfect details of his house�to tell her about his new job.\n";
		escort += 3;
	}
	else if (girl->service() >= 75)
	{
		ss << "Given the amount of research that " << girlName << " conducted prior to arriving, the relationship is easily believable. She knows everything about the two of them, and is able to describe with great accuracy how they met, and all of their previous dates. When the conversation drifts towards her job, she is able to provide substantial details about her fictitious employment.\n";
		escort += 2;
	}
	else if (girl->service() >= 50)
	{
		ss << "Fortunately, " << girlName << "'s study of their relationship before arriving gives her plenty of details. Whenever the conversation drifts towards their own history, such as where they met and what they like to do together, she is able to provide the answers that the client expects, which makes the process run smoothly. She has to invent quite a bit about her own job, however.\n";
		escort += 1;
	}
	else if (girl->service() >= 25)
	{
		ss << "The relationship has only the barest hint of believability with the level of details that " << girlName << " managed to memorize before arriving. She knows where they met and what she does for a living, but that is about all. She is forced to improvise the remainder, and while the client is visibly upset for a moment at some of her answers during the conversation, he manages to roll with it and follows her lead.\n";
	}
	else
	{
		ss << "It is hard to make that relationship believable when " << girlName << " did none of the necessary research. Her lack of specific knowledge about their dating history means that she is forced to improvise throughout the entire conversation, and the client looks on with dismay at her wild answers to questions before he resigns himself to the situation and begins following her lead, trying to make it all up as he goes.\n";
		escort -= 1;
	}



	if (girl->performance() > 99)
	{
		ss << girlName << " is the perfect actress. Every lie comes across with such conviction, replete with minute detail, that soon even the client is convinced that this relationship is a real thing. She positively sparkles when she talks about the magical evening when they first met, and she paints a picture of her own daily life that they are all soon engrossed with her narrative of the interpersonal dramas of her own fictitious workplace.\n";
		escort += 3;
	}
	else if (girl->performance() >= 75)
	{
		ss << girlName << " is trained as an actress, so when she starts talking about their relationship and her own life, she paints a vivid and thoroughly believable picture of all of it. She is able to improvise some details that were not in the client's list, and all three of them listen appreciably while she describes the night that they met and how happy she was when he told her all about his new job.\n";
		escort += 2;
	}
	else if (girl->performance() >= 50)
	{
		ss << girlName << " is a decent performer, so when she begins telling all of these details to the boss and his wife, she manages to make it sound convincing. The two of them are thoroughly persuaded that the relationship is real, though the lack of conviction and detail in the performance makes " << girlName << " seem bland and forgettable as the dinner concludes, primarily with the other three leading most of the conversation.\n";
		escort += 1;
	}
	else if (girl->performance() >= 25)
	{
		ss <<  girlName << " cannot perform well as an actress, and no matter what details she may know about the relationship, she has a difficult time making it believable. She shares her part of the conversation like she is reading from a hidden teleprompter. Fortunately, while her performance is not very convincing, the client is able to keep everything believable, but only barely, as the dinner comes fortuitously to an end.\n";
	}
	else
	{
		ss << "Her performance telling these lies is, however, incredibly poor. She has no acting ability, and no matter what details she may know about the two of them, every sentence she speaks about their relationship sounds forced and fake. The boss and his wife share a suspicious glance at one point before the client intervenes and redirects the conversation to more comfortable ground before the dinner ends.\n";
		escort -= 1;
	}

	//random stuff
	if (girl->has_trait( "Dominatrix"))
	{
		ss << "This conversation would have gone better in one respect: if " << girlName << " were not such a dominant girl, she would have wisely allowed the client to lead the conversation, as it is ultimately his relationship with his own boss that is ultimately most important. Her constant interjections and attempts to steer the conversation where she wants it to go did give the impression that the client had picked a particularly chatty girl with some difficulty at understanding social cues.\n";
		escort -= 1;
	}

	if (girl->has_trait( "Alcoholic"))
	{
		ss << "The conversation would have gone better if " << girlName << " had not started shooting the sake as fast as it was delivered. Her alcoholism managed to capture everyone's attention, and the boss and his wife did share one silent concerned look with the client as " << girlName << " started her fifth bottle.\n";
		escort -= 1;
	}
	else if (girl->has_trait( "Social Drinker"))
	{
		ss << "The sake flowed freely throughout the dinner, and " << girlName << ", being a social drinker, energetically convinces all three others to keep pace with her. They all start enjoying themselves with the social lubricant in no time, and the dinner party took a more jovial tone than any of them had expected.\n";
		escort += 1;
	}

	if (girl->has_trait( "Porn Star"))
	{
		ss << "Throughout the dinner, " << girlName << " did notice the boss occasionally staring at her with curiosity. \"I could swear I've seen you before somewhere,\" he muttered, unable to put his finger on it, but somehow all the more interested in her. " << girlName << " suspects that her work as a porn star may mean that he certainly has seen her before, and he has probably seen a lot of other people putting their fingers, and other body parts on \"it,\" too.\n";
		escort += 1;
	}

	if (girl->has_trait( "Bisexual"))
	{
		ss << girlName << " is finely attuned to the sexual signals of both men and women, since she is attracted to both, and she can easily detect the vibes that the boss's wife is giving. They are both looking her over with more than casual interest, and this gives " << girlName << " some interesting ideas.\n";
		escort += 2;
	}
	else if (girl->has_trait( "Lesbian"))
	{
		ss << girlName << " may escort men for a living, but her real romantic interest is with other women. Consequently, she detects a certain vibe coming from the boss's wife throughout the dinner that she recognizes pretty quickly. The two of them share some lingering looks that the men do not see or understand, but they know that there is the potential for something here tonight.\n";
		escort += 1;
	}


	//RESULTS PHASE (POINT CHECK)
	if (escort >= 9)
	{
		ss << "The four of them are laughing like old friends by the time the bill arrives, and " << girlName << " can feel looks of attraction and lust coming her way from the client, which is expected, and from the boss, which is a nice bonus. She can also feel them coming from the boss's wife, which was something she had not expected this evening, but the looks were strong enough that she was not completely surprised when the wife looked knowingly at her husband and whispered something in his ear. He smiles for a second and then looks at his new employee and " << girlName << ". \"I know this may be a bit unorthodox,\" he begins, \"but we've been having a great evening, and we wanted to know if.. maybe.. the two of you wanted to keep the night going at our place?\" The client looks at " << girlName << " with surprise and amazement, but then thinks about it and smiles. His look changes to that of an open question, meaning that he will take " << girlName << "'s lead on the answer.\n";
		fame += 2;
		group_offer = true;
		cust_wealth = 2;
	}
	else if (escort >= 6)
	{
		ss << "As the dinner comes to a pleasant close, the client says all his goodbyes and escorts " << girlName << " out the door. \"As always,\" he says once they are alone, \"you are amazing. I can't believe that worked. Here is your fee, plus some extra for making this all work. So.. uh..\" he continues, \"do you want to keep the fiction going for a bit and come back to my place for a celebratory drink?\"\n";
		fame += 1;
		sex_offer = true;
		cust_wealth = 1.5;
	}
	else if (escort >= 1)
	{
		ss << "As the dinner comes to a close, the client fidgets and looks for excuses for the two of them to leave. As soon as one appears, he escorts " << girlName << " quickly out of the restaurant. \"I can't believe that worked,\" he says, nervously, \"but it almost didn't. Thanks, but let's just call it a night. Take your fee,\" he whispers, discreetly handing her an envelope with her payment. \"I'll tell them you had a work thing tonight and go chat some more. Thanks,\" he says dismissively as he turns back towards the restaurant.";
	}
	else
	{
		ss << "The dinner ends with the boss and his wife sharing a skeptical glance as the client hurries " << girlName << " out the door to an imaginary late-night work assignment. \"That was absolutely terrible,\" he whispers ferociously to her when they are alone outside. \"Now I have to go back in and do damage control. For all I know, you may have just cost me my job. Maybe that's my fault for taking this risk with you, but I can definitely tell you one thing: I will not be paying for this. Tell your boss that he needs to be much more careful with his assignments in the future.\" He turns angrily on his heels, composes himself, and reenters the restaurant, leaving " << girlName << " to walk back to your establishment, asking herself about the myriad things she did wrong.";
		fame -= 1;
		cust_wealth = 0;
	}

	//SEX PHASE (LIBIDO CHECK)
	if (group_offer)
	{
		if (girl->morality() >= 50)
		{
			ss << girlName << " is savvy enough to know what they are suggesting, but is too conservative in her morals to consider having sex with all of them. She politely declines, much to the client's regret, but tells them that she did have a wonderful evening. The client escorts her outside and thanks her for her incredible performance. \"I know that the group offer might not be your thing,\" he says, \"but I thought maybe you and I might keep our little fake relationship going for another hour or two, if you're interested,\" he concludes, waiting hopefully for her answer.\n";
			sex_offer = true;
		}
		else if (girl->libido() <= 50)
		{
			ss << girlName << " politely considers the offer, but the fact of the matter is that she has no interest in sex with anyone tonight. She invents an excuse about having a work event to get to, apologizing profusely, but suggesting that maybe the four of them grab another dinner sometime soon. The client is surprised at the suggestion, since it means that he may have to pay for another group date with his \"girlfriend,\" but he is hardly angry, as the possibility of fucking his boss's wife is a bit too enticing for him to ignore. The client escorts her outside and discreetly hands her an envelope with a substantial tip for her excellent work tonight.";
		}
		else
		{
			if (girl->check_virginity())
			{
				ss << girlName << " smiles at the offer, and leans a bit over the table before answering. \"I think we would love to join you,\" she says. They all grin with anticipation, getting up to leave. As they walk out the door, " << girlName << " leaves the client's side for a moment to speak to the wife. \"I think you should know,\" she whispers, \"that I am still a virgin, and that the two of us have decided to wait until marriage.\" The wife appears disappointed, but livens as " << girlName << " continues: \"This doesn't mean that the two of us can't get to know each other a little better at your place, though. As long as the boys know that my body is off limits, I think we could still have a very enjoyable evening.\" The wife smiles and turns to whisper something to her husband while " << girlName << " discreetly informs the client of the arrangement. He smiles as well, and the four of them soon arrive at a lovely expensive home, where they soon begin undressing themselves around a large bed.\n";
				if (girl->lesbian() > 99)
				{
					ss << "Within minutes, the wife has her lips wrapped around her husband's cock as the client spears her ass. She writhes in pleasure, pulling " << girlName << "'s head towards her open pussy. " << girlName << " licks with masterful skill, nibbling with expertise on the clitoris as she thrusts her fingers into the pussy, timing each thrust with the attacks on her ass and mouth. She is truly a master at cunnilingus, and by the time the men have exploded their semen into the sweaty woman, she is screaming with passion on her fifth powerful orgasm, spraying her juices everywhere. They all lie panting on the bed afterwards, with the wife purring in ecstasy.\n";
					fame += 3;
				}
				else if (girl->lesbian() >= 75)
				{
					ss << "Within minutes, the wife has her lips wrapped around her husband's cock as the client spears her ass. She writhes in pleasure, pulling " << girlName << "'s head towards her open pussy. " << girlName << " passionately attacks her clitoris between the thrusts, fingering her pussy vigorously. The four fuck in ecstasy for half an hour, with the wife rocking back and forth with three powerful orgasms, spurting her juices all over " << girlName << "'s face just as the client explodes in her ass and she swallows deeply from her husband's cum. The four lay there, panting, on the bed.\n";
					fame += 2;
				}
				else if (girl->lesbian() >= 50)
				{
					ss << "Within minutes, the wife has her lips wrapped around her husband's cock as the client spears her ass. She writhes in pleasure, pulling " << girlName << "'s head towards her open pussy. " << girlName << " licks with skill, timing her tongue to hit with the client's thrusts into her ass. She inserts a few fingers as she nibbles on the clitoris, and by the time the wife has collapsed onto the bed, leaking cum from her ass and mouth, she has enjoyed two powerful orgasms.\n";
					fame += 1;
				}
				else if (girl->lesbian() >= 25)
				{
					ss << "Within minutes, the wife has her lips wrapped around her husband's cock as the client spears her ass. She writhes in pleasure, pulling " << girlName << "'s head towards her open pussy. " << girlName << " licks with the limited skill she possesses, being somewhat new to the act of pleasuring another woman. Fortunately, with the help of the two men, " << girlName << " is able to bring one good orgasm out of her before the wife collapses on the bed, covered in their cum.\n";
				}
				else
				{
					ss << "Within minutes, the wife has her lips wrapped around her husband's cock as the client spears her ass. She writhes in pleasure, pulling " << girlName << "'s head towards her open pussy. " << girlName << " licks it amateurishly, completely ignorant of how to satisfy another woman. While the powerful fucking continues, the three of them pay little attention to the inexperienced efforts of " << girlName << ", and mostly manage to please each other. They are all somewhat surprised by her lack of skills, but by the time the wife manages to bring both men to orgasm, leaking cum from her mouth and ass, there is no doubt that they still had a good time, despite her.\n";
					fame -= 1;
				}
				imagetype = IMGTYPE_LESBIAN;
				girl->lesbian(2);
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
			else
			{
				ss << "\"What a wonderful idea,\" announces " << girlName << ", bringing anticipatory grins from the other three. \"I'm sure we would love to share ourselves with you this evening.\" The four of them are soon entering the door of the boss's lovely expensive home, and within minutes they are all undressing each other on a large bed. \"The best business arrangements are about sharing, after all,\" smiles " << girlName << " as she positions herself to pleasure all three of them.\n";
				if (girl->group() > 99)
				{
					ss << "While some women might be at a loss as to how to pleasure two cocks and a pussy at the same time, " << girlName << " is a master at group fucking and soon shows her expertise. She rotates herself constantly between the other three, sometimes just sucking them alternately with her mouth, sometimes enjoying a cock in her mouth and another in her ass as the wife fists her with reckless abandon. Both women have orgasmed at least a handful of times by the time the men pull out of them, push their heads together, and shoot their loads violently into " << girlName << "'s mouth. She swallows with a wan smile, giving the wife's asshole onoe last vigorous fingering. The four collapse in an ecstatic pile on the bed, with all three of the others staring with incredulity at " << girlName << ". \"You are incredible,\" they agree, panting and gasping for air.\n";
					fame += 3;
				}
				else if (girl->group() >= 75)
				{
					ss << girlName << " definitely knows her way around a group fuck, and soon she is rotating the two men between her ass and pussy as she furiously nibbles on the wife's clit. They roll around each other, moaning with pleasure, as " << girlName << " brings orgasm after orgasm from the other woman, whilst simultaneously bouncing with enthusiasm on the two cocks buried inside her. Soon " << girlName << " is leaking with cum from her lower holes and streaked with pussy juices on her mouth. The three others collapse in ecstasy around her, amazed by her incredible performance. \"What a night,\" says the boss, gasping for air on his corner of the bed.\n";
					fame += 2;
				}
				else if (girl->group() >= 50)
				{
					ss << girlName << " is not a stranger to group sex, and she is soon enjoying the boss's cock in her ass and the client's in her pussy as she licks voraciously at the wife's clit. The four of them rotate between each other for a blissful half hour before the men pull out, just in time for both women to kiss and stroke each other passionately as the men shoot their loads all over their sensuously mashing tits. The four collapse happily into the bed, panting with ecstasy.\n";
					fame += 1;
				}
				else if (girl->group() >= 25)
				{
					ss << girlName << " has only limited experience in these kinds of situations, however, and her efforts at fucking the cock in her ass while licking the wife's pussy are merely amateurish. Nevertheless, the four of them are able to enjoy the time together, and as the client explodes his cum deep into the wife's ass, " << girlName << " feels a spray of the wife's pussy juices in her mouth as the boss shoots his load deep into her womb. The four collapse contended into the bed.\n";
				}
				else
				{
					ss << "Unfortunately, " << girlName << " has no idea what to do with two cocks and a pussy. She tries to lick the pussy, but is soon distracted by the cock filling her ass and stops licking while she concentrates on her own sensations. The wife is surprised and disappointed, as are the other men, who eventually turn their attentions to the wife when they realize that anything more than one partner confuses " << girlName << " into inaction. By the end of the session, the two men are double-teaming the wife, and " << girlName << " is relegated to occasional licks. She watches, embarrassed in her own lack of skills, as they poor their cum all over the rabidly panting woman.\n";
					fame -= 1;
				}
				imagetype = IMGTYPE_GROUP;
				girl->group(2);
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
			ss << "Once they have recovered, " << girlName << " and the client dress themselves, thank the other two for a truly wonderful night, and begin the walk back towards their own destinations. Before they part, the client kisses " << girlName << " again and hands her an envelope with her fee, plus a hefty bonus for her incredible performance. \"I only wish you were actually my girlfriend,\" he says sheepishly. \"Well,\" she replies, giving him another kiss, \"if you ever want to have a fake girlfriend again some night, you know where to find me.\"\n";
		}
	}

	if (sex_offer)
	{
		if (girl->morality() >= 50)
		{
			ss << girlName << " blinks with surprise. \"I am a nice conservative girl,\" she says, weighing this in her mind, \"but this is not a first date for us, and I guess you could say that you are my boyfriend tonight, if only in pretend.\" She thinks it over for a minute before surrendering to her own desires. \"Listen,\" she begins, \"we can't have sex, but I think I might be able to help you in some other ways.\" The client grins with pleasure and escorts her back to his place, opening the door and inviting her inside. She smiles nervously and silently reaches down to caress his manhood through his pants.";
			oral = true;
		}
		else if (girl->libido() <= 50)
		{
			ss << girlName << " is flattered, but not interested in anything with this client. She politely declines his offer, mentioning that her work does not permit it and her employer has already assigned her other work for the night, and lightens the mood with a final few jokes before departing the restaurant to return to your establishment. He takes it well, if a little disappointed that he cannot consummate their fictional relationship, but is grateful that at least his plan worked.";
		}
		else
		{
			ss << girlName << " smiles back, with a flash of lust in her eyes. \"That is what a boyfriend and girlfriend would do now, isn't it,\" she concludes, rubbing her body against his as she kisses him. \"Lead the way.\" Soon they are back at his door and he is inviting her inside, though he is barely through the entrance before she grabs his cock through his pants and moans with savage passion.\n";
			sex_event = true;
		}
	}

	if (sex_event)
	{
		if (oral)
		{
			if (girl->oralsex() > 99)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She cannot take him deeply out of concern for her gag reflex, but she knows that her incredible mouth will be more than enough to pleasure him anyway, and licks and sucks various parts of his cock and balls as he builds towards orgasm, moaning with pleasure the whole time. She forces herself to take him deeply right before he cums, making herself gag on him but using her involuntary reflex as another way of massaging his shaft. He shoots a violent explosion of cum into her mouth and then staggers back, collapsing on his couch. \"That was amazing,\" he pants as " << girlName << " licks the cum from her lips.\n";
							fame += 2;
						}
						else
						{
							ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She takes him deeply in one swallow and holds him there, locked in her as she swallows repeatedly to massage him with her throat. She stays there, intentionally depriving herself of oxygen for his pleasure, until she feels him building towards orgasm. She then releases, teasing him. \"Not yet,\" she whispers before turning her phenomenal tongue to his balls. She returns to his shaft as he recovers himself, and then slowly and masterfully builds the tension until she knows the time is approaching. She then plunges herself deep onto his cock again, licking his balls as her throat plays around his tip, until he shoots a violent explosion of cum straight down to her stomach. \"That was absolutely incredible,\" moans the client as he collapses onto his couch.\n";
							fame += 3;
						}
					}
					else if (girl->oralsex() >= 75)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She gently begins licking his balls while wrapping her hand delicately over his shaft. After a few minutes of ball pleasuring, she begins licking his shaft with increased pressure, sucking on his tip passionately. She does not go deeper, aware that her gag reflex would cause problems, but is easily able to satisfy him with the licks and shallow sucking alone. Soon he builds to a powerful orgasm that floods her mouth after a few more minutes of her expert technique.\n";
							fame += 1;
						}
						else
						{
							ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She takes him deeply in one swallow and holds herself all the way to the base of his cock, darting her tongue out to lick his balls as she does so, and then begins pumping her head back and forth. The client moans with pleasure as she expertly works around his manhood, and he builds to a powerful orgasm that floods her mouth after a few minutes of her masterful technique.\n";
							fame += 2;
						}
					}
					else if (girl->oralsex() >= 50)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"Allow me to show my appreciation for a lovely evening,\" says " << girlName << " as she falls to her knees and pulls his cock out from his pants. She lovingly licks every part of his crotch, teasing his balls with her tongue and then engulfing his tip with her mouth as she rotates her tongue around the head. She avoids going any deeper out of concern for her gag reflex, which ultimately results in a mediocre blowjob, but the client is soon shooting semen until it leaks out of the sides of her mouth.\n";
						}
						else
						{
							ss << "\"Allow me to thank you,\" says " << girlName << " as she lowers herself to her knees and removes his cock from his pants. She takes his erection deep into her mouth and massages it with her throat, and then begins to pump her head back and forth, stopping to occasionally lick his balls. It is not the best blowjob he has ever received, but it is nevertheless very pleasurable, and " << girlName << " is soon rewarded for her efforts with an eruption of cum into her mouth.\n";
							fame += 1;
						}
					}
					else if (girl->oralsex() >= 25)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build towards orgasm. " << girlName << "'s gag reflex suddenly kicks in, though, which is probably why she was staying so shallow on the tip in the first place, and she is soon coughing all over his cock and pulling herself away to recover. This kills the mood for a period, and it is not until she regains composure and the client masturbates himself into her mouth that he is able to cum. The whole experience leaves him slightly disappointed.\n";
							fame -= 1;
						}
						else
						{
							ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build up enough tension to ejaculate. It is all in all a mediocre experience, but he is not one to complain about a free blowjob from a lovely lady, and at the very least, the climax is enjoyable.\n";
						}
					}
					else
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. Disaster strikes, however, when she tries to swallow his cock into her throat and her pronounced gag reflex kicked in. The gagging, combined with the alcohol from the bar, triggers instant and projectile vomit all over the client's cock. \"Oh no!\" cries " << girlName << "  as she recovers from her retching, staring in horror at the client, who is silently gazing at the pile of puke covering him. \"I'm so sorry! I'm so so so sorry,\" she continues looking around for something to wipe it all up. After a few minutes of mopping the vomit from his crotch, " << girlName << "  awkwardly pulls her clothes together and prepares to leave, painfully aware that the client will not be getting another erection any time soon. \"I'm so embarrassed,\" she whimpers as she moves for the door.\n";
							fame -= 2;
						}
						else
						{
							ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. She tries to swallow his shaft but does not get too far. The icing on the cake is when she starts blowing on his dick, as if someone fundamentally failed to explain what a �blowjob' was. Still, with enough enthusiasm from her and enough manual stimulation from himself, he is able to eventually bring himself to cum in her inexperienced mouth. He is happy to have gotten a load off, but he can barely contain his surprise at how bad she is at giving head.\n";
							fame -= 1;
						}
					}
					ss << girlName << " licks the remaining cum from her lips with a smile. \"If you happen to have a real girlfriend, I hope she can do that as well as I can,\" she flirts as she grabs her clothing and prepares to go. \"And if you ever need a fake girlfriend again for a night, give me a call.\"";
					imagetype = IMGTYPE_ORAL;
					girl->oralsex(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}

		if (roll_sex >= 50 && !girl->check_virginity())/*sex*/
		{
			if (girl->normalsex() > 99)
				{
					ss << "\"Let me do the work,\" she suggests, as she straddles him on the couch and envelops him with her hot, wet, pulsing pussy. She runs her fingernails gently over his chest as she circles herself on his cock. After a minute of heated teasing she begins to scratch him ever so slightly with her fingernails as she bounces up and down. She expertly matches the tempo to his thrusts, slowing when he gets close to orgasm, and picking it back up as he recovers. Then, when she feels he is ready, she deftly throws her left leg over his head and rotates so that now she is riding him with his back to her. She gyrates and humps like a wild woman, screaming in passion, until he erupts violently inside her. The two collapse in the couch together, sweating and panting, but very, very satisfied.\n";
					fame += 3;
				}
			else if (girl->normalsex() >= 75)
				{
					ss << "\"Fill me,\" she whispers, gently pushing him until he is seated on the couch and then guiding his erection into her wet pussy. He moans with pleasure as she begins to slowly gyrate on his cock, both gasping as she picks up the pace and begins to ride him in an increasingly furious tempo. She is able to prolong the experience by slowing down when he gets close, and then picking it back up, so that eventually the client erupts into her like a raging volcano. The two collapse onto each other in a sweaty pile. \"That was amazing,\" he says.\n";
					fame += 2;
				}
			else if (girl->normalsex() >= 50)
				{
					ss << "\"I'm so wet,\" she whispers, bending over while guiding his cock into her pussy with one solid thrust. Both of them moan with satisfaction as he finds that she is right, and soon she is pumping pleasurably back and forth, rocking her body to match his thrusts. After a few minutes of sexual pleasure he shoots his load deep into her uterus, and collapses onto his couch with a contented sigh.\n";
					fame += 1;
				}
			else if (girl->normalsex() >= 25)
				{
					ss << "\"Take me,\" she whispers, lying on his couch and spreading her legs so he can access her pussy. He grins and thrusts quickly inside her, finding her wet enough to continue but dry enough to cause some discomfort. She moans at all the right times, but other than keeping her legs spread, " << girlName << " does not do much else to help with this, and ends up just lying there. While the client is easily able to spurt cum deep inside her after a few minutes, he is surprised at how mediocre her sexual abilities are compared to the rest of the evening.\n";
				}
			else
				{
					ss << "While he pants with anticipation, she bends over and guides him to the entrance of her pussy. They both grunt with discomfort as he realizes that she is not really wet yet, but they force their way through it, though " << girlName << " pumps at all the wrong times and keeps overextending so that he pops out and she has to fumble until he is back inside her. This unfortunate grinding continues for a few minutes before the client takes over, pushes her to the ground, and then just has her lie there while he does all the work. He is able to cum, but only with a lot of help of his hands and imagination.\n";
					fame -= 1;
				}
				ss << "After she wipes the cum from her thighs, she lies there for a moment beside him, stroking his chest with her fingers. \"I had a great time, love, but I do have to go,\" she whispers. \"But if you ever need a girlfriend for a night to do that again, you know where to find me.\"";
				imagetype = IMGTYPE_SEX;
				girl->normalsex(2);
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
		else if (roll_sex >= 40 || anal)/*Anal*/
				{
					if (girl->has_item_j("Compelling Buttplug") != -1)
					{
						ss << girlName << " turns around, lifting her dress and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, pulling out the plug and presenting her gaping ass to him.\n";
						fame += 1;
					}
					else if (girl->has_item_j("Large Buttplug") != -1)
					{
						ss << girlName << " turns around, lifting her dress and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, tugging with her hand at the base of the buttplug. As she pulls it out, her ass clutches the plug and expands dramatically. The client is amazed at how far it keeps stretching, until the full massive plug plops down onto the floor, leaving her ass wide open and gaping in front of him. He pauses in wonder, having never realized that a girl's ass could carry anything so massive.";
						fame += 2;
					}
					if (girl->anal() > 99)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " sighs with immense pleasure, truly happy to have her ass filled by his cock. She holds him back with her hand, asking that she be able to do the work for him. Soon she is gently massaging his dick with perfectly-timed squeezes of her asshole, purring with rapture. She then begins pumping herself back and forth on him, forcing him deep inside her, picking up the pace to a furious level and then slowing it down to keep him from cumming too early. After twenty heavenly minutes inside her, she begins throwing her hips backwards and forwards, riding his cock like the anal master that she is. He screams with bliss as he erupts violently into her ass, shooting string after string of cum deep inside her. \"Incredible!\" he exclaims, collapsing panting onto his couch.\n";
						fame += 3;
					}
					else if (girl->anal() >= 75)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure and begins to pump herself expertly around his manhood. \"Fuck my ass,\" she purrs, massaging him with well-timed squeezes as she forces him deeper into her with increasing thrusts. She gyrates herself expertly around him, and within a matter of minutes, the client is sweating and panting towards climax. \"Cum in me!\" she cries, as he fills her bowels with a violent eruption of cum.\n";
						fame += 2;
					}
					else if (girl->anal() >= 50)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure at the invasion and begins gyrating her hips around his cock, forcing him deeper into her and enticing him with the vision of her ass bouncing against his thighs. She does not know how to squeeze her ass at the right moments, but with her efforts and the mere fact that his cock in her ass is plenty of encouragement enough, soon he is shooting cum deep inside her bowels. \"Whoa,\" he pants, collapsing on the couch afterwards.\n";
						fame += 1;
					}
					else if (girl->anal() >= 25)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with a combination of pleasure and pain and tries to push herself back and forth to stimulate his cock. Her motions are amateurish and not very pleasurable, but the client can hardly complain, and soon he is shooting his cum deep inside her bowels.\n";
					}
					else
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " whimpers at the sudden intrusion, and then proceeds to lie there motionlessly while he rams himself back and forth. As he builds to climax, though, he realizes that the sounds she is making are not moans of pleasure but grunts of pain, and he sees that she is crying with discomfort. He forces himself to slow his pace, afraid to hurt her further, and then pulls out so only the tip is still inside her as he wraps his own hand around his cock and strokes himself to climax. He is surprised that " << girlName << ", who seemed to so eagerly want a cock in her ass, was so bad at taking one, and is by and large disappointed with the entire experience.\n";
						fame -= 1;
					}
					ss << girlName << " lies beside the client for a moment, stroking his chest with her fingers as the cum leaks from her ass. \"That was amazing,\" she purrs, before she stands to gather her clothes to leave. \"If you have a real girlfriend, I hope she lets you do that to her ass from time to time,\" she flirts. \"But if you need a fake girlfriend again some night, you know where to find me.\"";
					imagetype = IMGTYPE_ANAL;
					girl->anal(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
		else if (roll_sex >= 25)/*titty job*/
				{
					ss << girlName << " is quickly stripping out of her dress to reveal her tits, and in a matter of seconds she has freed his cock from his pants and dropped to her knees to bury it between her cleavage.\n";
					if (girl->tittysex() > 99)
					{
						ss << girlName << " gently rubs her nipples with the head of his cock while whispering dirty talk, then gives his tip a few teasing licks as she guides him into her oiled cleavage. She pushes her breasts together and begins to slowly rock back and forth on him, building tension and increasing the tempo as he moans louder and louder. Her tongue darts out at the perfect moment in each pump, right as the tip approaches her mouth, and by the time the client is screaming with rapture, she is pumping him with her tits at a furious pace. He almost passes out when he shoots string after string of cum into her open mouth. \"Where did you learn how to do that?\" he asks in amazement, collapsing on his couch.\n";
						fame += 3;
					}
					else if (girl->tittysex() >= 75)
					{
						ss << girlName << " is an expert titty-fucker, and she alternates the head of his cock deftly from nipple to nipple, rubbing it seductively, between quick darts of tongue licks. Once he is fully primed, she guides him into her oiled cleavage and pushes her breasts together, pumping him back and forth and licking his tip every time it comes near her mouth. He moans with deep satisfaction as he shoots his load into her open and welcoming mouth.\n";
						fame += 2;
					}
					else if (girl->tittysex() >= 50)
					{
						ss << "Fortunately, " << girlName << " knows what she is doing, and after sensuously rubbing the head of his cock over each nipple, she guides him into her oiled cleavage and shrugs her tits together. She bounces back and forth on his cock, purring dirty talk as he builds towards climax, and lowers her mouth to his tip just in time for a healthy dose of semen.\n";
						fame += 1;
					}
					else if (girl->tittysex() >= 25)
					{
						ss << "While " << girlName << " is enthusiastic, she has some difficulty using her breasts to bring him off. Her lack of skills result in her spending more time smacking his dick with her nipples than building up a good pace. He is not very upset�few men in his position would be�but he does need to use his own hands a little to build up enough tension that he can cum.\n";
					}
					else
					{
						ss << "After a moment, though, the client realizes that " << girlName << " really has no idea how to titty-fuck, and her efforts are lackluster at best. In the end, he has to use his imagination and help with his own hands in order to cum.\n";
						fame -= 1;
					}
					ss << "After he erupts cum all over her face, " << girlName << " smiles, cleans herself off, and turns to collect her things to go. \"I'm afraid that's all our relationship can enjoy for tonight,\" she continues. \"Though if you ever need me to be your girlfriend again for the night, please give me a call.\"";
					imagetype = IMGTYPE_TITTY;
					girl->tittysex(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
		else /*Hand job*/
				{
					if (girl->handjob() > 99)
					{
						ss << "\"Don't worry,\" she purrs as she licks her hand and fingers, \"you're in good hands.\" She delicately teases his erection with her fingers, grasping him between them and rotating her hand back and form before she expertly massages his balls. She kisses him passionately as she grasps him with a full grip, and then begins pulsing back and forth on his cock, increasing her pressure as she builds him skillfully to climax. \"3..\" she begins, moving faster around him. \"2..\" she continues, gripping with perfect pressure and timing her pulls with quick kisses on his mouth. \"1..\" she moans seductively, moving the pressure to the level of no return. \"Now,\" she declares, and instantly he cannot help but cum all over her fingers. \"That was AMAZING,\" he declares, collapsing on his couch.\n";
						fame += 3;
					}
					else if (girl->handjob() >= 75)
					{
						ss << "\"Allow me to get a handle on things,\" she jokes as she licks her palm and fingers. Then she gently wraps them around his erection, pulling ever so slightly and then moving her fingers to massage his balls. She kisses him deeply as she slowly builds up the pace, timing each pull to his increasing pleasure, until he is gasping around her lips. She increases the pressure to a critical level once he hits the point of no return, and finishes the kiss just in time for his sperm to shoot all over her expert fingers. \"Wow,\" he grunts.\n";
						fame += 2;
					}
					else if (girl->handjob() >= 50)
					{
						ss << "\"I think you'll find I'm quite handy to have around,\" she whispers before she licks her right palm and wraps her fingers around his pulsing manhood. She kisses him deeply as her hands maneuver around his cock, pulling at just the right pressure and using her fingers to tease his balls from time to time. As she finishes a last deep kiss, she escalates the pace and sends a violent stream of cum into her palm. \"That was something,\" he says.\n";
						fame += 1;
					}
					else if (girl->handjob() >= 25)
					{
						ss << "\"Perhaps you'd like my hand to do a little more than hold yours?\" she continues, lifting her right hand to spit on the palm before grabbing his cock and pulling back and forth on it. He is surprised by her eagerness, but also by her lack of skill, as she bumbles back and forth, gripping far too tight and pulling far too hard. Still, it is enough to bring him to climax, and while he is amused at how badly she is at handjobs, he was aroused enough to send a wad of semen shooting forth onto her fingers. \"You certainly are enthusiastic,\" he says.\n";
					}
					else
					{
						ss << "\"Maybe I can give you a hand,\" she continues, grabbing his erect cock with enthusiasm and freeing it from his pants. She begins tugging at him, which causes him to whimper momentarily with pain as there is no lubrication and she is using a lot of force. He hopes for a minute that this is just a little rough foreplay before she starts doing something a little more pleasurable, but as the minute turns into two minutes, he realizes that this is all there is going to be. He spits on his own hand and tries to lubricate his own cock while guiding her hands to be slower or at least a little less powerful, and with enough help from his own hands, he is able to at least masturbate himself into her palm before she rips all the skin from his dick. \"That was different,\" he murmurs.\n";
						fame -= 1;
					}
					ss << girlName << " sensuously licks the semen from her fingers while the client recovers himself. \"If you have a real girlfriend, I hope she can do that as well as your pretend one,\" she smiles, packing her things to leave. \"And if you ever need a fake girlfriend again for a night, give me a call.\"";
					imagetype = IMGTYPE_HAND;
					girl->handjob(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
	}
}
break;	// end Es_Regular

case Es_Commoner:
{
	cust_type = 2; cust_type_text = "Commoner";
	//COMMENER
	//telling what she is meeting and where
	ss << girlName << " has been requested by a commoner client, an artisan who has used your services before and is looking for a refined and charming young lady to join him for dinner at a popular restaurant in the Crossgate city center. She is to wear casual clothes and meet him for drinks, dinner, and conversation.\n";


	//how prepared and when does she arrive...
	if (prepare > 99)//very early very prepared
	{
		ss << girlName << " immediately delves into the customer biography on the client and does some independent research on him. She quickly develops a list of things the two of them have in common, and a separate list of conversation topics that he is sure to enjoy. She selects her most appealing casual clothes, applies her best makeup, and arrives at the door of the restaurant at the exact appointed time.\n";
		escort += 3;
	}
	else if (prepare >= 75)//early and prepared
	{
		ss << girlName << " arrives at the door of the bustling downtown restaurant just in time. She actually arrived half an hour ago, and has spent her time scoping out the restaurant and its patrons and contemplating conversation topics. She read the biography that your escort clients are obligated to provide, and knows enough details about the client to be able to navigate the conversation quickly into comfortable ground for him.\n";
		escort += 2;
	}
	else if (prepare >= 50)//on time and prepared
	{
		ss << girlName << " arrives just on time, dressed in attractive casual clothes, and made up to entice him with her charms. She has prepared a few conversation topics to help the date along.\n";
		escort += 1;
	}
	else if (prepare >= 25)//kind prepard sort late
	{
		ss << girlName << " arrives ten minutes late, dressed in suitable casual clothes and mulling over conversation topics. She knows her lateness will probably upset him, but hopes that she can improve the date with witty banter.\n";
	}
	else//very late and uprepared
	{
		ss << girlName << " arrives at the restaurant half an hour late, barely groomed and with minimal efforts to make herself presentable. She does not even think about whether this will upset the client.\n";
		escort -= 1;
	}

	//beauty check
	ss << girlName << " enters the restaurant and locates the artisan, sitting at a table for two in a quiet candlelit corner.\n";
	if (girl->beauty() > 99)
	{
		ss << "He looks up as she approaches, and then he just stares in silence for an awkward silence. \"I'm sorry,\" he stammers, collecting himself. \"You are radiantly beautiful, and I lost my words for a moment.\" " << girlName << " smiles politely in response and makes a kind remark in return as he stands, pulls out her chair, and informs her that he has ordered a bottle of wine for them to share while they consider their food choices.\n";
		escort += 3;
	}
	else if (girl->beauty() >= 75)
	{
		ss << "He looks up as she approaches and smiles radiantly. \"You are absolutely lovely, " << girlName << ". I'm so glad you could join me this evening.\" He stands and pulls out the seat for her, and she can feel his eyes quickly scanning her backside. " << girlName << " smiles politely and sits.\n";
		escort += 2;
	}
	else if (girl->beauty() >= 50)
	{
		ss << "He looks up as she approaches and smiles. " << girlName << " is not a bombshell looker, but she can hold her own, and she smiles back, allowing him to take her all in. \"I'm so glad you could join me,\" he begins, his eyes returning to her face. \"Please have a seat.  I've ordered us a bottle of wine, and I hope you'll like it.\"\n";
		escort += 1;
	}
	else if (girl->beauty() >= 25)
	{
		ss << "He looks up as she approaches and smiles politely. She can detect that he is disappointed with her looks, but he hides it well. \"Please have a seat,\" he says.\n";
	}
	else
	{
		ss << "He looks up as she approaches, and tries but fails to disguise his disappointment at her appearance. " << girlName << " is not an attractive girl, but she does not like to be reminded of this. She swallows her pride and sits down, hoping that her conversational abilities will redeem her.\n";
		escort -= 1;
	}


	//boob event
	if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs")
					|| girl->has_trait( "Titanic Tits") || girl->has_trait( "Big Boobs")
					|| girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs"))
				{
					ss << girlName << " catches the client's eyes going briefly to her substantial breasts,";
					titty_lover = true;
					if (g_Dice.percent(75))
					{
						ss << " before he realizes he has been caught in the act and snaps his head back to make eye contact with her, looking a little sheepish. She did see him smile, though. It seems the client is a bit of a breast man.\n";
						escort += 1;
						if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
							{
								ss << "Being a bit of an exhibitionist, " << girlName << " is pleased to see that her tits are getting some extra attention. She decides to reward his interest by leaning forward over the table to give him a good look, and \"accidentally\" catches her bracelet on her shirt as her hand moves down to grab her drink, flashing a nipple briefly at him. \"Whoops!\" she giggles, feigning embarrassment.\n";
								if (g_Dice.percent(75))
								{
									ss << "He grins back at her, appreciating the view. \"Oh, no,\" he laughs, \"what an unfortunate wardrobe malfunction!\"\n";
									escort += 1;
								}
								else
								{
									ss << "He blushes and looks away awkwardly, appearing slightly offended by the display. \"Is everything suitable again, miss?\" he inquires before turning to look at her again.\n";
									escort -= 1;
								}
							}
					}
					else
					{
						ss << " which she is very used to experiencing. She is not so used to his reaction, though, which seems a bit disappointed. He mutters something about top-heavy ships being poorly made before he catches himself, meets her gaze, and smiles. It seems that " << girlName << "'s great tits are not such an asset tonight.\n";
						escort -= 1;
					}

				}
	else
	{
		if (g_Dice.percent(40))
		{
			ss << girlName << "  notices when the client flashes his eyes quickly over her breasts. She has always been a bit self-conscious about their small size, but she is surprised to see him smile back at her with greater appreciation. \"Aren't you just perfect,\" he smiles.\n";
			escort += 1;
			titty_lover = true;
			if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
				{
					ss << "As a bit of an exhibitionist and pleased at this unusual attention on her small breasts, " << girlName << " decides to reward his interest by \"accidentally\" catching the top of her shirt on her bracelet, giving him a brief view of her pert tits and even a flash of nipple. \"Oh, how silly of me,\" she exclaims, readjusting herself.\n";
					if (g_Dice.percent(75))
						{
							ss << "\"Aren't you just perfect, indeed,\" he repeats, smiling rakishly at her.\n";
							escort += 1;
						}
						else
						{
							ss << "The client whips his head away in embarrassment, and does not look at her again until she is done readjusting. He blushes and asks whether her clothes are quite suitable for polite conversation again.\n";
							escort -= 1;
						}
				}
		}
		else
		{
			ss << girlName << " notices when the client flashes his eyes quickly over her small breasts, and she is mentally ready when she detects the disappointment in his eyes. He is a breast man, like so many, and " << girlName << " is very familiar with the ways that the client tries to disguise his regret. She resolves to win him with her other charms.\n";
			escort -= 1;
		}
	}

	//ass
	if (girl->has_trait( "Great Arse") || girl->has_trait( "Deluxe Derriere") || girl->has_trait( "Phat Booty") && !titty_lover)
	{
		ss << girlName << " feels the client's gaze as she rests her fantastic ass in her chair. He is clearly mesmerized by it for a second, before he forces himself to stop staring and returns to his own chair.";
		escort += 1;
		ass_lover = true;
		if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
				{
					ss << girlName << " does not mind his gaze, however, and her exhibitionist tendencies make her decide to keep his attention on her amazing ass for a few more moments. She sits so that her skirt hitches on the chair and rides up, giving him a brief view of the contours of her ass cheeks. \"Oh my,\" she giggles, quickly�but not too quickly�settling her skirt back to normal.\n";
					if (g_Dice.percent(75))
						{
							ss << "The client fans himself for a moment with the menu and smiles. \"My,\" he begins, \"did it just get hot in here?\"\n";
							escort += 1;
						}
						else
						{
							ss << "\"You seem to be having some difficulty with your skirt,\" he says with surprising harshness, \"and you may want to fix it before we have a polite conversation.\" " << girlName << " blushes and makes sure her skirt is fully concealing everything before turning back to him. It seems he does not appreciate her little exhibitionism.\n";
							escort -= 1;
						}
				}
	}

	if (girl->has_trait( "Sexy Air") || girl->has_trait( "Great Figure") || girl->has_trait( "Hourglass Figure"))
	{
		ss << "\"There's something about the way you walk and carry yourself,\" he begins, \"that is just really, really attractive. It's really very enticing,\" he concludes, smiling, hoping the compliment is well-received.";
		escort += 1;
	}


	if (girl->has_trait( "Bruises"))
	{
		if (g_Dice.percent(33))
		{
			ss << "The client cannot help but notice the bruises all over " << girlName << "'s body, and his eyes flash momentarily with regret and sympathy for her life. He finds himself feeling inexplicably protective of her, and concerned for what may happen when she returns to her normal work.\n";
			escort += 1;
		}
		else if (g_Dice.percent(33))
		{
			ss << "The client cannot help but notice the bruises all over " << girlName << "'s body, and he finds them unseemly. He does not know what earned her these marks, but he clearly considers them to be inappropriate for a lady engaged in the escort profession.\n";
			escort -= 1;
		}
		else
		{
			// nothing probably better way to do this
		}
	}


	//CONVERSATION PHASE (REFINEMENT  CHECK)
	ss << "The two of them enjoy a bottle of wine while chatting amiably in the lovely restaurant. While the client is merely a commoner, the rest of the patrons here are members of the gentry and middle class, and it seems that he has decided to spend a significant portion of his weekly wages to come here with " << girlName << ". An impeccably polite waiter arrives to discuss the wine list and then take their orders for food.\n";
	if (girl->refinement() > 99)
	{
		ss << "This is a restaurant for the gentry and other upper-middle class, and while that may have intimidated some other girls, " << girlName << " is quite comfortable having dinner in a palace. She is incredibly refined, and shortly this is apparent to the waiter, the other patrons, and especially the client, who is not really at home here and is trying to enjoy the way the other half lives for a night. He has invested quite a bit of money into this evening, and he is thrilled to see that " << girlName << " is perfectly at ease in this setting and even able to cleverly help him feel more at home. She subtly educates him on the way to hold the wine glass, order food, and then eat that food as any landed gentry would. This is the perfect companion for his evening, and he watches her every move with deep respect, appreciation, and affection.\n";
		escort += 3;
		if (girl->has_trait( "Elegant"))
		{
			ss << girlName << " is watched by a number of other patrons as well, because her elegance and unconscious grace soon becomes the envy of every woman, and every man in the restaurant wishes he was the client. He beams with barely-contained pride and pleasure as the waiters dote over the two of them as if they are royalty.\n";
			escort += 1;
		}
		else if (girl->has_trait( "Dominatrix"))
		{
			ss << "Being the dominant type, " << girlName << " soon commandeers the conversation. The client could not be happier about this, because he does not really have a working knowledge of upper-middle class entertainment, and her control over the conversation allows him to respond in generalities but still fool any observer into thinking the two of them are experts in opera, investments, and Crossgate politics. He spends a remarkably pleasant evening living as if he were already the gentry that he aspires to be, and he is deeply grateful to " << girlName << " for taking charge.\n";
			escort += 2;
		}
		if (girl->has_trait( "Social Drinker"))
		{
			ss << girlName << "'s deeply refined palate allows her to enjoy the expensive wine the client has ordered, which she suspects amounts to two days of an artisan's wages, and her tendency to drink more in social occasions allows her to enjoy the wine faster than he had anticipated. Conscious of his pocketbook, she suggests a second slightly less expensive bottle, known to wine experts as a hidden gem, and he is praised with newfound respect by the waiter when he orders it. The praise reduces some of the sting of the expense, and he is happy to pay it to please such a charming and refined companion, but he knows this is money that he will not have to tip her later.\n";
			escort -= 1;
		}
	}
	else if (girl->refinement() >= 75)
	{
		ss << "This restaurant is a favorite of the upper-middle class gentry, and fortunately, " << girlName << " is very refined and quite at home with the customs of the lesser nobility, let alone the gentry. She is also able to tell that her client is not as comfortable in this setting, and helps to guide him toward the right menu selections with such refined subterfuge that he believes his selections to be his own, and not the result of her polite suggestions. She quickly impresses the waiter with well-informed questions on the source of each dish and the style of cooking to be employed. The client gratefully follows her lead, very glad that she is able to deftly navigate a world to which he can only aspire.\n";
		escort += 2;
		if (girl->has_trait( "Elegant"))
		{
			ss << "Every move " << girlName << " makes is so elegant that soon the women from nearby tables are looking at her with barely-contained jealousy, and many try to copy her graceful way of sipping the wine and delicately cutting her food. The client beams with pleasure when he realizes that everyone around him not only believes that he is one of them, but suspects he might be one of their betters, slumming it for a day.\n";
			escort += 1;
		}
		else if (girl->has_trait( "Dominatrix"))
		{
			ss << "Being the dominant sort, " << girlName << " soon commandeers the conversation and deftly leads it into subjects that are appropriate for two upper-middle class patrons on the town. She declaims upon the most popular operas in Crossgate with well-informed expertise, but cleverly does so in such a way that the client, who is not really familiar with this world, is able to respond in generalities and any observer would believe the two of them are not merely gentry, but maybe some lesser nobles slumming it for the evening. He is very glad to follow her lead.\n";
			escort += 1;
		}
		if (girl->has_trait( "Social Drinker"))
		{
			ss << girlName << " has a very refined palate for wine, but she also enjoys it quite a bit in social situations, and soon the two of them have polished off the first bottle. Sooner than the client had anticipated, actually, since the bottle cost him two days' wages and he had hoped it would last all night. She suggests a second, slightly less expensive bottle, and he quickly orders it, knowing very well that she is far more at home in this environment than he is and suspecting that this is the normal custom for the gentry. While the waiter's praise at his selection of wine takes some of the sting out of the price, this is still money he had not anticipated spending, and it is money he will not be able to use to tip her for her excellent company later.\n";
			escort -= 1;
		}
	}
	else if (girl->refinement() >= 50)
	{
		ss << "This restaurant is a favorite for members of the gentry, which generally are considered upper-middle class. Fortunately, " << girlName << " is sufficiently refined to be very comfortable in this atmosphere, and she speaks, drinks, and eats with ease. The client is immediately grateful to see that she is in her natural element, because he is not, and while he was ready to fake it for the evening, he is much happier to follow her lead. This is his evening to live like his economic betters, even if it costs him a full week's worth of wages, and he happily notes that the waiter congratulates her for her excellent choice of perfectly pronounced French cuisine, and does his best to emulate her.\n";
		escort += 1;
		if (girl->has_trait( "Elegant"))
		{
			ss << "He then beams with pleasure as " << girlName << " takes hold of her wine glass and sips with unconscious grace. She is an elegant lady, and the client soon realizes that other women at nearby tables are soon looking at her with some jealousy and trying to hold their wine glasses in just the same way. He beams with pleasure to be sitting with such a refined and elegant woman.\n";
			escort += 1;
		}
		else if (girl->has_trait( "Dominatrix"))
		{
			ss << "Being a dominant type, " << girlName << " soon takes over the conversation topics and steers the two of them towards the appropriate subject matter for two upper-middle class patrons out for a night on the town. She discusses the latest operas with ease, which could have been a problem as the client does not actually know much about them, but she cleverly discusses them in such a way that she is informing him without lecturing him, and she deftly manages the conversation such that any observer would think the two of them to be quite well acquainted with the subject. He is happy to follow her lead.\n";
			escort += 1;
		}
		if (girl->has_trait( "Social Drinker"))
		{
			ss << girlName << " compliments the client on his excellent wine selection, which pleases him greatly, as it easily cost him two days' wages. She does tend to drink quite a bit in social situations, though, and soon the two of them have polished off the bottle that he thought would last them all night. She suggests a second, slightly less expensive, bottle to him, and he happily follows her recommendation, as her palate and refinement make her the expert out of the two of them. The waiter praises his choice of a second bottle, which takes the sting out of its price, but nevertheless, this is money that he had not anticipated spending, and it will naturally cut into the amount he is able to tip her for her pleasurable company.\n";
			escort -= 1;
		}
	}
	else if (girl->refinement() >= 25)
	{
		ss << girlName << " has the refinement of a girl raised in the lower-middle class, and she is somewhat out of sorts in this restaurant, which is definitely upper-middle class. While the client is also out of place here, he does a much better job of hiding it. He smells the wine cork when the bottle is offered as if he has done that many times before, and as soon as the waiter leaves, " << girlName << " leans towards him and asks in a whisper what that was all about. \"Honestly,\" he replies, \"I have no idea. It's just what I see them do.\" Understanding about decorum, " << girlName << " knows enough about her own limitations to be uncomfortable here, and quietly orders the most inexpensive thing from the menu that she can. The client is slightly charmed by her discomfort, as he obviously sympathizes, but it seems that he was hoping that he would be the only one faking it, and that " << girlName << " should have been the kind of lady who would be right at home here.\n";
		if (girl->has_trait( "Elegant"))
		{
			ss << "However, " << girlName << " soon allows her own elegant style to overcome her unease. She might not be familiar with the customs and traditions of the gentry, but her unconscious grace and ease when lifting the wine glass to her lips soon draws the envious glances of patrons at nearby tables. The client smiles softly to himself, happy to be sitting beside such an elegant, even if unrefined, lady.\n";
			escort += 1;
		}
		else if (girl->has_trait( "Dominatrix"))
		{
			ss << "Being the dominant type, " << girlName << " soon finds herself leading the conversation, whether the client wants that or not. As both are out of place in this restaurant and trying to look as if they belong, the conversation becomes awkward quickly as " << girlName << " forcefully navigates away from subjects that cause her unease and towards subjects that she believes the gentry usually discuss. This would be more successful if she was actually conversant about opera.\n";
		}
		if (girl->has_trait( "Social Drinker"))
		{
			ss << "As " << girlName << " tends to knock back alcohol in social situations with surprising vigor, she at least looks like she is quite comfortable drinking expensive wine. The first bottle is drained long before the client had anticipated, especially since it cost approximately two days' wages for him. He is glad when " << girlName << " suggests a less expensive bottle for their next drink, but he does silently mourn for his pocketbook.\n";
			escort -= 1;
		}
	}
	else
	{
		ss << girlName << " has no refinement and is immediately identifiable as a fish out of water in this restaurant. She drinks the wine in large swallows, totally unaware of the client's cringes as she knocks back a bottle worth two days of work for him without any apparent recognition of its value. When the waiter arrives, she orders a moderately expensive French dish loudly, thoroughly mispronouncing it. The waiter politely confirms her order and moves away to prepare it, leaving the client alone with " << girlName << " and his own embarrassment.\n";
		escort -= 1;
		if (girl->has_trait( "Elegant"))
		{
			ss << "While " << girlName << " is clearly out of her element in a middle-class environment, she at least possesses her own kind of lower-class elegance. When she chugs the wine, she does so with some grace, and her mispronunciation of the dishes is more charming than cringe-worthy. This does quite a bit to make the client, who is also out of his own element though doing an excellent job of hiding it, much more comfortable with " << girlName << ".\n";
			escort += 1;
		}
		else if (girl->has_trait( "Dominatrix"))
		{
			ss << "Being a very dominant personality, " << girlName << " soon forced herself into leading the conversation between the two of them. This would not necessarily be such a bad thing with an escort, considering that she is probably more at ease with the situation than the client, but when paired with her lack of refinement in this classier establishment, the results are unfortunate. While the other tables are discussing investment portfolios and the opera, " << girlName << " is deep in a soliloquy on Crossgate professional sports gossip. Loudly. The client furtively glances from side to side, hoping the other patrons are not judging them as being out of place.\n";
			escort -= 1;
		}
		if (girl->has_trait( "Social Drinker"))
		{
			ss << girlName << " is a social drinker who enjoys a good bottle of wine. As her palate is as unrefined as the rest of her, however, she also really enjoys a terrible bottle of wine. Once the two of them have polished off the expensive first bottle, which happens sooner than the client expected or would have liked, " << girlName << " suggests a second bottle called \"Frankia\" that is generally considered to be grape-flavored paint thinner. While the client is glad that the second bottle is much less expensive, he is concerned that " << girlName << " may not be the right girl to join him for this kind of date.\n";
			escort -= 1;
		}
	}


	//RESULTS PHASE (POINT CHECK)
	if (escort >= 9)
	{
		ss << "\"Every woman in that place wishes they were you,\" laughs the client as he escorts her outside after a delicious dessert and a final cocktail. \"I have to admit, " << girlName << ", I am captivated by you. You are so graceful and beautiful that you would probably never associate with a simple man like me, but you made me feel like royalty tonight, and I cannot thank you enough for that. Please accept this envelope with your fee and a substantial token of my esteem and affection.\" He pauses, gathering his courage before continuing. \"And if you do not have other plans for the night, I would be forever grateful if you would join me at my home for coffee. I know that I'm probably not the kind of man you usually join, but maybe my respect and gratitude for you might let you look passed my.. well, my lack of refinement, I suppose.\" He stands before her bravely awaiting her answer, but knowing it is probably a long shot at best.";
		fame += 2;
		sex_offer = true;
		cust_wealth = 2;
	}
	else if (escort >= 6)
	{
		ss << "\"I have had a great evening with you,\" begins the client as the two of them finish their dessert. \"I cannot thank you enough for making this a wonderful evening for me. I know that you probably have other arrangements for the rest of the night, but on the chance that you do not, would you care to come back to my place for coffee? No pressure, of course! It's just that I've had such a great time that I would hate for it to end so early. And of course, whatever you decide, please accept this envelope with your fee and an extra token of my gratitude.\"";
		fame += 1;
		sex_offer = true;
		cust_wealth = 1.5;
	}
	else if (escort >= 1)
	{
		ss << "As the two of them finish their main course, the client pays the bill and rises to escort " << girlName << " from her chair and walks with her outside the restaurant. \"Thank you for a nice evening,\" he says simply, handing her an envelope with her fee. \"I don't think the two of us are a good fit, but I still had a decent time tonight. Thank you, at least, for that,\" he concludes, turning to walk away. " << girlName << " begins her trek back to your establishment, wondering what she could have done better.";
	}
	else
	{
		ss << "As the two of them finish their main course, the client trying to hide his embarrassment with his choice of companions, he decides to cut the evening short without dessert. \"I came here thinking that I might not belong with the upper crust,\" he begins, whispering to her across the table sternly, \"but you definitely do not belong. I don't think I've ever spent so much money to be humiliated in public. Please tell your employer that I will not be paying for this evening, and that he should be more careful in his choice of companions for future assignments. I bid you good night,\" he concludes, his voice quivering with anger, as he pays the bill and walks out into the night.";
		fame -= 1;
		cust_wealth = 0;
	}

	//SEX PHASE (LIBIDO CHECK)
	if (sex_offer)
	{
		if (girl->morality() >= 50)
		{
			ss << girlName << " blinks in surprise at the offer. \"I'm afraid I just couldn't,\" she begins, doing her best to politely decline the offer. \"Thank you, but I'm a conservative kind of girl, and since we just met, I feel it would be wrong to do anything more.\" She blushes and makes ready to leave. \"Of course, I understand,\" stammers the client. \"A woman of your class and refinement would naturally be surprised by so forward an advance. A million apologies,\" he finishes, but " << girlName << " brushes the apology away with tenderness, sparing his ego, as she turns to begin her trek back to your establishment.";
		}
		else if (girl->libido() <= 50)
		{
			ss << girlName << " is flattered, but not interested in anything with this client. She politely declines his offer, mentioning that her work does not permit it and her employer has already assigned her other work for the night, and lightens the mood with a final few jokes before departing the restaurant to return to your establishment. He takes it well, as her gentle delivery spares his ego, and he thanks her again for a wonderful evening.";
		}
		else
		{
			ss << girlName << " smiles back, with a flash of lust in her eyes. \"I would love to get to know you better, too,\" she responds, reaching over to brush his fingers with hers. \"A coffee at your home would be lovely.\" The two of them walk hand-in-hand out of the restaurant and a few blocks to his place, laughing at each other's jokes the whole way. The journey from Crossgate's city center takes a fairly long time, and " << girlName << " observes with a new understanding as they pass by the expensive homes into poorer and poorer neighborhoods before they finally come to a humble artisan's cottage on the outskirts. The client opens the door like a gentleman, hiding his embarrassment at his own relative poverty, and the two enter.\n";
			sex_event = true;
		}
	}



	//sex event
	//IF POSITIVE RESPONSE TO BREASTS IN MEETING PHASE
	if (sex_event)
	{
		if (titty_lover)
		{
			ss << "Once the two enter his place and shut the door, " << girlName << " starts seductively stripping away her shirt to reveal her beautiful breasts. \"I got the feeling you like these,\" she purrs, leaning close and bringing her hand to grab her left tit. \"Maybe you'd like to get to know them a bit better, too,\" she continues, using her other hand to free his cock from his pants. " << girlName << " disappears briefly into his kitchen and returns a moment later carrying a jar. \"I found some olive oil� I hope you don't mind if I borrow some,\" she smiles. \"Not at all,\" he pants in response, eagerly waiting. Soon she has lathered up her breasts in oil and is rubbing them up and down his cock while she moans seductively.\n";
			if (girl->tittysex() > 99)
			{
				ss << girlName << " gently rubs her nipples with the head of his cock while whispering dirty talk, then gives his tip a few teasing licks as she guides him into her oiled cleavage. She pushes her breasts together and begins to slowly rock back and forth on him, building tension and increasing the tempo as he moans louder and louder. Her tongue darts out at the perfect moment in each pump, right as the tip approaches her mouth, and by the time the client is screaming with rapture, she is pumping him with her tits at a furious pace. He almost passes out when he shoots string after string of cum into her open mouth. \"Where did you learn how to do that?\" he asks in amazement, collapsing on his couch.\n";
				fame += 3;
			}
			else if (girl->tittysex() >= 75)
			{
				ss << girlName << " is an expert titty-fucker, and she alternates the head of his cock deftly from nipple to nipple, rubbing it seductively, between quick darts of tongue licks. Once he is fully primed, she guides him into her oiled cleavage and pushes her breasts together, pumping him back and forth and licking his tip every time it comes near her mouth. He moans with deep satisfaction as he shoots his load into her open and welcoming mouth.\n";
				fame += 2;
			}
			else if (girl->tittysex() >= 50)
			{
				ss << "Fortunately, " << girlName << " knows what she is doing, and after sensuously rubbing the head of his cock over each nipple, she guides him into her oiled cleavage and shrugs her tits together. She bounces back and forth on his cock, purring dirty talk as he builds towards climax, and lowers her mouth to his tip just in time for a healthy dose of semen.\n";
				fame += 1;
			}
			else if (girl->tittysex() >= 25)
			{
				ss << "While " << girlName << " is enthusiastic, she has some difficulty using her breasts to bring him off. Her lack of skills result in her spending more time smacking his dick with her nipples than building up a good pace. He is not very upset�few men in his position would be�but he does need to use his own hands a little to build up enough tension that he can cum.\n";
			}
			else
			{
				ss << "After a moment, though, the client realizes that " << girlName << " really has no idea how to titty-fuck, and her efforts are lackluster at best. In the end, he has to use his imagination and help with his own hands in order to cum.\n";
				fame -= 1;
			}
			ss << "After he erupts cum all over her face, " << girlName << " smiles, cleans herself off, and turns to collect her things to go. \"Thank you for that,\" she says as she approaches the door. \"You should know that, at the very least in this respect, you are better than any of the gentry I have known.\"";
			imagetype = IMGTYPE_TITTY;
			girl->tittysex(2);
			girl->upd_temp_stat(STAT_LIBIDO, -20, true);
		}
		else if (ass_lover)
		{
			ss << "Once the two of them enter his place and close the door, " << girlName << " turns her back to him and grinds her fantastic ass against his thigh. \"I got the feeling you liked my ass,\" she purrs, writhing back and forth on his growing erection. \"Maybe you would like to get to know it a bit better?\" She turns back to face him and nibbles on his neck as she frees his cock from his pants, while she uses her hips to shimmy out of her casual skirt.\n";
			if (girl->check_virginity())
			{
				ss << "\"I should let you know,\" she whispers while stroking his cock, \"that I am still a virgin, and my employer considers it important that I stay one.\" The client's jaw drops with amazement. \"Don't worry, though,\" she says, spitting on her hand and rubbing it on his cock before guiding him towards her asshole, \"there are other ways of enjoying my ass tonight.\"";
				anal = true;
			}
			else
			{
				ss << "";
				sex = true;
			}
		}
		else
		{
			ss << "Once the two of them are behind closed doors, " << girlName << " kisses him passionately and runs her hand to his crotch to feel his growing erection. \"Now how can I reward my perfect gentleman tonight?\" she asks, stripping off her casual clothes.";
		}

		//sex stuff needs linked up somehow
		if (!titty_lover && !ass_lover)
			{
				if (roll_sex >= 50 && !girl->check_virginity() || sex)/*sex*/
				{
					if (girl->normalsex() > 99)
					{
						ss << "\"Let me do the work,\" she suggests, as she straddles him on the couch and envelops him with her hot, wet, pulsing pussy. She runs her fingernails gently over his chest as she circles herself on his cock. After a minute of heated teasing she begins to scratch him ever so slightly with her fingernails as she bounces up and down. She expertly matches the tempo to his thrusts, slowing when he gets close to orgasm, and picking it back up as he recovers. Then, when she feels he is ready, she deftly throws her left leg over his head and rotates so that now she is riding him with his back to her. She gyrates and humps like a wild woman, screaming in passion, until he erupts violently inside her. The two collapse in the couch together, sweating and panting, but very, very satisfied.\n";
						fame += 3;
					}
					else if (girl->normalsex() >= 75)
					{
						ss << "\"Fill me,\" she whispers, gently pushing him until he is seated on the couch and then guiding his erection into her wet pussy. He moans with pleasure as she begins to slowly gyrate on his cock, both gasping as she picks up the pace and begins to ride him in an increasingly furious tempo. She is able to prolong the experience by slowing down when he gets close, and then picking it back up, so that eventually the client erupts into her like a raging volcano. The two collapse onto each other in a sweaty pile. \"That was amazing,\" he says.\n";
						fame += 2;
					}
					else if (girl->normalsex() >= 50)
					{
						ss << "\"I'm so wet,\" she whispers, bending over while guiding his cock into her pussy with one solid thrust. Both of them moan with satisfaction as he finds that she is right, and soon she is pumping pleasurably back and forth, rocking her body to match his thrusts. After a few minutes of sexual pleasure he shoots his load deep into her uterus, and collapses onto his couch with a contented sigh.\n";
						fame += 1;
					}
					else if (girl->normalsex() >= 25)
					{
						ss << "\"Maybe you would like some of this,\" she whispers, lying on his couch and spreading her legs so he can access her pussy. He grins and thrusts quickly inside her, finding her wet enough to continue but dry enough to cause some discomfort. She moans at all the right times, but other than keeping her legs spread, " << girlName << " does not do much else to help with this, and ends up just lying there. While the client is easily able to spurt cum deep inside her after a few minutes, he is surprised at how mediocre her sexual abilities are compared to the rest of the evening.\n";
					}
					else
					{
						ss << "While he pants with anticipation, she bends over and guides him to the entrance of her pussy. They both grunt with discomfort as he realizes that she is not really wet yet, but they force their way through it, though " << girlName << " pumps at all the wrong times and keeps overextending so that he pops out and she has to fumble until he is back inside her. This unfortunate grinding continues for a few minutes before the client takes over, pushes her to the ground, and then just has her lie there while he does all the work. He is able to cum, but only with a lot of help of his hands and imagination.\n";
						fame -= 1;
					}
					ss << "After she wipes the cum from her thighs, she lies there for a moment beside him, stroking his chest with her fingers. \"I had a great time, love, but I do have to go,\" she whispers. \"I hope you know that, at least as far as sex is concerned, and in many other ways, you are better than practically every gentry I have ever met.\"";
					imagetype = IMGTYPE_SEX;
					girl->normalsex(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
				else if (roll_sex >= 40 || anal)/*Anal*/
				{
					if (girl->has_item_j("Compelling Buttplug") != -1)
					{
						ss << girlName << " turns around, lifting her casual skirt and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, pulling out the plug and presenting her gaping ass to him.";
						fame += 1;
					}
					else if (girl->has_item_j("Large Buttplug") != -1)
					{
						ss << girlName << " turns around, lifting her casual skirt and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, tugging with her hand at the base of the buttplug. As she pulls it out, her ass clutches the plug and expands dramatically. The client is amazed at how far it keeps stretching, until the full massive plug plops down onto the floor, leaving her ass wide open and gaping in front of him. He pauses in wonder, having never realized that a girl's ass could carry anything so massive.";
						fame += 2;
					}
					if (girl->anal() > 99)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " sighs with immense pleasure, truly happy to have her ass filled by his cock. She holds him back with her hand, asking that she be able to do the work for him. Soon she is gently massaging his dick with perfectly-timed squeezes of her asshole, purring with rapture. She then begins pumping herself back and forth on him, forcing him deep inside her, picking up the pace to a furious level and then slowing it down to keep him from cumming too early. After twenty heavenly minutes inside her, she begins throwing her hips backwards and forwards, riding his cock like the anal master that she is. He screams with bliss as he erupts violently into her ass, shooting string after string of cum deep inside her. \"Incredible!\" he exclaims, collapsing panting onto his couch.\n";
						fame += 3;
					}
					else if (girl->anal() >= 75)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure and begins to pump herself expertly around his manhood. \"Fuck my ass,\" she purrs, massaging him with well-timed squeezes as she forces him deeper into her with increasing thrusts. She gyrates herself expertly around him, and within a matter of minutes, the client is sweating and panting towards climax. \"Cum in me!\" she cries, as he fills her bowels with a violent eruption of cum.\n";
						fame += 2;
					}
					else if (girl->anal() >= 50)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure at the invasion and begins gyrating her hips around his cock, forcing him deeper into her and enticing him with the vision of her ass bouncing against his thighs. She does not know how to squeeze her ass at the right moments, but with her efforts and the mere fact that his cock in her ass is plenty of encouragement enough, soon he is shooting cum deep inside her bowels. \"Whoa,\" he pants, collapsing on the couch afterwards.\n";
						fame += 1;
					}
					else if (girl->anal() >= 25)
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with a combination of pleasure and pain and tries to push herself back and forth to stimulate his cock. Her motions are amateurish and not very pleasurable, but the client can hardly complain, and soon he is shooting his cum deep inside her bowels.\n";
					}
					else
					{
						ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " whimpers at the sudden intrusion, and then proceeds to lie there motionlessly while he rams himself back and forth. As he builds to climax, though, he realizes that the sounds she is making are not moans of pleasure but grunts of pain, and he sees that she is crying with discomfort. He forces himself to slow his pace, afraid to hurt her further, and then pulls out so only the tip is still inside her as he wraps his own hand around his cock and strokes himself to climax. He is surprised that " << girlName << ", who seemed to so eagerly want a cock in her ass, was so bad at taking one, and is by and large disappointed with the entire experience.\n";
						fame -= 1;
					}
					ss << girlName << " lies beside the client for a moment, stroking his chest with her fingers as the cum leaks from her ass. \"That was amazing,\" she purrs, before she stands to gather her clothes to leave. \"I'm glad we got to know each other a little better, and I hope you know that when it comes to tearing my ass open, you are better than any gentry I have ever met.\"";
					imagetype = IMGTYPE_ANAL;
					girl->anal(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
				else if (roll_sex >= 25)/*Hand job*/
				{
					if (girl->handjob() > 99)
					{
						ss << "\"Don't worry,\" she purrs as she licks her hand and fingers, \"you're in good hands.\" She delicately teases his erection with her fingers, grasping him between them and rotating her hand back and form before she expertly massages his balls. She kisses him passionately as she grasps him with a full grip, and then begins pulsing back and forth on his cock, increasing her pressure as she builds him skillfully to climax. \"3..\" she begins, moving faster around him. \"2..\" she continues, gripping with perfect pressure and timing her pulls with quick kisses on his mouth. \"1..\" she moans seductively, moving the pressure to the level of no return. \"Now,\" she declares, and instantly he cannot help but cum all over her fingers. \"That was AMAZING,\" he declares, collapsing on his couch.\n";
						fame += 3;
					}
					else if (girl->handjob() >= 75)
					{
						ss << "\"Allow me to get a handle on things,\" she jokes as she licks her palm and fingers. Then she gently wraps them around his erection, pulling ever so slightly and then moving her fingers to massage his balls. She kisses him deeply as she slowly builds up the pace, timing each pull to his increasing pleasure, until he is gasping around her lips. She increases the pressure to a critical level once he hits the point of no return, and finishes the kiss just in time for his sperm to shoot all over her expert fingers. \"Wow,\" he grunts.\n";
						fame += 2;
					}
					else if (girl->handjob() >= 50)
					{
						ss << "\"I think you'll find I'm quite handy to have around,\" she whispers before she licks her right palm and wraps her fingers around his pulsing manhood. She kisses him deeply as her hands maneuver around his cock, pulling at just the right pressure and using her fingers to tease his balls from time to time. As she finishes a last deep kiss, she escalates the pace and sends a violent stream of cum into her palm. \"That was something,\" he says.\n";
						fame += 1;
					}
					else if (girl->handjob() >= 25)
					{
						ss << "\"Perhaps you'd like my hand to do a little more than hold yours?\" she continues, lifting her right hand to spit on the palm before grabbing his cock and pulling back and forth on it. He is surprised by her eagerness, but also by her lack of skill, as she bumbles back and forth, gripping far too tight and pulling far too hard. Still, it is enough to bring him to climax, and while he is amused at how badly she is at handjobs, he was aroused enough to send a wad of semen shooting forth onto her fingers. \"You certainly are enthusiastic,\" he says.\n";
					}
					else
					{
						ss << "\"Maybe I can give you a hand,\" she continues, grabbing his erect cock with enthusiasm and freeing it from his pants. She begins tugging at him, which causes him to whimper momentarily with pain as there is no lubrication and she is using a lot of force. He hopes for a minute that this is just a little rough foreplay before she starts doing something a little more pleasurable, but as the minute turns into two minutes, he realizes that this is all there is going to be. He spits on his own hand and tries to lubricate his own cock while guiding her hands to be slower or at least a little less powerful, and with enough help from his own hands, he is able to at least masturbate himself into her palm before she rips all the skin from his dick. \"That was different,\" he murmurs.\n";
						fame -= 1;
					}
					ss << girlName << " sensuously licks the semen from her fingers while the client recovers himself. \"I think I got that drink you were talking about,\" she says as she finishes the last string of cum. \"And I'm happy we got to know each other a bit better. I hope a gentleman such as yourself is kind enough to call on me again,\" she suggests as she gathers her clothes to leave.";
					imagetype = IMGTYPE_HAND;
					girl->handjob(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
				else/*oral*/
				{
					if (girl->oralsex() > 99)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She cannot take him deeply out of concern for her gag reflex, but she knows that her incredible mouth will be more than enough to pleasure him anyway, and licks and sucks various parts of his cock and balls as he builds towards orgasm, moaning with pleasure the whole time. She forces herself to take him deeply right before he cums, making herself gag on him but using her involuntary reflex as another way of massaging his shaft. He shoots a violent explosion of cum into her mouth and then staggers back, collapsing on his couch. \"That was amazing,\" he pants as " << girlName << " licks the cum from her lips.\n";
							fame += 2;
						}
						else
						{
							ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She takes him deeply in one swallow and holds him there, locked in her as she swallows repeatedly to massage him with her throat. She stays there, intentionally depriving herself of oxygen for his pleasure, until she feels him building towards orgasm. She then releases, teasing him. \"Not yet,\" she whispers before turning her phenomenal tongue to his balls. She returns to his shaft as he recovers himself, and then slowly and masterfully builds the tension until she knows the time is approaching. She then plunges herself deep onto his cock again, licking his balls as her throat plays around his tip, until he shoots a violent explosion of cum straight down to her stomach. \"That was absolutely incredible,\" moans the client as he collapses onto his couch.\n";
							fame += 3;
						}
					}
					else if (girl->oralsex() >= 75)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She gently begins licking his balls while wrapping her hand delicately over his shaft. After a few minutes of ball pleasuring, she begins licking his shaft with increased pressure, sucking on his tip passionately. She does not go deeper, aware that her gag reflex would cause problems, but is easily able to satisfy him with the licks and shallow sucking alone. Soon he builds to a powerful orgasm that floods her mouth after a few more minutes of her expert technique.\n";
							fame += 1;
						}
						else
						{
							ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She takes him deeply in one swallow and holds herself all the way to the base of his cock, darting her tongue out to lick his balls as she does so, and then begins pumping her head back and forth. The client moans with pleasure as she expertly works around his manhood, and he builds to a powerful orgasm that floods her mouth after a few minutes of her masterful technique.\n";
							fame += 2;
						}
					}
					else if (girl->oralsex() >= 50)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"Allow me to show my appreciation for a lovely evening,\" says " << girlName << " as she falls to her knees and pulls his cock out from his pants. She lovingly licks every part of his crotch, teasing his balls with her tongue and then engulfing his tip with her mouth as she rotates her tongue around the head. She avoids going any deeper out of concern for her gag reflex, which ultimately results in a mediocre blowjob, but the client is soon shooting semen until it leaks out of the sides of her mouth.\n";
						}
						else
						{
							ss << "\"Allow me to thank you,\" says " << girlName << " as she lowers herself to her knees and removes his cock from his pants. She takes his erection deep into her mouth and massages it with her throat, and then begins to pump her head back and forth, stopping to occasionally lick his balls. It is not the best blowjob he has ever received, but it is nevertheless very pleasurable, and " << girlName << " is soon rewarded for her efforts with an eruption of cum into her mouth.\n";
							fame += 1;
						}
					}
					else if (girl->oralsex() >= 25)
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build towards orgasm. " << girlName << "'s gag reflex suddenly kicks in, though, which is probably why she was staying so shallow on the tip in the first place, and she is soon coughing all over his cock and pulling herself away to recover. This kills the mood for a period, and it is not until she regains composure and the client masturbates himself into her mouth that he is able to cum. The whole experience leaves him slightly disappointed.\n";
							fame -= 1;
						}
						else
						{
							ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build up enough tension to ejaculate. It is all in all a mediocre experience, but he is not one to complain about a free blowjob from a lovely lady, and at the very least, the climax is enjoyable.\n";
						}
					}
					else
					{
						if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
						{
							ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. Disaster strikes, however, when she tries to swallow his cock into her throat and her pronounced gag reflex kicked in. The gagging, combined with the alcohol from the bar, triggers instant and projectile vomit all over the client's cock. \"Oh no!\" cries " << girlName << "  as she recovers from her retching, staring in horror at the client, who is silently gazing at the pile of puke covering him. \"I'm so sorry! I'm so so so sorry,\" she continues looking around for something to wipe it all up. After a few minutes of mopping the vomit from his crotch, " << girlName << "  awkwardly pulls her clothes together and prepares to leave, painfully aware that the client will not be getting another erection any time soon. \"I'm so embarrassed,\" she whimpers as she moves for the door.\n";
							fame -= 2;
						}
						else
						{
							ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. She tries to swallow his shaft but does not get too far. The icing on the cake is when she starts blowing on his dick, as if someone fundamentally failed to explain what a �blowjob' was. Still, with enough enthusiasm from her and enough manual stimulation from himself, he is able to eventually bring himself to cum in her inexperienced mouth. He is happy to have gotten a load off, but he can barely contain his surprise at how bad she is at giving head.\n";
							fame -= 1;
						}
					}
					ss << girlName << " licks the remaining cum from her lips with a smile. \"Thanks for the drink,\" she says, as she begins gathering her clothes to leave. \"I hope a gentleman like yourself is kind enough to call for me again sometime,\" she suggests as she heads for the door to return to your establishment.";
					imagetype = IMGTYPE_ORAL;
					girl->oralsex(2);
					girl->upd_temp_stat(STAT_LIBIDO, -20, true);
				}
		}
	}
}
break;	// end Es_Commoner


case Es_DeadBeat:
{
	cust_type = 1; cust_type_text = "Dead Beat";
	//telling what she is meeting and where
	ss << girlName << " has been requested by a client who has not used your services before, and has no reputation with your establishment. Because she is not famous enough to attract more reputable customers, she has no other clientele. She is to wear casual clothing and meet him at a nearby bar for a drink.\n";

	//how prepared and when does she arrive...
	if (prepare > 99)//very early very prepared
	{
		ss << girlName << " immediately delves into the customer biography on the client and does some independent research on him. She quickly develops a list of things the two of them have in common, and a separate list of conversation topics that he is sure to enjoy. She selects her most appealing casual clothes, applies her best makeup, and arrives at the door of the bar at the exact appointed time.\n";
		escort += 3;
	}
	else if (prepare >= 75)//early and prepared
	{
		ss << girlName << " arrives at the door of the bar just in time. She actually arrived half an hour ago, and has spent her time scoping out the bar and contemplating conversation topics. She read the very sparse biography that your escort clients are obligated to provide, and knows enough details about him to be able to navigate the conversation quickly into comfortable ground for him.\n";
		escort += 2;
	}
	else if (prepare >= 50)//on time and prepared
	{
		ss << girlName << " arrives just on time, dressed in attractive casual clothes, and made up to entice him with her charms. She has prepared a few conversation topics to help the date along.\n";
		escort += 1;
	}
	else if (prepare >= 25)//kind prepard sort late
	{
		ss << girlName << " arrives ten minutes late, dressed in suitable casual clothes and mulling over conversation topics. She knows her lateness will probably upset him, but hopes that she can improve the date with witty banter.\n";
	}
	else//very late and uprepared
	{
		ss << girlName << " arrives at the bar half an hour late, barely groomed and with minimal efforts to make herself presentable. She does not even think about whether this will upset the client.\n";
		escort -= 1;
	}

	//beauty check
	ss << girlName << " enters the bar and quickly locates the client.\n";
	if (girl->beauty() > 99)
	{
		ss << "He looks up as she approaches, and then he just stares in silence for an awkward silence. \"I'm sorry,\" he stammers, collecting himself. \"You are just the most beautiful woman I've ever seen. I don't mean that as some kind of lame line.. I mean it.\" " << girlName << " smiles politely in response and makes a kind remark in return as he stands, pulls out her chair, and asks her if she would like to join him for a drink.\n";
		escort += 3;
	}
	else if (girl->beauty() >= 75)
	{
		ss << "He looks up as she approaches and smiles radiantly. \"You are absolutely lovely, " << girlName << ". I'm so glad you could join me this evening.\" He stands and pulls out the seat for her, and she can feel his eyes quickly scanning her backside. " << girlName << " smiles politely and sits.\n";
		escort += 2;
	}
	else if (girl->beauty() >= 50)
	{
		ss << "He looks up as she approaches and smiles. " << girlName << " is not a bombshell looker, but she can hold her own, and she smiles back, allowing him to take her all in. \"I'm so glad you could join me,\" he begins, his eyes returning to her face. \"Please have a seat and I'll get you a drink.\"\n";
		escort += 1;
	}
	else if (girl->beauty() >= 25)
	{
		ss << "He looks up as she approaches and smiles politely. She can detect that he is disappointed with her looks, but he hides it well. \"Please have a seat,\" he says.\n";
	}
	else
	{
		ss << "He looks up as she approaches, and tries but fails to disguise his disappointment at her appearance. " << girlName << " is not an attractive girl, but she does not like to be reminded of this. She swallows her pride and sits down, hoping that her conversational abilities will redeem her.\n";
		escort -= 1;
	}

	//boob event
	if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs")
					|| girl->has_trait( "Titanic Tits") || girl->has_trait( "Big Boobs")
					|| girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs"))
				{
					ss << girlName << " catches the client's eyes going briefly to her substantial breasts,";
					titty_lover = true;
					if (g_Dice.percent(75))
					{
						ss << " before he realizes he has been caught in the act and snaps his head back to make eye contact with her, looking a little sheepish. She did see him smile, though. It seems the client is a bit of a breast man.\n";
						escort += 1;
						if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
							{
								ss << "Being a bit of an exhibitionist, " << girlName << " is pleased to see that her tits are getting some extra attention. She decides to reward his interest by leaning forward over the table to give him a good look, and \"accidentally\" catches her bracelet on her shirt as her hand moves down to grab her drink, flashing a nipple briefly at him. \"Whoops!\" she giggles, feigning embarrassment.\n";
								if (g_Dice.percent(75))
								{
									ss << "He grins back at her, appreciating the view. \"Oh, no,\" he laughs, \"what an unfortunate wardrobe malfunction!\"\n";
									escort += 1;
								}
								else
								{
									ss << "He blushes and looks away awkwardly, appearing slightly offended by the display. \"Is everything suitable again, miss?\" he inquires before turning to look at her again.\n";
									escort -= 1;
								}
							}
					}
					else
					{
						ss << " which she is very used to experiencing. She is not so used to his reaction, though, which seems a bit disappointed. He mutters something about top-heavy ships being poorly made before he catches himself, meets her gaze, and smiles. It seems that " << girlName << "'s great tits are not such an asset tonight.\n";
						escort -= 1;
					}

				}
	else
	{
		if (g_Dice.percent(40))
		{
			ss << girlName << "  notices when the client flashes his eyes quickly over her breasts. She has always been a bit self-conscious about their small size, but she is surprised to see him smile back at her with greater appreciation. \"Aren't you just perfect,\" he smiles.\n";
			escort += 1;
			titty_lover = true;
			if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
				{
					ss << "As a bit of an exhibitionist and pleased at this unusual attention on her small breasts, " << girlName << " decides to reward his interest by \"accidentally\" catching the top of her shirt on her bracelet, giving him a brief view of her pert tits and even a flash of nipple. \"Oh, how silly of me,\" she exclaims, readjusting herself.\n";
					if (g_Dice.percent(75))
						{
							ss << "\"Aren't you just perfect, indeed,\" he repeats, smiling rakishly at her.\n";
							escort += 1;
						}
						else
						{
							ss << "The client whips his head away in embarrassment, and does not look at her again until she is done readjusting. He blushes and asks whether her clothes are quite suitable for polite conversation again.\n";
							escort -= 1;
						}
				}
		}
		else
		{
			ss << girlName << " notices when the client flashes his eyes quickly over her small breasts, and she is mentally ready when she detects the disappointment in his eyes. He is a breast man, like so many, and " << girlName << " is very familiar with the ways that the client tries to disguise his regret. She resolves to win him with her other charms.\n";
			escort -= 1;
		}
	}


	//ass
	if (girl->has_trait( "Great Arse") || girl->has_trait( "Deluxe Derriere") || girl->has_trait( "Phat Booty") && !titty_lover)
	{
		ss << girlName << " feels the client's gaze as she rests her fantastic ass in her chair. He is clearly mesmerized by it for a second, before he forces himself to stop staring and returns to his own chair.";
		escort += 1;
		ass_lover = true;
		if (girl->has_trait( "Exhibitionist") && g_Dice.percent(50))
				{
					ss << girlName << " does not mind his gaze, however, and her exhibitionist tendencies make her decide to keep his attention on her amazing ass for a few more moments. She sits so that her skirt hitches on the chair and rides up, giving him a brief view of the contours of her ass cheeks. \"Oh my,\" she giggles, quickly�but not too quickly�settling her skirt back to normal.\n";
					if (g_Dice.percent(75))
						{
							ss << "The client fans himself for a moment with the menu and smiles. \"My,\" he begins, \"did it just get hot in here?\"\n";
							escort += 1;
						}
						else
						{
							ss << "\"You seem to be having some difficulty with your skirt,\" he says with surprising harshness, \"and you may want to fix it before we have a polite conversation.\" " << girlName << " blushes and makes sure her skirt is fully concealing everything before turning back to him. It seems he does not appreciate her little exhibitionism.\n";
							escort -= 1;
						}
				}
	}


	if (girl->has_trait( "Sexy Air") || girl->has_trait( "Great Figure") || girl->has_trait( "Hourglass Figure"))
	{
		ss << "\"There's something about the way you walk and carry yourself,\" he begins, \"that is just really, really attractive. It's really very enticing,\" he concludes, smiling, hoping the compliment is well-received.\n";
		escort += 1;
	}


	if (girl->has_trait( "Bruises"))
	{
		if (g_Dice.percent(33))
		{
			ss << "The client cannot help but notice the bruises all over " << girlName << "'s body, and his eyes flash momentarily with regret and sympathy for her life. He finds himself feeling inexplicably protective of her, and concerned for what may happen when she returns to her normal work.\n";
			escort += 1;
		}
		else if (g_Dice.percent(33))
		{
			ss << "The client cannot help but notice the bruises all over " << girlName << "'s body, and he finds them unseemly. He does not know what earned her these marks, but he clearly considers them to be inappropriate for a lady engaged in the escort profession.\n";
			escort -= 1;
		}
		else
		{
			// nothing probably better way to do this
		}
	}


	//CONVERSATION PHASE (CHARISMA CHECK)
	ss << "The two of them order a drink and engage in small talk. They keep the conversation light and easy, avoiding complicated subjects.\n";
	if (girl->charisma() > 99)
	{
		ss << girlName << " and the client spend the next hour joking and bantering like old friends. She is wonderfully charismatic, and while she expertly keeps the conversation on comfortable subjects for him, she knows that he would be happy talking with her about anything. By the end of the hour he is wiping tears of laughter from his eyes and smiling at her with deep appreciation.\n";
		escort += 3;
		if (girl->has_trait( "Alcoholic") && g_Dice.percent(50))
		{
			ss << girlName << "He also appreciates the sheer volume of alcohol that " << girlName << " has consumed in the last hour. He accurately suspects that she may be an alcoholic, but she seems to be able to handle it well, and while this concerns him, it has hardly ruined his evening.\n";
			escort -= 1;
			girl->upd_temp_stat(STAT_LIBIDO, +20, true);
		}
	}
	else if (girl->charisma() >= 75)
	{
		ss << girlName << " and the client are bantering like best friends within minutes. She charismatically leads him into conversations where they share interests and deftly steers him away from controversial or uncomfortable subjects. An hour has passed before he even realizes it, and their laughter reverberates around the surrounding tables.\n";
		escort += 2;
		if (girl->has_trait( "Alcoholic") && g_Dice.percent(50))
		{
			ss << girlName << " is laughing a bit louder than he is, though. This is probably because her desperate need for alcohol has resulted in her downing five beverages over the course of the hour. While their conversation was so engaging that the client did not seem to mind how drunk she was getting, he did notice.\n";
			escort -= 1;
			girl->upd_temp_stat(STAT_LIBIDO, +20);
		}
	}
	else if (girl->charisma() >= 50)
	{
		ss << girlName << " has the client laughing at jokes and talking about shared hobbies in no time. The two of them enjoy a pleasant conversation for the next forty-five minutes, and they both share another drink. After about an hour, the conversation starts slowing down as " << girlName << " runs out of ideas to keep it going, but by the end of the evening, she can tell the client enjoyed their time together.\n";
		escort += 1;
		if (girl->has_trait( "Alcoholic") && g_Dice.percent(50))
		{
			ss << "All except for " << girlName << "'s drinking, that is. As an alcoholic, she is unable to control herself once the drinking starts, and after they share another drink, she gets a third, and then a fourth, and by the fifth she is sloppy. The silent judgment in his eyes verifies that " << girlName << "'s alcoholism has reversed any rapport she was able to gain via conversation.\n";
			escort -= 1;
			girl->upd_temp_stat(STAT_LIBIDO, +20, true);
		}
	}
	else if (girl->charisma() >= 25)
	{
		ss << girlName << " is able to lead the two of them in a few minutes of good conversation, but it begins to sputter out once she is out of ideas. The client's efforts at leading the conversation would have been successful if " << girlName << " was more charismatic and able to think and speak on her feet. The conversation is not the worst thing in the world, but it ends up being a part of the date that both of them would be happier forgetting.\n";
		if (girl->has_trait( "Alcoholic") && g_Dice.percent(50))
		{
			ss << "What he cannot forget, however, is how many drinks " << girlName << " knocks back during the hour. Her need is not social; it is clearly an addiction. She is much drunker than he is by the end of the hour, and while he is willing to forget the conversation, he does feel compelled to judge her conduct.\n";
			escort -= 1;
			girl->upd_temp_stat(STAT_LIBIDO, +20, true);
		}
	}
	else
	{
		ss << girlName << " has the charisma of a sponge, and the conversation grinds to a halt within minutes. Her efforts at small talk are painfully awkward, and whenever she tries to start a new conversation topic, she sounds like an alien who has just discovered the human race. After she asks him to tell her more about \"this thing called football,\" he starts ordering more drinks. He knows at this point he is not drinking to be social; he is drinking to survive.\n";
		escort -= 1;
		if (girl->has_trait( "Alcoholic") && g_Dice.percent(50))
		{
			ss << girlName << " is more than familiar with the idea of drinking like her life depended on it, and her craving for alcohol soon has her keeping pace with the client. Soon the two are an absolute mess together.\n";
			escort += 1;
			girl->upd_temp_stat(STAT_LIBIDO, +20, true);
		}
	}

	//RESULTS PHASE (POINT CHECK)
	if (escort >= 9)
	{
		ss << "\"You are the most amazing woman,\" begins the client, as he realizes that the scheduled time is ending, \"and I've had a truly great time with you tonight. I don't know if I've ever felt this level of connection with somebody before. I'm new to this whole escort thing, and I don't really know how this works, but would you want to grab another drink at my place? I would hate myself forever if I didn't even try to get to know you better. If you can't, I get it, and while I'd be crushed, I understand,\" he finishes, sliding " << girlName << "'s payment, with a hefty bonus, politely across the table.\n";
		fame += 2;
		sex_offer = true;
		cust_wealth = 2;
	}
	else if (escort >= 6)
	{
		ss << "The chiming of the bar clock, which marks the end of the appointed hours, surprises the client. \"I've been having a very good time with you this evening,\" he begins, \"and I'm really sorry to see it end. Would you.. would you be interested in grabbing another drink at my place? I'd love to learn more about you. If you have to go, I understand, and your payment is here, plus some extra to show my appreciation. But I'd love to keep this going for a bit longer.\"\n";
		fame += 1;
		sex_offer = true;
		cust_wealth = 1.5;
	}
	else if (escort >= 1)
	{
		ss << "As the end of the scheduled time approaches, the client spends more and more time glancing with evident relief at his watch. \"Listen,\" he begins, \"this was a new experience for me, and maybe I was doing something wrong, but I just didn't enjoy this very much. If you don't mind, I'd like to just end the evening now. This is your fee,\" he states, sliding an envelope to her as he moves towards the door. \"Thanks, I guess,\" he finishes, and then leaves.";
	}
	else
	{
		ss << "By the end of the hour, the client is groaning and eager to leave. \"This was a bad idea,\" he declares while paying the tab. \"I thought it might get better, but it just didn't. I'm sorry, but please tell your employer that I will not be paying for this evening.\" He frowns and walks out the door, leaving " << girlName << " to ponder the many, many things she did wrong tonight.";
		fame -= 1;
		cust_wealth = 0;
	}

	//SEX PHASE (LIBIDO CHECK)
	if (sex_offer)
	{
		if (girl->morality() >= 50)
		{
			ss << girlName << " blinks in surprise at the offer. \"I'm afraid I just couldn't,\" she begins, doing her best to politely decline the offer. \"Thank you, but I'm a conservative kind of girl, and since we just met, I feel it would be wrong to do anything more.\" She blushes and makes ready to leave. \"Maybe a second date, then?\" he asks, hopefully. \"Maybe,\" she responds as she walks away.";
		}
		else if (girl->libido() <= 50)
		{
			ss << girlName << " is flattered, but not interested in anything with this client. She politely declines his offer, mentioning that her work does not permit it, and lightens the mood with a final few jokes before departing the bar to return to your establishment.";
		}
		else
		{
			ss << girlName << " smiles back, with a flash of lust in her eyes. \"I would love to get to know you better, too,\" she responds, reaching over to brush his fingers with hers. \"A drink at your place it is.\" The two of them walk hand-in-hand out of the bar and a few blocks to his place, laughing at each other's jokes the whole way.\n";
			sex_event = true;
		}
	}

	//sex event
	//IF POSITIVE RESPONSE TO BREASTS IN MEETING PHASE
	if (sex_event)
	{
		if (titty_lover)
		{
			ss << "Once the two enter his place and shut the door, " << girlName << " starts seductively stripping away her shirt to reveal her beautiful breasts. \"I got the feeling you like these,\" she purrs, leaning close and bringing her hand to grab her left tit. \"Maybe you'd like to get to know them a bit better, too,\" she continues, using her other hand to free his cock from his pants. " << girlName << " disappears briefly into his kitchen and returns a moment later carrying a jar. \"I found some olive oil� I hope you don't mind if I borrow some,\" she smiles. \"Not at all,\" he pants in response, eagerly waiting. Soon she has lathered up her breasts in oil and is rubbing them up and down his cock while she moans seductively.\n";
			if (girl->tittysex() > 99)
			{
				ss << girlName << " gently rubs her nipples with the head of his cock while whispering dirty talk, then gives his tip a few teasing licks as she guides him into her oiled cleavage. She pushes her breasts together and begins to slowly rock back and forth on him, building tension and increasing the tempo as he moans louder and louder. Her tongue darts out at the perfect moment in each pump, right as the tip approaches her mouth, and by the time the client is screaming with rapture, she is pumping him with her tits at a furious pace. He almost passes out when he shoots string after string of cum into her open mouth. \"Where did you learn how to do that?\" he asks in amazement, collapsing on his couch.\n";
				fame += 3;
			}
			else if (girl->tittysex() >= 75)
			{
				ss << girlName << " is an expert titty-fucker, and she alternates the head of his cock deftly from nipple to nipple, rubbing it seductively, between quick darts of tongue licks. Once he is fully primed, she guides him into her oiled cleavage and pushes her breasts together, pumping him back and forth and licking his tip every time it comes near her mouth. He moans with deep satisfaction as he shoots his load into her open and welcoming mouth.\n";
				fame += 2;
			}
			else if (girl->tittysex() >= 50)
			{
				ss << "Fortunately, " << girlName << " knows what she is doing, and after sensuously rubbing the head of his cock over each nipple, she guides him into her oiled cleavage and shrugs her tits together. She bounces back and forth on his cock, purring dirty talk as he builds towards climax, and lowers her mouth to his tip just in time for a healthy dose of semen.\n";
				fame += 1;
			}
			else if (girl->tittysex() >= 25)
			{
				ss << "While " << girlName << " is enthusiastic, she has some difficulty using her breasts to bring him off. Her lack of skills result in her spending more time smacking his dick with her nipples than building up a good pace. He is not very upset�few men in his position would be�but he does need to use his own hands a little to build up enough tension that he can cum.\n";
			}
			else
			{
				ss << "After a moment, though, the client realizes that " << girlName << " really has no idea how to titty-fuck, and her efforts are lackluster at best. In the end, he has to use his imagination and help with his own hands in order to cum.\n";
				fame -= 1;
			}
			ss << "After he erupts cum all over her face, " << girlName << " smiles, cleans herself off, and turns to collect her things to go. \"No need for another drink,\" she grins, \"because I think I just got one. I really do hope you'll call for me again, cutie.\"";
			imagetype = IMGTYPE_TITTY;
			girl->upd_temp_stat(STAT_LIBIDO, -20, true);
		}
		else if (ass_lover)
		{
			ss << "Once the two of them enter his place and close the door, " << girlName << " turns her back to him and grinds her fantastic ass against his thigh. \"I got the feeling you liked my ass,\" she purrs, writhing back and forth on his growing erection. \"Maybe you would like to get to know it a bit better?\" She turns back to face him and nibbles on his neck as she frees his cock from his pants, while she uses her hips to shimmy out of her casual skirt.\n";
			if (girl->check_virginity())
			{
				ss << "\"I should let you know,\" she whispers while stroking his cock, \"that I am still a virgin, and my employer considers it important that I stay one.\" The client's jaw drops with amazement. \"Don't worry, though,\" she says, spitting on her hand and rubbing it on his cock before guiding him towards her asshole, \"there are other ways of enjoying my ass tonight.\"";
				anal = true;
			}
			else
			{
				ss << "";
				sex = true;
			}
		}
		else
		{
			ss << "Once the two of them are behind closed doors, " << girlName << " kisses him passionately and runs her hand to his crotch to feel his growing erection. \"Now how can I reward my perfect gentleman tonight?\" she asks, stripping off her casual clothes.";
		}
		//sex stuff needs linked up somehow
		if (!titty_lover && !ass_lover)
		{
			if (roll_sex >= 50 && !girl->check_virginity() || sex)/*sex*/
			{
				if (girl->normalsex() > 99)
				{
					ss << "\"Let me do the work,\" she suggests, as she straddles him on the couch and envelops him with her hot, wet, pulsing pussy. She runs her fingernails gently over his chest as she circles herself on his cock. After a minute of heated teasing she begins to scratch him ever so slightly with her fingernails as she bounces up and down. She expertly matches the tempo to his thrusts, slowing when he gets close to orgasm, and picking it back up as he recovers. Then, when she feels he is ready, she deftly throws her left leg over his head and rotates so that now she is riding him with his back to her. She gyrates and humps like a wild woman, screaming in passion, until he erupts violently inside her. The two collapse in the couch together, sweating and panting, but very, very satisfied.\n";
					fame += 3;
				}
				else if (girl->normalsex() >= 75)
				{
					ss << "\"Fill me,\" she whispers, gently pushing him until he is seated on the couch and then guiding his erection into her wet pussy. He moans with pleasure as she begins to slowly gyrate on his cock, both gasping as she picks up the pace and begins to ride him in an increasingly furious tempo. She is able to prolong the experience by slowing down when he gets close, and then picking it back up, so that eventually the client erupts into her like a raging volcano. The two collapse onto each other in a sweaty pile. \"That was amazing,\" he says.\n";
					fame += 2;
				}
				else if (girl->normalsex() >= 50)
				{
					ss << "\"I'm so wet,\" she whispers, bending over while guiding his cock into her pussy with one solid thrust. Both of them moan with satisfaction as he finds that she is right, and soon she is pumping pleasurably back and forth, rocking her body to match his thrusts. After a few minutes of sexual pleasure he shoots his load deep into her uterus, and collapses onto his couch with a contented sigh.\n";
					fame += 1;
				}
				else if (girl->normalsex() >= 25)
				{
					ss << "\"Maybe you would like some of this,\" she whispers, lying on his couch and spreading her legs so he can access her pussy. He grins and thrusts quickly inside her, finding her wet enough to continue but dry enough to cause some discomfort. She moans at all the right times, but other than keeping her legs spread, " << girlName << " does not do much else to help with this, and ends up just lying there. While the client is easily able to spurt cum deep inside her after a few minutes, he is surprised at how mediocre her sexual abilities are compared to the rest of the evening.\n";
				}
				else
				{
					ss << "While he pants with anticipation, she bends over and guides him to the entrance of her pussy. They both grunt with discomfort as he realizes that she is not really wet yet, but they force their way through it, though " << girlName << " pumps at all the wrong times and keeps overextending so that he pops out and she has to fumble until he is back inside her. This unfortunate grinding continues for a few minutes before the client takes over, pushes her to the ground, and then just has her lie there while he does all the work. He is able to cum, but only with a lot of help of his hands and imagination.\n";
					fame -= 1;
				}
				ss << "After she wipes the cum from her thighs, she lies there for a moment beside him, stroking his chest with her fingers. \"I had a great time, cutie, but I do have to go,\" she whispers. \"I hope you call for me again, though.\"";
				imagetype = IMGTYPE_SEX;
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
			else if (roll_sex >= 40 || anal)/*Anal*/
			{
				if (girl->has_item_j("Compelling Buttplug") != -1)
				{
					ss << girlName << " turns around, lifting her casual skirt and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, pulling out the plug and presenting her gaping ass to him.";
					fame += 1;
				}
				else if (girl->has_item_j("Large Buttplug") != -1)
				{
					ss << girlName << " turns around, lifting her casual skirt and giving him a good view of her ass. For the first time, the client is able to see a faint outline of plastic against her panties. \"I bet you didn't know I had this in me the whole time,\" " << girlName << " giggles as she deftly moves the crotch of her panties to the side, revealing the base of a buttplug sticking out of her ass. \"I'm nice and ready for you to take its place,\" she purrs, tugging with her hand at the base of the buttplug. As she pulls it out, her ass clutches the plug and expands dramatically. The client is amazed at how far it keeps stretching, until the full massive plug plops down onto the floor, leaving her ass wide open and gaping in front of him. He pauses in wonder, having never realized that a girl's ass could carry anything so massive.";
					fame += 2;
				}
				if (girl->anal() > 99)
				{
					ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " sighs with immense pleasure, truly happy to have her ass filled by his cock. She holds him back with her hand, asking that she be able to do the work for him. Soon she is gently massaging his dick with perfectly-timed squeezes of her asshole, purring with rapture. She then begins pumping herself back and forth on him, forcing him deep inside her, picking up the pace to a furious level and then slowing it down to keep him from cumming too early. After twenty heavenly minutes inside her, she begins throwing her hips backwards and forwards, riding his cock like the anal master that she is. He screams with bliss as he erupts violently into her ass, shooting string after string of cum deep inside her. \"Incredible!\" he exclaims, collapsing panting onto his couch.\n";
					fame += 3;
				}
				else if (girl->anal() >= 75)
				{
					ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure and begins to pump herself expertly around his manhood. \"Fuck my ass,\" she purrs, massaging him with well-timed squeezes as she forces him deeper into her with increasing thrusts. She gyrates herself expertly around him, and within a matter of minutes, the client is sweating and panting towards climax. \"Cum in me!\" she cries, as he fills her bowels with a violent eruption of cum.\n";
					fame += 2;
				}
				else if (girl->anal() >= 50)
				{
					ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with pleasure at the invasion and begins gyrating her hips around his cock, forcing him deeper into her and enticing him with the vision of her ass bouncing against his thighs. She does not know how to squeeze her ass at the right moments, but with her efforts and the mere fact that his cock in her ass is plenty of encouragement enough, soon he is shooting cum deep inside her bowels. \"Whoa,\" he pants, collapsing on the couch afterwards.\n";
					fame += 1;
				}
				else if (girl->anal() >= 25)
				{
					ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " moans with a combination of pleasure and pain and tries to push herself back and forth to stimulate his cock. Her motions are amateurish and not very pleasurable, but the client can hardly complain, and soon he is shooting his cum deep inside her bowels.\n";
				}
				else
				{
					ss << "Once " << girlName << " has turned around, lifted her skirt, and displayed her ass to the client, he grins devilishly, spits on his hand, lubricates his cock, and then plunges into her. " << girlName << " whimpers at the sudden intrusion, and then proceeds to lie there motionlessly while he rams himself back and forth. As he builds to climax, though, he realizes that the sounds she is making are not moans of pleasure but grunts of pain, and he sees that she is crying with discomfort. He forces himself to slow his pace, afraid to hurt her further, and then pulls out so only the tip is still inside her as he wraps his own hand around his cock and strokes himself to climax. He is surprised that " << girlName << ", who seemed to so eagerly want a cock in her ass, was so bad at taking one, and is by and large disappointed with the entire experience.\n";
					fame -= 1;
				}
				ss << girlName << " lies beside the client for a moment, stroking his chest with her fingers as the cum leaks from her ass. \"That was amazing,\" she purrs, before she stands to gather her clothes to leave. \"I'm glad we got to know each other a little better, cutie, and I really do hope you call for me again sometime soon.\"";
				imagetype = IMGTYPE_ANAL;
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
			else if (roll_sex >= 25)/*Hand job*/
			{
				if (girl->handjob() > 99)
				{
					ss << "\"Don't worry,\" she purrs as she licks her hand and fingers, \"you're in good hands.\" She delicately teases his erection with her fingers, grasping him between them and rotating her hand back and form before she expertly massages his balls. She kisses him passionately as she grasps him with a full grip, and then begins pulsing back and forth on his cock, increasing her pressure as she builds him skillfully to climax. \"3..\" she begins, moving faster around him. \"2..\" she continues, gripping with perfect pressure and timing her pulls with quick kisses on his mouth. \"1..\" she moans seductively, moving the pressure to the level of no return. \"Now,\" she declares, and instantly he cannot help but cum all over her fingers. \"That was AMAZING,\" he declares, collapsing on his couch.\n";
					fame += 3;
				}
				else if (girl->handjob() >= 75)
				{
					ss << "\"Allow me to get a handle on things,\" she jokes as she licks her palm and fingers. Then she gently wraps them around his erection, pulling ever so slightly and then moving her fingers to massage his balls. She kisses him deeply as she slowly builds up the pace, timing each pull to his increasing pleasure, until he is gasping around her lips. She increases the pressure to a critical level once he hits the point of no return, and finishes the kiss just in time for his sperm to shoot all over her expert fingers. \"Wow,\" he grunts.\n";
					fame += 2;
				}
				else if (girl->handjob() >= 50)
				{
					ss << "\"I think you'll find I'm quite handy to have around,\" she whispers before she licks her right palm and wraps her fingers around his pulsing manhood. She kisses him deeply as her hands maneuver around his cock, pulling at just the right pressure and using her fingers to tease his balls from time to time. As she finishes a last deep kiss, she escalates the pace and sends a violent stream of cum into her palm. \"That was something,\" he says.\n";
					fame += 1;
				}
				else if (girl->handjob() >= 25)
				{
					ss << "\"Perhaps you'd like my hand to do a little more than hold yours?\" she continues, lifting her right hand to spit on the palm before grabbing his cock and pulling back and forth on it. He is surprised by her eagerness, but also by her lack of skill, as she bumbles back and forth, gripping far too tight and pulling far too hard. Still, it is enough to bring him to climax, and while he is amused at how badly she is at handjobs, he was aroused enough to send a wad of semen shooting forth onto her fingers. \"You certainly are enthusiastic,\" he says.\n";
				}
				else
				{
					ss << "\"Maybe I can give you a hand,\" she continues, grabbing his erect cock with enthusiasm and freeing it from his pants. She begins tugging at him, which causes him to whimper momentarily with pain as there is no lubrication and she is using a lot of force. He hopes for a minute that this is just a little rough foreplay before she starts doing something a little more pleasurable, but as the minute turns into two minutes, he realizes that this is all there is going to be. He spits on his own hand and tries to lubricate his own cock while guiding her hands to be slower or at least a little less powerful, and with enough help from his own hands, he is able to at least masturbate himself into her palm before she rips all the skin from his dick. \"That was different,\" he murmurs.\n";
					fame -= 1;
				}
				ss << girlName << " sensuously licks the semen from her fingers while the client recovers himself. \"I think I got that drink you were talking about,\" she says as she finishes the last string of cum. \"And I'm happy we got to know each other a bit better. Call me again sometime, cutie,\" she suggests as she gathers her clothes to leave.\"";
				imagetype = IMGTYPE_HAND;
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
			else/*oral*/
			{
				if (girl->oralsex() > 99)
				{
					if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
					{
						ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She cannot take him deeply out of concern for her gag reflex, but she knows that her incredible mouth will be more than enough to pleasure him anyway, and licks and sucks various parts of his cock and balls as he builds towards orgasm, moaning with pleasure the whole time. She forces herself to take him deeply right before he cums, making herself gag on him but using her involuntary reflex as another way of massaging his shaft. He shoots a violent explosion of cum into her mouth and then staggers back, collapsing on his couch. \"That was amazing,\" he pants as " << girlName << " licks the cum from her lips.\n";
						fame += 2;
					}
					else
					{
						ss << "\"I know you'll enjoy this,\" says " << girlName << " as she drops to her knees, pulls out his cock from his pants, and puts her masterful oral skills to work. She takes him deeply in one swallow and holds him there, locked in her as she swallows repeatedly to massage him with her throat. She stays there, intentionally depriving herself of oxygen for his pleasure, until she feels him building towards orgasm. She then releases, teasing him. \"Not yet,\" she whispers before turning her phenomenal tongue to his balls. She returns to his shaft as he recovers himself, and then slowly and masterfully builds the tension until she knows the time is approaching. She then plunges herself deep onto his cock again, licking his balls as her throat plays around his tip, until he shoots a violent explosion of cum straight down to her stomach. \"That was absolutely incredible,\" moans the client as he collapses onto his couch.\n";
						fame += 3;
					}
				}
				else if (girl->oralsex() >= 75)
				{
					if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
					{
						ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She gently begins licking his balls while wrapping her hand delicately over his shaft. After a few minutes of ball pleasuring, she begins licking his shaft with increased pressure, sucking on his tip passionately. She does not go deeper, aware that her gag reflex would cause problems, but is easily able to satisfy him with the licks and shallow sucking alone. Soon he builds to a powerful orgasm that floods her mouth after a few more minutes of her expert technique.\n";
						fame += 1;
					}
					else
					{
						ss << "\"I think you'll like this,\" says " << girlName << " as she lowers herself slowly and seductively to her knees while freeing his erection from his trousers. She takes him deeply in one swallow and holds herself all the way to the base of his cock, darting her tongue out to lick his balls as she does so, and then begins pumping her head back and forth. The client moans with pleasure as she expertly works around his manhood, and he builds to a powerful orgasm that floods her mouth after a few minutes of her masterful technique.\n";
						fame += 2;
					}
				}
				else if (girl->oralsex() >= 50)
				{
					if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
					{
						ss << "\"Allow me to show my appreciation for a lovely evening,\" says " << girlName << " as she falls to her knees and pulls his cock out from his pants. She lovingly licks every part of his crotch, teasing his balls with her tongue and then engulfing his tip with her mouth as she rotates her tongue around the head. She avoids going any deeper out of concern for her gag reflex, which ultimately results in a mediocre blowjob, but the client is soon shooting semen until it leaks out of the sides of her mouth.\n";
					}
					else
					{
						ss << "\"Allow me to thank you,\" says " << girlName << " as she lowers herself to her knees and removes his cock from his pants. She takes his erection deep into her mouth and massages it with her throat, and then begins to pump her head back and forth, stopping to occasionally lick his balls. It is not the best blowjob he has ever received, but it is nevertheless very pleasurable, and " << girlName << " is soon rewarded for her efforts with an eruption of cum into her mouth.\n";
						fame += 1;
					}
				}
				else if (girl->oralsex() >= 25)
				{
					if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
					{
						ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build towards orgasm. " << girlName << "'s gag reflex suddenly kicks in, though, which is probably why she was staying so shallow on the tip in the first place, and she is soon coughing all over his cock and pulling herself away to recover. This kills the mood for a period, and it is not until she regains composure and the client masturbates himself into her mouth that he is able to cum. The whole experience leaves him slightly disappointed.\n";
						fame -= 1;
					}
					else
					{
						ss << "\"Maybe this will help,\" she says while lowering herself to her knees and pulling his cock free from his pants. She licks the tip seductively and then engulfs the head with her mouth, sucking gently. The client groans with pleasure, and " << girlName << " keeps sucking the tip of his cock. And keeps sucking. For minutes. She does not vary her technique, or move to lick or suck anything else, or take the cock deeper into her mouth. She simply sucks as if it is a nipple and she is waiting for milk to come out, which the client supposes is not too far from the truth. Still, it is not a great blowjob, and it is not until he takes over and starts thrusting himself into her that he is able to build up enough tension to ejaculate. It is all in all a mediocre experience, but he is not one to complain about a free blowjob from a lovely lady, and at the very least, the climax is enjoyable.\n";
					}
				}
				else
				{
					if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
					{
						ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. Disaster strikes, however, when she tries to swallow his cock into her throat and her pronounced gag reflex kicked in. The gagging, combined with the alcohol from the bar, triggers instant and projectile vomit all over the client's cock. \"Oh no!\" cries " << girlName << "  as she recovers from her retching, staring in horror at the client, who is silently gazing at the pile of puke covering him. \"I'm so sorry! I'm so so so sorry,\" she continues looking around for something to wipe it all up. After a few minutes of mopping the vomit from his crotch, " << girlName << "  awkwardly pulls her clothes together and prepares to leave, painfully aware that the client will not be getting another erection any time soon. \"I'm so embarrassed,\" she whimpers as she moves for the door.\n";
						fame -= 2;
					}
					else
					{
						ss << "Wordlessly, " << girlName << " lowers herself to her knees and begins unbuttoning the client's pants. She grins and begins licking his erection. He sighs with satisfaction and prepares to enjoy the blowjob until he realizes that she has absolutely no idea what she is doing. She licks the cock like it is a lollipop, with the skill of a virgin and the enthusiasm of a dog going after peanut butter. She tries to swallow his shaft but does not get too far. The icing on the cake is when she starts blowing on his dick, as if someone fundamentally failed to explain what a �blowjob' was. Still, with enough enthusiasm from her and enough manual stimulation from himself, he is able to eventually bring himself to cum in her inexperienced mouth. He is happy to have gotten a load off, but he can barely contain his surprise at how bad she is at giving head.\n";
						fame -= 1;
					}
				}
				ss << girlName << " licks the remaining cum from her lips with a smile. \"Thanks for the drink,\" she says, as she begins gathering her clothes to leave. \"I had a great time tonight as well, cutie. You should call on me again,\" she suggests as she heads for the door to return to your establishment.";
				imagetype = IMGTYPE_ORAL;
				girl->upd_temp_stat(STAT_LIBIDO, -20, true);
			}
		}
	}
}
break;	// end Es_DeadBeat
};

#if 0
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
			if (g_Dice.percent(girl->intelligence()))
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
#endif



	// work out the pay between the house and the girl
	wages = (int)(girl->askprice() * cust_type * cust_wealth);
	//tips = (jobperformance > 0) ? (g_Dice%jobperformance) * cust_type * cust_wealth : 0;
	ss << "\n \n" << girlName << " receives " << wages << " in payment for her work as an Escort for a " << cust_type_text << " client. Her fame as an Escort has changed by " << fame << ".";

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 20, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->intelligence(g_Dice%skill + 1);
	girl->confidence(g_Dice%skill + 1);
	girl->fame(g_Dice%skill);
	girl->performance(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

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
