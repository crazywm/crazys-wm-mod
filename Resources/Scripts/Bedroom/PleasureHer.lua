function RepayFinger(girl)
    wm.UpdateImage(wm.IMG.HAND, ImageOptions.HETERO)
    if girl:skill_check(wm.SKILLS.HANDJOB, 40) then
        Dialog("She uses the juices between her thighs to lubricate her hand, and then starts stroking your member. " ..
                "If only everybody could appreciate your hard work the way " .. girl:firstname() .. " does.")
        girl:handjob(1)
    else
        Dialog("Despite her enthusiasm, the handjob you receive is more uncomfortable than enjoyable. However, " .. girl:firstname() ..
                " insists that she provide you the 'perfect' experience, so you use this opportunity to impart some wisdom. By the end, her " ..
                "technique has markedly improved.")
        girl:handjob(5)
    end
end

function RepayLick(girl)
    if girl:skill_check(wm.SKILLS.ORAL, 40) then
        wm.UpdateImage(wm.IMG.SUCKBALLS, ImageOptions.HETERO)
        Dialog("She begins by gently licking and kissing the tip of your cock. Her tongue then traces down the length of your shaft and draws circles around your balls.")
        Dialog("She gently sucks each ball into her mouth in turn. She turns her head and slides her lips back forth on your shaft. She kisses the tip again and slowly slides you into her mouth.")
        wm.UpdateImage(wm.IMG.ORAL, ImageOptions.HETERO)
        Dialog("You're not sure how long this pleasure lasted, but soon you feel the volcano about to erupt. She senses it too and pulls your cock from her mouth and gently strokes it with her hands until you explode all over her face.")
        wm.UpdateImage(wm.IMG.CUMSHOT, ImageOptions.HETERO)
        Dialog("What a fantastic way to end your evening.")
        girl:oral(1)
    else
        wm.UpdateImage(wm.IMG.ORAL, ImageOptions.HETERO)
        Dialog("Despite her enthusiasm, the blowjob you receive is more uncomfortable than enjoyable. However, " .. girl:firstname() ..
                " insists that she provide you the 'perfect' experience, so you use this opportunity to impart some wisdom. By the end, her " ..
                "technique has markedly improved.")
        girl:oral(5)
    end
end

function HornyHateActionFinger(girl)
    if wm.GetPlayerDisposition() < -33 then
        Dialog("You unceremoniously shove three of your fingers into her pussy. \"Damn, you really are quite the slut\". " ..
                "You continue your mockery as you show her your fingers, dripping wet. \"Well, I'll give you what you want!\"\n" ..
                "And with that, you start finger-fucking her in earnest.")
    else
        Dialog("You start inserting one finger. She is so wet that you decide she is ready for more, and add two more fingers. " ..
                "Now, the finger-fucking begins in earnest.")
    end
end

function HornyHateActionLick(girl)
    if wm.GetPlayerDisposition() < -33 then
        Dialog("You place your head between her thighs and start lapping up her juices. She tries to move her crotch away from your tongue. " ..
                "\"Stay right where you are\", you instruct, \"or I'll make things unpleasant for you!\" To emphasize your point, you bite down on her clit, and are " ..
                "rewarded with a pained yelp. The following noises might have been sobs or moans, but at least she's keeping still.")
    else
        Dialog("You place your head between her thighs and start lapping up her juices. She tries to move her crotch away from your tongue, so you firmly " ..
                "place your hands behind her hips. \"None of that, my dear\", you chide her and continue your work.")
    end
end

