---@param girl wm.Girl
function Bathing(girl)
    if girl:pclove() > 80 then
        Dialog("\"My dear, why don't you take a relaxing bath?\"")
    else
        Dialog("You look ${name} over: \"I think you need a bath, you look dirty!\" :wink:")
    end

    if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
        Dialog("You lead ${name} to you private chambers, and into your personal bathroom. One of your personal slaves " ..
                "has already drawn a steaming hot bath.")
        local wants_to_please = girl:pclove() > 80 or girl:has_trait("Exhibitionist") or girl:libido() > 75
        if wants_to_please and girl:skill_check(wm.SKILLS.STRIP, 50) then
            wm.UpdateImage(wm.IMG.STRIP)
            Dialog("${name} begins to undress, slowly and sensually, making sure you get a good view.")
        else
            wm.UpdateImage(wm.IMG.NUDE)
            if girl:pcfear() > 50 and wm.Percent(50 + girl:pcfear()) then
                Dialog("You order her to undress. She takes her clothes of mechanically, " ..
                        "until she stands in front of you, naked and shaking with fear.\n With your hand, you indicate the bathtub. ")
                local choice = ChoiceBox("Leave her alone", "Let her bath by herself", "Enjoy the show")
                if choice == 0 then
                    Dialog("Seeing the distress your presence is causing her, you decide to let her take the bath alone.\n" ..
                            "${name} lets out a sigh of relief when you announce your intentions.")
                    girl:pcfear(-3)
                    girl:happiness(3)
                    girl:tiredness(-5)
                    wm.UpdateImage(wm.IMG.SWIM)
                    Dialog("Half an hour later, you announce yourself with a knock. " ..
                            "You hear a splash, followed by the frantic rustling of fabric.\n" ..
                            "Once things have settled down, you enter. ${name} has put on some clothes, and " ..
                            "appears visibly more relaxed.\n\nShe will be less afraid of you now.")
                    return
                else
                    wm.SetPlayerDisposition(4)
                    girl:pclove(-4)
                    girl:pcfear(2)
                    girl:happiness(-5)
                    girl:tiredness(3)
                    wm.UpdateImage(wm.IMG.BATH)
                    Dialog("${name} enters the tub, letting her body glide into the warm water. She tries to hide " ..
                            "her features under the water, but you had been very clear in your instruction *not* to have any foam.")
                    Dialog("The look of relaxation on her face quickly vanishes as she notices you leering at her body. " ..
                            "\"I've called you here so you could clean yourself properly, not so you could relax in my tub!\", you chide. " ..
                            "\"There seems to be some stubble between your legs\", you comment and hand her a razer. \"We can't have that!\"")
                    Dialog("She glares at you angrily. \"Either you do it yourself, or I'll do it\", you respond cheerily. " ..
                            "\"I'll have to get someone to hold you down, though\", you muse, \"We don't want to accidentally damage our merchandise, now do we?\"\n" ..
                            "You turn towards the door.")
                    Dialog("\"Alright, I'll do it\"\nUnder your watchful supervision, she first shaves her legs, then her crotch.\n" ..
                            "You run your hand over her pubic mound \"just to ensure everything is in order\"")
                    Dialog("\"Alright dear, now wash the rest of your body, and then off you go!\"")
                    return
                end
            end
            Dialog("You tell ${name} to get undressed. She takes off her clothes, and puts them in a neat pile ")
        end

        -- regular bathing
        wm.UpdateImage(wm.IMG.BATH)
        if wants_to_please and wm.Percent(66) then
            Dialog("${name} enters the tub and beckons you. \"Won't you join me?\"")
            local choice = ChoiceBox("Join her?", "Get into the tub", "Continue watching")
            if choice == 0 then
                wm.UpdateImage(wm.IMG.BATH, ImageOptions.HETERO)
                Dialog("Never one to decline such an invitation, you're soon floating in the warm water, liberally applying soap all over ${name}'s body.")
                Dialog("After rinsing herself off, she soaps up her hands and grabs your shaft. \"Let's get you cleaned, too.\"")
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("She starts stroking your now rock-hard cock. Just as you are about to cum, she stops. " ..
                        "\"I know something that will clean you even better than soap.\"")
                wm.UpdateImage(wm.IMG.DEEPTHROAT)
                -- TODO gag reflex
                Dialog("She takes the entire length of your shaft into her mouth, making sure to run her tongue over every inch. " ..
                        "You can't hold it any longer and spurt your load down her throat. ")
                Dialog("A few more flicks of her tongue, then she lets go of your cock. \"There, shiny as new\", she proclaims proudly.\n" ..
                        "Personal hygiene can be so much fun.")
                girl:happiness(3)
                girl:tiredness(-5)
                girl:oralsex(1)
            else
                wm.UpdateImage(wm.IMG.BATH)
                if girl:breast_size() > 4 and wm.Percent(66) then
                    Dialog("After a brief look of disappointment, ${girl} relaxes in the tub. She covers her body with foam, " ..
                            "taking great care to lather her ample breasts with exaggerated motions. After playing with her now stiff " ..
                            "nipples for a bit, she ducks underwater to wash off the soap.")
                else
                    Dialog("After a brief look of disappointment, ${girl} relaxes in the tub. She washes first her hair, then the rest of her body.")
                end
                Dialog("Having cleaned herself, she closes her eyes and lets her fingers slip between her legs.")
                wm.UpdateImage(wm.IMG.FINGER, ImageOptions.SOLO)
                Dialog("Soon, you can hear the water splashing with her faster movements, and her breathing quickens. Soft moans escape her lips, but she keeps her eyes closed.")
                Dialog("Then her body explodes into orgasm, her back arching, water splashing all around her. After is subsides, she finally opens her eyes. " ..
                        "\"A pity you couldn't join me.\"")
                SheJustCame(girl, 3)
                girl:happiness(3)
                girl:tiredness(-3)
                girl:pclove(-1)
            end
            return
        else
            Dialog("${name} enters the tub, letting her body glide into the warm water. As she washes herself, she " ..
                    "mostly ignores your presence.")
            local choice = ChoiceBox("Join her?", "Get into the tub", "Stay where you are")
            if choice == 0 then
                Dialog("Seeing her naked form, you decide to join her. She looks up in surprise when you enter the tub. Apparently, she did not notice you undressing.")
                wm.UpdateImage(wm.IMG.BATH, ImageOptions.HETERO)
                if girl:handjob() < 33 or girl:pclove() < 0 or girl:libido() < 25 or girl:has_trait("Chaste") then
                    Dialog("You instruct ${name} to wash you. She start diligently, lathering up your hair, back and chest, then continues to scrub your legs. ")
                    Dialog("\"Haven't you forgotten something\", you ask, pointing to your erect member.")
                    wm.UpdateImage(wm.IMG.HAND)
                    girl:handjob(1)
                    Dialog("With an audible sigh, she puts her soapy hands around your penis. You had hoped for a relaxing handjob, but all you " ..
                    "seem to get is some none-to-gentle scrubbing. \"That's enough, thank you.\"")
                    if wm.GetPlayerDisposition() < 0 then
                        local rape = ChoiceBox("Get your satisfaction by force?", "Yes", "No")
                        if rape == 0 then
                            Dialog("After the bath, as ${name} is starting to get dressed, " ..
                                    "you go to one of your drawers and take out a bottle of chloroform. You sneak up on her from behind while she is distracted, and press the rag to her mouth.")
                            girl:pclove(-10)
                            girl:pcfear(15)
                            wm.SetPlayerDisposition(-8)
                            if not girl:is_slave() then
                                wm.SetPlayerSuspicion(5)
                            end
                            wm.UpdateImage(wm.IMG.NUDE, ImageOptions.TIED_UP)
                            Dialog("She is only unconscious for a few minutes, but for a professional like you this is enough time to tie her up securely.")
                            PlayerRapeGirl(girl)
                            wm.UpdateImage(wm.IMG.RAPE, ImageOptions.TIED_UP)
                            Dialog("Just as she is beginning to stir, you push your dick inside her. She awakens with a scream.")
                            Dialog("\"You could have just satisfied me in the tub,\" you scold between rough thrust punctuating each word. \"Then none of this would have been necessary.\"")
                            wm.UpdateImage(wm.IMG.CUMSHOT, ImageOptions.TIED_UP)
                            Dialog("Just as you are about to cum, you pull out, then proceed to spurt your load over her face.\n" ..
                                    "You get dressed, ignoring her whimpers. Back in your office, you instruct one of your servants to \"Let the bitch stew for one hour, then escort her to her quarters.\"")
                            girl:obedience(5)
                            girl:add_trait("Recently Punished", 4)
                            return
                        else
                            Dialog("As you both are getting dressed after to bath, you address ${name} sternly. "..
                                    "\"I'm sure you know that this was not a satisfying performance. I'm not the kind of person you should disappoint. " ..
                                    "Do better next time, or you'll regret the consequences.\" She whimpers in agreement.")
                            girl:add_trait("Recently Scolded", 3)
                            girl:spirit(-2)
                            girl:pclove(-2)
                            girl:pcfear(5)
                            return
                        end
                    else
                        Dialog("As you both are getting dressed after to bath, you decide that you'll have to squeeze in a visit to your favourite girl before going back to work.")
                    end
                end
            else
                Dialog("After a few more minutes of soaking, she exits the tub, puts on her clothes, and leaves for her quarters.")
                girl:happiness(2)
                girl:tiredness(-5)
            end

            return
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to let you watch her take a bath.")
        return girl:trigger("girl:refuse")
    end
end