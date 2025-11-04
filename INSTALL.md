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

You can customize the horde behavior by editing the configuration constants at the top of the `init.c` file (lines 1-16). This makes customization easy and clear.

#### Configuration Constants

All configuration is done at the top of the file using constants:

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

#### Spawn Interval
Change how often hordes spawn (in milliseconds):
- `300000` = 5 minutes
- `600000` = 10 minutes (default)
- `900000` = 15 minutes
- `1200000` = 20 minutes

#### Horde Size
Adjust the number of zombies per horde:
- Default: MIN=5, MAX=16 (5-15 zombies)
- Small: MIN=3, MAX=8 (3-7 zombies)
- Large: MIN=10, MAX=26 (10-25 zombies)

#### Spawn Distance
Change how far from players hordes spawn (in meters):
- Default: MIN=50.0, MAX=150.0
- Close: MIN=30.0, MAX=100.0
- Far: MIN=100.0, MAX=200.0

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
