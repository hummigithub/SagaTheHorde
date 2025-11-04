static const int STH_RPC_TP_SND = 465001;
static const int STH_RPC_SPAWN_SMOKE = 465002;

modded class PlayerBase
{
    ref map<string, Particle> m_STH_Smokes;

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == STH_RPC_TP_SND)
        {
            Param3<string, EntityAI, vector> data;
            if (!ctx.Read(data))
            {
                return;
            }
            string soundSet = data.param1;
            EntityAI targetObj = data.param2;
            vector pos = data.param3;
            Print("[SagaTheHorde] Client received teleport sound RPC: " + soundSet + " on obj=" + targetObj + " at " + pos);
            if (soundSet && soundSet.Length() > 0)
            {
                if (targetObj)
                {
                    DayZInfected zDeadCheck = DayZInfected.Cast(targetObj);
                    if (zDeadCheck && (zDeadCheck.IsDamageDestroyed() || !zDeadCheck.IsAlive()))
                    {
                        EffectSound effDead = SEffectManager.PlaySound(soundSet, pos, 2.0, 1.0);
                        if (effDead)
                        {
                            effDead.SetSoundVolume(2.0);
                            effDead.SetSoundAutodestroy(true);
                        }
                    }
                    else
                    {
                        PlayTpSoundOnObject(soundSet, targetObj);
                    }
                }
                else
                {
                    EffectSound eff = SEffectManager.PlaySound(soundSet, pos, 2.0, 1.0);
                    if (eff)
                    {
                        eff.SetSoundVolume(2.0);
                        eff.SetSoundAutodestroy(true);
                    }
                    else
                    {
                        EffectSound eff2;
                        this.PlaySoundSet(eff2, soundSet, 0.0, 0.0);
                        if (eff2)
                        {
                            eff2.SetSoundVolume(2.0);
                            eff2.SetSoundAutodestroy(true);
                        }
                    }
                }
            }
        }
        else if (rpc_type == STH_RPC_SPAWN_SMOKE)
        {
            Param2<int, vector> pdata;
            if (!ctx.Read(pdata))
            {
                return;
            }
            int code = pdata.param1;
            vector ppos = pdata.param2;
            string pname = "";
            if (code == 1001)
            {
                pname = "smoke_heli_wreck_01";
            }
            else if (code == 1002)
            {
                if (!m_STH_Smokes)
                {
                    return;
                }
                string key2 = ppos.ToString();
                if (m_STH_Smokes.Contains(key2))
                {
                    Particle pstop = m_STH_Smokes.Get(key2);
                    if (pstop)
                    {
                        pstop.Stop();
                    }
                    m_STH_Smokes.Remove(key2);
                }
                return;
            }
            else if (code == 2001)
            {
                pname = "fire_small_roadflare_red_01";
            }
            if (pname.Length() > 0)
            {
                int pid = ParticleList.GetParticleIDByName(pname);
                if (ParticleList.IsValidId(pid))
                {
                    vector adj = ppos;
                    if (code == 1001)
                    {
                        adj[1] = adj[1] - 10.0;
                    }
                    Print("[SagaTheHorde] Spawning particle '" + pname + "' (id=" + pid + ") at " + adj + " (code=" + code + ")");
                    Particle p = Particle.PlayInWorld(pid, adj);
                    if (p)
                    {
                        if (!m_STH_Smokes)
                        {
                            m_STH_Smokes = new map<string, Particle>;
                        }
                        string key = ppos.ToString();
                        m_STH_Smokes.Set(key, p);
                    }
                }
                else
                {
                    Print("[SagaTheHorde] Could not resolve particle name '" + pname + "' to a valid id.");
                }
            }
        }
    }

    void PlayTpSoundOnObject(string soundSet, EntityAI target)
    {
        if (!target)
        {
            return;
        }
        EffectSound eff = SEffectManager.PlaySoundOnObject(soundSet, target);
        if (eff)
        {
            eff.SetSoundVolume(2.0);
            eff.SetSoundAutodestroy(true);
        }
        else
        {
            vector pos = target.GetPosition();
            EffectSound eff2 = SEffectManager.PlaySound(soundSet, pos, 2.0, 1.0);
            if (eff2)
            {
                eff2.SetSoundVolume(2.0);
                eff2.SetSoundAutodestroy(true);
            }
        }
    }
}
