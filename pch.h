#ifndef PCH_H
#define PCH_H

// Qt核心头文件
#include <QMainWindow>
#include <QWidget>
#include <QApplication>
#include <QObject>
#include <QString>
#include <QVariant>

// Qt GUI相关
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QScrollBar>

// Qt布局和容器
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QStackedWidget>
#include <QScrollArea>

// Qt事件和输入
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>

// Qt对话框和消息
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QDialog>
#include <QDialogButtonBox>

// Qt时间和定时器
#include <QTimer>
#include <QDateTime>

// Qt网络
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

// Qt文件和目录
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QStandardPaths>

// Qt数据结构
#include <QMap>
#include <QVector>
#include <QList>
#include <QStringList>
#include <QByteArray>

// Qt JSON处理
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Qt图形和动画
#include <QPixmap>
#include <QIcon>
#include <QPalette>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>

// Qt实用工具
#include <QSettings>
#include <QProcess>
#include <QClipboard>
#include <QMimeData>
#include <QDesktopServices>
#include <QUrlQuery>

// Qt图表(如果使用)
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>

// 标准库
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <functional>
#include <cmath>
#include <random>

// Windows特定
#ifdef _WIN32
#include <windows.h>
#endif

#endif // PCH_H