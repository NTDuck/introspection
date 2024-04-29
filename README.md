
<a id="yorick"></a>
# introspection

> an engineless RPG.


## Table of content
1. [**Demo**](#demo)
2. [**Installation**](#installation)
    1. [**Windows**](#installation-windows)
    2. [**Debian-based Linux**](#installation-debian)
    3. [**Arch-based Linux**](#installation-arch)
3. [**Features**](#features)
4. [**Tech stack**](#tech-stack)
5. [**Programming techniques**](#programming-techniques)
6. [**Class hierarchy**](#hierarchy)
7. [**Acknowledgements**](#acknowledgements)
8. [**License**](#license)
9. [**Support**](#support)


<a id="demo"></a>
## Demo

https://github.com/NTDuck/8964/assets/105532764/d3aede64-7881-4e20-8720-b111422541fb


<a id="installation"></a>
## Installation

<a id="installation-windows"></a>
### Windows

#### Prerequisites:
- **A** configured C++ compiler, preferably [MinGW](https://sourceforge.net/projects/mingw/) or [MinGW-w64](https://www.mingw-w64.org/)
- **B**rain (optional)
- **C**ompatible hardware

#### Configuration:
None required.

#### Clone the repository:
```bash
git clone https://github.com/NTDuck/8964 
cd 8964
```

#### Build the project:
```bash
mingw32-make
mingw32-make run
```

#### Troubleshooting:
Compile-time and runtime errors are unlikely to occur.

<a id="installation-debian"></a>
### Debian-based Linux distributions

#### Prerequisites:
```shell
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev zlib1g-dev
```
#### Configuration:
None required.

#### Clone the repository:
Same as Windows.

#### Build the project:
```shell
make
chmod +x ./build/8964
make run
```

#### Troubleshooting:
Linux builds, due to unknown reasons, are more prone to runtime segmentation faults.

<a id="installation-arch"></a>
### Arch-based Linux distributions

#### Prerequisites:
```shell
sudo pacman -Syu
sudo pacman -S gcc make sdl2 sdl2_image sdl2_ttf sdl2_mixer zlib
```
#### Configuration:
None required.

#### Clone the repository:
Same as Windows.

#### Build the project:
```shell
make
chmod +x ./build/8964
make run
```

#### Troubleshooting:
Linux builds, due to unknown reasons, are more prone to runtime segmentation faults.

### Others:

Official os-specific releases are not available, and compatibility layers (e.g. [wine](https://www.winehq.org/)) are not tested.


<a id="features"></a>
## Features

#### System
- `ESC`, when held, exits the game
- `F1`, while in-game, returns to the menu
- `F4`, while in-game, reloads the current level
- `F5`, while in-game, toggles between camera angles
- `F6` toggles grayscale

#### Player
- **Movement:** press `W` (up), `A` (left), `S` (down), and `D` (right) keys for navigation
- **Sprint:** holding down `Shift` increases the player's speed
- **Interaction:** press `E` to interact with certain objects and engage with in-game dialogues
- **Melee attack:** press `Space` to execute a melee attack
- **Ranged attack:** press `1`, `2`, `3`, `4`, and `5` to launch unique sets of projectiles

#### Entities
- **Non-interactables** do not offer any form of interaction
- **Interactables**, as the name suggests, can be interacted
- **Teleporter entities**, upon being trampled, immediately teleport the player to a different location
- **Hostile entities**, within a specified range, pursue and attempt to attack the player
- **Surge projectiles** are tile-bound projectiles

#### Components
Encompass common graphical elements such as **Buttons**, **Progress bars**, and **Dialogue boxes**. Offer interactivity through mouse and/or keyboard inputs.

#### Interfaces
Encapsulate all operations within a game state, therefore facilitating modular design and fostering adherence to [SoC](https://en.wikipedia.org/wiki/Separation_of_concerns).

#### Levels
- **`level-begin`**
- **`level-forest-0`**
- **`level-forest-1`**
- **`level-forest-2`**
- **`level-forest-3`**
- **`level-forest-4`**
- **`level-white-space`**


<a id="tech-stack"></a>
## Tech Stack

#### Language: C++ 17

#### Dependencies:
- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.28.3) 2.28.3, [SDL_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.6.3) 2.6.3, [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.20.2) 2.20.2, [SDL_mixer](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-2.6.3) 2.6.3
- [JSON for Modern C++](https://github.com/nlohmann/json/releases/tag/v3.11.2) 3.11.2
- [pugixml](https://github.com/zeux/pugixml/releases/tag/v1.14) 1.14
- [zlib](https://www.zlib.net/) 1.3 (with Microsoft Github's precompiled [zlib.dll](https://github.com/microsoft/malmo/files/5758471/zlib.zip))

#### Third-party tools:
- [Tiled](https://www.mapeditor.org/) 1.10.2 for level generation
- [KRMisha's Makefile](https://github.com/KRMisha/Makefile) for cross-platform compilation

#### Compiler: [MinGW GCC](https://github.com/niXman/mingw-builds-binaries/releases/tag/13.2.0-rt_v11-rev0) 13.2.0 (64-bit)

#### Build system: [GNU Make](https://www.gnu.org/software/make/) 4.4.1

#### Version control: [Git](https://git-scm.com/) 2.40.0.windows.1


<a id="programming-techniques"></a>
## Programming techniques:

#### Design patterns
Uses [**OOP**](https://en.wikipedia.org/wiki/Object-oriented_programming) with [**CRTP**](https://en.cppreference.com/w/cpp/language/crtp) and [**Explicit Template Instantiation**](https://en.cppreference.com/w/cpp/language/class_template).

#### Data Structures
- Custom [hash functors](https://en.cppreference.com/w/cpp/utility/hash) and [operator overloading](https://en.cppreference.com/w/cpp/language/operators)
- [LRU Cache](https://www.geeksforgeeks.org/lru-cache-implementation/) using double-ended queue and hashmap

#### Algorithms
- Texture manipulation, grayscale conversion
- Zlib decompression
- Base64 decoding
- Hexadecimal to RGB conversion
- A* search algorithm, Dijkstra pathfinding algorithm, Greedy Best-First-Search algorithm (both reverted from A*)

#### Data Serialization
- Supports manipulation & retrieval of JSON/XML data formats (in-memory & from files)

#### Misc
- Uses [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) and [Parameter pack](https://en.cppreference.com/w/cpp/language/parameter_pack)
- Uses a manual garbage collection system
- Supports distribution & retrieval of [custom events](https://wiki.libsdl.org/SDL2/SDL_UserEvent)
- Supports frame-independent animation
- Supports IDE-independent, cross-platform compilation


<a id="hierarchy"></a>
## Class Hierarchy

![Class hierarchy, for code comprehension](/assets/graphics/misc/hierarchy.png)


<a id="acknowledgements"></a>
## Acknowledgements

We would like to credit certain assets used in this project. Licenses, if any, should be included in the links below.

#### Tilesets
- [1-bit Pulp-compliant Tileset](https://teaceratops.itch.io/1-bit-tileset)
- [Minimalistic Tileset Assets](https://endoblance.itch.io/minimalistic-game-assets-rpg-maker-mv)
- [OMORI Game Assets](https://www.spriters-resource.com/pc_computer/omori/)
- [Cat 50+ animations](https://bowpixel.itch.io/cat-50-animations)
- [Top-down Mobs - Dog](https://admurin.itch.io/top-down-mobs-dog)
- [Enemy Galore I](https://admurin.itch.io/enemy-galore-1)
- [Hana Caraka Base Character](https://bagong-games.itch.io/hana-caraka-base-character)
- [Hooded Protagonist](https://penzilla.itch.io/hooded-protagonist)

#### Fonts
- [Omori](https://dafontonline.com/omori-font/)
- [Phorssa](https://www.dafont.com/phorssa.font)

#### BGM & SFX
- [credits song for my death](https://youtu.be/urxeNbBc1nk) by [vivivivivi](https://www.youtube.com/@vivivivivi)
- [Your Reality - DDLC](https://youtu.be/GU2D4zmhvbo) by [Artemisia](https://www.youtube.com/@ArtemisiaBeats)
- [Inabakumori-styled な - instrumental](https://youtu.be/p0s0_4KO9t4) by [いめ44](https://www.youtube.com/@ime44)
- [Modern Touhou-styled World's End Valentine - OMORI](https://youtu.be/eX2O32VkU5s) by [Angelina2 RedRevo-Scarlet](https://www.youtube.com/@angelinaflan58)
- [Needy Streamer Overload OST - 16 No Salvation, No Angels](https://youtu.be/0Q_RiK3XPu8?list=PLXeqOX4jBooy_OV6DDXNgCC8bbcWV85Ls) by [Sesto](https://www.youtube.com/@Sesto-)
- [Determination - UNDERTALE OST 011](https://youtu.be/W1i4mTyidOc) by [Misaki](https://www.youtube.com/@Misakiu)
- [Wilhelm Scream SFX](https://youtu.be/r6JK-gRELI0)
- [Shoe on Wet Ground SFX](https://www.zapsplat.com/music/footstep-single-shoe-on-wet-ground-very-light-puddles-of-water-5/), [Heavy Clump on Gravel SFX](https://www.zapsplat.com/music/single-boot-footstep-heavy-clump-on-gravel-2/)
- Numerous BGM & SFX from OMORI
- [Lucky Star PS2 Bonus Disc](https://archive.org/details/lucky-star-ps-2-bonus-disc)

We would also like to recognize reknowned games from which certain concepts and mechanics in this project drew inspiration, which include but are not limited to [Elden Ring](https://store.steampowered.com/app/1245620/ELDEN_RING/), [Omori](https://www.omori-game.com/), [Undertale](https://store.steampowered.com/app/391540/Undertale/), and [The Coffin of Andy and Leyley](https://store.steampowered.com/app/2378900/The_Coffin_of_Andy_and_Leyley/).


<a id="license"></a>
## License

This project is licensed under the [BSD-3-Clause license](LICENSE).


<a id="support"></a>
## Support

For support, please email nguyentuduck@gmail.com.

<br><hr>
<blockquote style="display: flex; align-items: center; justify-content: flex-center; text-align: right">
  <div style="margin-right: 50px;">
    from the ground they come<br>
    and to it<br>
    they <a href="#yorick"><b>return</b></a>
  </div>
  <img src="https://static.wikia.nocookie.net/leagueoflegends/images/0/05/Yorick_Render.png/revision/latest?cb=20190112002553" alt="yorick" style="width: 100px; height: auto;">
</blockquote>
