---@param girl wm.Girl
---@param dangerLev number
function BouncerShakedown(girl, dangerLev)
    Dialog("Guard: \"Sir, please enjoy your drinks. You picked a really hot girl, and I know you want to touch things you are not allowed to.\"")
    Dialog("The guard actually winks at you. \"But just between us gentlemen, for a sufficient tip, I won\'t be watching what you and the lady are doing.\"")
    local shakedown = ChoiceBox("How do you respond?",
                                "Pay the guard, and for the champagne and the girl\'s time. (500 gold)",
                                "Pay for the champagne and the girl\'s time, without tipping the guard. (300 gold)",
                                "Excuse yourself, go to the front door, and ask to see the manager.")
    if shakedown == 0 then
        Dialog("You pay the man and he goes back to his post with a smile on his face. You think he may enjoy watching, as well as pocketing the money.")
        wm.TakePlayerGold(500)
        Dialog("You turn back to the girl, and realize you have made a major mistake.\n" ..
                "She snarls at you. \"I'm a stripper, not a whore. I don\'t care what you and that asshole guard decided, keep your hands to yourself.\"")
        Dialog("You wanted a pleasant diversion, but having an angry girl on your lap isn't that much fun, even when she is pretty and naked.\n" ..
                "You give it up as a bad job, and head home a little wiser.")
    elseif shakedown == 1 then
        Dialog("The guard looks a bit miffed when you stiff him on the tip. The girl on your lap giggles, nibbles on your ear, and whispers, \"Thank you\".")
        wm.TakePlayerGold(300)
        wm.UpdateImage(wm.IMG.NUDE)
        Dialog("Girl: \"That is one example of how shady this club is. I would move to a better club if I could, but I\'ve never passed the interview.\"")
        Dialog("You: \"I know a guy at a club. I'll get you an interview tomorrow, if you like.\"")
        Dialog("She looks at you suspiciously. \"You are pulling my leg, right? How convenient that you \'just happen\' to \'know a guy\'.\"")
        Dialog("You: \"OK, you caught me. I am actually the club owner. I am checking out the competition, and I will happily steal one of their best assets.\"")
        Dialog("She grins impishly. \"Especially when the asset has an ass like mine, right?\"")
        Dialog("You: \"Further, you can take this as your interview. Make me a happy man tonight, and you will get an instant job offer tomorrow.\"")
        if wm.Percent(50) then
            Dialog("Girl: \"You know that guard will be watching like a hawk because you stiffed him. But I\'ll bet I can get you a handy anyway.\"")
            wm.UpdateImage(wm.IMG.HAND)
            if wm.Percent(girl:handjob()) then
                Dialog("She rubs her butt and breasts against your crotch, skillfully keeping her hand out of sight as she strokes you.")
                Dialog("You keep your face blank and impassive as the pressure builds in your balls.")
                wm.UpdateImage(wm.IMG.TITTY)
                Dialog("She is grinding her breasts against you when you reach your breaking point, and squirt a huge load all over her tits.") 
                Dialog("That is just too obvious. The guard catches you, and takes great pleasure in throwing you out of the club.")
                Dialog("You don\'t care. You are looking forward to tomorrow, when your new employee will start her first day. Totally worth it!")
                wm.AcquireGirl(girl)
            else
                Dialog("But the girl proves unequal to the task. The guard catches you a couple of times and warns her to keep her hands off you.")
                Dialog("Girl: \"Sorry, Mister. I guess it is not going to happen tonight. At least I will keep my job here.\"")
            end
        elseif wm.Percent(girl:intelligence()) then
            Dialog("Girl: \"Well, how\'s this for a deal. You stay with me the rest of the night, and pay for my time. (700 gold)\"")
            Dialog("Girl: \"Then I will meet you outside, for a little suck and fuck to close the interview.\"")
            wm.TakePlayerGold(700)
            Dialog("You agree. You sit with her all night, her breasts and butt and occasionally her hand teasing you skillfully.")
            Dialog("After hours, you wait outside the door until she comes out. You moan as she grabs your dick through your pants and drags you behind the club.")
            wm.UpdateImage(wm.IMG.HAND)
            Dialog("You sigh with pleasure as she pulls your cock out, and massages you vigorously.")
            wm.UpdateImage(wm.IMG.DEEP)
            Dialog("You almost pop as she takes you balls deep into her warm wet mouth.")
            wm.UpdateImage(wm.IMG.SEX)
            Dialog("Then she impales herself on your dick, and almost instantly both of you are cumming intensely.")
            Dialog("You stay hard long enough that you fuck her to a second orgasm before you collapse, spent and very very happy with your new employee.")
            wm.AcquireGirl(girl)
        else
            Dialog("Girl: \"Now wait a minute. You want to hire me out of this sleazy place, but only if I act even sleazier than normal?\"")
            Dialog("She shakes her head at you. You can\'t help but appreciate the way her tits bounce when she does that, even though you don\'t like what she says next.")
            Dialog("Girl: \"I think you should leave, mister. This isn\'t going to work out.\"")
        end
    elseif shakedown == 2 then
        Dialog("You excuse yourself and go to the front of the club. You ask to see the club manager.")
        Dialog("You describe the guard's shakedown attempt. The club manager's face grows very cold.")
        Dialog("Manager: \"Thank you for reporting this. We don\'t allow that, of course. Please accept my apologies.\"")
        Dialog("Manager: \"Your champagne is on the house, and I assure you that guard will not trouble you or any of our other patrons, ever again.\"")
        Dialog("Upon your return, five of the house girls are huddled in the champagne room, whispering to each other.")
        Dialog("The guard is no longer around. In fact, you don\'t see any security or other patrons at all.")
        Dialog("Girl: \"Did you really get that guy fired? Oh, our hero!\"")
        Dialog("The other girls agree. \"He had been making it very uncomfortable to work the champagne room, and you know that\'s where we make most of our money.\"")
        Dialog("Girl: \"I think our hero deserves a special reward. What do you think, girls?\"")
        local stripper1 = wm.CreateRandomGirl(wm.SPAWN.MEETING, wm.Range(18,30))
        local stripper2 = wm.CreateRandomGirl(wm.SPAWN.MEETING, wm.Range(18,30))
        local stripper3 = wm.CreateRandomGirl(wm.SPAWN.MEETING, wm.Range(18,30))
        local stripper4 = wm.CreateRandomGirl(wm.SPAWN.MEETING, wm.Range(18,30))
        wm.UpdateImage(wm.IMG.GROUP)
        Dialog("She starts rubbing your cock through your pants, and suddenly you are surrounding by giggling, naked strippers each thanking you in turn.")
        Dialog("The rest of the night is a blur of flesh and pleasure. Some of those memories will stick with you for life.")
        Dialog("Motorboating one girl, another girl sucking your dick and yet another licking your balls...")
        Dialog("Two girls with tremendous breasts giving you a simultaneous titty fuck...")
        Dialog("Lying on the floor with one girl fucking you cowgirl, while another girl sits on your face with her pink, juicy cunt in your mouth...")
        Dialog("All five girls laying in a stack, so changing girls just requires you to pull out and immediately re-insert...")
        wm.UpdateImage(wm.IMG.HAND)
        Dialog("But one memory stands out clear and unblurred: The girl that brought you to the champagne room jerks out a truly tremendous load all over the other four girl's smiling faces.")
        Dialog("Exhausted and spent, you mention that your own club is hiring, and that any and all of them are welcome to join you.")
        
        if wm.Percent(girl:charisma()) and wm.Percent(girl:intelligence()) then
            Dialog("Your original date jumps at the chance to work for you!")
            wm.AcquireGirl(girl)
        else
            Dialog("Your original date says no. \"Now that you\'ve fixed the security situation, this place is great! I will stay with my regulars, but thanks for the sweet offer.\"")
        end
        
        local count = 0
        if wm.Percent(stripper1:charisma()) and wm.Percent(stripper1:intelligence()) then
            Dialog("One girl rubs your penis and kisses you sweetly as she accepts your offer.")
            wm.AcquireGirl(stripper1)
            count = count + 1
        end
        
        if wm.Percent(stripper2:charisma()) and wm.Percent(stripper2:intelligence()) then
            Dialog("With a smile and a boob jiggle, one girl says she will join your club.")
            wm.AcquireGirl(stripper2)
            count = count + 1
        end
        
        if wm.Percent(stripper3:charisma()) and wm.Percent(stripper3:intelligence()) then
            Dialog("A big smile lights up one girl\'s face as she says yes")
            wm.AcquireGirl(stripper3)
            count = count + 1
        end
        
        if wm.Percent(stripper4:charisma()) and wm.Percent(stripper4:intelligence()) then
            Dialog("One girl excitedly says yes, jumping up and down with her breasts swinging wildly.")
            wm.AcquireGirl(stripper4)
            count = count + 1
        end
        
        if count < 4 then
            Dialog("The rest of the girls decide to stay at this club, but they giggle and say you should come back often.")
        end
        Dialog("This was a night to remember, for sure!")
    end
end

---@param girl wm.Girl
function DecentCitizen(girl)
    Dialog("You notice an attractive woman walking the same way as you. She looks over her shoulder and smiles to herself.")
    GirlDescription(girl)
    if girl:has_trait(wm.TRAITS.FLEET_OF_FOOT) then
        Dialog("She is moving faster than you, but she slows down to let you catch up.")
    elseif girl:has_trait(wm.TRAITS.FLIGHT) then
        Dialog("She almost doesn\'t touch the ground, she is moving so fast. You hustle to catch up.")
    elseif girl:has_trait(wm.TRAITS.AGILE) then
        Dialog("You admire the way she moves, and how fast she goes. You lengthen your stride to catch her.")  
    else
        Dialog("You catch up to her, and adjust your pace to match hers.")
    end
    local approach = ChoiceBox("How do you want to approach her?",
                               "Pull her off the streets and rape her.",
                               "Ask if she wants to get dinner and a drink.",
                               "Get straight to the point and ask if she wants a job at your brothel.")
    if approach == 0 then
         RapeGirl(girl)
    elseif approach == 1 then
         DinnerAndADrink(girl)
    elseif approach == 2 then
         StraightAsk(girl)
    end
end

---@param girl wm.Girl
function DefendBrothel(girl)
    Dialog("You can tell there is danger in the air. You see no one in the streets.")
    Dialog("You enjoy a few moments of quiet in the pews. All too soon, though, it is time to return.")
    Dialog("Returning to your brothel, you find trouble. An enemy gang is attacking!")
    Dialog("Your guards are fighting back, but are having a hard time. You watch the fight for a moment and notice that a mage is paralyzing your guards one by one.")
    Dialog("Fortunately, you and your bodyguards have arrived behind the attackers, and they apparently have not detected you.")
    Dialog("You tell your guards to wait until they see the mage fall, and then to attack the enemy gang from behind.")
    local fight = ChoiceBox("You sneak up behind the mage.", "End this quickly. Strike to kill.", "She\'s cute. Strike to capture her.")
    if fight == 0 then
        Dialog("You grin evilly as you strike. \"Die, bitch!\"")
        wm.SetPlayerDisposition(-5)
        if wm.Percent(70) then
            Dialog("She never sees you coming. Such a pity. But your strike is true, and the mage drops dead at your feet.")
            Dialog("Your men scream their battle cries and charge into the fray. The enemy gang beats a hasty retreat.")
            Dialog("The serenity you felt at the church is destroyed, but your quick action has prevented serious damage to your brothel.")
            return
        else
            Dialog("As you swing, you feel your arm slow down dramatically. The mage turns to face you as the air around your sword starts to glow blue.")
            Dialog("She grins nastily. \"Ah, ah ah! Sneaks like you deserve to fry!\" She starts a gesture that you are sure is going to set you afire.")
            Dialog("That\'s when your men scream battle cries and charge the mage. She snarls at the interruption. \"Bother! Well, until next time.\"")
            Dialog("Her gesture changes into an expansive circle, and your eardrums pop as air rushes into her suddenly vacant location.")
            Dialog("Your men thunder up to you. \"Whew, we thought you were a goner, boss!\" Thank goodness they adjusted the plan to save your bacon!")
            Dialog("But while you have all been distracted by the enemy mage, things have gotten worse. The first floor is on fire, and most of your guards are down.")
            Dialog("Your eyes grow cold. Someone is going to pay for this. \"Get \'em!\" you snap at your men, as you lead them to battle.")
            Dialog("The enemy is soon routed, surprised by the rear attack. Fortunately, none of your girls were harmed, but significant damage has been done.")
            Dialog("It takes 2500 gold to clean up the mess.")
            wm.TakePlayerGold(2500)
            return
        end
    elseif fight == 1 then
        Dialog("Quietly, you pull out your blackjack, and swing at the back of her head.")
        wm.SetPlayerDisposition(3)
        if wm.Percent(50) then
            Dialog("The blow knocks her cold. The rest of your plan works just as well. Your men\'s rear attack scares off the enemy gang.")
            Dialog("Your quick action stopped the enemy gang from doing any significant damage, plus you have a captive.")
            Dialog("She looks comely enough. You are going to have fun training her.")
            wm.AcquireGirl(girl)
            girl:health(-20)
            girl:add_trait(wm.TRAITS.POWERFUL_MAGIC)
            girl:remove_trait(wm.TRAITS.WEAK_MAGIC)
            girl:remove_trait(wm.TRAITS.MUGGLE)
            return
        else
            Dialog("Your blow hits, but a little low. She\'s staggered, but still on her feet. Scared she will get a spell off, you hastily swing at her overhand.")
            Dialog("The sap smacks the crown of her head, and she stands looking stupidly at you for a second.")
            Dialog("Then blood starts flowing down her forehead, her eyes roll up, and she crumples to the ground.")
            Dialog("Your men launch their attack, but you need to make sure this mage can no longer affect the battle.")
            Dialog("You look at her pretty face and wonder if you have killed her.")
            Dialog("Checking her pulse, you discover that she is still alive, but just barely. The sap did brutal damage to her skull. She might not make it.")
            wm.AcquireGirl(girl)
            girl:health(-90)
            girl:add_trait(wm.TRAITS.POWERFUL_MAGIC)
            girl:remove_trait(wm.TRAITS.WEAK_MAGIC)
            girl:remove_trait(wm.TRAITS.MUGGLE)
            Dialog("The mage secured, you check the rest of the battle. The enemy gang has been routed, but your guards and your brothel have suffered greatly.")
            Dialog("It is going to take a lot of gold to fix this up. Hope you have a spare five grand laying around.")
            wm.TakePlayerGold(5000)
            return
        end
    end
