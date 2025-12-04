#include "mainwindow.h"
#include <QApplication>
#include <QStyleHints>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentIndex(0)
    , countdown(0)
    , isDarkTheme(false)
    , isPaused(false)  // 初始化暂停状态
    , readInterval(5)  // 初始化朗读时间间隔为5秒
    , isValidWordlistDir(false) // 初始化词库目录有效性为false
    , welcomeAnimationStep(0)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , topLayout(nullptr)
    , middleLayout(nullptr)
    , bottomLayout(nullptr)
    , welcomeLabel(nullptr)
    , viewWordsButton(nullptr)
    , settingsButton(nullptr)
    , startButton(nullptr)
    , themeButton(nullptr)
    , welcomeTimer(nullptr)
    , wordsWidget(nullptr)
    , wordsLayout(nullptr)
    , wordsTextEdit(nullptr)
    , addWordButton(nullptr)
    , removeWordButton(nullptr)
    , clearWordsButton(nullptr)
    , loadWordsButton(nullptr)
    , saveWordsButton(nullptr)
    , backToHomeButton(nullptr)
    , wordInput(nullptr)
    , countdownLabel(nullptr)
    , repeatButton(nullptr)
    , previousButton(nullptr)
    , nextButton(nullptr)
    , pauseResumeButton(nullptr)
    , testWidget(nullptr)
    , answersWidget(nullptr)
    , answersLayout(nullptr)
    , answersLabel(nullptr)
    , backToMainButton(nullptr)
    , answersScrollArea(nullptr)
    , timer(nullptr)
{
    // 启用高DPI支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // 检查词库目录
    checkWordlistDirectory();
    
    // 加载配置
    loadSettings();
    
    setupUI();
    
    // 确保所有控件都已创建后再连接信号和槽
    if (viewWordsButton) connect(viewWordsButton, &QPushButton::clicked, this, &MainWindow::onViewWords);
    if (settingsButton) connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onShowSettings);
    if (startButton) connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartTest);
    if (themeButton) connect(themeButton, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    
    // 监听系统主题变化
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, &MainWindow::setupUI);
    
    // 定时器初始化
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onNextWord);
    
    // 欢迎语动画定时器
    welcomeTimer = new QTimer(this);
    connect(welcomeTimer, &QTimer::timeout, this, &MainWindow::onUpdateWelcomeAnimation);
    
    // 加载默认词库
    loadWordsFromFile("wordlist.txt");
    
    // 更新欢迎语
    updateWelcomeMessage();
    
    // 开始欢迎语动画
    startWelcomeAnimation();
    
    // 调整按钮大小
    adjustButtons();
}

MainWindow::~MainWindow()
{
    // 停止并清理定时器
    if (welcomeTimer) {
        welcomeTimer->stop();
        delete welcomeTimer;
    }
    
    if (timer) {
        timer->stop();
        delete timer;
    }
}

