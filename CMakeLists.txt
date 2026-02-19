cmake_minimum_required(VERSION 3.16)
project(English_Listen)

# 编译优化配置
option(ENABLE_CCACHE "Enable ccache support" ON)
option(ENABLE_PARALLEL_BUILD "Enable parallel build" ON)

# 1. 基础环境设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS ON)

# 只构建64位版本
message(STATUS "Building 64-bit version only")

# 2. 编译器优化设置
if(MSVC)
    # MSVC优化选项
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    # GCC/Clang优化选项
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
endif()

# 3. 并行编译设置
if(ENABLE_PARALLEL_BUILD)
    if("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        # Ninja默认就是并行的
        message(STATUS "Using Ninja generator with automatic parallelism")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_MAKE_PROGRAM "make" CACHE STRING "Make program")
        set(ENV{MAKEFLAGS} "-j$(nproc)")
    elseif(WIN32)
        # Windows上通过-j参数控制并行度
        set(CMAKE_MAKE_PROGRAM "ninja" CACHE STRING "Make program")
    endif()
endif()

# 4. ccache支持
if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
        
        # 设置ccache配置
        set(ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros")
        set(ENV{CCACHE_BASEDIR} "${CMAKE_SOURCE_DIR}")
        set(ENV{CCACHE_NOHASHDIR} "true")
    else()
        message(STATUS "ccache not found, continuing without cache support")
    endif()
endif()

# 5. 启用 Qt 自动化
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 7. 构建类型特定优化
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Release模式下的额外优化
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug模式设置
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
    endif()
endif()

# 8. 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui Charts)

# 9. 定义可执行文件
add_executable(QtEnglishListen
    qt_main.cpp
    mainwindow.cpp
    mainwindow.h
    english_listen.rc
)

# 10. 链接库
target_link_libraries(QtEnglishListen
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Gui
    Qt6::Charts
)

# 移除预编译头文件
set_source_files_properties(pch.cpp PROPERTIES HEADER_FILE_ONLY ON)

# 预编译头文件功能已移除

# 11. Windows 平台特定配置
if(WIN32)
    set_target_properties(QtEnglishListen PROPERTIES WIN32_EXECUTABLE ON)
    enable_language(RC)
endif()

# 12. 设置输出目录
set_target_properties(QtEnglishListen PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "QtEnglishListen"
)

set(MAIN_TARGET QtEnglishListen)

# 13. 核心部署逻辑
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(QT_DEPLOY_TOOL NAMES windeployqt HINTS "${QT_BIN_DIR}")

    if(QT_DEPLOY_TOOL)
        # 部署
        add_custom_command(TARGET QtEnglishListen POST_BUILD
            COMMAND ${QT_DEPLOY_TOOL} "$<TARGET_FILE:QtEnglishListen>"
            --dir "${CMAKE_BINARY_DIR}/bin"
            --no-translations
            --no-opengl-sw
            
            # 拷贝MinGW库
            COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libgcc_s_seh-1.dll" "${CMAKE_BINARY_DIR}/bin/"
            COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libstdc++-6.dll" "${CMAKE_BINARY_DIR}/bin/"
            COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libwinpthread-1.dll" "${CMAKE_BINARY_DIR}/bin/"
            
            # 拷贝通知工具
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/snoretoast-x64.exe" "${CMAKE_BINARY_DIR}/bin/snoretoast.exe"
            
            COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_BINARY_DIR}/bin/qt.conf"
            COMMENT "Deploying application"
            VERBATIM
        )
    endif()
endif()