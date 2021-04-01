-- This file provides the functions that generate randomized sex messages for different sex acts


---@param girl wm.Girl
---@param customer wm.Customer
function GetGroupMessage(girl, customer)
    local skill = girl:group()
    local ugly = girl:beauty() < 45;
    local no_class = girl:refinement() < 35
    local big_boobs = girl:has_trait("Busty Boobs", "Big Boobs", "Giant Juggs", "Massive Melons", "Abnormally Large Boobs", "Titanic Tits")
    local message = ""
    if girl:has_trait("Zombie") then
        return "The group of customers had fun chasing, tackling and gangbanging their zombie sex toy."
    end

    if skill < 20 then
        -- Gondra: the girl is unskilled
        -- SIN - more spice
        if wm.Percent(33) and (girl:has_trait("Slut", "Nymphomaniac")) then
            --. common traits - added roll to stop this supressing everything else
            message = "At first ${name} seemed to be in her element surrounded by so many \"wonderful\" cocks, but it quickly became apparent that she does not have the experience to satisfy them all."
        elseif wm.Percent(60) and girl:check_virginity() then
            message = "${name} has never fucked ONE person before and had no idea how to handle this kind of group. "
                    .. "She was completely overwhelmed and had no control over what was happening "
                    .. "as strangers twisted and dragged her around while endless cocks were shoved painfully inside her and splurted cum in her face."
            girl:health(-5)
            girl:happiness(-5)
        elseif wm.Percent(60) and (girl:has_trait("Delicate", "Lolita")) then
            message = "This was far too much for a delicate flower like ${name}. By the end she had no control over what happened, as endless cocks rammed into her.";
            girl:health(-3)
            girl:happiness(-1)
        elseif wm.Percent(30) and (girl:has_trait("Optimist", "Quick Learner")) then
            message = "${name} was completely unable to handle this group. While it was damn uncomfortable, being fucked this many ways by this many customers, it was a powerful experience. She's quietly confident she'll do better next time.";
            girl:group(wm.Range(0, 2))
            girl:happiness(1)
        elseif wm.Percent(40) and girl:has_trait("Natural Pheromones") then
            message = "Her powerful pheromones drove the group insane. When the customers finally staggered out one of your staff found ${name} unconscious over a desk with cum coating her face and hair, and dribbling from her pussy, mouth and asshole.";
        elseif wm.Percent(30) and ugly then
            message = "${name} was completely unable to handle this group. The whole experience was awful, especially the bit where they held her to the bed and deliberately splurted stinking cum in her eyes, up her nose and all over her mouth to hide her 'ugly-bitch face.'";
        else
            local choice = wm.Range(0, 4)
            if choice == 0 then
                message = "${name} was overwhelmed by the group surrounding her, barely able to react to what was done to her."
            elseif choice == 1 then
                message = "${name} was being used by the group more than her actively servicing them."
            elseif choice == 2 then
                message = "${name} struggled to service everyone in the group that came to fuck her."
            elseif choice == 3 then
                message = "${name} was powerless as hands pinned her down and groped her, while cocks were stuffed into every hole."
            elseif choice == 4 then
                message = "${name} awkwardly tried to service the group, but was soon overwhelmed as they got bored and took the initiative."
            end
        end
    elseif skill < 40 then
        -- Gondra:  if the girl is slightly skilled
        -- SIN - more spice and variety
        if wm.Percent(35) and girl:has_trait("Plump") then
            message = "${name}'s jiggling body seemed to invite the cocks around her to prod her everywhere as she struggled to satisfy the demands of the group."
        elseif wm.Percent(35) and girl:has_trait("Dick-Sucking Lips") then
            local choice = wm.Range(0, 2)
            message = "${name}'s full, soft lips looked like heaven. Her body was almost untouched as they all had a turn making love to her mouth. She was left "
            if choice == 0 then
                message = message .. "pretty nauseous from all the cum she swallowed down."
                girl:health(-2)
            elseif choice == 1 then
                message = message .. "with a raw throat and cum all over her face and hair."
                girl:health(-1)
                girl:dignity(-1)
            elseif choice == 2 then
                message = message .. "broken, slumped naked in a corner, with cum and drool running from her mouth."
                girl:spirit(-2)
            end
        elseif wm.Percent(35) and big_boobs then
            message = "When ${name}'s pussy, ass and mouth can't keep the group happy, they start squeezing her breasts together and fucking them too.";
        elseif wm.Percent(35) and ugly then
            message = "${name} was barely able to handle this group. The whole experience was pretty bad for her, especially the bit where "
                    .. "they held her to the bed and deliberately splooged stinking cum in her eyes, nose and all over her head to hide 'that ugly-bitch face.'"
        elseif wm.Percent(35) and (no_class or girl:has_trait("Open Minded")) then
            message = "${name} struggled to keep everyone in the group happy, but impressed them with her lack of inhibitions. "
                    .. "Some girls have class, but ${name} was happy to do anything they wanted.";
        elseif wm.Percent(35) and ugly then
            message = "${name} didn't impress them with her looks, but she was okay in the bedroom."
        else
            -- Vanilla Messages
            message = RandomChoice(
                    "${name} was barely able to service everyone, but managed to entertain her customers nonetheless.",
                    "${name} used every trick she knew to keep the whole group satisfied.",
                    "${name} lost count after the first four or five creampies, but they all seemed satisfied.",
                    "${name} struggled to keep all the group satisfied all the time, but certainly gave everyone something to remember."
            )
        end
    elseif skill < 60 then
        -- Gondra: the girl is reasonably skilled
        if wm.Percent(50) and big_boobs then
            message = "${name}'s large chest was the center of attention as she serviced the group, being prodded and fucked by the customers numerous dicks, leaving her chest glazed with layers of cum"
            if girl:has_trait("Cum Addict") then
                message = message .. " which she hungrily licked off as if it was candy."
            else
                message = message .. " which took her quite a bit of time to clean up afterwards."
            end
            -- SIN - a little more variety
        elseif wm.Percent(66) and girl:has_trait("Shape Shifter") then
            message = "As a shape shifter, ${name} is able to fuck and suck the entire group simultaneously. It's not something they're ever likely to forget.";
            customer:happiness(30)
        elseif wm.Percent(66) and girl:has_trait("Shy") then
            message = "For a 'shy' girl, ${name} seems surprising comfortable fucking and sucking an group of random men.";
        elseif wm.Percent(50) and (girl:has_trait("Plump", "Fat")) then
            message = "${name} is pretty skilled at this. Her size just means there's plenty of girl go round.";
        elseif wm.Percent(35) and (no_class or girl:has_trait("Open Minded")) then
            message = "${name} worked hard to keep everyone happy, and impressed them with her lack on inhibitions. "
                    .. "Some girls won't lick a dick that just came inside her, but ${name} didn't seem to care.";
        elseif wm.Percent(35) and no_class then
            message = "The gang were disappointed with ${name}'s looks at first, but she soon made them forget about it.";
        else
            message = RandomChoice(
                    "${name} had some sexy tricks for pleasuring more customers at once than they expected.",
                    "${name} did a good job. When she got up, cum oozed out and was running down her leg for over a minute.",
                    "${name} managed to surprise a few of her customers as she pleasured more of them at the same time than they had thought possible.",
                    "${name} serviced everyone in the group of customers that fucked her."
            )
        end
    elseif skill < 80 then
        -- /Gondra: the girl is very skilled
        -- SIN - some reworking and a little added variety
        -- roll added to stop these common traits suppressing everything else
        if wm.Percent(40) and (girl:has_trait("Deep Throat", "No Gag Reflex")) then
            message = "After seeing ${name}'s throat easily handling the largest cock in the group, they all took turns cumming deep in her throat."
            if girl:has_trait("Cum Addict") then
                message = message .. " Leaving her happy and full."
            else
                message = message .. " Leaving her looking a tiny bit ill because of the sheer amount of cum forced down her throat in such a small amount of time."
                -- Gondra: chance to gain cum addict?
            end
        elseif wm.Percent(40) and girl:has_trait("Natural Pheromones") then
            message = "Her powerful pheromones drove the group insane. Luckily she was skilled enough to keep up with them all. "
                    .. "Many orgasms later, she lay amid her boys in a naked bundle of sweat, semen and satisfied smiles.";
        elseif wm.Percent(50) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = "While it certainly isn't the only thing the group uses, ${name}'s great arse sees near constant use, always a fresh cock ready to make her backside ripple when the previous one is done filling her insides with creamy white cum."
        elseif wm.Percent(50) and girl:has_trait("Slut", "Nymphomaniac") then
            message = "${name} was definitely in her element surrounded by so many \"wonderful\" cocks, and she refused to stop until she had drained every one dry."
        elseif wm.Percent(35) and (no_class or girl:has_trait("Open Minded")) then
            message = "${name} impressed the group with her total absence of inhibitions, licking and sucking *anything* no matter where it had been, and doing everything they could imagine.";
        elseif wm.Percent(35) and ugly then
            message = "The gang were disappointed with ${name}'s looks at first, but she soon made them forget about it.";
        else
            -- Gondra: Vanilla Messages
            message = RandomChoice(
                    "${name} was praised for her enthusiastic multitasking, which left everyone satisfied and a bit exhausted.",
                    "${name} fucked and came many times with everyone in the group of customers."
            )
        end
    else
        message = "${name} "
        local choice = wm.Range(1, 4)
        if choice == 1 then
            message = message .. "counted the number of customers: " .. wm.Range(5, 25) .. "."
            message = message .. RandomChoice(
                    "This was going to be rough", "Sweet", "It could be worse", "A smile formed on her lips. This was going to be fun",
                    "Boring", "Not enough", "'Could you get more?' she wondered"
            )
        elseif choice == 2 then
            message = message .. "was lost in " .. RandomChoice("a sea", "a storm", "an ocean", "a jungle") .. " of hot bodies"
        elseif choice == 3 then
            message = message .. RandomChoice("sat", "lay", "stood", "crouched") .. " " .. RandomChoice("", "blindfolded and ") ..
                    RandomChoice("handcuffed", "tied up", "wrists bound in rope", "wrists in chains hanging from the ceiling") ..
                    " in the middle of a " .. RandomChoice("circle", "smouldering pile", "phalanx", "wall") .. " of flesh."
        elseif choice == 4 then
            message = message .. "was " .. RandomChoice("amazed by", "disappointed by", "overjoyed with", "ecstatically happy with") ..
                    " the " .. RandomChoice("brigade", "army group", "squad", "battalion") .. " of " ..
                    RandomChoice("man meat", "cock", "muscle", "horny, brainless thugs") .. " around her"
        end

        choice = wm.Range(1, 8)
        if choice == 1 then
            message = message .. "She was thoroughly " .. RandomChoice("banged", "fucked", "disappointed", "penetrated") ..
                    " by"
        elseif choice == 2 then
            message = message .. "They handled her like " .. RandomChoice("an expensive", "a cheap") ..
                    RandomChoice("hooker", "street worker", "violin", "wine") .. " for"
        elseif choice == 3 then
            message = message .. "Her " .. RandomChoice("holes were", "love canal was") .. " plugged by"
        elseif choice == 4 then
            message = message .. "She " .. RandomChoice("was bukkaked by", "was given pearl necklaces by", "received a thorough face/hair job from")
        elseif choice == 5 then
            message = message .. "They demanded simultaneous hand, foot and mouth " .. RandomChoice("jobs", "action", "combat") .. " for"
        elseif choice == 6 then
            message = message .. "There was a positive side: 'So much " .. RandomChoice("meat", "cock", "testosterone", "to do") ..
                    "\", so little time' she said to"
        elseif choice == 7 then
            message = message .. "They made sure she had a nutritious meal of " .. RandomChoice("man meat", "cock", "penis", "meat rods") ..
                    " and drinks of delicious " .. RandomChoice("semen", "man mucus", "man-love", "man-cream") .. " from"
        elseif choice == 8 then
            message = message .. "She was " .. RandomChoice("fucked", "banged", "humped", "sucked") .. " silly " ..
                    RandomChoice("twice over", "three times over", "so many times") .. " by"
        end

        message = message .. " "
        choice = wm.Range(1, 11)
        if choice == 1 then
            message = message .. "every member of the Crossgate's " .. RandomChoice("roads crew.", "administrative staff.", "interleague volleyball team.", "short persons defense league.")
        elseif choice == 2 then
            message = message .. "all the " .. RandomChoice("lieutenants", "sergeants", "captains", "junior officers") ..
                    " in the Mundigan " .. RandomChoice("army.", "navy.", "elite forces.", "foreign legion.")
        elseif choice == 3 then
            message = message .. "the visiting " .. RandomChoice("half-giants. (Ouch!)", "storm giants.", "titans.", "ogres.")
        elseif choice == 4 then
            message = message .. "the " .. RandomChoice("Hentai Research", "Women's Rights", "Prostitution Studies", "Celibacy") ..
                    " club of the University of Cunning Linguists."
        elseif choice == 5 then
            message = message .. "the squad of " .. RandomChoice("hard-to-find ninjas", "raucous pirates") .. "."
        elseif choice == 6 then
            message = message .. "a group of people from some place called the 'Pink Petal forums'."
        elseif choice == 7 then
            message = message .. "the seemingly endless " .. RandomChoice("army", "horde", "number", "group") .. "of really" ..
                    RandomChoice(", really ", " ") .. RandomChoice("fired up", "horny", "randy", "backed up") .. " " ..
                    RandomChoice("gnomes", "halflings", "kobolds", "office workers") .. "."
        elseif choice == 8 then
            message = message .. "CSI " .. RandomChoice("New York", "Miami", "Mundigan", "Tokyo") .. " branch"
        elseif choice == 9 then
            message = message .. RandomChoice("frosh", "seniors", "young adults", "women") .. " on a " .. RandomChoice("", "serious ") ..
                    RandomChoice("bender", "road trip", "medical study", "lark") .. "."
        elseif choice == 10 then
            message = message .. "all the " .. RandomChoice("second stringers", "has-beens", "never-weres", "victims") .. " from the " ..
                    wm.Range(1991, 2020) .. " H anime season."
        elseif choice == 11 then
            message = message .. "Grandpa Parkins and his extended family of " .. wm.Range(100, 200) .. "."
            if wm.Percent(50) then
                message = message .. " (And " .. wm.Range(50, 150) ..
                        RandomChoice(" guests", " more from the extended extended family") .. ".)"
            end
        end
    end
    return message
end

