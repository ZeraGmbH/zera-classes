#ifndef SCPILOCALFILEREAD_H
#define SCPILOCALFILEREAD_H

#include <QString>

namespace SCPIMODULE
{

class ScpiLocalFileRead
{
public:
    static QString readLocalFileContent(const QString &baseDirPath, const QString &fileName);
};

}

#endif // SCPILOCALFILEREAD_H
