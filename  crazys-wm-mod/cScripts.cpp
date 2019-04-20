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
#include <string>
#include "cScripts.h"
#include "CLog.h"
#include "XmlUtil.h"
#include "FileList.h"
#include "cGirls.h"


#ifdef LINUX
#include "linux.h"
#endif

extern CLog g_LogFile;
extern string stringtolower(string name);
static CLog &l = g_LogFile;

using namespace std;

bool cScript::m_script_maps_setup = false;
map<string, unsigned int> cScript::script_lookup;

const char *cScript::script_names[] =
{
	"Script_Dialog",
	"Script_Init",
	"Script_EndInit",
	"Script_EndScript",
	"Script_Restart",
	"Script_ChoiceBox",
	"Script_m_Next",
	"Script_SetVar",
	"Script_SetVarRandom",
	"Script_IfVar",
	"Script_Else",
	"Script_EndIf",
	"Script_ActivateChoice",
	"Script_IfChoice",
	"Script_SetPlayerSuspision",
	"Script_SetPlayerDisposition",
	"Script_ClearGlobalFlag",
	"Script_AddCustToDungeon",
	"Script_AddRandomGirlToDungeon",
	"Script_SetGlobal",
	"Script_SetGirlFlag",
	"Script_AddRandomValueToGold",
	"Script_AddManyRandomGirlsToDungeon",
	"Script_AddTargetGirl",
	"Script_AdjustTargetGirlStat",
	"Script_PlayerRapeTargetGirl",
	"Script_GivePlayerRandomSpecialItem",
	"Script_IfPassSkillCheck",
	"Script_IfPassStatCheck",
	"Script_IfGirlFlag",
	"Script_GameOver",
	"Script_IfGirlStat",
	"Script_IfGirlSkill",
	"Script_IfHasTrait",
	"Script_TortureTarget",
	"Script_ScoldTarget",
	"Script_NormalSexTarget",
	"Script_BeastSexTarget",
	"Script_AnalSexTarget",
	"Script_BDSMSexTarget",
	"Script_IfNotDisobey",
	"Script_GroupSexTarget",
	"Script_LesbianSexTarget",
	"Script_OralSexTarget",
	"Script_StripTarget",
	"Script_CleanTarget",
	"Script_NudeTarget",
	"Script_MastTarget",
	"Script_CombatTarget",
	"Script_TittyTarget",
	"Script_DeathTarget",
	"Script_ProfileTarget",
	"Script_HandJobTarget",
	"Script_EcchiTarget",
	"Script_BunnyTarget",
	"Script_CardTarget",
	"Script_MilkTarget",
	"Script_WaitTarget",
	"Script_SingTarget",
	"Script_TorturePicTarget",
	"Script_FootTarget",
	"Script_BedTarget",
	"Script_FarmTarget",
	"Script_HerdTarget",
	"Script_CookTarget",
	"Script_CraftTarget",
	"Script_SwimTarget",
	"Script_BathTarget",
	"Script_NurseTarget",
	"Script_FormalTarget",
	"Script_AddFamilyToDungeon",
	"Script_AddTrait",
	"Script_RemoveTrait",
	"Script_AddTraitTemp",
	"Script_ShopTarget",
	"Script_MagicTarget",
	"Script_SignTarget",
	"Script_PresentedTarget",
	"Script_GetRandomGirl",
	"Script_DomTarget",
	"Script_AdjustGirlFlag",
	"Script_AdjustTraitTemp",
	"Script_AdjustTargetGirlSkill",
	"Script_DeepTarget",
	"Script_EatOutTarget",
	"Script_StrapOnTarget",
	"Script_Les69ingTarget",
	"Script_DildoTarget",
	"Script_SubTarget",
	"Script_LickTarget",
	"Script_SuckBallsTarget",
	"Script_CowGirlTarget",
	"Script_RevCowGirlTarget",
	"Script_SexDoggyTarget",
	"Script_NormalSexWithRandomTarget",
	"Script_IfGirlHasItem",
	"Script_AddItemtoGirl",
	"Script_GivePlayerItem",
	"Script_IfPlayerHasItem",
	"Script_GiveGirlInvItem",
	"Script_IfGirlIsSlave",
	"Script_IfGirlIsFree",
	"Script_GiveGoldToGirl",
	"Script_AdjustTargetGirlStatR",
	"Script_AdjustTargetGirlSkillR",
	"Script_EndIfNew",
	"Script_IfGirlStatus",
	"Script_SetGirlStatus",
	"Script_EndPregnancy",
	"Script_CreatePregnancy",
	"Script_ElseNew",
	"Script_BrandTarget",
	"Script_RapeTarget",
	"Script_RapeBeastTarget",
	"Script_BirthHumanTarget",
	"Script_BirthHumanMultipleTarget",
	"Script_BirthBeastTarget",
	"Script_ImpregSexTarget",
	"Script_ImpregGroupTarget",
	"Script_ImpregBDSMTarget",
	"Script_ImpregBeastTarget",
	"Script_VirginSexTarget",
	"Script_VirginGroupTarget",
	"Script_VirginBDSMTarget",
	"Script_VirginBeastTarget",
	"Script_EscortTarget",
	"Script_SportTarget",
	"Script_StudyTarget",
	"Script_TeacherTarget"
	
	// `J` When modifying Scripts, search for "J-Change-Scripts"  :  found in >> cScripts.cpp > script_names[]
};
const char *cScript::script_entry_types[] =
{
	"_NONE", "_TEXT", "_BOOL", "_INT", "_FLOAT", "_CHOICE"
};
const char *cScript::script_compare_types[] =
{
	"e", "l", "le", "g", "ge", "ne"
};

