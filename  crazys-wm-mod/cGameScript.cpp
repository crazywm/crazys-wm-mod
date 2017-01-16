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
#include "cGameScript.h"
#include "cMessageBox.h"
#include "cChoiceMessage.h"
#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cHouse.h"
#include "cFarm.h"
#include "GameFlags.h"
#include "cGirls.h"
#include "cInventory.h"
#include "cWindowManager.h"
#include "cGangs.h"
#include "cScriptUtils.h"
#include "cGirlGangFight.h"
#include "cGirlTorture.h"
#include "cScreenGirlDetails.h"
#include "cScreenBrothelManagement.h"
#include "cCustomers.h"

extern cCustomers g_Customers;
extern bool g_InitWin;
extern cWindowManager g_WinManager;
extern cMessageQue g_MessageQue;
extern cChoiceManager g_ChoiceManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager  g_Studios;
extern cArenaManager		g_Arena;
extern cClinicManager		g_Clinic;
extern cCentreManager		g_Centre;
extern cHouseManager		g_House;
extern cFarmManager			g_Farm;
extern cGirls g_Girls;
extern cInventory g_InvManager;
extern cScreenBrothelManagement g_BrothelManagement;
extern bool g_Cheats;
extern int g_CurrBrothel;
extern int g_TalkCount;
extern cRng g_Dice;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern cScreenGirlDetails g_GirlDetails;

extern cSurnameList g_SurnameList;
extern cPlayer* The_Player;

sScript *cGameScript::Process(sScript *Script)
{
	stringstream ss;
	ss << "Script: " << Script->m_Type;
	g_LogFile.write(ss.str());
	// Jump to function based on action type
	switch (Script->m_Type)
	{
	case 0: return Script_Dialog(Script);
	case 1: return Script_Init(Script);
	case 2: return Script_EndInit(Script);
	case 3: return Script_EndScript(Script);
	case 4: return Script_Restart(Script);
	case 5: return Script_ChoiceBox(Script);
	case 6: return Script->m_Next;	// the TEXT command only is used for adding choice box options, so shouldn't ever be executed
	case 7: return Script_SetVar(Script);
	case 8: return Script_SetVarRandom(Script);
	case 9: return Script_IfVar(Script);
	case 10: return Script_Else(Script);
	case 11: return Script_EndIf(Script);
	case 12: return Script_ActivateChoice(Script);
	case 13: return Script_IfChoice(Script);
	case 14: return Script_SetPlayerSuspision(Script);
	case 15: return Script_SetPlayerDisposition(Script);
	case 16: return Script_ClearGlobalFlag(Script);
	case 17: return Script_AddCustToDungeon(Script);
	case 18: return Script_AddRandomGirlToDungeon(Script);
	case 19: return Script_SetGlobal(Script);
	case 20: return Script_SetGirlFlag(Script);
	case 21: return Script_AddRandomValueToGold(Script);
	case 22: return Script_AddManyRandomGirlsToDungeon(Script);
	case 23: return Script_AddTargetGirl(Script);
	case 24: return Script_AdjustTargetGirlStat(Script);
	case 25: return Script_PlayerRapeTargetGirl(Script);
	case 26: return Script_GivePlayerRandomSpecialItem(Script);
	case 27: return Script_IfPassSkillCheck(Script);
	case 28: return Script_IfPassStatCheck(Script);
	case 29: return Script_IfGirlFlag(Script);
	case 30: return Script_GameOver(Script);
	case 31: return Script_IfGirlStat(Script);
	case 32: return Script_IfGirlSkill(Script);
	case 33: return Script_IfHasTrait(Script);
	case 34: return Script_TortureTarget(Script);
	case 35: return Script_ScoldTarget(Script);
	case 36: return Script_NormalSexTarget(Script);
	case 37: return Script_BeastSexTarget(Script);
	case 38: return Script_AnalSexTarget(Script);
	case 39: return Script_BDSMSexTarget(Script);
	case 40: return Script_IfNotDisobey(Script);
	case 41: return Script_GroupSexTarget(Script);
	case 42: return Script_LesbianSexTarget(Script);
	case 43: return Script_OralSexTarget(Script);
	case 44: return Script_StripTarget(Script);
	case 45: return Script_CleanTarget(Script);
	case 46: return Script_NudeTarget(Script);
	case 47: return Script_MastTarget(Script);
	case 48: return Script_CombatTarget(Script);
	case 49: return Script_TittyTarget(Script);
	case 50: return Script_DeathTarget(Script);
	case 51: return Script_ProfileTarget(Script);
	case 52: return Script_HandJobTarget(Script);
	case 53: return Script_EcchiTarget(Script);
	case 54: return Script_BunnyTarget(Script);
	case 55: return Script_CardTarget(Script);
	case 56: return Script_MilkTarget(Script);
	case 57: return Script_WaitTarget(Script);
	case 58: return Script_SingTarget(Script);
	case 59: return Script_TorturePicTarget(Script);
	case 60: return Script_FootTarget(Script);
	case 61: return Script_BedTarget(Script);
	case 62: return Script_FarmTarget(Script);
	case 63: return Script_HerdTarget(Script);
	case 64: return Script_CookTarget(Script);
	case 65: return Script_CraftTarget(Script);
	case 66: return Script_SwimTarget(Script);
	case 67: return Script_BathTarget(Script);
	case 68: return Script_NurseTarget(Script);
	case 69: return Script_FormalTarget(Script);
	case 70: return Script_AddFamilyToDungeon(Script);
	case 71: return Script_AddTrait(Script);
	case 72: return Script_RemoveTrait(Script);
	case 73: return Script_AddTraitTemp(Script);
	case 74: return Script_ShopTarget(Script);
	case 75: return Script_MagicTarget(Script);
	case 76: return Script_SignTarget(Script);
	case 77: return Script_PresentedTarget(Script);
	case 78: return Script_GetRandomGirl(Script);
	case 79: return Script_DomTarget(Script);
	case 80: return Script_AdjustGirlFlag(Script);
	case 81: return Script_AdjustTraitTemp(Script);
	case 82: return Script_AdjustTargetGirlSkill(Script);
	case 83: return Script_DeepTarget(Script);
	case 84: return Script_EatOutTarget(Script);
	case 85: return Script_StrapOnTarget(Script);
	case 86: return Script_Les69ingTarget(Script);
	case 87: return Script_DildoTarget(Script);
	case 88: return Script_SubTarget(Script);
	case 89: return Script_LickTarget(Script);
	case 90: return Script_SuckBallsTarget(Script);
	case 91: return Script_CowGirlTarget(Script);
	case 92: return Script_RevCowGirlTarget(Script);
	case 93: return Script_SexDoggyTarget(Script);
	case 94: return Script_NormalSexWithRandomTarget(Script);
	case 95: return Script_IfGirlHasItem(Script);					// `J` new .06.02.55
	case 96: return Script_AddItemtoGirl(Script);					// `J` new .06.02.55
	case 97: return Script_GivePlayerItem(Script);					// `J` new .06.02.55

		// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cGameScript.cpp

	default: return Script->m_Next;	// `J` if a script type is not found, skip it.
	}

	return 0; // Error executing
}

