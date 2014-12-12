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
// Manages resources (images, sounds, fonts etc)

#ifndef __CRESOURCEMANAGER_H
#define __CRESOURCEMANAGER_H

#include "CResource.h"

// Constants
const int MAX_RESOURCES = 30;	// The maximum amount of resources in a list, when it reaches this number one is removed from
								// the end. Note that when a resource is used, it is loaded onto the top of the list.
								// This means the only one removed is the least used resource
const int RESOURCE_LIFETIME = 600000;	// 10mins, if a resource hasn't been used in this time it is removed from list
//const int RESOURCE_LIFETIME = 3000;

// Resource Types
const int NUM_RESOURCES = 2;	// how many resource types there are
const int IMAGE_RESOURCE = 0;
const int SOUND_RESOURCE = 1;

class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	void Free();	// Destroyes all lists
	void Free(int type);	// Frees a specific list

	// Management
	CResource* AddResource(CResource* resource, int type);//, bool loaded);	// Adds a resource to the loaded list and returns pointer to the resource,
															// or null on error, this is called only when loading a resource for the first time
//	void ReloadedResource(CResource* resource, int type);	// tells the resource manager that this resource has be reloaded so it is moved to the loaded list
	void Cull(int type);		// unloads the last resource and moves it to the unloaded list (used when number of loaded resources is too big)
	void Cull(CResource* resource, int type);	// As above but used when resource is old and for a specific resource
	void CullOld(unsigned long currentTime);	// moves through all resources and culls any that havn't been used in a while

//	void MoveToUnloaded(CResource *resource, int type);

//	bool KillResource(CResource* resource, int type);	// removes it from both lists without affecting its data

private:
	CResource* m_Resources[NUM_RESOURCES];	// The parent of each loaded resource
	CResource* m_Last[NUM_RESOURCES];	// The last on the list for each loaded resource
	int m_ResourceCount[NUM_RESOURCES];	// the number of each loaded resource type

//	CResource* m_UnloadedResources[NUM_RESOURCES];	// The parent of each loaded resource
//	CResource* m_UnloadedLast[NUM_RESOURCES];	// The last on the list for each loaded resource
//	int m_UnloadedResourceCount[NUM_RESOURCES];	// the number of each loaded resource type
};

#endif