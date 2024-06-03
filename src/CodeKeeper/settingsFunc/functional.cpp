#include "mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>

void SettingsWindow::QuitW()
{
    this->close();
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    MainWindow *mainWindow = static_cast<MainWindow *>(parent());
    mainWindow->setGraphicsEffect(nullptr);
    QMainWindow::closeEvent(event);
    saveData();
}

void SettingsWindow::checkUpdates() { }

void SettingsWindow::checkRepo()
{
    QString repo = "https://github.com/" + gitUser->text() + "/" + gitRepo->text();

    QNetworkAccessManager nam;
    QNetworkReply *reply = nam.get(QNetworkRequest(QUrl(repo)));
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        // qDebug() << repo;
        // qDebug() << "🟢 Repository is available";
        repoAvailability->setText("Repository is available");
    } else {
        qDebug() << repo;
        qWarning() << "🔴 The repository isn't available";
        repoAvailability->setText("The repository isn't available");
    }

    reply->deleteLater();
}

void SettingsWindow::saveData()
{
    qDebug() << "🟢 Saving data";

    globalSettings = new QSettings("CodeKeeper", "CodeKeeper");
    QMessageBox *messageBox = new QMessageBox();

    QFont selectedFont = fontSelector->currentFont();
    globalSettings->setValue("font", selectedFont);

    int font_size = fontSize->value();
    globalSettings->setValue("fontSize", font_size);

    QString theme = themeSelector->currentText();
    globalSettings->setValue("theme", theme);

    QString dir = pathToFolder->text();
    globalSettings->setValue("path", dir);

    QString git_tokenS = gitToken->text();
    globalSettings->setValue("git_token", git_tokenS);

    QString git_userS = gitUser->text();
    globalSettings->setValue("git_user", git_userS);

    QString git_repoS = gitRepo->text();
    globalSettings->setValue("git_repo", git_repoS);

    isDateB = isDate->isChecked();
    globalSettings->setValue("isDate", isDateB);

    isHostB = isHost->isChecked();
    globalSettings->setValue("isHost", isHostB);

    isTimeB = isTime->isChecked();
    globalSettings->setValue("isTime", isTimeB);

    isAutoSyncB = isSync->isChecked();
    globalSettings->setValue("isAutoSync", isAutoSyncB);

    isCustomTitlebar = customTitleBar->isChecked();
    globalSettings->setValue("isCustomTitlebar", isCustomTitlebar);

    isCreated = CisCreated->isChecked();
    globalSettings->setValue("isCreated", isCreated);
    isReleaseDate = CisReleaseDate->isChecked();
    globalSettings->setValue("isReleaseDate", isReleaseDate);
    isLastCommit = CisLastCommit->isChecked();
    globalSettings->setValue("isLastCommit", isLastCommit);
    isPullReq = CisPullReq->isChecked();
    globalSettings->setValue("isPullReq", isPullReq);
    isLicense = CisLicense->isChecked();
    globalSettings->setValue("isLicense", isLicense);
    isRelease = CisRelease->isChecked();
    globalSettings->setValue("isRelease", isRelease);
    isIssue = CisIssue->isChecked();
    globalSettings->setValue("isIssue", isIssue);
    isDownloads = CisDownloads->isChecked();
    globalSettings->setValue("isDownloads", isDownloads);
    isCommit = CisCommit->isChecked();
    globalSettings->setValue("isCommit", isCommit);
    isLang = CisLang->isChecked();
    globalSettings->setValue("isLang", isLang);
    isStars = CisStars->isChecked();
    globalSettings->setValue("isStars", isStars);
    isForks = CisForks->isChecked();
    globalSettings->setValue("isForks", isForks);
    isRepoSize = CisRepoSize->isChecked();
    globalSettings->setValue("isRepoSize", isRepoSize);

    messageBox->setIcon(QMessageBox::Information);
    messageBox->setWindowTitle("CodeKeeper - Settings");
    messageBox->setText("To apply the settings, restart the application.");

    messageBox->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // messageBox->exec();

    MainWindow *mainWindow = static_cast<MainWindow *>(parent());
    setFontPr2(&selectedFont, &font_size);
    mainWindow->setFontPr1(&selectedFont, &font_size);
    mainWindow->getSettingsData();

    // mainWindow->createCustomTitlebar();
    mainWindow->setConnectionStatus();
    mainWindow->createConnection(dir);
    mainWindow->loadNotes();
    mainWindow->loadTasks();
    mainWindow->loadProjects();

    SyncWindow *syncWindow = static_cast<SyncWindow *>(parent());
    // syncWindow->setFontStyle();

    AccountWindow *accountWindow = static_cast<AccountWindow *>(parent());
    // accountWindow->setFontStyle();
}

void SettingsWindow::fopenFolder()
{
    QString str = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!str.isEmpty()) {
        qDebug() << str;
        globalSettings->setValue("path", str);

        pathToFolder->setText(str);

        QMessageBox *messageBox = new QMessageBox();

        messageBox->setIcon(QMessageBox::Information);
        messageBox->setWindowTitle("CodeKeeper - Settings");
        messageBox->setText("To apply the settings, restart the application.");

        messageBox->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        messageBox->exec();
        MainWindow *mainWindow = static_cast<MainWindow *>(parent());
        mainWindow->loadNotes();
    }
}

