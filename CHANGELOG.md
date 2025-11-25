# Changelog

All notable changes to this project are recorded in this file.

## 2025-11-25 — Major: Screen refactor, PROGMEM, RAM optimizations

Summary
- Introduced a `Screen` base class and migrated existing UI screens to subclasses.
- Centralized menu and title strings in flash (PROGMEM).
- Added safe PROGMEM helpers: `readProgmemString()` and `readProgmemTableString()` in `src/utils.*`.
- Replaced unsafe `sprintf` calls with bounded `snprintf` where appropriate.
- Fixed/recreated corrupted `src/screens/ScreenUARTMenu.cpp` and consolidated parity table in PROGMEM.
- Converted several data structures to memory-friendlier representations (packed fields, fixed-point for some analog values).

Build / memory
- PlatformIO local build (ATmega128):
  - RAM: 60.0% (2458 bytes of 4096)
  - Flash: 21.1% (27582 bytes of 130560)

Files touched (representative)
- src/utils.h / src/utils.cpp              — PROGMEM helpers + menu utilities
- src/main.cpp                             — centralized PROGMEM menu/title tables
- src/MenuStructs.h                        — packed structs / menu definitions
- src/GlobalState.h / src/GlobalState.cpp  — memory-friendly state layout (where applicable)
- src/screens/Screen*.h / src/screens/Screen*.cpp
  - ScreenMainMenu, ScreenSettingsMenu, ScreenUARTMenu, ScreenEthMenu,
    ScreenAnalogSettings, ScreenDateTimeMenu, etc.

Testing
- Performed local PlatformIO builds after each migration batch.
- Verified that key screens compile and render strings read from PROGMEM via helper APIs.

Notes & follow-ups
- There is no git history in the workspace; create a branch and commit the changes before sending a PR.
- Remaining tasks (recommended):
  - Sweep other workspace projects (`ModbusTCP_Ratmm`, etc.) and unify PROGMEM handling.
  - Finish full float → fixed-point migration for all analog data (see README section for guidance).
  - Add small unit/integration tests for `readProgmem*` helpers.

If you need, I can prepare a ready-to-paste PR description and a suggested commit/branch name.
