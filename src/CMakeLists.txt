add_library (Sndwch sndwch.c)

target_include_directories (Sndwch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (Sndwch LINK_PUBLIC xml2 yaml)