end

---@param girl wm.Girl
function DinnerAndADrink(girl)
    local msg = RandomChoice("Girl: \"What a coincidence! ", 
                             "Girl: \"That\'s funny! ", 
                             "She smiles. \"Best offer I have had today. ")  .. 
                RandomChoice("I am just going out! ",
                             "I was thinking about food. ",
                             "I am very hungry! ")
    local msg2 = RandomChoice("Italian", "Vietnamese", "country", "fusion", "burger", "steak", "Indian", "seafood", "dumpling", "Chinese") .. " " .. 
                 RandomChoice("restaurant", "place", "house", "diner", "kitchen", "establishment")
    local exp = wm.Range(1, 3)
    if exp == 1 then
        Dialog(msg .. "Have you heard about that great new " .. msg2 .. " across town? Want to take me?\"")
        Dialog("You wince a little. You know the place and it is very pricy. But it is hard to back out now.")
        wm.TakePlayerGold(500 + (wm.Range(0,3) * 50))
        wm.SetPlayerDisposition(4)
    elseif exp == 2 then
        Dialog(msg .. "See that nice little " .. msg2 .. " across the street? Let\'s go there!\"")
        Dialog("Reading the menu, you are relieved to see the prices are reasonable.")
        wm.TakePlayerGold(100 + (wm.Range(0,3) * 25))
        wm.SetPlayerDisposition(2)
    elseif exp == 3 then
        Dialog(msg .. "I know a hole in the wall " .. msg2 .. " with great food. It\'s cheap but very nice.\"")
        Dialog("You had hoped for a classier place, but decide to just go with the flow.")
        wm.TakePlayerGold(20 + (wm.Range(0,3) * 5))
    end

    Dialog("You like the place. The maitre d\' is welcoming, and the wait staff is attentive without being intrusive.")
    Dialog("The food is also excellent. You and your date have several glasses of a great beverage recommended by your waiter.")
    local charisma = girl:charisma()
    local count = 0
    local demeanor = ""
    if wm.Percent(charisma) then
        demeanor = demeanor .. "Your date is friendly and flirtatious, laughs at all your jokes, and converses easily. "
        count = count + 1
    else
        demeanor = demeanor .. RandomChoice("Your date concentrates pretty exclusively on the food and drink, rather than you. ",
                                            "Conversation is a little stilted. ")
    end
    local intl = girl:intelligence()
    if wm.Percent(intl) then
        demeanor = demeanor .. "She demonstrates her smarts without being stuck up about it. Your conversation is lively, witty and unexpectedly profound."
        count = count + 1
    else
        demeanor = demeanor .. "She\'s not the brightest bulb in the lamp. You carry the majority of the conversation, keeping things light and playful. "
    end
    Dialog(demeanor)
    Dialog("You offer to walk her home, and she accepts your arm. At her door, you hold her, look in her eyes, and lean in for a kiss.")
    local kiss = 0
    if count >= wm.Range(1, 3) then
        Dialog("She hugs you tightly during the kiss, and asks if you would like to join her for some tea.")
        kiss = ChoiceBox("She likes you! Do you accept her invitation?", 
                         "Why, I would love to have some tea with you, my dear.",
                         "I am afraid I must be going.")
        if kiss == 0 then
            Dialog("She grins, then leads you to a swing on her porch. \"I\'ll be right back with some tea, then.\"")
        elseif kiss == 1 then
            Dialog("She is mystified, and disappointed by your refusal. \"Oh. OK, then. The offer is open... Maybe next time?\"")
            Dialog("You say goodbye and head back home.")
            return
        end
    else
        Dialog("She does kiss you, but it is best described as \"polite\".")
        kiss = ChoiceBox("She turns to go in. Try to continue the evening?",
                         "\"Honey, that dinner was on me. Don\'t you think I deserve more than just a tiny kiss?\"",
                         "\"I have really enjoyed talking to you. It\'s a pity we can\'t continue the conversation.\"",
                         "You bow to the inevitable. \"Thank you for the lovely evening. Perhaps we will meet again.\"")
        if kiss == 0 then
            Dialog("Girl: \"Oh, please. Dinner was lovely. Don\'t ruin it now.\"")
            Dialog("You: \"Dinner was lovely, indeed, but you will be the one ruining things if you leave me with a case of blue balls.\"")
            local handy = girl:has_trait(wm.TRAITS.MEEK, wm.TRAITS.MIND_FUCKED) or not wm.Percent(girl:intelligence())
            if handy == 0 then
                Dialog("She snarls at you. \"There is a really easy cure for blue balls. I\'m sure you are intimately familiar with it.\"")
                Dialog("\"You bought dinner, not a ticket into my pants. Jerk off, jerk.\" The door slams in your face, and it seems your date is over.")
                return
            else
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("She hesitates, and you see the alcohol from dinner working on her.\n" ..
                        "She shrugs and frowns, but then rubs her hand across your crotch. \"Sure, why not?\"")
                Dialog("She pulls you into some bushes by the side of her house. \"Shhh, keep quiet, we don't want to wake up my roommates.\"\n" ..
                        "She unzips you, pulls your cock out, turns you around to face the house, and reaches around to start tugging.")
                Dialog("This isn\'t exactly what you had in mind, but it\'s really hard to complain.")
                Dialog("She knows what she\'s doing, as one hand tugs away lustily while her other rubs your taint through your pants.")
                Dialog("You spasm and coat the wall of her house with a few spoonfuls of spunk.")
                Dialog("It\'s only then that you notice several other stains on the wall. Apparently this isn\'t the first time she\'s done a reach-around here." ..
                        "You chuckle and turn around to thank her, but she's already up the stairs. The evening is obviously over, so you zip up and head home.")
                return
            end 
        elseif kiss == 1 then
            Dialog("She smiles. \"I really enjoyed talking to you, too. Why don\'t we sit on my porch swing for a bit, then?\"")
        elseif kiss == 2 then
            Dialog("You shrug. At least the dinner was good.")
            return
        end
    end
    Dialog("The porch swing is surprisingly comfortable, but she is curious. \"It\'s getting very late. Don\'t you work tomorrow?\"")
    local fessup = ChoiceBox("Will the truth scare her off?",
                             "\"Well, I run a brothel, so I mostly work at night. I kind of fell into it when my dad left me the place.\"",
                             "\"Oh, that\'s no problem. I do a little of this, a little of that, and I can do it when I like.\"")
    if fessup == 0 then
        Dialog("She is startled by your frank admission. \"You... own a brothel? Where prostitutes service men?\"")
        Dialog("You smile. \"Well, we have many women clients, too. Sometimes there are more lesbians in my shop than whores.\"")
        Dialog("She licks her lips. \"I've always wondered why a girl does sex for money.\"")
        Dialog("You laugh. \"Your question answers itself. For the money, of course.\"")
        Dialog("\"Oh, most of the girls are quite interested in the sex for it\'s own sake, but when it's business time, they always get the money up front.\"")
        Dialog("She looks excited. \"So, if I offered you a blowjob right now, would you pay me for it?\"")
        wm.UpdateImage(wm.IMG.ORAL)
        Dialog("You: \"Well, I wouldn't say no, but I\'d be more inclined to view it as a job interview. My brothel is always hiring beautiful girls like you.\"")
        Dialog("She chuckles. \"Not so fast. Dinner and a compliment are nice, but didn\'t you just tell me that your girls always get the money up front?\"")
        Dialog("You: \"Nicely played. So how about this? You have a job, whether or not I get a blowjob tonight. Will that do?\"")
        Dialog("She is clearly enjoying this battle of wills. \"So, dinner, a compliment, and now you are adding a promise! " .. 
               "Add 50 gold hard currency, and that will do fine.\"")
        wm.TakePlayerGold(50 + (wm.Range(1, 2) * 25))
        Dialog("You laugh and give her the money. After you feel her warm mouth on your cock, and gush a big load down her throat, you even add a tip.")
        wm.AcquireGirl(girl)
    elseif fessup == 1 then
        Dialog("She looks dubious. \"That was evasive. At least \'a litle bit of this, and a little of that\' can buy a good dinner.\"")
        Dialog("It\'s clear she thinks you are either a criminal or unemployed. An awkward silence settles over the evening, and your date ends quickly.") 
    end
end

