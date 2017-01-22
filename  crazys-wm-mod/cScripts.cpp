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
	"Script_AdjustTargetGirlSkillR"
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
	// Make sure there are some script actions
	if ((ScriptPtr = ScriptRoot) == 0) return false;

	TiXmlDocument doc(Filename);
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "yes");
	doc.LinkEndChild(decl);
	TiXmlElement* pRoot = new TiXmlElement("Root");
	doc.LinkEndChild(pRoot);

	//		FILE *fp;

	int NumActions = 0; int i = 0; int j = 0;

	// Count the number of actions
	while (ScriptPtr != 0)
	{
		NumActions++; // Increase count
		ScriptPtr = ScriptPtr->m_Next; // Next action
	}

	// Output # of script actions
	//	fwrite(&NumActions, 1, sizeof(long), fp);
	string name = Filename;
	int start = name.find_last_of("\\") + 1;
	int end = name.find_last_of(".") - start;
	name = name.substr(start, end);


	pRoot->SetAttribute("ScriptName", name);
	pRoot->SetAttribute("NumActions", NumActions);


#if 1
	// Loop through each script action
	ScriptPtr = ScriptRoot;
	for (i = 0; i<NumActions; i++)
	{
		// Output type of action and # of entries
		//		fwrite(&ScriptPtr->m_Type, 1, sizeof(long), fp);
		//		fwrite(&ScriptPtr->m_NumEntries, 1, sizeof(long), fp);
		TiXmlElement* action = new TiXmlElement("Action");
		pRoot->LinkEndChild(action);
		action->SetAttribute("ActionNumber", i);
		action->SetAttribute("Type", cScript::script_names[ScriptPtr->m_Type]);	// `J` save human readable
		int maintype = ScriptPtr->m_Type;
		action->SetAttribute("NumEntries", ScriptPtr->m_NumEntries);
		switch (maintype)
		{
		case 5:	// choicebox should have 2 entries
			action->SetAttribute("Info", "The Actions after the Choicebox must be the choices available.");
			break;
		case 31:	// Script_IfGirlStat has 3 entries
			break;

		default: break;
		}

#if 1
		// Output entry data (if any)
		if (ScriptPtr->m_NumEntries)
		{
			for (j = 0; j<ScriptPtr->m_NumEntries; j++)
			{
				// Write entry type and data
				//				fwrite(&ScriptPtr->m_Entries[j].m_Type, 1, sizeof(long), fp);
				//				fwrite(&ScriptPtr->m_Entries[j].m_IOValue, 1, sizeof(long), fp);
				//				fwrite(&ScriptPtr->m_Entries[j].m_Var, 1, sizeof(unsigned char), fp);

				TiXmlElement* entry = new TiXmlElement("Entry");
				action->LinkEndChild(entry);
				stringstream info;
				stringstream iovalue;
				switch (maintype)
				{
				case 5:	// Script_ChoiceBox
					if (j == 0) info << "ID of the choicebox";
					if (j == 1) info << "Number of choices";
					break;
				case 9:		// Script_IfVar(Script);
				case 29:	// Script_IfGirlFlag(Script);
					if (j == 1) { iovalue << cScript::script_compare_types[ScriptPtr->m_Entries[j].m_IOValue]; info << "IOValue Options:   e , l , le , g , ge , ne"; }
					break;
				case 12:	// Script_ActivateChoice(Script);
					if (j == 0) info << "Activate what choicebox ID?";
					break;
				case 13:	// Script_IfChoice(Script);
					if (j == 0) info << "What choicebox ID was used?";
					if (j == 1) info << "What was the choice?";
					break;
				case 17:	// Script_AddCustToDungeon(Script);
					if (j == 0) { info << "Reason:   0 = for not paying, 1 = beating a girl"; }
					if (j == 1) { info << "Number of Daughters"; }
					if (j == 2) { info << "Has Wife? 0 = no, 1 = yes"; }
					break;
				case 18:	// Script_AddRandomGirlToDungeon(Script);
					if (j == 0) { info << "Reason:  0 = Kidnaped, 1 = Captured"; }
					if (j == 1) { info << "Min Age?"; }
					if (j == 2) { info << "Max Age?"; }
					if (j == 3) { info << "Slave?   0 = no, 1 = yes"; }
					if (j == 4) { info << "NonHuman?   0 = no, 1 = yes"; }
					if (j == 5) { info << "Arena?   0 = no, 1 = yes"; }
					if (j == 6) { info << "Make her your daughter?   0 = no, 1 = yes"; }
					break;
				case 22:	// Script_AddManyRandomGirlsToDungeon(Script);
					if (j == 0) { info << "How Many?"; }
					if (j == 1) { info << "Reason:  0 = Kidnaped, 1 = Captured"; }
					if (j == 2) { info << "Min Age?"; }
					if (j == 3) { info << "Max Age?"; }
					if (j == 4) { info << "Slave?   0 = no, 1 = yes"; }
					if (j == 5) { info << "NonHuman?   0 = no, 1 = yes"; }
					if (j == 6) { info << "Arena?   0 = no, 1 = yes"; }
					break;
				case 24:	// Script_AdjustTargetGirlStat(Script);
					if (j == 0)
					{
						if (ScriptPtr->m_Entries[j].m_IOValue < NUM_STATS)
							iovalue << sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue];
						else
							iovalue << sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue - NUM_STATS];
						info << "Stat/Skill";
					}
					break;
				case 27:	// Script_IfPassSkillCheck(Script);
					if (j == 0) { iovalue << sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Skill"; }
					break;
				case 28:	// Script_IfPassStatCheck(Script);
					if (j == 0) { iovalue << sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Stat"; }
					break;
				case 31:	// Script_IfGirlStat
					if (j == 0) { iovalue << sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Stat"; }
					if (j == 1) { iovalue << cScript::script_compare_types[ScriptPtr->m_Entries[j].m_IOValue]; info << "IOValue Options:   e , l , le , g , ge , ne"; }
					if (j == 2) { info << "Amount"; }
					break;
				case 32:	// Script_IfGirlSkill(Script);
					if (j == 0) { iovalue << sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Skill"; }
					if (j == 1) { iovalue << cScript::script_compare_types[ScriptPtr->m_Entries[j].m_IOValue]; info << "IOValue Options:   e , l , le , g , ge , ne"; }
					if (j == 2) { info << "Amount"; }
					break;
				case 70:	// Script_AddFamilyToDungeon(Script);
					if (j == 0) { info << "How Many Daughters?"; }
					if (j == 1) { info << "Take Mother?   0 = no, 1 = yes"; }
					if (j == 2) { info << "Reason:  0 = Kidnaped, 1 = Captured"; }
					if (j == 3) { info << "Slave?   0 = no, 1 = yes"; }
					if (j == 4) { info << "NonHuman?   0 = no, 1 = yes"; }
					if (j == 5) { info << "Arena?   0 = no, 1 = yes"; }
					break;
				case 73:	// Script_AddTraitTemp(Script);
					if (j == 1) { info << "How long will it last?"; }
					break;
				case 78:	// Script_GetRandomGirl(Script);
					if (j == 0) { info << "From where?  0 = Anywhere, 1 = Brothel, 2 = Studio, 3 = Arena, 4 = Centre, 5 = Clinic, 6 = Farm, 7 = House"; }
					if (j == 1) { info << "If Brothel, which one?"; }
					break;
				case 82:	// Script_AdjustTargetGirlSkill(Script);
					if (j == 0) { iovalue << sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Skill"; }
					if (j == 1) { info << "Amount"; }
					break;
				case 96:	// Script_AddItemtoGirl(Script);
					if (j == 0) { info << "What Item?"; }
					if (j == 1) { info << "How Many?"; }
					if (j == 2) { info << "Use/Equip?   0 = no, 1 = yes"; }
					break;
				case 97:	// Script_GivePlayerItem(Script);
					if (j == 0) { info << "What Item?"; }
					if (j == 1) { info << "How Many?"; }
					break;
				case 99:	// Script_GiveGirlInvItem(Script);
					if (j == 0) { info << "What Item?"; }
					if (j == 1) { info << "Use/Equip?   0 = no, 1 = yes"; }
					break;
				case 102:	// Script_GiveGoldToGirl(Script);
					if (j == 0) { info << "Min Amount"; }
					if (j == 1) { info << "Max Amount"; }
					break;
				case 103:	// Script_AdjustTargetGirlStatR(Script);
					if (j == 0) { iovalue << sGirl::stat_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Stat"; }
					if (j == 1) { info << "Min Amount"; }
					if (j == 2) { info << "Max Amount"; }
					if (j == 3) { info << "Temporary?   0 = no, 1 = yes"; }
					break;
				case 104:	// Script_AdjustTargetGirlSkillR(Script);
					if (j == 0) { iovalue << sGirl::skill_names[ScriptPtr->m_Entries[j].m_IOValue]; info << "Skill"; }
					if (j == 1) { info << "Min Amount"; }
					if (j == 2) { info << "Max Amount"; }
					if (j == 3) { info << "Temporary?   0 = no, 1 = yes"; }
					break;


				default:
					break;
				}
				if (iovalue.str().size() == 0)	iovalue << ScriptPtr->m_Entries[j].m_IOValue;

				entry->SetAttribute("ActionNumber", i);
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
			}
		}
#endif

		// Go to next script structure in linked list
		ScriptPtr = ScriptPtr->m_Next;
	}

#endif
	//	fclose(fp);
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
	TiXmlElement *root_el = doc.RootElement();
	const char *pt;
	int Num = 0;
	sScript *ScriptRoot = 0, *Script = 0, *ScriptPtr = 0;

	if (pt = root_el->Attribute("NumActions"))		xu.get_att(root_el, "NumActions", Num);
	if (!Num) return 0;

#if 1
	if (!cScript::m_script_maps_setup)	cScript::setup_maps();									// only need to do this once

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
						Script->m_Entries[entrynum].m_IOValue = a.length()+1;
							
					}
				}
			}
		}
	}
#endif

	return ScriptRoot;
}