---@param girl wm.Girl
---@param customer wm.Customer
function GetBestialityMessage(girl, customer)
    local skill = girl:beastiality()
    local message = ""

    if girl:has_trait("Zombie") then
        -- SIN - ADDED
        message = "This customer wanted to see an undead girl fucked by a beast. "
        -- TODO Beast Availability
        if wm.GetBeasts() == 0 then
            message = message .. "Unfortunately there were no beasts, so a fake was used. It's not the same.\n";
            customer:happiness(-30)
        end
        message = message .. "\n${name}"
        if wm.Percent(50) then
            message = message .. " was docile and unresponsive"
            if wm.Percent(50) then
                message = message .. " as the beast tried a number of ways to mate with her cold, motionless body.\nThe customer was entertained.\n";
                customer.happiness(30)
            else
                message = message .. ". The beast instinctively recoiled, refusing to go anywhere near her.\nThe customer was disappointed.\n"
                customer.happiness(-30)
            end
        else
            message = message .. " was irrational and aggressive. With an angry grunt she lurched and clawed at the amorous beast"
            if wm.Percent(girl:combat()) then
                message = message .. " quickly tearing the poor creature apart and feasting on its flesh.\nThe customer seemed a little shocked.\n";
                wm:AddBeasts(-1)
                girl:happiness(5)
                girl:health(5);
                customer.happiness(-10)
            else
                message = message .. ", but your beast easily overpowered her.\nThe customer is thrilled to watch as your beast pins and fucks this furious, snarling zombie-girl.\n";
                girl:health(-10)
                customer.happiness(50)
            end
        end
        return message
    end

    -- availability of beasts / harming of beasts
    if wm.GetBeasts() == 0 then
        message = message .. "${name} found that there were no beasts available, so some fake ones were used. This disappointed the customer somewhat."
        customer:happiness(-10)
    else
        local harmchance = -(girl:beastiality() + girl:animalhandling() - 50)  -- 50% chance at 0 skill, 1% chance at 49 skill
        local harm = 1
        if girl:has_trait("Aggressive") then
            harm = harm + 3
        end
        if girl:has_trait("Assassin") then
            harm = harm + 1
        end
        if girl:has_trait("Merciless") then
            harm = harm + 1
        end
        if girl:has_trait("Sadistic") then
            harm = harm + 2
        end
        if girl:has_trait("Twisted") then
            harm = harm + 1
        end
        if wm.Percent(harmchance) then
            message = message .. "${name} accidentally harmed some beasts during the act.\n"
            wm:AddBeasts(-wm.Range(1, 3))
        elseif wm.Percent(harm) then
            message = message .. "${name} \"accidentally\" harmed a beast during the act.\n" --Made it actually use quote marks CRAZY
            wm:AddBeasts(-1)
        end
    end

    -- now the actual act
    if skill < 20 then
        -- Gondra: if the girl is unskilled show one of these messages
        -- Gondra: Trait messages
        if girl:has_trait("Cow Girl") then
            message = message .. "Held down by the customer, ${name} gritted her teeth as the beast penetrated her roughly, mooing with definite discomfort when the customer told her to do so.";
        else
            -- Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
            message = message .. RandomChoice("Initially the customers was amused as the beast pounced ${name}, who was not able to handle the beast, but the panicked shrieks of the girl as the horny monster tried to force itself onto her killed his mood though.",
                    "${name} was disgusted by the idea but still allowed the customer to watch as she was fucked by some animals.")
        end
    elseif skill < 40 then
        -- Gondra:  if the girl is slightly skilled
        if girl:has_trait("Canine", "Cat Girl") then
            -- //Gondra: Trait messages
            message = message .. "The beast seemed to be a bit wary of ${name} but came closer as the customer made her present herself, fucking her hard as the customer watched."
            -- //Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
        else
            message = message .. RandomChoice(
                    "${name} was not very enthusiastic about this, but allowed the customer to direct the beast to mount her.",
                    "${name} was a little put off by the idea, but still allowed the customer to watch and help as she was fucked by animals."
            )
        end
    elseif skill < 60 then
        -- Gondra: the girl is reasonably skilled
        if girl:has_trait("Prehensile Tail", "Playful Tail") then
            message = message .. "${name} playfully lifted her tail, presenting her wet cunt to the beast which eagerly filled her needy cunt as the customer stroked his length."
        elseif girl:has_trait("Fertile") or girl:has_trait("Broodmother") then
            message = message .. "The beast kept pumping its cum deep into ${name}'s pussy, leaving her belly a bit distended with cum afterwards, making the customer remark that the monster seemed intent on making her carry its progeny.";
        else
            message = message .. RandomChoice(
                    "Tentacles wriggled themselves into ${name}'s welcoming holes, making her cum while the customer leered at them.",
                    "${name} took a large animal's cock deep inside her and enjoyed being fucked by it, her cries of pleasure being muffled by the customer's cock in her mouth."
            )

        end
    elseif skill < 80 then
        if girl:has_trait("Cum Addict") then
            message = message .. "${name} eagerly swallowed every drop of cum she could squeeze from the many cocks of the beast while it fucked her, only pausing for a moment to receive the customers load."
        elseif girl:has_trait("Cat Girl", "Canine") then
            message = message .. "${name} acted like she was in heat as she kept fucking the beasts, cumming too often to count, fully embracing her wild side until she had exhausted the monsters."
        elseif girl:has_trait("Cum Addict") then
            message = message .. "${name} captivated the customer with her performance of a noble elf falling into depravity. Although she could not really hide that she was well experienced, she made it up to the customer by subtly making it so that he could see the beast fucking her from the best angle as she enjoyed herself getting railed by the monster."
        else
            message = message .. RandomChoice(
                    "${name} made sure that the customer could see the monsters many tentacles and cocks fucking her, as she came again and again.",
                    "${name} fucked some exotic beasts covered with massive cocks and tentacles, she came over and over alongside the customer."
            )
        end
    else
        -- Gondra: the girl is EXTREMELY skilled
        message = message .. "${name} was "
        local choice = wm.Range(1, 7)
        if choice == 1 then
            message = message .. "filled with " .. RandomChoice("vibrating", "wiggling", "living", "energetic", "big", "pokey") ..
                    " things that " .. RandomChoice("tickled", "pleasured", "massaged", "scraped") .. " her insides"
        elseif choice == 2 then
            message = message .. "forced against " .. RandomChoice("a wall", "a window", "another client", "another girl") ..
                    " and told to " .. RandomChoice("spread her legs", "give up hope", "hold on tight", "smile through it")
        elseif choice == 3 then
            message = message .. "worried by the " .. RandomChoice("huge size", "skill", "reputation", "aggressiveness") .. " of the client"
        elseif choice == 4 then
            message = message .. "stripped down to her " .. RandomChoice("g-string", "panties", "bra and panties", "teddy", "skin") ..
                    " and covered in" .. RandomChoice("compliments", "abuse", "peanut butter", "honey", "motor oil")
        elseif choice == 5 then
            message = message .. "chained up in the " .. RandomChoice("dungeon", "den", "kitchen",
                    "most public of places", "backyard") .. " and her " .. RandomChoice("arms", "legs", "arms and legs") .. " were lashed to posts"
        elseif choice == 6 then
            message = message .. RandomChoice("tied up", "wrapped up", "trapped", "bound", "covered") .. " in " ..
                    RandomChoice("flesh", "tentacles", "cellophane", "tape", "false promises") .. " and " ..
                    RandomChoice("groped", "tweaked", "licked", "spanked", "left alone") .. " for hours"
        elseif choice == 7 then
            message = message .. "pushed to the limits of " .. RandomChoice("flexibility", "endurance", "patience", "consciousness", "sanity") ..
                    " and " .. RandomChoice("cried out", "swooned", "spasmed")
        end

        choice = wm.Range(1, 9)
        if choice == 1 then
            message = message .. "She " .. RandomChoice("smoothly", "roughly", "lustily", "repeatedly", "orgasmically") .. " " ..
                    RandomChoice("fucked", "railed", "banged", "screwed", "pleasured")
        elseif choice == 2 then
            message = message .. "She was " .. RandomChoice("teased", "taunted", "roughed up") .. " and " ..
                    RandomChoice("orally", "mentally", "repeatedly", "haughtily", "") .. " violated by"
        elseif choice == 3 then
            message = message .. "She was drenched in " .. RandomChoice("cum", "sweat", "broken hopes and dreams", "Koolaid", "sticky secretions") .. " by"
        elseif choice == 4 then
            message = message .. "She " .. RandomChoice("moaned", "winced", "swooned", "orgasmed", "begged for more") .. " as her stomach repeatedly poked out from "
                    .. RandomChoice("thrusts", "strokes", "fistings", "a powerful fucking") .. " by"
        elseif choice == 5 then
            message = message .. "She used her "
            local useand = false
            if wm.Percent(50) then
                message = message .. "hands, "
                useand = true
            end
            if wm.Percent(50) then
                message = message .. "feet, "
                useand = true
            end
            if wm.Percent(50) then
                message = message .. "mouth, "
                useand = true
            end
            if useand then
                message = message .. "and "
            end
            message = message .. RandomChoice("pussy", "holes") .. " to " .. RandomChoice("please", "pleasure", "tame")
        elseif choice == 6 then
            message = message .. "She shook with " .. RandomChoice("orgasmic joy", "searing pain", "frustration", "agony", "frustrated boredom") .. " when fondled by"
        elseif choice == 7 then
            message = message .. "It felt like she was " .. RandomChoice("ripping apart", "exploding", "imploding", "nothing", "absent") .. " when handled by"
        elseif choice == 8 then
            message = message .. "She passed out from " .. RandomChoice("pleasure", "pain", "boredom", "rough sex", "inactivity") .. " from"
        elseif choice == 9 then
            message = message .. "She screamed as " .. RandomChoice("wrenching pain", "powerful orgasms", "incredible sensations", "freight trains", "lots and lots of nothing") ..
                    " thundered through her from"
        end

        choice = wm.Range(1, 12)
        if choice == 1 then
            message = message .. "the ravenous " .. RandomChoice("tentacles.", ", sex-starved essences of lust.", "Balhannoth. (Monster Manual 4, pg. 15.)", "priest.", "Yugoloth.")
        elseif choice == 2 then
            message = message .. RandomChoice("an evil", "a misunderstood") .. " " .. RandomChoice("tengu.", "Whore Master developer.", "school girl.", "garden hose.", "thought.")
        elseif choice == 3 then
            message = message .. "a major " .. RandomChoice("demon", "devil", "oni", "fire elemental", "god", "Mr. Coffee") .. " from the outer planes."
        elseif choice == 4 then
            message = "the angel."
            if wm.Percent(50) then
                message = message .. " ('" .. RandomChoice("You're very pretty", "I was never here", "I had a great time", "I didn't know my body could do that") .. "' he said.)"
            end
        elseif choice == 5 then
            message = message .. "the " .. RandomChoice("demon", "major devil", "oni", "earth elemental", "raging hormone beast", "Happy Fun Ball") ..
                    " with an urge to exercise his " .. wm.Range(20, 50) .. " cocks and " .. wm.Range(20, 50) .. " claws"
        elseif choice == 6 then
            message = message .. "the swamp thing with (wait for it) swamp tentacles!"
        elseif choice == 7 then
            message = message .. "the pirnja gestalt. (The revolution is coming.)"
        elseif choice == 8 then
            -- MYR: Not a mistake. I meant to write 'seven'.
            message = message .. "the color " .. RandomChoice("purple", "seven", "mauve", "silver", "ochre", "pale yellow") .. "."
        elseif choice == 9 then
            message = message .. wm.Range(5, 15) .. " werewolves wearing " .. wm.Range(1, 10) .. RandomChoice("true", "minor artifact", "greater artifact", "godly", "near omnipotent") ..
                    " rings of the " .. RandomChoice("eternal", "body breaking", "corporal", "transcendent", "incorporeal") .. " hard-on."
        elseif choice == 10 then
            message = message .. wm.Range(1, 12) .. " Elder Gods."
            if wm.Percent(30) then
                message = message .. " (She thought " .. RandomChoice("Cthulhu", "Hastur", "an Old One", "Shub-Niggurath", "Nyarlathotep", "Yog-Sothoth") ..
                        " was amongst them, but blacked out after a minute or so.)"
            end
        elseif choice == 11 then
            message = message .. "the level " .. wm.Range(25, 45) .. " epic paragon " .. RandomChoice("troll", "beholder", "displacer beast", "ettin", "gargoyle", "fire extinguisher") ..
                    " with " .. RandomChoice(20, 40) .. " strength and " .. RandomChoice(20, 40) .. " constitution."
        elseif choice == 12 then
            message = message .. "the phalanx of " .. RandomChoice("horny orcs.", "goblins.", "sentient marbles.", "living garden gnomes.", "bugbears.")
        end
    end
    return message
end

