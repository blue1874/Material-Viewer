# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 项目信息
project (opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(VCPKG_ROOT "D:/blue/Coding/vcpkg")
set(VCPKG_LIB "${VCPKG_ROOT}/installed/x86-windows/debug/lib")
set(VCPKG_BIN "${VCPKG_ROOT}/installed/x86-windows/debug/bin")

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


link_directories()
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

find_package(libpng CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE png)

find_package(CImg CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CImg::CImg)

# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)