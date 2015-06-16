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
#include "cImageItem.h"
#include "CGraphics.h"
#include "libintl.h"
#include "cGirls.h"
#include "DirPath.h"
#include "FileList.h"
#include "cInterfaceWindow.h"
#include "SDL_anigif.h"

extern CGraphics	g_Graphics;
extern cGirls		g_Girls;
extern cConfig		cfg;
extern string		stringtolowerj(string name);
extern CLog			g_LogFile;

string numeric = "0123456789 ().,[]-";

string pic_types[] =	// `J` moved this out to global and removed file extensions
{
	// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem > pic_types
	"anal*.", "bdsm*.", "sex*.", "beast*.", "group*.", "les*.", "torture*.",
	"death*.", "profile*.", "combat*.", "oral*.", "ecchi*.", "strip*.", "maid*.", "sing*.",
	"wait*.", "card*.", "bunny*.", "nude*.", "mast*.", "titty*.", "milk*.", "hand*.",
	"foot*.", "bed*.", "farm*.", "herd*.", "cook*.", "craft*.", "swim*.", "bath*.",
	"nurse*.", "formal*.", "shop*.", "magic*.", "sign*.", "presented*.", "dom*.*",
	"preg*.",	// pregnant varients
	"preganal*.", "pregbdsm*.", "pregsex*.", "pregbeast*.", "preggroup*.", "pregles*.",
	"pregtorture*.", "pregdeath*.", "pregprofile*.", "pregcombat*.", "pregoral*.", "pregecchi*.",
	"pregstrip*.", "pregmaid*.", "pregsing*.", "pregwait*.", "pregcard*.", "pregbunny*.", "pregnude*.",
	"pregmast*.", "pregtitty*.", "pregmilk*.", "preghand*.", "pregfoot*.", "pregbed*.", "pregfarm*.",
	"pregherd*.", "pregcook*.", "pregcraft*.", "pregswim*.", "pregbath*.", "pregnurse*.", "pregformal*.",
	"pregshop*.", "pregmagic*.", "pregsign*.", "pregpresented*.", "pregdom*.*"
};
string galtxt[] = 
{
	// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem.cpp > galtxt
	"Anal", "BDSM", "Sex", "Beast", "Group", "Lesbian", "Torture", "Death", "Profile", "Combat",
	"Oral", "Ecchi", "Strip", "Maid", "Sing", "Wait", "Card", "Bunny", "Nude", "Mast", "Titty", "Milk", "Hand",
	"Foot", "Bed", "Farm", "Herd", "Cook", "Craft", "Swim", "Bath", "Nurse", "Formal", "Shop", "Magic", "Sign",
	"Presented", "Dominatrix", 
	"Pregnant",	// pregnant varients
	"Pregnant Anal", "Pregnant BDSM", "Pregnant Sex", "Pregnant Beast", "Pregnant Group",
	"Pregnant Lesbian", "Pregnant Torture", "Pregnant Death", "Pregnant Profile", "Pregnant Combat",
	"Pregnant Oral", "Pregnant Ecchi", "Pregnant Strip", "Pregnant Maid", "Pregnant Sing", "Pregnant Wait",
	"Pregnant Card", "Pregnant Bunny", "Pregnant Nude", "Pregnant Mast", "Pregnant Titty", "Pregnant Milk",
	"Pregnant Hand", "Pregnant Foot", "Pregnant Bed", "Pregnant Farm", "Pregnant Herd", "Pregnant Cook",
	"Pregnant Craft", "Pregnant Swim", "Pregnant Bath", "Pregnant Nurse", "Pregnant Formal", "Pregnant Shop",
	"Pregnant Magic", "Pregnant Sign", "Pregnant Presented", "Pregnant Dominatrix"
};