void cScript::setup_maps()
{
	g_LogFile.os() << "[cScript::setup_maps] Setting up script codes." << endl;
	m_script_maps_setup = true;
	script_lookup["script_dialog"] = 0;
	script_lookup["script_init"] = 1;
	script_lookup["script_endinit"] = 2;
	script_lookup["script_endscript"] = 3;
	script_lookup["script_restart"] = 4;
	script_lookup["script_choicebox"] = 5;
	script_lookup["script_m_next"] = 6;
	script_lookup["script_setvar"] = 7;
	script_lookup["script_setvarrandom"] = 8;
	script_lookup["script_ifvar"] = 9;
	script_lookup["script_else"] = 10;
	script_lookup["script_endif"] = 11;
	script_lookup["script_activatechoice"] = 12;
	script_lookup["script_ifchoice"] = 13;
	script_lookup["script_setplayersuspision"] = 14;
	script_lookup["script_setplayerdisposition"] = 15;
	script_lookup["script_clearglobalflag"] = 16;
	script_lookup["script_addcusttodungeon"] = 17;
	script_lookup["script_addrandomgirltodungeon"] = 18;
	script_lookup["script_setglobal"] = 19;
	script_lookup["script_setgirlflag"] = 20;
	script_lookup["script_addrandomvaluetogold"] = 21;
	script_lookup["script_addmanyrandomgirlstodungeon"] = 22;
	script_lookup["script_addtargetgirl"] = 23;
	script_lookup["script_adjusttargetgirlstat"] = 24;
	script_lookup["script_playerrapetargetgirl"] = 25;
	script_lookup["script_giveplayerrandomspecialitem"] = 26;
	script_lookup["script_ifpassskillcheck"] = 27;
	script_lookup["script_ifpassstatcheck"] = 28;
	script_lookup["script_ifgirlflag"] = 29;
	script_lookup["script_gameover"] = 30;
	script_lookup["script_ifgirlstat"] = 31;
	script_lookup["script_ifgirlskill"] = 32;
	script_lookup["script_ifhastrait"] = 33;
	script_lookup["script_torturetarget"] = 34;
	script_lookup["script_scoldtarget"] = 35;
	script_lookup["script_normalsextarget"] = 36;
	script_lookup["script_beastsextarget"] = 37;
	script_lookup["script_analsextarget"] = 38;
	script_lookup["script_bdsmsextarget"] = 39;
	script_lookup["script_ifnotdisobey"] = 40;
	script_lookup["script_groupsextarget"] = 41;
	script_lookup["script_lesbiansextarget"] = 42;
	script_lookup["script_oralsextarget"] = 43;
	script_lookup["script_striptarget"] = 44;
	script_lookup["script_cleantarget"] = 45;
	script_lookup["script_nudetarget"] = 46;
	script_lookup["script_masttarget"] = 47;
	script_lookup["script_combattarget"] = 48;
	script_lookup["script_tittytarget"] = 49;
	script_lookup["script_deathtarget"] = 50;
	script_lookup["script_profiletarget"] = 51;
	script_lookup["script_handjobtarget"] = 52;
	script_lookup["script_ecchitarget"] = 53;
	script_lookup["script_bunnytarget"] = 54;
	script_lookup["script_cardtarget"] = 55;
	script_lookup["script_milktarget"] = 56;
	script_lookup["script_waittarget"] = 57;
	script_lookup["script_singtarget"] = 58;
	script_lookup["script_torturepictarget"] = 59;
	script_lookup["script_foottarget"] = 60;
	script_lookup["script_bedtarget"] = 61;
	script_lookup["script_farmtarget"] = 62;
	script_lookup["script_herdtarget"] = 63;
	script_lookup["script_cooktarget"] = 64;
	script_lookup["script_crafttarget"] = 65;
	script_lookup["script_swimtarget"] = 66;
	script_lookup["script_bathtarget"] = 67;
	script_lookup["script_nursetarget"] = 68;
	script_lookup["script_formaltarget"] = 69;
	script_lookup["script_addfamilytodungeon"] = 70;
	script_lookup["script_addtrait"] = 71;
	script_lookup["script_removetrait"] = 72;
	script_lookup["script_addtraittemp"] = 73;
	script_lookup["script_shoptarget"] = 74;
	script_lookup["script_magictarget"] = 75;
	script_lookup["script_signtarget"] = 76;
	script_lookup["script_presentedtarget"] = 77;
	script_lookup["script_getrandomgirl"] = 78;
	script_lookup["script_domtarget"] = 79;
	script_lookup["script_adjustgirlflag"] = 80;
	script_lookup["script_adjusttraittemp"] = 81;
	script_lookup["script_adjusttargetgirlskill"] = 82;
	script_lookup["script_deeptarget"] = 83;
	script_lookup["script_eatouttarget"] = 84;
	script_lookup["script_strapontarget"] = 85;
	script_lookup["script_les69ingtarget"] = 86;
	script_lookup["script_dildotarget"] = 87;
	script_lookup["script_subtarget"] = 88;
	script_lookup["script_licktarget"] = 89;
	script_lookup["script_suckballstarget"] = 90;
	script_lookup["script_cowgirltarget"] = 91;
	script_lookup["script_revcowgirltarget"] = 92;
	script_lookup["script_sexdoggytarget"] = 93;
	script_lookup["script_normalsexwithrandomtarget"] = 94;
	script_lookup["script_ifgirlhasitem"] = 95;
	script_lookup["script_additemtogirl"] = 96;
	script_lookup["script_giveplayeritem"] = 97;
	script_lookup["script_ifplayerhasitem"] = 98;
	script_lookup["script_givegirlinvitem"] = 99;
	script_lookup["script_ifgirlisslave"] = 100;
	script_lookup["script_ifgirlisfree"] = 101;
	script_lookup["script_givegoldtogirl"] = 102;
	script_lookup["script_adjusttargetgirlstatr"] = 103;
	script_lookup["script_adjusttargetgirlskillr"] = 104;
	script_lookup["Script_EndIfNew"] = 105;
	script_lookup["Script_IfGirlStatus"] = 106;
	script_lookup["Script_SetGirlStatus"] = 107;
	script_lookup["Script_EndPregnancy"] = 108;
	script_lookup["Script_CreatePregnancy"] = 109;
	script_lookup["Script_ElseNew"] = 110;
	script_lookup["Script_BrandTarget"] = 111;
	script_lookup["Script_RapeTarget"] = 112;
	script_lookup["Script_RapeBeastTarget"] = 113;
	script_lookup["Script_BirthHumanTarget"] = 114;
	script_lookup["Script_BirthHumanMultipleTarget"] = 115;
	script_lookup["Script_BirthBeastTarget"] = 116;
	script_lookup["Script_ImpregSexTarget"] = 117;
	script_lookup["Script_ImpregGroupTarget"] = 118;
	script_lookup["Script_ImpregBDSMTarget"] = 119;
	script_lookup["Script_ImpregBeastTarget"] = 120;
	script_lookup["Script_VirginSexTarget"] = 121;
	script_lookup["Script_VirginGroupTarget"] = 122;
	script_lookup["Script_VirginBDSMTarget"] = 123;
	script_lookup["Script_VirginBeastTarget"] = 124;
	script_lookup["Script_EscortTarget"] = 125;
	script_lookup["Script_SportTarget"] = 126;
	script_lookup["Script_StudyTarget"] = 127;
	script_lookup["Script_TeacherTarget"] = 128;

	// `J` When modifying Scripts, search for "J-Change-Scripts"  :  found in >> cScripts.cpp > setup_maps()

	script_lookup["_none"] = 0;
	script_lookup["_text"] = 1;
	script_lookup["_bool"] = 2;
	script_lookup["_int"] = 3;
	script_lookup["_float"] = 4;
	script_lookup["_choice"] = 5;

	script_lookup["e"] = 0;
	script_lookup["l"] = 1;
	script_lookup["le"] = 2;
	script_lookup["g"] = 3;
	script_lookup["ge"] = 4;
	script_lookup["ne"] = 5;

	script_lookup["No"] = 0;
	script_lookup["no"] = 0;
	script_lookup["NO"] = 0;
	script_lookup["Yes"] = 1;
	script_lookup["yes"] = 1;
	script_lookup["YES"] = 1;


}