---@param girl wm.Girl
function GirlAudition(girl)
    local debug = 0
    
    if debug == 1 then 
        Dialog("Testing the whole thing")
        girl:normalsex(-100)
        girl:handjob(-100)
        girl:oralsex(-100)
        girl:intelligence(100)
        girl:beauty(100)
        girl:strip(100)
        girl:charisma(100)
        girl:tittysex(-100)
        girl:anal(100)
        girl:add_trait(wm.TRAITS.VIRGIN)
    end
    
    Dialog("You take her to a fancy hotel nearby. It costs 100 gold but you think she might be worth it.")
    wm.TakePlayerGold(100)
    Dialog("You negotiate prices, and realize she wants 200 gold for her services.")
    Dialog("You: \"Did you know I own the brothel down the way? I always need pretty girls like you. How about we go for free, and we consider this an audition?\"")
    local score = 0
    if wm.Percent(girl:intelligence()) then
        Dialog("She smiles wickedly. \"Sure, honey, and I own this hotel, too. It\'s still payment in advance, whether or not this is a job interview.\"")
        Dialog("You also smile. \"Well, my dear, that\'s rule number one at my establishments, too. You pass the first test. Of course I will pay for your time.\"")
        score = score + 20
        wm.TakePlayerGold(200)
    elseif wm.Percent(girl:beauty()) then
        Dialog("You grimace. She might not be smart enough to be good at this. But you look again, and realize she is pretty enough that her brains don\'t matter.")
        Dialog("You: \"Babe, that was a test. Always get the money up front!\" You proceed to pay her.")
        wm.TakePlayerGold(200)
    elseif girl:has_trait(wm.TRAITS.CHARISMATIC, wm.TRAITS.CHARMING, wm.TRAITS.COOL_PERSON) or wm.Percent(girl:charisma()) then
        Dialog("That\'s a shame. You shake your head. \"Girl, you gotta get the money up front or you will get stiffed!\"")
        Dialog("Girl: \"Oh, please, sir! I usually get better tips if I bring up payment afterwards.\"")
        Dialog("Her fingers trace the length of your obvious erection. \"You wouldn\'t fail me over a simple matter of technique, would you?\"")
        Dialog("You: \"Promise me that if I hire you, you will always get the money up front, and we can continue.\"")
        Dialog("Girl: \"Honey, I will promise you anything to get to that nice looking piece of meat.\"")
        Dialog("You smile. A friendly girl often makes more money for you than a better looker. \"Nice save. I like the way you think.\"")
        score = score + 10
    else
        Dialog("Girl: \"Oh, steady work and getting off my feet would be so nice! I\'ll do my best to earn a job at your brothel!\"")
        Dialog("You grimace. \"Well, honey, that was the first test... and you need to always get the money up front to work for me.\"")
        Dialog("She realizes you aren\'t going to hire her now. \"Well, you don\'t want to waste the room, sugar. That 200 gold will still get you a great fuck!\"")
        local fuckher = ChoiceBox("She isn\'t suitable to hire, but do you want to pay her and use her body?",
                                  "Sure, 200 gold is a cheap fuck.",
                                  "Naw, this skank ain\'t worth the money or time.")
        if fuckher == 0 then
            wm.TakePlayerGold(200)
            wm.UpdateImage(wm.IMG.SEX)
            Dialog("You smile, strip down, and fuck her silly until you spray her insides with sperm.")
            Dialog("You may not have gotten a new employee, but it is never a bad thing to get your rocks off in a new pussy.")
            return
        elseif fuckher == 1 then
            Dialog("You shake your head. \"Sorry, no. But you can use the room for the rest of the night. I am sure you can find some other john to share it with.\"")
            return
        end
    end
    Dialog("You: \"Now for the second test. Tease me.\"")
    score = score + 20
    if wm.Percent(girl:strip()) then
        wm.UpdateImage(wm.IMG.STRIP)
        Dialog("Girl: \"Ooh, sugar... happy to oblige.\"")
        Dialog("Smiling coyly, she reaches out and closes your eyelids. Her fingers roam smoothly across your body, lightly massaging your neck.")
        Dialog("You feel her fingers move lightly to your nipples, down your stomach, finally tracing the outline of your rigid member through your pants.")
        Dialog("You open your eyes, and see that she has somehow managed to get all her clothes off without you even noticing.")
        score = score + 10
    else
        wm.UpdateImage(wm.IMG.ECCHI)
        Dialog("Girl: \"What do you mean, tease you?\"")
        Dialog("You sigh. \"Maybe you can take your clothes off for me?\"")
        Dialog("She stands up and attempts a striptease. Her clothes come off smoothly, but to be exciting, she needs music and stage lighting.")
        Dialog("Far too quickly, her clothes are off and she stands nude before you.")           
    end
    Dialog("You remove your clothes to catch up. \"Final test: Satisfy me.\"")
    if wm.Percent(girl:charisma()) then
        score = score + 20
        Dialog("She winks at you saucily. \"Oh, you just let me take care of you.\" Her low, smoky voice is extremely sexy.")
    else
        Dialog("She shrugs her shoulders. \"OK.\"")
    end
    wm.UpdateImage(wm.IMG.HAND)
    if wm.Percent(girl:handjob()) then
        score = 100
        Dialog("She glides over and begins massaging your member. With feathery touches and a little tender tugging, she soon has you ready to explode.")
        Dialog("Her eyes bore into yours, imploring you as she drives you insane. She begins to tug a bit more urgently.")
        Dialog("She purrs in your ear. \"Come for me, honey.\" You shudder and shoot cum clear across the room.")
    else
        Dialog("She grabs your cock and strokes it a few times.")
    end
    if score < 100 then
        wm.UpdateImage(wm.IMG.ORAL)
        if wm.Percent(girl:oralsex()) then
            Dialog("Then she uses her mouth, and her true artistry emerges.")
            Dialog("She teases your shaft, then licks your balls, then lustily sucks you deep into her throat.")
            Dialog("She growls around your cock, the vibrations tingling your core. \"You ARE going to feed me your cum!\"")
            Dialog("She grabs your dick with both hands, tugs furiously, and vacuums the baby batter out of your balls.")
            score = 100
        else
            Dialog("She artlessly sucks at the tip of your cock, drooling saliva all over.")
        end
    end
    if score < 100 then
        wm.UpdateImage(wm.IMG.TITTY)
        if wm.Percent(girl:tittysex()) then
            score = 100
            Dialog("She smiles. \"You have been a good boy. Now mommy has a treat for you.\"")
            Dialog("She slaps the tip of your cock on her nipples. Keeping you lubricated with her saliva, she envelops you with her soft breasts.")
            Dialog("Her hand holds you in place firmly. \"Fuck my tits, daddy! Come all over my beautiful tits!\"")
            Dialog("You love thrusting into her cleavage. When she starts licking your tip as it peeks out between her breasts, you lose it.")
            Dialog("You spasm wildly and coat her neck and mouth with sticky man-frosting.")
        else
            Dialog("She slides your dick between her breasts, and rubs her tits up and down your length a few times.")
        end
    end
    if score < 100 then
        if girl:has_trait(wm.TRAITS.VIRGIN) then
            if debug == 1 then Dialog ("trying presented") end
            wm.UpdateImage(wm.IMG.PRESENTED)
            Dialog("You are disappointed when she doesn\'t let you get near her pussy. \"That\'s never been used, sugar. But my ass is more than willing.\"")
        else
            wm.UpdateImage(wm.IMG.SEX)
            if wm.Percent(girl:normalsex()) then
                score = 100
                Dialog("She pushes you down on the bed and grins. \"I love this part the best!\"")
                Dialog("She gives the tip of your cock a tiny little kiss. \"I hope you are ready, big boy!\"")
                Dialog("Then she literally leaps on your well-lubricated cock. You gasp as she impales herself on your pole, taking your entire length.")
                Dialog("She bounces wildly, screaming through multiple orgasms, her pussy sliding up and down, left and right, and then in gyrating circles.")
                Dialog("You feel like you are thrusting into the middle of a sensual tornado. Your last thrust lifts her off the bed as you explode inside.")
                Dialog("You can\'t believe it, but you are still hard. You roll her over and take her again. Your shuddering orgasm finally leaves your balls completely empty.")
            else
                Dialog("She lays on the bed and begs, \"Take me!\" You get on top and dutifully start grinding.")
            end
        end
    end
    if debug == 1 then Dialog("Trying anal") end
    if score < 100 then
        if wm.Percent(girl:anal()) then
            wm.UpdateImage(wm.IMG.ANAL)
            Dialog("Girl: \"Honey, try the other hole!\"")
            Dialog("She lifts her hips, giving you a clear shot at her ass. You lube her up, and your raging hard-on slides in easily.") 
            Dialog("She clamps down tightly, raking your back with her fingers. She wiggles and thrusts against you, massaging your entire length.")
            Dialog("It\'s just too much. You shudder and fill her colon with ejaculate.")
            score = 100
        else
            Dialog("You fill her ass crack with lube, and manage to push yourself in with a little effort.")
            Dialog("You pump her butt for a few minutes, waiting for the magic to happen. Finally, you give up and fantasize about your favorite girl back at the brothel.")
            Dialog("That gives you enough incentive to finish, but the whole experience was sub-par at best.")
        end
    end
    if score == 100 then
        Dialog("You: \"Honey, you have some talent. Stop working the streets independently, come and work for me.\"")
        Dialog("She giggles. \"You gotta watch my hands more carefully. I already came. I accept your kind offer.\"")
        wm.AcquireGirl(girl)
    else
        local hire = 0
        if wm.Percent(score) then
            hire = ChoiceBox("She showed a little promise, but needs some training. Hire her?",
                             "She is definitely worth trying out.",
                             "I already have too many girls with better skills.")
        else
            hire = ChoiceBox("That was a dismal performance. She will not be a star for you without a lot of investment. Hire her anyway?",
                             "I\'m desperate. I need bodies in beds making me bank. Hers will do for now.",
                             "I don\'t hire skanks like her.")
        end
        if hire == 0 then
            Dialog("You passed the audition, girl! Report for work at my brothel in the morning!")
            Dialog("Girl: \"Thank you for giving me a chance, sir!\"")
            wm.AcquireGirl(girl)
        else
            Dialog("As you get dressed, the girl senses she didn\'t do that well. \"Did I pass the audition, sir?\"")
            Dialog("You: \"I am sorry, but I am not going to be hiring you. Keep on fucking, though, and maybe we can try again when you are a bit more skilled.\"")
        end
    end
end

---@param girl wm.Girl
function GirlDescription(girl)
    local desc = ""
    local debug = 2
    if girl:has_trait(wm.TRAITS.UNDEAD, wm.TRAITS.SKELETON, wm.TRAITS.ZOMBIE) then
        desc = desc .. RandomChoice("This girl is clearly not alive. ",
                                    "She is magically animated. ")
    end
    if girl:has_trait(wm.TRAITS.DEMON, wm.TRAITS.SUCCUBUS, wm.TRAITS.VAMPIRE) then
        desc = desc .. "There is an air of brooding, almost supernatural danger about her. "
    end
    if girl:has_trait(wm.TRAITS.ANGEL) then
        desc = desc .. "Just looking at this girl calms you. You feel serene in her presence. " 
    end
    if girl:has_trait(wm.TRAITS.CANINE, wm.TRAITS.CAT_GIRL, wm.TRAITS.COW_GIRL, wm.TRAITS.DRYAD, wm.TRAITS.ELF, wm.TRAITS.EQUINE,
            wm.TRAITS.FURRY, wm.TRAITS.HALF_BREED, wm.TRAITS.NOT_HUMAN, wm.TRAITS.REPTILIAN, wm.TRAITS.SLITHERER, wm.TRAITS.CYCLOPS,
            wm.TRAITS.PLAYFUL_TAIL, wm.TRAITS.PREHENSILE_TAIL, wm.TRAITS.WINGS) then
        desc = desc .. RandomChoice("She obviously has non-human DNA. ", 
                                    "She is not completely human. ")
    end
    if girl:has_trait(wm.TRAITS.SOLAR_POWERED) then
        desc = desc .. "You see her stretch lazily in the sun, with a big smile on her face. "
    end
    if girl:has_trait(wm.TRAITS.CONSTRUCT, wm.TRAITS.BATTERY_OPERATED, wm.TRAITS.HALF_CONSTRUCT) then
        desc = desc .. "She is partially or fully mechanical. "
    end
    if girl:has_trait(wm.TRAITS.FUTANARI, wm.TRAITS.SHAPE_SHIFTER) then
        desc = desc .. "Her face and manner promises you many fun surprises. "
    end
    if (girl:beauty() > 90) then
        desc = desc .. RandomChoice("She is stunning beautiful. People openly stare at her. ", 
                                    "You have never seen anyone as beautiful as her. ")
    elseif (girl:beauty() > 60) then
        desc = desc .. RandomChoice("She\'s extremely pretty. You notice others trying to be cool as they check her out. ", 
                                    "Your pulse quickens looking at her. ")
    elseif (girl:beauty() < 30) then
        desc = desc .. RandomChoice("She\'s not very pretty, ",
                                    "Her picture could be on a butter stick, ")
        desc = desc .. "but maybe she has a nice personality. "
    else
        desc = desc .. RandomChoice("She is pretty enough that you wouldn\'t say no if she said yes. ",
                                    "You think you can help her be prettier. ",
                                    "She is fairly attractive. ")
    end

    if girl:has_trait(wm.TRAITS.CHARISMATIC, wm.TRAITS.CHARMING, wm.TRAITS.COOL_PERSON) then
        desc = desc .. RandomChoice("When she smiles, you instantly feel a connection to her. ",
                                    "She makes you smile just by looking at you. ")
    end
    if girl:has_trait(wm.TRAITS.ELEGANT, wm.TRAITS.NOBLE, wm.TRAITS.PRINCESS, wm.TRAITS.QUEEN, wm.TRAITS.FALLEN_GODDESS,
            wm.TRAITS.GODDESS, wm.TRAITS.ANGEL) then
        desc = desc .. RandomChoice("Everything about this girl says she is above your social standing. ",
                                    "Women rarely make you uncomfortable, but this one seems out of your league. ")
    end
    if girl:has_trait(wm.TRAITS.BEAUTY_MARK, wm.TRAITS.EXOTIC, wm.TRAITS.CUTE) then
        desc = desc .. "There is something about her look that makes her very attractive. "
    end
    if girl:has_trait(wm.TRAITS.INCORPOREAL) then
        desc = desc .. RandomChoice("From certain angles, you swear you can see right through her. ",
                                    "She is just a wisp of a thing, but you get the feeling she can endure a lot.")
    end
    if girl:has_trait(wm.TRAITS.DICK_SUCKING_LIPS, wm.TRAITS.SEXY_AIR) then
        desc = desc .. RandomChoice("Her lips look full and inviting. ",
                                    "Your dick stiffens as she purses her lips at you. ")
    end
    if (girl:has_trait(wm.TRAITS.LOLITA) or girl:age() < 21) then
        desc = desc .. RandomChoice("She looks very young. ",
                                    "Your jailbait alarm is going off. ")
    end

    if girl:breast_size() >= 6 then
        desc = desc .. RandomChoice("Given her rack, you hope she has a strong back. ", 
                                    "Her sweater puppies look like a pair of St. Bernards. ",
                                    "She has great tracts of land. ") 
    end

    if girl:breast_size() < 3 then
        desc = desc .. "She has almost no breasts. "
    end

    if girl:breast_size() >= 3 and girl:breast_size() < 5 then
        desc = desc .. "Her sweater puppies look to be chihuahuas. "
    end

    if girl:has_trait(wm.TRAITS.GREAT_ARSE, wm.TRAITS.PHAT_BOOTY, wm.TRAITS.PLUMP_TUSH, wm.TRAITS.DELUXE_DERRIERE) then
        desc = desc .. RandomChoice("Her ass rolls and sways suggestively. You have trouble keeping your eyes off it. ",
                                    "She has a perfect cushion for the pushin\'. ")
    end

    if girl:has_trait(wm.TRAITS.FLAT_ASS) then
        desc = desc .. RandomChoice("Her butt could use more curves. ",
                                    "She has the slim butt of a very young girl. ")
    end

    if girl:has_trait(wm.TRAITS.TIGHT_BUTT) then
        desc = desc .. RandomChoice("Her butt is finely-sculpted. She must work out. ",
                                    "Those ass cheeks are works of art. ")
    end

    if girl:has_trait(wm.TRAITS.GREAT_FIGURE, wm.TRAITS.HOURGLASS_FIGURE) then
        desc = desc .. RandomChoice("Her figure is wonderful. ", 
                                    "You want to run your hands over her curves. ",
                                    "She has curves in all the right places. ")
    end

    if girl:has_trait(wm.TRAITS.FAT, wm.TRAITS.PLUMP) then
        desc = desc .. RandomChoice("She looks like she hasn\'t missed many meals lately. ", 
                                    "She is a little heavy. You wonder how much she would increase your food budget. ", 
                                    "She is a little soft looking. ", 
                                    "She doesn\'t any possibility of a thigh gap. ")
    end

    if girl:has_trait(wm.TRAITS.MUSCULAR, wm.TRAITS.STRONG) then
        desc = desc .. RandomChoice("She is very obviously fit and athletic. ", 
                                    "Her muscles ripple as she moves. ",
                                    "She wears skin tight clothes that show off her ripped body. ")
    end

    if girl:has_trait(wm.TRAITS.GIANT, wm.TRAITS.TALL) then
        desc = desc .. "She is quite tall. " .. RandomChoice("You wonder how the weather is up there. ", 
                                                             "She may give you an under-boob fetish. ")
    elseif girl:has_trait(wm.TRAITS.SHORT, wm.TRAITS.DWARF) then
        desc = desc .. "She is very short. " .. RandomChoice("She has to look up to meet your eyes. ",
                                    "You can easily see the top of her head. ")
    end

    if girl:has_trait(wm.TRAITS.LONG_LEGS) then
        desc = desc .. RandomChoice ("Her legs seem to go on forever. ",
                                     "She has beautiful legs. ")
    end

    if girl:has_trait(wm.TRAITS.LARGE_HIPS, wm.TRAITS.WIDE_BOTTOM) then
        desc = desc .. RandomChoice ("She is broad across the beam. ",
                                     "She is delightfully thicc. ")
    end

    if (girl:has_trait(wm.TRAITS.MATURE_BODY, wm.TRAITS.MILF, wm.TRAITS.MIDDLE_AGED) or (girl:age() > 30 and girl:age() < 61)) then
        desc = desc .. RandomChoice ("You wonder if she has kids. ",
                                     "You think she possibly has children. ",
                                     "She might be a mom. ")
    end

    if (girl:has_trait(wm.TRAITS.OLD) or girl:age() > 61) then
        desc = desc .. RandomChoice ("She is well past the bloom of youth. ",
                                     "She clearly knows her way around. ")
    end

    if girl:has_trait(wm.TRAITS.ALCOHOLIC, wm.TRAITS.FAIRY_DUST_ADDICT, wm.TRAITS.SHROUD_ADDICT, wm.TRAITS.VIRAS_BLOOD_ADDICT) then
        desc = desc .. RandomChoice ("You wonder if she is on something. ",
                                     "She seems just a touch desperate. ")
    end

    if girl:has_trait(wm.TRAITS.DIFFERENT_COLORED_EYES, wm.TRAITS.STRANGE_EYES) then
        desc = desc .. "There is something weird about her eyes. "
    end

    if girl:has_trait(wm.TRAITS.MISSING_FINGER, wm.TRAITS.MISSING_FINGERS, wm.TRAITS.MISSING_TEETH, wm.TRAITS.MALFORMED,
            wm.TRAITS.NO_ARMS, wm.TRAITS.NO_FEET, wm.TRAITS.NO_HANDS, wm.TRAITS.NO_LEGS, wm.TRAITS.NO_TEETH, wm.TRAITS.ONE_ARM,
            wm.TRAITS.ONE_EYE, wm.TRAITS.ONE_FOOT, wm.TRAITS.ONE_HAND, wm.TRAITS.ONE_LEG, wm.TRAITS.BLIND, wm.TRAITS.EYE_PATCH) then
        desc = desc .. "You notice that she has a physical handicap. "
    end

    if girl:has_trait(wm.TRAITS.ADVENTURER, wm.TRAITS.ASSASSIN) then
        desc = desc .. "She looks like she could handle herself in any situation. "
    end

    if girl:has_trait(wm.TRAITS.WHORE, wm.TRAITS.PORN_STAR) then
        desc = desc .. RandomChoice("She has that hard look associated with being with too many men. ",
                                    "You feel she has traded her body for money more than once. ") 
    end

    if girl:has_trait(wm.TRAITS.AGILE, wm.TRAITS.FLIGHT, wm.TRAITS.FLEXIBLE, wm.TRAITS.FLEET_OF_FOOT, wm.TRAITS.WINGS) then
        desc = desc .. RandomChoice("She seems very light on her feet. ",
                                    "She moves as if she is about to launch into the sky. ")
        if girl:has_trait(wm.TRAITS.WINGS) then
            desc = desc .. "Her visible wings make you think this woman can fly. "
        end
    end

    if girl:has_trait(wm.TRAITS.NATURAL_PHEROMONES) then
        desc = desc .. RandomChoice("Her scent is intoxicating. ",
                                    "You resist an urge to sniff her hair. ")
    end

    if girl:has_trait(wm.TRAITS.STRONG_MAGIC, wm.TRAITS.POWERFUL_MAGIC) then
        desc = desc .. RandomChoice("The very air about her seems charged with energy. ",
                                    "Her eyes almost look like they could bore a hole through a wall. ")
    end

    if girl:has_trait(wm.TRAITS.PSYCHIC) then
        desc = desc .. RandomChoice("She looks at you as if she knows exactly what you are thinking. ",
                                    "Her glance seems to read your very soul. ")
    end

    if girl:has_trait(wm.TRAITS.BRANDED_ON_THE_ASS) or girl:has_trait(wm.TRAITS.BRANDED_ON_THE_FOREHEAD) then
        desc = desc .. RandomChoice("You can see a prominent slave tattoo. ",
                                    "She seems resigned to the look on your face when you notice her slave brand. ")
    end

    if girl:has_trait(wm.TRAITS.TATTOOED, wm.TRAITS.SMALL_TATTOOS, wm.TRAITS.HEAVILY_TATTOOED) then
        desc = desc .. "You see that she is a tattoo fan. "
    end

    if girl:has_trait(wm.TRAITS.PIERCED_TONGUE, wm.TRAITS.PIERCED_NOSE, wm.TRAITS.PIERCED_NAVEL, wm.TRAITS.PIERCED_NIPPLES) then
        desc = desc .. "She wears clothes designed to show off her piercings. "
    end

    if girl:has_trait(wm.TRAITS.SMALL_SCARS, wm.TRAITS.COOL_SCARS, wm.TRAITS.HORRIFIC_SCARS) then
        desc = desc .. "You notice she has been scarred. "
        if girl:has_trait(wm.TRAITS.COOL_SCARS) then
            desc = desc .. "On her, though, the scars add a feeling of mystery and adventure. "
        end 
    end

    if girl:has_trait(wm.TRAITS.BRUISES) then
        desc = desc .. RandomChoice("She may have been abused lately, as you can see fresh bruises. ",
                                    "She has a patchwork of fresh and fading bruises on her body. ",
                                    "She has fading but still colorful bruises on her arms and legs.")
    end

    Dialog (desc)
