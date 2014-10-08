#include <xmlconfigreader.h>

#include "basemoduleconfiguration.h"

namespace OSCIMODULE
{

cBaseModuleConfiguration::cBaseModuleConfiguration()
{
    m_pXMLReader = new Zera::XMLConfig::cReader();
    m_bConfigured = false;
}


cBaseModuleConfiguration::~cBaseModuleConfiguration()
{
    delete m_pXMLReader;
}


bool cBaseModuleConfiguration::isConfigured()
{
    return m_bConfigured;
}

}

