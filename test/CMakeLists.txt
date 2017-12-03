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
add_test(NAME parse_yaml COMMAND diff -q ./log_config_parse.txt assets/expected/config_parse.txt)
add_test(NAME merge COMMAND diff -q ./merged.svg assets/expected/a_b_merged.svg)
add_test(NAME parse_and_merge COMMAND diff -q ./parsed_and_merged.svg assets/expected/a_b_merged.svg)
add_test(NAME parse_and_translate COMMAND diff -q ./translated.svg assets/expected/a_b_merged_and_translated.svg)
add_test(NAME slice COMMAND diff -q ./log_slice.txt assets/expected/slice.txt)
add_test(NAME cutorder COMMAND ./test_bin cutorder)
add_test(NAME layerorder COMMAND ./test_bin layerorder)
add_test(NAME neighbors COMMAND ./test_bin neighbors)
add_test(NAME layer COMMAND ./test_bin layer)
add_test(NAME minimise COMMAND ./test_bin minimise)

#add_test(NAME verify_parse_2dcuts COMMAND diff -q ./parsed.yaml assets/cuts.yaml)
