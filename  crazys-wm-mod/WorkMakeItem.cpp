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

// `J` Job Farm - Producers
bool cJobManager::WorkMakeItem(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	ss << " was assigned to make items at the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	double wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//


	double jobperformance = JP_MakeItem(girl, false);
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
		tired /= 14;
		enjoy -= g_Dice % 3;
		if (roll_b < 30)	// injury
		{
			girl->health(-(1 + g_Dice % 5));
			craftpoints *= 0.8;
			if (girl->magic() > 50 && girl->mana() > 20)
			{
				girl->mana(-10 - (g_Dice % 10));
				ss << "While trying to enchant an item, the magic rebounded on her";
			}
			else
				ss << "She injured herself with the " << (g_Dice.percent(40) ? "sharp" : "heavy") << " tools";
			if (girl->health() <= 0)
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident while making items at the Farm.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}
		else	// unhappy
		{
			ss << "She did not like making things today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 20;
		craftpoints *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 17;
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


	// `J` Farm Bookmark - adding in items that can be created in the farm
	if (craftpoints > 0)
	{
		// `J` Incomplete Craftable code - commenting out
#if 0
		ss << g_InvManager.CraftItem(girl, JOB_MAKEITEM, int(craftpoints));
#else

#if 0		// everything that could be made in this job
		string Name = "", Desc = "";
			Name = "150 Piece Drum Kit"						Desc = "A 150 piece percussion set, including 360 degree ascending sky cage. (+10 Happiness, Charisma, Confidence, +5 Spirit.)"
			Name = "Anal Beads"								Desc = "Anal sex toy consisting in a series of beads attached to a piece of plastic. (+12 Anal, +2 BDSM, +10 Libido, -8 Refinement, -2 Agility)"
			Name = "Appreciation Trophy"					Desc = "Just a small trophy to show appreciation to a hard worker. (Happiness +5, Love +1)"
			Name = "Arcane Club"							Desc = "This heavy, knobbly length of petrified wood is studded with several very pointy magic crystals. Perfect for those who want a little skull-crushing with their spell-slinging, or vice-versa. (A) (+15 Comb, +15 Mag, -5 Hate, -10 Fear, +10 Conf, -5 Cha)"
			Name = "Archmage Staff"							Desc = "This deceptively simple painted wood staff is woven with such powerful enchantments that a mage of any talent can instantly detect its true nature just by touching it. It is bursting at the seams with arcane energy that almost begs to be unleashed. (A) (+30 Mag, +8 Comb, +16 Conf, -32 Fear, -16 Hate)"
			Name = "Art Easel "								Desc = "A nice easel for a girl to practice her painting. Relaxing, and who knows? She may even get famous. (+10 Intelligence, +5 love, -5 Tired, Fear, Hate. Possible increase to fame)"
			Name = "Art Easel"								Desc = "A nice easel for a girl to practice her painting. Relaxing, and who knows? She may even get famous. (+10 Intelligence, +5 love, -5 Tired, Fear, Hate. Possible increase to fame)"
			Name = "Bunny Tail Plug"						Desc = "Anal plug that ends in a fluffy bunny tail. (+15 Anal, +2 BDSM, +2 Best, +2 Con and Obed, +3 Cha, +7 Lib, -5 Agi, -2 Com, +2 Hate, -2 Hap, -5 Spirit, adds Cute)"
			Name = "Buttplug"								Desc = "A plain buttplug. It goes in the butt. Not much else to be said about it. (+15 Anal, +2 BDSM and Obed, +2 Con, +7 Lib, -8 Ref, -5 Agi, -2 Combat, +5 Hate, -5 Happy and Spirit)"
			Name = "Cat Bell Collar"						Desc = "A fancy collar with a little bell on it. It's the perfect thing for keeping an eye on your pet cat's location, and no kitty is complete without one. Or you could put it on a girl, whichever floats your boat. (+5 Best, +2 Cha, +3 Obed, -2 Spi)"
			Name = "Cat Tail Plug"							Desc = "Anal plug that ends in a cute cat tail. (+15 Anal, +2 BDSM, +7 Best, +2 Con and Obed, +3 Cha, +7 Lib, -5 Agi, -2 Com, +2 Hate, -2 Happy, -5 Spirit)"
			Name = "Combs of the Magi"						Desc = "An incredibly fancy set of combs. You'll have to sell that fancy watch of yours to pay for these! (Beauty +30, Charisma +10, adds Elegant.)"
			Name = "Compelling Buttplug"					Desc = "Unknown to the girl who receives it, this seemingly-normal buttplug will magically compel her to wear it as much as possible. As long as she has this in her possession, her interest in any type of anal sex or insertion will also increase dramatically. Between this new found interest and constantly having a plug in her ass, the girl's anal skills will quickly become very advanced. The size can sometimes make it harder to move around comfortably. (+40 Anal, +15 Libido, -5 Agi)"
			Name = "Compelling Dildo"						Desc = "Unknown to the girl to which it's given, this seemingly normal dildo will magically compel her to use it very often. It enhances her sexual abilities and also increases her natural desire. It's use won't affect a girl's virginity, because intense magical vibrations can let it do its job without actual insertion. (+30 Normal Sex, +20 Libido)"
			Name = "Computer"								Desc = "A laptop that somehow found its way here. Careful your girls don't get addicted to that internet! (Adds Nerd and Quick Learner, +10 Intelligence, -5 Constitution)"
			Name = "Cow Tail Plug"							Desc = "Big anal plug that ends in a cow tail. (+17 Anal, +3 BDSM, +7 Best, +2 Cha, +5 Con and Obed, +8 Lib, -10 Ref, -10 Agi -7 Com, +7 Hate, -2 Happiness, -10 Spirit, removes Elegant)"
			Name = "Crystal Ball"							Desc = "Magical item commonly used used by fortune tellers. (+7 Mag, +2 Int, +3 Mana)"
			Name = "Crystal Plate Armor"					Desc = "Plate armor made of enchanted crystals. It's weaker than a regular plate armor but it's lighter and doesn't interfere with spellcasting. (+5 Charisma, +10 Con, +7 Conf, -10 Fear, -10 Agi, removes Fleet of Foot)"
			Name = "Crystal Staff"							Desc = "This long wooden staff is clearly the tool of a wizard. One end bears a fist-sized translucent crystal; it serve as a focus for arcane energy, but it also adds extra power to its wielder's spells. Curiously, after a few weeks of use, the crystal attunes to its owner and glows different colors depending on their mood. (C) (+20 Mag, +6 Comb, +8 Conf, -16 Fear, -8 Hate)"
			Name = "Crystal Sword"							Desc = "A great focus for magic, but a really piece of shit for melee (+40 Magic, +10 Mana, -5 Combat)"
			Name = "Dildo"									Desc = "A plain dildo. Not much else to be said about it. (+7 Normal, +2 Les, +10 Lib)"
			Name = "Double Dildo"							Desc = "This twin-headed dildo is intended for two women who just have to use the same toy at the same time, both their movements increasing each other's pleasure. (+10 Les) (This Seller has a full stock but he charges twice as much)"
			Name = "Double Dildo"							Desc = "This twin-headed dildo is intended for two women who just have to use the same toy at the same time, both their movements increasing each other's pleasure. (+10 Les)"
			Name = "Dreidel Dildo"							Desc = "This...uh...strange...dildo does the job good and if the girl wants, she can play with it. (Oh boy...) (+25 Anal/Norm/Les/Lib, +15 Con)"
			Name = "Elegant Combs"							Desc = "A fancy set of combs for your girl's hairt. (Beauty +10. Adds Elegant)"
			Name = "Elegant Glasses"						Desc = "A fancy pair of glasses. (+10 Charisma. Adds Elegant, removes Bad Eyesight)"
			Name = "Fancy Glasses"							Desc = "A fancy pair of glasses. (+10 Charisma. Adds Nerd, removes Bad Eyesight)"
			Name = "Fancy Sybian"							Desc = "Some craftsman discovered the market for sex machines, and started making his own, bringing improvements to both performance and style. For a price, ofcourse. He claims that, once you get your girls to mount this toy, it will be hard to get them off again."
			Name = "Free Weights"							Desc = "This is a set of free weights a girl can train with during her spare time. (+10 Cons/Str/BDSM, +15 Gro)"
			Name = "Glass Cane"								Desc = "This simple wooden cane is topped with a small sphere of clear glass. It could be mistaken for a walking stick, but mages can use it to help focus and direct arcane energy. It can also deliver a good whack if need be. (D) (+15 Mag, +3 Comb, +4 Conf, -8 Fear, -4 Hate)"
			Name = "Glass Shoes"							Desc = "These extravagant shoes are a wonder of glass-blowing talent, and are a favorite of foot lovers everywhere. Somehow they remind their wearer of their hopes and dreams. (S) (+20 Hap, +20 Cha, adds Optimist)"
			Name = "Glasses of Singular Brilliance"			Desc = "These glasses look perfectly normal to anyone except the wearer, but they affect a person's vision rather uniquely. They cause complete blindness in one eye -- but at the same time, they give extraordinary vision in the other. The wearer's remaining sight shows the world as intensely more vibrant and beautiful. The glasses never come off unintentionally, and their enthralling nature keeps most people from ever removing them. However, taking them off causes the effects to fade within a few hours. (+One Eye, +Optimist)"
			Name = "Glasses"								Desc = "A cheap pair of glasses. (Adds Nerd, removes Bad Eyesight)"
			Name = "Happy Orb"								Desc = "At first glance, this pretty crystal ball filled with mesmerizing yellow mist seems to be simple decoration for a girl's bedroom. However, it is the focus of a powerful spell, a spell that ensures that she dreams only of you... and that these dreams are happy. (+10 Happiness, -1 Fear/Hate, Adds Optimist)"
			Name = "Homemade Sybian DeLuxe"					Desc = "It looks like someone spent his saturday AND sunday glueing a dildo onto a saddle. Less prone to breaking or injury. (+15 sex, +10 perf/lib, +5 anal/beast, +5 obed, +3 fear/hate, -15 conf)"
			Name = "House Rules"							Desc = "Remind your girl about the rules of the house. (Obedience +8, Spirit -1)"
			Name = "Large Anal Beads"						Desc = "Extra large anal beads for anal sex lovers. (+18 Anal, +5 BDSM, +5 Con, +2 Obed, +12 Lib, -10 Ref, -10 Agi, -5 Com, +5 Hate, -2 Happiness, -5 Spirit, removes Fleet of Foot)"
			Name = "Large Buttplug"							Desc = "Extra large buttplug for extreme anal sex enthusiasts. (+20 Anal, +5 BDSM, +5 Con and Obed, +10 Lib, -10 Ref, -15 Agi, -7 Com, +10 Hate, -7 Happiness, -10 Spirit, removes Fleet of Foot)"
			Name = "Leash"									Desc = "This length of rope attached to a leather collar isn't just an effective physical restraint: it's an humiliating punishment. In many ways, a woman wearing this is at the complete mercy of whoever is holding it. (D) (+5 BDSM, +10 Best, +20 Obed, -20 Spi, -20 Conf, -20 Comb, -30 Agi, -30 Happ, -20 Refinement, +10 Hate, +15 Fear, removes Elegant)"
			Name = "Lolita Wand (Lesser)"					Desc = "More powerful lolita wands are extremely rare, and their source and creation process is unknown. These wands were designed to create a similar effect that would be easier to replicate. Although they're still rather powerful, their creation is far less refined and produces a smaller effect. Because they lack the subtle quality of the originals, mental and physical development also gets reversed. (-12 age, +Small Boobs, +Lolita, -MILF, -10 Cons/Str, -30 All skills)"
			Name = "Lolita Wand"							Desc = "One zap from this cute pink wand will make any woman look like she never grew up. Having a second chance at life is a rare blessing, but it's best to keep this artifact away from the creepy guy next door. (-100 Age, adds Lolita and Small Boobs, removes MILF)"
			Name = "Lovers Orb"								Desc = "At first glance, this pretty crystal ball filled with mesmerizing pink mist seems to be simple decoration for a girl's bedroom. However, it is the focus of a powerful spell, a spell that ensures that she dreams only of you... and that these dreams are erotic. (+5 Love, -1 Fear/Hate)"
			Name = "Magic ball gag"							Desc = "For quiet time. This deluxe model also dulls the wearers combat senses. (+10 obedience +30 libido +10 most sex skills -all oral -charming/elegant -lots group/service/magic/combat)"
			Name = "Magic Christmas Present"				Desc = "Made by Santa himself, this present has anything the opener wants inside. Unfortunately, it makes them aggressively defend it, too. (Happiness +50, Love +50, Fear -50, Hate -50. Adds Aggressive.)"
			Name = "Magical Cowbell Collar"					Desc = "A collar with a magical cowbell that turns your girl into a cow girl. The bell originally replaced her brain with an actual cow brain, but too many people complained about it, so they changed it to just eliminate small breastedness. (+15 Best, +10 Obed, -5 Refinement, -20 Spi, +5 Con, -5 Int, adds Cow Girl, Increases breast size to Average, removes Elegant)"
			Name = "Magical Girl Staff"						Desc = "Comes complete with secret identity! However, also makes user much more attractive to tentacled things, and prone to be a tad bit reckless(+30 magic, +10 best. Adds Lolita, Fearless, Heroine, Strong Magic)"
			Name = "Magical Girl Wand"						Desc = "Magical wand made of a strange material known as plastic topped with a pink heart. It can also deliver a good whack if needed. Not to be confused with the Magical Girl Staff. (+7 Mag, +2 Combat, +2 Conf, Cha and Happiness, -8 Fear, -5 Hate)"
			Name = "Mana Crystal"							Desc = "This crystal seems to be made entirely out of flowing, swirling points of light. It can be absorbed to fully recharge one's magical energy. (+100 Mana)"
			Name = "Mystic Anal Beads"						Desc = "This toy will not only make a girl's ass better suited, but it will straighten her out as well. Even if she has the troubles....(Anal +40, Libedo +20, Happiness +10, Constitution +20, Charisma +20, +Great Arse)"
			Name = "Nightmare Orb"							Desc = "At first glance, this pretty crystal ball filled with mesmerizing purple mist seems to be simple decoration for a girl's bedroom. However, it is the focus of a powerful spell, a spell that ensures that she dreams only of you... and that these dreams are terrifying. (+10 Fear -Fearless)"
			Name = "Pet Collar"								Desc = "This human-sized pet collar with engraved brass name-tag isn't exactly the peak of elegance, but some would find it cute, and yet other would find it sexy, in the right circumstances... (B) (+10 BDSM, +30 Best, +15 Obed, -15 Spi, -5 Cha, -5 Hap, +10 Hate, removes Elegant)"
			Name = "Playco Armboy"							Desc = "An attachment that goes on your arm and makes you a good deal stronger. (+5 Cmb/Str)"
			Name = "Ponytail Plug"							Desc = "Big anal plug in the form of a pony's tail. Not the hairstyle. (+17 Anal, +3 BDSM, +7 Best, +1 Cha, +5 Con and Obed, +8 Lib, -10 Ref, -10 Agi, -7 Com, +7 Hate, -5 Happiness, -7 Spirit, removes Elegant)"
			Name = "Room Decorations"						Desc = "Some posters and stuffed animals and stuff to make your girl's room a little more upbeat. (+5 happiness)"
			Name = "Rose-tinted Glasses"					Desc = "They make the whole world look better. (B) +5 Hap/Lib/Conf/Spir/Mor, -15 Int, adds Open Minded, Optimist."
			Name = "Rune of Sexual Fixation"				Desc = "The rune carved into this small stone permanently transfers onto a girl's skin when pressed onto her. It causes her to become very focused on sex, to the point where doesn't think of much else. She'll also be much more physically desirable and attractive. (+40 all sex skills, +40 Beauty, +Nymphomaniac, +50 Libido, +Slow Learner, -45 Int, -10 Cha)"
			Name = "Rune of Sexual Obsession"				Desc = "Like a rune of fixation, the symbol carved into this small stone permanently transfers onto a girl's skin when pressed onto her. It causes her to become extremely focused on sex. Her mind is filled with little besides sheer animal lust and desire, and her body will be much more youthful and physically resilient. She'll also be extraordinarily desirable and attractive, although her personality will suffer. ( -65 Int, +Slow Learner, -10 Cha, +80 Beauty, +Nymphomaniac, +80 all sex skills, +Fast Orgasms, -20 Age, +60 Cons, +Tough, +100 Libido )"
			Name = "Rune Staff"								Desc = "This thin silver staff is inscribed with arcane symbols of power. It constantly draws energy from other planes of existence and channels it to its wielder. (B) (+25 Mag, +6 Com, +12 Conf, -24 Fear, -12 Hate)"
			Name = "Set of Combs"							Desc = "A small set of combs and brushes so your girl can keep her hair looking its best. (Beauty +3)"
			Name = "Shock Collar"							Desc = "A collar that negatively reinforces aggressive behavior with a shock. (Constitution -5, removes Aggressive)"
			Name = "Shocking Bead"							Desc = "This tiny, round object will magically attach itself inside a girl's vagina, delivering electric shocks automatically when she becomes willful or stubborn. It's designed to produce a somewhat small, corrective amount of pain -- but the very idea of the device is enough to strongly affect most girls. Additionally, it tends to exercise the vaginal muscles, making sex more enjoyable. (+50 Obed, +Nervous, +20 Fear, +30 Norm / BDSM)"
			Name = "Slippery Orb"							Desc = "Despite it's name, this tiny, magic orb is quite easy to grasp. When inserted into a girl's vagina, however, it causes her natural lubrication to increase tremendously. It doesn't increase desire in any way, but it does cause a girl to stay continuously wet, making sex better and more comfortable for both parties. The orb will work it's way into the uterus and stay there for a few weeks, but will eventually dissolve from all the increased moisture. (+25 Normal / Group Sex, -2 Fear)"
			Name = "Slut Collar"							Desc = "This pink leather collar with shiny silver studs and the word slut spelled out in golden, capital letters is an insult... and a compliment, in a special way. (+8 BDSM, +5 Best, +2 Serv, +8 Obed, +7 Conf, +5 Fame, -15 Spi, -2 Cha, -2 Hap, +10 Hate, -10 Refinement, -10 Dignity, removes Elegant and adds Slut)"
			Name = "Soul Ball"								Desc = "A bowling ball filled with the soul of the damned. Not useful for much aside from selling. (Fear +10)"
			Name = "Spiked Collar"							Desc = "This thick black leather collar is studded with shiny spikes. It looks like something you'd put on a dog, but it also looks quite intimidating. (C) (+10 BDSM, +10 Best, +2 Comb, -5 Cha)"
			Name = "Spiked Wrist"							Desc = "Chun-Li's favourite. It is a wristband that is spiked, shockingly enough. (+3 BDSM, +3 Combat, -1 Charisma, +2 Constitution)"
			Name = "Stick Hockey Game"						Desc = "A stick hockey game that you can put in a girl's room. Helps them relax and train. (Combat +2, Happiness +4, Tired -3)"
			Name = "Stick-On Beauty Mark"					Desc = "An artificial beauty mark for those who don't have one naturally. (Adds Beauty Mark, predictably.)"
			Name = "Studded Dildo"							Desc = "It isn't recorded who first thought of putting studs onto a sex toy, but this is the result of that innovation. Makes girls hornier than a regular dildo. (+2 Les, +10 Normal, +7 BDSM, +13 Lib)"
			Name = "Summoner's Rod"							Desc = "This thick iron rod is inscribed with the names of fifteen imps, magically bound in servitude to its wielder. The tiny winged devils can be summoned with a thought, and though they prefer to attack with their sharp claws and poisonous fangs, each of them is a competent spellcaster in its own right. (S) (+35 Mag, +35 Comb, +20 Conf, -40 Fear, -10 Hate)"
			Name = "Sunglasses"								Desc = "The coolest form of eyewear. (Adds Cool Person. Removes Nerd.)"
			Name = "Talon of Barren Paradise"				Desc = "When this sharply-clawed finger is inserted into a girl's vagina, strange magic causes it to start twisting around. While oddly painless, it modifies the girl's anatomy to work solely for pleasure. She'll no longer have a usable womb, but her vagina is significantly altered and enhanced. It develops a number of twists and ridges, and her nerve endings become far more exposed and sensitive. Unfortunately, greatly increased hormones may affect her in other ways.(+Sterile, +Fast Orgasms, +50 Norm, -30 Obed, -10 Con, +10 Fear/Hate)"
			Name = "Teacher Glasses"						Desc = "Glasses that look like they should be worn by a teacher. (Adds MILF and Nerd, removes Bad Eyesight)"
			Name = "The Realm of Darthon"					Desc = "A role playing game your girls can use to wind down after a long day of whoring. (-10 Tired. Adds Nerd)"
			Name = "The Russian"							Desc = "A stim machine that supposedly enhances one's strength to unimaginable levels! (+15 Str, +10 Cmb, +5 Con, -5 Agi)"
			Name = "Wooden Buckler"							Desc = "This small shields looks more like the lid of a small barrel, but it might still catch a few blows. (adds Brawler)"

#endif



#endif
	}

#pragma endregion
#pragma region	//	Finish the shift			//



	// `J` - Finish the shift - MakeItem

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
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, (g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, max(0, (g_Dice % skill) - 2));
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_MakeItem(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->crafting() +
		// secondary - second 100
		((girl->service() + girl->intelligence() + girl->magic()) / 3) +
		// level bonus
		girl->level();
		// traits modifiers

	return jobperformance;
}
