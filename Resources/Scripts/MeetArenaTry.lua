IsStrongGirl = false

---@param girl wm.Girl
function MeetArena(girl)
    Dialog("You hold open try outs to all girls willing to step into the arena and fight for their life.")
    IsStrongGirl = false
    if girl:has_trait("tough") or girl:has_trait("strong") or
            girl:magic() >= 50 or girl:combat() >= 50
    then
        IsStrongGirl = true
        Dialog("You hear there is a natural-born warrior applying today.")
    else
        if wm.Percent(33) then
            r3 = wm.Range(1, 3)
            IsStrongGirl = true
            if r3 >= 2 then
                girl:bdsm(50)
            else
                girl:refinement(50)
            end
        end
    end

    local choice = ChoiceBox("What would you like to do?",
            "Supervise the try-out personally.", "Watch the try-out from your luxury box."
    )

    if choice == 0 then
        PersonalSupervision(girl)
    else
        Dialog("You enjoy watching the fights in the privacy of your luxury box.")
        WatchFromSuite(girl)
    end

end

function PersonalSupervision(girl)
    Dialog("You make your way to the arena sand.")
    Dialog("You are pleased by the turnout. It looks like 30 or 40 women want to work for you. Surely one of them will measure up.")
    Dialog("The try-out is a single elimination tournament. A girl wins a match if she scores five touches before her opponent. Last girl standing wins a personal interview with you.")
    if IsStrongGirl == false then
        -- handle desperate girl
        Dialog("The first match is very lopsided. The defeated girl fails to even touch her opponent once. She falls to the arena sand, sobbing hopelessly.")
        Dialog("Curious, you approach her. \"You are clearly not a warrior. Why did you attempt this?\"")
        Dialog("Girl: \"Sir, I am destitute and desperate. I have no money and no prospects. I would do nearly anything for a decent job, but there are none to be had.\"")
        Dialog("Girl: \"I knew I would probably be beaten, possibly even injured or killed. But I had to try something.\"")
        local choice = ChoiceBox("",
                "You are pretty enough, and I own a brothel that is always hiring.",
                "That's too bad, wimp. This try-out is for warriors, not wusses.",
                "Willingly facing impossible odds shows a warrior's spirit. Would you care for a wager?"
        )
        if choice == 0 then
            Dialog("Girl: \"No sir, I won't do that. If I wanted to make a living as a mattress, I'd be at a different try-out.\"")
            Dialog("Drying her tears and stiffening her back, she marches out of the arena with head held high.")
            return
        elseif choice == 1 then
            if wm.Percent(20) then
                Dialog("Stung by your callousness, the girl slaps you.  It's a good one, too. All five fingers are clearly visible on your cheek.")
                Dialog("One of your guards stifles a laugh. Several cannot help smirking.")
                Dialog("You see the other try-out participants whispering and giggling to each other.")
                local choice = ChoiceBox("",
                        "\"Guards, this one needs a lesson. Throw her in the dungeon, I'll deal with her later.\"",
                        "\"You know, I was rude. I may have deserved that slap.\""
                )
                if choice == 0 then
                    Dialog("Spitting with anger, you detail five of your guards to drag her off to the dungeons.")
                    -- TODO this creates a new girl, why?
                    local age = wm.Range(18, 30)
                    local girl = wm.CreateRandomGirl(wm.SPAWN.KIDNAPPED, age)
                    -- TODO DUNGEON_GIRLKIDNAPPED
                    wm.ToDungeon(girl, 3)
                    Dialog("You hear murmurings from the crowd. You realize you have just kidnapped a woman in front of hundreds of witnesses. Your faux pas infuriates you even more.")
                    wm.SetPlayerDisposition(-25)
                    wm.SetPlayerSuspicion(25)
                    Dialog("You turn and roar at the rest of the girls. \"Get back to the try-out, or you will all join her!\"")
                    Dialog("One of the girls, a tall Amazon with ice-cold eyes, shakes her head. \"I'd have slapped you, too. I will not work for a petty despot like yourself.\"")
                    Dialog("The other girls nod their heads. One of them yells, \"Preach it, sister!\"")
                    Dialog("She continues, \"We are thirty battle-hardened warriors against your paltry few remaing guards. Do not think we will go into captivity easily.\"")
                    local choice = ChoiceBox("",
                            "Infuriated by her insolence, you draw your sword and strike for the Amazon's heart.",
                            "You blink. \"Very well, then. Depart this place, and never come back.\""
                    )
                    if choice == 0 then
                        Dialog("She parries your thrust easily, and suddenly you are fighting for your life. She's good, too. You can barely fend her off.")
                        Dialog("The other girls retrieve their weapons and advance upon you as well. There's no way you can handle that whole group.")
                        Dialog("\"To me, men! Teach these wenches a lesson!\" You hear your men draw their weapons, and you grin fiercely.")
                        Dialog("But your men shock you as they drop their weapons to the floor. One guard sheepishly says, \"Sorry, boss, but I ain't dying just because you've suddenly started acting like an total dickhead.\"")
                        Dialog("You've always thought that your preferred end would be in bed with thirty girls 'doing' you. But in your fantasies, your bed wasn't blood-soaked sand, and the girls weren't using swords.")
                        Dialog("The town coroner lists your cause of death as: \"TERMINAL RUDENESS\".")
                        wm.GameOver()
                        return
                    else
                        Dialog("\"Yeah, OK, tough guy. We're leaving,\" sneers the Amazon.")
                        Dialog("The girls take their belongings, muttering amongst themselves and eyeing your guards cautiously.")
                        Dialog("That night, sitting in your room alone, you get a case of the shakes.")
                        Dialog("You realize that treating proud women warriors like they are brothel slaves is not good for your health.")
                        return
                    end
                else
                    Dialog("\"Please accept my apologies, my dear. And while I'm afraid you are not qualified to work as a fighter in the arena, please stop by the house tonight and my chef will feed you famously.\"")
                    wm.SetPlayerDisposition(10)
                    Dialog("The girl smiles. It is a really lovely smile. \"Thank you, and your apology is accepted. I may avail myself of your hospitality.\"")
                    Dialog("You turn to the rest of the girls. \"Shall we continue with the try-out?\"")
                    Dialog("Realizing you would be a boss worth fighting for, the rest of the women put on a dazzling show of martial arts.")
                    Dialog("The eventual winner is truly a magnificent fighter. She looks at you and claims her prize. \"When will I fight my first match?\"")
                    HireFuckChoice(girl)
                    return
                end
            else
                Dialog("Stung by your insult, the girl collects herself. Stonefaced, she stalks out of the arena.")
                wm.SetPlayerDisposition(-5)
                Dialog("You gesture angrily to the rest of the girls. \"Alright, get back to the tryout!\"")
                Dialog("The girls get back to their matches, but you can see they are not really into it.")
                Dialog("Many women obviously let inferior opponents defeat them, and the eventual winner knows she isn't actually the best warrior in the arena today.")
                if wm.Percent(50) < 5 then
                    Dialog("Still, when you offer her the job, she takes it. You hear her mutter under her breath, \"Better working for an asshole than starving to death.\"")
                    girl:pclove(-125)
                    wm.AcquireGirl(girl)
                    return
                else
                    Dialog("Worse, she spurns your offer of a job. \"There's plenty of work on the battlefields of Kandor Plains, I don't actually need to work for an asshole.\"")
                    return
                end
            end
        elseif choice == 2 then
            Dialog("A warrior's heart is not always housed in a warrior's body. But I can give you a warrior's training.")
            Dialog("I have a potion that will temporarily increase your combat skills. Drink it, and spar with me. If you defeat me, I'll give you a job. ")
            Dialog("The girl agrees and drinks the potion. She suddenly looks... sleeker.  \"Let's dance!\" she purrs, and then strikes with astonishing speed.")
            wm.SetPlayerDisposition(10)
            girl:bdsm(50)
            girl:agility(50)
            girl:refinement(50)
            local wins = 0
            if wm.Percent(girl:combat()) then
                Dialog("You manage to deflect her first strike, but just barely.")
                wins = 1
            else
                Dialog("Despite her increased speed, you still parry her strike easily.")
            end

            if wm:Percent(girl:agility()) then
                Dialog("Your riposte meets only empty air, as she dances around your blade and scores the first strike.")
                wins = 3
            else
                Dialog("You catch her by surprise with a high line riposte, scoring the first touch.")
            end

            if wm:Percent(girl:magic()) then
                wins = 2
                Dialog("Suddenly, you feel weak and slow. The girl's eyes have started to glow, and she is smiling... lazily.  Like a spider looks at a fly.")
            else
                Dialog("The girl's eyes start to glow. You begin to inexplicably feel clumsy. But then the glow falters and fades, and the girl shakes her head angrily.")
            end

            if wins > 0 then
                -- TODO this is wrong, this should be a temp buff
                girl:agility(-50)
                girl:bdsm(-50)
                girl:refinement(-50)
                Dialog("The change in this girl is amazing. It's a hard fight, and her lack of experience hinders her, but she gets to five touches before you.")
                Dialog("\"A deal's a deal, girl. You are hired. I am only sorry that the effects of that potion are temporary. ")
                if wins == 2 then
                    if wm.Percent(10) then
                        Dialog("The girl's eyes start to glow again. \"Do not worry. I... I like the way I am now. I think I'm going to stay this way.\"")
                        Dialog("Awestruck, you turn to your guards. \"Get this woman a uniform!\"")
                        girl:agility(50)
                        girl:bdsm(50)
                        girl:refinement(50)
                    end
                end
                girl:confidence(25)
                girl:pclove(25)
                girl:pcfear(-25)
                girl:tiredness(25)
                wm.AcquireGirl(girl)
                return
            else
                -- she still lost
                Dialog("Despite the obvious and amazing increases in her skills, she is still unable to defeat you.")
                Dialog("Girl: \"Sir, despite not being worthy yet, this taste of my potential gives me hope.\"")
                Dialog("One of the other participants, an Amazon with ice-cold eyes, comments: \"They are always hiring on the battlefields of Kandor Plaiins. They will train you, and perhaps you will live to grow into your potential.\"")
                Dialog("Girl: \"Thank you. Thank you both. I'll be back someday to claim a job here.\"")
                return
            end
        end
    else
        -- handle strong girl
        local result = wm.Range(1, 3)
        if result == 1 then
            Dialog("The woman who wins the try-out looks at you challengingly. \"Do I get the job?\"")
            HireFuckChoice(girl)
        elseif result == 2 then
            Dialog("The try-out winner is beside herself with joy: \"I can't believe it! I won! I... didn't think I could do it, but I won!\"")
            choice = ChoiceBox("",
                    "\"You should be more self-confident, my dear. You fought extremely well, and the job is yours.\"",
                    "\"Surprised the hell out of me, too, honey. I bet against you in every one of your matches.\""
            )
            if choice == 0 then
                wm.AcquireGirl(girl)
                return
            else
                Dialog("The girl's wonder at her win is spoiled by your public lack of confidence in her abilities.")
                Dialog("Girl: \"Well, I hope you lost a TON of money on me. I am so out of here, you jerk!\"")
                return
            end
        else
            Dialog("The winner of the try-out arrogantly proclaims: \"I have proven myself superior to this rabble. When is my first match?\"")
            choice = ChoiceBox("",
                    "\"Woah. I like my girls confident, not bitchy.\"",
                    "\"You must defeat one of my beasts as a final test.\""
            )
            if choice == 0 then
                Dialog("Girl: \"Is that so?  Well, I may be a bitch, but at least I know who my parents are.\"")
                Dialog("She flips you off as she indignantly strides away.")
                return
            else
                FightTentacleBeast(girl)
                return
            end
        end
    end
