cmake_minimum_required(VERSION 3.20)

project(RogueMagicBox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(ENABLE_COVERAGE "Enable code coverage (Linux/gcc/clang)" OFF)
option(ENABLE_TUI "Enable optional TUI (placeholder)" OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include dirs
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Optional CURL for GitHub API
find_package(CURL QUIET)
if(CURL_FOUND)
  add_compile_definitions(HAVE_LIBCURL)
endif()

# Coverage flags (Linux)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message(STATUS "Enabling coverage flags")
  add_compile_options(--coverage -O0 -g)
  link_libraries(--coverage)
endif()

# Core sources
set(CORE_SRC
  src/cli/commands_scan.cpp
  src/cli/commands_init.cpp
  src/cli/commands_push.cpp
  src/core/scanner.cpp
  src/core/gitops.cpp
  src/core/github_api.cpp
  src/core/logger.cpp
  src/core/config.cpp
  src/core/utils.cpp
)

add_library(roguecore STATIC ${CORE_SRC})
target_include_directories(roguecore PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/third_party)

add_executable(roguebox src/main.cpp src/cli/args.hpp)
target_link_libraries(roguebox PRIVATE roguecore)

if(CURL_FOUND)
  target_link_libraries(roguecore PUBLIC CURL::libcurl)
endif()

if(WIN32)
  target_compile_definitions(roguebox PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# Install rules
install(TARGETS roguebox RUNTIME DESTINATION bin)

# CPack configuration for release archives
include(CPack)
set(CPACK_PACKAGE_NAME "RogueMagicBox")
set(CPACK_PACKAGE_VENDOR "EPSI/WIS Workshop")
set(CPACK_PACKAGE_CONTACT "workshop@example.com")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP;TGZ")
if(WIN32)
  set(CPACK_SYSTEM_NAME "windows")
else()
  set(CPACK_SYSTEM_NAME "linux")
endif()
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Tests
enable_testing()
add_subdirectory(tests)

# Install docs & config (optionnel, vérifie existence)
if(EXISTS "${CMAKE_SOURCE_DIR}/docs")
  install(DIRECTORY docs/ DESTINATION share/roguebox/docs)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/config")
  install(DIRECTORY config/ DESTINATION share/roguebox/config)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/.rogueignore")
  install(FILES .rogueignore DESTINATION share/roguebox)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
  install(FILES LICENSE DESTINATION share/roguebox)
endif()
