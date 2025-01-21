#ifndef SCPIDOCSHTMLGENERATOR_H
#define SCPIDOCSHTMLGENERATOR_H

#include <QFileInfo>

class ScpiDocsHtmlGenerator
{
public:
    static void createScpiDocHtmls(QString zenuxRelease, QString docsPath, QString xmlDirPath);
private:
    static void convertXmlToHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath);
};

#endif // SCPIDOCSHTMLGENERATOR_H