end

function FightTentacleBeast(girl)
    Dialog("Girl: \"Bring it on.\" She tosses aside her sparring sticks and retrieves her actual combat  weapons, while the rest of the try-out participants scramble out of the arena.")
    Dialog("You talk to the beastmaster for the arena. \"This girl needs to be brought down a peg. Bring out the phasing horror.\" The beastmaster grins and opens one of the cages lining the arena.")
    wm.SetPlayerDisposition(-10)
    Dialog("Girl: \"Oh, not another tentacled sex beast. It's almost boring killing these things, you just have to keep them from grabbing your arms.\"")
    Dialog("True to her word, she dances away from any tentacles near her legs. She hacks at a tentacle groping for her arms.")
    if Percent(girl:combat()) then
        Dialog("If she's surprised that her blade goes right through the tentacle, she doesn't show it. She jumps away from the beast and studies it closely.")
        Dialog("Girl: \"Ah, I see. Only about half those tentacles are real. The others are illusions designed to distract me.\"")
        Dialog("She jumps back on the attack, targeting particular tentacles and ignoring others.")
        Dialog("Her next two swings meet with success, and she actually lops off about six feet from one of the tentacles.")
        Dialog("Her third swing goes awry again. The tentacle simply isn't there as her blade passes through it.")
        if Percent(girl:combat()) then
            Dialog("The girl dances away and analyzes the situtation again: \"Oh, my. worse than I thought. The beast can route it's real tentacles through an alternate dimension and avoid damage if I'm not quick enough in my strikes.")
            Dialog("The girl then proceeds to methodically chop off all the tentacles from the phasing horror with swift and precise strokes. She finishes it off with a thrust through the horror's now defenseless thorax.")
            Dialog("Girl: \"Well, this was a nice change of pace. Do you have other interesting challenges like that in your bestiary?\"")
            Dialog("You smile. \" I do, and you can fight them daily if you accept the job.\"")
            Dialog("Girl: \"Sign me up!\"")
            wm.AcquireGirl(girl)
            return
        else
            Dialog("Assuming she had figured out the beast's tricks, this second failure takes her completely by surprise. She's good enough that she recovers quickly - but not quite quickly enough.")
        end
    else
        Dialog("A complete lack of resistance surprises her. Her blade simply passes through the tentacle.")
        Dialog("She committed too much to the swing and is over-balanced. The phasing horror is on her before she can recover.")
    end
    Dialog("You hear a meaty thud as a tentacle sweeps her legs out from underneath her. She strikes wildly at the tentacle attempting to wrap up her legs, but her blade passes through the tentacle to clang onto the ground.")
    Dialog("And just that suddenly, the fight is over. Each arm is grabbed by a tentacle that appears as if from nowhere. Other tentacles wrap up her legs.")
    Dialog("The tentacles force her legs apart while pinning her arms to the floor. Her armor momentarily stymies the beast, but after a few seconds, the horror's tentacles ripple seemingly pass right through her armor into the juicy orifices beneath.")
    Dialog("\"Please, don't let it kill me!\" The rest of her plea suddenly turns into inarticulate moans as a tentacle forces itself into her mouth.")
    Dialog("\"My dear, I'm sorry, but you failed the test. Don't worry, the phasing horror will not permanently injure you - but it does usually play with any new toy for a couple of days to insure fertilization. Enjoy your weekend!\"")
