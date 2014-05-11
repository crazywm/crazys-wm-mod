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
#include "CResourceManager.h"

CResourceManager::CResourceManager()
{
	for(int i=0; i < NUM_RESOURCES; i++)
	{
		m_Resources[i] = m_Last[i] = 0;
		m_ResourceCount[i] = 0;

//		m_UnloadedResources[i] = m_UnloadedLast[i] = 0;
//		m_UnloadedResourceCount[i] = 0;
	}
}


CResourceManager::~CResourceManager()
{
	Free();
}

void CResourceManager::Free()
{
	for(int i=0; i < NUM_RESOURCES; i++)
	{  
		if (m_Resources[i] != 0) // MYR: Trying to fix an exception upon exit
		{
			//m_Resources[i]->Free();   
			m_Resources[i] = 0;
		}
//		m_UnloadedResources[i] = 0;
		m_Last[i] = 0;
		m_ResourceCount[i] = 0;
//		m_UnloadedLast[i] = 0;
	}
}

void CResourceManager::Free(int type)
{
	m_Resources[type] = 0;
//	m_UnloadedResources[type] = 0;
	m_Last[type] = 0;
	m_ResourceCount[type] = 0;
//	m_UnloadedLast[type] = 0;
}

/*bool CResourceManager::KillResource(CResource* resource, int type)
{
	bool found = false;
	CResource* tmp = 0;
	if(!found)
	{
		tmp = m_Resources[type];
		while(tmp)
		{
			if(tmp == resource)
			{
				found = true;
				break;
			}
			tmp = tmp->m_Next;
		}

		if(found)
		{
			if(tmp->m_Next)
				tmp->m_Next->m_Prev = tmp->m_Prev;
			else
				m_Last[type] = tmp->m_Prev;

			if(tmp->m_Prev)
				tmp->m_Prev->m_Next = tmp->m_Next;
			else
				m_Resources[type] = tmp->m_Next;
		}
	}

	if(!found)
	{
		tmp = m_UnloadedResources[type];
		while(tmp)
		{
			if(tmp == resource)
			{
				found = true;
				break;
			}
			tmp = tmp->m_Next;
		}

		if(found)
		{
			if(tmp->m_Next)
				tmp->m_Next->m_Prev = tmp->m_Prev;
			else
				m_UnloadedLast[type] = tmp->m_Prev;

			if(tmp->m_Prev)
				tmp->m_Prev->m_Next = tmp->m_Next;
			else
				m_UnloadedResources[type] = tmp->m_Next;
		}
	}

	tmp = 0;

	if(found)
		return true;

	return false;
}*/

CResource* CResourceManager::AddResource(CResource *resource, int type)//, bool loaded)
{
//	if(loaded)
//	{
		if(m_Resources[type])
		{
			resource->m_Next = m_Resources[type];
			resource->m_Prev = 0;

			m_Resources[type]->m_Prev = resource;
			m_Resources[type] = resource;
		}
		else
		{
			m_Resources[type] = resource;
			m_Last[type] = m_Resources[type];
			m_Resources[type]->m_Next = m_Resources[type]->m_Prev = 0;
		}

		m_ResourceCount[type]++;
		while(m_ResourceCount[type] > MAX_RESOURCES)
			Cull(type);
/*	}
	else
	{
		if(m_UnloadedResources[type])
		{
			resource->m_Next = m_UnloadedResources[type];
			resource->m_Prev = 0;

			m_UnloadedResources[type]->m_Prev = resource;
			m_UnloadedResources[type] = resource;
		}
		else
		{
			m_UnloadedResources[type] = resource;
			m_UnloadedLast[type] = m_UnloadedResources[type];
			m_UnloadedResources[type]->m_Next = m_UnloadedResources[type]->m_Prev = 0;
		}

		m_UnloadedResourceCount[type]++;
	}*/

	return resource;
}

/*void CResourceManager::ReloadedResource(CResource* resource, int type)
{
	// Ok, this resource should already be in the unloaded list so move it
	// first fix it neighbours in the unloaded list
	if(resource->m_Next)
		resource->m_Next->m_Prev = resource->m_Prev;
	else
		m_UnloadedLast[type] = resource->m_Prev;

	if(resource->m_Prev)
		resource->m_Prev->m_Next = resource->m_Next;
	else
		m_UnloadedResources[type] = resource->m_Next;

	// Now place it onto the start of the loaded list
	if(m_Resources[type])
	{
		resource->m_Next = m_Resources[type];
		resource->m_Prev = 0;

		m_Resources[type]->m_Prev = resource;
		m_Resources[type] = resource;
	}
	else
	{
		m_Resources[type] = resource;
		m_Last[type] = m_Resources[type];
		m_Resources[type]->m_Next = m_Resources[type]->m_Prev = 0;
	}

	m_ResourceCount[type]++;
	m_UnloadedResourceCount[type]--;
}

void CResourceManager::MoveToUnloaded(CResource *resource, int type)
{
	// Ok, this resource should already be in the loaded list so move it
	// first fix it neighbours in the loaded list
	if(resource->m_Next)
		resource->m_Next->m_Prev = resource->m_Prev;
	else
		m_Last[type] = resource->m_Prev;

	if(resource->m_Prev)
		resource->m_Prev->m_Next = resource->m_Next;
	else
		m_Resources[type] = resource->m_Next;

	// Now place it onto the start of the unloaded list
	if(m_UnloadedResources[type])
	{
		resource->m_Next = m_UnloadedResources[type];
		resource->m_Prev = 0;

		m_UnloadedResources[type]->m_Prev = resource;
		m_UnloadedResources[type] = resource;
	}
	else
	{
		m_UnloadedResources[type] = resource;
		m_UnloadedLast[type] = m_UnloadedResources[type];
		m_UnloadedResources[type]->m_Next = m_UnloadedResources[type]->m_Prev = 0;
	}

	m_ResourceCount[type]--;
	m_UnloadedResourceCount[type]++;
}*/

void CResourceManager::Cull(int type)
{
	Cull(m_Last[type], type);
}

void CResourceManager::Cull(CResource *resource, int type)
{
	CResource* temp1 = resource->m_Next;
	CResource* temp2 = resource->m_Prev;

	resource->m_Next = resource->m_Prev = 0;
	resource->FreeResources();

	if(resource == m_Resources[type])
	{
		if(resource == m_Last[type])
		{
			m_Last[type] = m_Resources[type] = 0;
		}
		else
		{
			if(temp1 != 0)
				temp1->m_Prev = 0;
			m_Resources[type] = temp1;
		}
	}
	else if(resource == m_Last[type])
	{
		if(temp2 != 0)
			temp2->m_Next = 0;
		m_Last[type] = temp2;
	}
	else
	{
		temp1->m_Prev = temp2;
		temp2->m_Next = temp1;
	}

	m_ResourceCount[type]--;
}

void CResourceManager::CullOld(unsigned long currentTime)
{
	for(int i=0; i < NUM_RESOURCES; i++)
	{
		CResource* temp = m_Resources[i];
		while(temp)
		{
			if(int(currentTime - temp->m_TimeUsed) >= RESOURCE_LIFETIME)
			{
				CResource* save = temp->m_Next;
				Cull(temp,i);
				temp = save;
				save = 0;
			}
			else
				temp = temp->m_Next;
		}
		temp = 0;
	}
}
