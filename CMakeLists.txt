cmake_minimum_required(VERSION 3.16)
project(English_Listen)

# 1. 基础环境设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 显式声明构建共享库（动态链接）
set(BUILD_SHARED_LIBS ON)

# 2. 启用 Qt 自动化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置构建类型（默认 Release）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 3. 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui)

# 4. 定义可执行文件
add_executable(QtEnglishListen
        qt_main.cpp
        mainwindow.cpp
        mainwindow.h
        english_listen.rc
)

# 5. 链接库
# 动态链接模式下，Qt 会自动处理系统底层依赖，无需手动添加 imm32/ws2_32 等
target_link_libraries(QtEnglishListen
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Gui
)

# 6. Windows 平台特定配置
if(WIN32)
    set_target_properties(QtEnglishListen PROPERTIES
            WIN32_EXECUTABLE ON
    )
    enable_language(RC)
endif()

# 7. 编译优化与定义
target_compile_definitions(QtEnglishListen PRIVATE
        ENGLISH_LISTEN_VERSION="2.2.0"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(QtEnglishListen PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/O2>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2>
    )
    set_target_properties(QtEnglishListen PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION ON
    )
endif()

# 8. 设置输出目录
# 将 EXE 统一输出到 bin 文件夹
set(OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set_target_properties(QtEnglishListen PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BIN_DIR}"
)

# 9. Windows 自动部署与目录分离 (Runtime 分离逻辑)
if(WIN32)
    # 查找 windeployqt 工具
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(QT_DEPLOY_TOOL NAMES windeployqt HINTS "${QT_BIN_DIR}")

    if(QT_DEPLOY_TOOL)
        set(RUNTIME_DIR "${OUTPUT_BIN_DIR}/runtime")

        add_custom_command(TARGET QtEnglishListen POST_BUILD
                # 创建 runtime 目录
                COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNTIME_DIR}"

                # 部署 DLL 到 runtime 文件夹
                # --no-opengl-sw: 排除体积巨大的软件渲染器
                # --no-translations: 排除多国语言包
                # --compiler-runtime: 包含 MSVC 运行时库
                COMMAND ${QT_DEPLOY_TOOL} "$<TARGET_FILE:QtEnglishListen>"
                --dir "${RUNTIME_DIR}"
                --no-opengl-sw
                --no-translations
                --compiler-runtime

                # 生成修正版的 qt.conf (确保物理换行)
                COMMAND ${CMAKE_COMMAND} -E echo "[Paths]" > "${OUTPUT_BIN_DIR}/qt.conf"
                COMMAND ${CMAKE_COMMAND} -E echo "Prefix = runtime" >> "${OUTPUT_BIN_DIR}/qt.conf"
                COMMAND ${CMAKE_COMMAND} -E echo "Plugins = ." >> "${OUTPUT_BIN_DIR}/qt.conf"

                COMMAND ${CMAKE_COMMAND} -E echo "✓ Deployment complete: EXE and DLLs are separated."
                COMMENT "Deploying dependencies and generating qt.conf..."
                VERBATIM
        )
    endif()
endif()