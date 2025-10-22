#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>
#include <QWidget>
#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <windows.h>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddWord();
    void onStartTest();
    void onNextWord();
    void onAddWordsFromFile();
    void onSaveWordsToFile();
    void onRemoveWord();
    void onClearWords();
    void onToggleTheme();
    void onRepeatWord();  // 新增：重复朗读当前单词

private:
    // UI控件
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QHBoxLayout *middleLayout;
    QHBoxLayout *bottomLayout;
    
    QLabel *wordLabel;
    QLineEdit *wordInput;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *clearButton;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QPushButton *startButton;
    QPushButton *themeButton;  // 主题切换按钮
    QListWidget *wordList;
    
    // 测试界面控件
    QLabel *countdownLabel;
    QPushButton *repeatButton;  // 新增：重复朗读按钮
    QWidget *testWidget;
    
    // 功能相关
    std::vector<std::string> words;
    size_t currentIndex;
    QTimer *timer;
    int countdown;
    bool isDarkTheme;  // 当前是否为深色主题
    
    void setupUI();
    void loadWordsFromFile(const QString &filename);
    void saveWordsToFile(const QString &filename);
    void speakWord(const std::string &word);
    void showTestInterface();
    void showMainInterface();
    void toggleTheme();  // 切换主题的方法
};
#endif // MAINWINDOW_H