// Constructors, Deconstructors and Free
cImage::cImage()
{
	m_Surface = 0;
	m_Next = 0;
	m_AniSurface = 0;
}
cImage::~cImage()
{
	if (m_Surface && !m_Surface->m_SaveSurface) delete m_Surface;
	m_Surface = 0;
	//if (m_AniSurface)		delete m_AniSurface;
	m_AniSurface = 0;
	m_Next = 0;
}
cImageItem::cImageItem()
{
	m_Image = 0;
	m_Next = 0;
	m_Surface = 0;
	m_AnimatedImage = 0;
	m_loaded = false;
	m_Hidden = false;
}
cImageItem::~cImageItem()
{
	if (m_Next)		delete m_Next;
	m_Next = 0;
	if (m_Image)	delete m_Image;
	m_Image = 0;
	if (m_Surface)	SDL_FreeSurface(m_Surface);
	m_Surface = 0;
	if (m_AnimatedImage)	delete m_AnimatedImage;
	m_AnimatedImage = 0;
}

/* `J` image tree for each image type
*	tries starts at 40 and counts down until an image is found
*	if preferdefault is true default images will be checked for type returned
*	if preferdefault is false, when tries is less than 10, it will allow default images
*/
int TryImageType(int imagetype, int tries)
{
	int trytype = imagetype;
	/*	the first 10 checks will always try for the asked for type
	*	when tries == 10, it will try one last time for the asked for type
	*	when tries goes below 10 it will start looking for defaults
	*	9 will always look for the asked for type
	*	when it gets down to the last checks it will look for profile
	*/
	if (tries > 30 || tries == 9 || tries == 10 || tries == 0)	return trytype;
	if (tries == 1)	return IMGTYPE_PROFILE;

	int t = g_Dice.d100();
	// for 11 through 30 and 2 trhough 8, some choices need to be made for each type
	switch (trytype)
	{
	case IMGTYPE_ANAL:
	case IMGTYPE_BDSM:
	case IMGTYPE_BEAST:
	case IMGTYPE_GROUP:
	{
		if (tries > 6 && tries < 10)	return trytype;

		if (t < 5)	return IMGTYPE_NUDE;
		if (t < 10)	return IMGTYPE_STRIP;
		if (t < 15)	return IMGTYPE_ECCHI;
		return IMGTYPE_SEX;
	}break;
	case	IMGTYPE_PREGBDSM:
	case	IMGTYPE_PREGANAL:
	case	IMGTYPE_PREGBEAST:
	case	IMGTYPE_PREGGROUP:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 5)	return IMGTYPE_PREGECCHI;
			if (t < 10)	return IMGTYPE_PREGSTRIP;
			if (t < 15)	return IMGTYPE_PREGNUDE;
			if (t < 30)	return IMGTYPE_PREGBDSM;
			if (t < 50)	return IMGTYPE_PREGANAL;
			return IMGTYPE_PREGSEX;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 5)	return IMGTYPE_ECCHI;
			if (t < 10)	return IMGTYPE_STRIP;
			if (t < 15)	return IMGTYPE_NUDE;
			if (t < 30)	return IMGTYPE_BDSM;
			if (t < 50)	return IMGTYPE_ANAL;
			return IMGTYPE_SEX;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_SEX:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 10)	return IMGTYPE_STRIP;
		if (t < 20)	return IMGTYPE_ECCHI;
		if (t < 30)	return IMGTYPE_ANAL;
		if (t < 35)	return IMGTYPE_ORAL;
		if (t < 40)	return IMGTYPE_TITTY;
		if (t < 45)	return IMGTYPE_HAND;
		if (t < 50)	return IMGTYPE_FOOT;
		return IMGTYPE_NUDE;
	}break;
	case	IMGTYPE_PREGSEX:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 5)	return IMGTYPE_PREGFOOT;
			if (t < 10)	return IMGTYPE_PREGHAND;
			if (t < 15)	return IMGTYPE_PREGTITTY;
			if (t < 20)	return IMGTYPE_PREGORAL;
			if (t < 25)	return IMGTYPE_PREGECCHI;
			if (t < 35)	return IMGTYPE_PREGSTRIP;
			if (t < 50)	return IMGTYPE_PREGNUDE;
			return IMGTYPE_PREGANAL;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 5)	return IMGTYPE_FOOT;
			if (t < 10)	return IMGTYPE_HAND;
			if (t < 15)	return IMGTYPE_TITTY;
			if (t < 20)	return IMGTYPE_ORAL;
			if (t < 25)	return IMGTYPE_ECCHI;
			if (t < 35)	return IMGTYPE_STRIP;
			if (t < 50)	return IMGTYPE_NUDE;
			return IMGTYPE_ANAL;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_PREGLESBIAN:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 20)	return IMGTYPE_PREGSTRIP;
			if (t < 40)	return IMGTYPE_PREGECCHI;
			return IMGTYPE_PREGNUDE;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 20)	return IMGTYPE_STRIP;
			if (t < 40)	return IMGTYPE_ECCHI;
			return IMGTYPE_NUDE;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;
	case	IMGTYPE_LESBIAN:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 20)	return IMGTYPE_STRIP;
		if (t < 40)	return IMGTYPE_ECCHI;
		return IMGTYPE_NUDE;
	}break;


	case	IMGTYPE_ORAL:
	case	IMGTYPE_TITTY:
	case	IMGTYPE_HAND:
	case	IMGTYPE_FOOT:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 10)	return IMGTYPE_ORAL;
		if (t < 20)	return IMGTYPE_TITTY;
		if (t < 30)	return IMGTYPE_HAND;
		if (t < 40)	return IMGTYPE_FOOT;
		if (t < 50)	return IMGTYPE_STRIP;
		if (t < 60)	return IMGTYPE_ECCHI;
		return IMGTYPE_NUDE;
	}break;
	case	IMGTYPE_PREGTITTY:
	case	IMGTYPE_PREGORAL:
	case	IMGTYPE_PREGHAND:
	case	IMGTYPE_PREGFOOT:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 5)	return IMGTYPE_PREGFOOT;
			if (t < 10)	return IMGTYPE_PREGHAND;
			if (t < 20)	return IMGTYPE_PREGTITTY;
			if (t < 40)	return IMGTYPE_PREGORAL;
			if (t < 55)	return IMGTYPE_PREGECCHI;
			if (t < 70)	return IMGTYPE_PREGSTRIP;
			return IMGTYPE_PREGNUDE;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 5)	return IMGTYPE_FOOT;
			if (t < 10)	return IMGTYPE_HAND;
			if (t < 20)	return IMGTYPE_TITTY;
			if (t < 40)	return IMGTYPE_ORAL;
			if (t < 55)	return IMGTYPE_ECCHI;
			if (t < 70)	return IMGTYPE_STRIP;
			return IMGTYPE_NUDE;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_MAST:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 30)	return IMGTYPE_STRIP;
		if (t < 50)	return IMGTYPE_ECCHI;
		return IMGTYPE_NUDE;
	}break;
	case	IMGTYPE_PREGMAST:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 30)	return IMGTYPE_PREGSTRIP;
			if (t < 50)	return IMGTYPE_PREGECCHI;
			return IMGTYPE_PREGNUDE;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 30)	return IMGTYPE_STRIP;
			if (t < 50)	return IMGTYPE_ECCHI;
			return IMGTYPE_NUDE;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_DOM:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_COMBAT;
	}break;
	case	IMGTYPE_PREGDOM:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGCOMBAT;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_COMBAT;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_ECCHI:
	case	IMGTYPE_STRIP:
	case	IMGTYPE_MILK:
	case	IMGTYPE_BATH:
	case	IMGTYPE_BED:
	case	IMGTYPE_NUDE:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 20)	return IMGTYPE_ECCHI;
		if (t < 50)	return IMGTYPE_STRIP;
		return IMGTYPE_NUDE;
	}break;
	case	IMGTYPE_PREGBED:
	case	IMGTYPE_PREGECCHI:
	case	IMGTYPE_PREGSTRIP:
	case	IMGTYPE_PREGBATH:
	case	IMGTYPE_PREGNUDE:
	case	IMGTYPE_PREGMILK:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 20)	return IMGTYPE_PREGECCHI;
			if (t < 50)	return IMGTYPE_PREGSTRIP;
			return IMGTYPE_PREGNUDE;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 20)	return IMGTYPE_ECCHI;
			if (t < 50)	return IMGTYPE_STRIP;
			return IMGTYPE_NUDE;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_TORTURE:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 20)	return IMGTYPE_DEATH;
		return IMGTYPE_BDSM;
	}break;
	case	IMGTYPE_PREGTORTURE:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 20)	return IMGTYPE_PREGDEATH;
			return IMGTYPE_PREGBDSM;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 20)	return IMGTYPE_DEATH;
			return IMGTYPE_BDSM;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_COMBAT:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_MAGIC;
	}break;
	case	IMGTYPE_PREGCOMBAT:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGMAGIC;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_MAGIC;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_SWIM:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_ECCHI;
	}break;
	case	IMGTYPE_PREGSWIM:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGECCHI;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_ECCHI;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;



	case	IMGTYPE_SING:
	case	IMGTYPE_CARD:
	case	IMGTYPE_BUNNY:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_FORMAL;
	}break;
	case	IMGTYPE_PREGSING:
	case	IMGTYPE_PREGCARD:
	case	IMGTYPE_PREGBUNNY:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGFORMAL;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_FORMAL;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_MAID:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_BUNNY;
	}break;
	case	IMGTYPE_PREGMAID:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGBUNNY;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_BUNNY;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_WAIT:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 50)	return IMGTYPE_BUNNY;
		return IMGTYPE_FORMAL;
	}break;
	case	IMGTYPE_PREGWAIT:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 50)	return IMGTYPE_PREGBUNNY;
			return IMGTYPE_PREGFORMAL;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 50)	return IMGTYPE_BUNNY;
			return IMGTYPE_FORMAL;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_COOK:
	{
		if (tries > 6 && tries < 10)	return trytype;
		if (t < 70)	return IMGTYPE_WAIT;
		return IMGTYPE_MAID;
	}break;
	case	IMGTYPE_PREGCOOK:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			if (t < 20)	return IMGTYPE_PREGWAIT;
			return IMGTYPE_PREGMAID;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			if (t < 20)	return IMGTYPE_WAIT;
			return IMGTYPE_MAID;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case	IMGTYPE_HERD:
	case	IMGTYPE_CRAFT:
	{
		if (tries > 6 && tries < 10)	return trytype;
		return IMGTYPE_FARM;
	}break;
	case	IMGTYPE_PREGHERD:
	case	IMGTYPE_PREGCRAFT:
	{
		if (tries == 8)	return trytype;					// try with defaults
		if (tries > 20 || tries == 6 || tries == 7)		// try preg vars
		{
			return IMGTYPE_PREGFARM;
		}
		if (tries > 14 || tries == 4 || tries == 5)	// try nonpreg vars
		{
			return IMGTYPE_FARM;
		}
		// try profile
		if (tries > 13 || tries == 3) return IMGTYPE_PREGPROFILE;
		if (tries > 11 || tries == 2) return IMGTYPE_PREGNANT;
	}break;

	case IMGTYPE_PREGNANT:
	{
		if (tries > 15) tries = 15;
		if (tries < 5)	return IMGTYPE_PROFILE;
		else 			return IMGTYPE_PREGPROFILE;
	}break;




	// any preg varients not coded in yet just returns pregnant






	//	Pregnant versions of anything that only has profile as an alternative
	case IMGTYPE_PREGFARM:
	case IMGTYPE_PREGPRESENTED:
	case IMGTYPE_PREGNURSE:
	case IMGTYPE_PREGFORMAL:
	case IMGTYPE_PREGMAGIC:
	case IMGTYPE_PREGDEATH:
	case IMGTYPE_PREGPROFILE:
	case IMGTYPE_PREGSHOP:
	case IMGTYPE_PREGSIGN:
	{
		/* */if (tries > 25)	return IMGTYPE_PREGPROFILE;
		else if (tries > 20)	return IMGTYPE_PREGNANT;
		else if (tries > 10)	return trytype - PREG_OFFSET;	// the nonpregnant version
		else if (tries > 6)/**/	return IMGTYPE_PREGPROFILE;
		else if (tries > 3)/**/	return IMGTYPE_PREGNANT;
		else /*              */	return trytype - PREG_OFFSET;	// the nonpregnant version
	}break;

	//	anything that only has profile as an alternative
	case IMGTYPE_FARM:
	case IMGTYPE_PRESENTED:
	case IMGTYPE_NURSE:
	case IMGTYPE_FORMAL:
	case IMGTYPE_MAGIC:
	case IMGTYPE_DEATH:
	case IMGTYPE_PROFILE:
	case IMGTYPE_SHOP:
	case IMGTYPE_SIGN:
	default:
		return IMGTYPE_PROFILE;
		break;

	}

	return IMGTYPE_PROFILE;	// if all else fails just try profile
}

