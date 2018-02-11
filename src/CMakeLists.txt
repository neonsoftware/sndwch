add_library (sndwch sndwch.c sndwch_fs.c)

set_property(TARGET sndwch  PROPERTY C_STANDARD 99)

find_package(LibXml2)
find_package(LibYaml)

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${LIBYAML_INCLUDE_DIR})

target_include_directories (sndwch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra -Werror")

install (TARGETS sndwch ARCHIVE DESTINATION lib)
install (FILES ../include/sndwch.h DESTINATION include)

target_link_libraries (sndwch LINK_PUBLIC ${LIBYAML_LIBRARIES} ${LIBXML2_LIBRARY})
