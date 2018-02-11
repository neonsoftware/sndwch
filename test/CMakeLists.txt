add_executable(test_bin test.c)

set_property(TARGET test_bin PROPERTY C_STANDARD 99)

target_include_directories(test_bin PUBLIC ../include ../src ${LIBXML2_INCLUDE_DIR} ${LIBYAML_INCLUDE_DIR})

target_link_libraries(test_bin LINK_PUBLIC sndwch  ${LIBYAML_LIBRARIES} ${LIBXML2_LIBRARY})

# copy test comparison files 
file(COPY assets DESTINATION .)

# proper testing
add_test(NAME clean COMMAND rm -fr out)
add_test(NAME mkdir COMMAND mkdir -p out/full out/log)
add_test(NAME rel_path COMMAND ./test_bin rel_path)
add_test(NAME test  COMMAND ./test_bin)
add_test(NAME parse_yaml COMMAND diff -q ./out/log/log_config_parse.txt assets/expected/config_parse.txt)
add_test(NAME merge COMMAND diff -q ./out/merged.svg assets/expected/a_b_merged.svg)
add_test(NAME parse_and_merge COMMAND diff -q ./out/parsed_and_merged.svg assets/expected/a_b_merged.svg)
add_test(NAME parse_and_translate COMMAND diff -q ./out/translated.svg assets/expected/a_b_merged_and_translated.svg)
add_test(NAME slice COMMAND diff -q ./out/log/log_slice.txt assets/expected/slice.txt)
add_test(NAME cutorder COMMAND ./test_bin cutorder)
add_test(NAME layerorder COMMAND ./test_bin layerorder)
add_test(NAME neighbors COMMAND ./test_bin neighbors)
add_test(NAME layer COMMAND ./test_bin layer)
add_test(NAME minimise COMMAND ./test_bin minimise)
add_test(NAME sandwich COMMAND ./test_bin sandwich)
