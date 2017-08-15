# define  the  executable  in terms  of the  source  files
add_executable(test_syntaxe  main.c)

# copy test comparison files 
file(COPY expected DESTINATION .)
file(COPY assets DESTINATION .)
include_directories(../include)

# add_custom_target(test_uk.sh ALL  DEPENDS ${file_dest })

add_test(NAME test_syntaxe  COMMAND  ./test_syntaxe)
add_test(NAME comp_res COMMAND diff -q assets/a.svg assets/b.svg)

target_link_libraries (test_syntaxe LINK_PUBLIC Sndwch)

# add_test(NAME  test_uk  COMMAND  ./ test_uk.sh)
