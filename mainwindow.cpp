#include "mainwindow.h"
#include <QApplication>
#include <QStyleHints>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentIndex(0)
    , countdown(0)
    , isDarkTheme(false)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , topLayout(nullptr)
    , middleLayout(nullptr)
    , bottomLayout(nullptr)
    , wordLabel(nullptr)
    , wordInput(nullptr)
    , addButton(nullptr)
    , removeButton(nullptr)
    , clearButton(nullptr)
    , loadButton(nullptr)
    , saveButton(nullptr)
    , startButton(nullptr)
    , themeButton(nullptr)
    , wordList(nullptr)
    , countdownLabel(nullptr)
    , testWidget(nullptr)
    , timer(nullptr)
{
    setupUI();
    
    // 确保所有控件都已创建后再连接信号和槽
    if (addButton) connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddWord);
    if (removeButton) connect(removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveWord);
    if (clearButton) connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearWords);
    if (loadButton) connect(loadButton, &QPushButton::clicked, this, &MainWindow::onAddWordsFromFile);
    if (saveButton) connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveWordsToFile);
    if (startButton) connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartTest);
    if (themeButton) connect(themeButton, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    
    // 监听系统主题变化
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, &MainWindow::setupUI);
    
    // 定时器初始化
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onNextWord);
    
    // 加载默认词库
    loadWordsFromFile("wordlist.txt");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 如果是首次创建界面
    if (!centralWidget) {
        // 创建中央窗口部件
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // 创建布局
        mainLayout = new QVBoxLayout(centralWidget);
        topLayout = new QHBoxLayout();
        middleLayout = new QHBoxLayout();
        bottomLayout = new QHBoxLayout();
        
        // 创建主界面控件
        wordLabel = new QLabel("请输入单词:", this);
        wordInput = new QLineEdit(this);
        addButton = new QPushButton("添加单词", this);
        removeButton = new QPushButton("删除选中", this);
        clearButton = new QPushButton("清空列表", this);
        loadButton = new QPushButton("从文件加载", this);
        saveButton = new QPushButton("保存到文件", this);
        startButton = new QPushButton("开始听写测试", this);
        themeButton = new QPushButton("切换主题", this);  // 主题切换按钮
        wordList = new QListWidget(this);
        
        // 创建测试界面控件
        testWidget = new QWidget(this);
        QVBoxLayout *testLayout = new QVBoxLayout(testWidget);
        countdownLabel = new QLabel("5", testWidget);
        countdownLabel->setAlignment(Qt::AlignCenter);
        QFont font = countdownLabel->font();
        font.setPointSize(48);
        font.setBold(true);
        countdownLabel->setFont(font);
        
        // 创建重复朗读按钮
        repeatButton = new QPushButton("再读一遍", testWidget);
        repeatButton->setMaximumWidth(100);
        repeatButton->setMaximumHeight(30);
        connect(repeatButton, &QPushButton::clicked, this, &MainWindow::onRepeatWord);
        
        testLayout->addWidget(countdownLabel);
        testLayout->addWidget(repeatButton, 0, Qt::AlignCenter);
        testWidget->hide(); // 默认隐藏测试界面
        
        // 设置主界面布局
        topLayout->addWidget(wordLabel);
        topLayout->addWidget(wordInput);
        topLayout->addWidget(addButton);
        
        middleLayout->addWidget(wordList);
        
        bottomLayout->addWidget(removeButton);
        bottomLayout->addWidget(clearButton);
        bottomLayout->addWidget(loadButton);
        bottomLayout->addWidget(saveButton);
        bottomLayout->addWidget(themeButton);  // 添加主题切换按钮
        bottomLayout->addWidget(startButton);
        
        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(testWidget);
        mainLayout->addLayout(middleLayout);
        mainLayout->addLayout(bottomLayout);
        
        // 设置窗口属性
        setWindowTitle("英语听写练习");
        resize(500, 400);
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
}

void MainWindow::toggleTheme()
{
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
        if (themeButton) {
            themeButton->setText("浅色主题");  // 更新按钮文本
        }
    } else {
        // 应用浅色主题
        qApp->setPalette(qApp->style()->standardPalette());
        if (themeButton) {
            themeButton->setText("深色主题");  // 更新按钮文本
        }
    }
}

