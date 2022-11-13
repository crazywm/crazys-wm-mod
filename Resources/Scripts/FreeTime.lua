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
        result:add_text("and decide to get a \"special\" message.")
        local sex_type = wm.Range(0, 3)
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
            result:add_text("She paid the woman masseuse to intensely lick her clit until she got off.")
            result:set_image(wm.IMG.LESBIAN)
        elseif sex_type == 1 then
            result:add_text("She had the masseuse oil up her ass and fuck her.")
            result:set_image(wm.IMG.ANAL)
        elseif sex_type == 2 then
            result:add_text("She told the masseuse to fuck her silly.")
            result:set_image(wm.IMG.SEX)
        else
            result:add_text("She told the masseuse to just focus on her pussy until she cums.")
            result:set_image(wm.IMG.FINGER)
        end
        SheJustCame(girl, 5)
        girl:take_money(100)
    elseif girl:money() >= 35 and girl:tiredness() > 60 then
        result:add_text("and decide to get a message. She is now feeling relaxed.")
        girl:take_money(35)
        girl:tiredness(-10)
        result:set_image(wm.IMG.MASSAGE)
    elseif girl:money() >= 25 then
        result:add_text("and had her nails and hair done. She is going look better for a few days.")
        girl:take_money(25)
        girl:stat(wm.STATS.BEAUTY, 8, true)
    elseif girl:money() >= 20 then
        result:add_text("and worked on getting a tan. She is going look better for a few days.")
        girl:take_money(20)
        girl:stat(wm.STATS.BEAUTY, 6, true)
    elseif girl:money() >= 15 then
        result:add_text("and had her hair done. She is going look better for a few days.")
        girl:take_money(15)
        girl:stat(wm.STATS.BEAUTY, 4, true)
    else
        result:add_text("and had her nails done. She is going look better for a few days.")
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


