#include <QDir>
#include <QFileInfo>
#include <QWebEngineView>

void MainWindow::onMovingProjectFrom(QListWidgetItem *item, QListWidget *list)
{
    qDebug() << "🟢 Moving project: " << item->text() << " from: " << list->objectName();
}

void MainWindow::onMovingProjectTo(QListWidgetItem *item, QListWidget *list)
{
    qDebug() << "🟢 Moved project: " << item->text() << " to: " << list->objectName();
    QStringList data = item->text().split("\n");
    QString status = list->objectName();
    QString date = getCurrentDateTimeString();

    updateProjectStatus(&status, &date, &data[2]);
}

void loadDocumentations(QDir path, QComboBox &comboBox)
{
    QFileInfoList fileInfoList =
            path.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        if (fileInfo.suffix() == "md") {
            comboBox.addItem(fileInfo.baseName());
        }
    }

    QDir subdir;
    QFileInfoList subdirList =
            path.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    foreach (const QFileInfo &subdirInfo, subdirList) {
        subdir.setPath(subdirInfo.filePath());
        if (subdir.exists()) {
            loadDocumentations(subdir, comboBox);
        }
    }
}

void MainWindow::createProject()
{
    QString date = getCurrentDateTimeString();
    QString status = "NotStartedProjects";
    QString title = "Teamplate";
    QString git = "https://github.com/";
    QString newProjectTeamplate = title + "\n" + git + "\n" + date;

    qDebug() << "🟢 New project: " << newProjectTeamplate;

    notStartedProjects->addItem(newProjectTeamplate);

    saveProjectToDB(&title, &git, &status, &date);
}

void MainWindow::removeProject()
{
    QListWidget *listWidgets[] = { notStartedProjects, startedProjects, finishlineProjects,
                                   finishedProjects };

    for (QListWidget *listWidget : listWidgets) {
        QListWidgetItem *item = listWidget->currentItem();
        if (item) {
            QStringList data = item->text().split("\n");
            QString status = listWidget->objectName();

            listWidget->takeItem(listWidget->row(item));

            removeProjectFromDB(&data[1], &status, &data[2]);

            qDebug() << "🟢 Removed project: " << item->text();
            delete item;
            break;
        }
    }
}

void MainWindow::getTotalProjects(QTabWidget *projectsTab, QListWidget *notStartedProjects,
                                  QListWidget *startedProjects, QListWidget *finishedProjects,
                                  QListWidget *finishlineProjects)
{
    if (projectsTab->currentIndex() == 3) {
        QTimer *timer3 = new QTimer(this);
        connect(timer3, &QTimer::timeout, [=]() {
            int totalProjects = notStartedProjects->count() + finishlineProjects->count()
                    + startedProjects->count() + finishedProjects->count();

            totalProjectsL->setText("Total projects: " + QString::number(totalProjects) + " ");
        });
        timer3->start(500);
    }
}

void MainWindow::openDocumentation(QString fileName)
{
    QString name = fileName + ".md";
    qDebug() << name;
    tabs->setCurrentIndex(1);

    selectFileInQTreeView(notesList, name);
}

void MainWindow::selectFileInQTreeView(QTreeView *treeView, const QString &fileName)
{
}

void MainWindow::createGitBadges(QString git_url, QWebEngineView *page) {
    QString prefix = "https://github.com/";
    QString repo = git_url.replace(prefix, "");

    page->setHtml("<style>"
                    "   .badge {"
                    "       top: 10%;"
                    "       position: relative;"
                    "       height: 20px;"
                    "       margin: 3px 3px;"
                    "       border: 0px;"
                    "       border-radius: 5px;"
                    "   }"
                    "</style>"
                    "<html>"
                    "   <div align='center' style='position: relative; height: 100%;'>"
                    "       <img class='badge' src='https://img.shields.io/github/last-commit/" + repo + "?style=for-the-badge&color=7dc4e4&logoColor=D9E0EE&labelColor=171b22'/><br>"
                    "       <img class='badge' src='https://img.shields.io/github/license/" + repo + "?style=for-the-badge&color=a6e0b8&logoColor=D9E0EE&labelColor=171b22'/><br>"
                    "       <img class='badge' src='https://img.shields.io/github/release/" + repo + "?style=for-the-badge&color=7589d5&logoColor=D9E0EE&labelColor=171b22'/><br>"
                    "       <img class='badge' src='https://img.shields.io/github/issues/" + repo + "?style=for-the-badge&color=dbb6ed&logoColor=D9E0EE&labelColor=171b22'/><br>"
                    "       <img class='badge' src='https://img.shields.io/github/issues-pr/" + repo + "?style=for-the-badge&color=ef9f9c&logoColor=85e185&labelColor=1c1c29'/><br>"
                    "       <img class='badge' src='https://img.shields.io/github/stars/" + repo + "?style=for-the-badge&color=eed49f&logoColor=D9E0EE&labelColor=171b22'/>"
                    "</div>"
                    "</html>");
}

