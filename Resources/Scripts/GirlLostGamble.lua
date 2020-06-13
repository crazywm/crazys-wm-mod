---@param girl wm.Girl
function LostRivalGamble(girl)
    Dialog(girl:name() .. " didn't have enough to pay her gambling debt to your rival so they brought her to you to see if you would.")
    HandleDecision(girl)
end

---@param girl wm.Girl
function LostOwnGamble(girl)
    Dialog(girl:name() .. " didn't have enough to pay her gambling debt so they brought her to you to decide her fate.")
    HandleDecision(girl)
end

function HandleDecision(girl)
    Dialog(girl:name() .. " didn't have enough to pay her gambling debt to your rival so they brought her to you to see if you would.")
    Dialog("Crying she asks you to pay her debt.")
    local choice = ChoiceBox("", "\"How much does she owe?\"", "\"No way\"")
    if choice == 0 then
        Dialog("Man: \"She owes us 1000 gold.\"")
        local result = ChoiceBox("", "\"Fine I'll pay.\"", "\"You must be joking you can keep her.\"")
        if result == 0 then
            Dialog("Girl: \"Thank you so much.\"")
            -- TODO does this make sense?
            wm.AcquireGirl(girl)
        else
            Dialog("Girl: \"Please don't do this..\"")
        end
    else
        Dialog("Girl: \"Please don't.\"")
    end
end
