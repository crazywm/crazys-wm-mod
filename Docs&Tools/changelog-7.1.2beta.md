# Version 7.1.2beta1
This version fixes some older and newer bugs in 7.1.2alpha5. Overall, things
appear stable enough by now (no game-breaking bugs I hope), so this is going to 
be the next beta version.

## Fixes
* Show dungeon events in turn summary
* Fixed wages for house cook
* Fixed some stats being accidentally set to 100
* Fixed `Waitress` trait name
* Fixed display of `DUMMY` job name

# Version 7.1.2alpha5

## Gameplay Changes
* removed npclove stat that wasn't really implemented anyway
* matron now gives obedience buff to all girls instead of directly changing the chance of refusal.
  This bonus accumulates and decays over time and depends on the quality of the matron.
* Adjusted hitpoint calculation for combat
* Increased XP gains for jobs and decreased XP for whoring.
* Decreased XP required for level-up slightly.
* Merged Hate and Love stats

## Fixes
* Fixed centre manager job (thanks @g539812)
* Fixed using stat instead of skill when determining skill of children
* Fixed wages for bar jobs
* Fixed HousePet.xml and Training.xml (thanks @CriticalMaestro)
* Fixed crash when girl runs away during job processing
* Fixed girl leveling up two levels if she has enough XP
* Fixed default theme
* Fixed rendering bug in multi-page message box
* Fixed weeks played for new game

## Code changes
* Dealer, Entertainer, XXXEntertainer, Masseuse, Stripper, Peep-Show, Advertising,
  Beast Care, Whore, Catacombs, Security jobs now xml-based
* Some generic cleanup (reduces c++ code by ~3000 lines)

# Version 7.1.2alpha4
This version introduces dynamic skill caps. This means that the maximum
value a girl's skills can get is now limited by her level and other
stats/skills, (hopefully) making skill development a bit more interesting.
Also, it prevents situations like a girl which has 100 skill in group sex
but 0 skill in regular sex, by making regular sex a requirement for group sex.
Skill decay now focuses on skills that are close to their cap, and is less likely
for lower skills.

## Changes
* Added dynamic skill caps
* New format for rgirl xml that allows specification of spawn reasons
* Reworked spawn code

## Fixes
* text placement on gang screen
* job xml loading for bar jobs @Smargoos
* traits are correctly removed when an item is unequipped. @Smargoos
* equipped items set permanent skill/stat modifiers instead of temporary ones @Smargoos
* loading of equipped items now applies their effects @Smargoos
* fixed blocked traits showing up in trait list @Smargoos
* several small fixes @Smargoos


# Version 7.1.2alpha3
Mostly smaller fixes and changes.

## Changes
* Added `piano` image tag, `music` tag as fallback for `sing` and `piano`
* Added `refuse` tag for when a job is refused
* Added Next Week button to girl management screens
* Updated UI code such that it is possible to reuse common screen structure across xml files
* `suckballs` images may now be shown in bedroom interaction
* Escort is now an XML job.

## Fixes
* Clear customer list each week
* Fixed walk button
* Don't try to render empty text in edit box and list widgets
* Don't ask for building name after the player already entered it
* Read first/middle/surname from girl xml
* Use correct skill for sex message @Smargoos
* Correctly show inventory type @Smargoos
* Fix random girls not using defaults when stat is missing in xml @Smargoos

# Version 7.1.2alpha2
This is a very experimental release that has switched the
rendering from being based on SDL_Surface to SDL_Texture.

## Changes
* **New rendering implementation**
* Next Week button on town and gang screens
* Number of free interactions can be set in game options
* Themes can specify image directories

## Fixes
* Increased text size for message box

# Version 7.1.2alpha1

## Changes
* show amount of money taken from girl @chris-nada
* made more gang related settings configurable
* display girl stat changes from last week @chris-nada
* reworked themes: automatically adapt to resolution
* in fullscreen mode, render in screen resolution
* text item and check box text color can now be specified
* added an incomplete version for a dark mode @Shadowflux

## Fixes
* text in new game screen no longer resets on tab / when returning from more details screen
* milk increasing without trait @chris-nada
* don't show deleted saves @chris-nada
* text fixes @chris-nada
* Add a cap to certain game settings to prevent nonsensical values
* Improved numerical stability of sigmoid function
* small bugfixes provided by @Yopyop0
* Increased font size in input prompt