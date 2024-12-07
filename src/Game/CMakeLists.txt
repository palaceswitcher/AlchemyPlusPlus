add_library (Game
  GameHandler.hpp
  GameHandler.cpp
  Sprite.hpp
  Sprite.cpp
  Element.hpp
  Element.cpp)

target_link_libraries(Game PRIVATE JSON)
target_link_libraries(Game PRIVATE Menu)

install(TARGETS Game DESTINATION bin)
install(FILES GameHandler.hpp DESTINATION include)
install(FILES Sprite.hpp DESTINATION include)
install(FILES Element.hpp DESTINATION include)
