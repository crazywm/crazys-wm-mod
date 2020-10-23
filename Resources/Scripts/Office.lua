function InteractOffice(girl)
    local choice = ChoiceBox("", "Instruct her to tidy up your office", "Review her performance",
            "Send her on a mission", "Personal instruction", "Inspect her", "Go Back")
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
                girl:oral(2)
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
            Dialog("She refuses to be inspected like some prize heifer.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        Dialog("Go Back")
    end
end
