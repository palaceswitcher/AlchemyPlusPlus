add_library (Misc
  IO.cpp
  IO.cpp)

install (TARGETS Misc DESTINATION bin)
install (FILES IO.hpp DESTINATION include)
