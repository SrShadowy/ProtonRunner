#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include "configpath.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

Proton protonpath;

void MainWindow::scanSteam(){

    parseConfigVDF();
    QDir defaultSteamApps(steamPath + "/steamapps");
    processManifestsInDir(defaultSteamApps);

    QFile libFile(steamPath + "/steamapps/libraryfolders.vdf");
    if (!libFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Não foi possível abrir libraryfolders.vdf";
        return;
    }

    QString content = libFile.readAll();
    libFile.close();


    static QRegularExpression re("\"path\"\\s*\"(.*?)\"");
    QRegularExpressionMatchIterator matches = re.globalMatch(content);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString libPath = match.captured(1).replace("\\\\", "/");
        QDir steamAppsDir(libPath + "/steamapps");

        if (steamAppsDir.exists()) {
            processManifestsInDir(steamAppsDir);
        } else {
            qWarning() << "Diretório steamapps não encontrado em:" << libPath;
        }
    }
}

void MainWindow::processManifestsInDir(const QDir &steamAppsDir) {
    QStringList manifests = steamAppsDir.entryList(QStringList() << "appmanifest_*.acf", QDir::Files);

    for( auto it = manifests.begin(); it != manifests.end(); it ++)
    {
           parseAppManifest(steamAppsDir.absoluteFilePath(*it));
    }
}


void MainWindow::parseConfigVDF() {
    QFile file(steamPath + "/config/config.vdf");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    QString content = in.readAll();
    static QRegularExpression re("\"(\\d+)\"\\s*\\{[^\\}]*\"CompatToolMapping\"[^\\}]*\"name\"\\s*\"([^\"]+)\"");
    QRegularExpressionMatchIterator i = re.globalMatch(content);

    while (i.hasNext()) {
        auto match = i.next();
        QString appid = match.captured(1);
        QString proton = match.captured(2);
        protonGames[appid] = proton;
    }
}

QString MainWindow::getProtonVersion(const QString appid)
{
    QString configInfoPath = steamPath + "/steamapps/compatdata/" + appid + "/config_info";
    QString protonVer = "None / Native";
    QFile configInfo(configInfoPath);
    if (configInfo.exists() && configInfo.open(QIODevice::ReadOnly)) {
        QTextStream inInfo(&configInfo);
        protonVer = inInfo.readLine().trimmed();
    }
    return protonVer;
}


QString GetProtonPath(const QString Path)
{

    QString protonPath;
    QFile configInfo(Path);
    if (configInfo.exists() && configInfo.open(QIODevice::ReadOnly)) {
        QTextStream inInfo(&configInfo);
        inInfo.readLine();
        protonPath = inInfo.readLine();
        int index = protonPath.indexOf("files/");
        if (index != -1) {
            protonPath = protonPath.left(index);
        } else {
            index = protonPath.indexOf("dist/");
            if (index != -1){
                protonPath = protonPath.left(index);
            }else
            {
                QMessageBox::warning(nullptr, "Path Proton", "I cannot find the Proton path");
            }
        }
    }

    return protonPath;
}

void MainWindow::executeOnProton(const QString &appid, const QString exec)
{
    QString proton =  protonpath.path[appid] + protonpath.proton;
    QString prefix = steamPath + "/steamapps/compatdata/" + appid ;

    if (!QFile::exists(proton)) {
        qWarning("Proton não encontrado: %s", qPrintable(proton));
        return;
    }

    if (!QFile::exists(exec)){
        qWarning("executavel não encontrado: %s", qPrintable(exec));
        return;
    }

    QProcess *process = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("STEAM_COMPAT_DATA_PATH", prefix);
    env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH", steamPath);

    process->setProcessEnvironment(env);

    QStringList arguments;
    arguments << "run" << exec;

    process->start(proton, arguments);

    if (!process->waitForStarted(5000)) {
        qWarning("Falha ao iniciar o processo com Proton.");
        qWarning() << process->errorString();

        delete process;
        return;
    }
    qDebug() << "Processo on " << process->processId();

    connect(process, &QProcess::finished, this, [process](int exitCode) {
        qDebug("Processo Proton finalizado com código %d", exitCode);
        process->deleteLater();
    });

}