end

---@param girl wm.Girl
---@param dangerLev number
function GroupChurch(girl, dangerLev)
    if dangerLev == 1 then
        Dialog("GroupChurch1 coming soon!")
    elseif dangerLev == 2 then
        Dialog("GroupChurch2 coming soon!")
    elseif dangerLev == 3 then
        DefendBrothel(girl)
    else
        Dialog("Unknown danger level GroupChurch")
    end
end

---@param girl wm.Girl
---@param dangerLev number
function GroupStreets(girl, dangerLev)
    if (dangerLev == 3) and wm.Percent(75) then
        Dialog("The women in the town seem to be smarter than you, and most are staying off the dangerous streets.")
        Dialog("You can't find any action. The few women that are out, look at your bodyguards nervously and move to the other side of the street.")
        Dialog("You decide to go to a club where the bodyguards can blend into the crowd.")
        GroupStripClub(girl, dangerLev)
        return
    elseif (dangerLev == 1) then
        local score = 0
        Dialog("A streetwalker notices you walking about the peaceful streets, guarded by a full crew of a dozen toughs.")
        Dialog("She starts laughing. \"What brings a tough guy like you to this neighborhood? You must find us whores pretty scary to need that much backup.\"")
        GirlDescription(girl)
        Dialog("You give it right back. \"Maybe I am looking to thank my crew for doing such good work. You want to be their reward?\"")
        if wm.Percent(50) then
            Dialog("She stops laughing. \"Nuh-uh, sugar. All of you would tear me up too bad to work for a couple days, and it ain\'t worth it.\"")
            Dialog("You: \"Well, you might not find any customers tomorrow, but I have a wad of cash in my pocket, today.\"")
            Dialog("Girl: \"Gonna take at least 1000 gold for all of you. You got that much, sugar daddy?\"")
        else        
            score = score + 1
            Dialog("Some of your thugs grin at her answer. \"Sure, I got a baker\'s dozen special, so I will do you, too, bossman. All of you for 1000 gold.\"")
        end
        local whore = ChoiceBox("Take the girl up on her offer?",
                                "Sounds good to me, but only if I get to test you out first.",
                                "I\'ll pass, but I am sure my men will enjoy you.",
                                "Woah, I don\'t think you can be worth that much. I think we will pass.")
        if whore == 2 then
            Dialog("She turns her back on you. She looks back and addresses your guards: \"You guys sure have a cheap-ass boss. You can do better.\"")
            wm.SetPlayerDisposition(-5)
            return
        else
            Dialog("You hire a suite from a hotel, order room service for 14, and retire to the bedroom with the hooker. This costs you an extra 500 gold.")
            wm.TakePlayerGold(1500)
            if whore == 0 then
                Dialog("You take her into one of the bedrooms and tell her to get naked.")
                wm.UpdateImage(wm.IMG.STRIP)
                Dialog("While she is getting undressed, you tell her to take good care of your men. If she does, you will offer her a job afterwards.")
                Dialog("Girl: \"That is a nice offer, but I think I need to take care of you first.\"")
                Dialog("You grin, and point to your dick. \"I agree. That ain\'t gonna suck itself!\"")
                if wm.Percent(girl:oralsex()) then
                    wm.UpdateImage(wm.IMG.ORAL)
                    Dialog("She is quite good. As she sucks, licks and rubs your manhood, you can feel the cum boiling in your balls.")
                    Dialog("For the finale, you grab the back of her head, hold her still, and face fuck her savagely before pumping your joy juice down her throat.")
                else
                    wm.UpdateImage(wm.IMG.HAND)
                    Dialog("She manages to get you off, but being a connoisseur of head, you note that she uses her hand too much.")
                    Dialog("But as they say, there is no such thing as a bad blowjob. You aren\'t complaining as you pump your seed into her mouth.")
                end
                Dialog("You rejoin your men and let them have their way with your present.")
            end

            local men = 1
            
            repeat
                local howmany = 1
                if wm.Percent(30) then
                    howmany = wm.Range(2, 4)
                end
                if men + howmany > 13 then
                    howmany = 13 - men
                end
                local sexact = wm.Range(0, 5)
                if howmany == 2 then
                    wm.UpdateImage(wm.IMG.GROUP)
                    if wm.Percent(girl:group()) then
                        score = score + 2
                        Dialog("Thugs #" .. men .. " & #" .. (men + 1) .. ": It sounds like the bed breaks, before they return with huge grins on their faces.")
                    else
                        Dialog("Thugs #" .. men .. " & #" .. (men + 1) .. ": After a long, quiet interlude, they think it should have been more fun.") 
                    end
                elseif howmany == 3 then
                    wm.UpdateImage(wm.IMG.GROUP)
                    if wm.Percent(girl:group()) then
                        score = score + 3
                        Dialog("Thugs #" .. men .. " to #" .. (men + 2) .. ": They pick up the girl and take her into the bedroom.")
                        Dialog("They return grinning. \"Woah, she is airtight!\"")
                    else
                        Dialog("Thugs #" .. men .. " to #" .. (men + 2) .. ": When they return, they are frowning, and so is the girl.") 
                    end
                elseif howmany == 4 then
                    wm.UpdateImage(wm.IMG.GROUP)
                    if wm.Percent(girl:group()) then
                        score = score + 4
                        Dialog("Thugs " .. men .. " to #" .. (men + 3) .. ": Four guys at once is a lot. But she managed it!")
                        Dialog("Each guard gives her a warm kiss and a thank you when they come back.")
                    else
                        Dialog("Thugs " .. men .. " to #" .. (men + 3) .. ": Four guys at once is a lot. All five participants look sore when they return.") 
                    end
                elseif sexact == 0 then
                    wm.UpdateImage(wm.IMG.FOOT)
                    if wm.Percent(girl:footjob()) then
                        score = score + 1
                        Dialog("Thug # " .. men .. ": A warm, sensual foot job makes the next guy extremely happy.")
                    else
                        Dialog("Thug # " .. men .. ": She uses her feet on him, but she might as well have kicked him instead.")
                    end
                elseif sexact == 1 then
                    wm.UpdateImage(wm.IMG.HAND)
                    if wm.Percent(girl:handjob()) then
                        Dialog("Thug #" .. men .. ": He has been dreaming about a perfect handjob all day. He got one.")
                        score = score + 1
                    else
                        Dialog("Thug #" .. men .. ": He says handjobs are great, if the girl doesn\'t have calluses.")
                    end
                elseif sexact == 2 then
                    wm.UpdateImage(wm.IMG.ANAL)
                    if wm.Percent(girl:anal()) then
                        score = score + 1
                        Dialog("Thug #" .. men .. ": This guy tells you her asshole is perfect. She is tight, clean and enthusiastic!")
                    else
                        Dialog("Thug #" .. men .. ": He is an anal enthusiast. Unfortunately for him, she doesn\'t seem quite as enthralled about butt sex.")
                    end
                elseif sexact == 3 then
                    wm.UpdateImage(wm.IMG.SEX)
                    if wm.Percent(girl:normalsex()) then
                        score = score + 1
                        Dialog("Thug #" .. men .. ": He got his rocks off inside her pussy, and is grinning from ear to ear.")
                    else
                        Dialog("Thug #" .. men .. ": He thinks she enjoys mermaid sex. She just lies there like a cold fish.")
                    end
                elseif sexact == 4 then
                    wm.UpdateImage(wm.IMG.ORAL)
                    if wm.Percent(girl:oralsex()) then
                        score = score + 1
                        Dialog("Thug #" .. men .. ": You hear him screaming behind the door. \"Yes! Eat me! EAT ME!\" She apparently did.")
                    else
                        Dialog("Thug #" .. men .. ": She apparently committed a cardinal sin. She spit instead of swallowing.")
                    end
                elseif sexact == 5 then
                    wm.UpdateImage(wm.IMG.TITTY)
                    if wm.Percent(girl:tittysex()) then
                        score = score + 1
                        Dialog("Thug #" .. men .. ": She is loudly moaning that she wants him to cum on her tits. She comes out with oil and cum in her cleavage.")
                    else
                        Dialog("Thug #" .. men .. ": Like many men, he discovers that titty fucking is not quite like a lubricated pussy.")
                    end
                end
                men = men + howmany
            until men > 12

            if whore == 1 then
                Dialog("You regret not testing her out. You admire her body as she pockets her money, wipes off about a gallon of cum, and blows you an air kiss.")
            elseif whore == 0 and score > 7 then
                wm.UpdateImage(wm.IMG.STRIP)
                Dialog("You pull the girl aside. She is tired, sweaty and covered in cum... Just the way you like them in your brothel.")
                Dialog("You: \"My men are quite happy with you. Want a job?\"")
                Dialog("Girl: \"Sounds great to me, bossman!\"")
                wm.AcquireGirl(girl)
            elseif whore == 0 then
                Dialog("You pull the girl aside at the end. She is tired and frustrated, being unable to completely satisfy a large number of your men.")
                Dialog("Girl: \"Please sir, I did my best! But I know I need good training. Surely the best whoremaster in Crossgate can help me improve!")
                local training = ChoiceBox("Take a chance on training this girl?",
                                           "\"Yes, I think with a little help you can be a great whore!\"",
                                           "\"I am afraid not. But here is your money.\"")  
                if training == 0 then
                    wm.AcquireGirl(girl)
                    Dialog("Girl: \"Oh, thank you sir! Can I do anything else for you tonight?")
                    wm.UpdateImage(wm.IMG.ORAL)
                    Dialog("You: \"As it so happens, there is.\" You stick your dick in her face again.")
                    Dialog("You: \"I didn\'t want to spoil your makeup for my men the first time. This time, you get the full facial.\"")
                    Dialog("Waiting while she serviced your men was exciting. It doesn\'t take long for you to blast a surprisingly big load on her pretty face.")
                    Dialog("She smiles through your goo. \"Mmmmm, thanks boss! See you at work tomorrow!\"")
                else
                    Dialog("She is disappointed at losing the job, but sticks the cash down her cleavage and blows you a kiss as she heads out. \"Ciao, sugar!\"")
                end
            end
        return
        end
    end
    if wm.Percent(25) then
        Dialog("As your group makes your way through the streets, you approach a lady of negotiable.")
        LadyOfNegotiableAffection(girl)
        return
    end
    Dialog("As your group makes your way through the streets, a beggar girl asks for some money.")
    Dialog(RandomChoice("Girl: \"Please, sir, you are obviously wealthy, and I haven't eaten for two days.\"",
                        "Girl: \"Mister, can you spare a little change for a pretty girl down on her luck?\"",
                        "Girl: \"Hey buddy, God loves you. Can you help me out a little?\""))
    GirlDescription(girl)
    local beggar = 0
    if dangerLev == 3 then
        beggar = ChoiceBox("What will you do with this beggar girl?",
                           "Make fun of her",
                           "Tell her to get a job",
                           "Take pity and give her 50 gold",
                           "Ignore her",
                           "Ask if she has someplace safe to get off the streets")

    else
        beggar = ChoiceBox("What will you do with this beggar girl?",
                           "Make fun of her",
                           "Tell her to get a job",
                           "Take pity and give her 50 gold",
                           "Ignore her")
    end
    if beggar == 0 then
        Dialog("You grin at your men. \"If you are that hungry, I've got some meat for you! And if you're good, I'll pay you for your time.\"")
        Dialog(RandomChoice("She wipes her nose on her sleeve, and laughs scornfully. ",
                            "She laughs derisively as she stares at your crotch. ",
                            "She scratches her ass and lets out an audible fart. ") ..
               RandomChoice("\"I need a meal, not a snack. Keep your little willy in your pants and leave me be.\"",
                            "\"You ain\'t nothing big enough to bait a hook. Come back when you grow up, little boy.\"",
                            "\"Meat? That looks like a plant-based substitute to me. And it needs fertilizer.\""))
        Dialog("One of your men stifles a laugh. You glare at him, then cut your walk short and head home.")
    elseif beggar == 1 then
        Dialog("She sneers at you. \"Sure, guv\'nor! You hiring beggar girls, are you?")
        Dialog("You step closer and examine her face intently. \"Those are fake sores, aren\'t they? I\'ll bet they wash off with a little soap and water.\"")
        Dialog("She looks uncomfortable. \"Hush, guv'nor! You\'ll give the game away to the marks!\"")
        Dialog("You continue on. \"You look to be in good enough health. Do you wash up pretty?\"")
        Dialog("You: \"Are you interested in a job with steady income and steady meals?\"")
        Dialog("She thinks a moment. \"Since you want me pretty, I assume that you expect the use of my body in this job?\"")
        Dialog("You: \"Maybe not me, but my customers certainly will.\"")
        Dialog("She grins. \"Good! I am not only hungry, I am horny as hell. Hard for a beggar girl to get dates, you know! You got a new hire, guv'nor!\"")
        wm.AcquireGirl(girl)
        girl:add_trait(wm.TRAITS.OPTIMIST)
        girl:libido(75)
    elseif beggar == 2 then
        Dialog("You throw her a 50 gold piece. \"Don\'t spend it all in one place!\"")
        Dialog("She gawks at the coin. \"Bless you, sir!\"")
        wm.SetPlayerDisposition(2)
        wm.TakePlayerGold(50)
    elseif beggar == 3 then
        Dialog("You fix your eyes straight forward and ignore her.")
        Dialog("She curses you. " .. RandomChoice("\"May your next blowjob be from a hungry zombie!\"",
                                                  "\"May the fleas of 1000 camels infest your asshole!\"",
                                                  "\"May your balls shrivel to match your soul!\""))
        wm.SetPlayerDisposition(-2)
    elseif beggar == 4 then
        Dialog("You look at the girl with some concern. \"You know tonight is not safe, for a pretty girl to be out alone?\"")
        Dialog("She sighs sadly. \"I haven\'t anyplace to go, sir. I will go hide in a nearby alley and take my chances.\"")
        Dialog("You grimace. \"That decision could cost your life tonight. Why don\'t you spend the night at my brothel?\"")
        Dialog("You see fire flash in her eyes, and quickly add, \"No strings attached, milady. No one will touch you.\"")
        Dialog("She consents, and your group escorts her back to your nearest brothel. She gasps when you offer her a room. \"So luxurious, sir!\"")
        Dialog("\"I have to keep my girls and their clients happy, you know.\" She blushes, thinking that over, and you smile to yourself.") 
        Dialog("She squeals in delight when she sees the bath. \"I would give anything for a long hot bath!\"")
        Dialog("\"Be careful saying things like that in this place!\" She blushes again, and you laugh out loud while showing her the towels, soaps, shampoos and perfumes.")
        wm.UpdateImage(wm.IMG.BATH)
        Dialog("The sound of running water is interrupted by a loud crash. You run to make sure your guest is alright.")
        Dialog("You find her in the bath, a broken bottle of expensive shampoo on the floor.")
        Dialog("The girl is mortified, and crying. \"It just slipped! I am so sorry! I will pay for that!\"")
        local shampoo = 0
        shampoo = ChoiceBox("You know she can\'t afford to replace it.",
                             "Don\'t be silly. Accidents happen.",
                             "That was very expensive! If you can\'t pay for that, you will have to work off your debt.")
        if shampoo == 0 then
            Dialog("You busy yourself clearing the floor of broken glass, while the girl quickly finishes cleaning herself.")
            Dialog("You feed her a good meal, too. Being clean and well fed quickly makes her drowsy. She retires to bed very early.") 
            wm.UpdateImage(wm.IMG.BED)
            Dialog("In the morning, you can see her mulling things over. She clearly does not want to go back out on the streets.")
            Dialog("She wistfully remarks as she packs up her things, \"I used to live like this. I wish I could again.\"")
            Dialog("Very gently, you ask, \"Would you be willing to work for room and board? You know what kind of work I am talking about?\"")
            Dialog("\"Oh, sir. I AM willing to do anything to have a bath like that every night!\" She looks at you with steely determination mixed with gratitude.")
            wm.AcquireGirl(girl)
            Dialog("A little soap and kindness can go a long way!")
        elseif shampoo == 1 then
            Dialog("She groans to herself. \"I don\'t have the cash. What kind of service do you require, sir?\"")
            Dialog("You: \"I think my thugs and I will have our way with you.\"")
            wm.UpdateImage(wm.IMG.GROUP)
            Dialog("All that evening, she \'entertains\' your group with her body. In the morning, you give her breakfast and set her back on the streets.")
            Dialog("You give her a tip of 200 gold, and tell her to keep safe.")
            if wm.Percent(girl:intelligence()) then
                Dialog("She looks at the money, and begs to stay. \"Please, I would like to stay, even though I know what working here means.\"")
                Dialog("You: \"I hoped you would think that way. Welcome aboard!\"")
                wm.AcquireGirl(girl)
            end
        end
    end
