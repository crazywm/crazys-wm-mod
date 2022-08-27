
---@param girl wm.Girl
function LesbianSex(girl)
    wm.UpdateImage(wm.IMG.BED)
    Dialog("She notices that you have not come alone. Following her gaze you speak \"" ..
            "Ah I see you noticed. I'd like you two girls to get to know each other better and... :you wink slyly: I'd like to watch.\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        local what = ChoiceBox("Your suggestions", "Bondage", "Butt Stuff", "Dildo", "Oral", "69")
        local other_girl = RandomGirlName()
        if what == 0 then
            Dialog("\"Also, I've brought some equipment ;)\"\n")
            HandleLesBDSM(girl, other_girl)
            return
        elseif what == 1 then
            -- TODO!!!
            wm.UpdateImage(wm.IMG.ANAL, image_options.lesbian)
        elseif what == 1 then
            -- TODO!!!
            wm.UpdateImage(wm.IMG.DILDO, image_options.lesbian)
        end
        HandleLesbianSex(girl, other_girl)
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She wrinkles her nose in disgust and refuses.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function HandleLesBDSM(girl, other_name)
    if (girl:skill_check(wm.SKILLS.LESBIAN, 25) and girl:skill_check(wm.SKILLS.BDSM, 40)) then
        girl:experience(6)
        girl:tiredness(2)
        wm.UpdateImage(wm.IMG.SUB, image_options.lesbian)
        local suggest = "."
        if girl:bdsm() > 60 then
            suggest = ", with " .. girl:firstname() .. " occasionally making suggestions for her bondage."
        end
        Dialog(other_name .. " starts to tie up " .. girl:firstname() .. ". During the process, " .. girl:firstname() ..
                " and " .. other_name .. " are chatting amicably" .. suggest)
        if girl:libido() > 85 then
            Dialog("A dark spot appears on " .. girl:firstname() .. "'s panties. \"Wow, you are already soaking wet\", " ..
            other_name .. " comments. \"Today, you have to earn your orgasms, though!\"")
            wm.UpdateImage(wm.IMG.SPANKING, image_options.lesbian)
            Dialog("And with that she pulls " .. girl:firstname()..  "'s panties down and starts thoroughly spanking her ass.")
            if girl:has_trait("Masochist") and wm.Percent(50) then
                AdjustLust(girl, 10)
                SheJustCame(girl, 10)
                Dialog("Being such a pain slut, this is enough to make " .. girl:firstname() .. "cum. " .. other_name ..
                        "looks a bit put-off, apparently this is not what she intended. \"You like that, huh? Well, there is plenty more where that came from.\"" ..
                        "She intensifies her spanking, the loud smacks now echoing through the room. " .. girl:firstname() .. "'s ass turns bright red, then some bruises " ..
                        "start to appear.")
                local step_in = ChoiceBox("Step in?", girl:firstname() .. "has had enough", "Let " .. other_name .. " proceed")
                girl:health(-1)
                if step_in then
                    Dialog("\"I think that's enough for today,\" you tell " .. other_name ..
                            ". \"This fine ass has to make me money, I can't have it all bruised and battered for that.\"")
                    wm.SetPlayerDisposition(1)
                    wm.UpdateImage(wm.IMG.CHASTITY, image_options.lesbian)
                    Dialog("\"You came without my permission\", " .. other_name .. " chides " .. girl:firstname() .. " \"As a punishment, I'm making sure "  ..
                            " that you won't come again any time soon.\" With that, " .. other_name .. " locks her into a chastity belt. \"Have fun!\"")
                    return
                else
                    Dialog(other_name .. " continues for several minutes, until " .. girl:firstname() .. "'s ass has tuned into one big bruise.")
                    girl:add_trait("Bruises", 2)
                    girl:health(-5)
                    wm.SetPlayerDisposition(2)
                    return
                end
            else
                Dialog("This continuous on for several minutes, until " .. girl:firstname() .. "'s rear has turned bright red." ..
                        "\"I'm doing all the work here,\" " .. other_name .. " complains. \"It's time for you to do something useful.\"")
                BdsmFaceSitting(girl, other_name)
                local choice = ChoiceBox("Reward her?", "Let her cum", "Deny her", "Fuck her yourself")
                if choice == 0 then
                    Dialog("\"I think she's earned her reward, don't you?\" You turn to " .. other_name .. " \"And besides, I want to see you fuck her brains out!\"")
                    wm.UpdateImage(wm.IMG.FINGER, {participants=wm.IMG_PART.LESBIAN, tied=true})
                    Dialog("Dutifully, " .. other_name .. " starts working on " .. girl:firstname() .. " with her fingers. She is so horny, it doesn't even take her a " ..
                        "minute to get her to climax for the first time. ")
                    SheJustCame(girl, 10)
                    wm.UpdateImage(wm.IMG.LICK, {participants=wm.IMG_PART.LESBIAN, tied=true})
                    Dialog("After the second orgasm, " .. other_name .. " switches to licking " .. girl:firstname() .. "'s pussy. As her next orgasm builds, " ..
                            "she begins to struggle in her bounds, but to no avail. She cums, arching her back as far as her bindings allow. You sit in your chair, casually " ..
                            "stroking your cock as you drink in the sight of her.")
                    SheJustCame(girl, 10)
                    wm.UpdateImage(wm.IMG.DILDO, {participants=wm.IMG_PART.LESBIAN, tied=true})
                    Dialog("After the fourth climax, " .. other_name .. " goes to you bag o' goodies and takes out the largest dildo she can find. " ..
                            "\"It's time we fill you up\", she proclaims. " .. girl:firstname() .. " is so wet that despite its girth, the dildo slides in easily.")
                    if girl:has_trait("Nymphomaniac") and girl:tiredness() < 70 then
                        wm.UpdateImage(wm.IMG.CUMSHOT, {participants=wm.IMG_PART.LESBIAN, tied=true})
                        Dialog(other_name .. " brings " .. girl:firstname() .. " to orgasm over and over again, but she seems to have unlimited energy. " ..
                                "You feel yourself approaching the point of climax, so you get up and stand next to her. Just as she starts bucking under her next orgasm, " ..
                                "you spurt your load all over her writhing body. Feeling satisfied, you motion to " .. other_name .. " that the session is done.")
                    else
                        wm.UpdateImage(wm.IMG.REST, {tied=true})
                        Dialog(other_name .. " manages to coax two more orgasms out of " .. girl:firstname() .. ", but it appears that she is completely spent now." ..
                                "She falls asleep even before she has been untied. You look down at your throbbing erection, then at " .. other_name .. ": \"I guess this task falls to you, then ...\"")
                    end
                    return
                elseif choice == 1 then
                    Dialog("\"I don't think " .. girl:firstname() .. "was giving her all today. Maybe we should motivate her a bit more\". ")
                    wm.UpdateImage(wm.IMG.CHASTITY, image_options.lesbian)
                    Dialog(girl:firstname() .. " is placed in a chastity belt. \"If you want to cum, you'd better step up your game next time.\"")
                    -- If her performance truely wasn't good
                    if girl:oralsex() + girl:lesbian() < 50 then
                        girl:pclove(-1)
                    else
                        -- If you're just mean
                        wm.SetPlayerDisposition(1)
                        girl:pcfear(1)
                        girl:pclove(-2)
                    end
                    return
                elseif choice == 2 then
                    wm.UpdateImage(wm.IMG.SEX, {participants=wm.IMG_PART.FFM, tied=true})
                    Dialog("\"You did well\", you tell " .. girl:firstname() .. ". \"So here's your reward!\"\n" ..
                            "And with that, you plunge your dick into her slick pussy. She lets out a load moan. " ..
                            "\"Please, fuck me, " .. PlayerTitleFor(girl) .. "\", she pleads.")
                    Dialog("You've barely started pumping away when you feel her contract around your length. She must've really needed it. " ..
                            "You take this as encouragement and speed up your pounding. You make her cum one more time, then you feel yourself about to explode ...")
                    SheJustCame(girl, 10)
                    local inside = ChoiceBox("Where?", "In Her", "On Her")
                    if inside == 0 then
                        Dialog("With a few more deep thrusts, you spurt your load into her.")
                        PlayerFucksGirlUpdate(girl)
                    else
                        wm.UpdateImage(wm.IMG.CUMSHOT, {participants=wm.IMG_PART.HETERO, tied=true})
                        Dialog("You pull your cock out of her, and then proceed to scatter your load all over her face. She tries to gather as much as she can with her tongue.")
                        if girl:has_trait("Cum Addict") then
                            Dialog("After you've untied her, Cum Addict " .. girl:firstname() .. " uses her fingers to gather all the cum from her face, licking it up with a satisfied sigh.")
                        end
                    end
                    return
                end -- all branches returned
            end -- all branches returned
        elseif girl:libido() > 33 then
            -- at least a little horny
            wm.UpdateImage(wm.IMG.BED, {participants=wm.IMG_PART.LESBIAN, tied=true})
            local breast_text = ""
            if girl:breast_size() > 4 and girl:beauty() > 33 then
                breast_text = girl:firstname() .. " has no choice but to arch her back, giving you a wonderful view of her large boobs."
            end
            Dialog("Soon, " .. girl:firstname() .. "'s wrists and feet are cuffed and secured to the four corners of her bed. " ..
                    other_name .. " rummages in your crate of goodies until she finds a pair of nipple clamps. \"These will do nicely, she proclaims.\".\n" ..
                    "She puts them on " .. girl:firstname() .. " and starts pulling on the connecting chain. " .. breast_text
            )
            local maso_text = ""
            if girl:has_trait("Masochist") and wm.Percent(50) then
                AdjustLust(girl, 10)
                maso_text = " Clearly, " .. girl:firstname() .. " is getting off on this."
            end
            wm.UpdateImage(wm.IMG.BDSM, {participants=wm.IMG_PART.LESBIAN, tied=true})
            Dialog(other_name .. " fetches a flogger. \"Let's see if we can make your squirm.\"\n" ..
                    "She proceeds to flog " .. girl:firstname() .. "'s stomach, chest, and inner thighs. " ..
                    "First lightly, but then increasing the intensity." .. maso_text)
            local what = ChoiceBox("Intervene?", "\"Go easy on her\"", "Let " .. other_name .. " continue", "\"Harder!\"")
            if what == 0 then
                if girl:constitution() < 40 or girl:health() < 40 then
                    wm.SetPlayerDisposition(1)
                    girl:pcfear(-1)
                end
                Dialog("\"".. other_name ..", I think this is about as much as " .. girl:firstname() .. " can take\"." ..
                        "Then a grin spreads across your face. \"It's time for you to give her her reward.\"")
                wm.UpdateImage(wm.IMG.FINGER, {participants=wm.IMG_PART.LESBIAN, tied=true})
                Dialog(other_name .. " nods and puts away the flogger. She stars caressing " .. girl:firstname() .. "'s " ..
                        "lower lips, then inserts a finger, positioning herself in such a way that she doesn't block your view of the action." )
                if girl:libido() > 66 and wm.Percent(50) then
                    Dialog("Soon, she has " .. girl:firstname() .. " screaming in the throngs of passion.")
                    SheJustCame(girl, 5)
                else
                    Dialog("After some vigorous finger-fucking, and with the help of her tongue, " .. other_name ..
                            "manages to bring " .. girl:firstname() .. " to orgasm.")
                    SheJustCame(girl, 3)
                end
                wm.UpdateImage(wm.IMG.BED, {participants=wm.IMG_PART.LESBIAN, tied=true})
                Dialog("You would have liked to stay longer and also collect your *reward*, " ..
                        "but your other duties are calling. You leave the two girls to clean up.")
            elseif what == 1 then
                if girl:constitution() < 40 or girl:bdsm() < 40 then
                    girl:health(-5)
                end

                if girl:has_trait("Masochist") and wm.Percent(50) then
                    AdjustLust(girl, 5)
                end

                if girl:constitution() < 40 or girl:health() < 40 then
                    wm.SetPlayerDisposition(-1)
                    girl:pcfear(1)
                    Dialog("By the time " .. other_name .. " is done with her flogging, tears are streaming down " .. girl:firstname() .. "'s face." ..
                                 "\"Oh you poor thing, was that too much for you?\" " .. other_name .. " mocks. \"Then let us switch things up a bit!\"")
                else
                    Dialog(other_name .. " continues flogging " .. girl:firstname() .. " for another minute or so. " ..
                            "\"I am doing all the work here\", she complains. \"I think its time for me to enjoy this a bit more!\"")
                end
                BdsmFaceSitting(girl, other_name)
                Dialog("You would have liked to stay longer and let " .. girl:firstname() .. " service you too, " ..
                        "but your other duties are calling. You leave the two girls to clean up.")
            else
                if girl:constitution() < 40 or girl:health() < 40 then
                    wm.SetPlayerDisposition(-5)
                    girl:pcfear(5)
                else
                    wm.SetPlayerDisposition(-3)
                    girl:pcfear(3)
                end
                Dialog("\"As you wish!\" " .. other_name .. " shrugs, then picks up a whip from your crate.")
                if girl:has_trait("Masochist") and wm.Percent(50) then
                    AdjustLust(girl, 10)
                end
                Dialog("Each loud crack announces another welt appearing on " .. girl:firstname() .. "'s skin. She is " ..
                        "crying and begging now: \"Please, " .. PlayerTitleFor(girl) .. ", please make her stop.\"")
                local gag = ChoiceBox("Gag her?", "The whimpering is music to my ears", "Shut her up")
                if gag == 1 then
                    Dialog("While " .. other_name .. " continues " .. girl:firstname() .. "'s whipping, you insert an inflatable gag into her mouth." ..
                            "You give it a few good pumps, making sure it is large enough to obstruct the airflow through her mouth.")
                    if girl:has_trait("Strong Gag Reflex") or (wm.Percent(50) and girl:has_trait("Gag Reflex") ) then
                        Dialog(girl:firstname() .. " makes a retching sound, and some fluid comes out of her nose. Apparently, you've triggered her Gag Reflex.")
                        wm.UpdateImage(wm.IMG.DEATH, {tied=true})
                        Dialog("It appears she is unable to breathe, some of her stomach's contents must have gotten into her trachea. " ..
                                "You quickly take out her gag and untie her hands so you can set her upright and perform a Heimlich maneuver. " ..
                                "By the time you manage to dislodge the bits of undigested food that have gotten stuck, her face has turned blue.")
                        wm.UpdateImage(wm.IMG.KISS, {participants=wm.IMG_PART.LESBIAN, tied=true})
                        Dialog("You turn to " .. other_name .. "\"We need to take things a bit more slowly. " ..
                                girl:firstname() .. " seems to have difficulty breathing. Maybe you could provide some assistance, mouth-to-mouth.\" You wink.")
                        girl:pcfear(50)
                        girl:pclove(-50)
                        wm.SetPlayerDisposition(-10)
                        Dialog("You find it difficult to get in the mood again after this shock, and soon decide to go, leaving " ..
                                other_name .. " in charge or making sure " .. girl:firstname() .. " is alright.")
                    else
                        Dialog("Now, whenever " .. other_name .. " is about to strike, you pinch " .. girl:firstname() ..
                                "'s nose shut, cutting of her air and making her unable to scream.\n\n" ..
                                "Pinch ... crack ... her chest bucking, in vain trying to get air ... " ..
                                "You let go of her nose, and she gets a few desperate breaths in before you start the cycle again.")
                        girl:pcfear(5)
                        girl:pclove(-5)
                        wm.SetPlayerDisposition(-2)
                        Dialog("You get a bit carried away in your sadistic enjoyment of this torture, and when you next actually " ..
                                "check on her twitching body, it is covered in bloody welts. It will take some time for these marks to fade, and " ..
                                "you've decided you've damaged your merchandise enough.")
                        girl:add_trait("Bruises", 3)
                    end
                    girl:health(-5)
                    girl:tiredness(5)
                else
                    Dialog("Her pleas fall on deaf ears on your part. By the time you motion " .. other_name .. " to stop, " ..
                            girl:firstname() .. "'s body is covered in red welts. \"I think she can make better use of that tongue than this whimpering\", " ..
                            other_name .. " proclaims")
                    girl:add_trait("Bruises", 1)
                    BdsmFaceSitting(girl, other_name)
                    Dialog("You would have liked to stay longer and let " .. girl:firstname() .. " service you too, " ..
                            "but your other duties are calling. You leave the two girls to clean up.")
                end
            end
            return
        else
            -- not horny at all
            -- TODO
            Dialog("Low Libido TODO")
        end
    else
        girl:experience(6)
        girl:tiredness(4)
        wm.UpdateImage(wm.IMG.SUB, image_options.lesbian)
        Dialog(other_name .. " starts to tie up " .. girl:firstname() .. ". From her panicked expression it becomes apparent that "
                .. girl:firstname() .. " lacks the necessary experience for this.")
        if wm.GetPlayerDisposition() > -33 then
            Dialog("\"OK, " .. other_name .. ", I think that's enough for now. Poor " .. girl:firstname() .. " is already shaking.\"\n" ..
            "Then you add: \"Maybe there is another way you can get her to shake.\"")
            wm.UpdateImage(wm.IMG.LICK, image_options.lesbian)
            Dialog("The ropes undone, " .. girl:firstname() .. " lies down on your bed and " .. other_name .. " buries her head in her crotch. ")
            Dialog("After the first orgasm shudders through her body, you tell " .. other_name .. " to intensify her efforts, while you " ..
            "start binding " .. girl:firstname() .. " in a spread-eagle position, fixing her hands and feet to the bedposts.")
            wm.UpdateImage(wm.IMG.BDSM, image_options.lesbian)
            Dialog("This time, she is too distracted to really notice. It's only when she bucks under her next orgasm that the ropes " ..
            " become apparent, constraining her movements. ")
            if girl:libido() > 33 then
                Dialog("\"This was awesome\", " .. girl:firstname() ..  " \"can we do that again?\"\n" ..
                "You take out a blindfold. \"If you wear this, you can go as many rounds as you like.\"\n" ..
                "You can see conflicting emotions warring on " .. girl:firstname() .. "'s face.")
                if girl:libido() > 66 then
                    Dialog("\"OK, lets do it,\" she finally says. You proceed to place the blindfold over her eyes " ..
                    "and nod to " .. other_name ". As she gets to work take out your cock and start stroking. Just as " ..
                    girl:firstname() .. " reaches her next climax, you spurt your load all over her belly. With " .. other_name ..
                    " continuing her stimulation, she doesn't even seem to notice.")
                    girl:happiness(3)
                    girl:experience(2)
                    girl:bdsm(2)
                else
                    Dialog("\"No, I don't think I'm ready for that\" she finally admits. " ..
                    "\"Maybe " .. other_name .. " wants to have a go?\"\n" ..
                    "Even though you really wanted to see " .. girl:firstname() .. " in bondage, you suppose " ..
                    "this is better than nothing. ")
                    wm.UpdateImage(wm.IMG.DOM, image_options.lesbian)
                    girl:happiness(1)
                    girl:bdsm(1)
                    girl:lesbian(1)
                end
            else
                Dialog("\"Please, can you get these off\",  she pleads as she regains her senses.\n" ..
                "Not wanting to overdo it, you comply. \"Now, thank " .. other_name .. " for what she did to you!\"")
                wm.UpdateImage(wm.IMG.EATOUT)
                Dialog("They switch places, with " .. girl:firstname() .. " now licking " .. other_name .. "'s pussy.")
                if girl:skill_check(wm.SKILLS.LESBIAN, 33) then
                    Dialog("This is not quite what you wanted, but in the end you sigh, take your dick out and start wanking.")
                    girl:happiness(1)
                    girl:lesbian(1)
                else
                    Dialog("It soon becomes apparent, though, that " .. girl:firstname() .. " isn't really up to that task either. " ..
                    "With an apology for " .. girl:firstname() .. "'s lack of skill, you send " .. other_name .. " on her way, excited but unsatisfied. " ..
                    "You yourself leave the room in utter disappointment.")
                    girl:happiness(-1)
                    girl:lesbian(1)
                end
            end
        else
            wm.UpdateImage(wm.IMG.BDSM, image_options.lesbian)
            Dialog("This just turns you on more. \"No mercy\", you instruct " .. other_name .. ".\n" ..
                    girl:firstname() .. " will hate you for this, but a lion does not concern himself with " ..
                    "the opinion of a sheep -- or a bitch, in this case. You certainly enjoy her panicked sobs and " ..
                    "cries, and that's what matters.")
            girl:bdsm(1)
            girl:happiness(-10)
            girl:pclove(-10)
            girl:pcfear(3)
        end
    end
end

---@param girl wm.Girl
function BdsmFaceSitting(girl, other_name)
    -- TODO position=wm.IMG_POS.FACE_SITTING
    wm.UpdateImage(wm.IMG.ORAL, {participants=wm.IMG_PART.LESBIAN, tied=true})
    Dialog("With that, " .. other_name .. " lowers herself on " .. girl:firstname() .. "'s face. " ..
            girl:firstname() .. "'s tongue eagerly starts licking the offered pussy. Soon, " .. other_name ..
            " is moaning loudly. She presses her hips further into " .. girl:firstname() .. "'s face, cutting off her air supply at times.")
    Dialog("When " .. other_name .. " finally cums and lifts herself up a bit, " .. girl:firstname() .. "'s face is dark red, " ..
            "and she desperately is sucking in big gulps of air.")
    girl:lesbian(1)
    girl:oralsex(1)
end

---@param girl wm.Girl
function HandleLesbianSex(girl, other_name)
    girl:experience(6)
    girl:tiredness(3)
    Dialog("You sit down on the bed and make yourself comfortable as the girls approach one another.")
    if girl:skill_check(wm.SKILLS.LESBIAN, 75) then
        wm.UpdateImage(wm.IMG.STRIP)
        Dialog("The girls lose themselves in passionate kisses. They take turns removing each others clothes with their mouths; some of the intimate articles are playfully tossed your way.")
        wm.UpdateImage(wm.IMG.LESBIAN)
        Dialog("They join you on the bed and slowly and expertly probe each other with fingers and tongues. You alternate fondling their lithe bodies and stroking your member.")
        local position = wm.Range(0, 2)
        if position == 0 then
            wm.UpdateImage(wm.IMG.LES69)
            Dialog("They straddle each other and each girl buries her tongue in the other's cunt. Before long, both are panting and moaning loudly.")
            Dialog("Their bodies quiver with each orgasm that overpowers them.")
        elseif position == 1 then
            wm.UpdateImage(wm.IMG.SCISSOR)
            local beauty_text = ""
            if girl:beauty() > 35 then
                beauty_text = "You appreciate the gesture, mesmerized by the sight in front of you as they start rubbing their pussies together. "
            else
                beauty_text = "Unfortunately, " .. girl:firstname() .. " isn't really that impressive to look at, and you resort to closing your eyes and focus on her moans, which do get you excited. "
            end
            Dialog("Now that they are both properly excited, they arrange themselves in a scissor position, making sure that they give you a good view. " .. beauty_text ..
                    "Their cries increase in volume until they start to quiver and collapse onto each other.")
        elseif position == 2 then
            wm.UpdateImage(wm.IMG.EATOUT)
            Dialog(other_name .. " lies down on her back, and " .. girl:firstname() ..
                    " starts munching on her pussy. Judging by the moans and sight, she seems to be doing a rather good job, and soon " ..
                    other_name .. " shudders as she climaxes. ")
        end

        Dialog("They lay beside each other on the bed looking into each others eyes.")
        if girl:skill_check(wm.SKILLS.ORALSEX, 50) and not girl:has_trait("Lesbian") then
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("You stand over them; your erect member inches from their faces. \"What about me, girls?\" They giggle and begin to lick and stroke your staff with skill. The sensation is amazing and you shoot a large stream of semen across those cute faces.")
        end
        girl:happiness(2)
        girl:lesbian(1)
    else
        Dialog("The girls hesitantly move closer. A few awkward pecks on the cheek later you find the need to direct them.")
        wm.UpdateImage(wm.IMG.STRIP)
        Dialog("You tell them to get undressed after which they begin to cautiously touch one another.  \"No, no, no!\" you exclaim, \"Lick her damn cunt!\"  They both immediately try to comply and bump heads.")
        wm.UpdateImage(wm.IMG.EATOUT)
        Dialog("You shake your head as the girls make faces after each time tongue meets pussy.")
        Dialog("After all the time you spent directing you had no chance to enjoy the show; awkward as it was.  You sigh and pray to goddess of Yuri that they at least learned something from the experience.")
        girl:lesbian(2)
        girl:tiredness(3)
    end
end