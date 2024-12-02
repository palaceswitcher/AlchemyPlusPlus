add_library (Menu
  ElementMenu.hpp
  ElementMenu.cpp
  Lang.hpp
  Lang.cpp)

target_link_libraries(Game PRIVATE JSON)
target_include_directories(Game PUBLIC "../Game/") #Include JSON library
target_link_libraries(Menu PRIVATE JSON)
target_include_directories(Menu PUBLIC "../JSON/") #Include JSON library
target_link_libraries(Menu PRIVATE Misc)
target_include_directories(Menu PUBLIC "../Misc/") #Include JSON library

install(TARGETS Menu DESTINATION bin)
install(FILES ElementMenu.hpp DESTINATION include)
install(FILES Lang.hpp DESTINATION include)
