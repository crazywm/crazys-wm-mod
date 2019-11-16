# Girl Events
* NextWeek
* Kidnap
* CustNoPay

# Code Design
We need the following capabilities in our code:
 1) Override default behaviour for mods / single girls in an event specific way. This should combine
    intuitively, i.e. Girl Specific -> Mod -> Default
 2) Multiple scripts can wait for the same event
 3) 
 
# Combat
Combat always happens between two groups of characters, 
henceforth called a *party*, which may consist only of a
single member. Each member of the party has the following 
attributes:
 * HitPoints
 * Mana
 * Constitution
 * Agility
 * Combat
 * Magic

Each party has one of the following goals:
 * Capture
 * Escape
 * Kill

Valid combinations of goals that occur in the game are
 * Escape vs Capture -- When a girl tries to escape from you; When capturing girls
 * Capture vs Kill -- Capturing beasts in the catacombs
 * Kill vs Kill -- When fighting gang against gang
 * Escape vs Kill -- When being attacked in the catacombs

## Combat Rounds
Combat is round based. In each round, the following steps 
happen:
### Determine Move Order
For each participant, a random number is drawn in the range of
`[0, INITIATIVE]`. The `INITIATIVE` value of each contestant is
determined as follows:
```
INITIATIVE = CLIP(2*AGILITY - TIREDNESS + VITALITY, 10, 100) + BONUS
```
If the character is question wants to escape, `BONUS` is 10, 
otherwise 0. Furthermore, some actions of the last turn can
provide a `BONUS` to initiative.
The characters then perform their actions in descending order
of their initiative draws.

### Actions
A character can choose exactly one of the following actions.
Each action has a vitality cost. To chose an action, first
a target is picked randomly from the opponents. Then a score
is calculated for each action, based on costs vs remaining vitality
and the chance of success of the action. The action is then
selected randomly, with the scores as weightings for each option.

#### Physical Attack
`Vit:  100 - Mana: 0`
Physically attack the opponent. Ineffective against
certain targets (e.g. Incorporeal ones). Cannot be
performed by Incorporeal characters. Success depends
on the attackers and defenders combat and agility.
The defender can block or evade the attack. This costs
some vitality for the defender.

In order to physically attack an `ESCAPE`d target,
the attacker ahs first to pursue, which costs additional
`VITALITY`.
A successful physical attack removes the `ESCAPE` flag
from the target. 

#### Magical Attack
`Vit:  20 - Mana: 100`
Magically attack the opponent. Success depends on the
attackers magic skill and the defenders magic and agility 
skills. The defernder can deflect (costs mana) or evade (costs vitality)
the attack.

A successful magical attack might remove the `ESCAPE`
flag from the target.

#### Capture
`Vit:  50 - Mana: 0`
Tries to capture the opponent. Only available when the
party has the `CAPTURE` goal. Success depends on own and
targets agility. Capture chance can be increased by using
a net.

#### Escape
`Vit: 50 - Mana: 0`
Try to flee from the combat. This action is also performed
against an opponent. Success is determined by agility. 
If successful, the character gets the `ESCAPE` flag until
their next action. If the character escapes successfully while the
`ESCAPE` flag is already set, they have escaped for good and
leave the combat.

#### Rest
Restores `VITALITY` and `MANA`, depending on the characters
`CONSTITUTION` and `MAGIC` respectively. This has the same effects
as the regeneration at the end of each round. Also gives an initiative boost
for the next round.


### Other Effects
#### Mana consumption
Whenever a character uses mana, the amount is subtracted from their mana store.
However, there is also a hidden `MAX_MANA` stat, that is decreased by a third of
this amount, and determines how much mana can be regenerated.

#### Crowding
Whenever a character is attacked, their `CROWD` gets increased. If more
than one attack is performed against a character in a single round, there
is a chance of further attacks failing due to crowding.
