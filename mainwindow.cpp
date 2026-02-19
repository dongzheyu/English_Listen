#include "mainwindow.h"
#include <QProgressDialog>
#include <QKeyEvent>
#include <QStyleHints>
#include <QtCharts/QChart>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QTimer>
#include <QShowEvent>
#include <QHideEvent>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTimer>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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
    , aboutButton(nullptr)
    , welcomeTimer(nullptr)
    , welcomeAnimationStep(0)
    , fadeInOutGroup(nullptr)
    , homeOpacityEffect(nullptr)
    , wordsOpacityEffect(nullptr)
    , testOpacityEffect(nullptr)
    , answersOpacityEffect(nullptr)
    , loadingLabel(nullptr)
    , loadingTimer(nullptr)
    , loadingAnimationStep(0)
    , loadingRotationAnim(nullptr)
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
    , networkManager(nullptr)
    , downloadProgressDialog(nullptr)
    , currentIndex(0)
    , timer(nullptr)
    , countdown(5)
    , isDarkTheme(false)
    , isPaused(false)
    , readInterval(5)
    , isValidWordlistDir(false)
    , wordlistDirPath("./wordlist")
    , tempWordlistFile("")
    , isRandomOrder(false)
    , wordsTextChangedTimer(nullptr)
    , lastWordsText("")
    , isOnlineDictationMode(false)
    , onlineInputLineEdit(nullptr)
    , submitAnswerButton(nullptr)
    , onlineStatusLabel(nullptr)
    , currentUser("")
    , userDataPath("")
    , encryptionEnabled(true)
    , speechEngine(0)
{
    // 设置窗口图标
    setWindowIcon(QIcon("logo.ico"));
    
    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onDownloadFinished);

    // 初始化用户系统
    initializeUserSystem();
    
    // 加载设置
    loadSettings();
    
    // 初始化用户菜单
    updateUserMenu();
    
    // 初始化用户菜单
    updateUserMenu();
    
    setupUI();
    checkWordlistDirectory();
    loadWordlistFiles();
    createTempWordlist();
    // 注释掉这行，避免在启动时自动加载临时词库文件
    // loadFromTempWordlist();
    startWelcomeAnimation();
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
    
    // 清理动画组
    if (fadeInOutGroup) {
        delete fadeInOutGroup;
    }
    
    // 清理加载动画
    if (loadingTimer) {
        loadingTimer->stop();
        delete loadingTimer;
    }
    if (loadingRotationAnim) {
        delete loadingRotationAnim;
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
        welcomeFont.setFamily("Microsoft YaHei"); // 使用微软雅黑字体
        welcomeFont.setPointSize(18);
        welcomeFont.setBold(true);
        welcomeLabel->setFont(welcomeFont);
        welcomeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // 创建用户状态标签
        userStatusLabel = new QLabel("👤 未登录", this);
        userStatusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QFont statusFont = userStatusLabel->font();
        statusFont.setFamily("Microsoft YaHei");
        statusFont.setPointSize(10);
        userStatusLabel->setFont(statusFont);
        userStatusLabel->setStyleSheet("color: gray; padding: 5px;");
        userStatusLabel->setCursor(Qt::PointingHandCursor);
        
        // 为用户状态标签安装事件过滤器
        userStatusLabel->installEventFilter(this);

        // 创建按钮样式表
        QString buttonStyle = "QPushButton { "
                             "font-family: 'Microsoft YaHei'; "
                             "font-size: 9pt; "
                             "padding: 8px 16px; "
                             "margin: 4px; "
                             "border: 2px solid #555555; "
                             "border-radius: 6px; "
                             "background-color: #ffffff; "
                             "color: #333333; "
                             "transition: all 0.3s ease; "
                             "} "
                             "QPushButton:hover { "
                             "background-color: #e0e0e0; "
                             "border: 2px solid #333333; "
                             "transform: translateY(-2px); "
                             "box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); "
                             "} "
                             "QPushButton:pressed { "
                             "background-color: #d0d0d0; "
                             "border: 2px solid #000000; "
                             "transform: translateY(0); "
                             "box-shadow: none; "
                             "} "
                             "QPushButton:disabled { "
                             "background-color: #f0f0f0; "
                             "color: #999999; "
                             "border: 2px solid #cccccc; "
                             "transform: none; "
                             "box-shadow: none; "
                             "} ";

        // 创建按钮布局
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(15);  // 增加按钮间距
        buttonLayout->setContentsMargins(20, 10, 20, 10);  // 添加布局边距

        viewWordsButton = new QPushButton("查看单词", this);
        settingsButton = new QPushButton("设置", this);
        startButton = new QPushButton("开始听写测试", this);
        themeButton = new QPushButton("切换主题", this);
        aboutButton = new QPushButton("关于", this);
        QPushButton *guideButton = new QPushButton("使用指南", this);

        // 设置按钮样式
        viewWordsButton->setStyleSheet(buttonStyle);
        settingsButton->setStyleSheet(buttonStyle);
        startButton->setStyleSheet(buttonStyle);
        themeButton->setStyleSheet(buttonStyle);
        aboutButton->setStyleSheet(buttonStyle);
        guideButton->setStyleSheet(buttonStyle);

        // 设置按钮尺寸策略以支持缩放
        QSize minButtonSize(120, 35);  // 增加按钮最小尺寸
        viewWordsButton->setMinimumSize(minButtonSize);
        settingsButton->setMinimumSize(minButtonSize);
        startButton->setMinimumSize(minButtonSize);
        themeButton->setMinimumSize(minButtonSize);
        aboutButton->setMinimumSize(minButtonSize);
        guideButton->setMinimumSize(minButtonSize);

        viewWordsButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        settingsButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        startButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        themeButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        aboutButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        guideButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

        buttonLayout->addWidget(viewWordsButton);
        buttonLayout->addWidget(settingsButton);
        buttonLayout->addWidget(startButton);
        buttonLayout->addWidget(themeButton);
        buttonLayout->addWidget(aboutButton);
        buttonLayout->addWidget(guideButton);

        // 创建顶部布局（欢迎标签和用户状态）
        QHBoxLayout *topLayout = new QHBoxLayout();
        topLayout->addWidget(welcomeLabel);
        topLayout->addWidget(userStatusLabel);
        topLayout->setStretch(0, 1); // 欢迎标签占更多空间
        
        homeLayout->addLayout(topLayout);
        homeLayout->addLayout(buttonLayout);

        // 创建单词界面控件
        wordsWidget = new QWidget(this);
        wordsWidget->setObjectName("wordsWidget");
        wordsLayout = new QVBoxLayout(wordsWidget);
        wordsLayout->setSpacing(10);
        wordsLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *wordsTitle = new QLabel("单词列表", wordsWidget);
        QFont titleFont = wordsTitle->font();
        titleFont.setFamily("Microsoft YaHei"); // 使用微软雅黑字体
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        wordsTitle->setFont(titleFont);
        wordsTitle->setAlignment(Qt::AlignCenter);

        // 创建搜索和筛选区域
        QWidget *searchFilterWidget = new QWidget(wordsWidget);
        QHBoxLayout *searchFilterLayout = new QHBoxLayout(searchFilterWidget);
        searchFilterLayout->setSpacing(10);
        
        QLabel *searchLabel = new QLabel("搜索:", wordsWidget);
        QLineEdit *searchLineEdit = new QLineEdit(wordsWidget);
        searchLineEdit->setPlaceholderText("输入单词进行搜索");
        searchLineEdit->setMinimumWidth(200);
        
        QLabel *filterLabel = new QLabel("筛选:", wordsWidget);
        QComboBox *filterComboBox = new QComboBox(wordsWidget);
        filterComboBox->addItem("全部单词");
        filterComboBox->addItem("以字母开头");
        filterComboBox->addItem("以数字开头");
        filterComboBox->addItem("包含特殊字符");
        
        QPushButton *resetButton = new QPushButton("重置", wordsWidget);
        resetButton->setStyleSheet(buttonStyle);
        
        searchFilterLayout->addWidget(searchLabel);
        searchFilterLayout->addWidget(searchLineEdit);
        searchFilterLayout->addWidget(filterLabel);
        searchFilterLayout->addWidget(filterComboBox);
        searchFilterLayout->addWidget(resetButton);
        searchFilterLayout->addStretch();

        wordsTextEdit = new QTextEdit(wordsWidget);
        wordsTextEdit->setPlaceholderText("在此处输入单词，每行一个单词");
        
        // 设置词库编辑框监控
        setupWordsTextEditWatcher();
        
        // 连接搜索和筛选信号
        connect(searchLineEdit, &QLineEdit::textChanged, [=]() {
            QString searchText = searchLineEdit->text();
            QString filterText = filterComboBox->currentText();
            
            // 保存原始内容
            static QString originalText = wordsTextEdit->toPlainText();
            if (searchText.isEmpty() && filterText == "全部单词") {
                wordsTextEdit->setPlainText(originalText);
                return;
            }
            
            if (searchText.isEmpty() && originalText.isEmpty()) {
                originalText = wordsTextEdit->toPlainText();
            }
            
            // 筛选单词
            QStringList lines = originalText.split('\n');
            QStringList filteredLines;
            
            for (const QString &line : lines) {
                if (line.trimmed().isEmpty()) continue;
                
                // 应用搜索
                if (!searchText.isEmpty() && !line.contains(searchText, Qt::CaseInsensitive)) {
                    continue;
                }
                
                // 应用筛选
                if (filterText == "以字母开头" && !line.isEmpty() && !((line.at(0) >= 'a' && line.at(0) <= 'z') || (line.at(0) >= 'A' && line.at(0) <= 'Z'))) {
                    continue;
                } else if (filterText == "以数字开头" && !line.isEmpty() && !(line.at(0) >= '0' && line.at(0) <= '9')) {
                    continue;
                } else if (filterText == "包含特殊字符" && !line.contains(QRegularExpression("[^a-zA-Z0-9\\s]"))) {
                    continue;
                }
                
                filteredLines.append(line);
            }
            
            wordsTextEdit->setPlainText(filteredLines.join('\n'));
        });
        
        connect(filterComboBox, &QComboBox::currentTextChanged, [=]() {
            // 触发搜索框的文本变化信号
            QString searchText = searchLineEdit->text();
            if (searchText.isEmpty()) {
                searchLineEdit->setText(" ");
                searchLineEdit->setText("");
            } else {
                searchLineEdit->textChanged(searchText);
            }
        });
        
        connect(resetButton, &QPushButton::clicked, [=]() {
            searchLineEdit->clear();
            filterComboBox->setCurrentIndex(0);
        });

        // 创建单词操作按钮容器（垂直布局，包含两行按钮）
        QVBoxLayout *wordButtonContainer = new QVBoxLayout();
        wordButtonContainer->setSpacing(10);

        wordInput = new QLineEdit(wordsWidget);
        wordInput->setPlaceholderText("输入单词");
        wordInput->setMinimumHeight(30);
        wordInput->setMinimumWidth(150);  // 设置输入框最小宽度

        addWordButton = new QPushButton("添加单词", wordsWidget);
        removeWordButton = new QPushButton("删除选中行", wordsWidget);
        clearWordsButton = new QPushButton("清空列表", wordsWidget);
        loadWordsButton = new QPushButton("从文件加载", wordsWidget);
        saveWordsButton = new QPushButton("保存到文件", wordsWidget);
        QPushButton *importWordsButton = new QPushButton("导入词库", wordsWidget);
        QPushButton *exportWordsButton = new QPushButton("导出词库", wordsWidget);
        backToHomeButton = new QPushButton("返回主页", wordsWidget);

        // 设置按钮样式
        addWordButton->setStyleSheet(buttonStyle);
        removeWordButton->setStyleSheet(buttonStyle);
        clearWordsButton->setStyleSheet(buttonStyle);
        loadWordsButton->setStyleSheet(buttonStyle);
        saveWordsButton->setStyleSheet(buttonStyle);
        importWordsButton->setStyleSheet(buttonStyle);
        exportWordsButton->setStyleSheet(buttonStyle);
        backToHomeButton->setStyleSheet(buttonStyle);

        // 设置按钮尺寸策略以支持缩放
        addWordButton->setMinimumSize(minButtonSize);
        removeWordButton->setMinimumSize(minButtonSize);
        clearWordsButton->setMinimumSize(minButtonSize);
        loadWordsButton->setMinimumSize(minButtonSize);
        saveWordsButton->setMinimumSize(minButtonSize);
        importWordsButton->setMinimumSize(minButtonSize);
        exportWordsButton->setMinimumSize(minButtonSize);
        backToHomeButton->setMinimumSize(minButtonSize);

        // 创建两行按钮布局，避免按钮过多导致重叠
        QHBoxLayout *firstRowLayout = new QHBoxLayout();
        firstRowLayout->setSpacing(10);
        firstRowLayout->addWidget(wordInput);
        firstRowLayout->addWidget(addWordButton);
        firstRowLayout->addWidget(removeWordButton);
        firstRowLayout->addWidget(clearWordsButton);
        firstRowLayout->addStretch();
        
        QHBoxLayout *secondRowLayout = new QHBoxLayout();
        secondRowLayout->setSpacing(10);
        secondRowLayout->addWidget(loadWordsButton);
        secondRowLayout->addWidget(saveWordsButton);
        secondRowLayout->addWidget(importWordsButton);
        secondRowLayout->addWidget(exportWordsButton);
        secondRowLayout->addWidget(backToHomeButton);
        secondRowLayout->addStretch();
        
        wordButtonContainer->addLayout(firstRowLayout);
        wordButtonContainer->addLayout(secondRowLayout);
        
        // 将容器布局赋值给 wordButtonLayout（注意：这里改变了原有变量的类型）
        wordButtonLayout = wordButtonContainer;
        
        // 连接导入导出按钮的信号槽
        connect(importWordsButton, &QPushButton::clicked, [=]() {
            QString fileName = QFileDialog::getOpenFileName(this, "导入词库", "", "All Files (*.*);;Text Files (*.txt);;CSV Files (*.csv);;Excel Files (*.xlsx *.xls)");
            if (!fileName.isEmpty()) {
                importWordlist(fileName);
            }
        });
        
        connect(exportWordsButton, &QPushButton::clicked, [=]() {
            // 创建导出格式选择对话框
            QDialog dialog(this);
            dialog.setWindowTitle("选择导出格式");
            dialog.resize(300, 150);
            
            QVBoxLayout layout(&dialog);
            QLabel label("请选择导出格式：", &dialog);
            layout.addWidget(&label);
            
            QPushButton txtButton("文本文件 (*.txt)", &dialog);
            QPushButton csvButton("CSV文件 (*.csv)", &dialog);
            QPushButton excelButton("Excel文件 (*.xlsx)", &dialog);
            
            txtButton.setStyleSheet(buttonStyle);
            csvButton.setStyleSheet(buttonStyle);
            excelButton.setStyleSheet(buttonStyle);
            
            layout.addWidget(&txtButton);
            layout.addWidget(&csvButton);
            layout.addWidget(&excelButton);
            
            // 连接按钮信号
            QObject::connect(&txtButton, &QPushButton::clicked, [&dialog, this]() {
                QString fileName = QFileDialog::getSaveFileName(this, "导出词库", "", "Text Files (*.txt)");
                if (!fileName.isEmpty()) {
                    exportWordlist(fileName, "txt");
                }
                dialog.accept();
            });
            
            QObject::connect(&csvButton, &QPushButton::clicked, [&dialog, this]() {
                QString fileName = QFileDialog::getSaveFileName(this, "导出词库", "", "CSV Files (*.csv)");
                if (!fileName.isEmpty()) {
                    exportWordlist(fileName, "csv");
                }
                dialog.accept();
            });
            
            QObject::connect(&excelButton, &QPushButton::clicked, [&dialog, this]() {
                QString fileName = QFileDialog::getSaveFileName(this, "导出词库", "", "Excel Files (*.xlsx)");
                if (!fileName.isEmpty()) {
                    exportWordlist(fileName, "excel");
                }
                dialog.accept();
            });
            
            dialog.exec();
        });

        wordsLayout->addWidget(wordsTitle);
        wordsLayout->addWidget(searchFilterWidget);
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
        font.setFamily("Microsoft YaHei"); // 使用微软雅黑字体
        font.setPointSize(48);
        font.setBold(true);
        countdownLabel->setFont(font);
        countdownLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // 创建设置控件布局
        QHBoxLayout *settingsLayout = new QHBoxLayout();
        settingsLayout->setSpacing(10);
        
        QLabel *intervalLabel = new QLabel("时间间隔(秒):", testWidget);
        QSpinBox *intervalSpinBox = new QSpinBox(testWidget);
        intervalSpinBox->setRange(1, 60);
        intervalSpinBox->setValue(readInterval);
        
        QPushButton *applyIntervalButton = new QPushButton("应用", testWidget);
        applyIntervalButton->setFixedSize(60, 30);
        
        settingsLayout->addWidget(intervalLabel);
        settingsLayout->addWidget(intervalSpinBox);
        settingsLayout->addWidget(applyIntervalButton);
        settingsLayout->addStretch(); // 添加弹簧，将控件推到左侧

        // 创建控制按钮布局
        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->setSpacing(15);  // 增加按钮间距
        controlLayout->setContentsMargins(20, 10, 20, 10);  // 添加布局边距

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

        // 连接时间间隔设置按钮
        connect(applyIntervalButton, &QPushButton::clicked, [=]() {
            readInterval = intervalSpinBox->value();
            saveSettings(); // 保存设置
            QMessageBox::information(testWidget, "设置成功", 
                QString("朗读时间间隔已设置为 %1 秒").arg(readInterval));
        });

        testLayout->addWidget(countdownLabel);
        testLayout->addLayout(settingsLayout);
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
        titleFont.setFamily("Microsoft YaHei"); // 使用微软雅黑字体
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

        // 连接主页按钮
        connect(viewWordsButton, &QPushButton::clicked, this, &MainWindow::onViewWords);
        connect(startButton, &QPushButton::clicked, this, &MainWindow::onSelectDictationMode);
        connect(themeButton, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
        connect(aboutButton, &QPushButton::clicked, this, &MainWindow::onShowAbout);
        connect(guideButton, &QPushButton::clicked, this, &MainWindow::onShowGuide);

        // 为设置按钮添加右键菜单以支持学习进度功能
        connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onShowSettings);
        
        // 添加右键菜单
        connect(settingsButton, &QPushButton::customContextMenuRequested, this, [=]() {
            QMenu *menu = new QMenu(this);
            menu->addAction("常规设置", this, &MainWindow::onShowSettings);
            menu->addAction("学习进度可视化", this, &MainWindow::showProgressChart);
            menu->exec(QCursor::pos());
        });
        settingsButton->setContextMenuPolicy(Qt::CustomContextMenu);

        // 连接单词界面的按钮
        connect(addWordButton, &QPushButton::clicked, this, &MainWindow::onAddWord);
        connect(removeWordButton, &QPushButton::clicked, this, &MainWindow::onRemoveWord);
        connect(clearWordsButton, &QPushButton::clicked, this, &MainWindow::onClearWords);
        connect(loadWordsButton, &QPushButton::clicked, this, &MainWindow::onAddWordsFromFile);
        connect(saveWordsButton, &QPushButton::clicked, this, &MainWindow::onSaveWordsToFile);
        connect(backToHomeButton, &QPushButton::clicked, this, &MainWindow::showMainInterface);

        // 设置窗口属性
        setWindowTitle("英语听写练习");
        resize(800, 500);  // 增加初始窗口尺寸
        setMinimumSize(700, 400);  // 增加最小窗口尺寸，确保按钮不重叠
    }

    // 初始化测试定时器
    if (!timer) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::onNextWord);
    }

    // 初始化欢迎动画定时器
    if (!welcomeTimer) {
        welcomeTimer = new QTimer(this);
        connect(welcomeTimer, &QTimer::timeout, this, &MainWindow::onUpdateWelcomeAnimation);
    }
    
    // 初始化界面切换动画组
    if (!fadeInOutGroup) {
        fadeInOutGroup = new QParallelAnimationGroup(this);
    }
    
    // 初始化界面透明度效果
    setupInterfaceAnimations();

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
                             "font-family: 'Microsoft YaHei'; "
                             "font-size: 9pt; "
                             "padding: 6px 14px; "  // 缩小2px
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
                             "}";

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
        if (aboutButton) aboutButton->setStyleSheet(buttonStyleDark);

        // 查找并设置主页的指南按钮样式
        if (centralWidget) {
            for (auto button : centralWidget->findChildren<QPushButton*>()) {
                if (button->text() == "使用指南") {
                    button->setStyleSheet(buttonStyleDark);
                }
            }
        }

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
        if (aboutButton) aboutButton->setStyleSheet(buttonStyleLight);

        // 查找并设置主页的指南按钮样式
        if (centralWidget) {
            for (auto button : centralWidget->findChildren<QPushButton*>()) {
                if (button->text() == "使用指南") {
                    button->setStyleSheet(buttonStyleLight);
                }
            }
        }

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

