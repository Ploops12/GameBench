# Salem Hexshooter - Combat Slice

A Raylib 6.x C++ first-person witch combat prototype.

This vertical slice focuses on fast movement, hostile Salem combat, real-time coat rummage, stackable reagents, curse delivery vectors, and an activatable ward. Camp brewing, cauldrons, carriages, queued recipes, and persistent laboratory systems are intentionally outside this slice.

## Build

```sh
cmake -S . -B build-codex
cmake --build build-codex
```

Run:

```sh
./build-codex/cauldron
```

The prototype opens on a minimal line-art main menu. Press `Enter`, `Space`, or click `PLAY` to start.

Direct-to-game smoke launch:

```sh
./build-codex/cauldron --play
```

Headless logic self-test:

```sh
./build-codex/cauldron --self-test
```

## Controls

- `WASD`: move
- Mouse: look
- `Shift`: sprint
- `Space`: jump
- Hold `TAB`: open the real-time coat rummage UI
- While rummaging:
	- `1`: Bottle projectile vector
	- `2`: Apple self-centered vector
	- `3`: Spider mine/trap vector
	- `4`: Poppet fallback vector
	- `Z X C V B N`: add reagents
	- Mouse click: select visible vector/reagent slots
	- `Backspace`: clear selected reagents
- Release `TAB`, then `LMB`: cast
- `RMB` or `F`: hold ward
- `R`: restart after death/clear, or restart the slice at any time
- `LMB`: restart from the death/clear overlay
- `Esc`: toggle mouse capture

## Spell Rules

Each spell uses exactly one base vector plus zero to three reagent selections. Repeating the same reagent strengthens that reagent's normal effect.

Base vectors:

- Bottle: thrown projectile curse
- Apple: eaten self-targeted burst/ward curse
- Spider: placed mine/trap
- Poppet: always-available weak fallback pin curse

Reagents:

- Grave Salt: damage and bite radius
- Saint Ash: ward gain
- Black Wax: rot and lingering floor hex
- Widow Nettle: thorn snare
- Moth Dust: fear/panic
- Bell Bronze: seeking and resonant radius

## Visual Direction

The prototype uses Raylib-native programmer art: high-contrast line shapes, billboard enemies/pickups, sparse colors, outlines, simple panels, and readable HUD feedback.
