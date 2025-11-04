# Repository Structure

This document explains the structure and purpose of each file in the SagaTheHorde mod.

## Root Directory Files

### Documentation
- **README.md** - Main documentation with overview, features, and quick start guide
- **INSTALL.md** - Detailed installation and configuration instructions
- **CONFIGURATION.md** - Configuration examples and presets for different server types
- **CHANGELOG.md** - Version history and list of changes
- **LICENSE** - MIT License for the mod

### Mod Configuration Files
- **mod.cpp** - Mod metadata and description
- **config.cpp** - Component configuration for the mod

## Mission Files

### mpmissions/dayzOffline.chernarusplus/
This directory contains the mission files for the Chernarus map. The folder name can be changed to match other maps:
- `dayzOffline.enoch` for Livonia
- `dayzOffline.takistanplus` for Takistan
- Other custom map names as needed

#### init.c
The main script file containing:
- **main()** - Mission initialization function
  - Hive and weather setup
  - Economy initialization
  - Horde system startup
  
- **SpawnHordes()** - Core horde spawning logic
  - Player detection
  - Random horde generation
  - Zombie type selection
  - Spawn position calculation
  
- **CustomMission** - Custom mission class
  - Character creation
  - Starting equipment setup
  - Player spawn configuration

## File Relationships

```
SagaTheHorde/
├── mod.cpp                          # Mod metadata
├── config.cpp                       # Mod configuration
├── README.md                        # Main documentation
├── INSTALL.md                       # Installation guide
├── CONFIGURATION.md                 # Configuration examples
├── CHANGELOG.md                     # Version history
├── LICENSE                          # License file
└── mpmissions/
    └── dayzOffline.chernarusplus/
        └── init.c                   # Main mission script
```

## How It Works

1. **Server Starts** → Loads mission files
2. **main() Executes** → Initializes server systems and horde spawning
3. **SpawnHordes() Runs** → Periodically spawns zombie hordes near players
4. **Players Experience** → Encounter dynamic zombie hordes during gameplay

## Customization Points

### For Server Admins
- Edit `init.c` to change spawn rates, horde sizes, and distances
- Copy mission folder for different maps
- Adjust zombie types in the spawn function

### For Mod Developers
- Extend `CustomMission` class for additional features
- Add new zombie types to the spawn selection
- Implement additional gameplay mechanics
- Create configuration file parser for easier customization

## Integration with Other Mods

This mod uses standard DayZ mission files and should be compatible with most other mods. However, if another mod also uses a custom `init.c` file, you'll need to merge the functionality:

1. Keep the existing `main()` function from the other mod
2. Add the `SpawnHordes()` function from this mod
3. Add the horde system initialization line to `main()`
4. Test thoroughly to ensure both mods work together

## Support

For questions about the structure or how to modify the mod:
- Check the documentation files (README, INSTALL, CONFIGURATION)
- Review the code comments in init.c
- Open an issue on GitHub for specific questions
