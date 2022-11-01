/*
 * Copyright 2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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


#ifndef WM_TEXTINTERFACE_H
#define WM_TEXTINTERFACE_H

#include <unordered_map>
#include <functional>
#include "text/repo.h"

class IGenericJob;
class sImagePreset;

class cJobTextInterface : public IInteractionInterface {
public:
    cJobTextInterface() = delete;
    explicit cJobTextInterface(IGenericJob* job) : m_Job(job) {}

    bool LookupBoolean(const std::string& name) const final;

    int LookupNumber(const std::string& name) const final;

    void TriggerEvent(const std::string& name) const final;
    void SetVariable(const std::string& name, int value) const final;
    void SetVariable(const std::string& name, std::string value) const final;

    void RegisterVariable(std::string name, int& value);
    void RegisterVariable(std::string name, sImagePreset& value);
private:
    std::unordered_map<std::string, int*> m_MappedIntValues;
    std::unordered_map<std::string, std::function<void(std::string)>> m_MappedStringValues;
    IGenericJob* m_Job;
};


#endif //WM_TEXTINTERFACE_H
