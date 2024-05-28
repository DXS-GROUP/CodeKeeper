#include "syncwindow.h"
#include "syncFunc/functional.cpp"

#include <QtWidgets>
#include <QNetworkAccessManager>
#include <QNetworkReply>

SyncWindow::SyncWindow(QWidget *parent) : QMainWindow(parent)
{
    QFile file(":/stylesheet/stylesheet_setting_window.qss");
    file.open(QFile::ReadOnly);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setFixedSize(300, 500);
    this->setStyleSheet(file.readAll());

    globalSettings = new QSettings("CodeKeeper", "CodeKeeper");
    selectedFont = globalSettings->value("font").value<QFont>();
    font_size = globalSettings->value("fontSize").value<QString>();
    dir = globalSettings->value("path").value<QString>();

    mainLayout = new QGridLayout;

    appIcon = new QLabel("Syncing with Git");
    appIcon->setPixmap(QPixmap(":/icon.png"));
    appIcon->setAlignment(Qt::AlignCenter);

    appName = new QLabel("CodeKeeper");
    appName->setStyleSheet("font-size: 30px;");
    appName->setAlignment(Qt::AlignCenter);

    infoLabel = new QLabel("Syncing with Git");
    infoLabel->setAlignment(Qt::AlignCenter);

    syncingProgress = new QProgressBar();
    syncingProgress->setMaximum(100);
    syncingProgress->setValue(0);
    syncingProgress->setFixedSize(200, 20);
    syncingProgress->setAlignment(Qt::AlignCenter);
    syncingProgress->setFormat(" ");

    startSyncing = new QPushButton("Start");
    startSyncing->setFixedSize(100, 25);

    stopSyncing = new QPushButton("Cancel && Quit");
    stopSyncing->setFixedSize(100, 25);

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setAlignment(Qt::AlignHCenter);
    // infoLayout->addWidget(appIcon);
    infoLayout->addWidget(appName);
    infoLayout->addWidget(infoLabel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignHCenter);
    buttonsLayout->addWidget(startSyncing);
    buttonsLayout->addWidget(stopSyncing);
    buttonsLayout->setSpacing(4);

    mainLayout->addLayout(infoLayout, 0, 0, 2, 2, Qt::AlignCenter);
    mainLayout->addWidget(syncingProgress, 2, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addLayout(buttonsLayout, 3, 0, 1, 2, Qt::AlignCenter);

    setFontStyle();

    qDebug() << "🟢 Opening Sync Window";

    centralWidget->setLayout(mainLayout);

    connect(startSyncing, SIGNAL(clicked()), this, SLOT(startSyncingFunc()));
    connect(stopSyncing, SIGNAL(clicked()), this, SLOT(cancelSyncingFunc()));
}

SyncWindow::~SyncWindow() { }
