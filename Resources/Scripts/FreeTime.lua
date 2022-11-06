-- helper functions

---@param girl wm.Girl
function is_horny(girl)
    return girl:libido() > 70 or (girl:has_trait(wm.TRAITS.NYMPHOMANIAC) and girl:libido() > 30)
end

function mast_tool(girl)
    if girl:has_item("Compelling Dildo") then
        girl:libido(-10)
        return "pleasure herself with her Compelling Dildo helping her get off much easier."
    else
        return "pleasure herself with her fingers."
    end
end

---@param girl wm.Girl
---@param result wm.EventResult
function TakeBath(girl, result)
    result:add_text("${name} took a bath.\n")
    result:set_image(wm.IMG.BATH)

    if girl:pclove() >= 80 and wm.Percent(10) then  -- loves you
        result:add_text("She invites you to join her in the tub.")  -- will get around to adding sex options later
    elseif is_horny(girl) then
        result:add_text("While in the tub the mood hit her and she proceed to " .. mast_tool(girl))
        girl:libido(-15)
        girl:happiness(5)
        result:set_image(wm.IMG.MASTURBATE)
    elseif girl:tiredness() > 80 then
        if wm.Percent(100) and girl:tiredness() > 95 and
                not girl:has_trait(wm.TRAITS.INCORPOREAL, wm.TRAITS.UNDEAD, wm.TRAITS.ZOMBIE, wm.TRAITS.SKELETON) then
            -- 'MUTE' this is meant to kill the girl
            if girl:is_addict() then
                result:add_text("${name} took an overdose of drugs and drowned in the tub.")
            else
                result:add_text("${name} fell asleep in the tub and no one came to check on her so she drowned.")
            end
            girl:health(-500)
            result:set_image(wm.IMG.DEATH)
        else
            result:add_text("She fell asleep in the tub and woke up in the cold water.")
            girl:health(-2)
        end
    elseif wm.Percent(5) and girl:has_trait(wm.TRAITS.ZOMBIE, wm.TRAITS.SKELETON) then
        result:add_text("Someone knocks on the door, \"Are you cooking in there? something smells good.\" " ..
                "  \"What? No, I'm taking a bath.\"   \"Oh, Sorry. Wait, What? EWwwwwwwwwwwwwwwwwwwwww.\"")
    else
        result:add_text("She enjoyed a nice long soak.")
        girl:tiredness(-5)
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function StayInBed(girl, result)
    result:set_image(wm.IMG.BED)
    result:add_text("${name}")
    if girl:has_item("Chrono Bed") and girl:tiredness() > 20 then
        result:add_text("took a nap in her Chrono Bed woke up feeling wonderful.")
        girl:health(50)
        girl:tiredness(-50)
        return
    elseif girl:has_item("Rejuvenation Bed") and girl:tiredness() > 20 then
        result:add_text("took a nap in her Rejuvenation Bed and woke up feeling better.")
        girl:health(25)
        girl:tiredness(-25)
        return
    else
        result:add_text(" stayed in bed most of the day.")
        girl:health(10)
        girl:tiredness(-10)
    end

    if is_horny(girl) then
        result:add_text("While in bed the mood hit her and she proceed to " .. mast_tool(girl))
        girl:libido(-15)
        girl:happiness(5)
        result:set_image(wm.IMG.MASTURBATE)
    end
end