---@param girl wm.Girl
---@param customer wm.Customer
function GetBDSMMessage(girl, customer)
    local skill = girl:bdsm()
    local message = ""

    if girl:has_trait("Zombie") then
        return "${name} resisted a little as her customer tied her up and played with her."
    end

    if skill < 20 then
        -- Gondra: if the girl is unskilled show one of these messages
        if girl:has_trait("Masochist") then
            --Gondra: Trait messages
            -- Gondra: Would this one even show up? I know Masochist gives +BDSM but not how much
            -- `J` while masochist gives +50 bdsm (probably a little too high) other things could reduce it below 20
            message = message .. "While ${name} was visibly uncomfortable, she was eager to learn more about this 'interesting' act after the fact."
        elseif wm.Percent(66) and girl:is_pregnant() then
            if girl:weeks_pregnant() < girl:pregnancy_term() / 4 then
                -- if she's not showing
                message = "${name} was awkward with the customer, refusing to do anything too extreme for fear of harming her unborn child."
                customer:happiness(-10)
            elseif wm.Percent(10) then
                message = "${name} was bound and roughly used by some sicko who seemed to get off on her pregnancy."
                customer:happiness(20)
                girl:enjoyment(ACTION_SEX, -5)
                girl:happiness(-5)
            else
                if girl:weeks_pregnant() < ((3 * girl:pregnancy_term()) / 4) then
                    -- if she's showing
                    message = "Both ${name} and her customer were reluctant to do any real bondage while she is bearing child."
                    customer:happiness(-20)
                else
                    -- if she's about to drop
                    message = "\"Can you do this in your condition?\"\nWith ${name}'s very obvious pregnancy, the customer was reluctant to do anything at all for fear harming her unborn child."
                    customer:happiness(-30)
                end
            end
        elseif wm.Percent(66) and girl:has_trait("Mind Fucked") then
            message = "Naked, hollow-eyed and open-mouthed ${name} smiled at the customer drooling "
            message = message .. RandomChoice("and singing a Nursery Rhyme as she brutally slapped her clit.",
                    "and crying as she fisted herself.",
                    "into her hands, and then wiping the slobber over her face and giggling like a child.",
                    "and purring as she played with the candle, its wax and its exquisite flame.",
                    "and laughing as she played with the high-voltage toys and insertions."
            ) .. "\nShe lives for bondage. Unfortunately she had the customer very freaked out and unable to get aroused.\n"
        elseif wm.Percent(33) and girl:has_trait("Mute") then
            message = "${name} was tied down for a BDSM session.\nNot realising ${name} was mute, the customer took her silence as a challenge, and grew increasingly "
                    .. "brutal in his efforts to get a scream out of 'this insolent bitch.'\nHe was frustrated, and she won't sit comfortably for a few weeks."
            girl:health(-10)
        elseif wm.Percent(25) and girl:has_trait("Shy") then
            message = "Annoyed by ${name}'s boring shyness, the customer finally jacked off in her face, zipped up, angrily flung open the door and left, "
                    .. "leaving waiting customers with a clear view of her butt-naked, chained spread-eagled to a rack, as cum ran down her face. A number of them wolf-whistled.";
            girl:dignity(-5)
            girl:fame(5)
        elseif false and wm.Percent(25) then
            -- and girl:sanity() < 25 then
            -- TODO SANITY
            -- this was commented out in the C++ code, because no sanity stat exists
            message = "The customer was unsettled by this BDSM session. As he " .. InsaneBDSM(girl) .. " It was just weird.\n"
            girl:fame(5)
        else
            message = RandomChoice(
                    "${name} was horrified as the customer clamped things on her, shoved things in her, and whipped, hurt and deliberately degraded her for his own sexual gratification.",
                    "${name} was frightened by being tied up and having pain inflicted on her.",
                    "${name} wept in pain and humiliation as the customer poured hot candle-wax on her sensitive parts.",
                    "Being unfamiliar with the tools of this part of the trade, ${name} had a questioning look on her face that made it hard for the customer to enjoy themselves."
            )
        end

        if wm.Percent(20) then
            -- customer goes wild - unlikely and only affects lower level girls (more skilled girls can take it)
            message = message .. HandleBDSMAggressiveCustomer(girl, customer)
        end
    elseif skill < 40 then
        -- Gondra:  if the girl is slightly skilled
        if girl:has_trait("Masochist") then
            -- Gondra: Trait messages
            message = message .. "${name} eagerly let herself be bound by the customer, visibly enjoying herself as the customer began inflicting pain on her. It wasn't that great for him though.";
        elseif wm.Percent(60) and girl:is_pregnant() then
            if girl:weeks_pregnant() < (girl:pregnancy_term() / 4) then
                -- if she's not showing
                message = "${name} was a little awkward with the customer, refusing to do things that may harm her unborn child."
                customer:happiness(-5)
            elseif wm.Percent(10) then
                message = "${name} was bound and roughly used by some sicko who seemed to get off on her pregnancy."
                customer:happiness(20)
                girl:enjoyment(ACTION_SEX, -2)
                girl:happiness(-2)
            else
                if girl:weeks_pregnant() < ((3 * girl:pregnancy_term()) / 4) then
                    -- if she's showing
                    message = "The customer was reluctant to do bondage while ${name} is bearing child, but she told him some things would be okay."
                    customer:happiness(-5)
                else
                    -- if she's about to drop
                    message = "\"Can you do this in your condition?\"\nWith her advanced pregnancy, the customer and ${name} proceeded carefully, so as not to harm her unborn child.";
                    customer:happiness(-10)
                end
            end
        elseif wm.Percent(33) and girl:has_trait("Plump", "Fat") then
            message = message .. "The customer tied ${name}to a rack, where he used a paddle to spank her fat ass, her wobbly thighs and her flabby breasts, sending fat waves rippling all over her body. "
                    .. "The pain was too much for her, and her whimpering totally killed his mood.";
        else
            message = message .. RandomChoice("${name} was upset as the customer clamped things on her nipples and shoved things in her mouth, deliberately degrading her for his amusement.",
                    "${name} was not enjoying being bound and hurt, but endured it.",
                    "${name} squealed and struggled as the customer dripped sizzling candle-wax on sensitive areas.",
                    "${name} was still a bit scared as the customer began applying the bondage gear on her body, but didn't really show it.")
        end

        if wm.Percent(20) then
            -- customer goes wild - unlikely and only affects lower level girls (more skilled girls can take it)
            message = message .. HandleBDSMAggressiveCustomer(girl, customer)
        end
    elseif skill < 60 then
        -- Gondra: the girl is reasonably skilled
        if wm.Percent(40) and girl:has_trait("Masochist") then
            message = "Once bound, ${name} was already beginning to show visible arousal, that only intensified as the customer started to use the various tools available on her."
        elseif wm.Percent(50) and girl:is_pregnant() then
            if girl:weeks_pregnant() < girl:pregnancy_term() / 4 then
                -- if she's not showing
                message = "${name} provided an exciting bondage session, while subtly steering the customer away from things that might be harmful to her unborn child."
            elseif wm.Percent(10) then
                message = "${name} was bound and roughly used by some sicko who got off on her pregnancy. She enjoyed it."
                customer:happiness(20)
                girl:enjoyment(ACTION_SEX, -2)
                girl:happiness(-2)
            else
                if girl:weeks_pregnant() < ((3 * girl:pregnancy_term()) / 4) then
                    -- if she's showing
                    message = "Seeing her condition, the customer was reluctant to do bondage. ${name} reassured him and showed him there was lots he could still do to her.";
                else
                    -- if she's about to drop
                    message = "Seeing her advanced pregnancy, the customer figured bondage was impossible. ${name} helped him see there were lots of fun things he could still do to her."
                            .. " She squeezed her breasts, watching him watch the milk run down."
                end
            end
        elseif wm.Percent(30) and girl:has_trait("Pierced Clit", "Pierced Nipples", "Pierced Tongue") then
            local action = ""
            if girl:has_trait("Pierced Clit") then
                action = "Her clit piercing got extra special attention.\n"
            elseif girl:has_trait("Pierced Nipples") then
                action = "He 'led' her between tools using a chain on her nipple piercings.\n"
            else
                action = "He pulled her around using a chain on her tongue piercing.\n"
            end
            message = "The customer showed real imagination involving ${name}'s piercings in the BDSM action. " .. action
        elseif wm.Percent(30) and girl:has_trait("Phat Booty", "Deluxe Derriere", "Wide Bottom", "Plump Tush") then
            message = "${name} was aroused being abused by the customer. He particularly enjoyed spanking her ass and thighs, just to watch her booty ripple."
        elseif wm.Percent(30) and (girl:has_trait("MILF", "Whore")) then
            message = "The customer ties ${name} upside-down and starts inserting large 'toys' in her pussy. An impressive number fit inside.\n"
                    .. "She enjoys this.\n"
        elseif wm.Percent(30) and girl:has_trait("Smoker") then
            message = "While 'playing' with her, the customer finds ${name}'s cigarettes and carefully singes her with them.\n"
                    .. "She actually quite enjoys this.\n";
        elseif false and wm.Percent(25) then
            -- and girl:sanity() < 25 then
            -- TODO SANITY
            -- this was commented out in the C++ code, because no sanity stat exists
            message = "The customer had a great time in this BDSM session. As he " .. InsaneBDSM(girl) .. " It was just weird.\n"
            girl:fame(5)
        else
            message = RandomChoice("${name} was aroused as the customer deliberately hurt, used and degraded her for his sexual gratification.",
                    "${name} was a little turned on by being tied up and having the customer hurting her.",
                    "${name} squealed and groaned as the customer stimulated her sensitive areas with scalding candle-wax.",
                    "Being at the mercy of the customer was something ${name} actually found herself enjoying a bit.")
        end
    elseif skill < 80 then
        if wm.Percent(30) and girl:has_trait("Masochist") then
            -- Gondra: Trait messages
            message = "After telling the customer to hit her harder several times, ${name} found herself gagged. Her now muffled cries seemingly adding to the enjoyment of both her and her customer.";
        elseif wm.Percent(30) and girl:has_trait("No Gag Reflex", "Deep Throat") then
            message = "${name} found her drooling mouth filled by the customers hard, pulsing cock, as he continued to slap her bound body, enjoying his impromptu gag service by her throat."
        elseif wm.Percent(40) and girl:is_pregnant() then
            if girl:weeks_pregnant() < (girl:pregnancy_term() / 4) then
                -- if she's not showing
                message = "${name} gave an amazing bondage session, steering the customer away from things that might be harmful to her unborn child, "
                        .. "without him ever noticing."
            elseif wm.Percent(10) then
                message = "${name} was bound and roughly used by some sicko who got off on her pregnancy. She loved it: "
                        .. "it was great to be treated like a real whore again, and not some little china doll. She completely forgot herself and gave him an amazing time."
                customer:happiness(20)
                girl:enjoyment(ACTION_SEX, -2)
                girl:health(-4)
            else
                if girl:weeks_pregnant() < ((3 * girl:pregnancy_term()) / 4) then
                    -- if she's showing
                    message = "Due to her obvious pregnancy, the customer was reluctant to do bondage.\n${name} shoved her hands into his pants and grabbed him by the balls. "
                            .. "\"If you're not man enough, I can take charge?\"\nWith just the right blend of goading and encouragement, ${name} pushed the customer past his inhibitions, "
                            .. "and they had an amazing time.";
                    girl:health(-1)
                else
                    -- if she's about to drop
                    message = "Seeing her advanced pregnancy, the customer figured bondage was impossibe. ${name} helped him see there were lots of fun things he could still do to her."
                            .. " She squeezed her breasts, squirting milk in his eye.\n\"Mister,\" she smiled, impudently. \"Are you going to let me get away with that?!\""
                    girl:health(-1)
                end
            end
        elseif wm.Percent(30) and girl:has_trait("Pierced Clit", "Pierced Nipples", "Pierced Tongue") then
            local action = ""
            if girl:has_trait("Pierced Clit") then
                action = "Her clit piercing was definitely the most 'useful'.\n"
            elseif girl:has_trait("Pierced Nipples") then
                action = "Her nipple piercings were useful both for control, and for target practice.\n"
            else
                action = "The tongue-piercing was great for holding her mouth open while he face-fucked her.\n"
            end
            message = "${name} urged the customer to use her piercings while torturing her. " .. action
        elseif wm.Percent(33) and girl:has_trait("Plump", "Fat") then
            message = "The customer tied ${name} to a rack and spanked her fat ass, her wobbly thighs and her flabby breasts with a paddle, sending fat waves rippling all over. "
                    .. "She was into the pain and humiliation, forcing the customer to degrade her further. They ended up drenched with sweat and cum.";
        elseif false and wm.Percent(25) then
            -- and girl:sanity() < 25 then
            -- TODO SANITY
            -- this was commented out in the C++ code, because no sanity stat exists
            message = "The customer loved this BDSM session. ${name} was completely crazy, and as he " .. InsaneBDSM(girl) .. " He didn't need to be told twice, and got so into the session.\n"
            girl:fame(10)
        elseif wm.Percent(30) and girl:has_trait("Flat Chest", "Petite Breasts", "Small Boobs") then
            message = "The customer repeatedly spanked and slapped her 'pathetic little breasts', demanding that she grow some. ${name} was aroused from the pain and degradation."
        elseif wm.Percent(30) and girl:has_trait("Mute") then
            message = "When he realises she can't scream, the customer takes the BDSM to a whole new level. Luckily she's tough and into it, "
                    .. "and they both have a great time."
        else
            message = RandomChoice("${name} was aroused as the customer deliberately hurt, used and degraded her for his sexual gratification.",
                    "Thoroughly bound, ${name} found herself being teased endlessly by the customers cock and hands, coming hard under his expert care shortly before the end of the session.",
                    "${name} was aroused as the customer singed her sensitive areas with candle-wax, begging him for more.",
                    "${name} was highly aroused by the pain and bondage, even more so when fucking at the same time.")
        end
    else
        -- Gondra: the girl is EXTREMELY skilled
        message = "${name} was " .. RandomChoice("dressed as a dominatrix", "stripped naked", "dressed as a (strictly legal age) school girl", "dressed as a nurse", "put in heels", "covered in oil")
        if wm.Percent(33) then
            message = message .. RandomChoice(", rendered helpless by drugs", ", restrained by magic")
        end
        if wm.Percent(25) then
            message = message .. ", blindfolded"
        end
        if wm.Percent(25) then
            message = message .. ", gagged"
        end
        message = message .. ", and " .. RandomChoice("chained", "lashed", "tied", "bound", "cuffed", "leashed")
        if wm.Percent(25) then
            message = message .. ", arms behind her back"
        end
        if wm.Percent(25) then
            message = message .. ", fettered"
        end
        if wm.Percent(25) then
            message = message .. ", spread eagle"
        end
        if wm.Percent(25) then
            message = message .. ", upside down"
        end
        message = message .. " " .. RandomChoice("to a bed", "to a post", "to a wall", "to vertical stocks", "to a table", "on a wooden horse", "in stocks", "at the dog house")

        local choice = wm.Range(1, 8)
        if choice == 1 then
            message = message .. "She was fucked " .. RandomChoice("with a rake", "with a giant dildo", "and flogged", "and lashed", "tenderly", "like a dog") .. " by"
        elseif choice == 2 then
            message = message .. "Explanations were necessary before she was " .. RandomChoice("screwed", "penetrated", "abused", "whipped", "yelled at", "banged repeatedly", "smacked around") .. " by"
        elseif choice == 3 then
            message = message .. "Her holes were filled " .. RandomChoice("with wiggly things", "with vibrating things", "with sex toys", "by things with uncomfortable edges",
                    "with marbles", "with foreign objects", "with hopes and dreams", "with semen") .. " by"
        elseif choice == 4 then
            message = message .. "A massive aphrodisiac was administered before she was " ..
                    RandomChoice("teased", "fucked", "left alone", "repeatedly brought to the edge of orgasm, but not over", "mercilessly tickled by a feather wielded") .. " by"
        elseif choice == 5 then
            message = message .. "Entertainment was demanded before she was " .. RandomChoice(
                    "humped", "rough-housed", "pinched over and over", "probed by instruments", "fondled roughly", "sent away"
            ) .. " by"
        elseif choice == 6 then
            message = message .. "She was pierced repeatedly by " .. RandomChoice("needles", "magic missiles", "evil thoughts") .. " from"
        elseif choice == 7 then
            message = message .. RandomChoice("Weights", "Christmas ornaments", "Lewd signs", "Trinkets", "Abstract symbols")
                    .. " were hung from her unmentionables by"
        elseif choice == 8 then
            message = message .. "She was ordered to " .. RandomChoice("flail herself", "perform fellatio", "masturbate", "beg for it") .. " by"
        end

        choice = wm.Range(1, 18)
        if choice == 1 then
            message = message .. "Iron Man"
        elseif choice == 2 then
            message = message .. "the " .. RandomChoice("wizard", "sorceress", "archmage", "warlock") .. "'s " ..
                    RandomChoice("golem.", "familiar.", "homunculus.", "summoned monster.")
        elseif choice == 3 then
            message = message .. "the amazingly hung " .. RandomChoice("goblin.", "civic worker.", "geletanious cube.", "sentient shirt.")
        elseif choice == 4 then
            message = message .. "the pirate dressed as a ninja. (Cool things are cool.)"
        elseif choice == 5 then
            message = message .. "Hannibal Lecter."
        elseif choice == 6 then
            message = message .. "the stoned " .. RandomChoice("dark elf.", "gargoyle.", "earth elemental.", "astral deva.", "college kid.")
        elseif choice == 7 then
            message = message .. "your hyperactive " .. RandomChoice("cousin.", "grandmother.", "grandfather.", "brother.", "sister.")
        elseif choice == 8 then
            message = message .. "someone who looks exactly like you!"
        elseif choice == 9 then
            message = message .. "the horny " .. RandomChoice("genie.", "fishmonger.", "chauffeur.", "Autobot.", "thought.")
        elseif choice == 10 then
            message = message .. "the rampaging " .. RandomChoice("english professor.", "peace activist.", "color red.", "special forces agent.", "chef.")
        elseif choice == 11 then
            message = message .. RandomChoice("disloyal thugs", "girls", "dissatisfied customers", "workers", "malicious agents") .. " from a competing brothel."
        elseif choice == 12 then
            message = message .. "a cruel " .. RandomChoice("Cyberman.", "Dalek.", "Newtype.", "Gundam.")
        elseif choice == 13 then
            message = message .. "Sexbot Mk-" .. wm.Range(50, 250) .. "."
        elseif choice == 14 then
            message = message .. "underage kids " .. RandomChoice("(Who claim to be of age.)", "(Who snuck in.)", "(Who are somehow related to the Brothel Master, so its ok.)",
                    "(They paid, so who cares?)", "(We must corrupt them while they're still young.)")
        elseif choice == 15 then
            message = message .. "Grandpa Parkins from down the street."
        elseif choice == 16 then
            message = message .. "the ... thing living " .. RandomChoice("in the underwear drawer", "in the sex-toy box", "under the bed", "in her shadow", "in her psyche") .. "."
        elseif choice == 17 then
            message = message .. "the senior member of the cult of " .. RandomChoice("tentacles.", "unending pain.", "joy and happiness.", "Whore Master developers.", "eunuchs.")
        elseif choice == 18 then
            message = message .. "this weirdo who appeared out of this blue box called a " .. RandomChoice("TARDIS.", "TARDIS.", "TURDIS.")
            if wm.Percent(50) then
                message = message .. RandomChoice("His female companion was in on the action too.",
                        "His mechanical dog was involved as well.",
                        "His female companion and mechanical dog did lewd things to each other while they watched.")
            end
        end
    end
    return message
end

---@param customer wm.Customer
function HandleBDSMAggressiveCustomer(girl, customer)
    local message = "The customer suddenly turned sadist and started really hurting ${name}. ";
    local upset = 4
    local damage = 4
    local PCLove = -1
    local PCFear = 1

    -- bool guardingGang = (!g_Game->gang_manager().gangs_watching_girls().empty());
    -- bool guardingGirl = girl->m_Building->num_girls_on_job(JOB_SECURITY, Day0Night1) > 0;
    -- TODO re-enable this
    local guardingGang = false
    local guardingGirl = false
    if guardingGirl then
        -- sGirl * guard = random_girl_on_job(*girl->m_Building, JOB_SECURITY, Day0Night1);
        local guard = nil
        if wm.GetPlayerDisposition() > 30 then
            -- Player is Actively Good.
            message = message .. "Knowing you wouldn't want things to go too far, your guard, " .. guard:name() .. ", intervened, instructing the customer to take it easy on ${name} or get banned from your establishment.\n";
            PCFear = PCFear + 3;    -- you protect her
            PCLove = PCLove + 3;    -- ditto
            damage = damage - 2;    -- reduced damage
            upset = upset - 3;    -- and upset
            customer:happiness(-5)
        elseif guard:morality() > 50 then
            -- If guard is good
            message = message .. "Your guard, " .. guard:name() .. ", intervened, angrily demanding the customer 'back off' when things were clearly getting too extreme for ${name}.\n";
            PCFear = PCFear - 1;
            damage = damage - 2;
            upset = upset - 3;
            customer:happiness(-8)
        elseif wm.Percent(50) and (girl:has_trait("Psychic", "Strong Magic")) then
            -- If girl can compell
            message = message .. "Your guard, " .. guard:name() .. ", felt oddly compelled to help ${name}, demanding the customer take it easier on ${name}.\n";
            damage = damage - 2;
            upset = upset - 2;
            customer.happiness(-5)
        else
            message = message .. "Your guard, " .. guard:name() .. ", heard " .. girl:name() .. "'s screams, but didn't care to do anything. It's bought and paid for.\n";
            girl:sanity(-4)
        end
    elseif guardingGang then
        if wm.GetPlayerDisposition() > 30 then
            -- Player is Actively Good.
            message = message .. "Knowing you wouldn't want things to go too far, your defending gang intervened, ordering the customer to take it easy on ${name} or get banned from your establishment.\n";
            PCFear = PCFear - 3;     -- you protect her
            PCLove = PCLove + 3;     -- ditto
            damage = damage - 2;     -- reduced damage
            upset = upset - 3;         -- and upset
            customer.happiness(-5)
        elseif wm.Percent(50) and (girl:has_trait("Psychic", "Strong Magic")) then
            -- If girl can compel
            message = message .. "Your defending gang felt oddly compelled to help ${name}, demanding the customer take it easier on her.\n";
            damage = damage - 2;
            upset = upset - 2;
            customer:happiness(-5)
        else
            message = message .. "Your guards heard ${name}'s screams, but didn't care to do anything. It's bought and paid for.\n";
            girl:sanity(-4)
        end
    elseif girl:has_trait("Psychic") and girl:mana() > 60 then
        -- If girl can sway customer
        message = message .. "Suddenly, he calmed down and stopped. ${name}'s eyes glowed as the customer tenderly untied her bindings.\n";
        damage = damage - 2;
        upset = upset - 2;
        customer:happiness(5) -- magical bliss
        girl:mana(-50); --BIG mana hit
    else
        message = message .. "No one was around to stop him, so she ended up taking some damage.";
        -- girl->upd_stat(STAT_SANITY, -4);
    end
    girl:pcfear(PCFear)
    girl:pclove(PCLove)
    girl:health(-damage)
    girl:happiness(-upset)
    return message
end

function InsaneBDSM(girl)
    local message = RandomChoice("thwacked her nipples red with a stick", "deliberately came in her eye", "shoved his dick in her ass",
            "dripped hot candle wax on her asshole", "cruelly fisted her") .. " {name} " ..
            RandomChoice("started suddenly laughing", "softly began to sing", "stared up at him with an unnerving empty smile",
                    "grinned and silently wept", "started speaking in tongues") .. ", "
    if girl:morality() > 33 then
        message = message .. "urging him to cleanse her and saying something about fate, the Gods, and divine retribution."
    elseif girl:morality() > -33 then
        message = message .. "calling him 'bunny' and urging him to teach her the secret ways."
    else
        message = message .. "urging him to make her stronger, and muttering about the strong consuming the unworthy."
    end
    return message;
end

