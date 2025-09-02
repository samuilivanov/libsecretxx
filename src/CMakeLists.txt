file(GLOB HEADER_LIST CONFIGURE_DEPENDS
  "${libsecretxx_SOURCE_DIR}/include/secret/*.hpp"
  "${libsecretxx_SOURCE_DIR}/include/util/*.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(secret_core mocklib.cpp secret_schema.cpp secret_service.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(secret_core PUBLIC ../include)

# This depends on (header only) boost
target_link_libraries(secret_core PRIVATE glib-2.0 secret-1)
target_include_directories(secret_core PUBLIC /usr/include/libsecret-1 /usr/include/glib-2.0 /lib/x86_64-linux-gnu/glib-2.0/include)
# All users of this library will need at least C++11
target_compile_features(secret_core PUBLIC cxx_std_23)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

