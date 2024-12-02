add_library (Game
  GameHandler.hpp
  GameHandler.cpp
  Sprite.hpp
  Sprite.cpp
  Element.hpp
  Element.cpp)

target_link_libraries(Game PRIVATE JSON)
target_include_directories(Game PUBLIC "../JSON/") #Include JSON library
target_include_directories(Game PUBLIC "../GFX/") #Include graphical code
target_link_libraries(Game PRIVATE Menu)
target_include_directories(Game PUBLIC "../Menu/") #Include UI code
target_include_directories(Game PUBLIC "../Misc/") #Include IO code

install(TARGETS Game DESTINATION bin)
install(FILES GameHandler.hpp DESTINATION include)
install(FILES Sprite.hpp DESTINATION include)
install(FILES Element.hpp DESTINATION include)
