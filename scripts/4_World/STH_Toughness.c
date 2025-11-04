class STH_ToughnessRegistry
{
    static ref map<EntityAI, float> m_Mults;

    static void Ensure()
    {
        if (!m_Mults)
        {
            m_Mults = new map<EntityAI, float>();
        }
    }

    static void SetMultiplier(EntityAI ent, float mult)
    {
        if (!ent) return;
        Ensure();
        if (mult <= 1.0)
        {
            if (m_Mults.Contains(ent)) m_Mults.Remove(ent);
            return;
        }
        m_Mults.Set(ent, mult);
    }

    static float GetMultiplier(EntityAI ent)
    {
        if (!ent) return 1.0;
        if (!m_Mults) return 1.0;
    float m;
        if (m_Mults.Find(ent, m))
        {
            return m;
        }
        return 1.0;
    }

    static void ClearDead()
    {
        if (!m_Mults) return;
        array<EntityAI> toRemove = new array<EntityAI>();
        int i;
        for (i = 0; i < m_Mults.Count(); i++)
        {
            EntityAI keyEnt = m_Mults.GetKey(i);
            ZombieBase zb = ZombieBase.Cast(keyEnt);
            if (!zb || zb.IsDamageDestroyed())
            {
                toRemove.Insert(keyEnt);
            }
        }
        for (i = 0; i < toRemove.Count(); i++)
        {
            m_Mults.Remove(toRemove.Get(i));
        }
    }
}
