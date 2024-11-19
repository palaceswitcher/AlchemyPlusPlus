# AlchemyPlusPlus
AlchemyPlusPlus is an SDL2 Alchemy clone written in C++. It is heavily inspired by [Andrey "Zed" Zaikin's Alchemy](https://youtu.be/_cVWWY5Mlug) and is data-driven, meaning most content (like element combinations and names) can be added to the game without modifying the code. The majority of this game's data is stored in `.json` files, and can be easily modified with the scripts in the `script` folder.

# Building
This is currently only tested on Linux. This should work fine under Windows, though this has not been verified.

## Dependencies
- SDL2
- SDL_image
- SDL_ttf
- CMake

```
git clone https://github.com/palaceswitcher/AlchemyPlusPlus
cd AlchemyPlusPlus
mkdir build && cd build
cmake ..
make
```
