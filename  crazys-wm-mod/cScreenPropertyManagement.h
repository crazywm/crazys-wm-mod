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
#pragma once

#include "DirPath.h"
#include "cInterfaceWindow.h"
#include "InterfaceGlobals.h"

class cScreenPropertyManagement : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// interface/event IDs
	int back_id;			// Back button
	int barstaff_id;		// Bar Staff Cost text
	int barhire_id;			// Hire Bar Staff button
	int barfire_id;			// Fire Bar Staff button
	int casinostaff_id;		// Casino Staff Cost text
	int casinohire_id;		// Hire Casino Staff button
	int casinofire_id;		// Fire Casino Staff button
	int roomcost_id;		// Room Cost text
	int restrict_id;		// Sex Restrictions text

	int curbrothel_id;		// Current Brothel text
	int buyrooms_id;		// Add 5 Rooms button
	int potioncost_id;		// Potion Cost text
	int potionavail_id;		// Available Potions text
	int potions10_id;		// Buy 10 Potions button
	int potions20_id;		// Buy 20 Potions button
	int autopotions_id;		// Auto Buy Potions checkbox
	int noanal_id;			// Prohibit Anal checkbox
	int nobdsm_id;			// Prohibit BDSM checkbox
	int nonormal_id;		// Prohibit Normal Sex checkbox
	int nobeast_id;			// Prohibit Bestiality checkbox
	int nogroup_id;			// Prohibit Group Sex checkbox
	int nolesbian_id;		// Prohibit Lesbian checkbox
	int nooral_id;			// Prohibit Oral checkbox
	int notitty_id;			// Prohibit Titty Sex checkbox
	int nohand_id;			// Prohibit Hand Job checkbox
	int advertsli_id;		// Advertising Budget slider
	int advertamt_id;		// Advertising Budget value output text

	// `J` individual building ids
	int p_b0_id, p_b1_id, p_b2_id, p_b3_id, p_b4_id, p_b5_id, p_b6_id, p_st_id, p_ar_id, p_ce_id, p_cl_id, p_ho_id, p_fa_id;
	int p_b0_addroom, p_b1_addroom, p_b2_addroom, p_b3_addroom, p_b4_addroom, p_b5_addroom, p_b6_addroom, p_st_addroom, p_ar_addroom, p_ce_addroom, p_cl_addroom, p_ho_addroom, p_fa_addroom;
	int p_b0_pot_c, p_b1_pot_c, p_b2_pot_c, p_b3_pot_c, p_b4_pot_c, p_b5_pot_c, p_b6_pot_c, p_st_pot_c, p_ar_pot_c, p_ce_pot_c, p_cl_pot_c, p_ho_pot_c, p_fa_pot_c;
	int p_b0_pot_a, p_b1_pot_a, p_b2_pot_a, p_b3_pot_a, p_b4_pot_a, p_b5_pot_a, p_b6_pot_a, p_st_pot_a, p_ar_pot_a, p_ce_pot_a, p_cl_pot_a, p_ho_pot_a, p_fa_pot_a;
	int p_b0_pot10, p_b1_pot10, p_b2_pot10, p_b3_pot10, p_b4_pot10, p_b5_pot10, p_b6_pot10, p_st_pot10, p_ar_pot10, p_ce_pot10, p_cl_pot10, p_ho_pot10, p_fa_pot10;
	int p_b0_pot20, p_b1_pot20, p_b2_pot20, p_b3_pot20, p_b4_pot20, p_b5_pot20, p_b6_pot20, p_st_pot20, p_ar_pot20, p_ce_pot20, p_cl_pot20, p_ho_pot20, p_fa_pot20;
	int p_b0_pot_b, p_b1_pot_b, p_b2_pot_b, p_b3_pot_b, p_b4_pot_b, p_b5_pot_b, p_b6_pot_b, p_st_pot_b, p_ar_pot_b, p_ce_pot_b, p_cl_pot_b, p_ho_pot_b, p_fa_pot_b;
	int p_b0_noanal, p_b1_noanal, p_b2_noanal, p_b3_noanal, p_b4_noanal, p_b5_noanal, p_b6_noanal, p_st_noanal, p_ar_noanal, p_ce_noanal, p_cl_noanal, p_ho_noanal, p_fa_noanal;
	int p_b0_nobdsm, p_b1_nobdsm, p_b2_nobdsm, p_b3_nobdsm, p_b4_nobdsm, p_b5_nobdsm, p_b6_nobdsm, p_st_nobdsm, p_ar_nobdsm, p_ce_nobdsm, p_cl_nobdsm, p_ho_nobdsm, p_fa_nobdsm;
	int p_b0_nonorm, p_b1_nonorm, p_b2_nonorm, p_b3_nonorm, p_b4_nonorm, p_b5_nonorm, p_b6_nonorm, p_st_nonorm, p_ar_nonorm, p_ce_nonorm, p_cl_nonorm, p_ho_nonorm, p_fa_nonorm;
	int p_b0_nobeast, p_b1_nobeast, p_b2_nobeast, p_b3_nobeast, p_b4_nobeast, p_b5_nobeast, p_b6_nobeast, p_st_nobeast, p_ar_nobeast, p_ce_nobeast, p_cl_nobeast, p_ho_nobeast, p_fa_nobeast;
	int p_b0_nogroup, p_b1_nogroup, p_b2_nogroup, p_b3_nogroup, p_b4_nogroup, p_b5_nogroup, p_b6_nogroup, p_st_nogroup, p_ar_nogroup, p_ce_nogroup, p_cl_nogroup, p_ho_nogroup, p_fa_nogroup;
	int p_b0_noles, p_b1_noles, p_b2_noles, p_b3_noles, p_b4_noles, p_b5_noles, p_b6_noles, p_st_noles, p_ar_noles, p_ce_noles, p_cl_noles, p_ho_noles, p_fa_noles;
	int p_b0_nooral, p_b1_nooral, p_b2_nooral, p_b3_nooral, p_b4_nooral, p_b5_nooral, p_b6_nooral, p_st_nooral, p_ar_nooral, p_ce_nooral, p_cl_nooral, p_ho_nooral, p_fa_nooral;
	int p_b0_notitty, p_b1_notitty, p_b2_notitty, p_b3_notitty, p_b4_notitty, p_b5_notitty, p_b6_notitty, p_st_notitty, p_ar_notitty, p_ce_notitty, p_cl_notitty, p_ho_notitty, p_fa_notitty;
	int p_b0_nohand, p_b1_nohand, p_b2_nohand, p_b3_nohand, p_b4_nohand, p_b5_nohand, p_b6_nohand, p_st_nohand, p_ar_nohand, p_ce_nohand, p_cl_nohand, p_ho_nohand, p_fa_nohand;
	int p_b0_advslid, p_b1_advslid, p_b2_advslid, p_b3_advslid, p_b4_advslid, p_b5_advslid, p_b6_advslid, p_st_advslid, p_ar_advslid, p_ce_advslid, p_cl_advslid, p_ho_advslid, p_fa_advslid;
	int p_b0_advamnt, p_b1_advamnt, p_b2_advamnt, p_b3_advamnt, p_b4_advamnt, p_b5_advamnt, p_b6_advamnt, p_st_advamnt, p_ar_advamnt, p_ce_advamnt, p_cl_advamnt, p_ho_advamnt, p_fa_advamnt;

	void set_ids();
public:
	cScreenPropertyManagement();
	~cScreenPropertyManagement();

	void init();
	void process();
	void check_events();
};
