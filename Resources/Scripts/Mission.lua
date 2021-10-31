---@param girl wm.Girl
function GoOnMission(girl)
    local action = ChoiceBox("",
            "Assassinate a troublesome politician    COST: 1000 gold",
            "Sneak into the Hall of Records   COST: 750 gold",
            "Seduce/Bribe a government official    COST: 500 to 4000 gold",
            "Duel a Rival Brothel Girl  ENTRY FEE: 100 gold  PRIZE MONEY: 500 gold",
            "Accompany you into the catacombs",
            "Send her into the night to steal",
            "Send her on a Quest",
            "Go Back"
    )
    if action == 0 then
        AssassinatePolitician(girl)
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
            return girl:trigger("girl:refuse")
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
            return girl:trigger("girl:refuse")
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
            return girl:trigger("girl:refuse")
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
            return girl:trigger("girl:refuse")
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
            return girl:trigger("girl:refuse")
        end
    elseif action == 6 then
        return girl:trigger("girl:quest")
    end
end

function AssassinatePolitician(girl)
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
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function Quest(girl)
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
        local where = wm.Range(1, 3)
        if where == 1 then  -- Tavern
            girl:trigger("girl:quest:tavern")
        elseif where == 2 then -- Church
            girl:trigger("girl:quest:church")
        elseif where == 3 then -- Streets
            girl:trigger("girl:quest:streets")
        end
    end
end
