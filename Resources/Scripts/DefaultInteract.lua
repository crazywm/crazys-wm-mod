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
        return girl:trigger("girl:chat.dungeon")
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
                PlayerFucksGirl_Group(girl)
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
            PlayerFucksGirl(girl)
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
           -- threesome (placeholder)
            return DungeonInteractChoice(girl)
        elseif choice == 7 then
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
        return girl:trigger("girl:chat.brothel")
    elseif choice == 2 then
         if girl:pclove() > 90 then
            choice = ChoiceBox("",
                               "Make love",
                               "Watch her have sex with another girl",
                               "Watch her have sex with your pet sex beast",
                               "Play your favorite bondage roleplay",
                               "Ask her to lick your love shaft",
                               "Worship her cute little ass hole",
                               "Watch her masturbate for you",
                               "Have some others join the two of you for an orgy",
                               "Have her tease you",
                               "Go Back")
         else
            choice = ChoiceBox("",
                               "Have Sex",
                               "Ask her to have sex with another girl",
                               "Ask her to have sex with a beast",
                               "Ask her to be in a bondage session",
                               "Ask for a blowjob",
                               "Have anal sex",
                               "Ask her to masturbate while you watch",
                               "Ask her to be in a gangbang",
                               "Ask her to show you her skills on the stripper pole",
                               "Go Back")
         end
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
                return girl:trigger("girl:refuse")
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
                return girl:trigger("girl:refuse")
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
                    -- TODO what happens if the player doesn't have the funds?
                    wm.TakePlayerGold(wm.Range(50, 75))
                end
            else
                Dialog("She refuses to have sex with a beast.")
                return girl:trigger("girl:refuse")
            end
        elseif choice == 3 then
            Dialog("As you enter her room you accidently jostle the crate you're carrying.  Hearing the clinking and clanging she eyes the crate.  \"What do you have got in the crate?\" She asks.")
            Dialog("\"Take a look.\" you say.  Her eyes widen at the sight of the whips, chains, and harnesses.")
            if girl:obey_check(wm.ACTIONS.SEX) then
                PlayerFucksGirl_BDSM(girl)
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
                return girl:trigger("girl:refuse")
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
                    return girl:trigger("girl:refuse")
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
                    return girl:trigger("girl:refuse")
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
                    return girl:trigger("girl:refuse")
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
                return girl:trigger("girl:refuse")
            end
        elseif choice == 6 then
            Dialog("Her back is toward you as you enter the room.  You notice that one of her hands  is between her legs.")
            Dialog("You sneak around her side to get a better view.  A few moments later she gasps and jumps up as she notices you standing there.")
            Dialog("\"Oh, you don't have to stop on my account.\"  you say with a wink.")
            if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
                wm.UpdateImage(wm.IMG.STRIP)
                Dialog("She relaxes and grins devilishly.  \"Alright, I hope you will enjoy the show,\" she then adds \"but no touching.\"")
                if wm.Percent(girl:strip()) then
                    wm.UpdateImage(wm.IMG.MAST)
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
                girl:service(2)
            else
                if girl:has_trait("Meek") then
                    Dialog("She blushes to a deep red and pushes you out of the room without ever making eye contact.")
                else
                    Dialog("She immediately covers herself and demands you leave her room.")
                    return girl:trigger("girl:refuse")
                end
            end
        elseif choice == 7 then
            Dialog("You ask the group of men to wait in the hall as you enter her room.")
            Dialog("You find her relaxing on her bed looking through some of her lingerie.  She looks up as you speak.\"I wonder if you could help me, my dear.  I've got a group of gentlemen outside and I was wondering if you could help me entertain them?\"")
            if girl:obey_check(wm.ACTIONS.SEX) then
                PlayerFucksGirl_Group(girl)
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
                return girl:trigger("girl:refuse")
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
                    return girl:trigger("girl:refuse")
                end
            end
        elseif choice == 9 then
            Dialog("Go Back")
            return girl:trigger("girl:interact.brothel")
        end
    elseif choice == 3  then -- Office
        return girl:trigger("girl:interact.office")
    elseif choice == 4 then
        Dialog("\"Hello My Dear, I wanted to ask you to come by my private quarters this evening.  Perhaps, we can get to know each other better.\" ")
        if girl:obey_check(wm.ACTIONS.SEX) then
            return girl:trigger("girl:interact.private")
        else
            Dialog("She declines your invitation.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        return girl:trigger("girl:training")
    else
        ScoldGirl(girl)
    end
end

function Refuse(girl)
    local choice = ChoiceBox("", "Allow Her to Refuse.", "Scold Her", "Spank Her", "Take Her Clothing.",
        "Force Sex")
    if choice == 0 then
        Dialog("You say nothing and go about your other business.")
        wm.SetPlayerDisposition(3)
    elseif choice == 1 then
        ScoldGirl(girl)
    elseif choice == 2 then
        Dialog("\"You will learn to obey me!\" You yell as you grab her arm and drag her across your knees.")
        Dialog("She begins to cry as you pull her clothing and expose her ass.  \"Perhaps this will teach you some discipline.\" ")
        Dialog("You smack her ass until her cheeks are rosy red and send her away sobbing.")
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:health(-1)
        girl:obedience(3)
    elseif choice == 3 then
        Dialog("\"Oh I see.  You feel you have no need to obey me?\"  You ask calmly.  \"Perhaps then you also have no need for the things I have given you?\" ")
        Dialog("\"I'll just be taking a few things back then.\"  You order your guards to strip her naked and make her stand in front of the brothel all day and night.")
        Dialog("As she is lead outside you remark. \"Perhaps next time you will be more mindful of who it is that takes care of you.\"")
        wm.UpdateImage(wm.IMG.NUDE)
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:pchate(5)
        girl:pclove(-5)
        girl:obedience(5)
    elseif choice == 4 then
        Dialog("Your eyes flash with rage. \"You dare refuse? I'll show you what happens to whores that refuse to do their master's bidding\"")
        Dialog("You knock her down and begin to tear away her clothing.  She cries out as you force yourself inside her.")
        Dialog("You release you semen deep inside her and leave her sobbing on the floor.")
        PlayerRapeGirl(girl)
        girl:happiness(-5)
        girl:obedience(5)
        girl:pchate(10)
        girl:pclove(-10)
        girl:pcfear(10)
    end
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
        PlayerFucksGirl_Group(girl)
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
        -- Flavour text doesn't mention penis-in-vagina penetration,
        -- so don't call PlayerFucksGirl_BDSM()
        wm.UpdateImage(wm.IMG.BDSM)
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
        wm.UpdateImage(wm.IMG.BEST)
        Dialog("Player: \"Let's see if a night with Cthulu's cousin improves her mood?\"")
        Dialog("Your men pick the girl up from the floor and haul her off to the beast pit.")
        Dialog("beast sex dialog")
        girl:happiness(-20)
        girl:tiredness(20)
        girl:pcfear(10)
    elseif choice == 4 then
        if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
            wm.UpdateImage(wm.IMG.NUDE)
            Dialog("However, they don't like when anyone disrespects their employer.  Before they leave her room they tear away her clothing and leave her naked and sobbing.")
        else
            Dialog("Your men don't like when someone disrespects their boss.  They tell her that they are going to strip her naked when they get to her room...")
            Dialog("However, as the group rounds the corner to the girl's dormitorys she makes a quick dash for her room and bolts the door behind her.  Your men pound on the door yelling for a bit, but eventually walk away.")
        end
    end
end

