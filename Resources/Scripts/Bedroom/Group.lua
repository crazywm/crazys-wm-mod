---@param girl wm.Girl
function FFMSex(girl)
    Dialog("You fetch one of your girls before going to ${name}'s room'")
    local other_name = RandomGirlName()
    wm.UpdateImage(wm.IMG.BED)
    Dialog("You find her relaxing on her bed looking through some of her lingerie. She looks up as you speak. " ..
            "\"I wonder if you could help me, my dear. ".. other_name .. " and I would like you to join us in some fun\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        if girl:has_trait(wm.TRAITS.LESBIAN) and girl:pclove() < 50 then
            if wm.Percent(50) or girl:is_slave() then
                Dialog("She looks at you skeptically, but starts to smile as soon as " .. other_name .. " enters the room. \"Sure, should be fun!\"")
            else
                Dialog("\"I'd prefer to spend the evening with ".. other_name .. "\", she says.")
                local choice = ChoiceBox("", "Tough luck", "If you let me watch, that's fine by me")
                if choice == 0 then
                    Dialog("\"Surely, you aren't turning down your boss after he went through, " ..
                            "especially after he went through all the trouble of bringing an extra companion\", you chide her. \n" ..
                            "She sights: \"If you insist.\"")
                    girl:pclove(-1)
                    girl:happiness(-1)
                else
                    HandleLesbianSex(girl, other_name)
                    girl:happiness(1)
                    return
                end
            end
        elseif girl:has_trait(wm.TRAITS.STRAIGHT) and girl:pclove() > 75 and wm.Percent(50) then
            Dialog("\"I'd much rather have you all to myself\"")
            local choice = ChoiceBox("", "Sorry, but I already promised " .. other_name, "OK, it'll be just us two")
            if choice == 0 then
                Dialog("She sights: \"If you insist.\"")
                girl:pclove(-1)
                girl:happiness(-1)
            else
                HandleNormalSex(girl)
                girl:happiness(1)
                return
            end
        else
            Dialog("She nods in agreement. \"Sure, come on in!\"")
        end

        girl:experience(6)
        girl:tiredness(4)

        if girl:skill_check(wm.SKILLS.GROUP, 50) and girl:skill_check(wm.SKILLS.LESBIAN, 50) then
            wm.UpdateImage(wm.IMG.FFM)
            Dialog(other_name .. " and ${name} start undressing and caressing each other, providing you with quite the show.\n" ..
                    "Once they are both naked, they beckon you to join them on the bed. " .. other_name .. " starts massaging your shaft while ${firstname}" ..
                    " is making out with you. This is what heaven must be like!")
            if girl:has_trait(wm.TRAITS.LESBIAN) and wm.Percent(50) then
                girl:lesbian(1)
                Dialog("You notice that ${firstname} appears to be more interested in " .. other_name .. " than in you. " ..
                        "You decide to let her have her fun, and let the two girls have at it while you stroke your cock.")
                wm.UpdateImage(wm.IMG.LES69)
                Dialog("And what a show you get! They are both licking each others cunt, moaning and sighing with delight until they explode with pleasure")
                Dialog("Once they are done ${firstname} looks at you sheepishly: \"Sorry, I got carried away and forgot about you. Here, let me take care of that\"")
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("With that, she takes your dick in her hands. After the spectacle you've just seen, it doesn't take long for you to erupt.")
                Dialog("Though this was not quite what you had in mind, you cannot really complain. Your only regret is that you did not think to bring a " ..
                        "kinematograph. If you could advertise with what you've witnessed tonight, you are sure that your establishment would be overflowing.")
            else
                girl:oralsex(1)
                Dialog("After a time, they switch things up. ${firstname}"  ..
                        " starts licking your cock, and you decide to pay it forward and ask " .. other_name .. " to sit on your face.\n" ..
                        "Your bury your tongue in her cunt at the same time as ${firstname} takes you all the way into her mouth. \n" ..
                        "As you feel yourself approaching climax, you intensify your efforts, and are rewarded with " .. other_name ..
                        " squirting hot juices all over you just as you spurt your load in ${firstname}'s mouth. With your cum still " ..
                        " on her tongue, the two girls share a passionate kiss.")
                wm.UpdateImage(wm.IMG.LICK)
                Dialog("Not wanting to show favouritism, you ask ${firstname} to lie down and spread her legs for you. " ..
                        " Again your nimble tongue starts doing its work.")
                wm.UpdateImage(wm.IMG.FFM)
                Dialog("You spent the next hours with ${firstname} and " .. other_name ..
                        " engaged in various positions. Why can't every evening be like this one?")
            end
            girl:happiness(5)
            girl:libido(2)
            girl:group(1)
            girl:lesbian(1)
        else
            if girl:has_trait(wm.TRAITS.STRAIGHT) and wm.Percent(50) then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("${firstname} doesn't really know what to do with another woman, and is not interacting with " ..
                        other_name .. " at all. In the end, you end up receiving a blowjob while " .. other_name .. " just watches." ..
                        "Not really what you had hoped for.")
                girl:oralsex(1)
            elseif girl:has_trait(wm.TRAITS.LESBIAN) and wm.Percent(50) then
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog("${firstname} seems to be only interested in " .. other_name .. ", giving you basically zero attention. " ..
                        "In the end, you decide to just let the two girls enjoy each other and lean back to indulge in the spectacle.")
                if girl:skill_check(wm.SKILLS.LESBIAN, 50) then
                    wm.UpdateImage(wm.IMG.EATOUT)
                    Dialog("At least ${firstname} appears to be quite skilled at pleasuring " .. other_name .. ", " ..
                            "judging by the moans and cries she elicits.")
                    Dialog("With this scenery, it doesn't take long for your cock to become rock-hard. Seeing that the two girls will be busy for a while, " ..
                            "you sigh and take matters into your own hand. Not exactly what you had hoped for, but not a total waste of a night either.")
                else
                    Dialog("However, even in that regard you end up thoroughly disappointed: ${firstname} has no idea how to pleasure another girl. " ..
                            "Frustrated, you leave the room in search for better entertainment.")
                end
            else
                wm.UpdateImage(wm.IMG.FFM)
                Dialog("${firstname} seemed unsure what to do, and mostly just watches as " .. other_name .. " takes your dick in her mouth." ..
                        "When you suggested that she help, she just managed to butt her head with " .. other_name .. "'s, and ended up not only " ..
                        "not participating herself, but also ruining an up-to-that-point perfectly fine blowjob. What a disastrous night!")
            end
            girl:happiness(-2)
            girl:libido(-3)
            girl:tiredness(4)
            girl:group(1)
            girl:lesbian(1)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        if girl:has_trait(wm.TRAITS.STRAIGHT) and girl:dignity() < 33 then
            Dialog("She refuses. \"I'm not having sex with another girl!\"")
        else
            Dialog("She refuses to be have sex with you and the other girl.")
        end
        wm.UpdateImage(wm.IMG.REFUSE)
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function MMFSex(girl)
    Dialog("You fetch one of your goons before going to ${name}'s room'")
    local other_name = RandomChoice("John", "James", "Tommy", "Chad", "Steve", "Alex")
    Dialog("You find her relaxing on her bed looking through some of her lingerie. She looks up as you speak." ..
            "\"" .. other_name .. " and I are looking for some entertainment for the evening. A threesome would be nice, don't you think my dear...\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        if girl:has_trait(wm.TRAITS.LESBIAN) and wm.Percent(33) then
            Dialog("Her face looks disgusted for a second, but she catches herself and beckons \"Come in.\"")
            girl:happiness(-3)
        elseif girl:pclove() > 80 and wm.Percent(33) then
            Dialog("\"I'd much rather have you all to myself\", she says.")
            local choice = ChoiceBox("", "Sorry, but I already promised " .. other_name, "OK, it'll be just us two")
            if choice == 0 then
                Dialog("She sights: \"If you insist.\"")
                girl:pclove(-1)
                girl:happiness(-1)
            else
                HandleNormalSex(girl)
                girl:happiness(1)
                return
            end
        else
            Dialog("She nods in agreement, but asks for a few minutes to get ready.  As the door closes behind you, many rumbling and rustling sounds can be heard.  A minute or so later she declares that she is ready.")
            Dialog("When you enter the room you find her wearing only her favorite black lingerie. \"Welcome, gentlemen!\"")
        end
        girl:experience(6)
        girl:tiredness(4)

        if girl:skill_check(wm.SKILLS.GROUP, 50) then
            wm.UpdateImage(wm.IMG.MMF)
            if girl:has_trait(wm.TRAITS.LESBIAN) then
                Dialog("Even though her appetites go in another direction, ${firstname} is undeniably skilled at " ..
                        "pleasing a man -- or two men, as it were. If you hadn't know, you would never have guessed that she is Lesbian.")
            else
                local option = wm.Range(0, 2)
                if option == 0 then
                    Dialog("A cock in each hand, ${firstname} expertly demonstrates how to pleasure two men at the same time.")
                    girl:handjob(1)
                elseif option == 1 then
                    Dialog("She expertly sucks " .. other_name .. "'s cock while you pound her from behind. This is exactly the kind of relaxation you needed tonight.")
                    girl:normalsex(1)
                else
                    Dialog("One cock in her hand, the other in her mouth, ${firstname} expertly demonstrates how to pleasure two men at the same time.")
                    girl:oralsex(1)
                end
            end

            girl:happiness(4)
            girl:libido(2)
            girl:group(1)
        else
            if girl:has_trait(wm.TRAITS.LESBIAN) then
                wm.UpdateImage(wm.IMG.MMF)
                Dialog("${firstname} doesn't know what to do with a single cock, let alone two.")
            else
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("Pleasing two men at the same time proved to much for ${firstname}. " ..
                        "While " .. other_name .. " is receiving a handjob, your are left to watch. Your clear your throat " ..
                        "loudly to get her attention, and she immediately switches to stroking your cock, now leaving " ..
                        other_name .. " with his throbbing erection to watch dumbfounded.")
                Dialog("In the end, " .. other_name .. " has to settle for waiting his turn. Not really what ")
            end
            girl:happiness(-2)
            girl:libido(-3)
            girl:tiredness(4)
            girl:group(1)
            girl:handjob(1)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to be have sex with you and the other guy.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function OrgySex(girl)
    Dialog("You ask the group of men to wait in the hall as you enter her room.")
    wm.UpdateImage(wm.IMG.BED)
    Dialog("You find her relaxing on her bed looking through some of her lingerie.  She looks up as you speak.\"I wonder if you could help me, my dear.  I've got a group of gentlemen outside and I was wondering if you could help me entertain them?\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        girl:experience(6)
        girl:tiredness(5)
        Dialog("She nods in agreement, but asks for a few minutes to get ready.  As the door closes behind you, many rumbling and rustling sounds can be heard.  A minute or so later she declares that she is ready.")
        if girl:has_trait(wm.TRAITS.NYMPHOMANIAC) then
            wm.UpdateImage(wm.IMG.NUDE)
            Dialog("You lead the men inside and you all stand at attention for the amazing sight before you.  She stands in the center of the room surrounded by pillows and cushions.  There isn't a stitch of clothing on her body, which shines from the coating of lubricant she has applied.  She waits for the door to close before she strikes a sexy pose and exclaims \"Lets see who can catch the greased courtesan first!\"")
            wm.UpdateImage(wm.IMG.ORGY)
            Dialog("She slips and slides among the group, escaping holds to be caught by others. Before long everyone is panting and slippery. " ..
                    "She arranges you all laying on the floor and slides her body along the group taking turns and stopping at each man to ride his throbbing erection. " ..
                    "She expertly times each individual session and doesn't leave anyone wanting. The Sun breaks through the window and you awake on her floor with her laying on top of the group.")
            AdjustLust(girl, 5)
            girl:happiness(2)
            girl:tiredness(6)
            girl:group(1)
        else
            if girl:skill_check(wm.SKILLS.GROUP, 75) then
                wm.UpdateImage(wm.IMG.ECCHI)
                Dialog("The group enters the room and forms a circle around the kneeling and eager girl in her favorite black lingerie.")
                wm.UpdateImage(wm.IMG.ORGY)
                Dialog("She reaches up and frees your cock from it's cloth prison.  As she begins to suck and lick the tip she reaches to her sides and liberates the other mens' dicks as well.")
                Dialog("She continues to suck your cock and stroke the men next to you with her hands as another man climbs beneath her and inserts himself into her vagina.  Another man kneels down behind her and penetrates her ass.  This continues through the night with men taking turns with all her holes.  ")
                Dialog("The men compliment you on an excellent evening and blow kisses to the completely exhausted and sleeping woman on the bed.")
                girl:happiness(5)
                girl:libido(2)
                girl:group(1)
            else
                wm.UpdateImage(wm.IMG.FORMAL)
                Dialog("The room has been arranged with a table in the center circled by chairs.  She bows \"It will be my pleasure to serve you tonight, gentlemen.\"  You sigh to yourself as you realize she has gotten the wrong idea.  She realizes her error as the group removes erect penises from their pants and move toward her.")
                wm.UpdateImage(wm.IMG.ORGY)
                Dialog("To her credit she recovers from the shock quickly and lays back with her feet on the table.  The men take turns passing and sliding her around the table.  She allows the group to enter her every orifice but does little to enhance the experience.")
                Dialog("You see the men out; lost in your thoughts of the disastrous performance.  One older gentlemen attempts to cheer you up by saying \"It was an enjoyable enough night, Sir.  Any gangbang is a good gangbang\"")
                girl:happiness(-2)
                girl:libido(-3)
                girl:tiredness(5)
                girl:group(2)
            end
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to be gangbanged like some dirty beggar on the streets.")
        return girl:trigger("girl:refuse")
    end
end
