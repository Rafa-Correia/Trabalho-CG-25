file(GLOB_RECURSE GENERATOR_SOURCES *.cpp)
add_executable(generator ${GENERATOR_SOURCES})
target_include_directories(generator PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(generator
    PRIVATE
        math
)