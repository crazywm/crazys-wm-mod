# Game Design: Movie Studio
Movies are created at the MovieStudio and are its primary way of generating revenue. 
To create a movie, you first have to shoot a few scenes. These are then combined into 
the movie, which is released and hopefully brings much coin into your coffers.

How much money a movie makes depends on how many people go to see the movie, and how
much a ticket costs. The game simulates this buy generating several groups of people
with similar tastes and spending patterns (i.e. a certain Target Group). If a movie 
both appeals to the taste of a target group, and it not more expensive than that 
groups willingness to spend, they will go see the movie.

## The Audience
The movie audience consists of several target groups. Each target group has a 
certain number of members, a quality threshold and a spending 
threshold. Additionally, they may have certain turn-offs. The number of people
that go to see a movie is calculated as follows:
First, the movie is checked against the target groups turn-off list. Turn-offs
are that the movie contains at least one scene of a certain category (see below for 
scene categories), or a non-consensual scene. In that case, a large fraction of the
group will not even consider going to the movie. 

For the remaining candidates, a score is calculated for each scene (which depends
on the scene quality) and a weighted sum according to the audiences preferences
is calculated. Each target group has one favourite scene, which always gets a weight
of 1. If the same scene type appears multiple times in one movie, its score gets
reduced.

The ticket price is compared to the groups spending power, and if it is too large,
they will not see it. If it is equal or lower, they might come to watch the movie.
This weighted sum is compared to the audiences quality threshold, and the percentage
of people who would like to watch the movie is calculated. If the movie is worse than
the expected quality, but also cheaper than the spending power, they might still
come. For 25% decrease in ticket price, they will accept a 10% decrease
in quality. However, iIf it is much less than they expect to spend (<50%), 
they will avoid the movie, presuming it to be bad quality.

The final factor is the amount of people who know about the movie, and who haven't
seen or rejected it yet. This depends on the movies HYPE value and on how long a 
movie has been out yet. 

The total amount of people who will watch the movie in a given week is then calculated
by multiplying all the previous factors together:
```
WATCHERS = GROUP SIZE * NOT_TURNED_OFF * CAN_AFFORD * QUALITY * HYPE
```

## Hype and Market Saturation
One important factor in how many views your movie gets is how many people in
the target group know about it and are interested. For a new movie, interest
is relatively high but quickly decays as time passes. This can be reduced by 
hyping a movie. 

### Hype
A movie generates hype if its quality to ticket price ratio is good
among a target group which can afford to go to the movie. For example, if 
'The Workers' expect a quality of 100 at a price of 10, but you are showing a movie
with quality 200 at price of 10, then they will produce 1 point of hype per week.
This point is multiplied by the fraction of the worker audience that would watch the
movie. Each point of hype cancels the interest decay of one week of running time.
Hype decays at a rate of 10% per week.

Hype can also be built by employing a promoter at your Movie Studio. The strength
of the effect depends on your promoter's skill, and the advertising budget set in
the studio.

### Market Saturation
People that just went to see a movie have a 50% chance of not wanting to see a movie
just next week. In this way, market saturation builds up, and the pool of available
viewers in a target group decreases if you show many films to that group. These 
people become available again at a rate of 5% per week.

Even if you are not showing any movies at the moment, a small rate (5%) of potential
viewers will decide that they don't want to watch a film in the near future.

A promoter can be used to stimulate demand and decrease the market saturation.

## Fame
Your actresses gain fame if their movies perform well, in two different ways:
1) While the movie is screening, they gain fame based on the number of viewers in that particular week.
The maximum amount of fame they can reach this way is given by the square root of
the number of viewers. Fame gain decreases if the girl only contributed few/bad 
scenes to the movie.
2) When the movie is taken out of circulation, the actresses gain fame based on 
its total box office. The maximum fame that can be reached this way is the calculated
as `SQRT(BoxOffice / 10)`. For one movie, no more than 5 points of fame can be gained.
If the movie performs particularly bad relative to her current fame, she might lose
fame points.

