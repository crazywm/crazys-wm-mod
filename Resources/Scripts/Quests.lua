---@param girl wm.Girl
function TavernQuest(girl)
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

        if girl:has_trait("Nymphomaniac") then
            Dialog("Broken wand Nympho attempt to fix his limp \"wand\"")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("tavern wizard. nympho critical success. wand is penis.  she revitalises the wizard and lets him try her ass.")
                -- TODO anal sex
            elseif result > 60 then
                Dialog("tavern wizard. nympho success.  Wand is magical sex toy. she helps test and improve it for the wizard.")
            elseif result > 10 then
                Dialog("taven wizard. nympho. failure.  Wand is magical sex toy. she breaks it completely.")
            else
                Dialog("tavern wizard. nympho critical failure.  Wand is penis.  she realizes that the wand is his penis. she manages to excite him but rides him too hard and severely injures the wizards manhood.")
                -- TODO normal sex
            end
        elseif girl:has_trait("Adventurer") then
            Dialog("Broken wand Adventurer attempt to find a new wand")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("tavern wizard. adventurer. critical success. get bonus item.")
                -- TODO give player random special item
            elseif result > 60 then
                Dialog("tavern wizard. adventurer. success.")
                -- TODO strip
            elseif result > 10 then
                Dialog("tavern wizard. adventurer failure.")
            else
                Dialog("tavern wizard. adventurer critical failure. wizard set a trap. fucks girls ass.")
                -- TODO anal sex
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
                -- TODO take gold
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
    elseif what == 4 then
        Dialog("Tavern Group of Adventurers needs another")
        if girl:has_trait("Construct") then
            Dialog("Group of Adventurers. Construct attempt.  group needs a non organic to cross a magical barrier.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("tavern group. construct critical success.  bonus item and stamina.")
                girl:constitution(10)
                -- TODO special item
            elseif result > 60 then
                Dialog("tavern group. construct. success. get EXP.")
                girl:experience(35)
            elseif result > 10 then
                Dialog("tavern group. construct. failure.  cannot pass through gateway.")
            else
                Dialog("tavern group. construct. critical failure.  gateway immobilizes her and group gang bangs her.")
                -- TODO group sex
            end
        elseif girl:has_trait("Adventurer") then
            Dialog("Group of Adventurers. Adventurer Attempt. Joins the \"raid\" on a mighty monster.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("tavern group. adventurer. critical success. down the boss. top dps. rolls a 100 on loot. sells it for big profit.")
                wm.AddPlayerGold(wm.Range(500, 3000))
            elseif result > 60 then
                Dialog("tavern group. adventurer. success. get exp.")
                girl:experience(35)
            elseif result > 10 then
                Dialog("tavern group. adventurer. failure. raid wipe.  she does well, but the casters were standing in bad.")
            else
                Dialog("tavern group. adventurer. critical failure. She stood in bad and wiped the raid. pays for equipment repairs and must suck the healers cock to apologize.")
                -- TODO oral sex
            end
        elseif girl:has_trait("Fleet of Foot") then
            Dialog("Group of Adventurers. Fleet of Foot attempt. a group of 12 dwarves needs a thief to burgle something. \"we always give our thieves a nickname in alphabetical order.  we will call you Bimbo.\"")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Tavern group. thief. critical success. finds dragons weakness. gets exp and bonus item.")
                girl:experience(50)
                -- TODO special item
            elseif result > 60 then
                Dialog("tavern group. thief. success.")
                girl:experience(35)
            elseif result > 10 then
                Dialog("tavern group. thief. failure. dragon chases them back to town.")
            else
                Dialog("tavern group. thief.  critical failure.  talks to dragon while attempting to steal. dragon burns down the dwarves camp and eats the wizard.  dwarves gang bang her as punishment. \"it's in her contract, after all\"")
                -- TODO group sex
            end
        elseif girl:has_trait("Assassin") then
            Dialog("Tavern Group. Assassin attempt. adventuring guild needs a rogue for an upcoming battle against a rival guild.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Tavern group. Assassin Critical success.")
            elseif result > 60 then
                Dialog("tavern group. assassin. success")
                girl:experience(35)
            elseif result > 10 then
                Dialog("tavern group. assassin. failure")
            else
                Dialog("tavern group. assassin. critical failure.")
                -- TODO group sex
            end
        elseif girl:has_trait("Strong Magic") then
            Dialog("Tavern Group. Strong Magic attempt.  group needs a mage to teleport them, keep them supplied with magical food and drink, and do most of the damage to monsters.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Tavern group. Mage. critical success.")
            elseif result > 60 then
                Dialog("Tavern Group. Mage. Success.")
                girl:experience(35)
            elseif result > 10 then
                Dialog("tavern group. mage. failure.")
            else
                Dialog("Tavern Group. Mage. Critical failure.")
            end
        else
            Dialog("Tavern Group. Generic attempt. Santa, Bobo, and Pierce needs a girl to \"distract\" the monsters. they keep calling her \"pink\"")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Tavern Group. Generic. Critical Success.")
                -- TODO Beast Sex
            elseif result > 60 then
                Dialog("Tavern Group. Generic. Success.")
                -- TODO Beast Sex
            elseif result > 10 then
                Dialog("Tavern Group. Generic. Failure.")
                -- TODO Group Sex
            else
                Dialog("Tavern Group. Generic. Critical Failure.")
                -- TODO Lesbian Sex
            end
        end
    end
end

---@param girl wm.Girl
function ChurchQuest(girl)
    Dialog("Church")
    local what = wm.Range(1, 4)
    if what == 1 then
        Dialog("Church. Priest sends her to fight demon")
        if girl:has_trait("Adventurer") then
            Dialog("Church. Fight Demon. Adventurer attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Priest. Fight Demon. Critical success.")
            elseif result > 10 then
                if wm.Percent(girl:combat()) then
                    Dialog("Church Priest. Fight Demon. Adventurer. Combat success.")
                else
                    Dialog("Church Priest. Fight Demon. Adventurer. Combat Fail.")
                end
            else
                Dialog("Church Priest. Demon. Adventurer. Critical Failure.")
            end
        elseif girl:has_trait("Assassin") then
            Dialog("Church. Fight Demon. Assassin Attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Priest. Fight Demon. Assassin. Critical Success.")
            elseif result > 60 then
                Dialog("Church Priest. Demon. Assassin. Success.")
                -- TODO Beast Sex
            elseif result > 10 then
                Dialog("Church Priest. Demon. Assassin. failure")
                -- TODO Bondage Sex
            else
                Dialog("Church Priest. Demon. Assassin. Critical failure.")
                -- TODO Beast Sex
            end
        elseif girl:has_trait("Nymphomaniac") then
            Dialog("Church. Fight Demon. Nympho attempt. Priest is possessed by demon. must be fucked into submission.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Priest. Demon. Nympho. Critical Success.")
                -- TODO Anal sex
            elseif result > 10 then
                if wm.Percent(girl:normalsex()) then
                    -- TODO SEX
                    Dialog("Church Priest. Demon. nympho. success.")
                else
                    Dialog("Church. Priest. Demon. nympho. failure.")
                end
            else
                Dialog("Church Priest. Demon. Nympho. Critical Failure.")
                -- TODO Beast Sex
            end
        elseif girl:has_trait("Construct") then
            Dialog("Church. Fight Demon. Construct Attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Priest. Demon. Construct. critical success.")
            elseif result > 60 then
                Dialog("Church Priest. demon. construct. success.")
            elseif result > 10 then
                Dialog("Church Priest. Demon. Construct. Failure.")
            else
                Dialog("Church. Priest. Demon. Constuct. Critical failure.")
            end
        elseif girl:has_trait("Strong Magic") then
            Dialog("Church. Fight Demon. Strong Magic attempt to enslave the demon.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Priest. Demon. Strong magic. Critical Success. enslave demon in a monster girl form.")
                -- TODO Add Monster Girl
            elseif result > 10 then
                if wm.Percent(girl:magic()) then
                    Dialog("Church Priest. Demon. Strong Magic. Success. Demon is enslaved as an imp that boosts her magic ability.")
                    girl:magic(10)
                else
                    Dialog("Church Priest. Demon. Strong Magic. failure.  Demon injures girl and saps some of her magic ability.")
                    girl:magic(-10)
                    girl:health(-10)
                end
            else
                Dialog("Church. Priest. Demon. Strong Magic. Critical failure. demon transforms into beast and fucks the girl. as well as cursing her with some aging.")
                -- TODO Beast Sex
                girl:age(5)
            end
        elseif girl:has_trait("Demon") then
            Dialog("Church. Fight Demon. Demon attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("church priest. demon. demon critical success. convince former demon friend to take mortal form and join your brothel.")
                -- TODO lesbian sex
                -- TODO add girl to dungeon
            elseif result > 60 then
                Dialog("Church. Priest. demon to demon. success. seduce male demon and convince him to move on.")
                -- TODO normal sex
            elseif result > 10 then
                Dialog("Church Priest. demon to demon. failure.  she is injured by the demon and forced to flee.")
                girl:health(-20)
            else
                Dialog("Church Priest. Demon to demon. critical failure. she is defeated and violated by the other demon.")
                -- TODO beast sex
            end
        else
            Dialog("Church. Fight Demon. Generic attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Priest. Demon Generic critical success.")
            elseif result > 60 then
                Dialog("church priest. demon. generic success.")
            elseif result > 10 then
                Dialog("church priest demon. generic failure.")
            else
                Dialog("church priest demon generic critical failure.")
            end
        end
    elseif what == 2 then
        Dialog("Church. Young Inexperienced Paladin of the Phallic Order requests aid.")
        if girl:has_trait("Adventurer") then
            Dialog("Church. Young Paladin. Adventurer attempt to teach combat.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Young Paladin. Adventurer. Critical Success.  she teaches him some sword skills then rewards his other sword.")
                -- TODO Oral Sex
            elseif result > 60 then
                Dialog("Church. Young Paladin. Adventurer. success. young paladin learns some skills. Head paladin thanks her and gives her a small gold bonus.")
                wm.AddPlayerGold(wm.Range(100, 200))
            elseif result > 10 then
                Dialog("Church. Young Paladin. Adventurer. failure.  boy learns nothing. she is tired from the attempt.")
                girl:tiredness(-20)
            else
                Dialog("Church Young Paladin. adventurer.  Critical failure. boy tricks her and leads her to a secluded training ground where he defeats her in combat and takes her ass as a prize.")
                -- TODO Anal sex
            end
        elseif girl:has_trait("Nymphomaniac") then
            Dialog("Church. Young Paladin.  Nympho attempt. he is good with sword but not with women.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church Young Paladin. nympho. critical success. she realizes her lessons are paying off as he makes her orgasm several times.  He gains confidence and she figures out some new things about sex.")
                -- TODO Normal Sex
                girl:normalsex(10)
                girl:confidence(10)
            elseif result > 60 then
                Dialog("church. Young Pally. Nympho. success. she teaches him well and he pays her for her lessons.")
                wm.AddPlayerGold(wm.Range(50, 150))
                -- TODO normal sex
            elseif result > 10 then
                Dialog("Church. Young Pally.  nympho.  failure.  her lessons don't help the boy and his confidence remains low.")
            else
                Dialog("hurch. Young Paladin. nympho. she discovers that the boy was being passed around the barracks as a toy. She helps him flee, but is caught in the process.. the other knights spend hours passing her around the room instead.")
                -- TODO group sex
                girl:happiness(-20)
                AdjustLust(girl, -60)
                girl:health(-20)
            end
        elseif girl:has_trait("Lesbian") then
            Dialog("Church. Young Paladin. Lesbian attempt.  Paladin is actually a girl.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("church. young paladin. Lesbian. critical success. She spends several days with the girl and they form a bond.  She convinces her that it's better to run from the order and come back to the brothel with her.")
                -- TODO Lesbian Sex
            elseif result > 60 then
                Dialog("church. young paladin. lesbian. success.  she gives the girl a magical strap-on and teaches her what she knows about men based on her experience and the girl is able to remain hidden among the ranks.")
                -- TODO Lesbian sex
            elseif result > 10 then
                Dialog("church. young paladin. lesbian. failure.  she is unable to help the young girl.")
            else
                Dialog("church. young paladin. lesbian. critical failure.  the two are discovered together one night and the knights force the women to fuck each other in front of them in the barracks.  the men form a circle around them and cover the women in cum.")
                -- TODO lesbian sex
            end
        else
            Dialog("Church. Young Paladin. Generic Attempt. he has lost his sword in the marketplace.")
            if girl:has_trait("Slow Learner") then
                Dialog("Church. young paladin. generic.  Upon hearing the words \"lost sword\" she immediately drops to her knees and removes his quickly hardening cock from his pants.  she exclaims with a giggle. \"I found it!\" and begins to suck his member in the middle of the chapel.  you are forced to come down to the chapel later to collect her and give a sizeable \"donation\" to the church for the disruption.  You see the young paladin with a huge smile on his face, whistling a tune as he goes about his chores.")
                wm.TakePlayerGold(500)
            else
                local result = wm.Range(1, 100)
                if result >= 95 then
                    Dialog("church. young paladin. generic. critical success.  she calms him down with a blowjob and he is able to remember where the sword is.")
                elseif result > 60 then
                    Dialog("church. young paladin. generic. success. she helps him retrace his steps and find his sword.")
                elseif result > 10 then
                    Dialog("church. young paladin. generic. failure. she is unable to help him find his sword.")
                else
                    Dialog("church. young paladin. generic. critical failure. a trap.  gang bang in a dark alley.")
                end
            end
        end
    elseif what == 3 then
        -- Church. Naughty Nun : About middle of the old script
        Dialog("Church. Naughty Nun.")
        if girl:has_trait("Nymphomaniac") then
            Dialog("Church. Naughty Nun. Nympho attempt. church forbids same sex intercourse.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("church. naughty nun. nympho. critical success. shows her the joys of sex and convinces her to join the brothel.")
                -- TODO Lesbian Sex
                -- TODO Add Random Girl
            elseif result > 60 then
                Dialog("church. naughty nun. nympho. success.  she helps reduce her high libedo and promises to visit regularly to keep it under control.")
                -- TODO Lesbian Sex
            elseif result > 10 then
                Dialog("church. naughty nun. nympho. failure.  they are almost caught together and she is forced to flee before she could get the nun off.  which only added to her libedo troubles.")
            else
                Dialog("church. naughty nun. nympho. critical failure. they are caught together in the nuns room.  you have to pay a fine and come collect your whore.")
                -- TODO Lesbian Sex
                wm.TakePlayerGold(500)
            end
        elseif girl:has_trait("Futanari") then
            Dialog("Church. Naughty Nun. Futanari attempt. Nun desperately craves cock but the convent forbids sex with men and won't allow them inside.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("church. naughty nun. futa. critical success.  Shows her the joys of a penis and helps her escape the convent.")
                -- TODO Lesbian Sex
            elseif result > 60 then
                Dialog("hurch. naughty nun. futa. success.  she spends several nights at the convent and helps alleviate the nuns raging need for cock.")
                -- TODO Lesbian Sex
            elseif result > 10 then
                Dialog("church. naughty nun. futa. failure.  nun is not attracted to women, even if they do have a penis")
                -- TODO mast
            else
                Dialog("church. naughty nun. futa. critical failure.  she is caught with her dick in the nuns ass ,and the nun is forced to leave.")
                -- TODO Lesbian Sex
            end
        elseif girl:service() > 70 then
            Dialog("Church. Naughty Nun. Service. Help her clean up.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Naughty Nun. Service. critical success.  Find items hidden in abandoned broom closet.")
                -- TODO Cleaning
                -- TODO add special item
            elseif result <= 10 then
                Dialog("Church. Naughty Nun. Service. Critical Failure.  break a holy relic.")
                -- TODO Cleaning
                wm.TakePlayerGold(1000)
            elseif wm.Percent(girl:service()) then
                Dialog("Church. Naughty Nun. Service Success.")
                -- TODO Cleaning
            else
                Dialog("Church. Naughty Nun. Service fail.  unable to clean up in time. nun gets in trouble.")
            end
        else
            Dialog("Church. Naughty Nun. Generic Attempt. help her focus on her prayers and eliminate sexual thoughts. the Nun asks about her time spent in the brothel.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Naughty Nun. Generic. Critical Success.  She finds the focus to block out impure thoughts by listening to some stories about how bad sex can be. she thanks your girl and blesses you publicly.")
            elseif result > 60 then
                Dialog("Church. Naughty Nun. Generic. success. she shows the nun some techniques she uses to shut out the sounds and thoughts of sex she uses when she needs to sleep. the nun thanks her for her help.")
            elseif result > 10 then
                Dialog("Church. Naughty Nun. generic. Failure.  She regales the nun with her sex stories for hours, but the nun only seems to become more horny.  The nun thanks her for trying but asks her to not visit again.")
            else
                Dialog("Church. Naughty Nun. critical failure.  Each story makes the nun hornier and hornier.  The nun pounces on her and they become lost in deep passionate sex.  afterwards, the nun renounces her vows and leaves the convent.")
                -- TODO Lesbian Sex
            end
        end
    elseif what == 4 then
        Dialog("Church. beggar needs help.")
        if girl:has_trait("Charismatic") then
            Dialog("Church. Beggar. Charismatic attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Beggar. Charismatic. critical success.  She brings the young girl back to the brothel and convinces you to take her in.")
                -- TODO Lesbian Sex
                -- TODO Add Random Girl
            elseif result <= 10 then
                Dialog("Church. Beggar. Critical failure.  The old beggar follows her always out of the church. he sneaks up behind her and drags her down a dark alley and fucks her ass.")
                -- TODO Anal Sex
            elseif wm.Percent(girl:charisma()) then
                Dialog("Church. Beggar.  success. she helps the young mother find a job and a place for her and her children to stay.")
            else
                Dialog("Church. Beggar. Charismatic. Failure. she is at a loss to help the deformed man. she tosses a few coins at him and runs away.")
                wm.TakePlayerGold(wm.Range(10, 80))
            end
        else
            Dialog("Church. Beggar. Generic Attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Church. Beggar. Generic. Critical Success.")
            elseif result > 60 then
                Dialog("Church. Beggar. Generic. Success.")
            elseif result > 10 then
                Dialog("Church. Beggar. Generic. Failure.")
            else
                Dialog("Church. Beggar. Generic. Critical Failure.  Group of Beggars gang bang her.")
                -- TODO Group Sex
            end
        end
    end
end


---@param girl wm.Girl
function StreetsQuest(girl)
    Dialog("Streets")
    local what = wm.Range(1, 4)
    if what == 1 then
        Dialog("Streets.  Gang clean up")
        if girl:has_trait("Adventurer") then
            Dialog("Streets. Gang Clean up. Adventurer attempt. defeat the gang.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang Clean up. Adventurer. Critical Success.  forces gang to clean up and do repairs to neighborhood.")
                wm.SetPlayerDisposition(10)
            elseif result <= 10 then
                Dialog("Streets. Gang Clean up. Adventurer. Critical Failure.  She is defeated and gang banged in the middle of the streets.")
            elseif wm.Percent(girl:combat()) then
                Dialog("Streets. Gang Clean up. Adventurer. Combat Success.  defeats the gang and forces them out of neighborhood.")
            else
                Dialog("Streets. Gang Clean up. Adventurer. Combat Failure.  She is defeated and gang holds on to territory.")
            end
        elseif girl:has_trait("Charismatic") then
            Dialog("Streets. Gang Clean up. Charismatic attempt. convince the gangs to leave the neighborhood.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang Clean up. Charismatic. Crit Success. Convinces gang to leave and do repairs and clean the neighborhood.")
            elseif result <= 10 then
                Dialog("Streets. Gang. Charismatic. Crit Fail.  gang banged in the streets.")
            elseif wm.Percent(girl:charisma()) then
                Dialog("Streets. Gang. Charisma Pass.  convinces them to leave.")
            else
                Dialog("Streets. Gang. Charisma fail. they refuse to leave.")
            end
        elseif girl:has_trait("Nymphomaniac") then
            Dialog("Streets. Gang Clean up. Nympho attempt. Fuck them all at once and arrest them when they are passed out.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang Clean up. Nympho. Crit Success.  pleasures the entire gang, but manages to get the Gang leader sent to your dungeon.")
                -- TODO group sex
            elseif result <= 10 then
                Dialog("Streets. Gang. Nympho. Cirt Fail. gang uses and abuses her. pay ransom.")
                wm.TakePlayerGold(500)
            elseif wm.Percent(girl:group()) then
                Dialog("Streets. gang. Nympho. success.  fucks them all into sleepy stupor. brings the city guard in to arrest them.")
                -- TODO group sex
            else
                Dialog("Streets. Gang. Nympho. fail.  Gang leader ties her up and uses her for a while.. pay ransom.")
                -- TODO bondage sex
                wm.TakePlayerGold(500)
            end
        elseif girl:has_trait("Strong Magic") then
            Dialog("Streets. Gang Cleanup. Strong Magic attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang. Magic. Crit Win.  Mind controls gang to clean up and make repairs then forces them to join the city guard.")
            elseif result <= 10 then
                Dialog("Streets. Gang. Magic. Crit fail.  Magic goes out of control and damages local buildings.  gang temporarily retreats. you pay damages.")
                wm.TakePlayerGold(2000)
            elseif wm.Percent(girl:magic()) then
                Dialog("Streets. Gang. Magic. Win.  they are no match for her magic and they flee in terror.")
            else
                Dialog("Streets. Gang. Magic. Lose.  They defeat her summoned monsters and she flees.")
            end
        elseif girl:has_trait("Assassin") then
            Dialog("Streets. Gang Clean up. Assassin Attempt. Kill the gang leader.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang. Assassin. Crit. Win.   Kill the leader, but by the rules of the gang she becomes new leader. orders them to protect neighborhood instead.")
            elseif result <= 10 then
                Dialog("Streets. Gang. Assassin. Crit Fail.  Fails to sneak in.  guards are alerted. she is captured and gang banged. pay ransom.")
                -- TODO group sex
            elseif wm.Percent(girl:agility()) then
                Dialog("Streets. Gang. Assassin.  Agile pass.  Kill gang leader. gang disolves.")
            else
                Dialog("Streets. Gang. Assassin. Agile Lose.  She is unable to find a break in the gangs defenses.")
            end
        else
            Dialog("Streets. Gang Cleanup. Generic Attempt. gather information for the city guard.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Gang. Generic. Crit Win.  she finds enough evidence for the city guard to arrest the leader and information to use as leverage against the rest of the gang.  they each pay you a small amount to keep it quiet but it adds up.")
            elseif result > 60 then
                Dialog("Streets. Gang. Generic. Win.   Gathers evidence and gang is arrested.")
            elseif result > 10 then
                Dialog("Streets. Gang. Generic. fail.  Unable to find anything.")
            else
                Dialog("Street.Gang. Generic. Crit fail. gang discovers what she is doing. gang banged and held hostage. pay ransom.")
                wm.TakePlayerGold(300)
                -- TODO Group Sex
            end
        end
    elseif what == 2 then
        Dialog("Streets. Orphans need help")
        if girl:has_trait("Adventurer") then
            Dialog("Streets. Orphans. Adventurer attempt. reclaim the orphans home from monsters.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Orphans. Adventurer. Crit Win.  Reclaim the farm. kill the minotaur group that had been there.  orphans are supplied with food. and make a profit off the monsters weapons.")
            elseif result <= 10 then
                Dialog("Streets. Orphans. Adventurer. Crit Fail.  Beast defeats the girl and abuses her.")
                -- TODO Beast Sex
            elseif wm.Percent(girl:combat()) then
                Dialog("Streets. Orphans. Adventurer. Combat win.  reclaim the home for children.")
            else
                Dialog("Streets. Orphans. Adventurer. Combat lose.  she is forced to retreat.")
            end
        elseif girl:has_trait("Charismatic") then
            Dialog("Street. Orphans. Charismatic attempt. convince city officials to build a new nicer orphanage.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Orphans. Charisma. Crit Win. City opens a deluxe orphanage that appeals to higher class families.  You , as her boss, receive a commendation from city.")
            elseif result <= 10 then
                Dialog("Streets. Orphans. Charisma. Crit Fail.  she is tricked by the city and the orphans are sent to slave mines.")
            elseif wm.Percent(girl:charisma()) then
                Dialog("Streets. Orphans. Charisma. Win.  New orphanage is built in the city.")
            else
                Dialog("Streets. Orphans. Charisma fail.  No new orphanage. children remain homeless.")
            end
        elseif girl:service() > 60 then
            Dialog("Streets. Orphans. Service attempt.  Teach them how to care for themselves.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Orphans. Service. Crit win.  They not only learn housework, but basic textiles and open a little shop.")
            elseif result <= 10 then
                Dialog("Streets. Orphans. Service. Crit. Fail.  She thought they learned, but later that day. they burn down half the city block trying to cook.  you must pay damages.")
                wm.TakePlayerGold(wm.Range(1000, 2000))
            elseif wm.Percent(girl:service()) then
                Dialog("Streets. Orphans. Service.  Pass.  they learn enough to keep fed and maintain their dwelling.")
            else
                Dialog("Streets. Orphans. Service. fail.  she is unable to teach them anything.")
            end
        else
            Dialog("Street. Orphans. Generic Attempt.  find them homes.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Orphans. Generic.  Crit win.  find the 2 youngest nice homes, but oldest girl come to work for you.")
                -- TODO Add Girl
            elseif result > 60 then
                Dialog("Streets.Orphans. Generic. Find them all homes.")
            elseif result > 10 then
                Dialog("Streets. Orphans.  Generic Fail.  Unable to find them homes.")
            else
                Dialog("Streets. Orphans. Generic. Crit Fail.  Oldest boy gets angry that she can't help and knocks her down and fucks her ass.")
                -- TODO Anal Sex
            end
        end
    elseif what == 3 then
        Dialog("Streets. Hookers need protection.")
        if girl:has_trait("Adventurer") then
            Dialog("Streets. Hooker. Adventurer attempt. defeat the abusive john.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets Hooker. Adventurer. crit win.  wins fight and sends john to your dungeon.")
            elseif result <= 10 then
                Dialog("Street. Hooker. Adventurer. Crit Fail.  Man kills street hooker. ties up your girl. fucks her in a nearby cellar.")
            elseif wm.Percent(girl:combat()) then
                Dialog("Street. Hooker.  Adventurer. Combat win.  defeat man and scare him off.")
                if girl:has_trait("Sadistic") then
                    Dialog("before he leaves she cuts off his penis as a trophy.")
                end
            else
                Dialog("Street. Hooker.  Adventurer. combat fail.  man kidnaps the hooker.")
            end
        elseif girl:beauty() > 80 or girl:has_trait("Great Figure") or girl:has_trait("Great Arse") or girl:has_trait("Long Legs") then
            Dialog("Street. Hooker. Amazing beauty seduce jack the ripper type. attempt.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Street. Hooker. Beauty. Crit win.  ripper actually falls madly in love with her.  she forces him to buy her an expensive item and then tricks him into your dungeon.")
                -- TODO Normal Sex
                -- TODO Add Customer to Dungeon
            elseif result > 60 then
                Dialog("street hooker. beauty. win.  seduces the ripper and has him arrested as he sleeps.")
                -- TODO Normal Sex
            elseif result > 10 then
                Dialog("Street hooker. beauty. lose. she camps out the wrong alley and never finds the ripper.")
            else
                Dialog("Street. Hooker. Beauty. Crit Fail.  she falls victim to the ripper and he injures and disfigures her.")
                girl:beauty(-70)
                girl:health(-40)
            end
        elseif girl:has_trait("Charismatic") then
            Dialog("Street. Hooker. Charismatic attempt. Clean her up and get her a job in a brothel")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Street Hooker. Charisma. Crit Win.  She does such a great job teaching her and cleaning up her appearance. that you agree to hire her.")
                -- TODO Lesbian Sex
            elseif result <= 10 then
                Dialog("Street Hooker. Charisma Crit Fail.  she has to seal the deal with a rival brothel by fucking one of their beasts. only too find out the street hooker was fed to the beasts.")
                -- TODO Beast Sex
            elseif wm.Percent(girl:charisma()) then
                Dialog("Street Hooker.  Charisma. Pass.  She finds her work on a safer street.")
            else
                Dialog("Street Hooker. Charisma fail.  She is unable to find anywhere for the hooker to work.")
            end
        else
            Dialog("Streets. Hooker. Generic Attempt. try to get city guard to patrol the hookers alley.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Street Hooker. Generic. Crit Win. City guard agrees and you get a commission as the guards become regular customers of the hooker.")
            elseif result > 60 then
                Dialog("Street Hooker.  Generic.  Win. City guards agree to a few more patrols.")
            elseif result > 10 then
                Dialog("Street Hooker. Generic. City guard refuse to patrol area.")
            else
                Dialog("Street Hooker. generic.  Hooker was a city guards trap.  they gang bang her in the barracks.")
                -- TODO group sex
            end
        end
    elseif what == 4 then
        Dialog("Streets. Passage to underground lair.")
        if girl:has_trait("Adventurer") then
            Dialog("Streets. Underground Lair. Adventurer attempt. defeat the rival gangs secret stockpile guards.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Underground. Adventurer. Crit Win.  defeat gang bonus item.")
                -- TODO special item
            elseif result <= 10 then
                Dialog("Street. Underground. Adventurer. Crit Fail. injured and gang banged.")
                -- TODO group sex
                girl:health(-20)
            elseif wm.Percent(girl:combat()) then
                Dialog("Streets. Underground. Adventurer. Combat Win.  defeats gang. cash bonus")
                wm.AddPlayerGold(wm.Range(300, 700))
            else
                Dialog("Streets. Underground. Adventurer. Combat lose.  Forced to retreat injured.")
                girl:health(-10)
            end
        elseif girl:has_trait("Nymphomaniac") then
            Dialog("Streets. Underground Lair. Nympho attempt. find a ancient sex pleasure dungeon.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. Underground. Nympho. fucking the statue of the god of anal sex unlocks a hidden chamber with treasure. item bonus.")
                -- TODO special item; Anal sex
                girl:level(1)
                girl:charisma(15)
            elseif result <= 10 then
                Dialog("Street. Underground. nympho.  magical sex devices trap her for hours but they absorb her youth.")
                -- TODO bondage sex
                girl:age(20)
            elseif wm.Percent(girl:libido()) then
                Dialog("Streets. UNderground. Nympho. Win.  she uses the devices for hours and gains knowledge of sex skills.")
                -- TODO normal sex
                girl:normalsex(5)
                girl:oralsex(5)
                girl:tittysex(5)
                girl:handjob(5)
                girl:footjob(5)
                girl:group(5)
                girl:lesbian(5)
                girl:strip(5)
                gril:bdsm(5)
                girl:charisma(5)
            else
                Dialog("Streets. underground. nympho.  fail.  She pleasures her self with the machines for hours but gains nothing but some happiness and a satisfied libido.")
                girl:happiness(20)
                AdjustLust(girl, -30)
                -- TODO anal sex
            end
        elseif girl:has_trait("Construct") then
            Dialog("Streets. Underground Lair. Construct attempt.  Finds alchemy lab.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Streets. UNderground. Construct. Crit Win.  Finds alchemical potion to boost her constitution. and some items")
                -- TODO special item
            elseif result <= 10 then
                Dialog("Streets. underground. construct.  Crit Fail.  Alchemist captures her. He \"explores\" her for hours before wiping he memory of the location and letting her go.")
                -- TODO normal sex
                girl:tiredness(80)
                girl:experience(-40)
            elseif wm.Percent(girl:constitution()) then
                Dialog("Streets. Underground. Construct. Con. pass.  she finds some potions to improve her constitution.")
            else
                Dialog("Street. UNderground. Construct. Fail. She finds nothing of value.")
            end
        elseif girl:has_trait("Strong Magic") then
            Dialog("Streets. Underground lair. Strong Magic attempt. Ancient wizards secret lair.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Street. Underground. Magic. Crit Win.  She boosts her Int and Magic and finds an item.")
                -- TODO special item
                girl:magic(15)
                girl:intelligence(15)
            elseif result <= 10 then
                Dialog("Street. UNderground. Magic. Crit Fail.  Wizard comes to life to fuck her ass and steal some youth.")
                -- TODO anal sex
                girl:age(5)
            elseif wm.Percent(girl:magic()) then
                Dialog("Street. Underground. Magic. win.  Studies some of the texts and increases her INT.")
            else
                Dialog("Street. Underground. Magic. Fail.  Books turn to dust when touched. she finds nothing.")
            end
        else
            Dialog("Streets. Underground Lair. Generic attempt. investigate the bandit hide out.")
            local result = wm.Range(1, 100)
            if result >= 95 then
                Dialog("Street. Underground. Generic. Crit win.  City guard arrests bandits and she finds a cash bonus.")
                wm.SetPlayerDisposition(15)
                wm.AddPlayerGold(wm.Range(300, 500))
            elseif result > 60 then
                Dialog("Street. Underground. Generic. win.  Finds information for city guards.")
                wm.SetPlayerDisposition(10)
            elseif result > 10 then
                Dialog("Streets. Underground. Generic. Fail.  She finds nothing to help the guards.")
            else
                Dialog("Street. Underground. Generic. Crit Fail.  Bandits capture her. gang bang. you pay ransom.")
                -- TODO group sex
                wm.TakePlayerGold(wm.Range(300, 400))
            end
        end
    end
end