end

---@param girl wm.Girl
---@param dangerLev number
function GroupShopping(girl, dangerLev)
    Dialog("GroupShopping danger=" .. dangerLev)
    if dangerLev == 1 then
        Dialog("Coming soon!")
    elseif dangerLev == 2 then
        Dialog("Coming soon!")
    elseif dangerLev == 3 then
        Dialog("Despite the dangerous streets, you are determined to get a birthday present for one of your favorite girls.")
        Dialog("She has dropped hints that she wants a pearl necklace from one of the best jewelers in town.")
        Dialog("But when you get to the shop, you find it has burned to the ground. You recognize one of the clerks. \"What happened here?\"")
        Dialog("Girl: \"Our owner was paying protection to the local crime boss, but then another gang tried to move in.\"")
        Dialog("The owner refused to pay twice, so they killed him and burned the shop.\"")
        Dialog("Looks like there is a gang war brewing. \"Oh, no. What are you going to do now?\"")
        Dialog("Girl: \"I don't really know. I guess I'll have to find another job.\" She smiles sadly. \"I don't suppose you know anyone hiring, do you?\"")
        local response = ChoiceBox("How do you respond?", 
                                   "I own a lot of businesses, but you will be handling completely different types of jewels.", 
                                   "If you want to work for me, you will have to audition. With your parts.")
        if response == 0 then
            Dialog("She smiles: \"Oh, you could call that a leading question. I know all about your line of work.\"")
            Dialog("Girl: \"I stripped to earn school money. A little skin... and maybe a bit more... isn't really a problem.\"")
            Dialog("You: \"Really? Then I have a proposition for you. One of my girls wants something pretty from here, and she mentioned a pearl necklace.\"")
            Dialog("\"She\'s very bisexual. While I can handle giving her a pearl necklace all by myself, I think you could be the pretty gift from this store.\"")
            Dialog("The clerk grins wickedly. \"I'll bet that is going to be a BIG pearl necklace. I think you will find I can be pretty 'handy' for that part, too.\"")
            wm.UpdateImage(wm.IMG.GROUP)
            wm.AcquireGirl(girl)
            Dialog("Your girlfriend loves her two-part gift that night. And with your new employee's help, you really enjoy producing a big, sticky pearl necklace.")
        elseif response == 1 then
            Dialog("She frowns: \"I don\'t think you said that right. You mean I have to audition FOR a part, right?\"")
            Dialog("You and your men chuckle knowingly. \"Nope. I said exactly what I mean. Do you want me to spell out what parts I am talking about?\"")
            if wm.Percent(50) then
                Dialog("Girl: \"No, I get the idea.\" She shakes her head sadly. \"I had heard you had some class, but I think I will find another way to get by.\"")
            else
                Dialog("She grimaces... then puts an obviously fake smile on her face. \"OK, I am game. Let\'s do this!\"")
                GirlAudition(girl)
            end    
        end
    end
end

---@param girl wm.Girl
---@param dangerLev number
function GroupStripClub(girl, dangerLev)
    if dangerLev == 1 then
        Dialog("You and your bodyguards strut confidently through the quiet city streets to a local strip club.")
        Dialog("Your thugs take positions around you, until you wave them off and tell them to get a drink.")
        StripperOrWaitress(girl, dangerLev) 
    elseif dangerLev == 2 then
        if wm.Percent(99) then
            Dialog("You have no problem getting to the strip club, and ask your men to stay at the bar, where they can watch unobtrusively.")
            StripperOrWaitress(girl, dangerLev)
            Dialog("You enjoy the walk home, talking to your guards about the women at the club.")
        else
            StreetAmbush(girl, dangerLev)
        end
    elseif dangerLev == 3 then
        if wm.Percent(60) then
            Dialog("The streets have a brooding, dangerous air. You are relieved to get to the strip club without a problem.")
            Dialog("Now you are interested in a different type of relief.")
            Dialog("You ask your men to stay at the bar, where they can watch without interfering.")
            StripperOrWaitress(girl, dangerLev)
            Dialog("Your men safely escort you home through the dangerous streets.")
        else
            if wm.Percent(50) then
                StripperOrWaitress(girl, dangerLev)
            else
                StreetAmbush(girl, dangerLev)
            end
        end
    end
end

