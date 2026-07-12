# Salem Hexshooter

A Raylib 6 first-person witch combat prototype. The arena is a hostile Salem lane: collect occult reagents, hold `TAB` to rummage the coat while the world continues, assemble a three-slot curse, and clear the hunters.

## Controls

- `WASD`: move, `SHIFT`: sprint, `SPACE`: jump
- Mouse: look, `ESC`: release/capture cursor
- `LMB`: cast the current curse, `R`: activate ward
- Hold `TAB`: coat rummage; `Q/E`: change base vector
- `1-6`: select reagent, `ENTER` or mouse click: add it, `BACKSPACE`: remove the last reagent

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
```
