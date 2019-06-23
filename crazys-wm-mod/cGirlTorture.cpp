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

#include "cGirls.h"
#include "cGirlTorture.h"
#include "cMessageBox.h"
#include "cBrothel.h"
#include "cDungeon.h"
#include "cGirlGangFight.h"
#include "libintl.h"

extern	cRng			g_Dice;
extern	cGirls			g_Girls;
extern	cMessageQue		g_MessageQue;
extern	cBrothelManager	g_Brothels;
extern	bool			g_Cheats;
extern	int				g_CurrBrothel;
extern	CLog			g_LogFile;
extern	cPlayer*		The_Player;

/*
* ideally, we'd keep a queue of message strings and
* pop them out in order at the end
*/
cGirlTorture::~cGirlTorture()		// deconstructor
{
	int color = COLOR_BLUE;
	if (m_Girl->m_RunAway != 0) color = COLOR_RED;

	// Display any outstanding messages
	if (!m_Message.empty())
	{
		if (m_TorturedByPlayer)
		{
			g_MessageQue.AddToQue(m_Message, color);
			m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);	// `J` added
		}
		else
		{
			if (m_Girl->health()>0)		// Make sure girl is alive
				m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);
		}
	}
}

cGirlTorture::cGirlTorture(sGirl* a_girl)		// Torture girl by player
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture(sGirl* a_girl)"; g_LogFile.ssend(); }
	// Init for DoTorture()
	m_TorturedByPlayer = true;
	m_Dungeon = g_Brothels.GetDungeon();
	m_Girl = a_girl;
	int nTemp = m_Dungeon->GetGirlPos(m_Girl);
	if (nTemp > -1)
	{
		m_DungeonGirl = m_Dungeon->GetGirl(nTemp);
		DoTorture();
		return;
	}

	// Sanity check for Girl is not in dungeon
	m_Message += m_Girl->m_Realname;
	m_Message += " cannot be tortured unless in the dungeon!";
}

cGirlTorture::cGirlTorture(sDungeonGirl* a_girl)	// Torture Dungeon girl by player
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture(sDungeonGirl* a_girl)"; g_LogFile.ssend(); }
	// Init for DoTorture()
	m_TorturedByPlayer = true;
	m_DungeonGirl = a_girl;
	m_Girl = m_DungeonGirl->m_Girl;
	m_Dungeon = g_Brothels.GetDungeon();

	DoTorture();
}

cGirlTorture::cGirlTorture(sDungeonGirl* a_girl, sGirl* Tourturer)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::cGirlTorture(sDungeonGirl* a_girl, sGirl* Tourturer)"; g_LogFile.ssend(); }
	// Init for DoTorture()
	m_TorturedByPlayer = false;
	m_DungeonGirl = a_girl;
	m_Torturer = Tourturer;
	m_Girl = m_DungeonGirl->m_Girl;
	m_Dungeon = g_Brothels.GetDungeon();

	DoTorture();
}