void MainWindow::setupInterfaceAnimations()
{
    // 为主界面组件添加透明度效果
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    if (homeWidget && !homeOpacityEffect) {
        homeOpacityEffect = new QGraphicsOpacityEffect(this);
        homeOpacityEffect->setOpacity(1.0);
        homeWidget->setGraphicsEffect(homeOpacityEffect);
    }
    
    if (wordsWidget && !wordsOpacityEffect) {
        wordsOpacityEffect = new QGraphicsOpacityEffect(this);
        wordsOpacityEffect->setOpacity(0.0);
        wordsWidget->setGraphicsEffect(wordsOpacityEffect);
    }
    
    if (testWidget && !testOpacityEffect) {
        testOpacityEffect = new QGraphicsOpacityEffect(this);
        testOpacityEffect->setOpacity(0.0);
        testWidget->setGraphicsEffect(testOpacityEffect);
    }
    
    if (answersWidget && !answersOpacityEffect) {
        answersOpacityEffect = new QGraphicsOpacityEffect(this);
        answersOpacityEffect->setOpacity(0.0);
        answersWidget->setGraphicsEffect(answersOpacityEffect);
    }
}

void MainWindow::animateInterfaceSwitch(QWidget *fromWidget, QWidget *toWidget)
{
    // 停止正在进行的动画
    if (fadeInOutGroup->state() == QAbstractAnimation::Running) {
        fadeInOutGroup->stop();
    }
    
    fadeInOutGroup->clear();
    
    // 创建淡出动画
    if (fromWidget) {
        QGraphicsOpacityEffect *fadeOutEffect = qobject_cast<QGraphicsOpacityEffect*>(fromWidget->graphicsEffect());
        if (fadeOutEffect) {
            QPropertyAnimation *fadeOutAnim = new QPropertyAnimation(fadeOutEffect, "opacity", this);
            fadeOutAnim->setDuration(400);
            fadeOutAnim->setStartValue(1.0);
            fadeOutAnim->setEndValue(0.0);
            fadeOutAnim->setEasingCurve(QEasingCurve::OutCubic);
            fadeInOutGroup->addAnimation(fadeOutAnim);
            
            // 添加缩放动画
            QPropertyAnimation *scaleOutAnim = new QPropertyAnimation(fromWidget, "geometry", this);
            QRect startRect = fromWidget->geometry();
            QRect endRect = QRect(startRect.x() + 20, startRect.y() + 20, startRect.width() - 40, startRect.height() - 40);
            scaleOutAnim->setDuration(400);
            scaleOutAnim->setStartValue(startRect);
            scaleOutAnim->setEndValue(endRect);
            scaleOutAnim->setEasingCurve(QEasingCurve::OutCubic);
            fadeInOutGroup->addAnimation(scaleOutAnim);
        }
    }
    
    // 创建淡入动画
    if (toWidget) {
        QGraphicsOpacityEffect *fadeInEffect = qobject_cast<QGraphicsOpacityEffect*>(toWidget->graphicsEffect());
        if (fadeInEffect) {
            QPropertyAnimation *fadeInAnim = new QPropertyAnimation(fadeInEffect, "opacity", this);
            fadeInAnim->setDuration(400);
            fadeInAnim->setStartValue(0.0);
            fadeInAnim->setEndValue(1.0);
            fadeInAnim->setEasingCurve(QEasingCurve::InCubic);
            fadeInOutGroup->addAnimation(fadeInAnim);
            
            // 添加缩放动画
            QPropertyAnimation *scaleInAnim = new QPropertyAnimation(toWidget, "geometry", this);
            QRect endRect = toWidget->geometry();
            QRect startRect = QRect(endRect.x() + 20, endRect.y() + 20, endRect.width() - 40, endRect.height() - 40);
            scaleInAnim->setDuration(400);
            scaleInAnim->setStartValue(startRect);
            scaleInAnim->setEndValue(endRect);
            scaleInAnim->setEasingCurve(QEasingCurve::InCubic);
            fadeInOutGroup->addAnimation(scaleInAnim);
        }
    }
    
    // 启动动画组
    fadeInOutGroup->start();
}

void MainWindow::showLoadingAnimation(const QString &message)
{
    // 创建加载标签
    if (!loadingLabel) {
        loadingLabel = new QLabel(this);
        loadingLabel->setAlignment(Qt::AlignCenter);
        loadingLabel->setStyleSheet(
            "QLabel { "
            "background-color: rgba(0, 0, 0, 180); "
            "color: white; "
            "font-size: 14px; "
            "border-radius: 10px; "
            "padding: 20px; "
            "}");
        
        // 设置在主窗口中央
        loadingLabel->setGeometry(width()/2 - 100, height()/2 - 50, 200, 100);
    }
    
    loadingLabel->setText(message + "\n◐");
    loadingLabel->show();
    
    // 创建旋转动画
    if (!loadingRotationAnim) {
        loadingRotationAnim = new QPropertyAnimation(loadingLabel, "windowOpacity", this);
        loadingRotationAnim->setDuration(1000);
        loadingRotationAnim->setLoopCount(-1); // 无限循环
        loadingRotationAnim->setStartValue(0.7);
        loadingRotationAnim->setEndValue(1.0);
        loadingRotationAnim->setEasingCurve(QEasingCurve::InOutQuad);
    }
    
    // 创建定时器更新旋转符号
    if (!loadingTimer) {
        loadingTimer = new QTimer(this);
        connect(loadingTimer, &QTimer::timeout, this, &MainWindow::updateLoadingAnimation);
    }
    
    loadingAnimationStep = 0;
    loadingTimer->start(150);
    loadingRotationAnim->start();
}

void MainWindow::hideLoadingAnimation()
{
    if (loadingLabel) {
        loadingLabel->hide();
    }
    
    if (loadingTimer) {
        loadingTimer->stop();
    }
    
    if (loadingRotationAnim && loadingRotationAnim->state() == QAbstractAnimation::Running) {
        loadingRotationAnim->stop();
    }
}

void MainWindow::updateLoadingAnimation()
{
    if (!loadingLabel) return;
    
    // 旋转符号数组
    QStringList spinnerChars = {"◐", "◓", "◑", "◒"};
    QString currentChar = spinnerChars[loadingAnimationStep % spinnerChars.size()];
    
    // 获取当前文本（去掉最后一行的符号）
    QString currentText = loadingLabel->text();
    QStringList lines = currentText.split('\n');
    if (lines.size() >= 2) {
        lines[lines.size()-1] = currentChar;
        loadingLabel->setText(lines.join('\n'));
    }
    
    loadingAnimationStep++;
}

void MainWindow::showMainInterface()
{
    // 获取当前显示的界面
    QWidget *currentWidget = nullptr;
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    
    // 确定当前显示的是哪个界面
    if (wordsWidget && wordsWidget->isVisible()) {
        currentWidget = wordsWidget;
    } else if (testWidget && testWidget->isVisible()) {
        currentWidget = testWidget;
    } else if (answersWidget && answersWidget->isVisible()) {
        currentWidget = answersWidget;
    }
    
    // 隐藏其他界面
    if (wordsWidget) wordsWidget->hide();
    if (testWidget) testWidget->hide();
    if (answersWidget) answersWidget->hide();

    // 显示主页界面元素
    if (homeWidget) {
        homeWidget->show();
        // 确保主页界面能接收焦点
        homeWidget->setEnabled(true);
    }

    // 执行界面切换动画
    animateInterfaceSwitch(currentWidget, homeWidget);

    // 更新欢迎语
    updateWelcomeMessage();

    // 启动欢迎语动画
    startWelcomeAnimation();
    
    // 确保设置按钮能接收点击
    if (settingsButton) {
        settingsButton->setEnabled(true);
    }
}

void MainWindow::showWordsInterface()
{
    // 获取当前显示的界面
    QWidget *currentWidget = nullptr;
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    
    // 确定当前显示的是哪个界面
    if (homeWidget && homeWidget->isVisible()) {
        currentWidget = homeWidget;
    } else if (testWidget && testWidget->isVisible()) {
        currentWidget = testWidget;
    } else if (answersWidget && answersWidget->isVisible()) {
        currentWidget = answersWidget;
    }
    
    // 隐藏其他界面
    if (homeWidget) homeWidget->hide();
    if (testWidget) testWidget->hide();
    if (answersWidget) answersWidget->hide();

    // 显示单词界面
    if (wordsWidget) wordsWidget->show();

    // 执行界面切换动画
    animateInterfaceSwitch(currentWidget, wordsWidget);

    // 更新单词列表显示
    QString wordsText;
    for (const auto& word : words) {
        wordsText += QString::fromStdString(word) + "\n";
    }
    if (wordsTextEdit) wordsTextEdit->setPlainText(wordsText);
}

void MainWindow::showTestInterface()
{
    // 获取当前显示的界面
    QWidget *currentWidget = nullptr;
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    
    // 确定当前显示的是哪个界面
    if (homeWidget && homeWidget->isVisible()) {
        currentWidget = homeWidget;
    } else if (wordsWidget && wordsWidget->isVisible()) {
        currentWidget = wordsWidget;
    } else if (answersWidget && answersWidget->isVisible()) {
        currentWidget = answersWidget;
    }
    
    // 隐藏其他界面
    if (homeWidget) homeWidget->hide();
    if (wordsWidget) wordsWidget->hide();
    if (answersWidget) answersWidget->hide();

    // 显示测试界面
    if (testWidget) testWidget->show();
    
    // 执行界面切换动画
    animateInterfaceSwitch(currentWidget, testWidget);
    
    // 显示开始听写测试的通知
    showNotification("听写测试开始", QString("共有 %1 个单词，准备开始听写！").arg(words.size()), "logo.ico");
}

void MainWindow::showAnswersInterface()
{
    // 停止定时器
    if (timer) timer->stop();

    // 获取当前显示的界面
    QWidget *currentWidget = nullptr;
    QWidget *homeWidget = centralWidget->layout()->itemAt(0)->widget();
    
    // 确定当前显示的是哪个界面
    if (homeWidget && homeWidget->isVisible()) {
        currentWidget = homeWidget;
    } else if (wordsWidget && wordsWidget->isVisible()) {
        currentWidget = wordsWidget;
    } else if (testWidget && testWidget->isVisible()) {
        currentWidget = testWidget;
    }

    // 隐藏其他界面
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
    
    // 执行界面切换动画
    animateInterfaceSwitch(currentWidget, answersWidget);
    
    // 显示听写测试完成的通知
    showNotification("听写测试完成", QString("已完成 %1 个单词的听写练习！").arg(words.size()), "logo.ico");
}

void MainWindow::onViewWords()
{
    // 如果词库目录有效且有可用的词库文件，让用户选择加载
    if (isValidWordlistDir && !wordlistFiles.isEmpty()) {
        // 创建选择对话框
        QDialog dialog(this);
        dialog.setWindowTitle("选择词库文件");
        dialog.resize(500, 400);
        
        QVBoxLayout layout(&dialog);
        QLabel label("请选择要加载的词库文件（可多选）：", &dialog);
        layout.addWidget(&label);
        
        // 创建分组和文件的树状视图
        QTreeWidget *wordlistTreeView = new QTreeWidget(&dialog);
        wordlistTreeView->setHeaderLabel("词库文件");
        wordlistTreeView->setSelectionMode(QAbstractItemView::MultiSelection);
        
        // 添加分组和文件到树状视图
        for (auto it = wordlistGroups.constBegin(); it != wordlistGroups.constEnd(); ++it) {
            QString groupName = it.key();
            QTreeWidgetItem *groupItem = new QTreeWidgetItem(wordlistTreeView);
            groupItem->setText(0, groupName);
            groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsSelectable);
            
            // 添加分组中的词库文件
            QStringList wordlistsInGroup = it.value();
            for (const QString &wordlistName : wordlistsInGroup) {
                if (wordlistFiles.contains(wordlistName)) {
                    QTreeWidgetItem *fileItem = new QTreeWidgetItem(groupItem);
                    fileItem->setText(0, wordlistName);
                }
            }
        }
        
        // 添加未分组的词库文件
        QTreeWidgetItem *ungroupedItem = new QTreeWidgetItem(wordlistTreeView);
        ungroupedItem->setText(0, "未分组");
        ungroupedItem->setFlags(ungroupedItem->flags() & ~Qt::ItemIsSelectable);
        
        for (auto it = wordlistFiles.constBegin(); it != wordlistFiles.constEnd(); ++it) {
            QString fileName = it.key();
            bool isGrouped = false;
            
            // 检查文件是否已在某个分组中
            for (auto groupIt = wordlistGroups.constBegin(); groupIt != wordlistGroups.constEnd(); ++groupIt) {
                if (groupIt.value().contains(fileName)) {
                    isGrouped = true;
                    break;
                }
            }
            
            if (!isGrouped) {
                QTreeWidgetItem *fileItem = new QTreeWidgetItem(ungroupedItem);
                fileItem->setText(0, fileName);
            }
        }
        
        layout.addWidget(wordlistTreeView);
        
        // 添加分组管理按钮
        QHBoxLayout *groupButtonsLayout = new QHBoxLayout();
        QPushButton *createGroupButton = new QPushButton("创建分组", &dialog);
        QPushButton *editGroupButton = new QPushButton("编辑分组", &dialog);
        QPushButton *deleteGroupButton = new QPushButton("删除分组", &dialog);
        
        groupButtonsLayout->addWidget(createGroupButton);
        groupButtonsLayout->addWidget(editGroupButton);
        groupButtonsLayout->addWidget(deleteGroupButton);
        layout.addLayout(groupButtonsLayout);
        
        QDialogButtonBox buttonBox(Qt::Horizontal, &dialog);
        QPushButton *loadButton = buttonBox.addButton("加载", QDialogButtonBox::AcceptRole);
        QPushButton *skipButton = buttonBox.addButton("跳过", QDialogButtonBox::RejectRole);
        
        // 初始状态下禁用加载按钮
        loadButton->setEnabled(false);
        
        // 连接信号槽，当选中项改变时检查是否启用加载按钮
        connect(wordlistTreeView, &QTreeWidget::itemSelectionChanged, [&]() {
            QList<QTreeWidgetItem*> selectedItems = wordlistTreeView->selectedItems();
            bool hasSelectedFiles = false;
            for (QTreeWidgetItem *item : selectedItems) {
                if (item->parent()) { // 只有子项（文件）可选择
                    hasSelectedFiles = true;
                    break;
                }
            }
            loadButton->setEnabled(hasSelectedFiles);
        });
        
        // 连接分组管理按钮
        connect(createGroupButton, &QPushButton::clicked, [&]() {
            bool ok;
            QString groupName = QInputDialog::getText(&dialog, "创建分组", "请输入分组名称:", QLineEdit::Normal, "", &ok);
            if (ok && !groupName.isEmpty()) {
                createWordlistGroup(groupName);
                // 重新加载树状视图
                wordlistTreeView->clear();
                for (auto it = wordlistGroups.constBegin(); it != wordlistGroups.constEnd(); ++it) {
                    QString gName = it.key();
                    QTreeWidgetItem *groupItem = new QTreeWidgetItem(wordlistTreeView);
                    groupItem->setText(0, gName);
                    groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsSelectable);
                }
            }
        });
        
        connect(editGroupButton, &QPushButton::clicked, [&]() {
            QTreeWidgetItem *currentItem = wordlistTreeView->currentItem();
            if (currentItem && !currentItem->parent()) {
                QString oldGroupName = currentItem->text(0);
                bool ok;
                QString newGroupName = QInputDialog::getText(&dialog, "编辑分组", "请输入新的分组名称:", QLineEdit::Normal, oldGroupName, &ok);
                if (ok && !newGroupName.isEmpty() && newGroupName != oldGroupName) {
                    editWordlistGroup(oldGroupName, newGroupName);
                    currentItem->setText(0, newGroupName);
                }
            }
        });
        
        connect(deleteGroupButton, &QPushButton::clicked, [&]() {
            QTreeWidgetItem *currentItem = wordlistTreeView->currentItem();
            if (currentItem && !currentItem->parent()) {
                QString groupName = currentItem->text(0);
                if (groupName != "默认分组") {
                    if (QMessageBox::question(&dialog, "删除分组", QString("确定要删除分组 '%1' 吗？").arg(groupName), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                        deleteWordlistGroup(groupName);
                        delete currentItem;
                    }
                }
            }
        });
        
        layout.addWidget(&buttonBox);
        
        connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        
        // 显示对话框并处理用户选择
        int result = dialog.exec();
        if (result == QDialog::Accepted) {
            // 获取选中的文件
            QList<QTreeWidgetItem*> selectedItems = wordlistTreeView->selectedItems();
            // 只有当选中了文件时才执行加载操作
            if (!selectedItems.isEmpty()) {
                // 清空当前词库
                words.clear();
                cachedWords.clear(); // 同时清空缓存
                
                // 加载选定的词库文件
                for (QTreeWidgetItem *item : selectedItems) {
                    if (item->parent()) { // 只有子项（文件）可选择
                        QString fileName = item->text(0);
                        QString filePath = wordlistFiles.value(fileName);
                        loadWordsFromFile(filePath);
                    }
                }
            }
            // 如果没有选中文件，则不执行任何加载操作，保持当前词库不变
        }
        // 如果是Rejected（点击跳过或关闭窗口），则不执行任何加载操作，保持当前词库不变
    }
    
    // 显示单词界面
    showWordsInterface();
}

void MainWindow::onShowSettings()
{
    qDebug() << "设置按钮被点击";
    showSettingsDialog();
}

