add_library (Sndwch sndwch.c sndwch_fs.c)

include_directories(${LIBXML2_INCLUDE_DIR})

target_include_directories (Sndwch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries (Sndwch LINK_PUBLIC xml2 yaml)