void MainWindow::setupUI()
{
    // 如果是首次创建界面
    if (!centralWidget) {
        // 创建中央窗口部件
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // 启用高DPI支持
        setAttribute(Qt::WA_NativeWindow, true);
        
        // 创建主布局
        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(10);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        
        // 创建主页界面控件
        QWidget *homeWidget = new QWidget(this);
        homeWidget->setObjectName("homeWidget");
        QVBoxLayout *homeLayout = new QVBoxLayout(homeWidget);
        homeLayout->setSpacing(20);
        homeLayout->setContentsMargins(20, 20, 20, 20);
        
        // 创建欢迎标签
        welcomeLabel = new QLabel(this);
        welcomeLabel->setAlignment(Qt::AlignCenter);
        QFont welcomeFont = welcomeLabel->font();
        welcomeFont.setPointSize(18);
        welcomeFont.setBold(true);
        welcomeLabel->setFont(welcomeFont);
        welcomeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        // 创建按钮样式表
        QString buttonStyle = "QPushButton { "
                             "padding: 8px 16px; "
                             "margin: 4px; "
                             "border: 1px solid #cccccc; "
                             "border-radius: 4px; "
                             "background-color: #f0f0f0; "
                             "} "
                             "QPushButton:hover { "
                             "background-color: #e0e0e0; "
                             "border: 1px solid #999999; "
                             "} "
                             "QPushButton:pressed { "
                             "background-color: #d0d0d0; "
                             "border: 1px solid #666666; "
                             "} ";
        
        // 创建按钮布局
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(10);
        
        viewWordsButton = new QPushButton("查看单词", this);
        settingsButton = new QPushButton("设置", this);
        startButton = new QPushButton("开始听写测试", this);
        themeButton = new QPushButton("切换主题", this);
        
        // 设置按钮样式
        viewWordsButton->setStyleSheet(buttonStyle);
        settingsButton->setStyleSheet(buttonStyle);
        startButton->setStyleSheet(buttonStyle);
        themeButton->setStyleSheet(buttonStyle);
        
        // 设置按钮尺寸策略以支持缩放
        QSize minButtonSize(100, 30);
        viewWordsButton->setMinimumSize(minButtonSize);
        settingsButton->setMinimumSize(minButtonSize);
        startButton->setMinimumSize(minButtonSize);
        themeButton->setMinimumSize(minButtonSize);
        
        viewWordsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        settingsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        themeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        
        buttonLayout->addWidget(viewWordsButton);
        buttonLayout->addWidget(settingsButton);
        buttonLayout->addWidget(startButton);
        buttonLayout->addWidget(themeButton);
        
        homeLayout->addWidget(welcomeLabel);
        homeLayout->addLayout(buttonLayout);
        
        // 创建单词界面控件
        wordsWidget = new QWidget(this);
        wordsWidget->setObjectName("wordsWidget");
        wordsLayout = new QVBoxLayout(wordsWidget);
        wordsLayout->setSpacing(10);
        wordsLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel *wordsTitle = new QLabel("单词列表", wordsWidget);
        QFont titleFont = wordsTitle->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        wordsTitle->setFont(titleFont);
        wordsTitle->setAlignment(Qt::AlignCenter);
        
        wordsTextEdit = new QTextEdit(wordsWidget);
        wordsTextEdit->setPlaceholderText("在此处输入单词，每行一个单词");
        
        // 创建单词操作按钮
        QHBoxLayout *wordButtonLayout = new QHBoxLayout();
        wordButtonLayout->setSpacing(5);
        
        wordInput = new QLineEdit(wordsWidget);
        wordInput->setPlaceholderText("输入单词");
        
        addWordButton = new QPushButton("添加单词", wordsWidget);
        removeWordButton = new QPushButton("删除选中行", wordsWidget);
        clearWordsButton = new QPushButton("清空列表", wordsWidget);
        loadWordsButton = new QPushButton("从文件加载", wordsWidget);
        saveWordsButton = new QPushButton("保存到文件", wordsWidget);
        backToHomeButton = new QPushButton("返回主页", wordsWidget);
        
        // 设置按钮样式
        addWordButton->setStyleSheet(buttonStyle);
        removeWordButton->setStyleSheet(buttonStyle);
        clearWordsButton->setStyleSheet(buttonStyle);
        loadWordsButton->setStyleSheet(buttonStyle);
        saveWordsButton->setStyleSheet(buttonStyle);
        backToHomeButton->setStyleSheet(buttonStyle);
        
        // 设置按钮尺寸策略以支持缩放
        addWordButton->setMinimumSize(minButtonSize);
        removeWordButton->setMinimumSize(minButtonSize);
        clearWordsButton->setMinimumSize(minButtonSize);
        loadWordsButton->setMinimumSize(minButtonSize);
        saveWordsButton->setMinimumSize(minButtonSize);
        backToHomeButton->setMinimumSize(minButtonSize);
        
        wordInput->setMinimumHeight(30);
        
        wordButtonLayout->addWidget(wordInput);
        wordButtonLayout->addWidget(addWordButton);
        wordButtonLayout->addWidget(removeWordButton);
        wordButtonLayout->addWidget(clearWordsButton);
        wordButtonLayout->addWidget(loadWordsButton);
        wordButtonLayout->addWidget(saveWordsButton);
        wordButtonLayout->addWidget(backToHomeButton);
        
        wordsLayout->addWidget(wordsTitle);
        wordsLayout->addWidget(wordsTextEdit);
        wordsLayout->addLayout(wordButtonLayout);
        wordsWidget->hide();
        
        // 创建测试界面控件
        testWidget = new QWidget(this);
        testWidget->setObjectName("testWidget");
        QVBoxLayout *testLayout = new QVBoxLayout(testWidget);
        testLayout->setSpacing(10);
        testLayout->setContentsMargins(10, 10, 10, 10);
        
        countdownLabel = new QLabel("5", testWidget);
        countdownLabel->setAlignment(Qt::AlignCenter);
        QFont font = countdownLabel->font();
        font.setPointSize(48);
        font.setBold(true);
        countdownLabel->setFont(font);
        countdownLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        // 创建控制按钮布局
        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->setSpacing(10);
        
        // 创建重复朗读按钮
        repeatButton = new QPushButton("再读一遍", testWidget);
        repeatButton->setStyleSheet(buttonStyle);
        repeatButton->setMinimumSize(minButtonSize);
        repeatButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(repeatButton, &QPushButton::clicked, this, &MainWindow::onRepeatWord);
        
        // 创建上一个单词按钮
        previousButton = new QPushButton("上一个", testWidget);
        previousButton->setStyleSheet(buttonStyle);
        previousButton->setMinimumSize(minButtonSize);
        previousButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(previousButton, &QPushButton::clicked, this, &MainWindow::onPreviousWord);
        
        // 创建下一个单词按钮
        nextButton = new QPushButton("下一个", testWidget);
        nextButton->setStyleSheet(buttonStyle);
        nextButton->setMinimumSize(minButtonSize);
        nextButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(nextButton, &QPushButton::clicked, this, &MainWindow::onNextWordClicked);
        
        // 创建暂停/继续按钮
        pauseResumeButton = new QPushButton("暂停", testWidget);
        pauseResumeButton->setStyleSheet(buttonStyle);
        pauseResumeButton->setMinimumSize(minButtonSize);
        pauseResumeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(pauseResumeButton, &QPushButton::clicked, this, &MainWindow::onPauseResumeTest);
        
        // 创建退出测试按钮
        QPushButton *exitTestButton = new QPushButton("退出测试", testWidget);
        exitTestButton->setStyleSheet(buttonStyle);
        exitTestButton->setMinimumSize(minButtonSize);
        exitTestButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(exitTestButton, &QPushButton::clicked, this, [=]() {
            // 显示确认对话框
            QMessageBox msgBox;
            msgBox.setWindowTitle("确认退出");
            msgBox.setText("好学生是不会放弃的！");
            QPushButton *returnButton = msgBox.addButton("返回", QMessageBox::ActionRole);
            QPushButton *determinedButton = msgBox.addButton("去意已决", QMessageBox::ActionRole);
            msgBox.setDefaultButton(returnButton);
            
            msgBox.exec();
            
            if (msgBox.clickedButton() == determinedButton) {
                // 用户选择去意已决，停止测试并返回主界面
                if (timer) timer->stop();
                startButton->setEnabled(true);
                showMainInterface();
            }
            // 如果用户选择返回或者关闭对话框，则继续测试
        });
        
        // 将按钮添加到控制布局
        controlLayout->addWidget(previousButton);
        controlLayout->addWidget(repeatButton);
        controlLayout->addWidget(nextButton);
        controlLayout->addWidget(pauseResumeButton);
        controlLayout->addWidget(exitTestButton);
        
        testLayout->addWidget(countdownLabel);
        testLayout->addLayout(controlLayout);
        testLayout->addWidget(new QLabel("注意：点击'下一个'按钮将跳过当前单词并立即播放下一个单词", testWidget));
        testWidget->hide(); // 默认隐藏测试界面
        
        // 创建答案界面控件
        answersWidget = new QWidget(this);
        answersWidget->setObjectName("answersWidget");
        answersLayout = new QVBoxLayout(answersWidget);
        answersLayout->setSpacing(10);
        answersLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel *answersTitle = new QLabel("听写答案", answersWidget);
        titleFont = answersTitle->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        answersTitle->setFont(titleFont);
        answersTitle->setAlignment(Qt::AlignCenter);
        
        answersScrollArea = new QScrollArea(answersWidget);
        answersScrollArea->setWidgetResizable(true);
        answersLabel = new QLabel(answersWidget);
        answersLabel->setWordWrap(true);
        answersLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        answersScrollArea->setWidget(answersLabel);
        
        backToMainButton = new QPushButton("返回", answersWidget);
        backToMainButton->setStyleSheet(buttonStyle);
        backToMainButton->setMinimumSize(minButtonSize);
        backToMainButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(backToMainButton, &QPushButton::clicked, this, &MainWindow::onBackToMain);
        
        answersLayout->addWidget(answersTitle);
        answersLayout->addWidget(answersScrollArea);
        answersLayout->addWidget(backToMainButton, 0, Qt::AlignCenter);
        answersWidget->hide(); // 默认隐藏答案界面
        
        // 添加所有界面到主布局
        mainLayout->addWidget(homeWidget);
        mainLayout->addWidget(wordsWidget);
        mainLayout->addWidget(testWidget);
        mainLayout->addWidget(answersWidget);
        
        // 连接单词界面的按钮
        connect(addWordButton, &QPushButton::clicked, this, &MainWindow::onAddWord);
        connect(removeWordButton, &QPushButton::clicked, this, &MainWindow::onRemoveWord);
        connect(clearWordsButton, &QPushButton::clicked, this, &MainWindow::onClearWords);
        connect(loadWordsButton, &QPushButton::clicked, this, &MainWindow::onAddWordsFromFile);
        connect(saveWordsButton, &QPushButton::clicked, this, &MainWindow::onSaveWordsToFile);
        connect(backToHomeButton, &QPushButton::clicked, this, &MainWindow::showMainInterface);
        
        // 设置窗口属性
        setWindowTitle("英语听写练习");
        resize(600, 400);
        setMinimumSize(400, 300);
    }
    
    // 检查系统主题
    Qt::ColorScheme colorScheme = qApp->styleHints()->colorScheme();
    
    // 根据系统主题设置初始主题状态
    if (colorScheme == Qt::ColorScheme::Dark) {
        isDarkTheme = true;
    } else {
        isDarkTheme = false;
    }
    
    // 应用当前主题
    toggleTheme();
    
    // 更新欢迎语
    updateWelcomeMessage();
    
    // 调整按钮大小
    adjustButtons();
}

