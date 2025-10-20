# English Listen - 英语听写练习程序

一个基于C++和Qt6开发的英语听写练习工具，支持CLI命令行版本和GUI图形界面版本。

## 功能特性

- 双版本支持：CLI命令行版本和Qt6图形界面版本
- 词库管理：添加、删除、清空单词，支持从文件导入和导出
- 听写测试：自动朗读单词，支持间隔时间设置
- 主题切换：支持浅色和深色主题，自动适配Windows系统主题
- Windows TTS集成：使用Windows SAPI进行文本朗读

## 界面预览

### CLI命令行版本
```
==========欢迎使用英语听考程序==========
[1]输入词库
[2]开始听考
请输入选项数字：
```

### Qt6图形界面版本
- 主界面：词库管理和控制按钮
- 测试界面：倒计时显示
- 主题支持：自动适配系统主题或手动切换

## 编译构建

### 环境要求
- Windows操作系统
- Qt6开发环境
- C++17或更高版本
- CMake 3.16或更高版本

### 构建步骤

1. 使用CMake构建：
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

2. 或使用qmake构建：
```bash
qmake QtEnglishListen.pro
make
```

## 使用说明

### CLI版本
1. 运行`English_Listen`可执行文件
2. 选择[1]添加词库，输入要练习的单词（输入"end"结束）
3. 选择[2]开始听写测试
4. 程序会逐个朗读单词，每个单词间隔5秒

### Qt6版本
1. 运行`QtEnglishListen`可执行文件
2. 在输入框中输入单词，点击"添加单词"按钮添加到词库
3. 点击"开始听写测试"按钮开始测试
4. 点击"切换主题"按钮可在浅色和深色主题间切换

## 文件说明

- `main.cpp` - CLI版本主程序
- `qt_main.cpp` - Qt6版本入口点
- `mainwindow.h/cpp` - Qt6版本主窗口实现
- `CMakeLists.txt` - CMake构建配置
- `QtEnglishListen.pro` - qmake项目文件
- `wordlist.txt` - 默认词库文件（程序自动生成）

## 技术实现

### 核心功能
- 使用Windows SAPI实现文本到语音功能
- 通过VBS脚本调用系统语音接口
- 支持定时器控制单词朗读间隔

### Qt6特性
- 支持Windows浅色和深色主题自动适配
- 响应式界面设计
- 信号与槽机制实现界面交互

## 许可证

MIT License

## 作者

JetCPP Studio