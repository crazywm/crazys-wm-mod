---@param girl wm.Girl
function InteractDungeon(girl)
    DungeonInteractChoice(girl)
end

---@param girl wm.Girl
function InteractBrothel(girl)
    BrothelInteractChoice(girl)
end


---@param girl wm.Girl
function DungeonInteractChoice(girl)
    local choice = ChoiceBox("What would you like to do with her?", "Torture", "Chat", "Scold", "Ask", "Force")
    if choice == 0 then
        girl:torture()
        wm.UpdateImage(wm.IMG.TORTURE)
    elseif choice == 1 then
        Chat(girl)
    elseif choice == 2 then
        ScoldGirl(girl)
    elseif choice == 3 then
        local have_sex = "To have sex"
        if girl:pclove() > 90 then
            have_sex = "To make love"
        end

        choice = ChoiceBox("", have_sex, "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            if girl:pclove() > 90 then
                girl:happiness(5)
                girl:libido(1)
                girl:experience(1)
                girl:tiredness(1)
                girl:obedience(1)
                girl:pcfear(-1)
                girl:pchate(-1)
                girl:pclove(2)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both have passionate wild sex, and then bask in each others glow.")
                PlayerFucksGirl(girl)
            elseif girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(2)
                girl:libido(1)
                girl:tiredness(2)
                girl:pclove(1)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both enjoy fucking one another.")
            else
                Dialog("She refuses to sleep with you.")
            end
        elseif choice == 1 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog("She enjoy you watching her while another girl fucks her.")
            else
                Dialog("She refused to have sex with another girl.")
            end
        elseif choice == 2 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BEAST)
                Dialog("She enjoys you watching her being fucked by all sorts of tentacled sex fiends.")
            else
                Dialog("She refused to have sex with creatures.")
            end
        elseif choice == 3 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BDSM)
                Dialog("She allows you to tie her up and spank her while you both cum hard.")
            else
                Dialog("She refused to do this.")
            end
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                if girl:obey_check(wm.ACTIONS.SEX) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.DEEPTHROAT)
                    Dialog("She lets you shove your cock deep down the back of her throat until you cum into her head.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            elseif choice == 1 then
                if girl:obey_check(wm.ACTIONS.SEX) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.ORAL)
                    Dialog("She sucks your cock until you cum in her mouth.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.ANAL)
                Dialog("She lets you fuck her in her tight little ass until you both cum.")
                -- ANAL SEX
            else
                Dialog("She refused to let you fuck her ass.")
            end
        elseif choice == 6 then
            -- THREESOME
            return DungeonInteractChoice(girl)      -- tail call
        elseif choice == 7 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.GROUP)
                Dialog("You and a group of your male servants take the poor girl in all way, she was hurt.")
            else
                Dialog("She refuse to be fucked in a gangbang")
            end

        else
            return DungeonInteractChoice(girl)      -- tail call
        end
    elseif choice == 4 then -- FORCE
        choice = ChoiceBox("", "To have sex with you", "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            wm.UpdateImage(wm.IMG.SEX)
            Dialog("She stuggles to no avail as you force yourself inside her, you fuck her roughly until you unload yourself into her.")
        elseif choice == 1 then
            wm.UpdateImage(wm.IMG.LESBIAN)
            Dialog("You call a female customer who fuck the poor girl with dildo.")
        elseif choice == 2 then
            wm.UpdateImage(wm.IMG.BEAST)
            Dialog("You forcefully tie her down and let in a tentacle creature followed by several other creatures to have their way with her.")
        elseif choice == 3 then
            wm.UpdateImage(wm.IMG.BDSM)
            Dialog("You hog-tie her and pour hot wax all over her body before spanking her with a paddle.")
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                wm.UpdateImage(wm.IMG.DEEPTHROAT)
                girl:dignity(-2)
                Dialog("You grab her by the back of her head and force your cock into her throat, she gags as you cum into her stomach.")
            elseif choice == 1 then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("You grab her and force her to suck your cock, she struggles but cannot stop you.")
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            wm.UpdateImage(wm.IMG.ANAL)
            Dialog("Although she tries to keep her ass closed you manage to get inside her and proceed to fuck her painfully.")
        elseif choice == 6 then
            wm.UpdateImage(wm.IMG.GROUP)
            Dialog("One of your servant holds his head in his hands while your sperm spreads his face. All your servants are happy to unload on her pretty face")
            girl:dignity(-1)
        else
            return DungeonInteractChoice(girl)
        end
        girl:happiness(-4)
        girl:libido(1)
        girl:confidence(-1)
        girl:obedience(1)
        girl:spirit(-1)
        girl:tiredness(4)
        girl:health(-1)
        girl:pcfear(2)
        girl:pclove(-2)
        girl:pchate(1)
    end
end


function Chat(girl)
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
        return DungeonInteractChoice(girl)
    end
end


function BrothelInteractChoice(girl)
    local choice = ChoiceBox("What would you like to do?", "Reward " .. girl:name(),
            "Chat with " .. girl:name(), "Visit " .. girl:name() .. "'s Bedroom",
            "Call " .. girl:name() .. " to your office", "Invite " .. girl:name() .. " to your private chambers",
            "Train " .. girl:name(), "Scold " .. girl:name()
    )

    if choice == 0 then
        Dialog("You give " .. girl:name() .. " some spending money to cheer her up.")
        girl:give_money(100)
        girl:happiness(2)
        girl:pclove(1)
        girl:pchate(-1)
    elseif choice == 1 then
        Dialog("Lets take a moment to talk.")
        choice = ChoiceBox("Topic?", "Casual Conversation", "Discuss her", "Talk about you", "<Go Back>")
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
        end
    elseif choice == 2 then
        local have_sex = "To have sex"
        if girl:pclove() > 90 then
            have_sex = "To make love"
        end
        choice = ChoiceBox("", have_sex, "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "For a strip tease", "Go Back")
        if choice == 0 then
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.SEX)
                if wm.Percent(girl:normalsex()) then
                    Dialog("As the head of your penis passes her labia you feel her squeeze her muscles around your member.  She starts rocking her hips and pushing against you.")
                    Dialog("Without you realizing it she has switched positions and is now bouncing and grinding on top of you.")
                    Dialog("Many positions and orgasms later you both lie next to each other completely exhausted and satisfied.")
                    girl:happiness(1)
                    girl:normalsex(1)
                    girl:experience(3)
                else
                    Dialog("You begin with slow thrusts hoping for a long night of pleasure.")
                    Dialog("As you continue to thrust she barely moves and hardly makes a sound.")
                    Dialog("Bored by her lackluster performance you finish quickly and leave her room.")
                end
            else
                Dialog("She refuses to have sex; mumbling some half hearted excuse.")
                RefusedSexAct(girl)
            end
        elseif choice == 1 then
            Dialog("She notices that you have not come alone.  Following her gaze you speak \"Ah I see you noticed.  I'd like you two girls to get to know each other better and...  :you wink slyly:  I'd like to watch.\"")
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog("You sit down on the bed and make yourself comfortable as the girls approach one another.")
                if wm.Percent(girl:lesbian()) then
                    Dialog("The girls lose themselves in passionate kisses.  They take turns removing each others clothes with their mouths; some of the intimate articles are playfully tossed your way.")
                    Dialog("They join you on the bed and slowly and expertly probe each other with fingers and tongues.   You alternate fondling their lithe bodies and stroking your member.")
                    Dialog("Their bodies quiver with each orgasm that overpowers them.")
                    Dialog("They lay beside each other on the bed looking into each others eyes.")
                    Dialog("You stand over them; your erect member inches from their faces. \"What about me, girls?\" They giggle and begin to lick and stroke your staff with skill. The sensation is amazing and you shoot a large stream of semen across those cute faces.")
                    girl:happiness(2)
                    girl:libido(1)
                else
                    Dialog("The girls hesitantly move closer,  A few awkward pecks on the cheek later you find the need to direct them.")
                    Dialog("You tell them to get undressed after which they begin to cautiously touch one another.  \"No, no, no!\" you exclaim, \"Lick her damn cunt!\"  They both immediately try to comply and bump heads.")
                    Dialog("You shake your head as the girls make faces after each time tongue meets pussy.")
                    Dialog("After all the time you spent directing you had no chance to enjoy the show; awkward as it was.  You sigh and pray to goddess of Yuri that they at least learned something from the experience.")
                end
            else
                Dialog("She wrinkles her nose in disgust and refuses.")
                RefusedSexAct(girl)
            end
        elseif choice == 2 then
            Dialog("I wonder if you would cheer up my pet Malboro. He's been down lately and could really use a good fucking.")
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.BEAST)
                Dialog("She smiles and nods. I've always liked that \"little\" guy.  Let's go cheer him up.")
                if wm.Percent(girl:beastiality()) then
                    Dialog("She smartly removes her clothing before she enters the cage. Beasts don't really care about what their fuck toys look like anyway.")
                    Dialog("She approaches the massive tentacled beast with skill of and professional handler.  She finds just the right spots to arouse the monster.")
                    Dialog("The malboro immediately responds and several tenctacles seize her arms and legs.  She squeals with delight as tentacles enter her pussy and anus.")
                    Dialog("After a good long while of probing her with tentacles the malboro releases the girls and slithers off satisfied.")
                    girl:happiness(1)
                    girl:beastiality(1)
                else
                    Dialog("She enters the cage fully clothed, which turns out to be a mistake when she walks up behind the creature and startles it.")
                    Dialog("The creature goes into a blind sexual fury and shreds her clothing and violently shoves mutliple tentacles into her orifices.")
                    Dialog("Her screams of terror are muffled by the tentacles in her mouth, but they are still audible enough for you to hear as a fifth tentacle approaches her ass.")
                    Dialog("Fearing for her safety you and your men rush to the rescue.  You are forced to injure your pet in the process, not to mention the cost to replace her clothing.")
                    girl:health(-5)
                    girl:tiredness(10)
                    -- TODO player spendsgold 50 - 75
                end
            else
                Dialog("She refuses to have sex with a beast.")
                RefusedSexAct(girl)
            end
        elseif choice == 3 then
            Dialog("As you enter her room you accidently jostle the crate you're carrying.  Hearing the clinking and clanging she eyes the crate.  \"What do you have got in the crate?\" She asks.")
            Dialog("\"Take a look.\" you say.  Her eyes widen at the sight of the whips, chains, and harnesses.")
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.BDSM)
                Dialog("A smile graces her face as she picks through the plethora of various bondage toys. ")
                if wm.Percent(girl:bdsm()) then
                    Dialog("She chooses a large wicked looking dildo and some anal beads from the crate.  She blushes as she begs you to use them after you've tied her up.")
                    Dialog("You bind her arms and legs to the large wooden X.  Selecting riding crop from the crate, you proceed to spank her ass harder and harder.  You begin shoving bead after bead into her eager ass hole.")
                    Dialog("You unbind her from the X and chain her arms above her head in a standing position.  You grasp the dildo she selected and begin ramming it into her wet cunt as you smack her tits with the crop.")
                    Dialog("Each stroke draws gasps of pain and pleasure.  she moans in ecstasy as you ravage her body.  You leave the dildo in her pussy and quickly pull the beads from her ass;  You pinch her nipples as you ram you cock deep into her ass.  You thrust roughly until you both cum hard.  You unchain her and she collapses on the floor exhausted and ecstatic.")
                    girl:happiness(2)
                    girl:tiredness(1)
                    girl:libido(2)
                else
                    Dialog("She seems to have something to prove today and she selects the biggest and most painful instruments for you to use on her.  Before you begin she tells you to hit her as hard as you like.")
                    Dialog("You whip her repeatedly with the studded cat-o-nine whip.  It takes a lot of effort to force the gigantic dildo into her pussy, she screams as a small trickle of blood runs down her inner thigh.")
                    Dialog("Concerned you ask if you should stop.  She sobs \"No, I can do this.  Keep going.\"  You force the massive butt plug into her sphincter bringing more tears streaming down her face.  ")
                    Dialog("she loses her resolve and the now pathetic crying and begging to remove the toys causes you to lose your erection. Disgusted,  You leave her bound and sobbing in the room.  A few hours later you send some men to untie her.")
                    girl:happiness(-1)
                    girl:libido(-5)
                    girl:health(-5)
                end
            else
                Dialog("She refuses to let you tie her up.")
                RefusedSexAct(girl)
            end
        elseif choice == 4 then
            Dialog("You almost trip over her as you enter the room. \"My Dear, what are you doing kneeling on the floor?\"  Seeing her kneeling before you gives you an idea...")
            -- Choice 3 // GOTO 101
            local choice = ChoiceBox("", "Blowjob", "Deep Throat", "Titty Fuck")
            Dialog("\"Well never mind the previous reason you're down there.  I shall give you a new one.\"  You pull your member from you pants and  bring it within inches of her face.")
            if choice == 0 then
                Dialog("\"Now let's see how well you've learned to suck a cock.\"")
                if girl:obey_check(wm.ACTIONS.SEX) then
                    wm.UpdateImage(wm.IMG.ORAL)
                    if wm.Percent(girl:oral()) then
                        Dialog("She smiles and reaches up to grasp your cock.  She begins by gently licking and kissing the tip.  Her tongue then traces down the length of your shaft and  draws circles around our balls.")
                        Dialog("She gently sucks each ball into her mouth in turn.  She turns her head and slides her lips back forth on your shaft.  She kisses the tip again and slowly slides you into  her mouth.")
                        Dialog("You're not sure how long this pleasure lasted but soon you feel the volcano about to erupt.  She  senses it too and pulls your cock from her mouth and gently strokes it with her hands until you explode all over her face.")
                        girl:oral(3)
                        girl:happiness(2)
                    else
                        Dialog("She giggles a bit and quickly closes her mouth over your member.  You become a little alarmed as you start to feel her teeth grazing the sensitive skin of your cock.")
                        Dialog("You become slightly distracted as her teeth begin to grind harder and harder on your soft flesh.")
                        Dialog("Fearing for your dick you tell her to just lick it and use her hands.  No longer distracted you concentrate on her pretty face and how you will soon cover it in your semen.  You feel the pressure building and building. ")
                        Dialog("Your dick explodes in an eruption of cum, but as you look down to admire your handiwork you realize that she dodged at the last moment.  You leave disappointed.")
                        girl:happiness(-2)
                        girl:tiredness(3)
                    end
                else
                    Dialog("She closes her mouth tight and turns her head way from your penis.")
                    RefusedSexAct(girl)
                end
            elseif choice == 1 then
                Dialog("\"I want to see how much of this you can fit in your mouth.\"")
                if girl:obey_check(wm.ACTIONS.SEX) then
                    wm.UpdateImage(wm.IMG.DEEPTHROAT)
                    if wm.Percent(girl:bdsm()) then
                        Dialog("She eyes your cock hungrily and begins to suck the tip, swirling her tongue around the head.  She sucks harder and harder and you feel yourself being pulled deeper and deeper inside.")
                        Dialog("You are amazed as you feel her nose brush against your abdomen. You look down at her and gaze into her eyes as you grasp the back of her head.")
                        Dialog("You begin to move back and forth,  holding tight to her hair.  She trills and hums as you fuck her throat.  You feel the surge of pressure build in your cock and her eyes plead with you to release your load deep inside her.")
                        Dialog("She smiles as the thick cream slides down the back of her throat.  As you slowly pull out of her mouth, she sucks and licks every last drop of cum from your penis and swallows it down with a smile.")
                        girl:happiness(3)
                        girl:bdsm(2)
                    else
                        Dialog("Her lips part and she begins to gobble down your cock like a sausage staved bavarian.  Despite her efforts she begins to gag violently.")
                        Dialog("She tries to soldier on but each time your cock gets halfway into her mouth she gags and has to stop.")
                        Dialog("Determined to do as she ask she tries to ram it all down at once.  The result is disastrous as she not only gags but vomits all over you penis.  She apologizes as she cleans you up and you leave unsatisfied.")
                        girl:happiness(-2)
                        girl:tiredness(3)
                    end
                else
                    Dialog("She closes her mouth tight and shakes her head refusingly.")
                    RefusedSexAct(girl)
                end
            elseif choice == 2 then
                Dialog("You sit down on a stool beside her. \"Why don't you use your tits and your mouth this time?\"")
                if girl:obey_check(wm.ACTIONS.SEX) then
                    wm.UpdateImage(wm.IMG.TITTY)
                    -- TODO TRAITS
                    if girl:has_trait("Abnormally Large Boobs") then
                        Dialog("As she frees her massive jugs from her, top you marvel at their size.  Your dick becomes lost in the undulating sea of soft pillowy flesh.")
                        Dialog("Occasionally, you feel her tongue reach into the sea of breasts and lick the end of your shaft.")
                        Dialog("Your mind becomes lost in a fog and you soon feel your volcano about to erupt, but like a volcano under the sea your hot sticky magma is lost under the tide of those gigantic breasts.")
                        girl:libido(5)
                        girl:tittysex(3)
                    elseif girl:has_trait("Big Boobs") then
                        Dialog("She giggles a bit as her breasts envelop your member.  She uses her wonderfully large breasts to stroke your shaft as her mouth and tongue work magic on the tip.")
                        Dialog("Faster and faster her breasts move up and down your length.  She begins to alternate the motion of each breast and the stimulation begins to overwhelm you..")
                        Dialog("You release a massive load of cum covering her breasts and mouth.  She licks your cum from her nipples and then cleans your member with her mouth.  You stagger from her room in ecstacy.")
                        girl:libido(4)
                        girl:tittysex(3)
                    elseif girl:has_trait("Small Boobs") then
                        Dialog("You catch her gaze as she looks down at her mostly flat chest and then you see a glint of determination in her eyes.  She begins to rub the tip of your dick around her hardening nipples.")
                        Dialog("You enjoy the feeling of her smooth skin against your cock.  She tries valiantly to force her breasts together, but ultimately fails.")
                        Dialog("She relies heavily on her expert tongue and her rock hard nipples brushing the tip of your shaft.  You explode suddenly across her chest and she coats her fingers with your cum and licks them like a lollipop, giving you a spritely wink.")
                        girl:libido(2)
                        girl:tittysex(3)
                    else
                        Dialog("She licks the full length of your spear to lubricate it and then forces her breasts together tightly and begins to rock up and down on your member.")
                        Dialog("The feeling of her softness surrounding your manhood is beyond compare and you are lost in pleasure.")
                        Dialog("She notices the sudden swelling of your cock before you do and she gently but vigorously strokes you with her hands.  You cum hard and your milk spills over her breasts.  She  cleans you both up and you leave with a smile on your face.")
                        girl:libido(3)
                        girl:tittysex(3)
                    end
                else
                    Dialog("She stands up quickly and turns her back to you defiantly.")
                    RefusedSexAct(girl)
                end

            end
        elseif choice == 5 then
            Dialog("As you enter the room, the sight of her luscious ass assails your delighted eyes.  She looks back at you and greets you with a smile.")
            Dialog("A thought pops into your head as you continue to admire the view.  You voice your thought \"I think we should try something a little different tonight?\"")
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.ANAL)
                Dialog("She shakes her ass and bends over further; slightly pulling down her panties.  'Well, come and get it.\" she purrs")
                if wm.Percent(girl:anal()) then
                    Dialog("You move toward her and slide the panties down to the floor.  She reaches back and grips your rod, rubbing it against her wet pussy for lubrication and finally presses the now slick tip against her anus.")
                    Dialog("A shiver of anticipation washes over you before you thrust deep into her ass.  A gasp of delight escapes her lips and she begins to rock back against you")
                    if girl:has_trait("Great Arse") then
                        Dialog("You can't help but be amazed by the sight of your cock penetrating  her truly fine ass.  The enjoyment overwhelms you and you explode deep inside her ass.")
                    else
                        Dialog("She expertly moves and teases you with her ass and you both collapse in ecstacy.")
                    end
                    girl:happiness(2)
                    girl:libido(2)
                    girl:tiredness(2)
                else
                    Dialog("She visibly winces as she feels your throbbing manhood pressing against her anus.  A tear streaks down her cheek as you penetrate her.")
                    Dialog("She begins crying as you begin to move back and forth inside her.  You try to enjoy yourself inspite of her ,but the crying proves too distracting and you begin to lose interest.")
                    Dialog("You pull out your cock and manually finish yourself on her ass.")
                    girl:happiness(-5)
                    girl:libido(-5)
                    girl:tiredness(5)
                end
            else
                Dialog("She quickly stands up and turns around.  \"I know what your thinking and the answer is NO.\"")
                RefusedSexAct(girl)
            end
        elseif choice == 6 then
            Dialog("Her back is toward you as you enter the room.  You notice that one of her hands  is between her legs.")
            Dialog("You sneak around her side to get a better view.  A few moments later she gasps and jumps up as she notices you standing there.")
            Dialog("\"Oh, you don't have to stop on my account.\"  you say with a wink.")
            if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
                wm.UpdateImage(wm.IMG.STRIP)
                Dialog("She relaxes and grins devilishly.  \"Alright, I hope you will enjoy the show,\" she then adds \"but no touching.\"")
                if wm.Percent(girl:strip()) then
                    Dialog("She moves to her bed and makes herself comfortable and making sure you have a good view.  She begins rubbing her mound through her panties and before long a dark wet spot begins to grow;  you also start feeling some growth.")
                    if girl:has_trait("Great Figure") then
                        Dialog("As she rubs, pulls, and teases her pussy; You admire her incredible figure as it undulates and gyrates from the stimulation.")
                    end
                    if girl:beauty() > 75 then
                        Dialog("As you watch her sliding her fingers in and out you marvel at how beautiful everything about this girls is.  It occurs to you that she has one of the most fantastic vaginas you have ever seen.  A moan brings your attention back to the show.")
                    end
                    Dialog("She pulls her panties to the side and begins to work herself over with the precision of a practised expert.")
                    Dialog("You are impressed that she is able to keep up the stimulation as she bucks wildly.  You stroke you shaft in time with her probings.")
                    Dialog("Her moans become screams of pleasure as she approaches another massive orgasm.  You both cum simultaneously and some of your orgasm lands across her stomach.")
                else
                    Dialog("She moves to the bed to get comfortable but gives no consideration to your view.  She reaches inside her panties and begins massaging her clit.")
                    Dialog("She barely makes a sound and you can't tell what she is doing but there is certainly very little movement.")
                    Dialog("After a few minutes of the same routine, she looks up and you and tells you she is done.  You leave disappointed.")
                end
            else
                if girl:has_trait("Meek") then
                    Dialog("She blushes to a deep red and pushes you out of the room without ever making eye contact.")
                else
                    Dialog("She immediately covers herself and demands you leave her room.")
                    RefusedSexAct(girl)
                end
            end
        elseif choice == 7 then
            Dialog("You ask the group of men to wait in the hall as you enter her room.")
            Dialog("You find her relaxing on her bed looking through some of her lingerie.  She looks up as you speak.\"I wonder if you could help me, my dear.  I've got a group of gentlemen outside and I was wondering if you could help me entertain them?\"")
            if girl:obey_check(wm.ACTIONS.SEX) then
                wm.UpdateImage(wm.IMG.GROUP)
                Dialog("She nods in agreement, but asks for a few minutes to get ready.  As the door closes behind you, many rumbling and rustling sounds can be heard.  A minute or so later she declares that she is ready.")
                if girl:has_trait("Nymphomaniac") then
                    Dialog("You lead the men inside and you all stand at attention for the amazing sight before you.  She stands in the center of the room surrounded by pillows and cushions.  There isn't a stitch of clothing on her body, which shines from the coating of lubricant she has applied.  She waits for the door to close before she strikes a sexy pose and exclaims \"Lets see who can catch the greased courtesan first!\"")
                    Dialog("She slips and slides among the group, escaping holds to be caught by others. Before long everyone is panting and slippery.")
                    Dialog("She arranges you all laying on the floor and slides her body along the group taking turns and stopping at each man to ride his throbbing erection.  She expertly times each individual session and doesn't leave anyone wanting.  The Sun breaks through the window and you awake on her floor with her laying on top of the group.")
                    girl:libido(5)
                    girl:happiness(2)
                    girl:tiredness(10)
                else
                    if wm.Percent(girl:group()) then
                        Dialog("The group enters the room and forms a circle around the kneeling and eager girl in her favorite black lingerie.")
                        Dialog("She reaches up and frees your cock from it's cloth prison.  As she begins to suck and lick the tip she reaches to her sides and liberates the other mens's dicks as well.")
                        Dialog("She continues to suck your cock and stroke the men next to you with her hands as another man climbs beneath her and inserts himself into her vagina.  Another man kneels down behind her and penetrates her ass.  This continues through the night with men taking turns with all her holes.  ")
                        Dialog("The men compliment you on an excellent evening and blow kisses to the completely exhausted and sleeping woman on the bed.")
                        girl:happiness(5)
                        girl:libido(2)
                        girl:tiredness(5)
                    else
                        Dialog("The room has been arranged with a table in the center circled by chairs.  She bows \"It will be my pleasure to serve you tonight, gentlemen.\"  You sigh to yourself as you realize she has gotten the wrong idea.  She realizes her error as the group removes erect penises from their pants and move toward her.")
                        Dialog("To her credit she recovers from the shock quickly and lays back with her feet on the table.  The men take turns passing and sliding her around the table.  She allows the group to enter her every orifice but does little to enhance the experience.")
                        Dialog("You see the men out; lost in your thoughts of the disastrous performance.  One older gentlemen attempts to cheer you up by saying \"It was an enjoyable enough night, Sir.  Any gangbang is a good gangbang\"")
                        girl:happiness(-2)
                        girl:libido(-3)
                        girl:tiredness(4)
                    end
                end
            else
                Dialog("She refuses to be gangbanged like some dirty beggar on the streets.")
                RefusedSexAct(girl)
            end
        elseif choice == 8 then
            Dialog("\"Good evening, my dear.  I stopped by because I wanted to see how well you've learned to work the pole.\"")
            if girl:has_trait("Slow Learner") then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("She immediately drops to her knees and takes you into her mouth.  Well, It's not exactly what you had in mind, but there's no point to stopping her now.  She swallows every drop as you unload in her mouth.")
            else
                if girl:obey_check(wm.ACTIONS.SEX) then
                    wm.UpdateImage(wm.IMG.STRIP)
                    Dialog("She leads you to a comfortable chair next to a small stage with a tall, metal pole in the center.")
                    if wm.Percent(girl:strip()) then
                        -- TODO other traits?
                        if girl:has_trait("Big Boobs") then
                            Dialog("You glue your eyes to her wonderfully large breasts.  They sway and bounce as she begins her dance.")
                        end
                        if girl:has_trait("Great Figure") then
                            Dialog("As she strips away each layer of clothing you admire her perfectly proportioned body.  You become lost in her curves as they undulate before you.")
                        end

                        Dialog("She moves fluidly and naturally. You marvel as each piece of clothing isn't just forcibly removed, rather it slides off her body like cascading water.  She alternates slow deliberate stretches with incredible acrobatics on the pole.  Each bend and twirl seems blended together perfectly and you are lost in the experience.")
                        Dialog("She slides across the stage and continues her dance in your lap.  You can almost feel every part of her womanhood as she grinds against you crotch.  Your mind is swirling and just as you are about to explode...she comes to a complete stop, straddling your lap.")
                        Dialog("She looks deep into your eyes and her lips begin to part slightly... Suddenly she sits upright and sticks out her tongue with a wink.  \"How was that for a tease?\" She asks playfully")
                        girl:happiness(2)
                        girl:libido(5)
                        girl:tiredness(2)
                    else
                        Dialog("She nearly trips as she steps onto the stage.  She seems unsteady on tall stilettos, yet she still attempts to dance.")
                        Dialog("She trips and falls several times and fumbles every time she has to undo a clasp or button.")
                        Dialog("When she finally manages to remove all her clothing; she has given up on trying to move and stands clinging to the pole and doing a kind of slow wiggle.")
                        Dialog("After several minutes of her standing there naked, she stops and asks \"How was that?\"  You feel bad but she needs to hear the truth.  \"That was the worst striptease I have ever seen.  You are beautiful, but stripping is not simply standing naked in front of a crowd.  You have a lot to learn.\"")
                        girl:tiredness(5)
                        girl:happiness(-1)
                    end
                else
                    Dialog("She refuses to perform a striptease for you.")
                    RefusedSexAct(girl)
                end
            end
        elseif choice == 9 then
            Dialog("Go Back")
            return BrothelInteractChoice(girl)
        end
    elseif choice == 3  then -- Office
        InteractOffice(girl)
    elseif choice == 4 then
        Dialog("\"Hello My Dear, I wanted to ask you to come by my private quarters this evening.  Perhaps, we can get to know each other better.\" ")
        if girl:obey_check(wm.ACTIONS.SEX) then
            Dialog("She smiles slightly and nods her agreement.")
            local event = ChoiceBox("", "Cocktails", "Dinner")
            -- GOTO 70 // 5
        else
            Dialog("She declines your invitation.")
            RefusedSexAct(girl)
        end
    elseif choice == 5 then
        local action = ChoiceBox("",
                "Send her to the Arcane Citadel for Magic Lessons. COST: 500 gold",
                "Have her do Agility training with the local street acrobats.  Cost: 250 gold",
                "Work with the stevedore's at the shipyard to improve her Strength and Stamina.  COST: 300 gold",
                "Spend the day with a veteran adventurer for combat training. COST: 500 gold",
                "Go Back"
        )
        -- GOTO 80 // 6
    end
