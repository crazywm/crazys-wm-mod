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
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a Jeweler at the arena.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	double wages = 40, tips = 0;
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
	ss << "\n\n";

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
			if (girl->health() <= 0)
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


	// `J` Arena Bookmark - adding in items that can be created in the Arena
	if (craftpoints > 0)
	{
		// `J` Incomplete Craftable code - commenting out
#if 0
		ss << g_InvManager.CraftItem(girl, JOB_JEWELER, int(craftpoints));
#else

#if 0		// everything that could be made in this job
		string Name = "", Desc = "";
			Name = "Amulet of Fleshy Softness"					Desc = "Turn your girl completely human again. She'll need some time adjusting to the newer, fragile human body of hers, though. (Removes Construct, Incorporeal, Sterile. Adds Fragile)"
			Name = "Amulet of Incest"							Desc = "A magical amulet that makes the girl biologically related to whoever gives it them. (Adds Your Daughter.)"
			Name = "Amulet of Invigoration"						Desc = "This bright, simple pendant is composed of some unknown metal. It always trembles slightly, as if some energy inside it simply can't remain stagnant. When put on, this energy extends to the wearer, ensuring that they never feel tired. The strong, stimulating effect can cause some nervousness, and the lack of any sleep may reduce physical resilience. (-100 Tiredness, -10 Constitution, +Nervous)"
			Name = "Amulet of Life"								Desc = "Contender for the Most Heroes Killed award. (B) (+20 Cons, +10 Hea, +10 Conf, Int -15)"
			Name = "Amulet of Pinocchio"						Desc = "This fey-enchanted amulet has the unique ability to make any construct into a real, living, breathing, flesh and bone human being! (+50 Hap, -10 Com, -25 Cons, removes Construct)"
			Name = "Amulet of the Cunning Linguist"				Desc = "This tongue-shaped amulet instantly gives its wearer the ability to speak and understand all known languages perfectly, magnifying the effects of learning both mundane and arcane. It also has the curious side effect of making one intensely desire putting one's tongue on a woman's genitals. Odd. Strange. (+10 Mag, +10 Les, +10 Lib, gives Lesbian and Quick Learner)"
			Name = "Amulet of the Sex Elemental"				Desc = "This beautiful ruby amulet, hanging from a gold chain, holds a terrifying power. Any woman wearing it becomes an insatiable thing of pure lust, mentally and physically. Her breasts grow to an enormous size, an immense, permanently-erect penis appears between her legs so she can pleasure (and be pleasured by) anyone regardless of gender, and every moment of every day sees her mind obsessed with intense thoughts of sex. She also gains supernatural endurance, allowing her body to withstand the exertion of her new appetites. (+100 all sex skills, +100 Lib, Cons, Obed, removes Big Boobs, Lolita, Small Boobs, adds Fast Orgasms, Not Human, Nymphomaniac, Great Figure, Great Arse, Fearless, Sterile, Tough, Futanari, Mind Fucked, Long Legs, Puffy Nipples, Perky Nipples, Abnormally Large Boobs)"
			Name = "Asexual Armlet"								Desc = "This inconspicuous bracelet locks onto a girl's wrist, affecting her with powerful magic. She completely loses all sexual drive and ability but retains any natural beauty. It bestows a level of intelligence and enlightenment that allows the wearer to see great joy in virtually everything. She'll also be so attuned to the world around her that the feelings of others become easily realized. (-100 All Sex Skills, -100 Lib, +10 Int, +50 Hap, +Sterile, +Optimist, +Psychic)"
			Name = "Bauble Necklace"							Desc = "This necklace is basically some semi-precious stones on a string. It's pretty, but it's not the kind of jewelry you write home about. (D) (+5 Cha)"
			Name = "Belly Button Ring"							Desc = "Barbell ring usually worn by exotic dancers. (+3 Strip, +2 Charisma, +1 Beauty, Adds Pierced Navel)"
			Name = "Belly Chain"								Desc = "Metallic chain usually worn by exotic dancers around their waist. (+5 Strip, +3 Performance, +3 Charisma, +1 Beauty)"
			Name = "Bestial Ring"								Desc = "This rough bronze ring awakens its wearer's primal instincts, suppressing their inhibitions and intellect as it draws their base passions to the surface. (+20 Lib/Str, +50 Best, -50 Strip and Serv, +10 all other sex skills, -50 Int, removes Elegant)"
			Name = "Bimbo Ring"									Desc = "This gold-plated tin ring considerably increases its wearer's physical beauty... at the cost of making her vapid, crude, and generally not as much fun to be around. (+50 Bea, -50 Cha, -20 Serv, removes Cool Person)"
			Name = "Blonde Ring"								Desc = "A ring given by the cult of blondes. It can shoot lasers. (Combat +5, Beauty +5)"
			Name = "Bracer of Power"							Desc = "This magical bracer boosts the strength of whoever wears it. (+5 Combat, +15 Strength)"
			Name = "Clit Ring"									Desc = "Clit piercing that keeps the girl in a permanent state of arousal. (+2 Normal, +2 BDSM, +7 Hate and Fear, -5 Happiness, +10 Libido, adds Pierced Clit)"
			Name = "Cock Ring of Removal"						Desc = "A magic ring that is designed to be worn around a penis. When worn, the penis disappears and the ring along with it. (Removes Futanari)"
			Name = "Collar of Empathy"							Desc = "These ornate, locking collars were originally used voluntarily by priests committed to non-violence. It causes the wearer to shun aggression and lets them psychically share the emotions of those around them. The increased empathy also leads to greater acceptance towards the will of others. (-30 Combat, +30 Obed, +Psychic, +20 Cha, +50 Spirit, -30 Hate, -Aggressive, -Sadistic, -Merciless, -Iron Will, -Tsundere/Yandere, -Assassin, -Fearless, -Twisted)"
			Name = "Cowbell Collar"								Desc = "Moo. I hope you don't find the sound of a cowbell annoying. (+10 Best, -2 Cha, +7 Obed, -5 Happ, -15 Spi, -5 Refinement, removes Elegant)"
			Name = "Cowbell Nipple Rings of Lactation"			Desc = "Nipple rings with dangling magical cowbells that increase the girl's lactation to impressive levels. They'll also increase breast size, but only if the wearer is of below-average size. (+10 Beast, +2 BDSM, +3 Libido, +5 Titty, -5 Spirit, adds Cow Tits and Pierced Nipples, Increases breast size to Average)"
			Name = "Cross Earrings"								Desc = "Earrings with a dangling crucifix made of silver. Popular with all types of girls - except vampires. (+5 Spirit, +3 Charisma)"
			Name = "Cross Necklace"								Desc = "Necklace with a gold crucifix. One can't help but want to be good when they wear it. Unfortunately, not all sex acts are considered 'good' by most people. (+10 Spirit, +7 Charisma, +5 Morality, -20 Lesbian, -15 Group, -10 Anal/Oral)"
			Name = "Daddy's Girl Necklace"						Desc = "A small necklace with the word's Daddy's Girl on it. (Obedience +5, Adds Dependant)"
			Name = "Diamond Hairpin"							Desc = "A Diamond Hairpin. She'll feel like a million gold pieces with this, though it fortunately doesn't actually cost that much. (+5 Refinement, +10 Cha)"
			Name = "Diamond Necklace"							Desc = "Platinum necklace with inlaid diamonds, elegant and expensive. (+15 Cha/Refine, +5 Bea, +20 Love, -10 Hate)"
			Name = "Disguised Slave Band"						Desc = "It looks like a simple piece of jewelry, but the subtle interlocking geometrical designs on this beautiful silver armband barely hint at the object's true power: to suppress the wearer's ego and put her entirely under your will. (+50 Obed, adds Controlled status, -Iron Will)"
			Name = "Doll's Ring (Greater)"						Desc = "This ring is somehow a tight fit on any girl's finger, regardless of her actual size. It causes the wearer to become permanently youthful, as well as making her body like smooth, delicate, flawless porcelain. In addition, her breasts will become smaller and more perky, but her sexual ability decreases. (Age never above minimum, +10 Beauty, +Lolita, +Small Boobs, +Puffy Nipples, +Fragile, -25 Constitution, -25 Sex Skills, -50 Combat)"
			Name = "Doll's Ring (Lesser)"						Desc = "This ring makes the wearer become significantly younger, as well as giving her rather doll-like features. She becomes more fair and beautiful, but she'll also be more fragile. (-8 Age, +5 Beauty, +Lolita, +Small Boobs, -Tough, -15 Cons/Str, -30 Combat)"
			Name = "Elven Tiara"								Desc = "This mithril tiara is a proof of the mastery of elven blacksmiths and jewelcrafters. (+10 Charisma, +15 Refinement, +5 Magic)"
			Name = "Engagement Ring"							Desc = "A gold ring with a big diamond in the middle. If you give a girl this, she pretty much has to say yes. (Love +50, Beauty +10)"
			Name = "Epic Necklace of Babeosity"					Desc = "A serious refinement of the initial design. (Removes Lolita, Small Boobs, Slow Orgasms. Adds Fast Orgasms, Nymphomaniac, Great Figure, Great Arse, Long Legs, Puffy Nipples, Perky Nipples, Abnormally Large Boobs, +50 cha, beauty, libido, confidence, constitution)"
			Name = "Eridium Jewelry"							Desc = "A bracelet made of the rare element Eridium. Very rare, very valuabe, and slightly magic. (Magic +5, Love +25, Hate and Fear -15)"
			Name = "Exotic Amulet"								Desc = "This amulet gives the wearer a slight otherworldly quality. Their eyes appear unnaturally striking, and they seem more beautiful and compelling. (+10 Beauty, +Strange Eyes, +Exotic)"
			Name = "Gold Bangle"								Desc = "Handcrafted gold bangle with a beautiful decoration carved around it. (+5 Charisma, +12 Refinement)"
			Name = "Gold Pendant"								Desc = "A finely crafted heart pendant hangs on this pair of twisting gold chains. (A) (+20 Cha)"
			Name = "Gold Ring"									Desc = "A plain gold ring. Despite its price it doesn't have any magical properties. (+15 Refinement)"
			Name = "Hime Amulet"								Desc = "A magical amulet that makes your girl into a perfect little princess. (Adds Lolita, Cute, and Princess.)"
			Name = "Hime Ring"									Desc = "Removes all blemishes - physical and mental. A princess should be perfect, after all! (+10 Beauty. Removes Scars, Mind Fucked, Malformed, Retarded, Slow Learner, Twisted)"
			Name = "Hime Tiara"									Desc = "A cute little crown to complete a princess's ensemble. Teaches her to act like a perfect little girl. (+10 Beauty. Removes Yandere, Iron Will, Assassin. Adds Meek, Charming.)"
			Name = "Hooker Hoops"								Desc = "Huge hoop earrings. They are the opposite of classy but also sexy in their own special way. (+3 Cha, -1 Hap, -15 Refinement, removes Elegant)"
			Name = "Invincible Armlet "							Desc = "Fist or flame, sword or spell, the wearer of this adamantine armlet needs fear harm no longer. Time alone will take its toll. (S) (-100 Fear, adds Incorporeal)"
			Name = "Invincible Armlet"							Desc = "Fist or flame, sword or spell, the wearer of this adamantine armlet needs fear harm no longer. Time alone will take its toll. (S) (-100 Fear, adds Incorporeal)"
			Name = "Jeweled Poignard"							Desc = "A finely crafted dagger, inlaid with silver and bearing a precious stone on its hilt. It's almost jewelry. (C) (+5 Cha, +10 Com, -10 Fear, -5 Hate, +5 Conf)"
			Name = "Magic Belly-Button Piercing"				Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  The piercing and the jewelry's magic make the girl's figure seem more attractive, especially when scantily-dressed or naked. (+Great Figure and Pierced Navel, +10 Strip, +5 Fear)"
			Name = "Magic Clit Hood Piercing"					Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This piercing and the jewelry's magic keep the girl more aroused and compliant. (+Fast Orgasms, +15 Lib/Obed, +10 Fear)"
			Name = "Magic Clit Piercing"						Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This unpleasant piercing goes straight through the clitoris, and the jewelry's magic keeps the girl too sensitive and aroused to ever be fully satisfied.  (+Fast Orgasms, +Nympho, +40 Lib, +12 Fear/Hate)"
			Name = "Magic Ear-Series Piercings"					Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This makes a series of several piercings around each of a girl's earlobes and cartilidge.  Many find it sexy, but it serves little other purpose. (+Sexy Air, +3 Beauty +5 Fear)"
			Name = "Magic Facial Piercings"						Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This versatile item pierces a girl's lips, eyebrows, or nose.  While not especially useful, many find it cool and kind of sexy. (+Cool Person, +5 Cha. +5 Fear)"
			Name = "Magic Labia Piercings"						Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  This rare set of labia piercings create a magical desire for pain, while the jewelry contains many small, sharp barbs to help satisfy that desire.  (+Masochist, +Fast Orgasms, -20 Norm, +20 Libedo, +30 BDSM, +10 Fear)"
			Name = "Magic Nipple Piercings"						Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  These piercings make the girl's nipples seem perky and cute, even under clothes. The jewelry's magic also keeps her slightly aroused and anxious. (+Perky Nips, +Cute, +15 Lib, +10 BDSM, +10 Fear, -15 Conf)"
			Name = "Magic Tongue Piercing"						Desc = "Magic piercings are small balls that eject a spike, pierce the corresponding body part, and turn into solid jewelry removable only by a secret phrase.  Healing is painful, but only takes a few hours.  The piercing and the jewelry's magic will make her tongue feel more pleasing to others, but the magic makes her less able to articulate and speak elegantly. (+Good Kisser, -Elegant, +15 Norm/Les Sex, +25 Oral, +8 Fear)"
			Name = "Minor Ring of Beauty"						Desc = "This copper band smoothes your rough edges, hides small imperfections and makes you look good. (+30 Beauty)"
			Name = "Minor Ring of Charisma"						Desc = "A solid crystal band who telepathically whispers to you the right things to say and do. (+30 Charisma)"
			Name = "Minor Ring of the Horndog"					Desc = "All night, every night and they'll never know. (+30 Libido)"
			Name = "Minor Ring of the Schwarzenegger"			Desc = "Personally endorsed by the Governator of California. (+30 Str, +15 Cons)"
			Name = "Necklace of Control"						Desc = "Worn about the neck, this discreet iron chain affects its wearer's mind, suppressing her willpower and putting her under your control. (+100 Obed, -Iron Will,  adds Controlled status)"
			Name = "Necklace of Humbling"						Desc = "This unattractive necklace requires a magical key to be removed. Designed to teach humility, it makes anyone quite plain and dull. It's magic will cause even the most self-absorbed girl to get over herself and be think more of others. (+100 Serv, +25 Obed, -10 Beauty/Cha -50 Spirit/Conf, +10 Hate/Fear, Removes: Yandere, Tsundere, Iron Will, Aggressive, Merciless, Assassin, Fearless, Sadistic, Cute, Elegant, Sexy Air, Great Figure/Arse, Charismatic, Charming, Long Legs, Puffy/Perky Nipples, Big/Huge Breasts)"
			Name = "Necklace of Pain Reversal"					Desc = "This sinister steel necklace transforms the wearer's pain into pleasure. It sounds good at first, but a tendency to seek bodily harm may come with its own set of problems... (-15 Comb, -15 Cons, adds Masochist and Fearless)"
			Name = "Nipple Barbells"							Desc = "Barbell ring nipple piercings. (+2 BDSM, +2 Confidence, +1 Titty, +1 Libido, adds Pierced Nipples)"
			Name = "Nipple Chain"								Desc = "Set of nipple rings with a metallic chain that connects them. (+3 BDSM, +3 Titty, +2 Charisma, +3 Libido, adds Pierced Nipples)"
			Name = "Nipple Rings of Breast Expansion"			Desc = "Set with crystals refined from oil of breast growth, this pair of silver rings cause a woman's chest to an impressive (if still plausible) size when placed around the nipples. Naturally well-endowed woman won't see much of a change, though. (adds Big Boobs)"
			Name = "Nipple Rings of Lactation"					Desc = "This pair of nipple rings increase the girl lactation to impressive levels. (+1 BDSM, +1 Libido, adds Abundant Lactation and Pierced Nipples)"
			Name = "Nipple Rings of Perkiness"					Desc = "Special rings designed to keep the nipples of her wearer erect. (+1 BDSM, +1 Libido, adds Perky Nipples and Pierced Nipples)"
			Name = "Nipple Rings of Pillowy Softness"			Desc = "When placed on a woman's nipples, this pair of gold rings causes her breasts to expand to a prodigious size. Fortunately, they are trivial to remove, allowing their wearer to go about their daily life without repercussions. (adds Abormally Large Boobs)"
			Name = "Nipple Rings of Puffiness"					Desc = "This set of nipple rings causes a women's areolas to puff up. (+1 BDSM, +1 Libido, adds Puffy Nipples and Pierced Nipples)"
			Name = "Nipple Rings"								Desc = "Ring shaped nipple piercings. (+2 BDSM, +3 Libido and Titty, adds Pierced Nipples)"
			Name = "Nose Ring"									Desc = "A thin, silver ring that pierces the septum and hangs in between the nostrils. (+5 Best, +2 BDSM, -7 Refinement, adds Pierced nose, removes Elegant)"
			Name = "Obsidian Choker"							Desc = "This black silk choker boasts a polished obsidian medallion. Simple, but sophisticated. (B) (+10 Lib, +5 Cha, adds Elegant)"
			Name = "Pacifist's Armlet"							Desc = "The enchantment placed on this pretty blue armband slows down the wearer's movements whenever they are attempting to harm another person for any reason. It's the bane of warriors everywhere, but also an unpleasant object for anyone to wear. (+5 Cha, -60 Comb, -40 Str, -30 BDSM, -50 Hap, -10 Conf, +60 Fear, +30 Hate)"
			Name = "Pearl Necklace"								Desc = "A simple string of pearls, short enough to remain classy but long enough to be worth a lot of money. (B) (+15 Cha, +5 Lib)"
			Name = "Pink Bangle"								Desc = "Pink bangle made of plastic, frequently worn by women with no class. (+3 Charisma, -6 Refinement)"
			Name = "Prophylactic Ring"							Desc = "A common enchantment on this strange flexible ring makes it impossible for its wearer to become pregnant by any means. It can't stop non-human creatures from implanting autonomous seed in the body, however. (-10 Fear, -10 Hate, adds Sterile)"
			Name = "Rainbow Ring"								Desc = "This magical stone ring allows its wearer to change the color of their hair at will, and even to slightly alter their skin tone to fit their whims or their partners'. (+15 Beau, +5 Happ)"
			Name = "Ring of Beauty"								Desc = "Made of electrum, this band alters your body to make you stunningly attractive. (+50 Beauty)"
			Name = "Ring of Breast Reduction"					Desc = "Prized by the likes of female acrobats and warriors whose assets are occasionally a liability, this simple copper ring causes a woman's chest to shrink and flatten. (adds Small Boobs)"
			Name = "Ring of Charisma"							Desc = "This blood red band is more knowledgeable and aggressive than the minor ring in making you the life of the party. (+50 Charisma)"
			Name = "Ring of Enlightenment"						Desc = "This delicate silver ring opens the eyes of its wearer's mind, increasing their capacity for learning. (adds Quick Learner)"
			Name = "Ring of Limited Freedom (Jade)"				Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure the slave remains subservient to their master. The effects only go away if the master is the one to remove the ring. This variation causes a slave to feel increasingly unbearable anxiety if away from their master for any length of time. (-Slave, +10 Obedience, +Nervous, +Dependent)"
			Name = "Ring of Limited Freedom (Onyx)"				Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure the former slave remains subservient to their master. The effects only go away if the master is the one to remove the ring. This variation causes the wearer to grow ridiculously large breasts that both impede her bodily movement and objectify her to everyone. (-Slave, +10 Obedience, +Abnormally Large Boobs, -20 Agility)"
			Name = "Ring of Limited Freedom (Ruby)"				Desc = "These rings are designed to allow a slave to temporarily live as a free person, albeit with strong limitations. While worn, it neutralizes any slave branding or marks, but also imposes effects to ensure a former slave remains subservient to their master. This variation gives the wearer greater sexual desire, especially towards men, but also makes it extremely difficult to achieve release or orgasm. (-Slave, +10 Obedience, +60 Libido, +10 Normal/Group. -10 Lesbian. +Slow Orgasms)"
			Name = "Ring of Major Intellect"					Desc = "This ring is beautifully crafted using precious materials, and it greatly enhances the wearer's intelligence. Such enchanted items are uncommon, but can usually be found by those who can afford them. Anyone can appreciate both the beauty and practicality of such an item. (+30 Int, +10 Love)"
			Name = "Ring of Minor Intellect"					Desc = "This ring is enchanted to moderately increase the wearer's intelligence. Magic users often learn this as one of their first enchantments, which makes rings like this one readily available. (+8 Int)"
			Name = "Ring of Pleasure"							Desc = "This magical ring affects the wearer's nervous system, heightening the effects of sexual stimulation and allowing her to reach orgasm much faster than most. (adds Fast Orgasms)"
			Name = "Ring of Reliance"							Desc = "While worn, this ring causes the wearer to feel very needy towards other, especially the person that gave it to them. The effect also extends to the ring itself, ensuring that anyone who wears it will always want to have it on. Any girl will develop a significant sense of love and connection, but she will also feel very dependent and helpless without you. (+Dependent, +30 Love, -100 Confidence)"
			Name = "Ring of the Hivemind"						Desc = "This topaz ring magically links its wearer's mind with the minds of others around her, making her feel their surface emotions. The effect is too faint to work one-on-one, but it will encourage her to seek the pleasure of others in group situations. (+20 Group, +5 Strip)"
			Name = "Ring of the Horndog"						Desc = "All day and night for the rest of time and they'll never know. (+50 Libido)"
			Name = "Ring of the MILF"							Desc = "A mysterious ring that makes a woman slightly older and more mature. (Age +5. Adds Big Boobs and MILF. Removes Lolita.)"
			Name = "Ring of the Schwarzenegger"					Desc = "Only the best for the Californian Governator, This ring terminates flabby muscles!. (+50 Str, +25 Cons)"
			Name = "Silver Bracelet"							Desc = "This handcrafted bracelet is a good complement to any outfit. (+3 Charisma, +10 Refinement)"
			Name = "Silver Necklace"							Desc = "This is a slender silver neck chain. It's subtle, but definitely elegant. (C) (+10 Cha)"
			Name = "Silver Ring"								Desc = "A plain silver ring. Despite its price it doesn't have any magical properties. (+10 Refinement)"
			Name = "Slave Band"									Desc = "This thick steel armband affects the wearer's mind and makes it difficult for her to refuse what is asked of her. It has a frightening aura about it, and it is almost impossible to conceal the object's true nature. (+50 Obed, adds Controlled status)"
			Name = "Spiked Nipple Barbells"						Desc = "This set of black spiked barbell rings look dangerous and sexy at the same time. (+5 BDSM, +3 Confidence, -1 Titty, +1 Libido, +3 Spirit, adds Pierced Nipples)"
			Name = "Star Pendant"								Desc = "Star-shaped pendant with a jewel at its center. (Prevents poison and heavy poison)"
			Name = "Tongue Stud"								Desc = "Tongue piercing that will drive her customers crazy. (+2 BDSM, -5 Refinement,  -Elegant, adds Pierced Tongue)"

#endif



#endif
	}

#pragma endregion
#pragma region	//	Finish the shift			//


	// `J` - Finish the shift - Jeweler

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
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING,		(g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	g_Girls.UpdateStat(girl, STAT_AGILITY, (g_Dice % skill) - 1);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
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
