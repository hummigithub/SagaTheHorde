# SagaTheHorde Configuration Examples

This file contains various configuration examples for customizing the horde mod.

## Example 1: Intense Survival (Large, Frequent Hordes)
```cpp
// In init.c, modify these lines:

// Spawn every 5 minutes
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 300000, true);

// 10-25 zombies per horde
int hordeSize = Math.RandomInt(10, 26);

// Spawn closer to players (30-100m)
float distance = Math.RandomFloat(30, 100);
```

## Example 2: Casual Mode (Small, Rare Hordes)
```cpp
// In init.c, modify these lines:

// Spawn every 15 minutes
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 900000, true);

// 3-7 zombies per horde
int hordeSize = Math.RandomInt(3, 8);

// Spawn farther from players (100-200m)
float distance = Math.RandomFloat(100, 200);
```

## Example 3: Apocalypse Mode (Massive Hordes)
```cpp
// In init.c, modify these lines:

// Spawn every 3 minutes
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 180000, true);

// 20-40 zombies per horde
int hordeSize = Math.RandomInt(20, 41);

// Spawn at varied distances (40-120m)
float distance = Math.RandomFloat(40, 120);
```

## Example 4: Balanced Experience (Default)
```cpp
// In init.c, modify these lines:

// Spawn every 10 seconds (for testing) or 600000 (10 minutes for production)
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 600000, true);

// 5-15 zombies per horde
int hordeSize = Math.RandomInt(5, 16);

// Spawn at safe distance (50-150m)
float distance = Math.RandomFloat(50, 150);
```

## Tips for Server Admins

### Performance Considerations
- Monitor server performance when adjusting horde size
- Larger hordes require more server resources
- More frequent spawns increase server load
- Consider your server's player count and hardware

### Balancing Guidelines
- **PvE Servers**: Use larger, more frequent hordes for challenge
- **PvP Servers**: Use moderate hordes to maintain PvP focus
- **Roleplay Servers**: Use rare, large hordes for special events
- **Hardcore Servers**: Use frequent, large hordes at close range

### Testing Recommendations
1. Start with default settings
2. Test with typical player count
3. Monitor server performance
4. Adjust one parameter at a time
5. Get player feedback
6. Fine-tune based on experience