------------------------------------------------------------------------------------------------------------------------
--                                     LESBIAN
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetLesMessage(girl, customer)
    local message = ""
    local skill = girl:lesbian()

    if girl:has_trait("Zombie") then
        local choice = wm.Range(1, 100)
        if choice < 45 then
            message = "This thrill-seeking woman wanted a zombie-girl to eat her pussy. "
            if girl:has_trait("No Teeth") then
                message = message .. "Frighteningly, ${name} had exactly the same idea. Luckily she has no teeth, so her furious efforts to consume "
                        .. "the customer's pussy only succeeded in bringing the thrilled woman to a powerful orgasm."
                customer:happiness(60);
            elseif wm.Percent(girl:health()) then
                message = message .. "Luckily, ${name} has been fed recently and showed no interest."
            elseif wm.Percent(girl:intelligence()) then
                message = message .. "Luckily, ${name} has some of sense left and did not eat the woman."
            else
                message = message .. "Unfortunately, ${name} had exactly the same idea, "
                        .. "and bit into the customer's cunt.\nHer shocked screams quickly brought help.\n"
                customer:happiness(-50)
            end
        elseif choice < 85 then
            message = "This slim, unnaturally pale female customer stared silently at your zombie-girl. She stood in the corner waiting silently until ${name}'s handler left.\nThe guards heard the lock click shut"
            message = message .. RandomChoice(" and after that, not another sound", ". Then the screams started. They echoed through the halls for a solid hour",
                    ", and an Angelic singing begun, punctuated by strange zombie grunts", ". Moments later, with no warning, several of the guards passed out. Suddenly there was a wraithlike scream that came from everywhere. Then silence",
                    " and not the slightest sound was heard, except for one guard who swore he could hear weeping"
            )
            message = message .. ".\nAt the end of her time, the lock clicked open. Without a word, the woman walked out of the building.\n"
            if wm.Percent(40) then
                message = message .. "\nFearing she hadn't paid your guards gave chase" .. RandomChoice(
                        ", but she was already gone", " and caught her at the door. They surrounded her and demanded she turn around. She did. Your guards woke up minutes later with little idea what had happened",
                        ". They actually caught her, but for reasons none could explain, they let her go"
                ) .. ".\n"
            end
            message = message .. "\nRetrieving ${name}, the handler found her rocking and making a strange noise.\nNext to her, a bundle of gold.\n\n"
            customer:happiness(50)
        else
            message = "This slutty young woman was involved in some kind of bet that her mindblowing oral could even make an undead girl come.\n" ..
                    "She spread the zombie's legs, slipped her tongue into ${name}'s cold pussy and"
            if choice < 95 then
                message = message .. " almost immediately threw up at the taste. She won't try that again.\n"
                customer:happiness(-50)
            elseif choice < 87 then
                message = message .." retched repeatedly, complaining about the 'taste,' and gave it up for impossible after a few minutes\n"
                customer:happiness(-30)
            else
                message = message .. " warmed it up with her expert licks. Despite some early gagging, the customer ate her out for a long time. ${name} just sat motionless with a slightly confused expression.\n"
                customer:happiness(-10)
            end
        end
        return message
    end

    if skill < 20 then -- Gondra: the girl is unskilled
        if girl:has_trait("Lesbian") then
            message = "${name} was a bit too enthused about getting a female customer, fumbling quite a bit between the customers legs, including an unfortunate contact between her teeth and the clit she was sucking on."
        elseif girl:has_trait("Farmers Daughter") then
            message = "${name} looked a bit perplexed when she saw that her customer was a woman. The customer needed to push ${name}'s head between her legs to get her to work instead of looking around dumbfounded like a cow."
        else -- TODO Gondra: Replace/supplement these Vanilla messages.
            message = "${name} " .. RandomChoice(
                    "mechanically worked the customers pussy, barely managing to satisfy her.",
                    "managed to make the female customer cum, but seemed distressed about where she was touched by a fellow woman.",
                    "licked her female customer's cunt until she came. She didn't want any herself."
            )
        end
    elseif skill < 40 then
        if girl:has_trait("Dick-Sucking Lips") then
            message = "The customer enjoyed feeling ${name}'s lips run over her body, especially when they were sucking on her nipples or clit, eliciting an orgasm from the customer after a few directions."
        else
            message = "${name} " ..  RandomChoice(
                    "enjoyed herself a little bit as her hands and tongue made her customer cum.",
                    "didn't seem to mind her customers hands drifting over her body as she brought her to orgasm.",
                    "was a bit uncomfortable with herself being visibly aroused after servicing her customer.",
                    "certainly was surprised that her customer insisted on being the one licking and rubbing, but enjoyed the attention and gave back as well as she could.",
                    "was aroused as she made her female customer cum."
            )
        end
    elseif skill < 60 then
        if girl:has_trait("Straight") then
            message = "Although it doesn't do anything for her, ${name} made the woman buying her service happy without a problem, "
            if girl:has_trait("Fake Orgasm Expert") then
                message = message .. "believably faking an orgasm as her customer returned the favor."
                customer:happiness(5)
            else
                message = message .. "declining the customer's offer to return the favor without being rude."
            end
        elseif girl:has_trait("Good Kisser") then
            message = "${name} managed to elicit the first few moans from her customer just placing a handful kisses on her neck, gradually traveling down her customers body, teasing her for quite a while before finally making her cum."
        else
            message = RandomChoice(
                    "${name} and her customer both came as they rubbed their bodies against each other.",
                    "Both of their faces had a satisfied look to them when ${name} and her customer were done.",
                    "${name} fucked and was fucked by her female customer."
            )
        end
    elseif skill < 80 then
        if girl:has_trait("Lesbian") then
            message = "${name}'s customer moaned loudly the first time before she even dropped a single piece of clothing, the first orgasm audible soon after, quickly followed by several more, before${name} is heard cumming for the first time. In the end the customer walks away with quivering knees, exhausted but practically glowing with happiness."
        else
            message = "${name} " .. RandomChoice(
                    "seemingly had a blast with her customer as both their moans were quite loud.",
                    "managed to make her partner shriek loudly several times as she tickled multiple orgasms out of her customer.",
                    "and her female customer's cumming could be heard throughout the building."
            )
        end
    else
        local choice = wm.Range(1, 6)
        message = " "
        if choice == 1 then
            message = message .. RandomChoice("aggressively", "tenderly", "slowly", "authoratively", "violently") .. " " ..
                    RandomChoice("straddled", "scissored", "symmetrically docked with", "cowgirled", "69ed") .. " the woman"
        elseif choice == 2 then
            message = RandomChoice("shaved her", "was shaved") .. " with a +" .. wm.Range(4, 10) .. " " ..
                    RandomChoice("vorporal broadsword", "dagger, +" .. wm.Range(8, 15) .. " vs pubic hair",
                            "flaming sickle", "lightning burst bo-staff", "human bane greatsword", "acid burst warhammer",
                            "feral halfling"
                    )
        elseif choice == 3 then
            message = message .. "had a " .. RandomChoice("pleasant", "long", "heartfelt", "deeply personal", "emotional", "angry", "violent") ..
                    " conversation with her lady-client about " .. RandomChoice("sadism", "particle physics", "domination", "submission", "brewing poisons",
                    "flower arranging", "the Brothel Master", "assassination techniques")
        elseif choice == 4 then
            message = message .. "massaged the woman with " .. RandomChoice("bath oil", "aloe vera", "the tears of Chuck Norris's many victims",
                    "the blood of innocent angels", "Unicorn blood", "Unicorn's tears", "a strong aphrodisiac", "oil of greater breast growth",
                    "potent oil of massive breast growth", "oil of camel-toe growth"
            )
        elseif choice == 5 then
            message = message .. "put a " .. RandomChoice("ball gag and blindfolded", "sensory deprivation hood") .. " on, was " ..
                    RandomChoice("lashed", "cuffed", "tied", "chained") .. " to a " .. RandomChoice("bed", "bench", "table", "post") ..
                    " and " .. RandomChoice("symmetrically docked", "69ed", "straddled", "scissored")
        elseif choice == 6 then
            message = message .. "looked at the woman across from her. " .. RandomChoice("Position", "Toy", "Oil", "Bed sheet color", "Price") ..
                    " was to be " .. RandomChoice("a trial", "decided", "resolved", "dictated") .. " by combat. Both had changed into " ..
                    RandomChoice("string bikinis", "lingerie", "body stockings", "their old school uniforms", "dominatrix outfits") ..
                    " and wielded " .. RandomChoice("whips", "staves", "boxing gloves", "cat-o-nine tails") .. " of " ..
                    RandomChoice("explosive orgasms", "clothes shredding", "humiliation", "subjugation", "brutal stunning") .. ". "
            if wm.Percent(50) then
                message = message .. "They stared at each other across the " .. RandomChoice("mud", "jello", "whip cream", "clothes-eating slime") .. " pit."
            end
            message = message .. " A bell sounded! They charged and " .. RandomChoice("dueled", "fought it out", "battled") .. "!\n" ..
                    RandomChoice("The customer won", "The customer was vanquished")

        end
    end
    message = message .. ". "
    choice = wm.Range(1, 8)
    if choice == 1 then
        message = message .. "She was " .. RandomChoice("tormented", "teased", "massaged", "frustrated", "satisfied") .. " with " ..
                RandomChoice("great care", "deva feathers", "drug-soaked sex toys", "wild abandon") .. " by"
    elseif choice == 2 then
        message = message .. "She used " .. RandomChoice("phoenix down", "deva feathers", "drug-soaked sex toys", "restraints", "her wiles") .. " to " ..
                RandomChoice("torment", "tease", "massage", "frustrate", "satisfy")
    elseif choice == 3 then
        message = message .. "She " .. RandomChoice("fingered", "teased", "caressed", "fondled", "pinched") .. " the client's " ..
                RandomChoice("clit", "clitoris", "love bud") .. " and expertly elicited orgasm after orgasm from"
    elseif choice == 4 then
        message = message .. "Her " .. RandomChoice("clit", "clitoris", "love bud") .. " was " .. RandomChoice("fingered", "teased", "caressed", "fondled", "pinched") ..
                " and she orgasmed repeatedly under the expert touch of"
    elseif choice == 5 then
        message = message .. "She " .. RandomChoice("clam wrestled", "rubbed", "attacked", "hammered") .. " the client's " ..
                RandomChoice("clit", "clitoris", "love bud", "vagina") .. " causing waves of orgasms to thunder through"
    elseif choice == 6 then
        message = message .. "She " .. RandomChoice("single mindedly", "repeatedly", "roughly") .. " " ..
                RandomChoice("rubbed", "fondled", "prodded", "attacked", "tongued", "licked") .. " the client's g-spot. Wave after wave of " ..
                RandomChoice("orgasms", "pleasure", "powerful sensations", "indescribable joy") .. " " .. RandomChoice("rushed", "thundered", "cracked", "pounded") ..
                " through"
    elseif choice == 7 then
        message = message .. "Wave after wave of " .. RandomChoice("orgasms", "back-stretching joy", "madness", "incredible feeling") .. " " ..
                RandomChoice("throbbed", "shook", "arced", "stabbed") .. " through her as she was " ..
                RandomChoice("single mindedly", "repeatedly", "roughly") .. " " .. RandomChoice("rubbed", "fondled", "prodded", "attacked", "tongued", "licked") .. " by"
    elseif choice == 8 then
        message = message .. "Work stopped " .. RandomChoice("in the brothel", "on the street", "all over the block", "in the town",
                "within the country", "over the whole planet", "within the solar system", "all over the galactic sector") .. ". Everything was drowned out by:\n\n" ..
                "Ahhhhh!\n\n"
        if wm.Percent(50) then
            message = message .. RandomChoice("For the love... of aaaaahhhhh mercy.  No nnnnnnnnh more!",
                    "oooooOOOOOO YES! ahhhhhhHHHH!", "nnnnnhhh nnnnnhhhh NNNHHHHHH!!!!") .. "\n\n"
        end
        message = message .. "Annnnnhhhhhaaa!\n\n\"AHHHHHHHH! I'm going to " .. RandomChoice("CCCUUUUUUMMMMMMMM!!!!!",
                "EEEXXXXXPLLLOOODDDDEEEE!!!", "DIEEEEEE!", "AHHHHHHHHHHH!!!!", "BRRRREEEEAAAAKKKKKK!!!!", "WAAAAHHHHHOOOOOOO!!!"
        ) .. "\nfrom "
    end

    message = message .. " "
    choice = wm.Range(1, 6)
    if choice == 1 then
        message = message .. "the " .. RandomChoice("prostitute", "street worker", "hooker", "street walker", "working girl") ..
                " from a " .. RandomChoice("friendly", "rival", "hostile", "feeder") .. " brothel."
    elseif choice == 2 then
        -- auto brothel = g_Game->buildings().building_with_type(BuildingType::BROTHEL, g_Dice % g_Game->buildings().num_buildings(BuildingType::BROTHEL));
        --        auto girl = brothel->find_random_girl();
        --        // TODO(buildings) manager level random girl
        --        /* */if (girl == nullptr) OStr << "a girl";
        --        else /*            */    OStr << girl->FullName();
        --        OStr << " from ";
        --        OStr << brothel->name();
        --        OStr << " brothel.";
    elseif choice == 3 then
        message = message .. "the " .. RandomChoice("sexy", "rock hard", "hot", "androgynous", "spirited", "exuberant") .. " MILF."
    elseif choice == 4 then
        message = message .. "the " .. RandomChoice("senior", "junior") .. " " .. RandomChoice("Sorceress", "Warrioress", "Priestess", "Huntress", "Amazon", "Druidess") ..
                " of the " .. RandomChoice("Hidden", "Silent", "Masters", "Scarlet", "Resolute") .. " " .. RandomChoice("Hand", "Dagger", "Will", "League", "Hearts") .. "."
    elseif choice == 5 then
        message = message .. "the " .. RandomChoice("high-ranking", "mid-tier", "low-ranking") .. " " ..
                RandomChoice("elf", "woman", "dryad", "succubus", "nymph", "eyrine", "cat girl") .. " from the " ..
                RandomChoice("Nymphomania", "Satyriasis", "'Women Who Love Sex'", "'Real Women Don't Marry'", "'Monster Sex is Best'") .. " " ..
                RandomChoice("support group", "league", "club", "faction", "guild") .. "."
    elseif choice == 6 then
        message = message .. "the "..RandomChoice("disguised", "hot", "sexy", "curvaceous", "sultry") .. " " ..
                RandomChoice("idol singer", "princess", "school girl", "nurse", "maid", "waitress") .. "."
    end
    return message
end

