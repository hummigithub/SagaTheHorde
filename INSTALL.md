# Installation Guide

## Requirements
- DayZ Server
- Access to server files
- Basic knowledge of DayZ server configuration

## Installation Steps

### 1. Download the Mod
Download or clone the SagaTheHorde mod from the repository.

### 2. Install Mission Files
Copy the mission files to your server:
```
SagaTheHorde/mpmissions/dayzOffline.chernarusplus/init.c
```
to your server's mission folder:
```
YOUR_SERVER/mpmissions/dayzOffline.chernarusplus/
```

**Note:** Adjust the mission folder name according to your server configuration:
- `dayzOffline.chernarusplus` - Chernarus map
- `dayzOffline.enoch` - Livonia map
- `dayzOffline.takistanplus` - Takistan map

### 3. Configure the Mod (Optional)

You can customize the horde behavior by editing the `init.c` file:

#### Spawn Interval
Change how often hordes spawn (in milliseconds):
```cpp
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnHordes, 10000, true);
```
- `10000` = 10 seconds (default)
- `60000` = 1 minute
- `300000` = 5 minutes

#### Horde Size
Adjust the number of zombies per horde:
```cpp
int hordeSize = Math.RandomInt(5, 16);
```
- First number (5) = minimum zombies
- Second number (16) = maximum zombies

#### Spawn Distance
Change how far from players hordes spawn:
```cpp
float distance = Math.RandomFloat(50, 150);
```
- First number (50) = minimum distance in meters
- Second number (150) = maximum distance in meters

### 4. Restart Your Server
After installing the mod files, restart your DayZ server for changes to take effect.

## Verification

To verify the mod is working:
1. Join your server
2. Check the server logs for messages like:
   ```
   [SagaTheHorde] Spawned horde of X zombies near player at <position>
   ```
3. Wait for the spawn interval and look for zombie hordes appearing near you

## Troubleshooting

### Hordes Not Spawning
- Check that `init.c` is in the correct mission folder
- Verify the server restarted properly
- Check server logs for errors
- Ensure zombies are enabled in your server configuration

### Too Many/Too Few Zombies
- Adjust the `hordeSize` parameters in `init.c`
- Adjust the spawn interval if hordes are too frequent or rare

### Performance Issues
- Increase spawn interval (spawn less frequently)
- Reduce maximum horde size
- Check server performance and available resources

## Support

For issues, questions, or contributions:
- GitHub: https://github.com/hummigithub/SagaTheHorde
- Create an issue with detailed information about your problem
