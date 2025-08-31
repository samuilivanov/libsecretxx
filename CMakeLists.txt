# Works with 3.15 and tested through 4.0
cmake_minimum_required(VERSION 3.15...4.0)

project(
  libsecretxx 
  VERSION 0.1
  DESCRIPTION "Modern C++ wrapper for libsecret with sync/async and coroutine support"
  LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
# FetchContent_MakeAvailable was added in CMake 3.14; simpler usage
include(FetchContent)

# The compiled library code is here
add_subdirectory(src)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    # List of source files for cppcheck (could be entire src dir)
    file(GLOB_RECURSE CPP_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )

    set(INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    )

    set(CPPCHECK_OPTIONS
        --enable=all
        --inconclusive
        --force
        --std=c++17
        --inline-suppr
        --verbose
        --suppress=missingIncludeSystem
    )

    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} ${CPPCHECK_OPTIONS}
                -I"${INCLUDE_DIRS}"
                ${CPP_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cppcheck..."
        VERBATIM
    )
endif()

find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    file(GLOB_RECURSE CPP_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )

    set(INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    )


    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY} -p=${CMAKE_BINARY_DIR} ${CPP_FILES} ${INCLUDE_DIRS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy on all source files..."
        VERBATIM
    )
endif()