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
    girl:upd_temp_stat(STAT_LIBIDO, 2, true);
    girl:confidence(-1);
    girl:obedience(2);
    girl:pcfear(2);
    girl:pclove(-2);
    girl:pchate(3);

    if wm.Percent(2) then
        girl:add_trait("Broken Will")
    end
    if girl:check_virginity() then
        girl:lose_virginity()
    end -- `J` updated for trait/status

    local preg = girl:calc_player_pregnancy(false, 1.0)
    if preg then Dialog(girl:name() .. " has gotten pregnant") end
    -- set image if possible
    wm.UpdateImage(wm.IMG.SEX)
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
    if girl:spirit() <= 10 then
        Dialog("She is bawling the entire time you yell at her, obviously wanting to do her best", 0);
        girl:happiness(-5);
        girl:confidence(-5);
        girl:obedience(10);
        girl:spirit(-3);
        girl:pclove(-4);
        girl:pcfear(2);
        girl:pchate(2);
    elseif girl:spirit() <= 20 then
        Dialog("She sobs a lot while you yell at her and fearfully listens to your every word", 0);
        girl:happiness(-2);
        girl:confidence(-2);
        girl:obedience(6);
        girl:spirit(-2);
        girl:pclove(-1);
        girl:pcfear(1);
    elseif girl:spirit() <= 30 then
        Dialog("She listens with attention and promises to do better", 0);
        girl:happiness(-1);
        girl:confidence(-1);
        girl:obedience(5);
        girl:spirit(-2);
        girl:pclove(-1);
    elseif girl:spirit() <= 50 then
        Dialog("She listens to what you say but barely pays attention", 0);
        girl:obedience(3);
        girl:spirit(-2);
        girl:pchate(1);

    elseif girl:spirit() <= 80 then
        Dialog("She looks at you defiantly while you yell at her", 0);
        girl:obedience(2);
        girl:spirit(-1);
        girl:pclove(-3);
        girl:pchate(2);
    else
        Dialog("She stares you down while you yell at her, daring you to hit her", 0);
        girl:obedience(-1);
        girl:spirit(-1);
        girl:pclove(-4);
        girl:pchate(5);
    end
end

function RandomChoice(...)
    local options = {...}
    local choice = wm.Range(1, #options)
    return options[choice]
end