---@param girl wm.Girl
function LadyOfNegotiableAffection(girl)
    if girl:has_trait(wm.TRAITS.CUM_ADDICT) then
        Dialog("A girl is standing on the street corner. She eyes you hungrily. \"Hey, good-looking...  Wanna party?\"")
        GirlDescription(girl)
        local cumaddict = ChoiceBox("Do you want to give this hungry girl something to eat?",
                                    "I don\'t pay for sex, girl.",
                                    "I like parties. Let\'s do it.")
        if cumaddict == 0 then
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("She shrugs. \"I\'ll do you for free on one condition: You have to cum in my mouth.\"")
            Dialog("Surprised, you let her lead you into a nearby alley.  She tugs your pants off quickly, and starts pulling at your cock like a heifer on an udder.")
            Dialog("Between sucks, she looks at you almost desperately. \"Come on, mister. Shoot it! Get it in my belly!\"")
            Dialog("She is good. She knows just what to touch, when to rub, and what to suck. You quickly erupt at the back of her throat.")
            girl:oralsex(70) 
            Dialog("She swallows, smiles widely, and then sizes you up again. \"Hey, do you fancy another? I'm still hungry.\"")
            Dialog("You are intrigued. \"Honey, if you like cum that much, I have a job where you will get plenty of dick, and I'll pay you well on top of it.\"")
            Dialog("She smiles. \"That sounds wonderful. But are you sure you don\'t have another load in there for me?\"")
            wm.UpdateImage(wm.IMG.ORAL)           
            Dialog("As it turns out, you do.")
            wm.AcquireGirl(girl)
            return
        else
            GirlAudition(girl)
        end
    elseif girl:has_trait(wm.TRAITS.HOMELESS) then
        Dialog("A desperate looking girl is sitting on the sidewalk as you go by, a small bundle of possessions at her feet. \"Hey, mister. Can you help a girl out?\"")
    elseif girl:has_trait(wm.TRAITS.AGGRESSIVE, wm.TRAITS.YANDERE, wm.TRAITS.BRAWLER) then
        Dialog("A girl walks up to you, pushes you against a wall, and says, \"Wanna try to satisfy me, dude?\"")
    elseif girl:has_trait(wm.TRAITS.AUDACITY, wm.TRAITS.TOMBOY) then
        Dialog("A girl walks by you, slaps your ass, and wolf whistles at you. \"You might be worth skipping work for, honey.\"")
    elseif girl:has_trait(wm.TRAITS.FAIRY_DUST_ADDICT) then
        Dialog("A desperate looking girl is hugging herself on the corner. \"You look like you need something as much as I do, buddy.\"")
    elseif girl:has_trait(wm.TRAITS.SHROUD_ADDICT) then
        Dialog("A girl in scruffy clothes is scratching at herself. \"Dude, let\'s party, OK?\"")
    elseif girl:has_trait(wm.TRAITS.VIRAS_BLOOD_ADDICT) then
        Dialog("You feel a hand on your shoulder, and turn to see a girl with crazy dilated eyes right in your face. \"Hey there, friend! Wanna get your freak on?\"")
    elseif girl:has_trait(wm.TRAITS.BIMBO) then
        Dialog("You walk by an outdoor cafe. A girl sitting alone at a table flips her hair at you. \"Hey cutie, I\'m bored and you look like fun.\"")
    elseif girl:has_trait(wm.TRAITS.DOJIKKO) then
        Dialog("A streetwalker approaches, but before she can even give you a come-on, she trips and falls into you.")
        Dialog("She must have been a linebacker in another life, as she neatly cuts your legs from beneath you.")
        Dialog("You both fly into the air and land in a heap. You have landed on top of her with your face between her breasts and one of your hands up her skirt.")
        Dialog("She smiles brightly. \"Well, now that we\'ve been introduced, want to continue this party a little more privately?\"")
    elseif girl:has_trait(wm.TRAITS.DOMINATRIX, wm.TRAITS.SADISTIC) then
        Dialog("A woman wearing a black leather skirt sneers at you. \"A worm like you doesn\'t deserve the pleasure I could give you.\"")
        Dialog("She looks you up and down, and apparently discovers some hidden worth anyway. \"Still... would you like to party, little boy?\"")
    elseif girl:has_trait(wm.TRAITS.NYMPHOMANIAC) then
        Dialog("A seemingly desperate woman looks you in the eye, rubs her hand up her crotch and shivers delicately. \"Buddy, I need it bad. Party with me, please?\"")
    elseif girl:has_trait(wm.TRAITS.EXHIBITIONIST) then
        Dialog("As you walk by, a woman lifts her top to show you her breasts. \"Now that you\'ve seen \'em, do you want to play with \'em?\"")
    elseif girl:has_trait(wm.TRAITS.SLUT) then
        Dialog("A woman on the corner is sucking a lollipop. She sticks her tongue out at you. \"There's going to be a party in my mouth, soon. Wanna come?\"")
    elseif girl:has_trait(wm.TRAITS.SUCCUBUS, wm.TRAITS.TWISTED) then
        Dialog("A woman walks by you, swaying a little more than strictly necessary. \"You look like a horny little devil. Wanna party?\"")
    elseif girl:has_trait(wm.TRAITS.DEMON_POSSESSED, wm.TRAITS.DEMON) then
        Dialog("A woman accosts you in a deep, gravelly voice. \"My mother sucks cocks in hell. I suck cocks on earth. Want me to do yours?\"")
    elseif girl:has_trait(wm.TRAITS.ANGEL) then
        Dialog("A woman with soft, bright features says sweetly, \"Fornication is actually blessed by heaven. Shall we pray?\"")
    elseif girl:has_trait(wm.TRAITS.SPIRIT_POSSESSED) then
        Dialog("A woman approaches weirdly, almost as if she is still learning how to walk. \"Would you like to use this body?\"")
    elseif girl:has_trait(wm.TRAITS.PORN_STAR, wm.TRAITS.ACTRESS, wm.TRAITS.IDOL) then
        Dialog("As you pass by a woman in the street, you do a double take. You are certain you have seen her on a movie or TV screen.")
        Dialog("She notices your attention and approaches you. \"You recognize me, I think. Wanna party with someone famous?\"")
    elseif girl:has_trait(wm.TRAITS.MEEK, wm.TRAITS.SHY, wm.TRAITS.NERVOUS, wm.TRAITS.PESSIMIST) then
        Dialog("A girl timidly approaches you. She looks ready to bolt, but manages to ask, \"Excuse me sir, but... maybe you would like to party with me?\"")
    elseif girl:has_trait(wm.TRAITS.OPTIMIST) then
        Dialog("A girl smiles brightly when she sees you. \"Oooooh, I just knew someone like you would come along. Ready to party, sweetie?\"")
    elseif wm.Percent(60) then
        Dialog("You find yourself catching up to an woman going the same direction. She looks over her shoulder, smiling. \"Don\'t run. I'll wait for you. Wanna get together for a party?\"")
    else
        Dialog("A pretty woman licks her lips and looks you over. \"Looking for a party, sugar?\"")
    end
    GirlDescription(girl)
    local partychoice = ChoiceBox("Do you want to \"party\" with her?",
                                  RandomChoice("\"If we are partying but I'm not paying, then \"Yes!\"",
                                               "\"You don't measure up to my standards, babe.\"",
                                               "\"I am afraid I don't pay for parties."),
                                  RandomChoice("\"Looks like today might be my 'getting lucky' day!\"",
                                               "\"You are just what I have been looking for today, girl!\"",
                                               "\"Well, well, you are certainly in the right place at the right time!\""))
    if partychoice == 0 then
        Dialog("She sniffs. \"You are saying no to me? Honey, you just missed out on ten minutes of heaven.\"\n" ..
                "Then she sneeringly looks you up and down. \"Naw, I take that back. Thirty seconds tops.\"\n" ..
                "She flips you off as she walks away.")
    elseif partychoice == 1 then
        GirlAudition(girl)
    end
end

---@param girl wm.Girl
function RapeGirl(girl)
    Dialog("She is obviously asking for it. You resolve to give it to her.")
    Dialog("You time it perfectly. You draw even with her just as you pass the mouth of a dark alleyway.")
    if wm.Percent(5) or girl:has_trait(wm.TRAITS.ASSASSIN, wm.TRAITS.HEROINE) then
        Dialog("You grab her, muffling her cries, and pull her into the alley.")
        Dialog("Unfortunately for you, she is no shrinking violet, and bites your hand. She draws blood, even through your gloves.")
        Dialog("You can't help but let her go... and are dismayed to find she has pulled a knife on you!")
        Dialog("Well, the two of you take to fighting, but she turns out to be expert with her weapon.")
        Dialog("She carves you into a jigsaw puzzle, with a couple of pieces gone.")
        wm.GameOver()
        return       
    elseif girl:has_trait(wm.TRAITS.DEMON, wm.TRAITS.VAMPIRE, wm.TRAITS.UNDEAD, wm.TRAITS.SKELETON, wm.TRAITS.ZOMBIE) then
        Dialog("Clapping a hand over her mouth, you hustle her down the alley, out of sight of the street.")
        Dialog("You threaten her with your knife, but she just starts laughing at you.")
        Dialog("\"Silly human! You thought you were hunting me, but I am actually the hunter, and you are my prey.\"")
        Dialog("Your last thought, as she tears you limb from limb, is that you picked on the wrong supernatural being.")
        wm.GameOver()
        return
    elseif girl:has_trait(wm.TRAITS.SUCCUBUS) then
        Dialog("Clapping a hand over her mouth, you hustle her down the alley, out of sight of the street.")
        Dialog("You threaten her with your knife, but she just starts laughing at you.")
        Dialog("Her eyes start glowing, and you do not resist as she tears your clothes off.")
        Dialog("She grabs your cock and sucks out a huge torrent of cum. You muzzily think this is not going the way you wanted.")
        Dialog("She swallows lustily, then sneers at you. \"Forcing a succubus never ends well. Silly human! Now I am going to fuck you to death.\"")
        Dialog("She pushes you to the ground, mounts you, and forces you to orgasm, time after time. You feel yourself growing weaker with each spasm.")
        Dialog("Your heart fails as she rips one last orgasm from you. The last sound you hear is her mocking laughter, as she feeds on your life force.")
        wm.GameOver()
        return
    elseif girl:has_trait(wm.TRAITS.AGILE, wm.TRAITS.BRAWLER) then
        Dialog("Clapping a hand over her mouth, you hustle her down the alley, out of sight of the street.")
        Dialog("Initially too startled to resist, she twists easily and slips out of your grasp.")
        Dialog("As you try to follow her move, you run right into a knee to the crotch. She runs away while you tend to your boys.")
        Dialog("You hobble out of the area as quickly as possible, before she returns with the city guard.")
        Dialog("The worst part is that she saw your face. You must expect increased police scrutiny. Bummer.")
        wm.SetPlayerSuspicion(100)
        wm.SetPlayerDisposition(-25)
        return
    elseif girl:has_trait(wm.TRAITS.FLIGHT) then
        Dialog("Clapping a hand over her mouth, you hustle her down the alley, out of sight of the street.")
        Dialog("She twists in your grasp and hugs you tightly. Suddenly you realize your feet aren't touching the ground. The girl is flying you upwards!")
        Dialog("She gets about twenty feet up, moves like a bull shaking off an inept rider, and you fall onto your back.")
        if wm.Percent(95) then
            Dialog("You crack your head painfully, but not fatally.")
            Dialog("Sadly, she not only mocked you while flying away, she also saw your face. You must expect increased police scrutiny. Bummer.")
            wm.SetPlayerSuspicion(100)
            wm.SetPlayerDisposition(-25)
            return
        else
            Dialog("Unfortunately for you, like your mother, she dropped you on your head.")
            Dialog("You look like Humpty Dumpty, and there ain\'t no putting you back together again.")
            wm.GameOver()
            return
        end
    elseif girl:has_trait(wm.TRAITS.FLEET_OF_FOOT) then
        Dialog("You are huffing and puffing with the effort of keeping up with her. Amused, she turns around, then gasps as she sees the look in your eye.")
        Dialog("She starts running before you make your move. She\'s GOTTA GO FAST, and you realize you won\'t catch her.")
        Dialog("The only good thing is that her story won\'t be that convincing. After all, you really didn\'t do anything.")
        wm.SetPlayerSuspicion(10)
        wm.SetPlayerDisposition(-5)
        return
    else
        wm.UpdateImage(wm.IMG.RAPE)
        Dialog("Before she knows what is happening, you hustle her down the alley and place your knife at her throat.\n" ..
                "You snarl at her. \"Keep quiet, do exactly what I say, and you will live through this. Understand?\"\n " ..
                "She whimpers a little and nods her head.")
        local rape = ChoiceBox("What do you make her do?", "Hand job", "Blow job", "Titfuck", "Vaginal Sex")
        if rape == 0 then
            wm.UpdateImage(wm.IMG.HAND)
            Dialog("She is very scared and fumbles getting your dick out of your pants, but it doesn\'t matter.")
            Dialog("You are drunk with power, your dick is iron hard, and she only needs a few strokes before you pop.")
        elseif rape == 1 then
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("She is too scared and the head is bad. She can\'t summon enough spit, and she won\'t take you very deep.")
            Dialog("You don\'t care. The knife in your hand is what matters, and your dick is iron hard from the power.")
            Dialog("You roughly pull out of her mouth and squirt semen all over her face.")
        elseif rape == 2 then
            wm.UpdateImage(wm.IMG.TITTY)
            Dialog("You make her remove her top, and start ramming your dick between her tits.")
            Dialog("She is wimpering as you use her breasts, but the roaring in your ears is almost too loud for you to hear.")
            Dialog("Despite the lack of lubrication, you dick rams between her tits a few times and you coat her chin with goo.")
        elseif rape == 3 then
            wm.UpdateImage(wm.IMG.STRIP)
            Dialog("You make her strip, watching impatiently.")
            wm.UpdateImage(wm.IMG.SEX)
            Dialog("She starts crying as you force your dick inside her, but she stays quiet.")
            Dialog("Your dick feels as hard as the iron in your knife. The illicit power is too much, and after three thrusts you fill your victim with cum.")
        end
        Dialog("The orgasm was barely worth it. Disappointed, your mind returns to the girl.")
        local after = ChoiceBox("She saw your face! What are you going to do?",
                                "Kill her.",
                                "Tell her you know where she lives, and you will kill her if she says anything.",
                                "In for a penny... Kidnap her and throw her in your dungeon.")
        if after == 0 then
            Dialog("You shrug. \"Sorry, honey. It's been fun.\" You slash her throat while trying to muffle her cries of agony.\n\n" ..
                    "Unfortunately, she gets out one or two shrill screams before the deed is done.")
            Dialog("The noise attracts the attention of a city guard. \"What are you doing? Unhand that woman!\"\n" ..
                    "That\'s when the guard sees the blood, and that the woman\'s head is half cut off. \"You'll hang for that, you monster!\"")
            Dialog("You are too drained from your exertions to outrun this cop." ..
                    "Your only chance is to kill this guard before reinforcements arrive. You draw your sword and charge.")
            Dialog("Unfortunately, this guard is an old vet, who calmly draws a pistol and blows your brains out. \"Never bring a sword to a gun fight.\"")
            wm.GameOver()
            return
        elseif after == 1 then
            Dialog("She meekly nods that she understands. \"I... I won't say nothin\' to no one. Just leave me be!\"\n" ..
                    "You take your leave, but make a note to expect increased police scrutiny.")
            wm.SetPlayerSuspicion(100)
            wm.SetPlayerDisposition(-50)
        elseif after == 2 then
            Dialog("You: \"Honey, that was some mighty fine action. I\'m gonna need some more of that. You are going to enjoy working for me.\"\n" ..
                    "You knock her unconscious and carry her home.")
            wm.AcquireGirl(girl)
            wm.ToDungeon(girl, 3)  -- 3 is kidnapped
            girl:happiness(-100)
            girl:obedience(-100)
            girl:tiredness(100)
            girl:health(-100)
            girl:health(10)
            girl:pcfear(100)
            girl:pclove(-200)
            wm.SetPlayerSuspicion(50)
            wm.SetPlayerDisposition(-100)
        end
    end