---@param girl wm.Girl
---@param result wm.EventResult
function Concert(girl, result)
    local genre_id = wm.Range(0, 7)
    local invite = false
    local enjoy = wm.Range(0, 25)
    -- if she has more money, she buys more expensive tickets
    if girl:money() > 100 then
        girl:take_money(50)
        enjoy = enjoy + wm.Range(0, 25)
    else
        girl:take_money(25)
    end

    result:add_text("${name} decides to go to a concert.")

    -- to get this added ill just try and get some kind of text in and events and trait based stuff can be added later CRAZY
    if genre_id == 0 then
        result:add_text("They were performing Rap music.\n")
        result:add_text("${name} listened as the rappers sung for a few hours.")
    elseif genre_id == 1 then
        result:add_text("They were playing Classical music.\n")
        if girl:has_trait(wm.TRAITS.ELEGANT) then
            result:add_text("${name} seems to really enjoy this type of music.")
            girl:happiness(5)
            enjoy = 96
        else
            result:add_text("${name} listened as the orchestra played for a few hours.")
        end
    elseif genre_id == 2 then
        result:add_text("They were playing Metal music.\n")
        if girl:has_trait(wm.TRAITS.AGGRESSIVE, wm.TRAITS.BRAWLER, wm.TRAITS.TOUGH) then
            result:add_text("${name} likes this type of music. It gets her blood pumping.")
            girl:happiness(5)
            girl:tiredness(3)
            enjoy = 96
        else
            result:add_text("${name} listened as the band played for a few hours.")
        end
    elseif genre_id == 3 then
        result:add_text("They were playing Rock music.\n")
        result:add_text("${name} listened as the band played for a few hours.")
    elseif genre_id == 4 then
        result:add_text("They were playing Country music.\n")
        if girl:has_trait(wm.TRAITS.FARMERS_DAUGHTER, wm.TRAITS.COUNTRY_GAL) then
            result:add_text("${name} loves this type of music as she grew up listen to it.\n")
            girl:happiness(5)
            enjoy = 96
        else
            result:add_text("${name} listened as the band played for a few hours.")
        end
    elseif genre_id == 5 then
        result:add_text("They were playing Death Metal.\n")
        if girl:has_trait(wm.TRAITS.AGGRESSIVE, wm.TRAITS.BRAWLER, wm.TRAITS.TOUGH,
                wm.TRAITS.DEMON, wm.TRAITS.DEMON_POSSESSED) then
            result:add_text("${name} loves this type of music. It gets her blood pumping!")
            girl:happiness(5)
            enjoy = 96
            girl:tiredness(5)
        elseif girl:has_trait(wm.TRAITS.MEEK, wm.TRAITS.FRAGILE) then
            result:add_text("${name} finds this music too angry to enjoy.")
            girl:happiness(-2)
            enjoy = 4
        else
            girl:tiredness(3)
            result:add_text("${name} listened as the band played for a few hours.")
        end
    elseif genre_id == 6 then
        result:add_text("They were playing Pop music.\n")
        if girl:has_trait(wm.TRAITS.IDOL) then
            result:add_text("The crowd keep chanting ${name} wanting her to take the stage and sing for them.\n")
            girl:happiness(5)
            enjoy = 96 -- could add a way for her to make gold off this, and need to add if she takes the stage or not
            if wm.Percent(50) and not girl:has_trait(wm.TRAITS.SHY) then
                result:add_text("She agrees and took to the stage putting on a show for the crowd." ..
                        "They threw some gold on stage for her performance.")
                -- TODO I think give_money subtracts from player, check/fix this!
                girl:give_money(wm.Range(33, 90))
            else
                result:add_text("She declines, not wanting to sing tonight. She is just here to enjoy the show.")
            end
        elseif girl:has_trait(wm.TRAITS.AGGRESSIVE, wm.TRAITS.BRAWLER, wm.TRAITS.DEMON_POSSESSED, wm.TRAITS.DEMON) then
            result:add_text("${name} hates this kind of music. She thinks its for pussy's.")
            girl:happiness(-2)
            enjoy = 4
        else
            result:add_text("${name} listened as the band sung for a few hours.\n")
        end
    end

    if girl:tiredness() > 66 then
        enjoy = enjoy - 15
        if wm.Percent(25 + girl:tiredness() / 4) then
            result:add_para("She was so tired that she fell asleep during the show.")
            girl:tiredness(-5)
            return
        else
            result:add_para("She might have enjoyed the concert more, had she not been so tired.")
        end
    end

    -- random things that can happen at any show type
    if girl:has_trait(wm.TRAITS.EXHIBITIONIST) and wm.Percent(30) then
        result:add_text("Before the show was over ${name} had thrown all her clothes on stage and was now walking around naked.\n")
        result:set_image(wm.IMG.NUDE)
        if girl:beauty() > 50 then
            invite = true
        end
    end

    if girl:is_addict() and wm.Percent(20) then
        result:add_text("Noticing her addiction, someone offered her some drugs. She accepted, and got baked for the concert.\n")
        -- I've removed adding the drugs to the inventory: It makes the script much more complicated, and she is supposed to consume them
        -- on the spot anyway
        if wm.Percent(10) and girl:beauty() > 85 and not girl:is_virgin() then
            result:add_para("After noticing her great beauty and the fact that she is baked, " ..
                    "a group of guys take her off alone somewhere and have their way with her.")
            result:set_image(wm.IMG.GANGBANG)
        end
    end

    if invite and wm.Percent(50) then
        enjoy = enjoy + 15
    end

    -- did she enjoy it or not?
    if enjoy <= 5 then
        result:add_para("${firstname} thought the concert was crap.")
        girl:happiness(-1)
    elseif enjoy >= 95 then
        result:add_para("${firstname} thought the concert was amazing. She had a really great time.")
        girl:happiness(5)
    else
        result:add_para("${firstname} enjoyed herself. The concert wasn't the best she ever been to, but she had a good time.")
        girl:happiness(3)
    end

    if invite then
        result:add_para("Having seen her walking around naked, the band invites her to come backstage and meet them.")
    elseif girl:beauty() > 85 and wm.Percent(girl:beauty() - 50) then
        result:add_para("Having seen her amazing beauty the band invites her to come backstage and meet them.")
    else
        return
    end

    if enjoy < 5 then
        result:add_para("${name} declined as she thought they sucked.")
        return
    elseif enjoy < 50 then
        result:add_para("${name} told them she had a good time but had to be going.")
        return
    elseif enjoy > 90 then
        result:add_para("${name} accepted with great joy.")
        girl:happiness(5)
        -- add anything from them trying to have sex with her to just talking
        if wm.Percent(30) and not girl:is_virgin() then
            result:add_para("After talking for a while, they asked if she wanted to have sex with them.")
            Concert_FuckBand(girl, result, enjoy)
        else
            result:add_para("They talked for a few hours about many things. " ..
                    "She left later then she normally would have, very happy with the show.")
        end
    else
        result:add_para("${name} enjoyed herself so she accepted.")
        -- add anything from them trying to have sex with her to just talking
        if wm.Percent(20) and not girl:is_virgin() then
            result:add_para("After talking for a while, they asked if she wanted to have sex with them.")
            Concert_FuckBand(girl, result, enjoy)
        else
            result:add_para("They talked for a few hours about many things. She left later then she normally would have, happy with the show.")
        end
    end
