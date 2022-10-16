function ChoiceBox(question, ...)
    wm.ChoiceBox(question, ...)
    return coroutine.yield()
end

function Dialog(text)
    wm.Dialog(text)
    coroutine.yield()
end


-- Default code for interactions with the player

---@param girl wm.Girl
function PlayerRapeGirl(girl)
    girl:bdsm(2);
    girl:anal(2);
    girl:normalsex(2);

    girl:happiness(-5);
    girl:health(-10);
    girl:libido(-1);
    girl:confidence(-1);
    girl:obedience(2);
    girl:pcfear(2);
    girl:pclove(-5);

    if wm.Percent(2) then
        girl:add_trait("Broken Will")
    end
    if girl:check_virginity() then
        girl:lose_virginity()
    end -- `J` updated for trait/status

    local preg = girl:calc_player_pregnancy(false, 1.0)
    if preg then Dialog(girl:name() .. " has gotten pregnant") end
    -- set image if possible
    wm.UpdateImage(wm.IMG.RAPE)
end

---@param girl wm.Girl
function PlayerFucksGirl(girl)
    if (girl:check_virginity()) then
        girl:lose_virginity();
    end -- `J` updated for trait/status

    local preg = girl:calc_player_pregnancy(false, 1.0);
    if preg then Dialog(girl:name() .. " has gotten pregnant") end

    -- Set image
    wm.UpdateImage(wm.IMG.SEX)
end

---@param girl wm.Girl
--- This function updates virginity and pregnancy. It creates a dialog box if the girl gets pregnant.
function PlayerFucksGirlUpdate(girl)
    if (girl:check_virginity()) then
        girl:lose_virginity();
    end -- `J` updated for trait/status

    local preg = girl:calc_player_pregnancy(false, 1.0);
    if preg then Dialog(girl:name() .. " has gotten pregnant") end
end

---@param girl wm.Girl
function PlayerFucksGirl_BDSM(girl)
    girl:bdsm(2);

    if (girl:check_virginity()) then
        girl:lose_virginity();
    end -- `J` updated for trait/status

    local preg = girl:calc_player_pregnancy(false, 0.75);
    if preg then Dialog(girl:name() .. " has gotten pregnant") end

    -- Set image
    wm.UpdateImage(wm.IMG.BDSM)
end

---@param girl wm.Girl
function PlayerFucksGirl_Group(girl)
    girl:group(2);

    if (girl:check_virginity()) then
        girl:lose_virginity();
    end -- `J` updated for trait/status

    local preg = girl:calc_group_pregnancy(false, 1.0);
    if preg then Dialog(girl:name() .. " has gotten pregnant") end

    -- Set image
    wm.UpdateImage(wm.IMG.GROUP)
end



---@param girl wm.Girl
function ScoldGirl(girl)
    local spirit = girl:spirit()
    -- if she has been scolded recently, the effect is diminished
    if girl:has_trait("Recently Scolded") then
        spirit = spirit + 10
        girl:pclove(-1)
        girl:happiness(-1)
    end
    -- if she has been punished recently, the effect is increased
    if girl:has_trait("Recently Punished") then
        spirit = spirit - 10
        girl:pcfear(1)
        girl:spirit(-1)
    end

    if spirit <= 10 then
        Dialog("She is bawling the entire time you yell at her, obviously wanting to do her best")
        girl:happiness(-5)
        girl:confidence(-5)
        girl:obedience(10)
        girl:spirit(-3)
        girl:pclove(-6)
        girl:pcfear(2)
    elseif spirit <= 20 then
        Dialog("She sobs a lot while you yell at her and fearfully listens to your every word")
        girl:happiness(-2)
        girl:confidence(-2)
        girl:obedience(6)
        girl:spirit(-2)
        girl:pclove(-1)
        girl:pcfear(1)
    elseif  spirit <= 30 then
        Dialog("She listens with attention and promises to do better")
        girl:happiness(-1)
        girl:confidence(-1)
        girl:obedience(5)
        girl:spirit(-2)
        girl:pclove(-1)
    elseif spirit <= 50 then
        Dialog("She listens to what you say but barely pays attention")
        girl:obedience(3)
        girl:spirit(-2)
        girl:pclove(-1)

    elseif spirit <= 80 then
        Dialog("She looks at you defiantly while you yell at her")
        girl:obedience(2)
        girl:spirit(-1)
        girl:pclove(-5)
    else
        Dialog("She stares you down while you yell at her, daring you to hit her")
        girl:obedience(-1)
        girl:spirit(-1)
        girl:pclove(-9)
    end

    girl:add_trait("Recently Scolded", 3)
end

