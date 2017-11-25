# define  the  executable  in terms  of the  source  files
add_executable(test_bin  test.c)
target_include_directories(test_bin PUBLIC ../include)
target_link_libraries(test_bin LINK_PUBLIC Sndwch)

# copy test comparison files 
file(COPY assets DESTINATION .)

# add_custom_target(test_uk.sh ALL  DEPENDS ${file_dest })
# add_test(NAME  test_uk  COMMAND  ./ test_uk.sh)

# proper testing
add_test(NAME test  COMMAND  ./test_bin)
add_test(NAME verify_merge COMMAND diff -q ./merged.svg assets/expected/a_b_merged.svg)
add_test(NAME verify_parse_yaml COMMAND diff -q ./parsed.txt assets/expected/2d_cuts_list.txt)
#add_test(NAME verify_parse_2dcuts COMMAND diff -q ./parsed.yaml assets/cuts.yaml)
