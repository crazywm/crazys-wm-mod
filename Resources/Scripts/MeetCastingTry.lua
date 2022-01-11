function MeetCasting(girl)
    Dialog("You hold an open casting call to try to get a new actress for your movies.")
    Dialog("Your audition call is packed with hopeful women. You scan for someone attractive enough to suit your needs. One girl catches your eye.")
    local choice = ChoiceBox("",
            "\"So you want to work in one of my films?\"",
            "\"So what makes you think you are a fit for our upcoming movie?\""
    )
    if choice == 0 then
        local response = wm.Range(1, 3)
        if response == 0 then
            Dialog("Girl: \"Yes, sir. I.. don't actually know how I got here. I need a way to make some money and saw your casting call.\"")
        elseif response == 1 then
            Dialog("Her eyes brim with tears. \"Ma and Pa were killed during a gang fight. I have to make my own way in the world now, and have nothing to market but... myself.\"")
        else
            Dialog("Girl: \"This sounds like fun. I've always wanted to be a movie star.\"")
        end
        choice = ChoiceBox("",
                "You know my movies involve nudity and explicit sexual activity?",
                "I'm sure we can find you a... big part in our upcoming movie."
        )
        if choice == 0 then
            if response == 3 then
                Dialog("Girl: \"Wait, you mean, like... porn? No thanks, mister!  What kind of girl do you think I am?\"")
                Dialog("I guess she thought it was all fun and games until someone gets cum in their eye.")
                return
            else
                Dialog("Girl: \"I've been told and it's fine,  I really need the money.\"")
                wm.AcquireGirl(girl)
            end
        else
            if response == 3 then
                Dialog("Girl: \"I'm glad you saw my talent! I'm looking forward to learning my lines. Do you have a script?\"")
                Dialog("You roll your eyes at her naivete. \"Sure, honey, just sign this contract and we'll make you a star.\"")
                wm.AcquireGirl(girl)
                Dialog("You suspect when she really finds out what she's signed up for, she's not going to like you very much.")
                girl:happiness(-50)
                girl.pclove(-80)
                wm.SetPlayerDisposition(-25)
                return
            else
                Dialog("Girl: \"My situation is not a joke, sir. If you are going to be this disrespectful, I don't think I can work for you.\"")
                Dialog("You mentally kick yourself. If you had been more polite, you might have a new girl working for you.")
            end
        end
    else
        Dialog("Girl: \"Sir, I can assure you, fit is no problem.\"")
        Dialog("Girl: \"In fact, I think anything you want to fit anywhere is fine by me.\"")
        choice = ChoiceBox("",
                "OK, you've convinced me. Strip off and report to work.",
                "Your attitude is commendable - you're hired!! I don't suppose you have a sister looking for work?"
        )
        if choice == 0 then
            Dialog("The girl gives you a bonecrushing hug and deliberately grinds her crotch into you. \"You won't regret this, sir!\"")
            wm.AcquireGirl(girl)
            girl:pclove(25)
        else
            Dialog("Girl: \"Oh, pooh.  I thought you were interested in just me.\"")
            wm.AcquireGirl(girl)
            girl:pclove(-50)
            if wm.Percent(50) then
                Dialog("Girl: \"I don't have sister, but I can tell you where to find a girl who would be easy to... recruit.\"")
                choice = ChoiceBox("",
                        "\"Woah, what kind of guy do you think I am? I only hire willing girls.\"",
                        "\"I'm guessing you want to even a score on a rival. OK by me, the audience won't care if the girl is willing or enslaved.\"")
                if choice == 0 then
                    Dialog("Girl: \"Oh, I thought you were a great big evil crime boss.\"")
                    Dialog("Girl: \"I'm glad you're not, though. I'll feel safer at night.\"")
                    Dialog("She musses your hair, strips saucily, and heads over to makeup.")
                    girl:pcfear(-25)
                    wm.SetPlayerDisposition(15)
                else
                    girl:pcfear(25)
                    Dialog("You send some men to the address she gives you.")
                    Dialog("Your men return with an angry girl, who curses you and spits on your face.")
                    Dialog("Your men know you well enough - they drag her to the dungeon before you even have to say anything.")
                    Dialog("Wiping spittle off your cheek, you yell after them: \"Throw a slave bracelet on that bitch. I'll attend to her shortly.\"")
                    local age = wm.Range(18, 22)
                    local othergirl = wm.CreateRandomGirl(wm.SPAWN.KIDNAPPED, age)
                    -- TODO Set slave status
                    -- TODO DUNGEON_GIRLKIDNAPPED
                    wm.ToDungeon(othergirl, 3)
                end
            end
        end
    end
end

