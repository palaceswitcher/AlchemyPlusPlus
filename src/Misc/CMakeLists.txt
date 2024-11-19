add_library (Misc
  IO.cpp
  IO.cpp)

target_link_libraries(Misc PRIVATE JSON)

install (TARGETS Misc DESTINATION bin)
install (FILES IO.hpp DESTINATION include)
