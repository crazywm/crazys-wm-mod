function Event()
    Dialog("Goon: \"Sir, one of your rivals has come before us after losing his assets.\"")
    local choice = ChoiceBox("",
            "\"Cast him out on the streets!\"",
            "\"Take him downstairs and kill him.\"",
            "\"Take him out of the city safely.\""
    )

    if choice == 0 then
        Dialog("The man leaves looking sadly at the floor, you know it is only a matter of time before someone else kills him and he will live his remaining days fearful of that time.")
        wm.SetPlayerDisposition(-5)
    elseif choice == 1 then
        Dialog("He tries to escape but your goons drag him kicking and screaming down below where his screams are quickly cut short.")
        wm.SetPlayerDisposition(-20)
    else
        Dialog("The man repeatedly thanks you as your men escort him away.")
        wm.SetPlayerDisposition(10)
    end

    wm.GivePlayerRandomSpecialItem()
end