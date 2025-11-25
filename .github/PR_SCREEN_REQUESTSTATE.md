Title: Refactor screens — centralized state transitions and requestState()

Summary
-------
This patch refactors screen state transitions across the project to centralize lifecycle handling in `main.cpp`. Instead of screens assigning `gState.currentState` directly, they now call `requestState(AppState)` which sets `gState.nextState` and lets the main loop call `exit()` on the previous screen and `enter()/load()/render()` on the new screen. Legacy `show*/handle*` wrappers were removed and screen handlers were unified to avoid extra renders after a transition request.

Motivation
----------
- Ensure `Screen::exit()` and `Screen::enter()` are always called during a transition.
- Avoid duplicated/unsynchronized rendering when screens change states directly.
- Simplify screen lifecycle and make behavior predictable.
- Add a defensive `deferRender` flag and a conditional `STATE_LOGGING` macro to allow safe debugging.

What changed (high-level)
-------------------------
- Added `requestState(AppState)` helper in `src/utils.*` (declaration + implementation).
- Added `gState.nextState` and `gState.deferRender` to `src/GlobalState.h`.
- Centralized transition handling in `src/main.cpp` (processes `gState.nextState`).
- Replaced direct assignments to `gState.currentState` / `gState.nextState` across screens with `requestState(...)`.
- Removed legacy `show*`/`handle*` wrapper declarations and definitions; converted usages to direct `render()` / `handleKey()` where needed.
- Implemented `handleKey` for `ScreenDateTimeMenu`.
- Made `requestState()` logging conditional via `STATE_LOGGING` macro (default 0).

Files touched (representative)
-----------------------------
- src/GlobalState.h
- src/main.cpp
- src/utils.h, src/utils.cpp
- src/screens/* (ScreenMainMenu, ScreenSettingsMenu, ScreenUARTMenu, ScreenEthMenu,
  ScreenAnalogInputsMenu, ScreenParameters, ScreenEnterPassword, ScreenDateTimeMenu,
  ScreenAnalogSettings) — various updates

How to test
-----------
1. Build the project:
   - Open the project in PlatformIO and run a build.
2. Smoke test on device or simulator:
   - Navigate between screens and verify transitions always call `exit()`/`enter()` (you can enable `STATE_LOGGING` for debug builds to see requests printed to Serial).
   - Verify `W` (back) and `E` (enter/apply) keys move between screens as before.
   - Verify screens that update values (e.g. UART/ETH) still save/load to registers correctly.

Reviewer checklist
------------------
- [ ] Confirm `main.cpp` handles `nextState` reliably and `deferRender` prevents double-renders.
- [ ] Verify no screen left calling `gState.currentState = ...` directly (grep for `currentState =`).
- [ ] Check that `requestState()` is visible via `utils.h` and that logging is conditional.
- [ ] Run full build and verify memory usage — ensure RAM/Flash are within acceptable ranges.

If push fails
-------------
If this repo does not have a remote configured or push fails for auth reasons, the branch will exist locally. I can provide the exact git commands I used so you can run them in your environment, or guide you through adding a remote and pushing the branch.

Notes
-----
This change is intentionally conservative: behavioral changes were minimized. `deferRender` is a protective measure; if your codebase always returns immediately after `requestState()` you can remove it to save 1 byte of RAM.