void MainWindow::toggleTheme()
{
    QString buttonStyleLight = "QPushButton { "
                              "padding: 8px 16px; "
                              "margin: 4px; "
                              "border: 1px solid #cccccc; "
                              "border-radius: 4px; "
                              "background-color: #f0f0f0; "
                              "} "
                              "QPushButton:hover { "
                              "background-color: #e0e0e0; "
                              "border: 1px solid #999999; "
                              "} "
                              "QPushButton:pressed { "
                              "background-color: #d0d0d0; "
                              "border: 1px solid #666666; "
                              "} ";
                              
    QString buttonStyleDark = "QPushButton { "
                             "padding: 8px 16px; "
                             "margin: 4px; "
                             "border: 1px solid #555555; "
                             "border-radius: 4px; "
                             "background-color: #404040; "
                             "color: white; "
                             "} "
                             "QPushButton:hover { "
                             "background-color: #505050; "
                             "border: 1px solid #aaaaaa; "
                             "} "
                             "QPushButton:pressed { "
                             "background-color: #606060; "
                             "border: 1px solid #cccccc; "
                             "} ";

    if (isDarkTheme) {
        // 应用深色主题
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(128, 128, 128));
        
        qApp->setPalette(darkPalette);
        
        // 应用深色主题按钮样式
        if (themeButton) {
            themeButton->setText("浅色主题");
            themeButton->setStyleSheet(buttonStyleDark);
        }
        if (viewWordsButton) viewWordsButton->setStyleSheet(buttonStyleDark);
        if (settingsButton) settingsButton->setStyleSheet(buttonStyleDark);
        if (startButton) startButton->setStyleSheet(buttonStyleDark);
        if (addWordButton) addWordButton->setStyleSheet(buttonStyleDark);
        if (removeWordButton) removeWordButton->setStyleSheet(buttonStyleDark);
        if (clearWordsButton) clearWordsButton->setStyleSheet(buttonStyleDark);
        if (loadWordsButton) loadWordsButton->setStyleSheet(buttonStyleDark);
        if (saveWordsButton) saveWordsButton->setStyleSheet(buttonStyleDark);
        if (backToHomeButton) backToHomeButton->setStyleSheet(buttonStyleDark);
        if (repeatButton) repeatButton->setStyleSheet(buttonStyleDark);
        if (previousButton) previousButton->setStyleSheet(buttonStyleDark);
        if (nextButton) nextButton->setStyleSheet(buttonStyleDark);
        if (pauseResumeButton) pauseResumeButton->setStyleSheet(buttonStyleDark);
        if (backToMainButton) backToMainButton->setStyleSheet(buttonStyleDark);
        
        // 查找并设置测试界面的退出按钮样式
        if (testWidget) {
            QList<QPushButton*> buttons = testWidget->findChildren<QPushButton*>();
            for (QPushButton* button : buttons) {
                if (button->text() == "退出测试") {
                    button->setStyleSheet(buttonStyleDark);
                }
            }
        }
    } else {
        // 应用浅色主题
        qApp->setPalette(qApp->style()->standardPalette());
        
        // 应用浅色主题按钮样式
        if (themeButton) {
            themeButton->setText("深色主题");
            themeButton->setStyleSheet(buttonStyleLight);
        }
        if (viewWordsButton) viewWordsButton->setStyleSheet(buttonStyleLight);
        if (settingsButton) settingsButton->setStyleSheet(buttonStyleLight);
        if (startButton) startButton->setStyleSheet(buttonStyleLight);
        if (addWordButton) addWordButton->setStyleSheet(buttonStyleLight);
        if (removeWordButton) removeWordButton->setStyleSheet(buttonStyleLight);
        if (clearWordsButton) clearWordsButton->setStyleSheet(buttonStyleLight);
        if (loadWordsButton) loadWordsButton->setStyleSheet(buttonStyleLight);
        if (saveWordsButton) saveWordsButton->setStyleSheet(buttonStyleLight);
        if (backToHomeButton) backToHomeButton->setStyleSheet(buttonStyleLight);
        if (repeatButton) repeatButton->setStyleSheet(buttonStyleLight);
        if (previousButton) previousButton->setStyleSheet(buttonStyleLight);
        if (nextButton) nextButton->setStyleSheet(buttonStyleLight);
        if (pauseResumeButton) pauseResumeButton->setStyleSheet(buttonStyleLight);
        if (backToMainButton) backToMainButton->setStyleSheet(buttonStyleLight);
        
        // 查找并设置测试界面的退出按钮样式
        if (testWidget) {
            QList<QPushButton*> buttons = testWidget->findChildren<QPushButton*>();
            for (QPushButton* button : buttons) {
                if (button->text() == "退出测试") {
                    button->setStyleSheet(buttonStyleLight);
                }
            }
        }
    }
}

