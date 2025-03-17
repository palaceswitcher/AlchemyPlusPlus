add_library (Game
	GameHandler.cpp
	Board.cpp
	Sprite.cpp
	Element.cpp)

target_link_libraries(Game PRIVATE glaze::glaze)
target_link_libraries(Game PRIVATE UI)

install(TARGETS Game DESTINATION bin)
install(FILES GameHandler.hpp DESTINATION include)
install(FILES Board.hpp DESTINATION include)
install(FILES Sprite.hpp DESTINATION include)
install(FILES Element.hpp DESTINATION include)
