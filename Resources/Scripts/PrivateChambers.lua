
---@param girl wm.Girl
function Drinks(girl, dinner_already)
    Dialog("You invite her to make herself comfortable on your sofa as you mix some cocktails before joining her on the couch.")
    local what = wm.Range(0, 100)
    if what > 90 then
        Dialog("She becomes very comfortable and giddy as she downs drink after drink.  However, she over does it and before long she passes out on the couch.")
        return SheIsAsleep(girl)
    elseif what > 70 then
        Dialog("She opens up more and more with each drink.  Her words become slurred and the topics of conversation become more racy.  There can be no doubt that this girl is completely drunk.")
        local choice = ChoiceBox("", "Take Advantage of her intoxication.", "Lead her back to her room")
        if choice == 0 then
            Dialog("You sense an opportunity in her inebriated state.  You start rubbing her shoulders and back.  Soon your hands are roaming towards her breasts...")
            if wm.Percent(60) then
                PlayerfucksGirl(girl)
                Dialog("She lets out a soft moan as your fingers caress her nipples thru the fabric of her top. Your other hand sneaks along her inner thigh;  meeting no resistance your fingers slide easily under her panties and your stimulate her clitoris.")
                Dialog("Before long you are both kissing and probing each other...having lost all sense of time you come to your senses panting and gasping on the floor by your couch.  You look down to see that she her breathing heavily and deeply in an exhausted sleep.")
            else
                Dialog("A sharp smack on your hand and a harsh look in her eyes, tells you that your gamble has failed.  She regains a little sobriety and demands you take her back to her room.")
                Dialog("You escort her back to her room and you both pause just outside her door...")
                return HerRoom(girl)
            end
        elseif choice == 1 then
            Dialog("You help her to her feet and lead her to her room...")
            return HerRoom(girl)
        end
    elseif what > 10 then
        Dialog("The two of you pass the time enjoying each others company and a couple drinks.  She seems a little tipsy.")
        if dinner_already then
            AfterDinner(girl)
        else
            return Dinner(girl)
        end
    else
        Dialog("She politely asks for Non-alcoholic drinks.  Even without social lubrication you both enjoy some good conversation.")
        if dinner_already then
            AfterDinner(girl)
        else
            return Dinner(girl)
        end
    end
end

---@param girl wm.Girl
function HerRoom(girl)
    local choice = ChoiceBox("", "Polite Good Night", "Lean in for a Kiss")
    if choice == 0 then
        Dialog("You politely bow slightly and bring her hand to your lips.  \"Good Night, My Dear.  I hope you had a pleasant evening.\"")
        if girl:tiredness() > 70 then
            Dialog("Apparently, she was more tired then she let on and she begins to sway on her feet.  You catch her before she hits the ground.  You pick her up and carry her to her bed...")
            return SheIsAsleep(girl)
        else
            Dialog("She curtseys and smiles \"Good Night, Good Sir.\" She replies in a friendly but mocking tone. She closes the door behind her as she enters the room and  You head back down the hallway to your room alone.")
        end
    elseif choice == 1 then
        Dialog("As you lean in to kiss her, you ponder exactly what kind of kiss it should be...")
        local kiss = ChoiceBox("", "French Kiss", "Kiss")
        if kiss == 0 then
            Dialog("As your lips come together you slide your tongue into her mouth...")
            if girl:libido() >= 45 then
                Dialog("Her tongue meets yours and they begin a swirling dance back and forth. After several seconds the kiss ends with her gently biting your bottom lip as you separate.")
                PlayerFucksGirl(girl)
                Dialog("She takes your hands and leads you silently inside her room and toward her bed.")
                Dialog("The sounds of your lovemaking can be heard throughout the neighborhood.  A perfect ending to a perfect evening.")
            else
                Dialog("She kisses you back passively.  After a few moments you separate and say your final good nights.")
            end
        else
            Dialog("You decide to not try using your tongue and your lips come together in a kiss...")
            if girl:libido() >= 65 then
                Dialog("A moment later you find her tongue sliding past your lips and you reciprocate passionately...")
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("A trail of saliva hangs between you lips as you separate.  She kisses your neck unbuttoning your shirt; She kisses your chest and continues moving downward.  \"I'm still a little hungry.\" She whispers.  you revel in the feeling as she kisses the head of your dick and begins to lick and suck you hungrily.  You finish quickly under her onslaught and she gulps down every last drop.")
                Dialog("You stand reeling from the pleasure,  She lays back on the bed removing her lingerie and spreading her legs wide.  \"Are you still hungry?\" She asks spreading her pussy lips wide with her fingers.")
                Dialog("You smile and dive into the task before you.  You awake the next morning holding her tightly in her bed.")
                girl:oralsex(2)
            else
                Dialog("You kiss her lips and and say good night.")
            end
        end
    end
