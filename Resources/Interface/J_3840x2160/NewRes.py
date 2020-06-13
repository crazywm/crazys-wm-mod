#!/usr/bin/env python3
import re
from os import listdir as ls

# Regex String for editing files - Normal
# regex = r"((?:[XY]Pos|Width|Height)=\"[0-9]*\")"
# Regex String for editing files - Force Font Changes
regex = r"((?:[XY]Pos|Width|Height|FontSize)=\"[0-9]*\")"
# y Regex used for differing X and Y multipliers
yRegex = r"((?:YPos|Height)=\"[0-9]*\")"
# Regex string for parsing data
innerRegex = r"(.*=\")([0-9]*)"

# Regex for Offset Management

offsetReg = r"<ListBox.*HeaderDiv.*>"
closeOffsetReg = r"</ListBox>"
findOffsetReg = r"(Offset=\"[0-9]*\")"
innerFindOffsetReg = r"(Offset=\")([0-9]*)"

# What to multiply all numbers by (Set to 2 for 4K from 1080p version)
xMultiplier = 2
yMultiplier = 2

for xml in ls('./'):
    if xml.endswith(".xml"):
        data = []
        with open(xml, 'r') as edit:
            print("Editing file {}...".format(xml))
            offsetting = False
            oldOffsetArr = []
            newOffsetArr = []
            for line in edit:
                if re.search(offsetReg, line) is not None: 
                    offsetting=True
                    # print("Offsetting Triggered with Line: {}".format(line))
                    data.append(line)
                elif re.search(closeOffsetReg, line) is not None:
                    offsetting=False
                    oldOffsetArr = []
                    newOffsetArr = []
                    data.append(line)
                elif offsetting:
                    newLine = line # Cache old line as new line
                    for offset in re.findall(findOffsetReg, line):
                        oldOffsetArr.append(int(re.findall(innerFindOffsetReg, offset)[0][1]))
                        if oldOffsetArr[-1] == 0:
                            newOffsetArr.append(oldOffsetArr[-1])
                            break
                        diff = oldOffsetArr[-1] - oldOffsetArr[-2]
                        newDiff = round(diff * xMultiplier)
                        newOffsetArr.append(newOffsetArr[-1] + newDiff)
                        fixed = "Offset=\"{}\"".format(newOffsetArr[-1])
                        p = re.compile(offset)
                        newLine = p.sub(fixed, newLine)
                    data.append(newLine)
                elif re.search(regex, line) is not None:
                    newLine = line # Cache old line as new line
                    for var in re.findall(regex, line):
                        old = re.findall(innerRegex, var)[0]
                        if re.search(yRegex, var) is not None:
                            fixed = old[0] + str(round(int(old[1]) * yMultiplier)) + "\""
                        else:
                            fixed = old[0] + str(round(int(old[1]) * xMultiplier)) + "\""
                        p = re.compile(var)
                        newLine = p.sub(fixed, newLine)
                    data.append(newLine)
                else:
                    data.append(line)
        with open(xml, 'w') as fix:
            for line in data:
                fix.write(line)
