add_library (Game
	Board.cpp
	Sprite.cpp
	Element.cpp)

target_link_libraries(Game PRIVATE glaze::glaze)
target_link_libraries(Game PRIVATE SDL3::SDL3)
target_link_libraries(Game PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_link_libraries(Game PRIVATE UI)
target_link_libraries(Game PRIVATE GFX)

install(TARGETS Game DESTINATION bin)
install(FILES Board.hpp DESTINATION include)
install(FILES Sprite.hpp DESTINATION include)
install(FILES Element.hpp DESTINATION include)