void MainWindow::showMainInterface()
{
    // 隐藏其他界面
    if (wordsWidget) wordsWidget->hide();
    if (testWidget) testWidget->hide();
    if (answersWidget) answersWidget->hide();
    
    // 显示主页界面元素
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    if (homeWidget) homeWidget->show();
    
    // 更新欢迎语
    updateWelcomeMessage();
    
    // 启动欢迎语动画
    startWelcomeAnimation();
}

void MainWindow::showWordsInterface()
{
    // 隐藏其他界面
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    if (homeWidget) homeWidget->hide();
    if (testWidget) testWidget->hide();
    if (answersWidget) answersWidget->hide();
    
    // 显示单词界面
    if (wordsWidget) wordsWidget->show();
    
    // 更新单词列表显示
    QString wordsText;
    for (const auto& word : words) {
        wordsText += QString::fromStdString(word) + "\n";
    }
    if (wordsTextEdit) wordsTextEdit->setPlainText(wordsText);
}

void MainWindow::showTestInterface()
{
    // 隐藏其他界面
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    if (homeWidget) homeWidget->hide();
    if (wordsWidget) wordsWidget->hide();
    if (answersWidget) answersWidget->hide();
    
    // 显示测试界面
    if (testWidget) testWidget->show();
}

void MainWindow::showAnswersInterface()
{
    // 停止定时器
    if (timer) timer->stop();
    
    // 隐藏其他界面
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    if (homeWidget) homeWidget->hide();
    if (wordsWidget) wordsWidget->hide();
    if (testWidget) testWidget->hide();
    
    // 构建答案文本
    QString answersText;
    for (size_t i = 0; i < words.size(); ++i) {
        answersText += QString::number(i + 1) + ". " + QString::fromStdString(words[i]) + "\n";
    }
    
    // 设置答案文本
    if (answersLabel) answersLabel->setText(answersText);
    
    // 显示答案界面
    if (answersWidget) answersWidget->show();
}