## Target Groups
Target groups are a collection of people with similar tastes that may go and see 
a movie. A list of all target groups is provided below. The exact amount of people
in a target group, and the specific values of score requirement and spending power
as well as favourite scene are determined randomly at the game start, and updated
randomly with a chance of 2% each week.

### The Mob
A numerous group that will watch whatever you produce, 
but has very little spending power. 

### The Families
They will not watch movies with hardcore or extreme scenes, and have a significant
chance of not watching softcore scenes either. This group is also rather large, and
has medium spending power. They require medium quality movies.

### The Workers
A large group that has a moderate chance of shunning extreme scenes. Medium spending
power and medium quality requirement.

### The Perverts
A medium-sized group that will skip movies with teaser scenes (and with a small chance
also for softcore), that has medium spending power and quality requirement. Their
favourite scene is always a hardcore scene.

### The Merchants
A medium-sized group with medium spending power that mostly accepts all types of 
scenes. They have a medium quality requirement, and a small chance of skipping extreme
scenes. They will not watch forced scenes for fear of this affecting their business.

### The Aristocrats
A small group that might not watch extreme scenes (in public, at least). There is also
a small chance that they skip hardcore scenes. They have high spending power, but also
a high quality requirement.

### The Sadists
A small group that will skip movies with teaser scenes, that has high spending power
and medium quality requirement. Their favourite scene is always an extreme scene.

### The "Artistic" Society
A small group mostly interested in softcore scenes (will skip extreme and maybe
hardcore/teaser). They have a very high quality requirement and high spending power.
Their favourite scene is always softcore or tease/les.


## Scene Production
In order to produce a movie scene, you need an Actress, a Camera Mage, a 
Crystal Purifier and a Director. 
The Movie Studio processes these as follows. First, the Director is handled
(because she also doubles as the Matron -- see TODO). Then we process camera
mages and crystal purifiers. Their own job codes actually doesn't do much but
create a text event based on their skill, but if they don't refuse to work 
they are added to a list inside the MovieStudio that keeps track of all CMs and 
CPs. When the actresses are processed in the second pass, for each produced 
scene we look up one CM and CP based on which ones have produced the least amount
of scenes for now. These are taken to work on the scene, and based on their skill
the production value for the scene is determined. The latter is calculated by
averaging the results of the director, camera mage, and crystal purifier, and 
given double weight to the smallest of the values.

### Fluffers
Fluffers are needed to make sure the male stars are always up to their task. At
the beginning of each turn, the fluffers produce *Fluff Points* which will be used
up during the production the scenes. The amount of *Fluff Points* produced depends
on the skill of the Fluffer and varies between 5 and 10. The amount of *Fluff Points*
required depend on the scene type (roughly Solo Scenes 0, Couple Scenes 5, and 
Orgy Scenes 15) and on the skill of the Actress, and are calculated as
`FLUFF = SCENE_BASE_FLUFF * (110 - SCENE_QUALITY)/110`. Thus, if the actress is
perfect, almost no Fluff is required. 
The quality of the scene is decreased by up to 10 percent if not enough *Fluff Points*
are available.

### Stage Hands
Stage Hands are responsible for making sure production runs smoothly, and for 
managing props etc. Similar to Fluffers, they provide *Stage Hand Points* at the
beginning of production, which are then used up during filming. Contrary to 
Fluffers, the need for stage hands does not decrease if the Actress is better. A 
lack of Stage Hands reduces the scene quality (both content and production value), 
and it also increases the tiredness of the entire crew. Secondly, 
Stage Hands are responsible for keeping your studio clean. Filming in a dirty studio
also results in a worse scene.

### Performance
The girls performance in the different scenes are determined by their Beauty,
Charisma and Performance stats, as well as a skill relevant to the scene at
hand. For the hardcore scenes, Beauty is more important than Charisma, whereas
the softer scenes give more emphasis to Charisma. 

### Scene Jobs
The filming jobs for the scenes are handled in the following way:
1) It is checked whether the scene can be filmed. A scene cannot be filmed if there is
no director, camera mage, or crystal purifier. Furthermore, for the extreme scenes the
   crew might refuse to film if the girl is pregnant or hurt.
