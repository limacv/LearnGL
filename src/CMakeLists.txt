aux_source_directory(. LEARNGL_SRCS)
list(FILTER LEARNGL_SRCS EXCLUDE REGEX .*GL.cpp)
list(APPEND LEARNGL_SRCS ./basicGL.cpp)

add_executable(render ${LEARNGL_SRCS})

set(LIB_LIST_2
        ${PROJECT_SOURCE_DIR}/Dependencies/glfw3.lib
        ${PROJECT_SOURCE_DIR}/Dependencies/assimp/assimp-vc140-mt.lib
)

target_link_libraries(render ${LIB_LIST_2})