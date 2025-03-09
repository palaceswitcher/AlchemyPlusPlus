add_library (UI
  ElementMenu.hpp
  ElementMenu.cpp
  Lang.hpp
  Lang.cpp)

target_link_libraries(UI PRIVATE JSON)
target_link_libraries(UI PRIVATE Misc)
target_link_libraries(UI PRIVATE ImGui)

install(TARGETS UI DESTINATION bin)
install(FILES ElementMenu.hpp DESTINATION include)
install(FILES Lang.hpp DESTINATION include)