void cGirlTorture::DoTorture()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::DoTorture()"; g_LogFile.ssend(); }
	/*
	*	These variables to be initilisied befor calling DoTorture()
	*		bool			m_TorturedByPlayer
	*		sGirl			*m_Girl;
	*		sGirl			*m_Torturer;
	*		sDungeonGirl	*m_DungeonGirl;
	*		Dungeon			*m_Dungeon;
	*/

	// Sanity check. Abort on dead girl
	if (m_Girl->is_dead()) return;

	m_Fight = false;

	string sGirlName = m_Girl->m_Realname;
	string sMsg = "";

	if (cfg.debug.log_torture())
	{
		if (m_TorturedByPlayer) g_LogFile.ss() << "\ncGirlTorture: Player is torturing " << sGirlName << ".";
		else g_LogFile.ss() << "\ncGirlTorture: " << m_Torturer->m_Realname << " is torturing " << sGirlName << ".";
		g_LogFile.ssend();
	}

	// clear down the message and start with her name
	m_Message = "";
	if (m_TorturedByPlayer) m_Message = sGirlName + ": ";

	// WD	Don't allow girls to be tortured by both the Player and the Torturer
	if (m_Girl->m_Tort && !g_Cheats)		// only allow this once a week unless cheating
	{
		if (m_TorturedByPlayer) m_Message += "You may only torture someone once per week.\n";
		else m_Message += sGirlName + " has already been tortured this week.\n";
		return;
	}

	// Don't torture new mums
	if (m_Girl->m_JustGaveBirth)
	{
		if (m_TorturedByPlayer)
		{
			m_Message += "She has given birth and has the week off. So she and will not be tortured.\n";
		}
		else
		{
			sMsg = sGirlName + " gave birth and had the week off so was not tortured this week.\n";
			m_Message += "Since " + sGirlName + " gave birth she was not tortured this week.\n";
			//m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
			m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
		return;
	}

	m_Girl->m_Tort = true;
	if (!m_TorturedByPlayer) m_Dungeon->NumGirlsTort(+1);

	/*
	*	OK: she may fight, and if she wins she'll escape
	*
	*	BUG: Intermitent crash if cGirlGangFight() is called when tortured by Girl not Player
	*
	*	WD:	To balance this halve chance of gaining trait in cGirlTorture::add_trait()
	*/
	if (m_TorturedByPlayer)
	{
		if (girl_escapes()) return;
	}

	/*
	*	on the grounds that while intending to torture someone is bad actually doing it is worse still...
	*
	*	also if she's not a slave that's a bit worse still
	*	(allowing players who need to discipline their slaves a bit of a break...)
	*
	*	WD	With changes to cPlayer stats how quickly you gain Evil depends on how evil you currently are. Limited to min of 1 point.
	*
	*/
	if (m_TorturedByPlayer)
	{
		The_Player->evil(m_Girl->is_slave() ? 5 : 10);
	}
	else	// Tortured by Girl
	{
		The_Player->evil(m_Girl->is_slave() ? 2 : 4);
	}
	/*
	*	now add one of a number of torture messages...
	*/
	if (m_TorturedByPlayer) AddTextPlayer();
	else/*               */	AddTextTorturerGirl();
	/*
	*	check for injury
	*/
	if (IsGirlInjured(3))
	{
		if (m_TorturedByPlayer)
		{
			m_Message += " You were a little too heavy handed and badly injure her.\n";
		}
		else
		{
			sMsg = sGirlName + " was seriously injured in the dungeon this week.\n";
			m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
			m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
		}
	}
	/*
	*	hmmm... not entirely sure this needs to be a class
	*/
	UpdateStats();
	UpdateTraits();

	// `J` wear down rebellion
	g_Girls.updateTempTraits(m_Girl, "Kidnapped", -1);
	g_Girls.updateTempTraits(m_Girl, "Emprisoned Customer", -1);

	/*
	*	DANGER DEATH and low health warnings
	*
	*/
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || DANGER DEATH"; g_LogFile.ssend(); }
	if (m_TorturedByPlayer)
	{
		if (m_Girl->is_dead())		// Dead Girl
			m_Message += "She unfortunatly died from her wounds.\n";
		else if (m_Girl->health() < 20)
			m_Message += "Also, she is close to death.\n";

	}
	else	// Tortured by Torturer Girl
	{
		if (m_Girl->is_dead())
		{
			sMsg = "While torturing " + sGirlName + " in the dungeon she died from her wounds.";
			m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
		}
		else if (m_Girl->health() < 20)
		{
			// if she is on this low health the tortuer will start feeding again
			if (!m_DungeonGirl->m_Feeding)
			{
				m_DungeonGirl->m_Feeding = true;
				sMsg = m_Torturer->m_Realname + " has allowed " + sGirlName +
					"  to eat because her health was low.\n";
				m_Message += m_Torturer->m_Realname +
					" was allowed her food because her health was low.\n";
				m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_DANGER);
				m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
			}
			else
			{
				sMsg = sGirlName + "  health is low from ongoing torture.";
				// WD	Low health warnings done as part of cDungeon::Update()
				//m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_DANGER);
				m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DANGER);
			}

		}
	}
	if (cfg.debug.log_torture()) { g_LogFile.ss() << "cGirlTorture: " << sGirlName << " torture completed!\n"; g_LogFile.ssend(); }
}

