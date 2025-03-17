add_library (UI
	ElementMenu.cpp)

target_link_libraries(UI PRIVATE Misc)
target_link_libraries(UI PRIVATE ImGui)

install(TARGETS UI DESTINATION bin)
install(FILES ElementMenu.hpp DESTINATION include)
