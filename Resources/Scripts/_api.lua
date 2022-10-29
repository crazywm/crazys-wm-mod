---@class WM
---@field IMG table<string, number>
---@field IMG_PART table<string, number>
---@field ACTIONS table<string, number>
---@field STATS table<string, number>
---@field SKILLS table<string, number>
---@field TRAITS table<string, string>
wm = {}

---@class wm.Customer
local c = {}
function c:happiness(change) end

function wm.ChoiceBox(question, ...) end
function wm.Dialog(text) end
function wm.UpdateImage(image_type, ...) end
function wm.SelectImage(girl, image_type, ...) end
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
---@return number
---@param min number
---@param max number
function wm.Range(min, max) end
function wm.RandName(mode) end
function wm.GameOver() end

function wm.AcquireGirl(girl) end
function wm.CreateRandomGirl(reason, age) end
function wm.ToDungeon(girl, reason) end
function wm.ToJail(girl) end

---@class wm.Girl
local g = {}
---@param trait string
---@param duration number
---@overload fun(trait: string, duration: number) @add temporary trait
---@overload fun(trait: string): number
function g:add_trait(trait, duration) end
function g:has_trait(trait, ...) end
function g:remove_trait(trait) end

function g:has_item(item) end

function g:calc_player_pregnancy(good, factor) end
function g:calc_group_pregnancy(good, factor) end
function g:weeks_pregnant() end
function g:pregnancy_term() end
function g:stat(stat, change) end
function g:skill(skill, change) end
function g:enjoyment(what, change) end
function g:name() end
function g:firstname() end
function g:breast_size() end
function g:torture() end
function g:give_money() end
function g:trigger(event) end

function g:obey_check(action) end
function g:skill_check(skill, target) end

function g:is_slave() end
function g:is_pregnant() end
function g:check_virginity() end
function g:lose_virginity() end

-- shortcuts
---@overload fun(change: number): number
---@overload fun(): number
function g:anal(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:bdsm(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:beastiality(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:group(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:lesbian(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:service(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:strip(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:normalsex(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:oralsex(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:tittysex(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:handjob(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:footjob(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:magic(change) end

---@overload fun(change: number): number
---@overload fun(): number
function g:beauty(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:happiness(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:refinement(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:magic(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:medicine(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:crafting(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:herbalism(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:farming(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:brewing(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:cooking(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:animalhandling(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:performance(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:combat(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:pcfear(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:pclove(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:tiredness(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:obedience(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:libido(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:confidence(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:intelligence(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:charisma(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:age(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:spirit(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:health(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:experience(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:dignity(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:constitution(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:agility(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:fame(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:lactation(change) end
---@overload fun(change: number): number
---@overload fun(): number
function g:strength(change) end

g.ACTIONS = {}

wm.Girl = g
