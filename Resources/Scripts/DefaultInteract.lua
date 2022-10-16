---@param girl wm.Girl
function InteractDungeon(girl)
    DungeonInteractChoice(girl)
end

---@param girl wm.Girl
function InteractBrothel(girl)
    BrothelInteractChoice(girl)
end


---@param girl wm.Girl
function DungeonInteractChoice(girl)
    wm.UpdateImage(wm.IMG.JAIL)
    local choice = ChoiceBox("What would you like to do with her?", "Torture", "Chat", "Scold", "Ask", "Force")
    if choice == 0 then
        girl:torture()
        wm.UpdateImage(wm.IMG.TORTURE)
    elseif choice == 1 then
        return girl:trigger("girl:chat:dungeon")
    elseif choice == 2 then
        ScoldGirl(girl)
    elseif choice == 3 then
        local have_sex = "To have sex"
        if girl:pclove() > 90 then
            have_sex = "To make love"
        end

        choice = ChoiceBox("", have_sex, "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            if girl:pclove() > 90 then
                girl:happiness(5)
                girl:libido(1)
                girl:experience(2)
                girl:tiredness(1)
                girl:obedience(1)
                girl:pcfear(-1)
                girl:pclove(3)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both have passionate wild sex, and then bask in each others glow.")
                PlayerFucksGirlUpdate(girl)
            elseif girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(2)
                girl:libido(1)
                girl:tiredness(2)
                girl:pclove(1)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both enjoy fucking one another.")
                PlayerFucksGirlUpdate(girl)
            else
                Dialog("She refuses to sleep with you.")
            end
        elseif choice == 1 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog("She enjoy you watching her while another girl fucks her.")
            else
                Dialog("She refused to have sex with another girl.")
            end
        elseif choice == 2 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BEAST)
                Dialog("She enjoys you watching her being fucked by all sorts of tentacled sex fiends.")
            else
                Dialog("She refused to have sex with creatures.")
            end
        elseif choice == 3 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BDSM)
                Dialog("She allows you to tie her up and spank her while you both cum hard.")
                local preg = girl:calc_player_pregnancy(false, 1.0);
                if preg then Dialog("${firstname} has gotten pregnant") end
            else
                Dialog("She refused to do this.")
            end
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                if girl:obey_check(wm.ACTIONS.SEX) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.DEEPTHROAT)
                    Dialog("She lets you shove your cock deep down the back of her throat until you cum into her head.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            elseif choice == 1 then
                if girl:obey_check(wm.ACTIONS.SEX) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.ORAL)
                    Dialog("She sucks your cock until you cum in her mouth.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.ANAL)
                Dialog("She lets you fuck her in her tight little ass until you both cum.")
                -- ANAL SEX
            else
                Dialog("She refused to let you fuck her ass.")
            end
        elseif choice == 6 then
            -- THREESOME
            return DungeonInteractChoice(girl)      -- tail call
        elseif choice == 7 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                PlayerFucksGirl_Group(girl)
                Dialog("You and a group of your male servants take the poor girl in all way, she was hurt.")
            else
                Dialog("She refuse to be fucked in a gangbang")
            end

        else
            return DungeonInteractChoice(girl)      -- tail call
        end
    elseif choice == 4 then -- FORCE
        choice = ChoiceBox("", "To have sex with you", "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            PlayerRapeGirl(girl)
            Dialog("She stuggles to no avail as you force yourself inside her, you fuck her roughly until you unload yourself into her.")
        elseif choice == 1 then
            wm.UpdateImage(wm.IMG.LESBIAN)
            Dialog("You call a female customer who fuck the poor girl with dildo.")
        elseif choice == 2 then
            wm.UpdateImage(wm.IMG.BEAST)
            Dialog("You forcefully tie her down and let in a tentacle creature followed by several other creatures to have their way with her.")
        elseif choice == 3 then
            wm.UpdateImage(wm.IMG.BDSM)
            Dialog("You hog-tie her and pour hot wax all over her body before spanking her with a paddle.")
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                wm.UpdateImage(wm.IMG.DEEPTHROAT)
                girl:dignity(-2)
                Dialog("You grab her by the back of her head and force your cock into her throat, she gags as you cum into her stomach.")
            elseif choice == 1 then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("You grab her and force her to suck your cock, she struggles but cannot stop you.")
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            wm.UpdateImage(wm.IMG.ANAL)
            Dialog("Although she tries to keep her ass closed you manage to get inside her and proceed to fuck her painfully.")
        elseif choice == 6 then
           -- threesome (placeholder)
            return DungeonInteractChoice(girl)
        elseif choice == 7 then
            wm.UpdateImage(wm.IMG.GROUP)
            Dialog("One of your servant holds his head in his hands while your sperm spreads his face. All your servants are happy to unload on her pretty face")
            girl:dignity(-1)
        else
            return DungeonInteractChoice(girl)
        end
        girl:happiness(-4)
        AdjustLust(girl, 1)
        girl:confidence(-1)
        girl:obedience(1)
        girl:spirit(-1)
        girl:tiredness(4)
        girl:health(-1)
        girl:pcfear(2)
        girl:pclove(-3)
    end
end


function BrothelInteractChoice(girl)
    wm.UpdateImage(wm.IMG.PROFILE)
    local choice = ChoiceBox("What would you like to do?", "Reward ${firstname}" ,
            "Chat with ${firstname}", "Visit ${firstname}'s Bedroom",
            "Call ${firstname} to your office", "Invite ${firstname} to your private chambers",
            "Train ${firstname}", "Scold ${firstname}", "Punish ${firstname}"
    )

    if choice == 0 then
        Dialog("You give ${firstname} some spending money to cheer her up.")
        girl:give_money(100)
        girl:happiness(2)
        girl:pclove(2)
    elseif choice == 1 then
        return girl:trigger("girl:chat:brothel")
    elseif choice == 2 then
         return girl:trigger("girl:interact:bedroom")
    elseif choice == 3  then -- Office
        return girl:trigger("girl:interact:office")
    elseif choice == 4 then
        Dialog("\"Hello My Dear, I wanted to ask you to come by my private quarters this evening.  Perhaps, we can get to know each other better.\" ")
        if girl:obey_check(wm.ACTIONS.SEX) then
            return girl:trigger("girl:interact:private")
        else
            Dialog("She declines your invitation.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        return girl:trigger("girl:training")
    elseif choice == 6 then
        ScoldGirl(girl)
    else
        PunishGirl(girl)
    end
end

function Refuse(girl)
    wm.UpdateImage(wm.IMG.REFUSE)
    local choice = ChoiceBox("", "Allow Her to Refuse.", "Scold Her", "Spank Her", "Take Her Clothing.",
        "Force Sex")
    if choice == 0 then
        Dialog("You say nothing and go about your other business.")
        if girl:has_trait("Recently Scolded") then
            girl:pcfear(-3)
            girl:obedience(-1)
        end
        wm.SetPlayerDisposition(3)
        girl:pcfear(-1)
    elseif choice == 1 then
        ScoldGirl(girl)
    elseif choice == 2 then
        Dialog("\"You will learn to obey me!\" You yell as you grab her arm and drag her across your knees.")
        wm.UpdateImage(wm.IMG.SPANKING)
        Dialog("She begins to cry as you pull her clothing and expose her ass.  \"Perhaps this will teach you some discipline.\" \n" ..
                "You smack her ass until her cheeks are rosy red and send her away sobbing.")
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:health(-1)
        girl:obedience(3)
        girl:pcfear(5)
        girl:add_trait("Recently Punished", 3)
    elseif choice == 3 then
        Dialog("\"Oh I see.  You feel you have no need to obey me?\"  You ask calmly.  \"Perhaps then you also have no need for the things I have given you?\" ")
        Dialog("\"I'll just be taking a few things back then.\"  You order your guards to strip her naked and make her stand in front of the brothel all day and night.")
        Dialog("As she is lead outside you remark. \"Perhaps next time you will be more mindful of who it is that takes care of you.\"")
        wm.UpdateImage(wm.IMG.NUDE)
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:pclove(-10)
        girl:obedience(5)
        girl:add_trait("Recently Punished", 1)
    elseif choice == 4 then
        Dialog("Your eyes flash with rage. \"You dare refuse? I'll show you what happens to whores that refuse to do their master's bidding\"")
        Dialog("You knock her down and begin to tear away her clothing.  She cries out as you force yourself inside her.")
        Dialog("You release you semen deep inside her and leave her sobbing on the floor.")
        PlayerRapeGirl(girl)
        girl:happiness(-5)
        girl:obedience(5)
        girl:pclove(-20)
        girl:pcfear(10)
        wm.SetPlayerDisposition(-3)
        girl:add_trait("Recently Punished", 4)
    end
end