void cGirlTorture::AddTextPlayer()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::AddTextPlayer()"; g_LogFile.ssend(); }
	bool was, is;

	int mes = g_Dice.in_range(0, 4);
	switch (mes) {
	case 0:
		m_Message += "you torture her for hours leaving her sobbing.\n";
		break;
	case 1:
		m_Message += "you enjoy giving out all manners of pain imaginable.\n";
		break;
	case 2:
		m_Message += "you place a small wormlike creature called a vorm in her pussy and watch as it painfully sucks fluid from her.\n";
		break;
	case 3:
		m_Message += "after beating her around and using a few torture devices, you sit and watch her crying and cowering in the corner for a time.\n";
		break;
	case 4:
	default:
		m_Message += "you rape her many times making sure she is as uncomfotable as possible.\n";
		/*
		*		see if she was preggers before the rape
		*		check to see if the rape made her pregnant
		*		then check to see if she is afterward
		*/
		was = m_Girl->is_pregnant();
		/*
		*		supposed to be checking to see if
		*		she's preg by the goons (at group sex rates)
		*		then again by player. But I don't have a handy
		*		sCustomer so I'm going group rate
		*		and assigning any progeny to the player.
		*		Lazy, I know :)
		*/
		m_Girl->calc_pregnancy(The_Player, false, 1.5);
		is = m_Girl->is_pregnant();
		/*
		*		if she was not, but is now, then the player
		*		just knocked her up. We should mention this.
		*/
		if (is && !was)
		{
			m_Message += " She is now pregnant.";
		}
		m_Girl->lose_virginity();
		break;
	}
}

void cGirlTorture::AddTextTorturerGirl()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::AddTextTorturerGirl()"; g_LogFile.ssend(); }
	/*
	*		Sumary messages for Torture by girl
	*/
	if (m_DungeonGirl->m_Feeding)
		m_Message += m_Girl->m_Realname + " has been tortured.\n";

	else
		m_Message += m_Girl->m_Realname + " has been tortured and starved.\n";

}

void cGirlTorture::UpdateStats()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::UpdateStats()"; g_LogFile.ssend(); }
	/*
	*	WD Stats based on ortiginal torture job code
	*
	*/
	// do heavy torture
	if (cfg.initial.torture_mod() < 0) // `J`  added
	{
		m_Girl->health(-7);
		m_Girl->happiness(-7);
		m_Girl->constitution(1 + g_Dice % 5); // 1 ~ 5
		m_Girl->confidence(-7);
		m_Girl->obedience(9 + g_Dice % 3);    // 9 ~ 11
		m_Girl->spirit(-4 - g_Dice % 3);      // -4 ~ -6
		m_Girl->tiredness(-5 - g_Dice % 3);   // -5 ~ -7
		m_Girl->pchate(6);
		m_Girl->pclove(-10);
		m_Girl->pcfear(10);
		m_Girl->bdsm(1 + g_Dice % 5);         // 1 ~ 5
	}
	else if (m_Girl->health() > 10)
	{
		m_Girl->health(-5);
		m_Girl->happiness(-5);
		m_Girl->constitution(1);
		m_Girl->confidence(-5);
		m_Girl->obedience(10);
		m_Girl->spirit(-5);
		m_Girl->tiredness(-5);
		m_Girl->pchate(3);
		m_Girl->pclove(-5);
		m_Girl->pcfear(5);
		m_Girl->bdsm(1);
	}

	// do safer torture
	else	//	(m_Girl->health() <= 10)
	{
		m_Girl->happiness(-2);
		m_Girl->confidence(-2);
		m_Girl->obedience(4);
		m_Girl->spirit(-2);
		m_Girl->tiredness(-2);
		m_Girl->pchate(1);
		m_Girl->pclove(-2);
		m_Girl->pcfear(3);
	}
}