void MainWindow::onViewWords()
{
    // 如果词库目录有效且有可用的词库文件，让用户选择加载
    if (isValidWordlistDir && !wordlistFiles.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("加载词库");
        msgBox.setText("检测到可用的词库文件，是否加载？");
        QPushButton *loadButton = msgBox.addButton("加载词库", QMessageBox::ActionRole);
        QPushButton *skipButton = msgBox.addButton("跳过", QMessageBox::ActionRole);
        msgBox.setDefaultButton(skipButton);
        
        msgBox.exec();
        
        if (msgBox.clickedButton() == loadButton) {
            // 显示文件选择对话框，允许多选
            QStringList fileNames = QFileDialog::getOpenFileNames(
                this,
                "选择词库文件",
                wordlistDirPath,
                "Text Files (*.txt)"
            );
            
            if (!fileNames.isEmpty()) {
                // 清空当前词库
                words.clear();
                cachedWords.clear(); // 同时清空缓存
                
                // 加载选定的词库文件
                for (const QString &fileName : fileNames) {
                    loadWordsFromFile(fileName);
                }
            }
        }
    }
    
    showWordsInterface();
}

void MainWindow::onShowSettings()
{
    QMenu *menu = new QMenu(this);
    QAction *themeAction = menu->addAction("切换主题");
    connect(themeAction, &QAction::triggered, this, &MainWindow::onToggleTheme);
    
    QAction *intervalAction = menu->addAction("朗读时间间隔设置");
    connect(intervalAction, &QAction::triggered, this, [=]() {
        bool ok;
        int interval = QInputDialog::getInt(this, "朗读时间间隔设置", 
                                          "请输入朗读时间间隔(秒):", 
                                          readInterval, 1, 60, 1, &ok);
        if (ok) {
            readInterval = interval;
            QMessageBox::information(this, "设置成功", 
                                   QString("朗读时间间隔已设置为 %1 秒").arg(readInterval));
            // 保存设置
            saveSettings();
        }
    });
    
    menu->popup(QCursor::pos());
}

void MainWindow::onAddWord()
{
    if (!wordInput) return;
    
    QString word = wordInput->text().trimmed();
    if (!word.isEmpty()) {
        // 添加到缓存和主词库
        cachedWords.push_back(word.toStdString());
        words.push_back(word.toStdString());
        wordInput->clear();
        
        // 更新单词列表显示
        QString wordsText;
        for (const auto& w : words) {
            wordsText += QString::fromStdString(w) + "\n";
        }
        if (wordsTextEdit) wordsTextEdit->setPlainText(wordsText);
    }
}

void MainWindow::onRemoveWord()
{
    if (!wordsTextEdit) return;
    
    QTextCursor cursor = wordsTextEdit->textCursor();
    if (cursor.hasSelection()) {
        // 删除选中的文本
        cursor.removeSelectedText();
    } else {
        // 删除光标所在行
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        // 删除换行符
        if (!cursor.atStart()) {
            cursor.deletePreviousChar();
        }
    }
    
    // 更新单词向量和缓存
    words.clear();
    cachedWords.clear();
    QString text = wordsTextEdit->toPlainText();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.isEmpty()) {
            words.push_back(trimmedLine.toStdString());
            cachedWords.push_back(trimmedLine.toStdString());
        }
    }
}

void MainWindow::onClearWords()
{
    words.clear();
    cachedWords.clear();
    if (wordsTextEdit) wordsTextEdit->clear();
}

void MainWindow::onAddWordsFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择词库文件", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        loadWordsFromFile(fileName);
    }
}

void MainWindow::onSaveWordsToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存词库文件", "wordlist.txt", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        saveWordsToFile(fileName);
        QMessageBox::information(this, "提示", "词库保存成功");
    }
}

void MainWindow::onToggleTheme()
{
    isDarkTheme = !isDarkTheme;
    toggleTheme();
    
    // 保存主题设置
    saveSettings();
}

void MainWindow::onStartTest()
{
    if (words.empty()) {
        QMessageBox::warning(this, "警告", "词库为空，请先添加单词");
        return;
    }
    
    currentIndex = 0;
    isPaused = false;  // 重置暂停状态
    startButton->setEnabled(false);
    
    // 更新暂停按钮文本
    if (pauseResumeButton) pauseResumeButton->setText("暂停");
    
    // 切换到测试界面
    showTestInterface();
    
    // 设置初始倒计时为1秒，用于第一个单词的延迟
    countdown = 1;
    if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    if (timer) timer->start(1000);
}

