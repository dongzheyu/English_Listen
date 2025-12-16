cmake_minimum_required(VERSION 3.16)
project(English_Listen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

# 查找Flite库
find_library(FLITE_LIB flite HINTS "D:/msys64/mingw64/lib")
find_path(FLITE_INCLUDE_DIR flite.h HINTS "D:/msys64/mingw64/include")

# 添加Qt版本可执行文件
add_executable(QtEnglishListen
    qt_main.cpp
    mainwindow.cpp
    mainwindow.h
    english_listen.rc
)

# 链接Qt6库和Flite库
target_link_libraries(QtEnglishListen 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Network
)

# 如果找到了Flite库，则链接它
if(FLITE_LIB AND FLITE_INCLUDE_DIR)
    target_link_libraries(QtEnglishListen ${FLITE_LIB})
    target_include_directories(QtEnglishListen PRIVATE ${FLITE_INCLUDE_DIR})
    # 添加预处理器定义，标记Flite已启用
    target_compile_definitions(QtEnglishListen PRIVATE FLITE_ENABLED)
endif()

# Windows特定设置
if(WIN32)
    # 链接Windows多媒体库以支持Flite
    target_link_libraries(QtEnglishListen winmm)
    
    # 将Qt应用程序设置为Windows GUI应用程序，避免显示控制台窗口
    set_target_properties(QtEnglishListen PROPERTIES WIN32_EXECUTABLE ON)
    
    # 启用Windows资源编译
    enable_language(RC)
    
    # 构建后自动部署Qt依赖库和C++运行时库
    add_custom_command(
        TARGET QtEnglishListen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:QtEnglishListen>/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${Qt6_DIR}/../../../bin
        COMMAND Qt6::windeployqt --dir "$<TARGET_FILE_DIR:QtEnglishListen>/windeployqt" --compiler-runtime "$<TARGET_FILE:QtEnglishListen>"
        COMMENT "Deploying Qt libraries and C++ runtime with windeployqt"
    )
    
    # 构建后复制Flite运行时库（如果找到了Flite库）
    if(FLITE_LIB)
        get_filename_component(FLITE_LIB_NAME ${FLITE_LIB} NAME)
        get_filename_component(FLITE_LIB_DIR ${FLITE_LIB} DIRECTORY)
        add_custom_command(
            TARGET QtEnglishListen POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FLITE_LIB_DIR}/${FLITE_LIB_NAME}" "$<TARGET_FILE_DIR:QtEnglishListen>/" || echo "Warning: Could not copy Flite library."
            COMMENT "Copying Flite runtime library"
        )
    endif()
    
    # 构建后自动签名（使用指纹签名）
    add_custom_command(
        TARGET QtEnglishListen POST_BUILD
        COMMAND "D:/SignTool-10.0.22621.6-x64/signtool.exe" sign /sha1 02FD515617FB5BF6541508ACA7651728C6CB8118 /t http://timestamp.digicert.com /fd SHA256 "$<TARGET_FILE:QtEnglishListen>"
        COMMENT "使用SignTool对可执行文件进行数字签名（指纹签名）"
    )
endif()

# 添加发布配置
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # 优化编译选项
    target_compile_options(QtEnglishListen PRIVATE 
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -flto>
    )
    
    # 链接时优化
    set_target_properties(QtEnglishListen PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION TRUE
    )
    
    # 添加发布后处理命令
    add_custom_command(TARGET QtEnglishListen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Release build completed successfully!"
        COMMAND ${CMAKE_COMMAND} -E echo "Executable: $<TARGET_FILE:QtEnglishListen>"
        COMMENT "Release build post-processing"
    )
endif()