end

function HireFuckChoice(girl)
    local choice = ChoiceBox("",
            "\"That depends. Prove to me that you fuck as well as you fight, and you are hired.\"",
            "\"You are obviously a well-qualified fighter. The job is yours.\""
    )
    if choice == 0 then
        Dialog("\"Men!\" she exclaims. \"There is only one weapon you think about, and it is a small, shrivelled thing compared to an honest steel blade.\"")
        Dialog("\"Take this job and shove it!\" ")
        wm.SetPlayerDisposition(-5)
        return
    else
        Dialog("\"Thank you, sir! I will make you proud of me!\"")
        wm.SetPlayerDisposition(5)
        if IsStrongGirl == false then
            -- TODO WHAT???
            girl:bdsm(50)
            girl:refinement(50)
        end
        wm.AcquireGirl(girl)
        return
    end
end

function WatchFromSuite(girl)
    if IsStrongGirl == false then
        Dialog("One of the warriors eliminated from the tryout is very attractive. A pleasant fantasy about her briefly enters your mind.")
        local choice = ChoiceBox("", "Rub one out while fantasizing about her.",
            "Have a guard invite her to your luxury box", "On second thought, you would rather get a closer look at the try-out.")
        if wm.Percent(girl:beauty()) or wm.Percent(girl:charisma()) then
            if choice == 0 then
                Masturbate(girl)
            elseif choice == 1 then
                Dialog("You welcome the warrior woman to your luxury box.")
                Dialog("She looks around, clearly impressed by the opulence of your suite.")
                Dialog("You demonstrate the excellent view from your suite, and you spend a enjoyable hour or two discussing the combat techniques being used on the arena floor.")
                Dialog("During the final match, the girl asks you which of the combatants you think is prettier.")
                choice = ChoiceBox("",
                        "\"Prettiness doesn't really matter. I'm looking for warriors, not courtesans.\"",
                        "\"Well, neither of them compare to my current companion.\""
                )
                if choice == 0 then
                    Dialog("Girl: \" That's... very professional. But a little cold-blooded. Two women are fighting to get a job with you, and you don't care what they look like?\"")
                    Dialog("You chuckle. \"Well, do you know the old joke? No matter how well qualified the applicants, managers just hire the woman with the biggest tits.\"")
                    Dialog("She smiles, and asks you throatily: \"Would mine be big enough for you?\"")
                    Dialog("\"I would have to measure them. And luckily, I have an appropriate tool available.\"")
                    Dialog("\"Well then, let's see if they measure up!\" she says, removing her top.")
                    Dialog("You pull your measuring stick out of your pants. Her breasts are magnificent, and easily wrap around your cock completely.")
                    Dialog("It doesn't take long before you give her breasts a glistening seal of approval, coating her nipples with a salty glaze.")
                    Dialog("You certainly passed the qualifying interview, my dear!! Congratulations!")
                    wm.AcquireGirl(girl)
                else
                    Dialog("Girl: \"I love fishing for compliments when my companion is smart enough to give me one.\"")
                    Dialog("She leans close, and gives you a quick peck on the cheek.")
                    Dialog("You pull her in even closer. and go for a more serious kiss.")
                    Dialog("She puts a hand on your chest to stop you. \"This is too fast for me.  I would love to hang around with you, though, and try to get to know you a little better.\"")
                    Dialog("\"I would like to see you again, too. Join my enterprise, and we'll see how the relationship goes.\"")
                    wm.AcquireGirl(girl)
                end
            elseif choice == 2 then
                Dialog("You climb down to the arena floor to supervise the try-out.")
                PersonalSupervision(girl)
            end
        else
            if choice == 0 then
                if wm.Percent(20) then
                    Dialog("You've got your little friend out and are pulling lustily when you hear a squeak behind you.")
                    Dialog("Pulling your pants up quickly, you turn to find one of the arena hostesses at the door.")
                    Dialog("\"Sir, I was just checking your food and drink. I did knock, but I guess you didn't hear me.\"")
                    choice = ChoiceBox("",
                            "\"Well, that's good timing. Come over here and help me with this.\"",
                            "\"Give me one reason why I shouldn't fire you.\""
                    )
                    if choice == 0 then
                        Dialog("Girl: \"Sir, I am a hostess, not a whore. I'll just leave and let you finish up.\"")
                        Dialog("Girl: \"I will be discreet, sir. This will be our little secret. (Well, not 'little', exactly.)\" She giggles, and slips out the door.")
                    else
                        Dialog("Girl: \"Please, sir, it was an honest mistake.There is no need to over-react.\"")
                        Dialog("You snarl at her. \"You seem awfully calm for someone who's going to be looking for a new job tomorrow.\"")
                        Dialog("The girl shrugs: \"Well, I took this job because I was tired of fighting in the arena. I can always go back to that.\"")
                        Dialog("She suddenly giggles. \"I've faced down much worse beasts that that one.\"")
                        Dialog("You can't help yourself. You guffaw with laughter")
                        Dialog(" \"I like your style. You can do better than arena hostess. Perhaps we can find another job that will suit you better.\"")
                        Dialog("Girl: \"I'd like that, sir. Thank you.\"")
                        wm.SetPlayerDisposition(5)
                        girl:charisma(25)
                        wm.AcquireGirl(girl)
                    end
                else
                    Masturbate(girl)
                end
            elseif choice == 1 then
                Dialog("A guard ushers her in. Unfortunately, you discover that distance improved her looks greatly.")
                Dialog("She's not ugly, exactly, but the reality is far short of your imagination.")
                Dialog("Worse, she isn't the flirtatious sex bunny you had hoped for. She seems defensive and worried.")
                HaveDate(girl)
            elseif choice == 2 then
                PersonalSupervision(girl)
            end
        end
    else
        Dialog("You are distracted from the try-outs by an attractive woman sitting in the stands. You idly wonder what she would look like without clothes.")
        local choice = ChoiceBox("", "Rub one out while fantasizing about her.",
                "Have a guard invite her to your luxury box", "On second thought, you would rather get a closer look at the try-out.")
        if choice == 0 then
            if wm.Percent(20) then
                Dialog("Girl: \"Hello, sir, can I help you with... Oh, my goodness, what are you doing?\"")
                Dialog("You knew you should have put up a 'Do Not Disturb' sign. A horrified hostess is standing at the door staring at your member.")
                Dialog("You hurriedly try to put yourself away, and of course, you catch your johnson in your zipper.")
                Dialog("With a little effort, and a bit of pain, you extricate yourself and get your clothing adjusted.")
                Dialog("Girl: \"I'm so, so sorry, sir. I feel so bad that I made you hurt your... self. I'll go get a doctor.\"")
                choice = ChoiceBox("", "\"No doctor necessary, just get me some ice.\"",
                        "I don't need a doctor. I need you to kiss it and make it better."
                )
                if choice == 0 then
                    Dialog("Girl: \"Oh, of course sir! Let me help you with that!\"")
                    Dialog("After several minutes of holding a bag of ice to your groin, the girl starts fidgeting. \"I really think I should get a doctor.\"")
                    Dialog("Girl: \"Shouldn't the ice be REDUCING the swelling?\"")
                    Dialog("You grin. \"There's no chance that swelling will go down as long as you are touching me.\"")
                    Dialog("She grins back. \"I think I know a remedy to help.\" She pulls your cock out of your pants, and starts to suck on the tip.")
                    Dialog("She's very careful to avoid the damaged area. Her lips stay on the tip, while her hands busy themselves with your balls.")
                    Dialog("You gasp as she puts a finger into your ass, and spurt into her mouth as she massages your prostate.")
                    Dialog("She noisily swallows. \"Voila! I must be a doctor, the swelling is going down!\"")
                    Dialog("\"Honey, you are wasted as an arena hostess. How would you like to be head courtesan at one of my brothels?\"")
                    Dialog("Girl: \"Oh, I'll be a courtesan, alright. And I'll do more than just head!\"")
                    girl:libido(20)
                    girl:normalsex(20)
                    wm.AcquireGirl(girl)
                elseif choice == 1 then
                    Dialog("Girl\" \"Sir! That's... just rude! I'll be back with the arena doctor.\"")
                    Dialog("She flounces off angrily, and you sigh to yourself. At least the arena doctor is on your payroll, so she will be discreet about your 'accident'.")
                    return
                end
            else
                Masturbate(girl)
            end
        elseif choice == 1 then
            Dialog("You hear a knock on the door. Your guard sheepishly announces that he's brought the girl.")
            Dialog("She enters the room, along with a rather large man. \"Thank you for the invite. I didn't think you'd mind if I brought aloong my fiance, Erik.\"")
            Dialog("You grimace. Why did you expect an unescorted woman to be watching arena fights?  Even the hookers are going to have security.")
            if wm.Percent(girl:intelligence()) then
                Dialog("It turns out not to be a total loss. The couple is charming and intelligent, and you enjoy their company despite your disappointment.")
                Dialog("At the end of a lovely afternoon, Erik thanks you by giving you a trinket from his pocket. \"It's the least I can do for letting us enjoy your company and the view from your suite.\"")
                wm.GivePlayerRandomSpecialItem()
            else
                Dialog("You try to engage the couple in conversation, but not surprisingly, there are more awkward silences than actual words.")
                Dialog("Average citizens and crime lords tend to have little in common.")
                Dialog("The couple make excuses after an uncomfortable hour. \"Thanks for the invite, but we have to get back to our friends.\" You are happy to bring the episode to a merciful end.")

            end
            HaveDate(girl)
        elseif choice == 2 then
            PersonalSupervision(girl)
        end
    end
