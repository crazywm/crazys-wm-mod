function HandleEvent(girl)
    Dialog("Sobbing Girl: \"P.P.Please don't hurt me.\"")
    local choice = ChoiceBox("", "Rape her", "Comfort her", "Leaver her alone")
    if choice == 0 then
        Dialog("Force yourself onto her and spend the day having your way with her.")
        PlayerRapeGirl(girl)
    elseif choice == 1 then
        Dialog("You sit and talk with her until she accepts her situation a little better.")
        girl:happiness(2)
        girl.obedience(2)
        girl:pcfear(-2)
        girl:pclove(4)
        girl.pchate(-5)
    else

    end
end