bool cGirlTorture::IsGirlInjured(unsigned int unModifier)
{  // modifier: 5 = 5% chance, 10 = 10% chance
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::IsGirlInjured(unsigned int unModifier)"; g_LogFile.ssend(); }

	// Sanity check, Can't get injured
	if (m_Girl->has_trait("Incorporeal")) return false;


	/*
	*	WD	Injury was only possible if girl is pregnant or
	*		hasn't got the required traits.
	*
	*		Now check for injury first
	*		Use usigned int so can't pass negative chance
	*/
	string	sMsg;
	string	sGirlName = m_Girl->m_Realname;
	int		nMod = static_cast<int>(unModifier);
	if (cfg.initial.torture_mod() < 0){ nMod += nMod; }


	if (m_Girl->has_trait("Fragile"))	nMod += nMod;	// nMod *= 2;
	if (m_Girl->has_trait("Tough"))		nMod /= 2;
	if (nMod < 1) nMod = 1;		// `J` always at least a 1% chance

	// Did the girl get injured
	if (!g_Dice.percent(nMod)) return false;
	/*
	*	INJURY PROCESSING
	*	Only injured girls continue past here
	*/

	// Post any outstanding Player messages
	if (m_TorturedByPlayer && !m_Message.empty())
	{
		g_MessageQue.AddToQue(m_Message, 0);
		m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_SUMMARY);	// `J` added

		m_Message = sGirlName + ": ";
	}

	// getting hurt badly could lead to scars
	if (g_Dice.percent(nMod * 2) &&
		!m_Girl->has_trait("Small Scars") &&
		!m_Girl->has_trait("Cool Scars") &&
		!m_Girl->has_trait("Horrific Scars"))
	{
		int chance = g_Dice % 6;
		if (chance == 0)
		{
			m_Girl->add_trait("Horrific Scars", false);
			if (m_TorturedByPlayer)
				m_Message += "She was badly injured, and now has to deal with Horrific Scars.\n";
			else
				MakeEvent(sGirlName + " was badly injured, and now has Horrific Scars.\n");
		}
		else if (chance <= 2)
		{
			m_Girl->add_trait("Small Scars", false);
			if (m_TorturedByPlayer)
				m_Message += "She was injured and now has a couple of Small Scars.\n";
			else
				MakeEvent(sGirlName + " was injured, and now has Small Scars.\n");
		}
		else
		{
			m_Girl->add_trait("Cool Scars", false);
			if (m_TorturedByPlayer)
				m_Message += "She was injured and scarred. As scars go however, at least they are pretty Cool Scars.\n";
			else
				MakeEvent(sGirlName + " was injured and scarred. She now has Cool Scars.\n");
        }
	}

	// in rare cases, she might even lose an eye
	if (g_Dice.percent((nMod / 2)) &&
		!m_Girl->has_trait("One Eye") &&
		!m_Girl->has_trait("Eye Patch"))
	{
		int chance = g_Dice % 3;
		if (chance == 0)
		{
			m_Girl->add_trait("One Eye", false);
			if (m_TorturedByPlayer)
				m_Message += "Oh, no! She was badly injured, and now only has One Eye!\n";
			else
				MakeEvent(sGirlName + " was badly injured and lost an eye.\n");
		}
		else
		{
			m_Girl->add_trait("Eye Patch", false);
			if (m_TorturedByPlayer)
				m_Message += "She was injured and lost an eye, but at least she has a cool Eye Patch to wear.\n";
			else
				MakeEvent(sGirlName + " was injured and lost an eye, but at least she has a cool Eye Patch to wear.\n");
		}
	}

	// or lose tough or become fragile
	if (m_Girl->has_trait("Tough"))
	{
		if (g_Dice.percent(nMod))
		{
			m_Girl->remove_trait("Tough", false);
			if (m_TorturedByPlayer)
				m_Message += "Her body has become less Tough due to the extent of her injuries.\n";
			else
				MakeEvent("Due to " + sGirlName + " injuries her body has become less Tough.\n");
		}
	}
	else if (!m_Girl->has_trait("Fragile"))
	{
		if (g_Dice.percent(nMod / 2))
		{
			m_Girl->add_trait("Fragile", false);
			if (m_TorturedByPlayer)
				m_Message += "Her body has become rather Fragile due to the extent of her injuries.\n";
			else
				MakeEvent("Due to " + sGirlName + " injuries her body has become fragile.\n");
		}
	}

	// and if pregnant, she might lose the baby; I'll assume inseminations can't be aborted so easily
	if ((m_Girl->carrying_human() && g_Dice.percent(nMod * 2)) ||
		(m_Girl->carrying_monster() && g_Dice.percent(nMod)))	// `J` added insemination loss chance
	{  // unintended abortion time
		//injured = true;
		m_Girl->m_ChildrenCount[CHILD08_MISCARRIAGES]++;
		m_Girl->clear_pregnancy();
		m_Girl->happiness(-20);
		m_Girl->spirit(-5);
		if (m_TorturedByPlayer)
			m_Message += "Her unborn child has been lost due to the injuries she sustained, leaving her quite distraught.\n";
		else
			MakeEvent("Due to " + sGirlName + " injuries she has had a miscarriage, leaving her quite distraught.\n");
	}


	int damage = 5 + g_Dice % 10;								// Lose between 5 - 14 hp
	if (cfg.initial.torture_mod() < 0){ damage += damage / 2; }	// `J` Lose between 7 - 21 hp if harsh torture
	m_Girl->health(-damage);


	// Post any new Player messages in Red Message Box Colour 1
	if (m_TorturedByPlayer && !m_Message.empty() && m_Message != sGirlName + ": ")
	{
		g_MessageQue.AddToQue(m_Message, COLOR_RED);
		m_Girl->m_Events.AddMessage(m_Message, IMGTYPE_TORTURE, EVENT_DAYSHIFT);	// `J` added

		m_Message = sGirlName + ": ";
	}

	return true;
}