end

function Masturbate(girl)
    if wm.Percent(95) then
        Dialog("Stroking off is a fun way to pass the time, and you enjoy the next few minutes tremendously.")
        Dialog("Unfortunately, pleasuring yourself doesn't get any new girls hired.")
        return
    else
        Dialog("You are getting a good rhythm going, when you are rudely interrupted by the tip of a knife being pressed to your throat.")
        Dialog("A female voice whispers in your ear: \"You know, it's very easy to sneak up on a man when he's wrestling with Little Barney.")
        Dialog("Girl: \"Although in your case, maybe we should call him Big Barney.\"")
        AssassinationAttempt(girl)
    end
end

function AssassinationAttempt(girl)
    local choice = ChoiceBox("",
            "Get that knife away from my throat, or you are going to eat it",
            "If you are here to assassinate me, I'll double whatever you are getting paid."
    )
    if choice == 0 then
        Dialog("Girl: \"They told me you think you are tough - but you'll bleed out in seconds if you make me cut your jugular.\"")
        Dialog("Girl: \"I'm just here to deliver a message. You aren't safe anywhere, and you need to stop trying to expand your territory.\"")
        Dialog("Girl: \"Be content with what you have, and you'll live to a ripe old age. Otherwise, things are going to become... uncomfortable.\"")
        Dialog("Girl: \"Now be a good boy and use these manacles to attach your legs to the chair.\"")
        wm.SetPlayerSuspicion(10)
        -- 10
        choice = ChoiceBox("",
                "Once manacled, you'll be at her mercy. You go for the knife.",
                "Seeing no realistic option, you manacle your legs to the chair."
        )
        if choice == 0 then
            Dialog("Your effort is brave - but not very smart. She's alert and ready for resistance, and one simple thrust ends the fight.")
            Dialog("Choking on your own blood, you can't even call for help.")
            Dialog("You try to make it to the door, but you are tripped up as your undone pants fall to your ankles.")
            Dialog("Your last thought is that this is a pretty embarrassing way to die.")
            wm.GameOver()
        elseif choice == 1 then
            Dialog("After you have your legs secured, she hands you another manacle for your sword arm. She then shackles your shield arm, and pushes a ball gag into your mouth as well.")
            Dialog("Girl: \"My, aren't you trussed up nicely? But you know what, in all the excitement, we left Little Barney hanging out. It's going to be a little hard explaining that to whomever finds you.")
            Dialog("Girl: \"This kinda makes me horny. I'll tell you what, I'll make you a deal. I'm going to rape you in that chair, and if you get me off, I'll do up your pants before I leave. Deal?\"")
            choice = ChoiceBox("", "You've had all the humiliation you can stand. You angrily shake your head NO to her 'deal'",
                    "Sheepishly, you nod your head to agree to her 'deal'. (Besides, she's cute.)"
            )
            if choice == 0 then
                Dialog("Girl: \"Well, now I'm insulted. I guess I'm not pretty enough for you? Let's just make sure your men can laugh about you being a coward, as well as being beaten by a girl.\"")
                Dialog("She pulls off her pants, squats over your crotch, and deliberately urinates on your member. \"Now they'll think you pissed yourself in fear.\"")
                Dialog("She dresses herself, and exits with one last reminder: \"No more expanding your territory, or I'll cut Little Barney off next time.\"")
            elseif choice == 1 then
                Dialog("Girl: \"First things first - we need to get Mr. Softie hard again. She nibbles and suckles at your cock, one hand guiding your shaft into her mouth, while the other tickles the underside of your balls.")
                Dialog("You stiffen quickly, and she takes off her pants. She rubs her crotch on your face for a minute, and then drops her furry love tunnel on your erect dick.")
                Dialog("Girl: \"Don't you dare come before me!\" And she then starts into an explosive cowgirl fuck.")
                if wm.Percent(30) then
                    Dialog("Unfortunately, your body betrays you. Excited beyond endurance, you spend your seed after three strokes.\"")
                    Dialog("Girl: \"Well, I had hoped for better, Mr. Jack Rabbit. Have fun explaining this to your men.\"")
                else
                    Dialog("The girl is obviously turned on by your helplessness. Within two minutes of lusty thrusting, she shudders and wriggles violently. \"Oh, that was a good one. One more and I'll consider the deal satisfied.")
                    Dialog("Pulling herself off you, she turns around for some reverse cowgirl. You notice she has a wonderful ass as she buries your cock in her cunt. ")
                    Dialog("It only takes a couple more minutes before she is overcome by another violent orgasm. You convulse in ecstasy as well, spurting large gouts of seed into her gash.")
                    Dialog("\"Ooooh, lover, too bad this is just a one night stand.\" She giggles. \"Well, actually, more like a one afternoon sit.")
                    Dialog("She does up your pants and even wipes up an errant cum blob from the front of your trousers before she takes her leave.")
                end
            end
            return
        end
    elseif choice == 1 then
        Dialog("The girl laughs: \"And then I get no more contracts at all. I don't think this is a good deal for me.\"")
        Dialog("She seems to be quite mercenary. She's talking, not slicing. Maybe you just have to sweeten the deal.")
        choice = ChoiceBox("",
                "You leer at her: \"We can make it a long term contract - between you, me and Big Barney.\"",
                "Then we'll make it a long term contract - steady work, more pay, and I'll take no cut from any side income."
        )
        if choice == 0 then
            Dialog("Girl: \"Oh, what an attractive offer! Let me think about that for a second:  NO.\"")
            Dialog("Girl: I'm not here to kill you. I'm here to deliver a message. Stop expanding your territory, or we're going to make things uncomfortable for you.\"")
            Dialog("She ties you to your chair, and gags you. She runs her knife along your penis from balls to tip, just before she leaves. \"Remember, no more expansion.\"")
        else
            Dialog("That's a pretty good deal. Against my better judgment, I'll take it.")
            wm.AcquireGirl(girl)
            girl:obedience(100)
            girl:pclove(-100)
            girl:house(-100)
        end
    end
