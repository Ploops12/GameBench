# Satchel of Thorns

A Raylib 6 C++ game-jam vertical slice about a witch fighting through hostile Salem with improvised curses. The presentation uses sparse, high-contrast line art drawn with Raylib primitives.

## Build and run

```bash
cmake -S . -B build-codex -DCMAKE_BUILD_TYPE=Release
cmake --build build-codex -j
./build-codex/cauldron
```

CMake fetches Raylib 6.0 when it is not already installed or available through FetchContent.

## Controls

- `WASD`: move
- Mouse: look
- `Left Shift`: sprint
- `Space`: jump
- Left mouse: cast the prepared curse
- Hold `F`: raise the ward; it absorbs most incoming damage, drains while held, and regenerates after a delay
- Hold `Q`: open the coat while combat continues; movement is slowed and sprinting/jumping are disabled
- `1` / `2` / `3` / `4` while rummaging: Bottle, Apple, Spider, or free Poppet delivery
- `Z` / `X` / `C` / `V` / `B` / `N` while rummaging: add Grave Salt, Widow Nettle, Bog Marrow, Moth Dust, Saint Ash, or Thorn Heart
- `Backspace` while rummaging: clear selected reagents
- `Escape`: release or recapture the mouse
- `R`: restart after defeat or victory

Each spell has one delivery vector and up to three reagent selections. Selecting the same reagent repeatedly stacks its ordinary effect. Reagents are consumed on cast; the weak Poppet remains available for free.

## Combat loop

Collect reagents placed along the road and dropped by defeated hunters. Rummage through the coat in real time to assemble thrown Bottle hexes, eaten Apple curses, or placed Spider mines. The six reagents add direct harm, snaring, rot, panic/seeking, ward gain, and splash/lingering briars in predictable layers. Defeat every hunter to clear the road.