void MainWindow::onShowAbout()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("关于 English Listen");
    aboutBox.setTextFormat(Qt::RichText);

    // 设置字体为微软雅黑
    QFont aboutFont;
    aboutFont.setFamily("Microsoft YaHei");
    aboutBox.setFont(aboutFont);
    
    aboutBox.setText(
        "<h2 style='font-family: Microsoft YaHei;'>English Listen v2.5.0</h2>"
        "<p style='font-family: Microsoft YaHei;'>一个帮助学习英语的听写练习工具</p>"
        "<p style='font-family: Microsoft YaHei;'>该软件基于 Qt6 框架开发，支持 Windows SAPI 和 Flite 语音引擎。</p>"
        "<h3 style='font-family: Microsoft YaHei;'>功能特点：</h3>"
        "<ul style='font-family: Microsoft YaHei;'>"
        "<li>支持自定义词库</li>"
        "<li>可调节朗读时间间隔</li>"
        "<li>支持深色/浅色主题切换</li>"
        "<li>提供测试历史记录查看</li>"
        "<li>界面字体统一为微软雅黑</li>"
        "<li>按钮尺寸优化</li>"
        "<li>新增使用指南功能</li>"
        "<li>支持临时词库自动保存和另存为功能</li>"
        "<li>支持SAPI和Flite两种朗读引擎</li>"
        "<li>新增Flite语音引擎设置向导</li>"
        "<li>首个单词立即朗读无需等待</li>"
        "<li>设置功能整合到统一窗口</li>"
        "<li>模型设置中增加试听功能</li>"
        "<li>测试界面添加时间间隔设置</li>"
        "<li>修复设置对话框非模态导致的界面交互问题</li>"
        "<li>修复Flite语音引擎下载完成后错误显示\"下载取消\"提示的问题</li>"
        "</ul>"
"<p style='font-family: Microsoft YaHei;'>版权 © 2026 JetCPP。本软件使用 MIT 许可证发布。</p>"
        "<p style='font-family: Microsoft YaHei;'>GitHub 仓库地址：<a href='https://github.com/dongzheyu/English_Listen'>https://github.com/dongzheyu/English_Listen</a></p>"
        "<p style='font-family: Microsoft YaHei;'>Gitee 仓库地址：<a href='https://gitee.com/jetcpp/english_-listen'>https://gitee.com/jetcpp/english_-listen</a></p>"
    );
    
    aboutBox.exec();
}

void MainWindow::onShowGuide()
{
    QDialog guideDialog(this);
    guideDialog.setWindowTitle("使用指南");
    guideDialog.resize(600, 500);

    QVBoxLayout *layout = new QVBoxLayout(&guideDialog);

    QTextEdit *guideText = new QTextEdit(&guideDialog);
    guideText->setReadOnly(true);

    // 设置微软雅黑字体
    QFont guideFont;
    guideFont.setFamily("Microsoft YaHei");
    guideFont.setPointSize(10);
    guideText->setFont(guideFont);

    QString guideContent =
        "<h2 style='font-family: Microsoft YaHei;'>English Listen 使用指南</h2>"
        "<h3 style='font-family: Microsoft YaHei;'>1. 程序简介</h3>"
        "<p style='font-family: Microsoft YaHei;'>English Listen 是一个基于 Qt6 框架开发的英语听写练习工具，支持 Windows SAPI 和 Flite 两种语音引擎进行文本转语音。</p>"

        "<h3 style='font-family: Microsoft YaHei;'>2. 主要功能</h3>"
        "<ul style='font-family: Microsoft YaHei;'>"
        "<li><strong>词库管理</strong>：查看、添加、删除和保存单词列表</li>"
        "<li><strong>听写测试</strong>：自动朗读单词，支持调节朗读时间间隔</li>"
        "<li><strong>主题切换</strong>：支持浅色和深色主题，适配 Windows 系统主题</li>"
        "<li><strong>词库文件</strong>：支持从文件导入和导出词库</li>"
        "<li><strong>临时词库</strong>：编辑词库时自动保存到临时文件，退出时可选择保存</li>"
        "<li><strong>语音引擎</strong>：支持SAPI和Flite两种朗读引擎</li>"
        "</ul>"

        "<h3 style='font-family: Microsoft YaHei;'>3. 使用步骤</h3>"
        "<ol style='font-family: Microsoft YaHei;'>"
        "<li><strong>添加单词</strong>：点击'查看单词'按钮进入词库管理界面，可以通过以下方式添加单词：<br>"
        "  - 在输入框输入单词后点击'添加单词'<br>"
        "  - 点击'从文件加载'选择词库文件<br>"
        "  - 直接在文本框中输入多个单词（每行一个）</li>"

        "<li><strong>开始测试</strong>：返回主界面后点击'开始听写测试'按钮</li>"

        "<li><strong>测试控制</strong>：在测试过程中可以：<br>"
        "  - 点击'再读一遍'重新朗读当前单词<br>"
        "  - 点击'上一个'返回前一个单词<br>"
        "  - 点击'下一个'跳到下一个单词<br>"
        "  - 点击'暂停'/'继续'控制测试进程</li>"

        "<li><strong>查看答案</strong>：测试结束后可选择'显示答案'查看全部单词列表</li>"
        "</ol>"

        "<h3 style='font-family: Microsoft YaHei;'>4. 设置选项</h3>"
        "<p style='font-family: Microsoft YaHei;'>点击'设置'按钮可以：</p>"
        "<ul style='font-family: Microsoft YaHei;'>"
        "<li>切换深色/浅色主题</li>"
        "<li>设置朗读时间间隔（1-60秒）</li>"
        "<li>选择朗读引擎（SAPI或Flite）</li>"
        "</ul>"

        "<h3 style='font-family: Microsoft YaHei;'>5. 词库文件</h3>"
        "<p style='font-family: Microsoft YaHei;'>程序会自动在'wordlist'文件夹中查找词库文件，支持递归遍历子文件夹。<br>"
        "编辑词库时，所有更改会自动保存到临时文件(appdata/temp)，退出时可选择是否保存到指定位置。</p>"

        "<h3 style='font-family: Microsoft YaHei;'>6. 语音引擎</h3>"
        "<p style='font-family: Microsoft YaHei;'>程序支持两种语音引擎：<br>"
        "- <strong>SAPI</strong>：Windows内置的语音引擎，无需额外安装<br>"
        "- <strong>Flite</strong>：第三方轻量级语音引擎，独立程序已随软件部署</p>"

        "<h3 style='font-family: Microsoft YaHei;'>7. 快捷键说明</h3>"
        "<p style='font-family: Microsoft YaHei;'>在测试界面中，您可以使用以下快捷键来控制听写过程：</p>"
        "<ul style='font-family: Microsoft YaHei;'>"
        "<li><strong>空格键</strong>：重复朗读当前单词</li>"
        "<li><strong>左方向键</strong>：返回上一个单词</li>"
        "<li><strong>右方向键</strong>：跳转到下一个单词</li>"
        "<li><strong>ESC键</strong>：暂停/继续测试</li>"
        "</ul>"
        "<h3 style='font-family: Microsoft YaHei;'>8. 注意事项</h3>"
        "<ul style='font-family: Microsoft YaHei;'>"
        "<li>确保系统启用了TTS(text-to-speech)功能</li>"
        "<li>Flite引擎文件已随软件部署，无需额外安装</li>"
        "<li>程序退出时会询问是否保存当前词库</li>"
        "<li>测试过程中点击'退出测试'会有确认对话框</li>"
        "</ul>";

    guideText->setHtml(guideContent);
    layout->addWidget(guideText);

    QPushButton *closeButton = new QPushButton("关闭", &guideDialog);
    // 设置关闭按钮也使用微软雅黑字体
    QFont buttonFont;
    buttonFont.setFamily("Microsoft YaHei");
    buttonFont.setPointSize(9);
    closeButton->setFont(buttonFont);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, &guideDialog, &QDialog::accept);

    guideDialog.exec();
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
    // 在保存前，确保words向量包含当前编辑框中的内容
    if (wordsTextEdit) {
        // 清空当前词库
        words.clear();
        cachedWords.clear();

        // 从编辑框获取当前内容并更新words向量
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

    // 直接弹出保存对话框，实现另存为功能
    QString fileName = QFileDialog::getSaveFileName(this, "词库另存为", "", "Text Files (*.txt)");
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

void MainWindow::onSelectDictationMode()
{
    // 创建选择对话框
    QDialog dialog(this);
    dialog.setWindowTitle("选择听写模式");
    dialog.resize(350, 200);
    
    QVBoxLayout layout(&dialog);
    QLabel *titleLabel = new QLabel("请选择听写模式：", &dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setFamily("Microsoft YaHei");
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #333333; margin-bottom: 15px;");
    layout.addWidget(titleLabel);
    
    QPushButton paperButton("纸笔听写", &dialog);
    QPushButton onlineButton("在线听写", &dialog);
    
    // 设置按钮样式
    QString buttonStyle = "QPushButton { "
                        "font-family: 'Microsoft YaHei'; "
                        "font-size: 11pt; "
                        "font-weight: bold; "
                        "padding: 12px 20px; "
                        "margin: 6px; "
                        "border: 2px solid #555555; "
                        "border-radius: 8px; "
                        "background-color: #ffffff; "
                        "color: #333333; "
                        "}" 
                        "QPushButton:hover { "
                        "background-color: #e8f4fd; "
                        "border: 2px solid #3399ff; "
                        "color: #0066cc; "
                        "}" 
                        "QPushButton:pressed { "
                        "background-color: #cce6ff; "
                        "border: 2px solid #0066cc; "
                        "color: #004499; "
                        "}";
    
    paperButton.setStyleSheet(buttonStyle);
    onlineButton.setStyleSheet(buttonStyle);
    
    layout.addWidget(&paperButton);
    layout.addWidget(&onlineButton);
    
    // 连接按钮信号
    QObject::connect(&paperButton, &QPushButton::clicked, &dialog, [&dialog, this]() {
        this->startPaperDictation();
        dialog.accept();
    });
    
    QObject::connect(&onlineButton, &QPushButton::clicked, &dialog, [&dialog, this]() {
        this->startOnlineDictation();
        dialog.accept();
    });
    
    dialog.exec();
}

void MainWindow::startPaperDictation()
{
    if (words.empty()) {
        QMessageBox::warning(this, "警告", "词库为空，请先添加单词");
        return;
    }

    // 如果启用随机播放，则随机化单词列表
    if (isRandomOrder) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(words.begin(), words.end(), g);
    }

    currentIndex = 0;
    isPaused = false;  // 重置暂停状态
    startButton->setEnabled(false);

    // 更新暂停按钮文本
    if (pauseResumeButton) pauseResumeButton->setText("暂停");

    // 切换到测试界面
    showTestInterface();

    // 立即朗读第一个单词
    if (currentIndex < words.size()) {
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");

        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();

        // 朗读当前单词（隐藏CMD窗口）
        speakWord(words[currentIndex]);
    }

    currentIndex++;  // 增加索引

    // 设置下一词的朗读倒计时，使用用户设置的时间间隔
    countdown = readInterval;
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
            
            // 获取用户对本次测试的自我评估
            bool ok;
            int userAssessment = QInputDialog::getInt(this, "测试完成", 
                QString("本次测试共 %1 个单词，请评估您答对了多少个？").arg(words.size()),
                words.size(), 0, words.size(), 1, &ok);
            
            if (ok) {
                // 记录测试结果
                recordTestResult(userAssessment, words.size(), "当前词库");
            }
            
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

void MainWindow::startOnlineDictation()
{
    if (words.empty()) {
        QMessageBox::warning(this, "警告", "词库为空，请先添加单词");
        return;
    }

    // 保存原始单词顺序
    originalWordsOrder = words;

    // 如果启用随机播放，则随机化单词列表
    if (isRandomOrder) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(words.begin(), words.end(), g);
    }

    currentIndex = 0;
    isPaused = false;  // 重置暂停状态
    isOnlineDictationMode = true; // 设置为在线听写模式
    userInputs.clear(); // 清空之前的用户输入
    userInputs.resize(words.size()); // 预分配空间
    startButton->setEnabled(false);

    // 更新暂停按钮文本
    if (pauseResumeButton) pauseResumeButton->setText("暂停");

    // 切换到测试界面
    showTestInterface();
    
    // 设置在线听写界面
    setupOnlineDictation();

    // 立即朗读第一个单词
    if (currentIndex < words.size()) {
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");

        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();

        // 朗读当前单词（隐藏CMD窗口）
        speakWord(words[currentIndex]);
    }
}

void MainWindow::setupOnlineDictation()
{
    // 如果还没有创建在线听写界面组件，则创建它们
    if (!onlineInputLineEdit) {
        // 创建输入框
        onlineInputLineEdit = new QLineEdit(testWidget);
        onlineInputLineEdit->setPlaceholderText("请输入听到的单词...");
        
        // 创建提交按钮
        submitAnswerButton = new QPushButton("提交所有答案", testWidget);
        submitAnswerButton->setStyleSheet(
            "QPushButton { "
            "font-family: 'Microsoft YaHei'; "
            "font-size: 9pt; "
            "padding: 6px 14px; "
            "margin: 4px; "
            "border: 1px solid #cccccc; "
            "border-radius: 4px; "
            "background-color: #f0f0f0; "
            "}" 
            "QPushButton:hover { "
            "background-color: #e0e0e0; "
            "border: 1px solid #999999; "
            "}" 
            "QPushButton:pressed { "
            "background-color: #d0d0d0; "
            "border: 1px solid #666666; "
            "}");
        
        // 创建下一个单词按钮
        nextWordButton = new QPushButton("下一个单词", testWidget);
        nextWordButton->setStyleSheet(
            "QPushButton { "
            "font-family: 'Microsoft YaHei'; "
            "font-size: 9pt; "
            "padding: 6px 14px; "
            "margin: 4px; "
            "border: 1px solid #cccccc; "
            "border-radius: 4px; "
            "background-color: #f0f0f0; "
            "}" 
            "QPushButton:hover { "
            "background-color: #e0e0e0; "
            "border: 1px solid #999999; "
            "}" 
            "QPushButton:pressed { "
            "background-color: #d0d0d0; "
            "border: 1px solid #666666; "
            "}");
        
        // 创建状态标签
        onlineStatusLabel = new QLabel(testWidget);
        onlineStatusLabel->setAlignment(Qt::AlignCenter);
        
        // 连接信号槽
        connect(submitAnswerButton, &QPushButton::clicked, this, &MainWindow::checkOnlineAnswer);
        connect(nextWordButton, &QPushButton::clicked, this, &MainWindow::showNextWordOnline);
        connect(onlineInputLineEdit, &QLineEdit::returnPressed, this, [this]() {
            // Enter键时保存当前输入并跳转到下一个单词
            if (currentIndex < words.size()) {
                userInputs[currentIndex] = onlineInputLineEdit->text().trimmed();
                
                // 检查答案
                QString correctWord = QString::fromStdString(words[currentIndex]);
                QString userWord = userInputs[currentIndex];
                
                if (!userWord.isEmpty() && userWord.toLower() == correctWord.toLower()) {
                    onlineStatusLabel->setText(QString("正确！进入下一个单词..."));
                    onlineStatusLabel->setStyleSheet("color: green; font-weight: bold;");
                } else {
                    onlineStatusLabel->setText(QString("错误！正确答案: %1，进入下一个单词...").arg(correctWord));
                    onlineStatusLabel->setStyleSheet("color: red; font-weight: bold;");
                }
                
                // 清空输入框并跳转到下一个单词
                onlineInputLineEdit->clear();
                
                // 1秒后自动跳转到下一个单词
                QTimer::singleShot(1000, this, &MainWindow::showNextWordOnline);
            }
        });
        
        // 将输入框和按钮添加到测试界面布局
        QVBoxLayout *testLayout = qobject_cast<QVBoxLayout*>(testWidget->layout());
        if (testLayout) {
            // 插入到现有控件之后
            testLayout->insertWidget(1, onlineInputLineEdit);  // 在倒计时标签后插入
            testLayout->insertWidget(2, onlineStatusLabel);    // 在输入框后插入状态标签
            
            // 创建按钮布局
            QHBoxLayout *buttonLayout = new QHBoxLayout();
            buttonLayout->addWidget(submitAnswerButton);
            buttonLayout->addWidget(nextWordButton);
            
            testLayout->insertLayout(3, buttonLayout);   // 在状态标签后插入按钮布局
        }
    }
    
    // 显示在线听写界面组件
    if (onlineInputLineEdit) onlineInputLineEdit->show();
    if (submitAnswerButton) submitAnswerButton->show();
    if (nextWordButton) nextWordButton->show();
    if (onlineStatusLabel) onlineStatusLabel->show();
    
    // 隐藏倒计时标签（因为在线模式不需要倒计时）
    if (countdownLabel) {
        countdownLabel->hide();
    }
    
    // 清空输入框
    if (onlineInputLineEdit) onlineInputLineEdit->clear();
    
    // 临时隐藏设置控件
    QList<QWidget*> settingsControls = testWidget->findChildren<QWidget*>();
    for (QWidget* widget : settingsControls) {
        if (widget->objectName() == "intervalSettingWidget" || 
            widget->inherits("QSpinBox") || 
            widget->inherits("QLabel") && widget->parent()->objectName() == "settingsLayout") {
            widget->hide();
        }
    }
}

void MainWindow::checkOnlineAnswer()
{
    if (currentIndex >= words.size() || !isOnlineDictationMode) {
        return;
    }
    
    QString userInput = onlineInputLineEdit->text().trimmed();
    if (userInput.isEmpty()) {
        onlineStatusLabel->setText("请输入单词!");
        onlineStatusLabel->setStyleSheet("color: red;");
        return;
    }
    
    // 获取当前单词
    QString currentWord = QString::fromStdString(words[currentIndex]);
    
    // 比较用户输入与正确答案（忽略大小写）
    if (userInput.toLower() == currentWord.toLower()) {
        onlineStatusLabel->setText("正确！");
        onlineStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        onlineStatusLabel->setText(QString("错误！正确答案: %1").arg(currentWord));
        onlineStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    }
    
    // 清空输入框
    onlineInputLineEdit->clear();
    
    // 1.5秒后显示下一个单词
    QTimer::singleShot(1500, this, &MainWindow::showNextWordOnline);
}

void MainWindow::showNextWordOnline()
{
    currentIndex++;
    
    if (currentIndex < words.size()) {
        // 显示正在朗读提示
        if (countdownLabel) countdownLabel->setText("正在朗读");
        onlineStatusLabel->setText(QString("第 %1 / %2 个单词").arg(currentIndex + 1).arg(words.size()));

        // 处理界面事件，确保标签更新
        QCoreApplication::processEvents();

        // 朗读下一个单词
        speakWord(words[currentIndex]);
        
        // 清空状态标签
        if (onlineStatusLabel) {
            onlineStatusLabel->clear();
            onlineStatusLabel->setStyleSheet("");
        }
    } else {
        // 测试结束
        if (timer) timer->stop();
        startButton->setEnabled(true);
        
        // 计算正确率
        int correctCount = 0; // 这里需要在实际实现中跟踪正确答案数量
        
        // 测试结束，显示结果
        QMessageBox msgBox;
        msgBox.setWindowTitle("测试完成");
        msgBox.setText(QString("在线听写测试完成！共 %1 个单词").arg(words.size()));
        QPushButton *returnButton = msgBox.addButton("返回主界面", QMessageBox::ActionRole);
        QPushButton *showAnswersButton = msgBox.addButton("查看答案", QMessageBox::ActionRole);
        msgBox.setDefaultButton(returnButton);

        msgBox.exec();

        if (msgBox.clickedButton() == returnButton) {
            // 隐藏在线听写界面组件
            if (onlineInputLineEdit) onlineInputLineEdit->hide();
            if (submitAnswerButton) submitAnswerButton->hide();
            if (onlineStatusLabel) onlineStatusLabel->hide();
            
            // 显示倒计时标签
            if (countdownLabel) countdownLabel->show();
            
            showMainInterface();
        } else if (msgBox.clickedButton() == showAnswersButton) {
            // 隐藏在线听写界面组件
            if (onlineInputLineEdit) onlineInputLineEdit->hide();
            if (submitAnswerButton) submitAnswerButton->hide();
            if (onlineStatusLabel) onlineStatusLabel->hide();
            
            // 显示倒计时标签
            if (countdownLabel) countdownLabel->show();
            
            showAnswersInterface();
        }
        
        // 重置在线听写模式
        isOnlineDictationMode = false;
    }
}

void MainWindow::onStartTest()
{
    // 保持此函数以解决链接错误，实际功能由onSelectDictationMode()处理
    onSelectDictationMode();
}



void MainWindow::onNextWordClicked()
{
    // 在线听写模式下，这个函数不需要执行
    if (isOnlineDictationMode) {
        return;
    }
    
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
    
    // 初始化词库分组
    initializeWordlistGroups();
}

void MainWindow::initializeWordlistGroups()
{
    // 加载词库分组配置
    loadWordlistGroups();
    
    // 如果没有分组配置，创建默认分组
    if (wordlistGroups.isEmpty()) {
        createWordlistGroup("默认分组");
    }
}

void MainWindow::createWordlistGroup(const QString &groupName)
{
    if (!wordlistGroups.contains(groupName)) {
        wordlistGroups[groupName] = QStringList();
        saveWordlistGroups();
    }
}

void MainWindow::editWordlistGroup(const QString &oldGroupName, const QString &newGroupName)
{
    if (wordlistGroups.contains(oldGroupName) && !wordlistGroups.contains(newGroupName)) {
        wordlistGroups[newGroupName] = wordlistGroups.take(oldGroupName);
        saveWordlistGroups();
    }
}

void MainWindow::deleteWordlistGroup(const QString &groupName)
{
    if (wordlistGroups.contains(groupName) && groupName != "默认分组") {
        wordlistGroups.remove(groupName);
        saveWordlistGroups();
    }
}

void MainWindow::addWordlistToGroup(const QString &wordlistName, const QString &groupName)
{
    if (wordlistGroups.contains(groupName) && !wordlistGroups[groupName].contains(wordlistName)) {
        wordlistGroups[groupName].append(wordlistName);
        saveWordlistGroups();
    }
}

void MainWindow::removeWordlistFromGroup(const QString &wordlistName, const QString &groupName)
{
    if (wordlistGroups.contains(groupName)) {
        wordlistGroups[groupName].removeOne(wordlistName);
        saveWordlistGroups();
    }
}

void MainWindow::saveWordlistGroups()
{
    QSettings settings("wordlist_groups.ini", QSettings::IniFormat);
    settings.clear();
    
    for (auto it = wordlistGroups.constBegin(); it != wordlistGroups.constEnd(); ++it) {
        QString groupName = it.key();
        QStringList wordlists = it.value();
        
        settings.beginGroup(groupName);
        settings.setValue("wordlists", wordlists);
        settings.endGroup();
    }
    
    settings.sync();
}

void MainWindow::loadWordlistGroups()
{
    QSettings settings("wordlist_groups.ini", QSettings::IniFormat);
    QStringList groups = settings.childGroups();
    
    wordlistGroups.clear();
    
    for (const QString &group : groups) {
        settings.beginGroup(group);
        QStringList wordlists = settings.value("wordlists").toStringList();
        wordlistGroups[group] = wordlists;
        settings.endGroup();
    }
}

void MainWindow::importWordlist(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    
    if (extension == "csv") {
        importFromCSV(filePath);
    } else if (extension == "xlsx" || extension == "xls") {
        importFromExcel(filePath);
    } else if (extension == "txt") {
        loadWordsFromFile(filePath);
    }
}

void MainWindow::exportWordlist(const QString &filePath, const QString &format)
{
    if (format == "csv") {
        exportToCSV(filePath);
    } else if (format == "excel") {
        exportToExcel(filePath);
    } else if (format == "txt") {
        saveWordsToFile(filePath);
    }
}

void MainWindow::importFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "导入失败", "无法打开CSV文件");
        return;
    }
    
    QTextStream in(&file);
    words.clear();
    cachedWords.clear();
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            QStringList fields = line.split(",");
            if (!fields.isEmpty()) {
                QString word = fields[0].trimmed();
                if (!word.isEmpty()) {
                    words.push_back(word.toStdString());
                    cachedWords.push_back(word.toStdString());
                }
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
    
    // 显示导入成功的通知
    showNotification("导入成功", QString("已从 %1 导入 %2 个单词").arg(QFileInfo(filePath).fileName()).arg(words.size()), "logo.ico");
    
    QMessageBox::information(this, "导入成功", "词库已从CSV文件导入");
}

