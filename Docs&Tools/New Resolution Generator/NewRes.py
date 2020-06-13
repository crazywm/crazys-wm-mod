#!/usr/bin/env python3
import re
from pathlib import Path
from os import listdir as ls

# Font is included with other resizes because of some sort of DPI issue with the program
# Font size scaling is used in the original resolution versions, so it is here as well

# Regex String for editing files
regex = r"((?:[XY]Pos|Width|Height|FontSize|RowHeight)=\"[0-9]*\")"
# y Regex used for differing X and Y multipliers
yRegex = r"((?:YPos|Height)=\"[0-9]*\")"
# Regex string for parsing data
innerRegex = r"(.*=\")([0-9]*)"

# Regex for Offset Management

offsetReg = r"<ListBox.*HeaderDiv.*>"
closeOffsetReg = r"</ListBox>"
findOffsetReg = r"(Offset=\"[0-9]*\")"
innerFindOffsetReg = r"(Offset=\")([0-9]*)"

# What to multiply resolution and other numbers by
# X includes everything but Y and Height
xMultiplier = 2
yMultiplier = 2

# Assuming being run from directory file was left in
intDir = Path('../../') / 'Resources' / 'Interface'
source = intDir / 'J_1366x768'

# Array for generating new resolutions
# Multiplier based on default 1366x768 resolution
# Adding to this is as simple as adding to the array and creating the folder in the correct place
products = {
    # Resolution : [xMultiplier, yMultiplier]
    'J_1600x900': [800 / 683,75 / 64],
    'J_1920x1080': [960 / 683, 45 / 32],
    'J_3840x2160': [1920 / 683, 45 / 16]
}

def resize(source, dest, xMultiplier, yMultiplier):
    for xml in ls(source):
        if xml.endswith(".xml"):
            rXml = source / xml
            data = []
            with open(rXml, 'r') as edit:
                offsetting = False
                oldOffsetArr = []
                newOffsetArr = []
                for line in edit:
                    if offsetting:
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
                        line = newLine
                    if re.search(regex, line) is not None:
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
                    if re.search(offsetReg, line) is not None: 
                        offsetting=True
                        # print("Offsetting Triggered with Line: {}".format(line))
                    elif re.search(closeOffsetReg, line) is not None:
                        offsetting=False
                        oldOffsetArr = []
                        newOffsetArr = []
            wXml = dest / xml
            print("Writing edited file to: {}".format(wXml))
            with open(wXml, 'w') as fix:
                for line in data:
                    fix.write(line)

for resolution in list(products):
    print("Generating folder: {}...".format(resolution))
    dest = intDir / resolution
    xM = products[resolution][0]
    yM = products[resolution][1]
    resize(source, dest, xM, yM)
    print("Done")