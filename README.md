
# 8964

a RPG made without a game engine.


## Screenshots
![](preview.gif)


## Features

#### System:
- Some components can be interacted with your mouse!
- `F4`, while in-game, resets everything i.e. re-loads the level (without penalties!)
- `F5`, while in-game, toggles between camera angles
- `F6` toggles grayscale
- `ESC`, while in-game, returns to the menu

#### Player:
- `W` `A` `S` `D` moves the player
- `Shift`, when held, makes the player run
- `Space` launchs not-so-visually-aesthetic attacks!

#### Entities:
- Cat. Teleports you to somewhere fun.
- Slime. Approach with caution (will hurt you).

#### Map:
- Equilibrium
- Valley of Despair

(bad names btw - will expand in future commits)


## Installation

### Windows

**Prerequisites:**
- A configured C++ compiler, preferably [MinGW](https://sourceforge.net/projects/mingw/) or [MinGW-w64](https://www.mingw-w64.org/)
- Brain (optional)
- Compatible hardware

```bash
git clone https://github.com/NTDuck/8964 && cd 8964
.\compile.bat
```

### Ubuntu

**Prerequisites**:
```
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev zlib1g-dev
```

```
make
chmod +x ./build/8964
./build/8964
```

Alternatively, in case of weird segmentation fault:
```
gdb ./build/8964
run
backtrace
```

### Others:

Official os-specific releases are not available, and compatibility layers (e.g. [wine](https://www.winehq.org/)) are not tested.


## Tech Stack

**Language**: C++ 17

**Dependencies:** 
- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.28.3) 2.28.3, [SDL2_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.6.3) 2.6.3
- [JSON for Modern C++](https://github.com/nlohmann/json/releases/tag/v3.11.2) 3.11.2
- [pugixml](https://github.com/zeux/pugixml/releases/tag/v1.14) 1.14
- [zlib](https://www.zlib.net/) 1.3 (with Microsoft Github's precompiled [zlib.dll](https://github.com/microsoft/malmo/files/5758471/zlib.zip))

**Third-party tools:**
- [Tiled](https://www.mapeditor.org/) 1.10.2 for level generation
- [KRMisha's Makefile](https://github.com/KRMisha/Makefile) for cross-platform compilation

**Compiler:** [MinGW GCC](https://github.com/niXman/mingw-builds-binaries/releases/tag/13.2.0-rt_v11-rev0) 13.2.0

**Version control:** Git (_2.40.0.windows.1_)


## Acknowledgements

> We would like to credit certain assets used in this project. Licenses, if any, should be found in the links below.
- [Hooded Protagonist](https://penzilla.itch.io/hooded-protagonist)
- [Enemy Galore](https://admurin.itch.io/enemy-galore-1)
- [Cat 50+ animations](https://bowpixel.itch.io/cat-50-animations)
- [Modern Interiors](https://limezu.itch.io/moderninteriors) (paid version)
>
- [Omori Font](https://dafontonline.com/omori-font/)
- [Hermit Font](https://github.com/chrissimpkins/codeface/tree/master/fonts/hermit)
- [Badly Stuffed Animal Font](https://www.fontspace.com/badly-stuffed-animal-font-f107899) (DEMO)

> We would also like to recognize reknowned games from which certain concepts and mechanics in this project drew inspiration, which include but are not limited to [Elden Ring](https://store.steampowered.com/app/1245620/ELDEN_RING/), [Omori](https://www.omori-game.com/), [Undertale](https://store.steampowered.com/app/391540/Undertale/), and [The Coffin of Andy and Leyley](https://store.steampowered.com/app/2378900/The_Coffin_of_Andy_and_Leyley/).


## License

This project is licensed under the [BSD-3-Clause license](LICENSE).


## Support

For support, please email nguyentuduck@gmail.com.