#ifndef CONFIGPATH_H
#define CONFIGPATH_H
#include "ui_customproton.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include <QMap>

struct Proton
{
    QMap<QString, QString> path;
    QString appID;
    QString proton = "proton"; // could be wine / wine_ge
};


struct ProtonConfig {
    QString name;
    QString path;
    QString prefix;
    QString associatedApp;

    QVariant toVariant() const {
        QVariantMap map;
        map["name"] = name;
        map["path"] = path;
        map["prefix"] = prefix;
        map["associatedApp"] = associatedApp;
        return map;
    }

    static ProtonConfig fromVariant(const QVariant &variant) {
        ProtonConfig config;
        QVariantMap map = variant.toMap();
        config.name = map["name"].toString();
        config.path = map["path"].toString();
        config.prefix = map["prefix"].toString();
        config.associatedApp = map["associatedApp"].toString();
        return config;
    }
};

class ConfigPath
{
private:
    QString m_steamPath;
    Ui::MainWindow *p_mainui = nullptr;
    Ui::customproton *p_customui = nullptr;
public:
    ConfigPath();
    static ConfigPath* get();

    void SyncUi(Ui::MainWindow *ui);
    void SyncUi(Ui::customproton *ui);
    void SyncUi(Ui::MainWindow *MainUI, Ui::customproton *CustomUI);

    void SetSteamPath(QString path);
    QString GetSteamPath();

    void SaveSettings();
    void LoadSettings();

};

#endif // CONFIGPATH_H