bool cActionTemplate::GetNextQuotedLine(char *Data, FILE *fp, long MaxSize)
{
	int c;
	long Pos = 0;

	// Read until a quote is reached (or EOF)
	while (1)
	{
		if ((c = fgetc(fp)) == EOF)
			return false;

		if (c == '"')
		{
			// Read until next quote (or EOF)
			while (1)
			{
				if ((c = fgetc(fp)) == EOF)
					return false;

				// Return text when 2nd quote found
				if (c == '"')
				{
					Data[Pos] = 0;
					return true;
				}

				// Add acceptable text to line
				if (c != 0x0a && c != 0x0d)
				{
					if (Pos < MaxSize - 1)
						Data[Pos++] = c;
				}
			}
		}
	}
}
bool cActionTemplate::GetNextWord(char *Data, FILE *fp, long MaxSize)
{
	int c;
	long Pos = 0;

	// Reset word to empty
	Data[0] = 0;

	// Read until an acceptable character found
	while (1)
	{
		if ((c = fgetc(fp)) == EOF)
		{
			Data[0] = 0;
			return false;
		}

		// Check for start of word
		if (c != 32 && c != 0x0a && c != 0x0d)
		{
			Data[Pos++] = c;

			// Loop until end of word (or EOF)
			while ((c = fgetc(fp)) != EOF)
			{
				// Break on acceptable word separators
				if (c == 32 || c == 0x0a || c == 0x0d)
					break;

				// Add if enough room left
				if (Pos < MaxSize - 1)
					Data[Pos++] = c;
			}

			// Add end of line to text
			Data[Pos] = 0;
			return true;
		}
	}
}
sScript *cActionTemplate::CreateScriptAction(long Type)
{
	long i;
	sScript *Script;
	sAction *ActionPtr;

	// Make sure it's a valid action - Type is really the
	// action ID (from the list of actions already loaded).
	if (Type >= m_NumActions)
		return 0;

	// Get pointer to action
	if ((ActionPtr = GetAction(Type)) == 0)
		return 0;

	// Create new sScript structure
	Script = new sScript();

	// Set type and number of entries (allocating a list)
	Script->m_Type = Type;
	Script->m_NumEntries = ActionPtr->m_NumEntries;
	Script->m_Entries = new sScriptEntry[Script->m_NumEntries]();

	// Set up each entry
	for (i = 0; i<Script->m_NumEntries; i++)
	{
		// Save type
		Script->m_Entries[i].m_Type = ActionPtr->m_Entries[i].m_Type;
		// Set up entry data based on type
		switch (Script->m_Entries[i].m_Type)
		{
		case _TEXT:		Script->m_Entries[i].m_Text = 0;	break;
		case _INT:		Script->m_Entries[i].m_lValue = ActionPtr->m_Entries[i].m_lMin;	break;
		case _FLOAT:	Script->m_Entries[i].m_fValue = ActionPtr->m_Entries[i].m_fMin;	break;
		case _BOOL:		Script->m_Entries[i].m_bValue = true;	break;
		case _CHOICE:	Script->m_Entries[i].m_Selection = 0;	break;
		}
	}
	return Script;
}
sAction *cActionTemplate::GetAction(long Num)
{
	sAction* current = m_ActionParent;

	while (current)
	{
		if (current->m_ID == Num)
			break;
		current = current->m_Next;
	}

	return current;
}
long cActionTemplate::GetNumEntries(long ActionNum)
{
	sAction* current = m_ActionParent;

	while (current)
	{
		if (current->m_ID == ActionNum)
			break;
		current = current->m_Next;
	}

	return current->m_NumEntries;
}
sEntry *cActionTemplate::GetEntry(long ActionNum, long EntryNum)
{
	sAction* current = m_ActionParent;

	while (current)
	{
		if (current->m_ID == ActionNum)
			break;
		current = current->m_Next;
	}

	return &current->m_Entries[EntryNum];
}
bool cActionTemplate::ExpandDefaultActionText(char *Buffer, sAction *Action)
{
	return true;
}
bool cActionTemplate::ExpandActionText(char *Buffer, sScript *Script)
{
	return true;
}

// ----- Load save
bool cScript::Load(string filename)
{
	m_NumActions = 0;
	if (m_ScriptParent) delete m_ScriptParent;
	m_ScriptParent = 0;

	// load the script
	sScript* testscript = NULL;
	if (testscript = LoadScriptFile(filename))
	{
		m_ScriptParent = testscript;
		string testpath = filename.substr(2, filename.find_last_of("\\") - 1);
		string testname = filename.substr(filename.find_last_of("\\") + 1, filename.length()) + "x";
		DirPath dp = DirPath() << testpath;
		FileList test(dp, testname.c_str());
		if (test.size() == 0)
		{
			SaveScriptXML((filename + "x").c_str(), m_ScriptParent);
			l.ss() << "Message: Rebuilding XML '" << filename << "x' from Binary '" << filename << "'\n";
			l.ssend();
		}
	}
	else if ((testscript = LoadScriptXML(filename)) != 0)
	{
		m_ScriptParent = testscript;
		SaveScriptXML((filename + "x").c_str(), m_ScriptParent);
		SaveScriptFile(filename.c_str(), m_ScriptParent);
		l.ss() << "Warning: Script '" << filename << "' was XML instead of Binary.\nRebuilding Binary '" << filename << "' as Binary and Building XML as '" << filename << "x'\n";
		l.ssend();
	}
	else if ((testscript = LoadScriptXML(filename + "x")) != 0)
	{
		m_ScriptParent = testscript;
		string testpath = filename.substr(2, filename.find_last_of("\\") - 1);
		string testname = filename.substr(filename.find_last_of("\\") + 1, filename.length());
		DirPath dp = DirPath() << testpath;
		FileList test(dp, testname.c_str());
		if (test.size() == 0)
		{
			SaveScriptFile(filename.c_str(), m_ScriptParent);
			l.ss() << "Message: Rebuilding Binary '" << filename << "' from XML '" << filename << "x'\n";
			l.ssend();
		}
	}
	else
	{
		l.ss() << "\n \nError: Could not load script: '" << filename << "'\n \n";
		l.ssend(); return false;
	}									// and return false
	return true;
}