end

function Concert_FuckBand(girl, result, enjoy)
    local min_libido = 100 - enjoy / 2


    if girl:libido() > min_libido then
        if enjoy > 90 then
            result:add_text("As she was in the mood and loved the show, she agreed and spent many hours pleasing the ")
        else
            result:add_text("As she was in the mood and enjoyed the show, she agreed to have sex with the ")
        end
        if girl:dislikes_men() then
            result:add_text("only female member of the band.")
            result:set_image(wm.IMG.LESBIAN)
            girl:lesbian(1)
        else
            if wm.Percent(20 + enjoy / 3) then
                result:set_image(wm.IMG.GANGBANG)
                result:add_text("$[whole group|band].")
                girl:group(1)
                SheJustCame(girl, 15)
            else
                result:add_text("$[lead singer|lead guitarist|drummer|bass player|rhythm guitarist].")
                result:set_image(wm.IMG.SEX)
                girl:normalsex(1)
                SheJustCame(girl, 10)
            end

        end
    else
        result:add_para("Not in the mood she declined and returned home.")
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function WorkOut(girl, result)
    local workout = 0
    local kind = ""
    local quality = wm.Range(0, 100)
    result:add_text("${name} decided to workout today. She")
    local type = wm.Range(1, 10)
    -- add different types of workouts.. the type she does will affect the stat gain and maybe give a trait gain
    if type == 0 then
        result:add_text("did crunches working on her abs")
        kind = "str"
    elseif type == 1 then
        result:add_text("did squats working on her ass")
        kind = "ass"
    elseif type == 2 then
        result:add_text("did push ups working out her chest")
        kind = "str"
    elseif type == 3 then
        result:add_text("went for a run")
        kind = "job"
    elseif type == 4 then
        result:add_text("did some pull ups working her biceps")
        kind = "str"
    else
        result:add_text("did some yoga, working on her flexibility")
        kind = "flex"
    end

    if girl:has_item("Free Weights") and kind == "str" then
        result:add_text("and with the help of her Free Weights she got a better workout.")
        workout = workout + 2
    elseif girl:has_trait(wm.TRAITS.CLUMSY) and wm.Percent(50) then
        result:add_text(" but in her Clumsiness somehow she ended up hurting herself.")
        workout = workout - 2
        quality = 4
    else
        result:add_text(".")
    end

    if quality < 10 then
        result:add_para("Her workout went really poorly.")
        workout = workout - 2
    elseif quality > 90 then
        result:add_para("Her workout went really great.")
        workout = workout + 2
    else
        result:add_para("Her workout was nothing special.")
        workout = workout + 1
    end

    if workout >= 2 then
        if kind == "jog" and wm.Percent(5) and girl:add_trait(wm.TRAITS.GREAT_FIGURE) then
            result:add_para("With the help of her workouts she has got quite a Great Figure now.")
        elseif kind == "ass" and wm.Percent(5) and girl:add_trait(wm.TRAITS.GREAT_ARSE) then
            result:add_para("With the help of crunches her ass has become a sight to behold.")
        elseif kind == "str" and wm.Percent(5) and girl:add_trait(wm.TRAITS.STRONG) then
            result:add_para("With the help of her workout she has become Strong.")
        elseif kind == "flex" and wm.Percent(10) and girl:add_trait(wm.TRAITS.FLEXIBLE) then
            result:add_para("With the help of yoga she has become quite Flexible.")

        end
    end

    if workout < 0 then
        workout = 0
    end
    result:set_image(wm.IMG.SPORT)
    if kind == "str" then
        girl:strength(wm.Range(0, workout))
    elseif kind == "jog" then
        girl:constitution(wm.Range(0, workout))
    elseif kind == "flex" then
        girl:agility(wm.Range(0, workout))
    end
