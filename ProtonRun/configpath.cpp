#include "configpath.h"
#include <QDir>

ConfigPath::ConfigPath()
{
    steamPath = QDir::homePath() + "/.steam/steam";
}

QString ConfigPath::GetSteamPath()
{
    return steamPath;
}

void ConfigPath::SetSteamPath(QString path)
{
    steamPath = path;
}