bool cGirlTorture::girl_escapes()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::girl_escapes()"; g_LogFile.ssend(); }
	cGirlGangFight ggf(m_Girl);

	if (ggf.girl_submits())
	{
		m_Message += "She doesn't put up a fight so ";
		return false;	// she does NOT escape
	}
	/*
	*	Calculate combat between goons and girl if she decides to fight back
	*/
	m_Fight = true;
	m_Message += "She puts up a fight ";
	if (ggf.girl_lost())
	{
		if (ggf.player_won())
		{
			m_Message += " and would have escaped but for your personal intervention; ";
		}
		else
		{
			m_Message += "but goons defeat her and ";
		}
		return false;
	}

	if (ggf.wipeout())
	{
		m_Message += " the gang is wiped out and";
	}

	// If girl wins she escapes and leaves the brothel
	m_Message += "And after defeating you as well she escapes to the outside world.\n";
	m_Girl->m_RunAway = 6;	// player has 6 weeks to retreive
	if (m_Girl->m_NightJob == JOB_INDUNGEON)
		m_Dungeon->RemoveGirl(m_Girl);
	else
		g_Brothels.RemoveGirl(g_CurrBrothel, m_Girl, false);
	m_Girl->m_NightJob = m_Girl->m_DayJob = JOB_RUNAWAY;
	g_Brothels.AddGirlToRunaways(m_Girl);
	The_Player->evil(5);							// Add evilness for girl telling the tale
	The_Player->suspicion(15);
	return true;
}

void cGirlTorture::UpdateTraits()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::UpdateTraits()"; g_LogFile.ssend(); }
	int nWeekMod = 1;
	bool harshtorture = false;
	/* `J` added to allow permanent trait gain on torture
	if TortureTraitWeekMod is set to a negative number in the config.xml
	then "Broken Will", "Masochist" and "Mind Fucked" are permanent
	doubles chance of injuring girls when torturing them
	evil gained from torture is also doubled  */
	if (cfg.initial.torture_mod() < 0){ harshtorture = true; }
	else { nWeekMod = cfg.initial.torture_mod() * m_DungeonGirl->m_Weeks; }
	if (nWeekMod < 1) nWeekMod = 1;

	if (g_Dice.percent(30) && m_Girl->spirit() < 20 && m_Girl->health() < 20)
	{
		if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || add_trait Broken Will"; g_LogFile.ssend(); }
		if (harshtorture)	{ m_Girl->add_trait("Broken Will", false); }
		else				{ m_Girl->add_trait("Broken Will", int(5 + nWeekMod / 2)); }
	}
	if (g_Dice.percent(30) && m_Girl->bdsm() > 30)
	{
		if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || add_trait Masochist"; g_LogFile.ssend(); }
		if (harshtorture)	{ m_Girl->add_trait("Masochist", false); }
		else				{ m_Girl->add_trait("Masochist", 10 + nWeekMod); }
	}
	if (g_Dice.percent(30) && m_Girl->health() < 10)
	{
		if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || add_trait Mind Fucked"; g_LogFile.ssend(); }
		if (harshtorture)	{ m_Girl->add_trait("Mind Fucked", false); }
		else				{ m_Girl->add_trait("Mind Fucked", 10 + nWeekMod); }
	}
}

void cGirlTorture::add_trait(string trait, int pc)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::add_trait(string trait, int pc)"; g_LogFile.ssend(); }
	if (m_Girl->has_trait(trait)) return;
	/*
	*	WD:	To balance a crash bug workaround for Job Torturer
	*		unable to call GirlGangFight()
	*		Halve chance of gaining trait
	*/
	if (!m_TorturedByPlayer) pc /= 2;
	if (!g_Dice.percent(pc)) return;

	string sMsg = m_Girl->m_Realname + " has gained trait \"" + trait + "\" from being tortured.";

	if (m_TorturedByPlayer)
	{
		g_MessageQue.AddToQue(sMsg, 2);
		m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
	}
	else MakeEvent(sMsg);

	// Add trait
	m_Girl->add_trait(trait);
}

inline void cGirlTorture::MakeEvent(string sMsg)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGirlTorture || cGirlTorture::MakeEvent(string sMsg)"; g_LogFile.ssend(); }
	m_Girl->m_Events.AddMessage(sMsg, IMGTYPE_TORTURE, EVENT_WARNING);
	m_Torturer->m_Events.AddMessage(sMsg, IMGTYPE_PROFILE, EVENT_DUNGEON);
}

