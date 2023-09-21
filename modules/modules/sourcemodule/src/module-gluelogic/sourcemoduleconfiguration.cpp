#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>
#include "sourcemoduleconfiguration.h"

SourceModuleConfiguration::SourceModuleConfiguration()
{
}

void SourceModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    // We feed xml contents so have to set xsd props for absolute path of xsd
    // That took a while to understand why we had to use flags xml_schema::flags::dont_validate
    QString configPath = QStringLiteral(ZC_CONFIG_PATH);
    if(!configPath.endsWith("/")) {
        configPath += "/";
    }
    QString xsdLocation = QStringLiteral("file://") + configPath + QStringLiteral("sourcemodule-config.xsd");
    xml_schema::properties props;
    // taken from: https://www.codesynthesis.com/projects/xsd/documentation/cxx/tree/guide/#5.1
    props.no_namespace_schema_location(xsdLocation.toStdString());
    props.schema_location ("http://www.w3.org/XML/1998/namespace", "xml.xsd");

    try {
        std::istringstream stdStream(xmlString.toStdString());
        m_xsdGeneratedConfig = configuration_(stdStream, 0, props);
        m_bConfigured = true;
        emit configXMLDone();
    }
    catch (const xml_schema::exception& e) {
        m_bConfigError = true;
        qWarning("XML: %s\n%s", e.what(), qPrintable(xmlString));
    }
}


QByteArray SourceModuleConfiguration::exportConfiguration()
{
    std::stringstream oStream;
    configuration_(oStream, *m_xsdGeneratedConfig);
    QByteArray xmlExport = QByteArray::fromStdString(oStream.str());
    return xmlExport;
}

configuration* SourceModuleConfiguration::getConfigXMLWrapper()
{
    return m_xsdGeneratedConfig.get();
}

