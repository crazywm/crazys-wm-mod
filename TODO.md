# TODO
## Engine
* [ ] Make use of SDL2 Textures instead of Surfaces
* [ ] use ffmpeg to support animation/video files
* [ ] file system abstraction (physfs? / boost::filesystem?)
* [ ] libarchive for archive handling

## Code
* [ ] Move all remaining game code out of the user interface
* [ ] Replace image name based tags by an xml file
* [x] Scan subfolders for girl packs
* [ ] Improve image paths handling / image selection

## Scripting
* [ ] Extend lua scripting to cover everything the old scripts did
* [ ] New Commands:
  - [ ] Take gold
  - [ ] Player inventory
* [ ] New Triggers:
  - [ ] OnBought

## UI
* [ ] numeric column sorting

## Bugs
* [x] girl->run_away is currently dangerous, if called in an apply loop
* [ ] some parts of the old scripts are still missing
* [ ] weird behaviour when entering 1 in a text field
* [ ] Scrollbar release mouse while outside problem
* [ ] Pregnancies of runaway/prison girls currently don't work
* [ ] Crash: Load Game -> Go to dungeon -> exit -> load game -> go to dungeon: Dungeon Screen keeps reference to old
        building.
* [ ] GetRandomRivalToSabotage function is wrong
* [ ] "ERROR: Cannot find the torturer!"
* [ ] "Could not select the target. If you encounter this error, "
* [ ] "ERROR: Selected girl is a runaway"
* [ ] Clicking the item filter in the inventory screen sets focus to left inventory
* [ ] async scripts and messages may get scrambled
* [ ] Black bars in Gallery screen
