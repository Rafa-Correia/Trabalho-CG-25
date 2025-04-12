file(GLOB_RECURSE UTILS_SOURCES *.cpp)
add_library(external STATIC ${UTILS_SOURCES})
target_include_directories(external PUBLIC ${CMAKE_SOURCE_DIR}/include)