void MainWindow::onNextWord()
{
    // 如果处于暂停状态，则不执行任何操作
    if (isPaused) {
        return;
    }
    
    countdown--;
    if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    
    if (countdown <= 0) {
        if (currentIndex < words.size()) {
            // 显示正在朗读提示
            if (countdownLabel) countdownLabel->setText("正在朗读");
            
            // 处理界面事件，确保标签更新
            QCoreApplication::processEvents();
            
            // 朗读当前单词（隐藏CMD窗口）
            speakWord(words[currentIndex]);
        }
        
        currentIndex++;
        if (currentIndex < words.size()) {
            // 设置下一词的朗读倒计时，使用用户设置的时间间隔
            countdown = readInterval;
            if (countdownLabel) countdownLabel->setText(QString::number(countdown));
        } else {
            if (timer) timer->stop();
            startButton->setEnabled(true);
            // 测试结束，显示带有"直接返回"和"显示答案"按钮的对话框
            QMessageBox msgBox;
            msgBox.setWindowTitle("提示");
            msgBox.setText("听写测试结束");
            QPushButton *directReturnButton = msgBox.addButton("直接返回", QMessageBox::ActionRole);
            QPushButton *showAnswersButton = msgBox.addButton("显示答案", QMessageBox::ActionRole);
            // 不再添加OK按钮
            msgBox.setDefaultButton(directReturnButton);
            
            msgBox.exec();
            
            if (msgBox.clickedButton() == directReturnButton) {
                // 用户选择了"直接返回"，返回主界面
                showMainInterface();
            } else if (msgBox.clickedButton() == showAnswersButton) {
                // 用户选择了"显示答案"，显示答案界面
                showAnswersInterface();
            }
            // 移除了OK按钮的处理逻辑
        }
    }
}

void MainWindow::onRepeatWord()
{
    // 确保当前索引有效
    if (currentIndex < words.size()) {
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");
        
        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();
        
        // 朗读当前单词
        speakWord(words[currentIndex]);
        
        // 重置倒计时，使用用户设置的时间间隔
        countdown = readInterval;
        if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    }
}

void MainWindow::onPreviousWord()
{
    // 确保不是第一个单词
    if (currentIndex > 0) {
        currentIndex--;
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");
        
        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();
        
        // 朗读当前单词
        speakWord(words[currentIndex]);
        
        // 重置倒计时，使用用户设置的时间间隔
        countdown = readInterval;
        if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    }
}

void MainWindow::onNextWordClicked()
{
    // 确保不是最后一个单词
    if (currentIndex < words.size() - 1) {
        currentIndex++;
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");
        
        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();
        
        // 朗读当前单词
        speakWord(words[currentIndex]);
        
        // 重置倒计时，使用用户设置的时间间隔
        countdown = readInterval;
        if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    } else if (currentIndex == words.size() - 1) {
        // 如果是最后一个单词，直接结束测试
        if (timer) timer->stop();
        startButton->setEnabled(true);
        // 测试结束，显示带有"直接返回"和"显示答案"按钮的对话框
        QMessageBox msgBox;
        msgBox.setWindowTitle("提示");
        msgBox.setText("听写测试结束");
        QPushButton *directReturnButton = msgBox.addButton("直接返回", QMessageBox::ActionRole);
        QPushButton *showAnswersButton = msgBox.addButton("显示答案", QMessageBox::ActionRole);
        // 不再添加OK按钮
        msgBox.setDefaultButton(directReturnButton);
        
        msgBox.exec();
        
        if (msgBox.clickedButton() == directReturnButton) {
            // 用户选择了"直接返回"，返回主界面
            showMainInterface();
        } else if (msgBox.clickedButton() == showAnswersButton) {
            // 用户选择了"显示答案"，显示答案界面
            showAnswersInterface();
        }
        // 移除了OK按钮的处理逻辑
    }
}

void MainWindow::onPauseResumeTest()
{
    if (!isPaused) {
        // 暂停测试
        if (timer) timer->stop();
        isPaused = true;
        if (pauseResumeButton) pauseResumeButton->setText("继续");
    } else {
        // 继续测试
        if (timer) timer->start(1000);
        isPaused = false;
        if (pauseResumeButton) pauseResumeButton->setText("暂停");
    }
}

void MainWindow::onBackToMain()
{
    // 隐藏答案界面
    if (answersWidget) answersWidget->hide();
    
    // 显示主界面
    showMainInterface();
    
    // 重新启用开始按钮
    if (startButton) startButton->setEnabled(true);
}

void MainWindow::checkWordlistDirectory()
{
    // 获取当前目录
    QString currentDir = QDir::currentPath();
    wordlistDirPath = currentDir + "/wordlist";
    
    // 检查wordlist目录是否存在，如果不存在则创建
    QDir dir(wordlistDirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create wordlist directory";
            isValidWordlistDir = false;
            return;
        }
    }
    
    isValidWordlistDir = true;
    
    // 加载词库文件列表
    loadWordlistFiles();
}

