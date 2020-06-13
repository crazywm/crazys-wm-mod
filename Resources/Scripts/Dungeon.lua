function GirlDied(girl)
    Dialog("A girl has died in the dungeon.\nHer body will be removed by the end of the week.")
    if wm.Percent(10) then -- only 10% of being discovered
        wm.SetPlayerSuspicion(1)
    end
    wm.SetPlayerDisposition(-1)
    -- for(auto& broth : g_Game->buildings().buildings()) {
    --        broth->update_all_girls_stat(STAT_PCFEAR, 2);
    --    }
end

function CustDied(cust)
    Dialog("A customer has died in the dungeon.\nTheir body will be removed by the end of the week.", 1);

    if wm.Percent(10) then -- only 10% of being discovered
        wm.SetPlayerSuspicion(1)
    end

    wm.SetPlayerDisposition(-1)
    -- g_Game->player().customerfear(1);
end