bool SaveScriptFile(const char *Filename, sScript *ScriptRoot)
{
	FILE *fp;
	long i, j, NumActions;
	sScript *ScriptPtr;

	// Make sure there are some script actions
	if ((ScriptPtr = ScriptRoot) == 0)
		return false;

	// Count the number of actions
	NumActions = 0;
	while (ScriptPtr != 0)
	{
		NumActions++; // Increase count
		ScriptPtr = ScriptPtr->m_Next; // Next action
	}

	// Open the file for output
	if ((fp = fopen(Filename, "wb")) == 0) return false; // return a failure

	// Output # of script actions
	fwrite(&NumActions, 1, sizeof(long), fp);

	// Loop through each script action
	ScriptPtr = ScriptRoot;
	for (i = 0; i<NumActions; i++)
	{
		// Output type of action and # of entries
		fwrite(&ScriptPtr->m_Type, 1, sizeof(long), fp);
		fwrite(&ScriptPtr->m_NumEntries, 1, sizeof(long), fp);

		// Output entry data (if any)
		if (ScriptPtr->m_NumEntries)
		{
			for (j = 0; j<ScriptPtr->m_NumEntries; j++)
			{
				// Write entry type and data
				fwrite(&ScriptPtr->m_Entries[j].m_Type, 1, sizeof(long), fp);
				fwrite(&ScriptPtr->m_Entries[j].m_IOValue, 1, sizeof(long), fp);
				fwrite(&ScriptPtr->m_Entries[j].m_Var, 1, sizeof(unsigned char), fp);

				// Write text entry (if any)
				if (ScriptPtr->m_Entries[j].m_Type == _TEXT && ScriptPtr->m_Entries[j].m_Text != NULL)
					fwrite(ScriptPtr->m_Entries[j].m_Text, 1, ScriptPtr->m_Entries[j].m_Length, fp);
			}
		}

		// Go to next script structure in linked list
		ScriptPtr = ScriptPtr->m_Next;
	}

	fclose(fp);
	return true; // return a success!
}
sScript *LoadScriptFile(string Filename)
{
	FILE *fp;
	long i, j, Num;
	sScript *ScriptRoot = 0, *Script = 0, *ScriptPtr = 0;

	// Open the file for input
	if ((fp = fopen(Filename.c_str(), "rb")) == 0) return 0;
	
	if (sizeof(long) != 4) {
		g_LogFile.ss() << "Debug LoadScriptFile || sizeof(long) is " << sizeof(long) << " it should be 4. We are working on the fix for this. Please report it on PinkPetal.org"; g_LogFile.ssend();
	}
	
	// Get # of script actions from file
	fread(&Num, 1, sizeof(long), fp);

	// Loop through each script action
	for (i = 0; i < Num; i++)
	{
		// Allocate a script structure and link in
		Script = new sScript();
		if (ScriptPtr == 0) ScriptRoot = Script; // Assign root
		else ScriptPtr->m_Next = Script;
		ScriptPtr = Script;

		// Get type of action and # of entries
		fread(&Script->m_Type, 1, sizeof(long), fp);
		fread(&Script->m_NumEntries, 1, sizeof(long), fp);

		// Get entry data (if any)
		if (Script->m_NumEntries)
		{
			// Allocate entry array
			Script->m_Entries = new sScriptEntry[Script->m_NumEntries]();

			// Load in each entry
			for (j = 0; j < Script->m_NumEntries; j++)
			{
				// Get entry type and data
				fread(&Script->m_Entries[j].m_Type, 1, sizeof(long), fp);
				fread(&Script->m_Entries[j].m_IOValue, 1, sizeof(long), fp);
				fread(&Script->m_Entries[j].m_Var, 1, sizeof(unsigned char), fp);

				// Get text (if any)
				if (Script->m_Entries[j].m_Type == _TEXT && Script->m_Entries[j].m_Length)
				{
					// Allocate a buffer and get string
					Script->m_Entries[j].m_Text = new char[Script->m_Entries[j].m_Length];
					fread(Script->m_Entries[j].m_Text, 1, Script->m_Entries[j].m_Length, fp);
				}
			}
		}
	}

	fclose(fp);
	return ScriptRoot;
}