------------------------------------------------------------------------------------------------------------------------
--                                     ANAL
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetAnalMessage(girl, customer)
    local message = ""
    local skill = girl:anal()

    if girl:has_trait("Zombie") then
        return "${name} moaned lightly as her customer pounded her dead ass."
    end

    -- TODO Gondra: replace this with a descripton string in front of ALL Sexmessages?
    -- if (girl->has_trait("Great Arse") || girl->has_trait("Deluxe Derriere")) sexMessage << "'s behind is a thing of beauty. She ";
    -- else if (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush")) sexMessage << "'s big round booty was up in the air. She "; //Gondra: Wide Bottom is mising here?
    -- else if (girl->has_trait("Tight Butt")) sexMessage << " has a tight, round firm little butt. She ";
    -- else if (girl->has_trait("Flat Ass")) sexMessage << "'s ass is flat as a board. She ";

    if skill < 20 then
        if wm.Percent(33) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = "${name} was clearly uncomfortable as the customer pushed his cock into her jiggling booty.";
        elseif wm.Percent(60) and girl:has_trait("Prehensile Tail", "Playful Tail") then
            message = "Using her tail as a handhold the customer made the fuck a lot more traumatic for ${name} than it normally would have been.";
        elseif wm.Percent(33) and girl:has_trait("Tight Butt") then
            message = "It took the customer quite a bit of effort to force himself into " .. anme .. "'s tight ass, ignoring her cries when he was finally inside her, moving harshly until he finished.";
            -- Gondra: add happiness and health reduction?
        elseif wm.Percent(33) and girl:beauty() < 45 then
            message = "As he fucked her ass, the customer did it from behind, shoving ${name}'s face into the pillow so he could imagine it was someone prettier. Unfortunately she was awful at this.";
        else
            message = RandomChoice("When she saw the customer's cock, ${name} begged him to use her pussy instead. " ..
                    "But it was his dollar and he insisted on trying to squeeze all of that into her asshole, despite her yelps and squeals.",
                    "${name} found it difficult to get it in but painfully allowed the customer to fuck her in her tight ass.",
                    "${name} didn't realize he was aiming for *that* hole until it was too late. She couldn't really enjoy it.",
                    "${name} bit the pillow to muffle her cries as the customer managed to squeeze his cock into her ass."
            )
        end
    elseif skill < 40 then
        if wm.Percent(75) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = "${name} felt a bit uncomfortable as the customer's erect cock slipped between her ass-cheeks, but the customer hardly noticed as her plentiful flesh wrapped around him."
        elseif wm.Percent(75) and girl:check_virginity() then
            message = "${name}'s virginity was spared, as he used her ass-hole. Considering she's a 'virgin' she seems to have done THIS before."
        elseif wm.Percent(33) and girl:beauty() < 45 then
            message = "He fucked ${name}'s ass from behind, so that he wouldn't have to look at her ugly face. " ..
                    "She was okay at this.";
        else
            message = RandomChoice("When ${name} saw the customer's cock, her eyes widened. Getting all of that in her ass would take some work.",
                    "${name} had to relax somewhat but had the customer fucking her in her ass.",
                    "${name} enjoyed it, even though she's not that good at this yet. He did most of the work.",
                    "${name} struggled to relax, but was okay with the customer gently screwing her ass."
            )
        end
    elseif skill < 60 then  -- Gondra: the girl is reasonably skilled
        if wm.Percent(75) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = "${name}'s voluminous ass jiggles quite a bit as the customer goes at it."
        elseif wm.Percent(60) and girl:has_trait("Tight Butt") then
            message = "${name}'s very tight butt forced him to take it slow but the vice like grip seemed to do the trick either way as he came quickly."
        elseif wm.Percent(20) and girl:has_trait("Queen") then
            message = "As one of the rebels that once tried to overthrow ${name}, the customer relished the chance to fuck her in the ass for some paltry gold coins. " ..
                    "He left boasting to everyone about how good it had been to buttfuck a notorious Queen! (Leading to lots of sniggering and rumours that would follow him for years)\n"
            customer:happiness(10)
            girl:fame(5)
        elseif wm.Percent(33) and girl:beauty() < 45 then
            message = "He fucked ${name}'s ass from behind, so that he wouldn't have to look at her face. " ..
                    "She was pretty good at this."
        else
            message = RandomChoice("The customer lay on the bed groaning as ${name} bounced up and down, working his cock with her ass.",
                    "${name} found it easier going with the customer fucking her in her ass.",
                    "${name} was comfortable with the customer fucking her in her ass."
            )
        end
    elseif skill < 80 then --Gondra: the girl is VERY skilled
        if wm.Percent(60) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = RandomChoice(
                    "${name} enjoyed showing of that she can hide the customers whole cock between her cheeks, before she lets him slip into her ass proper.",
                    "Encouraged by ${name} the customer plowed her ass hard, both enjoying the sound her jiggling backside made each time he drove his cock home.",
                    "The customer slammed his cock again and again into ${name}'s asshole, loving the slap and the jiggle of flesh each time he drove it home."
            )
        elseif wm.Percent(60) and girl:has_trait("Tight Butt") then
            message = "Just as the customer wondered if he would fit into the tight ass in front of him, ${name}spread her ass for him, inviting him to push it deep into her."
        elseif wm.Percent(20) and girl:has_trait("Queen") then
            message = "As one of the rebels that once tried to overthrow ${name}, the customer relished the chance to fuck her in the ass for some gold coins. " ..
                    "He left telling to everyone about how amazing it felt to buttfuck a Queen! (Leading to lots of sniggering, and rumours that would follow him for years)\n";
            customer:happiness(10)
            girl:fame(5)
        elseif wm.Percent(33) and girl:beauty() < 45 then
            message = "At first the customer seemed uphappy with ${name}'s looks, but as soon as she had her sphincter locked around his cock " ..
                    "and started grinding her ass onto him, any complaints quickly vanished.";
        else
            message = RandomChoice("The customer lay on the bed gasping with pleasure as ${name} bounced up and down, riding his pole for all she was worth.",
                    "The customer slid it right into her ass and ${name} loved every minute of it.",
                    "${name} sat on the bed and placed her legs on his shoulders, as the customer plunged it deep in her ass.",
                    "${name} had the customer's cock go in easy. She found having him fuck her ass a very pleasurable experience."
            )
        end
    else  -- Gondra: the girl is EXTREMELY skilled
        if wm.Percent(60) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse") then
            message = "The customer played around with the big round ass ${name} held up for him, which already made her moan loudly. And then made her cum for the first of many times, just by pushing his throbbing length into her willing anus."
        elseif wm.Percent(50) and girl:has_trait("Tight Butt") then
            message = "The customer looked surprised when ${name} slipped her ass onto his cock, the tight embrace of her backside milking him several times with exquisite motions. Not a single drop of cum leaked from her even after they had finished and she accompanied him out."
        elseif wm.Percent(35) and girl:has_trait("Flexible", "Agile") then
            message = "${name} did the splits on the edge of the bed, her spread pussy and ass facing the customer. After a moment's indecision, he fucked her deep in the ass until they both came."
        elseif wm.Percent(40) and girl:has_trait("Cum Addict") then
            message = "${name} skillfully milked the customer's cock with her ass" .. RandomChoice(
                    ", quickly switching to her mouth when she sensed him ready to give up his precious cum.",
                    " until, without warning, he wasted his precious cum shooting it all up her ass. She managed to claw some back out, " ..
                            "licking it greedily off her fingers, but most was lost.\nThe customer was awed, thinking this was all part of the show.",
                    " until, without warning, he threw away his precious load in her ass.\nAnnoyed, she climbed off him and squatted on the floor, squeezing, " ..
                            "until - with a little toot - it squirted and oozed back out. Relieved, she nodded politely to the customer, and started lapping it off the floor."
            )
        else
            message = RandomChoice("The customer writhed on the bed breathless with pleasure as ${name} rode his pole, her anus milking orgasm after crippling orgasm out of him.",
                    "The customer started slow but quickly began to pound hard into ${name}'s ass making her moan like crazy.",
                    "${name} sat on the bed and put her legs around his neck, as the customer plunged his cock up her ass.",
                    "${name} came hard as the customer fucked her ass."
            )
        end
    end

    return message
end

------------------------------------------------------------------------------------------------------------------------
--                                     NORMALSEX
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetNormalMessage(girl, customer)
    local message = ""
    local skill = girl:anal()

    if girl:has_trait("Zombie") then
        return "${name} moaned lightly as her customer pounded her dead pussy."
    end

    if skill < 20 then  --  if the girl is unskilled show one of these messages
        if wm.Percent(35) and girl:has_trait("Aggressive") then
            message = "${name} stared angrily at the customer as she tore the clothes off of her body. It made the customer feel uncomfortable."
            customer:happiness(-5)
        elseif wm.Percent(35) and girl:has_trait("Nervous") then
            message = "${name} was clearly uncomfortable with the arrangement, and it made the customer feel uncomfortable."
            customer:happiness(-5)
        elseif wm.Percent(10) and girl:has_trait("Fake Orgasm Expert") then
            message = "${name}'s robotic moans along with her tearful eyes ruined the customer's boner. He didn't even manage to finish before angrily stomping out of the room."
            customer:happiness(-15)
        elseif wm.Percent(55) and girl:has_trait("Priestess") then
            message = "${name}'s mini-sermon about sin and judgement made the sex awkward. The customer was clearly uncomfortable."
            customer:happiness(-5)
        elseif wm.Percent(45) and girl:is_pregnant() then
            message = "${name} tried to have sex, but it was awkward because of her pregnancy."
            customer:happiness(-5)
        elseif wm.Percent(66) and girl:has_trait("Agile", "Flexible") then
            message = "Even though ${name} doesn't have much skill in bed, her suppleness meant that the customer could twist her into some great positions."
            customer:happiness(15)
        elseif wm.Percent(25) and girl:has_trait("Assassin") then
            message = "The look ${name} gave when the customer asked for a pre-sex blowie was terrifying. In fear of his life he got the deed done and escaped as fast as he could."
            customer:happiness(-5)
        elseif wm.Percent(25) and girl:has_trait("Alcoholic") then
            message =  "Somehow ${name} had gotten hold of alcohol. She lay incoherent on the bed while the customer 'came' and went."
        elseif wm.Percent(35) and girl:has_trait("Phat Booty", "Deluxe Derriere", "Wide Bottom", "Plump Tush", "Great Arse", "Tight Butt") then
            message = "${name} is embarrassingly awkward at sex, but also has a great ass. When the customer flipped her over, gripped onto her ass and did her from behind, he had a better time."
            customer:happiness(10)
        elseif wm.Percent(35) and wm.GetPlayerDisposition() > 40 then  -- bonus if you are nice
            message = "${name} is not great at sex. However, inspired by your goodness and not wanting to let you down, she tried her best to give the customer a decent fuck.";
            customer:happiness(20)
            girl:happiness(2)
            girl:obedience(2)
            -- TODO girl->upd_Enjoyment(ACTION_SEX, 2);
        elseif wm.Percent(35) and girl:refinement() > 66 then
            message = "${name} didn't do much as the customer fucked her pussy, but was clearly horrified when he dumped a load of cum inside."
        elseif wm.Percent(60) then
            message = RandomChoice("${name} didn't do much as the customer fucked her pussy.",
                    "${name}'s forced smile and awkward demeanor made the whole ordeal more awkward than necessary, but the deed got done.",
                    "${name} just laid back and let the customer fuck her."
            )
        else
            message = RandomChoice("\"You get what you pay for,\" the customer grumbled as he threw a few wads of money on the jizz covered floor.",
                    "The customer's inexperience combined with ${name}'s inexperience lead to lots of painful grabbing of breasts, aggressive thrusts, and a quick finish. Everyone was clearly unhappy."
            )
            customer:happiness(-5)
        end
    elseif skill < 40 then
        if wm.Percent(50) and girl:has_trait("Plump") then
            message = "The constant prodding and groping of her embarrassingly plump body made it hard for ${name} to concentrate on being a good fuck."
        elseif wm.Percent(33) and girl:has_trait("Fast Orgasms") then
            message = "${name}'s moans grew louder and louder as the customer kept going at it with her, and even though he came before she had a chance to, it was still an enjoyable fuck for both of them."
            customer:happiness(5)
        elseif wm.Percent(25) and girl:has_trait("Fake Orgasm Expert") then
            message = "${name}'s sudden faked orgasm just as her customer came didn't really do it's job, but as he had already finished the customer didn't bother reprimanding her."
        elseif wm.Percent(66) and girl:has_trait("Agile", "Flexible") then
            message = "${name} was okay in bed, and the positions she could twist herself into impressed the customer."
            customer:happiness(15)
        elseif wm.Percent(66) and girl:has_trait("Clumsy", "Dojikko") then
            message = "${name} was doing okay at sex until she accidentally sat on the customer balls, causing him quite a lot of pain."
            customer:happiness(-20)
        elseif wm.Percent(25) and girl:has_trait("Assassin") then
            message = "The customer was excited to have sex with a dangerous femme-fatale like ${name}."
            customer:happiness(5)
        elseif wm.Percent(25) and girl:has_trait("Alcoholic") then
            message = "Somehow ${name} had gotten hold of alcohol. She was too drunk to to anything for the customer and just lay back rocking and grunting while he fucked her.";
            customer:happiness(-15)
        elseif wm.Percent(35) and girl:has_trait("Phat Booty", "Plump Tush", "Wide Bottom", "Great Arse", "Tight Butt", "Deluxe Derriere") then
            message = "${name} is okay at sex, but the customer figured out: to have a really great time you have to grip dat ass and do her from behind."
            customer:happiness(20)
        elseif wm.Percent(35) and girl:pclove() - girl:pchate() > 0 and wm.GetPlayerDisposition() > 40 then  -- bonus if you are nice
            message = "${name} is inspired by your kindness and wants you to succeed. She tried her best to give the customer great sex."
            customer:happiness(20);
            girl:happiness(2)
            girl:obedience(2)
            -- girl->upd_Enjoyment(ACTION_SEX, 2);
        elseif wm.Percent(35) and girl:pclove() - girl:pchate() > 60 then  -- if she likes you
            message = "When ${name} couldn't get in the mood for the customer, she closed her eyes and imagined it was you. She fucked him with some real passion."
            customer:happiness(15)
        elseif wm.Percent(35) and (girl:refinement() < 35 or girl:has_trait("Cum Addict")) then
            message = "${name}'s sex was... efficient. However, after the customer came in her, she quickly sucked his cock clean and "
                    .. "started scooping his cum out of her pussy and lapping it off her fingers, while fingering herself to orgasm right in front of him. It was a hell of an after-show."
            customer:happiness(10)
        else
            message = RandomChoice("${name} let him fuck her, while she did her best to look like she loved it.",
                    "${name} fucked the customer back while their cock was embedded in her cunt.",
                    "${name} made the right noises and held the customer as he fucked her.",
                    "${name} let the customer push her down and paw at her breasts, allowing a few fake moans to escape."
            )
        end
    elseif skill < 60 then
        if wm.Percent(35) and girl:has_trait("Slut") then
            message = "${name} was on the customers cock quickly and surprised him with a few tricks while they fucked."
        elseif wm.Percent(50) and girl:has_trait("Fast Orgasms", "Fake Orgasm Expert") then
            message = "${name}'s increasingly audible pleasure spurns the customer to fuck her hard, pushing her over the edge before he cums himself."
            customer:happiness(5)
        elseif wm.Percent(35) and girl:is_pregnant() then
            message =  "Horny from her pregnancy, ${name} wanted cum inside her, and gave the customer a great time."
            customer:happiness(5)
        elseif wm.Percent(66) and girl:has_trait("Agile", "Flexible") then
            message = "${name} was great at sex, athletically twisting herself into positions the customer had never imagined."
            customer:happiness(20)
        elseif wm.Percent(66) and girl:has_trait("Clumsy", "Dojikko") then
            message =  "Despite a few clumsy bumps, ${name} gave the customer some damn good sex."
        elseif wm.Percent(25) and girl:has_trait("Assassin") then
            message = "The customer was excited to have sex with a dangerous femme-fatale. And ${name} was damn good in the sack.";
            customer:happiness(10)
        elseif wm.Percent(25) and girl:has_trait("Alcoholic") then
            message = "Somehow ${name} had gotten hold of alcohol. She was completely wild, fucking the customer like crazy even after he came."
            customer:happiness(5)
        elseif wm.Percent(35) and girl:pclove() - girl:pchate() > 0 and wm.GetPlayerDisposition() > 40 then  -- bonus if you are nice
            message = "${name} knows you are the best "
            if girl:is_slave() then
                message = message .. "master"
            else
                message = message .. "brothel owner"
            end
            message = message .. " in town, and pulls out all the stops to give the customer amazing sex."
            customer:happiness(20)
            girl:happiness(2)
            girl:obedience(2)
            -- girl->upd_Enjoyment(ACTION_SEX, 2);
        elseif wm.Percent(35) and girl:pclove() - girl:pchate() > 60 then  -- if she likes you
            message = "${name} closed her eyes and imagined it was you. She fucked him dry."
            customer:happiness(15)
        elseif wm.Percent(35) and (girl:refinement() < 35 or girl:has_trait("Cum Addict")) then
            message = "${name}'s sex was great. And afterward she made a show of scooping his cum out of her pussy "
                    .. "and licking it off her hands. Finally she fingered herself to orgasm right in front of him."
            customer:happiness(10)
        else
            message = RandomChoice("${name} let him fuck her, while she did her best to look like she loved it.",
                    "${name} pushed back against the customers pistoning hips, inspiring him to work a bit harder himself.",
                    "${name} allowed the customer's hands to roam along her soft curves as she complimented his \"great\" skill.",
                    "${name} managed to keep the customer going until he finished, but forgot to fake her own orgasm. Despite that, the customer left pleased with the experience."
            )
        end
    elseif skill < 80 then  -- Gondra: the girl is very skilled
        if wm.Percent(35) and girl:has_trait("Fast Orgasms", "Fake Orgasm Expert") then
            message = "${name} went at it hard with the customer, cumming shortly after he penetrated her, and then several times until she finished her performance with an especially loud orgasm as the customer came."
        elseif wm.Percent(35) and girl:has_trait("Slow Orgasms") then
            message = "Although she is known to be hard to please, ${name} managed to cum through a combination of her considerable skill and an particularly observant customer that left with a smile on his face."
            customer:happiness(5)
        elseif wm.Percent(50) and girl:has_trait("Plump", "Fat") then
            message = RandomChoice("The customer is stunned that a heavy-set girl like ${name} can be so good in the sack.",
                    "The customer initially grumbled about getting \"some fat whore\", but ${name} really showed him how a big-girl can fuck."
            )
            customer:happiness(5)
        elseif wm.Percent(35) and girl:beauty() < 45 then
            message = "The customer initially grumbled about getting \"some ugly skank\", but ${name} really showed what she could do."
            customer:happiness(5)
        elseif wm.Percent(35) and girl:is_pregnant() then
            message = "Despite her pregnancy - or perhaps because of it - ${name} really fucked the customer, orgasming twice before bringing him to a powerful climax.";
            customer:happiness(5)
        elseif wm.Percent(55) and girl:has_trait("Priestess") then
            message = "Sexy Priestess, ${name}, lay the customer on the floor and knelt over him. She energetically praised the divine as she rode his cock, "
                    .. "until she was filled with his divine blessing.\nThen she used her mouth and made him rise again."
            customer:happiness(5)
        elseif wm.Percent(40) and (girl:has_trait("Agile", "Flexible") or girl:agility() > 75) then
            message = RandomChoice("The customer made full use of ${name}'s agility, fucking her in a wide range of positions.",
                    "The customer fucked ${name} as she went through her stretch routine. It was hot.",
                    "The customer was nervous and had some performance problems... Until a naked ${name} lay on the bed, "
                            .. "twisted both feet behind her head and smiled up at him."
            )
            customer:happiness(5)
        elseif wm.Percent(35) and (girl:has_trait("Cum Addict") or girl:refinement() < 35) then
            message =  "${name}'s sex was amazing, and her well trained pussy drained him dry. Afterward she made a show of "
                    .. "crouching on the floor and watching his cum dribble out. Finally she made him fuck her again while she licked his cum off the floor."
            customer:happiness(10)
        elseif wm.Percent(25) and girl:has_trait("Assassin") then
            message = "The customer was excited to have sex with a notorious femme-fatale like ${name}. She gave him an incredible time.";
            customer:happiness(10)
        elseif wm.Percent(25) and girl:has_trait("Alcoholic") then
            message = "Somehow ${name} was drunk. She was completely wild and uninhibited fucking and sucking the customer like crazy even after he came twice."
            customer:happiness(15)
        elseif wm.Percent(35) and girl:pclove() - girl:pchate() > 0 and wm.GetPlayerDisposition() > 40 then  -- bonus if you are nice
            message = "${name}knows you are the kindest "
            if girl:is_slave() then
                message = message .. "master"
            else
                message = message .. "brothel owner"
            end
            message = message .. " in town, and feels like she owes you. She uses all her skills to make sure the customer has an incredible time."
            customer:happiness(20)
            girl:happiness(2)
            girl:obedience(2)
            -- girl->upd_Enjoyment(ACTION_SEX, 2)
        else
            message = RandomChoice("${name} passionately rode the customer's cock until it erupted inside her.",
                    "${name} loved having a cock buried in her cunt and fucked back as much as she got.",
                    "${name} stunned the customer with her range of positions and techniques, making him cum multiple times.",
                    "${name} fucked like a wild animal, cumming several times and ending with her and the customer covered in sweat."
            )
        end
    else  -- Gondra: the girl is EXTREMELY skilled
        message = "${name} "
        -- MYR: Can't resist a little cheeky chaos
        if wm.Range(0, 500) == 1 then
            return "(phrase 1). (phrase 2) (phrase 3)."
        end

        local choice = wm.Range(1, 8)
        if choice == 1 then
            message = message .. RandomChoice("straddled", "cow-girled", "wrapped her legs around", "contorted her legs behind her head for", "scissored") ..
                    " the client, because it " .. RandomChoice("turned him on", "made him crazy", "gave him a massive boner", "was more fun than talking",
                    "made him turn red")
        elseif choice == 2 then
            message = message .. "had fun with his " .. RandomChoice("foot", "stocking", "hair", "lace", "butt", "food") ..
                    " fetish and gave him an extended " .. RandomChoice("foot", "hand", "oral") .. " surprise"
        elseif choice == 3 then
            message = message .. "was told to grab " .. RandomChoice("her ankles", "the chair", "her knees", "the table", "the railing") ..
                    " and " .. RandomChoice("shook her hips", "spread her legs", "close her eyes", "look away", "bend waaaaayyy over")
        elseif choice == 4 then
            message = message .. "dressed as " .. RandomChoice("a school girl", "a nurse", "a nun", "an adventurer", "a dominatrix") ..
                    " to grease " .. RandomChoice("the little man", "his pole", "his tool", "his fingers", "his toes")
        elseif choice == 5 then
            message = message .. "decided to skip " .. RandomChoice("the bed", "foreplay", "niceties", "greetings") .. " and assumed position " .. wm.Range(1, 10000)
        elseif choice == 6 then
            message = message .. "gazed in awe at " .. RandomChoice("how well hung he was", "the time", "his muscles", "his handsome face", "his collection of sexual magic items") ..
            " and " .. RandomChoice("felt inspired", "played hard to get", "squealed like a little girl", "prepared for action")
        elseif choice == 7 then
            message = message .. "bent into " .. RandomChoice("a delightful", "an awkward", "a difficult", "a crazy") .. " position and " ..
                    RandomChoice("squealed", "moaned", "grew hot") .. " as he " .. RandomChoice("touched", "caressed", "probed") .. " her defenseless body"
        elseif choice == 8 then
            message = message .. "lay on the " .. RandomChoice("floor", "bed", "couch") .. " and " ..
                    RandomChoice("had him take off all her clothes", "told him exactly what turned her on", "encouraged him to take off her bra and panties with his teeth")
        end

        message = message .. ". "

        choice = wm.Range(1, 11)
        if choice == 1 then
            message = message .. "She " .. RandomChoice("rode him all the way to the next town", "massaged his balls and sucked him dry",
                    "titty fucked and sucked the well dry", "fucked him blind") .. ". He was a trooper though and rallied: She " ..
                    RandomChoice("was deeply penetrated", "was paralyzed with stunning sensations", "bucked like a bronko", "shook with pleasure") .. " and " ..
                    RandomChoice("came like a fire hose from", "repeatedly shook in orgasm with")
        elseif choice == 2 then
            message = message .. "It took a lot of effort to stay " .. RandomChoice("interested in", "awake for", "conscious for")
        elseif choice == 3 then
            message = message .. "She was fucked " .. RandomChoice("blind", "silly twice over", "all crazy like", "for hours", "for minutes") .. " by"
        elseif choice == 4 then
            message = message .. "She performed " .. RandomChoice("uninspired ", "inspired ") .. RandomChoice("missionary ", "oral ", "foot ", "hand ") .. "sex for"
        elseif choice == 5 then
            message = message .. RandomChoice("Semen", "Praise", "Flesh", "Drool", "Chocolate sauce") .. " rained down on her from"
        elseif choice == 6 then
            message = message .. "She couldn't " .. RandomChoice("stand", "walk", "swallow", "feel her legs", "move") .. " after screwing"
        elseif choice == 7 then
            message = message .. "It took a great deal of effort to look " .. RandomChoice("interested in", "awake for", "alive for", "enthusiastic for", "hurt for")
        elseif choice == 8 then
            message = message .. "She played 'clean up the " .. RandomChoice("tools", "customer", "sword", "sugar frosting") .. "' with"
        elseif choice == 9 then
            message = message .. "Hopefully her " .. RandomChoice("cervix", "pride", "reputation", "ego", "stomach") .. " wasn't bruised by"
        elseif choice == 10 then
            message = message .. "She called in " .. wm.Range(2, 5) .. " reinforcements to tame"
        elseif choice == 11 then
            message = message .. "She orgasmed " .. wm.Range(30, 130) .. " times with"
        end

        message = message .. " "
        choice = wm.Range(1, 20)

        if choice == 1 then
            message = message .. "the guy " .. RandomChoice("wearing three amulets of the sex elemental.", "wearing eight rings of the horndog.",
                    "wearing a band of invulnerability.", "carrying a waffle iron.", "carrying a body probe of irresistible sensations."
            )
        elseif choice == 2 then
            message = message .. "Thor, God of Thunderfucking!!!!"
        elseif choice == 3 then
            message = message .. "the frustrated " .. RandomChoice("astronomer.", "physicist.", "chemist.", "biologist.", "engineer.")
        elseif choice == 4 then
            message = message .. "the invisible something or other????"
        elseif choice == 5 then
            message = message .. "the butler. (He always did it.)"
        elseif choice == 6 then
            message = message .. "the master of the hidden dick technique. (Where is it? Nobody knows.)"
        elseif choice == 7 then
            message = message .. "cake. It isn't a lie!"
        elseif choice == 8 then
            message = message .. "the letter H."
        elseif choice == 9 then
            message = message .. "the number 69."
        elseif choice == 10 then
            message = message .. "grandmaster piledriver the 17th."
        elseif choice == 11 then
            message = message .. "the " .. RandomChoice("sentient apple tree.", "sentient sex toy.", "pan-dimensional toothbrush.", "magic motorcycle.", "regular bloke.")
        elseif choice == 12 then
            message = message .. "the unbelievably well behaved " .. RandomChoice("Pink Petal forum member.", "tentacle.", "pirate.", "sentient bottle.")
        elseif choice == 13 then
            message = message .. RandomChoice("Cousin", "Brother", "Saint", "Lieutenant", "Master", "Doctor", "Mr.", "Smith", "DockMaster", "Perfect") .. " Parkins from down the street."
        elseif choice == 14 then
            message = message .. "the really, really macho " .. RandomChoice("Titan.", "Storm Giant.", "small moon.", "kobold.", "madness.")
        elseif choice == 15 then
            message = message .. "the clockwork man! (With no sensation in his clockwork " .. RandomChoice("tool", "head", "fingers", "attachment", "clock") .. " and no sense to " ..
            RandomChoice("stop", "slow down", "moderate", "be gentle", "stop at " .. wm.Range(30, 80) .. " orgasms") .. ".)"
        elseif choice == 16 then
            -- MYR: This one gives useful advice to the players.  A gift from us to them.
            message = message ..WMDeveloper()
        elseif choice == 17 then
            message = message .. "the evolved sexual entity from " .. RandomChoice(
                    wm.Range(100000, 300000) .. " years in the future.",
                    "the closet.", "the suburbs.", "somewhere in deep space."
            )
        elseif choice == 18 then
            message = message .. "the " .. RandomChoice("mayor", "bishop", "town treasurer", "school principal") .. ", on one of his regular health checkups."
        elseif choice == 19 then
            message = message .. "a completely regular and unspectacular guy."
        elseif choice == 20 then
            message = message .. "the " .. wm.Range(5, 25) .. " dick, " .. wm.Range(5, 25) .. "-armed " ..
                    "(Each wearing " .. wm.Range(2, 6) .. " " .. RandomChoice("rings of the Schwarzenegger", "rings of the horndog", "rings of beauty", "rings of potent sexual stamina") ..
                    ") " .. RandomChoice("neighbor", "yugoloth", "abberation", "ancient one") .. "."

        end
    end
    return message
end

function WMDeveloper()
    return "the Brothel Master developer. " .. RandomChoice(
            "(Quick learner is a great talent to have.)",
            "(Don't ignore the practice skills option for your girls.)",
            "(Train your gangs.)",
            "(Every time you restart the game, the shop inventory is reset.)",
            "(Invulnerable (incorporeal) characters should be exploring the catacombs.)",
            "(High dodge gear is great for characters exploring the catacombs.)",
            "(For a character with a high constitution, experiment with working on both shifts.)",
            "(Matrons need high service skills.)",
            "(Girls see a max of 3 people for high reputations, 3 for high appearance and 3 for high skills.)",
            "(Don't overlook the bribery option in the town hall and the bank.)"
    )
end

------------------------------------------------------------------------------------------------------------------------
--                                     FOOTJOB
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetFootMessage(girl, customer)
    if girl:has_trait("Zombie") then
        -- " laid back as the customer used her feet to get off."; -- Its not great but trying to get something.. wrote when net was down so spelling isnt right CRAZY
        -- TODO
        return "(Z text not done)\n"
    end

    local skill = girl:footjob()
    if skill < 20 then
        if wm.Percent(30) and girl:has_trait("Cum Addict") then
            return "${name} squeezed the customer's cock around with her feet and licked up every last drop of cum when he finally finished."
        elseif wm.Percent(30) and girl:has_trait("Flexible") then
            customer:happiness(5)
            return "Though ${name} wasn't very skilled with her feet, her flexibility impressed the customer and improved his enjoyment a little.\n"
        elseif (wm.Percent(30) and girl:has_trait("Alcoholic")) or (wm.Percent(10) and girl:has_trait("Social Drinker")) then
            local message = "${name} was supposed to give a footjob. However, she'd had a few drinks"
            local choice = wm.Range(1, 3)
            if choice == 1 then
                customer:happiness(-15)
                return message .. " and was rough with her feet, giving the customer no pleasure and quite a lot of pain."
            elseif choice == 2 then
                customer:happiness(5)
                return message .. ", so even though the footjob was awful, she was fairly charming."
            elseif choice == 3 then
                customer:happiness(-10)
                return message .. " and accidentally stepped on a sensitive part of him."
            end
        else
            return RandomChoice("${name} awkwardly worked the customer's cock with her feet, not even managing to get him hard. Disappointed, he finally took matters into his own hands.\n",
                    "${name} squashed the customer's cock around with her feet, and accidentally stamped a ball, causing him some pain. He left disappointed.\n",
                    "${name} squeezed the customer's cock around with her feet, eventually managing to create some pleasant feelings, but recoiling when he finally came.\n"
            )
        end
    elseif skill < 40 then
        if wm.Percent(30) and girl:has_trait("Cum Addict") then
            return "${name} massage the customer's cock eagerly with her feet and licked up every last drop of cum when he finally came."
        elseif wm.Percent(30) and girl:has_trait("Sexy Air") then
            return "${name} flirted expertly as her feet massaged his cock. Her movements were crude and unskilled, but her sexy air more than made up for it."
        elseif wm.Percent(30) and girl:has_trait("Lesbian") then
            return "${name}  did her best to bring the customer off with her feet. This is about as close to a cock as this dyke wants to get."
        elseif (wm.Percent(30) and girl:has_trait("Alcoholic")) or (wm.Percent(10) and girl:has_trait("Social Drinker")) then
            local message =  "The customer asked ${name} for a footjob. She'd had a few drinks"
            local choice = wm.Range(1, 3)
            if choice == 1 then
                customer:happiness(-5)
                return message .. ", however, and was far too rough with her feet, making the whole experience a lot less pleasant. He did eventually come."
            elseif choice == 2 then
                customer:happiness(10)
                return message .. ", so even though the footjob was average, she flirted, goaded and charmed him to a powerful orgasm."
            elseif choice == 3 then
                customer:happiness(-15)
                return message .. " and accidentally kicked him a few times, causing him to leave in pain."
            end
        else
            return RandomChoice("${name} worked the customer's cock with her feet for a while, until his cum finally spurted onto her.",
                    "${name} rubbed the customer's cock around with her feet, accidentally swiping a ball with her toe, but she got him there in the end.\n",
                    "${name} massaged the customer's cock with her feet, managing to create some good feelings, but flinching when he finally came.\n"
            )
        end
    elseif skill < 60 then
        if wm.Percent(30) and girl:has_trait("Natural Pheromones") then
            return "Something about ${name}'s smell had him rock hard the moment he entered the room. She didn't have to do so much with her feet to bring him to an explosive orgasm.\n"
        elseif wm.Percent(40) and girl:has_trait("Twisted", "Audacity") then
            return "${name} skillfully rubbed the customer's cock with her feet. " ..
                    "At the last possible moment, she moved his cock so that he came all over his own stomach, laughing at his disgusted expression.\n"
        elseif wm.Percent(30) and girl:has_trait("Lesbian") then
            return "${name} skillfully milked the man's cock with her feet. She could do this all day with a smile if it meant she didn't have to fuck the stupid things.\n"
        elseif wm.Percent(30) and girl:has_trait("Cum Addict") then
            return "${name} massage the customer's cock powerfully with her feet until his cum exploded in her face.\n"
        elseif wm.Percent(30) and girl:has_trait("Sexy Air") then
            return "${name} flirted as her feet expertly massaged the customer's cock. Her movements were good, and her sexy air heightened the experience for him.\n"
        elseif wm.Percent(30) and girl:has_trait("Flexible") then
            customer:happiness(15)
            return "${name} used all her skills and her stunning flexibility to give the customer an excellent footjob.\n"
        else
            return RandomChoice("${name} skillfully worked the customer's cock with her feet, until his cum spurted all over her.\n",
                    "${name} deftly rubbed the customer's cock with her feet, making him cum all in her toes.\n",
                    "${name} expertly massaged the customer's cock with her feet, making him come twice.\n"
            )
        end
    else -- if skill < 80 then
        if wm.Percent(30) and girl:has_trait("Natural Pheromones") then
            return "Something about ${name}'s smell had his dick rock-hard the moment he entered the room. " ..
                    "Her expert feet took him through several orgasms, and left him blissed-out and exhausted.\n"
        elseif wm.Percent(30) and girl:has_trait("Twisted", "Audacity") then
            return "${name} expertly made love to the customer's cock with her feet until, at the last possible moment, " ..
                    "she pointed it up so that he shot his whole load up at his own gasping cum-face. After an experience like that, " ..
                    "he couldn't help laughing at her audacity, as he hocked his own cum out of his mouth.\n";
        elseif wm.Percent(30) and girl:has_trait("Lesbian") then
            return "${name} expertly milked the man's cock with her feet, making sure the cum mostly missed her. " ..
                    "Being skillful at this means she doesn't have to fuck the filthy things.\n"
        elseif wm.Percent(30) and girl:has_trait("Cum Addict") then
            return "${name} massage the customer's cock powerfully with her feet until his cum exploded in her open mouth.\n"
        elseif wm.Percent(30) and girl:has_trait("Sexy Air") then
            return "${name} flirted as her feet expertly massaged the customer's cock. Her movements were perfect, and her sexy air heightened the experience for to something divine.\n"
        elseif wm.Percent(30) and girl:has_trait("Flexible") then
            customer:happiness(25)
            return "${name} used all her foot-skills and her stunning flexibility to give the customer an unforgettable experience.\n"
        elseif wm.Percent(10) and girl:has_trait("Incest") then
            return "A child of incest, ${name} has unusual webbed-toes that make her footjobs an unforgettably 'gripping' experience.\n"
        else
            return RandomChoice("${name} wouldn't stop using her feet to massage the customer's cock until she had made him spill his entire load.\n",
                    "${name} expertly milked the customer's cock with her feet, until she felt his sticky cum pumping through her toes.\n",
                    "${name} loved using her feet on the customer's cock, and let him cum all over her.\n"
            )
        end
    end
    -- TODO Gondra: add extremely skilled texts.
end


------------------------------------------------------------------------------------------------------------------------
--                                     ORAL
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetOralMessage(girl, customer)
    if girl:has_trait("Zombie") then
        local message = "For some reason her customer wanted his dick in her mouth. She was all too happy to oblige him."
        if girl:has_trait("No Teeth") then
            message = message .. "Luckily, she has no teeth so she just gummed his dick until he came."
            customer:happiness(20)
        elseif wm.Percent(girl:health()) then
            message = message .. "Luckily, she has already been fed and did not eat him."
        elseif wm.Percent(girl:intelligence()) then
            message = message .. "Luckily, she has some of her senses left and did not eat him."
        elseif wm.Percent(50) then
            message = message .. "Unfortunately for him she was hungry and tried to eat what he put in her mouth."
            customer:happiness(-50)
        else
            message = message .. "Unfortunately for him she was hungry and she ate what he put in her mouth."
            customer:happiness(-100)
            girl:health(5)
            girl:happiness(5)
        end
        return message
    end

    local skill = girl:oralsex()
    if skill < 20 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return "The smell that came from the customers cock in front of her awoke ${name}'s hunger for cum, which made her work his shaft with considerable greed, forgetting to be careful with her teeth, until the customer came with a pained expression, letting her swallow what she craved."
        elseif wm.Percent(50) and girl:has_trait("Princess", "Queen", "Goddess", "Fallen Goddess", "Demon", "Your Daughter") then
            local message = "The customer is ecstatic. Sure it was awful and ${name} had no clue what she was doing - but"
            local royalty = girl:has_trait("Princess", "Queen")
            local divinity = girl:has_trait("Goddess", "Fallen Goddess")
            local demon = girl:has_trait("Demon")
            local yourKid = girl:has_trait("Your Daughter")

            if wm.Percent(25) then
                message = message .." he just made "
                if royalty then message = message .. "ROYALTY"
                elseif divinity then message = message .. "a GOD"
                elseif demon then message = message .."a DEMON"
                else message = message .. "your DAUGHTER"
                end
                message = message .. " swallow his cum!\n"
            else
                message = message .. RandomChoice("... He just had his dick sucked by ", " he just finished face-fucking ",
                        "... He just managed to Angry Dragon ")
                if royalty then message = message .. "Royalty!"
                elseif divinity then message = message .. "a Goddess!"
                elseif demon then message = message .. "a Demon Whore!"
                else message = message .. "your Daughter!\n"
                end
            end
            customer:happiness(20)
            -- TODO this was commented out in the C++ code
            --else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < (MINAGE + 2))) //if looks young or is near legal limit in game...
            --    {
            --    sexMessage << "The 'customer' was a City Official following up on complaints that " << girlName << " looked 'too young.' Specifically, girls under-"
            --    << MINAGE << " should not be used in this city. She explained that she is " << girl->age() << ", but with no documents, he demanded "
            --    << "- under Statute 2218-C - that she 'prove Majority' by demonstrating 'adult-level competence' in oral sex.\nShe submitted to testing";
            --    if (g_Dice.percent(GetSkill(girl, SKILL_ORALSEX)))  //Better she is, more hope she has
            --    {
            --    sexMessage << ", and against the odds managed to make him come in her mouth.\n\"She took a while, but seemed to have some basic idea,\" he said,"
            --    << " rating her 'Age Questionable.'\nThere is no fine, but this rating raises your suspicion.";
            --    g_Game->player().suspicion(10);
            --    }
            --    else
            --    {
            --    sexMessage << ", and failed badly, not even managing to get him hard.\n";
            --    if (g_Dice.percent(girl->beauty()))
            --    {
            --    sexMessage << "\"To make sure, I carried out other checks: she's young-looking, obviously; ";
            --    if (girl->has_trait("Delicate") || girl->has_trait("Fragile") || GetStat(girl, STAT_CONSTITUTION) < 40) sexMessage << "she's childishly delicate; ";
            --    if (girl->has_trait("Dependant")) sexMessage << "she's dependent, showing little sign of being able to care for herself; ";
            --    if (girl->has_trait("Tight Butt"))
            --    {
            --    sexMessage << "she has ";
            --    if (girl->has_trait("Flat Ass")) sexMessage << "a flat underdeveloped ass and ";
            --    sexMessage << "an extremely tight anus barely capable of fitting even my finger; ";
            --    }
            --    else if (girl->has_trait("Flat Ass")) sexMessage << "she has a flat underdeveloped ass; ";
            --    if (girl->has_trait("Short") || girl->has_trait("Dwarf")) sexMessage << "she's childishly short, probably not fully-grown; ";
            --    if (girl->has_trait("Virgin"))
            --    {
            --    sexMessage << "with modern surgery it's hard to be sure, but from closely examining her vagina she looks like a virgin, "
            --    << "it's certainly very tight; ";
            --    }
            --    if (girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) sexMessage << "she's clumsy; ";
            --    if (girl->has_trait("Shy") || (GetStat(girl,STAT_CHARISMA) < 40)) sexMessage << "she has the communication skills of a child; ";
            --    if (girl->has_trait("Tsundere") || girl->has_trait("Yandere")) sexMessage << "she has adolescent mood swings; ";
            --    if (girl->has_trait("Exhibitionist")) sexMessage << "she hasn't learned social norms yet, with no shame around nudity; ";
            --    if (girl->has_trait("Princess")) sexMessage << "she has childish delusions of being a 'Princess'; ";
            --    if (girl->has_trait("Nymphomaniac") || girl->has_trait("Fast Orgasms")) sexMessage << "she has sex-cravings and clitoral hyper-sensitivity - classic signs of adolescent nymphomania; ";
            --    if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
            --    {
            --    sexMessage << "she has underdeveloped breasts, noticeably firm to the touch";
            --    if (girl->has_trait("Perky Nipples")) sexMessage << " and highly-sensitive perky little nipples";
            --    else if (girl->has_trait("Puffy Nipples")) sexMessage << " and sensitive puffy nipples";
            --    sexMessage << "; ";
            --    }
            --    sexMessage << "she... Well, the list goes on. You get the picture.\"\n \n";
            --    }
            --    sexMessage << "He finally rated her 'underage' and fined you 1,000 gold on the spot, and filed a report against you with the City Hall.\nThis will not help your reputation.";
            --    g_Game->gold().misc_debit(1000);
            --    g_Game->player().suspicion(10);
            --    }
            --    }
            return message
        elseif wm.Percent(30) and girl:has_trait("Shy", "Nervous", "Lolita") then
            local message = "${name} blushed furiously, with no idea how to pleasure this thing in front of her. "
            if wm.Percent(50) then
                customer:happiness(5)
                girl:oralsex(2)
                return message .. "The customer was patient, teaching her how to do it properly."
            else
                message = message .. "The customer eventually got bored, grabbed her head and started face-fucking her. "
                if girl:has_trait("Strong Gag Reflex", "Gag Reflex") then
                    customer:happiness(-30)
                    return message .. "She gagged, retched and threw up on his cock. The customer left disgusted."
                end
                return message .. "She started gagging and just when she thought she was going to throw up, the customer's hot cum pumped into her mouth."
            end
        elseif wm.Percent(40) and girl:has_trait("Dick-Sucking Lips") then
            customer:happiness(5)
            return "Although she isn't particularly good at it, the customer enjoyed seeing ${name}'s lips wrapped around his cock."
        elseif wm.Percent(30) then
            return RandomChoice("${name} gave the customer a sloppy, awkward blowjob that wasn't going anywhere. Finally he finished himself off in her face.",
                    "${name} awkwardly licked the customer's cock, and recoiled when he came."
            )
        else
            customer:happiness(-5)
            return "Annoyed by her slow licks, the customer pushed his throbbing cock through ${name}'s lips, roughly fucking her mouth until he finished, leaving the room while she still spit out his cum.";
        end
    elseif skill < 40 then -- Gondra:  if the girl is slightly skilled
        --    #if FMA		//in case min age cannot be raised.
        --else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < 20)) //if looks young or is near legal limit in game...
        --#else
        --else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < (MINAGE + 2))) //if looks young or is near legal limit in game...
        --#endif
        --    {
        --    sexMessage << "The 'customer' was a City Official following up on complaints that " << girlName << " looked 'too young.' Specifically, girls under-"
        --    << MINAGE << " should not be used in this city. She explained that she is " << girl->age() << ", but with no documents, he demanded "
        --    << "- under Statute 2218-C - that she 'prove Majority' by demonstrating 'adult-level competence' in oral sex.\nShe submitted to testing, "
        --    << "and he carried out a full test, finally rating her skills 'insufficient' - \"I told her to swallow and she couldn't even do it,\" he explained, shaking his head.\n";
        --    int evidence = 0;
        --    if (g_Dice.percent(girl->beauty())) //He seems to investigate pretty girls more - coincidence, huh?!
        --    {
        --    sexMessage << "\"When she failed THAT,\" he explained. \"I carried out further tests to establish her maturity: firstly, she's clearly very young-looking; ";
        --    if (girl->has_trait("Delicate") || girl->has_trait("Fragile") || GetStat(girl, STAT_CONSTITUTION) < 40) sexMessage << "she's childishly delicate; ", evidence++;
        --    if (girl->has_trait("Dependant")) sexMessage << "she's dependent, showing little sign of being able to care for herself; ", evidence++;
        --    if (girl->has_trait("Tight Butt"))
        --    {
        --    sexMessage << "she has ";
        --    if (girl->has_trait("Flat Ass")) sexMessage << "a flat underdeveloped ass and ", evidence++;
        --    sexMessage << "an extremely tight anus barely capable of fitting even my finger; ", evidence++;
        --    }
        --    else if (girl->has_trait("Flat Ass")) sexMessage << "she has a flat underdeveloped ass; ", evidence++;
        --    if (girl->has_trait("Short") || girl->has_trait("Dwarf")) sexMessage << "she's childishly short, probably not fully-grown; ", evidence++;
        --    if (girl->has_trait("Virgin"))
        --    {
        --    sexMessage << "with modern surgery it's hard to be sure, but from closely examining her vagina she looks like a virgin, "
        --    << "it's certainly very tight; ", evidence++;
        --    }
        --    if (girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) sexMessage << "she's clumsy; ", evidence++;
        --    if (girl->has_trait("Shy") || (girl->charisma() < 40)) sexMessage << "she has the communication skills of a child; ", evidence++;
        --    if (girl->has_trait("Tsundere") || girl->has_trait("Yandere")) sexMessage << "she has adolescent mood swings; ", evidence++;
        --    if (girl->has_trait("Exhibitionist")) sexMessage << "she hasn't learned social norms yet, with no shame around nudity; ", evidence++;
        --    if (girl->has_trait("Princess")) sexMessage << "she has childish delusions of being a 'Princess'; ", evidence+=2;
        --    if (girl->has_trait("Nymphomaniac") || girl->has_trait("Fast Orgasms")) sexMessage << "she has sex-cravings and clitoral hyper-sensitivity - classic signs of adolescent nymphomania; ", evidence++;
        --    if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
        --    {
        --    sexMessage << "she has underdeveloped breasts, noticeably firm to the touch", evidence++;
        --    if (girl->has_trait("Perky Nipples")) sexMessage << " and highly-sensitive perky little nipples";
        --    else if (girl->has_trait("Puffy Nipples")) sexMessage << " and sensitive puffy nipples";
        --    sexMessage << "; ";
        --    }
        --    sexMessage << "she's ... Well, the list goes on. You get the picture.\"\n";
        --    }
        --    if (evidence > 3)
        --    {
        --    sexMessage << "\nOn this 'evidence' he rated her \"Likely Underage\" - fining you 500 gold and filing a report against you.\n\""
        --    << ((g_Game->player().Gender() == GENDER_MALE) ? "Men" : "Women")
        --    << " like you disgust me!\"\n";
        --    g_Game->gold().misc_debit(500);
        --    }
        --    else sexMessage << "He finally rated her \"Age Questionable - follow up visit required\" - this significantly raises suspicion on your establishments.";
        --    g_Game->player().suspicion(10);
        --    }
        if wm.Percent(40) and girl:has_trait("Cum Addict") then
            return "Knowing about the reward that awaited her, ${name} sucked on the customers length with a singular drive that made the customer come quickly. She continued sucking until she had swallowed the last drop of his cum"
        elseif wm.Percent(20) and girl:has_trait("Aggressive", "Twisted", "Sadistic") then
            customer:happiness(-20)
            return "After some time there was a high-pitch squeal. A few minutes later, ${name} left the room, with the customer hobbling out behind her clutching his groin."
                    .. "\"They come in my eyes,\" she calmly explained, wiping down her face with a tissue. \"I punch in their balls.\n\"Fair's fair.\""
        elseif wm.Percent(60) and girl:has_trait("Sexy Air") then
            customer:happiness(10)
            return "${name} isn't the best at this, but something about the sexy way she keeps eye-contact right through makes the experience far more intense."
        else
            return RandomChoice("Although still a bit awkward, ${name} worked the customers length with her tongue and mouth, only spitting out the customers cum after he had left.",
                    "${name} mechanically pleasured her customers cock, his load shooting all over her face as she didn't pay attention."
            )
        end
    elseif skill < 60 then  -- Gondra: the girl is reasonably skilled
        --[[
        if (g_Dice.percent(5) && girl->has_trait("Lolita"))
        {
        sexMessage << "The customer, a City Official, claimed to be responding to 'complaints' that " << girlName << " was 'clearly under-" << MINAGE << ".' She told him she is actually "
        << girl->age() << ", but in the absense of documentation, he demanded - under City Statute 2218-C - that she 'prove Majority' by 'demonstrating adult-level competence' in oral sex.\n"
        << "He carried out a thorough test and finally rated her skills 'good' - \"She was very efficient,\" he nodded. \"I tested her three times and her skill certainly matches an appropriate age.\"\n"
        << "He rated her age 'legal' - this slightly reduces suspicion around your establishments.";
        g_Game->player().suspicion(-5);
        }
        ]]
        if wm.Percent(35) and girl:has_trait("Cum Addict") then
            return "${name} managed to make the customer cum a second time as she continued to suck on him after she had swallowed his first load."
        elseif wm.Percent(35) and (girl:has_trait("Old") or girl:age() > 45) then
            customer:happiness(2)
            girl:happiness(-2)
            return "${name} sucked on his cock until his cum splattered across her face. She rubbed it into her skin - explaining that it's her secret ingredient for youthful looks.\nThe customer joked that she could probably use some more.";
        elseif wm.Percent(50) and girl:has_trait("Dick-Sucking Lips") then
            customer:happiness(10)
            return "Sure there are more skillful girls out there, but having ${name}'s full, soft lips wrapped around his meat blew the customer's mind."
        elseif wm.Percent(34) and girl:has_trait("Clumsy", "Dojikko") then
            local message =  "${name} gave pretty good head. Unfortunately, a clumsy accident "
            local choice = wm.Range(1, 5)
            if choice == 1 then
                message = message .. "with some candles"
            elseif choice == 2 then
                if girl:has_item("Cat") then message = message .. "involving her cat"
                elseif girl:has_item("Guard Dog") then message = message .. "with her Guard Dog"
                else message = message .. "with a badly timed sneeze"
                end
            elseif choice == 3 then
                if (girl:has_item("Studded Dildo") or girl:has_item("Dreidel Dildo") or girl:has_item("Dildo")) then
                    message = message .. "with a misplaced dildo"
                else
                    message = message .. "with some chewing gum and a lava lamp"
                end
            elseif choice == 4 then
                message = message .. "with a plugged-in hair-curler"
            elseif choice == 5 then
                if girl:has_trait("Pierced Clit", "Pierced Nipples", "Pierced Tongue", "Pierced Navel", "Pierced Nose") then
                    message = message .. "with her piercing"
                else
                    message = message .. "with an ornate hairpin"
                end
            end
            customer:happiness(-10)
            return message .. " left the customer in pain."
        else
            return RandomChoice("${name} licked and sucked the customer's cock with some skill.",
                    "${name} gave the customer some pretty good head, even giving the balls a good licking, until he came on her face.",
                    "${name} sucked the customer off until he came in her mouth. She then made a great show of dribbling it all onto her chest and rubbing it into her breasts.",
                    "${name} made a few more slurping noises than necessary, didn't forget to give his balls a bit of attention and swallowed the customer's cum after showing it to him. Altogether good work."
            )
        end
    elseif skill < 80 then
