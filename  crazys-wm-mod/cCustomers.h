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
#ifndef __CCUSTOMERS_H
#define __CCUSTOMERS_H

#include "cGirls.h"

struct sBrothel;

// Individual customers are randomly generated
typedef struct sCustomer
{
	// Regular Stats
	bool m_IsWoman;		// 0 means a man, 1 means a woman
	int m_Amount;			// how many customers this represents
	int m_Class;			// is the person rich, poor or middle class
	bool m_Official;		// is the person an official of the town

	int m_Money;

	bool m_HasAIDS;					// `J` Does the customer have AIDS?
	bool m_HasChlamydia;			// `J` Does the customer have Chlamydia?
	bool m_HasSyphilis;				// `J` Does the customer have Syphilis?
	bool m_HasHerpes;				// `J` Does the customer have Herpes?

	int m_Stats[NUM_STATS];
	int m_Skills[NUM_SKILLS];

	// `J` What the customers want to do in the building
	unsigned char m_GoalA;			// Primary goal
	unsigned char m_GoalB;			// Secondary goal
	unsigned char m_GoalC;			// Backup goal

	unsigned char m_Fetish;			// the customers fetish
	unsigned char m_SexPref;		// their sex preference
	unsigned char m_SexPrefB;		// their secondary sex preference

	unsigned char m_ParticularGirl;	// the id of the girl he wants

	sCustomer* m_Next;
	sCustomer* m_Prev;

	sCustomer();
	~sCustomer();
	int happiness()
	{
		return m_Stats[STAT_HAPPINESS];
	}
}sCustomer;

// Customer base
class cCustomers
{
public:
	cCustomers();
	~cCustomers();

	void Free();

	void GenerateCustomers(sBrothel& brothel, bool Day0Night1 = SHIFT_DAY);	// generates a random amount of possible customers based on the number of poor, rich, and middle class
	sCustomer* CreateCustomer(sBrothel& brothel);

	//	sCustomer* GetParentCustomer();		// Gets a random customer from the customer base
	sCustomer GetCustomer(sBrothel& brothel); // Creates a new customer.
	void ChangeCustomerBase();	// Changes customer base, it is based on how much money the player is bring into the town
	int GetNumCustomers() { return m_NumCustomers; }
	void AdjustNumCustomers(int amount) { m_NumCustomers += amount; }
	void Add(sCustomer* cust);
	void Remove(sCustomer* cust);
	void SetGoals(sCustomer* cust);
	//	int GetHappiness();	//mod

private:
	int m_Poor;		// percentage of poor people in the town
	int m_Middle;	// percentage of middle class people in the town
	int m_Rich;		// percentage of rich people in the town

	int m_NumCustomers;
	sCustomer* m_Parent;
	sCustomer* m_Last;
};

#endif
