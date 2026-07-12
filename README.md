# Thornwake: A Salem Hexshooter

A compact Raylib 6.x first-person combat prototype. Fight three escalating breaches of line-art Salem hunters, gather six occult reagents, assemble curses from inside the witch's coat without pausing the world, and survive with an activatable ward. The vector-art presentation and short casting/combat sounds are generated entirely with Raylib at runtime.

## Build and run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
./build/cauldron
```

CMake downloads the pinned Raylib 6.0 source on the first configure. No other external dependency is used by the game.

Run the deterministic whole-slice verification with:

```sh
./build/cauldron --self-test
```

It exercises movement, jumping, collision, rummage slowdown and vulnerability, all base deliveries, all six repeated reagent effects, inventory consumption, enemy attacks and statuses, ward absorption/regeneration, pickups, drops, occlusion, all three waves, victory, and game over. To additionally initialize Raylib/OpenGL/audio and render 120 hidden frames:

```sh
./build/cauldron --runtime-smoke
```

## Controls

- `WASD`: move
- Mouse: look
- `Left Shift`: sprint
- `Space`: jump
- Left mouse: cast the curse currently in hand
- Right mouse or `Q`: raise the ward; it drains while raised and regenerates after a delay
- Hold `R`: open the coat while combat keeps running; movement slows, sprint/jump lock, and incoming health damage increases
- While holding `R`, `1` / `2` / `3` / `4`: choose Bottle / Apple / Spider / Poppet
- While holding `R`, `Z` / `X` / `C` / `V` / `B` / `N`: add Bone Splinters / Grave Salt / Widow Nettle / Rotcap / Moth Dust / Saint Ash
- While holding `R`, `Backspace`: return the last selected reagent; `Delete`: clear all three reagent loops
- `Escape`: release or recapture the cursor and hold/resume the world
- `Enter`: start or restart a run

## Curse rules

Bottle throws an impact projectile, Apple creates a self-centered moving hex aura and mends the witch, and Spider places a proximity trap. A curse accepts zero to three reagents. Repeating a reagent linearly strengthens its normal effect; there are no hidden combination recipes. Poppet is an infinite weak hitscan fallback and cannot carry reagents.

The six reagent effects are always visible in the coat preview: damage, radius, thorn snare, rot/lingering mire, seeking/panic, and ward return. Solid scenery blocks Poppet rays, Bottle projectiles, and hunter attacks.