-- TODO replace this by an actual TextRepo kind of thing
local PleaseData = {
    image = {lick=wm.IMG.LICK, finger=wm.IMG.FINGER},
    horny_love_action = {
        lick=[[
She doesn't look like she is interested in any sort of foreplay, so without further ado you place your head between her legs and start licking her pussy.
{firstname} lets out a sigh and starts playing with her already-hard nipples.
It doesn't take long until you feel her muscles contract and spasm under your tongue.
You take this as encouragement, probing deeper into her cleft. Her loudening moans are your reward.
        ]],
        finger=[[
She doesn't look like she is interested in any sort of foreplay, so without further ado you begin to move your index finger
in and out of her vagina, using your thumb to massage her clit. {firstname} lets out a sigh and starts playing with her already-hard nipples.
It doesn't take long until you feel her muscles contract and clamp down on your finger. This is encouragement for you to insert a second finger,
and her moans let you know just how enthusiastically she approves.]]
    },
    horny_love_repay = {
        lick=[[
After her fourth orgasm, she softly pushes your head away. "I think it's time for me to repay the favour
She unzips your pants and pulls out your cock, already standing at attention. "Seems like someone already got excited", she winks.
        ]],
        finger=[[
After her fourth orgasm, she softly pushes your hand away. "I think it's time for me to repay the favour
She unzips your pants and pulls out your cock, already standing at attention. "Seems like someone already got excited", she winks.
        ]]
    },
    horny_love_repay_handler = {lick=RepayLick, finger=RepayFinger},
    neural_action_first = {
        lick=[[
Her breathing starts to speed up, and you notice that she is getting wet. You brush your lips
lightly against her labia, while continuing to caress her nipples. When the first soft moan escapes her,
you know she is ready. You place your mouth over your pussy and slowly slide your tongue into her.
        ]],
        finger=[[
Her breathing starts to speed up, and you notice that she is getting wet. You brush your fingers
lightly against her outer lips, all-the-while still continuing your kisses. When the first soft moan escapes her
lips, you know she is ready. You carefully insert one finger, moving it slowly at first, but speeding up as her
excitement grows.]]
    },
    neural_action_second = {
        lick="You speed up your tonguing, and can feel her getting more aroused. After a few minutes, you manage to bring her to orgasm.",
        finger="She is moaning ecstatically now, and you add a second finger to bring her over the edge."
    },
    refuse = {
        lick="She refuses to let you lick her cunt. \"Don't touch me!\"",
        finger="She refuses to let you finger her. \"Don't touch me!\""
    },
    horny_hate_action_handler = {lick=HornyHateActionLick, finger=HornyHateActionFinger},
    horny_hate_fisting_start = {
        lick="You stop your licking, and start pushing in a finger, then another, and finally your entire hand.",
        finger="You add a forth finger and a fifth, ignoring her protestations, and finally you push in your entire hand. "
    },
    horny_neutral_action = {
        lick=[[
Her juices start flowing almost immediately, so without much further ado you place your mouth on her lower lips and start licking.
Before long, you have her screaming in the throes of passion.]],
        finger=[[
Her juices start flowing almost immediately, so without much further ado you push two fingers into her vagina, curling them rhythmically inside her.
Before long, you have her screaming in the throes of passion.]]
    },
    nohorny_evil_action = {
        lick=[[
She doesn't seem responsive to your stimulation, but you came here to eat pussy, so you are
going to eat pussy, no matter what. "You're a little dry, but I can fix that", you tell {firstname}, before calling one of
your servants. A few minutes later, you are licking your favourite hot sauce off her pussy lips. It's rather enjoyable, to you at least.
]],
        finger=[[
She doesn't seem responsive to your stimulation, but you came here to finger-fuck, so you are
going to finger-fuck her, no matter what. You shove three fingers into her dry pussy, and are rewarded with a cry of pain.
It is a good thing that to your ears they sound just a good as moans of pleasure -- well, maybe not good for her.
]]
    },
    neural_neutral_action = {
        lick=[[
It takes you a while, but after a few minutes she is breathing heavily. You decide it she is ready for your fingers.
With a good does of spit for additional lubrication, you start slowly probing her with your index finger.
]],
        finger=[[
She doesn't seem responsive to your stimulation, but you came here to finger-fuck, so you are
going to finger-fuck her, no matter what. You shove three fingers into her dry pussy, and are rewarded with a cry of pain.
It is a good thing that to your ears they sound just a good as moans of pleasure -- well, maybe not good for her.
]]
    },
    neural_neutral_orgasm = {
        lick=[[
After half an hour of munching on her pussy, you manage to bring her to orgasm.
This probably would have been easier if she liked you more.
]],
        finger=[[
After half an hour of vigorous finger-fucking, you manage to bring her to orgasm.
This probably would have been easier if she liked you more.
]]
    }
}

function PleaseHer_Horny(girl, kind)
    local thanks_text = ""
    if girl:pclove() > 80 then
        if girl:is_slave() then
            thanks_text = "\"You truly are the best master I could hope for!\""
        else
            thanks_text = "\"You truly are the best employer I could hope for!\""
        end
    elseif not girl:is_slave() then
        thanks_text = "\"You sure know how to take care of your employees.\""
    end
    wm.UpdateImage(wm.IMG.STRIP)
    Dialog("You haven't even finished stating your intentions when " .. girl:firstname() .. " starts to undress " ..
            "and presents you her already glistening-wet pussy. " .. thanks_text)
    wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
    girl:pcfear(-1)
    Dialog(girl:format(PleaseData.horny_love_action[kind]))
    if wm.Percent(50) then
        wm.UpdateImage(wm.IMG.REST)
        Dialog("At the end of your session, " .. girl:firstname() .. " seems barely coherent. She mumbles something that appears to " ..
                "contain the words 'amazing' and 'thank you', rests her head on her pillow and is asleep before you even have a chance respond. " ..
                "You leave the room with a self-satisfied smile.")
        girl:pclove(3)
        SheJustCame(girl, 20)
    else
        girl:pclove(1)
        SheJustCame(girl, 10)
        if girl:pclove() > 60 and wm.Percent(50) and not girl:has_trait("Lesbian") then
            Dialog(girl:format(PleaseData.horny_love_repay[kind]))
            PleaseData.horny_love_repay_handler[kind](girl)
        else
            Dialog("After a few more orgasms, you apologize and tell her that you have to leave to fulfill your other duties. " ..
                    "\"That's alright, I'm not sure how much more of this I could take anyway\", she jokes.")
        end
    end
