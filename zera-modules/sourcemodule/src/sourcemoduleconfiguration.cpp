#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "sourcemoduleconfiguration.h"
#include "socket.h"

namespace SOURCEMODULE
{

cSourceModuleConfiguration::cSourceModuleConfiguration()
{
}


void cSourceModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    std::istringstream stdStream(xmlString.toStdString());
    try {
        m_xsdGeneratedConfig = configuration_(stdStream, xml_schema::flags::dont_validate);
        m_bConfigured = true;
        emit configXMLDone();
    }
    catch (const xml_schema::exception& e) {
        m_bConfigError = true;
        qWarning("XML: %s\n%s", e.what(), qPrintable(xmlString));
    }
}


QByteArray cSourceModuleConfiguration::exportConfiguration()
{
    std::stringstream oStream;
    configuration_(oStream, *m_xsdGeneratedConfig);
    return QByteArray::fromStdString(oStream.str());
}

configuration *cSourceModuleConfiguration::getConfigXMLWrapper()
{
    return m_xsdGeneratedConfig.get();
}

}