end

function HaveDate(girl)
    local choice = ChoiceBox("",
            "\"Something seems to be troubling you, my dear.\"",
            "Guards, this one is ugly and surly. Send her back."
    )
    if choice == 0 then
        Dialog("Girl: \"Well, it's all a bit overwhelming. All the girls assumed that you called me up here to try to fuck me.\"")
        Dialog("Girl: \"They said you think my spirit is crushed because I just lost, so I'll be easy prey for you.\"")
        choice = ChoiceBox("",
                "\"Well, my secret is out. So are we going to do it or not?\"",
                "My dear, seducing you is the furthest thing from my mind.",
                "I was actually hoping for some knowledgeable conversation while watching the fights."
        )
        if choice == 2 then
            Dialog("She clearly still doesn't trust you, but she agrees to stay.")
            Dialog("You order lunch and drinks, and engage in small talk while you wait.")
            Dialog("It turns out she is an expert in the use of small magical talismans, and you spend a fascinating half-hour talking about the combat tricks they provide.")
            Dialog("Dinner arrives just as she is describing the combat advantages of appearing to be six inches to the left of where you actually are.")
            Dialog("Please go on, dear, while I set out our plates.")
            choice = ChoiceBox("", "Slip a date rape drug into her drink.", "Leave her drink alone.")
            if choice == 0 then
                Dialog("You watch intently as she takes a sip from her drink. You are startled as one of her bracelets starts glowing and buzzing.\"")
                Dialog("The girl turns on you with a flat, cold stare. \"Did you know the first and easiest talisman made by most girls with my talents, is a charm against poisons?\" ")
                Dialog("She touches another bracelet, and the air around her is suddenly charged with energy. \"I'm leaving. You'll regret it if you try to stop me.\"")
                Dialog("Girl: \"I think the authorities will be very interested to find out what you serve your guests in this luxury box.\"")
                wm.SetPlayerSuspicion(50)
                wm.SetPlayerDisposition(-10)
            elseif choice == 1 then
                Dialog("With food, drink and a jovial host, the girl starts to relax a little.")
                Dialog("But she stiffens up again as you comment, \"Your friends were right about one thing - I did have an ulterior motive for asking you up here.\"")
                Dialog("\"I have a job opening for a woman just like yourself, working around the arena. Would you be interested?\"")
                Dialog("Girl: \"I came here looking for a job like that. I thought I'd have to earn it on the arena floor, but I'd be a fool to turn it down just because it's offered in a luxury suite.\"")
                Dialog("Girl: \"But... no strings attached, right?\"  You laugh. \"Suspicious to the end!  No, my dear, there are no strings attached.\"")
                wm.SetPlayerDisposition(10)
                wm.AcquireGirl(girl)
            end
        else
            Dialog("Your comment only enrages her. \"You could have said something dumber, but you'd have to work hard at it.\"")
            Dialog("As she leaves, she grins nastily. \"This is not going to help your reputation as a player. I can't wait to tell the other girls about what a klutz you really are.\"")
            wm.SetPlayerDisposition(-5)
            return
        end
    else
        Dialog("You expect her to react angrily, but she just looks relieved and leaves quietly.")
        Dialog("Puzzled, you ask a guard why she reacted that way.")
        Dialog("\"Sir, she's a warrior, comfortable on the sands of the arena or exploring dank dungeons..\"")
        Dialog("\"She was out of her element and uncomfortable in an arena's luxury suite. She didn't know how to act and was afraid of making a fool of herself.\"")
        Dialog("She probably didn't even hear your insult, she just took the opportunity to escape.")
        wm.SetPlayerDisposition(-5)
    end
end
