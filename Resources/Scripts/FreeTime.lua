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
function FreeTimeBath(girl, result)
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
function FreeTimeBath(girl, result)
    result:set_image(wm.IMG.BED)
    result.add_text("${name}")
    if girl:has_item("Chrono Bed") and girl:tiredness() > 20 then
        result.add_text("took a nap in her Chrono Bed woke up feeling wonderful.")
        girl:health(50)
        girl:tiredness(-50)
        return
    elseif girl:has_item("Rejuvenation Bed") and girl:tiredness() > 20 then
        result.add_text("took a nap in her Rejuvenation Bed and woke up feeling better.")
        girl:health(25)
        girl:tiredness(-25)
        return
    else
        result.add_text(" stayed in bed most of the day.")
        girl:health(10)
        girl:tiredness(-10)
    end

    if is_horny(girl) then
        result:add_text("While in bed the mood hit her and she proceed to " .. mast_tool(girl))
        girl:libido(-15)
        girl.happiness(5)
        result:set_image(wm.IMG.MASTURBATE)
    end
end