function EventNoPay()
    Dialog("Goon: \"Sir this customer says he can't pay.\"")
    PunishCustomerEvent()
end

function EventCheat()
    Dialog("Goon: \"Sir this customer was caught cheating in the gambling hall.\"")
    PunishCustomerEvent()
end

function PunishCustomerEvent()
    local event = wm.Range(0, 3)
    if event > 0 then
        Dialog("Customer: \"Please sir don't hurt me!\nI have a daughter who needs me!\"")
        local choice = ChoiceBox("",  "\"Take his daughter as payment\"",
                "\"Let him go\"", "\"Take him to the dungeon!\"")
        if choice == 0 then
            TakeDaughter(event)
        elseif choice == 1 then
            LetGo()
        else
            ToDungeon()
        end
    else
        Dialog("Customer: \"Please sir don't hurt me!\"")
        local choice = ChoiceBox("", "\"Let him go.\"", "\"Take him to the dungeon!\"")
        if choice == 0 then LetGo() else ToDungeon() end
    end
end

function TakeDaughter(event)
    if wm.Percent(50) then
        Dialog("Customer: \"No!\nYou can't take my daughter!\nI'll never tell you where she is!\"")
        local choice = ChoiceBox("", "\"Very well, you may go\" (tell your men to follow him home)",
                                  "\"Very well, you may go\"",
                                  "\"Take him to the dungeon!\"")
        if choice == 0 then
            Dialog("Customer: \"Thank you sir, thank you.\" (your men follow him soon after...)\nAnd a few hours later your men return...")
            if event > 0 then
                FollowHome(event)
            else
                Dialog("Goon: \"We found where he lives boss, he lives alone.\nWhat would you like us to do boss?\"")
                local choice = ChoiceBox("", "\"Ransack his home, leave nothing.\"",
                                    "\"Leave him be, for now.\"")
                if choice == 0 then
                    Ransack()
                else
                    Dialog("Goon: \"As you say boss.\"")
                end
            end
        elseif choice == 1 then
            LetGo()
        else
            ToDungeon()
        end
    else
        Dialog("Customer: \"Fine...\nI want to live...\njust please take good care of her.\" (he sobs as the coward is dragged away by some of your men)")
        Dialog("An hour later your men return with a young crying girl and drag her to the dungeon.")
        KidnapGirl(12, 14)
    end
end

function LetGo()
    Dialog("Customer: \"Thank you sir, thank you.\"")
    wm.SetPlayerSuspicion(-5)
    wm.SetPlayerDisposition(5)
end

function ToDungeon()
    Dialog("Your men drag the man away sobbing to himself.")
    wm.SetPlayerSuspicion(5)
    wm.SetPlayerDisposition(-5)
    -- TODO DUNGEON_CUSTNOPAY
    wm.AddCustomerToDungeon(1, 0, false)
end

function FollowHome(event)
    if event == 1 then
        Dialog("Goon: \"We found where he lives boss, he has a wife and a daughter.\nWhat would you like us to do boss?\"")
    elseif event == 2 then
        Dialog("Goon: \"We found where he lives boss, he has a wife and 2 daughters.\nWhat would you like us to do boss?\"")
    elseif event == 3 then
        Dialog("Goon: \"We found where he lives boss, he has a wife and 3 daughters.\nWhat would you like us to do boss?\"")
    end
    local choice = ChoiceBox("", "\"Kidnap his wife as payment.\"",
               "\"Kidnap his daughters.\"",
               "\"Kidnap both his wife and his daughters.\"",
               "\"Ransack his home, leave nothing.\"",
               "\"Leave him be, for now.\"")
    if choice == 0 then
        Dialog("Your men leave and return with a crying lady and drag her to the dungeon.")
        KidnapGirl(31, 40)
    elseif choice == 1 then
        if event == 1 then
            Dialog("Your men leave and return with a sobbing young girl.")
            KidnapGirl(12, 14)
        else
            Dialog("Your men leave and return with several crying girls and drag them to the dungeon.")
            wm.SetPlayerSuspicion(10)
            wm.SetPlayerDisposition(-10)
            wm.AddFamilyToDungeon(event, false)
        end
    elseif choice == 2 then
        Dialog("Your men leave and return with some crying women and drag them to the dungeon.")
        wm.SetPlayerSuspicion(15)
        wm.SetPlayerDisposition(-15)
        wm.AddFamilyToDungeon(event, true)
    elseif choice == 3 then
        Ransack()
    else
        Dialog("Goon: \"As you say boss.\"")
    end
end

function Ransack()
    Dialog("Your men leave and return later with some gold")
    wm.AddPlayerGold(wm.Range(100, 300))
    wm.SetPlayerSuspicion(10)
    wm.SetPlayerDisposition(-10)
end

function KidnapGirl(min_age, max_age)
    wm.SetPlayerSuspicion(5)
    wm.SetPlayerDisposition(-5)
    local age = wm.Range(min_age, max_age)
    local girl = wm.CreateRandomGirl(wm.SPAWN.KIDNAPPED, age)
    -- TODO DUNGEON_GIRLKIDNAPPED
    wm.ToDungeon(girl, 3)
end
