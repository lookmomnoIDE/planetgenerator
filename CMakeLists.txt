cmake_minimum_required(VERSION 3.5)
project("PlanetGenerator")

# Force static linking
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Static runtime linkage for MSVC
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(${PROJECT_NAME} 
    src/glad.c 
    src/main.cpp 
    src/imgui.cpp 
    src/imgui_draw.cpp 
    src/imgui_impl_glfw.cpp 
    src/imgui_impl_opengl3.cpp 
    src/imgui_tables.cpp 
    src/imgui_widgets.cpp 
    src/imgui_demo.cpp
    src/nfd_win.cpp
    src/planetGenerator.cpp
) 

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE include/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE include/imgui/backends)
target_include_directories(${PROJECT_NAME} PRIVATE include/glm)

# GLFW - force static build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(C:/projects/LiteEngine/extern/glfw-3.4/glfw-3.4)
target_link_libraries(${PROJECT_NAME} glfw)

# OpenGL (always static on Windows)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

target_link_libraries(PlanetGenerator winmm)

# ASSIMP - use static library
set(Assimp_DIR "C:/Program Files (x86)/Assimp/lib/cmake/assimp-6.0" CACHE PATH "Directory containing AssimpConfig.cmake")
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
find_package(assimp REQUIRED)
# Use static library instead of DLL
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARY_FILE})

# Link static C/C++ runtime and Windows libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()