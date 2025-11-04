modded class MissionServer
{
    ref STH_HordeManager m_SagaTheHorde_Manager;

    void MissionServer()
    {
        m_SagaTheHorde_Manager = new STH_HordeManager();
    }

    override void OnInit()
    {
        super.OnInit();
        if (GetGame().IsServer())
        {
            m_SagaTheHorde_Manager.Init();
        }
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        if (GetGame().IsServer() && m_SagaTheHorde_Manager)
        {
            m_SagaTheHorde_Manager.OnUpdate(timeslice);
        }
    }
};
