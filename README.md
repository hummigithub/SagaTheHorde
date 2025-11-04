# SagaTheHorde
DayZ Modification that brings zombie hordes to your server for an enhanced survival experience.

## Overview
SagaTheHorde is a server-side modification for DayZ that spawns dynamic zombie hordes near players. This mod increases the difficulty and excitement by creating intense survival scenarios with groups of 5-16 zombies spawning periodically around active players.

## Features
- **Dynamic Horde Spawning**: Zombie hordes spawn automatically near players
- **Configurable Size**: Hordes contain 5-16 zombies (customizable)
- **Safe Distance**: Zombies spawn 50-150 meters from players
- **Variety**: Multiple zombie types for diverse encounters
- **Performance Optimized**: Efficient spawning system
- **Easy Installation**: Simple mission file integration

## Quick Start
1. Copy mission files to your server
2. Restart the server
3. Enjoy zombie hordes!

See [INSTALL.md](INSTALL.md) for detailed installation and configuration instructions.

## Configuration
The mod is easily customizable through configuration constants at the top of the `init.c` file:
- **Spawn frequency** - how often hordes appear (default: 10 minutes)
- **Horde size** - number of zombies per horde (default: 5-16)
- **Spawn distance** - how far from players (default: 50-150 meters)

No need to search through code - all settings are clearly labeled at the top of the file!

## Requirements
- DayZ Server
- Server file access
- Compatible with any DayZ map (Chernarus, Livonia, Takistan, etc.)

## Support
Found a bug or have a suggestion? Please open an issue on GitHub.

## License
This project is open source and available for community use and modification.