void SettingsWindow::getAppVersion()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("https://api.github.com/repos/Nighty3098/CodeKeeper/releases/latest");

    QUrlQuery query;
    query.addQueryItem("login", git_user);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CodeKeeper");
    request.setRawHeader("Authorization", ("Bearer " + git_token).toUtf8());
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QNetworkReply *reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    QString version = obj["name"].toString();

    reply->deleteLater();

    versionInfo->setText(version);
}

void SettingsWindow::setFontPr2(QFont *selectedFont, int *font_size_int)
{
    qDebug() << "🟢 Applying preferences";

    QString font_size = QString::number(*font_size_int);

    // set font
    tabs->setFont(*selectedFont);
    tabs->setStyleSheet("QTabBar::tab { font-size: " + font_size
                        + "pt;} QTabBar::tab:selected {font-size: " + font_size + "pt;}");

    saveBtn->setFont(*selectedFont);
    saveBtn->setStyleSheet("font-size: " + font_size + "pt;");

    quitBtn->setFont(*selectedFont);
    quitBtn->setStyleSheet("font-size: " + font_size + "pt;");

    appName->setFont(*selectedFont);

    urlToRepo->setFont(*selectedFont);
    urlToRepo->setStyleSheet("font-size: " + font_size + "pt;");

    versionInfo->setFont(*selectedFont);
    versionInfo->setStyleSheet("font-size: " + font_size + "pt;");

    checkUpdatesBtn->setFont(*selectedFont);
    checkUpdatesBtn->setStyleSheet("font-size: " + font_size + "pt;");

    fontSize->setFont(*selectedFont);
    fontSize->setStyleSheet("font-size: " + font_size + "pt;");

    fontSelector->setFont(*selectedFont);
    fontSelector->setStyleSheet("font-size: " + font_size + "pt;");

    themeSelector->setFont(*selectedFont);
    themeSelector->setStyleSheet("font-size: " + font_size + "pt;");

    gitLabel->setFont(*selectedFont);

    gitLabel2->setFont(*selectedFont);

    gitToken->setFont(*selectedFont);
    gitToken->setStyleSheet("font-size: " + font_size + "pt;");

    gitUser->setFont(*selectedFont);
    gitUser->setStyleSheet("font-size: " + font_size + "pt;");

    gitRepo->setFont(*selectedFont);
    gitRepo->setStyleSheet("font-size: " + font_size + "pt;");

    autoSyncAfterStart->setFont(*selectedFont);
    autoSyncAfterStart->setStyleSheet("font-size: " + font_size + "pt;");

    isDate->setFont(*selectedFont);
    isDate->setStyleSheet("font-size: " + font_size + "pt;");

    isTime->setFont(*selectedFont);
    isTime->setStyleSheet("font-size: " + font_size + "pt;");

    isHost->setFont(*selectedFont);
    isHost->setStyleSheet("font-size: " + font_size + "pt;");

    mainTitle->setFont(*selectedFont);

    fontLabel->setFont(*selectedFont);
    fontSizeLabel->setFont(*selectedFont);

    fontSizeLabel->setFont(*selectedFont);
    fontSizeLabel->setStyleSheet("font-size: " + font_size + "pt;");

    themeLabel->setFont(*selectedFont);
    themeLabel->setStyleSheet("font-size: " + font_size + "pt;");

    storageLabel->setFont(*selectedFont);

    pathToFolder->setFont(*selectedFont);
    pathToFolder->setStyleSheet("font-size: " + font_size + "pt;");

    openFolder->setFont(*selectedFont);
    openFolder->setStyleSheet("font-size: " + font_size + "pt;");

    customTitleBar->setFont(*selectedFont);
    customTitleBar->setStyleSheet("font-size: " + font_size + "pt;");

    repoAvailability->setFont(*selectedFont);
    repoAvailability->setStyleSheet("font-size: " + font_size + "pt;");

    projectsContentLabel->setFont(*selectedFont);

    CisCreated->setFont(*selectedFont);
    CisReleaseDate->setFont(*selectedFont);
    CisLastCommit->setFont(*selectedFont);
    CisPullReq->setFont(*selectedFont);
    CisLicense->setFont(*selectedFont);
    CisRelease->setFont(*selectedFont);
    CisIssue->setFont(*selectedFont);
    CisDownloads->setFont(*selectedFont);
    CisCommit->setFont(*selectedFont);
    CisLang->setFont(*selectedFont);
    CisStars->setFont(*selectedFont);
    CisForks->setFont(*selectedFont);

    CisCreated->setStyleSheet("font-size: " + font_size + "pt;");
    CisReleaseDate->setStyleSheet("font-size: " + font_size + "pt;");
    CisLastCommit->setStyleSheet("font-size: " + font_size + "pt;");
    CisPullReq->setStyleSheet("font-size: " + font_size + "pt;");
    CisLicense->setStyleSheet("font-size: " + font_size + "pt;");
    CisRelease->setStyleSheet("font-size: " + font_size + "pt;");
    CisIssue->setStyleSheet("font-size: " + font_size + "pt;");
    CisDownloads->setStyleSheet("font-size: " + font_size + "pt;");
    CisCommit->setStyleSheet("font-size: " + font_size + "pt;");
    CisLang->setStyleSheet("font-size: " + font_size + "pt;");
    CisStars->setStyleSheet("font-size: " + font_size + "pt;");
    CisForks->setStyleSheet("font-size: " + font_size + "pt;");

    // set Data
    fontSelector->setCurrentFont(*selectedFont);
    fontSize->setValue(font_size.toInt());
    themeSelector->setCurrentText(theme);
}
