add_library (Game
  Sprite.hpp
  Sprite.cpp
  Element.hpp
  Element.cpp)

target_link_libraries(Game PRIVATE JSON)
target_include_directories(Game PUBLIC "../JSON/") #Include JSON library
target_include_directories(Game PUBLIC "../GFX/") #Include graphical libraries
target_link_libraries(Game PRIVATE Menu)
target_include_directories(Game PUBLIC "../Menu/") #Include graphical libraries
target_include_directories(Game PUBLIC "../Misc/") #Include IO library

install (TARGETS Game DESTINATION bin)
install (FILES Sprite.hpp DESTINATION include)
install (FILES Element.hpp DESTINATION include)