void cGameScript::RunScript()
{
	m_Leave = false;
	m_NestLevel = 0;
	sScript* curr = m_CurrPos;

	// Scan through script and process functions
	while (curr != 0 && !m_Leave && m_Active) curr = Process(curr);

	if (m_Active == false)
	{
		g_ChoiceManager.Free();
		Release();
	}
}
sScript *cGameScript::Script_Dialog(sScript *Script)
{
	string text = Script->m_Entries[0].m_Text;
	string replacestring = "*GIRLNAME*";
	string replacewith = (m_GirlTarget ? m_GirlTarget->m_Realname : "The Girl");
	int f = -1;
	do {
		f = text.find(replacestring);
		if (f > -1)	text.replace(f, replacestring.length(), replacewith);
	} while (f > -1);

	g_MessageQue.AddToQue(text, COLOR_BLUE);
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_Init(sScript *Script)
{
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_EndInit(sScript *Script)
{
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_EndScript(sScript *Script)
{
	m_Active = false;
	m_Leave = true;
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_ChoiceBox(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);

	g_ChoiceManager.CreateChoiceBoxResize(value[0], value[1]);
	int id = value[0];
	int num = value[1];
	int size = 0;
	Script = Script->m_Next;

	string replacestring = "*GIRLNAME*";
	string replacewith = (m_GirlTarget ? m_GirlTarget->m_Realname : "The Girl");
	for (int i = 0; i < num; i++)
	{
		string text = Script->m_Entries[0].m_Text;
		int f = -1;
		do { f = text.find(replacestring); if (f > -1)	text.replace(f, replacestring.length(), replacewith); } while (f > -1);
		int newlen = text.length();
		if (newlen > size) size = newlen;
		g_ChoiceManager.AddChoice(id, text, i);
		Script = Script->m_Next;
	}
	g_ChoiceManager.BuildChoiceBox(id, size);

	return Script; // Go to next script action
}
sScript *cGameScript::Script_Restart(sScript *Script)
{
	m_Leave = true;
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_SetVar(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);

	// Set value
	m_Vars[value[0]] = value[1];
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_SetVarRandom(sScript *Script)
{
	int num = 0;
	int value[3];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	value[2] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);
	num = (Script->m_Entries[1].m_lValue == 0 ? (g_Dice % (value[2] + 1)) + value[1] : (g_Dice % (value[2] + 1)) + value[1] - 1);

	m_Vars[value[0]] = num;
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_IfVar(sScript *Script)
{
	bool Skipping = false; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;
	int value[2];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);

	// See if variable matches second entry
	int sel = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);
	switch (sel)
	{
	case 0:		Skipping = !(m_Vars[value[0]] == value[1]);		break;
	case 1:		Skipping = !(m_Vars[value[0]] <  value[1]);		break;
	case 2:		Skipping = !(m_Vars[value[0]] <= value[1]);		break;
	case 3:		Skipping = !(m_Vars[value[0]] >  value[1]);		break;
	case 4:		Skipping = !(m_Vars[value[0]] >= value[1]);		break;
	case 5:		Skipping = !(m_Vars[value[0]] != value[1]);		break;
	}

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;
		// if else, flip skip mode
		if (Script->m_Type == 10 && Nest == m_NestLevel) Skipping = !Skipping;

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript *cGameScript::Script_Else(sScript *Script)
{
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_EndIf(sScript *Script)
{
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_ActivateChoice(sScript *Script)
{
	g_ChoiceManager.SetActive(Script->m_Entries[0].m_lValue);
	return Script->m_Next; // Go to next script action
}
sScript *cGameScript::Script_IfChoice(sScript *Script)
{
	bool Skipping; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;
	int value[2];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	// See if choice flag matches second entry
	if (g_ChoiceManager.GetChoice(value[0]) == value[1])
		Skipping = false;
	else
		Skipping = true;

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave)
			break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel)
				Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type))
				m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript *cGameScript::Script_SetPlayerSuspision(sScript *Script)
{
	int value;
	value = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);

	The_Player->suspicion(value);

	return Script->m_Next;
}
sScript *cGameScript::Script_SetPlayerDisposition(sScript *Script)
{
	int value;
	value = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);

	The_Player->disposition(value);

	return Script->m_Next;
}
sScript *cGameScript::Script_ClearGlobalFlag(sScript *Script)
{
	int value;
	value = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);

	ClearGameFlag(value);
	return Script->m_Next;
}
sScript *cGameScript::Script_AddCustToDungeon(sScript *Script)
{
	bool wife = false;
	int value[3];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_Selection] : Script->m_Entries[0].m_Selection);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	value[2] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);
	if (value[2] == 1)
		wife = true;

	if (value[0] == 0)
		g_Brothels.GetDungeon()->AddCust(DUNGEON_CUSTNOPAY, value[1], wife);
	else if (value[0] == 1)
		g_Brothels.GetDungeon()->AddCust(DUNGEON_CUSTBEATGIRL, value[1], wife);

	return Script->m_Next;
}
sScript *cGameScript::Script_AddRandomGirlToDungeon(sScript *Script)
{
	int value[7];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_Selection] : Script->m_Entries[0].m_Selection);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	value[2] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);
	value[3] = (Script->m_Entries[3].m_Var == 1 ? m_Vars[Script->m_Entries[3].m_lValue] : Script->m_Entries[3].m_lValue);
	value[4] = (Script->m_Entries[4].m_Var == 1 ? m_Vars[Script->m_Entries[4].m_lValue] : Script->m_Entries[4].m_lValue);
	value[5] = (Script->m_Entries[5].m_Var == 1 ? m_Vars[Script->m_Entries[5].m_lValue] : Script->m_Entries[5].m_lValue);
	value[6] = (Script->m_Entries[6].m_Var == 1 ? m_Vars[Script->m_Entries[6].m_lValue] : Script->m_Entries[6].m_lValue);

	bool kidnaped = false;
	int reason = 0;
	if (value[0] == 0)
	{
		kidnaped = true;
		reason = DUNGEON_GIRLKIDNAPPED;
	}
	else if (value[0] == 1)
	{
		kidnaped = true;
		reason = DUNGEON_GIRLCAPTURED;
	}

	bool slave = (value[3] == 1);
	bool allowNonHuman = (value[4] == 1);
	bool arena = (value[5] == 1);
	bool daughter = (value[6] == 1);

	// `J` reworking age
	int age = 18;
	int minage = value[1];
	int maxage = value[2];
	if (minage == 100 || maxage == 100)	age = 100;
	else
	{
		if (minage < 18) minage = 18;			// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		if (minage > 80) minage = 80;
		if (maxage < minage) maxage = minage;
		if (maxage > 80) maxage = 80;
		age = g_Dice.in_range(minage, maxage);
	}

	sGirl* newgirl = g_Girls.CreateRandomGirl(age, false, slave, false, allowNonHuman, kidnaped, arena, daughter);
	stringstream NGmsg;
	NGmsg << "(DEBUG:: " << newgirl->m_Realname << " was added by a script.\nLookup code: D_001)";
	newgirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_WARNING);

	g_Brothels.GetDungeon()->AddGirl(newgirl, reason);

	return Script->m_Next;
}
sScript *cGameScript::Script_SetGlobal(sScript *Script)
{
	SetGameFlag(Script->m_Entries[0].m_lValue, Script->m_Entries[1].m_lValue);
	return Script->m_Next;
}
sScript *cGameScript::Script_SetGirlFlag(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);

	if (m_GirlTarget == 0) return Script->m_Next;	// this shouldn't happen

	m_GirlTarget->m_Flags[value[0]] = (char)value[1];
	return Script->m_Next;
}
sScript *cGameScript::Script_AdjustGirlFlag(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);

	if (m_GirlTarget == 0) return Script->m_Next;	// this shouldn't happen

	m_GirlTarget->m_Flags[value[0]] += (char)value[1];
	return Script->m_Next;
}
sScript *cGameScript::Script_AddRandomValueToGold(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	long gold = (value[0] == 0 ? (g_Dice % (value[1] + 1)) + value[0] : (g_Dice % (value[1] + 1)) + value[0] - 1);

	g_Gold.misc_credit(gold);

	return Script->m_Next;
}
sScript *cGameScript::Script_AddManyRandomGirlsToDungeon(sScript *Script)
{
	int value[7];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);		// how many girls?
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);	// kidnaped or captured?
	value[2] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);		// min age
	value[3] = (Script->m_Entries[3].m_Var == 1 ? m_Vars[Script->m_Entries[3].m_lValue] : Script->m_Entries[3].m_lValue);		// max age
	value[4] = (Script->m_Entries[4].m_Var == 1 ? m_Vars[Script->m_Entries[4].m_lValue] : Script->m_Entries[4].m_lValue);		// slave
	value[5] = (Script->m_Entries[5].m_Var == 1 ? m_Vars[Script->m_Entries[5].m_lValue] : Script->m_Entries[5].m_lValue);		// nonhuman
	value[6] = (Script->m_Entries[6].m_Var == 1 ? m_Vars[Script->m_Entries[6].m_lValue] : Script->m_Entries[6].m_lValue);		// arena

	bool kidnaped = false;
	int reason = 0;
	if (value[1] == 0)		{ kidnaped = true; reason = DUNGEON_GIRLKIDNAPPED; }
	else if (value[1] == 1)	{ kidnaped = true; reason = DUNGEON_GIRLCAPTURED; }

	bool slave = (value[4] == 1);
	bool allowNonHuman = (value[5] == 1);
	bool arena = (value[6] == 1);

	for (int i = 0; i < value[0]; i++)
	{
		int age = (value[2] == 0 ? (g_Dice % (value[3] + 1)) + value[2] : (g_Dice % (value[3] + 1)) + value[2] - 1);

		sGirl* newgirl = g_Girls.CreateRandomGirl(age, false, slave, false, allowNonHuman, kidnaped, arena);
		stringstream NGmsg;
		NGmsg << "(DEBUG:: " << newgirl->m_Realname << " was added by a script.\nLookup code: D_002)";
		newgirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_WARNING);

		g_Brothels.GetDungeon()->AddGirl(newgirl, reason);
	}

	return Script->m_Next;
}
sScript *cGameScript::Script_AddFamilyToDungeon(sScript *Script)
{
	int value[6];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);		// how many daughters? Base age 18-25
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);	// mother taken also?
	value[2] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);		// kidnaped or captured?
	value[3] = (Script->m_Entries[3].m_Var == 1 ? m_Vars[Script->m_Entries[3].m_lValue] : Script->m_Entries[3].m_lValue);		// slave
	value[4] = (Script->m_Entries[4].m_Var == 1 ? m_Vars[Script->m_Entries[4].m_lValue] : Script->m_Entries[4].m_lValue);		// nonhuman
	value[5] = (Script->m_Entries[5].m_Var == 1 ? m_Vars[Script->m_Entries[5].m_lValue] : Script->m_Entries[5].m_lValue);		// arena

	bool kidnaped = false;
	int reason = 0;
	if (value[3] == 0)		{ kidnaped = true; reason = DUNGEON_GIRLKIDNAPPED; }
	else if (value[3] == 1)	{ kidnaped = true; reason = DUNGEON_GIRLCAPTURED; }

	bool slave = (value[4] == 1);
	bool allowNonHuman = (value[5] == 1);
	bool arena = (value[6] == 1);

	// Set the surname for the family
	string surname;
	for (int i = 0; i < 5; i++)
	{
		surname = g_SurnameList.random();
		if (i > 3) surname = surname + "-" + g_SurnameList.random();
		if (g_Girls.SurnameExists(surname)) continue;
		break;
	}

	// `J` zzzzzz - this can probably be done easier
    sGirl* Daughter1 = nullptr;
    sGirl* Daughter2 = nullptr;
    sGirl* Daughter3 = nullptr;
    sGirl* Mother = nullptr;
	stringstream NGmsg1;
	stringstream NGmsg2;
	stringstream NGmsg3;
	stringstream NGmsgM;

	int oldest = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
	if (value[0] > 0)
	{
		Daughter1 = g_Girls.CreateRandomGirl((g_Dice % 13) + 13, false, slave, false, allowNonHuman, kidnaped, arena);
		if (Daughter1->age() > oldest) oldest = Daughter1->age();
		Daughter1->m_Surname = surname;
		g_Girls.CreateRealName(Daughter1);
	}
	if (value[0] > 1)
	{
		Daughter2 = g_Girls.CreateRandomGirl((g_Dice % 13) + 13, false, slave, false, allowNonHuman, kidnaped, arena);
		if (Daughter2->age() == Daughter1->age())	// if only 2 daughters and their ages are the same, change that
		{											// if there is a third daughter, her age can be anything (to allow twins)
			if (Daughter1->age() > 20) Daughter2->age(-(g_Dice % 3 + 1));
			else Daughter2->age((g_Dice % 3 + 1));
		}
		if (Daughter2->age() > oldest) oldest = Daughter2->age();
		Daughter2->m_Surname = surname;
		g_Girls.CreateRealName(Daughter2);
	}
	if (value[0] > 2)
	{
		Daughter3 = g_Girls.CreateRandomGirl((g_Dice % 13) + 13, false, slave, false, allowNonHuman, kidnaped, arena);
		if (Daughter3->age() > oldest) oldest = Daughter3->age();
		Daughter3->m_Surname = surname;
		g_Girls.CreateRealName(Daughter3);
	}

	if (value[1])	// there is a mother
	{
		Mother = g_Girls.CreateRandomGirl((g_Dice % (50 - (oldest + 18))) + oldest + 18, false, slave, false, allowNonHuman, kidnaped, arena);	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		Mother->m_Surname = surname;
		g_Girls.CreateRealName(Mother);
		if (!g_Dice.percent(Mother->age())) g_Girls.AddTrait(Mother, "MILF");	// the younger the mother the more likely she will be a MILF
		g_Girls.LoseVirginity(Mother);

		string biography = "Daughter of " + Mother->m_Realname + " and a deadbeat brothel client.";

		if (value[0] > 0)	Daughter1->m_Desc = Daughter1->m_Desc + "\n\n" + biography;
		if (value[0] > 1)	Daughter2->m_Desc = Daughter2->m_Desc + "\n\n" + biography;
		if (value[0] > 2)	Daughter3->m_Desc = Daughter3->m_Desc + "\n\n" + biography;

		if (value[0] > 0)
		{
			Mother->m_ChildrenCount[CHILD00_TOTAL_BIRTHS] += value[0];
			Mother->m_ChildrenCount[CHILD02_ALL_GIRLS] += value[0];
			Mother->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] += value[0];
		}
	}
	string kstring = (kidnaped ? "kidnapped from her family" : "captured");

	if (value[0] > 0)
	{
		NGmsg1 << Daughter1->m_Realname << " was " << kstring;
		if (value[0] > 1 || value[1])
		{
			NGmsg1 << " along with ";
			if (value[1])					NGmsg1 << "her mother " << Mother->m_Realname;
			if (value[0] > 1 && value[1])	NGmsg1 << " and ";
			if (value[0] > 1)
			{
				NGmsg1 << "her sister" << (value[0] > 2 ? "s " : " ") << Daughter2->m_Realname;
				if (value[0] > 2)			NGmsg1 << " and " << Daughter3->m_Realname;
			}
		}
		NGmsg1 << ".";
		Daughter1->m_Events.AddMessage(NGmsg1.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
	}
	if (value[0] > 1)
	{
		NGmsg2 << Daughter2->m_Realname << " was " << kstring << " along with ";
		if (value[1]) NGmsg2 << "her mother " << Mother->m_Realname << " and ";
		NGmsg2 << "her sister" << (value[0] > 2 ? "s " : " ") << Daughter1->m_Realname;
		if (value[0] > 2) NGmsg2 << " and " << Daughter3->m_Realname;
		NGmsg2 << ".";
		Daughter2->m_Events.AddMessage(NGmsg2.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
	}
	if (value[0] > 2)
	{
		NGmsg3 << Daughter3->m_Realname << " was " << kstring << " along with ";
		if (value[1]) NGmsg3 << "her mother " << Mother->m_Realname << " and ";
		NGmsg3 << "her sisters " << Daughter1->m_Realname << " and " << Daughter2->m_Realname << ".";
		Daughter3->m_Events.AddMessage(NGmsg3.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
	}
	if (value[1])
	{
		NGmsgM << Mother->m_Realname << " was " << kstring;
		if (value[0] > 0)
		{
			NGmsgM << " along with her daughter" << (value[0] > 2 ? "s " : " ") << Daughter1->m_Realname;
			if (value[0] == 2)		NGmsgM << " and ";
			else if (value[0] == 3) NGmsgM << ", ";
			if (value[0] > 1)		NGmsgM << Daughter2->m_Realname;
			if (value[0] > 2)		NGmsgM << " and " << Daughter3->m_Realname;
		}
		NGmsgM << ".";
		Mother->m_Events.AddMessage(NGmsgM.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
	}

	if (value[0] > 0)	g_Brothels.GetDungeon()->AddGirl(Daughter1, reason);
	if (value[0] > 1)	g_Brothels.GetDungeon()->AddGirl(Daughter2, reason);
	if (value[0] > 2)	g_Brothels.GetDungeon()->AddGirl(Daughter3, reason);
	if (value[1])		g_Brothels.GetDungeon()->AddGirl(Mother, reason);

	return Script->m_Next;
}
sScript *cGameScript::Script_AddTargetGirl(sScript *Script)
{
	cScriptUtils::add_girl_to_brothel(m_GirlTarget);
	return Script->m_Next;
}
sScript *cGameScript::Script_AdjustTargetGirlStat(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_Selection] : Script->m_Entries[0].m_Selection);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	if (m_GirlTarget)
	{
		if (value[0] - (NUM_STATS) > 0)	// `J` correcting for old scripts for the new STAT_NPCLOVE
			g_Girls.UpdateSkill(m_GirlTarget, value[0] - NUM_STATS, value[1]);
		else
			g_Girls.UpdateStat(m_GirlTarget, value[0], value[1]);
	}
	return Script->m_Next;
}
sScript *cGameScript::Script_AdjustTargetGirlSkill(sScript *Script)
{
	int value[2];
	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_Selection] : Script->m_Entries[0].m_Selection);
	value[1] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	if (m_GirlTarget)
	{
			g_Girls.UpdateSkill(m_GirlTarget, value[0], value[1]);
	}
	return Script->m_Next;
}
sScript *cGameScript::Script_PlayerRapeTargetGirl(sScript *Script)
{
	if (m_GirlTarget == 0) return Script->m_Next;

	g_Girls.UpdateSkill(m_GirlTarget, SKILL_BDSM, 2);
	g_Girls.UpdateSkill(m_GirlTarget, SKILL_ANAL, 2);
	g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 2);

	g_Girls.UpdateStat(m_GirlTarget, STAT_HAPPINESS, -5);
	g_Girls.UpdateStat(m_GirlTarget, STAT_HEALTH, -10);
	g_Girls.UpdateStat(m_GirlTarget, STAT_LIBIDO, -1);
	g_Girls.UpdateStatTemp(m_GirlTarget, STAT_LIBIDO, 2, true);
	g_Girls.UpdateStat(m_GirlTarget, STAT_CONFIDENCE, -1);
	g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 2);
	g_Girls.UpdateStat(m_GirlTarget, STAT_PCFEAR, 2);
	g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -2);
	g_Girls.UpdateStat(m_GirlTarget, STAT_PCHATE, 3);

	if (g_Dice.percent(2)) g_Girls.AddTrait(m_GirlTarget, "Broken Will");

	if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status

	bool preg = !m_GirlTarget->calc_pregnancy(The_Player, false, 1.0);
	if (preg) g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten pregnant", COLOR_BLUE);
	g_GirlDetails.lastsexact = IMGTYPE_SEX;

	return Script->m_Next;
}
sScript *cGameScript::Script_GivePlayerRandomSpecialItem(sScript *Script)
{
	sInventoryItem* item = g_InvManager.GetRandomItem();
	while (item == 0)
		item = g_InvManager.GetRandomItem();

	bool ok = false;
	while (!ok)
	{
		if (item->m_Rarity >= RARITYSHOP05) ok = true;
		else
		{
			do { item = g_InvManager.GetRandomItem(); } while (item == 0);
		}
	}

	int curI = g_Brothels.HasItem(item->m_Name, -1);
	bool loop = true;
	while (loop)
	{
		if (curI != -1)
		{
			if (g_Brothels.m_NumItem[curI] >= 999)
				curI = g_Brothels.HasItem(item->m_Name, curI + 1);
			else
				loop = false;
		}
		else
			loop = false;
	}

	if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
	{
		if (curI != -1)
			g_Brothels.m_NumItem[curI]++;
		else
		{
			for (int j = 0; j<MAXNUM_INVENTORY; j++)
			{
				if (g_Brothels.m_Inventory[j] == 0)
				{
					g_Brothels.m_Inventory[j] = item;
					g_Brothels.m_EquipedItems[j] = 0;
					g_Brothels.m_NumInventory++;
					g_Brothels.m_NumItem[j]++;
					break;
				}
			}
		}
	}
	else
		g_MessageQue.AddToQue(" Your inventory is full\n", COLOR_RED);

	return Script->m_Next;
}
sScript *cGameScript::Script_IfPassSkillCheck(sScript *Script)
{
	bool Skipping; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;

	int value = Script->m_Entries[0].m_Selection;

	// See if variable matches second entry
	Skipping = !g_Dice.percent(g_Girls.GetSkill(m_GirlTarget, value));

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript *cGameScript::Script_IfPassStatCheck(sScript *Script)
{
	bool Skipping; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;

	int value = Script->m_Entries[0].m_Selection;

	// See if variable matches second entry
	Skipping = !g_Dice.percent(g_Girls.GetStat(m_GirlTarget, value));

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_IfGirlFlag(sScript* Script)
{
	bool Skipping = false; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;
	int value[2];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);

	// See if variable matches second entry
	int sel = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);
	switch (sel)
	{
	case 0:		Skipping = !(m_GirlTarget->m_Flags[value[0]] == value[1]);		break;
	case 1:		Skipping = !(m_GirlTarget->m_Flags[value[0]] <  value[1]);		break;
	case 2:		Skipping = !(m_GirlTarget->m_Flags[value[0]] <= value[1]);		break;
	case 3:		Skipping = !(m_GirlTarget->m_Flags[value[0]] >  value[1]);		break;
	case 4:		Skipping = !(m_GirlTarget->m_Flags[value[0]] >= value[1]);		break;
	case 5:		Skipping = !(m_GirlTarget->m_Flags[value[0]] != value[1]);		break;
	}

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_GameOver(sScript* Script)
{
	g_MessageQue.AddToQue("GAME OVER", COLOR_RED);
	g_WinManager.PopToWindow(&g_BrothelManagement);
	g_WinManager.Pop();
	g_InitWin = true;
	m_Active = false;
	m_Leave = true;
	return Script->m_Next;
}
sScript* cGameScript::Script_IfGirlStat(sScript* Script)
{
	bool Skipping = false; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;
	int value[2];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);

	// See if variable matches second entry
	int sel = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);
	switch (sel)
	{
	case 0:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) == value[1]);		break;
	case 1:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) <  value[1]);		break;
	case 2:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) <= value[1]);		break;
	case 3:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) >  value[1]);		break;
	case 4:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) >= value[1]);		break;
	case 5:		Skipping = !(g_Girls.GetStat(m_GirlTarget, value[0]) != value[1]);		break;
	}

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_IfGirlSkill(sScript* Script)
{
	bool Skipping = false; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;
	int value[2];

	value[0] = (Script->m_Entries[0].m_Var == 1 ? m_Vars[Script->m_Entries[0].m_lValue] : Script->m_Entries[0].m_lValue);
	value[1] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);

	// See if variable matches second entry
	int sel = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_Selection] : Script->m_Entries[1].m_Selection);
	switch (sel)
	{
	case 0:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) == value[1]);		break;
	case 1:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) < value[1]);		break;
	case 2:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) <= value[1]);		break;
	case 3:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) > value[1]);		break;
	case 4:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) >= value[1]);		break;
	case 5:		Skipping = !(g_Girls.GetSkill(m_GirlTarget, value[0]) != value[1]);		break;
	}

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_IfHasTrait(sScript* Script)
{
	bool Skipping; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;

	Skipping = !g_Girls.HasTrait(m_GirlTarget, Script->m_Entries[0].m_Text);

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_TortureTarget(sScript* Script)
{
	cGirlTorture gt(m_GirlTarget);
	g_GirlDetails.lastsexact = IMGTYPE_TORTURE;

	return Script->m_Next;
}
sScript* cGameScript::Script_ScoldTarget(sScript* Script)
{
	if (g_Girls.GetStat(m_GirlTarget, STAT_SPIRIT) <= 10)
	{
		g_MessageQue.AddToQue("She is bawling the entire time you yell at her, obviously wanting to do her best", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_HAPPINESS, -5);
		g_Girls.UpdateStat(m_GirlTarget, STAT_CONFIDENCE, -5);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 10);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -3);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -4);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCFEAR, 2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCHATE, 2);
	}
	else if (g_Girls.GetStat(m_GirlTarget, STAT_SPIRIT) <= 20)
	{
		g_MessageQue.AddToQue("She sobs a lot while you yell at her and fearfully listens to your every word", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_HAPPINESS, -2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_CONFIDENCE, -2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 6);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCFEAR, 1);
	}
	else if (g_Girls.GetStat(m_GirlTarget, STAT_SPIRIT) <= 30)
	{
		g_MessageQue.AddToQue("She listens with attention and promises to do better", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_HAPPINESS, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_CONFIDENCE, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 5);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -1);
	}
	else if (g_Girls.GetStat(m_GirlTarget, STAT_SPIRIT) <= 50)
	{
		g_MessageQue.AddToQue("She listens to what you say but barely pays attention", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 3);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCHATE, 1);
	}
	else if (g_Girls.GetStat(m_GirlTarget, STAT_SPIRIT) <= 80)
	{
		g_MessageQue.AddToQue("She looks at you defiantly while you yell at her", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, 2);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -3);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCHATE, 2);
	}
	else
	{
		g_MessageQue.AddToQue("She stares you down while you yell at her, daring you to hit her", 0);
		g_Girls.UpdateStat(m_GirlTarget, STAT_OBEDIENCE, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_SPIRIT, -1);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCLOVE, -4);
		g_Girls.UpdateStat(m_GirlTarget, STAT_PCHATE, 5);
	}

	return Script->m_Next;
}
sScript* cGameScript::Script_NormalSexTarget(sScript* Script)
{
	if (m_GirlTarget)
	{
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 2);

		if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status

		if (!m_GirlTarget->calc_pregnancy(The_Player, false, 1.0))
			g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten pregnant", 0);
	}
	g_GirlDetails.lastsexact = IMGTYPE_SEX;

	return Script->m_Next;
}
sScript* cGameScript::Script_BeastSexTarget(sScript* Script)
{
	if (m_GirlTarget)
	{
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_BEASTIALITY, 1);	// `J` divided skill gain

		if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status

		// mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
		if (g_Brothels.GetNumBeasts() > 0)
		{
			g_Girls.UpdateSkill(m_GirlTarget, SKILL_BEASTIALITY, 1);	// `J` divided skill gain
			if (!m_GirlTarget->calc_insemination(g_Girls.GetBeast(), false, 1.0))
				g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten inseminated", 0);
		}
	}
	g_GirlDetails.lastsexact = IMGTYPE_BEAST;

	return Script->m_Next;
}
sScript* cGameScript::Script_AnalSexTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ANAL, 2);
	g_GirlDetails.lastsexact = IMGTYPE_ANAL;

	return Script->m_Next;
}
sScript* cGameScript::Script_BDSMSexTarget(sScript* Script)
{
	if (m_GirlTarget)
	{
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_BDSM, 2);

		if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status
	}

	if (!m_GirlTarget->calc_pregnancy(The_Player, false, 0.75))
		g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten pregnant", 0);
	g_GirlDetails.lastsexact = IMGTYPE_BDSM;

	return Script->m_Next;
}
sScript* cGameScript::Script_IfNotDisobey(sScript* Script)
{
	bool Skipping; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;

	// See if choice flag matches second entry
	Skipping = g_Girls.DisobeyCheck(m_GirlTarget, ACTION_GENERAL, g_Brothels.GetBrothel(g_CurrBrothel));

	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached
}
sScript* cGameScript::Script_GroupSexTarget(sScript* Script)
{
	if (m_GirlTarget)
	{
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_GROUP, 2);

		if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status

		if (!m_GirlTarget->calc_group_pregnancy(The_Player, false, 1.0))
			g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten pregnant", 0);

		g_GirlDetails.lastsexact = IMGTYPE_GROUP;
	}

	return Script->m_Next;
}
sScript* cGameScript::Script_LesbianSexTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_LESBIAN, 2);
	g_GirlDetails.lastsexact = IMGTYPE_LESBIAN;
	return Script->m_Next;
}
sScript* cGameScript::Script_OralSexTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ORALSEX, 2);
	g_GirlDetails.lastsexact = IMGTYPE_ORAL;
	return Script->m_Next;
}
sScript* cGameScript::Script_StripTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_STRIP, 2);
	g_GirlDetails.lastsexact = IMGTYPE_STRIP;
	return Script->m_Next;
}
sScript* cGameScript::Script_CleanTarget(sScript* Script)
{
	sBrothel* brothel;
	brothel = g_Brothels.GetBrothel(g_CurrBrothel);

	if (m_GirlTarget)
	{
		int CleanAmt = (g_Girls.GetSkill(m_GirlTarget, SKILL_SERVICE) >= 10 ? ((g_Girls.GetSkill(m_GirlTarget, SKILL_SERVICE) / 10) + 5) * 10 : 50);
		brothel->m_Filthiness -= CleanAmt;
		stringstream sstemp;
		sstemp << "Cleanliness rating improved by " << (int)CleanAmt;
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_SERVICE, 1);
	}
	g_GirlDetails.lastsexact = IMGTYPE_MAID;

	return Script->m_Next;
}
sScript* cGameScript::Script_NudeTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_STRIP, 1);
	g_GirlDetails.lastsexact = IMGTYPE_NUDE;
	return Script->m_Next;
}
sScript* cGameScript::Script_MastTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_SERVICE, 2);
	g_GirlDetails.lastsexact = IMGTYPE_MAST;
	return Script->m_Next;
}
sScript* cGameScript::Script_CombatTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_COMBAT, 1);
	g_GirlDetails.lastsexact = IMGTYPE_COMBAT;
	return Script->m_Next;
}
sScript* cGameScript::Script_TittyTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_TITTYSEX, 2);
	g_GirlDetails.lastsexact = IMGTYPE_TITTY;
	return Script->m_Next;
}
sScript* cGameScript::Script_DeathTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_COMBAT, 0);
	g_GirlDetails.lastsexact = IMGTYPE_DEATH;
	return Script->m_Next;
}
sScript* cGameScript::Script_ProfileTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_PROFILE;
	return Script->m_Next;
}
sScript* cGameScript::Script_HandJobTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_HANDJOB, 2);
	g_GirlDetails.lastsexact = IMGTYPE_HAND;
	return Script->m_Next;
}
sScript* cGameScript::Script_EcchiTarget(sScript* Script)
{
	if (m_GirlTarget){}		//g_Girls.UpdateSkill(m_GirlTarget, SKILL_HANDJOB, 1);
	g_GirlDetails.lastsexact = IMGTYPE_ECCHI;
	return Script->m_Next;
}
sScript* cGameScript::Script_BunnyTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_PERFORMANCE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_BUNNY;
	return Script->m_Next;
}
sScript* cGameScript::Script_CardTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_PERFORMANCE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_CARD;
	return Script->m_Next;
}
sScript* cGameScript::Script_MilkTarget(sScript* Script)
{
	if (m_GirlTarget){}		//g_Girls.UpdateSkill(m_GirlTarget, SKILL_HANDJOB, 1);
	g_GirlDetails.lastsexact = IMGTYPE_MILK;
	return Script->m_Next;
}
sScript* cGameScript::Script_WaitTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_SERVICE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_WAIT;
	return Script->m_Next;
}
sScript* cGameScript::Script_SingTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_PERFORMANCE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_SING;
	return Script->m_Next;
}
sScript* cGameScript::Script_TorturePicTarget(sScript* Script)
{
	if (m_GirlTarget){}		//g_Girls.UpdateSkill(m_GirlTarget, SKILL_PERFORMANCE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_TORTURE;
	return Script->m_Next;
}
sScript* cGameScript::Script_FootTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_FOOTJOB, 1);
	g_GirlDetails.lastsexact = IMGTYPE_FOOT;
	return Script->m_Next;
}
sScript* cGameScript::Script_BedTarget(sScript* Script)
{
	if (m_GirlTarget){}		//g_Girls.UpdateSkill(m_GirlTarget, SKILL_PERFORMANCE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_BED;
	return Script->m_Next;
}
sScript* cGameScript::Script_FarmTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_FARMING, 1);
	g_GirlDetails.lastsexact = IMGTYPE_FARM;
	return Script->m_Next;
}
sScript* cGameScript::Script_HerdTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ANIMALHANDLING, 1);
	g_GirlDetails.lastsexact = IMGTYPE_HERD;
	return Script->m_Next;
}
sScript* cGameScript::Script_CookTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_COOKING, 1);
	g_GirlDetails.lastsexact = IMGTYPE_COOK;
	return Script->m_Next;
}
sScript* cGameScript::Script_CraftTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_CRAFTING, 1);
	g_GirlDetails.lastsexact = IMGTYPE_CRAFT;
	return Script->m_Next;
}
sScript* cGameScript::Script_SwimTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateStat(m_GirlTarget, STAT_STRENGTH, 1);
	g_GirlDetails.lastsexact = IMGTYPE_SWIM;
	return Script->m_Next;
}
sScript* cGameScript::Script_BathTarget(sScript* Script)
{
	if (m_GirlTarget){}		//g_Girls.UpdateSkill(m_GirlTarget, SKILL_CRAFTING, 1);
	g_GirlDetails.lastsexact = IMGTYPE_BATH;
	return Script->m_Next;
}
sScript* cGameScript::Script_NurseTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_MEDICINE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_NURSE;
	return Script->m_Next;
}
sScript* cGameScript::Script_FormalTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateStat(m_GirlTarget, STAT_REFINEMENT, 1);
	g_GirlDetails.lastsexact = IMGTYPE_FORMAL;
	return Script->m_Next;
}
sScript* cGameScript::Script_ShopTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_SHOP;
	return Script->m_Next;
}
sScript* cGameScript::Script_MagicTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_MAGIC;
	return Script->m_Next;
}
sScript* cGameScript::Script_SignTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_SIGN;
	return Script->m_Next;
}
sScript* cGameScript::Script_PresentedTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_PRESENTED;
	return Script->m_Next;
}
sScript* cGameScript::Script_DomTarget(sScript* Script)
{
	g_GirlDetails.lastsexact = IMGTYPE_DOM;
	return Script->m_Next;
}
sScript* cGameScript::Script_AddTrait(sScript* Script)						// `J` new
{
	if (m_GirlTarget && !g_Girls.HasTrait(m_GirlTarget, Script->m_Entries[0].m_Text))
			g_Girls.AddTrait(m_GirlTarget, Script->m_Entries[0].m_Text);
	return Script->m_Next;
}
sScript* cGameScript::Script_RemoveTrait(sScript* Script)					// `J` new
{
	if (m_GirlTarget && g_Girls.HasTrait(m_GirlTarget, Script->m_Entries[0].m_Text))
		g_Girls.RemoveTrait(m_GirlTarget, Script->m_Entries[0].m_Text);
	return Script->m_Next;
}
sScript* cGameScript::Script_AddTraitTemp(sScript* Script)						// `J` new
{
	if (m_GirlTarget && !g_Girls.HasTrait(m_GirlTarget, Script->m_Entries[0].m_Text))
		g_Girls.AddTrait(m_GirlTarget, Script->m_Entries[0].m_Text, Script->m_Entries[1].m_lValue);
	return Script->m_Next;
}
sScript* cGameScript::Script_AdjustTraitTemp(sScript* Script)					// `J` new
{
	if (m_GirlTarget)
	{
		g_Girls.updateTempTraits(m_GirlTarget, Script->m_Entries[0].m_Text, Script->m_Entries[1].m_lValue);
	}
	return Script->m_Next;
}
sScript* cGameScript::Script_GetRandomGirl(sScript* Script)						// `J` new
{
	m_GirlTarget = 0;
	sBrothel* brothel = 0;
	switch (Script->m_Entries[0].m_Selection)
	{
	case 1:	// brothel
		brothel = g_Brothels.GetBrothel(Script->m_Entries[1].m_Selection);
		if (brothel)
			m_GirlTarget = g_Brothels.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 2:	// studio
		brothel = g_Studios.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_Studios.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 3:	// arena
		brothel = g_Arena.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_Arena.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 4:	// center
		brothel = g_Centre.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_Centre.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 5:	// clinic
		brothel = g_Clinic.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_Clinic.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 6:	// farm
		brothel = g_Farm.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_Farm.GetGirl(brothel->m_id, g_Dice%brothel->m_NumGirls);
		break;
	case 7:	// house
		brothel = g_House.GetBrothel(0);
		if (brothel)
			m_GirlTarget = g_House.GetGirl(0, g_Dice%brothel->m_NumGirls);
		break;
	case 0:	// anywhere
	default:
	{
		int totalgirls = 0;
		for (int i = 0; i < g_Brothels.GetNumBrothels(); i++)
			totalgirls += g_Brothels.GetBrothel(i)->m_NumGirls;
		totalgirls += g_Studios.GetBrothel(0)->m_NumGirls;
		totalgirls += g_Arena.GetBrothel(0)->m_NumGirls;
		totalgirls += g_Centre.GetBrothel(0)->m_NumGirls;
		totalgirls += g_Clinic.GetBrothel(0)->m_NumGirls;
		totalgirls += g_Farm.GetBrothel(0)->m_NumGirls;
		totalgirls += g_House.GetBrothel(0)->m_NumGirls;
		int choosegirl = g_Dice%totalgirls;

		for (int i = 0; i < g_Brothels.GetNumBrothels(); i++)
		{
			if (choosegirl < g_Brothels.GetBrothel(i)->m_NumGirls)
			{
				m_GirlTarget = g_Brothels.GetGirl(i, choosegirl);
				return Script->m_Next;
			}
			else choosegirl -= g_Brothels.GetBrothel(i)->m_NumGirls;
		}
		if (choosegirl < g_Studios.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_Studios.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_Studios.GetBrothel(0)->m_NumGirls;
		if (choosegirl < g_Arena.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_Arena.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_Arena.GetBrothel(0)->m_NumGirls;
		if (choosegirl < g_Centre.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_Centre.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_Centre.GetBrothel(0)->m_NumGirls;
		if (choosegirl < g_Clinic.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_Clinic.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_Clinic.GetBrothel(0)->m_NumGirls;
		if (choosegirl < g_Farm.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_Farm.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_Farm.GetBrothel(0)->m_NumGirls;
		if (choosegirl < g_House.GetBrothel(0)->m_NumGirls)
		{
			m_GirlTarget = g_House.GetGirl(0, choosegirl);
			return Script->m_Next;
		}
		else choosegirl -= g_House.GetBrothel(0)->m_NumGirls;
	}
		break;
	}
	if (m_GirlTarget == 0)
	{
		m_Active = false;
		m_Leave = true;
	}
	return Script->m_Next;
}
sScript* cGameScript::Script_DeepTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ORALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_DEEPTHROAT;
	return Script->m_Next;
}
sScript* cGameScript::Script_EatOutTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_LESBIAN, 1);
	g_GirlDetails.lastsexact = IMGTYPE_EATOUT;
	return Script->m_Next;
}
sScript* cGameScript::Script_StrapOnTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_LESBIAN, 1);
	g_GirlDetails.lastsexact = IMGTYPE_STRAPON;
	return Script->m_Next;
}
sScript* cGameScript::Script_Les69ingTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_LESBIAN, 1);
	g_GirlDetails.lastsexact = IMGTYPE_LES69ING;
	return Script->m_Next;
}
sScript* cGameScript::Script_DildoTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_SERVICE, 1);
	g_GirlDetails.lastsexact = IMGTYPE_DILDO;
	return Script->m_Next;
}
sScript* cGameScript::Script_SubTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_BDSM, 1);
	g_GirlDetails.lastsexact = IMGTYPE_SUB;
	return Script->m_Next;
}
sScript* cGameScript::Script_LickTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ORALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_LICK;
	return Script->m_Next;
}
sScript* cGameScript::Script_SuckBallsTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_ORALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_SUCKBALLS;
	return Script->m_Next;
}
sScript* cGameScript::Script_CowGirlTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_COWGIRL;
	return Script->m_Next;
}
sScript* cGameScript::Script_RevCowGirlTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_REVCOWGIRL;
	return Script->m_Next;
}
sScript* cGameScript::Script_SexDoggyTarget(sScript* Script)
{
	if (m_GirlTarget) g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 1);
	g_GirlDetails.lastsexact = IMGTYPE_SEXDOGGY;
	return Script->m_Next;
}
sScript* cGameScript::Script_NormalSexWithRandomTarget(sScript* Script)
{
	if (m_GirlTarget)
	{
		g_Girls.UpdateSkill(m_GirlTarget, SKILL_NORMALSEX, 2);

		if (g_Girls.CheckVirginity(m_GirlTarget)) g_Girls.LoseVirginity(m_GirlTarget);	// `J` updated for trait/status
		sCustomer* Cust = new sCustomer;
		g_Customers.GetCustomer(Cust, g_Brothels.GetBrothel(g_CurrBrothel));
		if (!m_GirlTarget->calc_pregnancy(Cust, false, 1.0))
			g_MessageQue.AddToQue(m_GirlTarget->m_Realname + " has gotten pregnant", 0);
	}
	g_GirlDetails.lastsexact = IMGTYPE_SEX;

	return Script->m_Next;
}
sScript* cGameScript::Script_IfGirlHasItem(sScript* Script)					// `J` new .06.02.55
{
	bool Skipping = false; // Flag for if...then condition
	m_NestLevel++;
	int Nest = m_NestLevel;

	Skipping = g_Girls.HasItemJ(m_GirlTarget, Script->m_Entries[0].m_Text) == -1;
	// At this point, Skipping states if the script actions
	// need to be skipped due to a conditional if...then statement.
	// Actions are further processed if skipped = false, looking
	// for an else to flip the skip mode, or an endif to end
	// the conditional block.
	Script = Script->m_Next; // Go to next action to process
	while (Script != 0)
	{
		if (m_Leave) break;

		// if else, flip skip mode
		if (Script->m_Type == 10)
		{
			if (Nest == m_NestLevel) Skipping = !Skipping;
		}

		// break on end if
		if (Script->m_Type == 11)
		{
			if (Nest == m_NestLevel)
			{
				m_NestLevel--;
				return Script->m_Next;
			}
			m_NestLevel--;
		}

		// Process script function in conditional block
		// making sure to skip actions when condition not met.
		if (Skipping)
		{
			if (IsIfStatement(Script->m_Type)) m_NestLevel++;
			Script = Script->m_Next;
		}
		else
		{
			if ((Script = Process(Script)) == 0) return 0;
		}
	}
	return 0; // End of script reached

}
sScript* cGameScript::Script_AddItemtoGirl(sScript* Script)					// `J` new .06.02.55
{
	sInventoryItem* item = g_InvManager.GetItem(Script->m_Entries[0].m_Text);
	int value[2];
	value[0] = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);
	value[1] = (Script->m_Entries[2].m_Var == 1 ? m_Vars[Script->m_Entries[2].m_lValue] : Script->m_Entries[2].m_lValue);

	g_LogFile.ss() << "Debug: New script part: AddItemtoGirl  ||  Trying to add " << value[0] << " of item: " << Script->m_Entries[0].m_Text
		<< "  to girl: " << (m_GirlTarget ? m_GirlTarget->m_Name : " !NO GIRL! ")<< " | Item " << (item ? "Found" : " !NOT FOUND! ");

	if (item && m_GirlTarget && value[0] > 0)
	{
		for (int i = 0; i < value[0]; i++)
		{
			if (value[1] == 0)	g_Girls.AddInv(m_GirlTarget, item);
			else	g_InvManager.Equip(m_GirlTarget, g_Girls.AddInv(m_GirlTarget, item), false);
		}
	}
	return Script->m_Next;
}
sScript* cGameScript::Script_GivePlayerItem(sScript* Script)					// `J` new .06.02.55
{
	sInventoryItem* item = g_InvManager.GetItem(Script->m_Entries[0].m_Text);
	int amount = (Script->m_Entries[1].m_Var == 1 ? m_Vars[Script->m_Entries[1].m_lValue] : Script->m_Entries[1].m_lValue);

	g_LogFile.ss() << "Debug: New script part: GivePlayerItem  ||  Trying to add " << amount << " of item: " << Script->m_Entries[0].m_Text
		<< "  to Inventory  |  Item " << (item ? "Found" : " !NOT FOUND! ");

	if (item && amount > 0)
	{
		for (int i = 0; i < amount; i++)
		{
			g_Brothels.AddItemToInventory(item);
		}
	}
	return Script->m_Next;
}
//sScript* cGameScript::Script_GirlNameTarget(sScript* Script)
//{
//	if(m_GirlTarget)
//	{
//		sGirl* girl;
//		string girlName = girl->m_Realname;
//		stringstream sstemp;
//		sstemp << ("Im ") << girlName;
//	}
//		//g_GirlDetails.lastsexact = IMGTYPE_TORTURE;
//
//	return Script->m_Next;
//}
