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
    local choice = ChoiceBox("What would you like to do with her?", "Torture", "Chat", "Scold", "Ask", "Force")
    if choice == 0 then
        girl:torture()
        wm.UpdateImage(wm.IMG.TORTURE)
    elseif choice == 1 then
        return girl:trigger("girl:chat.dungeon")
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
                girl:experience(1)
                girl:tiredness(1)
                girl:obedience(1)
                girl:pcfear(-1)
                girl:pchate(-1)
                girl:pclove(2)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both have passionate wild sex, and then bask in each others glow.")
                PlayerFucksGirl(girl)
            elseif girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(2)
                girl:libido(1)
                girl:tiredness(2)
                girl:pclove(1)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both enjoy fucking one another.")
                PlayerFucksGirl(girl)
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
                if preg then Dialog(girl:name() .. " has gotten pregnant") end
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
            PlayerFucksGirl(girl)
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
        girl:libido(1)
        girl:confidence(-1)
        girl:obedience(1)
        girl:spirit(-1)
        girl:tiredness(4)
        girl:health(-1)
        girl:pcfear(2)
        girl:pclove(-2)
        girl:pchate(1)
    end
end


function BrothelInteractChoice(girl)
    local choice = ChoiceBox("What would you like to do?", "Reward " .. girl:name(),
            "Chat with " .. girl:name(), "Visit " .. girl:name() .. "'s Bedroom",
            "Call " .. girl:name() .. " to your office", "Invite " .. girl:name() .. " to your private chambers",
            "Train " .. girl:name(), "Scold " .. girl:name()
    )

    if choice == 0 then
        Dialog("You give " .. girl:name() .. " some spending money to cheer her up.")
        girl:give_money(100)
        girl:happiness(2)
        girl:pclove(1)
        girl:pchate(-1)
    elseif choice == 1 then
        return girl:trigger("girl:chat.brothel")
    elseif choice == 2 then
         return girl:trigger("girl:interact.bedroom")
    elseif choice == 3  then -- Office
        return girl:trigger("girl:interact.office")
    elseif choice == 4 then
        Dialog("\"Hello My Dear, I wanted to ask you to come by my private quarters this evening.  Perhaps, we can get to know each other better.\" ")
        if girl:obey_check(wm.ACTIONS.SEX) then
            return girl:trigger("girl:interact.private")
        else
            Dialog("She declines your invitation.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        return girl:trigger("girl:training")
    else
        ScoldGirl(girl)
    end
end

function Refuse(girl)
    local choice = ChoiceBox("", "Allow Her to Refuse.", "Scold Her", "Spank Her", "Take Her Clothing.",
        "Force Sex")
    if choice == 0 then
        Dialog("You say nothing and go about your other business.")
        wm.SetPlayerDisposition(3)
    elseif choice == 1 then
        ScoldGirl(girl)
    elseif choice == 2 then
        Dialog("\"You will learn to obey me!\" You yell as you grab her arm and drag her across your knees.")
        Dialog("She begins to cry as you pull her clothing and expose her ass.  \"Perhaps this will teach you some discipline.\" ")
        Dialog("You smack her ass until her cheeks are rosy red and send her away sobbing.")
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:health(-1)
        girl:obedience(3)
    elseif choice == 3 then
        Dialog("\"Oh I see.  You feel you have no need to obey me?\"  You ask calmly.  \"Perhaps then you also have no need for the things I have given you?\" ")
        Dialog("\"I'll just be taking a few things back then.\"  You order your guards to strip her naked and make her stand in front of the brothel all day and night.")
        Dialog("As she is lead outside you remark. \"Perhaps next time you will be more mindful of who it is that takes care of you.\"")
        wm.UpdateImage(wm.IMG.NUDE)
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:pchate(5)
        girl:pclove(-5)
        girl:obedience(5)
    elseif choice == 4 then
        Dialog("Your eyes flash with rage. \"You dare refuse? I'll show you what happens to whores that refuse to do their master's bidding\"")
        Dialog("You knock her down and begin to tear away her clothing.  She cries out as you force yourself inside her.")
        Dialog("You release you semen deep inside her and leave her sobbing on the floor.")
        PlayerRapeGirl(girl)
        girl:happiness(-5)
        girl:obedience(5)
        girl:pchate(10)
        girl:pclove(-10)
        girl:pcfear(10)
    end
end

---@param girl wm.Girl
function Punish(girl)
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
    elseif choice == 1 then
        PlayerFucksGirl_Group(girl)
        wm.SetPlayerDisposition(-40)
        Dialog("You and your men spend a few hours passing her around the room.")
        Dialog("For the grand finale you all stand around her and spray her with load after load of cum.")
        if girl:has_trait("Nymphomaniac") then
            Dialog("She lies on the floor breathing heavily from the marathon of orgasms; both the group's and her's.")
            girl:libido(5)
            girl:happiness(2)
            girl:tiredness(10)
        else
            Dialog("She lies gasping and gagging on the floor humiliated and sticky from sweat and semen.")
            girl:happiness(-30)
            girl:tiredness(20)
            girl:pcfear(5)
        end
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
    elseif choice == 3 then
        wm.UpdateImage(wm.IMG.BEST)
        Dialog("Player: \"Let's see if a night with Cthulu's cousin improves her mood?\"")
        Dialog("Your men pick the girl up from the floor and haul her off to the beast pit.")
        Dialog("beast sex dialog")
        girl:happiness(-20)
        girl:tiredness(20)
        girl:pcfear(10)
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