void MainWindow::showTestInterface()
{
    // 安全检查，确保控件存在
    if (wordLabel) wordLabel->hide();
    if (wordInput) wordInput->hide();
    if (addButton) addButton->hide();
    if (removeButton) removeButton->hide();
    if (clearButton) clearButton->hide();
    if (loadButton) loadButton->hide();
    if (saveButton) saveButton->hide();
    if (themeButton) themeButton->hide();
    if (startButton) startButton->hide();
    if (wordList) wordList->hide();
    
    // 显示测试界面
    if (testWidget) testWidget->show();
}

void MainWindow::showMainInterface()
{
    // 隐藏测试界面
    if (testWidget) testWidget->hide();
    
    // 显示主界面元素
    if (wordLabel) wordLabel->show();
    if (wordInput) wordInput->show();
    if (addButton) addButton->show();
    if (removeButton) removeButton->show();
    if (clearButton) clearButton->show();
    if (loadButton) loadButton->show();
    if (saveButton) saveButton->show();
    if (themeButton) themeButton->show();
    if (startButton) startButton->show();
    if (wordList) wordList->show();
}

void MainWindow::onAddWord()
{
    if (!wordInput) return;
    
    QString word = wordInput->text().trimmed();
    if (!word.isEmpty()) {
        words.push_back(word.toStdString());
        if (wordList) wordList->addItem(word);
        wordInput->clear();
    }
}

void MainWindow::onRemoveWord()
{
    if (!wordList) return;
    
    QListWidgetItem *item = wordList->currentItem();
    if (item) {
        int row = wordList->row(item);
        // 从内存中删除单词
        words.erase(words.begin() + row);
        // 从界面列表中删除单词
        delete wordList->takeItem(row);
        
        // 同时从词库文件中删除单词
        saveWordsToFile("wordlist.txt");
    } else {
        QMessageBox::information(this, "提示", "请选择要删除的单词");
    }
}

void MainWindow::onClearWords()
{
    words.clear();
    if (wordList) wordList->clear();
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
}

void MainWindow::onStartTest()
{
    if (words.empty()) {
        QMessageBox::warning(this, "警告", "词库为空，请先添加单词");
        return;
    }
    
    currentIndex = 0;
    startButton->setEnabled(false);
    
    // 切换到测试界面
    showTestInterface();
    
    // 设置初始倒计时为1秒，用于第一个单词的延迟
    countdown = 1;
    if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    if (timer) timer->start(1000);
}

void MainWindow::onNextWord()
{
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
            // 设置下一词的朗读倒计时
            countdown = 5;
            if (countdownLabel) countdownLabel->setText(QString::number(countdown));
        } else {
            if (timer) timer->stop();
            startButton->setEnabled(true);
            // 测试结束，先显示提示框，然后在提示框关闭后返回主界面
            QMessageBox::information(this, "提示", "听写测试结束");
            showMainInterface();
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
        
        // 重置倒计时
        countdown = 5;
        if (countdownLabel) countdownLabel->setText(QString::number(countdown));
    }
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
            if (wordList) wordList->addItem(line);
        }
    }
    
    file.close();
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

void MainWindow::speakWord(const std::string &word)
{
    // 使用QProcess以隐藏方式执行VBS脚本
    QString vbsCode = QString("CreateObject(\"SAPI.SpVoice\").Speak \"%1\"").arg(QString::fromStdString(word));
    
    // 创建临时VBS文件（使用Windows路径分隔符）
    QString tempPath = QDir::tempPath();
    QString vbsFile = tempPath + "\\temp_speak.vbs";  // 使用双反斜杠
    
    QFile file(vbsFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << vbsCode;
        file.close();
        
        // 确保文件创建成功后再执行
        if (QFile::exists(vbsFile)) {
            // 使用完整路径执行wscript
            QString program = "wscript.exe";
            QStringList arguments;
            arguments << "//nologo" << vbsFile;
            
            // 使用QProcess执行，不使用startDetached
            QProcess process;
            process.start(program, arguments);
            process.waitForFinished(-1); // 等待执行完成
            
            // 立即删除临时文件
            QFile::remove(vbsFile);
        }
    }
}