void MainWindow::loadWordlistFiles()
{
    wordlistFiles.clear();
    
    if (!isValidWordlistDir) {
        return;
    }
    
    QDir dir(wordlistDirPath);
    QStringList filters;
    filters << "*.txt";
    dir.setNameFilters(filters);
    
    QFileInfoList fileList = dir.entryInfoList(QDir::Files, QDir::Name);
    
    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        if (isValidWordlistFile(filePath)) {
            wordlistFiles[fileInfo.fileName()] = filePath;
        }
    }
}

bool MainWindow::isValidWordlistFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    int lineCount = 0;
    
    while (!in.atEnd() && lineCount < 100) { // 限制检查前100行
        QString line = in.readLine().trimmed();
        lineCount++;
        
        // 如果行不为空但包含非字母字符（除了空格和连字符），则可能是无效的词库文件
        if (!line.isEmpty()) {
            // 检查是否只包含字母、空格和连字符
            QRegExp regex("^[a-zA-Z\\s\\-']+$");
            if (!regex.exactMatch(line)) {
                // 允许包含一些标点符号，但不能是特殊格式
                if (line.contains(QRegExp("[0-9{}\\[\\]<>]"))) {
                    file.close();
                    return false;
                }
            }
        }
    }
    
    file.close();
    return lineCount > 0; // 至少有一行有效内容
}

void MainWindow::loadWordsFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            words.push_back(line.toStdString());
            // 所有加载的单词都添加到缓存中（除了内置的wordlist.txt）
            if (!filename.endsWith("wordlist.txt") || filename != "wordlist.txt") {
                cachedWords.push_back(line.toStdString());
            }
        }
    }
    
    file.close();
    
    // 更新单词列表显示
    if (wordsTextEdit) {
        QString wordsText;
        for (const auto& word : words) {
            wordsText += QString::fromStdString(word) + "\n";
        }
        wordsTextEdit->setPlainText(wordsText);
    }
}

void MainWindow::saveWordsToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件");
        return;
    }
    
    QTextStream out(&file);
    for (const auto &word : words) {
        out << QString::fromStdString(word) << "\n";
    }
    
    file.close();
}

void MainWindow::loadSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    
    // 加载主题设置
    isDarkTheme = settings.value("theme/isDark", false).toBool();
    
    // 加载朗读时间间隔设置
    readInterval = settings.value("test/readInterval", 5).toInt();
    
    // 确保时间间隔在合理范围内
    if (readInterval < 1 || readInterval > 60) {
        readInterval = 5;
    }
}

void MainWindow::saveSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    
    // 保存主题设置
    settings.setValue("theme/isDark", isDarkTheme);
    
    // 保存朗读时间间隔设置
    settings.setValue("test/readInterval", readInterval);
    
    // 确保数据写入磁盘
    settings.sync();
}

void MainWindow::updateWelcomeMessage()
{
    if (!welcomeLabel) return;
    
    QDateTime currentTime = QDateTime::currentDateTime();
    int hour = currentTime.time().hour();
    
    QString greeting;
    if (hour >= 5 && hour < 12) {
        greeting = "早上好";
    } else if (hour >= 12 && hour < 18) {
        greeting = "下午好";
    } else if (hour >= 18 && hour < 22) {
        greeting = "晚上好";
    } else {
        greeting = "夜深了";
    }
    
    QString welcomeMessage = QString("%1！欢迎使用英语听写练习系统").arg(greeting);
    welcomeLabel->setText(welcomeMessage);
}

void MainWindow::startWelcomeAnimation()
{
    if (welcomeTimer) {
        welcomeTimer->start(100); // 每100毫秒更新一次动画
    }
}

void MainWindow::onUpdateWelcomeAnimation()
{
    if (!welcomeLabel) return;
    
    // 更新动画步骤
    welcomeAnimationStep = (welcomeAnimationStep + 1) % 360;
    
    // 计算彩虹色渐变
    double hue = welcomeAnimationStep * 1.0;
    int r, g, b;
    
    // 将HSV转换为RGB
    double c = 1.0; // 饱和度
    double v = 1.0; // 明度
    
    double h = hue / 60.0;
    double x = c * (1 - fabs(fmod(h, 2) - 1));
    
    double r1, g1, b1;
    if (h >= 0 && h < 1) { r1 = c; g1 = x; b1 = 0; }
    else if (h >= 1 && h < 2) { r1 = x; g1 = c; b1 = 0; }
    else if (h >= 2 && h < 3) { r1 = 0; g1 = c; b1 = x; }
    else if (h >= 3 && h < 4) { r1 = 0; g1 = x; b1 = c; }
    else if (h >= 4 && h < 5) { r1 = x; g1 = 0; b1 = c; }
    else { r1 = c; g1 = 0; b1 = x; }
    
    double m = v - c;
    r = (int)((r1 + m) * 150 + 105); // 调整亮度范围
    g = (int)((g1 + m) * 150 + 105);
    b = (int)((b1 + m) * 150 + 105);
    
    // 计算透明度变化
    const double PI = 3.14159265358979323846;
    int alpha = 200 + (int)(55.0 * (1 + sin(welcomeAnimationStep * PI / 90.0)) / 2);
    
    // 应用颜色和透明度
    QString stylesheet = QString("QLabel { color: rgba(%1, %2, %3, %4); }").arg(r).arg(g).arg(b).arg(alpha);
    welcomeLabel->setStyleSheet(stylesheet);
    
    // 计算字体大小变化（更细微的变化）
    int fontSize = 18 + (int)(3.0 * (1 + sin(welcomeAnimationStep * PI / 90.0)) / 2);
    QFont font = welcomeLabel->font();
    font.setPointSize(fontSize);
    font.setBold(true);
    welcomeLabel->setFont(font);
    
    // 计算位置偏移（更轻微的浮动效果）
    int offset = (int)(5.0 * sin(welcomeAnimationStep * PI / 45.0));
    welcomeLabel->move(width()/2 - welcomeLabel->width()/2, 50 + offset);
}

