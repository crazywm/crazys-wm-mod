function InteractOffice(girl)
    local choice = ChoiceBox("", "Instruct her to tidy up your office", "Review her performance",
            "Send her on a mission", "Personal instruction", "Inspect her", "Manage Tattoos", "Manage Piercings", "Go Back")
    if choice == 0 then
        if girl:obey_check(wm.ACTIONS.WORKCLEANING) then
            wm.UpdateImage(wm.IMG.MAID)
            Dialog("She puts on her maid's attire and sets about tidying up your office.  You always enjoy being around a women in a maid's outfit .")
            girl:clean_building();
        else
            Dialog("She refuses to clean your office.")
            return girl:trigger("girl:refuse")
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
        return girl:trigger("girl:mission")
    elseif choice == 3 then
        Dialog("\"After observing your work these past couple days, I've noticed some things that could use improvement.  I think you will benefit from my personal instruction in these areas\"  *You wink at her slyly*")
        if girl:obey_check(wm.ACTIONS.SEX) then
            Dialog("\"Please, join me on my office couch there and we will begin....\"")
            -- TODO: Prefer easily trainable skills (not maxed-out, no
            -- adverse traits).
            local action = wm.Range(1, 5)
            if action == 1 then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("You lay back on your leather couch and pull your erect member from your trousers.   \"Now my dear, I'm going to teach you the right way to suck a cock...\"")
                girl:oralsex(2)
            elseif action == 2 then
                PlayerFucksGirl(girl)
                Dialog("You gently help her remove her clothing and lay her down on the sofa.  As you position yourself on top of her you begin explaining the finer points of vaginal sex.")
            elseif action == 3 then
                PlayerFucksGirl_BDSM(girl)
                Dialog("You chat with her a few moments on the merits of pain and pleasure sex.  She listens intently and doesn't notice as you bind her with the hidden couch restraints.  She notices as you cinch the last strap and looks up at you inquiringly.  \"No lesson is complete without a demonstration\"  You explain. \"Now be mindful of what I told you, as I demonstrate....\"")
            else
                wm.UpdateImage(wm.IMG.ANAL)
                Dialog("You instruct her to kneel on the couch, facing away from you.  You admire her ass for a moment as you ask her to pull her dress up.  Reaching out, you slowly pull down her panties.  As you stimulate her clitoris and rub her juices on her anus, You explain some techniques to help her relax her muscles during anal sex.  You continue your lesson with a practical demonstration and you easily slide your penis into her ass...")
                girl:anal(2)
            end
        else
            Dialog("She refuses your offer of instruction.")
            return girl:trigger("girl:refuse")
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
            girl:strip(2)
            wm.UpdateImage(wm.IMG.STRIP)
            if girl:has_trait("Exhibitionist", "Nymphomaniac") and girl:pclove() > 0 then
                Dialog("She eagerly removes her clothing and stands before you.")
                girl:happiness(1)
                AdjustLust(girl, 1)
            elseif girl:strip() < 40 or girl:has_trait("Chaste") then
                Dialog("She removes her clothing and stands nervously before you.")
            else
                Dialog("She removes her clothing and stands before you.")
            end

            if girl:has_trait("Futanari") then
                Dialog("Oh! Hmm...I didn't realize you were a dick girl...")
            end

            if girl:beauty() >= 50 then
                Dialog("I see you put a lot of effort into you appearance....Yes, quite beautiful.")
            else
                Dialog("You're not much to look at, are you.")
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
            Dialog("She refuses to be inspected like some prize heifer.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        if ManageTattoos(girl) then
            return
        else
            return InteractOffice(girl)
        end
    elseif choice == 6 then
        if ManagePiercings(girl) then
            return
        else
            return InteractOffice(girl)
        end
    elseif choice == 7 then
        Dialog("Go Back")
    end
end

---@param girl wm.Girl
function ManageTattoos(girl)
    if girl:tiredness() > 75 then
        Dialog("She is too tired for a tattoo session.")
        return false
    elseif girl:health() < 40 then
        Dialog("She is not healthy enough for a tattoo session.")
        return false
    end

    if girl:has_trait("Heavily Tattooed") then
        Dialog(girl:name() .. "'s entire body is covered in tattoos.")
    elseif girl:has_trait("Tattooed") then
        Dialog(girl:name() .. " has a large tattoo")
    elseif girl:has_trait("Small Tattoos") then
        Dialog(girl:name() .. " has a few small tattoos.")
    else
        Dialog(girl:name() .. "doesn't have any tattoos.")
    end

    local choice = ChoiceBox("What do you want her to get?",
            "Get a cheap back-alley tattoo (100)",
            "Get a small tattoo professionally done (200)",
            "Get a large professional tattoo (400)",
            "Send her to multiple tattooing sessions (750)",
            "Tattoo removal (300)",
            "Go Back"
    )

    if choice < 4 then
        if not girl:obey_check() then
            Dialog(girl:name() .. " refuses to get tattooed.")
            girl:trigger("girl:refuse")
            return true
        end
    end

    if choice == 0 then
        if not wm.TakePlayerGold(100) then
            Dialog("You don't have enough money")
            return false
        end

        Dialog("Sometimes you just can't afford to have it professionally done. You send " .. girl:name() ..
                " to the cheapest a back-alley tattoo artist you could find.")
        girl:tiredness(10)
        girl:health(-20)

        if girl:has_trait("Heavily Tattooed", "Tattooed") then
            Dialog(girl:name() .. "'s body is already covered in tattoos.")
            return false
        else
            if wm.Percent(20)and girl:has_trait("Small Tattoos") then
                girl:remove_trait("Small Tattoos")
                girl:add_trait("Tattooed")
            else
                girl:add_trait( "Small Tattoos")
            end
        end

        if wm.Percent(33) then
            if wm.Percent(50) then
                if girl:health() > 20 then
                    girl:health(-10)
                end
                Dialog("The cheap ink became infected, and the surrounding skin is now Scarred.")
                girl:add_trait("Small Scars")
            else
                Dialog("Maybe you should have invested a bit more to find a tattoo artist who uses clean needles. It appears that " .. girl:name() .. " has contracted AIDS.")
                girl:add_trait("AIDS")
            end
        else
            Dialog("Everything went well, and after a short recovery period ".. girl:name() .. " can show off her new Tattoo.")
        end
    elseif choice == 1 then
        if not wm.TakePlayerGold(200) then
            Dialog("You don't have enough money")
            return false
        end
        if girl:has_trait("Heavily Tattooed") then
            Dialog(girl:name() .. "'s entire body is already covered in tattoos.")
            return false
        end

        Dialog("You send " .. girl:name() .. " to a professional tattoo artist to get a small tattoo.")
        girl:tiredness(5)
        girl:health(-5)

        if girl:has_trait("Small Tattoos") then
            if wm.Percent(33) then
                Dialog("By now she has accumulated enough tattoos to be considered Tattooed.")
                girl:remove_trait("Small Tattoos")
                girl:add_trait("Tattooed")
            end
        elseif girl:has_trait("Tattooed") then
            if wm.Percent(10) then
                Dialog("By now she has accumulated enough tattoos to be considered Heavily Tattooed.")
                girl:remove_trait("Tattooed")
                girl:add_trait("Heavily Tattooed")
            else
                Dialog("She now has one more small tattoo on her already Tattooed body.")
            end
        else
            girl:add_trait( "Small Tattoos")
        end
    elseif choice == 2 then
        if not wm.TakePlayerGold(400) then
            Dialog("You don't have enough money")
            return false
        end
        if girl:has_trait("Heavily Tattooed") then
            Dialog(girl:name() .. "'s entire body is already covered in tattoos.")
            return false
        end

        Dialog("You send " .. girl:name() .. " to a professional tattoo artist to get a large tattoo.")
        girl:tiredness(10)
        girl:health(-10)

        if girl:has_trait("Tattooed") then
            if wm.Percent(33) then
                Dialog("With this new tattoo, her entire body appears to be covered in tattoos")
                girl:remove_trait("Tattooed")
                girl:add_trait("Heavily Tattooed")
            end
        else
            girl:remove_trait("Small Tattoos")
            girl:add_trait( "Tattooed")
        end
    elseif choice == 3 then
        if not wm.TakePlayerGold(750) then
            Dialog("You don't have enough money")
            return false
        end

        if girl:has_trait("Heavily Tattooed") then
            Dialog(girl:name() .. "'s entire body is already covered in tattoos.")
            return false
        end

        Dialog("You send " .. girl:name() .. " to a professional tattoo artist to get her entire body tattooed.")

        if girl:has_trait("Tattooed", "Small Tattoos", "Tough") then
            girl:tiredness(10)
            girl:health(-10)
            Dialog("Getting her body covered in tattoos is a long and arduous process, but " .. girl:name() .. " handled it like a champ.")
        else
            Dialog("She has no previous experience of getting tattoos. These long extended sessions take a heavy toll on her body.")
            girl:tiredness(20)
            girl:health(-20)
            girl:happiness(-20)
        end
        girl:remove_trait("Small Tattoos")
        girl:remove_trait("Tattooed")
        girl:add_trait("Heavily Tattooed")
    elseif choice == 4 then
        if not wm.TakePlayerGold(300) then
            Dialog("You don't have enough money")
            return false
        end

        if not girl:has_trait("Heavily Tattooed", "Tattooed", "Small Tattoos") then
            Dialog(girl:name() .. " has no tattoos that could be removed.")
            return false
        end

        girl:tiredness(10)
        girl:health(-10)
        local where = RandomChoice("back", "calf", "arm", "belly", "chest", "ass")
        if girl:has_trait("Heavily Tattooed") then
            if wm.Percent(50) then
                Dialog(girl:name() .. " gets a large tattoo removed from her " .. where .. ". Given her amount of ink, this hardly makes a difference though.")
            else
                Dialog(girl:name() .. " gets a large tattoo removed from her " .. where .. ".")
                girl:remove_trait("Heavily Tattooed")
                girl:add_trait("Tattooed")
            end
        elseif girl:has_trait("Tattooed") then
            if wm.Percent(50) then
                Dialog(girl:name() .. " gets a tattoo removed from her " .. where .. ". She still has quite a few remaining.")
            else
                Dialog(girl:name() .. " gets a tattoo removed from her " .. where .. ".")
                girl:remove_trait("Tattooed")
                girl:add_trait("Small Tattoos")
            end
        elseif girl:has_trait("Small Tattoos") then
            if wm.Percent(50) then
                Dialog(girl:name() .. " gets a small tattoo removed from her " .. where .. ". She still has some tattoos remaining.")
            else
                Dialog(girl:name() .. " gets a small tattoo removed from her " .. where .. ".")
                girl:remove_trait("Small Tattoos")
            end
        end
    elseif choice == 5 then
        return false
    end

    return true
end

---@param girl wm.Girl
function ManagePiercings(girl)
    if girl:tiredness() > 75 then
        Dialog("She is too tired to get pierced.")
        return false
    elseif girl:health() < 40 then
        Dialog("She is not healthy enough to get pierced.")
        return false
    end

    local piercings = ""
    local any = false
    if girl:has_trait("Pierced Tongue") then
        piercings = "She has a pierced tongue.\n"
        any = true
    end
    if girl:has_trait("Pierced Nose") then
        piercings = piercings .. "She has her nose pierced.\n"
        any = true
    end
    if girl:has_trait("Pierced Navel") then
        piercings = piercings .. "She has a piercing in her navel.\n"
        any = true
    end
    if girl:has_trait("Pierced Nipples") then
        piercings = piercings .. "Her nipples are pierced.\n"
        any = true
    end
    if girl:has_trait("Pierced Clit") then
        piercings = piercings .. "She has a clit piercing.\n"
        any = true
    end
    if not any then
        piercings = "She has no piercings."
    end
    Dialog(piercings)


    local choice = ChoiceBox("Where should she get pierced?",
            "Tongue (100)",
            "Nose (100)",
            "Navel (100)",
            "Nipples (200)",
            "Clit (200)"
    )

    if choice == 0 then
        return HandleGetPiercing(girl, 100, "Tongue")
    elseif choice == 1 then
        return HandleGetPiercing(girl, 100, "Nose")
    elseif choice == 2 then
        return HandleGetPiercing(girl, 100, "Navel")
    elseif choice == 3 then
        return HandleGetPiercing(girl, 200, "Nipples")
    elseif choice == 4 then
        return HandleGetPiercing(girl, 200, "Clit")
    elseif choice == 5 then
        return false
    end
end

function HandleGetPiercing(girl, cost, where)
    if not wm.TakePlayerGold(cost) then
        Dialog("You don't have enough money")
        return false
    end
    if girl:has_trait("Pierced " .. where) then
        Dialog("Her " .. where .. " is already pierced.")
        return false
    end

    if not girl:obey_check() then
        Dialog(girl:name() .. " refuses to get her " .. where .. " pierced.")
        girl:trigger("girl:refuse")
        return true
    end

    Dialog("You send " .. girl:name() .. " to a piercing parlour and let her get her " .. where .. " pierced.")
    girl:health(-10)
    girl:tiredness(10)
    girl:add_trait("Pierced " .. where)
    return true
end