// `J` Totally new method for image handling for .06.02.00
void cInterfaceWindow::PrepareImage(int id, sGirl* girl, int imagetype, bool rand, int ImageNum, bool gallery, string ImageName)
{
	if (!girl)	return;	// no girl, no images
	string girlName = girl->m_Name;

	int dir = 0; DirPath usedir = "";
	DirPath imagedirCc = DirPath(cfg.folders.characters().c_str()) << girlName;	// usedir = 1
	DirPath imagedirCo = DirPath() << "Resources" << "Characters" << girlName;	// usedir = 2
	DirPath imagedirDc = DirPath(cfg.folders.defaultimageloc().c_str());		// usedir = -1
	DirPath imagedirDo = DirPath() << "Resources" << "DefaultImages";			// usedir = -2
	FileList tiCc(imagedirCc, "*.*");
	FileList tiCo(imagedirCo, "*.*");
	FileList tiDc(imagedirDc, "*.*");
	FileList tiDo(imagedirDo, "*.*");
	int totalimagesCc = tiCc.size();
	int totalimagesCo = tiCo.size();
	int totalimagesDc = tiDc.size();
	int totalimagesDo = tiDo.size();
	if (totalimagesCc + totalimagesCo + totalimagesDc + totalimagesDo < 1)	// no images at all so return a blank image
	{
		m_Images[id]->m_Image = new CSurface(ImagePath("blank.png"));
		m_Images[id]->m_AnimatedImage = 0;
		m_Images[id]->m_Image->m_Message = "No image found.";
		return;
	}

	int tries = 40;
	if (gallery) tries = 0;
	else	// try some corrections
	{
		if (cfg.folders.preferdefault() || totalimagesCc + totalimagesCo < 1)	tries = 10;
		if (imagetype < 0 || imagetype > NUM_IMGTYPES)				imagetype = IMGTYPE_PROFILE;
		if (girl->is_pregnant() && imagetype < IMGTYPE_PREGNANT)	imagetype += PREG_OFFSET;
		if (!girl->is_pregnant() && imagetype == IMGTYPE_PREGNANT)	imagetype = IMGTYPE_PROFILE;
		if (!girl->is_pregnant() && imagetype > IMGTYPE_PREGNANT)	imagetype -= PREG_OFFSET;
	}

	CSurface* image;
	cAnimatedSurface* aimage = new cAnimatedSurface();
	m_Images[id]->m_Image->m_Message = "";
	string file = "";
	string filename = "";
	string ext = "";

	
	bool imagechosen = false;
	do
	{
		int tryimagetype = TryImageType(imagetype, tries);

		// choose an image folder
		dir = 0; usedir = "";
		string checkfor = pic_types[tryimagetype] + "*";
		if (tryimagetype == IMGTYPE_PREGNANT) checkfor = "pregnant*.*";

		if (totalimagesCc > 0)
		{
			FileList testall(imagedirCc, checkfor.c_str());
			if (tryimagetype == IMGTYPE_PREGNANT)
			{
				for (u_int i = 0; i < numeric.size(); i++)
				{
					string t = ("preg" + numeric.substr(i, 1) + "*.*");
					testall.add(t.c_str());
				}
			}
			if (testall.size() > 0)
			{
				usedir = imagedirCc;
				dir = 1;
			}
		}
		if (dir == 0 && totalimagesCo > 0)	// if config is not found, check for images in the original folder
		{
			FileList testall(imagedirCo, checkfor.c_str());
			if (tryimagetype == IMGTYPE_PREGNANT)
			{
				for (u_int i = 0; i < numeric.size(); i++)
				{
					string t = ("preg" + numeric.substr(i, 1) + "*.*");
					testall.add(t.c_str());
				}
			}
			if (testall.size() > 0)
			{
				usedir = imagedirCo;
				dir = 2;
			}
		}
		if (dir == 0 && gallery)	// gallery stops here if there are no images
		{
			m_Images[id]->m_Image->m_Message = "No images found.";
			break;
		}

		// if neither character folder has what we are looking for try the defaults
		if (totalimagesDc > 0 && dir == 0 && tries < 10)
		{
			FileList testall(imagedirDc, checkfor.c_str());
			if (tryimagetype == IMGTYPE_PREGNANT)
			{
				for (u_int i = 0; i < numeric.size(); i++)
				{
					string t = ("preg" + numeric.substr(i, 1) + "*.*");
					testall.add(t.c_str());
				}
			}
			if (testall.size() > 0)
			{
				usedir = imagedirDc;
				dir = -1;
			}
		}
		if (totalimagesDo > 0 && dir == 0 && tries < 10)
		{
			FileList testall(imagedirDo, checkfor.c_str());
			if (tryimagetype == IMGTYPE_PREGNANT)
			{
				for (u_int i = 0; i < numeric.size(); i++)
				{
					string t = ("preg" + numeric.substr(i, 1) + "*.*");
					testall.add(t.c_str());
				}
			}
			if (testall.size() > 0)
			{
				usedir = imagedirDo;
				dir = -2;
			}
		}
		if (dir == 0)
		{
			m_Images[id]->m_Image->m_Message = "No images found.";
			continue;
		}

		FileList testall(usedir, checkfor.c_str());
		if (tryimagetype == IMGTYPE_PREGNANT)
		{
			for (u_int i = 0; i < numeric.size(); i++)
			{
				string t = ("preg" + numeric.substr(i, 1) + "*.*");
				testall.add(t.c_str());
			}
		}
		if (testall.size() <= 0) continue;
		int num = ImageNum;
		if (rand || ImageNum < 0 || ImageNum >= testall.size()) num = g_Dice%testall.size();
		file = testall[num].full();
		filename = testall[num].leaf();
		ext = stringtolowerj(filename.substr(filename.find_last_of('.') + 1, filename.size()));

		if (ext == "ani")
		{
			DirPath anidir = usedir; anidir << "ani";
			string name = filename;
			name.erase(name.size() - 4, 4);
			name += ".jpg";
			FileList testani(anidir, name.c_str());
			if (testani.size() <= 0)
			{
				if (gallery)
				{
					image = new CSurface(ImagePath("blank.png"));
					m_Images[id]->m_Image = image;
					m_Images[id]->m_AnimatedImage = 0;
					m_Images[id]->m_Image->m_Message = "Bad ani file: Missing its matching jpg file: " + file;
					return;
				}
				continue;
			}
			anidir << name;
			cImage* newImage = new cImage();
			newImage->m_Surface = new CSurface();
			newImage->m_Surface->LoadImage(anidir.c_str());
			newImage->m_AniSurface = new cAnimatedSurface();
			int numFrames, speed, aniwidth, aniheight;
			ifstream input;
			input.open(file.c_str());
			if (!input)
			{
				g_LogFile.ss() << "Incorrect data file given for animation - " << file; g_LogFile.ssend();
				if (gallery)
				{
					image = new CSurface(ImagePath("blank.png"));
					m_Images[id]->m_Image = image;
					m_Images[id]->m_AnimatedImage = 0;
					m_Images[id]->m_Image->m_Message = "Bad ani file: Incorrect data file given for animation: " + file;
					return;
				}
			}
			else
			{
				input >> numFrames >> speed >> aniwidth >> aniheight;
				aimage->SetData(0, 0, numFrames, speed, aniwidth, aniheight, newImage->m_Surface);
				imagechosen = true;
				m_Images[id]->m_Image->m_Message = file;
			}
			input.close();

		}
		else if (ext == "gif")
		{
			// temp code to just get it loaded
			image = new CSurface(file);
			imagechosen = true;
		}
		else
		{
			image = new CSurface(file);
			imagechosen = true;
		}
	}	while (!imagechosen && --tries > 0);

	if (m_Images[id]->m_Image->m_Message == "")
	{
		m_Images[id]->m_Image->m_Message = m_Images[id]->m_Image->GetFilename();
	}

	if (ext == "")	// unrecognised extension
	{
		image = new CSurface(ImagePath("blank.png"));
		m_Images[id]->m_Image = image;
		m_Images[id]->m_AnimatedImage = 0;
		m_Images[id]->m_Image->m_Message = "No image found.";
	}
	else if (ext == "ani")
	{
		m_Images[id]->m_AnimatedImage = aimage;
	}
	else if (ext == "gif")
	{
		// temp code to just get it loaded
		m_Images[id]->m_Image = image;
		m_Images[id]->m_AnimatedImage = 0;
	}
	else
	{
		m_Images[id]->m_Image = image;
		m_Images[id]->m_AnimatedImage = 0;
	}


}

