#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QTextStream>
#include <QRegularExpression>
#include <QTreeWidgetItem>
#include <QMap>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void scanSteam();
    void parseConfigVDF();
    void parseAppManifest(const QString &path);
    QString getProtonVersion(const QString appid);
    void executeOnProton(const QString &appid, const QString exec);
    void processManifestsInDir(const QDir &steamAppsDir);



private slots:
    void on_edt_search_textChanged(const QString &arg1);

    void on_btn_get_clicked();

    void on_btn_run_clicked();

    void on_actionGit_triggered();

    void on_actionExit_triggered();

    void on_actionConfig_Path_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QString steamPath;
    QMap<QString, QString> protonGames;
};

struct Proton
{
    QMap<QString, QString> path;
    QString appID;
    const QString proton = "proton";
};

#endif // MAINWINDOW_H
