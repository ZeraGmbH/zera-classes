#ifndef SCPIDOCGENERATOR_H
#define SCPIDOCGENERATOR_H

#include <QString>
#include <QFileInfo>

class ScpiDocGenerator
{
public:
    static void createDocs(QString zenuxRelease, QString htmlOutPath);
private:
    static void createHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath);
};

#endif // SCPIDOCGENERATOR_H