end

---@param girl wm.Girl
function SheIsAsleep(girl)
    local choice = ChoiceBox("", "Gently tuck her in", "Sleep Creep")
    if choice == 0 then
        Dialog("You spend a few moments watching her breath deeply and sleeping soundly.  You grab a nearby blanket and  gently tuck her in.  ")
        wm.SetPlayerDisposition(10)
    elseif choice == 1 then
        Dialog("As you look at her sleeping soundly, an idea seeps into your brain.  You shake her slightly and call her name to see how deeply she is sleeping.  Satisfied, you begin to run your hands over her breasts.")
        Dialog("Growing bolder, you begin to pull down her top to expose her breasts.  You give each nipple a little kiss.  Her breathing changes slightly and you freeze, but she is sleeping soundly.  You stroke your cock as you move to  slowly pull down her panties.")
        Dialog("You pulse quickens as her panties slowly slide down.  Halfway down her ass....her upper thighs...her knees...calves...you stop and leave them at her ankles and begin to slide your fingers into her vagina as you stroke your cock.")
        if wm.Percent(40) then
            Dialog("She moans sleepily as you rub the head of your penis against her juicy labia,  Slowly you begin to push inside her.  You move slowly and deliberately enjoying the feeling.  She gasps repeatedly and her mouth hangs open slightly, giving you an idea.")
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("You pull out of her pussy and bring your dick to her mouth.  gently you slide it past her lips.  The sensation and tension of the situation feel amazing and you release a large amount of cum down her throat.  Still sleeping she swallows it down. After a few moments you carefully put her closes back on and leave her to sleep.")
            girl:oralsex(2)
        else
            Dialog("She awakes with a sudden start and stares wide eyed at your fingers inside her and your erection moving closer.  \"What the fuck!\" She cries \"Get off of me!\"")
            local action = ChoiceBox("", "Hold her down and Fuck her anyway.", "Apologize and Leave")
            if action == 0 then
                Dialog("Grabbing her arms you pin her down. \"I'm afraid I've come too far to stop now, my dear!\"  She tries to fight you off by you are too strong for her.  She cries as you force your penis inside her and pump her forcefully.  You explode while deep inside her and your cum fills her womb.")
                wm.SetPlayerDisposition(-20)
                PlayerRapeGirl(girl)
            elseif action == 1 then
                Dialog("Startled you jump off of her and issue pathetic apologies as you flee the room.")
                wm.SetPlayerDisposition(-5)
            end
        end
    end
end

