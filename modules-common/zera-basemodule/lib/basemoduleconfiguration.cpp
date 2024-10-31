#include <xmlconfigreader.h>
#include "basemoduleconfiguration.h"

BaseModuleConfiguration::BaseModuleConfiguration()
{
    m_pXMLReader = new Zera::XMLConfig::cReader();
    m_bConfigured = false;
}

BaseModuleConfiguration::~BaseModuleConfiguration()
{
    delete m_pXMLReader;
}

bool BaseModuleConfiguration::isConfigured()
{
    return m_bConfigured;
}

