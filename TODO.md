# TODO
## Engine
* [ ] Make use of SDL2 Textures instead of Surfaces
* [x] use ffmpeg to support animation/video files
* [ ] file system abstraction (physfs? / boost::filesystem?)
* [ ] libarchive for archive handling
* [ ] Integrate boost::stacktrace for better debugging
* [ ] Async loading of animations

## Code
* [ ] Move all remaining game code out of the user interface
* [ ] Replace image name based tags by an xml file
* [ ] Improve image paths handling / image selection
* [ ] Move game specific stuff out of sConfig
* [ ] Improve performance of animations (upscale only when blitting?)
* [ ] Pre-generate list of all images for each girl on game start
* [ ] De-interlacing

## Scripting
* [ ] Extend lua scripting to cover everything the old scripts did
* [ ] New Commands:
  - [ ] Take gold
  - [ ] Player inventory
* [ ] New Triggers:
  - [ ] OnBought

## UI
* [ ] numeric column sorting
* [ ] interface/resolution/fullscreen selection to settings screen
* [ ] Per building/global anti preg setting
* [ ] Game-Mode files
* [ ] Update Gallery

## Bugs
* [ ] some parts of the old scripts are still missing
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
* [ ] Bought a dead girl from the market?
* [x] Girls can't get pregnant 
* [ ] ffmpeg frame duration off by one