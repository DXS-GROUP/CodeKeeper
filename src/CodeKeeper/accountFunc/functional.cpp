#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPixmap>

#include "mainwindow.h"

void AccountWindow::setImageFromUrl(const QString &url, QLabel *label)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error())
        {
            qWarning() << "Error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());

        QImage image = pixmap.toImage();
        QImage roundedImage(image.size(), QImage::Format_ARGB32);
        roundedImage.fill(Qt::transparent);

        QPainter painter(&roundedImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(image));
        painter.setPen(Qt::NoPen);

        int radius = qMin(roundedImage.width(), roundedImage.height());
        painter.drawRoundedRect(0, 0, roundedImage.width(), roundedImage.height(), radius * 0.5, radius * 0.5);

        label->setPixmap(
            QPixmap::fromImage(roundedImage.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        reply->deleteLater();
    });
}

void AccountWindow::get_image_url(const QString &username, QLabel *label)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("https://api.github.com/users/" + git_user);

    QUrlQuery query;
    query.addQueryItem("login", username);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CodeKeeper");
    request.setRawHeader("Authorization", ("Bearer " + git_token).toUtf8());
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error())
        {
            qWarning() << "Error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        qDebug() << obj["avatar_url"].toString();

        setImageFromUrl(obj["avatar_url"].toString(), label);

        reply->deleteLater();
    });
}

void AccountWindow::closeWindowSlot()
{
    this->close();
}

void AccountWindow::getSettingsData()
{
    selectedFont = globalSettings->value("font").value<QFont>();
    font_size = globalSettings->value("fontSize").value<QString>();
    theme = globalSettings->value("theme").value<QString>();
    path = globalSettings->value("path").value<QString>();

    git_repo = globalSettings->value("git_repo").value<QString>();
    git_user = globalSettings->value("git_user").value<QString>();
    git_token = globalSettings->value("git_token").value<QString>();

    isAutoSyncB = globalSettings->value("isAutoSync").value<bool>();
}

void AccountWindow::setFontStyle()
{
    userName->setFont(selectedFont);

    int header_size = font_size.toInt() + 5;
    qDebug() << header_size;

    chartValuesDisplay->setFont(selectedFont);
    chartValuesDisplay->setStyleSheet("font-size: " + font_size + "pt;");

    tasksTitle->setFont(selectedFont);
    tasksTitle->setStyleSheet("font-size: " + QString(header_size) + "px;");

    projectTitle->setFont(selectedFont);
    projectTitle->setStyleSheet("font-size: " + QString(header_size) + "px;");

    userName->setStyleSheet("font-size: 36pt;");

    profilePicture->setFont(selectedFont);
    profilePicture->setStyleSheet("font-size: " + font_size + "pt;");

    openRepo->setFont(selectedFont);
    openRepo->setStyleSheet("font-size: " + font_size + "pt;");

    codeKeeperStats->setFont(selectedFont);
    codeKeeperStats->setStyleSheet("font-size: " + font_size + "pt;");

    profileInfo->setFont(selectedFont);
    profileInfo->setStyleSheet("font-size: " + font_size + "pt;");

    tasksStatsProgress->setFont(selectedFont);
    tasksStatsProgress->setStyleSheet("font-size: " + font_size + "pt;");

    closeWindow->setStyleSheet("QPushButton {"
                               "    border-color: rgba(0, 0, 0, 0);"
                               "    background-color: rgba(0, 0, 0, 0);"
                               "    background-image: url(':/red.png');"
                               "    background-repeat: no-repeat;"
                               "}"
                               "QPushButton:hover {"
                               "    border-color: rgba(0, 0, 0, 0);"
                               "    background-image: url(':/redHovered.png');"
                               "    background-repeat: no-repeat;"
                               "    background-color: rgba(0, 0, 0, 0);"
                               "}");
}

