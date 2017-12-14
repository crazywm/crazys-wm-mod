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
#ifndef __CGAMESCRIPT_H
#define __CGAMESCRIPT_H

#include "cScripts.h"
#include "cGirls.h"
#include "cBrothel.h"

const int NUMVARS = 20;

class cGameScript : public cScript
{
private:
	int m_Vars[NUMVARS]; // The scripts variables
	sScript *m_CurrPos;	// the current position within the script
	bool m_Active;	// keeps track of weather a script is active
	bool m_Leave;	// keeps track of pausing script to run game loop
	int m_NestLevel;	// keeps track of what if block we are in

	// The script function prototypes
	sScript* Script_Dialog(sScript* Script);
	sScript* Script_Init(sScript *Script);
	sScript* Script_EndInit(sScript *Script);
	sScript* Script_EndScript(sScript *Script);
	sScript* Script_Restart(sScript* Script);
	sScript* Script_ChoiceBox(sScript *Script);
	sScript* Script_SetVar(sScript* Script);
	sScript* Script_SetVarRandom(sScript *Script);
	sScript* Script_IfVar(sScript* Script);
	sScript* Script_Else(sScript* Script);
	sScript* Script_EndIf(sScript* Script);
	sScript* Script_ActivateChoice(sScript *Script);
	sScript* Script_IfChoice(sScript *Script);
	sScript* Script_SetPlayerSuspision(sScript *Script);
	sScript* Script_SetPlayerDisposition(sScript *Script);
	sScript* Script_ClearGlobalFlag(sScript *Script);
	sScript* Script_AddCustToDungeon(sScript *Script);
	sScript* Script_AddRandomGirlToDungeon(sScript *Script);
	sScript* Script_SetGlobal(sScript *Script);
	sScript* Script_SetGirlFlag(sScript *Script);
	sScript* Script_AddRandomValueToGold(sScript *Script);
	sScript* Script_AddManyRandomGirlsToDungeon(sScript *Script);
	sScript* Script_AddTargetGirl(sScript *Script);
	sScript* Script_AdjustTargetGirlStat(sScript *Script);
	sScript* Script_PlayerRapeTargetGirl(sScript *Script);
	sScript* Script_GivePlayerRandomSpecialItem(sScript *Script);
	sScript* Script_IfPassSkillCheck(sScript *Script);
	sScript* Script_IfPassStatCheck(sScript *Script);
	sScript* Script_IfGirlFlag(sScript* Script);
	sScript* Script_GameOver(sScript* Script);
	sScript* Script_IfGirlStat(sScript* Script);
	sScript* Script_IfGirlSkill(sScript* Script);
	sScript* Script_IfHasTrait(sScript* Script);
	sScript* Script_TortureTarget(sScript* Script);
	sScript* Script_ScoldTarget(sScript* Script);
	sScript* Script_NormalSexTarget(sScript* Script);
	sScript* Script_BeastSexTarget(sScript* Script);
	sScript* Script_AnalSexTarget(sScript* Script);
	sScript* Script_BDSMSexTarget(sScript* Script);
	sScript* Script_IfNotDisobey(sScript* Script);
	sScript* Script_GroupSexTarget(sScript* Script);
	sScript* Script_LesbianSexTarget(sScript* Script);
	sScript* Script_OralSexTarget(sScript* Script);
	sScript* Script_StripTarget(sScript* Script);
	sScript* Script_CleanTarget(sScript* Script);
	sScript* Script_NudeTarget(sScript* Script);
	sScript* Script_MastTarget(sScript* Script);
	sScript* Script_CombatTarget(sScript* Script);
	sScript* Script_TittyTarget(sScript* Script);
	sScript* Script_DeathTarget(sScript* Script);
	sScript* Script_ProfileTarget(sScript* Script);
	sScript* Script_HandJobTarget(sScript* Script);
	sScript* Script_EcchiTarget(sScript* Script);
	sScript* Script_BunnyTarget(sScript* Script);
	sScript* Script_CardTarget(sScript* Script);
	sScript* Script_MilkTarget(sScript* Script);
	sScript* Script_WaitTarget(sScript* Script);
	sScript* Script_SingTarget(sScript* Script);
	sScript* Script_TorturePicTarget(sScript* Script);
	sScript* Script_FootTarget(sScript* Script);
	sScript* Script_BedTarget(sScript* Script);
	sScript* Script_FarmTarget(sScript* Script);
	sScript* Script_HerdTarget(sScript* Script);
	sScript* Script_CookTarget(sScript* Script);
	sScript* Script_CraftTarget(sScript* Script);
	sScript* Script_SwimTarget(sScript* Script);
	sScript* Script_BathTarget(sScript* Script);
	sScript* Script_NurseTarget(sScript* Script);
	sScript* Script_FormalTarget(sScript* Script);
	sScript* Script_AddFamilyToDungeon(sScript *Script);			// `J` new
	sScript* Script_AddTrait(sScript* Script);						// `J` new
	sScript* Script_RemoveTrait(sScript* Script);					// `J` new
	sScript* Script_AddTraitTemp(sScript* Script);					// `J` new
	sScript* Script_ShopTarget(sScript* Script);					// `J` new
	sScript* Script_MagicTarget(sScript* Script);					// `J` new
	sScript* Script_SignTarget(sScript* Script);					// `J` new
	sScript* Script_PresentedTarget(sScript* Script);				// `J` new
	sScript* Script_GetRandomGirl(sScript* Script);					// `J` new
	sScript* Script_DomTarget(sScript* Script);						// `J` new
	sScript* Script_AdjustGirlFlag(sScript *Script);				// `J` new
	sScript* Script_AdjustTraitTemp(sScript *Script);				// `J` new
	sScript* Script_AdjustTargetGirlSkill(sScript *Script);			// `J` new
	sScript* Script_DeepTarget(sScript* Script);					// `J` new
	sScript* Script_EatOutTarget(sScript* Script);					// `J` new
	sScript* Script_StrapOnTarget(sScript* Script);					// `J` new
	sScript* Script_Les69ingTarget(sScript* Script);				// `J` new
	sScript* Script_DildoTarget(sScript* Script);					// `J` new
	sScript* Script_SubTarget(sScript* Script);						// `J` new
	sScript* Script_LickTarget(sScript* Script);					// `J` new
	sScript* Script_SuckBallsTarget(sScript* Script);				// `J` new
	sScript* Script_CowGirlTarget(sScript* Script);					// `J` new
	sScript* Script_RevCowGirlTarget(sScript* Script);				// `J` new
	sScript* Script_SexDoggyTarget(sScript* Script);				// `J` new
	sScript* Script_NormalSexWithRandomTarget(sScript* Script);		// `J` new
	sScript* Script_IfGirlHasItem(sScript* Script);					// `J` new .06.02.55
	sScript* Script_AddItemtoGirl(sScript* Script);					// `J` new .06.02.55
	sScript* Script_GivePlayerItem(sScript* Script);				// `J` new .06.02.55
	sScript* Script_IfPlayerHasItem(sScript* Script);				// `J` new .06.02.57
	sScript* Script_GiveGirlInvItem(sScript* Script);				// `J` new .06.02.57
	sScript* Script_IfGirlIsSlave(sScript* Script);					// `J` new .06.02.57
	sScript* Script_IfGirlIsFree(sScript* Script);					// `J` new .06.02.57
	sScript* Script_GiveGoldToGirl(sScript* Script);				// `J` new .06.02.57
	sScript* Script_AdjustTargetGirlStatR(sScript* Script);			// `J` new .06.02.57
	sScript* Script_AdjustTargetGirlSkillR(sScript* Script);		// `J` new .06.02.57
	sScript* Script_EndIfNew(sScript* Script);						// `J` new .06.03.00
	sScript* Script_IfGirlStatus(sScript* Script);					// `J` new .06.03.00
	sScript* Script_SetGirlStatus(sScript* Script);					// `J` new .06.03.00
	sScript* Script_EndPregnancy(sScript* Script);					// `J` new .06.03.00
	sScript* Script_CreatePregnancy(sScript* Script);				// `J` new .06.03.00
	sScript* Script_ElseNew(sScript* Script);						// `J` new .06.03.01
	sScript* Script_BrandTarget(sScript* Script);					// `J` new .06.03.01 for DarkArk
	sScript* Script_RapeTarget(sScript* Script);					// `J` new .06.03.01 for DarkArk
	sScript* Script_RapeBeastTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_BirthHumanTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_BirthHumanMultipleTarget(sScript* Script);		// `J` new .06.03.01 for DarkArk
	sScript* Script_BirthBeastTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_ImpregSexTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_ImpregGroupTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_ImpregBDSMTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_ImpregBeastTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_VirginSexTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_VirginGroupTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_VirginBDSMTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_VirginBeastTarget(sScript* Script);				// `J` new .06.03.01 for DarkArk
	sScript* Script_EscortTarget(sScript* Script);					// `J` new .06.03.02 for grishnak
	sScript* Script_SportTarget(sScript* Script);					// `J` new .06.03.02 for grishnak
	sScript* Script_StudyTarget(sScript* Script);					// `J` new .06.03.02 for grishnak
	sScript* Script_TeacherTarget(sScript* Script);					// `J` new .06.03.02 for grishnak