void MainWindow::openProject(QListWidget *listWidget, QListWidgetItem *item)
{
    if (item) {
        QDialog dialog(this);
        dialog.setFixedSize(300, 400);
        dialog.setWindowTitle(tr("Edit project"));
        dialog.setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

        QString data = item->text();
        QStringList splitData = data.split("\n");

        QString PTitle = splitData[0];
        QString PGit = splitData[1];
        QString PCreatedTime = splitData[2];
        QString PStatus = listWidget->objectName();

        QStringList projectData = GetProjectData(&PTitle, &PStatus, &PGit);
        qDebug() << "Open project: " << projectData[0] << " " << projectData[1] << " "
                 << projectData[2] << " " << projectData[3] << " " << projectData[4];

        QGridLayout mainLayout(&dialog);

        QLineEdit *title = new QLineEdit();
        title->setAlignment(Qt::AlignCenter);
        title->setPlaceholderText(" Project name: ");
        title->setStyleSheet("font-size: " + font_size + "pt;");
        title->setFixedHeight(25);
        title->setFont(selectedFont);

        QLineEdit *linkToGit = new QLineEdit();
        linkToGit->setAlignment(Qt::AlignCenter);
        linkToGit->setPlaceholderText(" Link to GIT");
        linkToGit->setStyleSheet("font-size: " + font_size + "pt;");
        linkToGit->setFixedHeight(25);
        linkToGit->setFont(selectedFont);

        QComboBox *documentation = new QComboBox();
        documentation->setFixedSize(140, 25);
        documentation->setFont(selectedFont);

        QLabel *lastMod = new QLabel();
        lastMod->setText("Last mod: ");
        lastMod->setStyleSheet("font-size: " + font_size + "pt;");
        lastMod->setFixedHeight(25);
        lastMod->setAlignment(Qt::AlignCenter);
        lastMod->setFont(selectedFont);

        QWebEngineView *git_stats = new QWebEngineView();
        git_stats->page()->setBackgroundColor(Qt::transparent);
        createGitBadges(projectData[1], git_stats);

        QPushButton *saveDataBtn = new QPushButton();
        saveDataBtn->setText("Save");
        saveDataBtn->setStyleSheet("font-size: " + font_size + "pt;");
        saveDataBtn->setFixedHeight(25);
        saveDataBtn->setIcon(QPixmap(":/save.png"));
        saveDataBtn->setIconSize(QSize(10, 10));
        saveDataBtn->setFont(selectedFont);

        QPushButton *cancelBtn = new QPushButton();
        cancelBtn->setText("Cancel");
        cancelBtn->setStyleSheet("font-size: " + font_size + "pt;");
        cancelBtn->setFixedHeight(25);
        cancelBtn->setIcon(
                QPixmap(":/quit.png")
                        .scaled(font_size.toInt() + 3, font_size.toInt() + 3, Qt::KeepAspectRatio));
        cancelBtn->setIconSize(QSize(10, 10));
        cancelBtn->setFont(selectedFont);

        QPushButton *openButton = new QPushButton();
        openButton->setText("Open");
        openButton->setStyleSheet("font-size: " + font_size + "pt;");
        openButton->setFixedHeight(25);
        openButton->setIcon(
                QPixmap(":/read.png")
                        .scaled(font_size.toInt() + 3, font_size.toInt() + 3, Qt::KeepAspectRatio));
        openButton->setIconSize(QSize(10, 10));
        openButton->setFont(selectedFont);

        title->setText(projectData[0]);
        linkToGit->setText(projectData[1]);
        lastMod->setText("Last mod: " + projectData[4]);

        loadDocumentations(dir, *documentation);
        documentation->setCurrentText(projectData[2]);

        mainLayout.addWidget(title, 0, 0, 1, 2);
        mainLayout.addWidget(linkToGit, 1, 0, 1, 2);
        mainLayout.addWidget(documentation, 2, 0);
        mainLayout.addWidget(openButton, 2, 1);
        mainLayout.addWidget(git_stats, 3, 0, 1, 2);
        mainLayout.addWidget(lastMod, 5, 0, 1, 2);
        mainLayout.addWidget(saveDataBtn, 4, 0);
        mainLayout.addWidget(cancelBtn, 4, 1);

        QObject::connect(saveDataBtn, &QPushButton::clicked, [&]() {
            QString projectTitle = title->text();
            QString projectLink = linkToGit->text();
            QString projectCreatedTime = getCurrentDateTimeString();
            QString projectDocumentation = documentation->currentText();
            
            QString itemText = projectTitle + "\n" + projectLink + "\n" + projectCreatedTime;
            item->setText(itemText);
            qDebug() << itemText;

            updateProjectData(&projectTitle, &projectLink, &projectDocumentation,
                              &projectCreatedTime, &PCreatedTime, &PGit);

            dialog.close();
        });

        QObject::connect(cancelBtn, &QPushButton::clicked, [&]() { dialog.close(); });

        QObject::connect(openButton, &QPushButton::clicked, [&]() {
            dialog.close();
            QString doc = documentation->currentText();
            openDocumentation(doc);
        });

        QObject::connect(linkToGit, &QLineEdit::textChanged, [&]() {
            QString prefix = "https://github.com/";
            QString projectLink = linkToGit->text();
            QString repo = projectLink.replace(prefix, "");

            createGitBadges(repo, git_stats);
        });

        dialog.exec();
    } else {
        qWarning() << "🔴 Error";
    }
}
