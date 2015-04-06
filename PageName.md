#Future work.

# To Do #

  1. ~~A are you sure message when you hit quit.  People ask for this one a lot and it isn't as easy as I thought it would be to add.~~ **Done**.
  1. Finish porting the game ui from legacy data to XML.
    * ~~Main Menu~~. **Done**.
    * Load Menu
    * ~~GetInt~~/GetStr
      * ~~I Want to merge both in just one screen for Int/String and Confirmation.~~ Done for Int and Confirm.
    * ~~Brothel Screen~~. **Done**.
  1. Code clean up.
    * Merging all those globals in a single global object. (Started)
    * Cleaning unused (global/local) variables. (Started)
    * Using more OO.
      * cDungeon implementation.
        * Derive from cBrothels?
        * Shouldn't have a different structure for girls there.

# Things laying around that should get to the game at some point #

  1. Sec lvl means nothing you have just as many problems when your in the negative as you do when you have 2500 this is something that makes no sense we should make it mean something to have high lvl or low lvl.  Also gangs on guard should show up to help more when sec fails.  Another problem with it is the number of people the girl has to fight can be just stupid.
  1. Accomodation effect very little it should mean more.
  1. Feel the game needs more and different ways to aquire girls http://www.pinkpetal.org/index.php?topic=429.0 is a pretty good ideal fits with one of the bigger changes.
  1. Money sinks. Some of the bigger changes will help but you need things to spend money on badly.
  1. Gang mission need balance and maybe even a few more missions if there going be in the game they need more reason then what they have I think.
  1. Script functions.  Having a good script can really really help the game.
  1. http://www.pinkpetal.org/index.php?topic=673.0 hell its supose to be in the code somewhere but I've looked and can't find it but it would help.
  1. Libido needs some work.
  1. If you know how to play the game jail is almost never used.  I would like to see girls randomly show up in jail that you can bail out and have them come work for you.
    * Here i would like to see something like a private prison. You are trusted a few prisoners that you have to "fix" them. It could be a new way to get girls as well.
  1. Customer happiness and fame need fixed as they change to much.
  1. I see no reason why when you burn down a rivals brothel you shouldn't be reward with a few girls that worked there.
  1. Anon did a new way to sort girls in the char folder I think there was a few problems with it so I never merged it.  But something like that would be nice.
  1. A trait lvling system.
  1. Building managment http://www.pinkpetal.org/index.php?topic=215.0 there is a bit of it already coded up don't know how much but some.
    * Building upgrades.  Stuff like sec camarea, fence (reduce chance of run aways), better beds, etc.
  1. http://www.pinkpetal.org/index.php?topic=665.0 I feel this would be a nice thing to have.  Just the marriage and lineage tracking not the sql
  1. http://www.pinkpetal.org/index.php?topic=455.0 this makes sense all girls shouldn't be good at everything and beening able to set what kinda sex acts they do by girl rather then brothel would help make girls feel different.
  1. The poltical side of the game could use some work.  I feel as you gain more areas and buy more buildings it should be harder to please the mayor and the police.
  1. The dungeon is a little boring would like to see new ways to break girls.  More hardcore to nicer ways.
  1. Would really like to see other building feel different then the brothels.  Movies is a good start but it could be pushed futher and arena could be so much more.  Some don't really need to be any different but I feel some should be.
  1. Get rid of gangs make them henchmen instead.  So u could have like or goku or guts been ur guards.  This seem like a huge change though.
  1. Improve scripting capabilities (**new**)
    * Filtering girl "spawn points"

# Things to think about #

  1. Job flavor text.. One reason ex is so popular is all the different things that can pop up on jobs.
    * Bar - Working on it
    * Hall - Done
    * Club -
    * Brothel -
  1. New ways to aquire unique girls.
    * Have girls randomly show up in the prison where u can pay there bail and have them come work for u
    * 
  1. Making jobs better.  Before there was things like buy achool and gambling %.  Buying different lvls of achool for the bartender or been able to set a odds for card dealer might make them more intresting that kinda thing..  Having strippers sell lap dances vip dance and such u get the ideal.
  1. New cool jobs and buildings.  Would love to know what if any jobs or buildings u might want to add.
    * I would like to see something like a contract system. Perhaps change the "Call to Office" dialogue to missions.
  1. Girl personality system.  Be based of traits would make girls do things differently in jobs and such.
  1. Ways to make current buildings better.  Make the player want to use them.
  1. New skills.  Could add hand job, foot job that kinda thing rather easy but should we?
    * Whats ur thoughts on this more skills or not?
  1. An Onsen building.
    * I would postpone this one. We already have too many building without proper use.
  1. An auction.  Place to acquire new unique girls, sell girls and sell a girls virginty.. If u look at the code I added a screens for this already on reason its not there is I havent full figured out how I want to do it.
  1. A maid cafe type building with a "special menu"...  Only reason I havent done this is it just seems like a mix of the bar and sleezy bar to me.  So thought of maybe have be themed based and making the player pick the theme.. Like big boobs or nice legs or cosplay u get the ideal but not sure on this yet.
  1. Escort job.  Would require like a high lvl girl or something.  Would only see few people week with high pay.  Havent figured it all out yet.
    * Perhaps as a mission/contract?  // That could be a good ideal have someone asking for a certain type girl and you gotta pick one to match what they want
12. unquie items..  scripts to give items to girls or player.  Honestly items in general are something to think on

# Bugs #

  1. ~~When you have more then one brothel hitting next girl in other building doesn't work but it does when you only have 1 brothel..  Akia posted a fix for this but when I merged it broke a few things.  U can find the fix posted here http://www.pinkpetal.org/index.php?topic=1031.105   Maybe u can get it working right~~. **Done**
  1. ~~Prev/Next don't iterate over dungeon girls~~. **Done**.