int AccountWindow::getStarsCount(const QString &username, const QString &token)
{
    QNetworkAccessManager manager;
    QUrl url("https://api.github.com/users/" + username + "/repos");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CodeKeeper");
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int totalStars = 0;
    if (!reply->error())
    {
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        QJsonArray repos = json.array();

        for (const QJsonValue &repo : repos)
        {
            QJsonObject repoObject = repo.toObject();
            totalStars += repoObject["stargazers_count"].toInt();
        }
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
    return totalStars;
}

void AccountWindow::setUserData(const QString &username, QLabel *label)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("https://api.github.com/users/" + git_user);

    QUrlQuery query;
    query.addQueryItem("login", username);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CodeKeeper");
    request.setRawHeader("Authorization", ("Bearer " + git_token).toUtf8());
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error())
        {
            qWarning() << "Error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        // do something with the user data
        qDebug() << "Name:" << obj["name"].toString();
        qDebug() << "Location:" << obj["location"].toString();
        qDebug() << "Avatar_url:" << obj["avatar_url"].toString();
        qDebug() << "Html_url:" << obj["html_url"].toString();
        qDebug() << "Created_at:" << obj["created_at"].toString();
        qDebug() << "Updated_at:" << obj["updated_at"].toString();
        qDebug() << "Following:" << obj["following"].toInt();
        qDebug() << "Followers:" << obj["followers"].toInt();
        qDebug() << "Public_repos:" << obj["public_repos"].toInt();
        qDebug() << "Bio:" << obj["bio"].toString();
        qDebug() << "Company:" << obj["company"].toString();
        qDebug() << "Login:" << obj["login"].toString();

        label->setText("\n\n" + obj["bio"].toString() + tr("\nPublic repos: ") +
                       QString::number(obj["public_repos"].toInt()) + tr("\n\nFollowing: ") +
                       QString::number(obj["following"].toInt()) + tr("\n\nFollowers: ") +
                       QString::number(obj["followers"].toInt()) + tr("\n\nStars: ") +
                       QString::number(getStarsCount(git_user, git_token)) + "\n");

        reply->deleteLater();
    });
}

void AccountWindow::onOpenRepoClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/" + git_user + "/"));
}

void AccountWindow::setTasksProgress()
{
    MainWindow *mainWindow = qobject_cast<MainWindow *>(this->parent());
    QString stats = mainWindow->getKeeperStats();

    int incompleteTasksCount = mainWindow->incompleteTasks->count();
    int completeTasksCount = mainWindow->completeTasks->count();
    int inprocessTasksCount = mainWindow->inprocessTasks->count();

    int totalTasks = incompleteTasksCount + completeTasksCount + inprocessTasksCount;

    double percentage = static_cast<double>(completeTasksCount) / static_cast<double>(totalTasks) * 100.0;

    double complete_percentage = static_cast<double>(completeTasksCount) / static_cast<double>(totalTasks) * 100.0;
    double started_percentage = static_cast<double>(inprocessTasksCount) / static_cast<double>(totalTasks) * 100.0;
    double ns_percentage = static_cast<double>(incompleteTasksCount) / static_cast<double>(totalTasks) * 100.0;

    qDebug() << completeTasksCount << "/" << totalTasks;

    tasksStatsProgress->setValue(percentage);
    tasksStatsProgress->setMaxValue(100);

    tasksChartValuesDisplay->addValue(tr("Completed"), complete_percentage, QColor("#78b3ba"), selectedFont);
    tasksChartValuesDisplay->addValue(tr("Started"), started_percentage, QColor("#b1e032"), selectedFont);
    tasksChartValuesDisplay->addValue(tr("Not Started"), ns_percentage, QColor("#c75d5e"), selectedFont);

    if (percentage < 101)
    {
        tasksStatsProgress->setProgressColor(QColor("#78b3ba"));
    }

    if (percentage < 51)
    {
        tasksStatsProgress->setProgressColor(QColor("#e09132"));
    }

    if (percentage < 26)
    {
        tasksStatsProgress->setProgressColor(QColor("#c75d5e"));
    }
}

void AccountWindow::setProjectsStats()
{
    MainWindow *mainWindow = qobject_cast<MainWindow *>(this->parent());

    int notStartedProjectsCount = mainWindow->notStartedProjects->count();
    int startedProjectsCount = mainWindow->startedProjects->count();
    int finishlineProjectsCount = mainWindow->finishedProjects->count();
    int finishedProjectsCount = mainWindow->finishlineProjects->count();

    int totalProjects =
        notStartedProjectsCount + startedProjectsCount + finishlineProjectsCount + finishedProjectsCount;

    int ns_p = static_cast<double>(notStartedProjectsCount) / static_cast<double>(totalProjects) * 100.0;
    int s_p = static_cast<double>(startedProjectsCount) / static_cast<double>(totalProjects) * 100.0;
    int fl_p = static_cast<double>(finishlineProjectsCount) / static_cast<double>(totalProjects) * 100.0;
    int f_p = static_cast<double>(finishedProjectsCount) / static_cast<double>(totalProjects) * 100.0;

    projectsChart->setMaximumValue(100);

    projectsChart->addValue(ns_p, QColor("#c75d5e"));
    projectsChart->addValue(s_p, QColor("#e09132"));
    projectsChart->addValue(fl_p, QColor("#b1e032"));
    projectsChart->addValue(f_p, QColor("#78b3ba"));

    chartValuesDisplay->addValue(tr("Not started"), ns_p, QColor("#c75d5e"), selectedFont);
    chartValuesDisplay->addValue(tr("In Dev"), s_p, QColor("#e09132"), selectedFont);
    chartValuesDisplay->addValue(tr("On Review"), fl_p, QColor("#b1e032"), selectedFont);
    chartValuesDisplay->addValue(tr("Finished"), f_p, QColor("#78b3ba"), selectedFont);
}
