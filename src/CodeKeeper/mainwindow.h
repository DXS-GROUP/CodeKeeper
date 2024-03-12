#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingswindow.h"
#include <QSettings>
#include <QCloseEvent>
#include "qmarkdowntextedit/qmarkdowntextedit.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSettings *GlobalSettings;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_settingsBtn_clicked();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMarkdownTextEdit *markdownTextEdit;
    SettingsWindow *settingsWindow;
};
#endif // MAINWINDOW_H
