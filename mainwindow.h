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
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QResizeEvent>
#include <QInputDialog>
#include <QSettings>
#include <QMap>
#include <QRegularExpression>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QDirIterator>
#include <QProgressDialog>
#include <QStyleHints>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QShortcut>
#include <QKeySequence>
#include <QDateTime>
#include <QMap>
#include <cmath>
#include <windows.h>
#include <vector>
#include <string>
#include <random>  // for random shuffle

// 网络相关头文件
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

// 用户账户系统相关头文件
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QDataStream>
#include <QRandomGenerator>



// 学习结果结构体
struct TestResult {
    QDateTime timestamp;
    int totalWords;
    int correctCount;
    double accuracy;
    QString wordListName;
};

// 用户数据结构
struct UserData {
    QString username;
    QString nickname;
    QDateTime createdTime;
    QDateTime lastLoginTime;
    bool isActive;
    
    // 学习相关数据
    QStringList wordLists;
    std::vector<TestResult> testHistory;
    int totalStudyTime;  // 总学习时间（分钟）
    int completedTests;  // 完成测试次数
    
    // 设置偏好
    bool isDarkTheme;
    int readInterval;
    int speechEngine;
    bool isRandomOrder;
    
    // 隐私设置
    bool allowDataCollection;     // 是否允许数据收集
    bool allowCloudSync;          // 是否允许云同步
    bool allowAnalytics;          // 是否允许数据分析
    bool shareLearningStats;      // 是否分享学习统计数据
    