bool cImageItem::CreateImage(int id, string filename, int x, int y, int width, int height, bool statImage, int R, int G, int B)
{
	m_ID = id;
	if (statImage)
	{
		m_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
		SDL_FillRect(m_Surface, 0, SDL_MapRGB(m_Surface->format, R, G, B));
	}
	SetPosition(x, y, width, height);

	if (filename != "")
	{
		m_loaded = true;
		m_Image = new CSurface(filename);
		m_Image->SetAlpha(true);
	}
	else
		m_loaded = false;

	m_Next = 0;

	return true;
}

bool cImageItem::CreateAnimatedImage(int id, string filename, string dataFilename, int x, int y, int width, int height)
{
	m_ID = id;
	SetPosition(x, y, width, height);

	if (filename != "")
	{
		m_loaded = true;
		m_Image = new CSurface(filename);
		m_Image->SetAlpha(true);

		// load the animation
		m_AnimatedImage = new cAnimatedSurface();
		int numFrames, speed, aniwidth, aniheight;
		ifstream input;
		input.open(dataFilename.c_str());
		if (!input)
		{

			g_LogFile.ss() << "Incorrect data file given for animation - " << dataFilename; g_LogFile.ssend();
			return false;
		}
		else
			input >> numFrames >> speed >> aniwidth >> aniheight;
		m_AnimatedImage->SetData(0, 0, numFrames, speed, aniwidth, aniheight, m_Image);
		input.close();
	}
	else
	{
		g_LogFile.ss() << "Incorrect image file given for animation"; g_LogFile.ssend();
		m_loaded = false;
		return false;
	}

	m_Next = 0;

	return true;
}

void cImageItem::Draw()
{
	if (m_Hidden)
		return;

	if (m_AnimatedImage)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;

		m_AnimatedImage->DrawFrame(m_XPos, m_YPos, m_Width, m_Height, g_Graphics.GetTicks());
	}
	else if (m_Image)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;

		m_Image->DrawSurface(m_XPos, m_YPos, 0, &rect, true);
	}
	else if (m_Surface)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Surface, 0, g_Graphics.GetScreen(), &offset);
	}
}