2) It is determined whether the girl is willing to perform in the scene. For certain jobs,
   it is possible to go ahead with the filming even if the girl refuses. This is the case
   if the girl is a slave and your disposition is negative. In that situation, the crew
   ties up the girl and proceeds with the filming. Since she is not cooperating, scene
   quality is calculated as if she had zero performance skill. Her hate for you increases,
   and for the extreme scenes her fear too.
   The decision of whether she agrees to the scene is made based on the following factors:
   - Her general rebelliousness
   - Her love/hate/fear for you
   - Her enjoyment for working as an Actress/Pornstar and the secondary action
   - Her lust. This value is based on the Libido stat, the girls skill and some scene-specific 
     values. The jobxml can specify a `<LustInfluence>` element with the following attributes:
     `Amount` -- maximum amount that the rebelliousness is decreased due to libido. 
     `SkillMin` and `Skill` -- If the girl has less than `SkillMin` skill points, she will not
     get a bonus due to her lust. `BaseValue` this is the initial revulsion for performing this
     on film. Thus is her libido is zero, the lust influence is given by `-BaseValue`, and if her
     skill and libido are at 100 the influence is `Amount`.
3) For some jobs, there will be bonuses to the scene depending on certain items the girl 
   possesses. Same with certain traits. TODO this is not implemented yet
4) Scene performance calculation and description: A description text is added to the 
   job event that depends on the girls performance, and potentially on some traits.
5) Virginity and Pregnancy updates.

### Scene Categories
The possible movie scenes are sorted into the following four categories.
#### Teaser Scenes
* Action
* Chef
* Music
* Tease

#### Softcore Scenes
* Titty
* Handjob
* Footjob
* Masturbate
* Strip

#### Hardcore Scenes
* Oral
* Anal
* Sex
* Group

#### Extreme Scenes
* Beast
* BDSM
* Bukkake
* Public BDSM
* Throatfuck


# General Comments
When an unpaid slavegirl is working in a scene, her ASKPRICE stat is used as the cost
for estimating the movie's budget. For CameraMage and Crystal Purifier, the budget is
always scaled as if they had filmed 3 scenes that day.


# IDEAS / TODO
## Movie Production Cost Estimate
Add an estimate how much the movie production cost you, to have a better sense
of how much revenue this is producing for you. This estimate is somewhat difficult,
as the movie studio staff works on more than one scene. Also, there is no direct
pay for slaves, so we should put an estimate there.

## Market Research
Add a "Market Research" job which help in providing feedback about what the moviegoers
like and dislike. This should combine a passive ability and an active ability. For the
the passive ability, market reports are generated depending on the girls jobs performance.
The active ability allows a player to get a specific report for an already released movie,
or for a movie in planning. For this, the girl generates MarketResearchPoints which can
be spent on the actions.

### Passive Effect
##### Visitor Count
${name} observed that most visitors to your movie ... came from the target group ...
${name} counted ... out of ... visitors to be from the target group ...
${name} counted all the visitors in your movie ...: ... from group ..., ... from group ...
and ... from group ...

#### Pricing Hints
${name} found many/few visitors who said that they would have paid even more money to see the movie
${name} interviewed people on the streets and found few/many who would have gone to see the movie if it had been cheaper
${name} determined that increasing/decreasing the ticket priced would have increased profits.
${name} determined that the ideal ticket price would have been ...

#### Target Group Infos
${name} interviewed people from the ... target group and found out that ... is their favourite scene.
${name} estimates the number of people in the .. group to be ...
${name} estimates the amount of capital in the ... group to be ...
${name} estimates the maximum ticket price for the ... group to be ...

#### Composition Hint
...




## Scenes
* Comedy
* Drama
* Horror
* Idol
* Futa
* Dominatrix

## Hype Mechanics
Based on Crew and Cast. In order to implement this, we 
first need UUIDs for characters, and have a global list of all girls in the game.
Then, we can create the initial hype value based on the fame of the involved
actresses (and reduce the effect of fame on the scene production itself).

## Fame
Make Actresses fame rise based on how well their movies perform. Make actresses 
wages depend on how much fame they have?

## Items to influence scene
* Dildo for Mast
* Lingerie for Strip/Tease
