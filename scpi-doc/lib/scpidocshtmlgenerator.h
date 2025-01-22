#ifndef SCPIDOCSHTMLGENERATOR_H
#define SCPIDOCSHTMLGENERATOR_H

#include <QFileInfo>

class ScpiDocsHtmlGenerator
{
public:
    static void createScpiDocHtmls(QString modmanConfigFile, QString zenuxRelease, QString htmlDirPath, QString xmlDirPath, QString sessionMapPath);
private:
    static void convertXmlToHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath);
};

#endif // SCPIDOCSHTMLGENERATOR_H