end

function PleaseHer_Neutral(girl, kind)
    local horny_text = ""
    if girl:libido() < 40 then
        horny_text = "She doesn't seem overly " ..
                " excited, so you start by placing soft kisses on her stomach, then on her breasts and inner thighs."
    end
    wm.UpdateImage(wm.IMG.FONDLE, ImageOptions.HETERO)
    Dialog("You ask " .. girl:firstname() .. " to undress and lie on her back. " .. horny_text)
    AdjustLust(girl, 3)
    wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
    Dialog(girl:format(PleaseData.neural_action_first[kind]))
    Dialog(girl:format(PleaseData.neural_action_second[kind]))
    if girl:pclove() > 60 then
        Dialog("\"That was amazing\". You smile. \"Want to go for round two?\"")
        AdjustLust(girl, 3)
        girl:happiness(4)
        girl:tiredness(4)
        girl:pcfear(-1)
        girl:libido(1)
    else
        wm.UpdateImage(wm.IMG.REST)
        Dialog("She collapses on her bed, tired but content. Satisfied with your work, you bid her a good night.")
        girl:happiness(2)
        girl:tiredness(1)
    end
end

function PleaseHer_HornyHate(girl, kind)
    local add_text = ""
    if wm.GetPlayerDisposition() < -50 then
        add_text = "To you, it looks almost cute, and her humiliation just turns you on."
    end
    Dialog(" Despite her obvious discomfort at your presence, her juices start flowing almost immediately. " ..
            "\"It's nice to see you are as excited as I am,\" you mock her, earning yourself a murderous glare. Its effect is somewhat diminished " ..
            " as she can't help but let out a soft moan, her face turning red with embarrassment. " .. add_text)
    wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
    PleaseData.horny_hate_action_handler[kind](girl)

    if wm.GetPlayerDisposition() < -65 then
        Dialog("Her attempts to resist your stimulation are futile, and before long she is moaning loudly and you can feel her " ..
                "spasming as she reaches her first climax. Now that she is properly warmed up, you can start taking things to the next level.")
        wm.UpdateImage(wm.IMG.FISTING, ImageOptions.HETERO)
        Dialog(PleaseData.horny_hate_fisting_start[kind] ..
                "That alone brings her over the edge again, her muscles clamping down so hard on your hand that you couldn't take it out even if you wanted to. " ..
                "But of course, that wasn't your intention anyway.")
        Dialog("As you start moving your fingers inside her, your other hand moves to her nipple, pinching hard. In her extasy, she barely seems to notice.")
        local choice = ChoiceBox("Continue?", "Yes, I'm having fun", "No, I think I've traumatized her enough")
        if choice == 0 then
            Dialog("You wait till you can feel her approaching climax again, then you locate her other nipple and start pulling while pressing" ..
                    " your nails in, as strongly as you dare without causing permanent injury. This at least gets a reaction. Her cry of pain gets swept "..
                    " away as she explodes in moans of pleasure.")
            wm.UpdateImage(wm.IMG.FINGER, ImageOptions.HETERO)
            Dialog("By now, you cock is almost painfully hard and throbbing, so you decide to take care of yourself. While you masturbate, you continue " ..
                    " to massage her pussy and manage to elicit two more orgasms. Finally, you deposit your load across her stomach. At this point, " .. girl:firstname() ..
                    " is in an almost catatonic state, so you decide that she's had enough. ")
            wm.UpdateImage(wm.IMG.BED)
            Dialog("You clean yourself up, pull up your pants, and leave her room, making a mental note to send of of your other girls to check on her later.")
            girl:pclove(-35)
            girl:happiness(-35)
            girl:tiredness(15)
            girl:bdsm(2)
            wm.SetPlayerDisposition(-20)
            girl:pcfear(4)
        else
            wm.UpdateImage(wm.IMG.BED)
            Dialog("You decide that she's had enough and remove your hand. You can't resist the urge to smear her own juices across her face as a parting gift. " ..
                    "By the time you leave her room, you can hear her sobbing uncontrollably.")
            girl:pclove(-20)
            girl:happiness(-20)
            girl:tiredness(5)
            girl:pcfear(2)
            wm.SetPlayerDisposition(-10)
        end
    else
        Dialog("Her attempts to resist your stimulation are futile, and before long she is moaning loudly and you can feel her " ..
                "muscles contracting as she climaxes.")
        wm.UpdateImage(wm.IMG.BED)
        Dialog("You decide that you have tormented her enough for today, and leave her to clean herself up.")
        girl:happiness(-10)
        girl:pclove(-5)
        girl:tiredness(2)
        girl:pcfear(1)
    end
