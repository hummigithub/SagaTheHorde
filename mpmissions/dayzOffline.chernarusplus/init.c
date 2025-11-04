void main()
{
	// Initialize mission
	Hive ce = CreateHive();
	if (ce)
		ce.InitOffline();

	// Initialize weather
	Weather weather = g_Game.GetWeather();
	weather.MissionWeather(false);
	weather.GetOvercast().Set(Math.RandomFloatInclusive(0.4, 0.7), 1, 0);
	weather.GetRain().Set(0, 0, 1);
	weather.GetFog().Set(Math.RandomFloatInclusive(0.05, 0.1), 1, 0);

	// Initialize economy
	GetCEApi().ExportProxyData(Vector(7500, 0, 7500), 15000);

	// Initialize horde system
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 10000, true);
}

// Horde spawning function
void SpawnHordes()
{
	// Get all players
	array<Man> players = new array<Man>;
	GetGame().GetPlayers(players);
	
	if (players.Count() == 0)
		return;
	
	// Spawn horde near random player
	int playerIndex = Math.RandomInt(0, players.Count());
	Man player = players.Get(playerIndex);
	
	if (player)
	{
		vector playerPos = player.GetPosition();
		
		// Spawn multiple zombies in a horde (5-15 zombies)
		int hordeSize = Math.RandomInt(5, 16);
		
		for (int i = 0; i < hordeSize; i++)
		{
			// Random position around player (50-150m away)
			float angle = Math.RandomFloat(0, 360);
			float distance = Math.RandomFloat(50, 150);
			
			vector spawnPos = playerPos;
			spawnPos[0] = spawnPos[0] + (Math.Cos(angle) * distance);
			spawnPos[2] = spawnPos[2] + (Math.Sin(angle) * distance);
			
			// Get ground position
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
			
			// Random zombie type
			string zombieType;
			int zombieTypeRoll = Math.RandomInt(0, 10);
			
			switch (zombieTypeRoll)
			{
				case 0:
					zombieType = "ZmbM_PatrolNormal_Autumn";
					break;
				case 1:
					zombieType = "ZmbM_PatrolNormal_Flat";
					break;
				case 2:
					zombieType = "ZmbM_PatrolNormal_Summer";
					break;
				case 3:
					zombieType = "ZmbF_PatrolNormal_Autumn";
					break;
				case 4:
					zombieType = "ZmbF_PatrolNormal_Beige";
					break;
				case 5:
					zombieType = "ZmbM_HunterOld_Autumn";
					break;
				case 6:
					zombieType = "ZmbM_HunterOld_Spring";
					break;
				case 7:
					zombieType = "ZmbF_CitizenANormal_Blue";
					break;
				case 8:
					zombieType = "ZmbF_CitizenANormal_Beige";
					break;
				default:
					zombieType = "ZmbM_CitizenASkinny_Blue";
					break;
			}
			
			// Spawn zombie
			EntityAI zombie = EntityAI.Cast(GetGame().CreateObject(zombieType, spawnPos, false, true));
			
			if (zombie)
			{
				// Make zombie aggressive
				zombie.SetAllowDamage(true);
			}
		}
		
		Print("[SagaTheHorde] Spawned horde of " + hordeSize + " zombies near player at " + playerPos);
	}
}

// Mission class
class CustomMission: MissionServer
{
	void SetRandomHealth(EntityAI itemEnt)
	{
		if (itemEnt)
		{
			float rndHlt = Math.RandomFloat(0.45, 0.65);
			itemEnt.SetHealth01("", "", rndHlt);
		}
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
		Class.CastTo(m_player, playerEnt);

		GetGame().SelectPlayer(identity, m_player);

		return m_player;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		EntityAI itemClothes;
		EntityAI itemEnt;
		ItemBase itemBs;
		float rand;

		// Clothing
		itemClothes = player.FindAttachmentBySlotName("Body");
		if (itemClothes)
		{
			SetRandomHealth(itemClothes);
			
			itemEnt = itemClothes.GetInventory().CreateInInventory("Rag");
			if (Class.CastTo(itemBs, itemEnt))
				itemBs.SetQuantity(6);

			SetRandomHealth(itemEnt);
		}

		itemClothes = player.FindAttachmentBySlotName("Legs");
		if (itemClothes)
			SetRandomHealth(itemClothes);

		itemClothes = player.FindAttachmentBySlotName("Feet");
		if (itemClothes)
			SetRandomHealth(itemClothes);
	}
}

Mission CreateCustomMission(string path)
{
	return new CustomMission();
}
