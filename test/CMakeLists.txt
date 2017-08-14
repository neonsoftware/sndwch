# define  the  executable  in terms  of the  source  files
add_executable(test_syntaxe  main.c)
file(COPY a DESTINATION .)
file(COPY b DESTINATION .)

include_directories(../src)

# add_custom_target(test_uk.sh ALL  DEPENDS ${file_dest })
add_test(NAME  test_syntaxe  COMMAND  ./test_syntaxe)
add_test(NAME wheream COMMAND pwd)
add_test(NAME comp_res COMMAND diff -q a b)

# add_test(NAME  test_uk  COMMAND  ./ test_uk.sh)
