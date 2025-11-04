# SagaTheHorde Config Builder (Web)

A lightweight, static web tool to create and edit `config.json` for the SagaTheHorde mod. Open `index.html` in your browser—no server, no installation.

## Features
- Covers all fields from the mod's config (version 3.0):
  - Global settings (timers, progress, smoke, rewards, etc.)
  - Reward items list
  - Multiple horde locations with position/radius/counts and per-location progress radius
  - Per-location zombie class lists with loot, teleport settings, and toughness
- Import existing `config.json`
- Export a clean, consistently ordered `config.json`
- Copy JSON to clipboard

## How to use
1. Open this file locally:
   - Windows: Double-click `tools/config-builder/index.html` or open it with your browser.
2. Start with `New config` or `Import config.json`.
3. Edit settings:
   - Global Settings, Reward Items, and Hordes.
   - Add/remove horde entries and zombie classes, adjust positions and loot.
4. Export:
   - Click `Export config.json` to download a file.
   - Copy the file to your DayZ profile folder if needed (e.g. `%USERPROFILE%\Documents\DayZ\SagaTheHorde\config.json`) or where your server reads it.

## Notes
- The tool enforces config version `3.0` on export to match the current mod.
- Fields are ordered to produce stable diffs and readability.
- Numeric inputs accept integers or floats as appropriate.

If you need more fields exposed or validation rules, open an issue or ask in chat.