end

---@param girl wm.Girl
function ShakeDown(girl)
    Dialog("A pretty girl approaches you. \"C\'mon, mister, let\'s have some fun!\"")
    girl:add_trait(wm.TRAITS.BRUISES)
    GirlDescription(girl)
    Dialog("She takes you to a local motel. It's a fairly sleezy little place, but as long as the mattress springs don't squeak, you won't be choosy.")
    Dialog("She leads you to the bed and tugs your pants down immediately. When she sees your penis, she exclaims loudly, \"Wow, that's a big one!\"")
    Dialog("You have no time to enjoy her compliment, however. A large man brandishing a knife bursts through the door.")
    Dialog("Thug: \"Whatchoo doing with my woman? I'm gonna cut that thing right off of you!\"")
    Dialog("The situation is a little awkward. You have your pants around your legs, no weapon readily available, and this guy is quite intimidating.")
    Dialog("You are pretty sure this is a con. His entrance is a little too obviously timed to the girl\'s loud exclamation.")
    Dialog("They probably won\'t be too violent. Dead marks bring police attention they don\'t want. \"Be cool, man. I\'m sure we can work something out.\"")
    Dialog("The man grunts and relaxes fractionally. \"Yeah, tell ya what. Drop your wallet and get out. That little thang ain\'t worth the effort to cut off.\"")
    local fightorflight = ChoiceBox("That knife looks sharp, too. What do you do?",
                                    "Pull up your pants and argue with the man.",
                                    "Buy your way out of trouble and drop your wallet on the bed.",
                                    "Talk to the girl.")
    if fightorflight == 0 then
        wm.UpdateImage(wm.IMG.COMBAT)
        Dialog("You start to pull your pants up to fight effectively, but discover a small problem.")
        Dialog("The girl has a firm grip on your trousers, and isn\'t letting go. She grins as you turn to face her. \"Sorry, fella.\"")
        Dialog("You feel a heavy blow on the back of your head, and darkness descends.")
        Dialog("...")
        if wm.Percent(5) then
            Dialog("You wake up floating two feet off the floor, looking down at... yourself?")
            Dialog("You hear the girl snarling at her partner. \"Dammit, you hit him too hard. He\'s dead!\"")
            Dialog("You see the man pull your wallet out of your pants, brimming with cash. \"So what? Look at this score!\"")
            Dialog("And that is the last sound you ever hear.")
            wm.GameOver()
        else
            Dialog("You wake up with a splitting headache, and your wallet missing. That is 1000 gold you will never get back.")
            wm.TakePlayerGold(1000)
            Dialog("You\'ve been sucker punched before. You will live. But maybe next time you will reconsider fighting when the odds are so heavily against you.")
        end
    elseif fightorflight == 1 then
        Dialog("You are also outnumbered. The girl has a firm grip on your trousers, further restricting your options.")
        Dialog("You sigh and surrender to the inevitable. You drop your wallet on the bed.")
        wm.TakePlayerGold(1000)
        Dialog("Then you hoist up your pants, and beat a hasty retreat.")
    elseif fightorflight == 2 then
        wm.TakePlayerGold(1000)
        Dialog("You look down at the girl. She is scared and looks back at you fearfully.")
        Dialog("Girl: \"Just give him the money, please. He hurts me if this doesn\'t go smoothly.\"")
        Dialog("You whisper back: \"You want out? There\'s a business card in my wallet.\" You then hand the girl your wallet and pull up your pants.")
        Dialog("You move to the door, while the guy with the knife heads towards the girl. \"Hey, now, I said put that money on the bed, dumbass!\"")
        Dialog("You need to give her a little time. You turn before leaving. \"Pretty low-rent con, by the way.\"")
        Dialog("The man turns to you and snarls. \"Get out of here before I change my mind.\" Behind him, the girl sneaks out your business card.")
        Dialog("You give her a little more time. \"This is probably the best a loser like you can do, though. I\'m surprised such a pretty girl even works with you.\"")
        if wm.Percent(girl:intelligence()) then
            Dialog("She\'s smart, too. She\'s leaving the money and hiding the business card on her person. You like quick thinkers.")
        else
            Dialog("She grabs some of your cash, while you are arguing with her buddy. You hope she gets it all hidden, but you don\'t have any time left.")
        end
        Dialog("Thug: \"Who you calling a loser, dead man?\" You shut the door and start to leave.")
        Dialog("You weren\'t quick enough to avoid hearing a terrified wail from the girl, and a meaty slap. \"Put that wallet down on the bed, you bitch!\"")
        Dialog("... ... ... ... ... ...")
        if wm.Percent(girl:intelligence()) or wm.Percent(girl:intelligence()) then
            Dialog("The next day, a guard at your brothel says a girl with your business card is asking to see you.")
            Dialog("Sure enough, it\'s her. She has been badly beaten. Despite that, her eyes spark fire when she sees you.")
            Dialog("\"How are you going to help me?\" she demands. \"He\'s going to beat me again when he finds out I came to you.\"")
            Dialog("You: \"I\'m going to offer you a job. It's not going to be... totally respectable, maybe.\"")
            Dialog("You shrug. \"You are going to have to deliver on your promises, instead of just luring marks into an ambush.\"")
            Dialog("You: \"I can promise steady work, room and board, and guards to make sure that guy never touches you again.\"")
            Dialog("She smiles. It's kind of gruesome through the bruising, but it\'s clearly a smile. Then she hauls off and smacks you across the face.")
            Dialog("Girl: \"That\'s for pissing him off while I got your card. You knew he was going to beat me up.\"")
            Dialog("Then she kisses you, hard. \"And that\'s for giving me a chance to escape.\"")
            Dialog("You lost 1000 gold in your wallet yesterday, but ended up with a new employee!")
            girl:health(-25)
            girl:pclove(25)
            wm.AcquireGirl(girl)
        else
            Dialog("After a few days, you sigh. The girl is not going to show up.")
            Dialog("That 1000 gold in your wallet is gone forever.") 
        end
    end
end

---@param girl wm.Girl
---@param dangerLev number
function SoloChurch(girl, dangerLev)
    Dialog("SoloChurch coming soon! danger=" .. dangerLev)
end

---@param girl wm.Girl
function SoloEncounter(girl)
    if wm.Percent(50) then
        LadyOfNegotiableAffection(girl)
    else
        DecentCitizen(girl)
    end
end

---@param girl wm.Girl
---@param dangerLev number
function SoloShopping(girl, dangerLev)
    Dialog("SoloShopping coming soon! danger=" .. dangerLev)
end

---@param girl wm.Girl
---@param dangerLev number
function SoloStreets(girl, dangerLev)
    if dangerLev == 1 then
        SoloEncounter(girl)
    elseif dangerLev == 2 then
        if wm.Percent(95) then
            SoloEncounter(girl)
        else
            ShakeDown(girl)
        end
    elseif dangerLev == 3 then
        if wm.Percent(50) then
            SoloEncounter(girl)
        else
            ShakeDown(girl)
        end
    end
end

---@param girl wm.Girl
function StraightAsk(girl)
    Dialog("You: \"My dear, my brothel is looking for beautiful young women like yourself. You could make a mint at my establishment. Are you interested in a job?\"")
    if girl:has_trait(wm.TRAITS.NYMPHOMANIAC, wm.TRAITS.DEMON_POSSESSED, wm.TRAITS.SUCCUBUS) then
        Dialog("Girl: \"Oh, that sounds great! I can turn my hobby into my career!\"")
        Dialog("You smile. Enthusiasm makes for happy customers.")
        wm.AcquireGirl(girl)
        return
    elseif girl:has_trait(wm.TRAITS.CUM_ADDICT) then
        Dialog("Girl: \"I will do it, but only if I get to suck lots and lots of dicks. Cum is so delicious!\"")
        Dialog("You: \"If you are good at blowjobs, you are going to get all the cum you can handle!\"")
        wm.AcquireGirl(girl)
        return
    elseif girl:has_trait(wm.TRAITS.MIND_FUCKED, wm.TRAITS.BROKEN_WILL, wm.TRAITS.SPIRIT_POSSESSED) then
        Dialog("Girl: \"Sure. Whatever you want is good with me.\"")
        Dialog("You grimace a little at her lack of enthusiasm, but she looks like all the necessary holes work and you have a bed with her name on it.")
        wm.AcquireGirl(girl)
        return
    elseif girl:has_trait(wm.TRAITS.ALCOHOLIC, wm.TRAITS.VIRAS_BLOOD_ADDICT, wm.TRAITS.SHROUD_ADDICT, wm.TRAITS.FAIRY_DUST_ADDICT) then
        Dialog("Girl: \"That sounds like a good way to keep the party going all the time. I\'m in!\"")
        Dialog("You resolve to keep an eye on her. \"Honey, you can have fun in this career, but remember: business first, and party second.\"")
        wm.AcquireGirl(girl)
        return
    elseif girl:has_trait(wm.TRAITS.AIDS, wm.TRAITS.SYPHILIS, wm.TRAITS.HERPES, wm.TRAITS.CHLAMYDIA, wm.TRAITS.WHORE) then
        Dialog("She thinks for a moment, then asks: \"Does this job have health benefits?\"")
        Dialog("You: \"I have a vested interest in keeping you in bed with clients, not in bed with a disease.\"")
        Dialog("Girl: \"That doesn\'t sound like a guarantee. But I\'ve spread my legs for less than this promise. I will take the job!\"")
        wm.AcquireGirl(girl)
        return
    elseif wm.Percent(25) then
        Dialog("Girl: \"That\'s convenient. I just lost my job, and need money desperately.\"")
        wm.AcquireGirl(girl)
        return
    end
    Dialog("Girl: \"What? Do I look like a hooker to you?\"")
    if wm.Percent(girl:combat()) then
        Dialog("She starts to slap you, and you lean out of the way... straight into the real punch, which knocks you on your ass.")
        Dialog("You wince and climb back to your feet. Not all surprises are fun.")
    else
        Dialog("She starts to slap you, but you anticipated that reaction and dodge easily.")
    end
    Dialog("You shrug and continue on your way. You know it takes a lot of noes to get to one yes.")
end

---@param girl wm.Girl
---@param dangerLev number
function SoloStripClub(girl, dangerLev)
    Dialog("SoloStripClub coming soon! danger=" .. dangerLev)
end

---@param girl wm.girl
---@param dangerLev number
function StreetAmbush(girl, dangerLev)
    Dialog("You decide to go to your normal club, where everyone knows your name.")
    Dialog("Despite the deserted streets, there is an electric feeling of danger in the air.")
    Dialog("Suddenly, one of your goons groans and drops to the ground. A crossbow quarrel is sticking out of his back.")
    Dialog("A group of warriors charges out of a cross street, heading straight for you.")
    Dialog("You realize your evening movements have become too predictable, allowing a well-planned ambush to take you by surprise.")
    Dialog("Now your options are limited. Half your guards are already down, and the rest are hard-pressed and losing.")
    local response = ChoiceBox("What are you going to do? Think quick!", 
                               "Challenge the enemy commander to single combat.", 
                               "Retreat to your nearest brothel. (Screaming like a little girl is optional.)", 
                               "Drop your weapon and surrender.")
    if response == 0 then
        Dialog("She sneers at your naivete. \"I decline your offer for single combat. I\'ve read the Evil Overlord\'s manifesto. You should try it.\"")
        Dialog("She snarls as she turns to her gang. \"Kill him!\"")
    elseif response == 1 then
        Dialog("You quickly see that a fighting retreat to your brothel is your only chance.")
        Dialog("\"Retreat to the brothel, men! MOVE!\"")
        if wm.Percent(1) then
            Dialog("Your quick assessment almost works. Almost.")
            Dialog("Your guards do their best, but the odds are just too long. One by one they fall, and you find yourself cornered in an alleyway.")
            Dialog("You turn, and snarl at the leader of the enemy. \"At least tell me why I am going to die today.\"")
            Dialog("In response, she lifts her crossbow and skillfully shoots you through the heart.")
            Dialog("In the scant moments it takes you to die, your assassin laughs. \"I am SO glad I read the Evil Overlord's Manifesto. Too bad you didn\'t.\"")
            wm.GameOver()
            return
        else
            Dialog("Your quick assessment saves your life. You lose a few more guards, but retreating quickly kept your casualties as low as can be expected.")
            Dialog("You resolve to vary your routine a bit. Hopefully you can avoid future ambushes.")
            return
        end
    elseif response == 2 then
        Dialog("One of your goons yells at you as he defends you. \"Dumbass! She told them to kill you, they aren\'t going to let you surrender!\"")
        Dialog("Distracted, he falls to his opponent\'s next thrust. You realize he gave his life to wake you up.")
    end
    Dialog("You turn to run. Your goons do their job, dying while giving you a chance to escape.")
    Dialog("A quarrel clangs off the ground next to you. You dodge down an alley to get out of the line of fire.")
    Dialog("\"Hey, you! In here!\" You duck into a doorway which slams shut behind you. You hear your pursuers race past the door. They missed you!")
    Dialog("You turn to your rescuer. \"Thank you.  Why did you save me?\"")
    Dialog("The man looks you over. You are not sure he likes what he sees. \"I don't really know. But I couldn't just let them kill you.\"")
    Dialog("You ask: \"What can I do to repay you for my life?\"")
    Dialog("He considers briefly. \"Take care of the families of your dead men. And leave now, before they come back, and I become another death on your behalf.\"")
    response = ChoiceBox("How will you repay this stranger?",
                         "Comply with his wishes. Make arrangements for the guard\'s families. (COST: 4000)",
                         "Give the man what you have in your wallet. (COST: 500)",
                         "Decline his request. \"Those men just did their jobs. I am not responsible for their families.\"")
    if response == 0 then
        Dialog("That\'s sound advice. You leave quickly, and spend the afternoon making arrangements for your men and their families. It's expensive, but worth it.")
        wm.TakePlayerGold(4000)
        wm.SetPlayerDisposition(10)
    elseif response == 1 then
        Dialog("\"I am not as wealthy as you think. Here is something for you, and I\'ll see the dead get good burials. But I can\'t support their families.\"")
        Dialog("The man sighs, but nods. \"I understand. Do what you can, then... but get the hell out of here!\"")
        wm.TakePlayerGold(500)
        wm.SetPlayerDisposition(-5)
    elseif response == 2 then
        Dialog("You take your leave, the relief of your escape now tainted with guilt. You hope your rescuer keeps his mouth shut about your ingratitude.")
        wm.SetPlayerDisposition(-20)
        wm.SetPlayerSuspicion(50)
    end
