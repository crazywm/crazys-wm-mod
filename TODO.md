# TODO
## Engine
* [ ] Make use of SDL2 Textures instead of Surfaces
* [ ] use ffmpeg to support animation/video files
* [ ] file system abstraction (physfs? / boost::filesystem?)

## Code
* [ ] Move all remaining game code out of the user interface
* [x] Create a new NewGame function with a callback for progress feedback
* [ ] Replace image name based tags by an xml file
* [x] New ownership semantics for girl pointers
* [ ] Scan subfolders for girl packs

## Scripting
* [ ] Extend lua scripting to cover everything the old scripts did
* [x] Fix logic for async scripts if many scripts are started at once.
* [x] Fix lua memory management: save shared_ptr in lua objects, and add finalizers
* [x] Fix lua thread management

## UI
* [x] Move selection in active ListBox with arrow keys
* [x] Save current selection before going into game settings detail screen
* [x] Allow disabling combat popups
* [x] screen to change user settings in-game
* [x] sex type check box alignment

## Bugs
* [ ] some parts of the old scripts are still missing
* [ ] weird behaviour when entering 1 in a text field
* [x] Whore on the streets missing
* [x] Shop empty in first week
* [x] Inventory duplication bug
* [ ] Scrollbar release mouse while outside problem
* [ ] Pregnancies of runaway/prison girls currently don't work
* [ ] Crash: Load Game -> Go to dungeon -> exit -> load game -> go to dungeon: Dungeon Screen keeps reference to old
        building.
* [ ] GetRandomRivalToSabotage function is wrong
* [ ] "ERROR: Cannot find the torturer!"
* [ ] "Could not select the target. If you encounter this error, "
* [ ] "ERROR: Selected girl is a runaway"
* [ ] Clicking the item filter in the inventory screen set focus to left inventory
* [ ] girl->run_away is currently dangerous, if called in an apply loop
* [ ] async scripts and messages may get scrambled