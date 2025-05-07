#include "configpath.h"
#include <QDir>
#include <QDebug>

ConfigPath::ConfigPath()
{
    m_steamPath = QDir::homePath() + "/.steam/steam";
}

QString ConfigPath::GetSteamPath()
{
    return m_steamPath;
}

void ConfigPath::SetSteamPath(QString path)
{
    m_steamPath = path;
}


void ConfigPath::SaveSettings()
{
    QSettings settings;

    if (p_mainui && !p_mainui->line_process->text().isEmpty()) {
        settings.setValue("Interface/ExecPath", p_mainui->line_process->text());
    }

    if (p_customui) {

        QVariantList protonConfigs = settings.value("protonConfigs").toList();

        bool allFieldsValid = !p_customui->edtAppname->text().isEmpty() &&
                              !p_customui->edtProtonVersion->text().isEmpty() &&
                              !p_customui->edtProton->text().isEmpty() &&
                              !p_customui->edtPfx->text().isEmpty();

        if (allFieldsValid) {
            ProtonConfig newConfig;
            newConfig.associatedApp = p_customui->edtAppname->text();
            newConfig.name = p_customui->edtProtonVersion->text();
            newConfig.path = p_customui->edtProton->text();
            newConfig.prefix = p_customui->edtPfx->text();

            bool configExists = false;
            QVariant newVariant = newConfig.toVariant();

            for(auto it = protonConfigs.begin(); it != protonConfigs.end(); it++)
            {
                if (*it == newVariant) {
                    configExists = true;
                    break;
                }
            }

            if (!configExists) {
                protonConfigs.append(newVariant);
                settings.setValue("protonConfigs", protonConfigs);
            }
        }
    }

    settings.sync();
}


void ConfigPath::LoadSettings()
{
    QSettings settings;
    QString Text = settings.value("Interface/ExecPath").toString();
    if(p_mainui)
    p_mainui->line_process->setText(Text);
}

void ConfigPath::SyncUi(Ui::MainWindow *ui)
{
    p_mainui = ui;
}

void ConfigPath::SyncUi(Ui::customproton *ui)
{
    p_customui = ui;
}

void ConfigPath::SyncUi(Ui::MainWindow *MainUI, Ui::customproton *CustomUI)
{
    p_mainui = MainUI;
    p_customui = CustomUI;
}

ConfigPath* ConfigPath::get(){
    static ConfigPath* MyThis = nullptr;
    if(MyThis == nullptr)
        MyThis = new ConfigPath();

    return MyThis;
}
