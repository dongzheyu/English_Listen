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
#include <QScrollArea>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QStackedWidget>
#include <QTextEdit>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QResizeEvent>
#include <QInputDialog>
#include <QSettings>
#include <QMap>
#include <QRegularExpression>
#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QDirIterator>
#include <cmath>
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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartTest();
    void onNextWord();
    void onAddWordsFromFile();
    void onSaveWordsToFile();
    void onRemoveWord();
    void onClearWords();
    void onToggleTheme();
    void onRepeatWord();
    void onPreviousWord();  // 新增：上一个单词
    void onNextWordClicked();  // 新增：下一个单词按钮
    void onPauseResumeTest();  // 新增：暂停/继续测试
    void onBackToMain();       // 新增：返回主界面
    void onViewWords();        // 新增：查看单词
    void onAddWord();          // 移动：添加单词到单词页面
    void onShowSettings();     // 新增：显示设置菜单
    void onUpdateWelcomeAnimation(); // 新增：更新欢迎语动画

private:
    // UI控件
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QHBoxLayout *middleLayout;
    QHBoxLayout *bottomLayout;
    
    QLabel *welcomeLabel;           // 新增：动态欢迎语
    QPushButton *viewWordsButton;   // 新增：查看单词按钮
    QPushButton *settingsButton;    // 恢复：设置按钮
    QPushButton *startButton;
    QPushButton *themeButton;       // 主题切换按钮
    
    // 动画相关
    QTimer *welcomeTimer;
    int welcomeAnimationStep;
    QStringList welcomeMessages;
    
    // 单词页面控件
    QWidget *wordsWidget;
    QVBoxLayout *wordsLayout;
    QTextEdit *wordsTextEdit;       // 用于显示和编辑单词
    QPushButton *addWordButton;     // 添加单词按钮
    QPushButton *removeWordButton;  // 删除单词按钮
    QPushButton *clearWordsButton;  // 清空单词按钮
    QPushButton *loadWordsButton;   // 从文件加载按钮
    QPushButton *saveWordsButton;   // 保存到文件按钮
    QPushButton *backToHomeButton;  // 返回主页按钮
    QLineEdit *wordInput;           // 单词输入框
    
    // 测试界面控件
    QLabel *countdownLabel;
    QPushButton *repeatButton;      // 重复朗读按钮
    QPushButton *previousButton;    // 上一个单词按钮
    QPushButton *nextButton;        // 下一个单词按钮
    QPushButton *pauseResumeButton; // 暂停/继续按钮
    QWidget *testWidget;
    
    // 答案界面控件
    QWidget *answersWidget;
    QVBoxLayout *answersLayout;
    QLabel *answersLabel;
    QPushButton *backToMainButton;
    QScrollArea *answersScrollArea;
    
    // 功能相关
    std::vector<std::string> words;
    std::vector<std::string> cachedWords; // 缓存的单词
    size_t currentIndex;
    QTimer *timer;
    int countdown;
    bool isDarkTheme;  // 当前是否为深色主题
    bool isPaused;     // 是否暂停
    int readInterval;  // 朗读时间间隔（秒）
    bool isValidWordlistDir; // 词库目录是否有效
    QString wordlistDirPath; // 词库目录路径
    QMap<QString, QString> wordlistFiles; // 词库文件列表
    
    void setupUI();
    void loadWordsFromFile(const QString &filename);
    void saveWordsToFile(const QString &filename);
    void speakWord(const std::string &word);
    void showTestInterface();
    void showMainInterface();
    void showAnswersInterface();  // 新增：显示答案界面
    void showWordsInterface();    // 新增：显示单词界面
    void toggleTheme();  // 切换主题的方法
    void updateWelcomeMessage();  // 更新欢迎语
    void startWelcomeAnimation(); // 开始欢迎语动画
    void adjustButtons();         // 调整按钮大小和布局
    void checkWordlistDirectory(); // 检查词库目录
    bool isValidWordlistFile(const QString &filePath); // 检查词库文件是否有效
    void loadSettings(); // 加载配置
    void saveSettings(); // 保存配置
    void loadWordlistFiles(); // 加载词库文件列表
};
#endif // MAINWINDOW_H