---@param girl wm.Girl
function PunishGirl(girl)
    local choice = ChoiceBox("", "\"Guard the door, I'm going to teach this bitch a lesson!\"",
            "\"Boys, help me teach this whore some manners!\"",
            "\"Hold her! I'm going to beat some sense into her!\"",
            "\"Throw her into the beast pit for a while!\"",
            "\"Nothing.  She's had enough.  Take her back to her room.\""
    )
    if choice == 0 then
        PlayerRapeGirl(girl)
        wm.SetPlayerDisposition(-40)
        Dialog("Now you'll really hate me girl. 'Cuz I'm gonna fuck you, and you won't enjoy it.")
        Dialog("She tries to crawl away but you're quickly on top of her.  You pull out your throbbing cock and thrust it violently into her cunt. ")
        if girl:libido() > 75 then
            Dialog("Despite your words she moans with pleasure as you continually ram her tight pussy.  She shakes with a massive orgasm as you release your semen into her.")
            girl:happiness(30)
        else
            Dialog("You release a huge load into her and leave her sobbing on the floor.")
            girl:happiness(-50)
            girl:pcfear(30)
        end
        girl:add_trait("Recently Punished", 4)
    elseif choice == 1 then
        PlayerFucksGirl_Group(girl)
        wm.SetPlayerDisposition(-40)
        Dialog("You and your men spend a few hours passing her around the room.")
        Dialog("For the grand finale you all stand around her and spray her with load after load of cum.")
        if girl:has_trait("Nymphomaniac") then
            Dialog("She lies on the floor breathing heavily from the marathon of orgasms; both the group's and her's.")
            AdjustLust(girl, 5)
            girl:libido(1)
            girl:happiness(2)
            girl:tiredness(10)
        else
            Dialog("She lies gasping and gagging on the floor humiliated and sticky from sweat and semen.")
            girl:happiness(-30)
            girl:tiredness(20)
            girl:pcfear(5)
        end
        girl:add_trait("Recently Punished", 4)
    elseif choice == 2 then
        -- Flavour text doesn't mention penis-in-vagina penetration,
        -- so don't call PlayerFucksGirl_BDSM()
        wm.UpdateImage(wm.IMG.BDSM)
        wm.SetPlayerDisposition(-40)
        Dialog("Your men seize her arms and drag her to stand before you.")
        if wm.Percent(girl:bdsm()) then
            Dialog("She cries out with every lash from the bullwhip, but each stroke sends a wave of pleasure as well. " ..
                    "Soon her thighs are covered with her hot juices.  Since she seems to be doing well with this you reward " ..
                    "her by ramming the handle of the whip into her pussy.  She  cums so hard that you men almost drop her as her legs give out.")
            girl:health(-10)
            girl:happiness(2)
            girl:tiredness(10)
            girl:obedience(5)
        else
            Dialog("She begs you to stop as you land blow after blow upon her. you are careful to avoid her face (wouldn't want to damage your earnings).")
            Dialog("Your men drag her limp body back to her room and toss her in like a ragdoll.")
            girl:happiness(-60)
            girl:tiredness(30)
            girl:health(-40)
            girl:pcfear(30)
        end
        girl:add_trait("Recently Punished", 4)
    elseif choice == 3 then
        wm.UpdateImage(wm.IMG.BEAST)
        Dialog("Player: \"Let's see if a night with Cthulu's cousin improves her mood?\"")
        Dialog("Your men pick the girl up from the floor and haul her off to the beast pit.")
        Dialog("beast sex dialog")
        girl:happiness(-20)
        girl:tiredness(20)
        girl:pcfear(10)
        girl:add_trait("Recently Punished", 4)
    elseif choice == 4 then
        if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
            wm.UpdateImage(wm.IMG.NUDE)
            Dialog("However, they don't like when anyone disrespects their employer.  Before they leave her room they tear away her clothing and leave her naked and sobbing.")
        else
            Dialog("Your men don't like when someone disrespects their boss.  They tell her that they are going to strip her naked when they get to her room...")
            Dialog("However, as the group rounds the corner to the girl's dormitorys she makes a quick dash for her room and bolts the door behind her.  Your men pound on the door yelling for a bit, but eventually walk away.")
        end
    end
end

function RandomChoice(...)
    local options = {...}
    local choice = wm.Range(1, #options)
    return options[choice]
end

function RandomGirlName()
    return wm.RandName(0)
end

function RandomBoyName()
    return wm.RandName(1)
end

function RandomSurname()
    return wm.RandName(2)
end

---@param girl wm.Girl
function PlayerTitleFor(girl)
    if girl:is_slave() then
        return "Master"
    else
        return "Sir"
    end
end

---@param girl wm.Girl
--- This function adjusts the libido stat in a temporary way. Until we get a separated LUST stat, this is the way
--- to adjust lust levels of the girl, without changing her libido over the long run.
function AdjustLust(girl, amount)
    girl:stat(wm.STATS.LIBIDO, amount, true)
end


---@param girl wm.Girl
--- This function temporarily decreases the libido stat, unless the girl is multi-orgasmic. It also adjusts happiness and tiredness.
function SheJustCame(girl, amount)
    if not girl:has_trait("Nymphomaniac") then
        girl:stat(wm.STATS.LIBIDO, -amount, true)
        girl:tiredness(amount // 2)
    else
        girl:tiredness(amount // 4)
    end
    girl:happiness(amount // 2)
end


local function make_union_add(tab)
    -- https://stackoverflow.com/a/1283399
    local function union ( a, b )
        local result = {}
        for k,v in pairs(a) do result[k] = v end
        for k,v in pairs(b) do result[k] = v end
        return result
    end

    local function table_to_string(tb)
        local result = ""
        for i,v in ipairs(tb) do
            result = result .. i .. "=" .. v .. " "
        end
        return result
    end

    local meta= {__add = union, __tostring=table_to_string}
    setmetatable(tab, meta)
    return tab
end

ImageOptions = {
    SOLO = make_union_add({participants=wm.IMG_PART.SOLO}),
    HETERO = make_union_add({participants=wm.IMG_PART.HETERO}),
    LESBIAN = make_union_add({participants=wm.IMG_PART.LESBIAN}),
    FFM = make_union_add({participants=wm.IMG_PART.FFM}),
    TIED_UP = make_union_add({tied=true}),
}
