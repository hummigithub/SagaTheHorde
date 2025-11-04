# Vanilla DayZ init.c Backup

This is a minimal vanilla DayZ init.c file for reference.
If you need to revert or merge with other mods, use this as a starting point.

## Vanilla init.c

```cpp
void main()
{
	// Initialize Hive
	Hive ce = CreateHive();
	if (ce)
		ce.InitOffline();

	// Initialize Weather
	Weather weather = g_Game.GetWeather();
	weather.MissionWeather(false);
	weather.GetOvercast().Set(Math.RandomFloatInclusive(0.4, 0.7), 1, 0);
	weather.GetRain().Set(0, 0, 1);
	weather.GetFog().Set(Math.RandomFloatInclusive(0.05, 0.1), 1, 0);

	// Initialize Economy
	GetCEApi().ExportProxyData(Vector(7500, 0, 7500), 15000);
}

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
```

## Merging with SagaTheHorde

To add SagaTheHorde to an existing init.c:

1. Keep your existing `main()` function
2. Add this line before the closing brace of `main()`:
   ```cpp
   GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 600000, true);
   ```
3. Add the entire `SpawnHordes()` function from SagaTheHorde's init.c
4. If you have a custom mission class, keep it; otherwise use SagaTheHorde's

## Merging with Other Mods

If another mod also modifies init.c:
1. Keep both `main()` function contents
2. Merge any custom mission classes
3. Add both mods' functions
4. Test thoroughly

## Backup Your Files

Always backup your original init.c before installing any mod:
```bash
cp init.c init.c.backup
```
