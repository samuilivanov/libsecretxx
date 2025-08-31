enable_testing()

function(add_doctest name lib)
add_executable(${name} ${name}.cpp)
target_compile_features(${name} PRIVATE cxx_std_17)
target_link_libraries(${name} PRIVATE ${lib})
endfunction()

add_doctest(test_secret_schema secret_core)

add_test(NAME test_secret_schema COMMAND test_secret_schema)
