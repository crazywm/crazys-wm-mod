---@param girl wm.Girl
function MeetClinic(girl)
    Dialog("You go to your clinic in the hopes that your men have brought in a potential new girl as per your orders.")
    Dialog("Looking around for someone of use that you could perhaps convince to work for you.  You notice a girl in the ER room after checking her chart you notice she was brought in unconnicess.")
    local choice = ChoiceBox("", "\"How did you end up here?\"",  "\"I've changed my mind\"")
    if choice == 0 then
        Dialog("Girl: \"I don't know and what's worse I have no way of paying my bill.\"")
        local hire = ChoiceBox("", "\"I'm the owner of this place.  How about you come work for me and we call the bill even?\"",
                "\"I must be off, bye\"")
        if hire == 0 then
            Dialog("Girl: \"Ok, that would really be helpful.\"")
            wm.AcquireGirl(girl)
        else
            Dialog("Girl: \"Ok, bye.\"")
        end
    end
end