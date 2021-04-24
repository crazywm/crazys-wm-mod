# TODO
## Engine and Refactorings
* [ ] Make use of SDL2 Textures instead of Surfaces
* [ ] file system abstraction (physfs? / boost::filesystem?)
* [ ] libarchive for archive handling
* [ ] Integrate boost::stacktrace for better debugging
* [ ] Async loading of animations
* [ ] for logging? https://github.com/gabime/spdlog
* [ ] Girl identifiers -- so we can have stable references in the save file e.g. for movie scenes
* [ ] named actions for screens -- so we can make configurable key bindings
* [ ] redo all the item code :( 

## Code
* [ ] Move all remaining game code out of the user interface
  - [ ] cScreenSlaveMarket
  - [ ] cScreenGang 
  - [ ] cScreenDungeon
* [ ] Replace image name based tags by an xml file
* [ ] Improve image paths handling / image selection
* [ ] Reduce usage of sConfig
* [ ] Improve performance of animations (upscale only when blitting?)
* [ ] Pre-generate list of all images for each girl on game start
* [ ] De-interlacing
* [ ] IListBox conversion
* [ ] Convert job functions to classes
  - [ ] General
  - [ ] Bar
  - [ ] Gambling
  - [ ] Brothel
  - [ ] PersonalBedWarmer
* [ ] adjustable unique chance for overworld encounters

## Scripting
* [ ] Extend lua scripting to cover everything the old scripts did
* [ ] New Commands:
  - [ ] Player inventory
* [ ] New Triggers:
  - [ ] OnBought
  - [ ] Next Week
  
## Movies
* [ ] More text variety
* [ ] Trait text influences for scenes
* [ ] Fame bonus for girls/crew whose movies are good/hyped
* [ ] Initial hype bonus for movies of famous girls/crew


## UI
* [ ] Per building/global anti preg setting
* [ ] Game-Mode files
* [ ] separate logical and actual render size and make configurable
* [ ] Active Building Widget
* [ ] LoadGame delete button
* [ ] Inventory screen resets
* [ ] load game: select most recent by default
* [ ] Restore Property Management screen

## Bugs
* [ ] some parts of the old scripts are still missing
* [ ] Scrollbar release mouse while outside problem
* [ ] Pregnancies of runaway/prison girls currently don't work
* [ ] GetRandomRivalToSabotage function is wrong
* [ ] async scripts and messages may get scrambled
* [ ] ffmpeg frame duration off by one
* [ ] bugs in shop/inventory screen
  - [ ] "your inventory is full", but seems to buy anyway
  - [ ] "your inventory is full" for sell button
  - [ ] Equip / unequip showed simultaneously
* [ ] gracefully handle missing/corrupt config file
* [ ] Setting jobs for multiple girls when girl list is not in default sort order
* [ ] animated webps don't load
* [ ] order of events:
  When matron puts girl into resting state, 
  the announcement isn't put into the correct spot in the event listing. 
  It is always at the top, instead of in the correct time sequence. 
  This appears to also be a problem with girls who get in fights or are attacked, 
  and if the girl refuses a customer due to sexual preference.
* [ ] Recruiters, cooks and cleaners in the house do not appear to cost any money???
* [ ] SexText.lua line 649