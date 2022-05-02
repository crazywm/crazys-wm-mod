---@param girl wm.Girl
function Training(girl)
    local action = ChoiceBox("Select Training",
        "Send her to the Arcane Citadel for Magic Lessons. COST: 500 gold",
        "Have her do Agility training with the local street acrobats.  Cost: 250 gold",
        "Work with the stevedore's at the shipyard to improve her Strength and Stamina.  COST: 300 gold",
        "Spend the day with a veteran adventurer for combat training. COST: 500 gold",
        "Send her to the conservatory to improve her acting, rhetoric and music skills. COST: 300 gold",
        "Go Back"
    )
    if action == 5 then
        return girl:trigger("girl:interact:brothel")
    elseif girl:tiredness() > 75 then
        Dialog("She is too tired for training")
        return
    elseif girl:health() < 40 then
        Dialog("She is not healthy enough for training today.")
    elseif action == 0 then -- Magic Lesson
        Dialog("Today you will spend the day at in the care of the mages at the Citadel.  Learn well from them. ")
        if girl:obey_check(wm.ACTIONS.GENERAL) then
            Dialog("You hand her a bag of gold containing the lesson fee and send her on her way.")
            wm.UpdateImage(wm.IMG.MAGIC)
            wm.TakePlayerGold(500)
            -- TODO this is a bit weird, magic training works better if she is good at magic
            girl:experience(wm.Range(5, 10))
            if wm.Percent(girl:magic()) then
                Dialog("She arrives at the Citadel a few minutes before her appointed time wearing he best robes.")
                Dialog("The Mages were impressed with her decorum and willingness to learn.  They praise her for her diligence and aptitude.")
                Dialog("She feels she will be able to apply the lessons she learned today. She also feels more confident in her ability.")
                girl:magic(wm.Range(3, 10))
                girl:intelligence(wm.Range(1, 5))
                girl:confidence(wm.Range(2, 7))
                girl:happiness(wm.Range(0, 5))
                girl:tiredness(wm.Range(1, 10))
            else
                Dialog("Although she arrived on time, she was still wearing her brothel attire.  Her teachers were a little distracted and as a result the lesson was somewhat lacking.")
                girl:magic(wm.Range(1, 5))
                girl:intelligence(wm.Range(0, 1))
                girl:confidence(wm.Range(0, 2))
                AdjustLust(girl, wm.Range(2, 10))
                girl:tiredness(wm.Range(1, 10))
            end
        else
            Dialog("She makes some excuses and says she can't go training today.")
            return girl:trigger("girl:refuse")
        end
    elseif action == 1 then
        Dialog("\"I have made arrangements with a local acrobatics troupe.  They have agreed to show you some of their techniques.\"  You hand her a parchment with a crude map and the name of the troupe leader.")
        if girl:obey_check(wm.ACTIONS.GENERAL) then
            wm.TakePlayerGold(250)
            -- TODO this is a bit weird, agility training works better if she is good at agility
            girl:experience(wm.Range(5, 10))
            wm.UpdateImage(wm.IMG.SPORT)
            if wm.Percent(girl:agility()) then
                Dialog("She finds the plaza easily and introduced herself to the Head Troubadour. She easily picked up on the subtle important motions involved with tumbling and they acrobats were impressed by her natural flexibility.")
                Dialog("She had a fun day and learned a great deal from the troupe.")
                girl:agility(wm.Range(3, 10))
                girl:strength(wm.Range(0, 4))
                girl:charisma(wm.Range(0, 4))
                girl:happiness(wm.Range(0, 5))
                girl:tiredness(wm.Range(1, 10))
            else
                Dialog("She got a little turned around but eventually found the correct street.")
                Dialog("She forgot to wear her underwear today, which made many of the lifts and holds distracting to both her and her fellow entertainers. More than one audience member got more of a show then they had expected.")
                Dialog("All in all, her unique brand of showmanship earned some better tips, but she was too distracted to learn everything.")
                girl:agility(wm.Range(1, 5))
                girl:strength(wm.Range(0, 1))
                girl:charisma(wm.Range(0, 1))
                AdjustLust(girl, wm.Range(5, 25))
                girl:tiredness(wm.Range(1, 10))
                wm.AddPlayerGold(wm.Range(20, 90))
            end
        else
            Dialog("She pretends to have an injured ankle and mumbles some apologies as she hobbles out of the room.")
            return girl:trigger("girl:refuse")
        end
    elseif action == 2 then
        Dialog("You have been lacking in exercise lately.  I've bribed the foreman of the shipyard workers to let you work with them for the day.")
        if girl:obey_check(wm.ACTIONS.GENERAL) then
            wm.TakePlayerGold(300)
            wm.UpdateImage(wm.IMG.SPORT)
            girl:experience(wm.Range(5, 10))
            if wm.Percent(girl:constitution()) then
                Dialog("She shows up early at the docks and she remembered to bring a pair of thick leather gloves with her.")
                Dialog("It was hard work, and just as they thought they had unloaded the last ship, another was spotted on the horizon.  The Foreman could be heard shouting \"Looks like we got some overtime tonight, Lads!\"")
                Dialog("Although she was tired and cranky she helped the men unload the ship long into the night.  The Foreman thanked her for her help and refunded most of the bribe.")
                wm.AddPlayerGold(wm.Range(100, 200))
                girl:strength(wm.Range(4, 10))
                girl:constitution(wm.Range(4, 10))
                girl:obedience(wm.Range(1, 5))
                girl:refinement(-wm.Range(0, 2))
                girl:tiredness(wm.Range(2, 20))
            else
                Dialog("She had meant to get to the docks on time, but somehow she managed to oversleep.  The Foreman was not pleased when she showed up around noon.")
                Dialog("She worked hard for the last half of the day, but she may have gotten more out of it if she had been on time.")
                girl:strength(wm.Range(1, 5))
                girl:constitution(wm.Range(1, 5))
                AdjustLust(girl, wm.Range(2, 10))
                girl:refinement(-wm.Range(1, 3))
                girl:tiredness(wm.Range(1, 15))
            end
        else
            Dialog("She is visibly disgusted by the idea of working around sweaty men all day.  She makes some weak excuses and walks away.")
            return girl:trigger("girl:refuse")
        end
    elseif action == 3 then
        Dialog("\"Grab your armor and gear.  I'm sending you to study with the veteran warrior, Titus Pullo.\"")
        if girl:obey_check(wm.ACTIONS.GENERAL) then
            wm.TakePlayerGold(500)
            girl:experience(wm.Range(5, 10))
            wm.UpdateImage(wm.IMG.COMBAT)
            if wm.Percent(girl:combat()) then
                Dialog("She arrives early to the combat ring and becomes lost in thought as she waits...")
                Dialog("She is startled out of her thoughts by the feeling of a hand between her thighs. She swings wildly at the brash pervert, but her attack is deflected.  \"Oh, You're some nice cunny aren't you, girl!\"  yells the man.  she attacks again and again, but each attack is deflected and followed by some other perverted comment.")
                Dialog("The battle continues for hours and the unnamed pervert continues to block, parry, and grope her. Throughout the day she becomes better at preventing his hands from reaching her.")
                girl:combat(wm.Range(3, 8))
                girl:strength(wm.Range(1, 5))
                girl:constitution(wm.Range(1, 7))
                AdjustLust(girl, wm.Range(5, 20))
                girl:tiredness(wm.Range(2, 20))
            else
                Dialog("She arrives late to the combat ring and not yet wearing her armor.  Her instructor continues to drink as he waits for her to be ready.")
                Dialog("When she is finally prepared, the lesson begins.  By this time however, the warrior is slightly inebriated.  He is able to handily block and parry and swing she attempts, but he offers little in the form of verbal instruction.")
                Dialog("After an exhausting day sparring with the superior, although, drunken swordsmen;  She returns to the brothel with some minor skill improvements.")
                girl:combat(wm.Range(1, 5))
                girl:strength(wm.Range(0, 2))
                girl:constitution(wm.Range(0, 2))
                AdjustLust(girl, wm.Range(2, 10))
                girl:tiredness(wm.Range(2, 20))
            end
        else
            Dialog("She mumbles something about her armor still being in the dirty laundry pile and meanders away.")
            return girl:trigger("girl:refuse")
        end
    elseif action == 4 then
        if girl:performance() < 33 then
            Dialog("\"Go to the conservatory and take some lessons in the performing arts! You really need them!\"")
        elseif girl:performance() < 66 then
            Dialog("\"Go to the conservatory and take some lessons in the performing arts!\"")
        else
            Dialog("\"I want you to go to the conservatory and perfect your skills in the performing arts!\"")
        end
        wm.UpdateImage(wm.IMG.SING)
        if girl:obey_check(wm.ACTIONS.ACTRESS) then
            wm.TakePlayerGold(300)
            girl:experience(wm.Range(5, 10))
            if girl:performance() < 33 then
                Dialog(girl:name() .. " takes part in some basic acting, singing, and dancing lessons. Pretty easy stuff, " ..
                        "but she still needs to learn this.")
                girl:performance(wm.Range(2, 5))
                girl:tiredness(wm.Range(5, 15))
                if girl:charisma() < 25 then
                    girl:charisma(wm.Range(0, 3))
                end
            elseif girl:performance() < 66 then
                Dialog(girl:name() .. " knows some basic dances, can sing some simple songs, and knows how to read lines from a script. " ..
                        " Now she has to learn how to convey emotion with her body and voice, and gets taught some tricks for memorizing her lines."
                )
                girl:performance(wm.Range(1, 3))
                girl:tiredness(wm.Range(5, 15))
                if girl:charisma() < 33 then
                    girl:charisma(wm.Range(0, 3))
                end
            else
                Dialog(girl:name() .. " already is a rather skilled thespian, but there are still some things she can learn from the masters. "..
                        "She works on her comedic timing, and practice nuanced facial expressions.")
                girl:performance(wm.Range(0, 2))
                girl:tiredness(wm.Range(5, 15))
                if girl:charisma() < 50 then
                    girl:charisma(wm.Range(0, 3))
                end
                if girl:refinement() < 33 then
                    girl:refinement(wm.Range(0, 2))
                end
            end
        end
    end
end
