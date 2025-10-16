cmake_minimum_required(VERSION 3.20)

add_executable(rogue_tests
  test_main.cpp
  test_scanner.cpp
  test_gitops.cpp
  test_config.cpp
)

target_link_libraries(rogue_tests PRIVATE roguecore)

add_test(NAME rogue_tests COMMAND rogue_tests)
