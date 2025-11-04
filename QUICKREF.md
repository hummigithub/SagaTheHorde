# Quick Reference Guide

Quick reference for server administrators running SagaTheHorde mod.

## Installation (Quick)
1. Copy `mpmissions/dayzOffline.chernarusplus/init.c` to your server's mission folder
2. Adjust folder name to match your map (e.g., `dayzOffline.enoch` for Livonia)
3. Restart server
4. Done!

## Common Customizations

All customizations are done by editing the constants at the top of `init.c` (lines 1-16).

### Change Spawn Frequency
```cpp
// Line 7 in init.c
const int HORDE_SPAWN_INTERVAL = XXXXX;
```
Replace XXXXX with milliseconds:
- 60000 = 1 minute
- 300000 = 5 minutes
- 600000 = 10 minutes (default)
- 900000 = 15 minutes

### Change Horde Size
```cpp
// Lines 10-11 in init.c
const int HORDE_SIZE_MIN = X;
const int HORDE_SIZE_MAX = XX;
```
Replace X and XX:
- MIN=3, MAX=8 = Small hordes (3-8 zombies)
- MIN=5, MAX=16 = Default (5-16 zombies)
- MIN=10, MAX=26 = Large hordes (10-26 zombies)
- MIN=20, MAX=41 = Massive hordes (20-41 zombies)

### Change Spawn Distance
```cpp
// Lines 14-15 in init.c
const float SPAWN_DISTANCE_MIN = XX.0;
const float SPAWN_DISTANCE_MAX = XXX.0;
```
Replace XX and XXX (in meters):
- MIN=30.0, MAX=100.0 = Close range
- MIN=50.0, MAX=150.0 = Default
- MIN=100.0, MAX=200.0 = Far range

## Presets

Edit the constants at the top of init.c:

### Easy Mode
```cpp
const int HORDE_SPAWN_INTERVAL = 900000;
const int HORDE_SIZE_MIN = 3;
const int HORDE_SIZE_MAX = 8;
const float SPAWN_DISTANCE_MIN = 100.0;
const float SPAWN_DISTANCE_MAX = 200.0;
```

### Normal Mode (Default)
```cpp
const int HORDE_SPAWN_INTERVAL = 600000;
const int HORDE_SIZE_MIN = 5;
const int HORDE_SIZE_MAX = 16;
const float SPAWN_DISTANCE_MIN = 50.0;
const float SPAWN_DISTANCE_MAX = 150.0;
```

### Hard Mode
```cpp
const int HORDE_SPAWN_INTERVAL = 300000;
const int HORDE_SIZE_MIN = 10;
const int HORDE_SIZE_MAX = 26;
const float SPAWN_DISTANCE_MIN = 40.0;
const float SPAWN_DISTANCE_MAX = 120.0;
```

### Nightmare Mode
```cpp
const int HORDE_SPAWN_INTERVAL = 180000;
const int HORDE_SIZE_MIN = 20;
const int HORDE_SIZE_MAX = 41;
const float SPAWN_DISTANCE_MIN = 30.0;
const float SPAWN_DISTANCE_MAX = 100.0;
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No hordes spawning | Check init.c is in correct mission folder |
| Server lag | Reduce horde size or increase spawn interval |
| Too many zombies | Decrease MAX in hordeSize line |
| Zombies spawn too close | Increase MIN in distance line |
| Zombies spawn too far | Decrease MAX in distance line |

## Server Log Messages
Look for this in your server logs:
```
[SagaTheHorde] Spawned horde of X zombies near player at <position>
```

## File Locations
```
YourServer/
└── mpmissions/
    └── dayzOffline.MAPNAME/
        └── init.c  <-- Edit this file
```

## Support
- Full docs: See INSTALL.md and CONFIGURATION.md
- Issues: https://github.com/hummigithub/SagaTheHorde/issues
- Questions: Open a GitHub issue

## Performance Tips
- Start with default settings
- Monitor server CPU/RAM usage
- Adjust one setting at a time
- Test with typical player count
- Get player feedback