void MainWindow::exportToCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "导出失败", "无法创建CSV文件");
        return;
    }
    
    QTextStream out(&file);
    for (const auto &word : words) {
        out << QString::fromStdString(word) << "\n";
    }
    
    file.close();
    
    // 显示导出成功的通知
    showNotification("导出成功", QString("已将 %1 个单词导出到 %2").arg(words.size()).arg(QFileInfo(filePath).fileName()), "logo.ico");
    
    QMessageBox::information(this, "导出成功", "词库已成功导出到CSV文件");
}

void MainWindow::importFromExcel(const QString &filePath)
{
    // 这里只是一个示例实现，实际的Excel导入需要使用第三方库如QtXlsx
    QMessageBox::information(this, "提示", "Excel导入功能需要安装QtXlsx库");
    // 作为回退方案，我们可以尝试使用简单的文本解析
    loadWordsFromFile(filePath);
}

void MainWindow::exportToExcel(const QString &filePath)
{
    // 这里只是一个示例实现，实际的Excel导出需要使用第三方库如QtXlsx
    QMessageBox::information(this, "提示", "Excel导出功能需要安装QtXlsx库");
    // 作为回退方案，我们可以导出为CSV格式
    QString csvPath = filePath;
    csvPath.replace(".xlsx", ".csv").replace(".xls", ".csv");
    exportToCSV(csvPath);
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

    // 递归遍历所有子目录，但限制深度以提高性能
    QDirIterator iterator(dir.path(), filters, QDir::Files, QDirIterator::Subdirectories);
    
    // 预先分配空间以减少内存重分配
    int estimatedFileCount = 50; // 预估文件数量
    // wordlistFiles.reserve(estimatedFileCount); // QMap没有reserve方法
    
    while (iterator.hasNext()) {
        iterator.next();
        QFileInfo fileInfo = iterator.fileInfo();
        
        // 跳过隐藏文件和系统文件
        if (fileInfo.isHidden() || fileInfo.fileName().startsWith(".")) {
            continue;
        }
        
        QString filePath = fileInfo.absoluteFilePath();
        
        // 异步检查文件有效性，避免阻塞UI线程
        QFile tempFile(filePath);
        if (tempFile.open(QIODevice::ReadOnly) && tempFile.size() < 1024 * 1024) { // 只检查小于1MB的文件
            if (isValidWordlistFile(filePath)) {
                // 使用相对路径作为键，以便区分不同子目录中的同名文件
                QString relativePath = dir.relativeFilePath(filePath);
                wordlistFiles[relativePath] = filePath;
            }
        }
    }
    
    qDebug() << QString("成功加载 %1 个词库文件").arg(wordlistFiles.size());
}

bool MainWindow::isValidWordlistFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // 限制文件大小，避免处理过大的文件
    if (file.size() > 1024 * 1024) { // 大于1MB的文件视为无效
        file.close();
        return false;
    }

    try {
        QTextStream in(&file);
        int lineCount = 0;
        const int maxLinesToCheck = 50; // 减少检查的行数以提高性能

        while (!in.atEnd() && lineCount < maxLinesToCheck) {
            QString line = in.readLine().trimmed();
            lineCount++;

            if (!line.isEmpty()) {
                // 简化的正则表达式检查，提高性能
                QRegularExpression regex("^[a-zA-Z\\s\\-']*$");
                if (!regex.match(line).hasMatch()) {
                    // 只检查明显无效的字符
                    if (line.contains(QRegularExpression("[{}[\\]<>]"))) {
                        file.close();
                        return false;
                    }
                }
            }
        }

        file.close();
        return lineCount > 0; // 至少有一行有效内容
    } catch (...) {
        file.close();
        return false;
    }
}

void MainWindow::loadWordsFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = QString("无法打开文件 %1: %2").arg(filename).arg(file.errorString());
        qDebug() << errorMsg;
        QMessageBox::warning(this, "加载失败", errorMsg);
        return;
    }

    try {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                words.push_back(line.toStdString());
                // 所有加载的单词都添加到缓存中（除了内置的wordlist.txt）
                if (filename != "wordlist.txt") {
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
        
        qDebug() << QString("成功从文件 %1 加载 %2 个单词").arg(filename).arg(words.size());
        
        // 显示加载成功的通知（仅在用户手动加载文件时显示，不显示临时文件加载）
        if (!filename.contains("temp_")) {
            showNotification("加载成功", QString("已从 %1 加载 %2 个单词").arg(QFileInfo(filename).fileName()).arg(words.size()), "logo.ico");
        }
    } catch (const std::exception& e) {
        QString errorMsg = QString("加载文件时发生错误: %1").arg(e.what());
        qDebug() << errorMsg;
        QMessageBox::critical(this, "加载错误", errorMsg);
        file.close();
    } catch (...) {
        QString errorMsg = "加载文件时发生未知错误";
        qDebug() << errorMsg;
        QMessageBox::critical(this, "加载错误", errorMsg);
        file.close();
    }
}

void MainWindow::loadFromTempWordlist()
{
    QFile file(tempWordlistFile);
    if (!file.exists()) {
        // 如果临时文件不存在，不做任何操作
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 清空当前词库
    words.clear();
    cachedWords.clear();

    // 解析内容并添加到词库
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.isEmpty()) {
            words.push_back(trimmedLine.toStdString());
            cachedWords.push_back(trimmedLine.toStdString());
        }
    }

    // 更新单词列表显示
    if (wordsTextEdit) {
        wordsTextEdit->setPlainText(content);
    }
}

void MainWindow::saveWordsToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString errorMsg = QString("无法保存文件到：%1\n错误信息：%2").arg(filename).arg(file.errorString());
        qDebug() << errorMsg;
        QMessageBox::warning(this, "保存失败", errorMsg);
        return;
    }

    try {
        QTextStream out(&file);
        for (const auto &word : words) {
            out << QString::fromStdString(word) << "\n";
        }

        file.close();

        // 显示保存成功的通知
        showNotification("保存成功", QString("已将 %1 个单词保存到 %2").arg(words.size()).arg(QFileInfo(filename).fileName()), "logo.ico");

        // 确认保存成功
        QString successMsg = QString("词库已成功保存至：%1\n共保存 %2 个单词").arg(filename).arg(words.size());
        qDebug() << successMsg;
        QMessageBox::information(this, "保存成功", successMsg);
    } catch (const std::exception& e) {
        QString errorMsg = QString("保存文件时发生错误: %1").arg(e.what());
        qDebug() << errorMsg;
        QMessageBox::critical(this, "保存错误", errorMsg);
        file.close();
    } catch (...) {
        QString errorMsg = "保存文件时发生未知错误";
        qDebug() << errorMsg;
        QMessageBox::critical(this, "保存错误", errorMsg);
        file.close();
    }
}

// 加密配置文件
bool MainWindow::encryptConfigFile(const QString& password)
{
    if (password.isEmpty()) return false;
    
    // 准备配置数据
    QJsonObject configObj;
    configObj["theme/isDark"] = isDarkTheme;
    configObj["test/readInterval"] = readInterval;
    configObj["speech/engine"] = speechEngine;
    configObj["test/randomOrder"] = isRandomOrder;
    configObj["security/encryptionEnabled"] = true; // 加密模式
    
    // 生成密码哈希
    QString passwordHash = hashPassword(password);
    configObj["security/passwordHash"] = passwordHash;
    
    // 转换为JSON
    QJsonDocument doc(configObj);
    QByteArray jsonData = doc.toJson();
    
    // 生成随机盐值
    QByteArray salt(16, 0);
    QRandomGenerator::global()->fillRange(reinterpret_cast<quint32*>(salt.data()), 4);
    
    // 派生密钥
    QByteArray key = deriveKeyFromPassword(password, salt);
    
    // 加密数据
    QByteArray encryptedData = encryptData(jsonData, key);
    
    // 保存为dat文件
    QFile file("config.dat");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << salt << encryptedData;
        file.close();
        
        // 删除旧的ini文件
        QFile::remove("config.ini");
        return true;
    }
    
    return false;
}

// 解密配置文件
bool MainWindow::decryptConfigFile(const QString& password, int attemptCount)
{
    if (password.isEmpty()) return false;
    
    // 检查尝试次数
    if (attemptCount >= 5) {
        // 达到最大尝试次数，初始化程序
        initializeProgram();
        return false;
    }
    
    QFile file("config.dat");
    if (!file.open(QIODevice::ReadOnly)) return false;
    
    // 读取盐值和加密数据
    QDataStream in(&file);
    QByteArray salt;
    QByteArray encryptedData;
    in >> salt >> encryptedData;
    file.close();
    
    if (salt.isEmpty() || encryptedData.isEmpty()) return false;
    
    // 派生密钥
    QByteArray key = deriveKeyFromPassword(password, salt);
    
    // 解密数据
    QByteArray decryptedData = decryptData(encryptedData, key);
    QJsonDocument doc = QJsonDocument::fromJson(decryptedData);
    
    if (!doc.isObject()) return false;
    
    QJsonObject configObj = doc.object();
    
    // 验证密码哈希
    QString storedHash = configObj["security/passwordHash"].toString();
    QString inputHash = hashPassword(password);
    if (storedHash != inputHash) return false;
    
    // 加载配置
    isDarkTheme = configObj["theme/isDark"].toBool(false);
    readInterval = configObj["test/readInterval"].toInt(5);
    speechEngine = configObj["speech/engine"].toInt(0);
    isRandomOrder = configObj["test/randomOrder"].toBool(false);
    encryptionEnabled = true;
    
    // 验证范围
    if (readInterval < 1 || readInterval > 60) readInterval = 5;
    if (speechEngine < 0 || speechEngine > 1) speechEngine = 0;
    
    return true;
}

// 初始化程序（删除所有用户和配置文件）
void MainWindow::initializeProgram()
{
    // 显示提醒弹窗
    QMessageBox::critical(this, "初始化程序", 
        "程序将初始化，所有数据将被删除！");
    
    // 删除配置文件
    QFile::remove("config.dat");
    QFile::remove("config.ini");
    
    // 删除用户数据文件
    if (!userDataPath.isEmpty()) {
        QDir userDir(userDataPath);
        QStringList filters;
        filters << "*.enc" << "*.json";
        userDir.setNameFilters(filters);
        
        QStringList files = userDir.entryList();
        for (const QString& file : files) {
            userDir.remove(file);
        }
    }
    
    // 重置内存中的数据
    userProfiles.clear();
    currentUser.clear();
    
    // 重置配置
    isDarkTheme = false;
    readInterval = 5;
    speechEngine = 0;
    isRandomOrder = false;
    encryptionEnabled = false; // 确保加密模式被禁用
    
    // 清除密码缓存
    savePasswordForUser("__config__", "");
    
    // 保存默认配置
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("theme/isDark", false);
    settings.setValue("test/readInterval", 5);
    settings.setValue("speech/engine", 0);
    settings.setValue("test/randomOrder", false);
    settings.setValue("security/encryptionEnabled", false);
    settings.sync();
    
    QMessageBox::information(this, "初始化完成", 
        "程序已初始化，所有数据已删除。请重新登录账户。");
    
    // 关闭主窗口
    this->close();
    
    // 重启应用程序
    QCoreApplication::quit();
    QProcess::startDetached(QCoreApplication::applicationFilePath());
}

// 保存配置文件（根据加密模式选择存储方式）
bool MainWindow::saveSettings(bool requirePasswordInput) // 添加参数控制是否要求输入密码
{
    if (encryptionEnabled) {
        // 加密模式下需要密码
        QString password = getPasswordForUser("__config__");
        if (password.isEmpty()) {
            // 只有在requirePasswordInput为true时，才提示用户输入密码
            if (requirePasswordInput) {
                bool ok;
                password = QInputDialog::getText(this, "输入加密密码", 
                                               "请输入配置文件加密密码:", 
                                               QLineEdit::Password, "", &ok);
                if (!ok || password.isEmpty()) return false;
            } else {
                // 在退出时，如果没有缓存密码，直接返回false，不保存设置
                return false;
            }
        }
        
        return encryptConfigFile(password);
    } else {
        // 明文模式使用ini文件
        QSettings settings("config.ini", QSettings::IniFormat);

        // 保存主题设置
        settings.setValue("theme/isDark", isDarkTheme);

        // 保存朗读时间间隔设置
        settings.setValue("test/readInterval", readInterval);

        // 保存语音引擎设置
        settings.setValue("speech/engine", speechEngine);
        
        // 保存随机播放设置
        settings.setValue("test/randomOrder", isRandomOrder);
        
        // 保存加密设置
        settings.setValue("security/encryptionEnabled", false);

        // 确保数据写入磁盘
        settings.sync();
        
        // 删除加密文件
        QFile::remove("config.dat");
        return true;
    }
}

// 加载配置文件（根据文件类型选择加载方式）
void MainWindow::loadSettings()
{
    // 首先检查是否存在明文配置文件
    if (QFile::exists("config.ini")) {
        QSettings settings("config.ini", QSettings::IniFormat);
        
        // 检查加密设置
        encryptionEnabled = settings.value("security/encryptionEnabled", false).toBool();
        
        if (!encryptionEnabled) {
            // 如果加密已禁用，直接加载明文配置
            // 加载主题设置
            isDarkTheme = settings.value("theme/isDark", false).toBool();

            // 加载朗读时间间隔设置
            readInterval = settings.value("test/readInterval", 5).toInt();

            // 加载语音引擎设置
            speechEngine = settings.value("speech/engine", 0).toInt(); // 默认为SAPI (0)
            
            // 加载随机播放设置
            isRandomOrder = settings.value("test/randomOrder", false).toBool();
            
            // 删除可能存在的旧加密文件
            QFile::remove("config.dat");
            
            // 确保时间间隔在合理范围内
            if (readInterval < 1 || readInterval > 60) {
                readInterval = 5;
            }

            // 确保语音引擎值有效
            if (speechEngine < 0 || speechEngine > 1) {
                speechEngine = 0; // 默认使用SAPI
            }
            
            return;
        }
    }
    
    // 只有当加密启用或存在加密文件时，才需要输入密码
    if (QFile::exists("config.dat")) {
        int attemptCount = 0;
        while (attemptCount < 5) {
            bool ok;
            QString password = QInputDialog::getText(this, "输入解密密码", 
                                                   QString("请输入配置文件解密密码: (尝试 %1/5)").arg(attemptCount + 1), 
                                                   QLineEdit::Password, "", &ok);
            
            if (!ok) {
                // 用户取消，增加尝试次数
                attemptCount++;
                continue;
            }
            
            if (decryptConfigFile(password, attemptCount)) {
                // 保存密码到缓存
                savePasswordForUser("__config__", password);
                return;
            } else {
                // 解密失败，增加尝试次数
                attemptCount++;
                if (attemptCount < 5) {
                    QMessageBox::warning(this, "解密失败", 
                        QString("密码错误或文件损坏，剩余尝试次数: %1").arg(5 - attemptCount));
                }
            }
        }
        
        // 达到最大尝试次数，初始化程序
        initializeProgram();
        return;
    }
    
    // 加载明文ini文件或使用默认值
    QSettings settings("config.ini", QSettings::IniFormat);

    // 加载主题设置
    isDarkTheme = settings.value("theme/isDark", false).toBool();

    // 加载朗读时间间隔设置
    readInterval = settings.value("test/readInterval", 5).toInt();

    // 加载语音引擎设置
    speechEngine = settings.value("speech/engine", 0).toInt(); // 默认为SAPI (0)
    
    // 加载随机播放设置
    isRandomOrder = settings.value("test/randomOrder", false).toBool();
    
    // 加载加密设置
    encryptionEnabled = settings.value("security/encryptionEnabled", false).toBool();

    // 确保时间间隔在合理范围内
    if (readInterval < 1 || readInterval > 60) {
        readInterval = 5;
    }

    // 确保语音引擎值有效
    if (speechEngine < 0 || speechEngine > 1) {
        speechEngine = 0; // 默认使用SAPI
    }
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

    // 应用颜色和透明度，但不使用渐变
    QString stylesheet = QString("QLabel { color: rgb(%1, %2, %3); }").arg(r).arg(g).arg(b);
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
    
    // 添加阴影效果变化
    int shadowOffsetX = (int)(2.0 * sin(welcomeAnimationStep * PI / 180.0));
    int shadowOffsetY = (int)(2.0 * cos(welcomeAnimationStep * PI / 180.0));
    int shadowAlpha = 50 + (int)(50.0 * (1 + sin(welcomeAnimationStep * PI / 60.0)) / 2);
    
    QString enhancedStylesheet = QString(
        "QLabel { "
        "color: rgb(%1, %2, %3); "
        "text-shadow: %4px %5px %6px rgba(0, 0, 0, %7); "
        "}").arg(r).arg(g).arg(b).arg(shadowOffsetX).arg(shadowOffsetY).arg(3).arg(shadowAlpha);
    
    // 注意：Qt不直接支持text-shadow，这里作为示例保留
    // 实际应用中可以考虑使用QGraphicsDropShadowEffect
}

