cmake_minimum_required(VERSION 3.16)
project(English_Listen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# 添加Qt版本可执行文件
add_executable(QtEnglishListen
    qt_main.cpp
    mainwindow.cpp
    mainwindow.h
    english_listen.rc
)

# 链接Qt6库
target_link_libraries(QtEnglishListen 
    Qt6::Core 
    Qt6::Widgets
)

# Windows特定设置
if(WIN32)
    # 将Qt应用程序设置为Windows GUI应用程序，避免显示控制台窗口
    set_target_properties(QtEnglishListen PROPERTIES WIN32_EXECUTABLE ON)
    
    # 启用Windows资源编译
    enable_language(RC)
    
    # 构建后自动签名
    add_custom_command(
        TARGET QtEnglishListen POST_BUILD
        COMMAND "D:/SignTool-10.0.22621.6-x64/signtool.exe" sign /f "D:/数字签名/English-Listen.pfx" /p DZY@013520 /t http://timestamp.digicert.com /fd SHA256 "$<TARGET_FILE:QtEnglishListen>"
        COMMENT "使用SignTool对可执行文件进行数字签名"
    )
endif()