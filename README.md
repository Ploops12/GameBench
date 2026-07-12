# Hexescape: The Salem Road

A playable Raylib 6 C++ combat prototype: sprint and jump through a hostile,
line-art Salem arena, gather occult reagents, build curses inside a live coat
menu, and survive escalating hunts.

## Controls

| Input | Action |
| --- | --- |
| `W A S D` | Move |
| Mouse | Look |
| `Space` | Jump |
| Left `Shift` | Sprint |
| Left mouse or `F` | Cast the readied curse |
| Right mouse | Hold the ward |
| Hold `Tab` | Rummage through the coat while combat continues |
| `1 / 2 / 3 / 0` | Select Bottle / Apple / Spider / Poppet while rummaging |
| `4`–`9` | Add a reagent dose while rummaging |
| `Ctrl` + `4`–`9` | Remove that reagent's latest dose |
| `Backspace` / `C` | Undo the last dose / clear all doses |
| Release `Tab` | Ready the displayed curse |
| `Esc` | Release or recapture the mouse |
| `F11` | Toggle fullscreen |
| `Enter` | Start or restart |

Rummaging is deliberately dangerous: the world stays live, movement is slowed,
and sprint is disabled, but the witch can still move, jump, look, and hold the
ward. Her hands remain visible around the coat interface.

## Curses

Each crafted curse has exactly one delivery vector and up to three additive
reagent doses. Repeating a dose strengthens the same effect linearly.

- **Bottle** — a thrown, arcing curse projectile.
- **Apple** — an eaten heal/haste with a self-centred curse pulse.
- **Spider** — a placed, armed ground mine.
- **Poppet** — a free, weak hitscan needle that is always available and is used
  automatically if a readied recipe can no longer be afforded.

The six reagents are fixed to stable coat keys:

| Key | Reagent | Additive effect |
| --- | --- | --- |
| `4` | Bone Shards | Impact damage |
| `5` | Widow Nettle | Thorn snare duration |
| `6` | Rotcap | Rot damage, duration, and lingering blight |
| `7` | Bat Wing | Fear/panic duration |
| `8` | Witch Glass | Ward gain when the curse resolves |
| `9` | Preserved Eye | Reach and delivery-specific hunting behavior |

The coat preview shows the selected doses, quantities still available, delivery,
damage, reach, status durations, ward gain, and other outcome values before the
curse is readied.

The ward has a visible 100-point capacity and starts each hunt at 70. Holding
right mouse absorbs incoming damage and drains the resource; a broken ward seals
briefly and must be released before it can reform. It regenerates after its delay
when inactive.

## Build, test, and run

```sh
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
./build/cauldron
```

The test executable exercises the additive spell rules, inventory consumption,
movement and collision, ward damage flow, enemy behavior, pickups, hazards, coat
editing, and the live-world rummage contract without opening a window.
