add_library (Core
	GameHandler.cpp
	Progress.cpp)

target_link_libraries(Core PRIVATE glaze::glaze)
target_link_libraries(Core PRIVATE SDL3::SDL3)
target_link_libraries(Core PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_link_libraries(Core PRIVATE UI)
target_link_libraries(Core PRIVATE GFX)

install(TARGETS Core DESTINATION bin)
install(FILES GameHandler.hpp DESTINATION include)
install(FILES Progress.hpp DESTINATION include)
