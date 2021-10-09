function ChatDungeon(girl)
    local choice = ChoiceBox("", "About general stuff", "About how she feels about her life",
            "About how she feels towards you", "Go Back")
    if choice == 0 then
        Dialog("You both sit and chat about all manner of things")
        girl:happiness(2)
        girl:pcfear(-1)
        girl:pchate(-1)
        girl:pclove(1)
    elseif choice == 1 then
        if girl:happiness() > 50 then
            Dialog("She tells you she is happy with her life.")
        else
            Dialog("She says she is unhappy and would like more free time.")
        end
        girl:happiness(2)
        girl:pcfear(-1)
        girl:pclove(2)
    elseif choice == 2 then
        if girl:pclove() < 30 then
            Dialog("She tells you she has no romantic feelings towards you")
        elseif girl:pclove() < 50 then
            Dialog("She likes the you as a friend but nothing more then that")
        elseif girl:pclove() < 70 then
            Dialog("She tells you she has some strong feelings towards you.")
        else
            Dialog("She tells you she is deeply in love with you and every moment together is like a dream.")
        end

        if girl:pcfear() < 50 then
            Dialog("She then tells you she doesn't find you intimidating.")
        else
            Dialog("She then says you are a little scary to be around.")
        end

        if girl:pchate() < 50 then
            Dialog("She lastly tells you that she doesn't hate anything about you.")
        else
            Dialog("She lastly tells you that despite all her other feelings, she feels some hatred towards you.")
        end
    else
        return girl:trigger("girl:interact:dungeon")
    end
end

function ChatBrothel(girl)
    Dialog("Lets take a moment to talk.")
    local choice = ChoiceBox("Topic?", "Casual Conversation", "Discuss her", "Talk about you", "<Go Back>")
    if choice == 0 then
        Dialog("After some small talk, your conversation turns to several subjects. She seems to enjoy the chance to chat.")
        girl:happiness(wm.Range(0, 4))
        girl:pcfear(wm.Range(-3, 0))
        girl:pclove(wm.Range(0, 2))
        girl:pchate(wm.Range(-3, 0))
    elseif choice == 1 then
        Dialog("How are you feeling my dear? Are you happy? Are you healthy?")
        if girl:happiness() >= 80 then
            Dialog("She giggles a bit and proclaims \"I'm happier than I have ever been\"")
        elseif girl:happiness() >= 50 then
            Dialog("She stifles a sob and tells you all the things that have been going wrong around her.")
        elseif girl:happiness() >= 16 then
            Dialog("She doesn't respond and stares ahead blankly")
        end
        Dialog("As you talk to her you take a few moments to look her over")
        if girl:tiredness() <= 10 then
            Dialog("The girl is so full of energy she is almost bouncing in her seat.")
        elseif girl:tiredness() <= 50 then
            Dialog("Looking closely she doesn't seem either tired or energetic.")
        elseif girl:tiredness() <= 80 then
            Dialog("As you look at her you can see that her duties are starting to take their toll.")
        else
            Dialog("It is clear that she can barely keep her eyes open!")
        end

        if girl:health() > 75 then
            Dialog("She seems to be in good health.")
        elseif girl:health() > 50 then
            Dialog("You can see some scrapes and bruises.")
        elseif girl:health() > 15 then
            Dialog("This girl has several bandages and large bruises all over her body.")
        else
            Dialog("She is covered in bloodied bandages and several open wounds. She will surely die without the services of a cleric.")
        end
    elseif choice == 2 then
        Dialog("How do you feel about me?")
        Dialog("Do I frighten you?")
        if girl:pcfear() > 80 then
            Dialog("She can't even bring herself to look at you and she trembles uncontrollably in her seat.")
        elseif girl:pcfear() > 50 then
            Dialog("Timidly she nods her head.")
        elseif girl:pcfear() > 15 then
            Dialog("She says there isn't anything particularly scary about you.")
        else
            Dialog("She laughs. \"You're about as scary as a bunny rabbit, boss.\"")
        end

        Dialog("Do you hate me?")
        if girl:pclove() > 90 then
            Dialog("She giggles to herself thinking about the question. Stifling a chuckle: \"Well, I do have very strong feelings toward you.\" she pauses...")
            Dialog("\"In truth, I am in love with you.\" she blushes.")
        elseif girl:pclove() > 60 then
            Dialog("She smiles a bit at the question.  \"I don't hate you.  I think you're the perfect gentlemen.\"")
        elseif girl:pchate() > 95 then
            Dialog("She spits at you and screams \"I hope you fucking die with a dick in your ass!\"")
            Dialog("She grabs the nearest object and leaps at you, swinging wildly.  You quickly master your surprise and with a hard backhand you knock her to the ground.")
            Dialog("Your bodyguards rush into the room.  Seeing the girl lying on the floor they ask. \"What do you want us to do with her , boss?\"")
            girl:pchate(wm.Range(2, 5))
            girl:health(wm.Range(-5, -1))
            Punish(girl)
        elseif girl:pchate() > 50 then
            Dialog("She glares at you before she responds. \"You're a sick, perverted asshole.\"")
        elseif girl:pchate() > 30 then
            Dialog("She rolls her eyes, \"No more than the average whore hates her pimp.\"")
        else
            Dialog("She tells you she doesn't hate you and that you have been a good, kind employer")
        end
    elseif choice == 3 then
        return girl:trigger("girl:interact:brothel")
    end
end