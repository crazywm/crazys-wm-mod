---@param girl wm.Girl
function MeetTown(girl)
    Dialog("You go out searching around town for any new girls. You notice a potential new girl and walk up to her.")
    Dialog("Girl: \"Excuse me, sir.  Could you spare a few gold?  I haven't eaten in days...\"")
    local c = ChoiceBox("", "\"You seem to be down on your luck.\"",
               "\"I must be off, bye\"")
    if c == 0 then
        Dialog("Girl: \"Ever since I arrived here I've been sleeping on the street and have barely eaten.  I don't even know where 'here' is...\"")
        local c = ChoiceBox("", "\"If you come work for me I can offer you a place to stay and a job to earn yourself some coin..\"",
                   "\"I must be off, bye\"")
        if c == 0 then
            Dialog("Girl: \"Really?  That would be wonderful!  I was getting absolutely desperate.\"")
            wm.AcquireGirl(girl)
        else
            Dialog("Girl: \"Ok, bye.\"")
        end
    else
        Dialog("Girl: \"Ok, bye.\"")
    end
end
