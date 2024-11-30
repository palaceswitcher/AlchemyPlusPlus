# AlchemyPlusPlus
AlchemyPlusPlus is an SDL2 Alchemy clone written in C++. It is heavily inspired by [Andrey "Zed" Zaikin's Alchemy](https://youtu.be/_cVWWY5Mlug) and is data-driven, meaning most content (like element combinations and names) can be added to the game without modifying the code. The majority of this game's data is stored in `.json` files, and can be easily modified with the scripts in the `script` folder.

# Building
The following build instructions should work on Linux and Windows provided the appropriate dependencies are installed. For Windows, the easiest way is via MSYS2, though Visual Studio should also be able to build this.

## Dependencies
- SDL2
- SDL_image
- SDL_ttf
- CMake

```
git clone --recursive https://github.com/palaceswitcher/AlchemyPlusPlus
cd AlchemyPlusPlus
mkdir build && cd build
cmake ..
make
```

# Game Directory Structure
This is the current directory structure. Each folder in game data represents a distinct "game", with its own assets and combinations. This feature has yet to be implemented, so only the default game assets are currently used.

```
gamedata/
├─ default/
│  ├─ font/
│  ├─ lang/
│  ├─ textures/
│  │  ├─ backgrounds/
│  │  ├─ buttons/
│  │  ├─ elems/
│  ├─ combos.json
│  ├─ icon.png
│  ├─ meta.json
```