end

function InteractOffice(girl)
    local choice = ChoiceBox("", "Instruct her to tidy up your office", "Review her performance",
            "Send her on a mission", "Personal instruction", "Inspect her", "Go Back")
    if choice == 0 then
        if girl:obey_check(wm.ACTIONS.WORKCLEANING) then
            wm.UpdateImage(wm.IMG.MAID)
            Dialog("She puts on her maid's attire and sets about tidying up your office.  You always enjoy being around a women in a maid's outfit .")
        else
            Dialog("She  refuses to clean your office.")
            RefusedSexAct(girl) -- TODO this is no sex act
        end
    elseif choice == 1 then
        Dialog("You glance down at the parchment before you. \"Now, lets see...\"")
        if wm.Percent(girl:charisma()) then
            Dialog("\"You do an excellent job of maintaining your personal appearance and you make the customer's feel welcome....\"")
        else
            Dialog("Hmm...You could put a little more effort into you appearance and You make the customers uncomfortable...")
        end

        if wm.Percent(girl:obedience()) then
            Dialog("I will say, you do an excellent job of following direction...")
        else
            Dialog("I've had to repeat my commands to you in the past; Don't let this become a habit...")
        end

        if wm.Percent(girl:normalsex()) then
            Dialog("Your clients all seem to be very satisfied with your abilities...")
        else
            Dialog("I have noticed more than once that your clients leave with a dissatisfied look,  That's bad for business...")
        end

        if wm.Percent(girl:service()) then
            Dialog("You've been very diligent at helping with the household tasks...")
        else
            Dialog("I don't think I have seen you chipping in with the housework very often;  every girl needs to do her part...")
        end

        Dialog("Well,  That is where things stand for now.  Remember, there is always room for improvement.")
    elseif choice == 2 then
        local action = ChoiceBox("",
                "Assassinate a troublesome politician    COST: 1000 gold",
                "Sneak into the Hall of Records   COST: 750 gold",
                "Seduce/Bribe a government official    COST: 500 to 4000 gold",
                "Duel a Rival Brothel Girl  ENTRY FEE: 100 gold  PRIZE MONEY: 500 gold",
                "Accompany you into the catacombs",
                "Send her on a Quest",
                "Go Back"
        )
        if action == 0 then
            Dialog("I have a new target for you.")
            if girl:obey_check() then  -- TODO select action
                Dialog("\"A particular political figure has been starting a crusade to outlaw brothels within the city limits.\"  *You hand her the dossier*  \"Of course this is just a smoke screen for his true goal of raising the taxes on brothels everywhere.\"")
                Dialog("\"What's worse is he is using evidence gathered about me as his leverage to push this all through.\"  *You give her a hard look* \"I don't want him to wake up tomorrow.\"")
                wm.SetPlayerDisposition(-20)
                if girl:has_trait("Sadistic") then
                    Dialog("I will enjoy using his entrails for lingerie...")
                end

                Dialog("Before she moves to leave your office you hand her a bag of gold to cover expenses.")
                wm.AddPlayerGold(-1000)
                if girl:has_trait("Assassin") then
                    Dialog("She vanishes from view before she even makes it to the door.")
                    Dialog("Moving silently amongst the shadows of the city, She stalks her prey.  Patience is her tool and she waits for her moment.")
                    Dialog("Her prey turns down a crowded street, choked with the evenings drunken revellers.  She moves to strike.  Effortlessly she slithers through the crowd and falls in step with the mark.")
                    Dialog("Her thin, long blade slides quickly and cleanly through the ribs beneath his shoulder blades. The second blade slices the arteries in his neck.  He staggers a moment before falling silently to the ground.")
                    Dialog("The bodyguards who had been busy deflecting drunks turn back around to find their client lying dead in a large pool of blood.  They search the area for the killer, but find nothing.")
                    Dialog("She returns to your office several hours later and places the evidence against you and the politician's signet ring on your desk.")
                    wm.SetPlayerSuspicion(-40)
                elseif girl:has_trait("Sadistic") then
                    Dialog("The politician, carrying his latest bribe, enters his home from the servant's entrance to avoid prying eyes.  As the lamps illuminate the room he notices the droplets of blood.  Following the trail to the sitting room  he doesn't notice the bodies at first as his attention was on the floor and the now massive pool of blood.  He looks up.")
                    Dialog("He sees them now.  His wife, children, and servants are arranged around the room in a macabre tea party.  The bodies are so broken and bloody he almost can't recognize them.  Terror grips his heart and he turns to run.")
                    Dialog("He stops immediately as a woman blocks his path.  She is naked save for the coating of blood from her head to her feet.  \"Leaving the party so soon?\" she inquires.  He strikes at her with wild abandon, but he counters and knocks him to the floor. She picks up the bag of gold. \"Looks like the boss gets a bonus.\"")
                    Dialog("He awakes as a steel rod strikes him across his face.  He locks eyes with her.  There is a demonic fire in her eyes as she speaks \"Now, the party can start for real....\"")
                    Dialog("You walk into your office the next morning to find your obsessive little killer still naked, bloody and balled up on your couch sleeping soundly.  You find the bag of gold, the politicians head, and the evidence on your desk.")
                    wm.SetPlayerSuspicion(-25)
                    wm.AddPlayerGold(wm.Range(200, 800))
                elseif girl:has_trait("Nymphomaniac") then
                    Dialog("She finds the Politician sitting at the bar of an upscale establishment.  She sits down next to him and strikes up a conversation.")
                    if wm.Percent(girl:charisma()) then
                        wm.UpdateImage(wm.IMG.ORAL)
                        Dialog("She uses her talent for seduction to lure the man back to a secluded room of the bar.  She begins by sucking his dick, before asking him to lick her pussy as she continues to give him head.  She enjoys the sensation of his tongue on her clitoris.  She sits up and begins to ride his face.  She presses down hard as she grinds her pussy.  He tries to scream as his breathing becomes difficult but to no avail.  She writhes and rides him hard  long after he has suffocated.")
                        Dialog("She had heard that a man's erection gets bigger and harder after he suffocates.  To her delight, she finds it to be true and she rides the dead man's dick for hours.")
                        Dialog("She returns to you office exhausted and with her clothes in disarray.  she doesn't speak as she places the signet ring on your desk and walks back to her room.")
                        wm.SetPlayerSuspicion(-15)
                    else
                        Dialog("She attempts to seduce the mark, but continues to fail throughout the night.  She manages to annoy him to the point that he leaves the bar quickly out the back entrance.")
                        Dialog("She angrily follows him down the alley and attempts to catch up with him.  He begins to run and she chases him down streets and alleys.")
                        Dialog("Fleeing wildly what he assumes a crazy person he doesn't notice the steep ledge until it's too late.  She looks over the edge at his lifeless body, but some of the city guard has caught up to her now and after some questioning they bring her back to your office.")
                        Dialog("The guardsmen tell you her story.  \"It appears that the gentlemen in question tried to skip out on paying your whore and she chased him attempting to get her money.  It appears to be an accident but he took a bad fall and died.  You should keep your eye on this girl she has a temper.\"  As they turn to leave they add \"Oh, and  you will be fined 200 gold for tonight's disturbance.\"")
                        wm.AddPlayerGold(-200)
                    end
                elseif girl:has_trait("Strong Magic") then
                    Dialog("She whispers a few incantations and disappears from your office.")
                    Dialog("She reappears on a rooftop overlooking the Politician's residence.  She begins preparing her components as she waits for him to return home.")
                    Dialog("She watches as he walks into his home and greets his family.  She waits until he is alone in his study and teleports into the room.")
                    Dialog("He jumps as the mage appears before him.  He tries to scream as she begins casting another spell but he is paralysed.  She walks over to him and removes his signet ring and takes the evidence from his desk.")
                    if girl:has_trait("Aggressive") then
                        Dialog("She teleports back to her vantage point and begins casting the Inferno spell.  She can still see the man paralysed in place as the flames begin to consume the room and his flesh.")
                        Dialog("The man's family and servants can be see fleeing the home as the inferno quickly devours the building.  They call back at the house for the master of the house, but he will never answer...")
                    else
                        Dialog("She releases his body from the paralysis but before he can call out she paralyses his heart and lungs.")
                        Dialog("He pounds on his chest but to no avail and falls to the floor and expires.")
                    end
                    Dialog("She teleports back to your office and hands you the ring and papers.  She looks tired from the night of conjuring and you dismiss her back to her room.")
                    wm.SetPlayerSuspicion(-30)
                elseif girl:combat() > 60 then
                    wm.UpdateImage(wm.IMG.COMBAT)
                    Dialog("She adorns her weapons and armor and heads to her chosen ambush location.")
                    Dialog("As the man and his bodyguards round the corner she takes a moment to evaluate their abilities.")
                    if wm.Percent(girl:combat()) then
                        Dialog("She locks her eyes on the large barbarian walking beside her target.  That one first.  As they move within her striking distance she leaps down on the big bodyguard and plunges her sword through past his collar bone and into his vitals.")
                        Dialog("The Barbarian staggers and falls dead.  Before his body hits the ground she runs her sword through the point man's spine.  Alert and swords drawn she squares off against the remaining two guarding the politician.")
                        Dialog("The guard on her left makes the first move.  he swings high trying to  strike over her shield.  She drops herself low allowing the blade to pass overhead, while striking him in the abdomen with  her shield. He collapses in a heap. The remaining guard tries to take advantage of her supposed weakness and charges.")
                        Dialog("In one fluid motion she rises, cutting the guards throat, and bringing her blade to bear on the defenseless politician.  He begins to plead \"Please don't kill me! I'll *gurk* before he can finish his cowards talk she disembowels him.")
                        Dialog("She returns to your office with the signet ring and documents.  She places them on your desk and you hear her say something about washing off coward.")
                    else
                        wm.UpdateImage(wm.IMG.ANAL)
                        Dialog("As she leaps from her perch her foot catches on a bit of string and she lands splayed out on the ground before the group.  The two brutes in front grab her arms and drag her to stand before the boss.")
                        Dialog("\"What have we here? A whore who would be an assassin?\"  A big barbarian guard speaks \"No, boss.  From the look of her gear. I'd say she fancies herself a fighter.\"  The Boss replies \"very well Ass Crusher. See if she can fight.\"")
                        Dialog("The two holding her let go and the big man steps forward. \"Come on little girl. Let's see what you got.\"  She swings hard but the Barbarian merely catches the blade in his bare hand.")
                        Dialog("He pulls a tiny dagger from his belt and blocks each of her swings, but as he does he makes small cuts on her armor.  Her armor begins to fall from her body and before long she stands naked before the huge man.")
                        Dialog("\"Now I will use my sword.\" and he pulls his massive erection from his loincloth.  Angered, She swings at the dick but his hands catch her by the arms.  Her spins her around and rams his cock into her ass.")
                        Dialog("He releases a huge load into her anal cavity and leaves her sprawled out on the street.  The men walk away laughing.")
                    end
                else
                    Dialog("She positions herself across the street with a high powered magic crossbow and wait for her target to arrive at his office.")
                    if wm.Percent(50) then
                        Dialog("generic success")
                    else
                        Dialog("generic fail")
                    end
                end
            else
                Dialog("She refuses to kill anyone.")
                RefusedSexAct(girl)
            end
        elseif action == 1 then
            Dialog("Mission start")
            if girl:obey_check() then  -- TODO select action
                Dialog("Mission text")
                Dialog("Expenses/Cost   ")
                wm.AddPlayerGold(-750)
                if girl:has_trait("Incorporeal") then
                    Dialog("Incorporeal mission success")
                elseif girl:has_trait("Fleet of Foot") then
                    Dialog("Fleet of Foot")
                elseif girl:has_trait("Strong Magic") then
                    Dialog("magic success")
                elseif girl:has_trait("Assassin") then
                    Dialog("assassin")
                elseif girl:has_trait("Adventurer") then
                    Dialog("Adventurer success")
                elseif girl:combat() > 60 then
                    if wm.Percent(girl:combat()) then
                        Dialog("combat pass")
                    else
                        Dialog("combat fail")
                    end
                elseif wm.Percent(50) then
                    Dialog("generic success")
                else
                    Dialog("Generic fail")
                end
            else
                Dialog("mission refuse")
                RefusedSexAct(girl)
            end
        elseif action == 2 then
            Dialog("Seduce/Bribe Mission Start")
            if girl:obey_check() then  -- TODO select action
                Dialog("Seduce/Bribe Mission accept")
                if girl:charisma() > 50 and girl:beauty() > 60 then
                    if wm.Percent(girl:charisma()) then
                        Dialog("Seduce pass")
                        -- TODO normal sex
                        wm.AddPlayerGold(100, 200)
                        wm.SetPlayerSuspicion(-20)
                    else
                        Dialog("Seduce fail")
                        wm.AddPlayerGold(-500)
                        wm.SetPlayerSuspicion(10)
                    end
                else
                    Dialog("Bribery")
                    if wm.Percent(50) then
                        Dialog("Bribery success")
                        wm.AddPlayerGold(-wm.Range(2000, 4000))
                        wm.SetPlayerSuspicion(-30)
                    else
                        Dialog("Bribery fail")
                        wm.AddPlayerGold(-wm.Range(2000, 4000))
                        wm.SetPlayerSuspicion(10)
                    end
                end
            else
                Dialog("Refuse seduce/bribe mission")
                RefusedSexAct(girl)
            end
        elseif action == 3 then
            Dialog("Duel Mission Start")
            if girl:obey_check() then  -- TODO select action
                wm.AddPlayerGold(-100)
                if wm.Percent(girl:confidence()) then
                    Dialog("Confidence Pass Accept")
                else
                    Dialog("Confidence Fail Accept")
                end
                Dialog("Duel start")
                if girl:combat() > 50 then
                    if wm.Percent(girl:combat()) then
                        Dialog("Combat win")
                        wm.AddPlayerGold(500)
                    else
                        Dialog("Combat lose")
                    end
                elseif girl:magic() > 50 then
                    if wm.Percent(girl:magic()) then
                        Dialog("Combat win")
                        wm.AddPlayerGold(500)
                    else
                        Dialog("Combat lose")
                    end
                elseif girl:lesbian() > 30 then
                    -- TODO lesbian sex
                    Dialog("Lesbian Attack")
                    if wm.Percent(girl:lesbian()) then
                        Dialog("Lesbian Win")
                        wm.AddPlayerGold(500)
                    else
                        Dialog("Lesbian lose")
                    end
                else
                    Dialog("Generic duel / random var test")
                    if wm.Percent(50) then
                        Dialog("generic duel win")
                        wm.AddPlayerGold(500)
                    else
                        Dialog("generic duel lose")
                    end
                end
            else
                Dialog("Refuse duel")
                RefusedSexAct(girl)
            end
        elseif action == 4 then
            Dialog("Catacombs Start")
            if girl:health() < 40 then
                Dialog("You notice that she does not seem to be healthy enough for an adventure right now.")
                Dialog("\"On second thought lets take you down to the clerics instead.\"  You see to it that she receives some minor healing and escort her back to her room.")
                girl:happiness(-2)
                girl:health(10)
                girl:tiredness(-10)
            elseif girl:obey_check() then -- TODO select action
                Dialog("Catacombs accept")
                if girl:has_trait("Adventurer") then
                    Dialog("Adventurer start")
                    local success = wm.Range(1, 100)
                    if success >= 95 then
                        Dialog("Adventurer Critical success")
                        -- TODO AddRandomGirlToDungeon(Captured, 17, 50, true, false, false, false)
                        -- TODO GivePlayerRandomSpecialItem
                        wm.AddPlayerGold(wm.Range(500, 2000))
                    elseif success > 60 then
                        Dialog("adventurer success")
                        wm.AddPlayerGold(wm.Range(200, 1000))
                        -- TODO GivePlayerRandomSpecialItem
                    elseif success > 15 then
                        Dialog("adventurer moderate success")
                        wm.AddPlayerGold(wm.Range(50, 100))
                    else
                        Dialog("Adventurer critical fail")
                        -- TODO Beast Sex
                        girl:confidence(-5)
                        girl:charisma(-3)
                        girl:health(-60)
                    end
                elseif girl:combat() > 30 then
                    Dialog("Regular combat start")
                    local success = wm.Range(1, 100)
                    if success >= 98 then
                        Dialog("Regular combat success")
                        -- TODO GivePlayerRandomSpecialItem
                        wm.AddPlayerGold(wm.Range(400, 1500))
                    elseif success > 60 then
                        Dialog("Regular Combat Success.")
                        wm.AddPlayerGold(wm.Range(200, 1000))
                    elseif success > 15 then
                        Dialog("Regular Combat Failure")
                        wm.AddPlayerGold(wm.Range(100, 300))
                        girl:health(-50)
                    else
                        Dialog("Regular Combat Critical Failure")
                        -- TODO Beast Sex
                        girl:confidence(-5)
                        girl:charisma(-3)
                        girl:health(-90)
                    end
                elseif girl:magic() > 30 then
                    Dialog("Regular combat start")
                    local success = wm.Range(1, 100)
                    if success >= 95 then
                        Dialog("Magic critical success")
                    elseif success > 60 then
                        Dialog("magic success")
                    elseif success > 15 then
                        Dialog("magic failure")
                    else
                        Dialog("Regular Combat Critical Failure")
                        -- TODO Beast Sex
                    end
                else
                    Dialog("As you enter the labyrinth, she loses her nerve and flees for the safety of the brothel..  \"Very Well,\" you think to yourself.\"If she wants to show her ass to the enemy...\"")
                    Dialog("You find her in her room. Face down with her head under a pillow.  You climb onto the bed behind her and pull down her knickers.  As you remove your belt,  You tell her to stick her ass up in the air and keep her face down.")
                    Dialog("The spanking starts slowly; forceful but not abusive.  Stopping a moment you take a second to admire those lovely cheeks now turning a bright pink.  A different punishment comes to mind and the next thing she feels is not a sting but the stab of your cock entering her anus.  You enjoy the feeling of your cock moving back and forth in her tight ass.  You come hard and deep inside her.  \"Next time you want to turn tail and run, My Dear,  I shall have your tail again.\"")
                    -- TODO anal sex
                end
            else
                Dialog("catacombs refuse")
                RefusedSexAct(girl)
            end
        elseif action == 5 then
            Dialog("Stealing mission start")
            if girl:obey_check() then -- TOOD action
                Dialog("Stealing accept")
                local success = wm.Range(1, 100)
                if success >= 95 then
                    if girl:has_trait("Nymphomaniac") then
                        Dialog("Nympho stealing critical success")
                        -- TODO group sex
                    elseif girl:has_trait("Aggressive") then
                        Dialog("Aggressive stealing critical success.  beat up young couple and take girl")
                        -- TODO AddRandomGirlToDungeon Kidnapped 17 21 false false false false
                    elseif girl:has_trait("Fleet of Foot") then
                        Dialog("Fleet of Foot critical success")
                    end
                    Dialog("normal Stealing Critical Success")
                    wm.AddPlayerGold(wm.Range(500, 1500))
                    -- GivePlayerRandomSpecialItem
                elseif success > 55 then
                    if girl:has_trait("Fleet of Foot") then
                        Dialog("Fleet of foot stealing success")
                    end
                    Dialog("Stealing success")
                    wm.AddPlayerGold(wm.Range(250, 1000))
                elseif success > 15 then
                    Dialog("Stealing fail")
                    wm.SetPlayerSuspicion(10)
                else
                    Dialog("Regular Combat Critical Failure")
                    -- TODO Bondage Sex
                    if girl:has_trait("Nymphomaniac") then
                        girl:libido(15)
                    end
                    Dialog("Stealing Critical fail")
                    girl:health(-5)
                    girl:happiness(-6)
                    wm.SetPlayerSuspicion(20)
                end
            else
                Dialog("Stealing refuse")
                RefusedSexAct(girl)
            end
        elseif choice == 6 then
            Dialog("I want you to go on a Quest for me.  You needn't go far.  Try to make the world a little better. Oh, One more thing.  Be sure to wear the tabard of my house so every knows who you work for.")
            if girl:tiredness() > 30 then
                Dialog("She is not rested enough to start a quest.")
            elseif girl:health() < 50 then
                Dialog("She is not healthy enough to start a quest.")
            elseif girl:obey_check() then -- TODO select action
                Dialog("Quest accept")
                wm.SetPlayerDisposition(10)
                girl:tiredness(50)
                Dialog("where")
                local where = wm.Range(1, 10)
                if where == 1 then  -- Tavern
                    local what = wm.Range(1, 4)
                    if what == 1 then
                        Dialog("Tavern Owner quest gangster issues")
                        if girl:has_trait("Adventurer") then
                            Dialog("Tavern Owner Adventurer Attempt to defeat entire gang.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("Tavern Owner. Adventurer. Critical Success.")
                            elseif result > 60 then
                                Dialog("Tavern Owner. Adventurer. success")
                            elseif result > 10 then
                                Dialog("Tavern Owner. adventuer. failure.")
                            else
                                Dialog("Tavern Owner. adventurer. critical failure. it was a trap. girl gets fucked.")
                                -- TODO group sex
                            end
                        elseif girl:has_trait("Assassin") then
                            Dialog("Tavern Owner Assassin Attempt to kill gang boss.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("Tavern Owner. Assassin. Critical success.")
                            elseif result > 60 then
                                Dialog("Tavern Owner. Assassin. success.")
                            elseif result > 10 then
                                Dialog("tavern owner. assassin. failure.")
                            else
                                Dialog("tavern owner. assassin. critical failure.")
                                -- TODO group sex
                            end
                        elseif girl:has_trait("Charismatic") then
                            Dialog("Tavern Owner Charismatic Attempt to convince gang boss to ease off.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern owner. charismatic. critical success.")
                            elseif result > 60 then
                                Dialog("tavern owner. charismatic. success.")
                            elseif result > 10 then
                                Dialog("tavern owner. charismatic. failure.")
                            else
                                Dialog("Tavern owner. charismatic. critical failure.  Negotiations fail so she must use her mouth another way.")
                                -- TODO oral sex
                            end
                        else
                            Dialog("Tavern Owner Generic Attempt. Involve authorities.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern owner. generic. critical success.")
                            elseif result > 60 then
                                Dialog("tavern owner. generic success.")
                            elseif result > 10 then
                                Dialog("tavern owner. generic. failure.")
                            else
                                Dialog("tavern owner. generic. critical failure")
                                -- TODO group sex
                            end
                        end
                    elseif what == 2 then
                        Dialog("Tavern Barmaid quest domestic trouble")

                        if girl:has_trait("Nymphomaniac") then
                            Dialog("Barmaid Domestic Troubles Nymphomaniac attempt to spice up the couples marriage.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("Tavern barmaid. nympho critical success.")
                            elseif result > 60 then
                                Dialog("tavern barmaid. nympho. success.")
                            elseif result > 10 then
                                Dialog("tavern barmaid. nympho. failure.")
                            else
                                Dialog("tavern barmaid. nympho. critical failure.  husband divorces wife and swears to never marry again.")
                            end
                        elseif girl:has_trait("Adventurer") then
                            Dialog("Barmaid Domestic troubles Adventurer attempt to recover the lost important item.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("Tavern barmaid. adventurer. critical success.")
                            elseif result > 60 then
                                Dialog("tavern barmaid. adventurer. success.")
                            elseif result > 10 then
                                Dialog("tavern barmaid. adventurer. failure.")
                            else
                                Dialog("Tavern barmaid. adventurer. critical failure")
                            end
                        elseif girl:has_trait("Assassin") then
                            Dialog("Barmaid domestic troubles Assassin attempt to kill abusive husband")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern barmaid. assassin. critical success.")
                            elseif result > 60 then
                                Dialog("tavern barmaid. assassin. success.")
                            elseif result > 10 then
                                Dialog("tavern barmaid. assassin. failure.")
                            else
                                Dialog("tavern barmaid. assassin. critical failure.")
                            end
                        else
                            Dialog("Barmaid Domestic troubles Generic attempt to comfort her")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern barmaid. generic. critical success")
                                -- TODO lesbian sex
                            elseif result > 60 then
                                Dialog("tavern barmaid. generic. success.")
                            elseif result > 10 then
                                Dialog("tavern barmaid. generic. failure.")
                            else
                                Dialog("Tavern barmaid. generic. critical failure")
                            end
                        end
                    elseif what == 3 then
                        Dialog("Tavern wizard quest broken wand")
                        Dialog("Tavern Barmaid quest domestic trouble")

                        if girl:has_trait("Nymphomaniac") then
                            Dialog("Broken wand Nympho attempt to fix his limp \"wand\"")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern wizard. nympho critical success. wand is penis.  she revitalises the wizard and lets him try her ass.")
                                -- TODO anal sex
                            elseif result > 60 then
                                Dialog("tavern barmaid. nympho. success.")
                            elseif result > 10 then
                                Dialog("tavern barmaid. nympho. failure.")
                            else
                                Dialog("tavern barmaid. nympho. critical failure.  husband divorces wife and swears to never marry again.")
                            end
                        elseif girl:has_trait("Adventurer") then
                            Dialog("Broken wand Adventurer attempt to find a new wand")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern wizard. adventurer. critical success. get bonus item.")
                                -- TODO give player random special item
                            elseif result > 60 then
                                Dialog("tavern wizard. nympho success.  Wand is magical sex toy. she helps test and improve it for the wizard.")
                                -- TODO strip
                            elseif result > 10 then
                                Dialog("taven wizard. nympho. failure.  Wand is magical sex toy. she breaks it completely.")
                            else
                                Dialog("tavern wizard. nympho critical failure.  Wand is penis.  she realizes that the wand is his penis. she manages to excite him but rides him too hard and severely injures the wizards manhood.")
                                -- TODO normal sex
                            end
                        elseif girl:has_trait("Strong Magic") then
                            Dialog("Tavern. Wizard's Wand. Strong Magic attempt to repair wand.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern wizard. strong magic critical success.  fixes both of the wizards wands.")
                                -- TODO sex
                            elseif result > 60 then
                                Dialog("tavern wizard. strong magic. success. fix wizards wand.")
                            elseif result > 10 then
                                Dialog("tavern wizard. strong magic. failure.  destroys wand. fee of 250 gold.")
                            else
                                Dialog("tavern wizard. strong  magic. critical failure.  accidentally transforms wand into sex beast.")
                                -- TODO beast sex
                            end
                        else
                            Dialog("broken wand. generic attempt to locate a wand repair shop.")
                            local result = wm.Range(1, 100)
                            if result >= 95 then
                                Dialog("tavern wizard. generic critical success.  repair shop remembers wand and that it was warranteed.")
                            elseif result > 60 then
                                Dialog("tavern wizard. generic. success. find a local repair shop for the wizard.")
                            elseif result > 10 then
                                Dialog("tavern wizard. generic. failure. only repair shop is in a far away land.")
                            else
                                Dialog("tavern wizard. generic. critical failure.  wizard set a trap as she searched city. gang-banged by wizard and his students.")
                                -- TODO group sex
                            end
                        end
                    end
                end
            end
        end
        -- GOTO 41 // 7
    elseif choice == 3 then
        Dialog("\"After observing your work these past couple days, I've noticed some things that could use improvement.  I think you will benefit from my personal instruction in these areas\"  *You wink at her slyly*")
        if girl:obey_check(wm.ACTIONS.SEX) then
            Dialog("\"Please, join me on my office couch there and we will begin....\"")
            local action = wm.Range(1, 4)
            if action == 1 then
                Dialog("You lay back on your leather couch and pull your erect member from your trousers.   \"Now my dear, I'm going to teach you the right way to suck a cock...\"")
                -- TODO ORAL SEX
            elseif action == 2 then
                Dialog("You gently help her remove her clothing and lay her down on the sofa.  As you position yourself on top of her you begin explaining the finer points of vaginal sex.")
                -- TODO Normal Sex
            elseif action == 3 then
                Dialog("You chat with her a few moments on the merits of pain and pleasure sex.  She listens intently and doesn't notice as you bind her with the hidden couch restraints.  She notices as you cinch the last strap and looks up at you inquiringly.  \"No lesson is complete without a demonstration\"  You explain. \"Now be mindful of what I told you, as I demonstrate....\"")
                -- TODO BDSM sex
            else
                Dialog("You instruct her to kneel on the couch, facing away from you.  You admire her ass for a moment as you ask her to pull her dress up.  Reaching out, you slowly pull down her panties.  As you stimulate her clitoris and rub her juices on her anus, You explain some techniques to help her relax her muscles during anal sex.  You continue your lesson with a practical demonstration and you easily slide your penis into her ass...")
                -- TODO ANAL SEX
            end
        else
            Dialog("She refuses your offer of instruction.")
            RefusedSexAct(girl)
        end
    elseif choice == 4 then
        if girl:has_trait("Lolita") then
            Dialog("You look up at her, \"Are you sure this parchment is correct?  You barely look of legal age.\"")
        elseif girl:age() > 30 then
            Dialog("Looking down the parchment:  \"Hmm, a little older than most perhaps...\"")
        else
            Dialog("You seem to be the right age for our needs.")
        end

        Dialog("Remove your clothing.  I want to get a better look my investment.")

        if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
            -- TODO strip
            Dialog("She removes her clothing and stands nervously before you.")
            if girl:has_trait("Futanari") then
                Dialog("Oh! Hmm...I didn't realize you were a dick girl...")
            end

            if girl:beauty() >= 50 then
                Dialog("I see you put a lot of effort into you appearance....Yes, quite beautiful.")
            else
                Dialog("You're not much to look at are you.")
            end

            Dialog("You stand up and begin to slowly circle her.")
            if girl:has_trait("Great Figure") then
                Dialog("Ahh, you do have a nice figure don't you.  Very nicely proportioned.")
            end
            if girl:has_trait("Long Legs") then
                Dialog("Wow, those legs certainly do go all the way up, don't they.")
            end
            if girl:has_trait("Great Arse") then
                Dialog("You stop circling directly behind her and gently pressing between her shoulder blades until she is bent over with her hands on your desk.  Your eyes are glued to her ass.  Now, that is truly a magnificent ass.")
            end

            if girl:has_trait("Abnormally Large Boobs") then
                Dialog("Oh My Goddess!  Those are some gigantic breasts.  How do you even stand up straight?")
            elseif girl:has_trait("Big Boobs") then
                Dialog("You stare a moment at her tits.  You have truly been blessed with some healthy breasts.")
            elseif girl:has_trait("Small Boobs") then
                Dialog("You reach out to her chest and try to cup one of her breasts in your hand.  \"Well, a little less than a handful, but on you my dear they seem right.\"")
            end

            if girl:has_trait("Perky Nipples") then
                Dialog("You run your palms across her perky nipples.  You give them a little pinch.")
            elseif girl:has_trait("Puffy Nipples") then
                Dialog("Running your hands over her breasts you take a moment to admire her soft, puffy nipples.")
            end

            if girl:has_trait("Strong") then
                Dialog("You examine her finely toned muscles.  Squeezing her biceps you feel the strength there.  \"You are strong. That is good.\"")
            end

            if girl:has_trait("Horrific Scars") then
                Dialog("You have noticed them the entire time, but you debate whether or not to comment on the awful scars covering large portions of her body.")
            end

            Dialog("You sit back down and allow her to get dressed and leave your office.")
        else
            Dialog("She refuses to be inspected like some prize heffer.")
            RefusedSexAct(girl)
        end
    elseif choice == 5 then
        Dialog("Go Back")
        
    end
end

function RefusedSexAct(girl)
    local choice = ChoiceBox("", "Allow Her to Refuse.", "Scold Her", "Spank Her", "Take Her Clothing.",
        "Force Sex")
end

---@param girl wm.Girl
function Punish(girl)
    local choice = ChoiceBox("", "\"Guard the door, I'm going to teach this bitch a lesson!\"",
            "\"Boys, help me teach this whore some manners!\"",
            "\"Hold her! I'm going to beat some sense into her!\"",
            "\"Throw her into the beast pit for a while!\"",
            "\"Nothing.  She's had enough.  Take her back to her room.\""
    )
    if choice == 0 then
        PlayerRapeGirl(girl)
        wm.SetPlayerDisposition(-40)
        Dialog("Now you'll really hate me girl. 'Cuz I'm gonna fuck you, and you won't enjoy it.")
        Dialog("She tries to crawl away but you're quickly on top of her.  You pull out your throbbing cock and thrust it violently into her cunt. ")
        if girl:libido() > 75 then
            Dialog("Despite your words she moans with pleasure as you continually ram her tight pussy.  She shakes with a massive orgasm as you release your semen into her.")
            girl:happiness(30)
        else
            Dialog("You release a huge load into her and leave her sobbing on the floor.")
            girl:happiness(-50)
            girl:pcfear(30)
        end
    elseif choice == 1 then
        -- TODO Group Sex
        wm.SetPlayerDisposition(-40)
        Dialog("You and your men spend a few hours passing her around the room.")
        Dialog("For the grand finale you all stand around her and spray her with load after load of cum.")
        if girl:has_trait("Nymphomaniac") then
            Dialog("She lies on the floor breathing heavily from the marathon of orgasms; both the group's and her's.")
            girl:libido(5)
            girl:happiness(2)
            girl:tiredness(10)
        else
            Dialog("She lies gasping and gagging on the floor humiliated and sticky from sweat and semen.")
            girl:happiness(-30)
            girl:tiredness(20)
            girl:pcfear(5)
        end
    elseif choice == 2 then
        wm.SetPlayerDisposition(-40)
        Dialog("Your men seize her arms and drag her to stand before you.")
        if wm.Percent(girl:bdsm()) then
            Dialog("She cries out with every lash from the bullwhip, but each stroke sends a wave of pleasure as well. " ..
                    "Soon her thighs are covered with her hot juices.  Since she seems to be doing well with this you reward " ..
                    "her by ramming the handle of the whip into her pussy.  She  cums so hard that you men almost drop her as her legs give out.")
            girl:health(-10)
            girl:happiness(2)
            girl:tiredness(10)
            girl:obedience(5)
        else
            Dialog("She begs you to stop as you land blow after blow upon her. you are careful to avoid her face (wouldn't want to damage your earnings).")
            Dialog("Your men drag her limp body back to her room and toss her in like a ragdoll.")
            girl:happiness(-60)
            girl:tiredness(30)
            girl:health(-40)
            girl:pcfear(30)
        end
    elseif choice == 3 then
        -- BEST SEX
        Dialog("Player: \"Let's see if a night with Cthulu's cousin improves her mood?\"")
        Dialog("Your men pick the girl up from the floor and haul her off to the beast pit.")
        Dialog("beast sex dialog")
        girl:happiness(-20)
        girl:tiredness(20)
        girl:pcfear(10)
    elseif choice == 4 then
        if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
            -- NUDE
            Dialog("However, they don't like when anyone disrespects their employer.  Before they leave her room they tear away her clothing and leave her naked and sobbing.")
        else
            Dialog("Your men don't like when someone disrespects their boss.  They tell her that they are going to strip her naked when they get to her room...")
            Dialog("However, as the group rounds the corner to the girl's dormitorys she makes a quick dash for her room and bolts the door behind her.  Your men pound on the door yelling for a bit, but eventually walk away.")
        end
    end
end