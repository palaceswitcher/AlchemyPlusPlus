add_library (GFX
  Animation.hpp
  Animation.cpp
  SDL_FontCache.h
  SDL_FontCache.c)

target_link_libraries(GFX PRIVATE Game)
target_link_libraries(GFX PRIVATE JSON)

install (TARGETS GFX DESTINATION bin)
install (FILES Animation.hpp DESTINATION include)
install (FILES SDL_FontCache.h DESTINATION include)
