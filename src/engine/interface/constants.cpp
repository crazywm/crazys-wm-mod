/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
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

#include "interface/cColor.h"

// interface colors
sColor g_ChoiceMessageTextColor{0, 0, 0};
sColor g_ChoiceMessageBorderColor{0, 0, 0};
sColor g_ChoiceMessageHeaderColor{229, 227, 52};
sColor g_ChoiceMessageBackgroundColor{88, 163, 113};
sColor g_ChoiceMessageSelectedColor{229, 227, 52};

sColor g_EditBoxBorderColor{0, 0, 0};
sColor g_EditBoxBackgroundColor{90, 172, 161};
sColor g_EditBoxSelectedColor{114, 211, 198};
sColor g_EditBoxTextColor{0, 0, 0};

sColor g_CheckBoxBorderColor{0, 0, 0};
sColor g_CheckBoxBackgroundColor{180, 180, 180};

sColor g_WindowBorderColor{0, 0, 0};
sColor g_WindowBackgroundColor{140, 191, 228};

sColor g_ListBoxBorderColor{0, 0, 0};
sColor g_ListBoxBackgroundColor{217, 214, 139};
sColor g_ListBoxElementBackgroundColor[] = {{114, 139, 217}, {200, 30, 30}, {80, 80, 250},
                                            {30, 190, 30}, {190, 190, 00}};
sColor g_ListBoxSelectedElementColor[] = {{187,   90, 224}, {255, 167, 180}, {187, 190, 224},
                                          {0, 250, 0}, {250, 250, 250}};
sColor g_ListBoxElementBorderColor{79, 79, 111};
sColor g_ListBoxElementBorderHColor{159, 175, 255};
sColor g_ListBoxTextColor{0, 0, 0};

sColor g_ListBoxHeaderBackgroundColor{25, 100, 144};
sColor g_ListBoxHeaderBorderColor{120, 155, 176};
sColor g_ListBoxHeaderBorderHColor{15, 49, 69};
sColor g_ListBoxHeaderTextColor{255, 255, 255};

sColor g_MessageBoxBorderColor{255, 255, 255};
sColor g_MessageBoxBackgroundColor[] =  {{100, 100, 150}, {200, 100, 150}, {100, 200, 150},
                                         {100, 100, 200}, {190, 190, 0}};
sColor g_MessageBoxTextColor{0, 0, 0};