end


---@param girl wm.Girl
---@param result wm.EventResult
function Hobby(girl, result)
    result:add_text("${name} decided to do something she really enjoys so she ")
    if girl:has_trait(wm.TRAITS.NYMPHOMANIAC) and girl:libido() > 80 and not girl:is_virgin() then
        result:add_text("went out looking to get laid.")
        result:next_para()
        if wm.Percent(35) then -- finds someone
            if (girl:likes_women() and wm.Percent(50)) or girl:dislikes_men() then  -- FIXME not sure this will work CRAZY
                result:add_para("She goes out and finds herself a woman that she likes enough. They go back to her place and have sex.")  -- FIXME needs better text and more varations CRAZY
                result:set_image(wm.IMG.LESBIAN)
                girl:lesbian(1)
                SheJustCame(girl, 5)
            else
                result:add_para("She goes out and finds herself a man that she likes enough. They go back to her place and have sex.")  -- FIXME needs better text and more varations CRAZY
                result:set_image(wm.IMG.SEX)
                girl:normalsex(1)
                SheJustCame(girl, 5)
            end
        else
            result:add_para("She couldn't find anyone she wanted to have sex with.")
        end
    elseif girl:has_trait(wm.TRAITS.NERD) then
        if wm.Percent(50) then
            result:add_text("stays home and plays some video games.")
        else
            result:add_text("stayed inside and read a book.")
            result:set_image(wm.IMG.STUDY)
        end
    elseif girl:is_slave() then
        if wm.Percent(75) then
            result:add_text("practiced her combat skills.")
            result:set_image(wm.IMG.COMBAT)
            girl:combat(1)
        else
            result:add_text("decided to go out and look for a fight.")  -- FIXME need to add more to this
        end
    elseif girl:has_trait(wm.TRAITS.ACTRESS) then
        result:add_text("practiced her acting skills.")
        girl:performance(1)
    elseif girl:has_trait(wm.TRAITS.HEROINE) then
        result:add_text("went looking for crime to stop.")
        if wm.Percent(50) then
            result:add_para("She found and put a stop some low level crimes.")  -- FIXME this could be expaned to have much more
            result:set_image(wm.IMG.COMBAT)
            girl:combat(1)
            girl:fame(1)
        else
            result:add_para("But she didn't find anything to help with.")
        end
    elseif girl:has_trait(wm.TRAITS.IDOL) then
        result:add_text("went out to meet and talk with her fans.")
    elseif girl:has_trait(wm.TRAITS.ADVENTURER) then
        result:add_text("went out looking for some adventure.")
    else
        result:add_text("spent the day doing various things she enjoys.")
    end
end

