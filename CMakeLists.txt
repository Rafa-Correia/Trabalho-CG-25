cmake_minimum_required(VERSION 3.15)
project(trabalhocg25)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src/math)
add_subdirectory(src/external)
add_subdirectory(src/engine)
add_subdirectory(src/generator)


find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OPENGL not found!")
endif()

if(WIN32)
    set(TOOLKITS_FOLDER "${CMAKE_SOURCE_DIR}/toolkits" CACHE PATH "Path to Toolkits folder")
    message(STATUS "Toolkits_DIR set to: ${TOOLKITS_FOLDER}")

    include_directories(
        ${TOOLKITS_FOLDER}/glut 
        ${TOOLKITS_FOLDER}/glew 
        ${TOOLKITS_FOLDER}/devil
    )

    target_link_libraries(engine PRIVATE
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
        ${TOOLKITS_FOLDER}/glew/glew32.lib
        ${TOOLKITS_FOLDER}/devil/DevIL.lib
    )

    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll" AND EXISTS "${TOOLKITS_FOLDER}/devil/DevIL.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${TOOLKITS_FOLDER}/devil/DevIL.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    if(NOT APPLE)
        find_package(GLEW REQUIRED)
        include_directories(${GLEW_INCLUDE_DIRS})
        target_link_libraries(engine ${GLEW_LIBRARIES})
    endif()

    find_package(DevIL REQUIRED)
    include_directories(${IL_INCLUDE_DIR})
    target_link_libraries(engine PRIVATE ${IL_LIBRARIES})

    target_link_libraries(engine PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

    if(NOT GLUT_FOUND)
        message(FATAL_ERROR ": GLUT not found!")
    endif()
endif()
