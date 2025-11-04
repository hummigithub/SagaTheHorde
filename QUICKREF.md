# Quick Reference Guide

Quick reference for server administrators running SagaTheHorde mod.

## Installation (Quick)
1. Copy `mpmissions/dayzOffline.chernarusplus/init.c` to your server's mission folder
2. Adjust folder name to match your map (e.g., `dayzOffline.enoch` for Livonia)
3. Restart server
4. Done!

## Common Customizations

### Change Spawn Frequency
```cpp
// Line 19 in init.c
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, XXXXX, true);
```
Replace XXXXX with milliseconds:
- 60000 = 1 minute
- 300000 = 5 minutes
- 600000 = 10 minutes (recommended)
- 900000 = 15 minutes

### Change Horde Size
```cpp
// Line 41 in init.c
int hordeSize = Math.RandomInt(MIN, MAX);
```
Replace MIN and MAX:
- (3, 8) = Small hordes
- (5, 16) = Default
- (10, 26) = Large hordes
- (20, 41) = Massive hordes

### Change Spawn Distance
```cpp
// Line 47 in init.c
float distance = Math.RandomFloat(MIN, MAX);
```
Replace MIN and MAX (in meters):
- (30, 100) = Close range
- (50, 150) = Default
- (100, 200) = Far range

## Presets

### Easy Mode
```cpp
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 900000, true);
int hordeSize = Math.RandomInt(3, 8);
float distance = Math.RandomFloat(100, 200);
```

### Normal Mode (Default)
```cpp
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 600000, true);
int hordeSize = Math.RandomInt(5, 16);
float distance = Math.RandomFloat(50, 150);
```

### Hard Mode
```cpp
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 300000, true);
int hordeSize = Math.RandomInt(10, 26);
float distance = Math.RandomFloat(40, 120);
```

### Nightmare Mode
```cpp
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 180000, true);
int hordeSize = Math.RandomInt(20, 41);
float distance = Math.RandomFloat(30, 100);
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
