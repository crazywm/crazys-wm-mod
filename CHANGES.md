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