void MainWindow::createTempWordlist()
{
    // 创建临时词库文件路径，使用AppData下的临时目录
    QString tempDir = QDir::tempPath();
    QString appTempDir = tempDir + "/EnglishListen";

    // 创建应用程序的临时目录
    QDir dir;
    if (!dir.exists(appTempDir)) {
        dir.mkpath(appTempDir);
    }

    // 设置临时词库文件路径
    tempWordlistFile = appTempDir + "/temp_wordlist.txt";
}

void MainWindow::saveToTempWordlist()
{
    // 如果文本编辑框不存在，直接返回
    if (!wordsTextEdit) return;

    // 如果临时文件路径为空，不执行保存
    if (tempWordlistFile.isEmpty()) return;

    QFile file(tempWordlistFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 如果无法创建临时文件，记录日志但不中断用户操作
        qDebug() << "Could not create temporary file:" << tempWordlistFile << "Error:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    out << wordsTextEdit->toPlainText();
    file.close();
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

    if (speechEngine == 0) { // SAPI
        // 构建VBScript代码
        QString vbsCode = "Dim voice\n";
        vbsCode += "Set voice = CreateObject(\"SAPI.SpVoice\")\n";
        vbsCode += "voice.Speak \"" + cleanedWord + "\"\n";

        // 创建临时VBS文件
        QString tempPath = QDir::tempPath();
        QString vbsFile = tempPath + "/temp_speak.vbs";

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
    } else if (speechEngine == 1) { // Flite
        QString flitePath = getFlitePath();

        // 检查Flite可执行文件是否存在（排除系统路径）
        if (flitePath != "flite.exe" && !QFile::exists(flitePath)) {
            // 如果Flite不存在，提示用户是否要下载
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Flite未找到",
                "Flite语音引擎未找到，是否要自动下载Flite语音引擎？\n\n"
                "点击'是'将从网络下载Flite引擎，点击'否'将使用系统默认的SAPI引擎。",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // 下载Flite
                downloadFlite();
                // 使用SAPI作为临时回退方案
                goto sapi_fallback;
            } else {
                // 使用SAPI作为回退方案
                goto sapi_fallback;
            }
        }

        // 使用QProcess代替system函数，避免cmd窗口闪烁
        QProcess process;
        QStringList arguments;
        arguments << "-t" << cleanedWord << "-voice" << "cmu_us_slt";

        // 启动进程并等待完成
        process.start(flitePath, arguments);
        if(process.waitForStarted(3000)) {  // 等待最多3秒启动
            process.waitForFinished(5000);  // 等待最多5秒完成

            // 检查进程是否成功执行
            if(process.exitCode() != 0) {
                // 如果Flite执行失败，尝试使用SAPI作为备选方案
                QMessageBox::warning(this, "朗读失败",
                    QString("Flite引擎朗读失败，错误码: %1\n尝试使用SAPI引擎").arg(process.exitCode()));

                // 回退到SAPI
                goto sapi_fallback;
            }
        } else {
            // 如果Flite启动失败，尝试使用SAPI作为备选方案
            QMessageBox::warning(this, "启动失败",
                QString("无法启动Flite引擎: %1\n尝试使用SAPI引擎").arg(flitePath));

            // 回退到SAPI
            goto sapi_fallback;
        }
    }
    return;

    // SAPI回退方案标签
sapi_fallback:
    {
        QString vbsCode = "Dim voice\n";
        vbsCode += "Set voice = CreateObject(\"SAPI.SpVoice\")\n";
        vbsCode += "voice.Speak \"" + cleanedWord + "\"\n";

        QString tempPath = QDir::tempPath();
        QString vbsFile = tempPath + "/temp_speak.vbs";

        QFile file(vbsFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << vbsCode;
            file.close();

            if (QFile::exists(vbsFile)) {
                QString program = "wscript.exe";
                QStringList args;
                args << "//nologo" << vbsFile;

                QProcess sapiProcess;
                sapiProcess.start(program, args);
                sapiProcess.waitForFinished(5000);
                QFile::remove(vbsFile);
            }
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // 限制窗口的最小纵横比，避免窗口太窄
    // 最小宽高比设为 1.4（700/500），确保按钮不会重叠
    QSize newSize = event->size();
    int minWidth = qMax(newSize.height() * 7 / 5, 700); // 最小宽高比 7:5
    
    if (newSize.width() < minWidth) {
        newSize.setWidth(minWidth);
        resize(newSize);
        return; // 避免递归调用
    }
    
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

    // 调整主页按钮（6个按钮：查看单词、设置、开始听写测试、切换主题、关于、使用指南）
    if (viewWordsButton && settingsButton && startButton && themeButton && aboutButton) {
        // 计算按钮总宽度和间距（6个按钮，5个间距，左右边距各20）
        int buttonCount = 6;
        int totalSpacing = 15 * (buttonCount - 1); // 按钮间距15
        int margin = 40; // 左右边距总和
        int totalWidth = (viewWordsButton->minimumWidth() * buttonCount) + totalSpacing + margin;

        // 如果总宽度超过窗口宽度，则缩小按钮
        if (totalWidth > windowWidth) {
            // 计算新的按钮宽度
            int newWidth = (windowWidth - totalSpacing - margin) / buttonCount;
            int adjustedWidth = qMax(80, newWidth); // 确保按钮不会太小（最小80像素）

            viewWordsButton->setMinimumWidth(adjustedWidth);
            settingsButton->setMinimumWidth(adjustedWidth);
            startButton->setMinimumWidth(adjustedWidth);
            themeButton->setMinimumWidth(adjustedWidth);
            aboutButton->setMinimumWidth(adjustedWidth);
        } else {
            // 窗口足够大，恢复默认宽度
            int defaultWidth = 120;
            viewWordsButton->setMinimumWidth(defaultWidth);
            settingsButton->setMinimumWidth(defaultWidth);
            startButton->setMinimumWidth(defaultWidth);
            themeButton->setMinimumWidth(defaultWidth);
            aboutButton->setMinimumWidth(defaultWidth);
        }
    }

    // 调整单词界面按钮（现在有两行布局，不需要特殊处理）
    // Qt 的布局管理器会自动处理

    // 调整测试界面按钮（类似处理）
    if (repeatButton && previousButton && nextButton && pauseResumeButton) {
        // 这些按钮在 QHBoxLayout 中，Qt 会自动处理布局
    }
}

QString MainWindow::getFlitePath()
{
    // 获取应用程序当前目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 优先检查项目根目录下的flite.exe
    QString flitePath = appDir + "/flite.exe";
    if (QFile::exists(flitePath)) {
        return flitePath;
    }

    // 检查runtime目录下的flite.exe
    flitePath = appDir + "/runtime/flite.exe";
    if (QFile::exists(flitePath)) {
        return flitePath;
    }

    // 返回系统路径中的flite.exe
    return "flite.exe";
}

bool MainWindow::checkFliteExecutable()
{
    QString flitePath = getFlitePath();
    // 检查文件是否存在（排除系统路径的情况）
    if (flitePath != "flite.exe") {
        return QFile::exists(flitePath);
    }
    // 对于系统路径，返回false（需要更复杂的检查）
    return false;
}

void MainWindow::downloadFlite()
{
    // 创建下载进度对话框
    downloadProgressDialog = new QProgressDialog("正在下载Flite语音引擎...", "取消", 0, 100, this);
    downloadProgressDialog->setWindowModality(Qt::WindowModal);
    downloadProgressDialog->setWindowTitle("下载Flite");
    downloadProgressDialog->setAutoClose(false);
    downloadProgressDialog->setAutoReset(false);
    
    // 显示进度对话框
    downloadProgressDialog->show();

    // 创建网络请求
    QNetworkRequest request;
    request.setUrl(QUrl("https://files.zohopublic.com.cn/public/workdrive-public/download/yjkvs424b7768f75548168d2e6c9dd069868c?x-cli-msg=%7B%22linkId%22%3A%221GumWstoOyK-35NGn%22%2C%22isFileOwner%22%3Afalse%2C%22version%22%3A%221.0%22%2C%22isWDSupport%22%3Afalse%7D"));
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");

    // 发起GET请求
    QNetworkReply* reply = networkManager->get(request);

    // 连接下载进度信号
    connect(reply, &QNetworkReply::downloadProgress, this, &MainWindow::onDownloadProgress);
    connect(downloadProgressDialog, &QProgressDialog::canceled, [=]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            
            QMessageBox::information(this, "下载取消", "Flite语音引擎下载已取消。将使用系统默认的SAPI引擎。");
            // 清理进度对话框
            if (downloadProgressDialog) {
                downloadProgressDialog->close();
                delete downloadProgressDialog;
                downloadProgressDialog = nullptr;
            }
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        this->onDownloadFinished(reply);
    });
}

void MainWindow::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (downloadProgressDialog && bytesTotal > 0) {
        int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
        downloadProgressDialog->setValue(percent);

        // 更新进度对话框的标签
        QString labelText = QString("正在下载Flite语音引擎... (%1MB/%2MB)")
                           .arg(static_cast<double>(bytesReceived) / (1024 * 1024), 0, 'f', 1)
                           .arg(static_cast<double>(bytesTotal) / (1024 * 1024), 0, 'f', 1);
        downloadProgressDialog->setLabelText(labelText);
    }
}

void MainWindow::onDownloadFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // 下载成功
        QByteArray data = reply->readAll();

        // 获取应用程序目录
        QString appDir = QCoreApplication::applicationDirPath();

        // 确保runtime目录存在
        QDir runtimeDir(appDir + "/runtime");
        if (!runtimeDir.exists()) {
            runtimeDir.mkpath(".");
        }

        // 保存文件到runtime目录
        QString flitePath = appDir + "/runtime/flite.exe";
        QFile file(flitePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();

            // 关闭进度对话框
            if (downloadProgressDialog) {
                downloadProgressDialog->close();
                delete downloadProgressDialog;
                downloadProgressDialog = nullptr;
            }

            // 显示下载完成的通知
            showNotification("下载完成", "Flite语音引擎下载完成！现在可以使用Flite引擎进行语音朗读了。", "logo.ico");

            QMessageBox::information(this, "下载完成",
                "Flite语音引擎下载完成！现在可以使用Flite引擎进行语音朗读了。\n\n"
                "您可以在设置中切换语音引擎。");

        } else {
            // 关闭进度对话框
            if (downloadProgressDialog) {
                downloadProgressDialog->close();
                delete downloadProgressDialog;
                downloadProgressDialog = nullptr;
            }

            QMessageBox::warning(this, "保存失败",
                "无法保存Flite语音引擎到文件: " + file.errorString() + "\n\n"
                "将使用系统默认的SAPI引擎。");

        }
    } else {
        // 下载失败
        // 关闭进度对话框
        if (downloadProgressDialog) {
            downloadProgressDialog->close();
            delete downloadProgressDialog;
            downloadProgressDialog = nullptr;
        }

        QMessageBox::warning(this, "下载失败",
            "无法下载Flite语音引擎: " + reply->errorString() + "\n\n"
            "将使用系统默认的SAPI引擎。");

    }

    // 清理reply对象
    reply->deleteLater();
}

bool MainWindow::testFliteEngine()
{
    // 获取应用程序当前目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 优先检查项目根目录下的flite.exe
    QString flitePath = appDir + "/flite.exe";

    // 检查项目根目录下的flite.exe是否存在
    if (!QFile::exists(flitePath)) {
        // 检查runtime目录下的flite.exe
        flitePath = appDir + "/runtime/flite.exe";

        // 检查runtime目录下的flite.exe是否存在
        if (!QFile::exists(flitePath)) {
            // 如果runtime目录下不存在，尝试直接调用flite.exe（系统路径中）
            flitePath = "flite.exe";
        }
    }

    // 检查Flite可执行文件是否存在
    if (!QFile::exists(flitePath)) {
        return false;
    }

    // 使用QProcess测试Flite引擎
    QProcess process;
    QStringList arguments;
    arguments << "-t" << "Hello" << "-voice" << "cmu_us_slt";

    // 启动进程并等待完成
    process.start(flitePath, arguments);
    if(process.waitForStarted(3000)) {  // 等待最多3秒启动
        process.waitForFinished(5000);  // 等待最多5秒完成

        // 检查进程是否成功执行
        if(process.exitCode() == 0) {
            return true;
        }
    }

    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 保存当前用户数据
    if (!currentUser.isEmpty()) {
        updateCurrentUserProfile();
        saveUserProfile(currentUser);
    }
    
    // 保存所有用户配置
    saveAllUserProfiles();
    
    // 保存设置，传入false参数表示退出时不要求输入密码
    saveSettings(false);

    // 检查临时词库文件是否存在且内容不为空
    QFile tempFile(tempWordlistFile);
    if (tempFile.exists()) {
        if (tempFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString tempContent = tempFile.readAll();
            tempFile.close();

            if (!tempContent.trimmed().isEmpty()) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("保存词库");
                msgBox.setText("是否保存当前编辑的词库？");
                QPushButton *saveButton = msgBox.addButton("保存", QMessageBox::ActionRole);
                QPushButton *discardButton = msgBox.addButton("不保存", QMessageBox::ActionRole);
                msgBox.setDefaultButton(discardButton);

                msgBox.exec();

                if (msgBox.clickedButton() == saveButton) {
                    // 弹出文件保存对话框，让用户选择保存位置和文件名
                    QString fileName = QFileDialog::getSaveFileName(this, "保存词库", "", "Text Files (*.txt)");
                    if (!fileName.isEmpty()) {
                        // 保存词库
                        saveWordsToFile(fileName);
                    }
                }
            }
        }
    }
    
    // 清理临时文件
    if (!tempWordlistFile.isEmpty() && QFile::exists(tempWordlistFile)) {
        QFile::remove(tempWordlistFile);
        qDebug() << "Temporary wordlist file deleted:" << tempWordlistFile;
    }

    event->accept();
}