---@param girl wm.Girl
---@param result wm.EventResult
function GoToChurch(girl, result)
    result:set_image(wm.IMG.PRAY)
    result:add_text("${name} had some free time so she went to the local temple.\n")
    -- `J` use U_Morality to increase or decrease Morality
    --   Try to make multiple options that either increase or decrease morality
    --   right now there is not much done with morality so anything that can be added would be good.
    --   for each morality span (<-80, <-60 ... >80) make atleast one of each "add", "no change" and "reduce" option.
    if (girl:morality() >= 80) then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} prayed for a couple of hours just to actively participate in the mass. Afterwards she helped clean up the temple grounds.")
            girl:morality(5)
        elseif choice < 66 then
            result:add_text("She spent almost her entire free time praying! But from her line of work, she got used to being on her knees for long hours.")
        else
            result:add_text("Being at the sanctuary for her whole free time, she could swear that she noticed a presence of some sort of holy being.")
        end
    elseif  girl:morality() >= 60 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} confessed her shameful actions that she did at work, asking for the God's forgiveness.")
            girl:morality(2)
        elseif choice < 66 then
            result:add_text("She humbly sat in the last row. Focused and with a lot of interest, she listened to the whole mass.")
        else
            result:add_text("Today she cleaned up a road side shrine and decorated it with fresh flowers.")
        end
    elseif girl:morality() >= 40 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} starts to find this way of spending free time interesting. Wanting to know more, she listens intently to the preacher.")
        elseif choice < 66 then
            result:add_text("After participating in today's ceremony she felt happier.")
            girl:happiness(5)
        else
            result:add_text("Being late, she tried to find a place to sit. " ..
                    "Happily, she noticed some free seats on the other side of the church. " ..
                    "Unfortunately her high heels were knocking pretty loudly while walking on the church's stone floor, " ..
                    "disturbing the silent prayers of the congregation.")
        end
    elseif girl:morality() >= 20 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} made a quick stop at the local temple for a short prayer before heading further into town.")
        elseif choice < 66 then
            result:add_text("On her way back, she gave a short prayer in front of a road side shrine.")
        else
            result:add_text("After eavesdropping on a couple of girls at work talking about a nearby temple, " ..
                    "she decided to visit this holy place. Listening to the preacher she felt that the girls were right about this place.")
        end
    elseif girl:morality() >= -20 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} was on her way to pray in the local temple, but on the way there, she saw a beautiful bag on display. " ..
                    "After entering the shop, she spent several hours, picking out the perfect bag for herself before returning.")
            result:set_image(wm.IMG.SHOP)
        elseif choice < 66 then
            result:add_text("Attending the mass, she felt somewhat bored and not focused on the topic. She even yawned a couple of times, fighting not to fell asleep.")
        else
            result:add_text("She visited the local church feeling somehow down. Listening to the preacher didn't have much impact on improving her mood.")
        end
    elseif girl:morality() >= -40 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} tried to listen to the preachers lecture, but she fell asleep halfway thru!")
        elseif choice < 66 then
            result:add_text("She was about to enter the sanctuary but she noticed a really cute kitten. She spent the time playing with the cat.")
        else
            result:add_text("After eavesdropping couple girls at work talking about a nearby temple, she decided to visit this holy place. " ..
                    "Listening to the preacher she felt that the girls were wrong about this place. Being bored, she left in the middle of the mass.")
        end
    elseif girl:morality() >= -60 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} threw some special weeds from your farm into a goblet of fire, " ..
                    "standing in front of one of the side altars. Later on, most of praying at the altar swear that they saw angels!")
        elseif choice < 66 then
            result:add_text("At the church, she noticed a really young priest hearing to today's confessions. " ..
                    "Feeling mischievous she entered the confessional. After confessing her sins in great detail, she proposed to recreate them with him!" ..
                    "The abashed youngster gave her a couple of prayers as penance and left right after.")
            result:set_image(wm.IMG.SEX)
        else
            result:add_text("Getting bored at the mass, she started to whisper things to a man sitting next to her, " ..
                    "not bothering with the fact that his wife was sitting next to him! ")
        end
    elseif girl:morality() >= -80 then
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} entered the confessional. Whispering sensually of her job experiences, " ..
                    "all in great detail, she made the priest have a heart attack! " ..
                    "When the man was squirming in pain on the floor, she left the temple laughing.")
            result:set_image(wm.IMG.ECCHI)
        elseif choice < 66 then
            result:add_text("Dressed really slutty, she strode down the center aisle of the church, " ..
                    "her high heals echoing throughout the place. She took a seat in the first row and sat " ..
                    "in a pose to best expose her pussy to all priests on the altar performing their Holy Mass.")
            result:set_image(wm.IMG.ECCHI)
        else
            result:add_text("In front of a temple she approached a young monk. After a brief chat about god, faith and salvation she gave him a proposal. " ..
                    "Claiming that it was such waste for such young and handsome man to live in chastity, she proposed he could spend some quality, fun time with her." ..
                    "The man quickly ran inside whispering some kind of mantra, while ${name} went her own way laughing.")
            result:set_image(wm.IMG.ECCHI)
        end
    else
        local choice = wm.Range(0, 100)
        if choice < 33 then
            result:add_text("${name} came to the temple with one of girls working for you, " ..
                    "but her companion flew right after seeing what kind place of worship this was. " ..
                    "No human sacrifice provided by ${name} today, sorry.")
        elseif choice < 66 then
            result:add_text("\"Scheming against the church, she came with a brilliant idea. " ..
                    "Promising local young thugs to repay a favor with her body, she made them to assault and beat up a group of priests. " ..
                    "Not waiting for the outcome of the brawl, she disappeared not holding up her end of the deal.")
        else
            result:add_text("Not liking the architectural style of the temple she decided to do something about it. " ..
                    "Unfortunately for her, the fire was extinguished fairly quickly.")
        end
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function FreeTimeCooking(girl, result)
    result:set_image(wm.IMG.COOK)
    result:add_text("${name} decided to cook a meal.\nThe meal she cooked was ")
    if girl:cooking() >= 85 then
        result:add_text("amazing. She really knows how to cook.")
        girl:health(2)
        girl:happiness(3)
        girl:enjoyment(wm.ACTIONS.ACTION_WORKCOOKING, 1)
    elseif girl:cooking() >= 50 then
        result:add_text("pretty good. She isn't the best but with some work she could be.")
        girl:health(1)
        girl:happiness(1)
    elseif girl:cooking() > 15 then
        result:add_text("plain. She really needs some practice at this.")
        girl:cooking(1)
    else
        result:add_text("awful. It can't really be called food it was so bad.")
        girl:enjoyment(wm.ACTIONS.COOKING, -1)
        girl:cooking(1)
    end

    if girl:pclove() >= 80 and wm.Percent(10) then
        result:add_text("She invites you to eat with her.")
        result:set_image(wm.IMG.DINNER)
        -- //FIXME add in different things here
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function WindowShopping(girl, result)
    result:set_image(wm.IMG.SHOP)
    local hap = wm.Range(-4, 6)
    girl:happiness(hap)
    result:add_text("${name} wandered around the shops trying things on that she knew she could not afford." ..
            " Though she wasted the shop girl's time, ")
    if hap > 0 then
        result:add_text("she enjoyed herself a ")
        if hap > 3 then
            result:add_text("lot.")
        else
            result:add_text("bit.")
        end
    else
        result:add_text("she was sad because she didn't find what she liked for a price she could afford.")
    end
