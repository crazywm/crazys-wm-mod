---@class wm.Girl
local g = {}
function g:add_trait(trait) end
function g:has_trait(trait) end
function g:calc_player_pregnancy(good, factor) end
function g:calc_group_pregnancy(good, factor) end
function g:stat(stat, change) end
function g:skill(skill, change) end
function g:name() end
function g:torture() end
function g:give_money() end
function g:trigger(event) end

function g:obey_check(action) end
function g:skill_check(skill, target) end

-- shortcuts
function g:anal(change) end
function g:bdsm(change) end
function g:beastiality(change) end
function g:group(change) end
function g:lesbian(change) end
function g:service(change) end
function g:strip(change) end
function g:normalsex(change) end
function g:oralsex(change) end
function g:tittysex(change) end
function g:handjob(change) end
function g:footjob(change) end

function g:beauty(change) end
function g:happiness(change) end
function g:refinement(change) end
function g:magic(change) end
function g:medicine(change) end
function g:crafting(change) end
function g:herbalism(change) end
function g:farming(change) end
function g:brewing(change) end
function g:cooking(change) end
function g:animalhandling(change) end
function g:performance(change) end
function g:combat(change) end
function g:pcfear(change) end
function g:pchate(change) end
function g:pclove(change) end
function g:tiredness(change) end
function g:obedience(change) end
function g:libido(change) end
function g:confidence(change) end

g.ACTIONS = {}


---@class wm.Customer
local c = {}
function c:happiness(change) end

---@module wm
local wm = {}
function wm.ChoiceBox(question, ...) end
function wm.Dialog(text) end
function wm.UpdateImage(image_type) end
function wm.SetPlayerDisposition(change) end
function wm.SetPlayerSuspicion(change) end
function wm.GetPlayerDisposition() end
function wm.GetPlayerSuspicion() end
function wm.AddPlayerGold(change) end
function wm.TakePlayerGold(change) end
function wm.GivePlayerRandomSpecialItem() end
function wm.AddBeasts(change) end
function wm.GetBeasts() end
function wm.AddCustomerToDungeon(reason, daughters, wife) end
function wm.AddFamilyToDungeon(num_daughters, mother) end
function wm.Percent(percent) end
function wm.Range(min, max) end
function wm.GameOver() end

function wm.AcquireGirl(girl) end
function wm.CreateRandomGirl(reason, age) end
function wm.ToDungeon(girl, reason) end