class STH_HordeInstance
{
    ref STH_HordeLocation Location;
    ref array<DayZInfected> Infected;
    float NextSpawnTime;
    float NextTeleportCheck;
    int SpawnedAt;
    ref map<DayZInfected, int> LastTeleportTime;
    ref map<DayZInfected, int> LastAssistTime;
    ref map<DayZInfected, float> LastObservedHealth;
    bool ChestSpawned;
    int OriginalCount;
    ref map<PlayerBase, int> LastProgressNotify;
    ref map<PlayerBase, int> ProgressLastShownDead;
    vector SmokePosition;
    bool SmokeActive;

    void STH_HordeInstance(STH_HordeLocation loc)
    {
        Location = loc;
        Infected = new array<DayZInfected>;
        NextSpawnTime = 0;
        NextTeleportCheck = 0;
            SpawnedAt = 0;
            LastTeleportTime = new map<DayZInfected, int>;
        LastAssistTime = new map<DayZInfected, int>;
        LastObservedHealth = new map<DayZInfected, float>;
        ChestSpawned = false;
        OriginalCount = 0;
        LastProgressNotify = new map<PlayerBase, int>;
    ProgressLastShownDead = new map<PlayerBase, int>;
        SmokePosition = "0 0 0";
        SmokeActive = false;
    }
};

class STH_HordeManager
{
    ref STH_Config m_Config;
    ref array<ref STH_HordeInstance> m_Hordes;
    bool m_Initialized;
    int m_NextToughnessUpdate;
    int m_NextProgressUpdate;
    ref array<vector> m_ActiveSmokePositions;
    ref array<int> m_ActiveSmokeExpire;
    int m_NextSmokeBroadcast;

    void STH_HordeManager()
    {
        m_Config = new STH_Config();
        m_Hordes = new array<ref STH_HordeInstance>;
        m_Initialized = false;
        m_NextToughnessUpdate = 0;
        m_NextProgressUpdate = 0;
        m_ActiveSmokePositions = new array<vector>;
        m_ActiveSmokeExpire = new array<int>;
        m_NextSmokeBroadcast = 0;
    }

    void Init()
    {
        if (m_Initialized)
        {
            return;
        }
        m_Config.Init();
        
    STH_HordeInstance inst = new STH_HordeInstance(NULL);
    
    int delay = GetGlobalInterval();
    inst.NextSpawnTime = GetGame().GetTime() / 1000 + delay;
    Print("[SagaTheHorde] First horde spawn scheduled in " + ("" + delay) + "s based on Global.HordeIntervalSeconds");
        m_Hordes.Insert(inst);

        m_Initialized = true;
    }

