add_compile_options(-Wall -Wextra -Wpedantic -finline-limit=1000)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(position)
add_subdirectory(magics)
target_link_libraries(determine_magics position)

add_executable(fce fce.cpp)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set_property(TARGET fce PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*,performance-*,modernize-*,bugprone-*,cppcoreguidelines-*,-modernize-use-trailing-return-type,-cppcoreguidelines-pro-bounds-constant-array-index,-cppcoreguidelines-pro-bounds-pointer-arithmetic)
endif()
target_link_libraries(fce position)