void MainWindow::speakWord(const std::string &word)
{
    // 清理输入文本，只保留安全的字符
    QString qword = QString::fromStdString(word);
    QString cleanedWord;
    
    for (const QChar& c : qword) {
        // 只保留安全的可打印字符
        if (c.unicode() >= 32 && c.unicode() <= 126) {
            // 特殊处理双引号
            if (c == '"') {
                cleanedWord.append("\"\"");
            } else {
                cleanedWord.append(c);
            }
        }
    }
    
    // 如果清理后的字符串为空，则使用默认文本
    if (cleanedWord.trimmed().isEmpty()) {
        cleanedWord = "empty";
    }
    
    // 构建VBScript代码
    QString vbsCode = "Dim voice\n";
    vbsCode += "Set voice = CreateObject(\"SAPI.SpVoice\")\n";
    vbsCode += "voice.Speak \"" + cleanedWord + "\"\n";
    
    // 创建临时VBS文件
    QString tempPath = QDir::tempPath();
    QString vbsFile = tempPath + "\\temp_speak.vbs";
    
    QFile file(vbsFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << vbsCode;
        file.close();
        
        // 执行VBS脚本
        if (QFile::exists(vbsFile)) {
            QString program = "wscript.exe";
            QStringList arguments;
            arguments << "//nologo" << vbsFile;
            
            QProcess process;
            process.start(program, arguments);
            process.waitForFinished(5000); // 最多等待5秒
            
            // 删除临时文件
            QFile::remove(vbsFile);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    
    // 更新欢迎语标签的位置，确保居中
    if (welcomeLabel && welcomeLabel->isVisible()) {
        welcomeLabel->move(width()/2 - welcomeLabel->width()/2, 50);
    }
    
    // 调整按钮大小以适应窗口宽度
    adjustButtons();
}

void MainWindow::adjustButtons()
{
    // 获取当前窗口宽度
    int windowWidth = width();
    
    // 调整主页按钮
    if (viewWordsButton && settingsButton && startButton && themeButton) {
        // 计算按钮总宽度和间距
        int totalButtonWidth = viewWordsButton->minimumWidth() * 4;
        int totalSpacing = 10 * 3; // 4个按钮间有3个间距
        int totalWidth = totalButtonWidth + totalSpacing + 40; // 加上左右边距
        
        // 如果总宽度超过窗口宽度，则缩小按钮
        if (totalWidth > windowWidth) {
            // 计算新的按钮宽度
            int newWidth = (windowWidth - totalSpacing - 40) / 4;
            int adjustedWidth = qMax(60, newWidth); // 确保按钮不会太小
            
            viewWordsButton->setMinimumWidth(adjustedWidth);
            settingsButton->setMinimumWidth(adjustedWidth);
            startButton->setMinimumWidth(adjustedWidth);
            themeButton->setMinimumWidth(adjustedWidth);
        } else {
            // 窗口足够大，恢复默认宽度
            viewWordsButton->setMinimumWidth(100);
            settingsButton->setMinimumWidth(100);
            startButton->setMinimumWidth(100);
            themeButton->setMinimumWidth(100);
        }
    }
    
    // 调整单词界面按钮（类似处理）
    if (addWordButton && removeWordButton && clearWordsButton && 
        loadWordsButton && saveWordsButton && backToHomeButton) {
        // 这些按钮在 QHBoxLayout 中，Qt 会自动处理布局
        // 如果需要特殊处理，可以在这里添加
    }
    
    // 调整测试界面按钮（类似处理）
    if (repeatButton && previousButton && nextButton && pauseResumeButton) {
        // 这些按钮在 QHBoxLayout 中，Qt 会自动处理布局
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 保存设置
    saveSettings();
    
    // 检查是否有缓存的单词
    if (!cachedWords.empty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("保存词库");
        msgBox.setText("是否保存本次听写的词库？");
        QPushButton *saveButton = msgBox.addButton("保存", QMessageBox::ActionRole);
        QPushButton *discardButton = msgBox.addButton("丢弃", QMessageBox::ActionRole);
        msgBox.setDefaultButton(discardButton);
        
        msgBox.exec();
        
        if (msgBox.clickedButton() == saveButton) {
            // 生成带日期的文件名
            QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
            QString fileName = QString("wordlist_%1.txt").arg(currentDate);
            QString filePath = wordlistDirPath + "/" + fileName;
            
            // 保存词库
            saveWordsToFile(filePath);
            
            QMessageBox::information(this, "保存成功", 
                                   QString("词库已保存至: %1").arg(filePath));
        }
    }
    
    event->accept();
}