void MainWindow::showSettingsDialog()
{
    // 创建设置对话框
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("设置");
    dialog->resize(400, 350);
    dialog->setModal(true); // 设置为模态对话框

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    // 主题设置部分
    QGroupBox *themeGroup = new QGroupBox("主题设置", dialog);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    
    QPushButton *themeToggleButton = new QPushButton(
        isDarkTheme ? "切换到浅色主题" : "切换到深色主题", themeGroup);
    
    // 应用统一按钮样式
    themeToggleButton->setStyleSheet(
        "QPushButton { "
        "font-family: 'Microsoft YaHei'; "
        "font-size: 9pt; "
        "padding: 8px 16px; "
        "margin: 4px; "
        "border: 2px solid #555555; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #333333; "
        "} "
        "QPushButton:hover { "
        "background-color: #e0e0e0; "
        "border: 2px solid #333333; "
        "} "
        "QPushButton:pressed { "
        "background-color: #d0d0d0; "
        "border: 2px solid #000000; "
        "} ");
    
    themeLayout->addWidget(themeToggleButton);
    mainLayout->addWidget(themeGroup);

    // 朗读设置部分
    QGroupBox *speechGroup = new QGroupBox("朗读设置", dialog);
    QVBoxLayout *speechLayout = new QVBoxLayout(speechGroup);

    // 时间间隔设置
    QHBoxLayout *intervalLayout = new QHBoxLayout;
    QLabel *intervalLabel = new QLabel("朗读时间间隔(秒):", speechGroup);
    QSpinBox *intervalSpinBox = new QSpinBox(speechGroup);
    intervalSpinBox->setRange(1, 60);
    intervalSpinBox->setValue(readInterval);
    
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(intervalSpinBox);
    speechLayout->addLayout(intervalLayout);

    // 语音引擎设置
    QHBoxLayout *engineLayout = new QHBoxLayout;
    QLabel *engineLabel = new QLabel("朗读引擎:", speechGroup);
    QComboBox *engineComboBox = new QComboBox(speechGroup);
    engineComboBox->addItem("SAPI (Windows内置)");
    engineComboBox->addItem("Flite (第三方引擎)");
    engineComboBox->setCurrentIndex(speechEngine);
    
    engineLayout->addWidget(engineLabel);
    engineLayout->addWidget(engineComboBox);
    speechLayout->addLayout(engineLayout);

    // Flite模型设置
    QHBoxLayout *modelLayout = new QHBoxLayout;
    QLabel *modelLabel = new QLabel("Flite语音模型:", speechGroup);
    QComboBox *modelComboBox = new QComboBox(speechGroup);
    modelComboBox->addItem("cmu_us_slt (女声)");
    modelComboBox->addItem("cmu_us_awb (男声)");
    modelComboBox->addItem("cmu_us_bdl (男声)");
    modelComboBox->addItem("cmu_us_jmk (男声)");
    modelComboBox->addItem("cmu_us_ksp (男声)");
    modelComboBox->addItem("cmu_us_rms (男声)");
    
    // 设置当前选中的模型
    // 这里可以根据需要从配置中读取当前模型
    
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(modelComboBox);
    
    // 添加试听按钮
    QPushButton *previewButton = new QPushButton("试听", speechGroup);
    modelLayout->addWidget(previewButton);
    
    speechLayout->addLayout(modelLayout);
    
    // 添加模型说明
    QLabel *modelDescription = new QLabel(
        "不同语音模型会产生不同的语音效果，您可以根据喜好选择合适的语音模型。点击试听按钮可预览效果。", speechGroup);
    modelDescription->setWordWrap(true);
    speechLayout->addWidget(modelDescription);
    
    mainLayout->addWidget(speechGroup);
    
    // 添加随机播放选项
    QHBoxLayout *randomLayout = new QHBoxLayout;
    QCheckBox *randomOrderCheckBox = new QCheckBox("随机播放单词", dialog);
    randomOrderCheckBox->setChecked(isRandomOrder);
    randomLayout->addWidget(randomOrderCheckBox);
    randomLayout->addStretch(); // 添加弹簧，使复选框靠左对齐
    speechLayout->addLayout(randomLayout);
    
    // 添加Flite下载按钮
    QPushButton *downloadFliteButton = new QPushButton("下载Flite语音引擎", dialog);
    downloadFliteButton->setStyleSheet(
        "QPushButton { "
        "font-family: 'Microsoft YaHei'; "
        "font-size: 9pt; "
        "padding: 8px 16px; "
        "margin: 4px; "
        "border: 2px solid #555555; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #333333; "
        "} "
        "QPushButton:hover { "
        "background-color: #e0e0e0; "
        "border: 2px solid #333333; "
        "} "
        "QPushButton:pressed { "
        "background-color: #d0d0d0; "
        "border: 2px solid #000000; "
        "} ");
    mainLayout->addWidget(downloadFliteButton);
    
    // 连接Flite下载按钮的信号和槽
    connect(downloadFliteButton, &QPushButton::clicked, [=]() {
        // 隐藏设置对话框
        dialog->hide();
        
        // 下载Flite
        downloadFlite();
    });
    
    // 添加引擎说明
    QLabel *engineDescription = new QLabel(
        "SAPI是Windows系统内置语音引擎，Flite是轻量级第三方引擎。"
        + QString("Flite引擎支持多种语音模型选择。"), dialog);
    engineDescription->setWordWrap(true);
    mainLayout->addWidget(engineDescription);
    
    // 连接随机播放复选框的信号和槽
    connect(randomOrderCheckBox, &QCheckBox::toggled, [=](bool checked) {
        isRandomOrder = checked;
        saveSettings();
    });
    
    // 安全设置部分
    QGroupBox *securityGroup = new QGroupBox("安全设置", dialog);
    QVBoxLayout *securityLayout = new QVBoxLayout(securityGroup);
    
    QHBoxLayout *encryptionLayout = new QHBoxLayout;
    QCheckBox *encryptionCheckBox = new QCheckBox("启用数据加密", dialog);
    encryptionCheckBox->setChecked(encryptionEnabled);
    encryptionLayout->addWidget(encryptionCheckBox);
    encryptionLayout->addStretch();
    
    securityLayout->addLayout(encryptionLayout);
    
    // 添加加密说明
    QLabel *encryptionDescription = new QLabel(
        "启用数据加密可以保护您的用户数据不被未授权访问。" 
        "加密模式下需要输入密码才能登录。", dialog);
    encryptionDescription->setWordWrap(true);
    securityLayout->addWidget(encryptionDescription);
    
    // 添加初始化程序按钮
    QPushButton *initializeButton = new QPushButton("初始化程序", dialog);
    initializeButton->setStyleSheet(
        "QPushButton { "
        "font-family: 'Microsoft YaHei'; "
        "font-size: 9pt; "
        "padding: 8px 16px; "
        "margin: 4px; "
        "border: 2px solid #ff5555; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #ff5555; "
        "} "
        "QPushButton:hover { "
        "background-color: #ffe0e0; "
        "border: 2px solid #ff3333; "
        "} "
        "QPushButton:pressed { "
        "background-color: #ffd0d0; "
        "border: 2px solid #ff0000; "
        "} ");
    securityLayout->addWidget(initializeButton);
    
    // 连接初始化按钮的信号和槽
    connect(initializeButton, &QPushButton::clicked, [=]() {
        // 显示提醒弹窗
        QMessageBox::critical(dialog, "初始化程序", 
            "程序将初始化，所有数据将被删除！");
        
        // 关闭设置对话框
        dialog->accept();
        
        // 初始化程序
        initializeProgram();
    });
    
    mainLayout->addWidget(securityGroup);
    
    // 连接加密复选框的信号和槽
    connect(encryptionCheckBox, &QCheckBox::toggled, [=](bool checked) {
        if (checked) {
            // 启用加密模式
            bool ok;
            QString password = QInputDialog::getText(dialog, "设置加密密码", 
                                                   "请设置配置文件加密密码:", 
                                                   QLineEdit::Password, "", &ok);
            if (!ok || password.isEmpty()) {
                // 用户取消或未输入密码，保持原有状态
                encryptionCheckBox->setChecked(encryptionEnabled);
                return;
            }
            
            // 验证密码强度
            if (password.length() < 6) {
                QMessageBox::warning(dialog, "密码强度不足", 
                    "密码长度至少为6个字符，请重新设置。");
                encryptionCheckBox->setChecked(encryptionEnabled);
                return;
            }
            
            // 保存密码到缓存
            savePasswordForUser("__config__", password);
            
            // 加密配置文件
            if (encryptConfigFile(password)) {
                encryptionEnabled = true;
                QMessageBox::information(dialog, "加密已启用", 
                    "数据加密已启用。配置文件已加密，用户数据将使用加密存储。");
            } else {
                QMessageBox::warning(dialog, "加密失败", 
                    "配置文件加密失败，请重试。");
                encryptionCheckBox->setChecked(encryptionEnabled);
                return;
            }
        } else {
            // 禁用加密模式
            if (encryptionEnabled) {
                // 当前是加密模式，需要先解密
                int attemptCount = 0;
                bool passwordCorrect = false;
                QString password;
                
                while (attemptCount < 5 && !passwordCorrect) {
                    // 获取缓存的密码，如果没有则提示用户输入
                    password = getPasswordForUser("__config__");
                    if (password.isEmpty()) {
                        // 如果没有缓存密码，提示用户输入
                        bool ok;
                        password = QInputDialog::getText(dialog, "输入解密密码", 
                                                       "请输入配置文件解密密码:", 
                                                       QLineEdit::Password, "", &ok);
                        if (!ok || password.isEmpty()) {
                            // 用户取消或未输入密码，保持加密模式
                            encryptionCheckBox->setChecked(true);
                            return;
                        }
                    }
                    
                    // 验证密码并解密配置
                    QFile file("config.dat");
                    if (file.exists()) {
                        if (decryptConfigFile(password, 0)) {
                            passwordCorrect = true;
                        } else {
                            attemptCount++;
                            QMessageBox::warning(dialog, "解密失败", 
                                QString("密码错误，无法禁用加密模式。剩余尝试次数: %1").arg(5 - attemptCount));
                            
                            // 清除缓存的密码，以便下次重新输入
                            savePasswordForUser("__config__", "");
                            password.clear();
                        }
                    } else {
                        // 如果加密文件不存在，直接切换到明文模式
                        passwordCorrect = true;
                    }
                }
                
                if (!passwordCorrect) {
                    // 达到5次错误，初始化程序
                    QMessageBox::critical(dialog, "密码错误次数过多", 
                        "密码错误次数已达5次，程序将初始化。");
                    
                    // 关闭设置对话框
                    dialog->accept();
                    
                    // 初始化程序
                    initializeProgram();
                    return;
                }
                
                // 切换到明文模式
                encryptionEnabled = false;
                
                // 保存为明文配置
                QSettings settings("config.ini", QSettings::IniFormat);
                settings.setValue("theme/isDark", isDarkTheme);
                settings.setValue("test/readInterval", readInterval);
                settings.setValue("speech/engine", speechEngine);
                settings.setValue("test/randomOrder", isRandomOrder);
                settings.setValue("security/encryptionEnabled", false);
                settings.sync();
                
                // 删除加密配置文件
                QFile::remove("config.dat");
                
                // 清除密码缓存
                savePasswordForUser("__config__", "");
                
                QMessageBox::information(dialog, "加密已禁用", 
                    "数据加密已禁用。配置文件已转换为明文，用户数据将使用明文存储。");
            }
        }
    });
    
    // 对话框按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, dialog);
    mainLayout->addWidget(buttonBox);

    // 连接信号和槽，实现设置即时生效
    connect(themeToggleButton, &QPushButton::clicked, [=]() {
        isDarkTheme = !isDarkTheme;
        toggleTheme();
        themeToggleButton->setText(isDarkTheme ? "切换到浅色主题" : "切换到深色主题");
    });
    
    connect(intervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        readInterval = value;
        saveSettings(); // 立即保存设置
    });
    
    connect(engineComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        speechEngine = index;
        saveSettings(); // 立即保存设置
    });
    
    connect(modelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        // 这里可以保存模型选择
        saveSettings(); // 立即保存设置
    });
    

    
            // 试听功能
            connect(previewButton, &QPushButton::clicked, [=]() {
                // 获取当前选择的模型
                QString selectedModel;
                switch (modelComboBox->currentIndex()) {
                    case 0: selectedModel = "cmu_us_slt"; break;
                    case 1: selectedModel = "cmu_us_awb"; break;
                    case 2: selectedModel = "cmu_us_bdl"; break;
                    case 3: selectedModel = "cmu_us_jmk"; break;
                    case 4: selectedModel = "cmu_us_ksp"; break;
                    case 5: selectedModel = "cmu_us_rms"; break;
                    default: selectedModel = "cmu_us_slt"; break;
                }
                
                // 试听功能
                if (speechEngine == 1) { // Flite引擎
                    // 获取应用程序当前目录
                    QString appDir = QCoreApplication::applicationDirPath();
                    
                    // 优先检查项目根目录下的flite.exe
                    QString flitePath = appDir + "/flite.exe";
    
                    // 检查项目根目录下的flite.exe是否存在
                    if (!QFile::exists(flitePath)) {
                        // 检查runtime目录下的flite.exe
                        flitePath = appDir + "/runtime/flite.exe";
                        
                        // 检查runtime目录下的flite.exe是否存在
                        if (!QFile::exists(flitePath)) {
                            // 如果runtime目录下不存在，尝试直接调用flite.exe（系统路径中）
                            flitePath = "flite.exe";
                        }
                    }
                    
                    // 检查Flite可执行文件是否存在
                    if (!QFile::exists(flitePath)) {
                        QMessageBox::warning(dialog, "Flite未找到", 
                            "Flite语音引擎未找到，请检查安装或切换到SAPI引擎。");
                        return;
                    }
                    
                    // 使用QProcess测试Flite引擎
                    QProcess process;
                    QStringList arguments;
                    arguments << "-t" << "Hello" << "-voice" << selectedModel;
                    
                    // 启动进程并等待完成
                    process.start(flitePath, arguments);
                    if(process.waitForStarted(3000)) {  // 等待最多3秒启动
                        process.waitForFinished(5000);  // 等待最多5秒完成
                        
                        // 检查进程是否成功执行
                        if(process.exitCode() != 0) {
                            QMessageBox::warning(dialog, "朗读失败", 
                                QString("Flite引擎朗读失败，错误码: %1").arg(process.exitCode()));
                        }
                    } else {
                        QMessageBox::warning(dialog, "启动失败", 
                            QString("无法启动Flite引擎: %1").arg(flitePath));
                    }
                } else { // SAPI引擎
                    // 构建VBScript代码
                    QString vbsCode = "Dim voice\n";
                    vbsCode += "Set voice = CreateObject(\"SAPI.SpVoice\")\n";
                    vbsCode += "voice.Speak \"Hello\"\n";
                    
                    // 创建临时VBS文件
                    QString tempPath = QDir::tempPath();
                    QString vbsFile = tempPath + "/temp_speak_preview.vbs";
                    
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
            });
    
            connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
            connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    
            // 显示对话框
            dialog->exec();
            
            // 清理资源
            dialog->deleteLater();
        }

void MainWindow::toggleRandomOrder()
{
    isRandomOrder = !isRandomOrder;
    
    // 保存设置
    saveSettings();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 在测试界面时处理快捷键
    if (testWidget && testWidget->isVisible()) {
        switch (event->key()) {
            case Qt::Key_Space:
                // 空格键 - 重复朗读当前单词
                onRepeatWord();
                break;
            case Qt::Key_Left:
                // 左方向键 - 上一个单词
                onPreviousWord();
                break;
            case Qt::Key_Right:
                // 右方向键 - 下一个单词
                onNextWordClicked();
                break;
            case Qt::Key_Escape:
                // ESC键 - 暂停/继续测试
                onPauseResumeTest();
                break;
            default:
                QMainWindow::keyPressEvent(event);
                break;
        }
    }
    // 如果在其他界面也可以添加相应的快捷键
    else {
        QMainWindow::keyPressEvent(event);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 处理用户状态标签的鼠标点击事件
    if (obj == userStatusLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            showUserLoginDialog();
            return true; // 事件已处理
        }
    }
    
    // 其他事件交给父类处理
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::recordTestResult(int correctCount, int totalWords, const QString& wordListName)
{
    TestResult result;
    result.timestamp = QDateTime::currentDateTime();
    result.totalWords = totalWords;
    result.correctCount = correctCount;
    result.accuracy = totalWords > 0 ? (double)correctCount / totalWords * 100.0 : 0.0;
    result.wordListName = wordListName;
    
    testHistory.push_back(result);
    
    // 限制历史记录数量，保留最近的50条记录
    if (testHistory.size() > 50) {
        testHistory.erase(testHistory.begin());
    }
    
    // 保存到文件
    saveTestHistory();
}

void MainWindow::saveTestHistory()
{
    QFile file("test_history.json");
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    
    QJsonArray historyArray;
    for (const auto& result : testHistory) {
        QJsonObject obj;
        obj["timestamp"] = result.timestamp.toString(Qt::ISODate);
        obj["totalWords"] = result.totalWords;
        obj["correctCount"] = result.correctCount;
        obj["accuracy"] = result.accuracy;
        obj["wordListName"] = result.wordListName;
        historyArray.append(obj);
    }
    
    QJsonDocument doc(historyArray);
    file.write(doc.toJson());
}

void MainWindow::loadTestHistory()
{
    QFile file("test_history.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isArray()) {
        QJsonArray historyArray = doc.array();
        testHistory.clear();
        
        for (const auto& value : historyArray) {
            QJsonObject obj = value.toObject();
            TestResult result;
            result.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
            result.totalWords = obj["totalWords"].toInt();
            result.correctCount = obj["correctCount"].toInt();
            result.accuracy = obj["accuracy"].toDouble();
            result.wordListName = obj["wordListName"].toString();
            testHistory.push_back(result);
        }
    }
}

QWidget* MainWindow::createProgressChartWidget()
{
    // 创建主容器
    QWidget *containerWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 添加统计摘要
    QWidget *summaryWidget = new QWidget();
    QGridLayout *summaryLayout = new QGridLayout(summaryWidget);
    summaryLayout->setSpacing(15);
    
    // 计算统计数据
    int totalTests = testHistory.size();
    double avgAccuracy = 0.0;
    int totalCorrect = 0;
    int totalWords = 0;
    int totalStudyTime = 0;
    
    for (const auto& result : testHistory) {
        avgAccuracy += result.accuracy;
        totalCorrect += result.correctCount;
        totalWords += result.totalWords;
    }
    
    if (totalTests > 0) {
        avgAccuracy /= totalTests;
    }
    
    // 添加统计项
    QLabel *totalTestsLabel = new QLabel();
    totalTestsLabel->setText(QString("<b>总测试次数:</b> %1").arg(totalTests));
    summaryLayout->addWidget(totalTestsLabel, 0, 0);
    
    QLabel *avgAccuracyLabel = new QLabel();
    avgAccuracyLabel->setText(QString("<b>平均准确率:</b> %1%").arg(QString::number(avgAccuracy, 'f', 1)));
    summaryLayout->addWidget(avgAccuracyLabel, 0, 1);
    
    QLabel *totalCorrectLabel = new QLabel();
    totalCorrectLabel->setText(QString("<b>总正确数:</b> %1").arg(totalCorrect));
    summaryLayout->addWidget(totalCorrectLabel, 1, 0);
    
    QLabel *totalWordsLabel = new QLabel();
    totalWordsLabel->setText(QString("<b>总测试词数:</b> %1").arg(totalWords));
    summaryLayout->addWidget(totalWordsLabel, 1, 1);
    
    mainLayout->addWidget(summaryWidget);
    
    // 创建图表视图
    QChart *chart = new QChart();
    chart->setTitle("学习进度趋势");
    chart->setTitleFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    
    // 创建折线系列 - 准确率
    QLineSeries *accuracySeries = new QLineSeries();
    accuracySeries->setName("准确率 (%)");
    accuracySeries->setColor(QColor(0, 123, 255));
    
    // 创建柱状系列 - 正确数量
    QBarSeries *barSeries = new QBarSeries();
    barSeries->setName("正确数");
    
    // 准备数据
    QStringList dateTimeLabels;
    int index = 0;
    for (const auto& result : testHistory) {
        QPointF point(index, result.accuracy);
        accuracySeries->append(point);
        
        QBarSet *barSet = new QBarSet(result.timestamp.toString("MM-dd"));
        *barSet << result.correctCount;
        barSet->setColor(QColor(40, 167, 69));
        barSeries->append(barSet);
        
        dateTimeLabels << result.timestamp.toString("MM-dd\nhh:mm");
        index++;
    }
    
    // 添加系列到图表
    if (!testHistory.empty()) {
        chart->addSeries(accuracySeries);
        chart->addSeries(barSeries);
        
        // 配置X轴
        QValueAxis *axisX = new QValueAxis();
        axisX->setTickCount(dateTimeLabels.size());
        axisX->setRange(0, qMax(1, static_cast<int>(dateTimeLabels.size()) - 1));
        axisX->setTitleText("测试时间");
        
        // 配置Y轴
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, 100);
        axisY->setTitleText("准确率 (%)");
        
        // 将轴添加到图表
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        
        accuracySeries->attachAxis(axisX);
        accuracySeries->attachAxis(axisY);
        
        barSeries->attachAxis(axisX);
        QValueAxis *barAxisY = new QValueAxis();
        int maxCorrect = 0;
        for (const auto& result : testHistory) {
            if (result.correctCount > maxCorrect) {
                maxCorrect = result.correctCount;
            }
        }
        barAxisY->setRange(0, maxCorrect > 0 ? maxCorrect * 1.2 : 20);
        barAxisY->setTitleText("正确数");
        chart->addAxis(barAxisY, Qt::AlignRight);
        barSeries->attachAxis(barAxisY);
        
        // 设置图表样式
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
    } else {
        // 无数据时显示提示
        QLabel *noDataLabel = new QLabel("暂无学习数据");
        noDataLabel->setAlignment(Qt::AlignCenter);
        noDataLabel->setFont(QFont("Microsoft YaHei", 14));
        // 创建一个容器widget来显示提示信息
        QWidget *noDataWidget = new QWidget();
        QVBoxLayout *noDataLayout = new QVBoxLayout(noDataWidget);
        noDataLayout->addWidget(noDataLabel);
        noDataLayout->setAlignment(Qt::AlignCenter);
        chart->setParent(noDataWidget);
    }
    
    // 创建图表视图
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(400);
    
    mainLayout->addWidget(chartView);
    
    // 添加词库学习进度
    if (!wordlistFiles.isEmpty()) {
        QWidget *wordlistProgressWidget = new QWidget();
        QVBoxLayout *wordlistLayout = new QVBoxLayout(wordlistProgressWidget);
        
        QLabel *wordlistTitle = new QLabel("词库学习进度");
        QFont titleFont = wordlistTitle->font();
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        wordlistTitle->setFont(titleFont);
        wordlistLayout->addWidget(wordlistTitle);
        
        // 创建词库进度列表
        QListWidget *wordlistList = new QListWidget();
        for (auto it = wordlistFiles.constBegin(); it != wordlistFiles.constEnd(); ++it) {
            QString wordlistName = it.key();
            QListWidgetItem *item = new QListWidgetItem(wordlistName);
            
            // 计算该词库的学习次数
            int studyCount = 0;
            for (const auto& result : testHistory) {
                if (result.wordListName == wordlistName) {
                    studyCount++;
                }
            }
            
            item->setToolTip(QString("学习次数: %1").arg(studyCount));
            wordlistList->addItem(item);
        }
        wordlistLayout->addWidget(wordlistList);
        
        mainLayout->addWidget(wordlistProgressWidget);
    }
    
    return containerWidget;
}

