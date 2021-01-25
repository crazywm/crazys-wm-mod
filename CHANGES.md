# Version 07.00.alpha12

## Changes

## Fixes
* fixed reset when selecting filter in item screen

# Version 07.00.alpha11

## Changes
* Internal work on cListBox
* Split Director into Executive (Matron) and Director (Filming scenes)
* New MarketResearch job and corresponding MarketResearch screen
* Reworked random scene and studio auto assignment code
* Improved MovieMaker Screen
* Slight updates to the bedroom interaction scripts
* More movie name variety, and custom movie names
* Movie configuration in advanced game settings
* Ad Campaign and Survey actions
* JobRating column in Studio Screen
* Allow hiring multiple gangs at once
* Added a new "send for training" interaction to improve performance skill

## Fixes
* fixed error in BarMaid.xml
* fixed error in SexText.lua
* fixed removing scene from movie

# Version 07.00.alpha10

## Major Changes
The Movie Studio has been rewritten almost entirely. The most important changes are:
* You can (and should) have more than one camera mage, crystal purifier, and promoter
* Finished movies are now tracked explicitly in the movie maker screen. There you can also
  set the ticket prices
* There are now different types of target audiences, which have differing requirements on
the movie.
* In the turn summary, BUILDINGS section, the Movie Studio will now have additional info about
created scenes and screened movies

**The new movie mechanics are still preliminary, and haven't really been balanced yet**

## Fixes
* fixed empty item after item transfer
* fixed shop contents after loading game
* fixed TraitModifiers after loading save game
* removed Day Job column from Movie Studio
* fixed errors in loading job xml
* error message (instead of crash) when invalid element is added to list box

# Version 07.00.alpha9
##  Changes
* Better error message if skill/stat/... lookup fails
* Restrict trait names to 
  - start with a letter
  - consist only of letters, digits, spaces, underscores and hyphens
  - end with a letter or digit
* updated film jobs
* added new film texts provided by jrok

## Fixes
* Fixed loading of trait modifiers from external files
* Fixed text generation for film jobs
* Fixed some out_of_range errors in job processing

# Version 07.00.alpha8

## Changes
* More job XML texts: Waitress, FilmBeast, FilmBondage, FilmBuk, FilmFaceFuck, FilmOral, FilmPublicBDSM

## Fixes
* Fixed loading of game configuration settings
* Fixed game setting screen
* Fixed loading of trait properties
* Fixed House Pet / FO training names
* Fixed loading of special stats 


# Version 07.00.alpha7

## Changes
* Updated the job text xmls, now supporting prefixes/suffixes and groups
* Allow variable updates from text xmls

## Fixes
* Can load/start game even if some girl/rgirl files are broken
* column sorting is now more robust
* several lua script fixes
* fixed a condition in the GetMilked job

# Version 07.00.alpha6
## Changes
* Reworked the job performance xml specifications and split them from the gains
* supports expanding ~ to home directory on linux
* support for multiple girl pack locations

## Fixes
* improved compilation with clang
* corrected sorting of numeric columns
* fixes for some crashes
* text rendering fixes
* fixed processing of matron jobs
* fixed reading of gang names


# Version 07.00.alpha5
## Changes
* More old scripts ported to lua
* Made lua scripts more composable
* Some jobs now read configurations from XML files in `Resources/Data/Jobs`
* Screen navigation buttons can be configured from interface xmls

## Fixes
* Autosave/continue game should work again
* Improved CMake build scripts

# Version 07.00.alpha4

## Minor improvements
* Slightly faster movie loading
* Cleaned up gallery screen
* better text formatting on brothel main screen
* More unification of job code -- slight changes in behaviour of Farm jobs possible

## Fixes
* Save folder set correctly in settings
* Fix crash on funeral


# Version 07.00.alpha3
## Changes
* Support for video files!
* Select theme and fullscreen from settings
* More unification of job code -- slight changes in behaviour of Bar jobs possible

## Fixes
* Girls can get pregnant again
* Fixed interaction script experience gain
* Fixed game setting loading


# Version 07.00.alpha2
## Changes
* Recursively scan character folder for girl files
* Adjusted (and made configurable) the gang weapon upgrade cost to account for the fact they
are now per gang.
* New per-game settings. Some of these have been ported over from the config.xml
  * `world.encounter_chance`    Chance to meet a girl when walking around
  * `world.catacombs.unique_chance` Unique girl chance for catacombs
  * `world.rape-chance.{brothel,street}` Chance that the customer tries to rape a whore 
  * `gang.weapon_upgrade_cost`  Base cost for gang weapon upgrades
  * `money.buy.slave` Multiplier for slave cost
  * `money.cost.{room,contraception,healing_potion,net}` Price for a room, contraceptive, healing potion, or net


## Minor improvements
* Preserve selected item after adjusting a setting in the new game screen
* Removed Gang Catacomb Orders from settings, as these are set per game in the Gang Management screen

## Fixes
* Fixed font bug on gallery screen
* Fixed Gallery back button
* Fixed runaway crashes
* Feeding/Interact buttons on dungeon screen


# Version 07.00.alpha1
## Major new features
* Events based on lua scripts
* Combat overhaul and combat reports
* Trait modifiers editable in xml files
* Specify crafting for items in xml
* Allow changing player settings (i.e. default house percentage) in-game

## Code
* Updated Library: SDL1.2 -> SLD2
  - now allows for non-ascci text input
* Updated Library: Lua 5.1 -> Lua 5.3
* Updated Library: tinyxml -> tinyxml2
* **Major** internal code refactorings

## Gameplay
* updated mechanics of catching girls (integrated in new combat)
* adjusted some traits (e.g. pessimist/optimist now give weekly happiness change)
* rival actions now create no popup messages, but have their own tab in the turn summary

## Minor changes
* Can use arrow keys to move selection in list box


Due to the large number of code changes, there will be minor changes in 
gameplay that are not listed here.
