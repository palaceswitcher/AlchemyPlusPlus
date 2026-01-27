add_library (UI
	Button.cpp
	ElementMenu.cpp)

target_link_libraries(UI PRIVATE Misc)
target_link_libraries(UI PRIVATE GFX)
target_link_libraries(UI PRIVATE ImGui)
target_link_libraries(UI PRIVATE Core)

install(TARGETS UI DESTINATION bin)
install(FILES Button.hpp DESTINATION include)
install(FILES ElementMenu.hpp DESTINATION include)
