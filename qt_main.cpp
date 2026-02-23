#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    // Qt6中高DPI支持已默认启用，无需额外设置
    // 仅在Qt5环境下才需要这些属性
    
    QApplication app(argc, argv);
    
    // 设置应用程序属性
    app.setApplicationName("英语听写练习");
    app.setApplicationVersion("2.5.0");
    app.setOrganizationName("English Learning Studio");
    
    // 设置应用程序图标
    app.setWindowIcon(QIcon("logo.ico"));
    
    MainWindow window;
    window.show();
    
    return app.exec();
}