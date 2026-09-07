add_library(locks INTERFACE)
target_include_directories(locks INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 2. Link standard thread support
find_package(Threads REQUIRED)
target_link_libraries(locks INTERFACE Threads::Threads)

# 3. Enable testing and add our test executable
enable_testing()

add_executable(test_s_lock tests/test_s_lock.cpp)
target_link_libraries(test_s_lock PRIVATE locks)

set_target_properties(test_s_lock PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME s_lock_correctness COMMAND test_s_lock)