    int GetGlobalInterval()
    {
        int baseInterval = 600;
        if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.HordeIntervalSeconds > 0)
        {
            baseInterval = m_Config.Data.Global.HordeIntervalSeconds;
        }
        if (baseInterval < 30)
        {
            baseInterval = 30;
        }
        return baseInterval;
    }

    void OnUpdate(float timeslice)
    {
        if (!m_Initialized)
        {
            return;
        }
        int i;
        int now = GetGame().GetTime() / 1000;
        if (now >= m_NextToughnessUpdate)
        {
            int it;
            for (it = 0; it < m_Hordes.Count(); it++)
            {
                AdjustToughness(m_Hordes.Get(it));
            }
            m_NextToughnessUpdate = now + 1;
        }
        for (i = 0; i < m_Hordes.Count(); i++)
        {
            STH_HordeInstance inst = m_Hordes.Get(i);
            if (inst.NextSpawnTime >= 0 && now >= inst.NextSpawnTime)
            {
                int active = 0;
                int k;
                for (k = 0; k < m_Hordes.Count(); k++)
                {
                    STH_HordeInstance hi = m_Hordes.Get(k);
                    if (hi && hi.Infected.Count() > 0)
                    {
                        active = active + 1;
                    }
                }
                int maxActive = 1;
                if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.MaxActiveHordes > 0)
                {
                    maxActive = m_Config.Data.Global.MaxActiveHordes;
                }
                if (active >= maxActive)
                {
                    inst.NextSpawnTime = now + 10;
                    continue;
                }
                int minPlayers = 0;
                if (m_Config && m_Config.Data && m_Config.Data.Global)
                {
                    minPlayers = m_Config.Data.Global.MinPlayersForHordeSpawn;
                }
                if (minPlayers > 0)
                {
                    array<Man> pcheck = new array<Man>;
                    GetGame().GetPlayers(pcheck);
                    int online = pcheck.Count();
                    if (online < minPlayers)
                    {
                        inst.NextSpawnTime = now + 30;
                        Print("[SagaTheHorde] Skipping horde spawn: online players " + online + " < required " + minPlayers + ". Retrying in 30s.");
                        continue;
                    }
                }
                STH_HordeInstance newInst = new STH_HordeInstance(NULL);
                newInst.NextSpawnTime = -1;
                int made = SpawnHorde(newInst);
                if (made > 0)
                {
                    m_Hordes.Insert(newInst);
                    inst.NextSpawnTime = now + GetGlobalInterval();
                    Print("[SagaTheHorde] Horde spawned with " + ("" + made) + " infected. Next spawn in " + ("" + GetGlobalInterval()) + "s. Active hordes: " + ("" + (active + 1)) + "/" + ("" + maxActive));
                }
                else
                {
                    inst.NextSpawnTime = now + 30;
                    Print("[SagaTheHorde] Spawn attempt produced 0 infected. Will retry in 30s.");
                }
            }
            if (now >= inst.NextTeleportCheck)
            {
                HandleTeleports(inst);
                inst.NextTeleportCheck = now + 5;
            }

            if (m_Config && m_Config.Data && m_Config.Data.Global)
            {
                int lifetime = m_Config.Data.Global.HordeLifetimeSeconds;
                if (lifetime > 0 && inst.SpawnedAt > 0)
                {
                    if ((now - inst.SpawnedAt) >= lifetime && inst.Infected.Count() > 0)
                    {
                        int removed = DespawnHorde(inst);
                        NotifyHordeDespawn(inst.Location, removed);
                        if (inst.SmokeActive)
                        {
                            StopSmokeForInstance(inst);
                        }
                        inst.SpawnedAt = 0;
                        inst.LastTeleportTime.Clear();
                        if (inst.LastAssistTime) inst.LastAssistTime.Clear();
                        if (inst.LastObservedHealth) inst.LastObservedHealth.Clear();
                        inst.ChestSpawned = false;
                    }
                }
                if (m_Config.Data.Global.EnableChestReward && inst.SpawnedAt > 0 && !inst.ChestSpawned)
                {
                    if (AreAllZombiesDead(inst))
                    {
                        if (inst.SmokeActive)
                        {
                            StopSmokeForInstance(inst);
                        }
                        SpawnRewardChestAt(inst);
                        inst.ChestSpawned = true;
                    }
                }
                if (m_Config.Data.Global.EnableHordeProgressDisplay && now >= m_NextProgressUpdate)
                {
                    ShowHordeProgress(inst);
                }
            }
        }
        if (now >= m_NextSmokeBroadcast)
        {
            RebroadcastActiveSmokes();
            m_NextSmokeBroadcast = now + 5;
        }
        if (m_Config && m_Config.Data && m_Config.Data.Global && now >= m_NextProgressUpdate)
        {
            int interv = m_Config.Data.Global.HordeProgressIntervalSeconds;
            if (interv <= 0) interv = 10;
            m_NextProgressUpdate = now + interv;
        }
    }

    bool AreAllZombiesDead(STH_HordeInstance inst)
    {
        int i;
        for (i = 0; i < inst.Infected.Count(); i++)
        {
            DayZInfected dz = inst.Infected.Get(i);
            if (dz && !dz.IsDamageDestroyed())
            {
                return false;
            }
        }
        return true;
    }

    void SpawnRewardChestAt(STH_HordeInstance inst)
    {
        if (!m_Config || !m_Config.Data || !m_Config.Data.Global)
        {
            return;
        }
        string chestClass = m_Config.Data.Global.ChestClass;
        if (!chestClass || chestClass.Length() == 0)
        {
            chestClass = "SeaChest";
        }
        // Spawn at the original location center on terrain height
        float px = inst.Location.Position.Get(0);
        float pz = inst.Location.Position.Get(2);
        float py = GetGame().SurfaceY(px, pz);
        vector cpos = Vector(px, py, pz);
        EntityAI e = EntityAI.Cast(GetGame().CreateObject(chestClass, cpos, false, true, true));
        ItemBase chest = ItemBase.Cast(e);
        if (!chest)
        {
            return;
        }
        // Optional: small white smoke above chest
        if (m_Config.Data.Global.EnableChestSmoke)
        {
            float oy = m_Config.Data.Global.ChestSmokeOffsetY;
            if (oy <= 0) oy = 0.6;
            vector spt = cpos;
            spt[1] = spt[1] + oy;
            array<Man> players = new array<Man>;
            GetGame().GetPlayers(players);
            int spi;
            for (spi = 0; spi < players.Count(); spi++)
            {
                PlayerBase pl = PlayerBase.Cast(players.Get(spi));
                if (!pl) continue;
                float dd = vector.Distance(pl.GetPosition(), spt);
                if (dd <= 150.0)
                {
                    Param2<int, vector> sdata3 = new Param2<int, vector>(2001, spt);
                    if (pl.GetIdentity())
                    {
                        GetGame().RPCSingleParam(pl, 465002, sdata3, true, pl.GetIdentity());
                    }
                    else
                    {
                        // SP/local fallback
                        GetGame().RPCSingleParam(pl, 465002, sdata3, true);
                    }
                }
            }
        }
            // Auto-despawn after configured seconds (0/neg = disabled)
            Param1<EntityAI> pChest = new Param1<EntityAI>(e);
            int chestSeconds = 600;
            if (m_Config && m_Config.Data && m_Config.Data.Global)
            {
                chestSeconds = m_Config.Data.Global.ChestDespawnSeconds;
            }
            if (chestSeconds > 0)
            {
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "DespawnChest", chestSeconds * 1000, false, pChest);
            }
        // Fill rewards
        if (m_Config.Data.Global.RewardItems)
        {
            array<int> eligible = new array<int>;
            int i;
            for (i = 0; i < m_Config.Data.Global.RewardItems.Count(); i++)
            {
                STH_RewardItem ri0 = m_Config.Data.Global.RewardItems.Get(i);
                if (!ri0 || !ri0.ClassName || ri0.ClassName.Length() == 0)
                {
                    continue;
                }
                float r0 = Math.RandomFloatInclusive(0.0, 1.0);
                if (r0 <= ri0.Chance)
                {
                    eligible.Insert(i);
                }
            }
            if (eligible.Count() > 0)
            {
                int pickMin = m_Config.Data.Global.RewardPickMin;
                int pickMax = m_Config.Data.Global.RewardPickMax;
                if (pickMin < 0) pickMin = 0;
                if (pickMax < pickMin) pickMax = pickMin;
                if (pickMin > eligible.Count()) pickMin = eligible.Count();
                if (pickMax > eligible.Count()) pickMax = eligible.Count();
                int pickCount = Math.RandomIntInclusive(pickMin, pickMax);
                // Shuffle eligible indices for random selection
                int sh; for (sh = 0; sh < eligible.Count(); sh++)
                {
                    int swapWith = Math.RandomInt(sh, eligible.Count());
                    if (swapWith >= eligible.Count()) swapWith = eligible.Count() - 1;
                    int tmp = eligible.Get(sh);
                    eligible.Set(sh, eligible.Get(swapWith));
                    eligible.Set(swapWith, tmp);
                }
                int used = 0;
                for (used = 0; used < pickCount && used < eligible.Count(); used++)
                {
                    int idx = eligible.Get(used);
                    STH_RewardItem ri = m_Config.Data.Global.RewardItems.Get(idx);
                    if (!ri) continue;
                    int qtyMin = ri.QuantityMin;
                    int qtyMax = ri.QuantityMax;
                    if (qtyMin <= 0) qtyMin = 1;
                    if (qtyMax < qtyMin) qtyMax = qtyMin;
                    int qty = Math.RandomIntInclusive(qtyMin, qtyMax);
                    ItemBase ib = ItemBase.Cast(chest.GetInventory().CreateInInventory(ri.ClassName));
                    if (ib)
                    {
                        Magazine mag = Magazine.Cast(ib);
                        if (mag)
                        {
                            mag.ServerSetAmmoCount(qty);
                        }
                        else
                        {
                            ib.SetQuantity(qty);
                        }
                    }
                }
            }
        }
    }

    void DespawnChest(Param1<EntityAI> p)
    {
        if (!p)
        {
            return;
        }
        EntityAI chest = p.param1;
        if (!chest)
        {
            return;
        }
        Print("[SagaTheHorde] Auto-despawning reward chest: " + chest.ToString());
        GetGame().ObjectDelete(chest);
    }

    void HandleTeleports(STH_HordeInstance inst)
    {
        if (!inst)
        {
            return;
        }

        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        if (players.Count() == 0)
        {
            return;
        }

        int i;
        for (i = 0; i < inst.Infected.Count(); i++)
        {
            DayZInfected dz = inst.Infected.Get(i);
            if (!dz)
            {
                continue;
            }
            // Skip if zombie is dead/destroyed
            if (dz.IsDamageDestroyed() || !dz.IsAlive())
            {
                continue;
            }

            // First: vertical melee assist for all infected (independent of teleport settings)
            // If a player is directly above within 0.5m horizontal distance and at least ~0.8m higher, apply a hit with cooldown
            bool assistedThisTick = false;
            vector zpos_assist = dz.GetPosition();
            int now_assist_any = GetGame().GetTime() / 1000;
            int lastAssistAny = 0;
            if (inst.LastAssistTime.Contains(dz))
            {
                lastAssistAny = inst.LastAssistTime.Get(dz);
            }
            int assistCooldownAny = 2; // seconds
            if ((now_assist_any - lastAssistAny) >= assistCooldownAny)
            {
                int pa;
                for (pa = 0; pa < players.Count(); pa++)
                {
                    PlayerBase pba = PlayerBase.Cast(players.Get(pa));
                    if (!pba)
                    {
                        continue;
                    }
                    // Ignore players in vehicles/helis
                    HumanCommandVehicle hcv_a = pba.GetCommand_Vehicle();
                    if (hcv_a)
                    {
                        continue;
                    }
                    vector ppos_a = pba.GetPosition();
                    float horiz_a = Math.Sqrt(Math.Pow(zpos_assist[0]-ppos_a[0], 2) + Math.Pow(zpos_assist[2]-ppos_a[2], 2));
                    if (horiz_a <= 0.5)
                    {
                        float vdiff = ppos_a[1] - zpos_assist[1]; // positive when player is above
                        if (vdiff >= 0.8)
                        {
                            // Apply direct damage as a simulated hit
                            pba.ProcessDirectDamage(DT_CUSTOM, dz, "", "MeleeDamage", zpos_assist, 12.0);
                            inst.LastAssistTime.Set(dz, now_assist_any);
                            assistedThisTick = true;
                            break;
                        }
                    }
                }
            }
            if (assistedThisTick)
            {
                continue; // do not attempt teleport in the same tick
            }

            // Determine the config entry for this infected's class
            STH_ZombieEntry zcfg = FindZombieEntry(inst.Location, dz.GetType());
            if (!zcfg || !zcfg.TeleportEnabled)
            {
                continue;
            }

            float bestDist = 1e9;
            PlayerBase bestTarget = NULL;
            int p;
            for (p = 0; p < players.Count(); p++)
            {
                PlayerBase pb = PlayerBase.Cast(players.Get(p));
                if (!pb)
                {
                    continue;
                }
                // Ignore players who are seated in vehicles/helis
                HumanCommandVehicle hcv_loop = pb.GetCommand_Vehicle();
                if (hcv_loop)
                {
                    continue;
                }
                    float d = vector.Distance(dz.GetPosition(), pb.GetPosition());
                    // Only consider players within detection radius
                    float detectR_loop = zcfg.TeleportDetectRadius;
                    if (detectR_loop <= 0)
                    {
                        detectR_loop = 20.0;
                    }
                    if (d > detectR_loop)
                    {
                        continue;
                    }
                if (d < bestDist)
                {
                    bestDist = d;
                    bestTarget = pb;
                }
            }

            if (bestTarget)
            {
                // Enforce detection radius: only teleport if player is within configured radius
                float detectR = zcfg.TeleportDetectRadius;
                if (detectR <= 0)
                {
                    detectR = 20.0; // safety default
                }
                vector zpos = dz.GetPosition();
                vector ppos2 = bestTarget.GetPosition();
                float distToPlayer = vector.Distance(zpos, ppos2);
                if (distToPlayer > detectR)
                {
                    continue; // too far: do not teleport
                }
                // Secondary assist (legacy): allow a hit across a larger vertical gap if very close horizontally
                // Conditions: player above by >= 1.0m, horizontal <= 0.5m, per-zombie 2s cooldown
                float vdiff2 = ppos2[1] - zpos[1];
                float horiz2 = Math.Sqrt(Math.Pow(zpos[0]-ppos2[0], 2) + Math.Pow(zpos[2]-ppos2[2], 2));
                if (vdiff2 >= 1.0 && horiz2 <= 0.5)
                {
                    int now_assist2 = GetGame().GetTime() / 1000;
                    int lastAssist2 = 0;
                    if (inst.LastAssistTime.Contains(dz))
                    {
                        lastAssist2 = inst.LastAssistTime.Get(dz);
                    }
                    int assistCooldown2 = 2;
                    if ((now_assist2 - lastAssist2) >= assistCooldown2)
                    {
                        bestTarget.ProcessDirectDamage(DT_CUSTOM, dz, "", "MeleeDamage", zpos, 12.0);
                        inst.LastAssistTime.Set(dz, now_assist2);
                        // Skip teleport in the same tick when assisted hit applied
                        continue;
                    }
                }
                // Skip teleport if already in combat based on per-class suppress radius
                float combatDetect = zcfg.TeleportCombatSuppressRadius;
                if (IsInCombat(dz, players, combatDetect))
                {
                    continue;
                }

                int now2 = GetGame().GetTime() / 1000;
                int last = 0;
                if (inst.LastTeleportTime.Contains(dz))
                {
                    last = inst.LastTeleportTime.Get(dz);
                }
                int cooldown = zcfg.TeleportCooldownSeconds;
                if (cooldown < 0)
                {
                    cooldown = 0;
                }
                if ((now2 - last) >= cooldown)
                {
                    // Determine player position and ensure we don't pile up: if a zombie already stands at the player, skip
                    vector ppos = bestTarget.GetPosition();
                    float occupyRadius = 1.5; // meters considered "at the player"
                    int x;
                    bool someoneAtPlayer = false;
                    for (x = 0; x < inst.Infected.Count(); x++)
                    {
                        DayZInfected other = inst.Infected.Get(x);
                        if (!other || other == dz)
                        {
                            continue;
                        }
                        float dpp = vector.Distance(other.GetPosition(), ppos);
                        if (dpp <= occupyRadius)
                        {
                            someoneAtPlayer = true;
                            break;
                        }
                    }
                    if (someoneAtPlayer)
                    {
                        continue; // avoid teleporting another one directly to the same player
                    }

                    // Find a safe ground position near the player (avoid interiors/blocked spots)
                    float minR = 0.6; // directly beside the player
                    float maxR = 1.6; // give a bit more space to find a valid point
                    vector tp;
                    bool found = FindSafePositionNear(ppos, minR, maxR, tp);
                    if (!found)
                    {
                        // Could not find a safe spot; skip this teleport
                        continue;
                    }
                    dz.SetPosition(tp);
                    // Ensure we don't proceed if the infected died exactly during teleport
                    if (dz.IsDamageDestroyed() || !dz.IsAlive())
                    {
                        continue;
                    }
                    // Optional teleport sound effect at the teleported zombie entity
                    if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.TeleportSoundEnabled)
                    {
                        string ss = "STH_Teleport_SoundSet";
                        // Send RPC to nearby players (<= 60m) to play the sound locally, attached to the teleported zombie entity
                        array<Man> allPlayers = new array<Man>;
                        GetGame().GetPlayers(allPlayers);
                        int rpci;
                        int recipients = 0;
                        for (rpci = 0; rpci < allPlayers.Count(); rpci++)
                        {
                            PlayerBase pl = PlayerBase.Cast(allPlayers.Get(rpci));
                            if (!pl)
                            {
                                continue;
                            }
                            float d_rpl = vector.Distance(pl.GetPosition(), ppos);
                            if (d_rpl <= 60.0)
                            {
                                // Include the teleported infected entity so clients can attach the sound to it
                                Param3<string, EntityAI, vector> pdata = new Param3<string, EntityAI, vector>(ss, dz, ppos);
                                // Prefer identity-targeted send; fallback to no-identity for local/SP contexts
                                if (pl.GetIdentity())
                                {
                                    GetGame().RPCSingleParam(pl, 465001, pdata, true, pl.GetIdentity());
                                }
                                else
                                {
                                    GetGame().RPCSingleParam(pl, 465001, pdata, true);
                                }
                                recipients = recipients + 1;
                            }
                        }
                        Print("[SagaTheHorde] Teleport sound RPC sent to " + ("" + recipients) + " player(s) within 60m.");
                    }
                    inst.LastTeleportTime.Set(dz, now2);
                }
            }
        }
    }

    STH_ZombieEntry FindZombieEntry(STH_HordeLocation loc, string typeName)
    {
        int i;
        for (i = 0; i < loc.ZombieClasses.Count(); i++)
        {
            STH_ZombieEntry z = loc.ZombieClasses.Get(i);
            if (z && z.ClassName == typeName)
            {
                return z;
            }
        }
        return NULL;
    }

    int SpawnHorde(STH_HordeInstance inst)
    {
        if (!inst)
        {
            return 0;
        }

        // Pick a random location from config
        if (!m_Config || !m_Config.Data || m_Config.Data.Hordes.Count() == 0)
        {
            Print("[SagaTheHorde] No horde locations found in config. Add entries under Hordes[] in $profile:\\SagaTheHorde\\config.json");
            return 0;
        }
        int index = Math.RandomInt(0, m_Config.Data.Hordes.Count());
        STH_HordeLocation picked = m_Config.Data.Hordes.Get(index);
        inst.Location = picked;

        if (!inst.Location || inst.Location.Position.Count() < 3)
        {
            Print("[SagaTheHorde] Selected location has invalid Position (need [x,y,z]). Skipping spawn.");
            return 0;
        }
        vector pos = Vector(inst.Location.Position.Get(0), inst.Location.Position.Get(1), inst.Location.Position.Get(2));
        if (inst.Location.Position.Count() >= 3)
        {
            float px = inst.Location.Position.Get(0);
            float pz = inst.Location.Position.Get(2);
            float py = GetGame().SurfaceY(px, pz);
            pos = Vector(px, py, pz);
        }

        int desired = Math.RandomIntInclusive(inst.Location.MinCount, inst.Location.MaxCount);
        int spawned = 0;
        int attempts = 0;

        // 1) Spawn all 'Always' classes first
        int iAlways;
        for (iAlways = 0; iAlways < inst.Location.ZombieClasses.Count(); iAlways++)
        {
            STH_ZombieEntry za = inst.Location.ZombieClasses.Get(iAlways);
            if (!za || !za.Always)
            {
                continue;
            }
            int countA = Math.RandomIntInclusive(za.CountMin, za.CountMax);
            int k;
            for (k = 0; k < countA && spawned < desired; k++)
            {
                vector offsetA = RandomPointInRadius(inst.Location.Radius);
                vector spawnPosA = pos + offsetA;
                // Project to terrain height
                spawnPosA[1] = GetGame().SurfaceY(spawnPosA[0], spawnPosA[2]);
                // Try to avoid spawning inside buildings/inaccessible interiors
                if (IsBlockedForInfected(spawnPosA))
                {
                    vector safeA;
                    if (!FindSafePositionNear(spawnPosA, 1.0, 4.0, safeA))
                    {
                        continue; // give up this attempt and try next
                    }
                    spawnPosA = safeA;
                }
                EntityAI ea = EntityAI.Cast(GetGame().CreateObject(za.ClassName, spawnPosA, false, true, true));
                ZombieBase dza = ZombieBase.Cast(ea);
                if (dza)
                {
                    inst.Infected.Insert(dza);
                    // Register toughness multiplier (effective HP scale via damage healing)
                    float effMulA = 1.0 + (za.HealthStrengthBonus);
                    if (effMulA > 1.0)
                    {
                        float maxA = dza.GetMaxHealth("","health");
                        if (maxA > 0)
                        {
                            // Try to raise current health proportionally without exceeding artificial cap
                            float newTarget = maxA * effMulA;
                            // Engine usually clamps at maxA, so this may have no effect; kept for potential modded subclasses.
                            dza.SetHealth("","health", newTarget);
                        }
                    }
                    AssignLoot(dza, za);
                    spawned = spawned + 1;
                }
            }
        }

        // 2) Fill remaining with random chance-based classes
        while (spawned < desired && attempts < (desired * 10))
        {
            attempts = attempts + 1;
            STH_ZombieEntry zrnd = PickZombie(inst.Location);
            if (!zrnd)
            {
                continue;
            }
            int countR = Math.RandomIntInclusive(zrnd.CountMin, zrnd.CountMax);
            int j;
            for (j = 0; j < countR && spawned < desired; j++)
            {
                vector offset = RandomPointInRadius(inst.Location.Radius);
                vector spawnPos = pos + offset;
                // Project to terrain height
                spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
                // Try to avoid spawning inside buildings/inaccessible interiors
                if (IsBlockedForInfected(spawnPos))
                {
                    vector safe;
                    if (!FindSafePositionNear(spawnPos, 1.0, 4.0, safe))
                    {
                        continue; // skip this one
                    }
                    spawnPos = safe;
                }
                EntityAI e;
                e = EntityAI.Cast(GetGame().CreateObject(zrnd.ClassName, spawnPos, false, true, true));
                ZombieBase dz = ZombieBase.Cast(e);
                if (dz)
                {
                    inst.Infected.Insert(dz);
                    float effMul = 1.0 + (zrnd.HealthStrengthBonus);
                    if (effMul > 1.0)
                    {
                        float maxR = dz.GetMaxHealth("","health");
                        if (maxR > 0)
                        {
                            float newTarget2 = maxR * effMul;
                            dz.SetHealth("","health", newTarget2);
                        }
                    }
                    AssignLoot(dz, zrnd);
                    spawned = spawned + 1;
                }
            }
        }

        if (spawned > 0)
        {
            inst.OriginalCount = spawned;
            // Optional spawn smoke effect at the configured location (simple toggle)
            if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.EnableSmoke)
            {
                // Notify nearby players to play a fixed smoke particle at the spawn position
                array<Man> players = new array<Man>;
                GetGame().GetPlayers(players);
                // Use configured position height (no terrain adjust) for smoke origin
                vector spos2 = Vector(inst.Location.Position.Get(0), inst.Location.Position.Get(1), inst.Location.Position.Get(2));
                // NOTE: Früher wurde hier bereits 10m abgesenkt und später auf dem Client NOCHMAL 10m -> Partikel landete ~20m unter Terrain.
                // Client senkt um 10m ab; hier keine Bodenhöhe berechnen.
                int pi;
                for (pi = 0; pi < players.Count(); pi++)
                {
                    PlayerBase plx2 = PlayerBase.Cast(players.Get(pi));
                    if (!plx2) continue;
                    if (!plx2.GetIdentity()) continue;
                    float d2 = vector.Distance(plx2.GetPosition(), spos2);
                    if (d2 <= 150.0)
                    {
                        // code 1001 = smoke_heli_wreck_01
                        Param2<int, vector> sdata2 = new Param2<int, vector>(1001, spos2);
                        GetGame().RPCSingleParam(plx2, 465002, sdata2, true, plx2.GetIdentity());
                    }
                }
                // Track active smoke with TTL (e.g., 300s)
                int now = GetGame().GetTime() / 1000;
                m_ActiveSmokePositions.Insert(spos2);
                m_ActiveSmokeExpire.Insert(now + 300);
                // Remember smoke for this horde instance
                inst.SmokePosition = spos2;
                inst.SmokeActive = true;
            }
            NotifyHordeSpawn(inst.Location, spawned);
            inst.SpawnedAt = GetGame().GetTime() / 1000;
        }
        return spawned;
    }

    void RebroadcastActiveSmokes()
    {
        int now = GetGame().GetTime() / 1000;
        int i;
        // Clean expired
        for (i = m_ActiveSmokePositions.Count() - 1; i >= 0; i--)
        {
            if (m_ActiveSmokeExpire.Get(i) <= now)
            {
                m_ActiveSmokePositions.Remove(i);
                m_ActiveSmokeExpire.Remove(i);
            }
        }
        if (m_ActiveSmokePositions.Count() == 0)
        {
            return;
        }
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        int p;
        for (p = 0; p < players.Count(); p++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(p));
            if (!pb || !pb.GetIdentity()) continue;
            int s;
            for (s = 0; s < m_ActiveSmokePositions.Count(); s++)
            {
                vector sp = m_ActiveSmokePositions.Get(s);
                float d = vector.Distance(pb.GetPosition(), sp);
                if (d <= 150.0)
                {
                    Param2<int, vector> pdata = new Param2<int, vector>(1001, sp);
                    GetGame().RPCSingleParam(pb, 465002, pdata, true, pb.GetIdentity());
                }
            }
        }
    }

    void ShowHordeProgress(STH_HordeInstance inst)
    {
        if (!inst || inst.SpawnedAt == 0)
        {
            return;
        }
        // Determine remaining alive
        int alive = 0;
        int total = inst.OriginalCount;
        int i;
        for (i = 0; i < inst.Infected.Count(); i++)
        {
            DayZInfected d = inst.Infected.Get(i);
            if (d && !d.IsDamageDestroyed())
            {
                alive = alive + 1;
            }
        }
        int dead = total - alive;
        if (alive == 0 && inst.SmokeActive)
        {
            StopSmokeForInstance(inst);
        }
        if (total <= 0)
        {
            return;
        }
        // Notify players inside the progress display radius
        float radius = 50.0;
        // Per-location override takes precedence when set (> 0)
        if (inst.Location && inst.Location.ProgressPlayerRadius > 0)
        {
            radius = inst.Location.ProgressPlayerRadius;
        }
        else if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.HordeProgressPlayerRadius > 0)
        {
            radius = m_Config.Data.Global.HordeProgressPlayerRadius;
        }
        vector center = Vector(inst.Location.Position.Get(0), inst.Location.Position.Get(1), inst.Location.Position.Get(2));
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        for (i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (!pb) continue;
            float dist = vector.Distance(pb.GetPosition(), center);
            if (dist <= radius)
            {
                int lastDeadShown = -1;
                if (inst.ProgressLastShownDead && inst.ProgressLastShownDead.Contains(pb))
                {
                    lastDeadShown = inst.ProgressLastShownDead.Get(pb);
                }
                // Only update when the progress (dead count) changes to avoid constant rewrites
                if (lastDeadShown != dead)
                {
                    string msg = "Killed Creatures " + dead + "/" + total;
#ifdef EXPANSIONMOD
                    if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.UseExpansionNotifications)
                    {
                        if (pb.GetIdentity())
                        {
                            ExpansionNotification("SagaTheHorde", msg).Create(pb.GetIdentity());
                        }
                        else
                        {
                            ExpansionNotification("SagaTheHorde", msg).Create();
                        }
                    }
                    else
                    {
                        pb.MessageStatus(msg);
                    }
#else
                    pb.MessageStatus(msg);
#endif
                    inst.ProgressLastShownDead.Set(pb, dead);
                }
            }
            else
            {
                // Player left the radius; clear last shown so message will reappear upon re-enter
                if (inst.ProgressLastShownDead && inst.ProgressLastShownDead.Contains(pb))
                {
                    inst.ProgressLastShownDead.Remove(pb);
                }
            }
        }
    }

    void StopSmokeForInstance(STH_HordeInstance inst)
    {
        if (!inst || !inst.SmokeActive)
            return;
        vector sp = inst.SmokePosition;
        // Remove from active smoke lists (all entries matching position)
        int i;
        for (i = m_ActiveSmokePositions.Count() - 1; i >= 0; i--)
        {
            vector v = m_ActiveSmokePositions.Get(i);
            if (v[0] == sp[0] && v[1] == sp[1] && v[2] == sp[2])
            {
                m_ActiveSmokePositions.Remove(i);
                m_ActiveSmokeExpire.Remove(i);
            }
        }
        // Broadcast a stop signal (code 1002) to nearby players to stop the particle client-side (if handled)
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        for (i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (!pb || !pb.GetIdentity()) continue;
            float d = vector.Distance(pb.GetPosition(), sp);
            if (d <= 200.0)
            {
                Param2<int, vector> pdata = new Param2<int, vector>(1002, sp);
                GetGame().RPCSingleParam(pb, 465002, pdata, true, pb.GetIdentity());
            }
        }
        inst.SmokeActive = false;
    }

    // Toughness scaling: heal back a portion of recent damage to simulate higher effective HP
    void AdjustToughness(STH_HordeInstance inst)
    {
        if (!inst || !inst.Location)
        {
            return;
        }
        int i;
        for (i = 0; i < inst.Infected.Count(); i++)
        {
            DayZInfected dz = inst.Infected.Get(i);
            if (!dz || dz.IsDamageDestroyed() || !dz.IsAlive())
            {
                continue;
            }
            STH_ZombieEntry zcfg = FindZombieEntry(inst.Location, dz.GetType());
            if (!zcfg)
            {
                continue;
            }
            float bonus = zcfg.HealthStrengthBonus;
            if (bonus <= 0.0)
            {
                // Track baseline but no scaling
                float cur0 = dz.GetHealth("","health");
                inst.LastObservedHealth.Set(dz, cur0);
                continue;
            }
            float cur = dz.GetHealth("","health");
            float last = 0.0;
            bool had = false;
            if (inst.LastObservedHealth.Contains(dz))
            {
                last = inst.LastObservedHealth.Get(dz);
                had = true;
            }
            if (!had)
            {
                inst.LastObservedHealth.Set(dz, cur);
                continue;
            }
            if (cur < last)
            {
                float m = 1.0 + bonus; // desired effective HP multiplier
                if (m < 1.0) m = 1.0;
                float delta = last - cur;
                float healBack = delta * (1.0 - (1.0 / m));
                if (healBack > 0.0)
                {
                    float newVal = cur + healBack;
                    // Clamp to last (do not overheal past pre-hit value)
                    if (newVal > last) newVal = last;
                    dz.SetHealth("","health", newVal);
                    cur = newVal;
                }
            }
            inst.LastObservedHealth.Set(dz, cur);
        }
    }

    STH_ZombieEntry PickZombie(STH_HordeLocation loc)
    {
        int i;
        // Always
        for (i = 0; i < loc.ZombieClasses.Count(); i++)
        {
            STH_ZombieEntry z = loc.ZombieClasses.Get(i);
            if (z && z.Always)
            {
                return z;
            }
        }

        // Random pick based on chance
        ref array<ref STH_ZombieEntry> candidates = new array<ref STH_ZombieEntry>;
        for (i = 0; i < loc.ZombieClasses.Count(); i++)
        {
            STH_ZombieEntry zr = loc.ZombieClasses.Get(i);
            if (zr && !zr.Always)
            {
                float roll = Math.RandomFloatInclusive(0.0, 1.0);
                if (roll <= zr.Chance)
                {
                    candidates.Insert(zr);
                }
            }
        }
        if (candidates.Count() > 0)
        {
            int idx = Math.RandomInt(0, candidates.Count());
            return candidates.Get(idx);
        }
        return NULL;
    }

    vector RandomPointInRadius(float radius)
    {
        float angle = Math.RandomFloatInclusive(0.0, Math.PI * 2.0);
        float r = Math.RandomFloatInclusive(0.0, 1.0) * radius;
        float x = Math.Cos(angle) * r;
        float z = Math.Sin(angle) * r;
        return Vector(x, 0, z);
    }

    vector RandomPointInAnnulus(float minRadius, float maxRadius)
    {
        float angle = Math.RandomFloatInclusive(0.0, Math.PI * 2.0);
        float t = Math.RandomFloatInclusive(0.0, 1.0);
        float r = minRadius + t * (maxRadius - minRadius);
        float x = Math.Cos(angle) * r;
        float z = Math.Sin(angle) * r;
        return Vector(x, 0, z);
    }

    // Simple but robust obstruction check to avoid placing infected under/inside buildings or on structures.
    bool IsBlockedForInfected(vector pos, float radius = 1.5)
    {
        // 1) Reject water
        if (GetGame().SurfaceIsSea(pos[0], pos[2]))
        {
            return true;
        }

        // 2) If point is significantly above terrain, it's likely on a structure (floor/roof). Avoid.
        float ty = GetGame().SurfaceY(pos[0], pos[2]);
        if ((pos[1] - ty) > 0.4)
        {
            return true;
        }

        // 3) Quick proximity check for obvious blockers (building proxies, fences)
        array<Object> objs = new array<Object>;
        array<CargoBase> cargos = new array<CargoBase>;
        GetGame().GetObjectsAtPosition3D(pos, radius, objs, cargos);
        int i;
        for (i = 0; i < objs.Count(); i++)
        {
            Object o = objs.Get(i);
            if (!o)
            {
                continue;
            }
            if (BuildingBase.Cast(o) || House.Cast(o) || Fence.Cast(o))
            {
                return true;
            }
        }

        // 4) Raycast upwards to detect ceilings/floors above (avoid under-building placements)
        vector fromUp = pos + Vector(0, 0.1, 0);
        vector toUp = pos + Vector(0, 6.0, 0);
        vector hitPosU, hitNormU;
        int compU;
        Object hitObjU;
        if (DayZPhysics.RaycastRV(fromUp, toUp, hitPosU, hitNormU, compU, hitObjU, NULL, NULL, false, false, ObjIntersectView))
        {
            if (hitObjU && (BuildingBase.Cast(hitObjU) || House.Cast(hitObjU)))
            {
                return true; // under a building ceiling/floor
            }
        }

        return false;
    }

    // Find a safe, unobstructed ground position around a center point.
    // Returns true and writes outPos when found; otherwise false.
    bool FindSafePositionNear(vector center, float minRadius, float maxRadius, out vector outPos)
    {
        // Try a few rings with several angles each
        int rings = 3;
        int steps = 16;
        int ri;
        for (ri = 0; ri < rings; ri++)
        {
            float r0 = Math.Lerp(minRadius, maxRadius, (ri + 0.5) / rings);
            int si;
            for (si = 0; si < steps; si++)
            {
                float a = ((Math.PI * 2.0) / steps) * si;
                float sx = center[0] + Math.Cos(a) * r0;
                float sz = center[2] + Math.Sin(a) * r0;
                float sy = GetGame().SurfaceY(sx, sz);
                vector cand = Vector(sx, sy, sz);
                if (!IsBlockedForInfected(cand))
                {
                    outPos = cand;
                    return true;
                }
            }
        }
        // Final random attempts in the annulus
        int tries = 12;
        int t;
        for (t = 0; t < tries; t++)
        {
            vector off = RandomPointInAnnulus(minRadius, maxRadius);
            vector cand2 = center + off;
            cand2[1] = GetGame().SurfaceY(cand2[0], cand2[2]);
            if (!IsBlockedForInfected(cand2))
            {
                outPos = cand2;
                return true;
            }
        }
        return false;
    }

    bool IsInCombat(DayZInfected dz, array<Man> players, float detectRadius)
    {
        if (!dz)
        {
            return false;
        }
        int i;
        for (i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (!pb)
            {
                continue;
            }
            HumanCommandVehicle hcv = pb.GetCommand_Vehicle();
            if (hcv)
            {
                continue;
            }
            vector ppos = pb.GetPosition();
            vector izpos = dz.GetPosition();
            float d = vector.Distance(izpos, ppos);
            float dy = izpos[1] - ppos[1];
            if (dy < 0)
            {
                dy = -dy;
            }
            // If vertical difference is large (e.g., player stands on top of a vehicle or structure), do not treat as combat
            if (d <= detectRadius && dy < 1.6)
            {
                return true;
            }
        }
        return false;
    }

    void AssignLoot(DayZInfected infected, STH_ZombieEntry z)
    {
        if (!infected || !z)
        {
            return;
        }
        int i;
        for (i = 0; i < z.Loot.Count(); i++)
        {
            string itemType = z.Loot.Get(i);
            if (itemType && itemType.Length() > 0)
            {
                infected.GetInventory().CreateInInventory(itemType);
            }
        }
    }

    void NotifyHordeSpawn(STH_HordeLocation loc, int count)
    {
        string msg = BuildMessage(true, loc.Name, count);

#ifdef EXPANSIONMOD
        if (m_Config && m_Config.Data && m_Config.Data.Global && m_Config.Data.Global.UseExpansionNotifications)
        {
            ExpansionNotification("SagaTheHorde", msg).Create();
            return;
        }
#endif
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        int i;
        for (i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb)
            {
                pb.MessageStatus(msg);
            }
        }
    }
};