end

---@param girl wm.Girl
---@param result wm.EventResult
function Salon(girl, result)
    result:add_text("${name} went to the salon")

    -- add more options for more money
    if is_horny(girl) and girl:money() >= 100 and not girl:is_virgin() and (
            girl:is_sex_type_allowed(wm.SKILLS.LESBIAN) or girl:is_sex_type_allowed(wm.SKILLS.ANAL) or
            girl:is_sex_type_allowed(wm.SKILLS.NORMALSEX)
    ) then
        result.add_text("and decide to get a \"special\" message.")
        local sex_type = wm.Range(0, 2)
        if girl:is_sex_type_allowed(wm.SKILLS.LESBIAN) and girl:dislikes_men() then
            sex_type = 0
        end
        if sex_type == 0 and (not girl:is_sex_type_allowed(wm.SKILLS.LESBIAN) or girl:dislikes_women()) then
            sex_type = 1
        end
        if sex_type == 1 and not girl:is_sex_type_allowed(wm.SKILLS.ANAL)  then
            sex_type = 2
        end
        if sex_type == 2 and not girl:is_sex_type_allowed(wm.SKILLS.NORMALSEX)  then
            sex_type = 3
        end

        if sex_type == 0 then
            result.add_text("She paid the woman masseuse to intensely lick her clit until she got off.")
            result.set_image(wm.IMG.LESBIAN)
        elseif sex_type == 1 then
            result.add_text("She had the masseuse oil up her ass and fuck her.")
            result.set_image(wm.IMG.ANAL)
        elseif sex_type == 2 then
            result.add_text("She told the masseuse to fuck her silly.")
            result.set_image(wm.IMG.SEX)
        else
            result.add_text("She told the masseuse to just focus on her pussy until she cums.")
            result.set_image(wm.IMG.FINGER)
        end
        SheJustCame(girl, 5)
        girl:take_money(100)
    elseif girl:money() >= 35 and girl:tiredness() > 60 then
        result.add_text("and decide to get a message. She is now feeling relaxed.")
        girl:take_money(35)
        girl:tiredness(-10)
        result.set_image(wm.IMG.MASSAGE)
    elseif girl:money() >= 25 then
        result.add_text("and had her nails and hair done. She is going look better for a few days.")
        girl:take_money(25)
        girl:stat(wm.STATS.BEAUTY, 8, true)
    elseif girl:money() >= 20 then
        result.add_text("and worked on getting a tan. She is going look better for a few days.")
        girl:take_money(20)
        girl:stat(wm.STATS.BEAUTY, 6, true)
    elseif girl:money() >= 15 then
        result.add_text("and had her hair done. She is going look better for a few days.")
        girl:take_money(15)
        girl:stat(wm.STATS.BEAUTY, 4, true)
    else
        result.add_text("and had her nails done. She is going look better for a few days.")
        girl:take_money(10)
        girl:stat(wm.STATS.BEAUTY, 2, true)
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function Picnic(girl, result)
    result:add_text("${name} decides to go on a picnic.")
    -- FIXME
end