void MainWindow::showProgressChart()
{
    // 加载历史记录
    loadTestHistory();
    
    // 创建进度对话框
    QDialog progressDialog(this);
    progressDialog.setWindowTitle("学习进度可视化");
    progressDialog.resize(900, 700);
    
    // 创建对话框主布局
    QVBoxLayout *dialogLayout = new QVBoxLayout(&progressDialog);
    dialogLayout->setSpacing(10);
    dialogLayout->setContentsMargins(10, 10, 10, 10);
    
    // 创建标题
    QLabel *titleLabel = new QLabel("学习进度统计", &progressDialog);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // 创建统计内容
    QWidget *chartWidget = createProgressChartWidget();
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // 创建导出按钮
    QPushButton *exportButton = new QPushButton("导出统计", &progressDialog);
    QFont buttonFont = exportButton->font();
    buttonFont.setPointSize(10);
    exportButton->setFont(buttonFont);
    exportButton->setMinimumSize(QSize(100, 30));
    
    // 创建关闭按钮
    QPushButton *closeButton = new QPushButton("关闭", &progressDialog);
    closeButton->setFont(buttonFont);
    closeButton->setMinimumSize(QSize(100, 30));
    
    // 连接按钮
    connect(closeButton, &QPushButton::clicked, &progressDialog, &QDialog::accept);
    
    connect(exportButton, &QPushButton::clicked, [this, &progressDialog]() {
        QString fileName = QFileDialog::getSaveFileName(const_cast<QWidget*>(static_cast<const QWidget*>(&progressDialog)), "导出统计", "学习统计.csv", "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << "测试时间,词库名称,总单词数,正确数,准确率" << "\n";
                
                for (const auto& result : testHistory) {
                    out << result.timestamp.toString("yyyy-MM-dd HH:mm:ss") << ",";
                    out << result.wordListName << ",";
                    out << result.totalWords << ",";
                    out << result.correctCount << ",";
                    out << QString::number(result.accuracy, 'f', 1) << "%" << "\n";
                }
                
                file.close();
                QMessageBox::information(const_cast<QWidget*>(static_cast<const QWidget*>(&progressDialog)), "导出成功", "学习统计已成功导出到CSV文件");
            }
        }
    });
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(exportButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    
    // 将组件添加到布局
    dialogLayout->addWidget(titleLabel);
    dialogLayout->addWidget(chartWidget);
    dialogLayout->addLayout(buttonLayout);
    
    // 显示对话框
    progressDialog.exec();
}

void MainWindow::setupWordsTextEditWatcher()
{
    if (!wordsTextEdit) return;
    
    // 创建定时器用于监控文本变化
    if (!wordsTextChangedTimer) {
        wordsTextChangedTimer = new QTimer(this);
        wordsTextChangedTimer->setSingleShot(true);
        connect(wordsTextChangedTimer, &QTimer::timeout, this, &MainWindow::syncWordsTextEditToTempFile);
    }
    
    // 连接文本变化信号
    connect(wordsTextEdit, &QTextEdit::textChanged, this, [=]() {
        // 每次文本变化时重启定时器（防抖）
        if (wordsTextChangedTimer) {
            wordsTextChangedTimer->stop();
            wordsTextChangedTimer->start(500); // 500ms延迟，避免频繁保存
        }
    });
    
    // 初始化时加载临时文件内容
    if (QFile::exists(tempWordlistFile)) {
        QFile file(tempWordlistFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = file.readAll();
            wordsTextEdit->setPlainText(content);
            lastWordsText = content;
        }
    }
}

void MainWindow::syncWordsTextEditToTempFile()
{
    if (!wordsTextEdit) return;
    
    QString currentText = wordsTextEdit->toPlainText();
    
    // 检查内容是否真的发生了变化
    if (currentText == lastWordsText) return;
    
    // 更新lastWordsText
    lastWordsText = currentText;
    
    // 如果文本为空，则不创建临时文件或删除已有的临时文件
    if (currentText.trimmed().isEmpty()) {
        if (QFile::exists(tempWordlistFile)) {
            QFile::remove(tempWordlistFile);
        }
        return;
    }
    
    // 创建临时词库文件路径（如果不存在）
    if (tempWordlistFile.isEmpty()) {
        createTempWordlist();
    }
    
    // 保存到临时文件
    QFile file(tempWordlistFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not create temporary file:" << tempWordlistFile << "Error:" << file.errorString();
        return;
    }
    
    QTextStream out(&file);
    out << currentText;
    file.close();
    
    // 同时更新words向量，以便可以直接开始听写
    updateWordsFromTextEdit();
}

void MainWindow::updateWordsFromTextEdit()
{
    if (!wordsTextEdit) return;
    
    QString text = wordsTextEdit->toPlainText();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    
    words.clear();
    cachedWords.clear();
    
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.isEmpty()) {
            words.push_back(trimmedLine.toStdString());
            cachedWords.push_back(trimmedLine.toStdString());
        }
    }
}

void MainWindow::cleanupTempFiles()
{
    if (!tempWordlistFile.isEmpty() && QFile::exists(tempWordlistFile)) {
        QFile::remove(tempWordlistFile);
        qDebug() << "Temporary wordlist file deleted:" << tempWordlistFile;
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    // 如果在单词界面显示时，启动监控
    if (wordsWidget && wordsWidget->isVisible()) {
        if (wordsTextChangedTimer && !wordsTextChangedTimer->isActive()) {
            // 立即同步一次，确保界面和数据一致
            syncWordsTextEditToTempFile();
        }
    }
}

void MainWindow::hideEvent(QHideEvent *event)
{
    // 如果隐藏的是单词界面，停止监控
    if (wordsWidget && wordsWidget->isVisible()) {
        if (wordsTextChangedTimer) {
            wordsTextChangedTimer->stop();
        }
    }
    
    QMainWindow::hideEvent(event);
}

// 用户账户系统实现
void MainWindow::initializeUserSystem()
{
    // 获取应用程序数据目录
    userDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    userDataPath += "/users";
    
    // 创建用户数据目录
    QDir userDir(userDataPath);
    if (!userDir.exists()) {
        userDir.mkpath(".");
    }
    
    // 加载所有用户配置
    loadAllUserProfiles();
    
    qDebug() << "User system initialized. Data path:" << userDataPath;
}

void MainWindow::createUser(const QString& username, const QString& nickname)
{
    if (username.isEmpty() || !validateUsername(username)) {
        QMessageBox::warning(this, "创建用户失败", "用户名不合法");
        return;
    }
    
    if (userProfiles.contains(username)) {
        QMessageBox::warning(this, "创建用户失败", "用户已存在");
        return;
    }
    
    // 创建新用户
    UserData newUser;
    newUser.username = username;
    newUser.nickname = nickname.isEmpty() ? username : nickname;
    newUser.createdTime = QDateTime::currentDateTime();
    newUser.lastLoginTime = QDateTime::currentDateTime();
    
    // 复制当前设置
    newUser.isDarkTheme = isDarkTheme;
    newUser.readInterval = readInterval;
    newUser.speechEngine = speechEngine;
    newUser.isRandomOrder = isRandomOrder;
    
    userProfiles[username] = newUser;
    
    // 保存用户配置
    saveUserProfile(username);
    
    QMessageBox::information(this, "创建成功", QString("用户 %1 创建成功").arg(username));
}

bool MainWindow::loginUser(const QString& username, const QString& password)
{
    if (!userProfiles.contains(username)) {
        QMessageBox::warning(this, "登录失败", "用户不存在");
        return false;
    }
    
    UserData& user = userProfiles[username];
    if (!user.isActive) {
        QMessageBox::warning(this, "登录失败", "用户已被禁用");
        return false;
    }
    
    // 加密模式下要求输入密码
    if (encryptionEnabled && password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "加密模式下必须输入密码才能登录");
        return false;
    }
    
    // 保存密码到缓存，用于后续的加密操作
    if (!password.isEmpty()) {
        savePasswordForUser(username, password);
    }
    
    // 更新最后登录时间
    user.lastLoginTime = QDateTime::currentDateTime();
    currentUser = username;
    
    // 加载用户设置
    isDarkTheme = user.isDarkTheme;
    readInterval = user.readInterval;
    speechEngine = user.speechEngine;
    isRandomOrder = user.isRandomOrder;
    
    // 应用主题
    toggleTheme();
    
    // 保存更新后的用户信息
    saveUserProfile(username);
    
    qDebug() << "User logged in:" << username;
    return true;
}

void MainWindow::logoutUser()
{
    if (!currentUser.isEmpty()) {
        // 保存当前用户数据
        updateCurrentUserProfile();
        saveUserProfile(currentUser);
        currentUser.clear();
        qDebug() << "User logged out";
    }
}

void MainWindow::saveUserProfile(const QString& username)
{
    if (encryptionEnabled) {
        // 从用户密码存储中获取密码（需要在登录时保存）
        QString password = getPasswordForUser(username);
        if (!password.isEmpty()) {
            saveEncryptedUserProfile(username, password);
        } else {
            // 如果没有密码，使用明文保存
            savePlainUserProfile(username);
        }
    } else {
        // 禁用加密时使用明文保存
        savePlainUserProfile(username);
    }
}

// 获取用户密码（需要在登录时保存）
QString MainWindow::getPasswordForUser(const QString& username)
{
    // 这里需要实现密码存储机制
    // 临时实现：从内存中的密码缓存获取
    static QMap<QString, QString> passwordCache;
    return passwordCache.value(username);
}

// 保存用户密码到缓存（登录时调用）
void MainWindow::savePasswordForUser(const QString& username, const QString& password)
{
    static QMap<QString, QString> passwordCache;
    passwordCache[username] = password;
}

void MainWindow::loadUserProfile(const QString& username)
{
    if (encryptionEnabled) {
        // 首先尝试加载加密文件
        QString encryptedFilePath = userDataPath + "/" + username + ".enc";
        if (QFile::exists(encryptedFilePath)) {
            // 需要密码才能加载加密文件
            // 这里需要实现密码获取机制
            QString password = getPasswordForUser(username);
            if (!password.isEmpty()) {
                loadEncryptedUserProfile(username, password);
                return;
            }
        }
    }
    
    // 尝试加载明文JSON文件
    loadPlainUserProfile(username);
}

void MainWindow::saveAllUserProfiles()
{
    for (const QString& username : userProfiles.keys()) {
        saveUserProfile(username);
    }
}

void MainWindow::loadAllUserProfiles()
{
    QDir userDir(userDataPath);
    QStringList userFiles = userDir.entryList(QStringList() << "*.json", QDir::Files);
    
    for (const QString& fileName : userFiles) {
        QString username = fileName.left(fileName.lastIndexOf('.'));
        loadUserProfile(username);
    }
}

QString MainWindow::hashPassword(const QString& password)
{
    return QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool MainWindow::validateUsername(const QString& username)
{
    // 用户名验证规则：3-20个字符，只能包含字母、数字、下划线
    QRegularExpression re("^[a-zA-Z0-9_]{3,20}$");
    return re.match(username).hasMatch();
}

void MainWindow::updateCurrentUserProfile()
{
    if (currentUser.isEmpty()) return;
    
    if (userProfiles.contains(currentUser)) {
        UserData& user = userProfiles[currentUser];
        user.isDarkTheme = isDarkTheme;
        user.readInterval = readInterval;
        user.speechEngine = speechEngine;
        user.isRandomOrder = isRandomOrder;
        user.testHistory = testHistory;
    }
}

void MainWindow::showNotification(const QString& title, const QString& message, const QString& iconPath)
{
    // 根据系统架构选择合适的 snoretoast 可执行文件
    QString snoretoastPath;
    
#ifdef _WIN64
    snoretoastPath = QCoreApplication::applicationDirPath() + "/snoretoast-x64.exe";
#else
    snoretoastPath = QCoreApplication::applicationDirPath() + "/snoretoast-x86.exe";
#endif
    
    // 检查文件是否存在
    if (!QFile::exists(snoretoastPath)) {
        qDebug() << "Snoretoast executable not found:" << snoretoastPath;
        return;
    }
    
    // 构建命令行参数
    QStringList arguments;
    arguments << "-t" << title;
    arguments << "-m" << message;
    
    // 如果提供了图标路径且文件存在，则添加图标参数
    if (!iconPath.isEmpty() && QFile::exists(iconPath)) {
        arguments << "-p" << iconPath;
    }
    
    // 添加应用程序ID以避免通知冲突
    arguments << "-id" << ("EnglishListen_" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    
    // 启动进程显示通知
    QProcess* process = new QProcess(this);
    process->start(snoretoastPath, arguments);
    
    // 连接信号以确保进程在完成后被删除
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [process](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);
                process->deleteLater();
            });
    
    // 如果进程启动失败，输出错误信息
    if (!process->waitForStarted(1000)) {
        qDebug() << "Failed to start snoretoast:" << process->errorString();
        process->deleteLater();
    }
}

// 数据加密实现
QByteArray MainWindow::generateEncryptionKey()
{
    // 生成32字节的随机密钥用于AES-256加密
    QByteArray key(32, 0);
    QRandomGenerator::global()->fillRange(reinterpret_cast<quint32*>(key.data()), 8);
    return key;
}

// 使用PBKDF2从密码派生密钥
QByteArray MainWindow::deriveKeyFromPassword(const QString& password, const QByteArray& salt, int iterations)
{
    if (password.isEmpty()) return QByteArray();
    
    // 使用SHA-256作为哈希算法，派生32字节密钥（AES-256）
    QByteArray key(32, 0);
    QByteArray passwordBytes = password.toUtf8();
    
    // PBKDF2算法实现
    for (int i = 1; i <= iterations; ++i) {
        // 计算 HMAC(SHA-256, salt || i)
        QByteArray data = salt;
        data.append(reinterpret_cast<const char*>(&i), sizeof(i));
        
        QByteArray hash = QCryptographicHash::hash(passwordBytes + data, QCryptographicHash::Sha256);
        
        // 累计异或结果
        for (int j = 0; j < 32 && j < hash.size(); ++j) {
            key[j] = key[j] ^ hash[j];
        }
        
        // 为下一次迭代准备数据
        passwordBytes = hash;
    }
    
    return key;
}

// 改进的XOR加密（临时方案，后续替换为AES）
QByteArray MainWindow::encryptData(const QByteArray& data, const QByteArray& key)
{
    if (data.isEmpty() || key.isEmpty()) return data;
    
    QByteArray encryptedData;
    encryptedData.reserve(data.size());
    
    for (int i = 0; i < data.size(); ++i) {
        encryptedData.append(data.at(i) ^ key.at(i % key.size()));
    }
    
    return encryptedData;
}

QByteArray MainWindow::decryptData(const QByteArray& encryptedData, const QByteArray& key)
{
    // XOR解密与加密使用相同的操作
    return encryptData(encryptedData, key);
}

QString MainWindow::encryptString(const QString& plaintext)
{
    if (plaintext.isEmpty()) return plaintext;
    
    // 生成会话密钥
    QByteArray sessionKey = generateEncryptionKey();
    
    // 加密数据
    QByteArray plainBytes = plaintext.toUtf8();
    QByteArray encryptedBytes = encryptData(plainBytes, sessionKey);
    
    // 将密钥和加密数据组合
    QByteArray combined;
    QDataStream stream(&combined, QIODevice::WriteOnly);
    stream << sessionKey << encryptedBytes;
    
    // Base64编码以便存储
    return QString::fromLatin1(combined.toBase64());
}

QString MainWindow::decryptString(const QString& ciphertext)
{
    if (ciphertext.isEmpty()) return ciphertext;
    
    // Base64解码
    QByteArray combined = QByteArray::fromBase64(ciphertext.toLatin1());
    if (combined.isEmpty()) return QString();
    
    // 分离密钥和加密数据
    QDataStream stream(&combined, QIODevice::ReadOnly);
    QByteArray sessionKey;
    QByteArray encryptedBytes;
    stream >> sessionKey >> encryptedBytes;
    
    // 解密数据
    QByteArray decryptedBytes = decryptData(encryptedBytes, sessionKey);
    return QString::fromUtf8(decryptedBytes);
}

void MainWindow::saveEncryptedUserProfile(const QString& username, const QString& password)
{
    if (!userProfiles.contains(username) || password.isEmpty()) return;
    
    UserData& user = userProfiles[username];
    QString userFilePath = userDataPath + "/" + username + ".enc";
    
    // 准备要加密的用户数据
    QJsonObject userDataObj;
    userDataObj["username"] = user.username;
    userDataObj["nickname"] = user.nickname; // 不再单独加密字符串
    userDataObj["createdTime"] = user.createdTime.toString(Qt::ISODate);
    userDataObj["lastLoginTime"] = user.lastLoginTime.toString(Qt::ISODate);
    userDataObj["isActive"] = user.isActive;
    userDataObj["totalStudyTime"] = user.totalStudyTime;
    userDataObj["completedTests"] = user.completedTests;
    userDataObj["isDarkTheme"] = user.isDarkTheme;
    userDataObj["readInterval"] = user.readInterval;
    userDataObj["speechEngine"] = user.speechEngine;
    userDataObj["isRandomOrder"] = user.isRandomOrder;
    
    // 隐私设置
    userDataObj["allowDataCollection"] = user.allowDataCollection;
    userDataObj["allowCloudSync"] = user.allowCloudSync;
    userDataObj["allowAnalytics"] = user.allowAnalytics;
    userDataObj["shareLearningStats"] = user.shareLearningStats;
    
    // 词库列表
    QJsonArray wordLists;
    for (const QString& wordList : user.wordLists) {
        wordLists.append(wordList); // 不再单独加密字符串
    }
    userDataObj["wordLists"] = wordLists;
    
    // 测试历史
    QJsonArray testHistory;
    for (const auto& result : user.testHistory) {
        QJsonObject resultObj;
        resultObj["timestamp"] = result.timestamp.toString(Qt::ISODate);
        resultObj["totalWords"] = result.totalWords;
        resultObj["correctCount"] = result.correctCount;
        resultObj["accuracy"] = result.accuracy;
        resultObj["wordListName"] = result.wordListName; // 不再单独加密字符串
        testHistory.append(resultObj);
    }
    userDataObj["testHistory"] = testHistory;
    
    // 生成16字节的随机盐值
    QByteArray salt(16, 0);
    QRandomGenerator::global()->fillRange(reinterpret_cast<quint32*>(salt.data()), 4);
    
    // 使用PBKDF2派生密钥
    QByteArray key = deriveKeyFromPassword(password, salt);
    
    // 加密整个JSON对象
    QJsonDocument doc(userDataObj);
    QByteArray jsonData = doc.toJson();
    QByteArray encryptedData = encryptData(jsonData, key);
    
    // 保存盐值和加密数据（不保存密钥）
    QFile file(userFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << salt << encryptedData;
        file.close();
        qDebug() << "Encrypted user profile saved for:" << username;
    }
}

