add_library (JSON
  cJSON.h
  cJSON.c)

install (TARGETS JSON DESTINATION bin)
install (FILES cJSON.h DESTINATION include)
