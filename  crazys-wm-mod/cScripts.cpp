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
#include "cScripts.h"
#include <string>

#ifdef LINUX
#include "linux.h"
#endif

using namespace std;

bool cActionTemplate::GetNextQuotedLine(char *Data, FILE *fp, long MaxSize)
{
	int c;
	long Pos = 0;

	// Read until a quote is reached (or EOF)
	while(1) 
	{
		if((c = fgetc(fp)) == EOF)
			return false;

		if(c == '"')
		{
			// Read until next quote (or EOF)
			while(1)
			{
				if((c = fgetc(fp)) == EOF)
					return false;
			
				// Return text when 2nd quote found
				if(c == '"')
				{
					Data[Pos] = 0;
						return true;
				}
				
				// Add acceptable text to line
				if(c != 0x0a && c != 0x0d) 
				{
					if(Pos < MaxSize-1)
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
	while(1) 
	{
		if((c = fgetc(fp)) == EOF) 
		{
			Data[0] = 0;
			return false;
		}
	
		// Check for start of word
		if(c != 32 && c != 0x0a && c != 0x0d)
		{
			Data[Pos++] = c;
	
			// Loop until end of word (or EOF)
			while((c=fgetc(fp)) != EOF) 
			{
				// Break on acceptable word separators
				if(c == 32 || c == 0x0a || c == 0x0d)
					break;

				// Add if enough room left
				if(Pos < MaxSize-1)
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
	if(Type >= m_NumActions)
		return 0;
	
	// Get pointer to action
	if((ActionPtr = GetAction(Type)) == 0)
		return 0;

	// Create new sScript structure
	Script = new sScript();
	
	// Set type and number of entries (allocating a list)
	Script->m_Type = Type;
	Script->m_NumEntries = ActionPtr->m_NumEntries;
	Script->m_Entries = new sScriptEntry[Script->m_NumEntries]();
	
	// Set up each entry
	for(i=0;i<Script->m_NumEntries;i++) 
	{
		// Save type
		Script->m_Entries[i].m_Type = ActionPtr->m_Entries[i].m_Type;
		// Set up entry data based on type
		switch(Script->m_Entries[i].m_Type) 
		{
		case _TEXT:
			Script->m_Entries[i].m_Text = 0;
			break;
		
		case _INT:
			Script->m_Entries[i].m_lValue = ActionPtr->m_Entries[i].m_lMin;
			break;
		
		case _FLOAT:
			Script->m_Entries[i].m_fValue = ActionPtr->m_Entries[i].m_fMin;
			break;
		
		case _BOOL:
			Script->m_Entries[i].m_bValue = true;
			break;
		
		case _CHOICE:
			Script->m_Entries[i].m_Selection = 0;
			break;
		}
	}
	return Script;
}

sAction *cActionTemplate::GetAction(long Num)
{
	sAction* current = m_ActionParent;

	while(current)
	{
		if(current->m_ID == Num)
			break;
		current = current->m_Next;
	}

	return current;
}

long cActionTemplate::GetNumEntries(long ActionNum)
{
	sAction* current = m_ActionParent;

	while(current)
	{
		if(current->m_ID == ActionNum)
			break;
		current = current->m_Next;
	}

	return current->m_NumEntries;
}

sEntry *cActionTemplate::GetEntry(long ActionNum, long EntryNum)
{
	sAction* current = m_ActionParent;

	while(current)
	{
		if(current->m_ID == ActionNum)
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

bool cActionTemplate::Load()
{
	FILE *fp;
	char Text[2048];
	sAction *Action, *ActionPtr = 0;
	sEntry *Entry;
	long i, j;

	// Free previous action structures
	Free();

	// Open the action file
	if((fp=fopen("ScriptCommands.txt", "rb"))==0)
		return false;
	
	// Keep looping until end of file found
	while(1)
	{
		// Get next quoted action
		if(GetNextQuotedLine(Text, fp, 2048) == false)
			break;

		// Quit if no action text
		if(!Text[0])
			break;

		// Allocate an action structure and append it to list
		Action = new sAction();
		Action->m_Next = NULL;

		if(ActionPtr == NULL)
			m_ActionParent = Action;
		else
			ActionPtr->m_Next = Action;
		
		ActionPtr = Action;
		
		// Copy action text
		strcpy(Action->m_Text, Text);
		
		// Store action ID
		Action->m_ID = m_NumActions;
		
		// Increase the number of actions loaded
		m_NumActions++;
		
		// Count the number of entries in the action
		for(i=0;i<(long)strlen(Text);i++) 
		{
			if(Text[i] == '~')
				Action->m_NumEntries++;
		}
		
		// Allocate and read in entries (if any)
		if(Action->m_NumEntries)
		{
			Action->m_Entries = new sEntry[Action->m_NumEntries]();
			for(i=0;i<Action->m_NumEntries;i++) 
			{
				Entry = &Action->m_Entries[i];

				// Get type of entry
				GetNextWord(Text, fp, 2048);

				// TEXT type, no data follows
				if(!strcmp(Text, "TEXT"))
				{
					// Set to text type
					Entry->m_Type = _TEXT;
				} 
				else // If not TEXT, then check INT type, get min and max values
				{
					if(!strcmp(Text, "INT")) 
					{
						// Set to INT type and allocate INT entry
						Entry->m_Type = _INT;
				
						// Get min value
						GetNextWord(Text, fp, 2048);
						Entry->m_lMin = atol(Text);
				
						// Get max value
						GetNextWord(Text, fp, 2048);
						Entry->m_lMax = atol(Text);
					}
					else // If not INT, then check FLOAT type, get min and max values
					{
						if(!strcmp(Text, "FLOAT")) 
						{
							// Set to FLOAT type and allocate FLOAT entry
							Entry->m_Type = _FLOAT;

							// Get min value
							GetNextWord(Text, fp, 2048);
							Entry->m_fMin = (float)atof(Text);

							// Get max value
							GetNextWord(Text, fp, 2048);
							Entry->m_fMax = (float)atof(Text);
						}
						else // If not FLOAT, then check bool type
						{
							// bool type, no options
							if(!strcmp(Text, "bool")) 
							{
								// Set to bool type and allocate bool entry
								Entry->m_Type = _BOOL;
							}
							else if(!strcmp(Text, "CHOICE"))		// If not bool, then check CHOICE type, get number of entries and entry's texts
							{
								// Set to CHOICE type and allocate CHOICE entry
								Entry->m_Type = _CHOICE;

								// Get the number of choices
								GetNextWord(Text, fp, 1024);
								Entry->m_NumChoices = atol(Text);
								Entry->m_Choices = new char*[Entry->m_NumChoices];

								// Get each entry text
								for(j=0;j<Entry->m_NumChoices;j++) 
								{
									GetNextQuotedLine(Text, fp, 2048);
									Entry->m_Choices[j] = new char[strlen(Text)+1];
									strcpy(Entry->m_Choices[j], Text);
								}
							}
						}
					}
				}
			}
		}
	}

	fclose(fp);
	return true;
}

bool cScript::Load(string filename)
{
	m_NumActions = 0;
	if(m_ScriptParent)
		delete m_ScriptParent;
	m_ScriptParent = 0;

	// load the script
	m_ScriptParent = LoadScriptFile(filename);
	return true;
}

bool SaveScriptFile(const char *Filename, sScript *ScriptRoot)
{
	FILE *fp;
	long i, j, NumActions;
	sScript *ScriptPtr;

	// Make sure there are some script actions
	if((ScriptPtr = ScriptRoot) == 0)
		return false;

	// Count the number of actions
	NumActions = 0;
	while(ScriptPtr != 0) 
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
	for(i=0;i<NumActions;i++) 
	{
		// Output type of action and # of entries
		fwrite(&ScriptPtr->m_Type, 1, sizeof(long), fp);
		fwrite(&ScriptPtr->m_NumEntries, 1, sizeof(long), fp);

		// Output entry data (if any)
		if(ScriptPtr->m_NumEntries) 
		{
			for(j=0;j<ScriptPtr->m_NumEntries;j++) 
			{
				// Write entry type and data
				fwrite(&ScriptPtr->m_Entries[j].m_Type, 1,sizeof(long),fp);
				fwrite(&ScriptPtr->m_Entries[j].m_IOValue,1,sizeof(long),fp);
				fwrite(&ScriptPtr->m_Entries[j].m_Var,1,sizeof(unsigned char),fp);

				// Write text entry (if any)
				if(ScriptPtr->m_Entries[j].m_Type == _TEXT && ScriptPtr->m_Entries[j].m_Text != NULL)
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