--[[    else if (g_Dice.percent(5) && girl->has_trait("Lolita"))
        {
        sexMessage << "The customer, a City Official, claimed to be responding to 'complaints' that " << girlName << " was 'clearly under-" << MINAGE << ".' She told him she is actually "
        << girl->age() << ", but in the absense of documentation, he demanded - under City Statute 2218-C - that she 'prove Majority' by 'demonstrating adult-level competence' in oral sex.\n"
        << "He carried out a thorough test and finally rated her skills 'Excellent' - \"A girl who can do that,\" he said. \"Is definitely old enough in my book.\"\n"
        << "He rated her age 'Legal' and left praising her skills - this reduces suspicion around your establishments and improves her fame.";
        g_Game->player().suspicion(-15);
        girl->upd_stat(STAT_FAME, 5);

    end]]
        if wm.Percent(65) and girl:has_trait("Doctor") then
            return "${name} " .. RandomChoice(" expertly stimulated the customer with her mouth and tongue - extracting every drop of semen with medical precision.",
                    " gives a whole new meaning to 'bedside manner' as she kneels beside the bed sucking the customer's balls dry."
            )
        elseif wm.Percent(65) and girl:has_trait("Cum Addict") then
            return "${name} kept caressing the customers cock and balls making him cum again and again, swallowing each load until he was dry."
        elseif wm.Percent(50) and girl:has_trait("Lesbian") then
            return "From the expert way ${name} sucks the customer's cock, you'd never think she was a lesbian. She even swallows. A true professional."
        elseif wm.Percent(60) and girl:has_trait("Deep Throat", "No Gag Reflex") then
            return "Surprising the customer, ${name} rammed his hard cock down her own throat, occasionally looking up to his face while she worked on it with all her skill."
        elseif wm.Percent(60) and girl:has_trait("Nimble Tongue") then
            return "Instead of a normal blowjob, ${name} showed off just how nimble her tongue is, making him blow his load after keeping him on edge for several minutes just with the tip of her tongue."
        elseif wm.Percent(50) and girl:has_trait("Princess", "Queen", "Goddess", "Fallen Goddess", "Demon", "Your Daughter") then
            local message = "The customer was overjoyed. ${name} gave him amazing oral "
            local royalty = girl:has_trait("Princess", "Queen")
            local divinity = girl:has_trait("Goddess", "Fallen Goddess")
            local demon = girl:has_trait("Demon")

            if wm.Percent(25) then
                message = message .. "and he just watched "
                if royalty then message = message .. "a ROYAL"
                elseif divinity then message = message .. "a GODDESS"
                elseif demon then message = message .."a DEMON"
                else message = message .. "your DAUGHTER"
                end
                message = message .. " swallow down his cum!"
            else
                message = message .. RandomChoice("and he can't wait to tell his friends he had his cock blown by ", "- he just face-fucked ",
                        "- and the girl whose face he painted... ")
                if royalty then message = message .. "Royalty!"
                elseif divinity then message = message .. "a Goddess!"
                elseif demon then message = message .. "a Demon!"
                else message = message .. "your Daughter!"
                end
            end
            customer:happiness(20)
            return message
        elseif wm.Percent(60) and girl:has_trait("Queen") then
            customer:happiness(10)
            girl:fame(5)
            return "${name}'s former subject, the customer was overjoyed to have his Monarch suck his dick, and felt great pride when she swallowed his cum. " ..
                    "He left to tell all his friends how he had been expertly sucked off by a Queen! (Leading to lots sniggering, and rumours that would follow him for years to come)\n"
        else
            return RandomChoice(
                    "${name} dropped to her knees and let her mouth make love to the customer's cock.",
                    "${name} loved sucking the customer's cock, and let him cum all over her.",
                    "${name} licked the customer's balls and shaft so expertly, he came within seconds of her putting his cock in her mouth. She looked up at him, swallowed, and started again.",
                    "${name} wouldn't stop licking and sucking the customer's cock until she had swallowed his entire load."
            )
        end
    else
        local message = "${name} "
        local choice = wm.Range(1, 6)
        if choice == 1 then
            message = message .. RandomChoice("knelt in front of", "bowed deeply to", "fingered herself in front of",
                    "stripped off before", "smiled hungrily at") .. " the client, and grabbed his " .. RandomChoice(
                    "meat", "one-eyed dragon", "cock", "trouser-snake", "love train", "bald bishop", "fuckpole",
                    "meaty womb raider", "crankshaft", "greasy gutbuster"
            )
        elseif choice == 2 then
            message = message .. "made him sit on a chair and watch as she 'fellated' a " .. RandomChoice(
                    "banana", "dildo", "bottle", "broomstick", "bedknob"
            ) .. " while " .. RandomChoice("never breaking eye-contact", "fingering her clit", "rubbing her nipples",
                    "another girl watched", "bending waaaaayyy over"
            )
        elseif choice == 3 then
            message = message .. "had some fun with his " .. RandomChoice("foot", "tickle", "hair", "butt", "food") ..
                    " fetish and gave him an extended " .. RandomChoice("ball", "anal", "cock") .. " 'massage'"
        elseif choice == 4 then
            message = message .. "dressed as a " .. RandomChoice("schoolgirl", "girl-next-door", "cheerleader",
                    "baby-sitter", "priestess") .. " and begged him to " .. RandomChoice(
                    "let her know what cum tastes like", "teach her how to please a man", "let her taste his love",
                    "use her worthless face", "be gentle... but not too gentle"
            )
        elseif choice == 5 then
            message = message .. "lay naked across " .. RandomChoice("the bed", "the desk", "some cushions",
                    "a cold table-top", "a pile of boxes") .. " with her head hanging over the edge " .. RandomChoice(
                    "and her mouth wide open", "and her throat ready", "ready for a face-fuck", "and her lips puckered",
                    "and a craving in her eyes"
            )
        elseif choice == 6 then
            message = message .. "warmed up by " .. RandomChoice("shoving a giant dildo down her throat",
                    "deepthroating a perfume bottle", "shoving her fingers down her throat",
                    "gagging down her favourite butt-plug", "deepthroating a sacred religious symbol") ", as she absently " ..
                    RandomChoice("rubbed her clit", "pinched her nipples", "flicked her bean", "fisted herself",
                            "fingered her anus")
        end
        message = message .. ". Finally, "
        choice = wm.Range(1, 7)
        if choice == 1 then
            message = message .. "her " .. RandomChoice("soft-lips made love to his", "open throat sucked down his",
                    "tongue milked his", "mouth energetically fucked his") .. " " .. RandomChoice("rock hard cock",
                    "slippery man-meat", "greasy sex-pole", "fat flesh-flute") .. " until " .. RandomChoice(
                    "hot cum was forced down her throat", "cum splattered across her face",
                    "she received a spunky facial", "cum was shooting out of her nose", "her mouth was shot full of semen") ..
                    " courtesy of"
        elseif choice == 2 then
            message = message .. "her jaw felt like it was gonna lock as " .. RandomChoice(
                    "she greedily gulped his meat", "she relentlessly sucked him off",
                    "her mouth expertly milked his manhood", "his cock slipped into her throat",
                    "she sucked and licked his cock"
            ) .. " until " .. RandomChoice("a warm desposit was left in her mouth",
                    "cum was pumped directly into her stomach", "her eyes bulged as cum filled her head",
                    "cum was blasted into her face", "sperm dripped from her mouth and nose"
            ) .. " courtesy of"
        elseif choice == 3 then
            message = message .. "she was face-fucked " .. RandomChoice("blind", "raw", "senseless", "for hours",
                    "against the wall") .. " by"
        elseif choice == 4 then
            message = message .. "her throat was stuffed full of " .. RandomChoice("hot cock", "male anatomy",
                    "her favourite thing", "quivering pork sword", "stanky dick") .. " until " ..
                    RandomChoice("her eyes bulged", "her ears popped", ", with a shudder, cum was shot straight into her gut",
                            "he pulled out and splurged in her face", "cum was left drooling from her mouth") .. " thanks to"
        elseif choice == 5 then
            message = message .. "she took his cock in her mouth and expertly " .. RandomChoice("sucked it down",
                    "licked from smooth head to hairy base", "pleasured the head with her soft lips", "deepthroated the lot",
                    "massaged it with her lips while licking with her tongue") .. " while her hands "..
                    RandomChoice("played with his balls", "cradled his balls", "gripped his butt", "stayed behind her back",
                            "fingered his ass-hole") .. " until she got a hot mouthful of cum from"
        elseif choice == 6 then
            message = message .. "the customer climbed over, and full-on fucked her face. She ended up " .. RandomChoice(
                    "with a warm deposit filling her mouth", "with her gut pumped full of cum", "spunked up from nose to navel",
                    "with cum blasted in her face", "with sperm exploding from her mouth and nose") .. " courtesy of"
        elseif choice == 7 then
            message = message .. "she clamped her lips around his dick and wouldn't stop " .. RandomChoice(
                    "sucking and stimulating it with her mouth", "orally fucking it", "deepthroating it", "pumping it",
                    "gulping it down") .. " until she had swallowed down "  .. wm.Range(2, 7) .. " loads of cum from"
        end
        message = message .. " "
        choice = wm.Range(1, 18)

        if choice == 1 then
            message = message .. "the guy " .. RandomChoice("wearing three amulets of the sex elemental.",
                    "wearing eight rings of the horndog.", "wearing a band of invulnerability.",
                    "in the Guy mask.", "with the funny eyes."
            )
        elseif choice == 2 then
            message = message .. "Poseidon, God of Salty Seamen!!!!"
        elseif choice == 3 then
            message = message .. "the good " .. RandomChoice("Lord Mayor.", "samaritan.", "Reverend.", "shepherd.",
                    "husband (a blowjob isn't cheating).")
        elseif choice == 4 then
            message = message .. "the surprisingly endowed dwarven gentleman."
        elseif choice == 5 then
            message = message .. "Colonel Mustard (right after he did it with the candlestick)."
        elseif choice == 6 then
            message = message .. "the "  .. RandomChoice("talking bear.", "bearded biker dude.",
                    "dude who looked like a lady.", "mischievous monkey man.", "escaped pensioner.")
        elseif choice == 7 then
            message = message .. "the unbelievably well behaved " .. RandomChoice("Pink Petal forum member.",
                    "Judge.", "pirate.", "ninja.")
        elseif choice == 8 then
            message = message .. "the infamous " .. RandomChoice("Lord", "Master", "Saint", "Lieutenant", "Master",
                    "Doctor", "Gardener", "Blacksmith", "DockMaster", "Sherrif") .. " Peterson."
        elseif choice == 9 then
            message = message ..  "this regular customer."
        elseif choice == 10 then
            message = message .. "the pesky poltergeist."
        elseif choice == 11 then
            message = message .. "her " .. RandomChoice("platonic guy friend, who she'd always thought of more as a brother.",
                    "personal trainer, Buck.", "excited former-classmate.", "father's best-friend.", "kindly uncle.")
        elseif choice == 12 then
            message = message .. "the clockwork man! (With no sensation in his clockwork " .. RandomChoice("tool", "head", "fingers", "attachment", "clock") .. " and no sense to " ..
                    RandomChoice("stop", "slow down", "moderate", "be gentle", "stop at " .. wm.Range(30, 80) .. " orgasms") .. ".)"
        elseif choice == 13 then
            message = message .. WMDeveloper()
        elseif choice == 14 then
            message = message .. "a guy she never noticed at school."
        elseif choice == 15 then
            message = message .. "a man cursed with permanent 'hardness.'"
        elseif choice == 16 then
            message = message .. "the " .. RandomChoice("mayor", "bishop", "town treasurer", "school principal") ..
                    ", on one of his regular health checkups."
        elseif choice == 17 then
            message = message .. "a young guy in " .. RandomChoice("a dress.", "robes.", "handcuffs.",
                    "a straight-jacket.", "trouble with the mob.")
        elseif choice == 18 then
            message = message .. "the untapped virgin."
        end
        return message
    end
end

------------------------------------------------------------------------------------------------------------------------
--                                     TITTY
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetTittyMessage(girl, customer)
    local message = ""
    if girl:has_trait("Zombie") then
        --message += " stared off vancantily as the customer used her tits to get off."; /*Its not great but try to get something.. wrote when net was down so spelling isnt right CRAZY*/
        message = "(Z text not done)\n"; -- TODO
    end

    local skill = girl:tittysex()
    if skill < 20 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return message .. "After he was done fucking her tits, ${name} scooped up his cum from her tits greedily licking off every single drop from her fingers."
        elseif wm.Percent(35) and girl:has_trait("Flat Chest", "Petite Breasts", "Small Boobs") then
            return message .. "${name} struggled to pleasure the customer with the little bit of chest she has, until the customer jerked off onto her tiny tits telling her to rub his cum in if she wants to have actual tits someday."
        elseif wm.Percent(35) and girl:has_trait("Busty Boobs", "Big Boobs", "Giant Juggs", "Massive Melons", "Abnormally Large Boobs", "Titanic Tits") then
            return message .. "${name} was lying on her back occasionally yelping in pain as the customer roughly fucked her quavering tits"
        elseif wm.Percent(35) and girl:has_trait("Furry", "Cow Girl") then
            message = message .. "Despite having more breasts than most, "
            if girl:has_trait("Furry") then
                message = message .. "beast-girl"
            else
                message = message .. "cow-girl"
            end
            return message .. " ${name} gave poor titty-sex.";
        elseif wm.Percent(35) and girl:has_trait("Plump", "Fat") then
            customer:happiness(-10)
            return message .. "${name} gave awful titty-sex and flubbered uselessly around the bed like a beached whale."
        elseif wm.Percent(35) and (girl:has_trait("Abundant Lactation", "Cow Tits") or girl:lactation() > 50) then
            return message .. "${name}'s breast milk squirted around as the customer tried to fuck her tits. She wasn't good at this."
        elseif wm.Percent(35) and girl:has_trait("Exotic") then
            return message .. "${name} finds this breast fetish strange. In her land people are topless and breasts are boring, functional things for feeding babies. Why would you fuck them?!"
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(5)
            return message .. "${name} had an amazing, sexy way of revealing her self which really aroused the customer. Sadly the titty-sex that followed was disappointing."
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(15)
            return message .. "${name} was awful at titty-sex and did nothing for the customer. But she's very cute. And there are worse things than spunking on a cute girl's chest, so the customer didn't mind so much."
        else
            return message .. RandomChoice("${name} lay passively on the bed as the customer tried to bring himself off with her tits.",
                    "${name} squeezed her breasts around amateurishly, making it difficult for the customer to get any good feeling from fucking them.",
                    "${name} awkwardly let the customer fuck her tits.",
                    "${name} held together her breasts for the customer to fuck, sighing loudly as his cum dirtied her chest."
            )
        end
    elseif skill < 40 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return message .. "After letting her customer use her tits, ${name} managed to catch most of his load in her mouth as he came, eagerly licking up the rest."
        elseif wm.Percent(35) and girl:has_trait("Flat Chest", "Petite Breasts", "Small Boobs") then
            return message .. "${name} let the customer rub his cock against the nipples of her meager breasts until he came."
        elseif wm.Percent(35) and girl:has_trait("Busty Boobs", "Big Boobs", "Giant Juggs", "Massive Melons", "Abnormally Large Boobs", "Titanic Tits") then
            return message .. "Her customers cock completely disappearing between her breasts, ${name} heaved her chest up and down her customers cock, until she could feel his hot cum between her breasts."
        elseif wm.Percent(35) and girl:has_trait("Furry", "Cow Girl") then
            message = message .. "Despite having more breasts than most girls, "
            if girl:has_trait("Furry") then
                message = message .. "beast-girl"
            else
                message = message .. "cow-girl"
            end
            customer:happiness(5)
            return message .. " ${name} gave okay titty-sex."
        elseif wm.Percent(35) and girl:has_trait("Plump", "Fat") then
            return message .. "${name} her sweat lubricated the titty-sex a little as she hefted herself around the bed."
        elseif wm.Percent(35) and (girl:has_trait("Abundant Lactation", "Cow Tits") or girl:lactation() > 50) then
            return message .. "${name}'s breast milk squirted around as the customer fucked her tits."
        elseif wm.Percent(35) and girl:has_trait("Exotic") then
            return message .. "${name} finds this breast fetish strange. In her land breasts are boring, functional things. However, she is making a good effort to learn."
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(5)
            return message .. "${name} had an amazing, sexy way of revealing her self which really aroused the customer. The titty-sex that followed was okay."
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(15)
            return message .. "${name} was cute enough that the customer barely noticed how poor she was at titty-sex. In the end he spunked on a cute girl's breasts - that's a win in anyone's book."
        else
            return message .. RandomChoice("${name} lay awkwardly on the bed as the customer used her tits.",
                    "${name} squeezed her breasts around, helping the customer get some good feeling from fucking them.",
                    "${name} used her breasts on the customer's cock.",
                    "Holding her customers cock between her breasts, ${name} unintentionally let her hot breath run over the customers tip as she massaged his shaft with her chest, earning her a sudden facefull of cum, some of it spraying into her mouth."
            )
        end
    elseif skill < 60 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return message .. "As she rubbed spit onto her tits, ${name} asked her customer to give her something nice and hot to drink when he is done. Smiling, the customer fulfilled her wish with a big load of cum sprayed directly into her mouth."
        elseif wm.Percent(50) and girl:has_trait("Flat Chest", "Petite Breasts", "Small Boobs") then
            return message .. "With her chest oiled up, ${name} moaned lightly as she rubbed her whole upper body against her customer, letting a pleasant moan escape her lips as his hot cum splattered over her chest."
        elseif wm.Percent(50) and girl:has_trait("Busty Boobs", "Big Boobs", "Giant Juggs", "Massive Melons", "Abnormally Large Boobs", "Titanic Tits") then
            return message .. "Moaning lightly as she 'accidentally' pushed the customers cock against one of her nipples, ${name} begun to run him through a long, teasing routine, at the end of which he covered her large chest with a large load of his seed."
        elseif wm.Percent(40) and girl:has_trait("Waitress") then
            customer:happiness(5)
            return message .. "${name} entered dressed as a topless waitress, with her breasts pushed up on a tray. She presented them to the customer, who excitedly fucked her chest."
        elseif wm.Percent(40) and girl:has_trait("Muscular") then
            customer:happiness(5)
            return message .. "${name} gripped his cock between her powerful chest muscles, and gave him a powerful titty-fuck."
        elseif wm.Percent(35) and girl:has_trait("Furry", "Cow Girl") then
            message = message .. "With her extra breasts, "
            if girl:has_trait("Furry") then
                message = message .. "beast-girl"
            else
                message = message .. "cow-girl"
            end
            customer:happiness(5)
            return message .. "${name} gave some memorable titty-sex."
        elseif wm.Percent(35) and girl:has_trait("Plump", "Fat") then
            customer:happiness(5)
            return message .. "${name} used her fat to good effect in titty-sex, making her breasts grip him, and ripple very pleasantly."
        elseif wm.Percent(35) and (girl:has_trait("Abundant Lactation", "Cow Tits") or girl:lactation() > 50) then
            return message .. "${name}'s breast milk squirted everywhere as the customer fucked her tits, providing nice lubrication."
        elseif wm.Percent(35) and girl:has_trait("Exotic") then
            return message .. "${name} finds this breast fetish strange. In her land breasts are boring, functional things. However, she is making a good effort to learn."
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(5)
            return message .. "${name} had an amazing, sexy way of revealing her self which really aroused the customer. The titty-sex that followed was okay."
        elseif wm.Percent(35) and girl:has_trait("Exhibitionist", "Sexy Air") then
            customer:happiness(15)
            return message .. "${name} was cute enough that the customer barely noticed how poor she was titty-sex. In the end he spunked on a cute girl's breasts - that's a win in anyone's book."
        else
            return message .. RandomChoice("${name} clamped her breasts around his cock and jiggled them rhythmically until his cum splattered onto her chin.",
                    "${name} squeezed her breasts around, enhancing the customer's feeling as he fucks them.",
                    "${name} smiled as her customer spurted his load over her face and chest, rubbing it over her chest as he left before she went and cleaned herself up.",
                    "${name} enjoyed using her breasts on the customer's cock, letting him cum all over her."
            )
        end
    else -- TODO There are no separate messages for the next two levels
        if wm.Percent(60) and girl:has_trait("Cum Addict") then
            return message .. "${name} expertly rubbed her chest against her customer, making him blow his load after only a few minutes, rubbing strength back into his length even as she still swallowed his first load, earning herself a second portion of her favorite meal before he left exhausted."
        elseif wm.Percent(60) and girl:has_trait("Flat Chest", "Petite Breasts", "Small Boobs") then
            return message .. "Although the customer seemed to have originally having wanted to pick on ${name}, he is left breathless as she easily makes him cum with her small bosom, that he had wanted to mock."
        elseif wm.Percent(60) and girl:has_trait("Busty Boobs", "Big Boobs", "Giant Juggs", "Massive Melons", "Abnormally Large Boobs", "Titanic Tits") then
            return message .. "It didn't take long before ${name} had the first load of cum coat the flesh between her breasts, but through a combination of breathless moans and expert handling of her large mammaries she managed to add a second load onto her jiggling flesh before the session ended."
        else
            return RandomChoice(
                    "${name} wouldn't stop using her breasts to massage the customer's cock until she had made him spill his entire load.",
                    "${name} expertly used her breasts to massage the customer's cock until his entire load exploded over her."
            )
        end
    end
end

------------------------------------------------------------------------------------------------------------------------
--                                     HANDJOB
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetHandjobMessage(girl, customer)
    local message = ""
    local skill = girl:handjob()
    if girl:has_trait("Zombie") then
        message = "(Z text not done)\n"
    end

    if skill < 20 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return message .. "${name} sat down for a few minutes to lick the cum from her hands after she had finally managed to get her customer off with her hands."
        elseif wm.Percent(50) and girl:has_trait("Farmer", "Farmers Daughter", "Country Gal") then
            customer:happiness(-10)
            return message .. "${name} yanks it about like she's milking a cow, leaving the customer in real pain.\n"
        else
            return RandomChoice("After enduring a few minutes of her awful handjob, the customer took hold of ${name}'s hand, spit on it, and then proceeded to quickly jerk off using her hand, not paying attention to her disgusted face as he left her hand dripping with his seed.",
            "${name} awkwardly worked the customer's cock with one hand, looking a bit disgusted at the gooey seed coating her hand after he had spurted his load without warning.")
        end
    elseif skill < 40 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return message .. "'s handjob was more awkward than necessary as she almost fell because she tried to catch all his seed in her hands as her customer came."
        elseif wm.Percent(50) and girl:has_trait("Farmer", "Farmers Daughter", "Country Gal") then
            customer:happiness(-5)
            return message .. "${name} still seems think this is like milking cows on the farm, yanking the customer around quite unpleasantly.\n"
        else
            return RandomChoice("${name} jerked her customer off mechanically, letting his seed dribble onto the floor.",
            "${name} used her hand on the customer's cock.")
        end
    elseif skill < 60 then
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return "${name} massaged his length and balls slowly with one hand, collecting his seed in a glass she held in her other hand as he came, greedily licking every last drop from it after she was done milking him."
        else
            return RandomChoice("Audibly breathing and seemingly completely fixated on the cock in her hands, ${name} teased her customer with her fingertips until he came hard, the first spurt of his seed hitting the floor quite a bit away.",
                    "${name} enjoyed using her hand on the customer's cock to make him cum.")
        end
    else
        if wm.Percent(50) and girl:has_trait("Cum Addict") then
            return "Massaging her customers scepter and crown jewels intensely, ${name} prevented him from cumming until with a small string of silken string wrapped around the base of his shaft until he begged her to let him cum, visibly enjoying as rope after rope of his hot cum landed in her mouth."
        elseif wm.Percent(50) and girl:has_trait("Prehensile Tail") then
            return message .. "${name}'s tail wrapped tightly around her customers cock jerking him off while both her hands ran over her body giving him quite the show, which he must have enjoyed judging by the mess he made when he came."
        else
            return RandomChoice("${name} was moaning lightly as strings of hot cum covered her body, but didn't stop moving her hands over his cock, squeezing another exited spurt from his balls before the customer left with quivering knees.",
                "${name} loved using her hand on the customer's cock, and let him cum all over her.")
        end
    end
end

------------------------------------------------------------------------------------------------------------------------
--                                     STRIP
------------------------------------------------------------------------------------------------------------------------
---@param girl wm.Girl
---@param customer wm.Customer
function GetStripMessage(girl, customer)
    if girl:has_trait("Zombie") then
        return "While Zombies don't generally care about clothes, ${name} did not so much \"strip\" as tear her clothes off.\n";
    end
    local skill = girl:strip()
    local ugly = girl:beauty() < 45;
    if skill < 20 then
        if wm.Percent(60) and ugly then
            return "${name} was self-conscious of her looks and gave a weak performance."
        else
            return RandomChoice("${name} shyly took her clothes off in front of the customer.",
                    "${name} stared at the floor as she stood there awkwardly taking off her clothes in front of the customer. She was embarrassed and kept covering herself with her arms and hands."
            )
        end
    elseif skill < 40 then
        if wm.Percent(60) and girl:is_pregnant() then
            return "${name} stripped off well, despite her pregnancy."
        else
            return RandomChoice("${name} coyly took her clothes off in front of the customer.",
                    "${name} made occasional eye contact as she coyly took her clothes off in front of the customer, moving around a little so the customer could see better."
            )
        end
    elseif skill < 60 then
        if wm.Percent(60) and ugly then
            return "${name} isn't the most beautiful, but gave a good enough show that the customer didn't notice."
        else
            return RandomChoice("${name} hotly took her clothes off in front of the customer.",
                    "${name} moved around and stripped off her clothes in front of the customer."
            )
        end
    elseif skill < 80 then
        if wm.Percent(60) and girl:is_pregnant() then
            return  "${name} stripped off incredibly well, despite the bulge in her belly."
        elseif wm.Percent(40) and girl:refinement() < 35 then
            return  "${name} clearly enjoyed taking everything off in front of the seated customer, and masturbated right in front of his face."
        else
            return RandomChoice("${name} proudly took her clothes off in front of the customer.",
                    "${name} sexily danced around the customer stripping off her clothes."
            )
        end
    else
        if wm.Percent(60) and ugly then
            return "${name} gave a such an energetic, joyous and sensual strip-show that the customer didn't even notice her plain face."
        elseif wm.Percent(40) and girl:refinement() < 35 then
            return "${name} was clearly aroused stripping everything off right in front of the seated customer" ..
                    RandomChoice(", fisting herself to orgasm inches from his nose.",
                            " before bending over and fingering both holes right in front of his face.",
                            " and stuffing her tiny, cotton panties up her vagina, before inviting the customer to pull back them out.",
                            ". She then took a few steps back, spread her legs, and pissed on her hands - afterwards licking and sucking her fingers in front of the astonished customer."
                    )
        else
            return RandomChoice(
                    "${name} joyously took her clothes off in front of the customer.",
                    "${name} sensuously prowled around the customer stripping off her clothes, while caressing herself, always making sure the customer had the best possible view."
            )
        end
    end
end