end

function PleaseHer_HornyNeutral(girl, kind)
    -- she is very horny, and she doesn't like you, but also doesn't hate you too much
    wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
    Dialog(girl:format(PleaseData.horny_neutral_action[kind]))
    -- if she really needed it, and doesn't hate you too much, the effect is positive
    if girl:pclove() > -33 and girl:libido() > 80 then
        girl:pclove(1)
        girl:happiness(1)
    else
        girl:pclove(-1)
        if girl:pclove() > -33 then
            girl:happiness(-1)
        else
            girl:happiness(-3)
        end
    end
end

function PleaseHer_NoHornyEvil(girl, kind)
    if wm.GetPlayerDisposition() > -50 then
        -- she doesn't like you and isn't horny, and you are not too evil
        local stare = "an icy stare"
        if girl:pclove() < - 33 then
            stare = "a murderous glare"
        end
        local aphro = ""
        if wm.GetPlayerDisposition() < 0 then
            aphro = "Or maybe you just have to give her an aphrodisiac next time."
        end
        Dialog("The only thing that earns you is " .. stare .. " from " .. girl:firstname() .. ". " ..
                "If she liked you more, you may have had more success easing her into it. " .. aphro)
        girl:happiness(-1)
        girl:libido(1)
        girl:pclove(-1)
    else
        -- you are evil
        wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
        Dialog(girl:format(PleaseData.nohorny_evil_action[kind]))
        wm.UpdateImage(wm.IMG.BED)
        Dialog("When you are done, " .. girl:name() .. " is a sobbing mess on her bed, and you imagine her pussy will feel sore for the next days. " ..
                " She really is going to hate you!")
        girl:pclove(-20)
        girl:happiness(-20)
        girl:tiredness(10)
        girl:health(-4)
        girl:pcfear(1)
        wm.SetPlayerDisposition(-2)
    end
end

function PleaseHer(girl, action)
    if girl:obey_check(wm.ACTIONS.SEX) then
        girl:experience(4)
        girl:tiredness(1)
        wm.UpdateImage(wm.IMG.BED)
        if girl:libido() >= 65 and girl:pclove() >= 50 then
            -- she is horny and likes you
            PleaseHer_Horny(girl, action)
        elseif girl:libido() < 65 and girl:pclove() > 0 then
            -- she isn't horny and doesn't hate you
            PleaseHer_Neutral(girl, action)
        else
            -- she isn't fond of you,
            wm.UpdateImage(wm.IMG.FONDLE, ImageOptions.HETERO)
            local pclove_text = ""
            if girl:pclove() < - 33 or girl:dignity() > 50 then
                pclove_text = "She seems disgusted by the idea, but wisely holds her tongue. "
                wm.SetPlayerDisposition(-1)
            end
            Dialog("You order " .. girl:firstname() .. " to undress and lie on her back. " .. pclove_text ..
                    "\"Let's get you in the mood, shall we,\" you proclaim as you start suckling on her breasts. At the same time, you move your hands between her " ..
                    "legs and start caressing her lower lips.")
            if girl:libido() > 50 and ((girl:pclove() < -33 and wm.GetPlayerDisposition() < -33) or girl:pclove() < - 66)  then
                -- she is horny but hates you, and your aren't a nice person, or she just really hates you
                PleaseHer_HornyHate(girl, action)
            elseif girl:libido() > 50 then
                PleaseHer_HornyNeutral(girl, action)
            elseif girl:libido() < 20 then
                -- she doesn't like you and isn't horny
                PleaseHer_NoHornyEvil(girl, action)
            else
                -- she isn't very horny, and she doesn't like you
                Dialog(girl:format(PleaseData.neural_neutral_action[action]))
                wm.UpdateImage(PleaseData.image[kind], ImageOptions.HETERO)
                girl:happiness(-1)
                girl:pclove(-1)
                Dialog(girl:format(PleaseData.neural_neutral_orgasm[action]))
            end
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog(PleaseData.refuse[action])
        return girl:trigger("girl:refuse")
    end
end


function FingerHer(girl)
    return PleaseHer(girl, "finger")
end

function EatHerOut(girl)
    return PleaseHer(girl, "lick")
end