---@param girl wm.Girl
function Dinner(girl)
    Dialog("A bell ring from the dining room, informs you that dinner is served.")
    local what = wm.Range(0, 100)
    if what < 5 then
        Dialog("As she samples a few of the rare treats, something causes her to have an allergic reaction and her throat begins to close up rapidly.  You rush her to the nearest healer and spend the rest of the evening by her bedside as the healers work to save her life.")
        return
    elseif what > 69 then
        Dialog("Your personal chef has prepared a succulent feast of delicious and suggestive food.  As you lock eyes across the table, each bite becomes a seductive tease.  To finish the meal she sucks a long strand of pasta slowly into her mouth,  licking her lips, and flashing a coy smile.")
        girl:libido(40)
    elseif what >= 37 then
        Dialog("You both look hungrily at the bounty laid before you.  Your Chef has prepared a wonderful assortment of exotic foods.  You both  spend the meal sampling the variety and conversing easily.")
    else
        Dialog("Although the meal before you is delectable beyond compare, both of you struggle to find topics of common interest and the meal passes quietly.")
        girl:libido(-20)
    end
    local choice = ChoiceBox("", "Dessert", "Escort her back to her room.", "After dinner Cocktails")
    if choice == 0 then
        Dialog("You call for the chef to bring out the dessert course.")
        if girl:libido() > 80 then
            Dialog("The Creme Brulee this evening is exquisite. You become lost in each others eyes,  in fact, you become so distracted that you accidentally drop a spoonful of the sweet substance in your lap.  You apologize and move to clean it up, but she stops you saying \"Let me get that for you.\"")
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("She kneels beside you and moves her face very close to your lap.  She begins to lick the creme from your crotch hungrily.  After a few moments your pants are clean, but she looks up at you poutingly \"Is that all there was?  I want more cream.\"")
            Dialog("A devilish grin graces her face and she begins to remove your already rock hard erection from your trousers.  She licks and sucks greedily and expertly.  You feel the wave of cream swell within you and you release a massive explosion in her mouth..")
            Dialog("She swallows it all down in one gulp and licks her lips.  \"Now that was really satisfying.\"  she says with that devilish grin.  She thanks you for the wonderful evening and returns to her room.")
            girl:oralsex(2)
            girl:happiness(3)
            girl:pclove(4)
        elseif girl:has_trait("Nymphomaniac") then
            Dialog("\"Wait\" She says \"I want to make you a dessert myself.\"  She excuses herself to the kitchen for a few moments.")
            Dialog("You nearly fall out of your chair when she returns wearing nothing but whipped cream lingerie with cherries over her nipples. \"I wanted to make you a banana split, but I couldn't find a good banana in the kitchen.\" she smiles coyly \"Do you know where I can find a banana?\"")
            Dialog("\"I think I can help you out with that\" you reply as you approach her.  You spend the rest of the evening applying and removing whipped cream from each others bodies.")
            PlayerFucksGirl(girl)
            girl:happiness(3)
            girl:pclove(4)
        elseif wm.Percent(65) then
            Dialog("You both enjoy sharing a large piece of creamy cheesecake.  She sighs contentedly as you feed her the last bite.  You both pause for several moments gazing into each others eyes.")
            return AfterDinner(girl)
        else
            Dialog("You both both sit back heavily as you finish the rather large portions of cake.  She thanks you for the food and begins to leave.  You contemplate asking her to stay, but realize that any activity would likely cause both of you to explode, and not in a good way.")
        end
    elseif choice == 1 then
        Dialog("You gently take her by the hand and walk back to her room, arm in arm.")
        return HerRoom(girl)
    else
        Dialog("You invite her to join you on the couch for some after dinner drinks.")
        return Drinks(girl, true)
    end
end

---@param girl wm.Girl
function AfterDinner(girl)
    local choice = ChoiceBox("", "Escort her to her room.", "Invite her to spend the night.")
    if choice == 0 then
        return HerRoom(girl)
    elseif choice == 1 then
        Dialog("\"What now my dear?  Shall we continue this evening in the bedroom?\"")
        if girl:tiredness() > 40 then
            Dialog("She yawns and apologizes. \"I'm sorry, but I think it might be best to call it a night.\"")
            return HerRoom(girl)
        elseif girl:pclove() < -50 then
            Dialog("She looks at you seriously. \"I appreciate that you're trying to be nice and all, but if you think a little wine and food means i'm going to fuck you, then you can go to hell.\"  She turns to storm out of your quarters.")
            -- GOTO 555 // 8
        elseif girl:pclove() > 50 then
            Dialog("She looks deep into your eyes. \"Of course, my love, I have to properly thank you for this lovely evening...\"")
            PlayerFucksGirl(girl)
        elseif girl:libido() > 60 then
            Dialog("She giggles a bit as she looks up at you.  \"I've always wondered what the Master's bedroom looks like.\"")
            Dialog("She adds with a wink \"Perhaps we could both enjoy some tossed salad on your bed.\"")
            wm.UpdateImage(wm.IMG.ANAL)
            girl:anal(2)
        elseif wm.Percent(50) then
            Dialog("\"How can I refuse the offer of an even more delicious dessert?\" She replies in a sultry voice.")
            PlayerFucksGirl(girl)
        else
            Dialog("\"That is a lovely offer,\" She replies \"but I think it's best if I return to my room.\"")
            return HerRoom(girl)
        end
    end
end

---@param girl wm.Girl
function PrivateChambers(girl)
    Dialog("She smiles slightly and nods her agreement.")
    local event = ChoiceBox("", "Cocktails", "Dinner")
    if event == 0 then
        return Drinks(girl, false)
    else
        return Dinner(girl)
    end
end
