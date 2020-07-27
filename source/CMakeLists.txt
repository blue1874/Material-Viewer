# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 设置VCPKG路径
set(DVCPKG_TARGET_TRIPLET "x86-windows")
set(DCMAKE_TOOLCHAIN_FILE "D:/blue/Coding/opengl-learn/vcpkg-mirror/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)
# 项目信息
project (opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. APP)
aux_source_directory(./imgui IMGUI)
aux_source_directory(./io IO)
aux_source_directory(./mgl MGL)
aux_source_directory(./scene SCENE)
aux_source_directory(./tool TOOL)

include_directories(${CMAKE_SOURCE_DIR})
# 指定生成目标
add_executable(${PROJECT_NAME} ${APP} ${IMGUI} ${IO} ${MGL} ${SCENE} ${TOOL})


find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)