    UserData() : isActive(true), totalStudyTime(0), completedTests(0), 
                 isDarkTheme(false), readInterval(5), speechEngine(0), isRandomOrder(false),
                 allowDataCollection(false), allowCloudSync(false), 
                 allowAnalytics(false), shareLearningStats(false) {}
};

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
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

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
    void onShowAbout();        // 新增：显示关于界面
    void onShowGuide();        // 新增：显示指南界面
    void onUpdateWelcomeAnimation(); // 新增：更新欢迎语动画
    void onSelectDictationMode(); // 新增：选择听写模式
    
    // 网络下载相关槽函数
    void onDownloadFinished(QNetworkReply* reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    // UI控件
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QHBoxLayout *middleLayout;
    QHBoxLayout *bottomLayout;
    
    QLabel *welcomeLabel;           // 新增：动态欢迎语
    QLabel *userStatusLabel;        // 用户状态标签
    QPushButton *viewWordsButton;   // 新增：查看单词按钮
    QPushButton *settingsButton;    // 恢复：设置按钮
    QPushButton *startButton;
    QPushButton *themeButton;       // 主题切换按钮
    QPushButton *aboutButton;       // 关于按钮
    
    // 动画相关
    QTimer *welcomeTimer;
    int welcomeAnimationStep;
    QStringList welcomeMessages;
    
    // 界面切换动画相关
    QParallelAnimationGroup *fadeInOutGroup;
    QGraphicsOpacityEffect *homeOpacityEffect;
    QGraphicsOpacityEffect *wordsOpacityEffect;
    QGraphicsOpacityEffect *testOpacityEffect;
    QGraphicsOpacityEffect *answersOpacityEffect;
    

    
    // 加载动画相关
    QLabel *loadingLabel;
    QTimer *loadingTimer;
    int loadingAnimationStep;
    QPropertyAnimation *loadingRotationAnim;
    
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
    
    // 网络下载相关
    QNetworkAccessManager *networkManager;
    QProgressDialog *downloadProgressDialog;
    
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
    QString tempWordlistFile; // 临时词库文件路径
    int speechEngine; // 语音引擎选择 (0=SAPI, 1=Flite)
    bool isRandomOrder; // 是否随机播放单词
    
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
    void createTempWordlist(); // 创建临时词库文件
    void saveToTempWordlist(); // 保存到临时词库文件
    void loadFromTempWordlist(); // 从临时词库加载
    bool testFliteEngine(); // 测试Flite引擎是否正常工作
    
    // 学习进度相关
    std::vector<TestResult> testHistory;
    
    // 网络下载相关方法
    void downloadFlite();
    bool checkFliteExecutable();
    QString getFlitePath();  // 获取Flite可执行文件路径
    
    // 新增设置窗口函数
    void showSettingsDialog();
    
    // 随机播放相关函数
    void toggleRandomOrder();
    
    // 学习进度相关函数
    void recordTestResult(int correctCount, int totalWords, const QString& wordListName);
    void showProgressChart();
    void saveTestHistory();
    void loadTestHistory();
    QWidget* createProgressChartWidget();
    

    
    // 临时词库相关
    QTimer *wordsTextChangedTimer;  // 监控词库编辑变化的定时器
    QString lastWordsText;          // 记录上次词库文本内容
    
    // 临时词库管理函数
    void setupWordsTextEditWatcher();  // 设置词库编辑监控
    void syncWordsTextEditToTempFile(); // 同步词库编辑到临时文件
    void cleanupTempFiles();           // 清理临时文件
    void updateWordsFromTextEdit();    // 从文本框更新words向量
    
    // 动画相关函数
    void setupInterfaceAnimations();   // 设置界面动画效果
    void animateInterfaceSwitch(QWidget *fromWidget, QWidget *toWidget); // 界面切换动画
    
    // 加载动画函数
    void showLoadingAnimation(const QString &message = "加载中...");
    void hideLoadingAnimation();
    void updateLoadingAnimation();
    
    // 重写的事件处理器
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    
    // 在线听写相关
    bool isOnlineDictationMode; // 是否在线听写模式
    QLineEdit *onlineInputLineEdit; // 在线听写输入框
    QPushButton *submitAnswerButton; // 提交答案按钮
    QPushButton *nextWordButton;     // 下一个单词按钮
    QLabel *onlineStatusLabel; // 在线听写状态标签
    std::vector<std::string> originalWordsOrder; // 保存原始单词顺序
    std::vector<QString> userInputs; // 用户输入缓存
    
    // 用户账户系统相关
    QString currentUser;  // 当前登录用户
    QMap<QString, UserData> userProfiles;  // 用户配置映射
    QString userDataPath;  // 用户数据存储路径
    bool encryptionEnabled;  // 是否启用数据加密
    
    void setupOnlineDictation(); // 设置在线听写界面
    void startOnlineDictation(); // 开始在线听写
    void startPaperDictation();  // 开始纸笔听写
    void checkOnlineAnswer();    // 检查在线听写答案
    void showNextWordOnline();   // 显示下一个单词（在线模式）
    void submitAllAnswers();     // 提交所有答案
    std::vector<std::string> getWords(); // 获取当前单词列表
    
    // 用户账户系统函数
    void initializeUserSystem();  // 初始化用户系统
    void createUser(const QString& username, const QString& nickname = "");
    bool loginUser(const QString& username);
    void logoutUser();
    void saveUserProfile(const QString& username);
    void loadUserProfile(const QString& username);
    void saveAllUserProfiles();
    void loadAllUserProfiles();
    QString hashPassword(const QString& password);
    bool validateUsername(const QString& username);
    void updateCurrentUserProfile();
    
    // 数据加密相关函数
    QByteArray generateEncryptionKey();
    QByteArray encryptData(const QByteArray& data, const QByteArray& key);
    QByteArray decryptData(const QByteArray& encryptedData, const QByteArray& key);
    QString encryptString(const QString& plaintext);
    QString decryptString(const QString& ciphertext);
    void saveEncryptedUserProfile(const QString& username);
    void loadEncryptedUserProfile(const QString& username);
    
    // 用户界面相关函数
    void showUserLoginDialog();
    void showUserProfileDialog();
    void updateUserMenu();
    void onUserMenuTriggered();
    
    // 隐私设置相关函数
    void showPrivacySettingsDialog();
    void loadPrivacySettings();
    void savePrivacySettings();
};

#endif // MAINWINDOW_H