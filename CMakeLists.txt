cmake_minimum_required(VERSION 3.16)
project(English_Listen)

# 1. 基础环境设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS ON)

# 2. 启用 Qt 自动化
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 3. 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui Charts)

# 4. 定义可执行文件
add_executable(QtEnglishListen
        qt_main.cpp
        mainwindow.cpp
        mainwindow.h
        english_listen.rc
)

# 5. 链接库
target_link_libraries(QtEnglishListen
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Gui
        Qt6::Charts
)

# 6. Windows 平台特定配置
if(WIN32)
    set_target_properties(QtEnglishListen PROPERTIES
            WIN32_EXECUTABLE ON
    )
    enable_language(RC)
endif()

# 7. 设置输出目录为 bin
set(OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set_target_properties(QtEnglishListen PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BIN_DIR}"
)

# 8. 核心部署逻辑：所有文件平铺在 bin 目录下
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(QT_DEPLOY_TOOL NAMES windeployqt HINTS "${QT_BIN_DIR}")

    if(QT_DEPLOY_TOOL)
        add_custom_command(TARGET QtEnglishListen POST_BUILD
                # A. 部署 Qt DLL 和插件到 bin 目录
                # --dir 指定为输出目录本身
                COMMAND ${QT_DEPLOY_TOOL} "$<TARGET_FILE:QtEnglishListen>"
                --dir "${OUTPUT_BIN_DIR}"
                --no-translations
                --no-opengl-sw

                # B. 拷贝 MinGW 核心库到 bin 目录
                COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libgcc_s_seh-1.dll" "${OUTPUT_BIN_DIR}/"
                COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libstdc++-6.dll" "${OUTPUT_BIN_DIR}/"
                COMMAND ${CMAKE_COMMAND} -E copy "${QT_BIN_DIR}/libwinpthread-1.dll" "${OUTPUT_BIN_DIR}/"

                # C. 确保删除可能残余的旧 qt.conf，防止它干扰同级目录加载
                COMMAND ${CMAKE_COMMAND} -E remove -f "${OUTPUT_BIN_DIR}/qt.conf"

                COMMENT "Deploying all DLLs to the same folder as EXE"
                VERBATIM
        )
    endif()
endif()