void MainWindow::loadEncryptedUserProfile(const QString& username, const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) return;
    
    QString userFilePath = userDataPath + "/" + username + ".enc";
    
    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly)) return;
    
    // 读取盐值和加密数据
    QDataStream in(&file);
    QByteArray salt;
    QByteArray encryptedData;
    in >> salt >> encryptedData;
    file.close();
    
    if (salt.isEmpty() || encryptedData.isEmpty()) return;
    
    // 使用PBKDF2派生密钥
    QByteArray key = deriveKeyFromPassword(password, salt);
    
    // 解密数据
    QByteArray decryptedData = decryptData(encryptedData, key);
    QJsonDocument doc = QJsonDocument::fromJson(decryptedData);
    
    if (!doc.isObject()) return;
    
    QJsonObject userDataObj = doc.object();
    UserData user;
    
    user.username = userDataObj["username"].toString();
    user.nickname = userDataObj["nickname"].toString(); // 不再需要解密字符串
    user.createdTime = QDateTime::fromString(userDataObj["createdTime"].toString(), Qt::ISODate);
    user.lastLoginTime = QDateTime::fromString(userDataObj["lastLoginTime"].toString(), Qt::ISODate);
    user.isActive = userDataObj["isActive"].toBool(true);
    user.totalStudyTime = userDataObj["totalStudyTime"].toInt(0);
    user.completedTests = userDataObj["completedTests"].toInt(0);
    user.isDarkTheme = userDataObj["isDarkTheme"].toBool(false);
    user.readInterval = userDataObj["readInterval"].toInt(5);
    user.speechEngine = userDataObj["speechEngine"].toInt(0);
    user.isRandomOrder = userDataObj["isRandomOrder"].toBool(false);
    
    // 隐私设置
    user.allowDataCollection = userDataObj["allowDataCollection"].toBool(false);
    user.allowCloudSync = userDataObj["allowCloudSync"].toBool(false);
    user.allowAnalytics = userDataObj["allowAnalytics"].toBool(false);
    user.shareLearningStats = userDataObj["shareLearningStats"].toBool(false);
    
    // 词库列表
    QJsonArray wordLists = userDataObj["wordLists"].toArray();
    for (const QJsonValue& value : wordLists) {
        user.wordLists.append(value.toString()); // 不再需要解密字符串
    }
    
    // 测试历史
    QJsonArray testHistory = userDataObj["testHistory"].toArray();
    for (const QJsonValue& value : testHistory) {
        QJsonObject resultObj = value.toObject();
        TestResult result;
        result.timestamp = QDateTime::fromString(resultObj["timestamp"].toString(), Qt::ISODate);
        result.totalWords = resultObj["totalWords"].toInt();
        result.correctCount = resultObj["correctCount"].toInt();
        result.accuracy = resultObj["accuracy"].toDouble();
        result.wordListName = resultObj["wordListName"].toString(); // 不再需要解密字符串
        user.testHistory.push_back(result);
    }
    
    userProfiles[username] = user;
    qDebug() << "Encrypted user profile loaded for:" << username;
}

void MainWindow::savePlainUserProfile(const QString& username)
{
    if (!userProfiles.contains(username)) return;
    
    UserData& user = userProfiles[username];
    QString userFilePath = userDataPath + "/" + username + ".json";
    
    // 准备用户数据
    QJsonObject userDataObj;
    userDataObj["username"] = user.username;
    userDataObj["nickname"] = user.nickname;
    userDataObj["createdTime"] = user.createdTime.toString(Qt::ISODate);
    userDataObj["lastLoginTime"] = user.lastLoginTime.toString(Qt::ISODate);
    userDataObj["isActive"] = user.isActive;
    userDataObj["totalStudyTime"] = user.totalStudyTime;
    userDataObj["completedTests"] = user.completedTests;
    userDataObj["isDarkTheme"] = user.isDarkTheme;
    userDataObj["readInterval"] = user.readInterval;
    userDataObj["speechEngine"] = user.speechEngine;
    userDataObj["isRandomOrder"] = user.isRandomOrder;
    
    // 隐私设置
    userDataObj["allowDataCollection"] = user.allowDataCollection;
    userDataObj["allowCloudSync"] = user.allowCloudSync;
    userDataObj["allowAnalytics"] = user.allowAnalytics;
    userDataObj["shareLearningStats"] = user.shareLearningStats;
    
    // 词库列表
    QJsonArray wordLists;
    for (const QString& wordList : user.wordLists) {
        wordLists.append(wordList);
    }
    userDataObj["wordLists"] = wordLists;
    
    // 测试历史
    QJsonArray testHistory;
    for (const auto& result : user.testHistory) {
        QJsonObject resultObj;
        resultObj["timestamp"] = result.timestamp.toString(Qt::ISODate);
        resultObj["totalWords"] = result.totalWords;
        resultObj["correctCount"] = result.correctCount;
        resultObj["accuracy"] = result.accuracy;
        resultObj["wordListName"] = result.wordListName;
        testHistory.append(resultObj);
    }
    userDataObj["testHistory"] = testHistory;
    
    // 保存为JSON文件
    QJsonDocument doc(userDataObj);
    QFile file(userFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Plain user profile saved for:" << username;
    }
}

void MainWindow::loadPlainUserProfile(const QString& username)
{
    if (username.isEmpty()) return;
    
    QString userFilePath = userDataPath + "/" + username + ".json";
    
    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) return;
    
    QJsonObject userDataObj = doc.object();
    UserData user;
    
    user.username = userDataObj["username"].toString();
    user.nickname = userDataObj["nickname"].toString();
    user.createdTime = QDateTime::fromString(userDataObj["createdTime"].toString(), Qt::ISODate);
    user.lastLoginTime = QDateTime::fromString(userDataObj["lastLoginTime"].toString(), Qt::ISODate);
    user.isActive = userDataObj["isActive"].toBool(true);
    user.totalStudyTime = userDataObj["totalStudyTime"].toInt(0);
    user.completedTests = userDataObj["completedTests"].toInt(0);
    user.isDarkTheme = userDataObj["isDarkTheme"].toBool(false);
    user.readInterval = userDataObj["readInterval"].toInt(5);
    user.speechEngine = userDataObj["speechEngine"].toInt(0);
    user.isRandomOrder = userDataObj["isRandomOrder"].toBool(false);
    
    // 隐私设置
    user.allowDataCollection = userDataObj["allowDataCollection"].toBool(false);
    user.allowCloudSync = userDataObj["allowCloudSync"].toBool(false);
    user.allowAnalytics = userDataObj["allowAnalytics"].toBool(false);
    user.shareLearningStats = userDataObj["shareLearningStats"].toBool(false);
    
    // 词库列表
    QJsonArray wordLists = userDataObj["wordLists"].toArray();
    for (const QJsonValue& value : wordLists) {
        user.wordLists.append(value.toString());
    }
    
    // 测试历史
    QJsonArray testHistory = userDataObj["testHistory"].toArray();
    for (const QJsonValue& value : testHistory) {
        QJsonObject resultObj = value.toObject();
        TestResult result;
        result.timestamp = QDateTime::fromString(resultObj["timestamp"].toString(), Qt::ISODate);
        result.totalWords = resultObj["totalWords"].toInt();
        result.correctCount = resultObj["correctCount"].toInt();
        result.accuracy = resultObj["accuracy"].toDouble();
        result.wordListName = resultObj["wordListName"].toString();
        user.testHistory.push_back(result);
    }
    
    userProfiles[username] = user;
    qDebug() << "Plain user profile loaded for:" << username;
}

// 用户界面实现
void MainWindow::showUserLoginDialog()
{
    QDialog loginDialog(this);
    loginDialog.setWindowTitle("用户登录");
    loginDialog.setModal(true);
    loginDialog.resize(300, 200);
    
    QVBoxLayout *layout = new QVBoxLayout(&loginDialog);
    
    // 标题
    QLabel *titleLabel = new QLabel("请选择或创建用户", &loginDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    
    // 用户列表
    QListWidget *userList = new QListWidget(&loginDialog);
    userList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 填充用户列表
    for (const QString& username : userProfiles.keys()) {
        UserData& user = userProfiles[username];
        QString displayText = QString("%1 (%2)").arg(user.nickname).arg(username);
        if (username == currentUser) {
            displayText += " [当前登录]";
        }
        QListWidgetItem *item = new QListWidgetItem(displayText, userList);
        item->setData(Qt::UserRole, username);
    }
    
    layout->addWidget(userList);
    
    // 密码输入框
    QLabel *passwordLabel = new QLabel("密码:", &loginDialog);
    QLineEdit *passwordEdit = new QLineEdit(&loginDialog);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("输入密码（加密模式下需要）");
    
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordEdit);
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *loginButton = new QPushButton("登录", &loginDialog);
    QPushButton *createButton = new QPushButton("创建新用户", &loginDialog);
    QPushButton *cancelButton = new QPushButton("取消", &loginDialog);
    
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(cancelButton);
    
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(loginButton, &QPushButton::clicked, [&]() {
        if (userList->currentItem()) {
            QString selectedUser = userList->currentItem()->data(Qt::UserRole).toString();
            QString password = passwordEdit->text();
            if (loginUser(selectedUser, password)) {
                updateUserMenu();
                loginDialog.accept();
            }
        }
    });
    
    connect(createButton, &QPushButton::clicked, [&]() {
        bool ok;
        QString username = QInputDialog::getText(&loginDialog, "创建用户", 
                                               "请输入用户名 (3-20个字符，字母数字下划线):", 
                                               QLineEdit::Normal, "", &ok);
        if (ok && !username.isEmpty()) {
            if (validateUsername(username)) {
                QString nickname = QInputDialog::getText(&loginDialog, "设置昵称", 
                                                       "请输入昵称 (可选):", 
                                                       QLineEdit::Normal, username, &ok);
                if (ok) {
                    // 创建用户时也需要密码
                    QString password = QInputDialog::getText(&loginDialog, "设置密码", 
                                                           "请输入密码 (加密模式下需要):", 
                                                           QLineEdit::Password, "", &ok);
                    if (ok) {
                        createUser(username, nickname);
                        // 保存密码到缓存
                        savePasswordForUser(username, password);
                        // 保存用户数据（可能会加密）
                        saveUserProfile(username);
                        // 刷新用户列表
                        userList->clear();
                        for (const QString& uname : userProfiles.keys()) {
                            UserData& user = userProfiles[uname];
                            QString displayText = QString("%1 (%2)").arg(user.nickname).arg(uname);
                            QListWidgetItem *item = new QListWidgetItem(displayText, userList);
                            item->setData(Qt::UserRole, uname);
                        }
                    }
                }
            } else {
                QMessageBox::warning(&loginDialog, "创建失败", "用户名格式不正确");
            }
        }
    });
    
    connect(cancelButton, &QPushButton::clicked, &loginDialog, &QDialog::reject);
    
    // 双击用户列表项直接登录（需要输入密码）
    connect(userList, &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item) {
        QString selectedUser = item->data(Qt::UserRole).toString();
        QString password = passwordEdit->text();
        if (loginUser(selectedUser, password)) {
            updateUserMenu();
            loginDialog.accept();
        }
    });
    
    loginDialog.exec();
}

void MainWindow::showUserProfileDialog()
{
    if (currentUser.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先登录用户");
        return;
    }
    
    UserData& user = userProfiles[currentUser];
    
    QDialog profileDialog(this);
    profileDialog.setWindowTitle("个人资料");
    profileDialog.setModal(true);
    profileDialog.resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(&profileDialog);
    
    // 标题
    QLabel *titleLabel = new QLabel(QString("用户: %1").arg(user.nickname), &profileDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    
    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    
    // 昵称输入
    QLineEdit *nicknameEdit = new QLineEdit(user.nickname, &profileDialog);
    formLayout->addRow("昵称:", nicknameEdit);
    
    // 用户名显示
    QLabel *usernameLabel = new QLabel(user.username, &profileDialog);
    formLayout->addRow("用户名:", usernameLabel);
    
    // 创建时间
    QLabel *createTimeLabel = new QLabel(user.createdTime.toString("yyyy-MM-dd hh:mm"), &profileDialog);
    formLayout->addRow("创建时间:", createTimeLabel);
    
    // 最后登录时间
    QLabel *lastLoginLabel = new QLabel(user.lastLoginTime.toString("yyyy-MM-dd hh:mm"), &profileDialog);
    formLayout->addRow("最后登录:", lastLoginLabel);
    
    // 学习统计
    QLabel *studyStatsLabel = new QLabel(QString("总学习时间: %1分钟\n完成测试: %2次")
                                        .arg(user.totalStudyTime).arg(user.completedTests), &profileDialog);
    formLayout->addRow("学习统计:", studyStatsLabel);
    
    layout->addLayout(formLayout);
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("保存更改", &profileDialog);
    QPushButton *logoutButton = new QPushButton("退出登录", &profileDialog);
    QPushButton *closeButton = new QPushButton("关闭", &profileDialog);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(logoutButton);
    buttonLayout->addWidget(closeButton);
    
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString newNickname = nicknameEdit->text().trimmed();
        if (!newNickname.isEmpty() && newNickname != user.nickname) {
            user.nickname = newNickname;
            saveUserProfile(currentUser);
            updateUserMenu();
            titleLabel->setText(QString("用户: %1").arg(user.nickname));
            QMessageBox::information(&profileDialog, "成功", "昵称已更新");
        }
    });
    
    connect(logoutButton, &QPushButton::clicked, [&]() {
        logoutUser();
        updateUserMenu();
        profileDialog.accept();
        QMessageBox::information(this, "提示", "已退出登录");
    });
    
    connect(closeButton, &QPushButton::clicked, &profileDialog, &QDialog::accept);
    
    profileDialog.exec();
}

void MainWindow::updateUserMenu()
{
    // 更新用户状态标签
    if (userStatusLabel) {
        if (!currentUser.isEmpty() && userProfiles.contains(currentUser)) {
            UserData& user = userProfiles[currentUser];
            userStatusLabel->setText(QString("👤 %1").arg(user.nickname));
            userStatusLabel->setStyleSheet("color: green; padding: 5px; font-weight: bold;");
        } else {
            userStatusLabel->setText("👤 未登录");
            userStatusLabel->setStyleSheet("color: gray; padding: 5px;");
        }
    }
    
    // 如果还没有用户菜单，创建它
    if (!settingsButton) return;
    
    // 更新设置按钮的右键菜单
    QMenu *menu = settingsButton->findChild<QMenu*>();
    if (!menu) {
        menu = new QMenu(this);
        settingsButton->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(settingsButton, &QPushButton::customContextMenuRequested, this, [=]() {
            menu->popup(QCursor::pos());
        });
    } else {
        menu->clear();
    }
    
    // 添加用户相关菜单项
    if (!currentUser.isEmpty()) {
        UserData& user = userProfiles[currentUser];
        QAction *userAction = menu->addAction(QString("👤 %1").arg(user.nickname));
        userAction->setEnabled(false);
        menu->addSeparator();
        menu->addAction("个人资料", this, &MainWindow::showUserProfileDialog);
        menu->addAction("切换用户", this, &MainWindow::showUserLoginDialog);
        menu->addSeparator();
    } else {
        QAction *guestAction = menu->addAction("👤 未登录");
        guestAction->setEnabled(false);
        menu->addAction("登录/注册", this, &MainWindow::showUserLoginDialog);
        menu->addSeparator();
    }
    
    // 添加原有菜单项
    menu->addAction("常规设置", this, &MainWindow::onShowSettings);
    menu->addAction("隐私设置", this, &MainWindow::showPrivacySettingsDialog);
    menu->addAction("学习进度可视化", this, &MainWindow::showProgressChart);
}

// 隐私设置功能实现
void MainWindow::showPrivacySettingsDialog()
{
    QDialog privacyDialog(this);
    privacyDialog.setWindowTitle("隐私设置");
    privacyDialog.setModal(true);
    privacyDialog.resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(&privacyDialog);
    
    // 标题
    QLabel *titleLabel = new QLabel("隐私控制", &privacyDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    
    // 说明文字
    QLabel *descLabel = new QLabel(
        "您可以控制应用程序如何处理您的个人数据。所有数据都存储在本地，不会上传到网络。",
        &privacyDialog);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: gray; margin-bottom: 10px;");
    layout->addWidget(descLabel);
    
    // 隐私设置选项
    QGroupBox *privacyGroup = new QGroupBox("数据使用权限", &privacyDialog);
    QVBoxLayout *privacyLayout = new QVBoxLayout(privacyGroup);
    
    // 数据收集选项
    QCheckBox *dataCollectionCheckBox = new QCheckBox(
        "允许收集使用数据以改进应用", &privacyDialog);
    dataCollectionCheckBox->setChecked(false); // 默认关闭
    privacyLayout->addWidget(dataCollectionCheckBox);
    
    // 云同步选项
    QCheckBox *cloudSyncCheckBox = new QCheckBox(
        "允许云同步（当前仅支持本地存储）", &privacyDialog);
    cloudSyncCheckBox->setChecked(false);
    cloudSyncCheckBox->setEnabled(false); // 暂时禁用，因为只支持本地存储
    privacyLayout->addWidget(cloudSyncCheckBox);
    
    // 数据分析选项
    QCheckBox *analyticsCheckBox = new QCheckBox(
        "允许匿名数据分析", &privacyDialog);
    analyticsCheckBox->setChecked(false);
    privacyLayout->addWidget(analyticsCheckBox);
    
    // 学习统计分享选项
    QCheckBox *shareStatsCheckBox = new QCheckBox(
        "允许分享学习统计数据", &privacyDialog);
    shareStatsCheckBox->setChecked(false);
    privacyLayout->addWidget(shareStatsCheckBox);
    
    layout->addWidget(privacyGroup);
    
    // 当前用户状态显示
    if (!currentUser.isEmpty() && userProfiles.contains(currentUser)) {
        UserData& user = userProfiles[currentUser];
        QGroupBox *currentUserGroup = new QGroupBox("当前用户设置", &privacyDialog);
        QVBoxLayout *currentUserLayout = new QVBoxLayout(currentUserGroup);
        
        QLabel *currentUserLabel = new QLabel(
            QString("用户: %1").arg(user.nickname), &privacyDialog);
        currentUserLayout->addWidget(currentUserLabel);
        
        // 显示当前设置状态
        QString statusText = "当前设置:\n";
        statusText += QString("• 数据收集: %1\n").arg(user.allowDataCollection ? "允许" : "禁止");
        statusText += QString("• 云同步: %1\n").arg(user.allowCloudSync ? "允许" : "禁止");
        statusText += QString("• 数据分析: %1\n").arg(user.allowAnalytics ? "允许" : "禁止");
        statusText += QString("• 统计分享: %1").arg(user.shareLearningStats ? "允许" : "禁止");
        
        QLabel *statusLabel = new QLabel(statusText, &privacyDialog);
        statusLabel->setWordWrap(true);
        currentUserLayout->addWidget(statusLabel);
        
        layout->addWidget(currentUserGroup);
    }
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("保存设置", &privacyDialog);
    QPushButton *resetButton = new QPushButton("重置为默认", &privacyDialog);
    QPushButton *closeButton = new QPushButton("关闭", &privacyDialog);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(closeButton);
    
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(saveButton, &QPushButton::clicked, [&]() {
        bool dataCollection = dataCollectionCheckBox->isChecked();
        bool cloudSync = cloudSyncCheckBox->isChecked();
        bool analytics = analyticsCheckBox->isChecked();
        bool shareStats = shareStatsCheckBox->isChecked();
        
        // 更新当前用户设置
        if (!currentUser.isEmpty() && userProfiles.contains(currentUser)) {
            UserData& user = userProfiles[currentUser];
            user.allowDataCollection = dataCollection;
            user.allowCloudSync = cloudSync;
            user.allowAnalytics = analytics;
            user.shareLearningStats = shareStats;
            
            saveUserProfile(currentUser);
            QMessageBox::information(&privacyDialog, "成功", "隐私设置已保存");
        } else {
            QMessageBox::warning(&privacyDialog, "警告", "请先登录用户账户");
        }
    });
    
    connect(resetButton, &QPushButton::clicked, [&]() {
        dataCollectionCheckBox->setChecked(false);
        cloudSyncCheckBox->setChecked(false);
        analyticsCheckBox->setChecked(false);
        shareStatsCheckBox->setChecked(false);
        QMessageBox::information(&privacyDialog, "提示", "已重置为默认设置");
    });
    
    connect(closeButton, &QPushButton::clicked, &privacyDialog, &QDialog::accept);
    
    privacyDialog.exec();
}

void MainWindow::loadPrivacySettings()
{
    // 从用户配置中加载隐私设置
    if (!currentUser.isEmpty() && userProfiles.contains(currentUser)) {
        UserData& user = userProfiles[currentUser];
        // 设置已经在loadUserProfile中加载
        qDebug() << "Privacy settings loaded for user:" << currentUser;
    }
}

void MainWindow::savePrivacySettings()
{
    // 保存隐私设置到用户配置
    if (!currentUser.isEmpty() && userProfiles.contains(currentUser)) {
        saveUserProfile(currentUser);
        qDebug() << "Privacy settings saved for user:" << currentUser;
    }
}