end

---@param girl wm.Girl
---@param dangerLev number
function StripperOrWaitress(girl, dangerLev)
    Dialog("You take a table to yourself. You watch the girl onstage removing her clothes, and also notice that your waitress is stunning.")
    local mission = ChoiceBox("What do you do?", "Tip the stripper.", "Hit on the waitress.")
    if mission == 0 then
        wm.UpdateImage(wm.IMG.STRIP)
        GirlDescription(girl)
        Dialog("You approach the stage and give the stripper a 50 gold tip. \"Stop by my table after your set, honey.\"")
        wm.TakePlayerGold(50)
        Dialog("You are pleased that the dancers at this club go completely nude, but are surprised that she comes straight to your table without dressing.")
        Dialog("She smiles as you struggle to raise your eyes from her body. \"I thought we\'d just save some time. Would you like to join me in the champagne room?\"")
        Dialog("You nod and follow her to the back. Several nude girls are sitting with, or on, their \"dates\".")
        Dialog("This is a shared room with plush, comfortable leather couches, small tables for food and drink, and very dim lighting.")
        Dialog("There is a security guard trying to stay unobtrusive, but he is clearly keeping an eye on the room.")
        Dialog("Despite the security, you notice that the girl three tables down is pretty obviously giving her \"date\" a handjob.")
        stripmission = ChoiceBox("The stripper is wiggling on your lap, and asks what you want. What do you say?", 
                                 "\"How about a bottle of champagne for our table?\" [Cost: 100]",
                                 "\"Can I order the same thing that guy three tables down is getting?\"",
                                 "You grab her ass and force her down harder into your lap. \"Just keep grinding for a little, honey.\"")
        if stripmission == 0 then
            Dialog("You see the guard intercept your waitress. He brings the bottle to the table himself.")
            wm.TakePlayerGold(100)
            BouncerShakedown(girl, dangerLev)
        elseif stripmission == 1 then
            Dialog("She looks over and giggles. \"That\'s Star. She will do anything for tips. But I am afraid I like my job, and won\'t risk it.\"")
            Dialog("You: \"What if I can offer you a better job?\"")
            Dialog("She laughs sarcastically. \"I would be a very rich woman if I had a gold piece for everytime I have heard that line.\"")
            Dialog("\"You chuckle and then groan a little, as she rolls her butt over your lap. \"Well, I really do own a club, and I am hiring.\"")
            Dialog("You tell her your name, and the name of your establishment. She slides down between your legs, and places her head in your lap.")
            Dialog("As she talks, she traces your outline with her chin, a tantalizing sensation. \"You know, I have been here a while. Might be time for a change.\"")
            Dialog("You give her a card. \"Stop by tomorrow, and I am sure I can find a place for you.\"")
            Dialog("You enjoy a nice lapdance that teases you skillfully and even more skillfully drains your wallet of 300 gold.")
            girl:strip(50)
            wm.TakePlayerGold(300)
            Dialog("You can\'t sleep for thinking about her hand and chin on your cock. Plus she might be a good earner for you.")
            Dialog("She does show up the next day, and you hire her on the spot. She rubs her body against you at the end of the negotiation.")
            wm.UpdateImage(wm.IMG.HAND)
            girl:handjob(50)
            Dialog("You feel her hand on your cock. \"I feel like I owe you a handjob from last night.\" It turns out she is every bit as good as you hoped.")
            Dialog("She licks your cum off her hand. \"Mmmmmm, I wanted to do that all night. Come by anytime you want more, boss!\"")
            wm.AcquireGirl(girl)
        elseif stripmission == 2 then
            Dialog("The stripper shrieks a little. The guard races over to your table. \"Hands off, bub!\"")
            Dialog("You look slowly at the guard. \"Do you know who I am?\"")
            Dialog("He laughs. \"Yessir, I do. You are the guy I am throwing out of the club.\"")
            Dialog("The stripper steps back, and everyone in the room is staring at you. A couple more beefy gentlemen back up the first guard.")
            Dialog("You aren't going to get any action here, after this. You decide to avoid an unproductive scene, and leave quietly.")                        
        end
    elseif mission == 1 then
        wm.UpdateImage(wm.IMG.WAIT)
        GirlDescription(girl)
        wm.TakePlayerGold(10)
        local line_1 = 0
        local line_2 = 0
        Dialog("Your waitress stops by with one of those silly little napkins.")
        line_1 = ChoiceBox("She is worth a try. What do you do?",
                            "You put a big wad of money on the table, lean back and leer at her.",
                            "You say, \"You are proof that the prettiest girls in any strip club are the waitresses.\"")            
        if line_1 == 0 then
            Dialog("She glances at your money, but avoids your eyes as she takes your drink order. She takes your order straight to the bartender.")
        elseif line_1 == 1 then
            Dialog("She smiles, but it's clear she\'s heard this one before. \"Not only the prettiest, but the most unavailable, too, sugar.\"")
            Dialog("She takes your drink order, and sashays to the bar. You enjoy the view, and notice she seems to have a little more wiggle than necessary.")
        else
            Dialog("She takes your order professionally and walks to the bar with no wasted movements.")
        end
        Dialog("When she comes back with your drink, you try again.")
        line_2 = ChoiceBox("She\'s worth a second shot. What do you do?",
                           "You ask, \"Have you ever danced here? You seem to be wasted as a waitress.\"",
                           "\"This is a nice club but mine is better. If you were a stripper for me, you would make a lot of money.\"")
        if line_2 == 0 then
            if line_1 == 1 then
                Dialog("This is not a new line, either. \"I've thought about it, but playing grab-ass in the champagne room doesn\'t seem like fun to me.")
            elseif line_1 == 0 then
                Dialog("Girl: \"I am afraid I have a strict \'No Touching By Strangers\' policy, sir.\"")
            end
        elseif line_2 == 1 then
            if wm.Percent(girl:charisma()) then
                Dialog("She smiles. \"If I wanted to be a stripper, I\'d already be on stage, sir. Do you need anything else?\"")
            else
                if wm.Percent(10) then
                    Dialog("She sighs and her gaze hardens. She says, \"Would you like anything else, sir?\"")
                else                
                    Dialog("She eyes your wad of money again. \"Well, more money is hard to refuse. What club do you own, sweetie?\"")
                    Dialog("You give her your card. \"Stop by tomorrow, I think I have a position for you!\"")
                    Dialog("You enjoy the show and attentive service all night.")
                    girl:add_trait(wm.TRAITS.SHROUD_ADDICT)
                    Dialog("She stops by in the morning at your office, and just like that, you have a new employee!")
                    wm.AcquireGirl(girl)
                    if girl:has_trait(wm.TRAITS.NYMPHOMANIAC) then
                        Dialog("She is a little mystified. \"That\'s it? When you said you had a position for me, I was hoping for a demonstration.\"")
                        wm.UpdateImage(wm.IMG.SEX)
                        Dialog("You smile and undo your belt buckle. \"The things I do for my business!\"")
                        return
                    end
                    if girl:has_trait(wm.TRAITS.CUM_ADDICT) then
                        Dialog("Girl: \"I think I need a drink to celebrate! Do you have anything on tap?\" She bats her eyes at you, making her meaning very clear.")
                        wm.UpdateImage(wm.IMG.ORAL)
                        Dialog("She pulls you out of your pants, and sucks at your \'tap\' greedily.")
                        Dialog("It doesn\'t take long for your cork to pop. She grins after an audible gulp. \"Mmmmm, warm salty drinks are the best, you know?\"")
                        return
                    end
                    return
                end
            end 
        end
        if (line_1 == 1) and (line_2 == 0) then
            Dialog("You decide to strike out swinging. \"Well then, I suppose a blowjob is out of the question?\"")
            Dialog("Surprisingly, she gets the reference and plays along. She winks at you. \"Depends. What size condom do you use? Small, medium or liar?\"")
            Dialog("You mimic a hopeless teenage boy: \"So how do blowjobs work, anyway? Do you like, what, swallow it or spit it out, you know?\"")
            Dialog("She cracks up. \"So you like stupid teens-trying-to-get-laid sexploitation movies, too?\"")
            Dialog("You: \"Yeah, I like \'Hot Resort\' a lot. It's not trying for moral points. It just wants laughs and cheesy reasons for girls to get naked.\"")
            Dialog("That breaks the ice. She laughs. \"Yeah, and some of those girls are definitely worth getting naked!\"")
            Dialog("The club isn\'t that busy, so when she is free she stops by your table and you swap bad movie references.")
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("And later, outside the club, you discover she prefers to swallow.")
            if wm.Percent(10) then
                Dialog("She thoughtfully hefts your manhood after you finish, as if weighing you.")
                Dialog("Girl: \"I\'m still horny. Looks like we might need one of those liar-sized condoms after all. That is, if you are up for it?\"")
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You are definitely \'up\' to the challenge. You both groan as you sink your still raging hardness into her sopping pussy.\"")
                Dialog("She is screaming in your ear on her second orgasm, as you explode inside her.")
                Dialog("She kisses you as she leaves. \"That was fun, sugar. Let\'s swap more movie trivia sometime!\"")
            end
            return
        else
            Dialog("She is clearly not into you at all. You shrug and enjoy the strip show before heading home.")
            return
        end
    end
end

---MeetTown(girl) is the routine called for the girl encounter.
---@param girl wm.Girl
function MeetTown(girl)

    Dialog("Guard: \"Going out today, sir?\"")
    local dangerLev = 0
    local protLev = 0

    if wm.Percent(10) then
       Dialog("Guard: " .. RandomChoice("\"The situation on the streets is rather tense, sir.\"",
                                        "\"Two of our rivals are fighting a war, sir.\"",
                                        "\"There is a lot of street violence, sir.\""))
       Dialog("Guard: \"I must insist that you take some guards along if you are going out.\"")
       protLev = ChoiceBox("Will you take this recommendation?", 
                           "I am not afraid of anything! No guards!", 
                           "Sounds like a good idea. Get a crew to accompany me.")
       dangerLev = 3
    else
       Dialog(RandomChoice("The guard sniffs the air, as if testing the city's atmosphere.",
                           "The guard lifts a finger into the wind, then cocks his head as if listening to the city's heartbeat.",
                           "The man at the brothel entrance looks off into the distance, as if he can see the dangers of the city."))
       if wm.Percent(50) then
           dangerLev = 2
           Dialog("Guard: " .. RandomChoice("\"A word of warning, sir. Please be careful.\"",
                                            "\"There is some danger in the air today, sir.\"",
                                            "\"You should keep your eyes peeled, sir.\""))
       else
           dangerLev = 1
           Dialog("Guard: " .. RandomChoice("\"It seems a nice day, sir, but you can never be too careful.\"",
                                            "\"It is good to see you getting outside, sir.\"",
                                            "\"Enjoy the day, sir.\"",
                                            "\"Good hunting, sir!\""))
       end
       protLev = ChoiceBox("What level of protection do you want?", 
                           "You wish to be alone, so you tell your guards to stay home.", 
                           "You ask for a security crew to accompany you.")
    end

    if protLev == 0 then
---        local mission = ChoiceBox("Guard: Very good, sir. What type of solo adventure suits you today?",
---                                  "Check out a strip club.",
---                                  "Scout the streets for new tail.",
---                                  "Do a little shopping.",
---                                  "Go to church services.")
        local mission = 1
        if mission == 0 then
            SoloStripClub(girl, dangerLev)
        elseif mission == 1 then
            SoloStreets(girl, dangerLev)
        elseif mission == 2 then
            SoloShopping(girl, dangerLev)
        elseif mission == 3 then
            SoloChurch(girl, dangerLev)
        end
    elseif protLev == 1 then
        local mission = ChoiceBox("Guard: Very good, sir. Where is your group going today?",
                                  "Scour the streets.",
                                  "Check out a strip club.")
---                                  "Do a little shopping.",
---                                  "Go to church services.")
        if mission == 0 then
            GroupStreets(girl, dangerLev)
        elseif mission == 1 then
            GroupStripClub(girl, dangerLev)
        elseif mission == 2 then
            GroupShopping(girl, dangerLev)
        elseif mission == 3 then
            GroupChurch(girl, dangerLev)
        end
    end
end