void MainWindow::parseAppManifest(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QString appid, name;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("\"appid\""))
            appid = line.section("\"", 3, 3);
        else if (line.startsWith("\"name\""))
            name = line.section("\"", 3, 3);
    }

    if (name.contains("Proton", Qt::CaseInsensitive) || name.contains("Steam Linux Runtime", Qt::CaseInsensitive) || name.contains("Steamworks", Qt::CaseInsensitive))
        return;

    QString protonVer = getProtonVersion(appid);

    protonpath.path[appid] = (GetProtonPath(steamPath + "/steamapps/compatdata/" + appid + "/config_info"));

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, name);
    item->setText(1, appid);
    item->setText(2, protonVer);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    steamPath = ConfigPath::get()->GetSteamPath();
    scanSteam();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_edt_search_textChanged(const QString &arg1)
{
    auto terms = ui->edt_search->text().split(' ', Qt::SkipEmptyParts);
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        bool match = terms.isEmpty();

        if (!match) {
            QStringList itemTexts;
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
                itemTexts << (*it)->text(col).toLower();
            }

            for (const QString &term : std::as_const(terms)) {
                match = std::any_of(itemTexts.begin(), itemTexts.end(),
                                    [&term](const QString &text) { return text.contains(term.toLower()); });

                if (!match) break;
            }
        }

        (*it)->setHidden(!match);
        ++it;
    }

}


void MainWindow::on_btn_get_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Select Executable"),
        QDir::homePath(),
        tr("Executables (*.exe *.bat *.com);;All Files (*)")
        );

    if (!fileName.isEmpty()) {
        fileName = QDir::toNativeSeparators(fileName.trimmed());

        if (QFile::exists(fileName)) {
            ui->line_process->setText(fileName);

            // Feedback visual
            QPalette palette = ui->line_process->palette();
            palette.setColor(QPalette::Text, Qt::darkGreen);
            ui->line_process->setPalette(palette);
        } else {
            QMessageBox::warning(this,
                                 tr("File Not Found"),
                                 tr("The selected file does not exist."));
        }
    }
}


void MainWindow::on_btn_run_clicked()
{
    auto selectedItems = ui->treeWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this,
                             tr("No Selection"),
                             tr("Please select a game first."));
        return;
    }

    QString executablePath = ui->line_process->text();
    if (executablePath.isEmpty()) {
        QMessageBox::warning(this,
                             tr("No Executable"),
                             tr("Please select an executable first."));
        return;
    }

    if (!QFile::exists(executablePath)) {
        QMessageBox::warning(this,
                             tr("File Not Found"),
                             tr("The specified executable does not exist."));
        return;
    }

    QString appId = selectedItems.first()->text(1);
    executeOnProton(appId, executablePath);
}


void MainWindow::on_actionGit_triggered()
{

    const QUrl githubUrl("https://github.com/SrShadowy/ProtonRunner");

    if (!QDesktopServices::openUrl(githubUrl)) {
        QMessageBox::warning(this,
                             tr("Erro"),
                             tr("No Browser?.\n"
                                "Try access by url:\n%1")
                                 .arg(githubUrl.toString()));
    }
}


void MainWindow::on_actionExit_triggered()
{
    exit(0);
}


void MainWindow::on_actionConfig_Path_triggered()
{
    const QUrl githubUrl("https://github.com/SrShadowy/ProtonRunner/issues");
    QDesktopServices::openUrl(githubUrl);
}


void MainWindow::on_actionAbout_triggered()
{
    QString aboutText =
        "<h2>Proton Prefix Runner</h2>"
        "<p><b>Version:</b> 1.0.0</p>"
        "<p>A good tool to run Windows programs within Steam game Proton prefixes</p>"
        "<hr>"
        "<p>Key Features:</p>"
        "<ul>"
        "<li>🔄 Seamless injection into existing Proton prefixes</li>"
        "<li>🎮 Supports Cheat Engine, mod loaders, trainers</li>"
        "<li>🔍 Automatic Steam library scanning</li>"
        "<li>🎨 Modern Qt interface</li>"
        "</ul>"
        "<hr>"
        "<p><b>Made with ❤️ by SrShadowy</b></p>"
        "<p>Special thanks to Valve and Wine developers</p>";

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("About Proton Prefix Runner");
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText(aboutText);

    aboutBox.setStyleSheet(
        "QMessageBox {"
        "   background-color: #2D2D2D;"
        "   color: #FFFFFF;"
        "}"
        "QMessageBox QLabel {"
        "   color: #FFFFFF;"
        "}"
        "QMessageBox QPushButton {"
        "   background-color: #0078D4;"
        "   color: white;"
        "   border-radius: 5px;"
        "   padding: 5px 10px;"
        "   min-width: 80px;"
        "}"
        "QMessageBox QPushButton:hover {"
        "   background-color: #006CBE;"
        "}");


    aboutBox.setIcon(QMessageBox::Information);

    aboutBox.exec();
}

