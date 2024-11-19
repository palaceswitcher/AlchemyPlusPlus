add_library (Board
  Sprite.hpp
  Sprite.cpp
  Element.hpp
  Element.cpp)

target_link_libraries(Board PRIVATE JSON)
target_include_directories(Board PUBLIC "../JSON/") #Include JSON library
target_include_directories(Board PUBLIC "../GFX/") #Include graphical libraries
target_link_libraries(Board PRIVATE menu)
target_include_directories(Board PUBLIC "../Menu/") #Include graphical libraries
target_include_directories(Board PUBLIC "../Misc/") #Include IO library

install (TARGETS Board DESTINATION bin)
install (FILES Sprite.hpp DESTINATION include)
install (FILES Element.hpp DESTINATION include)
