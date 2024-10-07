#include "sysinfo.h"
#include <QFile>
#include <QTextStream>

QString SysInfo::getSerialNr()
{
    // This is an alternate solution to StatusModule/PAR_SerialNr because
    // statusmodule serves writing of serial number either. Optional writing
    // done by zera-setup2 and in case of success a file
    // /opt/zera/conf/serialnumber is created
    QString path = "/opt/zera/conf/serialnumber";
    QString serialNo = "";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        serialNo = stream.readLine();
        file.close();
    }
    return serialNo;
}

QString SysInfo::getReleaseNr()
{
    QString path = "/opt/zera/conf/CHANGELOG";
    bool releaseNrFound = false;
    QString releaseNr = "";
    QFile file(path);
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        int start, end;
        QString line;
        do {
            line = stream.readLine();
            if ((start = line.indexOf("'release-")+1) > 0 ||
                (start = line.indexOf("'snapshot-")+1) > 0 ||
                (start = line.indexOf("'no-release-")+1) > 0) {
                end = line.indexOf("'", start);
                if ((releaseNrFound = (end > start)) == true) {
                    releaseNr = line.mid(start, end-start);
                }
            }
        } while (!line.isNull() && !(releaseNrFound));
        file.close();
    }
    return releaseNr;
}
