# CMake最低版本号
cmake_minimum_required(VERSION 2.8)
# 项目信息
project(LMAAG)

include_directories(./include)
include_directories(./Dependencies)

file(COPY ./Dependencies/assimp/assimp-vc140-mt.dll
    DESTINATION ./bin)

add_subdirectory(src bin)
