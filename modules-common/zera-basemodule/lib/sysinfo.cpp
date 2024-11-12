#include "sysinfo.h"
#include <QFile>
#include <QTextStream>

QString SysInfo::getReleaseNr()
{
    QString path = "/opt/zera/conf/CHANGELOG";
    QString releaseNr = "";
    QFile file(path);
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        int start, end;
        QString line;
        bool releaseNrFound = false;
        do {
            line = stream.readLine();
            if ((start = line.indexOf("'release-")+1) > 0 ||
                (start = line.indexOf("'snapshot-")+1) > 0 ||
                (start = line.indexOf("'no-release-")+1) > 0) {
                end = line.indexOf("'", start);
                releaseNrFound = end > start;
                if (releaseNrFound)
                    releaseNr = line.mid(start, end-start);
            }
        } while (!line.isNull() && !(releaseNrFound));
        file.close();
    }
    return releaseNr;
}
