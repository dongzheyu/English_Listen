cmake_minimum_required(VERSION 3.16)
project(English_Listen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 COMPONENTS Core Widgets Multimedia REQUIRED)

# 添加可执行文件
add_executable(English_Listen
    main.cpp
)

# 添加Qt版本可执行文件
add_executable(QtEnglishListen
    qt_main.cpp
    mainwindow.cpp
    mainwindow.h
)

# 链接Qt6库
target_link_libraries(QtEnglishListen 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Multimedia
)

# Windows特定设置
if(WIN32)
    target_compile_definitions(QtEnglishListen PRIVATE UNICODE _UNICODE)
endif()