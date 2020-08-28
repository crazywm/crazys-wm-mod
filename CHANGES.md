# Version 07.00.alpha4 (work in progress)

## Minor improvements
* Slightly faster movie loading
* Cleaned up gallery screen
* better text formatting on brothel main screen

## Fixes
* Save folder set correctly in settings
* Fix crash on funeral

# Version 07.00.alpha3
## Changes
* Support for video files!
* Select theme and fullscreen from settings
* More unification of job code -- slight changes in bar behaviour of Bar jobs possible

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
