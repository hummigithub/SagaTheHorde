# Changelog

All notable changes to the SagaTheHorde mod will be documented in this file.

## [1.0.0] - 2025-11-04

### Added
- Initial release of SagaTheHorde mod
- Dynamic zombie horde spawning system
- Configurable horde size (5-16 zombies default)
- Configurable spawn interval (10 minutes default)
- Safe spawn distance (50-150 meters from players)
- Multiple zombie types for variety
- Server log messages for horde spawns
- Comprehensive documentation
  - README.md with overview and quick start
  - INSTALL.md with detailed installation instructions
  - CONFIGURATION.md with example configurations
- Mission file for Chernarus map (compatible with other maps)
- Custom mission class with player spawn setup

### Features
- Automatic horde spawning near active players
- Random zombie type selection from 10 different variants
- Ground position detection for realistic spawns
- Performance-optimized spawning system
- Easy customization through init.c configuration

### Technical Details
- Written in Enforce Script (DayZ scripting language)
- Server-side implementation (no client mods required)
- Compatible with vanilla DayZ servers
- Mission-based integration
- Event-driven spawn system

## Future Plans
- Add configuration file for easier customization
- Implement difficulty scaling based on player count
- Add special zombie types for hordes
- Create admin commands for manual horde spawning
- Add sound effects for horde warnings
- Implement day/night spawn variations
- Add statistics tracking