	// `J` When modifying Scripts, search for "J-Change-Scripts"  :  found in >> cGameScript.h
	// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cGameScript.h

	// The overloaded process function
	sScript *Process(sScript *Script);

	bool IsIfStatement(int type)
	{
		if (type == 9 || type == 13 || type == 27 || type == 28 || type == 29 || type == 31 || type == 32 || type == 33 || type == 40 || type == 95 || type == 98 
			|| type == 100 || type == 101 || type == 106)
			return true;
		return false;
	}

	// script targets (things that the script will affect with certain commands)
	sGirl* m_GirlTarget;	// if not 0 then the script is affecting a girl

public:
	cGameScript()
	{
		// Clear all internal flags to false
		for(short i=0;i<NUMVARS;i++)
			m_Vars[i] = 0;
		m_CurrPos = 0;
		m_ScriptParent = 0;
		m_Active = false;
		m_Leave = false;
		m_GirlTarget = 0;
	}
	~cGameScript()
	{
		m_CurrPos = 0;
		if (m_ScriptParent) delete m_ScriptParent;
		m_ScriptParent = 0;
	}

	bool Prepare(sGirl* girlTarget)
	{
		m_Active = true;
		m_Leave = false;
		m_NestLevel = 0;

		m_GirlTarget = girlTarget;

		for(short i=0;i<NUMVARS;i++)
			m_Vars[i] = 0;

		// run the init portion of the script if it exists
		// MOD: docclox: 'twas crashing here with m_ScriptParent == 0
		// Delta's declared an interest in this area, so I've 
		// added the following test as a temp fix
		//
		// Which may not work at all, of course, since there's
		// no reliable way to test it.
		if(m_ScriptParent == 0) return true;
		if(m_ScriptParent && m_ScriptParent->m_Type == 1)
		{
			sScript* Ptr = m_ScriptParent;
			while(Ptr->m_Type != 2)
			{
				Ptr = Process(Ptr);
			}
			m_CurrPos = Ptr->m_Next;	// set the start of the script to the next entry after the init phase
		}
		else
			m_CurrPos = m_ScriptParent;	// begin at the start of the file

		return true;
	}

	bool Release()
	{
		m_Active = false;
		m_CurrPos = 0;
		if(m_ScriptParent)
			delete m_ScriptParent;
		m_ScriptParent = 0;
		m_GirlTarget = 0;
		return true;
	}

	void RunScript();	// runs the currently loaded script
	bool IsActive() {return m_Active;}
};

#endif