---@param girl wm.Girl
---@param result wm.EventResult
function Movie(girl, result)
    result:add_para("${name} decides to go watch a movie.")
    -- May add different ways for the girl to get into the movie CRAZY
    if wm.Percent(20) and girl:oralsex() >= 50 then
        result:add_para("Instead of paying for her ticket she slides under the ticket booth and sucks off the guy selling the tickets to get in for free.")
        result:set_image(wm.IMG.BLOWJOB)
        girl:oralsex(1)
    else
        result:add_para("She buys her ticket and goes in.")
        girl:take_money(10)
    end

    local mov_type = wm.Range(0, 5)
    local enjoy = wm.Range(0, 100)
    if mov_type == 0 then  -- romance
        result:add_para("They were playing a romance movie.")
        if girl:has_trait(wm.TRAITS.PESSIMIST) then
            if girl:pclove() >= 80 then
                result:add_para("Even though ${name} loves you greatly, this kind of movie always make her Pessimistic nature show up." ..
                        " She thinks the two of you will never get a happy ending like in this movie.")
            else
                result:add_para("Being the Pessimist she, is she hates sappy love movies as she don't believe she will ever find her true love.")
            end
            enjoy = 4
        elseif girl:has_trait(wm.TRAITS.OPTIMIST) then
            if girl:pclove() >= 80 then
                result:add_para("${name} loves you greatly, and her Optimistic nature makes her know that " ..
                        "one day the two of you will have a happy ending just like in this movie.")
            else
                result:add_para("Being the Optimist she is, she loves this kind of movie. She knows one day she will find her true love.")
            end
            enjoy = 96
        end
    elseif mov_type == 1 then
        result:add_para("They were playing a comedy.")
        if girl:has_trait(wm.TRAITS.AGGRESSIVE) then
            result:add_para("${name}'s Aggressive nature makes her wish the movie would have been an action flick.")
            enjoy = 4
        end
    elseif mov_type == 2 then -- horror
        result:add_para("They were playing a $[scary|horror] movie.")
        if girl:has_trait(wm.TRAITS.MEEK) then
            result:add_para("${name} Meekly ran from the theater crying. Seems she shouldn't have watched this kind of movie.")
            enjoy = 4
        end
    elseif mov_type == 3 then -- porno
        result:add_para("They were playing a porno.")
        if girl:has_trait(wm.TRAITS.CHASTE) then
            result:add_para("${name} face turned blood red when the movie got going. She snuck out of the movie and ran home.")
            enjoy = 4
        elseif girl:has_trait(wm.TRAITS.NYMPHOMANIAC) then
            result:add_para("${name} loves everything to do with sex, so this is her type of movie.")
            girl:stat(wm.STATS.LIBIDO, 5, true)
            enjoy = 96
            if girl:libido() >= 70 then
                result:add_text("The movie started to turn her on so she started to pleasure herself.")
                if wm.Percent(20) then
                    result:add_text("A man noticed and approached her, asking if she wanted the real thing instead of her fingers.")
                    result:next_para()
                    if girl:is_virgin() then
                        result:add_para("She informs him she is a Virgin and that she won't be having sex with him.")
                        result:set_image(wm.IMG.MASTURBATE)
                    elseif girl:dislikes_men() then
                        result:add_para("She informs him she is a Lesbian and that she doesn't have sex with guys.")
                        result:set_image(wm.IMG.MASTURBATE)
                    elseif girl:pclove() >= 80 and girl:libido() > 99 then
                        result:add_para("Despite the fact that she is in love with you," ..
                                " she couldn't help herself her lust is to great and she agrees.")
                        result:set_image(wm.IMG.SEX)
                        girl:normalsex(1)
                    elseif girl:pclove() >= 80 and girl:libido() <= 99 then
                        result:add_para("She tells him she is in love and that he can't compare to her love. " ..
                                "She finishes herself off then leaves with a smile on her face.")
                        result:set_image(wm.IMG.MASTURBATE)
                    else
                        result:add_para("She takes him up on the offer.")
                        result:set_image(wm.IMG.SEX)
                        girl:normalsex(1)
                    end
                else
                    result:set_image(wm.IMG.MASTURBATE)
                end
                SheJustCame(girl, 10)
            end
        end
    else
        result:add_para("They were playing an action  movie.")
        if girl:has_trait(wm.TRAITS.AGGRESSIVE, wm.TRAITS.BRAWLER, wm.TRAITS.ADVENTURER) then
            result:add_para("${name}'s loves this type of movie. With all the action it gets her blood pumping.")
            enjoy = 96
        end
    end

    if enjoy <= 10 then
        result:add_para("${firstname} thought the movie was crap.")
        girl:happiness(-3)
    elseif enjoy >= 90 then
        result:add_para("${firstname} thought the movie was amazing. She had a really great time.")
        girl:happiness(5)
    else
        girl:happiness(2)
        result:add_para("${name} enjoyed herself. The movie wasn't the best she ever seen but she had a good time.")
    end

end