bool SaveScriptXML(const char *Filename, sScript *ScriptRoot)
{
	sScript *ScriptPtr;
	if ((ScriptPtr = ScriptRoot) == 0) return false;	// Make sure there are some script actions
	TiXmlDocument doc(Filename);
	TiXmlDeclaration* decl = new TiXmlDeclaration("2.0", "", "yes");
	doc.LinkEndChild(decl);
	TiXmlElement* pRoot = new TiXmlElement("Root");
	doc.LinkEndChild(pRoot);

	string name = Filename;
	int start = name.find_last_of("\\") + 1;
	int end = name.find_last_of(".") - start;
	name = name.substr(start, end);
	pRoot->SetAttribute("ScriptName", name);

	int NumActions = 0;
	while (ScriptPtr != 0) { NumActions++; ScriptPtr = ScriptPtr->m_Next; }	// Count the number of actions
	pRoot->SetAttribute("NumActions", NumActions);

	int i = 0; int j = 0;
	ScriptPtr = ScriptRoot;
	for (i = 0; i < NumActions; i++)		// Loop through each script action
	{
		TiXmlElement* action = new TiXmlElement("Action");
		pRoot->LinkEndChild(action);

		int maintype = ScriptPtr->m_Type;
		if (maintype == 24 && ScriptPtr->m_Entries[0].m_IOValue >= NUM_STATS) { ScriptPtr->m_Entries[0].m_IOValue -= NUM_STATS; maintype = ScriptPtr->m_Type = 82; }

		action->SetAttribute("Type", cScript::script_names[maintype]);	// `J` save human readable

		if (ScriptPtr->m_NumEntries)
		{
			for (j = 0; j < ScriptPtr->m_NumEntries; j++)
			{
				switch (maintype)
				{
				case  0: // "Dialog ~"
				case  6: // "TEXT ~"
				{
					stringstream ss; ss << ScriptPtr->m_Entries[j].m_Text;
					action->SetAttribute("Text", ss.str());
				} break;
				case   5: // "CHOICEBOX ~ ~"
				case  12: // "ActivateChoice ~"
				case  13: // "If Choice from ChoiceBox ~ is ~"
				{
					if (j == 0)													/**/ action->SetAttribute("ID", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 5 && j == 1)								/**/ action->SetAttribute("Num", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 13 && j == 1)								/**/ action->SetAttribute("Val", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case 7: //"SetVar ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Var", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Val", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case   8: //"SetVarRandom ~ ~ ~"
				case 103: //"AdjustTargetGirlStatR ~ ~ ~ ~"
				case 104: //"AdjustTargetGirlSkillR ~ ~ ~ ~"
				{
					if (maintype == 8 && j == 0)								/**/ action->SetAttribute("Var", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 103 && j == 0)								/**/ action->SetAttribute("Stat", sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (maintype == 104 && j == 0)								/**/ action->SetAttribute("Skill", sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (j == 1)													/**/ action->SetAttribute("Min", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Max", ScriptPtr->m_Entries[j].m_IOValue);
					if ((maintype == 103 || maintype == 104) && j == 3)			/**/ action->SetAttribute("Temp", ScriptPtr->m_Entries[j].m_IOValue);
					if ((maintype == 103 || maintype == 104) && j == 3)			/**/ action->SetAttribute("Info", "Temp:  0 = false, 1 = true");
				} break;

				case   9: //"IfVar ~ ~ ~"
				case  31: //"IfGirlStat ~ ~ ~"
				case  29: //"IfGirlFlag ~ ~ ~"
				case  32: //"IfSkill ~ ~ ~"
				{
					if (maintype == 9 && j == 0)								/**/ action->SetAttribute("Var", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 29 && j == 0)								/**/ action->SetAttribute("Flag", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 31 && j == 0)								/**/ action->SetAttribute("Stat", sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (maintype == 32 && j == 0)								/**/ action->SetAttribute("Skill", sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (j == 1)													/**/ action->SetAttribute("Compare", cScript::script_compare_types[ScriptPtr->m_Entries[j].m_IOValue]);
					if (j == 2)													/**/ action->SetAttribute("Amount", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Info", "Compare Options:   e , l , le , g , ge , ne");
				} break;
				case  14: //"SetPlayerSuspicion ~"
				case  15: //"SetPlayerDisposition ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Val", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case  16: //"ClearGlobalFlag ~"
				case  19: //"Set Global ~ ~"
				case  20: //"SetGirlFlag ~ ~"
				case  80: //"AdjustGirlFlag ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Flag", ScriptPtr->m_Entries[j].m_IOValue);
					if ((maintype == 19 || maintype == 20) && j == 1)			/**/ action->SetAttribute("SetTo", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 80 && j == 1)								/**/ action->SetAttribute("Amount", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case  17: //"AddCustomerToDungeon ~ ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Reason", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("NumDaughters", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("HasWife", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Info", "Reason: 0 = for not paying, 1 = beating a girl   |   Has Wife? 0 = no, 1 = yes");
				} break;
				case  18: // "AddRandomGirlToDungeon ~ ~ ~ ~ ~ ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Reason", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("MinAge", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("MaxAge", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 3)													/**/ action->SetAttribute("Slave_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 4)													/**/ action->SetAttribute("NonHuman_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 5)													/**/ action->SetAttribute("Arena_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 6)													/**/ action->SetAttribute("YourDaughter_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 6)													/**/ action->SetAttribute("Info", "Reason:  0 = Kidnapped, 1 = Captured   |   _01:  0 = no, 1 = yes");
				} break;
				case  21: //"AddRandomValueToGold ~ ~"
				case 102: //"GiveGoldToGirl ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Min", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Max", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case  22: //"AddManyRandomGirlsToDungeon ~ ~ ~ ~ ~ ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("HowMany", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Reason", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("MinAge", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 3)													/**/ action->SetAttribute("MaxAge", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 4)													/**/ action->SetAttribute("Slave_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 5)													/**/ action->SetAttribute("NonHuman_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 6)													/**/ action->SetAttribute("Arena_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 6)													/**/ action->SetAttribute("Info", "Reason:  0 = Kidnapped, 1 = Captured   |   _01:  0 = no, 1 = yes");
				} break;
				case  24: //"AdjustTargetGirlStat ~ ~"
				case  82: //"AdjustTargetGirlSkill ~ ~"
				{
					if (maintype == 24 && j == 0)								/**/ action->SetAttribute("Stat", sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (maintype == 82 && j == 0)								/**/ action->SetAttribute("Skill", sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (j == 1)													/**/ action->SetAttribute("Amount", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case  27: //"IfPassSkillCheck ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Skill", sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]);
				} break;
				case  28: // "IfPassStatCheck ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Stat", sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]);
				} break;
				case  33: // "IfHasTrait ~"
				case  71: // "AddTrait ~"
				case  72: // "RemoveTrait ~"
				case  73: // "AddTraitTemp ~ ~"
				case  81: // "AdjustTraitTemp ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Trait", ScriptPtr->m_Entries[j].m_Text);
					if ((maintype == 73 || maintype == 81) && j == 1)			/**/ action->SetAttribute("Temp", ScriptPtr->m_Entries[j].m_IOValue);
				} break;
				case  70: //"AddFamilyToDungeon ~ ~ ~ ~ ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("NumDaughters", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("TakeMother_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Reason", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 3)													/**/ action->SetAttribute("Slave_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 4)													/**/ action->SetAttribute("NonHuman_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 5)													/**/ action->SetAttribute("Arena_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 5)													/**/ action->SetAttribute("Info", "Reason:  0 = Kidnapped, 1 = Captured   |   _01   0 = no, 1 = yes");
				} break;
				case  78: //"Get Random Girl ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("From", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Num", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Info", "From where?  0 = Anywhere, 1 = Brothel, 2 = Studio, 3 = Arena, 4 = Centre, 5 = Clinic, 6 = Farm, 7 = House   |   If Brothel, which one?");
				} break;
				case  95: //"IfGirlHasItem ~"
				case  96: //"AddItemtoGirl ~ ~ ~"
				case  97: //"GivePlayerItem ~ ~"
				case  98: //"IfPlayerHasItem ~"
				case  99: //"GiveGirlInvItem ~ ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Item", ScriptPtr->m_Entries[j].m_IOValue);
					if ((maintype == 96 || maintype == 97) && j == 1)			/**/ action->SetAttribute("Num", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 99 && j == 1)								/**/ action->SetAttribute("Use_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 96 && j == 2)								/**/ action->SetAttribute("Use_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (maintype == 96 && j == 2)								/**/ action->SetAttribute("Info", "_01:  0 = no, 1 = yes");
					if (maintype == 99 && j == 1)								/**/ action->SetAttribute("Info", "_01:  0 = no, 1 = yes");
				} break;
				case 106: //"IfGirlStatus ~ is ~"
				case 107: //"SetGirlStatus ~ to ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Status", sGirl::status_names[ScriptPtr->m_Entries[j].m_IOValue]);
					if (j == 1)													/**/ action->SetAttribute("Val_01", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Info", "_01: 0 = no, 1 = yes");
				} break;
				case 109: //"CreatePregnancy type ~ chance ~ force ~"
				{
					if (j == 0)													/**/ action->SetAttribute("Type", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 1)													/**/ action->SetAttribute("Chance", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Force", ScriptPtr->m_Entries[j].m_IOValue);
					if (j == 2)													/**/ action->SetAttribute("Info", "Type: 0 = Player, 1 = Customer, 2 = Beast   |   Force:  0 = no, 1 = yes");
				} break;
				default:	// left in for anything that I may have missed
				{
					action->SetAttribute("NumEntries", ScriptPtr->m_NumEntries);
					TiXmlElement* entry = new TiXmlElement("Entry");
					action->LinkEndChild(entry);
					stringstream info;
					stringstream iovalue;
					if (iovalue.str().size() == 0)	iovalue << ScriptPtr->m_Entries[j].m_IOValue;

					entry->SetAttribute("EntryNumber", j);
					entry->SetAttribute("Type", cScript::script_entry_types[ScriptPtr->m_Entries[j].m_Type]);	// `J` save human readable
					entry->SetAttribute("IOValue", iovalue.str());
					entry->SetAttribute("Var", ScriptPtr->m_Entries[j].m_Var);
					if (info.str().size() > 0) entry->SetAttribute("Info", info.str());

					// Write text entry (if any)
					if (ScriptPtr->m_Entries[j].m_Type == _TEXT && ScriptPtr->m_Entries[j].m_Text != NULL)
					{
						stringstream ss; ss << ScriptPtr->m_Entries[j].m_Text;
						entry->SetAttribute("IOValue", ss.str().length());
						entry->SetAttribute("Text", ss.str());
					}
				} break;
				}
			}
		}
		ScriptPtr = ScriptPtr->m_Next;
	}
	doc.SaveFile();
	return true; // return a success!
}


sScript *LoadScriptXML(string Filename)
{
	// Open the file for input
	TiXmlDocument doc(Filename);
	if (!doc.LoadFile())
	{
		l.ss() << "Error: Can't load script '" << Filename << "'. " << endl; l.ssend();
		return 0;
	}
	XmlUtil xu(Filename);
	TiXmlElement *ela = NULL;
	TiXmlElement *ele = NULL;
	string ver=	doc.FirstChild()->ToDeclaration()->Version();

	TiXmlElement *root_el = doc.RootElement();
	const char *pt;
	sScript *ScriptRoot = 0, *Script = 0, *ScriptPtr = 0;
	if (!cScript::m_script_maps_setup)	cScript::setup_maps();									// only need to do this once
	int Num = 0;
	if (pt = root_el->Attribute("NumActions"))		xu.get_att(root_el, "NumActions", Num);
	if (!Num) return 0;

	if (ver == "2.0")
	{
		// Loop through each script action
		for (ela = root_el->FirstChildElement(); ela; ela = ela->NextSiblingElement())
		{
			Script = new sScript();																// Allocate a script structure and link in
			if (ScriptPtr == 0) ScriptRoot = Script;											// Assign root
			else ScriptPtr->m_Next = Script;
			ScriptPtr = Script;
			if (pt = ela->Attribute("Type"))													// `J` check for human readable
			{
				string ty = ""; xu.get_att(ela, "Type", ty); ty = stringtolower(ty);
				if (cScript::script_lookup.find(ty) == cScript::script_lookup.end())			// `J` if human readable not found
					xu.get_att(ela, "Type", Script->m_Type);									// `J` old script so get the number
				else Script->m_Type = cScript::script_lookup[ty];
			}
			int maintype = Script->m_Type;

			switch (maintype)
			{
			case 0:	case 6:	case 12:	case 14:	case 15:	case 16:	case 27:	case 28:	case 33:	case 71:	case 72:	case 95:	case 98:
				Script->m_NumEntries = 1;
				break;
			case 5:	case 7:	case 13:	case 19:	case 20:	case 21:	case 24:	case 73:	case 78:	case 80:	case 81:	case 82:	case 97:	case 99:	case 102:	case 106:	case 107:
				Script->m_NumEntries = 2;
				break;
			case 8:	case 9:	case 17:	case 29:	case 31:	case 32:	case 96:	case 109:
				Script->m_NumEntries = 3;
				break;
			case 103:	case 104:
				Script->m_NumEntries = 4;
				break;
			case 70:
				Script->m_NumEntries = 6;
				break;
			case 18:	case 22:
				Script->m_NumEntries = 7;
				break;
			default:
				Script->m_NumEntries = 0;
				break;
			}

			// Get entry data (if any)
			if (Script->m_NumEntries)
			{
				Script->m_Entries = new sScriptEntry[Script->m_NumEntries]();
				
				// most are of type _INT so we set that as the default so we only need to change the non _INTs
				for (int vv = 0; vv < Script->m_NumEntries; vv++)	{ Script->m_Entries[vv].m_Type = _INT;  }

				string t = "";
				switch (maintype)
				{
				case 0:	case 6:
					Script->m_Entries[0].m_Type = _TEXT;
					t = ela->Attribute("Text");
					Script->m_Entries[0].m_Text = new char[t.length()];
					strcpy(Script->m_Entries[0].m_Text, ela->Attribute("Text"));
					Script->m_Entries[0].m_IOValue = t.length() + 1;
					break;
				case   5: // "CHOICEBOX ~ ~"
				case  12: // "ActivateChoice ~"
				case  13: // "If Choice from ChoiceBox ~ is ~"
					ela->QueryIntAttribute("ID", &Script->m_Entries[0].m_IOValue);
					if (maintype == 5)	ela->QueryIntAttribute("Num", &Script->m_Entries[1].m_IOValue);
					if (maintype == 13)	ela->QueryIntAttribute("Val", &Script->m_Entries[1].m_IOValue);
					break;
				case   7: // "SetVar ~ ~"
					ela->QueryIntAttribute("Var", &Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("Val", &Script->m_Entries[1].m_IOValue);
					break;
				case   8: // "SetVarRandom ~ ~ ~"
					ela->QueryIntAttribute("Var", &Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("Min", &Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("Max", &Script->m_Entries[2].m_IOValue);
					break;
				case 103: //"AdjustTargetGirlStatR ~ ~ ~ ~"
				case 104: //"AdjustTargetGirlSkillR ~ ~ ~ ~"
					Script->m_Entries[0].m_Type = _CHOICE;
					if (maintype == 103)	{ t = ela->Attribute("Stat");	Script->m_Entries[0].m_IOValue = sGirl::stat_lookup[t]; }
					if (maintype == 104)	{ t = ela->Attribute("Skill");	Script->m_Entries[0].m_IOValue = sGirl::skill_lookup[t]; }
					ela->QueryIntAttribute("Min", &Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("Max", &Script->m_Entries[2].m_IOValue);
					ela->QueryIntAttribute("Temp", &Script->m_Entries[3].m_IOValue);	Script->m_Entries[3].m_Type = _BOOL;
					break;
				case   9: // "IfVar ~ ~ ~"
				case  29: // "IfGirlFlag ~ ~ ~"
				case  31: // "IfStat ~ ~ ~"
				case  32: // "IfSkill ~ ~ ~"
					if (maintype == 9)	ela->QueryIntAttribute("Var", &Script->m_Entries[0].m_IOValue);
					if (maintype == 29)	ela->QueryIntAttribute("Flag", &Script->m_Entries[0].m_IOValue);
					if (maintype == 31)	{ t = ela->Attribute("Stat");		Script->m_Entries[0].m_IOValue = sGirl::stat_lookup[t]; Script->m_Entries[0].m_Type = _CHOICE; }
					if (maintype == 32)	{ t = ela->Attribute("Skill");		Script->m_Entries[0].m_IOValue = sGirl::skill_lookup[t]; Script->m_Entries[0].m_Type = _CHOICE; }
					t = ela->Attribute("Compare");	Script->m_Entries[1].m_IOValue = cScript::script_lookup[stringtolower(t)];	Script->m_Entries[1].m_Type = _CHOICE;
					ela->QueryIntAttribute("Amount", &Script->m_Entries[2].m_IOValue);
					break;
				case  14: // "SetPlayerSuspicion ~"
				case  15: // "SetPlayerDisposition ~"
					ela->QueryIntAttribute("Val", &Script->m_Entries[0].m_IOValue);
					break;
				case  16: //"ClearGlobalFlag ~"
				case  19: //"Set Global ~ ~"
				case  20: //"SetGirlFlag ~ ~"
				case  80: //"AdjustGirlFlag ~ ~"
					ela->QueryIntAttribute("Flag", &Script->m_Entries[0].m_IOValue);
					if (maintype == 19 || maintype == 20)	ela->QueryIntAttribute("SetTo", &Script->m_Entries[1].m_IOValue);
					if (maintype == 80)						ela->QueryIntAttribute("Amount", &Script->m_Entries[1].m_IOValue);
					break;
				case  21: // "AddRandomValueToGold ~ ~"
				case 102: // "GiveGoldToGirl ~ ~"
					ela->QueryIntAttribute("Min", &Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("Max", &Script->m_Entries[1].m_IOValue);
					break;

				case  24: // "AdjustTargetGirlStat ~ ~"
				case  82: // "AdjustTargetGirlSkill ~ ~"
					ela->QueryIntAttribute("Amount", &Script->m_Entries[1].m_IOValue);
				case  27: // "IfPassSkillCheck ~"
				case  28: // "IfPassStatCheck ~"
					Script->m_Entries[0].m_Type = _CHOICE;
					if (maintype == 24 || maintype == 28)
					{
						t = ela->Attribute("Stat");
						if (maintype == 24 && sGirl::skill_lookup.find(t) != sGirl::skill_lookup.end())		Script->m_Entries[0].m_IOValue = sGirl::skill_lookup[t] + NUM_STATS;
						else if (maintype == 24 && sGirl::stat_lookup.find(t) != sGirl::stat_lookup.end())	Script->m_Entries[0].m_IOValue = sGirl::stat_lookup[t];
						else Script->m_Entries[0].m_IOValue = sGirl::stat_lookup[t];
					}
					if (maintype == 27 || maintype == 82)	{ t = ela->Attribute("Skill");	Script->m_Entries[0].m_IOValue = sGirl::skill_lookup[t]; }
					break;

				case  73: // "AddTraitTemp ~ ~"
				case  81: // "AdjustTraitTemp ~ ~"
					Script->m_Entries[1].m_Type = _BOOL;
					ela->QueryIntAttribute("Temp", &Script->m_Entries[1].m_IOValue);
				case  33: // "IfHasTrait ~"
				case  71: // "AddTrait ~"
				case  72: // "RemoveTrait ~"
					Script->m_Entries[0].m_Type = _TEXT;
					t = ela->Attribute("Trait");
					Script->m_Entries[0].m_Text = new char[t.length()];
					strcpy(Script->m_Entries[0].m_Text, ela->Attribute("Trait"));
					Script->m_Entries[0].m_IOValue = t.length() + 1;
					break;
				case  78: // "Get Random Girl ~ ~"
					Script->m_Entries[0].m_Type = _CHOICE;
					ela->QueryIntAttribute("From", &Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("Temp", &Script->m_Entries[1].m_IOValue);
					break;
				case  96: // "AddItemtoGirl ~ ~ ~"
					Script->m_Entries[2].m_Type = _BOOL;
					ela->QueryIntAttribute("Use_01", &Script->m_Entries[2].m_IOValue);
				case  97: // "GivePlayerItem ~ ~"
					ela->QueryIntAttribute("Num", &Script->m_Entries[1].m_IOValue);
				case  99: // "GiveGirlInvItem ~ ~"
					if (maintype == 99)
					{
						Script->m_Entries[1].m_Type = _BOOL;
						ela->QueryIntAttribute("Use_01", &Script->m_Entries[1].m_IOValue);
					}
				case  98: // "IfPlayerHasItem ~"
				case  95: // "IfGirlHasItem ~"
					Script->m_Entries[0].m_Type = _TEXT;
					t = ela->Attribute("Item");
					Script->m_Entries[0].m_Text = new char[t.length()];
					strcpy(Script->m_Entries[0].m_Text, ela->Attribute("Item"));
					Script->m_Entries[0].m_IOValue = t.length() + 1;
					break;
				case 106: // "IfGirlStatus ~ is ~"
				case 107: // "SetGirlStatus ~ to ~"
					t = ela->Attribute("Status");			
					Script->m_Entries[0].m_IOValue = sGirl::status_lookup[t];			Script->m_Entries[0].m_Type = _CHOICE;
					ela->QueryIntAttribute("Val_01", &Script->m_Entries[1].m_IOValue);	Script->m_Entries[1].m_Type = _BOOL;
					break;
				case 109: // "CreatePregnancy type ~ chance ~ force ~"
					ela->QueryIntAttribute("Type", &Script->m_Entries[0].m_IOValue);	Script->m_Entries[0].m_Type = _CHOICE;
					ela->QueryIntAttribute("Chance", &Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("Force", &Script->m_Entries[2].m_IOValue);	Script->m_Entries[2].m_Type = _BOOL;
				break;


				case  17: // "AddCustomerToDungeon ~ ~ ~"
					Script->m_Entries[0].m_Type = _CHOICE;	Script->m_Entries[2].m_Type = _BOOL;
					ela->QueryIntAttribute("Reason",			&Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("NumDaughters",		&Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("HasWife",			&Script->m_Entries[2].m_IOValue);
					break;
				case  18: // "AddRandomGirlToDungeon ~ ~ ~ ~ ~ ~ ~"
					Script->m_Entries[0].m_Type = _CHOICE;	Script->m_Entries[3].m_Type = Script->m_Entries[4].m_Type = Script->m_Entries[5].m_Type = Script->m_Entries[6].m_Type = _BOOL;
					ela->QueryIntAttribute("Reason",			&Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("MinAge",			&Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("MaxAge",			&Script->m_Entries[2].m_IOValue);
					ela->QueryIntAttribute("Slave_01",			&Script->m_Entries[3].m_IOValue);
					ela->QueryIntAttribute("NonHuman_01",		&Script->m_Entries[4].m_IOValue);
					ela->QueryIntAttribute("Arena_01",			&Script->m_Entries[5].m_IOValue);
					ela->QueryIntAttribute("YourDaughter_01",	&Script->m_Entries[6].m_IOValue);
					break;
				case  22: // "AddManyRandomGirlsToDungeon ~ ~ ~ ~ ~ ~ ~"
					Script->m_Entries[1].m_Type = _CHOICE;	Script->m_Entries[4].m_Type = Script->m_Entries[5].m_Type = Script->m_Entries[6].m_Type = _BOOL;
					ela->QueryIntAttribute("HowMany",			&Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("Reason",			&Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("MinAge",			&Script->m_Entries[2].m_IOValue);
					ela->QueryIntAttribute("MaxAge",			&Script->m_Entries[3].m_IOValue);
					ela->QueryIntAttribute("Slave_01",			&Script->m_Entries[4].m_IOValue);
					ela->QueryIntAttribute("NonHuman_01",		&Script->m_Entries[5].m_IOValue);
					ela->QueryIntAttribute("Arena_01",			&Script->m_Entries[6].m_IOValue);

					break;
				case  70: // "AddFamilyToDungeon ~ ~ ~ ~ ~ ~"
					Script->m_Entries[2].m_Type = _CHOICE;	Script->m_Entries[1].m_Type = Script->m_Entries[3].m_Type = Script->m_Entries[4].m_Type = Script->m_Entries[5].m_Type = _BOOL;
					ela->QueryIntAttribute("NumDaughters",		&Script->m_Entries[0].m_IOValue);
					ela->QueryIntAttribute("TakeMother_01",		&Script->m_Entries[1].m_IOValue);
					ela->QueryIntAttribute("Reason",			&Script->m_Entries[2].m_IOValue);
					ela->QueryIntAttribute("Slave_01",			&Script->m_Entries[3].m_IOValue);
					ela->QueryIntAttribute("NonHuman_01",		&Script->m_Entries[4].m_IOValue);
					ela->QueryIntAttribute("Arena_01",			&Script->m_Entries[5].m_IOValue);
					break;
				default:		// left in for anything that I may have missed
				{
					int entrynum = -1;
					for (ele = ela->FirstChildElement(); ele; ele = ele->NextSiblingElement())
					{
						if (pt = ele->Attribute("EntryNumber"))		xu.get_att(ele, "EntryNumber", entrynum);
						else entrynum++;

						if (pt = ele->Attribute("Type"))	// `J` check for human readable
						{
							string ty = ""; xu.get_att(ele, "Type", ty); ty = stringtolower(ty);
							if (cScript::script_lookup.find(ty) == cScript::script_lookup.end())		// `J` if human readable not found
								xu.get_att(ele, "Type", Script->m_Entries[entrynum].m_Type);			// `J` old script so get the number
							else Script->m_Entries[entrynum].m_Type = cScript::script_lookup[ty];
						}
						if (pt = ele->Attribute("IOValue"))
						{
							string ty = ""; xu.get_att(ele, "IOValue", ty); string tyl = stringtolower(ty);
							if (cScript::script_lookup.find(ty) != cScript::script_lookup.end())					Script->m_Entries[entrynum].m_IOValue = cScript::script_lookup[tyl];
							else if (maintype == 24 && sGirl::skill_lookup.find(ty) != sGirl::skill_lookup.end())	Script->m_Entries[entrynum].m_IOValue = sGirl::skill_lookup[ty] + NUM_STATS;
							else if (sGirl::stat_lookup.find(ty) != sGirl::stat_lookup.end())						Script->m_Entries[entrynum].m_IOValue = sGirl::stat_lookup[ty];
							else if (sGirl::skill_lookup.find(ty) != sGirl::skill_lookup.end())						Script->m_Entries[entrynum].m_IOValue = sGirl::skill_lookup[ty];
							else if (sGirl::status_lookup.find(ty) != sGirl::status_lookup.end())					Script->m_Entries[entrynum].m_IOValue = sGirl::status_lookup[ty];
							else xu.get_att(ele, "IOValue", Script->m_Entries[entrynum].m_IOValue);		// `J` old script so get the number
						}
						if (pt = ele->Attribute("Var"))			xu.get_att(ele, "Var", Script->m_Entries[entrynum].m_Var);

						// Get text (if any)
						if (Script->m_Entries[entrynum].m_Type == _TEXT && Script->m_Entries[entrynum].m_Length)
						{
							if (ele->Attribute("Text"))
							{
								Script->m_Entries[entrynum].m_Text = new char[Script->m_Entries[entrynum].m_Length];
								strcpy(Script->m_Entries[entrynum].m_Text, ele->Attribute("Text"));
								string a = ele->Attribute("Text");
								Script->m_Entries[entrynum].m_IOValue = a.length() + 1;
							}
						}
					}
				}	break;
				}
			}
		}
	}
	else // if (ver == "1.0")
	{

		// Loop through each script action
		for (ela = root_el->FirstChildElement(); ela; ela = ela->NextSiblingElement())
		{
			// Allocate a script structure and link in
			Script = new sScript();

			if (ScriptPtr == 0) ScriptRoot = Script; // Assign root
			else ScriptPtr->m_Next = Script;
			ScriptPtr = Script;

			if (pt = ela->Attribute("Type"))	// `J` check for human readable
			{
				string ty = ""; xu.get_att(ela, "Type", ty); ty = stringtolower(ty);
				if (cScript::script_lookup.find(ty) == cScript::script_lookup.end())				// `J` if human readable not found
					xu.get_att(ela, "Type", Script->m_Type);										// `J` old script so get the number
				else Script->m_Type = cScript::script_lookup[ty];
			}
			if (pt = ela->Attribute("NumEntries"))	xu.get_att(ela, "NumEntries", Script->m_NumEntries);
			int maintype = Script->m_Type;
			// Get entry data (if any)
			if (Script->m_NumEntries)
			{
				// Allocate entry array
				Script->m_Entries = new sScriptEntry[Script->m_NumEntries]();
				int entrynum = -1;
				for (ele = ela->FirstChildElement(); ele; ele = ele->NextSiblingElement())
				{
					if (pt = ele->Attribute("EntryNumber"))		xu.get_att(ele, "EntryNumber", entrynum);
					else entrynum++;

					if (pt = ele->Attribute("Type"))	// `J` check for human readable
					{
						string ty = ""; xu.get_att(ele, "Type", ty); ty = stringtolower(ty);
						if (cScript::script_lookup.find(ty) == cScript::script_lookup.end())		// `J` if human readable not found
							xu.get_att(ele, "Type", Script->m_Entries[entrynum].m_Type);			// `J` old script so get the number
						else Script->m_Entries[entrynum].m_Type = cScript::script_lookup[ty];
					}
					if (pt = ele->Attribute("IOValue"))
					{
						string ty = ""; xu.get_att(ele, "IOValue", ty); string tyl = stringtolower(ty);
						if (cScript::script_lookup.find(ty) != cScript::script_lookup.end())					Script->m_Entries[entrynum].m_IOValue = cScript::script_lookup[tyl];
						else if (maintype == 24 && sGirl::skill_lookup.find(ty) != sGirl::skill_lookup.end())	Script->m_Entries[entrynum].m_IOValue = sGirl::skill_lookup[ty] + NUM_STATS;
						else if (sGirl::stat_lookup.find(ty) != sGirl::stat_lookup.end())						Script->m_Entries[entrynum].m_IOValue = sGirl::stat_lookup[ty];
						else if (sGirl::skill_lookup.find(ty) != sGirl::skill_lookup.end())						Script->m_Entries[entrynum].m_IOValue = sGirl::skill_lookup[ty];
						else xu.get_att(ele, "IOValue", Script->m_Entries[entrynum].m_IOValue);		// `J` old script so get the number
					}
					if (pt = ele->Attribute("Var"))			xu.get_att(ele, "Var", Script->m_Entries[entrynum].m_Var);

					// Get text (if any)
					if (Script->m_Entries[entrynum].m_Type == _TEXT && Script->m_Entries[entrynum].m_Length)
					{
						if (ele->Attribute("Text"))
						{
							Script->m_Entries[entrynum].m_Text = new char[Script->m_Entries[entrynum].m_Length];
							strcpy(Script->m_Entries[entrynum].m_Text, ele->Attribute("Text"));
							string a = ele->Attribute("Text");
							Script->m_Entries[entrynum].m_IOValue = a.length() + 1;

						}
					}
				}
			}
		}
	}
	return ScriptRoot;
}
