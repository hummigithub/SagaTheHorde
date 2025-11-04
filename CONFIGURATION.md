# SagaTheHorde Configuration Examples

This file contains various configuration examples for customizing the horde mod.

**Note:** All configuration is done by editing the constants at the top of `init.c` (lines 1-16). This makes customization simple and clear.

## Configuration Constants

```cpp
// Spawn interval in milliseconds (600000 = 10 minutes)
const int HORDE_SPAWN_INTERVAL = 600000;

// Horde size (min and max zombies per horde)
const int HORDE_SIZE_MIN = 5;
const int HORDE_SIZE_MAX = 16;

// Spawn distance from players in meters
const float SPAWN_DISTANCE_MIN = 50.0;
const float SPAWN_DISTANCE_MAX = 150.0;
```

## Example 1: Intense Survival (Large, Frequent Hordes)
```cpp
const int HORDE_SPAWN_INTERVAL = 300000;  // 5 minutes
const int HORDE_SIZE_MIN = 10;
const int HORDE_SIZE_MAX = 26;
const float SPAWN_DISTANCE_MIN = 30.0;
const float SPAWN_DISTANCE_MAX = 100.0;
```

## Example 2: Casual Mode (Small, Rare Hordes)
```cpp
const int HORDE_SPAWN_INTERVAL = 900000;  // 15 minutes
const int HORDE_SIZE_MIN = 3;
const int HORDE_SIZE_MAX = 8;
const float SPAWN_DISTANCE_MIN = 100.0;
const float SPAWN_DISTANCE_MAX = 200.0;
```

## Example 3: Apocalypse Mode (Massive Hordes)
```cpp
const int HORDE_SPAWN_INTERVAL = 180000;  // 3 minutes
const int HORDE_SIZE_MIN = 20;
const int HORDE_SIZE_MAX = 41;
const float SPAWN_DISTANCE_MIN = 40.0;
const float SPAWN_DISTANCE_MAX = 120.0;
```

## Example 4: Balanced Experience (Default)
```cpp
const int HORDE_SPAWN_INTERVAL = 600000;  // 10 minutes
const int HORDE_SIZE_MIN = 5;
const int HORDE_SIZE_MAX = 16;
const float SPAWN_DISTANCE_MIN = 50.0;
const float SPAWN_DISTANCE_MAX = 150.0;
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
