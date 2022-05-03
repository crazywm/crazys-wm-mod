# Version 7.1.2beta4

## Gameplay
* Added two new temporary traits, "Recently Punished" and "Recently Scolded", 
that get activated through your interactions. If you scold a recently scolded girl,
it is less effective, if you scold a recently punished girl it is more effective. Allowing
a girl to refuse an interaction while the "Recently Scolded" trait is active will decrease her
obedience.
* Updates to the interaction script

## UI
* Slightly increased dialog font size
* New image types `drink`, `dinner`, `kiss`, currently only used as part of the interactions
* Added some more default images

## Fixes
* Added missing `Weight` parameter to `Blacksmith.xml`
* Traits now update correctly after a temporary trait is added
* Duplicate girl bug when walking streets
* Added skill gains for waitress job
* Fixed error in TownMeet kidnap scenario
* Image reset if no file could be loaded
* Correctly present stat/skill changes after loading girl
* Fixed some errors in lua scripts
 
# Version 7.1.2beta3
## Gameplay
* Training job less likely to select skills close to skill cap
* New Tattoo/Piercing management in office interactions
* Prevent gangs from accidentally killing their catacomb captures
* Girls at very low health, but with constitution and stamina, 
will regenerate health more quickly but lose stamina/strength 
* Clinic will generate a warning message or status update for the number of treatments performed/requested/possible.

## Fixes
* Set correct matron job for brothel. This fixes obedience calculations.
* Fixed some errors in lua bindings.
* Fixed crash for runaway when branding in dungeon
* Fixed hiring of multiple gangs at the same time in non-default order
* Fixed saving/loading of beasts and other stored goods
* Added missing refuse text for `Intern` job
* Fixed disease traits to actually be registered as diseases.
* Full-Time job correctly applied to both day- and night-shift
* Fix text in dungeon screen, and joined together message for multiple brandings
* Fixed error message if not enough doctors are present


# Version 7.1.2beta2

## Gameplay
* In the first two weeks, "Walk around town" will always lead to an encounter
* Adapted success chance in skill checks in the lua scripts. 
  If too much skill is lacking, chance is now 0%
* Removed rival inventory simulation

## Fixes
* Fixed event type for Advertise job
* Fixed money computation for Barmaid and Catacombs
* Improved behaviour if no suitable random girl is found
* Fixed error in Office lua script
* Fixed spelling of `Dependent` tag; still accepts old spelling.

# Version 7.1.2beta1

## Gameplay changes since 7.1.1
* Added dynamic skill caps
* Merged PCHate and PCLove stats 
* Adjusted XP calculations
* New image tags `music`, `piano`, `refuse`

## Other changes since 7.1.1
* Reworked rendering implementation
* More game aspects configurable by xml
* Further general code cleanup
* Some UI improvements
* Added Theme support and a simple dark theme.

## Fixes since 7.1.2alpha5
* Show dungeon events in turn summary
* Fixed wages for house cook
* Fixed some stats being accidentally set to 100
* Fixed `Waitress` trait name
* Fixed display of `DUMMY` job name

For a detailed changelog see [Docs&Tools](Docs&Tools/changelog-7.1.2beta.md)

# Version 7.1.1beta2
## Changes
* included new meeting script by CaptC

# Version 7.1.1beta

## Major Changes
### Practice Skills Job Reworked
now moved to the house, and requiring a **Mistress** for optimal results.
  - Practice Skill: now a House job
  - Practice Skill: only increases sex skills and Performance
  - Only one girl can be personally trained at the same time
  - Mistress: strongly boosts the Practice Skills results for one or two girls, depending on Mistress skill
  - Mistress: can give temporary obedience increase to disobedient girls around the house

### More Game Aspects Defined Through XML
* Trait modifiers
* Item crafting
* Job performance, requirements, and job texts
  - Not for all jobs yet
  - Some jobs have only been partially converted

### Image Scaling and Video Support
* Small images and animations won't be stretched to full screen size. Instead, each image in the UI 
  now has a maximum and a minimum size, and will only be stretched if it is too small or too large. 
  To restore the old behaviour, you can edit the interface xml files so that they have identical maximum and minimum settings.

* The game can now read a variety of video files including mp4, avi, webm. 
  (Animated webp is not supported at the moment)

### Major internal code rework
Reduced the amount of C++ code to about half, without removing much from the game.
Some parts are still a bit broken, though. Much of the text generation for the girls'
jobs has been moved to XML, but this is still work in progress.

Also updated these libraries to more recent versions:
* SDL1.2 -> SLD2
  - now allows for non-ascci text input
* Lua 5.1 -> Lua 5.3
* tinyxml -> tinyxml2

### Reworked Movie Studio
The Movie Studio has been rewritten almost entirely. The most important changes are:
* You can (and should) have more than one camera mage, crystal purifier, and promoter
* Finished movies are now tracked explicitly in the movie maker screen. There you can also
  set the ticket prices
* There are now different types of target audiences, which have differing requirements on
  the movie.
* In the turn summary, BUILDINGS section, the Movie Studio will now have additional info about
  created scenes and screened movies

**The new movie mechanics are still preliminary, and haven't really been balanced yet**

* Split Director into Executive (Matron) and Director (Filming scenes)
* New MarketResearch job and corresponding MarketResearch screen
* JobRating column in Studio Screen
* Working as Pornstar makes girls lose the Chaste trait
* Added a new "send for training" interaction to improve performance skill
* Added new image tags for the studio crew jobs
* Ad Campaign and Survey actions to help promote your movies

### Other
* scripting ported to lua script
* support for multiple girl pack locations and recursively scan character folder for girl files
* New combat mechanics / combat reports

## Minor Changes
* More info in Load Game screen
* Removed Dump Bodies option for sell/fire girls
* At the centre, you need one counselor for each therapy
* Removed Mechanic and Get Repaired jobs. Construct girls will now also be treated by normal doctors.
* Cosmetic surgery no longer increases Charisma
* Liposuction can remove Fat trait, but no longer changes Charisma and Beauty
* Don't auto-assign new counselors if the counselor refuses for a shift
* added `massage` images for masseuse job, with fallback to `hand`
* Allow hiring multiple gangs at once
* supports expanding ~ to home directory on linux
* Select theme and fullscreen from settings
* rival actions now create no popup messages, but have their own tab in the turn summary
* Allow changing player settings (i.e. default house percentage) in-game in the house screen

