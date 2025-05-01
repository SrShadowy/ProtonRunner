#ifndef CONFIGPATH_H
#define CONFIGPATH_H
#include <QString>
class ConfigPath
{
private:
    QString steamPath;
public:
    ConfigPath();

    static ConfigPath* get(){
        static ConfigPath* MyThis = nullptr;
        if(MyThis == nullptr)
            MyThis = new ConfigPath();

        return MyThis;
    }

    void SetSteamPath(QString path);
    QString GetSteamPath();





};

#endif // CONFIGPATH_H