int DespawnHorde(STH_HordeInstance inst)
{
    if (!inst)
    {
        return 0;
    }
    int removed = 0;
    int i;
    for (i = inst.Infected.Count() - 1; i >= 0; i--)
    {
        DayZInfected dz = inst.Infected.Get(i);
        if (dz)
        {
            GetGame().ObjectDelete(dz);
            removed = removed + 1;
        }
        inst.Infected.Remove(i);
    }
    return removed;
}

void NotifyHordeDespawn(STH_HordeLocation loc, int count)
{
    string msg = BuildMessage(false, loc.Name, count);

#ifdef EXPANSIONMOD
    if (GetDayZGame() && GetDayZGame().IsMultiplayer())
    {
        ExpansionNotification("SagaTheHorde", msg).Create();
        return;
    }
#endif
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);
    int i;
    for (i = 0; i < players.Count(); i++)
    {
        PlayerBase pb = PlayerBase.Cast(players.Get(i));
        if (pb)
        {
            pb.MessageStatus(msg);
        }
    }
}

string BuildMessage(bool isSpawn, string locationName, int count)
{
    string tmpl;
    if (isSpawn)
    {
        if (GetSTHSpawnTemplate(tmpl))
        {
            // ok
        }
        else
        {
            tmpl = "A zombie horde has spawned near {location} ({count})";
        }
    }
    else
    {
        if (GetSTHDespawnTemplate(tmpl))
        {
            // ok
        }
        else
        {
            tmpl = "The zombie horde near {location} has disappeared ({count})";
        }
    }

    tmpl.Replace("{location}", locationName);
    tmpl.Replace("{count}", ("" + count));
    return tmpl;
}

bool GetSTHSpawnTemplate(out string outTmpl)
{
    STH_Config c = new STH_Config();
    c.Init();
    if (c && c.Data && c.Data.Global && c.Data.Global.SpawnMessageTemplate && c.Data.Global.SpawnMessageTemplate.Length() > 0)
    {
        outTmpl = c.Data.Global.SpawnMessageTemplate;
        return true;
    }
    return false;
}

bool GetSTHDespawnTemplate(out string outTmpl)
{
    STH_Config c = new STH_Config();
    c.Init();
    if (c && c.Data && c.Data.Global && c.Data.Global.DespawnMessageTemplate && c.Data.Global.DespawnMessageTemplate.Length() > 0)
    {
        outTmpl = c.Data.Global.DespawnMessageTemplate;
        return true;
    }
    return false;
}

