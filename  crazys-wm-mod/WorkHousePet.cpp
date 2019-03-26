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
extern cTariff tariff;
extern cJobManager m_JobManager;
extern cPlayer* The_Player;

extern cMovieStudioManager  g_Studios;
extern cArenaManager		g_Arena;
extern cClinicManager		g_Clinic;
extern cCentreManager		g_Centre;
extern cHouseManager		g_House;
extern cFarmManager			g_Farm;

#pragma endregion

// `J` Job House - General
bool cJobManager::WorkHousePet(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	if (Day0Night1) return false;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKHOUSEPET;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	int train = roll_a - girl->obedience() - girl->get_training(TRAINING_PUPPY);

	int wages = 100, tips = 0;
	int enjoy = 0, fame = 0, training = 0, ob = 0, fear = 0, love = 0;

	// `J` add in player's disposition so if the girl has heard of you
	int dispmod = 0;
	/* */if (The_Player->disposition() >= 100)	dispmod = 3;	// "Saint"
	else if (The_Player->disposition() >= 80)	dispmod = 2;	// "Benevolent"
	else if (The_Player->disposition() >= 50)	dispmod = 1;	// "Nice"
	else if (The_Player->disposition() >= 10)	dispmod = 0;	// "Pleasant"
	else if (The_Player->disposition() >= -10)	dispmod = 0;	// "Neutral"
	else if (The_Player->disposition() >= -50)	dispmod = -1;	// "Not nice"
	else if (The_Player->disposition() >= -80)	dispmod = -2;	// "Mean"
	else /*								  */	dispmod = -3;	// "Evil"

	int imagetype = IMGTYPE_PUPPYGIRL;
	int msgtype = Day0Night1;

	sGirl* headonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_HEADGIRL, Day0Night1);
	string headname = (headonduty ? "Head Girl " + headonduty->m_Realname + "" : "the Head girl");

	sGirl* tortureronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_TORTURER, Day0Night1);
	string torturername = (tortureronduty ? "Torturer " + tortureronduty->m_Realname + "" : "the Torturer");

	sGirl* recruiteronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_RECRUITER, Day0Night1);
	string recruitername = (recruiteronduty ? "Recruiter " + recruiteronduty->m_Realname + "" : "the Recruiter");

	sGirl* bedwarmeronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_PERSONALBEDWARMER, Day0Night1);
	string bedname = (bedwarmeronduty ? "Bed warmer " + bedwarmeronduty->m_Realname + "" : "the Bed warmer");

	sGirl* cleaneronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_CLEANHOUSE, Day0Night1);
	string cleanername = (cleaneronduty ? "House cleaner " + cleaneronduty->m_Realname + "" : "the house cleaner");

	sGirl* traningonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_PERSONALTRAINING, Day0Night1);
	string traningname = (traningonduty ? "Personal trained " + traningonduty->m_Realname + "" : "the Personal trained");

	sGirl* puppyonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_HOUSEPET, Day0Night1);
	string puppyname = (puppyonduty ? "Puppy girl " + puppyonduty->m_Realname + "" : "the Puppy girl");

	if (train >= 50)			// they refuse to train
	{
		ss << " refused to train during the " << (Day0Night1 ? "night" : "day") << " shift.\n";
		ss << girlName << " is still in training, and is having difficulty accepting her new role. Today she was a bad girl!\n";
		if (girl->get_training(TRAINING_PUPPY) >= 50)
		{
			if (roll_b >= 66)
			{
				if (dispmod >= 0)
					{
						//ss << "\nNice.";
						ss << "Good puppies eat their dog food and only beg for table scraps sparingly. " << girlName << " became bad tempered and flipped her bowls over, making a mess of the kitchen and herself. You were not amused, nor was the cleaning girl.\n";
						/**/
						ss << "It's difficult to stay upset at such a cute little animal for long, so other than a couple swats on " << girlName << "'s upturned ass and letting " << cleanername << " rub her face in it";
						ss << " (After all, she was the one who was going to have to clean her up later!), you take a more passive approach.";
						ss << " The next morning your pet clearly was expecting resistance when " << headname << " placed her food and water dishes on the floor, but you and your girls merely went about your business as she sullenly refused to eat.";
						ss << " As the day continued, she seemed confused as she was left to her own devices during mealtime. The food was there, but she wasn't forced to eat.\n";
						/**/
						ss << "By the end of the first day, " << girlName << " was clearly having a great deal of effort keeping herself under control as hunger rumbled through her stomach making her tits quiver.";
						ss << " She watched a little less sullenly as " << traningname << " eagerly sucked your cock under the dinner table while you attended to your meal and read business reports.";
						ss << " Her eyes watered as she watched you feed the girl scraps from your dinner and small chocolate truffles when she finished, and " << traningname << " happily rested her head in your lap, looking up at you.";
						ss << " How dare she? That was " << girlName << "'s job! But still " << girlName << " refused.\n";
						/**/
						ss << "By dinner time the next night, the seasoned kabobs of meat and veggies were too appetizing to resist, and " << girlName << " crawled under the table, placing her cheek against your manhood, tits quivering as hunger rocked her stomach. " << girlName << " made little whining noises when you smiled down at her,";
						ss << " stroking her hair, but you did not give her a yummy cock to suck. You did not give her treats. Tears formed in her eyes as the table was cleared.";
						ss << " It was all so unfair! She eyed her kibble grudgingly as you left the room with one of your personal bed warmers to join the others upstairs.";
						ss << " Gingerly she leaned in and began picking up little bits of stale kibble with her tongue and mouth,";
						ss << " gagging a bit as the dry crusts of the nauseating substance hit her throat. It was difficult, though alternating with lapping up water helped. After she had struggled it down,";
						ss << " she licked the dust from the bowl as her stomach growled, albeit less insistently.\"Good girl, " << girlName << ".\" " << girlName << "'s eyes snapped up, humiliated, to " << headname << " as she clipped a leash to her collar.";
						ss << " " << headname << " smiled and stroked her hair. \"See, you can be a good girl! I think your master is occupied tonight, but you deserve a reward.";
						ss << " I think you can sleep at the foot of my bed instead of in your kennel tonight.\" " << girlName << " crawled after " << headname << " obediently.";
						training += 2;
						ob += 2;
					}
				else
					{
						//ss << "\nBad.";
						ss << "Good puppies eat their dog food and only beg for table scraps sparingly. " << girlName << " became bad tempered and flipped her bowls over, making a mess of the kitchen and herself. You were not amused, nor was the cleaning girl.\n";
						/**/
						ss << "\"Master, what a mess!\" " << cleanername << " tried to sweep the kibble but the floor was covered in water, smearing the brown nuggets across the floor.";
						ss << " You nod to " << headname << ", not even looking up from your eggs and blowjob.";
						ss << " " << headname << " grabbed " << girlName << " by the hair, smushing her face into the mess, ignoring her tears.";
						ss << " \"Your master is displeased, bitch!\" she hissed in her ear. \"We WILL teach you to be a good girl!\"";
						ss << " " << headname << " scooped bits of the dirty, soggy dog food from the floor and began forcing it into " << girlName << "'s mouth, occasionally rubbing it on her face and giving her cheeks vicious slaps.\n";
						/**/
						ss << girlName << " was taken by the hair and dragged to the dungeon where she was turned over to Torturer. \"Well well, we've got a naughty puppy don't we?\" Torturer's voice was sweet and comforting, but her eyes were cruel.";
						ss << " Tying " << girlName << "'s hair into a braid, she tied the braid to a hook and pulled her off the ground. " << girlName << " screamed through her ring gag but her pleas were ignored and her plight just beginning.";
						ss << " Torturer really pulled out all the stops, first feeding her a mix of animal cum and not-quite ground up bugs, then taking a stinging whip to her body. " << girlName << " sobbed through the gag more when";
						ss << " Torturer used a spritz bottle of salt water all over her wounds, and left her in the dark, quivering. Hours later she returned with a few of your best Gang members.";
						ss << " Torturer patted " << girlName << "'s cheek with a smile to get her attention, while rubbing " << girlName << "'s face against the bulge of the largest member.";
						ss << " Then she turned to leave, and reminded the boys, \"Remember boys, no permanent marks. Be sure she eats ~something~ too. Your choice as to what.\"\n";
						/**/
						ss << "You didn't see your pet for two days, but when she arrived for breakfast she was much changed, at least for now. " << girlName << " ate her small cup of foul kibble without complaint, and then crawled over to you, lapping at your shoes in thanks.";
						ss << " You ignore her but still stroke her back and her ass, feeling her flinch each time your hands meet flesh that had thin, red welts from her whipping.";
						ss << " When your shoes have been buffed to a nice shine, she places her cheek on your groin and you finally look down into her her eyes,";
						ss << " clearly fearful of your displeasure. You smile and stroke her hair. \"You may.\" That's all you say, and you look away.";
						ss << " Clearly she is confused a moment, but quickly recovers and manages with difficulty to pull out your cock with her tongue and lips.";
						ss << " You continue to ignore her as she bobs up and down, but you can feel her eyes on you. When the cum hits the back of her throat,";
						ss << " she squeals as you hold her down. As your dick begins to soften you loosen your hold, stroking her hair as she licks you clean. \"Good girl, " << girlName << ".\"";
						ss << " You pat her cheek with a slight smile and " << girlName << " flushes in embarassment. " << headname << " clips a leash to " << girlName << "'s collar and leads her away with a smile.";
						training += 4;
						ob += 4;
					}
			}
			else if (roll_b >= 33)
			{
				ss << "While getting her daily bath from " << cleanername << ", " << girlName << " made a huuuuuuge mess " << cleanername << " was not pleased,";
				ss << " but realizes the bitch is still just an untrained, rebellious dog. After a few vicious swats on " << girlName << "'s upturned";
				ss << " ass, " << cleanername << " clipped a least to her collar. \"We'll see how you like making messes when you're forced to clean them!\" she exclaimed.";
				ss << " After getting " << headname << "'s permission, " << cleanername << " took " << girlName << " with her all day during her jobs. " << girlName << " was forced to clean toilets,";
				ss << " foul portions of the floor with day old cum, andevery other foul thing " << cleanername << " encountered with nothing but her tongue, enforced by an occasional whipping.\n";
				/**/
				ss << "By the end of the day, " << girlName << " was quite contrite as " << cleanername << " lead her crawling back to the bathroom. \"I'm sorry, but you'll learn, one way or another, " << girlName << ".\"";
				ss << " It didn't take much coaxing to get " << girlName << " into the frothy tub, and surprisingly " << cleanername << " joined her.";
				ss << " Both girls were clean soon, but " << cleanername << " decided they were going to stay and relax a bit longer.";
				ss << " \"Don't you think you owe me an apology?\" she asked, stroking the suds from " << girlName << "'s hair. Whether she did or";
				ss << " not, " << girlName << " leaned in, ignoring the soap getting in her mouth, and began to gently tongue " << cleanername << "'s clit. She gasped as your pet suckled her,";
				ss << " and then grabbed the back of her head as " << girlName << " began to fuck her with her tongue, occasionally pushing and nibbling at her clit at the same time using her upper lip.";
				ss << " Perhaps bath time would get better for both of them, but there was a ways to go.";
				training += 2;
				ob += 2;
			}
			else
			{
				ss << "Pets should always know better than to chew on shoes, yet here they were, one of your finest pairs, leather gouged by her teeth. This is common with new pets who still think they're people, but still disappointing.\n";
				if (dispmod >= 0)//GOOD
				{
					ss << "You initial reaction is a swift spanking, and you oblige with a number of well-aimed swats at " << girlName << "'s ass with the offended shoe.";
					ss << " She whimpers and cries, but you know this type of punishment doesn't necessarily help.";
					ss << " Sighing, you clip a leash to her collar and lead her out to get you a new pair of shoes. You go to your dad's old cobbler, where you've been going since you were a little boy.";
					ss << " He greets you like his own son. \"Here for a pair of shoes? Did she chew some up?";
					ss << " Not a word, I know just how to deal with this while we get you set.\" You pass the leash off to a smiling salesgirl who has obviously done this before.";
					ss << " She leads " << girlName << " away while you and the cobbler talk idly of the old days and get your new shoes sized.\n";
					/**/
					ss << "\"Well, you know the drill, Jr. No charge for you, but come on back this afternoon for your new shoes, I'll have them sized right.\"";
					ss << " The sales girl smiled and waved from behind the counter as you walked out the door.";
					ss << " You don't wonder where your pet is, you know she is likely between the girl's legs. You go about your day,";
					ss << " and when you return, the sales girl smiles brightly at you. \"Right this way!\" she says, and she leads you to the back office.";
					ss << " There you find the old cobbler stroking your pet's hair as she bobs up and down on his dick. \"Good to see you, be done here in just a minute!\"";
					ss << " You don't mind waiting, looking around while you wait. There had to be 4 dozen pairs of shoes lined up neatly, shining in the dim light.";
					ss << " You know from experience that these were left by high-paying customers to be properly cleaned, and each would get a video of the \"cleaning\" upon pickup.";
					ss << " No wonder the sales girl likes you, normally ~she~ is the one who has to lick them clean. You feel something something at the front of your own pants.";
					ss << " Looking down, you see just in time as the sales girl inhales your cock. \"Sometimes a girl just needs to be aclimated.\" the cobbler said sagely, stroking your pet's hair.";
					ss << " \"They don't always realize what they are from the start, it's our job to gently help them along.\" You couldn't agree more.";
					training += 2;
					ob += 2;
				}
				else//BAD
				{
					ss << "\"Bad dog!\" you should, wailing on your pet's upturned ass, your foot in the middle of her back, forcing her down. " << girlName << "";
					ss << " struggled as you jerk her up by her hair and begin slapping her tits and cheeks alternately.";
					ss << " \"Very bad dog! Chewing shoes is fucking bad behavior, you little bitch!\"";
					ss << " In a quick motion, you force a dildo that is entirely too large down " << girlName << "'s throat and pull a tight cloth hood over her face.";
					ss << " She struggle's to keep up as you drag her outside by her leash into the chilly air.";
					ss << " Using a padlock, you chain the little bitch to a small post cemented into the ground.";
					ss << " \"Let's see how you like a week out here!\" you hiss, and give her one more savage slap across the face that she couldn't see coming.\n";
					/**/
					ss << "For the next week, you occasionally look in on her, watching her shiver in the cold. The guard dogs found her rather quickly,";
					ss << " and their company was likely a mixed blessing for her. On the one hand, they curled up next to her often,";
					ss << " keeping her warm as one of their own. On the other, " << girlName << " received rather savage fuckings from all of them on a regular basis.";
					ss << " " << headname << " occasionally check in on her, to make sure her vitals were okay, despite the chilly weather,";
					ss << " sensory deprivation, and lack of food and water. Your guards cared a bit less, and often stopped by to relieve themselves on her face, soaking her hood.";
					ss << " They laughed at her distress, and occasionally placed a well aimed kick at her backside.";
					/**/
					ss << "Finally you go to get her at the end of the week. " << girlName << " is a complete wreck, shivering and skittish. You pull get her attention with a quick slap to the face, and a few more to the tits and cunt for good measure.";
					ss << " Finally you pull the dirty hood off her face. Pulling the dildo from her mouth was difficult, and obviously painful for her.";
					ss << " She yelped in surprise as you pushed it into her ass, dry, and the begin to rub her jaw affectionately, easing her lockjaw until she is able to close her mouth.";
					ss << " She whimpers and cries most of the process, but when it's over you cup her chin with one hand and stroke her dirty hair with the other.";
					ss << " \"See, you don't want to be a bad girl anymore, do you? Being a bad girl is... unpleasant.";
					ss << " Show me what a good girl you can be.\" " << girlName << " was dehydrated enough that tears didn't come,";
					ss << " even as she looked up at you and ran her dry tongue somewhat fearfully and appreciatively over your fingers as best she could.";
					ss << " You smile, undo the leash from the post, and lead her crawling dejectedly back to the house.";
					training += 3;
					ob += 3;
				}
			}
		}
		else//low skill
		{
			if (roll_b >= 50)
			{
				ss << "\"NO! What the fuck is wrong with you!\" " << girlName << " screamed up at you. \"I'm NOT your fucking dog! Let me up or you'll be sorry!\" ";
				ss << "\"You can't treat people like this!\" You sigh as " << girlName << " screams and raves. New pets often act out their aggression. It's not entirely their fault, ";
				ss << "they haven't realized yet that they are nothing but little bitches. Still, whether or not they were at fault pre-training, such behavior had to be nipped in the butt early.\n";
				if (dispmod >= 0)/*NICE DISP*/
					{
						ss << "\"Bad girl, " << girlName << ". Bitches don't use human words!\" You admonish her with a slight slap to the face, followed by soothing petting. " << girlName << " was having none of it. \"Fuck you, asshole!\" You give her a slighlty heavier slap, you face turning stern.";
						ss << " \"I just told you, if you want to talk, you need to do it like the bitch you are.";
						ss << " Since you're not ready to obey yet, I'll help you learn.\" " << girlName << "'s mouth flew open to begin again but you shove a large, rubber bone in her mouth, causing her to gag and choke.";
						ss << " She glares at you, trying to push the toy out of her mouth with little effect.";
						ss << " Meanwhile, you undo the lock on her collar and remove it, replacing it a moment later around her thrashing neck with a metal collar that seemed a little different.";
						ss << " As the collar clicked in place, you put your hand gently under " << girlName << "'s mouth, holding the toy in.\n";
						/**/
						ss << "\n\"Now then, you've earned yourself a week of quiet time, girl.\" " << girlName << " looks at you in consternation as you hold the toy in while stroking her hair. \"This collar isn't going to stop you from speaking, but it's going to help teach you not to. Anytime you try speak and it's not using.... proper language, you're going to get a small shock. ";
						ss << "If you keep trying within a short period of time, the shocks will get worse, and persist longer.\" You look down at the girl's watering eyes with some sympathy. ";
						ss << "\"I wish you'd instead just decided to be a good girl. No matter, I'm here for you, to help you learn.\" With a final pat of the cheek, you pull the bone from her mouth. Immediately " << girlName << " tries to speak, but whatever she was going to say was lost in her cry and shudder, shocked by the collar. You stand up, looking down at her, smiling sadly. ";
						ss << girlName << " tried once more to say something, and once more was met with a violent shock, bringing tears to her eyes and little sobs. Your pet was in for a long week.\n";
						/**/
						ss << "\nWithin the first day, though, " << headname << " remarked on " << girlName << "'s progress. Soon the girl was indeed trying to speak using barks and whines. She didn't like her food, she wanted attention, she wanted to go out. She did make a few attempts at words at first, but quickly was forced to abandon that line of attempt. By the end of the week, she was grudgingly using only puppy language, and literally eating out of " << headname << "'s hand. ";
						ss << " When it was finally time, you bent down and called " << girlName << " to you, her old collar in hand. She was clearly happy to see it, but before you replaced the new collar, you stroked the girl's chin and petted her hair.";
						ss << " \"Are you ready to be a good girl, " << girlName << "?\" She gave a small, reluctant yip, obviously trying to wag her tail. When you didn't replace the collar, she began to lick your fingers, nuzzling the hand that clutched her old collar.";
						ss << " \"Are you sure? Maybe you need another week?\" " << girlName << " let out a low, feminine howl, and began to lick and kiss her old collar. \"Just checking, girl.\"";
						ss << " You laugh and unlock the old and new collars, and " << girlName << " quietly places her neck in the band, shuddering a little as it clipped shut. \"Good girl.\" ";
						ss << " You pet her hair, and she leans into your hand, closing her eyes. \"Now you'll have to police yourself. Do you think you can do that, girl?\" She barked.";
						training += 2;
						ob += 2;
					}
				else if (dispmod < 0)/*BAD DISP*/
					{
						ss << "\"STUPID BITCH!\" " << girlName << " cries out as you grab her collar and begin slapping her face. \"Dumb cunt, bitches can only fucking bark.\" She tries to reply, perhaps as an apology, but you don't let her, continuing heavy slaps to the girl's face, jerking her up painfully by her hair and landing a few slaps on her bouncing tits.";
						ss << " \"WHY. CAN'T. YOU. FUCKING. LISTEN?!\" \"Please-\" She cried in a meek voice between sobs. \"WHAT. THE. FUCK? WHAT THE FUCK DID I JUST SAY BITCH?!\" ";
						ss << " You don't even bother with the leash, you just yank " << girlName << "'s hair as she struggles to keep up on all fours. In your anger it took no time at all to make it to the dungeon at your brothel.";
						ss << " The normally cheery Torturer blanched at your anger, and quickly handed over one of her most biting whips, designed to cause lasting pain without causing too much lasting damage.\n";
						/**/
						ss << girlName << " blubbered as you locked her on all fours in the middle of the room near the entrance. \"We'll fucking teach YOU what language is acceptable!\" She began to scream as you criss-crossed her back and her ass,";
						ss << " occasionally striking at her side to hit her tits that she had pressed to her knees, and her thighs. You even took the time to whip the soles of her feet.";
						ss << " After all, if the bitch didn't know how to talk, she probably needed correction on crawling too! Each time she thought you were done, she tried begging, causing you to start again.";
						ss << " After a couple hours, she wasn't even able to cry anymore, instead just heaving dryly, not trying to avoid the blows. Finally you stop and grab her hair. \"What do you say bitch?\" \"Tha- I-Th-\"";
						ss << " \"WHAT DID I JUST BEAT YOU FOR?\" It began again. Another hour and you tried again. \"Well bitch?\" It took a few tries, since " << girlName << "'s mouth was completely dry, but she managed a barely adequate, \"Woof...\"\n";
						/**/
						ss << "You left your pet there in the dungeon while you attended to other business. " << torturername << " sent a girl along with a bucket of water, and the girl was gently, generously letting " << girlName << " take a few swallows before she began to clean away the traces of her tears and her beating.";
						ss << " When she was done, the girl put the bitch's head in her lap and petted her,whispering sweetly to her.";
						ss << " \"Tha- thank you.\" " << girlName << " said haltingly. The cleaning girl's face turned bright, as she raised her voice.";
						ss << " \"Mistress? She spoke! The bitch spoke!\" " << girlName << " stared in disbelief at the girl who had just cared for her so tenderly. Soon she was sobbing again as the blows rained down on her.";
						ss << " When " << torturername << " was done, she pulled the girl's sagging head up by her hair. \"What do you say now, bitch?\" \"Woof.\" \"What was that?\" \"Woof!\" " << girlName << " croaked out. \"Good girl.\"";
						training += 3;
						ob += 3;
					}
			}
			else
			{
				if (dispmod > 0)
					{
						ss << "\nNice.";
					}
				else if (dispmod < 0)
					{
						ss << "\nBad.";
					}
				else
					{
						ss << "\nNetural.";
					}
			}
		}
	}
	else /*She did the training*/
	{
		ss << " trained to be a house pet.\n \n";
		if (girl->get_training(TRAINING_PUPPY) >= 70)
			{
				if (girl->m_WeeksPreg >= 38 && g_Brothels.GetNumGirlsOnJob(0, JOB_HEADGIRL, false) >= 1)
				{
					ss << headname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "" << girlName << " was very, very pregnant. " << headname << " had to isolate " << girlName << " from the other puppies, animals, guards, just about everyone really.";
					ss << " The rough play and enthusiastic activities " << girlName << " was used to could possibly injure her puppies, and";
					ss << " no one wanted that. " << girlName << " spent her days in a kennel that was much larger than she was used to, with a very soft bed. Her pregnancy was";
					ss << " making keeping her proper puppy posture difficult, but " << girlName << " was well-trained. The veterinarian would stop by every couple days to check on her,";
					ss << " so " << girlName << " knew it would be any day now, and she was anxious for the pregnancy to be over so she could get back to being a part of the household.\n";
					/**/
					ss << "" << girlName << " wasn't lonely, of course. Everyone wanted to visit her, and did! Under the watchful eye of " << headname << ", the girls would stop by and sit with her,";
					ss << " petting her hair and rubbing her swollen stomach, telling her what a good girl she was and how they missed her. The guards would send one of their number";
					ss << " every so often, and he would slip " << girlName << " a Bitch Bite when " << headname << " wasn't looking. Most enjoyable were her Master's visits. " << girlName << " saw";
					ss << " her Master every other day when he would stop and show her affection. She could always tell he coming by the sound of his boots, and his scent,";
					ss << " from down the hall. Each time she would greet him by licking his shoes and barking like a good girl. " << girlName << " loved how her Master would smile, and even";
					ss << " if he couldn't stay long, was happy when he came to check on her. Sometimes he would let her suck his cock, like she did before she was pregnant and";
					ss << " isolated. " << girlName << " loved that most of all. She missed laying at her Master's feet, missed sucking his dick, missed the feeling of his hands as he pet her.\n";
					/**/
					ss << "\"Time for bed girl.\" " << girlName << " gave a pouting whine at " << headname << ". Master must have been busy today and couldn't visit. " << girlName << " whimpered in resignation,";
					ss << " and respectfully licked " << headname << "'s long legs. " << headname << " smiled and pet her hair, but still said, \"I mean it, bed!\" " << girlName << " crawled";
					ss << " to her spacious kennel with her soft bed. Her belly and breasts swayed, both occasionally brushing the ground, and she back into the kennel and curled up";
					ss << " on her bed. " << headname << " closed the door, wished her a good night, and turned off the light. " << girlName << " sighed restlessly and slowly fell asleep.";
				}
				else if (roll_b >= 90 && girl->check_virginity())
					{
						if (roll_c >= 85)
						{
							if (dispmod >= 0)/*NICE DISP*/
							{
								ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
								/**/
								ss << "" << girlName << " was anxious. When ";
								if (headonduty)
								{ ss << "" << headname << ""; }
								else if (traningonduty)
								{ ss << "" << traningname << "";  }
								else if (cleaneronduty)
								{ ss << "" << cleanername << "";  }
								else if (bedwarmeronduty)
								{ ss << "" << bedname << "";  }
								else if (recruiteronduty)
								{ ss << "" << recruitername << "";  }
								else
								{ ss << "the girl"; }
								ss << " told her with a smile that today was a special day, she was ~so~ certain that her master would finally fuck her, and finally let her cum.";
								ss << " She paced back and forth, shaking in anticipation, whimpering as random members of the household stopped to play with her moist, throbbing pussy.";
								ss << " Her cunt seemed to throb as the seconds ticked on the clock, waiting for you to return home. When you returned, she greeted you at the door";
								ss << " like a good little bitch should, with happy barking, pushing her face into your groin. " << girlName << " was disappoint when you just";
								ss << " gave her a pat on the head and went to your room to read. " << girlName << " followed you, panting, and licked your shoes, pawing at your cock through your pants.\n";
								/**/
								ss << "She spent several minutes fawning over you before you finally looked down to find her humping your leg gently with a pleading look on her face.";
								ss << " You smile and stroke " << girlName << "'s hair.";
								ss << " \"Who's my good girl? Who's master's good little bitch?\" " << girlName << " barked happily, and licked your fingers. \"Alright girl,";
								ss << " I suppose you've waited long enough. Present!\" " << girlName << " yelped as her paws tangled beneath her, unable to move fast enough. Soon she had her face and";
								ss << " tits pressed to the carpet, her arched back pushing her round ass into the air. She panted in anticipation, waiting and waiting. Minutes dragged on.";
								ss << " Suddenly she felt a hand on her ass, giving it a little slap, and then playing with it, pushing her cheeks together. She yelped in surprise and moaned.\n";
								/**/
								ss << "It was so sudden, no real workup. She felt the cock she had been sucking for ages now thrust into her aching pussy hard. She whined at first, as your balls started slapping her clit,";
								ss << " your cock pushing through the tight walls. It hurt. Why did it hurt? When she tried to raise her head, you shushed her by gently placing your hand on the back";
								ss << " of her neck. After a couple minutes, " << girlName << " was able to concentrate past the initial pain. How could this feel so good? Unbidden, her cunt tightened";
								ss << " and clamped down on your cock, and she made little, happy whimpers with each thrust. And she came. Oh how she came. Every couple minutes an orgasm";
								ss << " build and rocked her body as you continued to thrust hard into her pussy. " << girlName << " barked happily as you wrapped her hair around your hand and pulled her up on";
								ss << " her paws, her tits swinging as you began to thrust more furiously. Suddenly she felt you slam into her with a mighty jerk of her hair, and she";
								ss << " howled as your throbbing cock released spurt after spurt of warm, sticky cum into her womb. She slumps, exhausted as you sit back into your chair and";
								ss << " close your eyes. After a moment you feel " << girlName << "'s tongue running along each surface of your cock and thighs, cleaning you as she had";
								ss << " been taught. When you finally open your eyes, " << girlName << "'s cheeks are resting between your cock and your thigh, smiling as you pet her hair. \"Good girl.\"";
								training += 4;
								ob += 4;
								girl->lose_virginity();
								imagetype = IMGTYPE_SEX;
							}
							else
							{
								ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
								/**/
								ss << "\"Today is the day, my pretty little bitch.\" You give " << girlName << " an evil smile as you yank her leash, causing her to yelp in surprise. " << girlName << " was torn between a base";
								ss << " need, having been denied and teased for weeks or months, and apprehension. She knew full well that this was the day she was finally losing";
								ss << " her virginity, and she had longed for the day with a fearful mind. Her cunt dripped constantly because she was in a constant state of excitement, but";
								ss << " her master had taken a worrisome, fiendish glee in counting down the days on the calendar, reminding her each morning as she sucked";
								ss << " him off or he whipped her to remind her of her place. As you lead " << girlName << " through the city, her pussy glistened for everyone looking on.\n";
								/**/
								ss << "When you finally reached your destination, " << girlName << " was shocked to hear whines and barks, and her own soon joined the chorus as you dragged her into the";
								ss << " city dog pound. \"First breeding day?\" A slovenly man walked from behind the counter in the dingy front room, and spit on the floor as you";
								ss << " handed " << girlName << "'s leash over. \"Yeah, feel free to use her when she's not enjoying her kennelmates. I'll be back in a week.\" " << girlName << "'s eyes grew wide.";
								ss << " A week in this place? This was where her virginity would be taken? " << girlName << " began to whine loudly and press herself against her uncaring master's";
								ss << " leg, but was cut off with a quick yelp as the man dragged heavily on the leash, pulling her through a back room and into the kennel yard. The barking";
								ss << " got louder as " << girlName << " was paraded past a line of kennels and dog runs, each with several dogs, most with a cowering girl. Finally the man stopped";
								ss << " in front of a run with 6 large dogs. \"Be careful of Prince. He's the black one, bitch. He mauled his last cunt.\"\n";
								ss << " The man unclipped the leash from the crying girl's collar and pushed her hard through the gate in the fence. The dogs pounced roughly in second.";
								/**/
								ss << "A week later, you returned to find the man half asleep behind the counter. When you ring the bell, he grunted and stood up.";
								ss << " \"Here for " << girlName << "?\" He asked shortly, shoving a leash and tape of her first day into your hands.";
								ss << " \"Out back.\" You know the way, of course, and head back to the kennels. " << girlName << " was occupied, Prince pounding on her back as she moaned and whined.";
								ss << " You politely wait until Prince is finished, and when he's done, " << girlName << " crawls over to the gate. \"The fuck? I taught you better manners than that,";
								ss << " bitch.\" You nod towards Prince in disgust, and the girl crawled back, dismayed. " << girlName << " pushed her way under the large dog and gently used";
								ss << " her lips and tongue to clean him as she had been taught, and finally crawled back to the gate where you waited. She licks and kisses your shoes as you leash her.";
								ss << " \"Have a good first time, bitch?\" " << girlName << "'s eyes streamed with tears, but she barked an affirmation. She was, after all, just a bitch dog. This is what she was.";
								training += 4;
								ob += 4;
								girl->lose_virginity();
								imagetype = IMGTYPE_BEAST;
							}
						}
						else
						{
							ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
							/**/
							ss << "" << girlName << " was in a constant state of confusion, some days. She was coming acclimating to her status as a mere pet, an animal,";
							ss << " but still having trouble at times with that meant. When she first became a pet, " << girlName << " was";
							ss << " so rebellious,  she often earned punishments that she now understood were well-deserved. On the flip side, her Master and occasionally";
							ss << " the other girls would reward her good behavior as well, allowing her to cum. As the weeks progressed and she began to learn the joy of";
							ss << " serving her owner for it's own sake, she was forbidden to cum, and slowly weaned off it being a reward. " << girlName << " simply didn't know what to do.";
							ss << " She was a good pet, but she spent most of her days unfulfilled, her cunt twitching with an anticipation and need that she couldn't fulfill.\n";
							/**/
							ss << "Secretly, " << girlName << " knew the answer to the question she couldn't ask. As she lay on the couch with her head in your lap, a special privilege in and";
							ss << " of itself, she understood that you were teaching her. Her master made ~everything~ a lesson. Her virginity remained intact and";
							ss << " she was forbidden to cum for one reason: she wasn't ready. Her master needed her to know that it was ~his~ decision, and that getting fucked was a privilege";
							ss << " she didn't deserve, and that she hadn't sufficient begged for that privilege. As you pet her hair, occasionally rubbing her constantly";
							ss << " hard nipples, she understood but was unwilling to admit that she was being trained to be horny at all times, but serve at her master's pleasure.\n";
							/**/
							ss << "" << girlName << " began to moan softly as you tickled her clit with your fingers, and rub her smooth mound. It was a torment, but a welcome one,";
							ss << " and in minutes she was occasionally trying to push backwards on your fingers. " << girlName << " desperately";
							ss << " wanted something, anything, in her pussy. She began to pant and nuzzle your hardening dick through your pants. You smile and stop rubbing,";
							ss << " leaving " << girlName << " bucking empty air for a few moments. She whimpered up at you, and you smiled at the tears pooling in the corners of her pleading eyes.";
							ss << " \"What's wrong girl? What do you want? Do you need something?\" " << girlName << " gave a tiny bark, wagging her ass, and you laugh, \"No, I don't think you're quite ready. Clean your mess.\"";
							ss << " " << girlName << " whined, but licked her master's sticky fingers, flushing with both humiliation and embarrassment. Things she never would have considered before now";
							ss << " gave her such perverse pleasure. When she finished, you quiet her by pushing her head back down and petting her hair, her eyes still full of tears.";
							training += 4;
							ob += 4;
							imagetype = IMGTYPE_ORAL;
						}
					}
				else if (roll_b >= 80 && g_Brothels.GetNumGirlsOnJob(0, JOB_HOUSEPET, false) >= 2)
					{
						ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
						/**/
						ss << "Bitches were were an odd mix of competitive and collaborative. " << girlName << " and " << puppyname << " were quite happy right now, between your legs,";
						ss << " sharing a bone. It was interesting, not to mention pleasurable, to watch their interactions,";
						ss << " so much so that you put down the book you were reading. " << girlName << " and " << puppyname << " seemed to alternately";
						ss << " compete with each other, pushing the other out of the way so that one was forced to the balls instead of having cock down her throat,";
						ss << " and then coming together to enjoy your cock with one another. When they weren't fighting to be the one sucking your dick greedily deeper,";
						ss << " they would almost shyly kiss each other around the tip, and run their tongues up and down it's length, watching it quiver with delight.\n";
						/**/
						ss << "At times they would even go further than sharing, with " << puppyname << " nuzzling " << girlName << "'s neck, kissing and licking her as";
						ss << " she sucked on your cock, or vice versa. A couple times you had to give both";
						ss << " girls a gentle slap to the cheek or the tit to remind them that they were supposed to be pleasuring you, not just each other.";
						ss << " It really was odd that the two seemed both so much at odds, and yet so in sync. You would almost think they were litter-mates,";
						ss << " but you knew that wasn't true. Perhaps kennel-mates, but they weren't actually related. Not this pair, anyway.";
						ss << " Perhaps being trained and sharing a kennel brought girls together in a way you couldn't really fathom.\n";
						/**/
						ss << "You suddenly erupt, spewing cum from the tip of your cock. " << puppyname << " had been gently licking up your shaft while " << girlName << " had been";
						ss << " moving to suck it, and both got a large blast to their faces as they yelped in delight.";
						ss << " Gingerly, both girls licked your cock clean before beginning to lick your salty cum from each other's faces, and the drops that hit the floor.";
						ss << " You return to your book, satisfied. For a couple moments, the two of them licked your left toes, making small sighs of happiness.";
						ss << " Puppies are easily distracted though, and the licking soon turned back on each other.";
						ss << " Within minutes they were curled up on their sides at your feet, licking each other's cunts, moaning in pleasure. It did make it hard to read,";
						ss << " but the bitches were just doing what they should. You smile at the happy look in " << girlName << "'s eyes, and turn the page.";
						training += 2;
					}
				else if (roll_b >= 60 && g_Studios.GetNumBrothels() > 0)
					{
						ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.";
							/**/
							ss << "Just another day at the studio, with " << girlName << " in tow. She was such a delightful pet, all your porn stars and scene crew loved her visits.";
							ss << " Some of the best scenes Director shot were when " << girlName << "'s leash was handed over and she was led into being an impromptu fluffer,";
							ss << " or used as a reward for an excellent scene. Today was different, though. Today, " << girlName << " was the star!\n";
							/**/
							ss << girlName << " yipped nervously as her makeup was done, pressing her body into the side of your leg. Once her makeup and hair were taken care of,";
							ss << " she watched in anticipation as the stage was set up like a kitchen. Suddenly a much louder, booming bark";
							ss << " approached. " << girlName << " quivered, pressing against you harder, nuzzling and licking your hand as a large mastiff was brought in,";
							ss << " barely under control as it strained against it's trainer's leash. You smile and quiet her, trying to calm her by stroking her hair.";
							ss << " \"Don't worry, sweetie, he's your co-star. We're shooting a commercial for a major brand of dog food I own shares in.\"";
							ss << girlName << " relaxed a little, but remained pressed against you until you handed her leash off to RandomStageHand.\n";//here CRAZY
							/**/
							ss << "The commercial was a little difficult to shoot. Your little bitch went through her roll with the grace trained into her, but";
							ss << " the mastiff was over-eager, and bits of the scene had to be shot several times.";
							ss << " Still, all went well, and the Director finally yelled \"Cut!\" with both " << girlName << " and the large dog's faces pressed";
							ss << " into their meal. ";
							if (m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel) && !girl->check_virginity())
							{
								ss << "Your bitch looked up, and began to crawl back over to you, but the mastiff had other ideas.";
								ss << " " << girlName << " suddenly yelped and began to let out a series of piercing, pleading barks as the mastiff mounted her from behind, and drove";
								ss << " it's large red cock into her welcoming cunt. \"Should we do something?\" someone on stage asked, more out of fascination rather";
								ss << " than actual desire. Almost certainly the cameras were still rolling at this point. \"No, he deserves his reward.\"";
								ss << " you reply, smiling as the dog plowed heavily into " << girlName << ", and her tits began to sway violent, her tongue panting like a good dog.";
								girl->upd_temp_stat(STAT_LIBIDO, -20, true);
								girl->beastiality(2);
								imagetype = IMGTYPE_BEAST;
								if (!girl->calc_insemination(*g_Girls.GetBeast(), false, 1.0))
								{
									g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
								}
							}
							else
							{
								ss << "Happy with " << girlName << " you promise her a reward later.";
							}
							training += 2;
					}
				else if (roll_b >= 40 && g_Brothels.GetNumGirlsOnJob(0, JOB_HEADGIRL, false) >= 1)
					{
						ss << headname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
							/**/
							ss << girlName << " spent a day with " << headname << ", as she often did when her Master was busy. Pets often became disappointed when their owners were neglectful,";
							ss << " but " << headname << " always worked hard to keep your pets busy. Sometimes she would play with them, sometimes organize fun activities,";
							ss << " sometimes even use them as rewards for the other girls in the house, since your pets were well-loved in the household, for a variety of reasons.";
							/**/
							ss << "Today " << girlName << " spent most of the day as a mischievous puppy, crawling behind " << headname << ", exploring the nooks and crannies of your house,";
							ss << " and generally causing playful trouble. " << headname << " was a little exasperated with " << girlName << "'s small antics, but could hardly";
							ss << " fault her for behaving as the little pet you trained her and intended her to be. " << girlName << " was sure not to completely crawl out of line, though,";
							ss << " and after a full day of endearing herself to her housemates with her barking, licking, bounding about, hiding of the girls' toys,";
							ss << " and playing with their used panties, she found herself contentedly curled up under " << headname << "'s desk, too pooped to party.";
							/**/
							ss << girlName << " cleaned " << headname << "'s shoes lovingly with her tongue, making sure not to miss a speck of dirt. When she was done, she curled up between her legs and";
							ss << " laid her cheek on " << headname << "'s thigh, watching her longingly as she did her paperwork. " << headname << " occasionally pet her hair,";
							ss << " and " << girlName << "'s tongue would hang out of her mouth in a cute pant, whining. \"Later,\" " << headname << " would say with a smile. " << girlName << " would not accept THAT, but";
							ss << " she also wouldn't openly disobey. Very gently, slowly even, she eased her face closer to " << headname << "'s panties, moving her skirt up as she went using";
							ss << " her nose. It took nearly half an hour before she was nosing " << headname << "'s clit through her panties. " << girlName << " would occasionally";
							ss << " flick her tongue out at the sweet dampness of the woman's panties, watching her occasionally twitch, her breathing slowly increasing. When " << headname << " still";
							ss << " did not say anything, " << girlName << " couldn't take it any longer, and pressed her face into the lovely mound, licking and kissing through the fabric,";
							ss << " trying to push it out of the way with her nose. " << headname << " finally gave up on her paperwork and leaned back in her chair,";
							ss << " enjoying the attention of the Master's little pet. After an hour and several orgasms had passed, " << headname << " finally gripped " << girlName << "'s hair and";
							ss << " pulled her away. \"Good girl, now let me work.\" Her voice was kind but firm, and " << girlName << " knew she was done for the time being.";
							ss << " Quietly, and curled up at " << headname << "'s feet, resting her cheek on her on of her shoes, waiting for her next instruction.";
							training += 2;
					}
				else if (roll_b >= 20)
					{
						ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
						/**/
						ss << "Many pet owners act neglectfully of their pets, but not you! " << girlName << " was happy when you led her crawling through the city";
						ss << " to your brothel, excited to keep you company while you worked. Upon arrival, however, you hand off";
						ss << " " << girlName << "'s leash to a member of a gang, who leads the confused girl fearfully into the dungeon, unsure of what to expect. " << girlName << " flinched";
						ss << " at the cries of pain, but instead of enduring punishment, she is led to a room with five girls, all young, frog-tied and placed";
						ss << " on their backs. " << girlName << " looked up at the gang member and yipped questioningly, nuzzling his thigh like a good puppy.\n";
						/**/
						ss << "�Often your Master gets girls that are virgins,� he explained, stroking " << girlName << "'s hair, �And this is part of their....";
						ss << " tenderizing process. We won't fuck them for a couple weeks yet at least, after they are branded, but we like to keep them excited.";
						ss << " That's your job today.� Not quite gently, the man grabbed her hair and pushed " << girlName << "'s face into the first girl's cunt. " << girlName << " began to lick";
						ss << " in earnest, both happy with her task, and that she was not being punished. Little muffled cries and moans came from the hooded girl";
						ss << " as she bucked her hips into the pet's lips and tongue. Clearly the girl had not had release in some some time. When the young slave-to-be's";
						ss << " nipples were hard as could be and she was on the cusp, the man tugged " << girlName << " away, leaving the teen thrashing, unfulfilled.\n";
						/**/
						ss << "The process was repeated over the course of a couple hours, the gang member occasionally giving " << girlName << "'s wagging ass little slaps to motivate her, as if she needed any motivation. As she licked the last girl,";
						ss << " she suddenly gasped as he shoved her face in the girl's pussy and pushed himself into hers. As " << girlName << " was being fucked, she crawled on top of the virgin";
						ss << " teen and pressed her tits into the other girl's, nipples roughly rubbing together as she ground their clits together. " << girlName << " came three";
						ss << " times as the man's cock throbbed in her, balls slapping into the teen's virgin pussy. When he was close, he pulled out of her pussy,";
						ss << " walked around the side, and jerked his cum onto the teen's budding breasts. \"Clean her up, and we'll get you back to your master.\"";
						ss << " The gang member slapped " << girlName << "'s ass once, and she began to gingerly lick the jizz from the teen's nipples.";
						training += 2;
					}
				else
					{
						ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
						/**/
						ss << "You take great care of your pets, taking them to the vet as often as you can. It's " << girlName << "'s turn to visit the veterinarian, and you'll be taking time out to bring her yourself.";
						ss << " Going to the vet is always distressing for any bitch, no matter how well trained, so you have a routine set out each time.";
						ss << " First " << girlName << " gets a treat with her breakfast, and soon after you're both off down the road to the vet, by way of the park.";
						ss << " In the park you stop and let " << girlName << " off her leash.";
						ss << " You smile and sit under a tree near the pond, watching her romp happily, barking at the ducks,";
						ss << " and occasionally socializing with other pets that happened by, and rubbing against the legs of other people passing through.";
						ss << " Many stop to smile and tell her she's a good girl, petting her hair.\n";
						/**/
						ss << "When she's tired herself out, " << girlName << " crawls back over to you and curls up between your legs, resting her head for a few minutes.";
						ss << " You sigh and stand up, dusting off your pants and pulling a few leaves from her hair as she smiles brightly at you and licks your hand.";
						ss << " Soon you continue on, and her mood changes a little as you take the side street to the vet's clinic.";
						ss << " She whimpers slightly as the bell rings and you lead her into the waiting room where you are greeted warmly by a nurse wearing scarcely anything.";
						ss << " Around the waiting room are a few other owners and pets, as well as a couple girls that were clearly there to be modified in... other ways. One cute puppy approached your own curiously, nuzzling her and exploring her with her tongue playfully.";
						ss << " It's clear " << girlName << " likes they new girl but is also afraid, perhaps because the girl's owner has given her limbs some amputations in order to make her a smaller breed.";
						ss << " " << girlName << " clearly had mixed feelings when led her back to the room for her checkup.";
						/**/
						ss << "It was simply routine. The vet massaged her tits, checking for lumps, and prodded her pussy and ass, taking a couple samples.";
						ss << " He also drew a vial of blood, all the while telling " << girlName << " what a good girl she was.";
						ss << " Half an hour later the nurse informed you of " << girlName << "'s clean bill of health, stroking her hair and letting your trembling pet what a good girl she was, letting her nibble a treat from her palm.";
						ss << " She finished up by rubbing your girl down with a special oil, both skin and hair, designed to keep her skin firm and supple and her coat soft and shiny.";
						ss << " " << girlName << " clearly enjoyed that part of the process, but was relieved to have it over and be heading home with her Master.";
						training += 2;
					}
			}
		else if (girl->get_training(TRAINING_PUPPY) >= 50)
			{
				if (girl->m_WeeksPreg >= 38 && g_Brothels.GetNumGirlsOnJob(0, JOB_HEADGIRL, false) >= 1)
				{
					ss << headname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "" << girlName << " was very, very pregnant. " << headname << " had to isolate " << girlName << " from the other puppies, animals, guards, just about everyone really.";
					ss << " The rough play and enthusiastic activities " << girlName << " was used to could possibly injure her puppies, and";
					ss << " no one wanted that. " << girlName << " spent her days in a kennel that was much larger than she was used to, with a very soft bed. Her pregnancy was";
					ss << " making keeping her proper puppy posture difficult, but " << girlName << " was well-trained. The veterinarian would stop by every couple days to check on her,";
					ss << " so " << girlName << " knew it would be any day now, and she was anxious for the pregnancy to be over so she could get back to being a part of the household.\n";
					/**/
					ss << "" << girlName << " wasn't lonely, of course. Everyone wanted to visit her, and did! Under the watchful eye of " << headname << ", the girls would stop by and sit with her,";
					ss << " petting her hair and rubbing her swollen stomach, telling her what a good girl she was and how they missed her. The guards would send one of their number";
					ss << " every so often, and he would slip " << girlName << " a Bitch Bite when " << headname << " wasn't looking. Most enjoyable were her Master's visits. " << girlName << " saw";
					ss << " her Master every other day when he would stop and show her affection. She could always tell he coming by the sound of his boots, and his scent,";
					ss << " from down the hall. Each time she would greet him by licking his shoes and barking like a good girl. " << girlName << " loved how her Master would smile, and even";
					ss << " if he couldn't stay long, was happy when he came to check on her. Sometimes he would let her suck his cock, like she did before she was pregnant and";
					ss << " isolated. " << girlName << " loved that most of all. She missed laying at her Master's feet, missed sucking his dick, missed the feeling of his hands as he pet her.\n";
					/**/
					ss << "\"Time for bed girl.\" " << girlName << " gave a pouting whine at " << headname << ". Master must have been busy today and couldn't visit. " << girlName << " whimpered in resignation,";
					ss << " and respectfully licked " << headname << "'s long legs. " << headname << " smiled and pet her hair, but still said, \"I mean it, bed!\" " << girlName << " crawled";
					ss << " to her spacious kennel with her soft bed. Her belly and breasts swayed, both occasionally brushing the ground, and she back into the kennel and curled up";
					ss << " on her bed. " << headname << " closed the door, wished her a good night, and turned off the light. " << girlName << " sighed restlessly and slowly fell asleep.";
				}
				else if (roll_b >= 85 && g_Brothels.GetNumGirlsOnJob(0, JOB_HEADGIRL, false) >= 1 && !girl->has_trait( "Your Daughter") && girl->has_trait( "Kidnapped"))
				{
					ss << headname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "" << girlName << " had mixed feelings about her occasional walks with " << headname << " or her Master, or even more occasionally her Master's recruiter.";
					ss << " She was deathly afraid of running into people from her life before she was a registered animal, but she knew better than to disobey.";
					ss << " A couple times, " << girlName << " thought she'd seen family or a friend in the distance, but never up close. Today was not to be her day.\n";
					/**/
					ss << "" << headname << " led the crawling girl down a path in the park towards the dog park, when they came upon a man who seemed like he was in a hurry.";
					ss << " " << girlName << " recognized the man almost immediately as her father, and almost said so,";
					ss << " but caught herself. She quickly looked up at " << headname << " who continued walking without a care. The man stopped in front of them.";
					ss << " He had clearly been preoccupied before, and almost ran into them. \"So sorry. What a cute d-\" " << girlName << "'s father froze, halfway bent to pet his";
					ss << " daughter, naked at the end of this woman's leash. \"...sweetie?\" " << girlName << "'s eyes brimmed with tears as she looked back and forth between her father and " << headname << ".";
					ss << " She knew she'd be punished severely for speaking. With resignation, " << girlName << " barked, and began licking her father's outstretched fingers.";
					ss << " \"Yes?\" " << headname << " said expectantly, not really sure what was going on, but certain it was ~gold~ for " << girlName << "'s training.\n";
					/**/
					ss << "\"I- uh, sorry.\" The man straightened, a lump in this throat. \"I thought I knew her, that's all.\" " << headname << " smiled, knowingly. \"She's a cute little bitch, isn't she, sir?";
					ss << " " << girlName << ", be a good girl, show him what you want!\" " << girlName << " whimpered, but slowly brought herself to her father's crotch, and began licking";
					ss << " and kissing the front of his pants. The man's eyes grew wide, and his face flushed as his prick clearly began to stiffen mightily in his pants.";
					ss << " \"Is there perhaps something she could... do for you?\" " << headname << " whispered in the man's ear coyly, stroking " << girlName << "'s hair with one hand,";
					ss << " her other on the man's chest. " << headname << "'s breath tickled the man's ear, her lips and nose so close he could feel them without touching them.";
					ss << " \"Ah- n, no...\" The man was clearly in pain, and his voice hoarse as his daughter continued, never looking away. \"Are.... you.... sure?\"";
					ss << " " << headname << " cooed, as the man's cock strained harder against his pants with each of his daughter's licks and kisses. \"She's very good.\" The man suddenly groaned and";
					ss << " shuddered as " << headname << " pressed his daughter's face into his crotch. \"I, I have to go.\" The man left in tears. \"Maybe you're still daddy's little girl?\"";
					ss << " " << headname << " said in an innocent tone, tugging " << girlName << "'s leash to propel the humiliated girl down the path again, face now sticky She never once spoke.";
				}
				else if (roll_b >= 70 && g_Brothels.GetNumGirlsOnJob(0, JOB_RECRUITER, false) >= 1)
				{
					ss << recruitername << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "Your girl decides to take your pet for a walk while she does errands. Passers ogle your pet as she crawls past different shops at the end of a leash, tits and ass swaying. Some turn away, but most laugh and point.";
					ss << " One dirty old man politely knelt down, petting " << girlName << "'s hair, and asked with a smile if she would like a bone.";
					ss << " Your pet barked happily, but the girl holding her leash tugged it gently, thanking the old man. " << girlName << " was a little disappointed, but she knew better than to disobey while leashed.";
					ss << " When they returned home, the girl stroked your pet's hair and offered her a treat for being such a good little bitch while they were out. ";
					ss << girlName << " became aroused as she happily licked the girl's cunt, sucking her clit till she came. For being such a good pet, she's allowed to sleep at the foot of the girl's bed that night instead of in her kennel.";
					training += 2;
				}
				else if (roll_b >= 55 && g_Brothels.GetNumGirlsOnJob(0, JOB_PERSONALBEDWARMER, false) >= 1)
				{
					ss << bedname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "\"Isn't she cute?\" " << bedname << " beamed, stroking " << girlName << "'s soft hair. The other girls nodded in agreement, occasionally reaching out to pet her. \"It must have been touch to realize you're just a little bitch though, huh?\"";
					ss << " " << bedname << " giggle as she gently gathered the pet's hair in her hand and pressed the girl's nose into her panties, rubbing her face up and down.";
					ss << " " << girlName << " didn't mind the humilitating comment. It's true, she was just a bitch, and deserved to be treated as what she was. She didn't think " << bedname << " was so great though.";
					ss << " Sure, maybe Master technically treated the girls warming his bed as human, but they really were little more than furniture. Not like her, not a beloved pet that Master cared about!";
					ss << " She her " << bedname << " moan gently and without thinking about it began to obediently eat her pussy right through the girl's panties, nibbling at her clit through a growing wet spot at her crease, and licking and kissing her like a good pet.\n";
					/**/
					ss << "\"I see you've already been getting my bed warm for me!\" You walk in the room smiling at the scene, and your " << bedname << " gasps, both from pleasure and from being startled.";
					ss << " \"Master?! I- we- that is, I was just getting myself ready for you. I hope you don't mind, I do love your pet. " << girlName << " is so affectionate.\"";
					ss << " You laugh. \"She is that. I don't mind at all, but your services aren't needed for the rest of the evening. My bed is warm, and I've got some reading to do.\"";
					ss << " Before " << bedname << " could complain, the dildo was already being pushed into her mouth.";
					ss << " Plugs were placed in her holes and turned on to keep her ready in case you changed your mind, and you strapped the girl up, lifting her to hang by a harness in the closet.";
					ss << " " << girlName << " yips and rubs her face against your bulge as you pull a hood tight over the bedwarmer's face and close the closet.\n";
					/**/
					ss << "\"Heel.\" You undress and climb into bed while your pet waits anxiously. Surely she wasn't going to spend a night in her kennel or in the doggy bed beside your bed, was she?";
					ss << " You smile and pat the side of your leg. With a happy bark, " << girlName << " leaped up on the bed and draped herself over your leg.";
					ss << " You gently stroke her hair as she slowly slurps on your cock and kisses your balls. You mostly ignore her and do your reading, but her adoring eyes are always on you.";
					ss << " When you stiffen and shoot your load in her waiting mouth, her eyes sparkle with happiness, and she spends another ten minutes suckling you clean.";
					ss << " When you don't look down from your book, she sighs with contentment, crawls to the end of the bed, and curls up.";
					ss << " After licking your feet, she falls asleep. It's just another day in the life of a petgirl.";
					training += 2;
				}
				else if (roll_b >= 35 && g_Brothels.GetNumGirlsOnJob(0, JOB_CLEANHOUSE, false) >= 1)
				{
					ss << cleanername << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << "It was time for " << girlName << "'s daily bath, something she and RandomHouseCleaner always enjoyed.";
					ss << " Your pet was happy to climb in the tub, and splash around enough to enjoy herself, but not make a huge mess for " << cleanername << ".";
					ss << " " << cleanername << " smiled and generously scrubbed the dirt from your girl, massaging her body and working the grime from her hair.";
					ss << " When she was finished, " << girlName << " was dried off with a towel, and a leash clipped to her collar.";
					ss << " \"I'm afraid the Master is out today, sweetie, and the other girls have more chores than normal.";
					ss << " It'll just be you and me.\" She could tell " << girlName << " was disappointed, but she crawled along after her anyway.\n";
					/**/
					ss << "" << cleanername << " cleaned the rooms, scrubbing the floor and dusting, gathering the girls' laundry for washing.";
					ss << " " << girlName << " did her best to behave, but still rolled around and looked for mischief,";
					ss << " occasionally making the girl holding her leash smile and pat her head before getting back to work. When the laundry was folded, there was still nearly an hour before dinner.";
					ss << " " << girlName << " was so good that " << cleanername << " decided to give her a treat, and let her play one of her favorite games:";
					ss << " Fetch the Dildo! " << girlName << " barked happily and would chase the dildo down, scooping it up in her mouth and bringing it back.";
					ss << " When it was back, she would growl and suck it in greedily, tugging on it back and forth.\n";
					/**/
					ss << "When " << girlName << " was worn out, both of them collapse on the ground. " << cleanername << " stroked " << girlName << "'s hair as the girl nuzzled and kissed her legs, licking as she went.";
					ss << " \"You're not so bad, you know?\" she said, and gently pressed the dildo between the pet's thighs, causing her to moan in happiness.";
					ss << " " << cleanername << " pushed the dildo back and forth, causing " << girlName << " to grind backwards, wanting the dildo deeper and faster.";
					ss << " As " << girlName << " was about to cum, the dildo stopped, causing her to whine, pleading. \"Come on girl, you know your owner hasn't given you permission.\" " << girlName << " knew she was right, but that didn't make it any easier.";
					ss << " Absently, " << cleanername << " pulled the dildo from the girl's pussy and brought it to her mouth. Obediently, " << girlName << " licked and sucked the dildo clean, and then licked her juices from the girl's fingers.";
					ss << " \"Come along now, Master is probably home for dinner.\" She stood up and used the leash to propel the pet crawling to the kitchen, excited that her Master might be home.";
					training += 2;
				}
				else if (roll_b >= 15)
				{
					ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
					/**/
					ss << "You decide to bring her with you to work and lead her through admiring patrons to your office.";
					ss << " She's restless as you go through the ledger and the tedium of running your businesses, and whines a bit until you quiet her by petting her hair and stroking her back, occasionally cupping her tits.";/*tit traits*/
					ss << " She busies herself cleaning your boots with her small tongue while you work, just happy to be of use to her Master.\n";
					/**/
					ss << "Halfway through your daily grind, you feel her nose nuzzling the front of your pants, occasionally licking, hoping for a treat.";
					ss << " When she sees her bone, " << girlName << " gives a happy yip and begins to lick your prick up and down, her pretty eyes peering at you from under your desk.";
					ss << " It becomes very difficult to concentrate with your cute pet bobbing up and down on your cock with the occasional needy whine, and even petting her hair wouldn't satisfy her.";
					ss << " Soon you forget about your work entirely as you erupt in her mouth.";
					ss << " For several minutes after she dutifully holds you in her mouth as you pet her hair, just enjoying yourself.";
					training += 2;
				}
				else
				{
					ss << "You clip a leash to " << girlName << "'s collar and she barks happily. She knows she's just your pet bitch, and she's happy to be with her master.\n";
					/**/
					ss << "\"Ah, damn, I forgot something.\" You curse, and loosely tie the leash to " << girlName << "'s collar to a tree outside the shop on the edge of town.";
					ss << " \"Stay, girl.\" " << girlName << " whined as her Master pet her hair a moment,";
					ss << " and walked back inside the shop. She panted a moment, looking after him, and then curled up on the soft grass.";
					ss << " There weren't really any people around, but there was still plenty for a puppy to see, hear,";
					ss << " and smell. " << girlName << " sniffed the air, enjoying the smell of the flowers, and listening to the birds chatter.";
					ss << " A cart drove past, pulled by a few ponygirls, with a coffel of pretty slaves trotting blindly behind it,";
					ss << " hooded. " << girlName << " barked excitedly at the moaning girls passing, but once they passed, she curled up again. Master sure was taking a while.\n";
					/**/
					ss << "Suddenly " << girlName << " noticed something. Her leash, which you had tied so hastily to the small tree, was slack. Indeed, it was laying on the ground, having come undone! " << girlName << " froze, frightened for some reason,";
					ss << " contemplating her new situation. Thoughts she'd not had in quite a while began to float through her head, and she trembled,";
					ss << " trying to get them to stop. She could speak, she could stand up, she could run away! Nothing was stopping her!";
					ss << " Memories of her life before, of friends and family, flowed unbidden through her mind, and she trembled sharply. She began to whine at herself,";
					ss << " trying to force the bad thoughts to stop and go away. As the minutes dragged on, the thoughts became more and more insistent,";
					ss << " while " << girlName << " became more and more afraid and perturbed by them. She forced herself to crawl to the edge of the grass,";
					ss << " close to the door, and lay back down, her legs bunched beneath her torso, her tits pressed hard into the ground.";
					ss << " It felt like if she could not get as low as possible, she would somehow fall up, off of solid ground.";
					ss << " The minutes continued to drag on, and she felt tears forming in her eyes, her heart pounding and feeling like it was breaking.\n";
					/**/
					ss << "Abruptly the door open. Master was back! \"Hey girl, sorry about that. Did you miss me?\" " << girlName << " barked loudly and began to whine, crawling";
					ss << " with passion to her Master and licking his shoes, pressing herself as close to the ground as she could.";
					ss << " \"Oh dear, I didn't tie your leash tight enough did I? Did you think I wasn't coming back? There there, girl.\" You crouched to one knee and she pressed herself against";
					ss << " you, cowering. You pick up the end of her leash and pet her hair, tits and chest until " << girlName << " stopped trembling.";
					ss << " Standing up, the tension was back on the leash, and the bad thoughts were gone. " << girlName << " was where she was supposed to be.";
					ss << " \"C'mon girl, let's go home. You deserve a treat, and I think " << headname << " got you some more Bitch Bites.\" " << girlName << " barked happily.";
					training += 2;
				}
			}
		else/*LOW SKILL*/
			{
				ss << "You clip a leash to " << girlName << "'s collar and she barks uneasily. She knows she's just your pet bitch, but she remembers when she wasn't. She hasn't quite embraced this life yet.\n";
				if (roll_b >= 66)
				{
					ss << girlName << " patiently waited in her kennel, still uneasy at being kept in the small cage. She was having a difficult time adjusting to her new life. New life? She guessed it was.";
					ss << " Each day seemed more humiliating than the last, as she was trained to do things she'd never dreamed of doing as a person. As a person? ";
					ss << girlName << " shuddered, realizing that she was having a hard time thinking of herself as such. She was afraid of losing herself, of becoming what she was- no, what she was being trained to be.\n";
					/**/
					ss << "But even so, there was a dark pleasure she had noticed forming in her, and that scared her most of all, even more than her Master and the girls that he let hold her leash as they walked upright. Leash. " << girlName << " shuddered again.";
					ss << " There was something about the humiliation and degradation that was freeing, that was.... she didn't know.";
					ss << " All she knew for sure was she was more wet than she had ever been in her life, and she loved it. Each shoe she licked, each dog that climbed on her, each time her Master pet her hair or unzipped his fly, her pussy tingled.";
					ss << " It tingled so badly. Each cock that slid down her throat somehow made her feel more complete, more whole, more.... just more. Leash? She hated it.";
					ss << " Then why, when she heard it jingle, did she feel such anticipation, and joy? Was she beginning to associate her humiliation with these things? " << girlName << " supposed she was. Jingle. Tingle. Jingle. Tingle.\n";
					/**/
					ss << girlName << " looked up as she got up on all fours, her pussy suddenly aching, her nipples stiffening. Master was here. How she hated him.... and loved him. She was so confused, all the time.";
					ss << " The leash was the only thing that made sense. Jingle. Tingle.";
					ss << " Master undid the lock on her kennel, and " << girlName << "'s ass began to wag unconsciously. She licked Master's hand affectionately, begrudgingly.";
					ss << " The hand holding the leash. Leash. Jingle. Tingle. Time seemed to slow for her as Master stroked under her chin, up her cheek, pushing the hair to the other side of her scalp.";
					ss << " Ever so slowly, his hand rose to meet her collar, leash in hand. An overly loud click. " << girlName << " shuddered, her eyes momentarily closed. \"Heel.\"";
					training += 2;
				}
				else if (roll_b >= 33 && g_Brothels.GetNumGirlsOnJob(0, JOB_HEADGIRL, false) >= 1)/*NEEDS MOVED TO MID LVL*/
				{
					ss << headname << " clips a leash to " << girlName << "'s collar. She is happy with her leash being held and content that she's a pet.\n";
					/**/
					ss << headname << " lead " << girlName << " into the kitchen, crawling with decent posture to her food bowl. " << girlName << " was a little confused, as Master was";
					ss << " already eating and doing paperwork";
						if (traningonduty)
						{
							ss << ", with " << traningname << " giving him his morning blowjob.";
						}
					else
						{
							ss << ".";
						}
					ss << " \"Good girl,\" " << headname << " cooed, kneeling beside " << girlName << " and stroking her hair.";
					ss << " \"Eat up!\" " << girlName << " lowered her head to her morning kibble, and felt " << headname << " gently tug her hair before she could begin.";
					ss << " \"Wait, girl, just a moment. You're doing so well, but it's time we work on your posture.\"";
					ss << " " << girlName << " looked up at the woman that was now stroking her hair and gently licked " << headname << "'s fingers.\n";
					/**/
					ss << "�A good puppy must have the right submissive posture at all times, even while eating.� " << headname << " explained.";
					ss << " She pressed " << girlName << "'s body with her hands, showing her the proper form.";
					ss << " Soon " << girlName << " had her tits pressed to the ground, just in front of her knees which were curled up beneath her.";
					ss << " " << headname << " then showed her how her paws, forearms, and elbows were to remain pressed to the ground at all times.";
					ss << " It took a few minutes, but soon " << girlName << " was able to hold the awkward posture, against the will of her screaming muscles.";
					ss << " \"Good girl,\" " << headname << " cooed again, petting " << girlName << "'s hair and back. \"Now, try to keep position.\"\n";
					/**/
					ss << "It was difficult to say the least. Each time girlName leaned in for kibble, her nipples pressed taut against the floor, and her";
					ss << " ass was pushed further in the air, baring her asshole and pussy, but with her back arching inward.";
					ss << " The posture itself gave a pleasing bouncy affect. " << headname << " stroked her back and occasionally patted " << girlName << "'s";
					ss << " ass comfortingly, pressing her fingers between her legs. Eating was a bit slower than normal,";
					ss << " though " << girlName << " tried to eat faster because the posture felt painful. Finally finished, she licked her bowl, took a few laps of";
					ss << " water, and arched her back up, pressing her cheek against " << headname << "'s breasts, hidden in her tight blouse.";
					ss << " \"Good girl, " << girlName << ".\" " << headname << " was quite pleased, petting the girl's head.";
					ss << " \"You'll get used to it with practice. Just keep at it, we wouldn't want to need to punish you, would we?\"";
					ss << " " << girlName << " barked, and a leash was attached to her collar. Her day was just beginning.";
					training += 2;
				}
				else
				{
					if (girl->oralsex() < 35)
					{
						ss << "You clip a leash to " << girlName << "'s collar and she barks uneasily. She knows she's just your pet bitch, but she remembers when she wasn't. She hasn't quite embraced this life yet.\n";
						/**/
						ss << headname << " happened by as you spent an afternoon  working out a new business strategy, " << girlName << " keeping herself entertained with your cock beneath your large, glass desk.";
						ss << " \"Problem sir?\" You sighed, clearly unhappy. \"Not really, " << headname << ".\" You reply, rubbing your temples.";
						ss << " \"" << girlName << " is coming along nicely, but her skills at sucking cock are... lackluster at best. It's not that she's not making an effort,";
						ss << " the basics just haven't clicked yet is all.\" " << headname << " smiled knowingly, watching through the glass as " << girlName << " bobbed up and down with difficulty.";
						ss << " \"I'm sure with more training she will learn to apply herself, sir. I've got just the thing that will fix this.\"\n";
						/**/
						ss << "When " << girlName << " crawled into the dining room for dinner, she was surprised to find only her bowl of kibble, no water bowl. \"Here girl!\" " << headname << " called from the";
						ss << " kitchen, \"I've got something for you!\" " << girlName << " crawled across the wooden floors past her master and the other girls and";
						ss << " real people who were sitting down to eat. " << headname << " was standing by the sink where " << girlName << " was greeted by a peculiar development.";
						ss << " On the wall near the cabinets, bellow the sink, a large flaccid dildo seemed to have been set into the wall.";
						ss << " " << headname << " knelt beside her, rubbing her stomach and swaying tits. \"From today on, you don't get a water bowl unless I decide.";
						ss << " See this cock? I've had someone rig it to mimic a real one. You have to please this cock for every little bit of water you get.\"";
						ss << " Tears welled up in " << girlName << "'s eyes but " << headname << " patted her asscheeks and cooed softly in her ear, \"Shhhh, good girl,";
						ss << " you'll learn. It's not so bad, you'll learn to love it.\" Then " << headname << " left " << girlName << " alone with the flaccid dick hanging on the wall.\n";
						/**/
						ss << "" << girlName << " gingerly put her lips around the cock. She was quite surprised to find it was warm to the touch, and pulsed gently, just like a human cock.";
						ss << " It took nearly ten frustrating minutes for her to realize she had to tease it with her tongue to bring the water";
						ss << " cock to an erect position. Once " << girlName << " finally finished running her tongue up and down the large member, tickling it's head, she finally began to suck.";
						ss << " It took another half hour, during which her thirst became quite apparent, to begin to notice little twitches in the cock indicating";
						ss << " good performance. She gradually changed her style from clumsy attempts to force the cock, often resulting in her teeth scraping it, to slow,";
						ss << " gentle sucking that slowly increased. She found that she needed to move her tongue while she sucked much more than she had thought,";
						ss << " continuing to tease the cock. " << girlName << " still had great difficulty getting the cock even a little bit into her throat, though it seemed a little";
						ss << " easier as time went on. Suddenly the cock spurted cool, crisp water down her throat, and " << girlName << " moaned. She had never imagined water could";
						ss << " taste so good, but there was so little! Was it to be like this every time? Dinner was already over, her food bowl was sure";
						ss << " to have been picked up by now. She felt the dick growing flaccid again, and quickly began licking it teasingly. She had a long, long way to go.";
						training += 2;
						girl->oralsex(2);
						imagetype = IMGTYPE_ORAL;
					}
					else
					{
						ss << girlName << " slurped heavily on her Master's cock, her head carefully bobbing so as not to graze the table. That skill had taken practice, but practice was something she got quite a bit of, these days.";
						ss << " Master mostly ignored her, drinking his evening whiskey as he went over the affairs of his household";
						if (headonduty)
							{
								ss << "with " << headname << ". ";
							}
						else
							{
								ss << ".";
							}
						ss << girlName << " kept her eyes up as she sucked, even though he didn't look down, as was proper etiquette. She hated to admit it, but she was eager for Master's cum.";
						ss << " She was always eager for Master's cum. Her meals were just bland and not filling. " << girlName << " blinked, stopping for a moment. Did she really just think that? That horrible dog food? Was she already forgetting being a person?";
						ss << " Master's hand brought her out of her revere, and she quickly resumed sucking his pulsing cock as he pet her hair. " << girlName << " marveled at the fact that his petting actually felt good, in so many ways. Why?";
						ss << " She was cut off as a blast of hot cum hit the back of her mouth mid-bob, and she locked her lips around his stiffening member, tickling it's underside with her tongue. How much better this tasted than her dog food! This was heaven.\n";
						/**/
						ss << "Something jolted " << girlName << " out of her trance. Master said her name. She wasn't looking up, she realized. " << girlName << " flicked her eyes upwards and began sucking and licking Master's waning cock again. He was smiling. \"Yes, you ~are~ a good girl. You definitely deserve this gift.\"";
						ss << " Gift? What was Master saying? What gift? " << girlName << " looked on longingly as he put away his cock, and clipped a leash to her collar. Where were they going? It didn't matter, " << girlName << " admonished herself. He held the leash.\n";
						/**/
						ss << "The bedroom, it turned out. Two of Master's bedwarmers lay wrapped in an embrace in his bed. Did she get to sleep at the foot of the bed? Was that her gift for being a good girl? " << girlName << " longed to play with Master and his bedwarmers, but he didn't lead her to the bed.";
						ss << " Instead, he led her around the side, to the doggie bed she sometimes slept in when she was good or when he just wanted her nearby.";
						ss << " Only it was a different doggie bed. It took " << girlName << " a few moments to get over her shock. HER CLOTHES. The clothes she was kidnapped in!";
						ss << " A bed made from them! Her mind reeled, both in humiliation and old memories, but also with a dark pleasure. She was torn between abject horror and sadness and humiliation, and also.... a perverse joy.";
						ss << " She edged closer. The skirt and low cut blouse that  she had last worn to school, now part of her bed. She could se the puckered cotton fabric of her old bra, and even the slight staining on her old panties, from a mixture of cum and urine from her fear at being taken.";
						ss << " \"Well, go on!\" " << girlName << " jolted back to reality with a gentle slap on her ass, realizing the leash had been unclipped. She crawled obediently to the bed, first sniffing and nuzzling it, then doing a couple turns as she crawled on top of it's fluff.";
						ss << " She plopped down, curled up like a precious puppy, her face resting between her paws, right on her old panties. Strangely she had no tears. This is what she was. \"Do you like it?\" She barked.";
						training += 2;
						girl->oralsex(1);
						imagetype = IMGTYPE_ORAL;
					}
			}
		}
	}


#pragma endregion
#pragma region //	Job Performance			//


	//enjoyed the work or not
	int roll = g_Dice.d100();
	if (roll <= 5)
	{
		ss << "Some of the girls made fun of her for been a puppy during the shift.";
		enjoy -= 1;
	}
	else if (roll <= 25)
	{
		ss << "She had a pleasant time training.";
		enjoy += 3;
	}
	else
	{
		ss << "Otherwise, the shift passed uneventfully.";
		enjoy += 1;
	}


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);


	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);
	girl->upd_Training(TRAINING_PUPPY, training);
	girl->obedience(ob);

	return false;
}
