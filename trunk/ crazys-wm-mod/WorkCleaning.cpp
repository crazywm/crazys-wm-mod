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
#include "cCustomers.h"
#include "cGangs.h"
#include "cGold.h"
#include "cInventory.h"
#include "cJobManager.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cRival.h"
#include "cRng.h"
#include "cTariff.h"
#include "cTrainable.h"
#include "libintl.h"
#include "sConfig.h"
#include <sstream>
#include "cBrothel.h"


extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;

extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Brothel Job - General - job_is_cleaning
bool cJobManager::WorkCleaning(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	stringstream ss;
	cConfig cfg;

	g_Girls.UnequipCombat(girl);	// put that shit away

	int CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 10;
	int enjoy = 0;
	int wages = 0;
	int tips = 0;
	int jobperformance = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	bool playtime = false;
	int imagetype = IMGTYPE_MAID;

	ss << girlName << " worked cleaning the brothel.\n\n";

	if (roll_a <= 50 && g_Girls.DisobeyCheck(girl, ACTION_WORKCLEANING, brothel))
	{
		ss << "She refused to clean the brothel.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	if (g_Girls.HasTrait(girl, "Maid"))						CleanAmt += 20;
	if (g_Girls.HasTrait(girl, "Powerful Magic"))			CleanAmt += 10;
	if (g_Girls.HasTrait(girl, "Strong Magic"))				CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Handyman"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Waitress"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Agile"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))			CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Strong"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Assassin"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Psychic"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Manly"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Tomboy"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Optimist"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Sharp - Eyed"))				CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Giant"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Prehensile Tail"))			CleanAmt += 3;



	if (g_Girls.HasTrait(girl, "Blind"))					CleanAmt -= 20;
	if (g_Girls.HasTrait(girl, "Queen"))					CleanAmt -= 20;
	if (g_Girls.HasTrait(girl, "Princess"))					CleanAmt -= 10;
	if (g_Girls.HasTrait(girl, "Mind Fucked"))				CleanAmt -= 10;
	if (g_Girls.HasTrait(girl, "Bimbo"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Retarded"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Smoker"))					CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Clumsy"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Delicate"))					CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Elegant"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Malformed"))				CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Massive Melons"))			CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))	CleanAmt -= 3;
	if (g_Girls.HasTrait(girl, "Titanic Tits"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Broken Will"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Pessimist"))				CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Meek"))						CleanAmt -= 2;
	if (g_Girls.HasTrait(girl, "Nervous"))					CleanAmt -= 2;
	if (g_Girls.HasTrait(girl, "Dependant"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Bad Eyesight"))				CleanAmt -= 5;

	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 0.8);
		if (roll_b < 50)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else				ss << "She did not like cleaning the brothel today.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 1.1);
		if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else				ss << "She had a great time working today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt = int(CleanAmt * 0.9);
		wages = 0;
	}
	else
	{
		wages = CleanAmt; // `J` Pay her based on how much she cleaned
	}

	// `J` if she can clean more than is needed, she has a little free time after her shift
	if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
	ss << gettext("\n\nCleanliness rating improved by ") << CleanAmt;
	if (playtime)	//SIN: a bit more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so ";
		roll_a = g_Dice % 6;
		if (roll_a == 1 && brothel->m_RestrictOral) roll_a = 0;

		switch (roll_a)
		{
		case 1:
		{
			ss << "she hung out at the brothel, offering to 'clean off' finished customers with her mouth.\n";
			tips = g_Dice % 6 - 1; //how many 'tips' she clean? <6 for now, considered adjusting to amount playtime - didn't seem worth complexity
			if (tips > 0)
			{
				brothel->m_Happiness += (tips);
				g_Girls.UpdateSkill(girl, SKILL_ORALSEX, tips / 2);
				tips *= 5; //customers tip 5 gold each
				ss << "She got " << tips << " in tips for this extra service.\n";
				imagetype = IMGTYPE_ORAL;
			}
			else
			{
				ss << "No one was interested.";
			}
		}	break;

		case 2:
		{
			ss << "she had a rest.";
			g_Girls.UpdateStat(girl, STAT_TIREDNESS, -(g_Dice % 10 + 1));
		}	break;

		case 3:
		{
			ss << "she hung out around the brothel chatting with staff and patrons.\n";
			g_Girls.UpdateStat(girl, STAT_CHARISMA, (g_Dice % 3) + 1);
			g_Girls.UpdateStat(girl, STAT_CONFIDENCE, (g_Dice % 2) + 1);
		}	break;

		case 4:
		{
			ss << "she spent some time training and getting herself fitter.\n";
			g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % 2);
			g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice % 2);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, g_Dice % 2);
			g_Girls.UpdateStat(girl, STAT_SPIRIT, g_Dice % 2);
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice % 2);
		}	break;

		case 5:
		{
			if (g_Dice.percent(70))
			{
				ss << "she hung out around the brothel, watching the other girls and trying to learn tricks and techniques.\n";
				g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, g_Dice % 2);
				g_Girls.UpdateSkill(girl, SKILL_ANAL, g_Dice % 2);
				g_Girls.UpdateSkill(girl, SKILL_ORALSEX, g_Dice % 2);
				g_Girls.UpdateSkill(girl, SKILL_BDSM, g_Dice % 2);
				g_Girls.UpdateSkill(girl, SKILL_LESBIAN, g_Dice % 2);
			}
			else
			{
				tips = 20; // you tip her for cleaning you
				ss << "she came to your room and cleaned you.\n\n" << girlName << " ran you a hot bath and bathed naked with you.";
				imagetype = IMGTYPE_BATH;

				if (!brothel->m_RestrictTitty)
				{
					ss << " Taking care to clean your whole body carefully, She rubbed cleansing oils over your back and chest with her ";
					if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits"))
						ss << "enormous, heaving ";
					else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
						ss << "big, round ";
					else if (g_Girls.HasTrait(girl, "Flat Chest") || g_Girls.HasTrait(girl, "Petite Breasts") || g_Girls.HasTrait(girl, "Small Boobs"))
						ss << "cute little ";
					else ss << "nice, firm ";
					ss << "breasts. ";
				}
				if (!brothel->m_RestrictOral)
				{
					ss << "She finished by cleaning your cock with her " << (g_Girls.HasTrait(girl, "Dick-Sucking Lips") ? "amazing dick-sucking lips" : "mouth") << ". ";
					if (g_Girls.HasTrait(girl, "Cum Addict"))
					{
						ss << "She didn't stop 'cleaning' until you came in her mouth.\nAfterward, you notice her carefully "
							<< "crawling around and licking up every stray drop of cum. She must really love cleaning.\n";
						g_Girls.UpdateSkill(girl, SKILL_ORALSEX, g_Dice % 2);
						g_Girls.UpdateStat(girl, STAT_SPIRIT, -(g_Dice % 2));
						tips += (g_Dice % 20);  // tip her for hotness
					}
					imagetype = IMGTYPE_ORAL;
				}
				g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice % 5);
				g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice % 2);
				g_Girls.UpdateStat(girl, STAT_OBEDIENCE, g_Dice % 4);
				g_Girls.UpdateStat(girl, STAT_PCLOVE, g_Dice % 5);
			}
		}	break;
		
		default:
			ss << "she hung out around the brothel a bit.";
			g_Girls.UpdateTempStat(girl, STAT_LIBIDO, g_Dice % 3 + 1);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, (g_Dice % 3) + 1);
			break;
		}
	}
	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	brothel->m_Filthiness -= CleanAmt;

	girl->m_Tips = tips;  //job is classed as player-paid, so this is only way to give her tip
	girl->m_Pay = wages;

	// Improve girl
	int xp = 5, libido = 1, skill = 3;
	if (enjoy > 1)										{ xp += 1; skill += 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 2);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, (g_Dice % skill) + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoy, true);
    //lose traits
    g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", Day0Night1 == SHIFT_NIGHT);

    return false;
}
