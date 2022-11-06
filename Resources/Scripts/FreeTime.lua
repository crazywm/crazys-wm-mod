---@param girl wm.Girl
function FreeTimeBath(girl)
    local text = "${name} took a bath.\n"
    girl:tiredness(85)
    girl:add_trait(wm.TRAITS.VIRAS_BLOOD_ADDICT)

    if girl:pclove() >= 80 and wm.Percent(10) then  -- loves you
        text = text .. "She invites you to join her in the tub.\n";  -- will get around to adding sex options later
    elseif girl:libido() > 70 or (girl:has_trait(wm.TRAITS.NYMPHOMANIAC) and girl:libido() > 30) then
        text = text .. "While in the tub the mood hit her and she proceed to pleasure herself with "
        if girl:has_item("Compelling Dildo") then
            text = text .. "her Compelling Dildo helping her get off much easier.\n"
            girl:libido(-10)
        else
            text = text .. "her fingers"
        end
        girl:libido(-15)
        girl:happiness(5)
        -- TODO imagetype = EImagePresets::MASTURBATE;
    elseif girl:tiredness() > 80 then
        if wm.Percent(100) and girl:tiredness() > 95 and not girl:has_trait(wm.TRAITS.INCORPOREAL, wm.TRAITS.UNDEAD, wm.TRAITS.ZOMBIE, wm.TRAITS.SKELETON) then
            -- 'MUTE' this is meant to kill the girl
            if girl:is_addict() then
                text = text .. "${name} took an overdose of drugs and drowned in the tub.\n"
            else
                text = text .. "${name} fell asleep in the tub and no one came to check on her so she drowned.\n"
            end
            girl:health(-500)
        else
            text = text .. "She fell asleep in the tub and woke up in the cold water.\n"
            girl:health(-2)
        end
    elseif wm.Percent(5) and girl:has_trait(wm.TRAITS.ZOMBIE, wm.TRAITS.SKELETON) then
        text = text .. "Someone knocks on the door, \"Are you cooking in there? something smells good.\" " ..
                "  \"What? No, I'm taking a bath.\"   \"Oh, Sorry. Wait, What? EWwwwwwwwwwwwwwwwwwwwww.\""
    else
        text = text .. "She enjoyed a nice long soak.\n"
        girl:tiredness(-5)
    end
    return text
end