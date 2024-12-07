add_library (Menu
  ElementMenu.hpp
  ElementMenu.cpp
  Lang.hpp
  Lang.cpp)

target_link_libraries(Game PRIVATE ImGui)
target_link_libraries(Game PRIVATE JSON)
target_link_libraries(Game PRIVATE Menu)
target_link_libraries(Menu PRIVATE JSON)
target_link_libraries(Menu PRIVATE Misc)

install(TARGETS Menu DESTINATION bin)
install(FILES ElementMenu.hpp DESTINATION include)
install(FILES Lang.hpp DESTINATION include)
