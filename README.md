# Hexescape: The Salem Road — Consolidated

A playable Raylib 6 C++ combat prototype at **720×720**: sprint and jump through
a hostile, line-art Salem arena, gather occult reagents, build curses inside a
live coat menu, and survive escalating hunts.

## Controls

| Input | Action |
| --- | --- |
| `W A S D` | Move |
| Mouse | Look |
| `Space` | Jump |
| Left `Shift` | Sprint |
| Left mouse | Cast the readied curse |
| Right mouse | Hold the ward |
| Hold `R` / tap `Tab` | Open the live coat rummage panel |
| `1 / 2 / 3 / 4` | Select Bottle / Apple / Spider / Poppet while rummaging |
| `Backspace` / `Delete` | Remove the last dose / clear all doses |
| `Enter` or click `CAST` | Cast directly from the open coat |
| Release `R` / tap `Tab` again | Close the coat and ready its displayed curse |
| `1 / 2 / 3` outside the coat | Select Bottle / Apple / Spider as the active vector |
| `Q / E` outside the coat | Cycle the active vector, including Poppet |
| `Esc` | Release or recapture the mouse |
| `F11` | Toggle fullscreen |
| `Enter` | Start or restart |

Rummaging is deliberately dangerous: the world stays live, movement is reduced
to 44%, sprinting, jumping, and looking are locked, and incoming hits deal 25%
more damage. The persistent **COAT STOCK** panel shows every carried reagent
outside the rummage screen.

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
| `Z` | Bone Shards | Impact damage |
| `X` | Widow Nettle | Thorn snare duration |
| `C` | Rotcap | Rot damage, duration, and lingering blight |
| `V` | Bat Wing | Fear/panic duration |
| `B` | Witch Glass | Ward gain when the curse resolves |
| `N` | Preserved Eye | Reach and delivery-specific hunting behavior |

The coat preview shows the selected doses, quantities still available, delivery,
damage, reach, status durations, ward gain, and other outcome values before the
curse is readied or cast. Casting a stitched curse consumes its doses and clears
the three stitched slots; Poppet is always no-cost.

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
